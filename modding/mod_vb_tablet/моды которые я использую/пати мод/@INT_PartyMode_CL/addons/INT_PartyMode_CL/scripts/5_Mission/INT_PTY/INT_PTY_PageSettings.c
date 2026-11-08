// INT PartyMode (client) — tab «Настройки» (3_c_expedition_settings.png): «Метки над друзьями», «Треккер группы»,
// «Пинг-маркеры» side by side, then «Предпросмотр» (a live card drawn with the current settings), «Уведомления»
// and «Интерфейс». Every control is bound to INT_PTY_Settings; after each change MarkDirty() + INT_PTY_Hud.Refresh()
// (the window flushes the file when it closes).

class INT_PTY_SetItem : Managed
{
	string Field;
	int Kind;
	float Min;
	float Max;
	float StepV;
	string Unit;
	ref INT_PTY_MToggle Tg;
	ref INT_PTY_MSlider Sl;
	ref INT_PTY_MSeg Sg;
	TextWidget Val;
	TextWidget Label;
}

class INT_PTY_PageSettings : INT_PTY_Page
{
	static const int K_TOGGLE = 0;
	static const int K_SLIDER = 1;
	static const int K_SEG = 2;

	static const float C1 = 80;
	static const float C2 = 540;
	static const float C3 = 1000;
	static const float C4 = 1460;
	static const float CW = 400;
	static const float PREV_W = 380;
	static const float PREV_H = 250;

	protected ref map<string, ref INT_PTY_SetItem> m_Items;
	protected ref array<ref INT_UIF_Surface> m_ShapeTiles;
	protected ref array<ImageWidget> m_ShapeIcons;
	protected TextWidget m_ShapeName;
	protected ImageWidget m_ColorRing;
	protected float m_ColorCy;
	protected Widget m_Prev;
	protected Widget m_Mini;
	protected float m_MiniY;
	protected float m_HudAcc;
	protected bool m_HudPending;
	protected ref array<Widget> m_Cols;

	void INT_PTY_PageSettings()
	{
		m_Items = new map<string, ref INT_PTY_SetItem>();
		m_ShapeTiles = new array<ref INT_UIF_Surface>();
		m_ShapeIcons = new array<ImageWidget>();
		m_Cols = new array<Widget>();
	}

	protected INT_PTY_Settings S()
	{
		return INT_PTY_SettingsStore.Get();
	}

	// --- field access (no reflection: every bound field by name) ------------------------------------------------
	protected int GetI(string f)
	{
		INT_PTY_Settings s = S();
		if (f == "TagsOn")
			return s.TagsOn;
		if (f == "TagsOnDead")
			return s.TagsOnDead;
		if (f == "TagDistance")
			return s.TagDistance;
		if (f == "TagHpStyle")
			return s.TagHpStyle;
		if (f == "TagNameAlpha")
			return s.TagNameAlpha;
		if (f == "TagDistAlpha")
			return s.TagDistAlpha;
		if (f == "TagSize")
			return s.TagSize;
		if (f == "EdgeArrows")
			return s.EdgeArrows;
		if (f == "StatusIcons")
			return s.StatusIcons;
		if (f == "TrackerOn")
			return s.TrackerOn;
		if (f == "TrackerHpStyle")
			return s.TrackerHpStyle;
		if (f == "TrackerAlpha")
			return s.TrackerAlpha;
		if (f == "TrackerSize")
			return s.TrackerSize;
		if (f == "TrackerBlink")
			return s.TrackerBlink;
		if (f == "TrackerOffline")
			return s.TrackerOffline;
		if (f == "PingShape")
			return s.PingShape;
		if (f == "PingColor")
			return s.PingColor;
		if (f == "PingSize")
			return s.PingSize;
		if (f == "PingViewM")
			return s.PingViewM;
		if (f == "PingFarM")
			return s.PingFarM;
		if (f == "PingNearM")
			return s.PingNearM;
		if (f == "PingAimDim")
			return s.PingAimDim;
		if (f == "PingAimZone")
			return s.PingAimZone;
		if (f == "NotifyOnline")
			return s.NotifyOnline;
		if (f == "NotifyInvite")
			return s.NotifyInvite;
		if (f == "NotifySounds")
			return s.NotifySounds;
		if (f == "Tooltips")
			return s.Tooltips;
		if (f == "ClickSounds")
			return s.ClickSounds;
		return 0;
	}

	protected void SetI(string f, int v)
	{
		INT_PTY_Settings s = S();
		if (f == "TagsOn")
			s.TagsOn = v;
		else if (f == "TagsOnDead")
			s.TagsOnDead = v;
		else if (f == "TagDistance")
			s.TagDistance = v;
		else if (f == "TagHpStyle")
			s.TagHpStyle = v;
		else if (f == "TagNameAlpha")
			s.TagNameAlpha = v;
		else if (f == "TagDistAlpha")
			s.TagDistAlpha = v;
		else if (f == "TagSize")
			s.TagSize = v;
		else if (f == "EdgeArrows")
			s.EdgeArrows = v;
		else if (f == "StatusIcons")
			s.StatusIcons = v;
		else if (f == "TrackerOn")
			s.TrackerOn = v;
		else if (f == "TrackerHpStyle")
			s.TrackerHpStyle = v;
		else if (f == "TrackerAlpha")
			s.TrackerAlpha = v;
		else if (f == "TrackerSize")
			s.TrackerSize = v;
		else if (f == "TrackerBlink")
			s.TrackerBlink = v;
		else if (f == "TrackerOffline")
			s.TrackerOffline = v;
		else if (f == "PingShape")
			s.PingShape = v;
		else if (f == "PingColor")
			s.PingColor = v;
		else if (f == "PingSize")
			s.PingSize = v;
		else if (f == "PingViewM")
			s.PingViewM = v;
		else if (f == "PingFarM")
			s.PingFarM = v;
		else if (f == "PingNearM")
			s.PingNearM = v;
		else if (f == "PingAimDim")
			s.PingAimDim = v;
		else if (f == "PingAimZone")
			s.PingAimZone = v;
		else if (f == "NotifyOnline")
			s.NotifyOnline = v;
		else if (f == "NotifyInvite")
			s.NotifyInvite = v;
		else if (f == "NotifySounds")
			s.NotifySounds = v;
		else if (f == "Tooltips")
			s.Tooltips = v;
		else if (f == "ClickSounds")
			s.ClickSounds = v;
	}

	// --- build ------------------------------------------------------------------------------------------------------
	override void Build(bool animate)
	{
		float d = m_Menu.PageDelay();
		m_Items.Clear();
		m_ShapeTiles.Clear();
		m_ShapeIcons.Clear();
		m_Cols.Clear();
		BuildTags();
		BuildTracker();
		BuildPings();
		BuildRight();
		if (!animate)
			return;
		for (int i = 0; i < m_Cols.Count(); i++)
		{
			INT_PTY_MenuFx.BlockIn(m_Cols.Get(i), 0, 16, d + i * 0.06, 0.32);
		}
	}

	protected Widget Col(float x, float w)
	{
		Widget c = INT_UIF_Draw.Frame(m_P, x, 0, w, 1080, "INT_PTY_SetCol");
		m_Cols.Insert(c);
		return c;
	}

	// Section head (.sec-h): "01" orange + the title, hairline under it. Returns the y of the first item.
	protected float SecHead(Widget p, float y, string num, string title, float w)
	{
		if (num == "")
			INT_PTY_MD.Dm(p, 6, y + 22, 8, INT_PTY_MC.O);
		else
			INT_PTY_Look.Text(p, 0, y + 12, 40, 20, INT_PTY_Text.Spaced(num), 13, INT_PTY_MC.O, INT_UIF_Align.LEFT, true);
		INT_PTY_MD.Txt(p, 34, y, w - 34, 40, title, 28, INT_PTY_MC.T);
		INT_PTY_MD.Line(p, 0, y + 48, w, 1, INT_PTY_MC.L);
		return y + 64;
	}

	// Label (16 px) + the ⓘ icon when the item has a tooltip.
	protected TextWidget LabelRow(Widget p, float y, string label, bool info, float w)
	{
		string t = INT_PTY_Text.Tr(label);
		TextWidget lw = INT_PTY_MD.Txt(p, 0, y, w - 70, 26, t, 16, INT_PTY_MC.LABEL);
		if (info)
		{
			float tw = INT_PTY_MD.Measure(p, t, 16);
			INT_PTY_MD.Ico(p, tw + 8, y + 6, 14, "info", INT_PTY_MC.D);
		}
		return lw;
	}

	protected INT_PTY_SetItem NewItem(string field, int kind)
	{
		INT_PTY_SetItem it = new INT_PTY_SetItem();
		it.Field = field;
		it.Kind = kind;
		m_Items.Set(field, it);
		return it;
	}

	protected float Toggle(Widget p, float y, string field, string label, string tip, float w)
	{
		INT_PTY_SetItem it = NewItem(field, K_TOGGLE);
		it.Label = LabelRow(p, y, label, tip != "", w);
		it.Tg = INT_PTY_MToggle.Create(p, w - 50, y, 50, 26, GetI(field) == 1);
		ButtonWidget hit = m_Menu.Hit(p, 0, y - 6, w, 38, "tg:" + field);
		m_Menu.SetHoverText(hit, it.Label, INT_PTY_MC.LABEL, INT_PTY_MC.T);
		SetItemTip(hit, label, tip);
		return y + 42;
	}

	protected void SetItemTip(ButtonWidget hit, string label, string tip)
	{
		string t = tip;
		if (t == "")
			t = "#STR_INT_PTY_W_TIP_GENERIC";
		m_Menu.SetTip(hit, label, t, 380);
	}

	protected float Slider(Widget p, float y, string field, string label, string tip, float lo, float hi, float step, string unit, string note, float w)
	{
		INT_PTY_SetItem it = NewItem(field, K_SLIDER);
		it.Min = lo;
		it.Max = hi;
		it.StepV = step;
		it.Unit = unit;
		it.Label = LabelRow(p, y, label, tip != "", w);
		it.Val = INT_PTY_Look.Text(p, w - 140, y, 140, 26, "", 16, INT_PTY_MC.T, INT_UIF_Align.RIGHT, true);
		it.Sl = INT_PTY_MSlider.Create(p, 0, y + 30, w, 0, note != "");
		RepaintSlider(it, false);
		ButtonWidget hit = m_Menu.Hit(p, 0, y - 6, w, 58, "sl:" + field);
		m_Menu.SetHoverText(hit, it.Label, INT_PTY_MC.LABEL, INT_PTY_MC.T);
		SetItemTip(hit, label, tip);
		m_Menu.SetSound(hit, false);
		float ny = y + 30 + 16 + 16;
		if (note != "")
		{
			INT_PTY_MD.Ico(p, 0, y + 54, 13, "warning", INT_PTY_MC.O);
			INT_PTY_MD.Txt(p, 20, y + 50, w - 20, 20, note, 13, INT_PTY_MC.O);
			ny = ny + 20;
		}
		return ny;
	}

	protected float Seg(Widget p, float y, string field, string label, string tip, array<string> opts, float w)
	{
		INT_PTY_SetItem it = NewItem(field, K_SEG);
		it.Label = LabelRow(p, y, label, tip != "", w);
		ButtonWidget lh = m_Menu.Hit(p, 0, y - 6, w, 34, "lbl:" + field);
		m_Menu.SetHoverText(lh, it.Label, INT_PTY_MC.LABEL, INT_PTY_MC.T);
		SetItemTip(lh, label, tip);
		m_Menu.SetSound(lh, false);
		it.Sg = INT_PTY_MSeg.Create(p, 0, y + 28, w, opts, GetI(field));
		for (int i = 0; i < it.Sg.Count(); i++)
		{
			float ox = it.Sg.m_X.Get(i);
			float ow = it.Sg.m_Wd.Get(i);
			ButtonWidget oh = m_Menu.Hit(p, ox, y + 28, ow, 38, "seg:" + field + ":" + i.ToString());
			m_Menu.SetHoverText(oh, it.Sg.m_Texts.Get(i), INT_PTY_MC.M, INT_PTY_MC.T);
			SetItemTip(oh, label, tip);
		}
		return y + 28 + 38 + 16;
	}

	protected array<string> Opts(string a, string b, string c, string e, string f = "")
	{
		array<string> r = new array<string>();
		r.Insert(a);
		r.Insert(b);
		r.Insert(c);
		r.Insert(e);
		if (f != "")
			r.Insert(f);
		return r;
	}

	protected void BuildTags()
	{
		Widget p = Col(C1, CW);
		float y = SecHead(p, 244, "01", "#STR_INT_PTY_W_SET_TAGS", CW);
		y = Toggle(p, y, "TagsOn", "#STR_INT_PTY_W_SET_TAGS_ON", "#STR_INT_PTY_W_SET_TAGS_ON_T", CW);
		y = Toggle(p, y, "TagsOnDead", "#STR_INT_PTY_W_SET_TAGS_DEAD", "#STR_INT_PTY_W_SET_TAGS_DEAD_T", CW);
		int maxTag = INT_PTY_Client.s_MaxTagDistance;
		if (maxTag < 100)
			maxTag = 100;
		string maxS = maxTag.ToString();
		string note = INT_PTY_Text.Fmt("#STR_INT_PTY_W_SET_MAX_NOTE", maxS);
		y = Slider(p, y, "TagDistance", "#STR_INT_PTY_W_SET_TAG_DIST", "#STR_INT_PTY_W_SET_TAG_DIST_T", 50, maxTag, 25, "m", note, CW);
		array<string> hp = Opts("#STR_INT_PTY_W_HP_DOT", "#STR_INT_PTY_W_HP_BAR", "#STR_INT_PTY_W_HP_PCT", "#STR_INT_PTY_W_HP_NUM", "#STR_INT_PTY_W_HP_NONE");
		y = Seg(p, y, "TagHpStyle", "#STR_INT_PTY_W_SET_TAG_HP", "#STR_INT_PTY_W_SET_TAG_HP_T", hp, CW);
		y = Slider(p, y, "TagNameAlpha", "#STR_INT_PTY_W_SET_NAME_A", "#STR_INT_PTY_W_SET_NAME_A_T", 10, 100, 5, "%", "", CW);
		y = Slider(p, y, "TagDistAlpha", "#STR_INT_PTY_W_SET_DIST_A", "#STR_INT_PTY_W_SET_DIST_A_T", 10, 100, 5, "%", "", CW);
		y = Slider(p, y, "TagSize", "#STR_INT_PTY_W_SET_TAG_SIZE", "#STR_INT_PTY_W_SET_TAG_SIZE_T", 60, 160, 5, "%", "", CW);
		y = Toggle(p, y, "EdgeArrows", "#STR_INT_PTY_W_SET_ARROWS", "#STR_INT_PTY_W_SET_ARROWS_T", CW);
		y = Toggle(p, y, "StatusIcons", "#STR_INT_PTY_W_SET_ICONS", "#STR_INT_PTY_W_SET_ICONS_T", CW);
	}

	protected void BuildTracker()
	{
		Widget p = Col(C2, CW);
		float y = SecHead(p, 244, "02", "#STR_INT_PTY_W_SET_TRK", CW);
		y = Toggle(p, y, "TrackerOn", "#STR_INT_PTY_W_SET_TRK_ON", "#STR_INT_PTY_W_SET_TRK_ON_T", CW);
		array<string> hp = Opts("#STR_INT_PTY_W_HP_PCT", "#STR_INT_PTY_W_HP_BAR2", "#STR_INT_PTY_W_HP_DOT", "#STR_INT_PTY_W_HP_NUM");
		string hpTip = INT_PTY_Text.Fmt("#STR_INT_PTY_W_SET_TRK_HP_T", "86%");
		y = Seg(p, y, "TrackerHpStyle", "#STR_INT_PTY_W_SET_TRK_HP", hpTip, hp, CW);
		y = Slider(p, y, "TrackerAlpha", "#STR_INT_PTY_W_SET_TRK_A", "#STR_INT_PTY_W_SET_TRK_A_T", 20, 100, 5, "%", "", CW);
		y = Slider(p, y, "TrackerSize", "#STR_INT_PTY_W_SET_TRK_SIZE", "#STR_INT_PTY_W_SET_TRK_SIZE_T", 60, 160, 5, "%", "", CW);
		string blink = INT_PTY_Text.Fmt("#STR_INT_PTY_W_SET_TRK_BLINK", "20%");
		y = Toggle(p, y, "TrackerBlink", blink, "#STR_INT_PTY_W_SET_TRK_BLINK_T", CW);
		y = Toggle(p, y, "TrackerOffline", "#STR_INT_PTY_W_SET_TRK_OFF", "#STR_INT_PTY_W_SET_TRK_OFF_T", CW);
		// position on screen: a mini screen + «Двигать в игре» + «Сбросить»
		TextWidget pl = LabelRow(p, y, "#STR_INT_PTY_W_SET_POS", true, CW);
		ButtonWidget ph = m_Menu.Hit(p, 0, y - 6, CW, 34, "lbl:pos");
		m_Menu.SetHoverText(ph, pl, INT_PTY_MC.LABEL, INT_PTY_MC.T);
		m_Menu.SetTip(ph, "#STR_INT_PTY_W_SET_POS", "#STR_INT_PTY_W_SET_POS_T", 380);
		m_Menu.SetSound(ph, false);
		m_MiniY = y + 32;
		m_Mini = INT_UIF_Draw.Frame(p, 0, m_MiniY, CW, 150, "INT_PTY_Mini");
		PaintMini();
		float by = m_MiniY + 162;
		INT_PTY_MButton mv = INT_PTY_MButton.Create(p, 0, by, 40, "move", "#STR_INT_PTY_W_SET_MOVE", INT_PTY_MButton.OL);
		ButtonWidget mh = m_Menu.Hit(mv.m_Root, 0, 0, mv.m_W, mv.m_H, "mover");
		m_Menu.SetHoverBtn(mh, mv);
		m_Menu.SetTip(mh, "#STR_INT_PTY_W_SET_MOVE", "#STR_INT_PTY_W_SET_MOVE_T", 360);
		m_MoveBtn = mv;
		INT_PTY_MButton rs = INT_PTY_MButton.Create(p, mv.m_W + 14, by, 40, "reset", "#STR_INT_PTY_W_SET_RESET", INT_PTY_MButton.GH);
		ButtonWidget rh = m_Menu.Hit(rs.m_Root, 0, 0, rs.m_W, rs.m_H, "resetpos");
		m_Menu.SetHoverBtn(rh, rs);
		m_Menu.SetTip(rh, "#STR_INT_PTY_W_SET_RESET", "#STR_INT_PTY_W_SET_RESET_T", 340);
		m_ResetBtn = rs;
	}

	protected ref INT_PTY_MButton m_MoveBtn;
	protected ref INT_PTY_MButton m_ResetBtn;

	// Mini screen (.mini): where the tracker sits (TrackerX/Y = screen fraction of its corner, -1 = left middle).
	protected void PaintMini()
	{
		if (!m_Mini)
			return;
		INT_UIF_Widgets.ClearChildren(m_Mini);
		Widget p = m_Mini;
		INT_PTY_MD.Box(p, 0, 0, CW, 150, ARGB(255, 22, 24, 21), INT_PTY_MC.L2);
		INT_PTY_Look.Tex(p, 1, 1, CW - 2, 74, "pty_grad_v_ca", ARGB(60, 120, 128, 118));
		INT_PTY_MD.Line(p, 1, 74, CW - 2, 1, ARGB(31, 255, 255, 255));
		INT_PTY_MD.Line(p, 199, 1, 1, 148, ARGB(31, 255, 255, 255));
		INT_PTY_Settings s = S();
		float tw = 74;
		float th = 58;
		float tx = 12;
		float ty = 46;
		float fx = s.TrackerX;
		float fy = s.TrackerY;
		if (fx >= 0 && fy >= 0)
		{
			tx = Math.Clamp(fx * CW, 2, CW - tw - 2);
			ty = Math.Clamp(fy * 150, 2, 150 - th - 2);
		}
		INT_PTY_MD.Line(p, tx, ty, tw, th, ARGB(46, 255, 122, 47));
		INT_PTY_MD.Line(p, tx, ty, tw, 1, INT_PTY_MC.O);
		INT_PTY_MD.Line(p, tx, ty + th - 1, tw, 1, INT_PTY_MC.O);
		INT_PTY_MD.Line(p, tx, ty, 1, th, INT_PTY_MC.O);
		INT_PTY_MD.Line(p, tx + tw - 1, ty, 1, th, INT_PTY_MC.O);
		for (int i = 0; i < 4; i++)
		{
			INT_PTY_MD.Line(p, tx + 8, ty + 10 + i * 12, tw - 16, 2, ARGB(179, 255, 255, 255));
		}
		// where it is, in words
		float cxf = (tx + tw * 0.5) / CW;
		float cyf = (ty + th * 0.5) / 150;
		string hk = "C";
		if (cxf < 0.36)
			hk = "L";
		else if (cxf > 0.64)
			hk = "R";
		string vk = "C";
		if (cyf < 0.36)
			vk = "T";
		else if (cyf > 0.64)
			vk = "B";
		string where = "";
		if (hk == "C" && vk == "C")
			where = INT_PTY_Text.Tr("#STR_INT_PTY_W_POS_C");
		else
		{
			string hw = INT_PTY_Text.Tr("#STR_INT_PTY_W_POS_" + hk);
			string vw = INT_PTY_Text.Tr("#STR_INT_PTY_W_POS_V_" + vk);
			where = hw + " " + vw;
		}
		string caps = INT_PTY_Text.Caps(where);
		float lw = INT_PTY_MD.Measure(p, caps, 13, true);
		float lx = tx + tw + 30;
		bool leftSide = lx + lw > CW - 6;
		if (leftSide)
			lx = tx - 30 - lw;
		float ly = ty + th * 0.5 - 10;
		ImageWidget arr = INT_PTY_MD.Ico(p, lx - 16, ly + 4, 12, "chevron", INT_PTY_MC.T);
		if (arr && !leftSide)
			arr.SetRotation(0, 0, 180);
		if (arr && leftSide)
			INT_UIF_Draw.Move(arr, lx + lw + 4, ly + 4);
		INT_PTY_Look.Text(p, lx, ly, INT_PTY_MD.Room(lw), 20, caps, 13, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
	}

	protected void BuildPings()
	{
		Widget p = Col(C3, CW);
		float y = SecHead(p, 244, "03", "#STR_INT_PTY_W_SET_PING", CW);
		// shapes
		TextWidget sl = LabelRow(p, y, "#STR_INT_PTY_W_SET_SHAPE", false, CW);
		m_ShapeName = INT_PTY_Look.Text(p, CW - 200, y, 200, 26, "", 16, INT_PTY_MC.T, INT_UIF_Align.RIGHT, true);
		for (int i = 0; i < INT_PTY_Shape.COUNT; i++)
		{
			float tx = i * 68;
			INT_UIF_Surface tile = INT_PTY_MD.Box(p, tx, y + 34, 58, 52, 0, INT_PTY_MC.L2);
			m_ShapeTiles.Insert(tile);
			ImageWidget ic = INT_PTY_MD.Ping(p, tx + 29, y + 60, 26, i, INT_PTY_MC.M, false);
			m_ShapeIcons.Insert(ic);
			ButtonWidget sh = m_Menu.Hit(p, tx, y + 34, 58, 52, "shape:" + i.ToString());
			m_Menu.SetTip(sh, INT_PTY_MenuGeo.ShapeLabel(i), "#STR_INT_PTY_W_SET_SHAPE_T", 340);
		}
		y = y + 34 + 52 + 16;
		// colours
		LabelRow(p, y, "#STR_INT_PTY_W_SET_COLOR", false, CW);
		float cy = y + 50;
		m_ColorCy = cy;
		m_ColorRing = INT_PTY_MD.DmLine(p, 15, cy, 28, 0xFFFFFFFF);
		for (int j = 0; j < 9; j++)
		{
			float cx = 15 + j * 37;
			INT_PTY_MD.Dm(p, cx, cy, 18, INT_PTY_MenuGeo.Swatch(j));
			ButtonWidget ch = m_Menu.Hit(p, cx - 17, cy - 17, 34, 34, "color:" + j.ToString());
			m_Menu.SetTip(ch, "#STR_INT_PTY_W_SET_COLOR", "#STR_INT_PTY_W_SET_COLOR_T", 340);
		}
		y = cy + 30;
		RepaintPingLook();
		int maxPing = INT_PTY_Client.s_MaxPingDistance;
		if (maxPing < 100)
			maxPing = 100;
		y = Slider(p, y, "PingSize", "#STR_INT_PTY_W_SET_PING_SIZE", "#STR_INT_PTY_W_SET_PING_SIZE_T", 60, 160, 5, "%", "", CW);
		y = Slider(p, y, "PingViewM", "#STR_INT_PTY_W_SET_PING_VIEW", "#STR_INT_PTY_W_SET_PING_VIEW_T", INT_PTY_Const.PING_VIEW_MIN, INT_PTY_Const.PING_VIEW_MAX, 50, "m", "", CW);
		y = Slider(p, y, "PingFarM", "#STR_INT_PTY_W_SET_PING_FAR", "#STR_INT_PTY_W_SET_PING_FAR_T", 20, maxPing, 10, "m", "", CW);
		y = Slider(p, y, "PingNearM", "#STR_INT_PTY_W_SET_PING_NEAR", "#STR_INT_PTY_W_SET_PING_NEAR_T", 0, 500, 5, "m", "", CW);
		y = Slider(p, y, "PingAimDim", "#STR_INT_PTY_W_SET_PING_DIM", "#STR_INT_PTY_W_SET_PING_DIM_T", 0, 100, 5, "%", "", CW);
		y = Slider(p, y, "PingAimZone", "#STR_INT_PTY_W_SET_PING_ZONE", "#STR_INT_PTY_W_SET_PING_ZONE_T", 50, 300, 10, "%", "", CW);
	}

	protected void RepaintPingLook()
	{
		INT_PTY_Settings s = S();
		for (int i = 0; i < m_ShapeTiles.Count(); i++)
		{
			INT_UIF_Surface tile = m_ShapeTiles.Get(i);
			ImageWidget ic = m_ShapeIcons.Get(i);
			bool on = i == s.PingShape;
			if (tile)
			{
				if (on)
				{
					tile.SetFill(INT_PTY_MC.O_SOFT);
					tile.SetBorder(INT_PTY_MC.O);
				}
				else
				{
					tile.SetFill(0);
					tile.SetBorder(INT_PTY_MC.L2);
				}
			}
			if (ic)
			{
				if (on)
					ic.SetColor(INT_PTY_MC.O);
				else
					ic.SetColor(INT_PTY_MC.M);
			}
		}
		if (m_ShapeName)
			INT_PTY_Look.SetHeavyText(m_ShapeName, INT_PTY_MenuGeo.ShapeLabel(s.PingShape));
		int sw = INT_PTY_MenuGeo.SwatchIndex(s.PingColor);
		if (m_ColorRing)
		{
			m_ColorRing.Show(sw >= 0);
			if (sw >= 0)
				INT_PTY_Look.DiamondPlace(m_ColorRing, 15 + sw * 37, m_ColorCy, 28);
		}
	}

	protected void BuildRight()
	{
		Widget p = Col(C4, PREV_W);
		SecHead(p, 244, "", "#STR_INT_PTY_W_SET_PREV", PREV_W);
		m_Prev = INT_UIF_Draw.Frame(p, 0, 307, PREV_W, PREV_H, "INT_PTY_Prev");
		PaintPreview();
		float y = SecHead(p, 580, "04", "#STR_INT_PTY_W_SET_NOTIFY", PREV_W);
		y = Toggle(p, y, "NotifyOnline", "#STR_INT_PTY_W_SET_N_ONLINE", "#STR_INT_PTY_W_SET_N_ONLINE_T", PREV_W);
		y = Toggle(p, y, "NotifyInvite", "#STR_INT_PTY_W_SET_N_INVITE", "#STR_INT_PTY_W_SET_N_INVITE_T", PREV_W);
		y = Toggle(p, y, "NotifySounds", "#STR_INT_PTY_W_SET_N_SOUND", "#STR_INT_PTY_W_SET_N_SOUND_T", PREV_W);
		y = SecHead(p, y + 12, "05", "#STR_INT_PTY_W_SET_UI", PREV_W);
		y = Toggle(p, y, "Tooltips", "#STR_INT_PTY_W_SET_TIPS", "#STR_INT_PTY_W_SET_TIPS_T", PREV_W);
		y = Toggle(p, y, "ClickSounds", "#STR_INT_PTY_W_SET_CLICKS", "#STR_INT_PTY_W_SET_CLICKS_T", PREV_W);
	}

	// Live preview (.prev): the real in-game frame of the concept (pty_preview_world_co: a 760x500 crop, the card keeps
	// its 1.52 aspect) with a name tag over the survivor's head, a ping on the rock and a tracker row at the bottom,
	// all drawn with the current settings. Anchors are fractions of the image: head (0.31, 0.30), rock (0.92, 0.39).
	protected void PaintPreview()
	{
		if (!m_Prev)
			return;
		INT_UIF_Widgets.ClearChildren(m_Prev);
		Widget p = m_Prev;
		INT_PTY_Settings s = S();
		INT_PTY_MD.Box(p, 0, 0, PREV_W, PREV_H, ARGB(255, 20, 20, 19), 0);
		INT_PTY_Look.Tex(p, 1, 1, PREV_W - 2, PREV_H - 2, "pty_preview_world_co", ARGB(255, 255, 255, 255));
		string name = "";
		int col = INT_PTY_MenuGeo.Swatch(0);
		INT_PTY_Friend f = null;
		if (INT_PTY_Client.s_Friends && INT_PTY_Client.s_Friends.Count() > 0)
			f = INT_PTY_Client.s_Friends.Get(0);
		if (f)
		{
			name = INT_PTY_Client.NameOf(f.Id);
			col = INT_PTY_MenuGeo.FriendColor(f.Id);
		}
		else
		{
			name = INT_PTY_Text.Tr("#STR_INT_PTY_W_PREV_NAME");
		}
		string dist = INT_PTY_Util.DistText(124);
		float headX = PREV_W * 0.31;
		float headY = PREV_H * 0.30;
		// name tag (.ctag): name + distance, health, a stem down to the head
		if (s.TagsOn == 1)
		{
			float k = s.TagSize / 100.0;
			float ts = 16 * k;
			float ds = 15 * k;
			string capsName = INT_PTY_Text.Spaced(INT_PTY_Text.Upper(name));
			float nw = INT_PTY_MD.Measure(p, capsName, ts, true);
			float dw = INT_PTY_MD.Measure(p, dist, ds, true);
			float dmw = 8 * k;
			float total = dmw + 12 + nw + 9 + dw;
			// the name sits at y 0.22 of the picture, the stem reaches down to the cap (0.30)
			float ty = PREV_H * 0.22 - ts * 0.65;
			if (s.TagHpStyle == INT_PTY_HpStyle.TAG_PCT || s.TagHpStyle == INT_PTY_HpStyle.TAG_NUM)
				ty = ty - 9;
			float hy = ty + ts * 1.3 + 2;
			float stemY = hy + 5;
			if (s.TagHpStyle == INT_PTY_HpStyle.TAG_PCT || s.TagHpStyle == INT_PTY_HpStyle.TAG_NUM)
				stemY = hy + 11;
			float stemH = headY - 1 - stemY;
			if (stemH < 3)
				stemH = 3;
			float x0 = headX - total * 0.5;
			INT_PTY_Look.Tex(p, x0 - 44, ty - 20, total + 88, headY - ty + 26, "pty_halo_dark_ca", ARGB(150, 0, 0, 0));
			int na = (int)(255 * s.TagNameAlpha / 100.0);
			int da = (int)(255 * s.TagDistAlpha / 100.0);
			INT_PTY_MD.Dm(p, x0 + dmw * 0.5, ty + ts * 0.62, dmw, col);
			float roomN = INT_PTY_MD.Room(nw);
			float roomD = INT_PTY_MD.Room(dw);
			int colN = ARGB(na, 255, 255, 255);
			int colD = ARGB(da, 255, 122, 47);
			INT_PTY_Look.Text(p, x0 + dmw + 12, ty, roomN, ts * 1.3, capsName, ts, colN, INT_UIF_Align.LEFT, true);
			INT_PTY_Look.Text(p, x0 + dmw + 21 + nw, ty + 1, roomD, ts * 1.3, dist, ds, colD, INT_UIF_Align.LEFT, true);
			float bw = 84 * k;
			if (s.TagHpStyle == INT_PTY_HpStyle.TAG_BAR)
			{
				INT_PTY_MD.Line(p, headX - bw * 0.5, hy, bw, 3, ARGB(71, 255, 255, 255));
				INT_PTY_MD.Line(p, headX - bw * 0.5, hy, bw * 0.86, 3, 0xFFFFFFFF);
			}
			else if (s.TagHpStyle == INT_PTY_HpStyle.TAG_DOT)
			{
				INT_PTY_MD.Dot(p, headX, hy + 1, 9 * k, INT_PTY_MC.G);
			}
			else if (s.TagHpStyle == INT_PTY_HpStyle.TAG_PCT)
			{
				INT_PTY_Look.Text(p, headX - 60, hy - 9, 120, 18, "86%", 14 * k, 0xFFFFFFFF, INT_UIF_Align.CENTER, true);
			}
			else if (s.TagHpStyle == INT_PTY_HpStyle.TAG_NUM)
			{
				INT_PTY_Look.Text(p, headX - 60, hy - 9, 120, 18, "86 / 100", 14 * k, 0xFFFFFFFF, INT_UIF_Align.CENTER, true);
			}
			INT_PTY_MD.Line(p, headX, stemY, 1, stemH, ARGB(140, 255, 255, 255));
		}
		// ping (.cping) on the rock: distance above, the icon at its constant size
		float pk = s.PingSize / 100.0;
		float psz = 22 * pk;		// the HUD ping icon at PingSize 100 % (owner 2026-10-23: half the sheet size)
		float rockX = PREV_W * 0.92;
		float rockY = PREV_H * 0.39;
		INT_PTY_Look.Tex(p, rockX - 40, rockY - psz * 0.5 - 30, 80, psz + 48, "pty_halo_dark_ca", ARGB(140, 0, 0, 0));
		string pd = INT_PTY_Util.DistText(410);
		INT_PTY_Look.Text(p, rockX - 60, rockY - psz * 0.5 - 24, 120, 20, pd, 9, 0xFFFFFFFF, INT_UIF_Align.CENTER, true);
		INT_PTY_MD.Ping(p, rockX, rockY, psz, s.PingShape, s.PingColor, true);
		// tracker row (.tr) at the bottom-left on a dark gradient, scaled by TrackerSize (up to 135 %: the row stays
		// inside the card)
		if (s.TrackerOn == 1)
		{
			float ta = s.TrackerAlpha / 100.0;
			float tk = Math.Min(s.TrackerSize / 100.0, 1.35);
			int bgA = (int)(214 * ta);
			int fg = (int)(255 * ta);
			float rb = PREV_H - 1;
			float rh = 41 * tk;
			float rc = rb - rh * 0.5;
			INT_PTY_Look.Tex(p, 1, rb - rh, 270 * tk, rh, "pty_grad_h_ca", ARGB(bgA, 8, 8, 8));
			int chip = INT_UIF_Color.WithAlpha(col, fg);
			INT_PTY_MD.Dm(p, 18 * tk, rc, 9 * tk, chip);
			string tn = INT_PTY_Look.FitText(name, 16, true, 70);
			int white = ARGB(fg, 255, 255, 255);
			INT_PTY_Look.Text(p, 32 * tk, rc - 11 * tk, 90 * tk, 22 * tk, tn, 16 * tk, white, INT_UIF_Align.LEFT, true);
			float hx = 112 * tk;
			if (s.TrackerHpStyle == INT_PTY_HpStyle.TRK_BAR)
			{
				int track = ARGB((int)(56 * ta), 255, 255, 255);
				INT_PTY_MD.Line(p, hx, rc - 1, 56 * tk, 3, track);
				INT_PTY_MD.Line(p, hx, rc - 1, 48 * tk, 3, white);
			}
			else if (s.TrackerHpStyle == INT_PTY_HpStyle.TRK_DOT)
			{
				int green = INT_UIF_Color.WithAlpha(INT_PTY_MC.G, fg);
				INT_PTY_MD.Dot(p, hx + 6 * tk, rc, 9 * tk, green);
			}
			else if (s.TrackerHpStyle == INT_PTY_HpStyle.TRK_NUM)
			{
				INT_PTY_Look.Text(p, hx, rc - 10 * tk, 80 * tk, 20 * tk, "86 / 100", 14 * tk, white, INT_UIF_Align.LEFT, true);
			}
			else
			{
				INT_PTY_Look.Text(p, hx, rc - 10 * tk, 70 * tk, 20 * tk, "86%", 14 * tk, white, INT_UIF_Align.LEFT, true);
			}
			int distCol = ARGB(fg, 232, 228, 221);
			INT_PTY_Look.Text(p, 184 * tk, rc - 10 * tk, 90 * tk, 20 * tk, dist, 15 * tk, distCol, INT_UIF_Align.LEFT, false);
		}
		// the card edge over the picture
		INT_PTY_MD.Box(p, 0, 0, PREV_W, PREV_H, 0, INT_PTY_MC.L2);
	}

	// --- values ---------------------------------------------------------------------------------------------------
	protected string ValueText(INT_PTY_SetItem it, int v)
	{
		if (it.Unit == "m")
			return INT_PTY_Util.DistText(v);
		return v.ToString() + it.Unit;
	}

	protected void RepaintSlider(INT_PTY_SetItem it, bool animate)
	{
		int v = GetI(it.Field);
		float shown = Math.Clamp(v, it.Min, it.Max);
		if (it.Val)
			it.Val.SetText(ValueText(it, (int)shown));
		float frac = 0;
		if (it.Max > it.Min)
			frac = (shown - it.Min) / (it.Max - it.Min);
		if (it.Sl)
			it.Sl.Set(frac, animate);
	}

	protected int Snap(INT_PTY_SetItem it, float v)
	{
		float c = Math.Clamp(v, it.Min, it.Max);
		if (it.StepV > 0)
			c = it.Min + Math.Round((c - it.Min) / it.StepV) * it.StepV;
		return (int)Math.Clamp(c, it.Min, it.Max);
	}

	// One value changed: store it, keep near < far, repaint the control and the preview, tell the HUD.
	protected void Apply(INT_PTY_SetItem it, int v, bool hudNow)
	{
		int old = GetI(it.Field);
		if (it.Field == "PingNearM")
		{
			int farM = GetI("PingFarM");
			if (v > farM - 10)
				v = farM - 10;
			if (v < 0)
				v = 0;
		}
		if (it.Field == "PingFarM")
		{
			int nearM = GetI("PingNearM");
			if (v < nearM + 10)
				v = nearM + 10;
		}
		if (v == old && !hudNow)
			return;
		SetI(it.Field, v);
		if (it.Kind == K_SLIDER)
			RepaintSlider(it, false);
		INT_PTY_SettingsStore.MarkDirty();
		PaintPreview();
		if (hudNow)
		{
			m_HudPending = false;
			INT_PTY_Hud.Refresh();
		}
		else
		{
			m_HudPending = true;
		}
	}

	protected INT_PTY_SetItem Item(string field)
	{
		INT_PTY_SetItem it;
		if (m_Items.Find(field, it))
			return it;
		return null;
	}

	// --- events ---------------------------------------------------------------------------------------------------
	override bool OnClick(string key, Widget w)
	{
		if (key.IndexOf("tg:") == 0)
		{
			string tf = key.Substring(3, key.Length() - 3);
			INT_PTY_SetItem ti = Item(tf);
			if (!ti || !ti.Tg)
				return true;
			int nv = 1;
			if (GetI(tf) == 1)
				nv = 0;
			ti.Tg.Set(nv == 1, true);
			Apply(ti, nv, true);
			INT_PTY_Sfx.Toggle();
			if (tf == "Tooltips" && nv == 0)
				m_Menu.HideTip();
			return true;
		}
		if (key.IndexOf("seg:") == 0)
		{
			string rest = key.Substring(4, key.Length() - 4);
			int colon = rest.IndexOf(":");
			if (colon < 0)
				return true;
			string sf = rest.Substring(0, colon);
			string si = rest.Substring(colon + 1, rest.Length() - colon - 1);
			int sidx = si.ToInt();
			INT_PTY_SetItem it = Item(sf);
			if (!it || !it.Sg)
				return true;
			if (GetI(sf) == sidx)
				return true;
			it.Sg.Select(sidx, true);
			Apply(it, sidx, true);
			INT_PTY_Sfx.Click();
			return true;
		}
		if (key.IndexOf("shape:") == 0)
		{
			string shs = key.Substring(6, key.Length() - 6);
			int nsh = shs.ToInt();
			INT_PTY_Settings st = S();
			st.PingShape = nsh;
			INT_PTY_SettingsStore.MarkDirty();
			INT_PTY_Hud.Refresh();
			RepaintPingLook();
			PaintPreview();
			INT_PTY_Sfx.Click();
			return true;
		}
		if (key.IndexOf("color:") == 0)
		{
			string cs = key.Substring(6, key.Length() - 6);
			int nci = cs.ToInt();
			int ncol = INT_PTY_MenuGeo.Swatch(nci);
			INT_PTY_Settings sc = S();
			sc.PingColor = ncol;
			INT_PTY_SettingsStore.MarkDirty();
			INT_PTY_Hud.Refresh();
			RepaintPingLook();
			PaintPreview();
			INT_PTY_Sfx.Click();
			return true;
		}
		if (key == "mover")
		{
			// HUD contract: the mover opens while the window is open, closes it on the next frame and brings it
			// back on this tab when the player finishes (Enter / right click / Esc)
			INT_PTY_Sfx.Click();
			INT_PTY_SettingsStore.Flush();
			INT_PTY_Hud.OpenMover();
			return true;
		}
		if (key == "resetpos")
		{
			INT_PTY_Sfx.Click();
			INT_PTY_Hud.ResetTrackerPos();
			PaintMini();
			if (m_Mini)
				INT_PTY_MenuFx.FadeIn(m_Mini, 0, 0.2);
			return true;
		}
		return false;
	}

	override bool OnDown(string key)
	{
		return key.IndexOf("sl:") == 0;
	}

	override void OnSlide(string key, int mx, bool done)
	{
		if (key.IndexOf("sl:") != 0)
			return;
		string f = key.Substring(3, key.Length() - 3);
		INT_PTY_SetItem it = Item(f);
		if (!it || !it.Sl)
			return;
		float frac = it.Sl.FracAt(mx);
		int v = Snap(it, it.Min + frac * (it.Max - it.Min));
		int before = GetI(f);
		Apply(it, v, done);
		if (v != before)
			INT_PTY_Sfx.Hover();
	}

	override bool OnWheel(string key, int wheel)
	{
		if (key.IndexOf("sl:") != 0)
			return false;
		string f = key.Substring(3, key.Length() - 3);
		INT_PTY_SetItem it = Item(f);
		if (!it)
			return true;
		int v = GetI(f);
		float shownV = Math.Clamp(v, it.Min, it.Max);
		int nv = Snap(it, shownV + wheel * it.StepV);
		Apply(it, nv, true);
		INT_PTY_Sfx.Hover();
		return true;
	}

	override void OnData(int what)
	{
		if ((what & INT_PTY_What.FRIENDS) != 0)
			PaintPreview();
	}

	override void Tick(float dt)
	{
		if (!m_HudPending)
			return;
		m_HudAcc = m_HudAcc + dt;
		if (m_HudAcc < 0.12)
			return;
		m_HudAcc = 0;
		m_HudPending = false;
		INT_PTY_Hud.Refresh();
	}

	override void OnLeave()
	{
		if (m_HudPending)
		{
			m_HudPending = false;
			INT_PTY_Hud.Refresh();
		}
	}

	override string TestInfo()
	{
		INT_PTY_Settings s = S();
		string r = "settings items=" + m_Items.Count().ToString() + " TagsOn=" + s.TagsOn.ToString() + " TagDistance=" + s.TagDistance.ToString();
		r = r + " TrackerHpStyle=" + s.TrackerHpStyle.ToString() + " PingShape=" + s.PingShape.ToString() + " PingSize=" + s.PingSize.ToString();
		r = r + " TrackerSize=" + s.TrackerSize.ToString() + " PingViewM=" + s.PingViewM.ToString();
		return r;
	}

	// Test seam: sets a slider like a drag to `frac` and releases it.
	bool TestSlide(string field, float frac)
	{
		INT_PTY_SetItem it = Item(field);
		if (!it || !it.Sl)
			return false;
		int v = Snap(it, it.Min + frac * (it.Max - it.Min));
		Apply(it, v, true);
		return true;
	}

	int TestGet(string field)
	{
		return GetI(field);
	}
}
