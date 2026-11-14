// ============================================================================
// VanillaPPMap «Экспедиция» — the MINIMAP settings panel inside our map (owner 2026-11-13, design mm_settings_1.png
// without the «Ориентация» row: concept A is north-up). Opened by the gear of the wide «Миникарта» row of the «Слои»
// island; built like the «Дальность / Вид 3D-меток» popover (card #141312, edge .15, 2 px orange top, 13 px orange
// caps titles, steppers [−] value [+] of 32 px at x 284, «СБРОСИТЬ» 120 x 28) and it takes the same place (only one
// of the two is open). Everything applies at once and is written to Client.json (INT_VPP_Prefs minimap section):
//   «ВКЛ» toggle (= the check of the row), Размер 60–160 % (10), Масштаб 5 steps (no «Прозрачность» row: removed by
//   the owner 2026-11-13, a MapWidget cannot be see-through), «Окно маршрута» on / off (2026-11-14: the in-game
//   «МАРШРУТ» ribbon at the top while the map is closed)
//   (200 / 350 / 500 / 750 / 1000 m), ПОЛОЖЕНИЕ: a schematic of the screen with the HUD (route ribbon top centre,
//   hotbar bottom centre, stamina, status icons) - a click near one of the 6 diamonds (corners + middles of the left /
//   right sides) = that place, LMB + drag anywhere = own place; the key hint row (live bindings) + where to change
//   them; «видна, когда включён GPS-приёмник» only when the server requires a GPS; «СБРОСИТЬ» (on / off untouched).
// Owned by INT_VPP_Islands (its screen frame, own sort 300, a blocker under the whole card); INT_VPP_Stat kind 13.
// The drag is polled in INT_Tick (LMB held + the cursor), the map's view guard keeps the map still meanwhile.
// ============================================================================

class INT_VPP_MiniPanel : ScriptedWidgetEventHandler
{
	static const float W = 476;
	static const float SCH_X = 18;
	// two stepper rows (52, 96), the «Окно маршрута» switch row (140), the separator 44 px under it, then ПОЛОЖЕНИЕ and
	// the schematic
	static const int ROWS = 2;
	static const float ROW_Y = 52;
	static const float ROW_STEP = 44;
	static const float HUD_ROW_Y = 140;
	static const float SEP_Y = 184;
	static const float SCH_Y = 220;
	static const float SCH_W = 440;
	static const int SCH_BG = 0xFF3D3F3C;
	static const int SCH_MAP = 0xFF566048;
	static const int SCH_HUD = 0xFF121110;
	static const float ANCHOR_HIT = 16;
	// two key rows (2026-11-14): [CTRL][NUM /] ПОКАЗАТЬ, then [CTRL][NUM +][NUM −] МАСШТАБ - one row is wider than 440
	static const float KEYS_STEP = 32;

	static const string TXT_TITLE = "Миникарта";
	static const string TXT_ON = "Вкл";
	static const string TXT_SIZE = "Размер";
	static const string TXT_ZOOM = "Масштаб";
	static const string TXT_ROUTE_HUD = "Окно маршрута";
	static const string TXT_PLACE = "Положение";
	static const string TXT_HINT = "щелчок у ромба — одно из 6 мест · ЛКМ + тянуть — своё место";
	static const string TXT_SHOW = "Показать";
	static const string TXT_KEYS_HINT = "клавиши меняются: Настройки → Управление → DAYZAVR INT MODS";
	static const string TXT_GPS = "видна, когда включён GPS-приёмник";
	static const string TXT_RESET = "Сбросить";

	protected INT_VPP_Islands m_INT_Isl;
	protected Widget m_INT_Root;
	protected float m_INT_H;
	protected float m_INT_ScrW;
	protected float m_INT_ScrH;
	protected float m_INT_SchH;
	protected float m_INT_SchK;
	protected ref INT_VPPX_Toggle m_INT_On;
	protected ref INT_VPPX_Toggle m_INT_RouteHud;
	// the panel's blocker, its drag handle on the map screen (2026-11-14)
	protected ButtonWidget m_INT_DragHandle;
	protected ref array<ButtonWidget> m_INT_Minus;
	protected ref array<ButtonWidget> m_INT_Plus;
	protected ref array<TextWidget> m_INT_Val;
	protected ref array<string> m_INT_RowNames;
	protected ButtonWidget m_INT_Reset;
	protected ButtonWidget m_INT_SchHit;
	protected Widget m_INT_Sch;
	protected Widget m_INT_Frame;
	protected ref array<ImageWidget> m_INT_Anchors;
	protected TextWidget m_INT_PlaceName;
	protected TextWidget m_INT_Gps;
	protected bool m_INT_Painting;
	// drag
	protected bool m_INT_LmbWas;
	protected bool m_INT_Press;
	protected bool m_INT_Drag;
	protected int m_INT_PressAnchor;
	protected float m_INT_PressMX;
	protected float m_INT_PressMY;
	protected float m_INT_GrabX;
	protected float m_INT_GrabY;
	protected float m_INT_DragX;
	protected float m_INT_DragY;

	void INT_VPP_MiniPanel()
	{
		m_INT_Minus = new array<ButtonWidget>();
		m_INT_Plus = new array<ButtonWidget>();
		m_INT_Val = new array<TextWidget>();
		m_INT_RowNames = new array<string>();
		m_INT_Anchors = new array<ImageWidget>();
		m_INT_PressAnchor = -1;
	}

	void ~INT_VPP_MiniPanel()
	{
		INT_Destroy();
	}

	// ------------------------------------------------------------------ build / destroy
	// screen = the islands' screen frame (design W x H); bottom = the top of the layers island
	void INT_Build(INT_VPP_Islands isl, Widget screen, float scrW, float scrH, float bottom)
	{
		m_INT_Isl = isl;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		float schH = SCH_W * scrH / scrW;
		if (schH > 300)
			schH = 300;
		m_INT_SchH = schH;
		m_INT_SchK = SCH_W / scrW;
		float hintY = SCH_Y + schH + 8;
		float keysY = hintY + 26;
		float keysHintY = keysY + 34 + KEYS_STEP;
		float sepY = keysHintY + 24;
		float botY = sepY + 12;
		m_INT_H = botY + 28 + 16;
		float py = bottom - 8 - m_INT_H;
		if (py < 8)
			py = 8;
		Widget root = INT_VPP_Ui.INT_Panel(screen, 24, py, W, m_INT_H, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "IntVppMiniPanel");
		if (!root)
			return;
		m_INT_Root = root;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_MINI_PANEL);
		if (isl)
			m_INT_DragHandle = isl.INT_AddBlocker(root, W, m_INT_H);
		string title = INT_VPPX_Text.Caps(TXT_TITLE);
		INT_VPPX_Look.Text(root, 18, 14, 300, 20, title, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		bool on = true;
		if (prefs)
			on = prefs.INT_MiniIsOn();
		m_INT_On = new INT_VPPX_Toggle();
		m_INT_On.Build(root, W - 18 - 120, 10, 120, TXT_ON, on);
		m_INT_On.OnChanged().Insert(INT_OnToggle);
		array<string> names = new array<string>();
		names.Insert(TXT_SIZE);
		names.Insert(TXT_ZOOM);
		for (int i = 0; i < ROWS; i++)
		{
			float ry = ROW_Y + i * ROW_STEP;
			string nm = names.Get(i);
			m_INT_RowNames.Insert(nm);
			INT_VPPX_Look.Text(root, 18, ry, 200, 32, nm, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			float bx = W - 18 - 32 - 110 - 32;
			INT_VPPX_Look.Fill(root, bx, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(root, bx, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(root, bx + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			ButtonWidget mh = INT_UIF_Draw.Hit(root, bx, ry, 32, 32, this);
			TextWidget vt = INT_VPPX_Look.Text(root, bx + 32, ry, 110, 32, " ", 15, INT_VPPX_Look.WHITE, INT_UIF_Align.CENTER, false);
			float px2 = bx + 32 + 110;
			INT_VPPX_Look.Fill(root, px2, ry, 32, 32, INT_VPP_Ui.BTN_BG);
			INT_VPPX_Look.Outline(root, px2, ry, 32, 32, INT_VPP_Ui.LINE2);
			INT_VPPX_Look.Fill(root, px2 + 10, ry + 15, 12, 2, INT_VPPX_Look.TEXT);
			INT_VPPX_Look.Fill(root, px2 + 15, ry + 10, 2, 12, INT_VPPX_Look.TEXT);
			ButtonWidget ph = INT_UIF_Draw.Hit(root, px2, ry, 32, 32, this);
			m_INT_Minus.Insert(mh);
			m_INT_Plus.Insert(ph);
			m_INT_Val.Insert(vt);
		}
		// «Окно маршрута» (2026-11-14): the in-game route ribbon on / off, the «ВКЛ» style
		bool hudOn = true;
		if (prefs)
			hudOn = prefs.INT_IsRouteHudOn();
		string hudTxt = INT_VPP_Ui.INT_Tr("STR_INT_VPP_ROUTE_HUD");
		INT_VPPX_Look.Text(root, 18, HUD_ROW_Y, 240, 32, hudTxt, 17, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_RouteHud = new INT_VPPX_Toggle();
		m_INT_RouteHud.Build(root, W - 18 - 120, HUD_ROW_Y + 4, 120, TXT_ON, hudOn);
		m_INT_RouteHud.OnChanged().Insert(INT_OnRouteHud);
		INT_VPPX_Look.Fill(root, 18, SEP_Y, W - 36, 1, INT_VPP_Ui.LINE2);
		string ptitle = INT_VPPX_Text.Caps(TXT_PLACE);
		INT_VPPX_Look.Text(root, 18, SEP_Y + 14, 200, 20, ptitle, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		m_INT_PlaceName = INT_VPPX_Look.Text(root, W - 18 - 220, SEP_Y + 12, 220, 22, "справа сверху", 14, INT_VPP_Ui.T2, INT_UIF_Align.RIGHT, false);
		INT_BuildSchematic(root);
		INT_VPPX_Look.Text(root, 18, hintY, W - 36, 18, TXT_HINT, 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		INT_BuildKeys(root, keysY);
		INT_VPPX_Look.Text(root, 18, keysHintY, W - 36, 18, TXT_KEYS_HINT, 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		INT_VPPX_Look.Fill(root, 18, sepY, W - 36, 1, INT_VPP_Ui.LINE2);
		// GPS note (only when the server requires a GPS receiver) + «СБРОСИТЬ»
		float rw = 120;
		float rx = W - 18 - rw;
		INT_VPPX_Look.Fill(root, rx, botY, rw, 28, INT_VPP_Ui.BTN_BG);
		INT_VPPX_Look.Outline(root, rx, botY, rw, 28, INT_VPP_Ui.LINE2);
		string rc = INT_VPPX_Text.Caps(TXT_RESET);
		INT_VPPX_Look.Text(root, rx, botY, rw, 28, rc, 12, INT_VPP_Ui.T2, INT_UIF_Align.CENTER, true);
		m_INT_Reset = INT_UIF_Draw.Hit(root, rx, botY, rw, 28, this);
		bool gps = INT_VPP_MiniMap.INT_ServerRequiresGps();
		if (gps)
		{
			INT_VPPX_Look.IconAt(root, 18, botY + 6, 16, "info", INT_VPP_Ui.M);
			m_INT_Gps = INT_VPPX_Look.Text(root, 42, botY + 3, rx - 50, 22, TXT_GPS, 13, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		}
		INT_VPP_Ui.INT_Sort(root);
		INT_Paint();
		// draggable like every map panel (2026-11-14); a saved place is applied at once
		if (isl && m_INT_DragHandle)
			isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_MINI, root, m_INT_DragHandle);
	}

	void INT_Destroy()
	{
		if (m_INT_On)
		{
			m_INT_On.OnChanged().Clear();
			m_INT_On = null;
		}
		if (m_INT_RouteHud)
		{
			m_INT_RouteHud.OnChanged().Clear();
			m_INT_RouteHud = null;
		}
		if (m_INT_Root)
		{
			m_INT_Root.Unlink();
			INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_MINI_PANEL);
		}
		m_INT_Root = null;
		m_INT_Minus.Clear();
		m_INT_Plus.Clear();
		m_INT_Val.Clear();
		m_INT_RowNames.Clear();
		m_INT_Anchors.Clear();
		m_INT_Reset = null;
		m_INT_SchHit = null;
		m_INT_Sch = null;
		m_INT_Frame = null;
		m_INT_PlaceName = null;
		m_INT_Gps = null;
		m_INT_Press = false;
		m_INT_Drag = false;
		m_INT_Isl = null;
	}

	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	// ------------------------------------------------------------------ schematic of the screen
	protected void INT_BuildSchematic(Widget root)
	{
		float sw = SCH_W;
		float sh = m_INT_SchH;
		float k = m_INT_SchK;
		Widget sch = INT_UIF_Draw.Frame(root, SCH_X, SCH_Y, sw, sh, "IntVppMiniSch");
		if (!sch)
			return;
		m_INT_Sch = sch;
		INT_VPPX_Look.Fill(sch, 0, 0, sw, sh, SCH_BG);
		INT_VPPX_Look.Outline(sch, 0, 0, sw, sh, INT_VPP_Ui.LINE2);
		float scrW = m_INT_ScrW;
		float scrH = m_INT_ScrH;
		// HUD of the game screen: route ribbon (top centre), hotbar (bottom centre), stamina, status icons
		float rx = (scrW - INT_VPP_Hud.INT_RIB_W) * 0.5 * k;
		INT_VPPX_Look.Fill(sch, rx, INT_VPP_Hud.INT_RIB_Y * k, INT_VPP_Hud.INT_RIB_W * k, INT_VPP_Hud.INT_RIB_H * k, SCH_HUD);
		INT_VPPX_Look.Fill(sch, rx, INT_VPP_Hud.INT_RIB_Y * k, INT_VPP_Hud.INT_RIB_W * k, 1, INT_VPPX_Look.ORANGE);
		float hbW = 270 * k;
		float hbX = (scrW * 0.5) * k - hbW * 0.5;
		float hbY = (scrH - 118) * k;
		for (int q = 0; q < 4; q++)
		{
			float qx = hbX + q * hbW / 4 + 2;
			INT_VPPX_Look.Fill(sch, qx, hbY, hbW / 4 - 4, 13, 0x29FFFFFF);
		}
		INT_VPPX_Look.Fill(sch, 165 * k, (scrH - 72) * k, 180 * k, 2, 0xBFFFFFFF);
		array<int> sc = {0xFFE8E04A, 0xFFC7E04A, 0xFF3EC7FF, 0xFFE6E3DE, 0xFFE6E3DE};
		for (int s = 0; s < 5; s++)
		{
			int scol = sc.Get(s);
			INT_VPPX_Look.Fill(sch, (scrW - 308) * k + s * 11, (scrH - 114) * k, 7, 10, scol);
		}
		// the minimap's frame at its place (rebuilt by INT_PaintSchematic)
		m_INT_Frame = INT_UIF_Draw.Frame(sch, 0, 0, 10, 10, "IntVppMiniSchFrame");
		// the 6 places: diamonds at the corners and the side middles
		m_INT_Anchors.Clear();
		for (int a = 0; a < 6; a++)
		{
			float ax;
			float ay;
			INT_AnchorXY(a, ax, ay);
			ImageWidget d = INT_VPPX_Look.Diamond(sch, ax, ay, 9, true, SCH_BG);
			m_INT_Anchors.Insert(d);
		}
		m_INT_SchHit = INT_UIF_Draw.Hit(sch, 0, 0, sw, sh, this);
	}

	// schematic px of place a's diamond (0 TL, 1 TR, 2 ML, 3 MR, 4 BL, 5 BR)
	protected void INT_AnchorXY(int a, out float x, out float y)
	{
		float sw = SCH_W;
		float sh = m_INT_SchH;
		x = 7;
		y = 7;
		if (a == 1 || a == 3 || a == 5)
			x = sw - 7;
		if (a == 2 || a == 3)
			y = sh * 0.5;
		if (a == 4 || a == 5)
			y = sh - 7;
	}

	protected void INT_PaintSchematic(int size, int place, float fx, float fy, bool dragging)
	{
		if (!m_INT_Frame)
			return;
		float k = m_INT_SchK;
		float x;
		float y;
		float w;
		float h;
		INT_VPP_MiniMap.INT_BoxFor(size, place, fx, fy, m_INT_ScrW, m_INT_ScrH, x, y, w, h);
		if (dragging)
		{
			x = m_INT_DragX;
			y = m_INT_DragY;
		}
		Widget f = m_INT_Frame;
		Widget kid = f.GetChildren();
		while (kid)
		{
			Widget next = kid.GetSibling();
			kid.Unlink();
			kid = next;
		}
		float fw = w * k;
		float fh = h * k;
		INT_UIF_Draw.Place(f, x * k, y * k, fw, fh);
		INT_VPPX_Look.Fill(f, 0, 0, fw, fh, SCH_MAP);
		INT_VPPX_Look.Outline(f, 0, 0, fw, fh, 0x40FFFFFF);
		INT_VPPX_Look.Fill(f, 0, 0, fw, 1.5, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.Diamond(f, fw * 0.5, fh * 0.5, 5, true, INT_VPPX_Look.ORANGE);
		INT_VPPX_Look.SortTree(f, 300);
		for (int a = 0; a < m_INT_Anchors.Count(); a++)
		{
			ImageWidget d = m_INT_Anchors.Get(a);
			if (!d)
				continue;
			bool lit = false;
			if (a == place && !dragging)
				lit = true;
			// unlit = a light outline over the screen grey, lit = filled orange
			int c = 0xB3FFFFFF;
			if (lit)
				c = INT_VPPX_Look.ORANGE;
			d.SetColor(c);
			INT_VPPX_Look.SetDiamondFilled(d, lit);
		}
		// the diamonds and the hit stay over the frame
		foreach (ImageWidget dd : m_INT_Anchors)
		{
			if (dd)
				dd.SetSort(300);
		}
		if (m_INT_SchHit)
			m_INT_SchHit.SetSort(300);
	}

	static string INT_PlaceText(int place)
	{
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_TL)
			return "слева сверху";
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_TR)
			return "справа сверху";
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_ML)
			return "слева по центру";
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_MR)
			return "справа по центру";
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_BL)
			return "слева снизу";
		if (place == INT_VPP_Prefs.INT_MINI_PLACE_BR)
			return "справа снизу";
		return "своё место";
	}

	// ------------------------------------------------------------------ keys rows (live bindings)
	protected void INT_BuildKeys(Widget root, float y)
	{
		array<string> mini = new array<string>();
		INT_VPP_MiniPanel.INT_ComboNames("UAIntVppMiniMap", mini);
		// default = CTRL + NUM / (2026-11-14, was «\»)
		if (mini.Count() == 0)
		{
			mini.Insert("Ctrl");
			mini.Insert("Num /");
		}
		float x = 18;
		int nm = mini.Count();
		for (int q = 0; q < nm; q++)
		{
			string km = mini.Get(q);
			float w0 = INT_VPPX_Look.Keycap(root, x, y, km, 26, INT_VPP_Ui.KBD_EDGE, INT_VPPX_Look.TEXT);
			x = x + w0 + 4;
		}
		string capShow = INT_VPPX_Text.Caps(TXT_SHOW);
		float lw = INT_VPPX_Look.Measure(capShow, 11, true);
		INT_VPPX_Look.Text(root, x + 6, y, lw + 20, 26, capShow, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		float y2 = y + KEYS_STEP;
		x = 18;
		array<string> zin = new array<string>();
		INT_VPP_MiniPanel.INT_ComboNames("UAIntVppMiniMapZoomIn", zin);
		array<string> zout = new array<string>();
		INT_VPP_MiniPanel.INT_ComboNames("UAIntVppMiniMapZoomOut", zout);
		// default = CTRL + NUM + / CTRL + NUM -: shown as CTRL, NUM +, NUM - (the modifier once)
		if (zin.Count() == 0)
		{
			zin.Insert("Ctrl");
			zin.Insert("Num +");
		}
		if (zout.Count() == 0)
		{
			zout.Insert("Ctrl");
			zout.Insert("Num −");
		}
		int n = zin.Count();
		for (int i = 0; i < n; i++)
		{
			string kn = zin.Get(i);
			float w1 = INT_VPPX_Look.Keycap(root, x, y2, kn, 26, INT_VPP_Ui.KBD_EDGE, INT_VPPX_Look.TEXT);
			x = x + w1 + 4;
		}
		int m = zout.Count();
		for (int j = 0; j < m; j++)
		{
			string ko = zout.Get(j);
			// the shared modifier is drawn once
			if (j < m - 1 && j < n - 1 && ko == zin.Get(j))
				continue;
			float w2 = INT_VPPX_Look.Keycap(root, x, y2, ko, 26, INT_VPP_Ui.KBD_EDGE, INT_VPPX_Look.TEXT);
			x = x + w2 + 4;
		}
		string capZoom = INT_VPPX_Text.Caps(TXT_ZOOM);
		INT_VPPX_Look.Text(root, x + 6, y2, W - x - 18, 26, capZoom, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
	}

	// the keyboard binding of an input as short key names (empty = not bound)
	static void INT_ComboNames(string input, array<string> dst)
	{
		map<int, ref TStringArray> all = InputUtils.GetComboButtonNamesFromInput(input, EUAINPUT_DEVICE_KEYBOARDMOUSE);
		if (!all || all.Count() == 0)
			return;
		TStringArray first = all.GetElement(0);
		if (!first)
			return;
		foreach (string b : first)
		{
			string t = INT_VPP_MiniPanel.INT_Short(b);
			dst.Insert(t);
		}
	}

	static string INT_KeyName(string input, string def)
	{
		array<string> names = new array<string>();
		INT_VPP_MiniPanel.INT_ComboNames(input, names);
		if (names.Count() == 0)
			return def;
		string s = "";
		int n = names.Count();
		for (int i = 0; i < n; i++)
		{
			if (i > 0)
				s = s + "+";
			s = s + names.Get(i);
		}
		return s;
	}

	// the engine's localised key names made keycap-short (measured on the stand, RU: "Левый Ctrl", "+ (Numpad)"):
	// any Ctrl / Strg -> CTRL, Shift, Alt, a numpad key -> "NUM +" / "NUM −" / "NUM /" (DE "+ (Ziffernblock)" too),
	// others upper case, at most 10 letters
	static string INT_Short(string name)
	{
		string t = INT_VPPX_Text.Tr(name);
		if (t.IndexOf("Ctrl") >= 0 || t.IndexOf("CTRL") >= 0 || t.IndexOf("Strg") >= 0)
			return "CTRL";
		if (t.IndexOf("Shift") >= 0 || t.IndexOf("SHIFT") >= 0)
			return "SHIFT";
		if (t.IndexOf("Alt") >= 0 || t.IndexOf("ALT") >= 0)
			return "ALT";
		int np = t.IndexOf("(Numpad)");
		if (np < 0)
			np = t.IndexOf("Numpad");
		if (np < 0)
			np = t.IndexOf("(Ziffernblock)");
		if (np > 0)
		{
			string sym = t.Substring(0, np);
			sym.TrimInPlace();
			if (sym == "-")
				sym = "−";
			return "NUM " + sym;
		}
		int n = t.LengthUtf8();
		if (n > 10)
			t = t.SubstringUtf8(0, 10);
		string up = INT_VPPX_Text.Upper(t);
		return up;
	}

	// ------------------------------------------------------------------ paint
	void INT_Paint()
	{
		if (!m_INT_Root)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		m_INT_Painting = true;
		bool on = prefs.INT_MiniIsOn();
		if (m_INT_On)
			m_INT_On.SetOn(on, false, false);
		bool hudOn = prefs.INT_IsRouteHudOn();
		if (m_INT_RouteHud)
			m_INT_RouteHud.SetOn(hudOn, false, false);
		m_INT_Painting = false;
		int size = prefs.INT_MiniGetSizePct();
		int zoom = prefs.INT_MiniGetZoom();
		int place = prefs.INT_MiniGetPlace();
		float fx = prefs.INT_MiniGetX();
		float fy = prefs.INT_MiniGetY();
		if (m_INT_Val.Count() >= ROWS)
		{
			TextWidget v0 = m_INT_Val.Get(0);
			if (v0)
				v0.SetText(size.ToString() + " %");
			TextWidget v1 = m_INT_Val.Get(1);
			int zm = INT_VPP_Prefs.INT_MiniZoomMetres(zoom);
			float zf = zm;
			string zs = INT_VPP_Fmt.INT_Dist(zf);
			if (v1)
				v1.SetText(zs);
		}
		if (m_INT_PlaceName)
		{
			string pn = INT_VPP_MiniPanel.INT_PlaceText(place);
			m_INT_PlaceName.SetText(pn);
		}
		INT_PaintSchematic(size, place, fx, fy, false);
	}

	// ------------------------------------------------------------------ actions
	protected void INT_OnToggle(bool on, INT_VPPX_Toggle t)
	{
		if (m_INT_Painting)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		prefs.INT_MiniSetOn(on);
		prefs.INT_Save();
		if (m_INT_Isl)
			m_INT_Isl.INT_RepaintMiniRow();
	}

	// «Окно маршрута» switched (2026-11-14)
	protected void INT_OnRouteHud(bool on, INT_VPPX_Toggle t)
	{
		if (m_INT_Painting)
			return;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		prefs.INT_SetRouteHudOn(on);
		prefs.INT_Save();
	}

	// TEST: the switch's own path
	void INT_TestRouteHud(bool on)
	{
		if (m_INT_RouteHud)
			m_INT_RouteHud.SetOn(on, true, false);
	}

	// row 0 size, 1 zoom; one step
	void INT_Step(int row, int dir)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs || row < 0 || row >= ROWS)
			return;
		bool moved = false;
		if (row == 0)
		{
			int s0 = prefs.INT_MiniGetSizePct();
			int s1 = INT_VPP_Prefs.INT_MiniClampSize(s0 + dir * INT_VPP_Prefs.INT_MINI_STEP);
			if (s1 != s0)
				moved = true;
			prefs.INT_MiniSetSizePct(s1);
		}
		else
		{
			int z0 = prefs.INT_MiniGetZoom();
			int z1 = INT_VPP_Prefs.INT_MiniClampZoom(z0 + dir);
			if (z1 != z0)
				moved = true;
			prefs.INT_MiniSetZoom(z1);
		}
		if (moved)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			prefs.INT_Save();
		}
		else
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		}
		INT_Paint();
	}

	void INT_Reset()
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		int rev = prefs.INT_MiniRev();
		prefs.INT_MiniReset();
		if (prefs.INT_MiniRev() != rev)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			prefs.INT_Save();
		}
		else
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
		}
		INT_Paint();
	}

	void INT_SetPlace(int place)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		prefs.INT_MiniSetPlace(place);
		prefs.INT_Save();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
		INT_Paint();
	}

	// own place from a design top-left corner of the minimap on the screen (the drag end)
	void INT_SetOwnAt(float x, float y)
	{
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		if (!prefs)
			return;
		int size = prefs.INT_MiniGetSizePct();
		float fx;
		float fy;
		INT_VPP_MiniMap.INT_FracFor(size, x, y, m_INT_ScrW, m_INT_ScrH, fx, fy);
		prefs.INT_MiniSetOwnPlace(fx, fy);
		prefs.INT_Save();
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
		INT_Paint();
	}

	// ------------------------------------------------------------------ drag (polled every frame by the islands)
	void INT_Tick(float dt)
	{
		if (!m_INT_Root || !m_INT_Sch)
			return;
		int lmb = GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK;
		bool down = false;
		if (lmb != 0)
			down = true;
		int mx;
		int my;
		GetMousePos(mx, my);
		if (down && !m_INT_LmbWas)
			INT_OnPress(mx, my);
		m_INT_LmbWas = down;
		if (!m_INT_Press)
			return;
		float dxm = mx - m_INT_PressMX;
		float dym = my - m_INT_PressMY;
		if (!m_INT_Drag && (Math.AbsFloat(dxm) > 4 || Math.AbsFloat(dym) > 4))
			m_INT_Drag = true;
		if (m_INT_Drag)
			INT_DragTo(mx, my);
		if (down)
			return;
		// release
		m_INT_Press = false;
		if (m_INT_Drag)
		{
			m_INT_Drag = false;
			INT_SetOwnAt(m_INT_DragX, m_INT_DragY);
			return;
		}
		if (m_INT_PressAnchor >= 0)
		{
			INT_SetPlace(m_INT_PressAnchor);
			return;
		}
		// a click elsewhere: the minimap's centre goes there
		INT_DragTo(mx, my);
		INT_SetOwnAt(m_INT_DragX, m_INT_DragY);
	}

	// schematic px of a screen (physical) point
	protected void INT_SchLocal(int mx, int my, out float sx, out float sy)
	{
		float px;
		float py;
		m_INT_Sch.GetScreenPos(px, py);
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		float fmx = mx;
		float fmy = my;
		sx = (fmx - px) / f;
		sy = (fmy - py) / f;
	}

	protected void INT_OnPress(int mx, int my)
	{
		float sx;
		float sy;
		INT_SchLocal(mx, my, sx, sy);
		if (sx < 0 || sy < 0 || sx > SCH_W || sy > m_INT_SchH)
			return;
		m_INT_Press = true;
		m_INT_Drag = false;
		m_INT_PressMX = mx;
		m_INT_PressMY = my;
		m_INT_PressAnchor = -1;
		for (int a = 0; a < 6; a++)
		{
			float ax;
			float ay;
			INT_AnchorXY(a, ax, ay);
			float dx = sx - ax;
			float dy = sy - ay;
			if (dx * dx + dy * dy <= ANCHOR_HIT * ANCHOR_HIT)
				m_INT_PressAnchor = a;
		}
		// grab: inside the drawn frame = keep the offset, else the frame centres on the cursor
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		int size = INT_VPP_Prefs.INT_MINI_SIZE_DEF;
		int place = INT_VPP_Prefs.INT_MINI_PLACE_DEF;
		float fx = 1;
		float fy = 0;
		if (prefs)
		{
			size = prefs.INT_MiniGetSizePct();
			place = prefs.INT_MiniGetPlace();
			fx = prefs.INT_MiniGetX();
			fy = prefs.INT_MiniGetY();
		}
		float x;
		float y;
		float w;
		float h;
		INT_VPP_MiniMap.INT_BoxFor(size, place, fx, fy, m_INT_ScrW, m_INT_ScrH, x, y, w, h);
		float k = m_INT_SchK;
		float dsx = sx / k;
		float dsy = sy / k;
		m_INT_GrabX = w * 0.5;
		m_INT_GrabY = h * 0.5;
		if (dsx >= x && dsx <= x + w && dsy >= y && dsy <= y + h)
		{
			m_INT_GrabX = dsx - x;
			m_INT_GrabY = dsy - y;
		}
		m_INT_DragX = x;
		m_INT_DragY = y;
	}

	protected void INT_DragTo(int mx, int my)
	{
		float sx;
		float sy;
		INT_SchLocal(mx, my, sx, sy);
		float k = m_INT_SchK;
		INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
		int size = INT_VPP_Prefs.INT_MINI_SIZE_DEF;
		if (prefs)
			size = prefs.INT_MiniGetSizePct();
		float x = sx / k - m_INT_GrabX;
		float y = sy / k - m_INT_GrabY;
		// keep it on the screen (the same 8 px inset as the own place)
		float bx;
		float by;
		float w;
		float h;
		INT_VPP_MiniMap.INT_BoxFor(size, INT_VPP_Prefs.INT_MINI_PLACE_TL, 0, 0, m_INT_ScrW, m_INT_ScrH, bx, by, w, h);
		float maxX = m_INT_ScrW - 8 - w;
		float maxY = m_INT_ScrH - 8 - h;
		if (x < 8)
			x = 8;
		if (y < 8)
			y = 8;
		if (x > maxX)
			x = maxX;
		if (y > maxY)
			y = maxY;
		m_INT_DragX = x;
		m_INT_DragY = y;
		INT_PaintSchematic(size, INT_VPP_Prefs.INT_MINI_PLACE_OWN, 0, 0, true);
	}

	// TEST: a drag ending with the minimap's top-left corner at design (x, y) of the screen
	void INT_TestDragEnd(float x, float y)
	{
		m_INT_DragX = x;
		m_INT_DragY = y;
		INT_SetOwnAt(x, y);
	}

	// TEST: stepper rows built (2 since the transparency row went)
	int INT_TestRowCount()
	{
		return m_INT_Val.Count();
	}

	// TEST: a stepper row with this title was built
	bool INT_TestHasRow(string title)
	{
		int at = m_INT_RowNames.Find(title);
		if (at >= 0)
			return true;
		return false;
	}

	bool INT_IsGpsNoteShown()
	{
		if (m_INT_Gps)
			return true;
		return false;
	}

	void INT_TestToggle(bool on)
	{
		if (m_INT_On)
			m_INT_On.SetOn(on, true, false);
	}

	// ------------------------------------------------------------------ events
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		int mi = m_INT_Minus.Find(b);
		if (mi >= 0)
		{
			INT_Step(mi, -1);
			return true;
		}
		int pi = m_INT_Plus.Find(b);
		if (pi >= 0)
		{
			INT_Step(pi, 1);
			return true;
		}
		if (b == m_INT_Reset)
		{
			INT_Reset();
			return true;
		}
		// the schematic is handled by the polled press / drag
		if (b == m_INT_SchHit)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		ButtonWidget b = ButtonWidget.Cast(w);
		if (!b)
			return false;
		if (b == m_INT_SchHit || b == m_INT_Reset)
			return true;
		if (m_INT_Minus.Find(b) >= 0 || m_INT_Plus.Find(b) >= 0)
			return true;
		return false;
	}
}
