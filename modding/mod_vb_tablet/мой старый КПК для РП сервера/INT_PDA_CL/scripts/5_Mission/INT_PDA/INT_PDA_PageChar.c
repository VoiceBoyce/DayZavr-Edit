// INT_PDA (client) — «Персонаж» (owner 2026-10-26: the vanilla character choice works badly). Left: the picked
// survivor in full height (a LOCAL character, INT_PDA_CharDummy, in the clothes of the player's spawn set; drag to
// turn). Middle: the survivors the server offers, men and women in columns. Right: name, the current and the saved
// character, «Сохранить» (every new character of this player is of that class) and «Переродиться» (saved + the
// current character dies after a confirmation; the mission asks for the respawn). The 3D model draws above every
// 2D widget: it is hidden while the confirmation is open and before the PDA's power-off shutter.
//
// Test commands (TestKit «#pda menu char ...»): char sel <class|index>, char save, char reborn (opens the
// confirmation), char yes / char no, char yaw <deg>, char state. Each logs a CHAR_TEST line.

class INT_PDA_PageCharRow
{
	string Type;
	Widget RowRoot;
	ref INT_UIF_Surface Box;
	ImageWidget Bar;
	TextWidget Name;
	TextWidget Now;
	ImageWidget Saved;
	ButtonWidget RowHit;
}

class INT_PDA_PageChar : INT_PDA_Page
{
	static const float PREV_X = 20;
	static const float PREV_W = 440;
	static const float LIST_X = 490;
	static const float LIST_W = 550;
	static const float INFO_X = 1070;
	static const float INFO_W = 346;
	static const float ROW_Y = 92;			// first row under the section captions
	static const float ROW_PITCH = 28;
	static const int ROWS_MAX = 20;			// rows per column

	protected Widget m_PreviewHost;
	protected PlayerPreviewWidget m_Preview;
	protected ButtonWidget m_PreviewHit;
	protected ref INT_PDA_CharDummy m_Dummy = new INT_PDA_CharDummy();
	protected bool m_Drag;
	protected int m_DragX;
	protected float m_Yaw;
	protected Widget m_List;
	protected Widget m_ListCols;			// section captions + rows (rebuilt when the offered list changes)
	protected Widget m_Info;
	protected Widget m_Lock;
	protected TextWidget m_LockText;
	protected ref array<ref INT_PDA_PageCharRow> m_Rows = new array<ref INT_PDA_PageCharRow>();
	protected string m_RowsKey;				// the offered list the rows were built for
	protected string m_Sel;
	protected TextWidget m_Name;
	protected TextWidget m_Gender;
	protected TextWidget m_NowText;
	protected TextWidget m_SavedText;
	protected TextWidget m_WaitText;
	protected INT_UIF_Button m_SaveBtn;
	protected INT_UIF_Button m_RebornBtn;
	protected float m_Tick;
	protected int m_SaveLook = -1;			// buttons repainted only when their state changes (1 = «Сохранён»)
	protected int m_RebornLook = -1;		// 1 = can be pressed (red), 0 = waiting (grey)
	// confirmation «Переродиться?»
	protected Widget m_Ask;
	protected TextWidget m_AskName;
	protected MultilineTextWidget m_AskText;
	protected INT_UIF_Button m_AskYes;
	protected INT_UIF_Button m_AskNo;
	protected bool m_RebornSent;

	override void OnBuild()
	{
		// left: the model (the profile's frame)
		m_PreviewHost = INT_UIF_Draw.Frame(m_Root, PREV_X, 0, PREV_W, m_H, "INT_PDA_CharPreviewHost");
		INT_UIF_Draw.Texture(m_PreviewHost, 0, 0, PREV_W, m_H, INT_PDA_Look.SET_DEV + "vignette", ARGB(120, 255, 255, 255));
		Widget area = INT_UIF_Draw.Frame(m_PreviewHost, 0, 4, PREV_W, m_H - 14, "INT_PDA_CharPreviewArea");
		Widget w = GetGame().GetWorkspace().CreateWidgets("INT_PDA_CL/gui/layouts/int_pda_preview.layout", area);
		if (w)
			m_Preview = PlayerPreviewWidget.Cast(w.FindAnyWidget("INT_PDA_Preview"));
		m_PreviewHit = Hit(m_PreviewHost, 0, 0, PREV_W, m_H);
		// middle: the list (rows come with the server's data)
		m_List = INT_UIF_Draw.Frame(m_Root, LIST_X, 0, LIST_W, m_H, "INT_PDA_CharList");
		Label(m_List, 0, 24, LIST_W, "#STR_INT_PDA_CHAR_TITLE");
		INT_UIF_Draw.Line(m_List, 0, 48, LIST_W, 1, INT_PDA_Look.LINE);
		// right: the picked one and the actions
		m_Info = INT_UIF_Draw.Frame(m_Root, INFO_X, 0, INFO_W, m_H, "INT_PDA_CharInfo");
		Label(m_Info, 0, 24, INFO_W, "#STR_INT_PDA_TAB_CHAR");
		m_Name = INT_UIF_Draw.Text(m_Info, 0, 44, INFO_W, 64, "", 52, INT_PDA_Look.TEXT);
		m_Gender = INT_UIF_Draw.Text(m_Info, 0, 108, INFO_W, 24, "", 17, INT_PDA_Look.DIM);
		INT_UIF_Draw.Line(m_Info, 0, 148, INFO_W, 1, INT_PDA_Look.LINE);
		m_NowText = Stat(m_Info, 0, 166, "#STR_INT_PDA_CHAR_NOW", false);
		m_SavedText = Stat(m_Info, 0, 250, "#STR_INT_PDA_CHAR_SAVED", true);
		INT_PDA_Look.MText(m_Info, 0, 342, INFO_W, 64, "#STR_INT_PDA_CHAR_SAVE_NOTE", 15, INT_PDA_Look.DIM);
		m_SaveBtn = INT_UIF_Button.Create(m_Bag, m_Info, 0, 452, INFO_W, 48, "#STR_INT_PDA_CHAR_SAVE", INT_UIF_Variant.SECONDARY, "check");
		m_RebornBtn = INT_UIF_Button.Create(m_Bag, m_Info, 0, 512, INFO_W, 48, "#STR_INT_PDA_CHAR_RESPAWN", INT_UIF_Variant.DANGER, "refresh");
		if (m_SaveBtn)
			m_SaveBtn.OnClick().Insert(OnSaveClick);
		if (m_RebornBtn)
			m_RebornBtn.OnClick().Insert(OnRebornClick);
		m_WaitText = INT_UIF_Draw.Text(m_Info, 0, 568, INFO_W, 22, "", 14, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		INT_PDA_Look.DrawIcon(m_Info, 0, 628, 18, "refresh", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_Info, 28, 624, INFO_W - 28, 26, "#STR_INT_PDA_CHAR_DRAG", 13, INT_PDA_Look.DIM2);
		// no account / switched off: a plate over the list and the info (the model stays the real player)
		m_Lock = INT_UIF_Draw.Frame(m_Root, LIST_X, 0, INFO_X + INFO_W - LIST_X, m_H, "INT_PDA_CharLock");
		INT_UIF_Draw.Image(m_Lock, 0, 0, INFO_X + INFO_W - LIST_X, m_H, "iuf_white", INT_PDA_Look.BG);
		float lw = INFO_X + INFO_W - LIST_X;
		INT_PDA_Look.DrawIcon(m_Lock, lw * 0.5 - 32, 230, 64, "lock", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_Lock, 0, 314, lw, 30, INT_PDA_Look.Caps("#STR_INT_PDA_CHAR_TITLE"), 22, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		m_LockText = INT_UIF_Draw.Text(m_Lock, 0, 354, lw, 24, "", 16, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		m_Lock.Show(false);
		BuildAsk();
	}

	protected TextWidget Stat(Widget parent, float x, float y, string key, bool accent)
	{
		int bar = INT_PDA_Look.LINE2;
		if (accent)
			bar = INT_PDA_Look.ORANGE;
		INT_UIF_Draw.Line(parent, x, y, 2, 66, bar);
		INT_UIF_Draw.Text(parent, x + 16, y, INFO_W - 16, 18, INT_PDA_Look.Caps(key), 13, INT_PDA_Look.DIM);
		return INT_UIF_Draw.Text(parent, x + 16, y + 22, INFO_W - 16, 40, "", 28, INT_PDA_Look.TEXT);
	}

	protected void BuildAsk()
	{
		float cw = 600;
		float ch = 244;
		m_Ask = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_CharAsk");
		INT_UIF_Draw.Image(m_Ask, 0, 0, m_W, m_H, "iuf_white", ARGB(170, 0, 0, 0));
		Hit(m_Ask, 0, 0, m_W, m_H);
		Widget box = INT_UIF_Draw.Frame(m_Ask, (m_W - cw) * 0.5, (m_H - ch) * 0.5, cw, ch, "INT_PDA_CharAskBox");
		INT_UIF_Surface.Shadow(box, -24, -16, cw + 48, ch + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(box, 0, 0, cw, ch, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		INT_UIF_Draw.Line(box, 0, 0, cw, 3, INT_PDA_Look.ENEMY);
		INT_UIF_Draw.Text(box, 28, 22, cw - 56, 28, INT_PDA_Look.Caps("#STR_INT_PDA_CHAR_ASK_TITLE"), 20, INT_PDA_Look.TEXT);
		m_AskName = INT_UIF_Draw.Text(box, 28, 58, cw - 56, 36, "", 28, INT_PDA_Look.ORANGE);
		m_AskText = INT_PDA_Look.MText(box, 28, 102, cw - 56, 56, "", 16, INT_PDA_Look.DIM);
		m_AskNo = INT_UIF_Button.Create(m_Bag, box, cw - 28 - 340, ch - 64, 140, 44, "#STR_INT_PDA_NO", INT_UIF_Variant.SECONDARY);
		m_AskYes = INT_UIF_Button.Create(m_Bag, box, cw - 28 - 190, ch - 64, 190, 44, "#STR_INT_PDA_CHAR_RESPAWN", INT_UIF_Variant.DANGER, "refresh");
		if (m_AskNo)
			m_AskNo.OnClick().Insert(OnAskNo);
		if (m_AskYes)
			m_AskYes.OnClick().Insert(OnAskYes);
		m_Ask.Show(false);
	}

	// --- life cycle ---------------------------------------------------------------------------------------------------
	override void OnShow()
	{
		m_RebornSent = false;
		if (INT_PDA_Client.s_LoggedIn)
			INT_PDA_Client.RequestChar();
		Refresh();
		Reveal(m_List, 0.06);
		Reveal(m_Info, 0.14);
	}

	override void OnHide()
	{
		m_Drag = false;
		CloseAsk();
		ReleasePreview();
	}

	// The model draws above every 2D widget: the menu hides it before the power-off shutter closes.
	override void HidePreview()
	{
		if (m_Preview)
			m_Preview.Show(false);
	}

	// The preview back on the real player, the local character goes a frame later.
	protected void ReleasePreview()
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (m_Preview && me)
			m_Preview.SetPlayer(me);
		if (m_Dummy)
			m_Dummy.Drop();
	}

	override void OnChanged(int what)
	{
		if (!m_Active)
			return;
		if (what == INT_PDA_Client.CH_CHAR || what == INT_PDA_Client.CH_AUTH)
			Refresh();
	}

	override void OnResult(int requestOp, int code)
	{
		if (requestOp == INT_PDA_Const.OP_CHAR_SAVE)
		{
			if (code == INT_PDA_Const.R_OK)
				INT_UIF_Toast.Success("#STR_INT_PDA_CHAR_SAVED_TOAST");
			else
				Toast(code);
			return;
		}
		if (requestOp != INT_PDA_Const.OP_CHAR_RESPAWN)
			return;
		m_RebornSent = false;
		if (code != INT_PDA_Const.R_OK)
		{
			Toast(code);
			RefreshButtons();
			return;
		}
		// the server killed the character: the PDA goes off, the mission asks for the respawn once he is dead
		INT_UIF_Toast.Info("#STR_INT_PDA_CHAR_REBORN_TOAST");
		m_Menu.PowerOff();
	}

	override bool HandleEscape()
	{
		if (AskOpen())
		{
			CloseAsk();
			return true;
		}
		return false;
	}

	// --- data ---------------------------------------------------------------------------------------------------------
	protected void Refresh()
	{
		INT_PDA_CharData d = INT_PDA_Client.s_Char;
		bool logged = INT_PDA_Client.s_LoggedIn;
		bool enabled = d && d.Enabled && d.Types.Count() > 0;
		bool locked = !logged || (d && !enabled);
		m_Lock.Show(locked);
		m_List.Show(!locked);
		m_Info.Show(!locked);
		if (!logged)
			m_LockText.SetText("#STR_INT_PDA_NO_ACCOUNT");
		else
			m_LockText.SetText("#STR_INT_PDA_CHAR_OFF");
		if (locked || !d)
		{
			ReleasePreview();
			BindRealPlayer();
			return;
		}
		BuildRows(d.Types);
		if (m_Sel == "" || d.Types.Find(m_Sel) < 0)
			m_Sel = FirstPick(d);
		RefreshRows();
		RefreshInfo();
		ShowSel();
	}

	// The saved class, else the current one, else the first offered.
	protected string FirstPick(INT_PDA_CharData d)
	{
		if (d.Chosen != "" && d.Types.Find(d.Chosen) >= 0)
			return d.Chosen;
		string cur = CurrentType();
		if (d.Types.Find(cur) >= 0)
			return cur;
		return d.Types[0];
	}

	protected string CurrentType()
	{
		Man me = GetGame().GetPlayer();
		if (!me)
			return "";
		return me.GetType();
	}

	protected void BindRealPlayer()
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!m_Preview || !me)
			return;
		m_Preview.Show(true);
		m_Preview.SetPlayer(me);
		m_Preview.SetModelPosition(Vector(0, 0, 0.605));
		m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		m_Preview.Refresh();
	}

	// --- list ---------------------------------------------------------------------------------------------------------
	// Men, then women, each in columns of ROWS_MAX rows; the columns share the list width.
	protected void BuildRows(array<string> types)
	{
		string key = "";
		foreach (string k : types)
			key = key + k + ";";
		if (key == m_RowsKey)
			return;
		m_RowsKey = key;
		m_Rows.Clear();
		if (m_ListCols)
			INT_UIF_Widgets.ClearChildren(m_ListCols);
		else
			m_ListCols = INT_UIF_Draw.Frame(m_List, 0, 56, LIST_W, m_H - 56, "INT_PDA_CharCols");
		array<string> men = new array<string>();
		array<string> women = new array<string>();
		foreach (string t : types)
		{
			if (INT_PDA_Chars.IsFemale(t))
				women.Insert(t);
			else
				men.Insert(t);
		}
		int colsMen = (men.Count() + ROWS_MAX - 1) / ROWS_MAX;
		int colsWomen = (women.Count() + ROWS_MAX - 1) / ROWS_MAX;
		int cols = colsMen + colsWomen;
		if (cols < 1)
			cols = 1;
		float gap = 20;
		float colW = (LIST_W - gap * (cols - 1)) / cols;
		float x = 0;
		x = Section(men, "#STR_INT_PDA_CHAR_MEN", x, colW, gap);
		Section(women, "#STR_INT_PDA_CHAR_WOMEN", x, colW, gap);
	}

	// One gender: caption over its first column, rows top-down; returns the x after its last column.
	protected float Section(array<string> types, string titleKey, float x, float colW, float gap)
	{
		if (types.Count() == 0)
			return x;
		string cap = INT_PDA_Look.Caps(titleKey) + "  " + types.Count().ToString();
		INT_UIF_Draw.Text(m_ListCols, x, 0, colW, 20, cap, 13, INT_PDA_Look.ORANGE);
		for (int i = 0; i < types.Count(); i++)
		{
			int col = i / ROWS_MAX;
			int row = i % ROWS_MAX;
			float rx = x + col * (colW + gap);
			float ry = ROW_Y - 56 + row * ROW_PITCH;
			m_Rows.Insert(BuildRow(types[i], rx, ry, colW));
		}
		int used = (types.Count() + ROWS_MAX - 1) / ROWS_MAX;
		return x + used * (colW + gap);
	}

	protected INT_PDA_PageCharRow BuildRow(string type, float x, float y, float w)
	{
		INT_PDA_PageCharRow r = new INT_PDA_PageCharRow();
		r.Type = type;
		float h = ROW_PITCH - 3;
		r.RowRoot = INT_UIF_Draw.Frame(m_ListCols, x, y, w, h, "INT_PDA_CharRow");
		r.Box = INT_UIF_Surface.Box(r.RowRoot, 0, 0, w, h, 3, INT_PDA_Look.BG, 0);
		r.Bar = INT_UIF_Draw.Image(r.RowRoot, 0, 0, 3, h, "iuf_white", INT_PDA_Look.ORANGE);
		string name = Widget.TranslateString(INT_PDA_Chars.NameKey(type));
		name = INT_PDA_Look.Fit(name, w - 90, 16);
		r.Name = INT_UIF_Draw.Text(r.RowRoot, 14, 0, w - 90, h, name, 16, INT_PDA_Look.DIM);
		r.Now = INT_UIF_Draw.Text(r.RowRoot, w - 92, 0, 64, h, INT_PDA_Look.Caps("#STR_INT_PDA_CHAR_NOW"), 11, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		r.Saved = INT_PDA_Look.DrawIcon(r.RowRoot, w - 22, (h - 14) * 0.5, 14, "check", INT_PDA_Look.ORANGE);
		r.RowHit = Hit(m_ListCols, x, y, w, h);
		return r;
	}

	protected void RefreshRows()
	{
		INT_PDA_CharData d = INT_PDA_Client.s_Char;
		string chosen = "";
		if (d)
			chosen = d.Chosen;
		string cur = CurrentType();
		foreach (INT_PDA_PageCharRow r : m_Rows)
		{
			bool sel = r.Type == m_Sel;
			r.Bar.Show(sel);
			int fill = INT_PDA_Look.BG;
			int col = INT_PDA_Look.DIM;
			if (sel)
			{
				fill = INT_PDA_Look.PANEL2;
				col = INT_PDA_Look.TEXT;
			}
			r.Box.SetFill(fill);
			r.Name.SetColor(col);
			r.Now.Show(r.Type == cur);
			r.Saved.Show(r.Type == chosen);
		}
	}

	protected void RefreshInfo()
	{
		INT_PDA_CharData d = INT_PDA_Client.s_Char;
		string name = INT_PDA_Look.Caps(INT_PDA_Chars.NameKey(m_Sel));
		name = INT_PDA_Look.Fit(name, INFO_W, 52);
		m_Name.SetText(name);
		if (INT_PDA_Chars.IsFemale(m_Sel))
			m_Gender.SetText("#STR_INT_PDA_CHAR_FEMALE");
		else
			m_Gender.SetText("#STR_INT_PDA_CHAR_MALE");
		m_NowText.SetText(NameOf(CurrentType()));
		string saved = "";
		if (d)
			saved = d.Chosen;
		if (saved == "")
			m_SavedText.SetText("#STR_INT_PDA_CHAR_NONE");
		else
			m_SavedText.SetText(NameOf(saved));
		RefreshButtons();
	}

	protected string NameOf(string type)
	{
		if (type == "")
			return "—";
		string n = Widget.TranslateString(INT_PDA_Chars.NameKey(type));
		return INT_PDA_Look.Fit(n, INFO_W - 16, 28);
	}

	protected void RefreshButtons()
	{
		INT_PDA_CharData d = INT_PDA_Client.s_Char;
		bool have = d && m_Sel != "";
		bool saved = have && d.Chosen == m_Sel;
		int wait = INT_PDA_Client.CharWaitSec();
		// a disabled UIF button looks the same as an enabled one: the state shows in the text / colour as well
		int saveLook = 0;
		if (saved)
			saveLook = 1;
		if (m_SaveBtn && saveLook != m_SaveLook)
		{
			m_SaveLook = saveLook;
			m_SaveBtn.SetEnabled(saveLook == 0);
			if (saveLook == 1)
				m_SaveBtn.SetText("#STR_INT_PDA_CHAR_SAVED");
			else
				m_SaveBtn.SetText("#STR_INT_PDA_CHAR_SAVE");
		}
		if (m_SaveBtn && !have)
			m_SaveBtn.SetEnabled(false);
		int rebornLook = 0;
		if (have && wait <= 0 && !m_RebornSent)
			rebornLook = 1;
		if (m_RebornBtn && rebornLook != m_RebornLook)
		{
			m_RebornLook = rebornLook;
			m_RebornBtn.SetEnabled(rebornLook == 1);
			if (rebornLook == 1)
				m_RebornBtn.SetVariant(INT_UIF_Variant.DANGER);
			else
				m_RebornBtn.SetVariant(INT_UIF_Variant.SECONDARY);
		}
		if (!d)
		{
			m_WaitText.SetText("");
			return;
		}
		string t;
		if (wait > 0)
		{
			int minutes = (wait + 59) / 60;
			t = Widget.TranslateString("#STR_INT_PDA_CHAR_WAIT");
			t.Replace("%1", minutes.ToString());
			m_WaitText.SetText(t);
			m_WaitText.SetColor(INT_PDA_Look.ORANGE);
		}
		else if (d.CooldownMin > 0)
		{
			t = Widget.TranslateString("#STR_INT_PDA_CHAR_COOLDOWN");
			t.Replace("%1", d.CooldownMin.ToString());
			m_WaitText.SetText(t);
			m_WaitText.SetColor(INT_PDA_Look.DIM2);
		}
		else
		{
			m_WaitText.SetText("");
		}
	}

	// The picked class on the model (in the spawn set's clothes).
	protected void ShowSel()
	{
		if (!m_Preview || m_Sel == "")
			return;
		array<string> wear = null;
		if (INT_PDA_Client.s_Char)
			wear = INT_PDA_Client.s_Char.Wear;
		PlayerBase man = m_Dummy.Make(m_Sel, wear);
		if (!man)
		{
			BindRealPlayer();
			return;
		}
		m_Preview.Show(!AskOpen());
		m_Preview.SetPlayer(man);
		m_Preview.SetModelPosition(Vector(0, 0, 0.605));
		m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		m_Preview.Refresh();
	}

	protected void Select(string type)
	{
		if (type == "" || type == m_Sel)
			return;
		m_Sel = type;
		INT_PDA_Sfx.Play("Toggle");
		RefreshRows();
		RefreshInfo();
		ShowSel();
	}

	// --- actions ------------------------------------------------------------------------------------------------------
	protected void OnSaveClick(INT_UIF_Button b)
	{
		if (m_Sel == "")
			return;
		INT_PDA_Sfx.Play("Click");
		INT_PDA_Client.SendCharSave(m_Sel);
	}

	protected void OnRebornClick(INT_UIF_Button b)
	{
		if (m_Sel == "" || m_RebornSent)
			return;
		INT_PDA_Sfx.Play("Click");
		OpenAsk();
	}

	protected void OpenAsk()
	{
		string name = INT_PDA_Look.Caps(INT_PDA_Chars.NameKey(m_Sel));
		m_AskName.SetText(name);
		string plain = Widget.TranslateString(INT_PDA_Chars.NameKey(m_Sel));
		string text = Widget.TranslateString("#STR_INT_PDA_CHAR_ASK_TEXT");
		text.Replace("%1", plain);
		m_AskText.SetText(text);
		HidePreview();
		m_Ask.Show(true);
		INT_UIF_Animator.Reveal(m_Ask, 0, 10, 0.2);
	}

	bool AskOpen()
	{
		return m_Ask && m_Ask.IsVisible();
	}

	protected void CloseAsk()
	{
		if (!AskOpen())
			return;
		m_Ask.Show(false);
		if (m_Preview && m_Active)
			m_Preview.Show(true);
	}

	protected void OnAskNo(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		CloseAsk();
	}

	protected void OnAskYes(INT_UIF_Button b)
	{
		INT_PDA_Sfx.Play("Click");
		CloseAsk();
		if (m_Sel == "")
			return;
		m_RebornSent = true;
		RefreshButtons();
		INT_PDA_Client.SendCharRespawn(m_Sel);
	}

	// --- input --------------------------------------------------------------------------------------------------------
	override void Update(float dt)
	{
		m_Tick = m_Tick + dt;
		if (m_Tick >= 1.0)
		{
			m_Tick = 0;
			RefreshButtons();
		}
		if (!m_Drag)
			return;
		int mx;
		int my;
		GetMousePos(mx, my);
		if ((GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) == 0)
		{
			m_Drag = false;
			return;
		}
		m_Yaw = m_Yaw + (mx - m_DragX) * 0.6;
		m_DragX = mx;
		if (m_Preview)
			m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w == m_PreviewHit && button == MouseState.LEFT && !AskOpen())
		{
			m_Drag = true;
			m_DragX = x;
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (AskOpen())
			return false;
		foreach (INT_PDA_PageCharRow r : m_Rows)
		{
			if (w == r.RowHit)
			{
				Select(r.Type);
				return true;
			}
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		foreach (INT_PDA_PageCharRow r : m_Rows)
		{
			if (w == r.RowHit && r.Type != m_Sel)
				r.Box.FadeFill(INT_PDA_Look.PANEL, 0.12);
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		foreach (INT_PDA_PageCharRow r : m_Rows)
		{
			if (w == r.RowHit && r.Type != m_Sel)
				r.Box.FadeFill(INT_PDA_Look.BG, 0.12);
		}
		return false;
	}

	// --- self-test ----------------------------------------------------------------------------------------------------
	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("char ") != 0)
			return;
		array<string> parts = new array<string>();
		cmd.Split(" ", parts);
		if (parts.Count() < 2)
			return;
		string verb = parts[1];
		string arg = "";
		if (parts.Count() >= 3)
			arg = parts[2];
		if (verb == "sel")
		{
			string type = arg;
			int idx = arg.ToInt();
			if (arg == idx.ToString() && idx >= 0 && idx < m_Rows.Count())
				type = m_Rows[idx].Type;
			Select(type);
		}
		else if (verb == "save")
		{
			OnSaveClick(null);
		}
		else if (verb == "reborn")
		{
			OnRebornClick(null);
		}
		else if (verb == "yes")
		{
			OnAskYes(null);
		}
		else if (verb == "no")
		{
			OnAskNo(null);
		}
		else if (verb == "yaw")
		{
			m_Yaw = arg.ToFloat();
			if (m_Preview)
				m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		}
		TestReport(verb);
	}

	protected void TestReport(string verb)
	{
		INT_PDA_CharData d = INT_PDA_Client.s_Char;
		string chosen = "-";
		int offered = 0;
		int wear = 0;
		bool enabled = false;
		if (d)
		{
			chosen = d.Chosen;
			offered = d.Types.Count();
			wear = d.Wear.Count();
			enabled = d.Enabled;
		}
		string dummy = "-";
		if (m_Dummy && m_Dummy.Get())
			dummy = m_Dummy.MadeOf();
		bool askOn = AskOpen();
		bool locked = m_Lock.IsVisible();
		string s = "CHAR_TEST " + verb + " sel=" + m_Sel + " chosen=" + chosen + " now=" + CurrentType() + " offered=" + offered.ToString();
		s = s + " rows=" + m_Rows.Count().ToString() + " enabled=" + enabled.ToString() + " locked=" + locked.ToString() + " wait=" + INT_PDA_Client.CharWaitSec().ToString();
		s = s + " dummy=" + dummy + " wear=" + wear.ToString();
		if (m_Dummy)
			s = s + " worn=" + m_Dummy.m_Worn.ToString();
		s = s + " ask=" + askOn.ToString() + " sent=" + m_RebornSent.ToString() + " pending=" + INT_PDA_Client.s_RebornPending.ToString();
		INT_PDA_Log.Raw(s);
	}
}
