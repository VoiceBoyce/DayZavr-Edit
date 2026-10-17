// ============================================================================
// INT_UIFramework - demo / design-system showcase (v2).
// Every v2 component, on two tabs: "Overview" (buttons, chips, data, list)
// and "Forms" (toggles, checkboxes, steppers, inputs, dropdowns, segmented
// control, sliders) with a live preview column wired to the controls.
// Also the reference for writing a consumer menu: subclass, GetWindowSize,
// OnBuildHeader (tabs) -> OnBuild(body) in design px, components from
// INT_UIF_Draw + factories, events through ScriptInvokers, panels registered
// with RevealOnOpen for the entrance cascade.
// Open: Ctrl+F10 (UAIntUifDemo) or INT_UIF_Menu.Open(INT_UIF_MENU_DEMO).
// ============================================================================
class INT_UIF_DemoMenu extends INT_UIF_Menu
{
	protected INT_UIF_Tabs m_Tabs;
	protected ref array<Widget> m_OverviewCards;
	protected ref array<Widget> m_FormCards;

	// overview
	protected INT_UIF_Ring m_Ring;
	protected INT_UIF_ProgressBar m_BarA;
	protected INT_UIF_ProgressBar m_BarB;
	protected INT_UIF_ProgressBar m_BarC;
	protected INT_UIF_StatTile m_TileA;
	protected INT_UIF_StatTile m_TileB;
	protected INT_UIF_List m_List;
	protected int m_PendingRemove = -1;
	protected ref array<string> m_Names;
	protected ref array<string> m_Roles;

	// forms
	protected INT_UIF_Toggle m_TgProtect;
	protected INT_UIF_Toggle m_TgNotify;
	protected INT_UIF_Toggle m_TgSound;
	protected INT_UIF_Checkbox m_CbHints;
	protected INT_UIF_Checkbox m_CbCompact;
	protected INT_UIF_Checkbox m_CbAnim;
	protected INT_UIF_Stepper m_StMembers;
	protected INT_UIF_Stepper m_StTerminals;
	protected INT_UIF_TextInput m_InName;
	protected INT_UIF_TextInput m_InSearch;
	protected TextWidget m_NameCount;
	protected INT_UIF_Dropdown m_DdMode;
	protected INT_UIF_Dropdown m_DdZone;
	protected INT_UIF_Tabs m_SegPeriod;
	protected INT_UIF_Tabs m_UlTabs;
	protected INT_UIF_Slider m_SlRadius;
	protected INT_UIF_Slider m_SlDelay;
	protected INT_UIF_Slider m_SlOpacity;

	// live preview
	protected INT_UIF_Ring m_PvRing;
	protected TextWidget m_PvName;
	protected TextWidget m_PvMode;
	protected TextWidget m_PvZone;
	protected INT_UIF_Badge m_PvBadge;
	protected INT_UIF_StatTile m_PvMembers;
	protected INT_UIF_StatTile m_PvArea;

	static const int NAME_MAX = 24;

	void INT_UIF_DemoMenu()
	{
		m_OverviewCards = new array<Widget>();
		m_FormCards = new array<Widget>();
		m_Names = new array<string>();
		m_Roles = new array<string>();
		m_Names.Insert("Intenso");
		m_Roles.Insert("owner");
		m_Names.Insert("Serega");
		m_Roles.Insert("admin");
		m_Names.Insert("Kovboy");
		m_Roles.Insert("member");
		m_Names.Insert("Bars");
		m_Roles.Insert("member");
		m_Names.Insert("Sobr");
		m_Roles.Insert("nearby");
		m_Names.Insert("Lisa");
		m_Roles.Insert("nearby");
	}

	override string GetTitle()
	{
		return "#STR_INT_UIF_DEMO_TITLE";
	}

	override string GetSubtitle()
	{
		return "#STR_INT_UIF_DEMO_SUB";
	}

	override string GetIcon()
	{
		return "bolt";
	}

	override string GetIconHue()
	{
		return INT_UIF_ThemeKeys.TEAL;
	}

	override void GetWindowSize(out float w, out float h)
	{
		w = 1480;
		h = 820;
	}

	// The forms tab has text fields: take the keyboard while open.
	override bool WantsKeyboard()
	{
		return true;
	}

	override void OnBuildHeader(Widget center, Widget right)
	{
		float slot = GetHeaderSlotW();
		m_Tabs = INT_UIF_Tabs.Create(m_Bag, center, (slot - 320) * 0.5, 20, 320, 36);

		string ver = "v" + INT_UIF_VERSION;
		float bw = INT_UIF_Badge.FitWidth(ver, 32, "star");
		INT_UIF_Badge.Create(m_Bag, right, slot - bw, 22, ver, INT_UIF_Variant.WARNING, 32, "star", bw);
		// Pill width follows the translated text: dot + 30 px inset + text + 14.
		float sw = INT_UIF_Draw.TextWidth("#STR_INT_UIF_DEMO_LIVE", 13) * 1.05 + 44;
		Widget pill = INT_UIF_Draw.Frame(right, slot - bw - 10 - sw, 22, sw, 32, "Status");
		INT_UIF_Surface.Box(pill, 0, 0, sw, 32, 10, INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 26), INT_UIF.Soft(INT_UIF_ThemeKeys.TEAL, 76));
		INT_UIF_Draw.Dot(pill, 14, 12, 8, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL), true);
		INT_UIF_Draw.Text(pill, 30, 0, sw - 36, 32, "#STR_INT_UIF_DEMO_LIVE", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
	}

	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		float gap = 14;
		float cw = (bw - 2 * gap) / 3.0;
		if (!m_Tabs)
			return;
		m_Tabs.SetPageArea(body, 0, 0, bw, bh);
		m_Tabs.AddTab("#STR_INT_UIF_TAB_OVERVIEW", "home");
		m_Tabs.AddTab("#STR_INT_UIF_TAB_FORMS", "list");
		m_Tabs.OnChanged().Insert(OnTabChanged);

		Widget overview = m_Tabs.GetPage(0);
		BuildControls(overview, 0, cw, bh);
		BuildData(overview, cw + gap, cw, bh);
		BuildPeople(overview, 2 * (cw + gap), cw, bh);

		Widget forms = m_Tabs.GetPage(1);
		BuildSwitches(forms, 0, cw, bh);
		BuildFields(forms, cw + gap, cw, bh);
		BuildPreview(forms, 2 * (cw + gap), cw, bh);

		AddHint("#STR_INT_UIF_KEY_DBLCLICK", "#STR_INT_UIF_HINT_ACTIVATE");
	}

	override void OnReady()
	{
		RandomizeData();
		if (m_List)
			m_List.Reveal(0.45);
		ResetForms(false);
	}

	// "forms" shows the forms tab, "popup" then opens the time-zone dropdown,
	// "scrolled" also scrolls that popup to its end.
	override void OnTestCommand(string command)
	{
		if (command.IndexOf("forms") >= 0 && m_Tabs)
			m_Tabs.Select(1, true, true);
		if (command.IndexOf("popup") >= 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(OpenZonePopup, 900, false);
		if (command.IndexOf("scrolled") >= 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(ScrollZonePopup, 1600, false);
	}

	void OpenZonePopup()
	{
		if (m_DdZone)
			m_DdZone.OpenPopup();
	}

	void ScrollZonePopup()
	{
		if (m_DdZone)
			m_DdZone.ScrollPopupTo(1);
	}

	protected void OnTabChanged(int index, INT_UIF_Tabs tabs)
	{
		INT_UIF_Dropdown.CloseActive();
		array<Widget> cards = m_OverviewCards;
		if (index == 1)
			cards = m_FormCards;
		int n = cards.Count();
		for (int i = 0; i < n; i++)
		{
			INT_UIF_Animator.Reveal(cards.Get(i), 0.04 + i * 0.06, 12, 0.34);
		}
		if (index == 0 && m_List)
			m_List.Reveal(0.2);
	}

	// =================================================================================
	// Overview tab
	// =================================================================================
	protected void BuildControls(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_COMPONENTS", "gear", "#STR_INT_UIF_TTL_BUTTONS");
		RevealOnOpen(card.GetRoot());
		m_OverviewCards.Insert(card.GetRoot());
		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float half = (cw - 12) * 0.5;

		array<string> labels = {"#STR_INT_UIF_V_PRIMARY", "#STR_INT_UIF_V_ACCENT", "#STR_INT_UIF_V_SECONDARY", "#STR_INT_UIF_V_GHOST", "#STR_INT_UIF_V_SUCCESS", "#STR_INT_UIF_V_DANGER", "#STR_INT_UIF_V_WARNING", "#STR_INT_UIF_V_INFO"};
		array<int> variants = {INT_UIF_Variant.PRIMARY, INT_UIF_Variant.ACCENT, INT_UIF_Variant.SECONDARY, INT_UIF_Variant.GHOST, INT_UIF_Variant.SUCCESS, INT_UIF_Variant.DANGER, INT_UIF_Variant.WARNING, INT_UIF_Variant.INFO};
		array<string> icons = {"plus", "check", "", "search", "success", "trash", "warning", "info"};
		for (int i = 0; i < 8; i++)
		{
			int col = i % 2;
			int row = i / 2;
			INT_UIF_Button b = INT_UIF_Button.Create(m_Bag, c, col * (half + 12), row * 52, half, 40, labels.Get(i), variants.Get(i), icons.Get(i));
			b.OnClick().Insert(OnVariantClick);
		}
		INT_UIF_Button off = INT_UIF_Button.Create(m_Bag, c, 0, 208, cw, 40, "#STR_INT_UIF_DISABLED", INT_UIF_Variant.SECONDARY, "lock");
		off.SetEnabled(false);

		INT_UIF_Draw.Caption(c, 0, 270, cw, "#STR_INT_UIF_CAP_ICONS", "star");
		array<string> iconNames = {"shield_check", "users", "calendar", "radar", "crown", "refresh", "trash", "gear"};
		array<string> iconTips = {"#STR_INT_UIF_TIP_SHIELD", "#STR_INT_UIF_TIP_USERS", "#STR_INT_UIF_TIP_CALENDAR", "#STR_INT_UIF_TIP_RADAR", "#STR_INT_UIF_TIP_CROWN", "#STR_INT_UIF_TIP_REFRESH", "#STR_INT_UIF_TIP_TRASH", "#STR_INT_UIF_TIP_GEAR"};
		for (int k = 0; k < 8; k++)
		{
			int iv = INT_UIF_Variant.SECONDARY;
			if (k == 4)
				iv = INT_UIF_Variant.WARNING;
			if (k == 6)
				iv = INT_UIF_Variant.DANGER;
			INT_UIF_IconButton ib = INT_UIF_IconButton.Create(m_Bag, c, k * 46, 292, 36, iconNames.Get(k), iv, iconTips.Get(k));
			ib.OnClick().Insert(OnIconClick);
		}

		INT_UIF_Draw.Caption(c, 0, 352, cw, "#STR_INT_UIF_CAP_CHIPS", "list");
		array<string> chipText = {"#STR_INT_UIF_ROLE_OWNER", "#STR_INT_UIF_ROLE_ADMIN", "#STR_INT_UIF_ROLE_MEMBER", "#STR_INT_UIF_ROLE_BANNED", "#STR_INT_UIF_ROLE_GUEST"};
		array<int> chipVar = {INT_UIF_Variant.WARNING, INT_UIF_Variant.SUCCESS, INT_UIF_Variant.INFO, INT_UIF_Variant.DANGER, INT_UIF_Variant.SECONDARY};
		array<string> chipIcon = {"crown", "shield", "user", "lock", ""};
		float cx = 0;
		float cy = 374;
		for (int j = 0; j < 5; j++)
		{
			float chw = INT_UIF_Badge.FitWidth(chipText.Get(j), 26, chipIcon.Get(j));
			if (cx + chw > cw)
			{
				cx = 0;
				cy = cy + 34;
			}
			INT_UIF_Badge.Create(m_Bag, c, cx, cy, chipText.Get(j), chipVar.Get(j), 26, chipIcon.Get(j), chw);
			cx = cx + chw + 8;
		}

		INT_UIF_Draw.Caption(c, 0, cy + 50, cw, "#STR_INT_UIF_CAP_KEYS", "bolt");
		float kx = 0;
		float adv;
		INT_UIF_Draw.Keycap(c, kx, cy + 72, "Esc", "#STR_INT_UIF_HINT_CLOSE", adv);
		kx = kx + adv + 18;
		INT_UIF_Draw.Keycap(c, kx, cy + 72, "Ctrl+F10", "#STR_INT_UIF_INPUT_DEMO", adv);
	}

	protected void BuildData(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_DATA", "radar", "#STR_INT_UIF_TTL_METRICS");
		RevealOnOpen(card.GetRoot());
		m_OverviewCards.Insert(card.GetRoot());
		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float ch = card.GetContentH();

		m_Ring = INT_UIF_Ring.Create(m_Bag, c, 0, 0, 120, INT_UIF_ThemeKeys.TEAL);
		m_Ring.SetSubText("#STR_INT_UIF_RING_SUB");
		INT_UIF_Draw.Text(c, 140, 18, cw - 140, 24, "#STR_INT_UIF_RING_TITLE", 17, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		INT_UIF_Draw.Text(c, 140, 44, cw - 140, 18, "#STR_INT_UIF_RING_TEXT", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		INT_UIF_Button rnd = INT_UIF_Button.Create(m_Bag, c, 140, 72, 190, 34, "#STR_INT_UIF_RANDOM", INT_UIF_Variant.SECONDARY, "refresh");
		rnd.OnClick().Insert(OnRandomClick);

		float y = 150;
		array<string> barLabels = {"#STR_INT_UIF_BAR_A", "#STR_INT_UIF_BAR_B", "#STR_INT_UIF_BAR_C"};
		for (int i = 0; i < 3; i++)
		{
			INT_UIF_Draw.Text(c, 0, y, cw, 18, barLabels.Get(i), 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
			y = y + 24;
			if (i == 0)
				m_BarA = INT_UIF_ProgressBar.Create(m_Bag, c, 0, y, cw, 8, 0, INT_UIF_ThemeKeys.TEAL);
			else if (i == 1)
				m_BarB = INT_UIF_ProgressBar.Create(m_Bag, c, 0, y, cw, 8, 0, INT_UIF_ThemeKeys.AMBER);
			else
				m_BarC = INT_UIF_ProgressBar.Create(m_Bag, c, 0, y, cw, 8, 0, INT_UIF_ThemeKeys.RED);
			y = y + 24;
		}

		float tw = (cw - 12) * 0.5;
		m_TileA = INT_UIF_StatTile.Create(m_Bag, c, 0, y + 6, tw, 78, "#STR_INT_UIF_TILE_A", "0", INT_UIF_ThemeKeys.TEAL);
		m_TileB = INT_UIF_StatTile.Create(m_Bag, c, tw + 12, y + 6, tw, 78, "#STR_INT_UIF_TILE_B", "0", INT_UIF_ThemeKeys.AMBER);

		INT_UIF_Callout.Create(m_Bag, c, 0, ch - 92 - 58, cw, 88, "#STR_INT_UIF_CALLOUT_T", "#STR_INT_UIF_CALLOUT_X", INT_UIF_Variant.WARNING, "warning");
		INT_UIF_Callout.Create(m_Bag, c, 0, ch - 48, cw, 48, "", "#STR_INT_UIF_NOTE", INT_UIF_Variant.SECONDARY, "info");
	}

	protected void BuildPeople(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_LIST", "users", "#STR_INT_UIF_TTL_PEOPLE");
		RevealOnOpen(card.GetRoot());
		m_OverviewCards.Insert(card.GetRoot());
		int total = m_Names.Count();
		card.AddTitleBadge(total.ToString(), INT_UIF_Variant.INFO);
		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float ch = card.GetContentH();

		m_List = INT_UIF_List.Create(m_Bag, c, 0, 0, cw, ch - 56, "#STR_INT_UIF_LIST_EMPTY");
		m_List.OnAction().Insert(OnListAction);
		m_List.OnActivated().Insert(OnListActivated);
		FillList();

		float bw = (cw - 20) / 3.0;
		INT_UIF_Button dlg = INT_UIF_Button.Create(m_Bag, c, 0, ch - 40, bw, 40, "#STR_INT_UIF_DIALOG", INT_UIF_Variant.WARNING, "warning");
		INT_UIF_Button tst = INT_UIF_Button.Create(m_Bag, c, bw + 10, ch - 40, bw, 40, "#STR_INT_UIF_TOASTS", INT_UIF_Variant.SECONDARY, "bolt");
		INT_UIF_Button thm = INT_UIF_Button.Create(m_Bag, c, 2 * (bw + 10), ch - 40, bw, 40, "#STR_INT_UIF_THEME", INT_UIF_Variant.PRIMARY, "star");
		dlg.OnClick().Insert(OnDialogClick);
		tst.OnClick().Insert(OnToastClick);
		thm.OnClick().Insert(OnThemeClick);
	}

	protected void FillList()
	{
		if (!m_List)
			return;
		m_List.Clear();
		int n = m_Names.Count();
		for (int i = 0; i < n; i++)
		{
			string name = m_Names.Get(i);
			string role = m_Roles.Get(i);
			string letter = name.Substring(0, 1);
			if (role == "owner")
			{
				int r0 = m_List.AddRow(name, "#STR_INT_UIF_SUB_OWNER", i);
				m_List.SetRowAvatar(r0, letter, INT_UIF_ThemeKeys.AMBER);
				m_List.SetRowBadge(r0, "#STR_INT_UIF_ROLE_OWNER", INT_UIF_Variant.WARNING, "crown");
			}
			else if (role == "admin" || role == "member")
			{
				string sub = "#STR_INT_UIF_SUB_MEMBER";
				string hue = INT_UIF_ThemeKeys.BLUE;
				int variant = INT_UIF_Variant.INFO;
				string chip = "#STR_INT_UIF_ROLE_MEMBER";
				if (role == "admin")
				{
					sub = "#STR_INT_UIF_SUB_ADMIN";
					hue = INT_UIF_ThemeKeys.TEAL;
					variant = INT_UIF_Variant.SUCCESS;
					chip = "#STR_INT_UIF_ROLE_ADMIN";
				}
				int r1 = m_List.AddRow(name, sub, i);
				m_List.SetRowAvatar(r1, letter, hue);
				m_List.AddRowAction(r1, "remove", "user_minus", INT_UIF_Variant.DANGER, "#STR_INT_UIF_REMOVE");
				m_List.AddRowAction(r1, "crown", "crown", INT_UIF_Variant.WARNING, "#STR_INT_UIF_TRANSFER");
				m_List.SetRowBadge(r1, chip, variant);
			}
			else
			{
				int r2 = m_List.AddRow(name, "#STR_INT_UIF_SUB_NEARBY", i);
				m_List.SetRowAvatar(r2, letter, INT_UIF_ThemeKeys.VIOLET);
				m_List.SetRowButton(r2, "add", "#STR_INT_UIF_ADD", INT_UIF_Variant.SUCCESS, "user_plus", 128);
			}
		}
	}

	// =================================================================================
	// Forms tab
	// =================================================================================
	protected void BuildSwitches(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_SWITCHES", "gear", "#STR_INT_UIF_TTL_SETTINGS");
		m_FormCards.Insert(card.GetRoot());
		Widget c = card.GetContent();
		float cw = card.GetContentW();

		m_TgProtect = INT_UIF_Toggle.Create(m_Bag, c, 0, 0, cw, "#STR_INT_UIF_F_PROTECT", true, "#STR_INT_UIF_F_PROTECT_SUB");
		m_TgProtect.OnChanged().Insert(OnProtectChanged);
		m_TgNotify = INT_UIF_Toggle.Create(m_Bag, c, 0, 60, cw, "#STR_INT_UIF_F_NOTIFY", true, "#STR_INT_UIF_F_NOTIFY_SUB");
		m_TgSound = INT_UIF_Toggle.Create(m_Bag, c, 0, 120, cw, "#STR_INT_UIF_F_SOUND", false, "#STR_INT_UIF_F_SOUND_SUB");
		INT_UIF_Toggle soon = INT_UIF_Toggle.Create(m_Bag, c, 0, 180, cw, "#STR_INT_UIF_F_AUTOCLEAN", false, "#STR_INT_UIF_F_SOON");
		soon.SetEnabled(false);

		INT_UIF_Divider.Create(m_Bag, c, 0, 242, cw);
		INT_UIF_Draw.Caption(c, 0, 258, cw, "#STR_INT_UIF_CAP_OPTIONS", "list");
		m_CbHints = INT_UIF_Checkbox.Create(m_Bag, c, 0, 284, cw, "#STR_INT_UIF_F_HINTS", true);
		m_CbCompact = INT_UIF_Checkbox.Create(m_Bag, c, 0, 320, cw, "#STR_INT_UIF_F_COMPACT", false);
		m_CbAnim = INT_UIF_Checkbox.Create(m_Bag, c, 0, 356, cw, "#STR_INT_UIF_F_ANIM", true);

		INT_UIF_Divider.Create(m_Bag, c, 0, 402, cw);
		INT_UIF_Draw.Caption(c, 0, 418, cw, "#STR_INT_UIF_CAP_LIMITS", "users");
		INT_UIF_Draw.Text(c, 0, 446, cw - 180, 36, "#STR_INT_UIF_F_MEMBERS", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_StMembers = INT_UIF_Stepper.Create(m_Bag, c, cw - 170, 446, 170, 1, 20, 6, 1);
		m_StMembers.OnChanged().Insert(OnMembersChanged);
		INT_UIF_Draw.Text(c, 0, 494, cw - 180, 36, "#STR_INT_UIF_F_TERMINALS", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_StTerminals = INT_UIF_Stepper.Create(m_Bag, c, cw - 170, 494, 170, 1, 5, 2, 1);
	}

	protected void BuildFields(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_INPUTS", "pin", "#STR_INT_UIF_TTL_FIELDS");
		m_FormCards.Insert(card.GetRoot());
		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float ch = card.GetContentH();

		INT_UIF_Draw.Caption(c, 0, 0, cw, "#STR_INT_UIF_CAP_NAME", "home");
		m_NameCount = INT_UIF_Draw.Text(c, cw - 90, 0, 90, 16, "", 11, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT), INT_UIF_Align.RIGHT);
		m_InName = INT_UIF_TextInput.Create(m_Bag, c, 0, 24, cw, 40, "#STR_INT_UIF_F_NAME_PH", NAME_MAX, "home");
		m_InName.OnChanged().Insert(OnNameChanged);

		INT_UIF_Draw.Caption(c, 0, 82, cw, "#STR_INT_UIF_CAP_SEARCH", "search");
		m_InSearch = INT_UIF_TextInput.Create(m_Bag, c, 0, 106, cw, 40, "#STR_INT_UIF_F_SEARCH_PH", 32, "search", "Enter");
		m_InSearch.OnSubmit().Insert(OnSearchSubmit);

		INT_UIF_Draw.Caption(c, 0, 164, cw, "#STR_INT_UIF_CAP_ACCESS", "shield");
		array<string> modes = {"#STR_INT_UIF_F_MODE_1", "#STR_INT_UIF_F_MODE_2", "#STR_INT_UIF_F_MODE_3", "#STR_INT_UIF_F_MODE_4"};
		m_DdMode = INT_UIF_Dropdown.Create(m_Bag, c, 0, 188, cw, modes, 1, "", 40, "shield");
		m_DdMode.OnSelected().Insert(OnPreviewDropdown);

		INT_UIF_Draw.Caption(c, 0, 246, cw, "#STR_INT_UIF_CAP_ZONE", "clock");
		array<string> zones = new array<string>();
		for (int z = 0; z < 12; z++)
		{
			zones.Insert("UTC+" + z.ToString());
		}
		m_DdZone = INT_UIF_Dropdown.Create(m_Bag, c, 0, 270, cw, zones, 3, "", 40, "clock");
		m_DdZone.OnSelected().Insert(OnPreviewDropdown);

		INT_UIF_Draw.Caption(c, 0, 328, cw, "#STR_INT_UIF_CAP_PERIOD", "calendar");
		m_SegPeriod = INT_UIF_Tabs.Create(m_Bag, c, 0, 352, cw, 38);
		m_SegPeriod.AddTab("#STR_INT_UIF_F_DAY");
		m_SegPeriod.AddTab("#STR_INT_UIF_F_WEEK");
		m_SegPeriod.AddTab("#STR_INT_UIF_F_MONTH");
		m_SegPeriod.OnChanged().Insert(OnPeriodChanged);

		INT_UIF_Draw.Caption(c, 0, 408, cw, "#STR_INT_UIF_CAP_TABS", "list");
		m_UlTabs = INT_UIF_Tabs.Create(m_Bag, c, 0, 430, cw, 40, INT_UIF_TabStyle.UNDERLINE);
		m_UlTabs.AddTab("#STR_INT_UIF_F_TAB_ALL", "", 12);
		m_UlTabs.AddTab("#STR_INT_UIF_F_TAB_ONLINE", "", 4);
		m_UlTabs.AddTab("#STR_INT_UIF_F_TAB_BANNED", "", 1);

		float bw = (cw - 12) * 0.5;
		INT_UIF_Button save = INT_UIF_Button.Create(m_Bag, c, 0, ch - 40, bw, 40, "#STR_INT_UIF_F_SAVE", INT_UIF_Variant.PRIMARY, "check");
		INT_UIF_Button reset = INT_UIF_Button.Create(m_Bag, c, bw + 12, ch - 40, bw, 40, "#STR_INT_UIF_F_RESET", INT_UIF_Variant.SECONDARY, "refresh");
		save.OnClick().Insert(OnSaveClick);
		reset.OnClick().Insert(OnResetClick);
	}

	protected void BuildPreview(Widget page, float x, float w, float h)
	{
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, page, x, 0, w, h, "#STR_INT_UIF_CAP_PREVIEW", "eye", "#STR_INT_UIF_TTL_LIVE");
		m_FormCards.Insert(card.GetRoot());
		Widget c = card.GetContent();
		float cw = card.GetContentW();
		float ch = card.GetContentH();

		m_PvRing = INT_UIF_Ring.Create(m_Bag, c, 0, 0, 120, INT_UIF_ThemeKeys.TEAL);
		m_PvRing.SetSubText("#STR_INT_UIF_F_RING_SUB");
		m_PvName = INT_UIF_Draw.Text(c, 140, 6, cw - 140, 26, "", 18, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_PvName)
			m_PvName.SetBold(true);
		m_PvMode = INT_UIF_Draw.Text(c, 140, 34, cw - 140, 18, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_PvZone = INT_UIF_Draw.Text(c, 140, 56, cw - 140, 18, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		float bwOn = INT_UIF_Badge.FitWidth("#STR_INT_UIF_F_ON", 26, "shield");
		float bwOff = INT_UIF_Badge.FitWidth("#STR_INT_UIF_F_OFF", 26, "shield");
		if (bwOff > bwOn)
			bwOn = bwOff;
		m_PvBadge = INT_UIF_Badge.Create(m_Bag, c, 140, 84, "#STR_INT_UIF_F_ON", INT_UIF_Variant.SUCCESS, 26, "shield", bwOn);

		INT_UIF_Draw.Caption(c, 0, 146, cw, "#STR_INT_UIF_CAP_TUNING", "target");
		m_SlRadius = INT_UIF_Slider.Create(m_Bag, c, 0, 172, cw, 10, 100, 50, 5, "#STR_INT_UIF_F_RADIUS");
		m_SlRadius.SetFormat(0, " m");
		m_SlRadius.OnChanged().Insert(OnRadiusChanged);
		m_SlDelay = INT_UIF_Slider.Create(m_Bag, c, 0, 236, cw, 0, 30, 5, 1, "#STR_INT_UIF_F_DELAY");
		m_SlDelay.SetFormat(0, " " + Widget.TranslateString("#STR_INT_UIF_UNIT_MIN"));
		m_SlOpacity = INT_UIF_Slider.Create(m_Bag, c, 0, 300, cw, 40, 100, 100, 1, "#STR_INT_UIF_F_OPACITY");
		m_SlOpacity.SetFormat(0, "%");
		m_SlOpacity.OnChanged().Insert(OnOpacityChanged);

		float tw = (cw - 12) * 0.5;
		m_PvMembers = INT_UIF_StatTile.Create(m_Bag, c, 0, 370, tw, 78, "#STR_INT_UIF_F_MEMBERS_T", "0", INT_UIF_ThemeKeys.TEAL);
		m_PvArea = INT_UIF_StatTile.Create(m_Bag, c, tw + 12, 370, tw, 78, "#STR_INT_UIF_F_AREA_T", "0", INT_UIF_ThemeKeys.AMBER);

		INT_UIF_Callout.Create(m_Bag, c, 0, ch - 88, cw, 88, "#STR_INT_UIF_F_TIP_T", "#STR_INT_UIF_F_TIP_X", INT_UIF_Variant.INFO, "info");
	}

	// Defaults; animate false on open, true from the Reset button.
	protected void ResetForms(bool animate)
	{
		if (!m_TgProtect)
			return;
		m_TgProtect.SetOn(true, false, animate);
		m_TgNotify.SetOn(true, false, animate);
		m_TgSound.SetOn(false, false, animate);
		m_CbHints.SetChecked(true, false, animate);
		m_CbCompact.SetChecked(false, false, animate);
		m_CbAnim.SetChecked(true, false, animate);
		m_StMembers.SetValue(6);
		m_StTerminals.SetValue(2);
		m_InName.SetText(Widget.TranslateString("#STR_INT_UIF_F_NAME_SAMPLE"));
		m_InSearch.SetText("");
		m_DdMode.SetSelected(1);
		m_DdZone.SetSelected(3);
		m_SegPeriod.Select(1, false, animate);
		m_SlRadius.SetValue(50, false, animate);
		m_SlDelay.SetValue(5, false, animate);
		m_SlOpacity.SetValue(100, false, animate);
		ApplyOpacity(100);
		RefreshPreview();
		if (m_PvRing)
			m_PvRing.SetValue(0.5, animate);
		if (m_PvMembers)
			m_PvMembers.CountTo(6, "", 0.6);
		UpdateArea();
	}

	protected void RefreshPreview()
	{
		if (!m_InName)
			return;
		string typedName = m_InName.GetText();
		string name = typedName;
		if (name == "")
			name = Widget.TranslateString("#STR_INT_UIF_F_NAME_PH");
		if (m_PvName)
			m_PvName.SetText(name);
		int len = typedName.LengthUtf8();
		int maxLen = NAME_MAX;
		if (m_NameCount)
			m_NameCount.SetText(len.ToString() + " / " + maxLen.ToString());
		if (m_PvMode)
			m_PvMode.SetText(m_DdMode.GetSelectedText());
		string period = "";
		if (m_SegPeriod)
		{
			int p = m_SegPeriod.GetActive();
			if (p == 0)
				period = "#STR_INT_UIF_F_DAY";
			else if (p == 1)
				period = "#STR_INT_UIF_F_WEEK";
			else
				period = "#STR_INT_UIF_F_MONTH";
		}
		if (m_PvZone)
			m_PvZone.SetText(m_DdZone.GetSelectedText() + "  ·  " + Widget.TranslateString(period));
		if (m_PvBadge && m_TgProtect)
		{
			if (m_TgProtect.IsOn())
			{
				m_PvBadge.SetText("#STR_INT_UIF_F_ON");
				m_PvBadge.SetVariant(INT_UIF_Variant.SUCCESS);
			}
			else
			{
				m_PvBadge.SetText("#STR_INT_UIF_F_OFF");
				m_PvBadge.SetVariant(INT_UIF_Variant.DANGER);
			}
		}
		if (m_PvRing && m_SlRadius)
		{
			int r = (int)m_SlRadius.GetValue();
			m_PvRing.SetCenterText(r.ToString() + " m");
		}
	}

	protected void UpdateArea()
	{
		if (!m_PvArea || !m_SlRadius)
			return;
		float rad = m_SlRadius.GetValue();
		int area = (int)Math.Round(Math.PI * rad * rad);
		m_PvArea.CountTo(area, "", 0.6);
	}

	protected void ApplyOpacity(float percent)
	{
		if (m_Window)
			m_Window.SetAlpha(percent / 100.0);
	}

	// =================================================================================
	// Events
	// =================================================================================
	protected void RandomizeData()
	{
		if (m_Ring)
			m_Ring.SetValue(Math.RandomFloat(0.2, 0.98));
		if (m_BarA)
			m_BarA.SetValue(Math.RandomFloat(0.1, 1));
		if (m_BarB)
			m_BarB.SetValue(Math.RandomFloat(0.1, 1));
		if (m_BarC)
			m_BarC.SetValue(Math.RandomFloat(0.1, 1));
		if (m_TileA)
			m_TileA.CountTo(Math.RandomInt(3, 40), "");
		if (m_TileB)
			m_TileB.CountTo(Math.RandomInt(20, 168), "h");
	}

	protected void OnRandomClick(INT_UIF_Button button)
	{
		RandomizeData();
	}

	protected void OnVariantClick(INT_UIF_Button button)
	{
		INT_UIF_Toast.Show(button.GetText(), INT_UIF_ToastKind.INFO, 2.5);
	}

	protected void OnIconClick(INT_UIF_IconButton button)
	{
		INT_UIF_Toast.Show("#STR_INT_UIF_ICON_CLICK", INT_UIF_ToastKind.INFO, 2.0);
	}

	protected void OnListAction(int row, string actionId, INT_UIF_List list)
	{
		int idx = list.GetData(row);
		if (actionId == "add")
		{
			m_Roles.Set(idx, "member");
			FillList();
			m_List.Reveal(0);
			INT_UIF_Toast.Success("#STR_INT_UIF_ADDED");
			return;
		}
		if (actionId == "crown")
		{
			INT_UIF_Toast.Warning("#STR_INT_UIF_TRANSFER");
			return;
		}
		if (actionId == "remove")
		{
			m_PendingRemove = idx;
			INT_UIF_Dialog d = INT_UIF_Dialog.Confirm("#STR_INT_UIF_CONFIRM_TITLE", "#STR_INT_UIF_CONFIRM_TEXT", "#STR_INT_UIF_REMOVE", "#STR_INT_UIF_CANCEL", INT_UIF_Variant.DANGER);
			if (d)
				d.OnResult().Insert(OnRemoveConfirmed);
		}
	}

	protected void OnRemoveConfirmed(int result, INT_UIF_Dialog dialog)
	{
		if (result != INT_UIF_Dialog.RESULT_YES || m_PendingRemove < 0)
			return;
		m_Roles.Set(m_PendingRemove, "nearby");
		m_PendingRemove = -1;
		FillList();
		m_List.Reveal(0);
		INT_UIF_Toast.Show("#STR_INT_UIF_REMOVED", INT_UIF_ToastKind.SUCCESS);
	}

	protected void OnListActivated(int row, INT_UIF_List list)
	{
		INT_UIF_Toast.Info(list.GetPrimary(row));
	}

	protected void OnDialogClick(INT_UIF_Button button)
	{
		INT_UIF_Dialog.Alert("#STR_INT_UIF_ALERT_TITLE", "#STR_INT_UIF_ALERT_TEXT", INT_UIF_ToastKind.SUCCESS);
	}

	protected void OnToastClick(INT_UIF_Button button)
	{
		INT_UIF_Toast.Show("#STR_INT_UIF_TOAST_INFO", INT_UIF_ToastKind.INFO, 4.0);
		INT_UIF_Toast.Show("#STR_INT_UIF_TOAST_SUCCESS", INT_UIF_ToastKind.SUCCESS, 4.5);
		INT_UIF_Toast.Show("#STR_INT_UIF_TOAST_WARNING", INT_UIF_ToastKind.WARNING, 5.0);
		INT_UIF_Toast.Show("#STR_INT_UIF_TOAST_ERROR", INT_UIF_ToastKind.ERROR, 5.5);
	}

	protected void OnThemeClick(INT_UIF_Button button)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (tm.Active().GetName() == "tactical")
			tm.SetActive("khaki");
		else
			tm.SetActive("tactical");
	}

	protected void OnProtectChanged(bool on, INT_UIF_Toggle toggle)
	{
		RefreshPreview();
	}

	protected void OnMembersChanged(float value, INT_UIF_Stepper stepper)
	{
		if (m_PvMembers)
			m_PvMembers.CountTo((int)value, "", 0.4);
	}

	protected void OnNameChanged(string text, INT_UIF_TextInput input)
	{
		RefreshPreview();
	}

	protected void OnSearchSubmit(string text, INT_UIF_TextInput input)
	{
		if (text == "")
			return;
		INT_UIF_Toast.Info(Widget.TranslateString("#STR_INT_UIF_F_SEARCHING") + " " + text);
	}

	protected void OnPreviewDropdown(int index, string text, INT_UIF_Dropdown dropdown)
	{
		RefreshPreview();
	}

	protected void OnPeriodChanged(int index, INT_UIF_Tabs tabs)
	{
		RefreshPreview();
	}

	protected void OnRadiusChanged(float value, INT_UIF_Slider slider, bool isFinal)
	{
		if (m_PvRing)
			m_PvRing.SetValue(value / 100.0, false);
		RefreshPreview();
		if (isFinal)
			UpdateArea();
	}

	protected void OnOpacityChanged(float value, INT_UIF_Slider slider, bool isFinal)
	{
		ApplyOpacity(value);
	}

	protected void OnSaveClick(INT_UIF_Button button)
	{
		INT_UIF_Toast.Success("#STR_INT_UIF_F_SAVED");
	}

	protected void OnResetClick(INT_UIF_Button button)
	{
		ResetForms(true);
		INT_UIF_Toast.Info("#STR_INT_UIF_F_RESET_DONE");
	}
}
