// INT_PDA (client) — the PDA window in style E «ЗОНА 2.0» (design/concepts_v1/5_e_zone2_*). No UIF chrome: the device
// body is a texture (set:int_pda_dev image:dev, 1500 x 890, no logo — owner 2026-10-23), the screen sits at (22, 24)
// 1456 x 800: top bar 88 (tab words + status), body 674 (pages), footer 38 (news ticker + key hints).
// Screens: boot → login / registration → tabs; a looted PDA shows the trophy screen; Alt+L opens the admin mode
// without a PDA. Open: the device rises, the screen opens from the centre (black halves part), the boot log types;
// close: the halves meet, the device drops. Esc closes on RELEASE (so the vanilla pause menu does not open behind),
// first leaving a focused text field; TAB switches tabs.

class INT_PDA_MenuHandler : ScriptedWidgetEventHandler
{
	INT_PDA_Menu m_Menu;

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (m_Menu)
			return m_Menu.OnHit(w);
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (m_Menu)
			m_Menu.OnHover(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (m_Menu)
			m_Menu.OnHover(w, false);
		return false;
	}
}

class INT_PDA_Menu extends INT_UIF_Menu
{
	static const int ST_BOOT = 0;
	static const int ST_AUTH = 1;
	static const int ST_MAIN = 2;
	static const int ST_TROPHY = 3;

	static const int TAB_PROFILE = 0;
	static const int TAB_MAP = 1;
	static const int TAB_CHAT = 2;
	static const int TAB_FACTION = 3;
	static const int TAB_NEWS = 4;
	static const int TAB_ADMIN = 5;
	static const int TAB_CHAR = 6;				// «Персонаж» (owner 2026-10-26): the last player tab, before «Админ» (m_TabIds = order)

	protected static INT_PDA s_Request;
	protected static bool s_BootedOnce;		// full boot log only the first time per connection
	protected static bool s_Opening;		// OpenWith is replacing a PDA window: the old one's OnHide keeps the admin mode

	INT_PDA m_Pda;
	protected ref INT_PDA_MenuHandler m_PdaHandler;
	protected int m_State = -1;
	protected float m_DevX;
	protected float m_DevY;
	protected Widget m_Device;
	protected ImageWidget m_Bezel;
	protected Widget m_Screen;
	protected Widget m_ScreenContent;
	protected Widget m_Top;
	protected Widget m_BodyArea;
	protected Widget m_Foot;
	protected Widget m_Overlay;			// prompts above the pages
	protected Widget m_Shutter;			// black halves of the power-on / off effect
	protected ImageWidget m_HalfTop;
	protected ImageWidget m_HalfBottom;
	protected ImageWidget m_ScanLine;
	protected Widget m_Boot;
	protected ref array<TextWidget> m_BootLines = new array<TextWidget>();
	protected ImageWidget m_BootBar;
	protected float m_BootT = -1;
	protected bool m_Closing2;
	// tabs
	protected ref array<ref INT_PDA_Page> m_Pages = new array<ref INT_PDA_Page>();
	protected ref array<int> m_TabIds = new array<int>();
	protected ref array<TextWidget> m_TabText = new array<TextWidget>();
	protected ref array<Widget> m_TabBadge = new array<Widget>();
	protected ref array<TextWidget> m_TabBadgeText = new array<TextWidget>();
	protected ref array<ButtonWidget> m_TabHit = new array<ButtonWidget>();
	protected ref array<float> m_TabX = new array<float>();
	protected ref array<float> m_TabW = new array<float>();
	protected ImageWidget m_TabLine;
	protected int m_Tab = -1;
	protected ref INT_PDA_PageAuth m_AuthPage;
	protected ref INT_PDA_PageTrophy m_TrophyPage;
	// status bar
	protected TextWidget m_Clock;
	protected TextWidget m_Battery;
	protected ImageWidget m_BatteryIcon;
	protected ref array<Widget> m_ModChips = new array<Widget>();
	protected ref array<TextWidget> m_ModText = new array<TextWidget>();
	protected ref array<ref INT_UIF_Surface> m_ModBox = new array<ref INT_UIF_Surface>();
	protected ButtonWidget m_PowerHit;
	protected ImageWidget m_CloseX;
	protected TextWidget m_NewsLine;
	protected int m_NewsIdx;					// rotation of the «Новости Зоны» line (0 = newest)
	protected float m_NewsT;
	protected ref array<INT_PDA_News> m_NewsLive = new array<INT_PDA_News>();
	protected TextWidget m_AdminFlag;
	protected Widget m_HeadTitle;			// «КПК СТАЛКЕРА / СЕТЬ ЗОНЫ» in place of the tabs before the login
	protected ref array<Widget> m_TabExtras = new array<Widget>();	// admin tab shield + «только админы», hidden with the tabs
	protected float m_StatusT;
	protected bool m_EscArmed;
	protected bool m_TabArmed;
	protected ref INT_PDA_Prompt m_Prompt;
	protected ref array<ref INT_PDA_Prompt> m_DeadPrompts = new array<ref INT_PDA_Prompt>();	// closed, dropped next frame
	protected float m_LowBeepT;

	// --- open ---------------------------------------------------------------------------------------------------------
	static void OpenWith(INT_PDA pda)
	{
		s_Request = pda;
		s_Opening = true;
		INT_UIF_Menu.Open(INT_PDA_Const.MENU_PDA);
		s_Opening = false;
	}

	static INT_PDA_Menu Get()
	{
		return INT_PDA_Menu.Cast(INT_UIF_Menu.Current());
	}

	override bool ShowWindow()
	{
		return false;
	}

	override bool CloseOnEscape()
	{
		return false;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override bool UsesBlur()
	{
		return true;
	}

	override void OnBuild(Widget body)
	{
		INT_PDA_Look.EnsureAssets();
		INT_PDA_Look.Activate();
		m_Pda = s_Request;
		s_Request = null;
		m_PdaHandler = new INT_PDA_MenuHandler();
		m_PdaHandler.m_Menu = this;
		m_DevX = (DesignW() - INT_PDA_Look.DEV_W) * 0.5;
		m_DevY = (DesignH() - INT_PDA_Look.DEV_H) * 0.5;
		INT_PDA_Look.AttachProbe(layoutRoot);
		m_Device = INT_UIF_Draw.Frame(layoutRoot, m_DevX, m_DevY + 70, INT_PDA_Look.DEV_W, INT_PDA_Look.DEV_H, "INT_PDA_Device");
		m_Bezel = INT_UIF_Draw.Texture(m_Device, 0, 0, INT_PDA_Look.DEV_W, INT_PDA_Look.DEV_H, INT_PDA_Look.SET_DEV + "dev", INT_PDA_Look.WHITE);
		m_Screen = INT_UIF_Draw.Clip(m_Device, INT_PDA_Look.SCR_X, INT_PDA_Look.SCR_Y, INT_PDA_Look.SCR_W, INT_PDA_Look.SCR_H, "INT_PDA_Screen");
		BuildBackground();
		m_ScreenContent =INT_UIF_Draw.Frame(m_Screen, 0, 0, INT_PDA_Look.SCR_W, INT_PDA_Look.SCR_H, "INT_PDA_Content");
		BuildTop();
		m_BodyArea = INT_UIF_Draw.Frame(m_ScreenContent, 0, INT_PDA_Look.TOP_H, INT_PDA_Look.SCR_W, INT_PDA_Look.BODY_H, "INT_PDA_Body");
		BuildFoot();
		BuildPages();
		BuildOverlays();
		INT_PDA_Client.s_OnChanged.Insert(OnClientChanged);
		INT_PDA_Client.s_OnResult.Insert(OnClientResult);
		INT_PDA_Hud.Suppress(true);
		if (m_Pda && !INT_PDA_Client.IsTrophy(m_Pda))
			INT_PDA_Client.RefreshListening();
	}

	override void OnReady()
	{
		// the device rises, the screen opens from the centre
		INT_UIF_Animator.MoveY(m_Device, INT_UIF_Draw.S(m_DevY), 0.42, INT_UIF_Ease.OUT_CUBIC);
		m_Bezel.SetAlpha(0);
		INT_UIF_Animator.FadeIn(m_Bezel, 0.3);
		PowerOn();
		INT_PDA_Sfx.Play("Boot");
		if (!s_BootedOnce && !INT_PDA_Client.s_LoggedIn && !INT_PDA_Client.IsAdminMode())
		{
			StartBootLog();
			return;
		}
		EnterState(PickState());
	}

	override void OnHide()
	{
		if (INT_PDA_Client.s_OnChanged)
			INT_PDA_Client.s_OnChanged.Remove(OnClientChanged);
		if (INT_PDA_Client.s_OnResult)
			INT_PDA_Client.s_OnResult.Remove(OnClientResult);
		ClosePrompt();
		DropDeadPrompts();
		foreach (INT_PDA_Page p : m_Pages)
		{
			if (p && p.m_Active)
				p.Show(false);
		}
		INT_PDA_Hud.Suppress(false);
		INT_PDA_Look.DetachProbe();
		INT_PDA_Look.Restore();
		// the admin mode (Alt+L) ends with this window (unless a new PDA window is replacing this one right now or
		// already did); the admin's listening without a PDA ends with it
		INT_PDA_Menu cur = Get();
		if (!s_Opening && (!cur || cur == this))
		{
			INT_PDA_Client.SetAdminMode(false);
			INT_PDA_Client.RefreshListening();
		}
		super.OnHide();
	}

	protected float DesignW()
	{
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			return 1920;
		return INT_UIF_Scale.ScreenW() / f;
	}

	protected float DesignH()
	{
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			return 1080;
		return INT_UIF_Scale.ScreenH() / f;
	}

	int PickState()
	{
		if (INT_PDA_Client.IsAdminMode())
			return ST_MAIN;
		if (INT_PDA_Client.IsTrophy(m_Pda))
			return ST_TROPHY;
		if (!INT_PDA_Client.s_LoggedIn)
			return ST_AUTH;
		return ST_MAIN;
	}

	// --- top bar --------------------------------------------------------------------------------------------------
	protected void BuildTop()
	{
		float w = INT_PDA_Look.SCR_W;
		m_Top = INT_UIF_Draw.Frame(m_ScreenContent, 0, 0, w, INT_PDA_Look.TOP_H, "INT_PDA_Top");
		INT_UIF_Draw.Line(m_Top, 0, INT_PDA_Look.TOP_H - 1, w, 1, INT_PDA_Look.LINE);
		INT_UIF_Draw.Image(m_Top, 0, 0, w, 90, "iuf_grad_v", ARGB(9, 255, 255, 255));
		// status block (time, battery, signal, modules) and the close cross
		float sx = 1156;
		m_Clock = INT_UIF_Draw.Text(m_Top, sx, 8, 110, 36, "00:00", 30, INT_PDA_Look.TEXT);
		m_BatteryIcon = INT_PDA_Look.DrawIcon(m_Top, sx + 100, 14, 22, "battery", INT_PDA_Look.TEXT);
		m_Battery = INT_UIF_Draw.Text(m_Top, sx + 126, 12, 70, 24, "--%", 16, INT_PDA_Look.TEXT);
		INT_PDA_Look.DrawIcon(m_Top, sx + 196, 12, 22, "signal", INT_PDA_Look.ORANGE);
		// module chips (concept E): short words in hairline boxes, orange while the module is in the PDA
		array<string> mods = {"GPS", "#STR_INT_PDA_CHIP_MAP", "#STR_INT_PDA_CHIP_CHAT", "#STR_INT_PDA_CHIP_GP"};
		array<float> widths = new array<float>();
		float rowW = 0;
		int i;
		for (i = 0; i < mods.Count(); i++)
		{
			float lw = INT_PDA_Look.TextW(INT_PDA_Look.Caps(mods[i]), 13) + 16;
			widths.Insert(lw);
			rowW = rowW + lw + 6;
		}
		float mx = 1386 - rowW;
		for (i = 0; i < mods.Count(); i++)
		{
			float cw = widths[i];
			Widget chip = INT_UIF_Draw.Frame(m_Top, mx, 52, cw, 20, "INT_PDA_Mod");
			INT_UIF_Surface box = INT_UIF_Surface.Box(chip, 0, 0, cw, 20, 3, 0, INT_PDA_Look.LINE2);
			TextWidget t = INT_UIF_Draw.Text(chip, 0, 0, cw, 20, INT_PDA_Look.Caps(mods[i]), 13, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
			m_ModChips.Insert(chip);
			m_ModText.Insert(t);
			m_ModBox.Insert(box);
			mx = mx + cw + 6;
		}
		Widget xbox = INT_UIF_Draw.Frame(m_Top, 1396, 23, 42, 42, "INT_PDA_Close");
		INT_UIF_Surface.Box(xbox, 0, 0, 42, 42, 6, 0, INT_PDA_Look.LINE2);
		m_CloseX = INT_PDA_Look.DrawIcon(xbox, 11, 11, 20, "close", INT_PDA_Look.TEXT);
		m_PowerHit = INT_UIF_Draw.Hit(m_Top, 1396, 23, 42, 42, m_PdaHandler);
		m_TabLine = INT_UIF_Draw.Line(m_Top, 40, INT_PDA_Look.TOP_H - 4, 10, 4, INT_PDA_Look.ORANGE);
		m_TabLine.Show(false);
		m_AdminFlag = INT_UIF_Draw.Text(m_Top, 40, 62, 600, 18, "", 13, INT_PDA_Look.ORANGE);
		// title of the screens without tabs (registration / login / trophy), concept 5_e_zone2_register
		m_HeadTitle = INT_UIF_Draw.Frame(m_Top, 40, 0, 1000, INT_PDA_Look.TOP_H, "INT_PDA_HeadTitle");
		INT_PDA_Look.DrawIcon(m_HeadTitle, 0, 32, 24, "signal", INT_PDA_Look.DIM);
		string product = INT_PDA_Look.Caps("#STR_INT_PDA_PRODUCT");
		string net = INT_PDA_Look.Caps("#STR_INT_PDA_HEAD_NET");
		float pw = INT_PDA_Look.TextW(product, 24) + 4;
		INT_UIF_Draw.Text(m_HeadTitle, 38, 0, pw, INT_PDA_Look.TOP_H, product, 24, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Text(m_HeadTitle, 38 + pw + 10, 0, 20, INT_PDA_Look.TOP_H, "/", 24, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(m_HeadTitle, 38 + pw + 38, 0, 500, INT_PDA_Look.TOP_H, net, 24, INT_PDA_Look.DIM);
		m_HeadTitle.Show(false);
		UpdateStatus();
	}

	protected void BuildTabs()
	{
		m_TabIds.Clear();
		m_TabIds.Insert(TAB_PROFILE);
		m_TabIds.Insert(TAB_MAP);
		m_TabIds.Insert(TAB_CHAT);
		m_TabIds.Insert(TAB_FACTION);
		m_TabIds.Insert(TAB_NEWS);
		m_TabIds.Insert(TAB_CHAR);
		if (INT_PDA_Client.s_IsAdmin)
			m_TabIds.Insert(TAB_ADMIN);
		array<string> titles = {"#STR_INT_PDA_TAB_PROFILE", "#STR_INT_PDA_TAB_MAP", "#STR_INT_PDA_TAB_CHAT", "#STR_INT_PDA_TAB_FACTION", "#STR_INT_PDA_TAB_NEWS", "#STR_INT_PDA_TAB_ADMIN", "#STR_INT_PDA_TAB_CHAR"};
		// fit the words into 40..1120 (the status block starts at 1156)
		float size = 24;
		float gap = 26;
		float total = 0;
		int i;
		string cap;
		for (i = 0; i < m_TabIds.Count(); i++)
		{
			cap = INT_PDA_Look.Caps(titles[m_TabIds[i]]);
			total = total + INT_PDA_Look.TextW(cap, size) + gap + 30;
		}
		if (total > 1080)
		{
			size = 24 * 1080 / total;
			gap = 18;
		}
		float x = 40;
		for (i = 0; i < m_TabIds.Count(); i++)
		{
			int id = m_TabIds[i];
			cap = INT_PDA_Look.Caps(titles[id]);
			float tw = INT_PDA_Look.TextW(cap, size) + 4;
			int col = INT_PDA_Look.DIM;
			if (id == TAB_ADMIN)
				col = INT_PDA_Look.ADMIN;
			float tx = x;
			if (id == TAB_ADMIN)
			{
				m_TabExtras.Insert(INT_PDA_Look.DrawIcon(m_Top, x, 32, 22, "admin", INT_PDA_Look.ORANGE));
				tx = x + 28;
			}
			TextWidget t = INT_UIF_Draw.Text(m_Top, tx, 0, tw, INT_PDA_Look.TOP_H, cap, size, col);
			Widget badge = INT_UIF_Draw.Frame(m_Top, tx + tw + 4, 31, 26, 26, "INT_PDA_Badge");
			INT_UIF_Surface.Pill(badge, 0, 0, 26, 26, INT_PDA_Look.ORANGE);
			TextWidget bt = INT_UIF_Draw.Text(badge, 0, 0, 26, 26, "", 15, INT_PDA_Look.ON_ORANGE, INT_UIF_Align.CENTER);
			badge.Show(false);
			float hitW = tx + tw - x + 30;
			ButtonWidget hit = INT_UIF_Draw.Hit(m_Top, x, 0, hitW, INT_PDA_Look.TOP_H, m_PdaHandler);
			m_TabText.Insert(t);
			m_TabBadge.Insert(badge);
			m_TabBadgeText.Insert(bt);
			m_TabHit.Insert(hit);
			m_TabX.Insert(tx);
			m_TabW.Insert(tw);
			if (id == TAB_ADMIN)
				m_TabExtras.Insert(INT_UIF_Draw.Text(m_Top, tx, 60, 200, 16, INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_ONLY"), 13, INT_PDA_Look.DIM2));
			x = tx + tw + 30 + gap;
		}
		UpdateBadges();
	}

	protected void BuildFoot()
	{
		float w = INT_PDA_Look.SCR_W;
		m_Foot = INT_UIF_Draw.Frame(m_ScreenContent, 0, INT_PDA_Look.SCR_H - INT_PDA_Look.FOOT_H, w, INT_PDA_Look.FOOT_H, "INT_PDA_Foot");
		INT_UIF_Draw.Line(m_Foot, 0, 0, w, 1, INT_PDA_Look.LINE);
		INT_PDA_Look.DrawIcon(m_Foot, 40, 11, 16, "news", INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(m_Foot, 66, 0, 170, INT_PDA_Look.FOOT_H, INT_PDA_Look.Caps("#STR_INT_PDA_ZONE_NEWS"), 14, INT_PDA_Look.ORANGE);
		m_NewsLine = INT_UIF_Draw.Text(m_Foot, 236, 0, 760, INT_PDA_Look.FOOT_H, "", 14, INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(m_Foot, 1000, 0, 416, INT_PDA_Look.FOOT_H, "#STR_INT_PDA_HINTS", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		UpdateNewsLine();
	}

	protected void BuildPages()
	{
		float w = INT_PDA_Look.SCR_W;
		float h = INT_PDA_Look.BODY_H;
		m_Pages.Clear();
		m_Pages.Insert(new INT_PDA_PageProfile());
		m_Pages.Insert(new INT_PDA_PageMap());
		m_Pages.Insert(new INT_PDA_PageChat());
		m_Pages.Insert(new INT_PDA_PageFaction());
		m_Pages.Insert(new INT_PDA_PageNews());
		m_Pages.Insert(new INT_PDA_PageAdmin());
		m_Pages.Insert(new INT_PDA_PageChar());
		foreach (INT_PDA_Page p : m_Pages)
			p.Build(this, m_BodyArea, w, h);
		// the auth / trophy pages use the whole screen below the top bar too
		m_AuthPage = new INT_PDA_PageAuth();
		m_AuthPage.Build(this, m_BodyArea, w, h);
		m_TrophyPage = new INT_PDA_PageTrophy();
		m_TrophyPage.Build(this, m_BodyArea, w, h);
	}

	// The slate under the content (concept E: flat #0B0B0C, an elliptic vignette from 62 % of the radius to 45 % black
	// at the rim, a faint gloss on top). Built from thin flat strips with stepped alpha instead of a stretched texture:
	// a small DXT gradient stretched over 1456 px shows its 4x4 blocks; strips step by < 1 colour level.
	protected void BuildBackground()
	{
		float w = INT_PDA_Look.SCR_W;
		float h = INT_PDA_Look.SCR_H;
		INT_UIF_Draw.Image(m_Screen, 0, 0, w, h, "iuf_white", INT_PDA_Look.BG);
		int n = 24;
		float rx = w * 0.5 * 0.38;
		float ry = h * 0.5 * 0.38;
		float sx = rx / n;
		float sy = ry / n;
		float t;
		int a;
		int col;
		for (int k = 0; k < n; k++)
		{
			t = 1.0 - (k + 0.5) / n;
			a = (int)Math.Round(115.0 * t * t);
			if (a <= 0)
				continue;
			col = ARGB(a, 0, 0, 0);
			INT_UIF_Draw.Image(m_Screen, k * sx, 0, sx + 1, h, "iuf_white", col);
			INT_UIF_Draw.Image(m_Screen, w - (k + 1) * sx, 0, sx + 1, h, "iuf_white", col);
			INT_UIF_Draw.Image(m_Screen, 0, k * sy, w, sy + 1, "iuf_white", col);
			INT_UIF_Draw.Image(m_Screen, 0, h - (k + 1) * sy, w, sy + 1, "iuf_white", col);
		}
		// gloss: 3.5 % white fading down over the top 90 px
		int gn = 15;
		float gs = 90.0 / gn;
		for (int g = 0; g < gn; g++)
		{
			t = 1.0 - (g + 0.5) / gn;
			a = (int)Math.Round(9.0 * t);
			if (a > 0)
				INT_UIF_Draw.Image(m_Screen, 0, g * gs, w, gs + 1, "iuf_white", ARGB(a, 255, 255, 255));
		}
	}

	protected void BuildOverlays()
	{
		float w = INT_PDA_Look.SCR_W;
		float h = INT_PDA_Look.SCR_H;
		m_Overlay = INT_UIF_Draw.Frame(m_Screen, 0, 0, w, h, "INT_PDA_Overlay");
		// (no grain / vignette over the UI: they lay noise over portraits and emblems — owner 2026-10-23; the slate's
		// shading is drawn UNDER the content in BuildBackground)
		// power-on / off shutter: two black halves and a hot line in the middle
		m_Shutter = INT_UIF_Draw.Frame(m_Screen, 0, 0, w, h, "INT_PDA_Shutter");
		m_HalfTop = INT_UIF_Draw.Image(m_Shutter, 0, 0, w, h * 0.5, "iuf_white", 0xFF050505);
		m_HalfBottom = INT_UIF_Draw.Image(m_Shutter, 0, h * 0.5, w, h * 0.5, "iuf_white", 0xFF050505);
		m_ScanLine = INT_UIF_Draw.Image(m_Shutter, 0, h * 0.5 - 1, w, 2, "iuf_white", INT_PDA_Look.ORANGE);
	}

	protected void PowerOn()
	{
		float h = INT_PDA_Look.SCR_H;
		m_ScanLine.SetAlpha(1);
		INT_UIF_Animator.MoveY(m_HalfTop, INT_UIF_Draw.S(-h * 0.5 - 4), 0.5, INT_UIF_Ease.IN_OUT_CUBIC).Delay(0.18);
		INT_UIF_Animator.MoveY(m_HalfBottom, INT_UIF_Draw.S(h + 4), 0.5, INT_UIF_Ease.IN_OUT_CUBIC).Delay(0.18);
		INT_UIF_Animator.Fade(m_ScanLine, 0, 0.35, INT_UIF_Ease.IN_QUAD).Delay(0.3);
	}

	// Animated close: halves meet, the device drops, then the engine Close().
	void PowerOff()
	{
		if (m_Closing2)
			return;
		m_Closing2 = true;
		float h = INT_PDA_Look.SCR_H;
		INT_PDA_Sfx.Play("Off");
		// the map and the 3D models (profile, character) draw above every 2D widget (incl. the shutter): hide them
		// before the power-off animation
		INT_PDA_Page active = PageOf(m_Tab);
		if (active && active.m_Active && INT_PDA_PageMap.Cast(active))
			active.Show(false);
		foreach (INT_PDA_Page withModel : m_Pages)
		{
			if (withModel)
				withModel.HidePreview();
		}
		INT_UIF_Animator.MoveY(m_HalfTop, 0, 0.22, INT_UIF_Ease.IN_CUBIC);
		INT_UIF_Animator.MoveY(m_HalfBottom, INT_UIF_Draw.S(h * 0.5), 0.22, INT_UIF_Ease.IN_CUBIC);
		INT_UIF_Animator.Fade(m_ScanLine, 1, 0.12);
		INT_UIF_Animator.MoveY(m_Device, INT_UIF_Draw.S(m_DevY + 90), 0.3, INT_UIF_Ease.IN_CUBIC).Delay(0.18);
		INT_UIF_Tween t = INT_UIF_Animator.Fade(m_Bezel, 0, 0.24, INT_UIF_Ease.IN_QUAD);
		t.Delay(0.22);
		t.OnComplete().Insert(OnPowerOffDone);
	}

	protected void OnPowerOffDone(INT_UIF_Tween t)
	{
		CloseNow();
	}

	// --- boot log -------------------------------------------------------------------------------------------------
	protected void StartBootLog()
	{
		s_BootedOnce = true;
		m_State = ST_BOOT;
		m_Boot = INT_UIF_Draw.Frame(m_BodyArea, 0, 0, INT_PDA_Look.SCR_W, INT_PDA_Look.BODY_H, "INT_PDA_Boot");
		float x = 470;
		INT_UIF_Draw.Text(m_Boot, x, 150, 600, 40, INT_PDA_Look.Caps("#STR_INT_PDA_PRODUCT"), 30, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Line(m_Boot, x, 196, 520, 1, INT_PDA_Look.LINE2);
		m_BootLines.Clear();
		for (int i = 0; i < 4; i++)
			m_BootLines.Insert(INT_UIF_Draw.Text(m_Boot, x, 214 + i * 30, 700, 26, "", 16, INT_PDA_Look.DIM));
		INT_UIF_Draw.Image(m_Boot, x, 350, 520, 4, "iuf_white", INT_PDA_Look.LINE);
		m_BootBar = INT_UIF_Draw.Image(m_Boot, x, 350, 1, 4, "iuf_white", INT_PDA_Look.ORANGE);
		m_BootT = 0;
	}

	protected void UpdateBoot(float dt)
	{
		if (m_BootT < 0)
			return;
		m_BootT = m_BootT + dt;
		for (int i = 0; i < 4 && i < m_BootLines.Count(); i++)
		{
			float start = 0.45 + i * 0.32;
			float k = (m_BootT - start) / 0.28;
			if (k <= 0)
				continue;
			string full = BootLine(i);
			int n = (int)Math.Min(full.LengthUtf8(), Math.Round(full.LengthUtf8() * Math.Min(1, k)));
			m_BootLines[i].SetText(full.SubstringUtf8(0, n));
		}
		float p = Math.Clamp((m_BootT - 0.4) / 1.4, 0, 1);
		INT_UIF_Draw.Resize(m_BootBar, Math.Max(1, 520 * p), 4);
		if (m_BootT >= 1.95)
		{
			m_BootT = -1;
			// a frame's alpha does not reach its children: remove the boot log at once (the page reveals itself)
			if (m_Boot)
				m_Boot.Unlink();
			m_Boot = null;
			EnterState(PickState());
		}
	}

	protected string BootLine(int i)
	{
		if (i == 0)
			return Widget.TranslateString("#STR_INT_PDA_BOOT_1") + " " + INT_PDA_Const.VERSION;
		if (i == 1)
			return Widget.TranslateString("#STR_INT_PDA_BOOT_2") + " " + ModuleList();
		if (i == 2)
			return Widget.TranslateString("#STR_INT_PDA_BOOT_3") + " " + BatteryPercent().ToString() + "%";
		return Widget.TranslateString("#STR_INT_PDA_BOOT_4");
	}

	protected string ModuleList()
	{
		if (!m_Pda)
			return "—";
		string s = "";
		if (m_Pda.HasGps())
			s = s + "GPS ";
		if (m_Pda.HasMap())
			s = s + Widget.TranslateString("#STR_INT_PDA_MOD_MAP") + " ";
		if (m_Pda.HasChat())
			s = s + Widget.TranslateString("#STR_INT_PDA_MOD_CHAT") + " ";
		if (m_Pda.HasGp())
			s = s + Widget.TranslateString("#STR_INT_PDA_MOD_GP");
		if (s == "")
			return "—";
		return s;
	}

	int BatteryPercent()
	{
		if (!m_Pda)
			return 100;
		return (int)Math.Round(m_Pda.BatteryLevel() * 100);
	}

	// --- states ---------------------------------------------------------------------------------------------------
	void EnterState(int st)
	{
		if (m_Closing2)
			return;
		if (m_State == st && st == ST_MAIN)
			return;
		m_State = st;
		m_AuthPage.Show(st == ST_AUTH);
		m_TrophyPage.Show(st == ST_TROPHY);
		if (st == ST_MAIN)
		{
			if (m_TabText.Count() == 0)
				BuildTabs();
			m_TabLine.Show(true);
			// Alt+L (admin PDA) starts on the admin tab
			if (INT_PDA_Client.IsAdminMode() && m_TabIds.Find(TAB_ADMIN) >= 0)
				SelectTab(TAB_ADMIN, false);
			else
				SelectTab(TAB_PROFILE, false);
		}
		else
		{
			if (m_Tab >= 0 && m_Tab < m_Pages.Count())
				m_Pages[m_Tab].Show(false);
			m_Tab = -1;
		}
		ShowTabs(st == ST_MAIN);
		if (m_HeadTitle)
			m_HeadTitle.Show(st != ST_MAIN);
		if (INT_PDA_Client.IsAdminMode())
			m_AdminFlag.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_ADMIN_MODE"));
	}

	protected void ShowTabs(bool show)
	{
		foreach (TextWidget t : m_TabText)
			t.Show(show);
		foreach (ButtonWidget h : m_TabHit)
			h.Show(show);
		foreach (Widget extra : m_TabExtras)
		{
			if (extra)
				extra.Show(show);
		}
		if (m_TabLine)
			m_TabLine.Show(show && m_Tab >= 0);
		if (!show)
		{
			foreach (Widget b : m_TabBadge)
				b.Show(false);
		}
		else
		{
			UpdateBadges();
		}
	}

	int State()
	{
		return m_State;
	}

	int ActiveTab()
	{
		return m_Tab;
	}

	INT_PDA_Page PageOf(int tab)
	{
		if (tab < 0 || tab >= m_Pages.Count())
			return null;
		return m_Pages[tab];
	}

	void SelectTab(int tab, bool sound = true)
	{
		int idx = m_TabIds.Find(tab);
		if (idx < 0)
			return;
		if (tab == m_Tab)
			return;
		int old = m_Tab;
		if (old >= 0 && old < m_Pages.Count())
			m_Pages[old].Show(false);
		m_Tab = tab;
		for (int i = 0; i < m_TabText.Count(); i++)
		{
			int col = INT_PDA_Look.DIM;
			if (m_TabIds[i] == TAB_ADMIN)
				col = INT_PDA_Look.ADMIN;
			if (i == idx)
				col = INT_PDA_Look.TEXT;
			m_TabText[i].SetColor(col);
		}
		// the orange line slides under the active word
		float lx = m_TabX[idx];
		float lw = m_TabW[idx];
		m_TabLine.Show(true);
		INT_UIF_Animator.MoveX(m_TabLine, INT_UIF_Draw.S(lx), 0.26, INT_UIF_Ease.OUT_CUBIC);
		INT_UIF_Animator.ResizeW(m_TabLine, INT_UIF_Draw.S(lw), 0.26, INT_UIF_Ease.OUT_CUBIC);
		INT_PDA_Page p = m_Pages[tab];
		p.Show(true);
		INT_UIF_Animator.Reveal(p.m_Root, 0, 10, 0.26);
		if (sound)
			INT_PDA_Sfx.Play("Tab");
	}

	protected void NextTab()
	{
		if (m_State != ST_MAIN || m_TabIds.Count() == 0)
			return;
		int idx = m_TabIds.Find(m_Tab);
		idx = idx + 1;
		if (idx >= m_TabIds.Count())
			idx = 0;
		SelectTab(m_TabIds[idx]);
	}

	// --- events ---------------------------------------------------------------------------------------------------
	bool OnHit(Widget w)
	{
		if (w == m_PowerHit)
		{
			INT_PDA_Sfx.Play("Click");
			PowerOff();
			return true;
		}
		for (int i = 0; i < m_TabHit.Count(); i++)
		{
			if (w == m_TabHit[i])
			{
				SelectTab(m_TabIds[i]);
				return true;
			}
		}
		return false;
	}

	void OnHover(Widget w, bool enter)
	{
		if (w == m_PowerHit && m_CloseX)
		{
			int c = INT_PDA_Look.TEXT;
			if (enter)
				c = INT_PDA_Look.ORANGE;
			m_CloseX.SetColor(c);
			return;
		}
		for (int i = 0; i < m_TabHit.Count(); i++)
		{
			if (w != m_TabHit[i] || m_TabIds[i] == m_Tab)
				continue;
			int col = INT_PDA_Look.DIM;
			if (enter)
				col = INT_PDA_Look.TEXT;
			else if (m_TabIds[i] == TAB_ADMIN)
				col = INT_PDA_Look.ADMIN;
			m_TabText[i].SetColor(col);
		}
	}

	protected void OnClientChanged(int what)
	{
		if (what == INT_PDA_Client.CH_AUTH)
		{
			if (INT_PDA_Client.s_LoggedIn && m_State == ST_AUTH)
				EnterState(ST_MAIN);
			else if (!INT_PDA_Client.s_LoggedIn && m_State == ST_MAIN && !INT_PDA_Client.IsAdminMode())
				EnterState(ST_AUTH);
		}
		if (what == INT_PDA_Client.CH_WELCOME && m_State == ST_MAIN && !INT_PDA_Client.s_LoggedIn && !INT_PDA_Client.IsAdminMode())
			EnterState(ST_AUTH);
		if (what == INT_PDA_Client.CH_NEWS)
		{
			// news that arrive while the news tab is open are read at once (the badge does not grow under the reader)
			INT_PDA_Page newsPage = PageOf(TAB_NEWS);
			if (m_State == ST_MAIN && m_Tab == TAB_NEWS && newsPage && newsPage.m_Active)
				INT_PDA_Client.ForgetUnread("news");
			// fresh news: the line starts again with the newest one
			m_NewsIdx = 0;
			m_NewsT = 0;
			UpdateNewsLine();
		}
		UpdateBadges();
		if (m_AuthPage && m_AuthPage.m_Active)
			m_AuthPage.OnChanged(what);
		if (m_TrophyPage && m_TrophyPage.m_Active)
			m_TrophyPage.OnChanged(what);
		foreach (INT_PDA_Page p : m_Pages)
		{
			if (p)
				p.OnChanged(what);
		}
	}

	protected void OnClientResult(int requestOp, int code)
	{
		if (m_AuthPage && m_AuthPage.m_Active)
			m_AuthPage.OnResult(requestOp, code);
		INT_PDA_Page p = PageOf(m_Tab);
		if (p)
			p.OnResult(requestOp, code);
	}

	// --- prompt ---------------------------------------------------------------------------------------------------
	// The prompt's components live in its own bag (they go with the prompt, the menu's bag does not grow).
	INT_PDA_Prompt OpenPrompt(string title, string first, string second, bool password, string valueA = "")
	{
		ClosePrompt();
		m_Prompt = new INT_PDA_Prompt();
		m_Prompt.Build(m_Overlay, INT_PDA_Look.SCR_W, INT_PDA_Look.SCR_H, title, first, second, password, valueA);
		INT_PDA_Sfx.Play("Click");
		return m_Prompt;
	}

	// Hides the prompt at once; the object is dropped on the next frame: the caller is usually the prompt's own
	// callback (Finish -> m_OnDone), and freeing it there would free its button / edit box inside their own event.
	void ClosePrompt()
	{
		if (!m_Prompt)
			return;
		if (m_Prompt.m_Root)
			m_Prompt.m_Root.Show(false);
		m_DeadPrompts.Insert(m_Prompt);
		m_Prompt = null;
	}

	protected void DropDeadPrompts()
	{
		if (m_DeadPrompts.Count() == 0)
			return;
		foreach (INT_PDA_Prompt dead : m_DeadPrompts)
		{
			if (dead)
				dead.Destroy();
		}
		m_DeadPrompts.Clear();
	}

	bool HasPrompt()
	{
		return m_Prompt && m_Prompt.m_Root && m_Prompt.m_Root.IsVisible();
	}

	// --- per frame ------------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		DropDeadPrompts();
		CheckPlayer();
		UpdateBoot(timeslice);
		m_StatusT = m_StatusT + timeslice;
		if (m_StatusT >= 1.0)
		{
			m_StatusT = 0;
			UpdateStatus();
			CheckPower();
		}
		// «Новости Зоны» line: the live news one after another every NewsRotateSeconds (admin setting)
		m_NewsT = m_NewsT + timeslice;
		if (m_NewsT >= RotateSeconds())
		{
			m_NewsT = 0;
			m_NewsIdx++;
			UpdateNewsLine();
		}
		HandleKeys();
		INT_PDA_Page p = PageOf(m_Tab);
		if (p && p.m_Active)
			p.Update(timeslice);
		if (m_AuthPage && m_AuthPage.m_Active)
			m_AuthPage.Update(timeslice);
	}

	protected void HandleKeys()
	{
		if (m_Closing2)
			return;
		// Esc: armed on press, acts on release (the vanilla pause menu must not open behind the closing PDA)
		bool esc = KeyState(KeyCode.KC_ESCAPE) != 0;
		if (esc)
		{
			m_EscArmed = true;
		}
		else if (m_EscArmed)
		{
			m_EscArmed = false;
			OnEscape();
		}
		// TAB: next field of an open prompt, else the next tab (not while typing in a page's field)
		bool tab = KeyState(KeyCode.KC_TAB) != 0;
		if (tab)
		{
			if (!m_TabArmed)
			{
				if (HasPrompt())
					m_Prompt.FocusNext();
				else if (!EditFocused())
					NextTab();
			}
			m_TabArmed = true;
		}
		else
		{
			m_TabArmed = false;
		}
	}

	protected bool EditFocused()
	{
		Widget f = GetFocus();
		if (!f)
			return false;
		if (EditBoxWidget.Cast(f))
			return true;
		if (MultilineEditBoxWidget.Cast(f))
			return true;
		return false;
	}

	protected void OnEscape()
	{
		if (INT_UIF_Dropdown.CloseActive())
			return;
		if (HasPrompt())
		{
			m_Prompt.Finish(false);
			ClosePrompt();
			return;
		}
		if (EditFocused())
		{
			SetFocus(null);
			return;
		}
		INT_PDA_Page p = PageOf(m_Tab);
		if (p && p.m_Active && p.HandleEscape())
			return;
		PowerOff();
	}

	protected void UpdateStatus()
	{
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		if (m_Clock)
			m_Clock.SetText(h.ToStringLen(2) + ":" + mi.ToStringLen(2));
		int pct = BatteryPercent();
		if (m_Battery)
		{
			if (m_Pda)
				m_Battery.SetText(pct.ToString() + "%");
			else
				m_Battery.SetText("—");
		}
		int bc = INT_PDA_Look.TEXT;
		if (m_Pda && pct < INT_PDA_Client.s_Settings.LowBatteryPercent)
			bc = INT_PDA_Look.ENEMY;
		if (m_BatteryIcon)
			m_BatteryIcon.SetColor(bc);
		array<bool> on = {false, false, false, false};
		if (m_Pda)
		{
			on[0] = m_Pda.HasGps();
			on[1] = m_Pda.HasMap();
			on[2] = m_Pda.HasChat();
			on[3] = m_Pda.HasGp();
		}
		if (INT_PDA_Client.IsAdminMode())
		{
			on[0] = true;
			on[1] = true;
			on[2] = true;
			on[3] = true;
		}
		for (int i = 0; i < m_ModText.Count(); i++)
		{
			int tc = INT_PDA_Look.DIM2;
			int bc2 = INT_PDA_Look.LINE;
			if (on[i])
			{
				tc = INT_PDA_Look.ORANGE;
				bc2 = INT_PDA_Look.ORANGE;
			}
			m_ModText[i].SetColor(tc);
			if (i < m_ModBox.Count() && m_ModBox[i])
				m_ModBox[i].SetBorder(bc2);
		}
	}

	// Every frame: the window closes when the player dies, falls unconscious or is restrained, and (outside the admin
	// mode, which needs no PDA) when the PDA is gone or no longer on this player (dropped, taken, deleted).
	protected void CheckPlayer()
	{
		if (m_Closing2)
			return;
		if (!INT_PDA_Client.PlayerCanUse())
		{
			PowerOff();
			return;
		}
		if (INT_PDA_Client.IsAdminMode())
			return;
		if (!m_Pda)
		{
			PowerOff();
			return;
		}
		Man holder = m_Pda.GetHierarchyRootPlayer();
		Man me = GetGame().GetPlayer();
		if (!holder || holder != me)
			PowerOff();
	}

	// The PDA switched off / battery dead while open: the screen goes dark (admin mode needs no PDA).
	protected void CheckPower()
	{
		if (INT_PDA_Client.IsAdminMode() || !m_Pda || m_Closing2)
			return;
		if (!m_Pda.IsWorkingPda())
		{
			// «КПК разряжен» only for a PDA without power (a broken one is not «discharged»)
			if (!m_Pda.IsRuined())
				INT_UIF_Toast.Warning("#STR_INT_PDA_TOAST_PDA_DEAD");
			PowerOff();
			return;
		}
		if (BatteryPercent() < INT_PDA_Client.s_Settings.LowBatteryPercent)
		{
			m_LowBeepT = m_LowBeepT + 1;
			if (m_LowBeepT >= 20)
			{
				m_LowBeepT = 0;
				INT_PDA_Sfx.Play("LowBat");
			}
		}
	}

	protected float RotateSeconds()
	{
		int s = 8;
		if (INT_PDA_Client.s_Settings)
			s = INT_PDA_Client.s_Settings.NewsRotateSeconds;
		if (s < 3)
			s = 3;
		return s;
	}

	// The live news (expired ones never show), newest first, then the older ones in turn; «Новостей нет» when none.
	void UpdateNewsLine()
	{
		if (!m_NewsLine)
			return;
		INT_PDA_Client.LiveNews(m_NewsLive);
		int n = m_NewsLive.Count();
		if (n == 0)
		{
			m_NewsIdx = 0;
			m_NewsLine.SetText("#STR_INT_PDA_NEWS_NONE");
			return;
		}
		if (m_NewsIdx < 0 || m_NewsIdx >= n)
			m_NewsIdx = 0;
		INT_PDA_News item = m_NewsLive[n - 1 - m_NewsIdx];
		// one line, cut with «…» to the ticker's 760 px; «1/3» in front when there are several
		string line = INT_PDA_Util.CleanLine(item.Title + " — " + item.Text, 400, 1600);
		if (n > 1)
		{
			int no = m_NewsIdx + 1;
			line = no.ToString() + "/" + n.ToString() + "  " + line;
		}
		m_NewsLine.SetText(INT_PDA_Look.Fit(line, 756, 14));
	}

	void UpdateBadges()
	{
		for (int i = 0; i < m_TabIds.Count() && i < m_TabBadge.Count(); i++)
		{
			int n = 0;
			int id = m_TabIds[i];
			if (id == TAB_CHAT)
				n = INT_PDA_Client.Unread("public") + INT_PDA_Client.Unread("faction") + INT_PDA_Client.UnreadPrivate();
			else if (id == TAB_NEWS)
				n = INT_PDA_Client.Unread("news");
			bool show = n > 0 && m_State == ST_MAIN;
			m_TabBadge[i].Show(show);
			if (!show)
				continue;
			string txt = n.ToString();
			if (n > 99)
				txt = "99";
			m_TabBadgeText[i].SetText(txt);
		}
	}

	override void OnTestCommand(string command)
	{
		if (command.IndexOf("tab ") == 0)
		{
			SelectTab(command.Substring(4, command.Length() - 4).ToInt());
			return;
		}
		if (m_State == ST_AUTH && m_AuthPage)
		{
			m_AuthPage.OnTestCommand(command);
			return;
		}
		INT_PDA_Page p = PageOf(m_Tab);
		if (p)
			p.OnTestCommand(command);
	}
}

// «КПК сталкера <позывной>» — somebody else's PDA (owner 2026-10-23: a looted PDA is a trophy only).
class INT_PDA_PageTrophy : INT_PDA_Page
{
	protected TextWidget m_Name;

	override void OnBuild()
	{
		float cx = m_W * 0.5;
		INT_PDA_Look.DrawIcon(m_Root, cx - 32, 150, 64, "lock", INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(m_Root, 0, 240, m_W, 30, INT_PDA_Look.Caps("#STR_INT_PDA_TROPHY_TITLE"), 22, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		m_Name = INT_UIF_Draw.Text(m_Root, 0, 280, m_W, 70, "", 56, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(m_Root, 0, 370, m_W, 26, "#STR_INT_PDA_TROPHY_TEXT", 17, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
	}

	override void OnShow()
	{
		Refresh();
		if (m_Menu && m_Menu.m_Pda)
			INT_PDA_Client.RequestOwner(m_Menu.m_Pda);
	}

	override void OnChanged(int what)
	{
		if (what == INT_PDA_Client.CH_OWNER)
			Refresh();
	}

	protected void Refresh()
	{
		string name = "";
		if (m_Menu)
			name = INT_PDA_Client.OwnerName(m_Menu.m_Pda);
		if (name == "")
			name = "…";
		m_Name.SetText(name);
	}
}
