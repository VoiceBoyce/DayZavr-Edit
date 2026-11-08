// INT PartyMode (client) — the main window (key P), concept C «ЭКСПЕДИЦИЯ» (spec D16, design/concepts_v1/
// 3_c_expedition_*.png): a full-screen overlay without window chrome. The 1920x1080 design canvas is centred on
// screen (4:3 keeps the width, 21:9 the height), the shade covers the whole screen. Header (brand, player, search
// indicator, close), five big tab words, the page of the active tab (INT_PTY_Page*), the footer with key hints and
// «MADE BY DAYZAVR PROJECT», an overlay layer for tooltips, in-window notifications and the confirm band.
// Live data comes from INT_PTY_Client.OnChanged; only what changed is repainted.

class INT_PTY_MToast : Managed
{
	Widget Root;
	ImageWidget Bar;
	float Life;
	float Total;
	float H;
	float Y;
	float Gone;
}

class INT_PTY_Menu extends INT_UIF_Menu
{
	static const int SEC_BG = 0;
	static const int SEC_TOP = 1;
	static const int SEC_TABS = 2;
	static const int SEC_PAGE = 3;
	static const int SEC_FOOT = 4;
	static const int SEC_OVER = 5;
	static const int SEC_COUNT = 6;

	static const int TAB_GROUP = 0;
	static const int TAB_SEARCH = 1;
	static const int TAB_IN = 2;
	static const int TAB_OUT = 3;
	static const int TAB_SETTINGS = 4;
	static const int TAB_COUNT = 5;

	static const float TOAST_LIFE = 4.5;
	static const float TOAST_W = 470;
	static const float TOAST_BOTTOM = 960;
	static const float TIP_DELAY = 0.28;
	static const float TAB_SIZE = 36;

	protected static INT_PTY_Menu s_Open;
	protected static int s_LastTab;
	protected static bool s_Registered;
	static bool s_TestHold;			// UI self-test: in-window notifications stay on screen
	static bool s_TestPinTip;		// UI self-test: a tooltip shown by TestHover stays (the real cursor sends hovers too)

	protected ref INT_PTY_MenuHandler m_PtyHandler;
	protected ref map<Widget, ref INT_PTY_MenuAct> m_Acts;
	protected ref array<ref array<Widget>> m_SecHits;
	protected ref INT_PTY_Page m_Page;
	protected ref array<ref INT_PTY_MToast> m_Toasts;

	protected Widget m_Canvas;
	protected Widget m_Sec[6];
	protected ImageWidget m_Shade;
	protected Widget m_PageFrame;
	protected Widget m_OldPage;
	protected float m_OldPageLeft;
	protected float m_PageDelay;
	protected int m_Tab;
	protected bool m_Built;
	protected bool m_Subscribed;
	protected bool m_Closing2;
	protected float m_Time;
	protected float m_CanvasY;

	// header
	protected ImageWidget m_SearchDot;
	protected ImageWidget m_SearchGlow;
	protected TextWidget m_SearchText;
	protected Widget m_SearchInd;
	protected int m_SearchShown = -1;
	protected INT_UIF_Tween m_DotPulse;
	protected INT_UIF_Tween m_GlowPulse;
	protected float m_SearchRight;

	// tabs
	protected TextWidget m_TabWord[5];
	protected TextWidget m_TabNum[5];
	protected TextWidget m_TabBadge[5];
	protected float m_TabX[5];
	protected float m_TabW[5];
	protected int m_BadgeVal[5];
	protected Widget m_TabUl;
	protected ImageWidget m_TabUlLine;

	// tooltip
	protected Widget m_TipHit;
	protected float m_TipWait;
	protected Widget m_TipW;

	// confirm band
	protected Widget m_ModalW;
	protected string m_ModalAction;
	protected bool m_EnterWas;

	// text field (rename)
	protected EditBoxWidget m_Edit;
	protected bool m_KbLocked;
	protected float m_EscGuard;		// W-7: > 0 while the Esc that left the field is held (+ a short tail)
	protected bool m_EscWas;
	static const float ESC_GUARD_S = 0.3;

	// slider drag
	protected string m_DragKey;

	protected INT_UIF_Tween m_CloseTween;

	void INT_PTY_Menu()
	{
		m_PtyHandler = new INT_PTY_MenuHandler();
		m_PtyHandler.SetMenu(this);
		m_Acts = new map<Widget, ref INT_PTY_MenuAct>();
		m_SecHits = new array<ref array<Widget>>();
		for (int i = 0; i < SEC_COUNT; i++)
		{
			m_SecHits.Insert(new array<Widget>());
		}
		m_Toasts = new array<ref INT_PTY_MToast>();
	}

	void ~INT_PTY_Menu()
	{
		Unsubscribe();
		if (s_Open == this)
			s_Open = null;
		if (GetGame())
			LockKeyboard(false);
	}

	// --- static entry points (INT_PTY_Mission, INT_PTY_Notify) ------------------------------------------------
	static bool IsOpen()
	{
		INT_PTY_MenuWatch.Ensure();
		if (!s_Open)
			return false;
		return !s_Open.IsClosingDown();
	}

	static INT_PTY_Menu GetOpen()
	{
		return s_Open;
	}

	static void Register()
	{
		if (s_Registered)
			return;
		s_Registered = true;
		INT_UIF_MenuRegistry.Register(INT_PTY_Const.MENU_MAIN, INT_PTY_Menu);
	}

	static void Toggle()
	{
		INT_PTY_MenuWatch.Ensure();
		if (s_Open)
		{
			// P typed into the rename field is a letter, not the key.
			if (s_Open.IsTyping())
				return;
			s_Open.RequestClose();
			return;
		}
		if (!GetGame() || !GetGame().GetPlayer())
			return;
		// W-4: a dead player does not open the window (it would close again on the next frame)
		if (!GetGame().GetPlayer().IsAlive())
			return;
		Register();
		INT_UIF_Menu.Open(INT_PTY_Const.MENU_MAIN);
	}

	// A notification inside the open window (kind = INT_UIF_ToastKind.*); on the HUD while it is closed.
	static void ShowToast(int kind, string title, string text)
	{
		INT_PTY_MenuWatch.Ensure();
		if (s_Open && !s_Open.IsClosingDown())
		{
			s_Open.Toast(kind, title, text);
			return;
		}
		INT_PTY_Hud.ShowToast(kind, title, text);
	}

	static int LastTab()
	{
		return s_LastTab;
	}

	// --- INT_UIF_Menu configuration ---------------------------------------------------------------------------
	override bool ShowWindow()
	{
		return false;
	}

	override bool CloseOnEscape()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return true;
	}

	override bool UsesBlur()
	{
		return true;
	}

	// --- build -----------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		if (m_Dim)
			m_Dim.Show(false);
		BuildShade();
		float f = INT_UIF_Scale.Factor();
		float ox = (INT_UIF_Scale.ScreenW() - 1920 * f) * 0.5;
		float oy = (INT_UIF_Scale.ScreenH() - 1080 * f) * 0.5;
		m_Canvas = INT_UIF_Draw.Frame(layoutRoot, 0, 0, 1920, 1080, "INT_PTY_Canvas");
		if (m_Canvas)
			m_Canvas.SetPos(ox, oy);
		m_CanvasY = oy;
		for (int i = 0; i < SEC_COUNT; i++)
		{
			string nm = "INT_PTY_Sec" + i.ToString();
			m_Sec[i] = INT_UIF_Draw.Frame(m_Canvas, 0, 0, 1920, 1080, nm);
		}
		m_Tab = s_LastTab;
		if (m_Tab < 0 || m_Tab >= TAB_COUNT)
			m_Tab = TAB_GROUP;
		BuildBg();
		BuildTop();
		BuildTabs();
		BuildFoot();
		m_PageDelay = 0.3;
		OpenPage(m_Tab, false);
		m_Built = true;
	}

	protected void BuildShade()
	{
		// 1.30.164200 darkens translucent UI over the world much less; same value as the skills window (owner 2026-10-26)
		m_Shade = INT_PTY_Look.Fill(layoutRoot, 0, 0, 10, 10, ARGB(251, 6, 6, 6));
		if (!m_Shade)
			return;
		int scrW = INT_UIF_Scale.ScreenW();
		int scrH = INT_UIF_Scale.ScreenH();
		m_Shade.SetPos(0, 0);
		m_Shade.SetSize(scrW, scrH);
		INT_PTY_MenuFx.FadeIn(m_Shade, 0, 0.22);
	}

	// Soft orange glow on the right (.glow) and a faint light on the left (.glow2).
	protected void BuildBg()
	{
		Widget p = m_Sec[SEC_BG];
		INT_PTY_Look.Tex(p, 980, 60, 1100, 980, "pty_glow_ca", ARGB(14, 255, 122, 47));
		INT_PTY_Look.Tex(p, -300, 420, 900, 900, "pty_glow_ca", ARGB(6, 255, 255, 255));
		INT_PTY_MenuFx.FadeIn(p, 0, 0.5);
	}

	// --- hit areas ---------------------------------------------------------------------------------------------
	ButtonWidget HitOn(int sec, Widget parent, float x, float y, float w, float h, string key)
	{
		ButtonWidget hit = INT_UIF_Draw.Hit(parent, x, y, w, h, m_PtyHandler);
		if (!hit)
			return null;
		INT_PTY_MenuAct a = new INT_PTY_MenuAct();
		a.Key = key;
		a.Section = sec;
		m_Acts.Set(hit, a);
		m_SecHits.Get(sec).Insert(hit);
		return hit;
	}

	// A hit area of the current page.
	ButtonWidget Hit(Widget parent, float x, float y, float w, float h, string key)
	{
		return HitOn(SEC_PAGE, parent, x, y, w, h, key);
	}

	INT_PTY_MenuAct ActOf(Widget w)
	{
		if (!w)
			return null;
		INT_PTY_MenuAct a;
		if (m_Acts.Find(w, a))
			return a;
		return null;
	}

	void SetHoverText(Widget hit, TextWidget t, int normal, int hover)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (!a)
			return;
		a.HoverText = t;
		a.TextNormal = normal;
		a.TextHover = hover;
	}

	void SetHoverImg(Widget hit, ImageWidget img, int normal, int hover)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (!a)
			return;
		a.HoverImg = img;
		a.ImgNormal = normal;
		a.ImgHover = hover;
	}

	// `bg` is shown at `alpha` while the mouse is over the hit (row highlights).
	void SetHoverBg(Widget hit, Widget bg, float alpha)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (!a || !bg)
			return;
		// SetAlpha REPLACES the colour alpha: the hover target is the alpha the widget was built with.
		a.HoverBg = bg;
		a.HoverBgAlpha = bg.GetAlpha() * alpha;
		bg.SetAlpha(0);
		bg.SetName(INT_PTY_MenuFx.NO_FADE);
	}

	void SetHoverBtn(Widget hit, INT_PTY_MButton b)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (a)
			a.Btn = b;
	}

	void SetTip(Widget hit, string title, string text, float w = 340)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (!a)
			return;
		// W-2: the same tooltip again does not rebuild the open card (it re-animates).
		if (a.TipTitle == title && a.TipText == text && a.TipW == w)
			return;
		a.TipTitle = title;
		a.TipText = text;
		a.TipW = w;
		if (hit == m_TipHit && m_TipW)
			ShowTip();
	}

	void SetOff(Widget hit, bool off)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (a)
			a.Off = off;
	}

	void SetSound(Widget hit, bool on)
	{
		INT_PTY_MenuAct a = ActOf(hit);
		if (a)
			a.Sound = on;
	}

	protected void DropHits(int sec)
	{
		array<Widget> hits = m_SecHits.Get(sec);
		foreach (Widget w : hits)
		{
			if (w)
				m_Acts.Remove(w);
			if (w && w == m_TipHit)
				HideTip();
		}
		hits.Clear();
	}

	// Removes the hits of `sec` that live under `root` (a block that is rebuilt).
	void DropHitsUnder(int sec, Widget root)
	{
		array<Widget> hits = m_SecHits.Get(sec);
		for (int i = hits.Count() - 1; i >= 0; i--)
		{
			Widget h = hits.Get(i);
			if (!h || INT_UIF_Widgets.IsDescendant(h, root))
			{
				if (h)
					m_Acts.Remove(h);
				if (h && h == m_TipHit)
					HideTip();
				hits.Remove(i);
			}
		}
	}

	protected void ClearSec(int sec)
	{
		DropHits(sec);
		Widget f = m_Sec[sec];
		if (!f)
			return;
		INT_UIF_Animator.Get().Cancel(f);
		INT_UIF_Widgets.ClearChildren(f);
	}

	Widget GetCanvas()
	{
		return m_Canvas;
	}

	// Entrance delay for the page being built (the window open cascade starts later than a tab switch).
	float PageDelay()
	{
		return m_PageDelay;
	}

	int CurrentTab()
	{
		return m_Tab;
	}

	// --- header ------------------------------------------------------------------------------------------------
	protected string MyNick()
	{
		Man me = GetGame().GetPlayer();
		if (me && me.GetIdentity())
			return me.GetIdentity().GetName();
		return INT_PTY_Client.s_MySteam;
	}

	protected void BuildTop()
	{
		ClearSec(SEC_TOP);
		Widget p = m_Sec[SEC_TOP];
		Widget hd = INT_UIF_Draw.Frame(p, 0, 0, 1920, 110, "INT_PTY_Head");
		// ◇ logo: outline diamond with a small solid one inside (.logo)
		ImageWidget logoA = INT_PTY_MD.DmLine(hd, 92, 70, 16, INT_PTY_MC.O);
		INT_PTY_MD.Dm(hd, 92, 70, 6, INT_PTY_MC.O);
		string brandA = INT_PTY_Text.Caps("DAYZAVR");
		float bw = INT_PTY_MD.Measure(hd, brandA, 22, true);
		INT_PTY_Look.Text(hd, 122, 52, INT_PTY_MD.Room(bw), 36, brandA, 22, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
		// «DAYZAVR | PARTY» (owner 2026-10-23): white word, a thin muted bar, the orange word
		INT_PTY_MD.Line(hd, 122 + bw + 14, 59, 1, 22, ARGB(110, 255, 255, 255));
		string brandB = INT_PTY_Text.Caps("#STR_INT_PTY_W_BRAND");
		float bw2 = INT_PTY_MD.Measure(hd, brandB, 22, true);
		float bx2 = 122 + bw + 30;
		INT_PTY_Look.Text(hd, bx2, 52, INT_PTY_MD.Room(bw2), 36, brandB, 22, INT_PTY_MC.O, INT_UIF_Align.LEFT, true);
		float sepX = bx2 + bw2 + 34;
		INT_PTY_MD.Line(hd, sepX, 57, 1, 26, INT_PTY_MC.L2);
		string crumb = INT_PTY_Text.Caps("#STR_INT_PTY_W_PLAYER");
		float cw = INT_PTY_MD.Measure(hd, crumb, 14, true);
		float crX = sepX + 33;
		INT_PTY_Look.Text(hd, crX, 52, INT_PTY_MD.Room(cw), 36, crumb, 14, INT_PTY_MC.M, INT_UIF_Align.LEFT, true);
		string nick = INT_PTY_Text.Spaced(MyNick());
		INT_PTY_Look.Text(hd, crX + cw + 18, 52, 360, 36, nick, 14, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
		// right group, laid out from the right edge: [search] | [P] ЗАКРЫТЬ [×]
		float right = 1840;
		INT_UIF_Surface xbox = INT_PTY_MD.Box(hd, right - 40, 50, 40, 40, 0, INT_PTY_MC.L2);
		ImageWidget xIcon = INT_PTY_MD.Ico(hd, right - 29, 61, 18, "close", INT_PTY_MC.T);
		string closeLab = INT_PTY_Text.Caps("#STR_INT_PTY_W_CLOSE");
		float clw = INT_PTY_MD.Measure(hd, closeLab, 14, true);
		float clx = right - 40 - 14 - clw;
		TextWidget closeText = INT_PTY_Look.Text(hd, clx, 52, clw + 12, 36, closeLab, 14, INT_PTY_MC.M, INT_UIF_Align.LEFT, true);
		string pKey = INT_PTY_MenuGeo.KeyName("UAIntPartyMenu", "P");
		float pkw = INT_PTY_MD.Measure(hd, pKey, 13, true) + 18;
		if (pkw < 30)
			pkw = 30;
		float pkx = clx - 14 - pkw;
		INT_PTY_MD.Keycap(hd, pkx, 57, pKey);
		ButtonWidget closeHit = HitOn(SEC_TOP, hd, pkx - 6, 46, right - pkx + 6, 48, "close");
		SetHoverText(closeHit, closeText, INT_PTY_MC.M, INT_PTY_MC.T);
		SetHoverImg(closeHit, xIcon, INT_PTY_MC.T, INT_PTY_MC.O);
		SetTip(closeHit, "#STR_INT_PTY_W_TIP_CLOSE_H", "#STR_INT_PTY_W_TIP_CLOSE", 300);
		float sep2 = pkx - 32;
		INT_PTY_MD.Line(hd, sep2, 57, 1, 26, INT_PTY_MC.L2);
		m_SearchRight = sep2 - 32;
		m_SearchInd = INT_UIF_Draw.Frame(hd, 0, 0, 1920, 110, "INT_PTY_SearchInd");
		m_SearchShown = -1;
		UpdateSearchInd(false);
		// header hairline sweeps in with a diamond at its tip (anim 0.3–0.95 s of the concept, compressed)
		ImageWidget line = INT_PTY_MD.Line(p, 80, 99, 1760, 1, INT_PTY_MC.L);
		INT_PTY_MenuFx.GrowW(line, 0.06, 0.42);
		ImageWidget tip = INT_PTY_MD.Dm(p, 80, 99, 10, INT_PTY_MC.O);
		if (tip)
		{
			float side = 10 * INT_PTY_Look.SQRT2;
			INT_UIF_Animator.MoveX(tip, INT_UIF_Draw.S(1840 - side * 0.5), 0.42, INT_UIF_Ease.OUT_EXPO).Delay(0.06);
			INT_UIF_Animator.Fade(tip, 0, 0.3, INT_UIF_Ease.IN_QUAD).Delay(0.5).HideOnEnd();
		}
		INT_PTY_MenuFx.BlockIn(hd, 0, -10, 0.05, 0.3);
		if (logoA)
		{
			logoA.SetRotation(0, 0, -90);
			INT_UIF_Animator.Rotate(logoA, 0, 0.4, INT_UIF_Ease.OUT_BACK).Delay(0.05);
		}
	}

	// ● В ПОИСКЕ ГРУППЫ (green, blinking) / ● ПОИСК ВЫКЛЮЧЕН (grey, still) — always visible.
	protected void UpdateSearchInd(bool animate)
	{
		int on = 0;
		if (INT_PTY_Client.s_Searching)
			on = 1;
		if (on == m_SearchShown || !m_SearchInd)
			return;
		m_SearchShown = on;
		DropHitsUnder(SEC_TOP, m_SearchInd);
		INT_UIF_Widgets.ClearChildren(m_SearchInd);
		Widget p = m_SearchInd;
		string lab = "#STR_INT_PTY_W_SEARCH_OFF";
		int col = INT_PTY_MC.M;
		if (on == 1)
		{
			lab = "#STR_INT_PTY_W_SEARCH_ON";
			col = INT_PTY_MC.G;
		}
		string caps = INT_PTY_Text.Caps(lab);
		float tw = INT_PTY_MD.Measure(p, caps, 14, true);
		float tx = m_SearchRight - tw;
		m_SearchText = INT_PTY_Look.Text(p, tx - 40, 52, tw + 44, 36, caps, 14, col, INT_UIF_Align.RIGHT, true);
		float dx = tx - 14 - 5;
		m_SearchGlow = null;
		if (on == 1)
		{
			m_SearchGlow = INT_PTY_MD.Glow(p, dx, 70, 32, ARGB(140, 86, 224, 126));
			if (m_SearchGlow)
				m_SearchGlow.SetName(INT_PTY_MenuFx.NO_FADE);
			m_SearchDot = INT_PTY_MD.Dot(p, dx, 70, 10, INT_PTY_MC.G);
		}
		else
		{
			m_SearchDot = INT_PTY_MD.Dot(p, dx, 70, 10, INT_PTY_MC.OFF);
		}
		if (m_SearchDot)
			m_SearchDot.SetName(INT_PTY_MenuFx.NO_FADE);
		if (on == 1)
		{
			m_DotPulse = INT_UIF_Animator.Pulse(m_SearchDot, 0.15, 1, 1.0);
			m_GlowPulse = INT_UIF_Animator.Pulse(m_SearchGlow, 0.05, 0.55, 1.0);
		}
		ButtonWidget hit = HitOn(SEC_TOP, p, dx - 20, 46, m_SearchRight - dx + 24, 48, "goto:1");
		SetTip(hit, "#STR_INT_PTY_W_TIP_SIND_H", "#STR_INT_PTY_W_TIP_SIND", 360);
		SetSound(hit, false);
		if (animate)
		{
			INT_PTY_MenuFx.BlockIn(p, 12, 0, 0, 0.25);
			INT_PTY_MenuSnd.Toggle();
		}
	}

	// --- tabs ----------------------------------------------------------------------------------------------------
	protected string TabLabel(int i)
	{
		if (i == TAB_SEARCH)
			return "#STR_INT_PTY_W_TAB_SEARCH";
		if (i == TAB_IN)
			return "#STR_INT_PTY_W_TAB_IN";
		if (i == TAB_OUT)
			return "#STR_INT_PTY_W_TAB_OUT";
		if (i == TAB_SETTINGS)
			return "#STR_INT_PTY_W_TAB_SET";
		return "#STR_INT_PTY_W_TAB_GROUP";
	}

	protected bool TabHasBadge(int i)
	{
		return i == TAB_SEARCH || i == TAB_IN || i == TAB_OUT;
	}

	protected int BadgeCount(int i)
	{
		if (i == TAB_SEARCH && INT_PTY_Client.s_SearchList)
			return INT_PTY_Client.s_SearchList.Count();
		if (i == TAB_IN && INT_PTY_Client.s_Incoming)
			return INT_PTY_Client.s_Incoming.Count();
		if (i == TAB_OUT && INT_PTY_Client.s_Outgoing)
		{
			int n = 0;
			foreach (INT_PTY_InviteRow r : INT_PTY_Client.s_Outgoing)
			{
				if (r && r.State == INT_PTY_Const.OUT_SENT)
					n++;
			}
			return n;
		}
		return 0;
	}

	protected void BuildTabs()
	{
		ClearSec(SEC_TABS);
		Widget p = m_Sec[SEC_TABS];
		array<float> words = new array<float>();
		float total = 0;
		float badgeRoom = INT_PTY_MD.Measure(p, "00", 15, true) + 8;
		for (int i = 0; i < TAB_COUNT; i++)
		{
			words.Insert(INT_PTY_MD.Measure(p, TabLabel(i), TAB_SIZE));
			float wi = words.Get(i);
			if (TabHasBadge(i))
				wi = wi + badgeRoom;
			m_TabW[i] = wi;
			total = total + wi;
		}
		float gap = 58;
		if (total + gap * 4 > 1760)
			gap = Math.Max(24, (1760 - total) / 4);
		float x = 80;
		for (int j = 0; j < TAB_COUNT; j++)
		{
			m_TabX[j] = x;
			Widget tab = INT_UIF_Draw.Frame(p, x, 112, m_TabW[j] + 20, 102, "INT_PTY_Tab");
			string num = INT_PTY_Text.Spaced(INT_PTY_Text.Pad2(j + 1));
			int numCol = INT_PTY_MC.D;
			int wordCol = INT_PTY_MC.M;
			if (j == m_Tab)
			{
				numCol = INT_PTY_MC.O;
				wordCol = INT_PTY_MC.T;
			}
			m_TabNum[j] = INT_PTY_Look.Text(tab, 0, 12, 80, 18, num, 13, numCol, INT_UIF_Align.LEFT, true);
			float wordRoom = INT_PTY_MD.Room(words.Get(j));
			string wordKey = TabLabel(j);
			m_TabWord[j] = INT_PTY_MD.Txt(tab, -2, 29, wordRoom, 48, wordKey, TAB_SIZE, wordCol);
			m_TabBadge[j] = null;
			m_BadgeVal[j] = -1;
			if (TabHasBadge(j))
				m_TabBadge[j] = INT_PTY_Look.Text(tab, words.Get(j) + 12, 32, 60, 20, "", 15, INT_PTY_MC.O, INT_UIF_Align.LEFT, true);
			ButtonWidget hit = HitOn(SEC_TABS, p, x - 10, 112, m_TabW[j] + 20, 102, "tab:" + j.ToString());
			if (j != m_Tab)
				SetHoverText(hit, m_TabWord[j], INT_PTY_MC.M, INT_PTY_MC.LABEL);
			// argument temporaries can be clobbered in Enforce: call results go into locals first
			string tipHead = TabLabel(j);
			string tipKey = "#STR_INT_PTY_W_TIP_TAB_" + j.ToString();
			SetTip(hit, tipHead, tipKey, 340);
			INT_PTY_MenuFx.BlockIn(tab, 0, 18, 0.12 + j * 0.045, 0.3);
			x = x + m_TabW[j] + gap;
		}
		INT_PTY_MD.Line(p, 80, 213, 1760, 1, INT_PTY_MC.L);
		// moving underline with a diamond at its left end (.cx-tab .ul)
		m_TabUl = INT_UIF_Draw.Frame(p, m_TabX[m_Tab], 207, m_TabW[m_Tab], 12, "INT_PTY_TabUl");
		m_TabUlLine = INT_PTY_MD.Line(m_TabUl, 0, 5, m_TabW[m_Tab], 2, INT_PTY_MC.O);
		INT_PTY_MD.Dm(m_TabUl, 0, 6, 10, INT_PTY_MC.O);
		INT_PTY_MenuFx.GrowW(m_TabUlLine, 0.36, 0.3);
		INT_PTY_MenuFx.FadeIn(m_TabUl, 0.34, 0.2);
		UpdateBadges(false);
	}

	protected void UpdateBadges(bool animate)
	{
		for (int i = 0; i < TAB_COUNT; i++)
		{
			TextWidget b = m_TabBadge[i];
			if (!b)
				continue;
			int n = BadgeCount(i);
			if (n == m_BadgeVal[i])
				continue;
			bool up = n > m_BadgeVal[i] && m_BadgeVal[i] >= 0;
			m_BadgeVal[i] = n;
			if (n <= 0)
			{
				b.SetText("");
				continue;
			}
			b.SetText(INT_PTY_Text.Pad2(n));
			if (animate && up)
				INT_PTY_MenuFx.Bump(b, -8);
		}
	}

	void SwitchTab(int tab)
	{
		if (tab < 0 || tab >= TAB_COUNT || tab == m_Tab)
			return;
		HideTip();
		CommitEdit();
		INT_PTY_Sfx.Tab();
		int old = m_Tab;
		m_Tab = tab;
		s_LastTab = tab;
		// words: active white + orange number; underline slides (anim 5.2–5.65 s of the concept)
		PaintTabState(old, false);
		PaintTabState(tab, true);
		if (m_TabUl)
		{
			INT_UIF_Animator.MoveX(m_TabUl, INT_UIF_Draw.S(m_TabX[tab]), 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
			INT_UIF_Animator.ResizeW(m_TabUl, INT_UIF_Draw.S(m_TabW[tab]), 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
		}
		if (m_TabUlLine)
			INT_UIF_Animator.ResizeW(m_TabUlLine, INT_UIF_Draw.S(m_TabW[tab]), 0.3, INT_UIF_Ease.IN_OUT_CUBIC);
		RebindTabHovers();
		m_PageDelay = 0.1;
		OpenPage(tab, true);
	}

	protected void PaintTabState(int i, bool on)
	{
		int wc = INT_PTY_MC.M;
		int nc = INT_PTY_MC.D;
		if (on)
		{
			wc = INT_PTY_MC.T;
			nc = INT_PTY_MC.O;
		}
		if (m_TabWord[i])
			INT_UIF_Animator.ColorTo(m_TabWord[i], wc, 0.25, INT_UIF_Ease.OUT_CUBIC);
		if (m_TabNum[i])
			INT_UIF_Animator.ColorTo(m_TabNum[i], nc, 0.25, INT_UIF_Ease.OUT_CUBIC);
	}

	protected void RebindTabHovers()
	{
		array<Widget> hits = m_SecHits.Get(SEC_TABS);
		foreach (Widget h : hits)
		{
			INT_PTY_MenuAct a = ActOf(h);
			if (!a || a.Key.IndexOf("tab:") != 0)
				continue;
			string ts = a.Key.Substring(4, a.Key.Length() - 4);
			int t = ts.ToInt();
			if (t == m_Tab)
				a.HoverText = null;
			else
				SetHoverText(h, m_TabWord[t], INT_PTY_MC.M, INT_PTY_MC.LABEL);
		}
	}

	// --- pages -----------------------------------------------------------------------------------------------------
	protected INT_PTY_Page CreatePage(int tab)
	{
		if (tab == TAB_SEARCH)
			return new INT_PTY_PageSearch();
		if (tab == TAB_IN)
			return new INT_PTY_PageInvites(false);
		if (tab == TAB_OUT)
			return new INT_PTY_PageInvites(true);
		if (tab == TAB_SETTINGS)
			return new INT_PTY_PageSettings();
		return new INT_PTY_PageGroup();
	}

	protected void OpenPage(int tab, bool switching)
	{
		if (m_Page)
			m_Page.OnLeave();
		if (m_OldPage)
		{
			m_OldPage.Unlink();
			m_OldPage = null;
		}
		if (m_PageFrame)
		{
			DropHits(SEC_PAGE);
			if (switching)
			{
				m_OldPage = m_PageFrame;
				m_OldPageLeft = 0.3;
				INT_PTY_MenuFx.FadeOut(m_OldPage, 0, 0.18);
				INT_UIF_Animator.MoveX(m_OldPage, INT_UIF_Draw.S(-40), 0.22, INT_UIF_Ease.IN_QUAD);
			}
			else
			{
				m_PageFrame.Unlink();
			}
			m_PageFrame = null;
		}
		m_Edit = null;
		LockKeyboard(false);
		m_DragKey = "";
		INT_PTY_MenuFx.ForgetHomes();
		m_PageFrame = INT_UIF_Draw.Frame(m_Sec[SEC_PAGE], 0, 0, 1920, 1080, "INT_PTY_Page");
		m_Page = CreatePage(tab);
		m_Page.Attach(this, m_PageFrame);
		m_Page.Build(true);
		if (switching && m_PageFrame)
		{
			m_PageFrame.SetPos(INT_UIF_Draw.S(40), 0);
			INT_UIF_Animator.MoveX(m_PageFrame, 0, 0.34, INT_UIF_Ease.OUT_CUBIC).Delay(0.08);
		}
	}

	// Rebuilds the current page in place (no slide).
	void RebuildPage(bool animate)
	{
		if (!m_PageFrame || !m_Page)
			return;
		DropHits(SEC_PAGE);
		m_Edit = null;
		LockKeyboard(false);
		m_DragKey = "";
		INT_UIF_Animator.Get().Cancel(m_PageFrame);
		INT_UIF_Widgets.ClearChildren(m_PageFrame);
		INT_PTY_MenuFx.ForgetHomes();
		m_PageDelay = 0.02;
		m_Page.Build(animate);
	}

	INT_PTY_Page GetPage()
	{
		return m_Page;
	}

	// --- footer -------------------------------------------------------------------------------------------------
	protected void BuildFoot()
	{
		ClearSec(SEC_FOOT);
		Widget p = m_Sec[SEC_FOOT];
		INT_PTY_MD.Line(p, 80, 1000, 1760, 1, INT_PTY_MC.L);
		Widget ft = INT_UIF_Draw.Frame(p, 0, 1000, 1920, 80, "INT_PTY_Foot");
		float x = 80;
		string kp = INT_PTY_MenuGeo.KeyName("UAIntPartyMenu", "P");
		string kt = INT_PTY_MenuGeo.KeyName("UAIntPartyPing", "T");
		string ky = INT_PTY_MenuGeo.KeyName("UAIntPartyClear", "Y");
		string hold = INT_PTY_Text.Upper(INT_PTY_Text.Tr("#STR_INT_PTY_W_K_HOLD"));
		x = KeyHint(ft, x, kp, "#STR_INT_PTY_W_K_CLOSE");
		x = KeyHint(ft, x, kt, "#STR_INT_PTY_W_K_PING");
		x = KeyHint(ft, x, ky, "#STR_INT_PTY_W_K_CLEAR");
		x = KeyHint(ft, x, ky + "  " + hold, "#STR_INT_PTY_W_K_CLEAR_ALL");
		x = KeyHint(ft, x, "ESC", "#STR_INT_PTY_W_K_BACK");
		// ◆ MADE BY DAYZAVR PROJECT (owner requirement, bottom right)
		string proj = INT_PTY_Text.Caps("DAYZAVR PROJECT");
		string made = INT_PTY_Text.Caps("MADE BY");
		float wp = INT_PTY_MD.Measure(ft, proj, 13, true);
		float wm = INT_PTY_MD.Measure(ft, made, 13, true);
		float right = 1840;
		// right-aligned boxes with spare room on the left: the text end is exact, the start never cut
		float roomP = INT_PTY_MD.Room(wp);
		float roomM = INT_PTY_MD.Room(wm);
		TextWidget sig = INT_PTY_Look.Text(ft, right - roomP, 16, roomP, 26, proj, 13, INT_PTY_MC.T, INT_UIF_Align.RIGHT, true);
		if (sig)
			sig.SetName("INT_PTY_MadeBy");
		float madeRight = right - wp - 16;
		INT_PTY_Look.Text(ft, madeRight - roomM, 16, roomM, 26, made, 13, INT_PTY_MC.M, INT_UIF_Align.RIGHT, true);
		INT_PTY_MD.Dm(ft, madeRight - wm - 16, 29, 9, INT_PTY_MC.O);
		INT_PTY_MenuFx.BlockIn(ft, 0, 10, 0.32, 0.3);
	}

	protected float KeyHint(Widget p, float x, string key, string label)
	{
		float kw = INT_PTY_MD.Keycap(p, x, 16, key);
		string lab = INT_PTY_Text.Caps(label);
		float lw = INT_PTY_MD.Measure(p, lab, 13, true);
		INT_PTY_Look.Text(p, x + kw + 10, 16, INT_PTY_MD.Room(lw), 26, lab, 13, INT_PTY_MC.M, INT_UIF_Align.LEFT, true);
		return x + kw + 10 + lw + 30;
	}

	// --- interaction ---------------------------------------------------------------------------------------------
	bool OnActHover(Widget w, bool enter)
	{
		INT_PTY_MenuAct a = ActOf(w);
		if (!a)
			return false;
		if (s_TestPinTip && w != m_TipHit)
			return true;
		if (!a.Off)
		{
			if (a.HoverText)
			{
				int tc = a.TextNormal;
				if (enter)
					tc = a.TextHover;
				INT_UIF_Animator.ColorTo(a.HoverText, tc, 0.1, INT_UIF_Ease.OUT_CUBIC);
				Widget twin = INT_PTY_Look.Twin(a.HoverText);
				if (twin)
					INT_UIF_Animator.ColorTo(twin, tc, 0.1, INT_UIF_Ease.OUT_CUBIC);
			}
			if (a.HoverImg)
			{
				int ic = a.ImgNormal;
				if (enter)
					ic = a.ImgHover;
				INT_UIF_Animator.ColorTo(a.HoverImg, ic, 0.1, INT_UIF_Ease.OUT_CUBIC);
			}
			if (a.HoverBg)
			{
				float ha = 0;
				if (enter)
					ha = a.HoverBgAlpha;
				INT_UIF_Animator.Fade(a.HoverBg, ha, 0.14, INT_UIF_Ease.OUT_CUBIC);
			}
			if (a.Btn)
				a.Btn.SetHover(enter);
			if (enter && a.Sound)
				INT_PTY_MenuSnd.Hover();
		}
		if (enter)
		{
			m_TipHit = w;
			m_TipWait = TIP_DELAY;
			if (m_TipW)
				HideTipWidget();
		}
		else if (w == m_TipHit)
		{
			HideTip();
		}
		return true;
	}

	bool OnActWheel(Widget w, int wheel)
	{
		INT_PTY_MenuAct a = ActOf(w);
		if (!a)
			return false;
		HideTip();
		if (m_ModalW)
			return true;
		if (m_Page && a.Section == SEC_PAGE)
			return m_Page.OnWheel(a.Key, wheel);
		return false;
	}

	bool OnActDown(Widget w)
	{
		INT_PTY_MenuAct a = ActOf(w);
		if (!a || m_ModalW || a.Off)
			return false;
		if (!m_Page || a.Section != SEC_PAGE)
			return false;
		if (!m_Page.OnDown(a.Key))
			return false;
		m_DragKey = a.Key;
		HideTip();
		int mx;
		int my;
		GetMousePos(mx, my);
		m_Page.OnSlide(m_DragKey, mx, false);
		return true;
	}

	bool OnActClick(Widget w)
	{
		INT_PTY_MenuAct a = ActOf(w);
		if (!a)
			return false;
		HideTip();
		string key = a.Key;
		if (m_ModalW && a.Section != SEC_OVER)
			return true;
		if (key != "rename")
			BlurEdit();
		if (a.Off)
			return true;
		if (key == "close")
		{
			RequestClose();
			return true;
		}
		if (key.IndexOf("tab:") == 0)
		{
			string ts = key.Substring(4, key.Length() - 4);
			SwitchTab(ts.ToInt());
			return true;
		}
		if (key.IndexOf("goto:") == 0)
		{
			string gs = key.Substring(5, key.Length() - 5);
			SwitchTab(gs.ToInt());
			return true;
		}
		if (key == "modal:yes")
		{
			ModalAnswer(true);
			return true;
		}
		if (key == "modal:no")
		{
			ModalAnswer(false);
			return true;
		}
		if (key == "modal:none")
			return true;
		if (a.Btn)
			a.Btn.Press();
		if (m_Page && a.Section == SEC_PAGE)
			return m_Page.OnClick(key, w);
		return false;
	}

	// --- text field (rename) ---------------------------------------------------------------------------------------
	// While the field has focus the keyboard belongs to the UI (vanilla chat pattern: ChangeGameFocus for the
	// keyboard), so typed letters do not move the character. Released on blur, on rebuild and on hide.
	void RegisterEdit(EditBoxWidget eb)
	{
		m_Edit = eb;
		if (eb)
			eb.SetHandler(m_PtyHandler);
	}

	void FocusEdit()
	{
		if (m_Edit)
			SetFocus(m_Edit);
	}

	bool IsTyping()
	{
		return m_KbLocked;
	}

	bool OnEditChange(Widget w, bool finished)
	{
		if (!m_Edit || w != m_Edit)
			return false;
		if (m_Page)
			m_Page.OnEditLive(m_Edit.GetText());
		// Enter arrives as finished = true (memory: EditBox Enter); focus moving away is "finished" without the key.
		if (finished && EnterDown())
			BlurEdit();
		return true;
	}

	bool OnEditKey(Widget w, int key)
	{
		if (!m_Edit || w != m_Edit)
			return false;
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			BlurEdit();
			return true;
		}
		// W-7: Esc leaves the field only; the same press must not close the window (guard read in Update).
		if (key == KeyCode.KC_ESCAPE)
		{
			EscLeavesEdit();
			return true;
		}
		return false;
	}

	protected void EscLeavesEdit()
	{
		m_EscGuard = ESC_GUARD_S;
		BlurEdit();
	}

	void OnEditFocus(Widget w, bool on)
	{
		if (!m_Edit || w != m_Edit)
			return;
		LockKeyboard(on);
		if (!on)
			CommitEdit();
	}

	protected bool EnterDown()
	{
		return KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0;
	}

	// Leaves the field: the page stores what was typed.
	void BlurEdit()
	{
		if (!m_KbLocked)
			return;
		SetFocus(null);
		LockKeyboard(false);
		CommitEdit();
	}

	protected void CommitEdit()
	{
		if (m_Edit && m_Page)
			m_Page.OnEditDone(m_Edit.GetText());
	}

	protected void LockKeyboard(bool on)
	{
		if (on == m_KbLocked)
			return;
		m_KbLocked = on;
		if (!GetGame() || !GetGame().GetInput())
			return;
		int delta = -1;
		if (on)
			delta = 1;
		GetGame().GetInput().ChangeGameFocus(delta, INPUT_DEVICE_KEYBOARD);
	}

	// --- tooltip (.tip) --------------------------------------------------------------------------------------------------
	void HideTip()
	{
		m_TipHit = null;
		m_TipWait = 0;
		HideTipWidget();
	}

	protected void HideTipWidget()
	{
		if (!m_TipW)
			return;
		INT_UIF_Animator.Get().Cancel(m_TipW);
		m_TipW.Unlink();
		m_TipW = null;
	}

	// Design-px rect of a widget on the canvas.
	void DesignRect(Widget w, out float x, out float y, out float ww, out float hh)
	{
		float sx;
		float sy;
		float sw;
		float sh;
		w.GetScreenPos(sx, sy);
		w.GetScreenSize(sw, sh);
		float cx = 0;
		float cy = 0;
		if (m_Canvas)
			m_Canvas.GetScreenPos(cx, cy);
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		x = (sx - cx) / f;
		y = (sy - cy) / f;
		ww = sw / f;
		hh = sh / f;
	}

	protected void ShowTip()
	{
		HideTipWidget();
		if (!m_TipHit || m_ModalW)
			return;
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (st && st.Tooltips != 1)
			return;
		INT_PTY_MenuAct a = ActOf(m_TipHit);
		if (!a || (a.TipTitle == "" && a.TipText == ""))
			return;
		float hx;
		float hy;
		float hw;
		float hh;
		DesignRect(m_TipHit, hx, hy, hw, hh);
		float w = a.TipW;
		string title = INT_PTY_Text.Caps(a.TipTitle);
		float titleW = INT_PTY_MD.Measure(m_Sec[SEC_OVER], title, 13, true) + 60;
		if (titleW > w)
			w = Math.Min(titleW, 560);
		array<string> lines = new array<string>();
		INT_PTY_MD.Wrap(a.TipText, 15, w - 34, lines, 6);
		float h = 14 + 18 + 8 + lines.Count() * 21 + 14;
		if (a.TipText == "")
			h = 14 + 18 + 14;
		float cxh = hx + hw * 0.5;
		float x = cxh - w * 0.5;
		if (x < 24)
			x = 24;
		if (x + w > 1896)
			x = 1896 - w;
		bool above = true;
		float y = hy - h - 14;
		if (y < 16)
		{
			above = false;
			y = hy + hh + 14;
		}
		m_TipW = INT_UIF_Draw.Frame(m_Sec[SEC_OVER], x, y, w, h, "INT_PTY_Tip");
		// notch first: the card covers its inner half
		float nx = Math.Clamp(cxh - x, 18, w - 18);
		float ny = h;
		if (!above)
			ny = 0;
		INT_PTY_MD.DmLine(m_TipW, nx, ny, 12, 0x26FFFFFF);
		INT_PTY_MD.Dm(m_TipW, nx, ny, 10, INT_PTY_MC.CARD);
		INT_PTY_MD.Box(m_TipW, 0, 0, w, h, INT_PTY_MC.CARD, 0x26FFFFFF);
		INT_PTY_MD.Line(m_TipW, 1, 0, w - 2, 2, INT_PTY_MC.O);
		INT_PTY_MD.Ico(m_TipW, 16, 15, 15, "info", INT_PTY_MC.O);
		INT_PTY_Look.Text(m_TipW, 40, 12, w - 24, 20, title, 13, INT_PTY_MC.O, INT_UIF_Align.LEFT, true);
		float ly = 40;
		foreach (string l : lines)
		{
			INT_PTY_Look.Text(m_TipW, 16, ly, w - 20, 21, l, 15, INT_PTY_MC.T);
			ly = ly + 21;
		}
		INT_PTY_MenuFx.BlockIn(m_TipW, 0, 4, 0, 0.14);
	}

	// --- in-window notifications (.toast) --------------------------------------------------------------------------
	void Toast(int kind, string title, string text)
	{
		if (!m_Built)
			return;
		int color = INT_PTY_MC.O;
		string icon = "bell";
		if (kind == INT_UIF_ToastKind.SUCCESS)
			icon = "check";
		else if (kind == INT_UIF_ToastKind.WARNING)
		{
			color = INT_PTY_MC.W;
			icon = "warning";
		}
		else if (kind == INT_UIF_ToastKind.ERROR)
		{
			color = INT_PTY_MC.R;
			icon = "skull";
		}
		array<string> lines = new array<string>();
		// up to 4 lines (the limit / search-off texts with a long nick need 3 in RU and 4 in DE; 2 cut them mid-word)
		INT_PTY_MD.Wrap(text, 17, TOAST_W - 96, lines, 4);
		float h = 74;
		if (lines.Count() > 1)
			h = 74 + 22 * (lines.Count() - 1);
		INT_PTY_MToast t = new INT_PTY_MToast();
		t.H = h;
		t.Life = TOAST_LIFE;
		t.Total = TOAST_LIFE;
		t.Root = INT_UIF_Draw.Frame(m_Sec[SEC_OVER], 1840 - TOAST_W, TOAST_BOTTOM - h, TOAST_W, h, "INT_PTY_Toast");
		Widget r = t.Root;
		INT_PTY_MD.Box(r, 0, 0, TOAST_W, h, INT_PTY_MC.TOAST, 0x1AFFFFFF);
		INT_PTY_MD.DmLine(r, 36, h * 0.5, 24, color);
		INT_PTY_MD.Ico(r, 29, h * 0.5 - 7, 14, icon, color);
		string capsTitle = INT_PTY_Text.Caps(title);
		INT_PTY_Look.Text(r, 70, 12, TOAST_W - 84, 20, capsTitle, 13, color, INT_UIF_Align.LEFT, true);
		float ly = 34;
		foreach (string l : lines)
		{
			INT_PTY_Look.Text(r, 70, ly, TOAST_W - 84, 24, l, 17, INT_PTY_MC.T);
			ly = ly + 22;
		}
		t.Bar = INT_PTY_MD.Line(r, 0, h - 2, TOAST_W, 2, color);
		if (t.Bar)
			t.Bar.SetName(INT_PTY_MenuFx.NO_FADE);
		m_Toasts.Insert(t);
		while (m_Toasts.Count() > 3)
		{
			INT_PTY_MToast old = m_Toasts.Get(0);
			if (old.Root)
				old.Root.Unlink();
			m_Toasts.RemoveOrdered(0);
		}
		Restack(true);
		INT_PTY_MenuFx.BlockIn(r, 70, 0, 0, 0.38);
	}

	// Newest at the bottom, older ones above it.
	protected void Restack(bool animate)
	{
		float bottom = TOAST_BOTTOM;
		for (int i = m_Toasts.Count() - 1; i >= 0; i--)
		{
			INT_PTY_MToast t = m_Toasts.Get(i);
			float y = bottom - t.H;
			if (t.Root && t.Y != y)
			{
				if (animate && t.Y > 0)
					INT_UIF_Animator.MoveY(t.Root, INT_UIF_Draw.S(y), 0.22, INT_UIF_Ease.OUT_CUBIC);
				else
					INT_UIF_Draw.Move(t.Root, 1840 - TOAST_W, y);
			}
			t.Y = y;
			bottom = y - 12;
		}
	}

	protected void TickToasts(float dt)
	{
		if (s_TestHold)
			return;
		bool changed = false;
		for (int i = m_Toasts.Count() - 1; i >= 0; i--)
		{
			INT_PTY_MToast t = m_Toasts.Get(i);
			if (t.Gone > 0)
			{
				t.Gone = t.Gone - dt;
				if (t.Gone <= 0)
				{
					if (t.Root)
						t.Root.Unlink();
					m_Toasts.RemoveOrdered(i);
					changed = true;
				}
				continue;
			}
			t.Life = t.Life - dt;
			if (t.Bar)
				INT_UIF_Draw.Resize(t.Bar, TOAST_W * Math.Clamp(t.Life / t.Total, 0, 1), 2);
			if (t.Life <= 0)
			{
				t.Gone = 0.24;
				INT_PTY_MenuFx.FadeOut(t.Root, 0, 0.2);
				if (t.Root)
					INT_UIF_Animator.MoveX(t.Root, INT_UIF_Draw.S(1840 - TOAST_W + 40), 0.22, INT_UIF_Ease.IN_QUAD);
			}
		}
		if (changed)
			Restack(true);
	}

	int ToastCount()
	{
		return m_Toasts.Count();
	}

	// --- confirm band (3_c_expedition_confirm.png) ---------------------------------------------------------------
	void AskRemove(string id)
	{
		INT_PTY_Friend f = INT_PTY_Client.Friend(id);
		if (!f)
			return;
		CloseModal();
		HideTip();
		BlurEdit();
		m_ModalAction = "remove:" + id;
		m_EnterWas = EnterDown();
		string name = INT_PTY_Client.NameOf(id);
		int idx = INT_PTY_Client.s_Friends.Find(f) + 1;
		m_ModalW = INT_UIF_Draw.Frame(m_Sec[SEC_OVER], 0, 0, 1920, 1080, "INT_PTY_Modal");
		Widget mw = m_ModalW;
		// the dim covers the whole screen, not only the canvas
		ImageWidget dim = INT_PTY_Look.Fill(mw, 0, 0, 10, 10, ARGB(184, 0, 0, 0));
		if (dim)
		{
			float cx0;
			float cy0;
			m_Canvas.GetScreenPos(cx0, cy0);
			int dimW = INT_UIF_Scale.ScreenW();
			int dimH = INT_UIF_Scale.ScreenH();
			dim.SetPos(-cx0, -cy0);
			dim.SetSize(dimW, dimH);
		}
		HitOn(SEC_OVER, mw, 0, 0, 1920, 1080, "modal:none");
		Widget band = INT_UIF_Draw.Frame(mw, 0, 352, 1920, 376, "INT_PTY_Band");
		ImageWidget bandBg = INT_PTY_Look.Fill(band, 0, 0, 10, 376, INT_PTY_MC.BAND);
		if (bandBg)
		{
			float bx0;
			float by0;
			m_Canvas.GetScreenPos(bx0, by0);
			int bandW = INT_UIF_Scale.ScreenW();
			float bandH = INT_UIF_Draw.S(376);
			bandBg.SetPos(-bx0, 0);
			bandBg.SetSize(bandW, bandH);
		}
		INT_PTY_Look.Tex(band, 1100, -200, 1000, 776, "pty_glow_ca", ARGB(9, 255, 75, 62));
		ImageWidget topLine = INT_PTY_MD.Line(band, 0, 0, 1920, 1, ARGB(140, 255, 75, 62));
		INT_PTY_MD.Line(band, 0, 375, 1920, 1, INT_PTY_MC.L);
		INT_PTY_MD.Line(band, 1120, 0, 1, 376, INT_PTY_MC.L);
		// left: ⚠ ПОДТВЕРЖДЕНИЕ / title / text
		INT_PTY_MD.Ico(band, 80, 72, 18, "warning", INT_PTY_MC.R_TEXT);
		INT_PTY_MD.Caps(band, 110, 70, 600, 22, "#STR_INT_PTY_W_C_TAG", 13, INT_PTY_MC.R_TEXT);
		string title = INT_PTY_Text.Tr("#STR_INT_PTY_W_C_TITLE");
		float ts = INT_PTY_Look.FitSize(title, 80, false, 1000, 40);
		INT_PTY_MD.Txt(band, 76, 112, 1030, 92, title, ts, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		string body = INT_PTY_Text.Fmt("#STR_INT_PTY_W_C_TEXT", name, f.Nick);
		INT_PTY_MD.Para(band, 80, 216, 900, body, 20, INT_PTY_MC.SOFT, 30, 3);
		// right: ДРУГ 01 / ◆ name + nick / buttons
		INT_PTY_MD.Caps(band, 1168, 70, 600, 22, INT_PTY_Text.Fmt("#STR_INT_PTY_W_C_FRIEND", INT_PTY_Text.Pad2(idx)), 13, INT_PTY_MC.M);
		INT_PTY_MD.Dm(band, 1176, 148, 16, INT_PTY_MenuGeo.FriendColor(id));
		float ns = INT_PTY_Look.FitSize(name, 52, false, 600, 28);
		INT_PTY_MD.Txt(band, 1204, 100, 640, 64, name, ns, INT_PTY_MC.T, INT_UIF_Align.LEFT, false, true);
		INT_PTY_MD.Txt(band, 1206, 164, 600, 24, f.Nick, 17, INT_PTY_MC.M);
		INT_PTY_MButton yes = INT_PTY_MButton.Create(band, 1168, 231, 56, "trash", "#STR_INT_PTY_W_C_YES", INT_PTY_MButton.DGF, false, 15, "ENTER", 260);
		ButtonWidget yh = HitOn(SEC_OVER, yes.m_Root, 0, 0, yes.m_W, yes.m_H, "modal:yes");
		SetHoverBtn(yh, yes);
		INT_PTY_MButton no = INT_PTY_MButton.Create(band, 1168 + yes.m_W + 16, 231, 56, "", "#STR_INT_PTY_W_C_NO", INT_PTY_MButton.PLAIN, false, 15, "ESC", 192);
		ButtonWidget nh = HitOn(SEC_OVER, no.m_Root, 0, 0, no.m_W, no.m_H, "modal:no");
		SetHoverBtn(nh, no);
		m_ModalYes = yes;
		m_ModalNo = no;
		INT_PTY_MenuFx.FadeIn(dim, 0, 0.16);
		INT_PTY_MenuFx.BlockIn(band, 0, 14, 0.02, 0.26);
		INT_PTY_MenuFx.GrowW(topLine, 0.06, 0.4);
		INT_PTY_Sfx.Warn();
	}

	protected ref INT_PTY_MButton m_ModalYes;
	protected ref INT_PTY_MButton m_ModalNo;

	bool HasModal2()
	{
		return m_ModalW != null;
	}

	protected void CloseModal()
	{
		if (!m_ModalW)
			return;
		DropHitsUnder(SEC_OVER, m_ModalW);
		INT_UIF_Animator.Get().Cancel(m_ModalW);
		m_ModalW.Unlink();
		m_ModalW = null;
		m_ModalYes = null;
		m_ModalNo = null;
	}

	protected void ModalAnswer(bool yes)
	{
		string action = m_ModalAction;
		m_ModalAction = "";
		CloseModal();
		if (!yes)
		{
			INT_PTY_Sfx.Close();
			return;
		}
		INT_PTY_Sfx.Click();
		if (action.IndexOf("remove:") == 0)
		{
			string id = action.Substring(7, action.Length() - 7);
			INT_PTY_Client.RemoveFriend(id);
		}
	}

	// --- live data ----------------------------------------------------------------------------------------------
	protected void Subscribe()
	{
		if (m_Subscribed)
			return;
		m_Subscribed = true;
		INT_PTY_Client.OnChanged().Insert(OnClientChanged);
	}

	protected void Unsubscribe()
	{
		if (!m_Subscribed)
			return;
		m_Subscribed = false;
		INT_PTY_Client.OnChanged().Remove(OnClientChanged);
	}

	void OnClientChanged(int what)
	{
		if (!m_Built || m_Closing2)
			return;
		if ((what & INT_PTY_What.SEARCH) != 0)
			UpdateSearchInd(true);
		UpdateBadges(true);
		if (m_ModalW && (what & INT_PTY_What.FRIENDS) != 0 && m_ModalAction.IndexOf("remove:") == 0)
		{
			string mid = m_ModalAction.Substring(7, m_ModalAction.Length() - 7);
			if (!INT_PTY_Client.Friend(mid))
			{
				m_ModalAction = "";
				CloseModal();
			}
		}
		if (m_Page)
			m_Page.OnData(what);
	}

	// --- frame / input / lifecycle --------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		m_Time = m_Time + timeslice;
		if (m_OldPage && m_OldPageLeft > 0)
		{
			m_OldPageLeft = m_OldPageLeft - timeslice;
			if (m_OldPageLeft <= 0)
			{
				m_OldPage.Unlink();
				m_OldPage = null;
			}
		}
		TickToasts(timeslice);
		if (m_TipHit && m_TipWait > 0)
		{
			m_TipWait = m_TipWait - timeslice;
			if (m_TipWait <= 0)
				ShowTip();
		}
		if (m_DragKey != "")
		{
			int mx;
			int my;
			GetMousePos(mx, my);
			bool held = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
			string dk = m_DragKey;
			if (!held)
				m_DragKey = "";
			if (m_Page)
				m_Page.OnSlide(dk, mx, !held);
		}
		if (m_Page && m_Built && !m_Closing2)
			m_Page.Tick(timeslice);
		if (m_Closing2)
			return;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb || !pb.IsAlive())
		{
			m_Closing2 = true;
			CloseNow();
			return;
		}
		bool enterNow = EnterDown();
		if (m_ModalW && enterNow && !m_EnterWas)
		{
			m_EnterWas = enterNow;
			ModalAnswer(true);
			return;
		}
		m_EnterWas = enterNow;
		// W-7: Esc in the rename field (raw key edge: UAUIBack may not arrive while the keyboard focus is ours).
		bool escNow = KeyState(KeyCode.KC_ESCAPE) != 0;
		bool escEdge = escNow && !m_EscWas;
		m_EscWas = escNow;
		if (m_KbLocked && escEdge)
		{
			EscLeavesEdit();
			return;
		}
		if (m_EscGuard > 0)
		{
			if (escNow)
				m_EscGuard = ESC_GUARD_S;
			else
				m_EscGuard = m_EscGuard - timeslice;
		}
		UAInput back = GetUApi().GetInputByID(UAUIBack);
		if (back && back.LocalPress())
		{
			// the press that left the rename field does not also close the window
			if (m_EscGuard > 0)
				return;
			if (m_KbLocked)
			{
				BlurEdit();
				return;
			}
			if (m_ModalW)
			{
				ModalAnswer(false);
				return;
			}
			RequestClose();
		}
	}

	override void OnShow()
	{
		super.OnShow();
		s_Open = this;
		m_Closing2 = false;
		Subscribe();
		INT_PTY_Sfx.Open();
	}

	override void OnHide()
	{
		CommitEdit();
		super.OnHide();
		if (s_Open == this)
			s_Open = null;
		Unsubscribe();
		LockKeyboard(false);
		HideTip();
		if (m_Page)
			m_Page.OnLeave();
		if (m_CloseTween)
			m_CloseTween.Cancel();
		m_CloseTween = null;
		INT_PTY_SettingsStore.Flush();
		INT_PTY_MenuFx.ForgetHomes();
		m_Built = false;
	}

	// Close (P / Esc / ×): the content fades and lifts, the shade fades, then the engine closes the menu.
	override void RequestClose()
	{
		if (m_Closing2)
			return;
		m_Closing2 = true;
		m_Closing = true;
		HideTip();
		BlurEdit();
		CommitEdit();
		INT_PTY_Sfx.Close();
		INT_PTY_MenuFx.FadeOut(m_Canvas, 0, 0.18);
		if (m_Canvas)
			INT_UIF_Animator.MoveY(m_Canvas, m_CanvasY - INT_UIF_Draw.S(16), 0.2, INT_UIF_Ease.IN_QUAD);
		if (m_Shade)
			INT_UIF_Animator.Fade(m_Shade, 0, 0.22, INT_UIF_Ease.IN_QUAD);
		INT_UIF_Tween t = INT_UIF_Animator.Wait(0.22);
		m_CloseTween = t;
		if (t)
			t.OnComplete().Insert(OnCloseFaded);
		else
			CloseNow();
	}

	protected void OnCloseFaded(INT_UIF_Tween t)
	{
		m_CloseTween = null;
		CloseNow();
	}

	// --- UI self-test seams (INT_PTY_TestKit_Client) ----------------------------------------------------------------
	override void OnTestCommand(string command)
	{
	}

	// Clicks the hit registered under `key` through the real click path.
	bool TestClick(string key)
	{
		Widget found = TestFind(key);
		if (!found)
			return false;
		OnActClick(found);
		return true;
	}

	Widget TestFind(string key)
	{
		foreach (Widget w, INT_PTY_MenuAct a : m_Acts)
		{
			if (w && a && a.Key == key)
				return w;
		}
		return null;
	}

	// Hovers `key` and shows its tooltip at once.
	bool TestHover(string key)
	{
		Widget found = TestFind(key);
		if (!found)
			return false;
		s_TestPinTip = false;
		OnActHover(found, true);
		m_TipWait = 0;
		ShowTip();
		s_TestPinTip = true;
		return m_TipW != null;
	}

	bool TestWheel(string key, int wheel)
	{
		Widget found = TestFind(key);
		if (!found)
			return false;
		OnActWheel(found, wheel);
		return true;
	}

	bool TestTipShown()
	{
		return m_TipW != null;
	}

	bool TestModal()
	{
		return m_ModalW != null;
	}

	int TestActCount()
	{
		return m_Acts.Count();
	}

	string TestPageInfo()
	{
		if (!m_Page)
			return "";
		return m_Page.TestInfo();
	}

	void TestSwitch(int tab)
	{
		SwitchTab(tab);
	}

	bool TestType(string text)
	{
		if (!m_Edit)
			return false;
		m_Edit.SetText(text);
		OnEditChange(m_Edit, false);
		if (m_Page)
			m_Page.OnEditDone(text);
		return true;
	}

	void TestEsc()
	{
		if (m_ModalW)
		{
			ModalAnswer(false);
			return;
		}
		RequestClose();
	}

	void TestEnter()
	{
		if (m_ModalW)
			ModalAnswer(true);
	}
}

// Window sounds go through INT_PTY_Sfx (which honours «Звук кнопок»).
class INT_PTY_MenuSnd
{
	static void Hover()
	{
		INT_PTY_Sfx.Hover();
	}

	static void Click()
	{
		INT_PTY_Sfx.Click();
	}

	static void Toggle()
	{
		INT_PTY_Sfx.Toggle();
	}
}
