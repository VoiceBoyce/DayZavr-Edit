// ============================================================================
// VanillaPPMap (our VPP fork) - concept B «ОБЗОР»: cards pinned to the map (MAP agent, phase 2, 2026-11-12).
//   INT_VPP_MarkerCard   a marker (hover/click): Изменить / Маршрут сюда / × (own), Маршрут сюда (others)
//   INT_VPP_EditorCard   new / edit marker (2_b_survey_marker): name, 9 colours, 60 icons in 10 groups,
//                        visibility (map / map + world / hidden), «только этот сервер», «Название в 3D» (2026-11-14:
//                        the name next to the marker's 3D icon), Сохранить / Отмена / Удалить
//   INT_VPP_RouteRibbon  route ribbon at the bottom (2_b_survey_route) + the next point's pinned card
//   INT_VPP_RulerCard    ruler: legs, total, run / walk time
// A card hangs on a map point by an orange hairline + diamond; it follows the point when the view changes
// (INT_Place, called by the menu only on view change). Cards are built on the menu's screen frame (design px) and
// closed through the menu's graveyard (hidden at once, unlinked next frame: never inside their own click).
// INT_VPP_Stat kinds: 3 card (marker card, ribbon, next card, ruler card), 4 editor. Client only.
// ============================================================================

class INT_VPP_CardBase : ScriptedWidgetEventHandler
{
	protected VPPMapMenu m_INT_Menu;
	protected Widget m_INT_Root;
	protected int m_INT_StatKind = 3;
	protected float m_INT_W;
	protected float m_INT_H;
	protected float m_INT_ScrW;
	protected float m_INT_ScrH;

	bool INT_IsOpen()
	{
		if (m_INT_Root)
			return true;
		return false;
	}

	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	protected Widget INT_NewRoot(Widget screen, float x, float y, float w, float h, string nm)
	{
		m_INT_W = w;
		m_INT_H = h;
		m_INT_Root = INT_UIF_Draw.Frame(screen, x, y, w, h, nm);
		if (m_INT_Root)
			INT_VPP_Stat.INT_Up(m_INT_StatKind);
		return m_INT_Root;
	}

	// hidden now, unlinked by the menu next frame
	void INT_Close()
	{
		if (!m_INT_Root)
			return;
		Widget r = m_INT_Root;
		m_INT_Root = null;
		INT_VPP_Stat.INT_Down(m_INT_StatKind);
		if (m_INT_Menu)
			m_INT_Menu.INT_Bury(r);
		else
			r.Unlink();
	}

	// card x/y (design px) next to an anchor (design px); right side first, else left
	protected void INT_SideOf(float mx, float my, float gap, float cardW, float cardH, float dy, out float cx, out float cy, out bool left)
	{
		left = false;
		cx = mx + gap;
		if (cx + cardW > m_INT_ScrW - 80)
		{
			left = true;
			cx = mx - gap - cardW;
		}
		if (cx < 24)
			cx = 24;
		cy = my - dy;
		if (cy < 124)
			cy = 124;
		if (cy + cardH > m_INT_ScrH - 24)
			cy = m_INT_ScrH - 24 - cardH;
	}
}

// ----------------------------------------------------------------------------
// The card of one marker (2_b_survey_map: .card 360 wide, pinned 92 px to the right of the marker).
class INT_VPP_MarkerCard : INT_VPP_CardBase
{
	static const float CARD_W = 360;

	protected INT_VPP_MapItem m_INT_Item;
	protected Widget m_INT_Panel;
	protected ImageWidget m_INT_PinLine;
	protected ImageWidget m_INT_PinDia;
	protected ref INT_VPPX_HButton m_INT_BtnEdit;
	protected ref INT_VPPX_HButton m_INT_BtnRoute;
	protected ref INT_VPP_IconBtn m_INT_BtnDel;
	protected ButtonWidget m_INT_Block;

	INT_VPP_MapItem INT_GetItem()
	{
		return m_INT_Item;
	}

	void INT_Build(Widget screen, float scrW, float scrH, VPPMapMenu menu, INT_VPP_MapItem it, vector me, bool meShown)
	{
		m_INT_Menu = menu;
		m_INT_Item = it;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		bool own = it.INT_IsMine();
		// a map admin edits / deletes the server's own markers (2026-11-14; events stay read-only)
		bool srvEdit = false;
		if (it.m_INT_Kind == INT_VPP_MapItem.KIND_SERVER && menu && menu.INT_IsAdmin())
			srvEdit = true;
		float h = 160;
		if (!INT_NewRoot(screen, 0, 0, CARD_W, h, "IntVppCard"))
			return;
		// pin first (under the panel), placed in INT_Place
		m_INT_PinLine = INT_VPPX_Look.Fill(m_INT_Root, 0, 0, 10, 1, INT_VPPX_Look.ORANGE);
		m_INT_PinDia = INT_VPPX_Look.Diamond(m_INT_Root, 0, 0, 10, true, INT_VPPX_Look.ORANGE);
		m_INT_Panel = INT_VPP_Ui.INT_Panel(m_INT_Root, 0, 0, CARD_W, h, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppCardP");
		if (!m_INT_Panel)
			return;
		m_INT_Block = INT_UIF_Draw.Hit(m_INT_Panel, 0, 0, CARD_W, h, this);
		string kindText = "Метка сервера";
		int tone = INT_VPPX_Look.WHITE;
		if (it.m_INT_Kind == INT_VPP_MapItem.KIND_MINE)
		{
			kindText = "Моя метка";
			tone = it.m_INT_Color;
		}
		else if (it.m_INT_Kind == INT_VPP_MapItem.KIND_MINE_SRV)
		{
			kindText = "Моя · этот сервер";
			tone = it.m_INT_Color;
		}
		else if (it.m_INT_Kind == INT_VPP_MapItem.KIND_EVENT)
		{
			kindText = "Событие";
			tone = 0xFFFFC246;
		}
		INT_VPPX_Look.Diamond(m_INT_Panel, 21, 22, 7, true, tone);
		string kc = INT_VPPX_Text.Caps(kindText);
		INT_VPPX_Look.Text(m_INT_Panel, 32, 13, 300, 18, kc, 12, tone, INT_UIF_Align.LEFT, true);
		INT_UIF_Draw.Texture(m_INT_Panel, 18, 38, 28, 28, it.m_INT_IconPath, it.m_INT_Color);
		string nm = INT_VPPX_Look.FitText(it.m_INT_Name, 26, false, 290);
		INT_VPPX_Look.Text(m_INT_Panel, 58, 34, 300, 36, nm, 26, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		// 85 м · ЮЗ   ·   13 239 · 5 931   ·   3D вкл.
		string line = "";
		if (meShown)
		{
			float dm = INT_VPP_Fmt.INT_Dist2D(me, it.m_INT_Pos);
			float brg = INT_VPP_Fmt.INT_Bearing(me, it.m_INT_Pos);
			string ds = INT_VPP_Fmt.INT_Dist(dm);
			string dw = INT_VPP_Fmt.INT_DirWord(brg);
			line = ds + " · " + dw + "   ·   ";
		}
		string co = INT_VPP_Fmt.INT_Coords(it.m_INT_Pos);
		line = line + co;
		if (own)
		{
			string st = "3D выкл.";
			if (!it.m_INT_Visible)
				st = "скрыта";
			else if (it.m_INT_3D)
				st = "3D вкл.";
			line = line + "   ·   " + st;
		}
		INT_VPPX_Look.Text(m_INT_Panel, 18, 76, CARD_W - 30, 20, line, 14, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
		float by = 108;
		if (own || srvEdit)
		{
			m_INT_BtnEdit = new INT_VPPX_HButton();
			m_INT_BtnEdit.Build(m_INT_Panel, 18, by, 128, 38, INT_VPPX_HButton.TONE_GHOST);
			m_INT_BtnEdit.SetText("Изменить");
			m_INT_BtnEdit.OnClick().Insert(INT_OnBtn);
			m_INT_BtnRoute = new INT_VPPX_HButton();
			m_INT_BtnRoute.Build(m_INT_Panel, 154, by, 142, 38, INT_VPPX_HButton.TONE_PRIMARY);
			// «Маршрут» as in 2_b_survey_map: «Маршрут сюда» did not fit 142 px (cut to «МАРШРУТ СЮ…», QA 2026-11-12)
			m_INT_BtnRoute.SetText("Маршрут");
			m_INT_BtnRoute.OnClick().Insert(INT_OnBtn);
			m_INT_BtnDel = new INT_VPP_IconBtn();
			string delTex = INT_VPPX_Look.IconPath("close");
			m_INT_BtnDel.INT_Build(m_INT_Panel, 304, by, 38, delTex, 16, 0xFFFF6D61);
			m_INT_BtnDel.INT_OnClick().Insert(INT_OnDel);
		}
		else
		{
			m_INT_BtnRoute = new INT_VPPX_HButton();
			m_INT_BtnRoute.Build(m_INT_Panel, 18, by, CARD_W - 36, 38, INT_VPPX_HButton.TONE_PRIMARY);
			m_INT_BtnRoute.SetText("Маршрут сюда");
			m_INT_BtnRoute.OnClick().Insert(INT_OnBtn);
		}
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		m_INT_Root.Show(false);
	}

	// anchor = the marker's absolute screen px
	void INT_Place(float ax, float ay, bool on, float f)
	{
		if (!m_INT_Root)
			return;
		if (!on || f <= 0)
		{
			if (m_INT_Root.IsVisible())
				m_INT_Root.Show(false);
			return;
		}
		float mx = ax / f;
		float my = ay / f;
		float cx;
		float cy;
		bool left;
		INT_SideOf(mx, my, 92, CARD_W, m_INT_H, 70, cx, cy, left);
		INT_UIF_Draw.Move(m_INT_Root, cx, cy);
		float py = my - cy;
		float x1 = mx + 20 - cx;
		float x2 = 0;
		if (left)
		{
			x1 = mx - 20 - cx;
			x2 = CARD_W;
		}
		float a = x1;
		float b = x2;
		if (b < a)
		{
			a = x2;
			b = x1;
		}
		if (m_INT_PinLine)
			INT_UIF_Draw.Place(m_INT_PinLine, a, py, b - a, 1);
		if (m_INT_PinDia)
		{
			float dw = 10 * INT_VPPX_Look.SQRT2;
			INT_UIF_Draw.Move(m_INT_PinDia, x2 - dw * 0.5, py - dw * 0.5);
		}
		if (!m_INT_Root.IsVisible())
			m_INT_Root.Show(true);
	}

	void INT_OnBtn(INT_VPPX_HButton b)
	{
		if (!m_INT_Menu || !m_INT_Item)
			return;
		INT_VPP_MapItem it = m_INT_Item;
		if (b == m_INT_BtnEdit)
			m_INT_Menu.INT_CardEdit(it);
		else if (b == m_INT_BtnRoute)
			m_INT_Menu.INT_CardRoute(it);
	}

	void INT_OnDel(INT_VPP_IconBtn b)
	{
		if (!m_INT_Menu || !m_INT_Item)
			return;
		INT_VPP_MapItem it = m_INT_Item;
		m_INT_Menu.INT_CardDelete(it);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_Block)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_Block)
			return true;
		return false;
	}
}

// ----------------------------------------------------------------------------
// Big name field of the editor (40 px look): edit box sized to its glyphs, own hit, orange underline on focus,
// «11 / 32» counter. Enter = OnChange(finished) while the key is down (memory: EditBox Enter).
class INT_VPP_BigInput : ScriptedWidgetEventHandler
{
	static const float FIELD_H = 60;
	static const float GLYPH = 34;
	static const int PH = 0xFF77726B;
	static const int SUBMIT_GAP_MS = 150;

	protected Widget m_INT_Root;
	protected EditBoxWidget m_INT_Edit;
	protected ButtonWidget m_INT_Hit;
	protected TextWidget m_INT_Ph;
	protected TextWidget m_INT_Count;
	protected ImageWidget m_INT_Line;
	protected float m_INT_FW;
	protected int m_INT_MaxChars;
	protected int m_INT_MaxBytes;
	protected bool m_INT_Focus;
	protected string m_INT_Last;
	protected string m_INT_LastCount;
	protected int m_INT_LastSubmit = -100000;
	protected ref ScriptInvoker m_INT_OnChanged;
	protected ref ScriptInvoker m_INT_OnSubmit;

	void INT_VPP_BigInput()
	{
		m_INT_OnChanged = new ScriptInvoker();
		m_INT_OnSubmit = new ScriptInvoker();
	}

	// (string text)
	ScriptInvoker INT_OnChanged()
	{
		return m_INT_OnChanged;
	}

	// (string text)
	ScriptInvoker INT_OnSubmit()
	{
		return m_INT_OnSubmit;
	}

	void INT_Build(Widget parent, float x, float y, float w, string placeholder, int maxChars, int maxBytes)
	{
		m_INT_FW = w;
		m_INT_MaxChars = maxChars;
		m_INT_MaxBytes = maxBytes;
		m_INT_Root = INT_UIF_Draw.Frame(parent, x, y, w, FIELD_H, "IntVppBigInput");
		if (!m_INT_Root)
			return;
		m_INT_Ph = INT_VPPX_Look.Text(m_INT_Root, 0, 4, w - 90, 52, placeholder, GLYPH, PH, INT_UIF_Align.LEFT, false);
		m_INT_Count = INT_VPPX_Look.Text(m_INT_Root, w - 80, 20, 80, 20, " ", 13, INT_VPP_Ui.D, INT_UIF_Align.RIGHT, false);
		m_INT_Hit = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, w, FIELD_H, this);
		m_INT_Edit = INT_UIF_Draw.Edit(m_INT_Root, 0, 4, w - 90, 52, GLYPH);
		if (m_INT_Edit)
		{
			m_INT_Edit.SetHandler(this);
			m_INT_Edit.SetColor(ARGB(255, 255, 255, 255));
			m_INT_Edit.SetTextColor(INT_VPPX_Look.WHITE);
		}
		m_INT_Line = INT_VPPX_Look.Fill(m_INT_Root, 0, FIELD_H - 1, w, 1, INT_VPP_Ui.LINE2);
		m_INT_Last = "";
		INT_Paint();
	}

	string INT_GetText()
	{
		if (!m_INT_Edit)
			return "";
		return m_INT_Edit.GetText();
	}

	void INT_SetText(string t)
	{
		string s = INT_Clip(t);
		m_INT_Last = s;
		if (m_INT_Edit)
			m_INT_Edit.SetText(s);
		INT_Paint();
	}

	bool INT_IsFocused()
	{
		if (m_INT_Focus)
			return true;
		if (!m_INT_Edit)
			return false;
		Widget f = GetFocus();
		if (f && f == m_INT_Edit)
			return true;
		return false;
	}

	void INT_Focus()
	{
		if (!m_INT_Edit)
			return;
		SetFocus(m_INT_Edit);
		m_INT_Focus = true;
		INT_Paint();
	}

	void INT_Blur()
	{
		bool mine = m_INT_Focus;
		Widget f = GetFocus();
		if (f && m_INT_Edit && f == m_INT_Edit)
			mine = true;
		if (mine)
			SetFocus(null);
		m_INT_Focus = false;
		INT_Paint();
	}

	protected string INT_Clip(string t)
	{
		string s = t;
		if (m_INT_MaxChars > 0 && s.LengthUtf8() > m_INT_MaxChars)
			s = s.SubstringUtf8(0, m_INT_MaxChars);
		if (m_INT_MaxBytes > 0 && s.Length() > m_INT_MaxBytes)
			s = INT_VPP_Validate.ClipBytes(s, m_INT_MaxBytes);
		return s;
	}

	protected void INT_Paint()
	{
		if (m_INT_Line)
		{
			if (m_INT_Focus)
			{
				INT_UIF_Draw.Place(m_INT_Line, 0, FIELD_H - 2, m_INT_FW, 2);
				m_INT_Line.SetColor(INT_VPPX_Look.ORANGE);
			}
			else
			{
				INT_UIF_Draw.Place(m_INT_Line, 0, FIELD_H - 1, m_INT_FW, 1);
				m_INT_Line.SetColor(INT_VPP_Ui.LINE2);
			}
		}
		string t = INT_GetText();
		if (m_INT_Ph)
		{
			bool show = false;
			if (t == "" && !m_INT_Focus)
				show = true;
			if (m_INT_Ph.IsVisible() != show)
				m_INT_Ph.Show(show);
		}
		if (m_INT_Count)
		{
			int n = t.LengthUtf8();
			string c = n.ToString() + " / " + m_INT_MaxChars.ToString();
			if (c != m_INT_LastCount)
			{
				m_INT_LastCount = c;
				m_INT_Count.SetText(c);
			}
		}
	}

	protected void INT_Submit()
	{
		if (!GetGame())
			return;
		int now = GetGame().GetTime();
		if (now - m_INT_LastSubmit < SUBMIT_GAP_MS)
			return;
		m_INT_LastSubmit = now;
		string t = INT_GetText();
		m_INT_OnSubmit.Invoke(t);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_INT_Hit)
			return false;
		INT_Focus();
		return true;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_Hit)
			return true;
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		m_INT_Focus = true;
		INT_Paint();
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		m_INT_Focus = false;
		INT_Paint();
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		string t = m_INT_Edit.GetText();
		string c = INT_Clip(t);
		if (c != t)
			m_INT_Edit.SetText(c);
		INT_Paint();
		if (c != m_INT_Last)
		{
			m_INT_Last = c;
			m_INT_OnChanged.Invoke(c);
		}
		if (finished)
		{
			bool enter = false;
			if (KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0)
				enter = true;
			if (enter)
				INT_Submit();
		}
		return true;
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (!m_INT_Edit || w != m_INT_Edit)
			return false;
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			INT_Submit();
			return true;
		}
		return false;
	}
}

// ----------------------------------------------------------------------------
// Editor card (2_b_survey_marker: .card 760 x 844 at the free side, pinned to the ghost).
class INT_VPP_EditorCard : INT_VPP_CardBase
{
	static const float CARD_W = 760;
	static const float CARD_H = 844;
	// «Моя / Серверная» in the header (2026-11-14): right side, 20 px left of × (CARD_W - 56)
	static const float SRV_SEG_W = 210;
	static const float SRV_SEG_X = 474;
	static const float CELL = 52;
	static const int CELL_ICON = 0xFFBDB7AF;
	static const int CELL_ON_BG = 0xFF2A160B;
	static const int CELL_HOV_BG = 0xFF1A1918;
	static const int NAME_CHARS = 32;

	protected INT_VPP_MapItem m_INT_Item;
	protected vector m_INT_Pos;
	protected vector m_INT_Me;
	protected bool m_INT_MeShown;
	protected int m_INT_Color;
	protected string m_INT_IconPath;
	protected string m_INT_IconId;
	protected int m_INT_Vis;
	protected bool m_INT_OnlyServer;
	protected bool m_INT_Left;
	// «Название в 3D» (2026-11-14): true = the name is drawn next to the 3D icon (MarkerInfo.INT_IsName3DHidden false)
	protected bool m_INT_Name3D = true;
	protected ref INT_VPPX_Toggle m_INT_Name3DToggle;
	// server marker (2026-11-14, map admins): m_INT_Srv = this card saves to the server's VPPMapConfig.json;
	// m_INT_SrvSwitch = a new marker of an admin: «Моя / Серверная» at the top (default «Моя»)
	protected bool m_INT_Srv;
	protected bool m_INT_SrvSwitch;
	protected ref INT_VPPX_HSegment m_INT_SrvSeg;
	protected TextWidget m_INT_Title;
	protected TextWidget m_INT_Where;

	protected Widget m_INT_Panel;
	protected ImageWidget m_INT_PinLine;
	protected ImageWidget m_INT_PinDia;
	protected ButtonWidget m_INT_Block;
	protected ref INT_VPP_BigInput m_INT_Name;
	protected ref INT_VPP_IconBtn m_INT_CloseBtn;
	protected ref array<int> m_INT_Palette;
	protected ref array<ButtonWidget> m_INT_SwHits;
	protected Widget m_INT_SwSel;
	protected ImageWidget m_INT_PrevIcon;
	protected TextWidget m_INT_PrevName;
	protected TextWidget m_INT_PrevDist;
	protected ref INT_VPPX_HSegment m_INT_Seg;
	protected ref INT_VPPX_Toggle m_INT_Only;
	protected ref array<ButtonWidget> m_INT_CellHits;
	protected ref array<ImageWidget> m_INT_CellIcons;
	protected ref array<float> m_INT_CellX;
	protected ref array<float> m_INT_CellY;
	protected Widget m_INT_CellSel;
	protected Widget m_INT_CellHov;
	protected int m_INT_SelCell = -1;
	protected int m_INT_HovCell = -1;
	protected TextWidget m_INT_Info;
	protected ref INT_VPPX_HButton m_INT_BtnSave;
	protected ref INT_VPPX_HButton m_INT_BtnCancel;
	protected ref INT_VPPX_HButton m_INT_BtnDel;

	void INT_VPP_EditorCard()
	{
		m_INT_StatKind = 4;
		m_INT_Palette = new array<int>();
		m_INT_SwHits = new array<ButtonWidget>();
		m_INT_CellHits = new array<ButtonWidget>();
		m_INT_CellIcons = new array<ImageWidget>();
		m_INT_CellX = new array<float>();
		m_INT_CellY = new array<float>();
	}

	static void INT_FillPalette(array<int> pal)
	{
		pal.Clear();
		pal.Insert(0xFFF3EFE8);
		pal.Insert(0xFFFF7A2F);
		pal.Insert(0xFFFF4B3E);
		pal.Insert(0xFFFFC246);
		pal.Insert(0xFF56E07E);
		pal.Insert(0xFF3EC7FF);
		pal.Insert(0xFF6F8CFF);
		pal.Insert(0xFFB08CFF);
		pal.Insert(0xFFFF6FAE);
	}

	// before INT_Build (2026-11-14): srv = a server marker (admin), canSwitch = show «Моя / Серверная» (new, admin)
	void INT_SetServerOptions(bool srv, bool canSwitch)
	{
		m_INT_Srv = srv;
		m_INT_SrvSwitch = canSwitch;
	}

	bool INT_IsServer()
	{
		return m_INT_Srv;
	}

	// a server marker moved by a click on the map (2026-11-14): the header shows the new place
	void INT_SetPos(vector pos)
	{
		m_INT_Pos = pos;
		INT_PaintWhere();
	}

	protected string INT_TitleText()
	{
		string title = "Новая метка";
		if (m_INT_Item)
			title = "Изменить метку";
		if (m_INT_Srv)
		{
			title = "Новая серверная метка";
			if (m_INT_Item)
				title = "Изменить серверную метку";
		}
		return title;
	}

	protected void INT_PaintWhere()
	{
		if (!m_INT_Where)
			return;
		int ix = Math.Round(m_INT_Pos[0]);
		int iz = Math.Round(m_INT_Pos[2]);
		string xs = INT_VPPX_Text.Thousands(ix);
		string zs = INT_VPPX_Text.Thousands(iz);
		string where = "X " + xs + " · Y " + zs;
		if (m_INT_MeShown)
		{
			float dm = INT_VPP_Fmt.INT_Dist2D(m_INT_Me, m_INT_Pos);
			string ds = INT_VPP_Fmt.INT_Dist(dm);
			where = where + "   ·   " + ds + " от вас";
		}
		m_INT_Where.SetText(where);
	}

	void INT_OnSrvSeg(int index, INT_VPPX_HSegment s)
	{
		bool srv = false;
		if (index == 1)
			srv = true;
		m_INT_Srv = srv;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		if (m_INT_Title)
		{
			string tc = INT_VPPX_Text.Caps(INT_TitleText());
			m_INT_Title.SetText(tc);
		}
		// «Только этот сервер» is a personal-marker choice
		if (m_INT_Only)
			m_INT_Only.SetEnabled(!srv);
	}

	// TEST: the «Моя / Серверная» switch's path
	void INT_TestSetServer(bool srv)
	{
		if (!m_INT_SrvSeg)
			return;
		int idx = 0;
		if (srv)
			idx = 1;
		INT_OnSrvSeg(idx, m_INT_SrvSeg);
	}

	// item null = new marker at pos
	void INT_Build(Widget screen, float scrW, float scrH, VPPMapMenu menu, INT_VPP_MapItem item, vector pos, string nm, int color, string iconPath, int vis, bool onlyServer, vector me, bool meShown, bool leftSide)
	{
		m_INT_Menu = menu;
		m_INT_Item = item;
		m_INT_Pos = pos;
		m_INT_Me = me;
		m_INT_MeShown = meShown;
		m_INT_Color = color;
		m_INT_IconPath = iconPath;
		m_INT_IconId = INT_VPP_Icons.INT_IdOfPath(iconPath);
		m_INT_Vis = vis;
		m_INT_OnlyServer = onlyServer;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		m_INT_Left = leftSide;
		m_INT_Name3D = true;
		if (item && item.m_INT_Info)
		{
			bool hidden = item.m_INT_Info.INT_IsName3DHidden();
			if (hidden)
				m_INT_Name3D = false;
		}
		INT_VPP_EditorCard.INT_FillPalette(m_INT_Palette);
		float x0 = scrW - 48 - CARD_W;
		if (leftSide)
			x0 = 48;
		if (!INT_NewRoot(screen, x0, 128, CARD_W, CARD_H, "IntVppEditor"))
			return;
		m_INT_PinLine = INT_VPPX_Look.Fill(m_INT_Root, 0, 0, 10, 1, INT_VPPX_Look.ORANGE);
		m_INT_PinDia = INT_VPPX_Look.Diamond(m_INT_Root, 0, 0, 10, true, INT_VPPX_Look.ORANGE);
		m_INT_Panel = INT_VPP_Ui.INT_Panel(m_INT_Root, 0, 0, CARD_W, CARD_H, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppEditorP");
		if (!m_INT_Panel)
			return;
		m_INT_Block = INT_UIF_Draw.Hit(m_INT_Panel, 0, 0, CARD_W, CARD_H, this);
		INT_BuildHead();
		INT_BuildName(nm);
		INT_BuildColours();
		INT_BuildPreview();
		INT_BuildVisibility();
		INT_BuildIcons();
		INT_BuildButtons();
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		INT_PaintPreview();
	}

	protected void INT_BuildHead()
	{
		string title = INT_TitleText();
		string tc = INT_VPPX_Text.Caps(title);
		// «Моя / Серверная» (2026-11-14): only for a new marker of a map admin. The switch takes the right side of the
		// header (left of ×) and the coordinates go to a second line under the title: on one line they ran into the
		// switch («СЕРВЕРНАЯ X 8 624 · Y 8 615», owner shot t14_marker_editor_srv_switch).
		if (m_INT_SrvSwitch)
		{
			m_INT_Title = INT_VPPX_Look.Text(m_INT_Panel, 32, 10, SRV_SEG_X - 44, 20, tc, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
			array<string> kinds = new array<string>();
			kinds.Insert("Моя");
			kinds.Insert("Серверная");
			int sel = 0;
			if (m_INT_Srv)
				sel = 1;
			m_INT_SrvSeg = new INT_VPPX_HSegment();
			m_INT_SrvSeg.Build(m_INT_Panel, SRV_SEG_X, 10, kinds, sel, SRV_SEG_W, 32, true, false);
			m_INT_SrvSeg.OnChanged().Insert(INT_OnSrvSeg);
			m_INT_Where = INT_VPPX_Look.Text(m_INT_Panel, 32, 31, SRV_SEG_X - 44, 20, " ", 12, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		}
		else
		{
			m_INT_Title = INT_VPPX_Look.Text(m_INT_Panel, 32, 18, 300, 20, tc, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
			m_INT_Where = INT_VPPX_Look.Text(m_INT_Panel, CARD_W - 76 - 420, 18, 420, 20, " ", 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
		}
		INT_PaintWhere();
		m_INT_CloseBtn = new INT_VPP_IconBtn();
		string closeTex = INT_VPPX_Look.IconPath("close");
		m_INT_CloseBtn.INT_Build(m_INT_Panel, CARD_W - 20 - 36, 12, 36, closeTex, 16, INT_VPPX_Look.TEXT);
		m_INT_CloseBtn.INT_OnClick().Insert(INT_OnClose);
	}

	protected void INT_BuildName(string nm)
	{
		m_INT_Name = new INT_VPP_BigInput();
		m_INT_Name.INT_Build(m_INT_Panel, 32, 56, 696, "Название метки", NAME_CHARS, INT_VPP_Net.NAME_MAX_BYTES);
		m_INT_Name.INT_SetText(nm);
		m_INT_Name.INT_OnChanged().Insert(INT_OnNameChanged);
		m_INT_Name.INT_OnSubmit().Insert(INT_OnNameSubmit);
	}

	protected void INT_BuildColours()
	{
		string cap = INT_VPPX_Text.Caps("Цвет");
		INT_VPPX_Look.Text(m_INT_Panel, 32, 136, 200, 20, cap, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_SwSel = INT_UIF_Draw.Frame(m_INT_Panel, 0, 162, 30, 44, "IntVppSwSel");
		if (m_INT_SwSel)
		{
			INT_VPPX_Look.Outline(m_INT_SwSel, 0, 0, 30, 30, INT_VPPX_Look.WHITE);
			INT_VPPX_Look.Diamond(m_INT_SwSel, 15, 37, 6, true, INT_VPPX_Look.ORANGE);
		}
		int n = m_INT_Palette.Count();
		for (int i = 0; i < n; i++)
		{
			float sx = 32 + i * 38;
			int c = m_INT_Palette.Get(i);
			INT_VPPX_Look.Fill(m_INT_Panel, sx + 5, 167, 20, 20, c);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Panel, sx, 162, 30, 30, this);
			m_INT_SwHits.Insert(hit);
		}
		INT_PaintSwatch();
	}

	protected void INT_PaintSwatch()
	{
		if (!m_INT_SwSel)
			return;
		int at = -1;
		int rgb = m_INT_Color & 0x00FFFFFF;
		for (int i = 0; i < m_INT_Palette.Count(); i++)
		{
			int pc = m_INT_Palette.Get(i) & 0x00FFFFFF;
			if (pc == rgb)
				at = i;
		}
		if (at < 0)
		{
			m_INT_SwSel.Show(false);
			return;
		}
		float sx = 32 + at * 38;
		INT_UIF_Draw.Move(m_INT_SwSel, sx, 162);
		m_INT_SwSel.Show(true);
	}

	protected void INT_BuildPreview()
	{
		string cap = INT_VPPX_Text.Caps("Как выглядит");
		INT_VPPX_Look.Text(m_INT_Panel, 410, 136, 300, 20, cap, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_PrevIcon = INT_UIF_Draw.Texture(m_INT_Panel, 410, 165, 24, 24, m_INT_IconPath, m_INT_Color);
		m_INT_PrevName = INT_VPPX_Look.Text(m_INT_Panel, 444, 163, 200, 28, " ", 16, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_PrevDist = INT_VPPX_Look.Text(m_INT_Panel, 600, 165, 128, 24, " ", 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
	}

	protected void INT_PaintPreview()
	{
		if (m_INT_PrevIcon)
		{
			m_INT_PrevIcon.LoadImageFile(0, m_INT_IconPath);
			m_INT_PrevIcon.SetImage(0);
			m_INT_PrevIcon.SetColor(m_INT_Color);
		}
		string nm = "";
		if (m_INT_Name)
			nm = m_INT_Name.INT_GetText();
		if (nm == "")
			nm = "Метка";
		string fit = INT_VPPX_Look.FitText(nm, 16, false, 140);
		float nw = INT_VPPX_Look.Measure(fit, 16);
		if (m_INT_PrevName)
		{
			m_INT_PrevName.SetText(fit);
			INT_UIF_Draw.Place(m_INT_PrevName, 444, 163, nw + 30, 28);
		}
		if (m_INT_PrevDist)
		{
			string d = " ";
			if (m_INT_MeShown)
			{
				float dm = INT_VPP_Fmt.INT_Dist2D(m_INT_Me, m_INT_Pos);
				string ds = INT_VPP_Fmt.INT_Dist(dm);
				d = "· в мире: " + ds;
			}
			m_INT_PrevDist.SetText(d);
			INT_UIF_Draw.Place(m_INT_PrevDist, 444 + nw + 10, 165, 200, 24);
		}
	}

	protected void INT_BuildVisibility()
	{
		string cap = INT_VPPX_Text.Caps("Видимость");
		INT_VPPX_Look.Text(m_INT_Panel, 32, 216, 200, 20, cap, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		// «Название в 3D» (2026-11-14) between the caption and «Только этот сервер»
		m_INT_Name3DToggle = new INT_VPPX_Toggle();
		string n3 = INT_VPP_Ui.INT_Tr("STR_INT_VPP_NAME_3D");
		m_INT_Name3DToggle.Build(m_INT_Panel, 150, 212, 250, n3, m_INT_Name3D);
		m_INT_Name3DToggle.OnChanged().Insert(INT_OnName3D);
		m_INT_Only = new INT_VPPX_Toggle();
		m_INT_Only.Build(m_INT_Panel, 410, 212, 318, "Только этот сервер", m_INT_OnlyServer);
		m_INT_Only.OnChanged().Insert(INT_OnOnly);
		if (m_INT_Srv)
			m_INT_Only.SetEnabled(false);
		array<string> labels = new array<string>();
		labels.Insert("Только на карте");
		labels.Insert("Карта и метка в мире");
		labels.Insert("Скрыта");
		m_INT_Seg = new INT_VPPX_HSegment();
		m_INT_Seg.Build(m_INT_Panel, 32, 242, labels, m_INT_Vis, 696, 44, true, false);
		m_INT_Seg.OnChanged().Insert(INT_OnSeg);
		INT_VPPX_Look.Fill(m_INT_Panel, 32, 312, 696, 1, INT_VPPX_Look.LINE);
	}

	protected void INT_BuildIcons()
	{
		string cap = INT_VPPX_Text.Caps("Значок");
		float cw = INT_VPPX_Look.Measure(cap, 13, true);
		INT_VPPX_Look.Text(m_INT_Panel, 32, 326, cw + 20, 20, cap, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		int total = INT_VPP_Icons.INT_Count();
		string tn = INT_VPPX_Text.Spaced(total.ToString());
		INT_VPPX_Look.Text(m_INT_Panel, 32 + cw + 18, 326, 60, 20, tn, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		// shared hover / selection frames (under the icons)
		m_INT_CellHov = INT_UIF_Draw.Frame(m_INT_Panel, 0, 0, CELL, CELL, "IntVppCellHov");
		if (m_INT_CellHov)
		{
			INT_VPPX_Look.Fill(m_INT_CellHov, 0, 0, CELL, CELL, CELL_HOV_BG);
			INT_VPPX_Look.Outline(m_INT_CellHov, 0, 0, CELL, CELL, INT_VPP_Ui.LINE2);
			m_INT_CellHov.Show(false);
		}
		m_INT_CellSel = INT_UIF_Draw.Frame(m_INT_Panel, 0, 0, CELL, CELL, "IntVppCellSel");
		if (m_INT_CellSel)
		{
			INT_VPPX_Look.Fill(m_INT_CellSel, 0, 0, CELL, CELL, CELL_ON_BG);
			INT_VPPX_Look.Outline(m_INT_CellSel, 0, 0, CELL, CELL, INT_VPPX_Look.ORANGE);
			m_INT_CellSel.Show(false);
		}
		array<int> idx = new array<int>();
		int groups = INT_VPP_Icons.INT_GroupCount();
		for (int g = 0; g < groups; g++)
		{
			int col = 0;
			if (g >= 5)
				col = 1;
			int row = g - col * 5;
			float gx = 32 + col * 356;
			float gy = 362 + row * 82;
			string gl = INT_VPP_Icons.INT_GroupShort(g);
			string glc = INT_VPPX_Text.Caps(gl);
			INT_VPPX_Look.Text(m_INT_Panel, gx, gy, 340, 18, glc, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
			INT_VPP_Icons.INT_IconsInGroup(g, idx);
			int cnt = idx.Count();
			for (int k = 0; k < cnt && k < 6; k++)
			{
				int icon = idx.Get(k);
				float cx = gx + k * (CELL + 4);
				float cy = gy + 22;
				string path = INT_VPP_Icons.INT_PathAt(icon);
				string iid = INT_VPP_Icons.INT_IdAt(icon);
				ImageWidget img = INT_UIF_Draw.Texture(m_INT_Panel, cx + 14, cy + 14, 24, 24, path, CELL_ICON);
				ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Panel, cx, cy, CELL, CELL, this);
				m_INT_CellHits.Insert(hit);
				m_INT_CellIcons.Insert(img);
				m_INT_CellX.Insert(cx);
				m_INT_CellY.Insert(cy);
				if (iid == m_INT_IconId && m_INT_IconId != "")
					m_INT_SelCell = m_INT_CellHits.Count() - 1;
			}
		}
		INT_VPPX_Look.IconAt(m_INT_Panel, 32, 786, 16, "info", INT_VPP_Ui.M);
		m_INT_Info = INT_VPPX_Look.Text(m_INT_Panel, 56, 782, 330, 24, " ", 14, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		INT_PaintCells();
	}

	protected void INT_PaintCells()
	{
		int n = m_INT_CellIcons.Count();
		for (int i = 0; i < n; i++)
		{
			ImageWidget img = m_INT_CellIcons.Get(i);
			if (!img)
				continue;
			int c = CELL_ICON;
			if (i == m_INT_SelCell)
				c = INT_VPPX_Look.ORANGE;
			else if (i == m_INT_HovCell)
				c = INT_VPPX_Look.WHITE;
			img.SetColor(c);
		}
		INT_PlaceCellFrame(m_INT_CellSel, m_INT_SelCell);
		int hov = m_INT_HovCell;
		if (hov == m_INT_SelCell)
			hov = -1;
		INT_PlaceCellFrame(m_INT_CellHov, hov);
		string info = " ";
		int shown = m_INT_HovCell;
		if (shown < 0)
			shown = m_INT_SelCell;
		if (shown >= 0)
		{
			string sid = INT_CellIconId(shown);
			info = INT_VPP_Icons.INT_LabelOf(sid);
		}
		if (m_INT_Info)
			m_INT_Info.SetText(info);
	}

	protected string INT_CellIconId(int cell)
	{
		// cells are made in group order = the registry's picker order
		int counted = 0;
		int groups = INT_VPP_Icons.INT_GroupCount();
		array<int> idx = new array<int>();
		for (int g = 0; g < groups; g++)
		{
			INT_VPP_Icons.INT_IconsInGroup(g, idx);
			int cnt = idx.Count();
			if (cnt > 6)
				cnt = 6;
			if (cell < counted + cnt)
			{
				int icon = idx.Get(cell - counted);
				return INT_VPP_Icons.INT_IdAt(icon);
			}
			counted = counted + cnt;
		}
		return "";
	}

	protected void INT_PlaceCellFrame(Widget f, int cell)
	{
		if (!f)
			return;
		if (cell < 0 || cell >= m_INT_CellX.Count())
		{
			if (f.IsVisible())
				f.Show(false);
			return;
		}
		float cx = m_INT_CellX.Get(cell);
		float cy = m_INT_CellY.Get(cell);
		INT_UIF_Draw.Move(f, cx, cy);
		if (!f.IsVisible())
			f.Show(true);
	}

	protected void INT_BuildButtons()
	{
		m_INT_BtnCancel = new INT_VPPX_HButton();
		m_INT_BtnCancel.Build(m_INT_Panel, CARD_W - 32 - 230 - 10 - 140, 770, 140, 50, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnCancel.SetText("Отмена");
		m_INT_BtnCancel.OnClick().Insert(INT_OnButton);
		m_INT_BtnSave = new INT_VPPX_HButton();
		m_INT_BtnSave.Build(m_INT_Panel, CARD_W - 32 - 230, 770, 230, 50, INT_VPPX_HButton.TONE_PRIMARY);
		m_INT_BtnSave.SetText("Сохранить");
		m_INT_BtnSave.SetKey("ENTER");
		m_INT_BtnSave.OnClick().Insert(INT_OnButton);
		if (m_INT_Item)
		{
			m_INT_BtnDel = new INT_VPPX_HButton();
			m_INT_BtnDel.Build(m_INT_Panel, CARD_W - 32 - 230 - 10 - 140 - 10 - 130, 770, 130, 50, INT_VPPX_HButton.TONE_DANGER);
			m_INT_BtnDel.SetText("Удалить");
			m_INT_BtnDel.OnClick().Insert(INT_OnButton);
		}
	}

	// ------------------------------------------------------------------ getters (menu saves)
	INT_VPP_MapItem INT_GetItem()
	{
		return m_INT_Item;
	}

	vector INT_GetPos()
	{
		return m_INT_Pos;
	}

	string INT_GetName()
	{
		string nm = "";
		if (m_INT_Name)
			nm = m_INT_Name.INT_GetText();
		string clean = INT_VPP_Validate.CleanText(nm, INT_VPP_Net.NAME_MAX_BYTES);
		if (clean == "")
			clean = "Метка";
		return clean;
	}

	int INT_GetColor()
	{
		return m_INT_Color;
	}

	string INT_GetIconPath()
	{
		return m_INT_IconPath;
	}

	int INT_GetVis()
	{
		return m_INT_Vis;
	}

	bool INT_GetOnlyServer()
	{
		return m_INT_OnlyServer;
	}

	bool INT_IsLeft()
	{
		return m_INT_Left;
	}

	bool INT_IsTyping()
	{
		if (m_INT_Name && m_INT_Name.INT_IsFocused())
			return true;
		return false;
	}

	void INT_Blur()
	{
		if (m_INT_Name)
			m_INT_Name.INT_Blur();
	}

	void INT_FocusName()
	{
		if (m_INT_Name)
			m_INT_Name.INT_Focus();
	}

	// pin from the ghost (absolute screen px) to the card edge
	void INT_Place(float ax, float ay, bool on, float f)
	{
		if (!m_INT_Root || !m_INT_PinLine || !m_INT_PinDia)
			return;
		if (f <= 0)
			return;
		bool show = on;
		float mx = ax / f;
		float my = ay / f;
		// where the card stands now (its default side, or where the player dragged it, 2026-11-14)
		float rx;
		float ry;
		m_INT_Root.GetPos(rx, ry);
		float x0 = rx / f;
		float y0 = ry / f;
		float py = my - y0;
		if (py < 20 || py > CARD_H - 20)
			show = false;
		// the pin leaves the card's side that faces the point; a point behind the card has no pin
		float x1 = mx + 48 - x0;
		float x2 = 0;
		if (mx - 48 >= x0 + CARD_W)
		{
			x1 = mx - 48 - x0;
			x2 = CARD_W;
		}
		else if (x1 > x2)
		{
			show = false;
		}
		if (m_INT_PinLine.IsVisible() != show)
			m_INT_PinLine.Show(show);
		if (m_INT_PinDia.IsVisible() != show)
			m_INT_PinDia.Show(show);
		if (!show)
			return;
		float a = x1;
		float b = x2;
		if (b < a)
		{
			a = x2;
			b = x1;
		}
		INT_UIF_Draw.Place(m_INT_PinLine, a, py, b - a, 1);
		float dw = 10 * INT_VPPX_Look.SQRT2;
		INT_UIF_Draw.Move(m_INT_PinDia, x2 - dw * 0.5, py - dw * 0.5);
	}

	// ------------------------------------------------------------------ events
	void INT_OnNameChanged(string t)
	{
		INT_PaintPreview();
	}

	void INT_OnNameSubmit(string t)
	{
		if (m_INT_Menu)
			m_INT_Menu.INT_EditorSave();
	}

	void INT_OnClose(INT_VPP_IconBtn b)
	{
		if (m_INT_Menu)
			m_INT_Menu.INT_CancelEditor();
	}

	void INT_OnOnly(bool on, INT_VPPX_Toggle t)
	{
		m_INT_OnlyServer = on;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
	}

	void INT_OnName3D(bool on, INT_VPPX_Toggle t)
	{
		m_INT_Name3D = on;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
	}

	// the card's blocker = its drag handle on the map screen (2026-11-14)
	ButtonWidget INT_GetBlock()
	{
		return m_INT_Block;
	}

	// «Название в 3D» of the card (true = the name is drawn next to the 3D icon)
	bool INT_GetName3D()
	{
		return m_INT_Name3D;
	}

	// TEST: the switch's own path
	void INT_TestSetName3D(bool on)
	{
		if (m_INT_Name3DToggle)
			m_INT_Name3DToggle.SetOn(on, true, false);
	}

	void INT_OnSeg(int index, INT_VPPX_HSegment s)
	{
		m_INT_Vis = index;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
	}

	void INT_OnButton(INT_VPPX_HButton b)
	{
		if (!m_INT_Menu)
			return;
		if (b == m_INT_BtnSave)
			m_INT_Menu.INT_EditorSave();
		else if (b == m_INT_BtnCancel)
			m_INT_Menu.INT_CancelEditor();
		else if (b == m_INT_BtnDel)
			m_INT_Menu.INT_EditorDelete();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
		{
			if (w == m_INT_Block)
				return true;
			return false;
		}
		ButtonWidget bw = ButtonWidget.Cast(w);
		int sw = m_INT_SwHits.Find(bw);
		if (sw >= 0)
		{
			m_INT_Color = m_INT_Palette.Get(sw);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
			INT_PaintSwatch();
			INT_PaintPreview();
			return true;
		}
		int cell = m_INT_CellHits.Find(bw);
		if (cell >= 0)
		{
			string iid = INT_CellIconId(cell);
			string path = INT_VPP_Icons.INT_PathOf(iid);
			if (path != "")
			{
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
				m_INT_SelCell = cell;
				m_INT_IconId = iid;
				m_INT_IconPath = path;
				INT_PaintCells();
				INT_PaintPreview();
			}
			return true;
		}
		if (w == m_INT_Block)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		if (w == m_INT_Block)
			return true;
		if (m_INT_SwHits.Find(bw) >= 0 || m_INT_CellHits.Find(bw) >= 0)
			return true;
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int cell = m_INT_CellHits.Find(bw);
		if (cell < 0)
			return false;
		m_INT_HovCell = cell;
		INT_PaintCells();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.HOVER);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int cell = m_INT_CellHits.Find(bw);
		if (cell < 0 || cell != m_INT_HovCell)
			return false;
		m_INT_HovCell = -1;
		INT_PaintCells();
		return false;
	}
}

// ----------------------------------------------------------------------------
// Route ribbon (2_b_survey_route: .rib at 524, 874 1156 x 182) + the next point's pinned card.
class INT_VPP_RouteRibbon : INT_VPP_CardBase
{
	static const float RIB_W = 1156;
	static const float RIB_H = 182;
	static const float NEXT_W = 330;
	static const float NEXT_H = 150;
	static const int MAX_NODES = 7;
	static const float LY = 92;

	protected Widget m_INT_Next;
	protected ImageWidget m_INT_NextPinLine;
	protected ImageWidget m_INT_NextPinDia;
	protected ButtonWidget m_INT_Block;
	protected ButtonWidget m_INT_NextBlock;
	protected ref array<ButtonWidget> m_INT_DelHits;
	protected ref array<int> m_INT_DelIdx;
	protected ref INT_VPPX_Toggle m_INT_Auto;
	protected ref INT_VPPX_HButton m_INT_BtnReverse;
	protected ref INT_VPPX_HButton m_INT_BtnClear;
	protected TextWidget m_INT_FirstLeg;
	protected TextWidget m_INT_NextDist;
	protected TextWidget m_INT_NextEta;
	protected TextWidget m_INT_NextUnit;
	protected bool m_INT_HasNext;

	void INT_VPP_RouteRibbon()
	{
		m_INT_DelHits = new array<ButtonWidget>();
		m_INT_DelIdx = new array<int>();
	}

	// the ribbon's blocker = its drag handle on the map screen (2026-11-14)
	ButtonWidget INT_GetBlock()
	{
		return m_INT_Block;
	}

	override void INT_Close()
	{
		if (m_INT_Next)
		{
			Widget n = m_INT_Next;
			m_INT_Next = null;
			INT_VPP_Stat.INT_Down(3);
			if (m_INT_Menu)
				m_INT_Menu.INT_Bury(n);
			else
				n.Unlink();
		}
		super.INT_Close();
	}

	void INT_Build(Widget screen, float scrW, float scrH, VPPMapMenu menu, vector me, bool meShown)
	{
		m_INT_Menu = menu;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		m_INT_DelHits.Clear();
		m_INT_DelIdx.Clear();
		INT_VPP_Route route = INT_VPP_Route.INT_Get();
		int n = 0;
		if (route)
			n = route.INT_Count();
		float y0 = scrH - 24 - RIB_H;
		if (!INT_NewRoot(screen, 524, y0, RIB_W, RIB_H, "IntVppRibbon"))
			return;
		INT_VPPX_Look.Fill(m_INT_Root, 0, 0, RIB_W, RIB_H, INT_VPP_Ui.ISL_BG);
		INT_VPPX_Look.Outline(m_INT_Root, 0, 0, RIB_W, RIB_H, INT_VPP_Ui.ISL_EDGE);
		INT_VPPX_Look.Fill(m_INT_Root, 0, 0, RIB_W, 2, INT_VPPX_Look.ORANGE);
		m_INT_Block = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, RIB_W, RIB_H, this);
		string cap = INT_VPPX_Text.Caps("Маршрут");
		INT_VPPX_Look.Text(m_INT_Root, 24, 12, 120, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		// right column: toggle + buttons
		bool autoOn = true;
		if (route)
			autoOn = route.INT_AutoRemove();
		m_INT_Auto = new INT_VPPX_Toggle();
		m_INT_Auto.Build(m_INT_Root, RIB_W - 24 - 300, 64, 300, "Убирать пройденные", autoOn);
		m_INT_Auto.OnChanged().Insert(INT_OnAuto);
		m_INT_BtnClear = new INT_VPPX_HButton();
		m_INT_BtnClear.Build(m_INT_Root, RIB_W - 24 - 140, 132, 140, 34, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnClear.SetText("Очистить");
		m_INT_BtnClear.OnClick().Insert(INT_OnBtn);
		m_INT_BtnReverse = new INT_VPPX_HButton();
		m_INT_BtnReverse.Build(m_INT_Root, RIB_W - 24 - 140 - 8 - 160, 132, 160, 34, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnReverse.SetText("Развернуть");
		m_INT_BtnReverse.OnClick().Insert(INT_OnBtn);
		if (n <= 0)
		{
			INT_VPPX_Look.Text(m_INT_Root, 150, 8, 600, 26, "нет точек", 17, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
			INT_VPPX_Look.Text(m_INT_Root, 60, 76, 640, 30, "ПКМ по карте — добавить точку маршрута", 17, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
			m_INT_BtnClear.SetEnabled(false);
			m_INT_BtnReverse.SetEnabled(false);
			INT_VPP_Ui.INT_Sort(m_INT_Root);
			return;
		}
		INT_VPP_RoutePoint lastPt = route.INT_At(n - 1);
		string lastName = "";
		if (lastPt)
			lastName = lastPt.m_INT_Name;
		string toWhom = "до " + lastName;
		string toFit = INT_VPPX_Look.FitText(toWhom, 17, false, 360);
		INT_VPPX_Look.Text(m_INT_Root, 150, 8, 400, 26, toFit, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		// chain: me + the points still to walk (passed ones are left out; numbers continue: INT_NumberOf)
		int nextIdx = route.INT_NextIndex();
		if (nextIdx < 0)
			nextIdx = 0;
		array<vector> chain = new array<vector>();
		array<string> names = new array<string>();
		array<int> realIdx = new array<int>();
		if (meShown)
		{
			chain.Insert(me);
			names.Insert("вы здесь");
			realIdx.Insert(-1);
		}
		for (int i = nextIdx; i < n; i++)
		{
			INT_VPP_RoutePoint rp = route.INT_At(i);
			if (!rp)
				continue;
			chain.Insert(rp.m_INT_Pos);
			names.Insert(rp.m_INT_Name);
			realIdx.Insert(i);
		}
		float total = 0;
		for (int t = 0; t + 1 < chain.Count(); t++)
		{
			vector ta = chain.Get(t);
			vector tb = chain.Get(t + 1);
			float legM = INT_VPP_Fmt.INT_Dist2D(ta, tb);
			total = total + legM;
		}
		INT_BuildTotal(total);
		// nodes (me + up to MAX_NODES - 1 points; the rest is a «+N» note)
		int nodes = chain.Count();
		int more = 0;
		if (nodes > MAX_NODES)
		{
			more = nodes - MAX_NODES;
			nodes = MAX_NODES;
		}
		float xs0 = 60;
		float xsEnd = 720;
		float span = 220;
		if (nodes > 1)
			span = (xsEnd - xs0) / (nodes - 1);
		if (span > 220)
			span = 220;
		float lastX = xs0 + span * (nodes - 1);
		INT_VPPX_Look.Fill(m_INT_Root, xs0, LY, lastX - xs0, 1, 0x73FF7A2F);
		float firstEnd = lastX;
		if (nodes > 1)
			firstEnd = xs0 + span;
		INT_VPPX_Look.Fill(m_INT_Root, xs0, LY - 1, firstEnd - xs0, 3, INT_VPPX_Look.ORANGE);
		bool namesFit = false;
		if (span >= 120)
			namesFit = true;
		for (int k = 0; k < nodes; k++)
		{
			float nx = xs0 + span * k;
			bool isMe = false;
			int ptIdx = realIdx.Get(k);
			if (ptIdx < 0)
				isMe = true;
			bool isNext = false;
			if (ptIdx == nextIdx)
				isNext = true;
			bool isLast = false;
			if (ptIdx == n - 1)
				isLast = true;
			string top = "";
			string nm = names.Get(k);
			if (isMe)
			{
				INT_UIF_Draw.Texture(m_INT_Root, nx - 12, LY - 12, 24, 24, INT_VPP_Icons.INT_UI_ME, INT_VPPX_Look.ORANGE);
				top = "Я";
			}
			else
			{
				int fillCol = INT_VPP_Ui.ISL_BG;
				if (isNext)
					fillCol = INT_VPPX_Look.ORANGE;
				INT_UIF_Draw.Texture(m_INT_Root, nx - 9, LY - 9, 18, 18, INT_VPP_Icons.INT_UI_ROUTEPT_FILL, fillCol);
				INT_UIF_Draw.Texture(m_INT_Root, nx - 9, LY - 9, 18, 18, INT_VPP_Icons.INT_UI_ROUTEPT, INT_VPPX_Look.ORANGE);
				int num = route.INT_NumberOf(ptIdx);
				top = INT_VPPX_Text.Pad2(num);
				if (isNext)
					top = top + " · следующая";
				else if (isLast)
					top = top + " · финиш";
				// remove ×
				INT_VPPX_Look.IconAt(m_INT_Root, nx + 12, LY - 26, 12, "close", INT_VPP_Ui.M);
				ButtonWidget del = INT_UIF_Draw.Hit(m_INT_Root, nx + 6, LY - 32, 24, 24, this);
				m_INT_DelHits.Insert(del);
				m_INT_DelIdx.Insert(ptIdx);
			}
			int topCol = INT_VPP_Ui.M;
			if (isNext)
				topCol = INT_VPPX_Look.ORANGE;
			string topCaps = INT_VPPX_Text.Caps(top);
			float cellW = span;
			if (cellW < 60)
				cellW = 60;
			if (namesFit || isNext || isMe)
			{
				INT_VPPX_Look.Text(m_INT_Root, nx - 110, LY + 14, 220, 16, topCaps, 11, topCol, INT_UIF_Align.CENTER, true);
				string nmFit = INT_VPPX_Look.FitText(nm, 15, false, cellW - 12);
				INT_VPPX_Look.Text(m_INT_Root, nx - 110, LY + 30, 220, 22, nmFit, 15, INT_VPP_Ui.T2, INT_UIF_Align.CENTER, false);
			}
			else
			{
				int numShort = route.INT_NumberOf(ptIdx);
				string numOnly = INT_VPPX_Text.Pad2(numShort);
				INT_VPPX_Look.Text(m_INT_Root, nx - 30, LY + 14, 60, 16, numOnly, 11, topCol, INT_UIF_Align.CENTER, true);
			}
			// leg label above the line
			if (k + 1 < nodes)
			{
				vector ka = chain.Get(k);
				vector kb = chain.Get(k + 1);
				float legL = INT_VPP_Fmt.INT_Dist2D(ka, kb);
				string legS = INT_VPP_Fmt.INT_Dist(legL);
				int legCol = INT_VPPX_Look.WHITE;
				if (k == 0)
					legCol = INT_VPPX_Look.ORANGE;
				float lx = nx + span * 0.5;
				TextWidget legW = INT_VPPX_Look.Text(m_INT_Root, lx - 60, LY - 30, 120, 22, legS, 15, legCol, INT_UIF_Align.CENTER, false);
				if (k == 0 && meShown)
					m_INT_FirstLeg = legW;
			}
		}
		if (more > 0)
		{
			string moreS = "+" + more.ToString();
			INT_VPPX_Look.Text(m_INT_Root, lastX + 20, LY - 11, 80, 22, moreS, 15, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		}
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		INT_BuildNext(route, me, meShown);
	}

	protected void INT_BuildTotal(float total)
	{
		// right aligned: ИТОГО 845 м ≈ 3 мин бегом · 9 шагом
		string itogo = INT_VPPX_Text.Caps("Итого");
		string num = INT_VPP_Fmt.INT_DistNum(total);
		string unit = INT_VPP_Fmt.INT_DistUnit(total);
		int run = INT_VPP_Fmt.INT_RunMin(total);
		int walk = INT_VPP_Fmt.INT_WalkMin(total);
		string eta = "≈ " + run.ToString() + " мин бегом · " + walk.ToString() + " шагом";
		float etaW = INT_VPPX_Look.Measure(eta, 14);
		float unitW = INT_VPPX_Look.Measure(unit, 15);
		float numW = INT_VPPX_Look.Measure(num, 30);
		float capW = INT_VPPX_Look.Measure(itogo, 11, true);
		float x = RIB_W - 24 - etaW;
		INT_VPPX_Look.Text(m_INT_Root, x, 16, etaW + 40, 22, eta, 14, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
		x = x - 10 - unitW;
		INT_VPPX_Look.Text(m_INT_Root, x, 16, unitW + 30, 22, unit, 15, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		x = x - 8 - numW;
		INT_VPPX_Look.Text(m_INT_Root, x, 2, numW + 40, 40, num, 30, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		x = x - 10 - capW;
		INT_VPPX_Look.Text(m_INT_Root, x, 18, capW + 30, 18, itogo, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
	}

	// the next point's card, pinned to it (СЛЕДУЮЩАЯ · 01 / name / 195 м ≈ 1 мин · ЮЗ)
	protected void INT_BuildNext(INT_VPP_Route route, vector me, bool meShown)
	{
		m_INT_HasNext = false;
		if (!route || route.INT_Count() <= 0)
			return;
		int nextIdx = route.INT_NextIndex();
		if (nextIdx < 0)
			return;
		INT_VPP_RoutePoint rp = route.INT_At(nextIdx);
		if (!rp)
			return;
		m_INT_Next = INT_UIF_Draw.Frame(m_INT_Root.GetParent(), 0, 0, NEXT_W, NEXT_H, "IntVppNext");
		if (!m_INT_Next)
			return;
		INT_VPP_Stat.INT_Up(3);
		m_INT_HasNext = true;
		m_INT_NextPinLine = INT_VPPX_Look.Fill(m_INT_Next, 0, 0, 10, 1, INT_VPPX_Look.ORANGE);
		m_INT_NextPinDia = INT_VPPX_Look.Diamond(m_INT_Next, 0, 0, 10, true, INT_VPPX_Look.ORANGE);
		Widget p = INT_VPP_Ui.INT_Panel(m_INT_Next, 0, 0, NEXT_W, NEXT_H, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppNextP");
		if (!p)
			return;
		m_INT_NextBlock = INT_UIF_Draw.Hit(p, 0, 0, NEXT_W, NEXT_H, this);
		int nextNum = route.INT_NumberOf(nextIdx);
		string nextPad = INT_VPPX_Text.Pad2(nextNum);
		string tt = INT_VPPX_Text.Caps("Следующая · " + nextPad);
		INT_VPPX_Look.Text(p, 16, 12, 280, 18, tt, 12, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		string nm = INT_VPPX_Look.FitText(rp.m_INT_Name, 24, false, NEXT_W - 30);
		INT_VPPX_Look.Text(p, 16, 32, NEXT_W - 20, 32, nm, 24, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_NextDist = INT_UIF_Draw.Text(p, 16, 68, 150, 52, " ", 40, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		m_INT_NextUnit = INT_VPPX_Look.Text(p, 120, 92, 40, 22, " ", 16, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		m_INT_NextEta = INT_VPPX_Look.Text(p, 160, 92, NEXT_W - 170, 22, " ", 14, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
		if (!meShown)
		{
			string co = INT_VPP_Fmt.INT_Coords(rp.m_INT_Pos);
			INT_VPPX_Look.Text(p, 16, 120, NEXT_W - 30, 20, co, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		}
		INT_VPP_Ui.INT_Sort(m_INT_Next);
		m_INT_Next.Show(false);
		INT_UpdateMe(me, meShown);
	}

	// live texts that depend on the player's position (first leg, next distance)
	void INT_UpdateMe(vector me, bool meShown)
	{
		INT_VPP_Route route = INT_VPP_Route.INT_Get();
		if (!route || route.INT_Count() <= 0)
			return;
		int nextIdx = route.INT_NextIndex();
		if (nextIdx < 0)
			return;
		INT_VPP_RoutePoint rp = route.INT_At(nextIdx);
		if (!rp)
			return;
		if (!meShown)
		{
			if (m_INT_NextDist)
				m_INT_NextDist.SetText("—");
			return;
		}
		float d = route.INT_DistToNext(me);
		string ds = INT_VPP_Fmt.INT_Dist(d);
		if (m_INT_FirstLeg)
			m_INT_FirstLeg.SetText(ds);
		string num = INT_VPP_Fmt.INT_DistNum(d);
		string unit = INT_VPP_Fmt.INT_DistUnit(d);
		int run = INT_VPP_Fmt.INT_RunMin(d);
		float brg = INT_VPP_Fmt.INT_Bearing(me, rp.m_INT_Pos);
		string dw = INT_VPP_Fmt.INT_DirWord(brg);
		string eta = "≈ " + run.ToString() + " мин · " + dw;
		float numW = INT_VPPX_Look.Measure(num, 40);
		if (m_INT_NextDist)
			m_INT_NextDist.SetText(num);
		if (m_INT_NextUnit)
		{
			m_INT_NextUnit.SetText(unit);
			INT_UIF_Draw.Move(m_INT_NextUnit, 16 + numW + 6, 92);
		}
		if (m_INT_NextEta)
		{
			float ux = 16 + numW + 6 + INT_VPPX_Look.Measure(unit, 16) + 12;
			m_INT_NextEta.SetText(eta);
			INT_UIF_Draw.Move(m_INT_NextEta, ux, 92);
		}
	}

	// the next point's absolute screen px
	void INT_PlaceNext(float ax, float ay, bool on, float f)
	{
		if (!m_INT_Next)
			return;
		if (!on || f <= 0)
		{
			if (m_INT_Next.IsVisible())
				m_INT_Next.Show(false);
			return;
		}
		float mx = ax / f;
		float my = ay / f;
		float cx;
		float cy;
		bool left;
		// keep clear of the ribbon at the bottom
		float saveH = m_INT_ScrH;
		m_INT_ScrH = m_INT_ScrH - RIB_H - 16;
		INT_SideOf(mx, my, 96, NEXT_W, NEXT_H, 30, cx, cy, left);
		m_INT_ScrH = saveH;
		INT_UIF_Draw.Move(m_INT_Next, cx, cy);
		float py = my - cy;
		float x1 = mx + 22 - cx;
		float x2 = 0;
		if (left)
		{
			x1 = mx - 22 - cx;
			x2 = NEXT_W;
		}
		float a = x1;
		float b = x2;
		if (b < a)
		{
			a = x2;
			b = x1;
		}
		if (m_INT_NextPinLine)
			INT_UIF_Draw.Place(m_INT_NextPinLine, a, py, b - a, 1);
		if (m_INT_NextPinDia)
		{
			float dw = 10 * INT_VPPX_Look.SQRT2;
			INT_UIF_Draw.Move(m_INT_NextPinDia, x2 - dw * 0.5, py - dw * 0.5);
		}
		if (!m_INT_Next.IsVisible())
			m_INT_Next.Show(true);
	}

	void INT_OnAuto(bool on, INT_VPPX_Toggle t)
	{
		if (m_INT_Menu)
			m_INT_Menu.INT_RouteAutoRemove(on);
	}

	void INT_OnBtn(INT_VPPX_HButton b)
	{
		if (!m_INT_Menu)
			return;
		if (b == m_INT_BtnClear)
			m_INT_Menu.INT_RouteClear();
		else if (b == m_INT_BtnReverse)
			m_INT_Menu.INT_RouteReverse();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		int del = m_INT_DelHits.Find(bw);
		if (del >= 0 && button == MouseState.LEFT)
		{
			int idx = m_INT_DelIdx.Get(del);
			if (m_INT_Menu)
				m_INT_Menu.INT_RouteRemove(idx);
			return true;
		}
		if (w == m_INT_Block || w == m_INT_NextBlock)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		if (w == m_INT_Block || w == m_INT_NextBlock || m_INT_DelHits.Find(bw) >= 0)
			return true;
		return false;
	}
}

// ----------------------------------------------------------------------------
// Ruler card: pinned to the last point (or under the top-left island while empty).
class INT_VPP_RulerCard : INT_VPP_CardBase
{
	static const float CARD_W = 320;

	protected ImageWidget m_INT_PinLine;
	protected ImageWidget m_INT_PinDia;
	protected ButtonWidget m_INT_Block;
	protected bool m_INT_Fixed;

	void INT_Build(Widget screen, float scrW, float scrH, VPPMapMenu menu, array<vector> pts, int maxPts)
	{
		m_INT_Menu = menu;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		int n = 0;
		if (pts)
			n = pts.Count();
		int legsShown = n - 1;
		if (legsShown < 0)
			legsShown = 0;
		if (legsShown > 4)
			legsShown = 4;
		float h = 148 + legsShown * 22;
		m_INT_Fixed = false;
		if (n == 0)
			m_INT_Fixed = true;
		if (!INT_NewRoot(screen, 24, 130, CARD_W, h, "IntVppRuler"))
			return;
		m_INT_PinLine = INT_VPPX_Look.Fill(m_INT_Root, 0, 0, 10, 1, INT_VPPX_Look.ORANGE);
		m_INT_PinDia = INT_VPPX_Look.Diamond(m_INT_Root, 0, 0, 10, true, INT_VPPX_Look.ORANGE);
		Widget p = INT_VPP_Ui.INT_Panel(m_INT_Root, 0, 0, CARD_W, h, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppRulerP");
		if (!p)
			return;
		m_INT_Block = INT_UIF_Draw.Hit(p, 0, 0, CARD_W, h, this);
		string cap = INT_VPPX_Text.Caps("Линейка");
		INT_VPPX_Look.Text(p, 16, 12, 200, 18, cap, 12, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		string cnt = n.ToString() + " / " + maxPts.ToString();
		INT_VPPX_Look.Text(p, CARD_W - 16 - 100, 12, 100, 18, cnt, 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
		float total = 0;
		for (int i = 0; i + 1 < n; i++)
		{
			vector pa = pts.Get(i);
			vector pb = pts.Get(i + 1);
			float leg = INT_VPP_Fmt.INT_Dist2D(pa, pb);
			total = total + leg;
		}
		if (n < 2)
		{
			string hint = "ЛКМ по карте — первая точка";
			if (n == 1)
				hint = "ЛКМ по карте — следующая точка";
			INT_VPPX_Look.Text(p, 16, 40, CARD_W - 30, 24, hint, 15, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		}
		else
		{
			string num = INT_VPP_Fmt.INT_DistNum(total);
			string unit = INT_VPP_Fmt.INT_DistUnit(total);
			float numW = INT_VPPX_Look.Measure(num, 40);
			INT_UIF_Draw.Text(p, 16, 32, numW + 40, 52, num, 40, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, true);
			INT_VPPX_Look.Text(p, 16 + numW + 6, 56, 60, 22, unit, 16, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		}
		int run = INT_VPP_Fmt.INT_RunMin(total);
		int walk = INT_VPP_Fmt.INT_WalkMin(total);
		string eta = "≈ " + run.ToString() + " мин бегом · " + walk.ToString() + " шагом";
		INT_VPPX_Look.Text(p, 16, 88, CARD_W - 30, 22, eta, 14, INT_VPP_Ui.T2, INT_UIF_Align.LEFT, false);
		// the last legs
		float ly = 114;
		int firstLeg = n - 1 - legsShown;
		for (int l = 0; l < legsShown; l++)
		{
			int li = firstLeg + l;
			vector la = pts.Get(li);
			vector lb = pts.Get(li + 1);
			float lm = INT_VPP_Fmt.INT_Dist2D(la, lb);
			string ls = INT_VPP_Fmt.INT_Dist(lm);
			int fromN = li + 1;
			int toN = li + 2;
			string lab = fromN.ToString() + " → " + toN.ToString();
			INT_VPPX_Look.Text(p, 16, ly, 120, 20, lab, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
			INT_VPPX_Look.Text(p, CARD_W - 16 - 120, ly, 120, 20, ls, 14, INT_VPPX_Look.WHITE, INT_UIF_Align.RIGHT, false);
			ly = ly + 22;
		}
		INT_VPPX_Look.Text(p, 16, h - 28, CARD_W - 30, 20, "ПКМ — убрать последнюю · R — выход", 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		if (m_INT_PinLine)
			m_INT_PinLine.Show(false);
		if (m_INT_PinDia)
			m_INT_PinDia.Show(false);
	}

	bool INT_IsFixed()
	{
		return m_INT_Fixed;
	}

	// the last ruler point's absolute screen px
	void INT_Place(float ax, float ay, bool on, float f)
	{
		if (!m_INT_Root || m_INT_Fixed)
			return;
		if (!on || f <= 0)
		{
			if (m_INT_Root.IsVisible())
				m_INT_Root.Show(false);
			return;
		}
		float mx = ax / f;
		float my = ay / f;
		float cx;
		float cy;
		bool left;
		INT_SideOf(mx, my, 70, CARD_W, m_INT_H, 40, cx, cy, left);
		INT_UIF_Draw.Move(m_INT_Root, cx, cy);
		float py = my - cy;
		float x1 = mx + 12 - cx;
		float x2 = 0;
		if (left)
		{
			x1 = mx - 12 - cx;
			x2 = CARD_W;
		}
		float a = x1;
		float b = x2;
		if (b < a)
		{
			a = x2;
			b = x1;
		}
		if (m_INT_PinLine)
		{
			INT_UIF_Draw.Place(m_INT_PinLine, a, py, b - a, 1);
			m_INT_PinLine.Show(true);
		}
		if (m_INT_PinDia)
		{
			float dw = 10 * INT_VPPX_Look.SQRT2;
			INT_UIF_Draw.Move(m_INT_PinDia, x2 - dw * 0.5, py - dw * 0.5);
			m_INT_PinDia.Show(true);
		}
		if (!m_INT_Root.IsVisible())
			m_INT_Root.Show(true);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_Block)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_Block)
			return true;
		return false;
	}
}
