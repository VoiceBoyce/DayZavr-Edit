// Оболочка планшета: корпус, боковое меню, верхняя строка, рабочая область. Размеры из макета:
// корпус 1540x920 с левым верхним углом 190,80; экран 1468x848; меню 240; верхняя строка 72.
class VB_TabletNavItem extends Managed
{
	string NavId;
	string NavTitle;	// заголовок в верхней строке
	string NavLabel;	// подпись в меню
	string NavIcon;
	Widget NavHit;
	ref INT_UIF_Surface NavBg;

	void VB_TabletNavItem(string id, string title, string label, string icon)
	{
		NavId = id;
		NavTitle = title;
		NavLabel = label;
		NavIcon = icon;
	}
}

class VB_TabletMenu extends INT_UIF_Menu
{
	static const float DEVICE_X = 190;
	static const float DEVICE_Y = 80;
	static const float SCREEN_X = 226;
	static const float SCREEN_Y = 116;
	static const float ATM_REACH = 4.0;

	protected static bool s_AssetsLoaded;
	protected static string s_StartPage = "profile";
	protected static bool s_AtmOnly;

	protected Widget m_Device;
	protected Widget m_NavRoot;
	protected Widget m_Work;
	protected Widget m_CloseHit2;
	protected TextWidget m_TopTitle;
	protected TextWidget m_ClockText;
	protected TextWidget m_ServerText;
	protected TextWidget m_SubText;
	protected ImageWidget m_CloseIcon2;
	protected ref array<ref VB_TabletNavItem> m_Items;
	protected ref array<ref INT_UIF_Surface> m_Surfaces;
	protected ref array<ref INT_UIF_Surface> m_NavSurfaces;
	protected ref VB_TabletPage m_Page;
	protected string m_PageId;
	protected string m_PendingPage;
	protected float m_Second = 1;
	protected bool m_NavAdmin;
	protected string m_NavKey;
	protected bool m_ServerTextShown;
	protected int m_DataVersion;
	protected Widget m_TopBar;
#ifdef DIAG_DEVELOPER
	protected static ref array<VBT_Input> s_DiagInputs = new array<VBT_Input>();

	static void DiagWatch(VBT_Input field)
	{
		s_DiagInputs.Insert(field);
	}
#endif
	protected Widget m_Screen;
	protected Widget m_SideRoot;
	protected Widget m_ScreenBg;
	protected ImageWidget m_DeviceTex;
	protected ImageWidget m_Glass;
	protected bool m_AtmShown;
	protected ref array<ImageWidget> m_TintIcons;
	protected bool m_Typing;
	protected int m_ResultVersion;
	protected ref VBT_Dialog m_Dialog;
	protected ref VBT_Dialog m_DialogDone;
	protected ref array<Widget> m_Trash = new array<Widget>();

	// --- открытие и закрытие ---------------------------------------------------------------
	static bool CanOpen()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.IsAlive())
			return false;
		if (player.IsUnconscious() || player.IsRestrained())
			return false;
		return true;
	}

	// Клавиша: открыть на экране page, повторно закрыть. "" значит экран по умолчанию.
	static void Toggle(string page)
	{
		// Пока вводится текст, клавиши планшета это просто буквы.
		Widget focus = GetFocus();
		if (focus && (EditBoxWidget.Cast(focus) || MultilineEditBoxWidget.Cast(focus)))
			return;
		UIScriptedMenu current = GetGame().GetUIManager().GetMenu();
		VB_TabletMenu ours = VB_TabletMenu.Cast(current);
		if (ours)
		{
			if (page == "" || page == ours.GetPageId() || s_AtmOnly)
				ours.RequestClose();
			else
				ours.Go(page);
			return;
		}
		if (current || !CanOpen())
			return;
		s_StartPage = page;
		if (s_StartPage == "")
			s_StartPage = "profile";
		VB_TabletClient.Get().RequestHello();
		INT_UIF_Menu.Open(VB_TABLET_MENU_ID);
	}

	// Действие у банкомата: только его окошко, без планшета.
	static void OpenAtm()
	{
		UIScriptedMenu current = GetGame().GetUIManager().GetMenu();
		VB_TabletMenu ours = VB_TabletMenu.Cast(current);
		if (ours)
		{
			ours.Go("bank");
			return;
		}
		if (current || !CanOpen())
			return;
		s_AtmOnly = true;
		Toggle("bank");
	}

	bool IsAtmOnly()
	{
		return s_AtmOnly;
	}

	string GetPageId()
	{
		return m_PageId;
	}

	// --- настройка окна фреймворка: без его рамки, персонаж ходит, мир не размыт --------------
	override bool ShowWindow()
	{
		return false;
	}

	override bool FreezesPlayer()
	{
		return false;
	}

	// Клавиатура нужна только пока вводится текст, иначе персонаж ходит.
	override bool WantsKeyboard()
	{
		return m_Typing;
	}

	void SetTyping(bool on)
	{
		if (m_Typing == on)
			return;
		m_Typing = on;
		Mission mission = GetGame().GetMission();
		if (!mission)
			return;
		if (on)
		{
			mission.PlayerControlDisable(INPUT_EXCLUDE_ALL);
			GetGame().GetInput().ChangeGameFocus(1);
		}
		else
		{
			mission.PlayerControlEnable(false);
			GetGame().GetInput().ChangeGameFocus(-1);
		}
	}

	Widget GetScreen()
	{
		return m_Screen;
	}

	// Виджет удалится в следующем кадре: из его же обработчика удалять нельзя.
	void Trash(Widget w)
	{
		if (w)
			m_Trash.Insert(w);
	}

	void InputChanged(VBT_Input field)
	{
		if (m_Dialog && m_Dialog.Field == field)
		{
			m_Dialog.OnField();
			return;
		}
		if (m_Page)
			m_Page.OnInput(field);
	}

	void InputSubmitted(VBT_Input field)
	{
		if (m_Dialog && m_Dialog.Field == field)
			return;
		if (m_Page)
			m_Page.OnSubmit(field);
	}

	void AreaChanged(VBT_Area field)
	{
		if (m_Page)
			m_Page.OnArea(field);
	}

	void DropdownPicked(VBT_Dropdown list, int index)
	{
		if (m_Page)
			m_Page.OnPick(list, index);
	}

	void Ask(int tag, string title, string text, string yes, bool danger = false, float h = 220)
	{
		m_Dialog = new VBT_Dialog(this, tag);
		m_Dialog.BuildAsk(title, text, yes, danger, h);
	}

	void Prompt(int tag, string title, string hint, string value, int maxLen = 40)
	{
		m_Dialog = new VBT_Dialog(this, tag);
		m_Dialog.BuildPrompt(title, hint, value, maxLen);
	}

	void ConfirmWord(int tag, string title, string text, string word, string yes)
	{
		m_Dialog = new VBT_Dialog(this, tag);
		m_Dialog.BuildConfirm(title, text, word, yes);
	}

	// На карте Esc сначала отменяет рисование или закрывает карточку объекта.
	override bool CloseOnEscape()
	{
		VB_TabletPageMap p = VB_TabletPageMap.Cast(m_Page);
		return !p || !p.HoldEscape();
	}

	override bool UsesBlur()
	{
		return false;
	}

	override Widget Init()
	{
		if (!s_AssetsLoaded)
		{
			s_AssetsLoaded = true;
			LoadWidgetImageSet(VB_TABLET_IMAGESET);
			LoadWidgetImageSet(VB_TABLET_IMAGESET_EM);
		}
		return super.Init();
	}

	override void OnBuild(Widget body)
	{
		m_Items = new array<ref VB_TabletNavItem>();
		m_Surfaces = new array<ref INT_UIF_Surface>();
		m_NavSurfaces = new array<ref INT_UIF_Surface>();
		m_ResultVersion = VB_TabletClient.Get().ResultVersion;
		m_DataVersion = VB_TabletClient.Get().DataVersion;

		// Холст 1920x1080 по центру экрана: на широких мониторах по бокам остаётся мир.
		float f = INT_UIF_Scale.Factor();
		float ox = (INT_UIF_Scale.ScreenW() / f - 1920) * 0.5;
		float oy = (INT_UIF_Scale.ScreenH() / f - 1080) * 0.5;
		INT_UIF_Draw.Place(body, ox, oy, 1920, 1080);

		m_Device = INT_UIF_Draw.Frame(body, 0, 0, 1920, 1080, "VBT_Device");
		m_TintIcons = new array<ImageWidget>();
		m_DeviceTex = INT_UIF_Draw.Texture(m_Device, DEVICE_X, DEVICE_Y, 1540, 920, VB_TABLET_TEX_DEVICE, VB_C_WHITE);
		Widget screen = INT_UIF_Draw.Frame(m_Device, SCREEN_X, SCREEN_Y, 1468, 848, "VBT_Screen");
		m_Screen = screen;
		m_ScreenBg = INT_UIF_Draw.Frame(screen, 0, 0, 1468, 848);
		m_Surfaces.Insert(INT_UIF_Surface.Box(m_ScreenBg, 0, 0, 1468, 848, 10, VB_C_BG));
		BuildSide(screen);
		BuildTop(screen);
		m_Work = INT_UIF_Draw.Clip(screen, 240, 72, 1228, 776, "VBT_Work");
		m_Glass = INT_UIF_Draw.Texture(m_Device, SCREEN_X, SCREEN_Y, 1468, 848, VB_TABLET_TEX_GLASS, VB_C_WHITE);
		if (s_AtmOnly)
		{
			m_DeviceTex.Show(false);
			m_Glass.Show(false);
			m_ScreenBg.Show(false);
			m_SideRoot.Show(false);
			m_TopBar.Show(false);
			m_Work.Show(false);
		}
		RevealOnOpen(m_Device);
		if (VB_TabletClient.Get().Clan)
			VB_TabletClient.Get().Request(VB_TabletOp.CONTRACTS);
		BuildNavItems();
		Go(s_StartPage);
	}

	protected void BuildSide(Widget screen)
	{
		Widget side = INT_UIF_Draw.Frame(screen, 0, 0, 240, 848, "VBT_Side");
		m_SideRoot = side;
		m_Surfaces.Insert(INT_UIF_Surface.Box(side, 0, 0, 240, 848, 10, VB_C_SIDE));
		INT_UIF_Draw.Line(side, 239, 0, 1, 848, VB_C_LINE);
		m_Surfaces.Insert(INT_UIF_Surface.Box(side, 18, 20, 44, 44, 10, VB_C_PANEL2, VB_C_LINE2));
		m_TintIcons.Insert(INT_UIF_Draw.Texture(side, 29, 31, 22, 22, VB_TABLET_ICON + "signal", VB_C_ACCENT));
		m_ServerText = INT_UIF_Draw.Text(side, 74, 20, 156, 24, "", 14, VB_C_TEXT);
		m_SubText = INT_UIF_Draw.Text(side, 74, 42, 156, 20, "#STR_VB_TAB_DEVICE", 13, VB_C_DIM2);
		m_NavRoot = INT_UIF_Draw.Frame(side, 0, 84, 240, 760, "VBT_Nav");
	}

	protected void BuildTop(Widget screen)
	{
		Widget top = INT_UIF_Draw.Frame(screen, 240, 0, 1228, 72, "VBT_Top");
		m_TopBar = top;
		INT_UIF_Draw.Line(top, 0, 71, 1228, 1, VB_C_LINE);
		m_TopTitle = INT_UIF_Draw.Text(top, 28, 0, 700, 72, "", 26, VB_C_TEXT);
		m_ClockText = INT_UIF_Draw.Text(top, 1000, 0, 70, 72, "00:00", 20, VB_C_TEXT, INT_UIF_Align.RIGHT);
		m_TintIcons.Insert(INT_UIF_Draw.Texture(top, 1086, 26, 20, 20, VB_TABLET_ICON + "signal", VB_C_ACCENT));
		m_TintIcons.Insert(INT_UIF_Draw.Texture(top, 1118, 26, 20, 20, VB_TABLET_ICON + "battery", VB_C_ACCENT));
		m_Surfaces.Insert(INT_UIF_Surface.Box(top, 1164, 16, 40, 40, 8, VB_C_PANEL2, VB_C_LINE2));
		m_CloseIcon2 = INT_UIF_Draw.Texture(top, 1174, 26, 20, 20, VB_TABLET_ICON + "close", VB_C_DIM);
		m_CloseHit2 = INT_UIF_Draw.Hit(top, 1164, 16, 40, 40, m_Handler);
		m_CloseHit2.SetName("VBT_Close");
	}

	protected void BuildNavItems()
	{
		m_Items.Clear();
		m_Items.Insert(new VB_TabletNavItem("profile", "#STR_VB_TAB_PROFILE", "#STR_VB_TAB_PROFILE", "user"));
		m_Items.Insert(new VB_TabletNavItem("rank", "#STR_VB_TAB_RANK", "#STR_VB_TAB_RANK", "rank"));
		m_Items.Insert(new VB_TabletNavItem("map", "#STR_VB_TAB_MAP", "#STR_VB_TAB_MAP", "map"));
		m_Items.Insert(new VB_TabletNavItem("rating", "#STR_VB_TAB_RATING", "#STR_VB_TAB_RATING", "trophy"));
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.Clan)
		{
			m_Items.Insert(new VB_TabletNavItem("clan", "#STR_VB_TAB_CLAN_PAGE", "#STR_VB_TAB_CLAN_PAGE", "flag"));
			m_Items.Insert(new VB_TabletNavItem("cmembers", "#STR_VB_TAB_MEMBERS", "#STR_VB_TAB_MEMBERS", "users"));
			m_Items.Insert(new VB_TabletNavItem("cchat", "#STR_VB_TAB_CHAT_TITLE", "#STR_VB_TAB_CHAT", "chat"));
			m_Items.Insert(new VB_TabletNavItem("cjournal", "#STR_VB_TAB_JOURNAL_TITLE", "#STR_VB_TAB_JOURNAL", "clock"));
			m_Items.Insert(new VB_TabletNavItem("cnews", "#STR_VB_TAB_NEWS_TITLE", "#STR_VB_TAB_NEWS", "news"));
			m_Items.Insert(new VB_TabletNavItem("cranks", "#STR_VB_TAB_RANKS", "#STR_VB_TAB_RANKS", "key"));
			m_Items.Insert(new VB_TabletNavItem("contracts", "#STR_VB_TAB_CONTRACTS", "#STR_VB_TAB_CONTRACTS", "contract"));
			m_Items.Insert(new VB_TabletNavItem("bank", "#STR_VB_TAB_BANK", "#STR_VB_TAB_BANK", "vault"));
			if (client.Can(VB_ClanPerm.SETTINGS))
				m_Items.Insert(new VB_TabletNavItem("csettings", "#STR_VB_TAB_SETTINGS_TITLE", "#STR_VB_TAB_SETTINGS", "sym_cog"));
		}
		else
		{
			m_Items.Insert(new VB_TabletNavItem("create", "#STR_VB_TAB_CREATE_TITLE", "#STR_VB_TAB_CREATE", "plusclan"));
		}
		m_NavAdmin = client.IsAdmin;
		m_NavKey = NavKey();
		foreach (ImageWidget tint : m_TintIcons)
			tint.SetColor(VB_C_ACCENT);
		if (m_NavAdmin)
			m_Items.Insert(new VB_TabletNavItem("admin", "#STR_VB_TAB_ADMIN", "#STR_VB_TAB_ADMIN", "admin"));
	}

	// Меню зависит от клана, права настроек и админа. Ключ меняется, значит пора собрать заново.
	protected string NavKey()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		string key = client.IsAdmin.ToString();
		if (client.Clan)
			key = key + client.Clan.Title + client.Can(VB_ClanPerm.SETTINGS).ToString() + client.Clan.Color.ToString();
		return key;
	}

	protected string ResolvePage(string id)
	{
		if (FindItem(id))
			return id;
		if (VB_TabletClient.Get().Clan)
		{
			if (id == "create" || id == "csettings")
				return "clan";
		}
		else if (id == "clan" || id == "cmembers" || id == "cchat" || id == "cjournal" || id == "cnews" || id == "cranks" || id == "contracts" || id == "bank" || id == "csettings")
		{
			return "create";
		}
		return "profile";
	}

	protected void PaintNav()
	{
		INT_UIF_Widgets.ClearChildren(m_NavRoot);
		m_NavSurfaces.Clear();
		float y = 0;
		int n = m_Items.Count();
		for (int i = 0; i < n; i++)
		{
			VB_TabletNavItem item = m_Items.Get(i);
			if (item.NavId == "clan")
			{
				y = y + 14;
				INT_UIF_Draw.Text(m_NavRoot, 30, y, 190, 18, VBT.Fit(VB_TabletClient.Get().Clan.Title, 190, 13), 13, VB_C_DIM2);
				y = y + 26;
			}
			if (item.NavId == "admin")
			{
				y = y + 14;
				INT_UIF_Draw.Text(m_NavRoot, 30, y, 190, 18, "#STR_VB_TAB_ADMIN_GROUP", 13, VB_C_DIM2);
				y = y + 26;
			}
			bool on = item.NavId == m_PageId;
			int fill = 0;
			if (on)
				fill = VB_C_PANEL2;
			item.NavBg = INT_UIF_Surface.Box(m_NavRoot, 14, y, 212, 44, 8, fill);
			m_NavSurfaces.Insert(item.NavBg);
			int iconColor = VB_C_DIM;
			int textColor = VB_C_DIM;
			if (on)
			{
				m_NavSurfaces.Insert(INT_UIF_Surface.Box(m_NavRoot, 14, y + 10, 3, 24, 2, VB_C_ACCENT));
				iconColor = VB_C_ACCENT;
				textColor = VB_C_TEXT;
			}
			INT_UIF_Draw.Texture(m_NavRoot, 30, y + 11, 22, 22, VB_TABLET_ICON + item.NavIcon, iconColor);
			INT_UIF_Draw.Text(m_NavRoot, 66, y, 156, 44, item.NavLabel, 16, textColor);
			int badge = NavBadge(item.NavId);
			if (badge > 0)
			{
				m_NavSurfaces.Insert(INT_UIF_Surface.Box(m_NavRoot, 186, y + 11, 24, 22, 11, VB_C_ACCENT));
				INT_UIF_Draw.Text(m_NavRoot, 186, y + 11, 24, 22, badge.ToString(), 13, VB_C_ON, INT_UIF_Align.CENTER);
			}
			else if (item.NavId == "bank" && TaxLow())
			{
				m_NavSurfaces.Insert(INT_UIF_Surface.Box(m_NavRoot, 186, y + 11, 24, 22, 11, VB_C_BAD));
				INT_UIF_Draw.Text(m_NavRoot, 186, y + 11, 24, 22, "!", 13, VB_C_ON, INT_UIF_Align.CENTER);
			}
			item.NavHit = INT_UIF_Draw.Hit(m_NavRoot, 14, y, 212, 44, m_Handler);
			item.NavHit.SetName("VBT_Nav_" + item.NavId);
			y = y + 48;
		}
	}

	protected int NavBadge(string id)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (id == "profile")
			return client.Invites.Count();
		VB_Clan c = client.Clan;
		if (!c)
			return 0;
		if (id == "cmembers" && client.Can(VB_ClanPerm.INVITE))
			return c.Requests.Count();
		if (id == "cchat")
			return Unread("chat", c.ChatTotal, id);
		if (id == "cnews")
			return Unread("news", c.NewsTotal, id);
		if (id == "contracts")
		{
			int ready = 0;
			array<ref VB_ClanContract> mine = client.MyContracts;
			foreach (VB_ClanContract k : mine)
			{
				if (k.Stage == VB_ContractStage.READY)
					ready++;
			}
			return ready;
		}
		return 0;
	}

	// Прочитанное хранится в профиле игры: значок не вернётся после перезахода.
	protected int Unread(string kind, int total, string page)
	{
		string key = "vbt_" + kind + "_" + VB_TabletClient.Get().Clan.Id;
		if (page == m_PageId)
		{
			GetGame().SetProfileString(key, total.ToString());
			return 0;
		}
		string seen;
		GetGame().GetProfileString(key, seen);
		return Math.Max(0, total - seen.ToInt());
	}

	protected bool TaxLow()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c || c.TaxFree > 0 || client.ClanTax <= 0)
			return false;
		return c.Bank / client.ClanTax <= 7;
	}

	// --- экраны ------------------------------------------------------------------------------
	void Go(string id)
	{
		id = ResolvePage(id);
		if (m_Page)
			m_Page.Leave();
		m_Page = null;
		bool changed = id != m_PageId;
		m_PageId = id;
		VB_TabletNavItem item = FindItem(id);
		m_TopTitle.SetText(item.NavTitle);
		PaintNav();
		INT_UIF_Widgets.ClearChildren(m_Work);
		Widget root = INT_UIF_Draw.Frame(m_Work, 28, 28, 1172, 720, "VBT_Page");
		m_Page = CreatePage(item);
		m_Page.Attach(this, root);
		m_Page.Build();
		if (changed)
			INT_UIF_Animator.Reveal(root, 0, 10, 0.26);
	}

	protected VB_TabletPage CreatePage(VB_TabletNavItem item)
	{
		if (item.NavId == "profile")
			return new VB_TabletPageProfile();
		if (item.NavId == "rank")
			return new VB_TabletPageRank();
		if (item.NavId == "rating")
			return new VB_TabletPageRating();
		if (item.NavId == "create")
			return new VB_TabletPageCreate();
		if (item.NavId == "clan")
			return new VB_TabletPageClan();
		if (item.NavId == "cmembers")
			return new VB_TabletPageMembers();
		if (item.NavId == "cchat")
			return new VB_TabletPageChat();
		if (item.NavId == "cjournal")
			return new VB_TabletPageJournal();
		if (item.NavId == "cnews")
			return new VB_TabletPageNews();
		if (item.NavId == "cranks")
			return new VB_TabletPageRanks();
		if (item.NavId == "contracts")
			return new VB_TabletPageContracts();
		if (item.NavId == "bank")
			return new VB_TabletPageBank();
		if (item.NavId == "csettings")
			return new VB_TabletPageSettings();
		if (item.NavId == "admin")
			return new VB_TabletPageAdmin();
		if (item.NavId == "map")
			return new VB_TabletPageMap();
		return new VB_TabletPageStub(item.NavIcon);
	}

	// Перерисовка текущего экрана в следующем кадре: кликнутая кнопка ещё жива.
	void RequestRepaint()
	{
		m_PendingPage = m_PageId;
	}

	void GoLater(string id)
	{
		m_PendingPage = id;
	}

	Widget GetTopBar()
	{
		return m_TopBar;
	}

	ScriptedWidgetEventHandler GetHitHandler()
	{
		return m_Handler;
	}

	override void OnShow()
	{
		super.OnShow();
		HoldHud(true);
		VB_TabletClient.SetAimLock(true);
	}

	override void OnHide()
	{
		SetFocus(null);
		SetTyping(false);
		VB_TabletClient.SetAimLock(false);
		if (m_Page)
			m_Page.Leave();
		s_AtmOnly = false;
		GetGame().SaveProfile();
		super.OnHide();
		HoldHud(false);
	}

	// Клики dayz-mcp приходят сюда, настоящая мышь идёт через обработчик кнопки в OnFrameClick.
	override bool OnClick(Widget w, int x, int y, int button)
	{
		return OnFrameClick(w, button);
	}

	protected VB_TabletNavItem FindItem(string id)
	{
		foreach (VB_TabletNavItem item : m_Items)
		{
			if (item.NavId == id)
				return item;
		}
		return null;
	}

	// --- события ------------------------------------------------------------------------------
	override bool OnFrameClick(Widget w, int button)
	{
		if (w && w == m_CloseHit2)
		{
			RequestClose();
			return true;
		}
		if (m_Dialog)
		{
			bool answered;
			bool result;
			if (m_Dialog.OnHit(w, answered, result))
			{
				if (answered)
				{
					string text = "";
					if (m_Dialog.Field)
						text = m_Dialog.Field.Get();
					int tag = m_Dialog.Tag;
					m_Dialog.Modal.Close();
					m_DialogDone = m_Dialog;
					m_Dialog = null;
					if (m_Page)
						m_Page.OnAnswer(tag, result, text);
				}
			}
			return true;
		}
		if (m_Page && m_Page.OnHit(w))
			return true;
		foreach (VB_TabletNavItem item : m_Items)
		{
			if (w && w == item.NavHit)
			{
				// Виджет кнопки пересоздаётся при смене экрана, поэтому переход в следующем кадре.
				if (item.NavId != m_PageId)
					m_PendingPage = item.NavId;
				return true;
			}
		}
		return super.OnFrameClick(w, button);
	}

	override bool OnFrameHover(Widget w, bool enter)
	{
		if (w && w == m_CloseHit2)
		{
			int col = VB_C_DIM;
			if (enter)
				col = VB_C_TEXT;
			m_CloseIcon2.SetColor(col);
			return true;
		}
		if (m_Page && m_Page.OnHover(w, enter))
			return true;
		foreach (VB_TabletNavItem item : m_Items)
		{
			if (w && w == item.NavHit)
			{
				if (item.NavId != m_PageId)
				{
					int fill = 0;
					if (enter)
						fill = VB_C_PANEL;
					item.NavBg.SetFill(fill);
				}
				return true;
			}
		}
		return super.OnFrameHover(w, enter);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (IsClosingDown())
			return;
		if (!CanOpen())
		{
			RequestClose();
			return;
		}
		foreach (Widget dead : m_Trash)
		{
			if (dead)
				dead.Unlink();
		}
		m_Trash.Clear();
		m_DialogDone = null;
#ifdef DIAG_DEVELOPER
		for (int di = s_DiagInputs.Count() - 1; di >= 0; di--)
		{
			VBT_Input watched = s_DiagInputs.Get(di);
			if (watched)
				watched.DiagPoll();
			else
				s_DiagInputs.Remove(di);
		}
#endif
		if (m_PendingPage != "")
		{
			string next = m_PendingPage;
			m_PendingPage = "";
			Go(next);
		}
		VB_TabletClient session = VB_TabletClient.Get();
		if (session.DataVersion != m_DataVersion)
		{
			m_DataVersion = session.DataVersion;
			if (NavKey() != m_NavKey)
			{
				BuildNavItems();
				Go(m_PageId);
			}
			else if (!m_Page || !m_Page.OnData())
			{
				Go(m_PageId);
			}
			else
			{
				PaintNav();
			}
		}
		if (session.ResultVersion != m_ResultVersion)
		{
			m_ResultVersion = session.ResultVersion;
			int code = session.LastResult;
			if (VBK.ResultIsGood(code))
				INT_UIF_Toast.Success(VBT.T(VBK.ResultText(code)));
			else
				INT_UIF_Toast.Error(VBT.T(VBK.ResultText(code)));
			if (m_Page)
				m_Page.OnResult(code);
		}
		if (m_Page)
			m_Page.Tick(timeslice);
		if (s_AtmOnly)
			AtmOnlyTick();
		m_Second = m_Second + timeslice;
		if (m_Second >= 1)
		{
			m_Second = 0;
			Second();
		}
	}

	// Окно банкомата закрылось или игрок отошёл: закрывается всё.
	protected void AtmOnlyTick()
	{
		VB_TabletPageBank bank = VB_TabletPageBank.Cast(m_Page);
		if (!bank)
			return;
		if (bank.AtmIsOpen())
			m_AtmShown = true;
		else if (m_AtmShown)
			RequestClose();
		VB_Clan c = VB_TabletClient.Get().Clan;
		Man player = GetGame().GetPlayer();
		if (!c || !player)
			return;
		vector at = player.GetPosition();
		float dx = at[0] - c.AtmX;
		float dz = at[2] - c.AtmZ;
		if (dx * dx + dz * dz > ATM_REACH * ATM_REACH)
			RequestClose();
	}

	protected void Second()
	{
		int h;
		int m;
		int s;
		GetHourMinuteSecond(h, m, s);
		string mm = m.ToString();
		if (m < 10)
			mm = "0" + mm;
		string hh = h.ToString();
		if (h < 10)
			hh = "0" + hh;
		m_ClockText.SetText(hh + ":" + mm);

		VB_TabletClient client = VB_TabletClient.Get();
		client.RequestHello();
		if (client.HasHello && !m_ServerTextShown)
		{
			m_ServerTextShown = true;
			m_ServerText.SetText(client.ServerName);
			m_SubText.SetText("#STR_VB_TAB_DEVICE");
			m_SubText.SetColor(VB_C_DIM2);
			if (client.IsAdmin != m_NavAdmin)
			{
				BuildNavItems();
				PaintNav();
			}
		}
		else if (!client.HasHello && client.IsServerMissing())
		{
			m_SubText.SetText("#STR_VB_TAB_NO_SERVER");
			m_SubText.SetColor(VB_C_BAD);
		}
	}
}
