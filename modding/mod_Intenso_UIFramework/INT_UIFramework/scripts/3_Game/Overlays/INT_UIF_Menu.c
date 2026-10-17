// ============================================================================
// INT_UIFramework - menu base + registry (v2 window).
//
// Subclass INT_UIF_Menu, override GetTitle()/GetWindowSize()/OnBuild(body)
// (+ optional header slots, hints), register the class once under a menu ID:
//     INT_UIF_MenuRegistry.Register(MY_MENU_ID, MyMenu);
// and open it with INT_UIF_Menu.Open(MY_MENU_ID).
//
// The window is composed in script from INT_UIF_Draw primitives on the
// 1920x1080 design canvas: soft shadow, charcoal body with a hairline border
// and top sheen, header (accent app tile + glow, title, CAPS subtitle, centre
// and right slots, close tile), body, key hints under the window. Opening fades
// the dim in, lifts the window into place and cascades registered panels in
// (RevealOnOpen); closing reverses it.
//
// Opening goes through vanilla's ID path (EnterScriptedMenu ->
// MissionGameplay.CreateScriptedMenu); the framework's 5_Mission hooks
// CreateScriptedMenu and asks the registry.
// ============================================================================

class INT_UIF_MenuRegistry
{
	protected static ref TIntTypenameMap s_Menus;

	static void Register(int id, typename menuType)
	{
		if (!s_Menus)
			s_Menus = new TIntTypenameMap();
		if (!menuType.IsInherited(UIScriptedMenu))
		{
			INT_UIF_Log.Error("Register: " + menuType.ToString() + " is not a UIScriptedMenu");
			return;
		}
		s_Menus.Set(id, menuType);
	}

	static bool Has(int id)
	{
		return s_Menus && s_Menus.Contains(id);
	}

	// Called from the modded MissionGameplay.CreateScriptedMenu.
	static UIScriptedMenu Create(int id)
	{
		if (!s_Menus)
			return null;
		typename t;
		if (!s_Menus.Find(id, t))
			return null;
		UIScriptedMenu menu = UIScriptedMenu.Cast(t.Spawn());
		if (menu)
			menu.SetID(id);
		return menu;
	}
}

// Forwards widget events to the menu: UIScriptedMenu is not a
// ScriptedWidgetEventHandler, so a menu cannot be a widget's handler itself.
class INT_UIF_MenuHandler extends ScriptedWidgetEventHandler
{
	protected INT_UIF_Menu m_Menu;

	void SetMenu(INT_UIF_Menu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (m_Menu)
			return m_Menu.OnFrameClick(w, button);
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (m_Menu)
			return m_Menu.OnFrameHover(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (m_Menu)
			return m_Menu.OnFrameHover(w, false);
		return false;
	}
}

class INT_UIF_Menu extends UIScriptedMenu
{
	static const float HEADER_H = 76;
	static const float PAD = 16;
	static const float FOOTER_H = 44;

	protected Widget m_Dim;
	protected Widget m_Window;
	protected Widget m_Header;
	protected Widget m_HeaderCenter;
	protected Widget m_HeaderRight;
	protected Widget m_Body;
	protected Widget m_Content;      // legacy alias of m_Body
	protected Widget m_Footer;
	protected Widget m_Hints;
	protected Widget m_ModalLayer;

	protected ref INT_UIF_Surface m_WinBg;
	protected ref INT_UIF_Surface m_AppTile;
	protected ref INT_UIF_Surface m_CloseTile;
	protected ImageWidget m_AppGlow;
	protected ImageWidget m_AppGlyph;
	protected ImageWidget m_HeaderLine;
	protected ImageWidget m_CloseIcon;
	protected ButtonWidget m_CloseHit;
	protected TextWidget m_Title;
	protected TextWidget m_Subtitle;

	protected ref INT_UIF_MenuHandler m_Handler;
	protected ref INT_UIF_Bag m_Bag;
	protected ref INT_UIF_Dialog m_Modal;
	protected ref array<Widget> m_RevealList;
	protected bool m_ModalOpen;
	protected bool m_Closing;
	protected bool m_CloseCalled;
	protected bool m_ThemeHooked;
	protected float m_WinW;
	protected float m_WinH;
	protected float m_HintsX;

	// Player freeze, HUD and blur are reference counted: menus can replace each
	// other (the old OnHide may run after the new OnShow) without leaking state.
	protected static int s_FreezeDepth;
	protected static int s_HudDepth;
	protected static int s_BlurDepth;

	void INT_UIF_Menu()
	{
		m_Bag = new INT_UIF_Bag();
		m_RevealList = new array<Widget>();
		m_Handler = new INT_UIF_MenuHandler();
		m_Handler.SetMenu(this);
	}

	void ~INT_UIF_Menu()
	{
		UnhookTheme();
	}

	// --- configuration hooks ---------------------------------------------------
	string GetTitle()
	{
		return "";
	}

	// Small CAPS line under the title.
	string GetSubtitle()
	{
		return "";
	}

	// Glyph on the accent app tile ("shield_fill", "gear", ...); "" = no tile.
	string GetIcon()
	{
		return "";
	}

	// Palette key of the app tile ("amber", "teal", ...).
	string GetIconHue()
	{
		return INT_UIF_ThemeKeys.AMBER;
	}

	// Window size in design px (1920x1080 canvas).
	void GetWindowSize(out float w, out float h)
	{
		w = 1100;
		h = 720;
	}

	bool HasFooter()
	{
		return false;
	}

	// False: no window chrome, only the dim layer (used by the dialog host).
	bool ShowWindow()
	{
		return true;
	}

	bool CloseOnEscape()
	{
		return true;
	}

	bool FreezesPlayer()
	{
		return true;
	}

	bool UsesBlur()
	{
		return true;
	}

	// Menus with text inputs may need keyboard focus. Default false: the
	// engine freezes mouse-look for a menu only when !UseKeyboard().
	bool WantsKeyboard()
	{
		return false;
	}

	// Build the window body (design px inside GetBodyW() x GetBodyH()).
	void OnBuild(Widget body)
	{
	}

	// Header slots: centre (status pills, tabs) and right (role chips, left of
	// close). Both are frames of GetHeaderSlotW() x HEADER_H design px.
	// Called BEFORE OnBuild, so the body can attach pages to header tabs.
	void OnBuildHeader(Widget center, Widget right)
	{
	}

	void OnBuildFooter(Widget footer)
	{
	}

	// Called once body, header and footer exist: fill data here.
	void OnReady()
	{
	}

	// Re-paint custom widgets of the subclass on theme change.
	void OnApplyTheme()
	{
	}

	// Self-test / preview hook: the words after "keep" in the self-test
	// marker (e.g. "forms popup"). Put the menu into the state to capture.
	void OnTestCommand(string command)
	{
	}

	// --- geometry helpers ---------------------------------------------------------
	float GetBodyW()
	{
		return m_WinW - 2 * PAD;
	}

	float GetBodyH()
	{
		float h = m_WinH - HEADER_H - 14 - PAD;
		if (HasFooter())
			h = h - FOOTER_H - 10;
		return h;
	}

	float GetHeaderSlotW()
	{
		return 420;
	}

	// --- static API -----------------------------------------------------------------
	// Opens a registered menu; closes whatever menu is open first (DayZ shows
	// one scripted menu at a time - a second EnterScriptedMenu on top is a no-op).
	static bool Open(int id)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return false;
		UIManager um = GetGame().GetUIManager();
		if (!um)
			return false;
		UIScriptedMenu current = um.GetMenu();
		if (current)
		{
			INT_UIF_Menu ours = INT_UIF_Menu.Cast(current);
			// The same menu mid-close is replaced, not kept (a dialog may be
			// waiting for the dialog host that is just closing).
			if (current.GetID() == id && (!ours || !ours.IsClosingDown()))
				return true;
			if (ours)
				ours.CloseNow();
			else
				um.CloseMenu(current.GetID());
		}
		return um.EnterScriptedMenu(id, null) != null;
	}

	static INT_UIF_Menu Current()
	{
		if (!GetGame() || !GetGame().GetUIManager())
			return null;
		return INT_UIF_Menu.Cast(GetGame().GetUIManager().GetMenu());
	}

	static void CloseCurrent()
	{
		INT_UIF_Menu m = Current();
		if (m)
			m.RequestClose();
	}

	// --- engine lifecycle -------------------------------------------------------------
	override Widget Init()
	{
		INT_UIF_Assets.EnsureLoaded();
		layoutRoot = INT_UIF_Widgets.Create("menu_frame.layout", null);
		if (!layoutRoot)
			return null;
		INT_UIF_Log.Debug("menu init " + Type().ToString());

		m_Dim = INT_UIF_Widgets.Find(layoutRoot, "Dim");
		m_Window = INT_UIF_Widgets.Find(layoutRoot, "Window");
		m_ModalLayer = INT_UIF_Widgets.Find(layoutRoot, "ModalLayer");
		layoutRoot.SetHandler(m_Handler);

		float ww;
		float wh;
		GetWindowSize(ww, wh);
		m_WinW = ww;
		m_WinH = wh;
		if (m_Window)
		{
			m_Window.Show(ShowWindow());
			INT_UIF_Draw.Resize(m_Window, m_WinW, m_WinH);
			m_Window.SetPos(0, 0);
			if (ShowWindow())
				BuildChrome();
		}
		// Window-less hosts (the dialog host) still get an OnBuild body.
		if (!m_Body)
			m_Body = INT_UIF_Draw.Frame(layoutRoot, 0, 0, 1, 1, "Body");

		if (CloseOnEscape() && ShowWindow())
			AddHint("Esc", "#STR_INT_UIF_HINT_CLOSE");
		// Header first: the body may hook into header components (tabs).
		if (m_HeaderCenter)
			OnBuildHeader(m_HeaderCenter, m_HeaderRight);
		if (m_Body)
			OnBuild(m_Body);
		if (m_Footer && HasFooter())
			OnBuildFooter(m_Footer);
		ApplyFrameTheme();
		HookTheme();
		OnReady();
		return layoutRoot;
	}

	protected void BuildChrome()
	{
		float w = m_WinW;
		float h = m_WinH;
		INT_UIF_Surface.Shadow(m_Window, -56, -44, w + 112, h + 112, 0);
		m_WinBg = INT_UIF_Surface.Box(m_Window, 0, 0, w, h, 14, 0, 0);
		INT_UIF_Draw.Image(m_Window, 0, 0, w, 160, "iuf_grad_v", ARGB(6, 255, 255, 255));
		INT_UIF_Draw.Image(m_Window, 40, 0, w - 80, 1, "iuf_grad_band", ARGB(40, 255, 255, 255));

		m_Header = INT_UIF_Draw.Frame(m_Window, 0, 0, w, HEADER_H, "Header");
		float tx = 24;
		if (GetIcon() != "")
		{
			m_AppGlow = INT_UIF_Draw.Image(m_Header, 0, -2, 80, 80, "iuf_radial", 0);
			Widget tile = INT_UIF_Draw.Frame(m_Header, 20, 18, 40, 40, "AppTile");
			m_AppTile = INT_UIF_Surface.Box(tile, 0, 0, 40, 40, 10, 0, 0);
			INT_UIF_Draw.Image(tile, 0, 0, 40, 20, "iuf_grad_v", ARGB(56, 255, 255, 255));
			m_AppGlyph = INT_UIF_Draw.Icon(tile, 10, 10, 20, GetIcon(), 0);
			tx = 74;
		}
		string sub = GetSubtitle();
		if (sub != "")
		{
			m_Title = INT_UIF_Draw.Text(m_Header, tx, 16, 560, 26, GetTitle(), 20, 0);
			m_Subtitle = INT_UIF_Draw.Text(m_Header, tx, 42, 560, 16, sub, 11, 0);
		}
		else
		{
			m_Title = INT_UIF_Draw.Text(m_Header, tx, 0, 560, HEADER_H, GetTitle(), 20, 0);
		}
		if (m_Title)
			m_Title.SetBold(true);

		float slot = GetHeaderSlotW();
		m_HeaderCenter = INT_UIF_Draw.Frame(m_Header, (w - slot) * 0.5, 0, slot, HEADER_H, "HeaderCenter");
		m_HeaderRight = INT_UIF_Draw.Frame(m_Header, w - 20 - 32 - 10 - slot, 0, slot, HEADER_H, "HeaderRight");

		Widget close = INT_UIF_Draw.Frame(m_Header, w - 20 - 32, 22, 32, 32, "Close");
		m_CloseTile = INT_UIF_Surface.Box(close, 0, 0, 32, 32, 10, 0, 0);
		m_CloseIcon = INT_UIF_Draw.Icon(close, 10, 10, 12, "close", 0);
		m_CloseHit = INT_UIF_Draw.Hit(close, 0, 0, 32, 32, m_Handler);
		m_HeaderLine = INT_UIF_Draw.Line(m_Header, 0, HEADER_H - 1, w, 1, 0);

		m_Body = INT_UIF_Draw.Frame(m_Window, PAD, HEADER_H + 14, GetBodyW(), GetBodyH(), "Body");
		m_Content = m_Body;
		if (HasFooter())
			m_Footer = INT_UIF_Draw.Frame(m_Window, PAD, h - PAD - FOOTER_H, w - 2 * PAD, FOOTER_H, "Footer");
		// Hints sit under the window but are NOT its child: widgets past a
		// parent's rect are clipped. Placed in screen px under the centred window.
		m_Hints = INT_UIF_Draw.Frame(layoutRoot, 0, 0, w, 22, "Hints");
		if (m_Hints)
		{
			float hx = (INT_UIF_Scale.ScreenW() - INT_UIF_Draw.S(w)) * 0.5 + INT_UIF_Draw.S(4);
			float hy = (INT_UIF_Scale.ScreenH() + INT_UIF_Draw.S(h)) * 0.5 + INT_UIF_Draw.S(14);
			m_Hints.SetPos(hx, hy);
			RevealOnOpen(m_Hints);
		}
		m_HintsX = 0;
	}

	// Key hint under the window: AddHint("Esc", "#STR_CLOSE").
	void AddHint(string key, string label)
	{
		if (!m_Hints)
			return;
		float adv;
		INT_UIF_Draw.Keycap(m_Hints, m_HintsX, 0, key, label, adv);
		m_HintsX = m_HintsX + adv + 18;
	}

	// Entrance cascade: widgets registered here fade + rise into place when the
	// window opens, `order` steps apart.
	void RevealOnOpen(Widget w)
	{
		if (!w)
			return;
		w.SetAlpha(0);
		m_RevealList.Insert(w);
	}

	void SetTitle(string title)
	{
		if (m_Title)
			m_Title.SetText(title);
	}

	void SetSubtitle(string subtitle)
	{
		if (m_Subtitle)
			m_Subtitle.SetText(subtitle);
	}

	protected void ApplyFrameTheme()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.Color(GetIconHue());
		if (m_Dim)
			m_Dim.SetColor(tm.Color(INT_UIF_ThemeKeys.OVERLAY));
		if (m_WinBg)
		{
			m_WinBg.SetFill(tm.Color(INT_UIF_ThemeKeys.WINDOW));
			m_WinBg.SetBorder(tm.Color(INT_UIF_ThemeKeys.WINDOW_BORDER));
		}
		Widget shadow = null;
		if (m_Window)
			shadow = m_Window.GetChildren();
		if (shadow)
		{
			ButtonWidget shadowFill = ButtonWidget.Cast(shadow.FindAnyWidget("Fill"));
			if (shadowFill)
				shadowFill.SetColor(tm.Color(INT_UIF_ThemeKeys.SHADOW));
		}
		if (m_AppGlow)
			m_AppGlow.SetColor(INT_UIF_Color.WithAlpha(hue, 82));
		if (m_AppTile)
			m_AppTile.SetFill(hue);
		if (m_AppGlyph)
			m_AppGlyph.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_ON_ACCENT));
		if (m_Title)
			m_Title.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_Subtitle)
			m_Subtitle.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		if (m_HeaderLine)
			m_HeaderLine.SetColor(tm.Color(INT_UIF_ThemeKeys.DIVIDER));
		if (m_CloseTile)
		{
			m_CloseTile.SetFill(tm.Color(INT_UIF_ThemeKeys.TILE));
			m_CloseTile.SetBorder(ARGB(20, 255, 255, 255));
		}
		if (m_CloseIcon)
			m_CloseIcon.SetColor(tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
	}

	protected void HookTheme()
	{
		if (m_ThemeHooked)
			return;
		INT_UIF_ThemeManager.Get().GetOnChanged().Insert(OnThemeChanged);
		m_ThemeHooked = true;
	}

	protected void UnhookTheme()
	{
		if (!m_ThemeHooked)
			return;
		m_ThemeHooked = false;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Peek();
		if (tm && tm.GetOnChanged())
			tm.GetOnChanged().Remove(OnThemeChanged);
	}

	void OnThemeChanged(INT_UIF_Theme theme)
	{
		ApplyFrameTheme();
		m_Bag.ApplyTheme();
		OnApplyTheme();
	}

	override bool UseKeyboard()
	{
		return WantsKeyboard();
	}

	override void OnShow()
	{
		super.OnShow();
		if (layoutRoot)
			layoutRoot.Show(true);
		m_Closing = false;
		m_CloseCalled = false;
		if (FreezesPlayer())
		{
			HoldHud(true);
			FreezePlayer(true);
		}
		if (UsesBlur())
			HoldBlur(true);

		// Open: dim fades in, the window lifts into place, then the registered
		// panels cascade in. A window-less host (dialogs) has no dim: the
		// dialog brings its own shade.
		if (m_Dim && ShowWindow())
		{
			m_Dim.SetColor(INT_UIF.Color(INT_UIF_ThemeKeys.OVERLAY));
			INT_UIF_Animator.FadeInColor(m_Dim, 0.24);
		}
		else if (m_Dim)
			m_Dim.Show(false);
		if (m_Window && ShowWindow())
			INT_UIF_Animator.Reveal(m_Window, 0, 18, 0.38);
		int n = m_RevealList.Count();
		for (int i = 0; i < n; i++)
		{
			INT_UIF_Animator.Reveal(m_RevealList.Get(i), 0.1 + i * 0.07, 12, 0.36);
		}
	}

	override void OnHide()
	{
		super.OnHide();
		// A menu closed without our fade (UIManager.CloseAll, the vanilla pause's
		// CloseAllMenus) kept its full-screen root drawn after the menu was gone:
		// measured 2026-10-14 (CIH settings window left on screen, GetMenu() = none).
		if (layoutRoot)
			layoutRoot.Show(false);
		INT_UIF_Tooltip.HideTip();
		if (FreezesPlayer())
		{
			HoldHud(false);
			FreezePlayer(false);
		}
		if (UsesBlur())
			HoldBlur(false);
		if (m_Modal)
			m_Modal.Destroy();
		m_ModalOpen = false;
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (!CloseOnEscape())
			return;
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			// Esc first closes an open dropdown popup, then the modal, then us.
			if (INT_UIF_Dropdown.CloseActive())
				return;
			if (m_ModalOpen && m_Modal)
				m_Modal.Cancel();
			else
				RequestClose();
		}
	}

	// --- closing -----------------------------------------------------------------
	// Animated close; the engine Close() runs when the fade-out is done.
	void RequestClose()
	{
		if (m_Closing)
			return;
		m_Closing = true;
		INT_UIF_Tween t = null;
		if (m_Window && ShowWindow())
		{
			INT_UIF_Animator.Fade(m_Window, 0, 0.16, INT_UIF_Ease.IN_QUAD);
			INT_UIF_Animator.MoveY(m_Window, INT_UIF_Scale.Px(12), 0.18, INT_UIF_Ease.IN_QUAD);
		}
		if (m_Hints)
			INT_UIF_Animator.Fade(m_Hints, 0, 0.14, INT_UIF_Ease.IN_QUAD);
		if (m_Dim && m_Dim.IsVisible())
			t = INT_UIF_Animator.Fade(m_Dim, 0, 0.2, INT_UIF_Ease.IN_QUAD);
		if (t)
			t.OnComplete().Insert(OnCloseAnimDone);
		else
			CloseNow();
	}

	protected void OnCloseAnimDone(INT_UIF_Tween t)
	{
		CloseNow();
	}

	// Immediate close, no animation.
	void CloseNow()
	{
		m_Closing = true;
		if (m_CloseCalled)
			return;
		m_CloseCalled = true;
		Close();
	}

	// True from RequestClose() on. Not IsClosing(): UIMenuPanel has a native one.
	bool IsClosingDown()
	{
		return m_Closing;
	}

	// --- modal dialogs -------------------------------------------------------------
	// The menu keeps a ref to the dialog until the next one replaces it (or the
	// menu closes): the dialog calls back into us from its own methods, so it
	// must never lose its last strong reference mid-call. Open/closed state is
	// the separate m_ModalOpen flag.
	void ShowModal(INT_UIF_Dialog dialog)
	{
		if (!dialog || !m_ModalLayer)
			return;
		if (m_ModalOpen && m_Modal && m_Modal != dialog)
			m_Modal.Destroy();
		m_Modal = dialog;
		m_ModalOpen = true;
		dialog.SetHost(this);
		m_ModalLayer.Show(true);
		dialog.Build(m_ModalLayer);
		dialog.PlayOpen();
	}

	// Called by the dialog once it is gone.
	void OnModalClosed(INT_UIF_Dialog dialog)
	{
		if (m_Modal != dialog)
			return;
		m_ModalOpen = false;
		if (m_ModalLayer)
			m_ModalLayer.Show(false);
	}

	bool HasModal()
	{
		return m_ModalOpen;
	}

	// --- accessors -----------------------------------------------------------------
	INT_UIF_Bag GetBag()
	{
		return m_Bag;
	}

	Widget GetBody()
	{
		return m_Body;
	}

	Widget GetContent()
	{
		return m_Body;
	}

	Widget GetFooter()
	{
		return m_Footer;
	}

	// --- frame events (forwarded by INT_UIF_MenuHandler) --------------------------
	bool OnFrameClick(Widget w, int button)
	{
		if (w && w == m_CloseHit)
		{
			SetFocus(null);
			RequestClose();
			return true;
		}
		return false;
	}

	bool OnFrameHover(Widget w, bool enter)
	{
		if (!w || w != m_CloseHit)
			return false;
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int red = tm.Color(INT_UIF_ThemeKeys.RED);
		if (enter)
		{
			m_CloseTile.FadeFill(INT_UIF_Color.WithAlpha(red, 40), 0.12);
			m_CloseTile.FadeBorder(INT_UIF_Color.WithAlpha(red, 110), 0.12);
			INT_UIF_Animator.ColorTo(m_CloseIcon, red, 0.12);
		}
		else
		{
			m_CloseTile.FadeFill(tm.Color(INT_UIF_ThemeKeys.TILE), 0.22);
			m_CloseTile.FadeBorder(ARGB(20, 255, 255, 255), 0.22);
			INT_UIF_Animator.ColorTo(m_CloseIcon, tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED), 0.22);
		}
		return true;
	}

	// --- player freeze / HUD (proven in INT_BP_Menu v1) ----------------------------
	static void ShowHud(bool show)
	{
		if (!GetGame() || !GetGame().GetMission())
			return;
		Hud hud = GetGame().GetMission().GetHud();
		if (!hud)
			return;
		hud.ShowQuickbarUI(show);
		hud.ShowHudUI(show);
	}

	// Counted HUD hide: the HUD comes back when the last menu releases it.
	static void HoldHud(bool hide)
	{
		if (hide)
		{
			s_HudDepth = s_HudDepth + 1;
			if (s_HudDepth == 1)
				ShowHud(false);
			return;
		}
		if (s_HudDepth <= 0)
			return;
		s_HudDepth = s_HudDepth - 1;
		if (s_HudDepth == 0)
			ShowHud(true);
	}

	// Counted inventory blur (PPERequesterBank.REQ_INVENTORYBLUR).
	static void HoldBlur(bool on)
	{
		if (on)
		{
			s_BlurDepth = s_BlurDepth + 1;
			if (s_BlurDepth == 1)
				PPERequesterBank.GetRequester(PPERequesterBank.REQ_INVENTORYBLUR).Start();
			return;
		}
		if (s_BlurDepth <= 0)
			return;
		s_BlurDepth = s_BlurDepth - 1;
		if (s_BlurDepth == 0)
			PPERequesterBank.GetRequester(PPERequesterBank.REQ_INVENTORYBLUR).Stop();
	}

	static void FreezePlayer(bool freeze)
	{
		if (!GetGame() || !GetGame().GetMission())
			return;
		if (freeze)
		{
			s_FreezeDepth = s_FreezeDepth + 1;
			if (s_FreezeDepth > 1)
				return;
			GetGame().GetMission().AddActiveInputExcludes({"menu"});
			GetGame().GetMission().AddActiveInputRestriction(EInputRestrictors.INVENTORY);
			LockCameraMotion(true);
			GetUApi().SupressNextFrame(true);
		}
		else
		{
			if (s_FreezeDepth > 0)
				s_FreezeDepth = s_FreezeDepth - 1;
			if (s_FreezeDepth > 0)
				return;
			LockCameraMotion(false);
			GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"});
			GetUApi().SupressNextFrame(true);
		}
	}

	// Excludes stop NEW input; these overrides stop motion already in flight.
	static void LockCameraMotion(bool lock)
	{
		DayZPlayer player = DayZPlayer.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		HumanInputController hic = player.GetInputController();
		if (!hic)
			return;
		// human.c:7-12 - ENABLED holds until DISABLED is passed.
		HumanInputControllerOverrideType mode = HumanInputControllerOverrideType.DISABLED;
		if (lock)
			mode = HumanInputControllerOverrideType.ENABLED;
		hic.OverrideAimChangeX(mode, 0);
		hic.OverrideAimChangeY(mode, 0);
		hic.OverrideMovementSpeed(mode, 0);
		hic.OverrideMovementAngle(mode, 0);
		hic.OverrideFreeLook(mode, false);
		if (lock)
			hic.ResetFreeLookToggle();
	}

	// Safety valve for mission end: never leave the player frozen.
	static void ResetFreeze()
	{
		if (s_HudDepth > 0)
		{
			s_HudDepth = 1;
			HoldHud(false);
		}
		if (s_BlurDepth > 0)
		{
			s_BlurDepth = 1;
			HoldBlur(false);
		}
		if (s_FreezeDepth <= 0)
			return;
		s_FreezeDepth = 1;
		FreezePlayer(false);
	}
}
