// ============================================================================
// DRL_CraftMenu - окно крафта на INT_UIFramework (замена DRL_CraftMenuUI).
//
// Раскладка как у INT_Trader (окно 1640x920):
//   шапка  — название, верстак/свободный крафт, очередь процессов, слоты, «Изучение»;
//   слева  — сайдбар категорий;
//   центр  — заголовок категории, поиск, сетка карточек рецептов
//            (поверх правой части выезжает панель вариаций / требований);
//   справа — детали рецепта: сцена с 3D-превью (+ доп. навесное), чипы,
//            варианты результата, вкладки «Состав / Описание / Разбор»,
//            количество и кнопка крафта с прогрессом.
// Логика выбора, фильтров, проверок, состояний кнопки и RPC повторяет старое
// меню 1:1 и идёт через те же хелперы (DRL_CraftInventoryHelper,
// DRL_CraftMergedHelper, DRL_CraftComponentResolver и т.д.).
// Сервер (обфусцированный DRL_CraftMenu_Server.pbo) не меняется.
// ============================================================================

class DRL_CraftWindow extends INT_UIF_Menu
{
	static const float SIDE_W = 250;
	static const float DETAILS_W = 540;
	static const float GAP = 16;
	static const float STAGE_H = 200;
	static const float DETAILS_FOOT_H = 104;
	static const float VARIANT_TILE = 52;
	static const float ROW_H = 50;
	static const float ROW_GAP = 6;
	static const float POPOVER_W = 440;

	static const int TAB_COMP = 0;
	static const int TAB_INFO = 1;
	static const int TAB_RECRAFT = 2;

	// Теги строк «Состава»: ингредиент (индекс в m_Merged), инструмент, доп. рецепт.
	static const int TAG_TOOL = 1000;
	static const int TAG_DOP = 2000;

	// = DRL_RecipeVarWidget.MAX_ATTACHED_QUANTITY
	static const int MAX_ATTACHED_QUANTITY = 10;

	// Контекст: верстак (null — свободное меню F9).
	protected EntityAI m_ContextWorkbench;
	protected string m_WorkbenchCls;

	// Состояние выбора (как в старом меню).
	protected int m_CurrentCategoryIdx = -1;
	protected DRL_Craft_Recipe m_SelectedRecipe;
	protected DRL_Craft_Recipe m_ShownRecipe;
	protected string m_SelectedResultClass;
	protected string m_Query;
	protected ref map<int, string> m_SelectedComponentVariations;
	protected ref array<int> m_SelectedAttachedRecipeIds;
	protected ref map<int, string> m_SelectedAttachedVariants;
	protected ref map<int, int> m_SelectedAttachedQuantities;

	// Данные за строками списков.
	protected ref array<ref DRL_CraftMergedComponent> m_Merged;
	protected ref array<DRL_Craft_Component> m_ToolComps;
	protected ref array<DRL_Craft_Recipe> m_DopRecipes;
	protected ref array<ref DRL_VariationClickInfo> m_VarInfos;
	protected ref array<DRL_Craft_Recipe> m_ReqTargets;
	protected ref DRL_CraftMergedComponent m_ActiveVariationMerged;

	// Процессы крафта и разбор.
	protected ref array<ref DRL_UIProc> m_Procs;
	protected EffectSound m_CraftSound;
	protected bool m_ReCraftActive;
	protected float m_ReCraftElapsed;
	protected float m_ReCraftDuration;

	// Компоненты.
	protected ref DRL_UISidebar m_Sidebar;
	protected ref DRL_UIRecipeGrid m_Grid;
	protected ref DRL_UIStage m_Stage;
	protected ref DRL_UIVariantStrip m_Variants;
	protected ref DRL_UIRowList m_CompList;
	protected ref DRL_UIRowList m_RecraftList;
	protected ref DRL_UIPopover m_Popover;
	protected ref DRL_UIHoverCard m_HoverCard;
	protected ref DRL_UIQueueBar m_Queue;
	protected INT_UIF_TextInput m_Search;
	protected INT_UIF_Button m_LearningBtn;
	protected INT_UIF_Badge m_SlotsBadge;
	protected TextWidget m_ListTitle;
	protected TextWidget m_ListSub;

	// Детали.
	protected Widget m_Details;
	protected Widget m_DetailsEmpty;
	protected Widget m_DetailsBody;
	protected TextWidget m_RecipeTitle;
	protected INT_UIF_Button m_ReqBtn;
	protected ref DRL_UIChip m_ChipStatus;
	protected ref DRL_UIChip m_ChipTime;
	protected ref DRL_UIChip m_ChipCount;
	protected float m_ChipY;
	protected float m_VariantsY;

	protected Widget m_TabsHost;
	protected INT_UIF_Tabs m_Tabs;
	protected Widget m_PageComp;
	protected Widget m_PageInfo;
	protected Widget m_PageRecraft;
	protected float m_PageMaxH;

	// Вкладка «Описание».
	protected TextWidget m_InfoName;
	protected TextWidget m_InfoState;
	protected TextWidget m_InfoFill;
	protected MultilineTextWidget m_RecipeDesc;
	protected Widget m_StatsBox;
	protected ref array<TextWidget> m_StatValues;
	protected Widget m_SlotsRow;
	protected Widget m_SlotIcons;
	protected TextWidget m_SlotsNone;

	// Вкладка «Разбор».
	protected Widget m_RecraftFoot;
	protected TextWidget m_RecraftTime;
	protected INT_UIF_Button m_RecraftBtn;
	protected INT_UIF_ProgressBar m_RecraftBar;
	protected string m_RecraftText;

	// Низ деталей: количество и кнопка крафта.
	protected INT_UIF_IconButton m_CountMinus;
	protected INT_UIF_IconButton m_CountPlus;
	protected TextWidget m_CountText;
	protected TextWidget m_TotalText;
	protected TextWidget m_FootTime;
	protected INT_UIF_Button m_CraftBtn;
	protected INT_UIF_ProgressBar m_CraftBar;
	protected string m_CraftText;

	protected float m_SlotTimer;

	void DRL_CraftWindow()
	{
		m_Sidebar = new DRL_UISidebar();
		m_Grid = new DRL_UIRecipeGrid();
		m_Stage = new DRL_UIStage();
		m_Variants = new DRL_UIVariantStrip();
		m_CompList = new DRL_UIRowList();
		m_RecraftList = new DRL_UIRowList();
		m_Popover = new DRL_UIPopover();
		m_HoverCard = new DRL_UIHoverCard();
		m_Queue = new DRL_UIQueueBar();

		m_SelectedComponentVariations = new map<int, string>();
		m_SelectedAttachedRecipeIds = new array<int>();
		m_SelectedAttachedVariants = new map<int, string>();
		m_SelectedAttachedQuantities = new map<int, int>();
		m_Merged = new array<ref DRL_CraftMergedComponent>();
		m_ToolComps = new array<DRL_Craft_Component>();
		m_DopRecipes = new array<DRL_Craft_Recipe>();
		m_VarInfos = new array<ref DRL_VariationClickInfo>();
		m_ReqTargets = new array<DRL_Craft_Recipe>();
		m_Procs = new array<ref DRL_UIProc>();
		m_StatValues = new array<TextWidget>();
	}

	void ~DRL_CraftWindow()
	{
		UnhookProcesses();
		StopCraftSound();
		ReleaseModels();
	}

	// Верстак задаётся до Init (как в старом DRL_WorkbenchMenuUI).
	void SetWorkbench(EntityAI workbench)
	{
		m_ContextWorkbench = workbench;
		m_WorkbenchCls = "";
		if (workbench)
		{
			m_WorkbenchCls = workbench.GetType();
			m_WorkbenchCls = m_WorkbenchCls.Trim();
			PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
			if (pb)
				pb.DRL_SetCurrentWorkbench(workbench);
		}
	}

	DRL_Craft_Config Cfg()
	{
		return g_Craft_Config;
	}

	// =================================================================================
	// Окно
	// =================================================================================
	override string GetTitle()
	{
		return "#STR_DRL_UI_TITLE";
	}

	override string GetSubtitle()
	{
		if (m_ContextWorkbench)
		{
			string dn = m_ContextWorkbench.GetDisplayName();
			if (dn != "")
				return dn;
			return "#STR_WBName";
		}
		return "#STR_DRL_UI_SUB_FREE";
	}

	override string GetIcon()
	{
		return "gear";
	}

	override void GetWindowSize(out float w, out float h)
	{
		w = 1640;
		h = 920;
	}

	// Поле поиска требует клавиатуру.
	override bool WantsKeyboard()
	{
		return true;
	}

	override void OnBuildHeader(Widget center, Widget right)
	{
		float slot = GetHeaderSlotW();
		m_Queue.Build(center, slot, HEADER_H);
		m_Queue.OnAction().Insert(OnQueueAction);

		DRL_Craft_Config cfg = Cfg();
		bool showLearning = false;
		if (cfg && cfg.IsLearningEnabled() && cfg.IsFreeLearningMenuEnabled())
			showLearning = true;
		float bx = slot;
		if (showLearning)
		{
			bx = slot - 150;
			m_LearningBtn = INT_UIF_Button.Create(m_Bag, right, bx, 20, 150, 36, "#STR_DRL_UI_LEARNING", INT_UIF_Variant.SECONDARY, "star");
			m_LearningBtn.OnClick().Insert(OnLearningClick);
			DRL_UIKitUtil.Expose(m_LearningBtn, "DRL_LearningBtn");
		}
		float bw = 190;
		m_SlotsBadge = INT_UIF_Badge.Create(m_Bag, right, bx - 12 - bw, 27, "", INT_UIF_Variant.INFO, 22, "list", bw);
		UpdateSlotInfo();
	}

	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);

		// --- слева: категории ---------------------------------------------------------
		Widget left = INT_UIF_Draw.Frame(body, 0, 0, SIDE_W, bh, "DRL_Left");
		RevealOnOpen(left);
		INT_UIF_Draw.Caption(left, 4, 2, SIDE_W, "#STR_DRL_UI_CATEGORIES", "list");
		m_Sidebar.Build(left, 0, 24, SIDE_W, bh - 24);
		m_Sidebar.OnSelected().Insert(OnCategorySelected);

		// --- центр: заголовок, поиск, сетка ---------------------------------------------
		float gx = SIDE_W + GAP;
		float gw = bw - SIDE_W - GAP - DETAILS_W - GAP;
		Widget mid = INT_UIF_Draw.Frame(body, gx, 0, gw, bh, "DRL_Mid");
		RevealOnOpen(mid);
		m_ListTitle = INT_UIF_Draw.Text(mid, 2, 0, gw - 340, 24, "", 19, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_ListTitle.SetBold(true);
		m_ListSub = INT_UIF_Draw.Text(mid, 2, 24, gw - 340, 16, "", 11, faint);
		m_Search = INT_UIF_TextInput.Create(m_Bag, mid, gw - 320, 0, 320, 40, "#STR_SEARCH_RECIPE", 40, "search", "");
		m_Search.OnChanged().Insert(OnSearchChanged);
		m_Search.GetRoot().SetName("DRL_Search");
		m_Grid.Build(mid, 0, 52, gw, bh - 52, 4, 3, 12);
		m_Grid.OnSelected().Insert(OnGridSelected);

		// Панель вариаций / требований выезжает поверх правой части сетки.
		m_Popover.Build(m_Bag, body, gx + gw - POPOVER_W, 0, POPOVER_W, bh);
		m_Popover.OnClose().Insert(OnPopoverClosed);
		m_Popover.GetList().OnAction().Insert(OnPopAction);
		m_Popover.GetList().OnHover().Insert(OnPopHover);

		// --- справа: детали рецепта ----------------------------------------------------
		BuildDetails(body, bw - DETAILS_W, 0, DETAILS_W, bh);

		AddHint("#STR_DRL_UI_KEY_LMB", "#STR_DRL_UI_HINT_SELECT");
		AddHint("#STR_DRL_UI_KEY_WHEEL", "#STR_DRL_UI_HINT_SCROLL");
		AddHint("#STR_DRL_UI_KEY_DRAG", "#STR_DRL_UI_HINT_ROTATE");
	}

	protected void BuildDetails(Widget parent, float x, float y, float w, float h)
	{
		m_Details = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Details");
		RevealOnOpen(m_Details);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);

		// Пусто: ничего не выбрано.
		m_DetailsEmpty = INT_UIF_Draw.Frame(m_Details, 0, 0, w, h, "DRL_DetailsEmpty");
		// Цвета — в локальные: два вызова одной функции в аргументах Enforce
		// сводит к результату последнего.
		int panelFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		int panelBorder = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL_BORDER);
		INT_UIF_Surface.Box(m_DetailsEmpty, 0, 0, w, h, 12, panelFill, panelBorder);
		INT_UIF_Draw.Image(m_DetailsEmpty, w * 0.5 - 120, h * 0.5 - 150, 240, 240, "iuf_radial", ARGB(16, 255, 255, 255));
		INT_UIF_Draw.Icon(m_DetailsEmpty, w * 0.5 - 22, h * 0.5 - 56, 44, "gear", faint);
		INT_UIF_Draw.Text(m_DetailsEmpty, 0, h * 0.5 + 4, w, 24, "#STR_DRL_UI_SELECT_HINT", 16, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);

		// Рецепт выбран.
		m_DetailsBody = INT_UIF_Draw.Frame(m_Details, 0, 0, w, h, "DRL_DetailsBody");
		m_Stage.Build(m_DetailsBody, 0, 0, w, STAGE_H);
		m_Stage.SetAccent(INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));

		float ty = STAGE_H + 12;
		m_RecipeTitle = INT_UIF_Draw.Text(m_DetailsBody, 2, ty, w - 170, 26, "", 20, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_RecipeTitle.SetBold(true);
		m_ReqBtn = INT_UIF_Button.Create(m_Bag, m_DetailsBody, w - 156, ty - 4, 156, 34, "#STR_DRL_UI_REQS", INT_UIF_Variant.SUCCESS, "shield");
		m_ReqBtn.OnClick().Insert(OnReqClick);
		DRL_UIKitUtil.Expose(m_ReqBtn, "DRL_ReqBtn");

		m_ChipY = ty + 34;
		m_ChipStatus = new DRL_UIChip();
		m_ChipStatus.Build(m_DetailsBody, 2, m_ChipY, 22, "DRL_DetailStatus");
		m_ChipTime = new DRL_UIChip();
		m_ChipTime.Build(m_DetailsBody, 120, m_ChipY, 22, "DRL_DetailTime");
		m_ChipCount = new DRL_UIChip();
		m_ChipCount.Build(m_DetailsBody, 240, m_ChipY, 22, "DRL_DetailCount");

		m_VariantsY = m_ChipY + 32;
		m_Variants.Build(m_DetailsBody, 0, m_VariantsY, w, VARIANT_TILE);
		m_Variants.OnSelected().Insert(OnVariantSelected);

		// Вкладки: область между вариантами (если есть) и низом панели.
		float footY = h - DETAILS_FOOT_H;
		m_PageMaxH = footY - 8 - m_VariantsY - 46;
		m_TabsHost = INT_UIF_Draw.Frame(m_DetailsBody, 0, m_VariantsY, w, footY - 8 - m_VariantsY, "DRL_TabsHost");
		m_Tabs = INT_UIF_Tabs.Create(m_Bag, m_TabsHost, 0, 0, w, 38);
		m_Tabs.AddTab("#STR_DRL_UI_TAB_COMP", "list");
		m_Tabs.AddTab("#STR_DRL_UI_TAB_INFO", "info");
		m_Tabs.AddTab("#STR_DRL_UI_TAB_RECRAFT", "refresh");
		m_Tabs.OnChanged().Insert(OnTabChanged);
		m_PageComp = INT_UIF_Draw.Frame(m_TabsHost, 0, 46, w, m_PageMaxH, "DRL_PageComp");
		m_PageInfo = INT_UIF_Draw.Frame(m_TabsHost, 0, 46, w, m_PageMaxH, "DRL_PageInfo");
		m_PageRecraft = INT_UIF_Draw.Frame(m_TabsHost, 0, 46, w, m_PageMaxH, "DRL_PageRecraft");

		m_CompList.Build(m_PageComp, 0, 0, w, m_PageMaxH, ROW_H, ROW_GAP, "DRL_Comp");
		m_CompList.SetEmptyText("#STR_DRL_UI_NO_INGR");
		m_CompList.OnAction().Insert(OnCompAction);
		m_CompList.OnHover().Insert(OnCompHover);
		BuildInfoPage(m_PageInfo, w);
		BuildRecraftPage(m_PageRecraft, w);
		ShowPage(TAB_COMP, false);

		BuildFooter(m_DetailsBody, footY, w);

		// Подсказка ингредиента ложится поверх сцены превью.
		m_HoverCard.Build(m_DetailsBody, 0, 0, w, STAGE_H);

		m_DetailsBody.Show(false);
	}

	protected void BuildInfoPage(Widget page, float w)
	{
		int text = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		float half = (w - 4) * 0.5;
		m_InfoName = INT_UIF_Draw.Text(page, 2, 0, w - 4, 18, "", 13, text);
		m_InfoState = INT_UIF_Draw.Text(page, 2, 20, half, 18, "", 12, muted);
		m_InfoFill = INT_UIF_Draw.Text(page, 2 + half, 20, half, 18, "", 12, muted);
		m_RecipeDesc = INT_UIF_Draw.MText(page, 2, 46, w - 4, 80, "", 13, muted);

		m_StatsBox = INT_UIF_Draw.Frame(page, 0, 0, w, 64, "DRL_Stats");
		INT_UIF_Draw.Caption(m_StatsBox, 2, 0, w, "#STR_DRL_UI_STATS", "shield");
		TStringArray labels = {"#STR_DRL_UI_STAT_HP", "#STR_DRL_UI_STAT_BULLET", "#STR_DRL_UI_STAT_MELEE", "#STR_DRL_UI_STAT_INFECTED", "#STR_DRL_UI_STAT_GRENADE"};
		float tw = (w - 4 * 8) / 5;
		int tileFill = INT_UIF.Color(INT_UIF_ThemeKeys.TILE);
		int tileBorder = INT_UIF.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		for (int i = 0; i < labels.Count(); i++)
		{
			float tx = i * (tw + 8);
			INT_UIF_Surface.Box(m_StatsBox, tx, 18, tw, 46, 8, tileFill, tileBorder);
			INT_UIF_Draw.Text(m_StatsBox, tx + 10, 22, tw - 16, 14, labels.Get(i), 10, faint);
			TextWidget val = INT_UIF_Draw.Text(m_StatsBox, tx + 10, 37, tw - 16, 22, "—", 15, text);
			val.SetBold(true);
			m_StatValues.Insert(val);
		}

		m_SlotsRow = INT_UIF_Draw.Frame(page, 0, 0, w, 24, "DRL_SlotsRow");
		INT_UIF_Draw.Caption(m_SlotsRow, 2, 5, 150, "#STR_DRL_UI_SLOTS_TITLE", "list");
		m_SlotIcons = INT_UIF_Draw.Frame(m_SlotsRow, 160, 0, w - 160, 24, "DRL_SlotIcons");
		m_SlotsNone = INT_UIF_Draw.Text(m_SlotsRow, 160, 3, w - 160, 18, "#STR_DRL_UI_NO_SLOTS", 12, faint);
	}

	protected void BuildRecraftPage(Widget page, float w)
	{
		m_RecraftList.Build(page, 0, 0, w, m_PageMaxH - 60, ROW_H, ROW_GAP, "DRL_Recraft");
		m_RecraftList.SetEmptyText("#STR_DRL_UI_RECRAFT_EMPTY");
		m_RecraftList.OnHover().Insert(OnRecraftHover);
		m_RecraftFoot = INT_UIF_Draw.Frame(page, 0, m_PageMaxH - 52, w, 52, "DRL_RecraftFoot");
		m_RecraftBar = INT_UIF_ProgressBar.Create(m_Bag, m_RecraftFoot, 0, 0, w, 4, 0, "amber");
		m_RecraftBar.Show(false);
		INT_UIF_Draw.Icon(m_RecraftFoot, 2, 24, 14, "clock", INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		m_RecraftTime = INT_UIF_Draw.Text(m_RecraftFoot, 22, 12, w - 260, 38, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_RecraftBtn = INT_UIF_Button.Create(m_Bag, m_RecraftFoot, w - 230, 10, 230, 42, "#STR_DRL_RECRAFT", INT_UIF_Variant.WARNING, "refresh");
		m_RecraftBtn.OnClick().Insert(OnRecraftClick);
		DRL_UIKitUtil.Expose(m_RecraftBtn, "DRL_RecraftBtn");
	}

	protected void BuildFooter(Widget parent, float y, float w)
	{
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		m_CountMinus = INT_UIF_IconButton.Create(m_Bag, parent, 0, y, 36, "minus", INT_UIF_Variant.SECONDARY);
		m_CountMinus.OnClick().Insert(OnCountMinus);
		DRL_UIKitUtil.Expose(m_CountMinus, "DRL_CountMinus");
		INT_UIF_Surface.Box(parent, 44, y, 64, 36, 8, INT_UIF.Color(INT_UIF_ThemeKeys.TILE), ARGB(24, 255, 255, 255));
		m_CountText = INT_UIF_Draw.Text(parent, 44, y, 64, 36, "1", 16, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.CENTER);
		m_CountText.SetBold(true);
		m_CountPlus = INT_UIF_IconButton.Create(m_Bag, parent, 116, y, 36, "plus", INT_UIF_Variant.SECONDARY);
		m_CountPlus.OnClick().Insert(OnCountPlus);
		DRL_UIKitUtil.Expose(m_CountPlus, "DRL_CountPlus");
		INT_UIF_Draw.Text(parent, 166, y, 170, 16, "#STR_DRL_UI_COUNT", 11, faint);
		m_TotalText = INT_UIF_Draw.Text(parent, 166, y + 16, 200, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		INT_UIF_Draw.Text(parent, w - 200, y, 200, 16, "#STR_DRL_UI_TOTAL_TIME", 11, faint, INT_UIF_Align.RIGHT);
		m_FootTime = INT_UIF_Draw.Text(parent, w - 200, y + 16, 200, 20, "", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.RIGHT);
		m_FootTime.SetBold(true);
		m_CraftBar = INT_UIF_ProgressBar.Create(m_Bag, parent, 0, y + 44, w, 4, 0, "teal");
		m_CraftBar.Show(false);
		m_CraftBtn = INT_UIF_Button.Create(m_Bag, parent, 0, y + 52, w, 52, "#STR_DRL_START", INT_UIF_Variant.PRIMARY, "gear");
		m_CraftBtn.OnClick().Insert(OnCraftClick);
		DRL_UIKitUtil.Expose(m_CraftBtn, "DRL_CraftBtn");
	}

	// Высота вкладок зависит от того, есть ли у рецепта варианты результата.
	protected void LayoutDetails(bool hasVariants)
	{
		float w = DETAILS_W;
		float footY = GetBodyH() - DETAILS_FOOT_H;
		float tabsY = m_VariantsY;
		if (hasVariants)
			tabsY = m_VariantsY + VARIANT_TILE + 28;
		float hostH = footY - 8 - tabsY;
		float pageH = hostH - 46;
		INT_UIF_Draw.Place(m_TabsHost, 0, tabsY, w, hostH);
		INT_UIF_Draw.Resize(m_PageComp, w, pageH);
		INT_UIF_Draw.Resize(m_PageInfo, w, pageH);
		INT_UIF_Draw.Resize(m_PageRecraft, w, pageH);
		m_CompList.SetViewportH(pageH);
		INT_UIF_Draw.Place(m_SlotsRow, 0, pageH - 26, w, 24);
		INT_UIF_Draw.Place(m_StatsBox, 0, pageH - 26 - 8 - 64, w, 64);
		INT_UIF_Draw.Resize(m_RecipeDesc, w - 4, pageH - 26 - 8 - 64 - 8 - 46);
		m_RecraftList.SetViewportH(pageH - 60);
		INT_UIF_Draw.Move(m_RecraftFoot, 0, pageH - 52);
	}

	override void OnReady()
	{
		BuildCategories();
		RefreshQueue();
	}

	override void OnShow()
	{
		super.OnShow();
		HookProcesses();
		RequestActiveCraftProcesses();
	}

	override void OnHide()
	{
		super.OnHide();
		UnhookProcesses();
		StopCraftSound();
		HideHoverCard();
		ReleaseModels();
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb)
			pb.DRL_SetCurrentWorkbench(null);
	}

	protected void ReleaseModels()
	{
		if (m_Grid)
			m_Grid.ClearModels();
		if (m_Stage)
			m_Stage.Clear();
		if (m_Variants)
			m_Variants.Clear();
		if (m_CompList)
			m_CompList.ClearModels();
		if (m_RecraftList)
			m_RecraftList.ClearModels();
		if (m_Popover)
			m_Popover.Close();
		if (m_Queue)
			m_Queue.Clear();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_Grid)
			m_Grid.Tick(timeslice);
		if (m_Stage)
			m_Stage.Tick(timeslice);
		TickProcesses(timeslice);
		TickReCraft(timeslice);
		m_SlotTimer = m_SlotTimer + timeslice;
		if (m_SlotTimer >= 1.0)
		{
			m_SlotTimer = 0;
			UpdateSlotInfo();
		}
	}

	// =================================================================================
	// Шапка
	// =================================================================================
	protected void UpdateSlotInfo()
	{
		if (!m_SlotsBadge)
			return;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
		{
			m_SlotsBadge.SetText("#STR_DRL_INV_SLOTS_EMPTY");
			return;
		}
		int totalCells = 0;
		int usedCells = 0;
		DRL_CraftInventoryHelper.CalculateInventorySlots(pb, m_ContextWorkbench, totalCells, usedCells);
		int freeCells = Math.Max(totalCells - usedCells, 0);
		m_SlotsBadge.SetText(DRL_UIText.Fmt("#STR_DRL_UI_SLOTS", freeCells.ToString(), totalCells.ToString()));
	}

	protected void OnLearningClick(INT_UIF_Button b)
	{
		// Новое окно крафта открывает новое окно изучения.
		CloseNow();
		GetGame().GetUIManager().EnterScriptedMenu(DRL_UI_MENU_LEARNING, null);
	}

	// =================================================================================
	// Категории (логика = DRL_CraftMenuUI.BuildCategoriesPanel / SelectCategory)
	// =================================================================================
	protected void BuildCategories()
	{
		DRL_Craft_Config cfg = Cfg();
		array<ref DRL_UICategoryEntry> entries = new array<ref DRL_UICategoryEntry>();
		if (!cfg || !cfg.Categories)
		{
			m_Sidebar.SetEntries(entries, -1);
			SelectCategory(-1);
			return;
		}
		int defaultEntry = -1;
		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			DRL_Craft_Category cat = cfg.Categories[i];
			if (!cat)
				continue;
			int count = CountCategoryRecipes(i);
			if (count <= 0)
				continue;
			string icon = "";
			if (cat.Icon != "")
				icon = DRL_CraftUIHelper.ResolveImagePath(cat.Icon);
			string name = cat.Name;
			if (name == "")
				name = "#STR_DRL_ALL_RECIPES";
			entries.Insert(new DRL_UICategoryEntry(i, name, icon, count));
			if (defaultEntry < 0 && cat.ShowAll == 1)
				defaultEntry = entries.Count() - 1;
		}
		if (defaultEntry < 0 && entries.Count() > 0)
			defaultEntry = 0;
		m_Sidebar.SetEntries(entries, defaultEntry);
		int key = -1;
		DRL_UICategoryEntry def = m_Sidebar.GetEntry(defaultEntry);
		if (def)
			key = def.m_Key;
		SelectCategory(key);
	}

	protected void OnCategorySelected(int entryIndex, DRL_UISidebar sidebar)
	{
		DRL_UICategoryEntry e = sidebar.GetEntry(entryIndex);
		if (e)
			SelectCategory(e.m_Key);
	}

	void SelectCategory(int idx)
	{
		DeselectRecipe();
		m_CurrentCategoryIdx = idx;
		DRL_Craft_Category cat = GetCurrentCategory();
		string title = "#STR_DRL_ALL_RECIPES";
		if (cat && cat.Name != "")
			title = cat.Name;
		m_ListTitle.SetText(title);
		RebuildRecipes();
	}

	DRL_Craft_Category GetCurrentCategory()
	{
		DRL_Craft_Config cfg = Cfg();
		if (!cfg || !cfg.Categories)
			return null;
		if (m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg.Categories.Count())
			return cfg.Categories[m_CurrentCategoryIdx];
		return null;
	}

	bool RecipeAllowedByContext(DRL_Craft_Recipe r)
	{
		if (!r)
			return false;
		if (m_ContextWorkbench)
		{
			if (!r.WorkbenchClasses)
				return false;
			for (int i = 0; i < r.WorkbenchClasses.Count(); i++)
			{
				if (r.WorkbenchClasses[i] == m_WorkbenchCls)
					return true;
			}
			return false;
		}
		return r.ShowInFreeMenu == 1;
	}

	bool RecipePassesCategoryIdx(DRL_Craft_Recipe r, int catIdx)
	{
		DRL_Craft_Config cfg = Cfg();
		if (!cfg || catIdx < 0 || catIdx >= cfg.Categories.Count())
			return true;
		DRL_Craft_Category cat = cfg.Categories[catIdx];
		if (!cat)
			return true;
		if (cat.ShowAll == 1)
			return true;
		if (cat.UseList == 1)
		{
			if (!cat.List)
				return false;
			for (int i = 0; i < cat.List.Count(); i++)
			{
				if (r && r.GetId() == cat.List[i])
					return true;
			}
			return false;
		}
		return true;
	}

	protected int CountCategoryRecipes(int catIdx)
	{
		if (!g_Craft_Config)
			return 0;
		array<ref DRL_Craft_Recipe> all = g_Craft_Config.GetAll();
		if (!all)
			return 0;
		int n = 0;
		for (int i = 0; i < all.Count(); i++)
		{
			DRL_Craft_Recipe r = all[i];
			if (!r || !RecipeAllowedByContext(r) || !RecipePassesCategoryIdx(r, catIdx))
				continue;
			n++;
		}
		return n;
	}

	// Категория рецепта для фонов/звука (= DRL_FindCategoryForRecipe).
	protected DRL_Craft_Category FindCategoryForRecipe(DRL_Craft_Recipe recipe)
	{
		DRL_Craft_Config cfg = Cfg();
		if (!recipe || !cfg || !cfg.Categories)
			return null;
		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			DRL_Craft_Category cat = cfg.Categories[i];
			if (!cat || cat.UseList != 1 || !cat.List)
				continue;
			for (int j = 0; j < cat.List.Count(); j++)
			{
				if (cat.List[j] == recipe.RecipeId)
					return cat;
			}
		}
		return null;
	}

	// Категория-контекст выбранного рецепта: текущая, если она не «все»,
	// иначе категория из списков (как HandleRecipeSelectionClick).
	protected DRL_Craft_Category ContextCategoryFor(DRL_Craft_Recipe r)
	{
		DRL_Craft_Category selected = GetCurrentCategory();
		if (selected && selected.ShowAll != 1)
			return selected;
		return FindCategoryForRecipe(r);
	}

	// =================================================================================
	// Рецепты (логика = DRL_CraftMenuUI.BuildRecipesList)
	// =================================================================================
	protected void OnSearchChanged(string text, INT_UIF_TextInput input)
	{
		string q = DRL_CraftUIHelper.CleanSearchQuery(text, "#STR_SEARCH_RECIPE");
		q.ToLower();
		if (q == m_Query)
			return;
		m_Query = q;
		RebuildRecipes();
	}

	protected void RebuildRecipes()
	{
		array<ref DRL_UIRecipeEntry> entries = new array<ref DRL_UIRecipeEntry>();
		DRL_Craft_Config cfg = Cfg();
		if (cfg)
		{
			array<ref DRL_Craft_Recipe> all = cfg.GetAll();
			array<EntityAI> cachedItems = new array<EntityAI>();
			GetInventorySnapshot(cachedItems);
			DRL_Craft_Category curCat = GetCurrentCategory();
			if (all)
			{
				for (int i = 0; i < all.Count(); i++)
				{
					DRL_Craft_Recipe r = all[i];
					if (!r)
						continue;
					string rn = r.RecipeName;
					rn.ToLower();
					if (m_Query != "" && rn != "" && rn.IndexOf(m_Query) == -1)
						continue;
					if (!RecipeAllowedByContext(r))
						continue;
					if (!RecipePassesCategoryIdx(r, m_CurrentCategoryIdx))
						continue;
					if (curCat && curCat.ShowOnlyAvailableCraft == 1)
					{
						if (!DRL_LearnState.HasLearned(r.RecipeId))
							continue;
					}
					entries.Insert(MakeEntry(r, cachedItems));
				}
			}
		}
		m_Grid.SetItems(entries, m_SelectedRecipe);
		m_ListSub.SetText(DRL_UIText.Fmt("#STR_DRL_UI_RECIPES_COUNT", entries.Count().ToString()));
	}

	protected DRL_UIRecipeEntry MakeEntry(DRL_Craft_Recipe r, array<EntityAI> cachedItems)
	{
		DRL_UIRecipeEntry e = new DRL_UIRecipeEntry();
		e.m_Recipe = r;
		e.m_Name = r.RecipeName;
		e.m_PreviewClass = r.Result;
		if (r.PreviewClassname != "")
			e.m_PreviewClass = r.PreviewClassname;
		e.m_IconPath = "";
		if (r.CraftIcon != "")
			e.m_IconPath = DRL_CraftUIHelper.ResolveImagePath(r.CraftIcon);
		e.m_TimeSec = r.CraftTimeSec;
		e.m_ResultCount = r.ResultCount;
		e.m_Status = RecipeStatus(r, cachedItems);
		return e;
	}

	protected int RecipeStatus(DRL_Craft_Recipe r, array<EntityAI> cachedItems)
	{
		if (!DRL_LearnState.HasLearned(r.RecipeId))
			return DRL_UI_STATUS_LOCKED;
		if (CanCraftRecipe(r, cachedItems))
			return DRL_UI_STATUS_CAN;
		return DRL_UI_STATUS_CANNOT;
	}

	// = RefreshRecipeListColors: статусы карточек после изменения инвентаря/выбора.
	protected void RefreshStatuses()
	{
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		int n = m_Grid.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRecipeEntry e = m_Grid.GetItem(i);
			if (e && e.m_Recipe)
				e.m_Status = RecipeStatus(e.m_Recipe, cachedItems);
		}
		m_Grid.RefreshValues();
	}

	// =================================================================================
	// Проверки крафта (= DRL_CraftMenuUI)
	// =================================================================================
	void GetInventorySnapshot(out array<EntityAI> items)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		DRL_CraftInventoryHelper.GetInventorySnapshot(p, m_ContextWorkbench, items);
	}

	int ComputeHaveForClass(string cls, int minHealthFrac, array<EntityAI> cachedItems = null, float quantityFrac = 0.0, bool useInheritance = false)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftInventoryHelper.ComputeHaveForClass(cls, minHealthFrac, cachedItems, quantityFrac, useInheritance, p, m_ContextWorkbench);
	}

	string GetActiveComponentClass(DRL_Craft_Component comp, int compIdx)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftMergedHelper.GetActiveComponentClass(comp, compIdx, m_SelectedComponentVariations, m_SelectedRecipe, p, m_ContextWorkbench);
	}

	// compIdx < 0 — без выбранной вариации (как ComputeHaveForComponent(c, -1)).
	int ComputeHaveForComponent(DRL_Craft_Component c, int compIdx = -1, array<EntityAI> cachedItems = null)
	{
		string activeCls = "";
		if (compIdx >= 0)
			activeCls = GetActiveComponentClass(c, compIdx);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftInventoryHelper.ComputeHaveForComponent(c, activeCls, cachedItems, p, m_ContextWorkbench);
	}

	int NeedPerCraft(DRL_Craft_Component c, int compIdx = -1)
	{
		string activeCls = "";
		if (compIdx >= 0)
			activeCls = GetActiveComponentClass(c, compIdx);
		return DRL_CraftInventoryHelper.NeedPerCraft(c, activeCls);
	}

	int NeedForCount(DRL_Craft_Component c, int countN, int compIdx = -1)
	{
		string activeCls = "";
		if (compIdx >= 0)
			activeCls = GetActiveComponentClass(c, compIdx);
		return DRL_CraftInventoryHelper.NeedForCount(c, countN, activeCls);
	}

	int NeedForClassCount(string cls, DRL_Craft_Component c, int countN)
	{
		return DRL_CraftInventoryHelper.NeedForClassCount(cls, c, countN);
	}

	DRL_Craft_Component GetComponentByKey(int key)
	{
		return DRL_CraftMergedHelper.GetComponentByKey(key, m_SelectedRecipe, Cfg());
	}

	array<ref DRL_CraftMergedComponent> BuildMergedComponents(DRL_Craft_Recipe r)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		array<int> attIds = GetSelectedAttachedRecipeIdsFlattened();
		string resCls = "";
		if (r == m_SelectedRecipe && m_SelectedResultClass != "")
			resCls = m_SelectedResultClass;
		else if (r)
			resCls = r.Result;
		return DRL_CraftMergedHelper.BuildMergedComponents(r, m_SelectedComponentVariations, attIds, Cfg(), p, m_ContextWorkbench, resCls);
	}

	// = DRL_CraftMenuUI.CanCraftRecipe: у выбранного рецепта — слитые компоненты
	// с учётом количества, вариаций и доп. навесного.
	bool CanCraftRecipe(DRL_Craft_Recipe r, array<EntityAI> cachedItems = null)
	{
		if (!r)
			return false;
		int cnt = 1;
		if (r == m_SelectedRecipe)
			cnt = Math.Max(1, r.CurrentCraftCount);
		if (r == m_SelectedRecipe)
		{
			array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(r);
			for (int m = 0; m < mergedList.Count(); m++)
			{
				DRL_CraftMergedComponent mc = mergedList[m];
				if (!mc)
					continue;
				DRL_Craft_Component cMerged = mc.ToCraftComponent();
				if (!cMerged || cMerged.AttachmentsEx > 0)
					continue;
				string activeMergedCls = mc.Classname;
				if (activeMergedCls == "")
					activeMergedCls = cMerged.Classname;
				int haveMerged = ComputeHaveForClass(activeMergedCls, cMerged.MinHealthFrac, cachedItems, cMerged.QuantityFrac, cMerged.UseInheritance == 1);
				int needMerged = NeedForClassCount(activeMergedCls, cMerged, cnt);
				if (haveMerged < needMerged)
					return false;
			}
		}
		else if (r.CraftComponents)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c || c.AttachmentsEx > 0)
					continue;
				if (!c.IsApplicableForResult(r.Result))
					continue;
				int have = ComputeHaveForComponent(c, -1, cachedItems);
				int need = NeedForCount(c, cnt, -1);
				if (have < need)
					return false;
			}
		}
		return HasRequiredAttachments(r);
	}

	// = DRL_CraftAttachmentsController.HasRequiredAttachments: активный результат
	// берётся у выбранного рецепта для любого r (как в старом меню).
	bool HasRequiredAttachments(DRL_Craft_Recipe r)
	{
		if (!r || !r.CraftComponents)
			return true;
		string activeResult = m_SelectedResultClass;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0)
				continue;
			if (!c.IsApplicableForResult(activeResult))
				continue;
			if (!DRL_CraftComponentResolver.HasComponentRequirement(c, pb, m_ContextWorkbench, activeResult, 1))
				return false;
		}
		return true;
	}

	EntityAI FindAttachment(DRL_Craft_Component c)
	{
		if (!c)
			return null;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftComponentResolver.FindItemForComponent(c, pb, m_ContextWorkbench, m_SelectedResultClass);
	}

	bool CanCraftSelected()
	{
		if (GetProcForCurrentRecipe() != null)
			return false;
		if (!m_SelectedRecipe)
			return false;
		if (m_Procs.Count() >= MaxQueueSlots())
			return false;
		array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(m_SelectedRecipe);
		for (int i = 0; i < mergedList.Count(); i++)
		{
			DRL_CraftMergedComponent mc = mergedList[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			string activeCls = mc.Classname;
			if (activeCls == "")
				activeCls = c.Classname;
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, null, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need)
				return false;
		}
		return HasRequiredAttachments(m_SelectedRecipe);
	}

	protected int GetMaxCraftable(DRL_Craft_Recipe r)
	{
		if (!r)
			return 0;
		int maxN = 1000000;
		array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(r);
		for (int i = 0; i < mergedList.Count(); i++)
		{
			DRL_CraftMergedComponent mc = mergedList[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			int have = ComputeHaveForComponent(c, i);
			int need1 = NeedPerCraft(c, i);
			if (need1 <= 0)
				return 0;
			int n = have / need1;
			if (n < maxN)
				maxN = n;
		}
		if (maxN < 0 || maxN == 1000000)
			maxN = 0;
		return maxN;
	}

	int GetTotalCraftTimeSec(DRL_Craft_Recipe r)
	{
		if (!r)
			return 0;
		int total = r.CraftTimeSec;
		DRL_Craft_Config cfg = Cfg();
		if (cfg)
		{
			for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
			{
				int dopId = m_SelectedAttachedRecipeIds[i];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				if (dopR)
					total = total + dopR.CraftTimeSec * GetSelectedAttachedQuantity(dopId);
			}
		}
		return total;
	}

	protected int MaxQueueSlots()
	{
		DRL_Craft_Config cfg = Cfg();
		if (cfg && cfg.Settings && cfg.Settings.MaxQueueSlots > 0)
			return cfg.Settings.MaxQueueSlots;
		return 5;
	}

	// =================================================================================
	// Доп. рецепты-навесное (= DRL_CraftAttachmentsController)
	// =================================================================================
	protected void ClearSelectedAttached()
	{
		m_SelectedAttachedRecipeIds.Clear();
		m_SelectedAttachedVariants.Clear();
		m_SelectedAttachedQuantities.Clear();
	}

	int GetSelectedAttachedQuantity(int recipeId)
	{
		int q;
		if (m_SelectedAttachedQuantities.Find(recipeId, q))
			return q;
		return 1;
	}

	array<int> GetSelectedAttachedRecipeIdsFlattened()
	{
		array<int> result = new array<int>();
		for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
		{
			int id = m_SelectedAttachedRecipeIds[i];
			int qty = GetSelectedAttachedQuantity(id);
			for (int q = 0; q < qty; q++)
			{
				result.Insert(id);
			}
		}
		return result;
	}

	string GetSelectedAttachedClass(int recipeId, DRL_Craft_Recipe dopR)
	{
		string cls;
		if (m_SelectedAttachedVariants.Find(recipeId, cls))
			return cls;
		if (dopR)
			return dopR.Result;
		return "";
	}

	// Варианты доп. рецепта (= DRL_RecipeVarWidget.InitVariants).
	protected array<string> DopVariants(DRL_Craft_Recipe dopR)
	{
		array<string> res = new array<string>();
		if (!dopR)
			return res;
		if (dopR.ResultClasses && dopR.ResultClasses.Count() > 1)
		{
			foreach (string rc : dopR.ResultClasses)
			{
				res.Insert(rc);
			}
		}
		else if (dopR.Result != "")
		{
			res.Insert(dopR.Result);
		}
		return res;
	}

	protected int DopVariantIndex(int recipeId, array<string> vars)
	{
		string cls;
		if (m_SelectedAttachedVariants.Find(recipeId, cls))
		{
			int f = vars.Find(cls);
			if (f >= 0)
				return f;
		}
		return 0;
	}

	int FindConflictingAttachedRecipe(DRL_Craft_Recipe newDopR)
	{
		if (!newDopR || m_SelectedAttachedRecipeIds.Count() == 0)
			return -1;
		string newCls = newDopR.Result;
		if (newCls == "")
			return -1;
		DRL_Craft_Config cfg = Cfg();
		if (!cfg)
			return -1;

		EntityAI mainPrev = m_Stage.GetEntity();
		bool isWeaponParent = false;
		if (mainPrev && mainPrev.IsWeapon())
		{
			isWeaponParent = true;
		}
		else if (m_SelectedRecipe)
		{
			string pCls = m_SelectedRecipe.PreviewClassname;
			if (pCls == "")
				pCls = m_SelectedRecipe.Result;
			if (GetGame().IsKindOf(pCls, "Weapon_Base"))
				isWeaponParent = true;
		}

		EDRLWeaponAttachType newWeaponType = EDRLWeaponAttachType.NONE;
		if (isWeaponParent)
			newWeaponType = DRL_AttachmentHelper.GetWeaponAttachmentType(newCls);

		TStringArray newSlots = new TStringArray();
		DRL_AttachmentHelper.GetAttachmentSlots(newCls, newSlots);

		for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
		{
			int existId = m_SelectedAttachedRecipeIds[i];
			if (existId == newDopR.RecipeId)
				continue;
			DRL_Craft_Recipe existR = cfg.FindById(existId);
			if (!existR)
				continue;
			string existCls = existR.Result;
			if (existCls == "")
				continue;
			if (isWeaponParent && newWeaponType != EDRLWeaponAttachType.NONE && newWeaponType != EDRLWeaponAttachType.OTHER)
			{
				EDRLWeaponAttachType existWeaponType = DRL_AttachmentHelper.GetWeaponAttachmentType(existCls);
				if (newWeaponType == existWeaponType)
					return existId;
			}
			TStringArray existSlots = new TStringArray();
			DRL_AttachmentHelper.GetAttachmentSlots(existCls, existSlots);
			if (newSlots && existSlots && newSlots.Count() > 0 && existSlots.Count() > 0)
			{
				for (int ns = 0; ns < newSlots.Count(); ns++)
				{
					if (existSlots.Find(newSlots[ns]) > -1)
						return existId;
				}
			}
		}
		return -1;
	}

	bool CanAttachToCurrentBuild(string targetCls)
	{
		EntityAI mainPrev = m_Stage.GetEntity();
		if (targetCls == "" || !mainPrev || !mainPrev.GetInventory())
			return false;

		if (mainPrev.IsWeapon() && (GetGame().IsKindOf(targetCls, "Magazine_Base") || DRL_AttachmentHelper.GetWeaponAttachmentType(targetCls) == EDRLWeaponAttachType.MAGAZINE))
		{
			int magSlotId = InventorySlots.GetSlotIdFromString("magazine");
			if (magSlotId != InventorySlots.INVALID && mainPrev.GetInventory().HasAttachmentSlot(magSlotId))
				return true;
		}

		TStringArray targetSlots = new TStringArray();
		DRL_AttachmentHelper.GetAttachmentSlots(targetCls, targetSlots);
		if (targetSlots && targetSlots.Count() > 0)
		{
			for (int s = 0; s < targetSlots.Count(); s++)
			{
				int sId = InventorySlots.GetSlotIdFromString(targetSlots[s]);
				if (sId != InventorySlots.INVALID && mainPrev.GetInventory().HasAttachmentSlot(sId))
					return true;
			}
			array<EntityAI> children = new array<EntityAI>();
			mainPrev.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, children);
			for (int c = 0; c < children.Count(); c++)
			{
				EntityAI ch = children[c];
				if (!ch || ch == mainPrev || !ch.GetInventory())
					continue;
				for (int cs = 0; cs < targetSlots.Count(); cs++)
				{
					int csId = InventorySlots.GetSlotIdFromString(targetSlots[cs]);
					if (csId != InventorySlots.INVALID && ch.GetInventory().HasAttachmentSlot(csId))
						return true;
				}
			}
		}
		return false;
	}

	// = HandleDopWidgetClick
	protected void HandleDopAction(int k, string action, int arg)
	{
		if (k < 0 || k >= m_DopRecipes.Count())
			return;
		DRL_Craft_Recipe dopR = m_DopRecipes[k];
		if (!dopR)
			return;
		int id = dopR.RecipeId;
		bool isChecked = m_SelectedAttachedRecipeIds.Find(id) > -1;

		if (action == "minus" || action == "plus")
		{
			if (!isChecked)
				return;
			int delta = 1;
			if (action == "minus")
				delta = -1;
			int q = GetSelectedAttachedQuantity(id);
			int nq = Math.Clamp(q + delta, 1, MAX_ATTACHED_QUANTITY);
			if (nq != q)
			{
				m_SelectedAttachedQuantities.Set(id, nq);
				OnAttachedSelectionChanged();
			}
			return;
		}

		if (action == "var")
		{
			array<string> dvars = DopVariants(dopR);
			if (arg < 0 || arg >= dvars.Count())
				return;
			m_SelectedAttachedVariants.Set(id, dvars[arg]);
			if (isChecked)
				RebuildMainPreview();
			BuildCompRows(true);
			return;
		}

		if (action == "info")
		{
			if (RecipeAllowedByContext(dopR))
				SelectRecipeDirectly(dopR);
			return;
		}

		// Строка или чекбокс: включить / выключить.
		if (!DRL_LearnState.HasLearned(id))
			return;
		if (isChecked)
		{
			int remIdx = m_SelectedAttachedRecipeIds.Find(id);
			if (remIdx > -1)
				m_SelectedAttachedRecipeIds.Remove(remIdx);
			m_SelectedAttachedQuantities.Remove(id);
			OnAttachedSelectionChanged();
			return;
		}
		if (!CanAttachToCurrentBuild(dopR.Result))
			return;
		int conflictId = FindConflictingAttachedRecipe(dopR);
		if (conflictId != -1)
		{
			int cIdx = m_SelectedAttachedRecipeIds.Find(conflictId);
			if (cIdx > -1)
				m_SelectedAttachedRecipeIds.Remove(cIdx);
			m_SelectedAttachedQuantities.Remove(conflictId);
		}
		m_SelectedAttachedRecipeIds.Insert(id);
		m_SelectedAttachedQuantities.Set(id, 1);
		OnAttachedSelectionChanged();
	}

	protected void OnAttachedSelectionChanged()
	{
		FillDetails();
	}

	// =================================================================================
	// Выбор рецепта
	// =================================================================================
	protected void OnGridSelected(int index, DRL_UIRecipeGrid grid)
	{
		DRL_UIRecipeEntry e = grid.GetItem(index);
		if (e)
			SelectRecipe(e.m_Recipe);
		else
			DeselectRecipe();
	}

	// = HandleRecipeSelectionClick
	void SelectRecipe(DRL_Craft_Recipe r)
	{
		if (!r)
		{
			DeselectRecipe();
			return;
		}
		if (m_SelectedRecipe != r)
			m_SelectedComponentVariations.Clear();
		m_SelectedRecipe = r;
		ClearSelectedAttached();
		ClosePopover();
		m_SelectedRecipe.CurrentCraftCount = 1;
		m_SelectedResultClass = "";
		FillDetails();
	}

	// = SelectRecipeDirectly: переход из требований, доп. навесного, очереди.
	void SelectRecipeDirectly(DRL_Craft_Recipe r)
	{
		if (!r)
			return;
		m_SelectedComponentVariations.Clear();
		m_SelectedRecipe = r;
		ClearSelectedAttached();
		m_SelectedResultClass = "";
		ClosePopover();
		m_SelectedRecipe.CurrentCraftCount = 1;
		m_Grid.Select(m_Grid.IndexOfRecipe(r), false);
		FillDetails();
	}

	void DeselectRecipe()
	{
		m_SelectedRecipe = null;
		m_ShownRecipe = null;
		m_SelectedResultClass = "";
		m_SelectedComponentVariations.Clear();
		ClearSelectedAttached();
		ClosePopover();
		HideHoverCard();
		if (m_Stage)
			m_Stage.Clear();
		if (m_Variants)
			m_Variants.Clear();
		if (m_CompList)
			m_CompList.ClearModels();
		if (m_RecraftList)
			m_RecraftList.ClearModels();
		if (m_CraftBar)
			m_CraftBar.Show(false);
		if (m_DetailsBody)
			m_DetailsBody.Show(false);
		if (m_DetailsEmpty)
			m_DetailsEmpty.Show(true);
	}

	protected string ChosenResult()
	{
		if (!m_SelectedRecipe)
			return "";
		if (m_SelectedResultClass != "" && m_SelectedRecipe.ResultClasses && m_SelectedRecipe.ResultClasses.Find(m_SelectedResultClass) > -1)
			return m_SelectedResultClass;
		return m_SelectedRecipe.Result;
	}

	// =================================================================================
	// Детали (= FillRecipeDetails + BuildAttachedRecipesList + UpdateOptionalTitles)
	// =================================================================================
	protected void FillDetails()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		bool sameRecipe = r == m_ShownRecipe;
		m_ShownRecipe = r;
		m_DetailsEmpty.Show(false);
		if (!sameRecipe || !m_DetailsBody.IsVisible())
		{
			m_DetailsBody.Show(true);
			DRL_UIAnim.Reveal(m_DetailsBody, 0, 0, 0, 8, 0.24);
		}
		HideVariationList();
		HideHoverCard();

		m_RecipeTitle.SetText(r.RecipeName);
		RebuildMainPreview();
		ApplyBackdrop(r);
		FillChips(r);
		FillVariants(r);
		BuildCompRows(sameRecipe);
		FillInfoPage(r);
		BuildRecraftRows(r, sameRecipe);
		UpdateRecraftButton();
		RefreshCountUI();
		UpdateCraftButtonState();
		RefreshStatuses();
	}

	// Главное превью: PreviewClassname / Result или выбранный вариант,
	// с выбранным доп. навесным (= RebuildMainPreviewWithAttachments).
	protected void RebuildMainPreview()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		string mainCls = r.PreviewClassname;
		if (mainCls == "")
			mainCls = r.Result;
		string selRes = m_SelectedResultClass;
		if (selRes != "" && ((r.ResultClasses && r.ResultClasses.Find(selRes) > -1) || selRes == r.Result))
			mainCls = selRes;

		array<string> atts = new array<string>();
		DRL_Craft_Config cfg = Cfg();
		if (cfg)
		{
			for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
			{
				int dopId = m_SelectedAttachedRecipeIds[i];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				if (!dopR)
					continue;
				string dopCls = GetSelectedAttachedClass(dopId, dopR);
				if (dopCls == "")
					continue;
				int attQty = GetSelectedAttachedQuantity(dopId);
				for (int q = 0; q < attQty; q++)
				{
					atts.Insert(dopCls);
				}
			}
		}
		m_Stage.SetBuild(mainCls, atts);
	}

	// Фоны панели крафта из конфига (Settings.CraftFonImagePath и
	// CategoryCraftFonImagePath, «recipemanu_*») — картинки старой панели с её
	// рамками и полосами: под сценой нового окна они выглядят мусором,
	// поэтому подложка не используется (решение по дизайну, логика не меняется).
	protected void ApplyBackdrop(DRL_Craft_Recipe r)
	{
		m_Stage.SetBackdrop("");
	}

	protected void FillChips(DRL_Craft_Recipe r)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		int status = RecipeStatus(r, cachedItems);
		float x = 2;
		float w = 0;
		if (status == DRL_UI_STATUS_LOCKED)
			w = m_ChipStatus.Set("#STR_DRL_UI_CHIP_LOCKED", "lock", tm.Color(INT_UIF_ThemeKeys.AMBER));
		else if (status == DRL_UI_STATUS_CAN)
			w = m_ChipStatus.Set("#STR_DRL_UI_CHIP_CAN", "check", tm.Color(INT_UIF_ThemeKeys.GREEN));
		else
			w = m_ChipStatus.Set("#STR_DRL_UI_CHIP_CANNOT", "close", tm.Color(INT_UIF_ThemeKeys.RED));
		x = x + w + 8;
		INT_UIF_Draw.Move(m_ChipTime.m_Root, x, m_ChipY);
		w = m_ChipTime.Set(DRL_UIText.Time(r.CraftTimeSec), "clock", tm.Color(INT_UIF_ThemeKeys.TEAL));
		x = x + w + 8;
		int cnt = r.ResultCount;
		if (cnt <= 0)
			cnt = 1;
		INT_UIF_Draw.Move(m_ChipCount.m_Root, x, m_ChipY);
		m_ChipCount.Set("x" + cnt.ToString(), "", tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
	}

	// Варианты результата (= BuildSelectResultList / UpdateSelectCraftHighlight).
	protected void FillVariants(DRL_Craft_Recipe r)
	{
		bool has = r.ResultClasses && r.ResultClasses.Count() > 0;
		m_Variants.Show(has);
		if (has)
		{
			int sel = -1;
			if (m_SelectedResultClass != "")
				sel = r.ResultClasses.Find(m_SelectedResultClass);
			if (sel < 0 && r.Result != "")
				sel = r.ResultClasses.Find(r.Result);
			if (sel < 0)
				sel = 0;
			m_Variants.SetItems(r.ResultClasses, sel);
		}
		else
		{
			m_Variants.Clear();
		}
		LayoutDetails(has);
	}

	// = HandleSelectResultClick
	protected void OnVariantSelected(int index, DRL_UIVariantStrip strip)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r || !r.ResultClasses || index < 0 || index >= r.ResultClasses.Count())
			return;
		string cls = r.ResultClasses[index];
		if (cls == "")
			return;
		m_SelectedResultClass = cls;
		FillDetails();
	}

	// --- вкладки ------------------------------------------------------------------------
	protected void OnTabChanged(int index, INT_UIF_Tabs tabs)
	{
		ShowPage(index, true);
	}

	protected void ShowPage(int index, bool animate)
	{
		m_PageComp.Show(index == TAB_COMP);
		m_PageInfo.Show(index == TAB_INFO);
		m_PageRecraft.Show(index == TAB_RECRAFT);
		HideHoverCard();
		if (!animate)
			return;
		Widget page = m_PageComp;
		if (index == TAB_INFO)
			page = m_PageInfo;
		else if (index == TAB_RECRAFT)
			page = m_PageRecraft;
		DRL_UIAnim.Reveal(page, 0, 46, 0, 8, 0.2);
	}

	// --- «Состав»: ингредиенты, инструменты, доп. навесное ------------------------------
	protected void BuildCompRows(bool keepScroll)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		array<ref DRL_UIRowData> rows = new array<ref DRL_UIRowData>();
		m_ToolComps.Clear();
		m_DopRecipes.Clear();
		if (!r)
		{
			m_Merged.Clear();
			m_CompList.SetItems(rows);
			return;
		}
		m_Merged = BuildMergedComponents(r);
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int green = tm.Color(INT_UIF_ThemeKeys.GREEN);
		int amber = tm.Color(INT_UIF_ThemeKeys.AMBER);
		int red = tm.Color(INT_UIF_ThemeKeys.RED);

		// Ингредиенты (= BuildIngredientsListUI).
		bool ingrHeader = false;
		for (int i = 0; i < m_Merged.Count(); i++)
		{
			DRL_CraftMergedComponent mc = m_Merged[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			if (!ingrHeader)
			{
				rows.Insert(DRL_UIRowData.Header("#STR_DRL_Craft_Ingridient_Titel"));
				ingrHeader = true;
			}
			string activeCls = mc.Classname;
			if (activeCls == "")
				activeCls = c.Classname;
			array<string> variations = DRL_CraftMergedHelper.GetVariationsForComponent(c);
			bool hasVariations = variations && variations.Count() > 1 && c.EnableVariationSelection == 1;
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, cachedItems, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, r.CurrentCraftCount);

			DRL_UIRowData d = new DRL_UIRowData();
			d.m_Title = DRL_UIText.DisplayName(activeCls);
			if (hasVariations)
			{
				d.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_PICK_VARIANT");
				d.m_Badge = "swap";
			}
			d.m_Right = DRL_CraftInventoryHelper.FormatIngredientAmount(activeCls, c, have, need);
			int col = red;
			if (have >= need)
				col = green;
			else if (have > 0)
				col = amber;
			d.m_RightColor = col;
			d.m_Accent = col;
			d.m_PreviewClass = activeCls;
			d.m_Clickable = hasVariations;
			d.m_Tag = i;
			rows.Insert(d);
		}

		// Инструменты и оснащение (= BuildAtachIngridientListUI).
		string activeResult = m_SelectedResultClass;
		bool toolHeader = false;
		if (r.CraftComponents)
		{
			for (int t = 0; t < r.CraftComponents.Count(); t++)
			{
				DRL_Craft_Component tc = r.CraftComponents[t];
				if (!tc || tc.AttachmentsEx <= 0)
					continue;
				if (!tc.IsApplicableForResult(activeResult))
					continue;
				string token = tc.Classname;
				if (token == "" && tc.Classnames && tc.Classnames.Count() > 0)
					token = tc.Classnames[0];
				if (token == "")
					continue;
				EntityAI present = FindAttachment(tc);
				string prevCls = "";
				if (present)
					prevCls = present.GetType();
				else
					prevCls = DRL_ItemTypeHelper.GetSafePreviewClass(token);
				if (!toolHeader)
				{
					rows.Insert(DRL_UIRowData.Header("#STR_WB_TOOLS"));
					toolHeader = true;
				}
				DRL_UIRowData td = new DRL_UIRowData();
				td.m_Title = ToolName(token);
				if (tc.ApplyDamageFrac > 0.0)
					td.m_Sub = DRL_UIText.Label("#STR_DRL_DAMAGE", DRL_UIText.Pct(tc.ApplyDamageFrac));
				if (present)
				{
					td.m_Right = DRL_UIText.Tr("#STR_DRL_ATTACHMENT_INSTALLED");
					td.m_RightColor = green;
					td.m_Accent = green;
				}
				else
				{
					td.m_Right = DRL_UIText.Tr("#STR_DRL_ATTACHMENT_MISSING");
					td.m_RightColor = red;
					td.m_Accent = red;
				}
				td.m_PreviewClass = prevCls;
				td.m_Glyph = "key";
				td.m_Clickable = false;
				td.m_Tag = TAG_TOOL + m_ToolComps.Count();
				m_ToolComps.Insert(tc);
				rows.Insert(td);
			}
		}

		// Доп. рецепты-навесное (= BuildAttachedRecipesList + UpdateAttachedSlotsAvailability).
		DRL_Craft_Config cfg = Cfg();
		if (r.HasAttachedRecipes() && cfg)
		{
			rows.Insert(DRL_UIRowData.Header("#STR_DRL_UI_SEC_DOP"));
			for (int k = 0; k < r.AttachedRecipeIds.Count(); k++)
			{
				int attachedId = r.AttachedRecipeIds[k];
				DRL_Craft_Recipe dopR = cfg.FindById(attachedId);
				if (!dopR)
					continue;
				bool isChecked = m_SelectedAttachedRecipeIds.Find(attachedId) > -1;
				bool allowed = RecipeAllowedByContext(dopR);
				bool learned = DRL_LearnState.HasLearned(attachedId);
				array<string> dvars = DopVariants(dopR);
				int dvi = DopVariantIndex(attachedId, dvars);
				bool slotOk = isChecked;
				if (!slotOk)
					slotOk = CanAttachToCurrentBuild(dopR.Result);

				DRL_UIRowData dd = new DRL_UIRowData();
				dd.m_Title = dopR.RecipeName;
				if (learned)
				{
					dd.m_Check = 0;
					if (isChecked)
						dd.m_Check = 1;
				}
				else
				{
					dd.m_Locked = true;
				}
				if (isChecked)
					dd.m_Qty = GetSelectedAttachedQuantity(attachedId);
				if (dvars.Count() > 1)
				{
					int vc = dvars.Count();
					if (vc > DRL_UI_ROW_MAX_VARS)
						vc = DRL_UI_ROW_MAX_VARS;
					dd.m_VarCount = vc;
					dd.m_VarSelected = dvi;
				}
				dd.m_Info = true;
				dd.m_InfoEnabled = allowed;
				dd.m_Dim = learned && !slotOk;
				if (!learned)
					dd.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_CHIP_LOCKED");
				else if (!slotOk)
					dd.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_NO_SLOT");
				else
				{
					string dopTime = DRL_UIText.Time(dopR.CraftTimeSec);
					dd.m_Sub = DRL_UIText.Fmt("#STR_DRL_UI_DOP_TIME", dopTime);
				}
				if (dopR.CraftIcon != "")
					dd.m_IconPath = DRL_CraftUIHelper.ResolveImagePath(dopR.CraftIcon);
				else if (dvi < dvars.Count())
					dd.m_PreviewClass = dvars[dvi];
				if (isChecked)
					dd.m_Accent = green;
				else if (!learned)
					dd.m_Accent = red;
				dd.m_Clickable = learned;
				dd.m_Tag = TAG_DOP + m_DopRecipes.Count();
				m_DopRecipes.Insert(dopR);
				rows.Insert(dd);
			}
		}
		m_CompList.SetItems(rows, keepScroll);
	}

	// Имя инструмента: класс предмета или слот (как в панели требований).
	protected string ToolName(string token)
	{
		string n = "";
		if (DRL_CraftUIHelper.GetDisplayNameForClass(token, n) && n != "")
			return n;
		int slotId = InventorySlots.GetSlotIdFromString(token);
		if (slotId > -1)
			return InventorySlots.GetSlotDisplayName(slotId);
		return token;
	}

	protected void OnCompAction(int index, string action, int arg, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d)
			return;
		int tag = d.m_Tag;
		if (tag >= TAG_DOP)
		{
			HandleDopAction(tag - TAG_DOP, action, arg);
			return;
		}
		if (tag >= TAG_TOOL || tag < 0 || action != "row")
			return;
		if (tag < m_Merged.Count())
			ToggleVariationList(m_Merged[tag]);
	}

	// Наведение на ингредиент / инструмент — подсказка поверх сцены.
	protected void OnCompHover(int index, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d || d.m_Tag < 0 || d.m_Tag >= TAG_DOP)
		{
			HideHoverCard();
			return;
		}
		DRL_Craft_Component comp = null;
		if (d.m_Tag >= TAG_TOOL)
		{
			int ti = d.m_Tag - TAG_TOOL;
			if (ti < m_ToolComps.Count())
				comp = m_ToolComps[ti];
		}
		else if (d.m_Tag < m_Merged.Count() && m_Merged[d.m_Tag])
		{
			comp = m_Merged[d.m_Tag].ToCraftComponent();
		}
		if (!comp)
		{
			HideHoverCard();
			return;
		}
		ShowHoverCard(comp.Classname, comp);
	}

	protected void OnRecraftHover(int index, DRL_UIRowList list)
	{
		HideHoverCard();
	}

	protected void ShowHoverCard(string cls, DRL_Craft_Component comp)
	{
		m_HoverCard.ShowFor(cls, comp);
		m_Stage.SetHidden(DRL_UIStage.HIDE_HOVERCARD, true);
	}

	protected void HideHoverCard()
	{
		if (m_HoverCard)
			m_HoverCard.Hide();
		if (m_Stage)
			m_Stage.SetHidden(DRL_UIStage.HIDE_HOVERCARD, false);
	}

	// --- вариации ингредиента (= DRL_CraftVariationsController) -------------------------
	protected void ToggleVariationList(DRL_CraftMergedComponent mc)
	{
		if (!mc || mc.EnableVariationSelection != 1)
			return;
		array<string> variationsMain = DRL_CraftMergedHelper.GetVariationsForMergedComponent(mc, m_SelectedRecipe, Cfg());
		if (!variationsMain || variationsMain.Count() <= 1)
			return;
		if (m_Popover.GetKind() == "var" && m_ActiveVariationMerged == mc)
		{
			HideVariationList();
			return;
		}
		BuildVariationList(mc);
	}

	protected void BuildVariationList(DRL_CraftMergedComponent mc)
	{
		m_VarInfos.Clear();
		array<ref DRL_UIRowData> rows = new array<ref DRL_UIRowData>();
		if (!mc)
			return;
		DRL_Craft_Component comp = mc.ToCraftComponent();
		if (!comp)
			return;
		array<string> variations = DRL_CraftMergedHelper.GetVariationsForMergedComponent(mc, m_SelectedRecipe, Cfg());
		if (!variations || variations.Count() <= 1)
		{
			HideVariationList();
			return;
		}
		m_ActiveVariationMerged = mc;
		array<EntityAI> items = new array<EntityAI>();
		GetInventorySnapshot(items);
		string activeCls = mc.Classname;
		if (activeCls == "")
			activeCls = comp.Classname;

		bool hasIndividualSubComps = mc.ComponentKeys && mc.ComponentKeys.Count() > 1;
		if (hasIndividualSubComps)
		{
			rows.Insert(DRL_UIRowData.Header("#STR_DRL_APPLY_TO_ALL_SAME_TYPE"));
			for (int gk = 0; gk < variations.Count(); gk++)
			{
				AddVarRow(rows, mc, -1, variations[gk], gk, activeCls, comp, items);
			}
			rows.Insert(DRL_UIRowData.Header("#STR_DRL_SELECT_BY_INDIVIDUAL_PARTS"));
			for (int sk = 0; sk < mc.ComponentKeys.Count(); sk++)
			{
				int targetKey = mc.ComponentKeys[sk];
				DRL_Craft_Component subComp = GetComponentByKey(targetKey);
				if (!subComp)
					continue;
				int rId = targetKey / 100;
				DRL_Craft_Recipe srcRecipe = null;
				if (m_SelectedRecipe && m_SelectedRecipe.RecipeId == rId)
					srcRecipe = m_SelectedRecipe;
				else if (Cfg())
					srcRecipe = Cfg().FindById(rId);
				int partNum = sk + 1;
				string partTitle = DRL_UIText.Tr("#STR_DRL_PART") + " " + partNum.ToString();
				if (srcRecipe)
					partTitle = srcRecipe.RecipeName;
				string subActiveCls = GetActiveComponentClass(subComp, targetKey);
				if (subActiveCls == "")
					subActiveCls = subComp.Classname;
				string badge = "";
				if (subComp.QuantityFrac > 0.0)
				{
					int partPct = Math.Round(subComp.QuantityFrac * 100);
					badge = "[ " + partPct.ToString() + "% ]";
				}
				else
				{
					int craftCnt = 1;
					if (m_SelectedRecipe)
						craftCnt = Math.Max(1, m_SelectedRecipe.CurrentCraftCount);
					int pieces = subComp.Amount * craftCnt;
					badge = string.Format(DRL_UIText.Tr("#STR_DRL_QTY_PIECES_FORMAT"), pieces);
				}
				rows.Insert(DRL_UIRowData.Header("• " + partTitle, badge));
				array<string> subVars = DRL_CraftMergedHelper.GetVariationsForComponent(subComp);
				for (int sv = 0; sv < subVars.Count(); sv++)
				{
					AddVarRow(rows, mc, targetKey, subVars[sv], sv, subActiveCls, subComp, items);
				}
			}
		}
		else
		{
			int singleKey = -1;
			if (mc.ComponentKeys && mc.ComponentKeys.Count() > 0)
				singleKey = mc.ComponentKeys[0];
			for (int sk0 = 0; sk0 < variations.Count(); sk0++)
			{
				AddVarRow(rows, mc, singleKey, variations[sk0], sk0, activeCls, comp, items);
			}
		}

		// Аргументы-вызовы — в локальные: Enforce затирает возвращённую строку
		// вложенными вызовами при вычислении следующих аргументов.
		string popTitle = DRL_UIText.Tr("#STR_DRL_Variation_Ingridient_Titel");
		string popSub = DRL_UIText.DisplayName(activeCls);
		int popHue = INT_UIF.Color(INT_UIF_ThemeKeys.AMBER);
		m_Popover.Open("var", popTitle, popSub, "swap", popHue, rows);
		m_Grid.HideModels(true);
	}

	// = DRL_CraftVariationsController.CreateVariationTile
	protected void AddVarRow(array<ref DRL_UIRowData> rows, DRL_CraftMergedComponent mc, int targetKey, string varCls, int varIdx, string activeCls, DRL_Craft_Component compRef, array<EntityAI> cachedItems)
	{
		if (!compRef)
			return;
		DRL_VariationClickInfo info = new DRL_VariationClickInfo(mc, targetKey, varCls, varIdx);
		m_VarInfos.Insert(info);

		int have = ComputeHaveForClass(varCls, compRef.MinHealthFrac, cachedItems, compRef.QuantityFrac);
		int craftCount = 1;
		if (m_SelectedRecipe)
			craftCount = m_SelectedRecipe.CurrentCraftCount;
		int need = 0;
		if (targetKey >= 0)
		{
			need = NeedForClassCount(varCls, compRef, craftCount);
		}
		else
		{
			int totalNeed = 0;
			if (mc.ComponentKeys)
			{
				for (int ck = 0; ck < mc.ComponentKeys.Count(); ck++)
				{
					DRL_Craft_Component cK = GetComponentByKey(mc.ComponentKeys[ck]);
					if (!cK)
						continue;
					array<string> cKVars = DRL_CraftMergedHelper.GetVariationsForComponent(cK);
					if ((cKVars && cKVars.Find(varCls) != -1) || varCls == cK.Classname)
						totalNeed = totalNeed + NeedForClassCount(varCls, cK, craftCount);
				}
			}
			if (totalNeed <= 0)
				totalNeed = NeedForClassCount(varCls, compRef, craftCount);
			need = totalNeed;
		}

		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int col = tm.Color(INT_UIF_ThemeKeys.RED);
		if (have >= need)
			col = tm.Color(INT_UIF_ThemeKeys.GREEN);
		else if (have > 0)
			col = tm.Color(INT_UIF_ThemeKeys.AMBER);
		DRL_UIRowData d = new DRL_UIRowData();
		d.m_Title = DRL_UIText.DisplayName(varCls);
		d.m_Right = DRL_CraftInventoryHelper.FormatIngredientAmount(varCls, compRef, have, need);
		d.m_RightColor = col;
		d.m_Accent = col;
		d.m_Selected = varCls == activeCls;
		if (d.m_Selected)
			d.m_Accent = tm.Color(INT_UIF_ThemeKeys.TEAL);
		d.m_PreviewClass = varCls;
		d.m_Tag = m_VarInfos.Count() - 1;
		rows.Insert(d);
	}

	// = HandleVariationIngrClick (выбор плитки вариации)
	protected void ApplyVariation(int tag)
	{
		if (tag < 0 || tag >= m_VarInfos.Count())
			return;
		DRL_VariationClickInfo info = m_VarInfos[tag];
		if (!info || !info.MergedComp)
			return;
		string chosenVariant = info.VariantClass;
		int targetKey = info.TargetKey;
		if (targetKey >= 0)
		{
			m_SelectedComponentVariations.Set(targetKey, chosenVariant);
		}
		else if (info.MergedComp.ComponentKeys)
		{
			for (int vk = 0; vk < info.MergedComp.ComponentKeys.Count(); vk++)
			{
				int compKey = info.MergedComp.ComponentKeys[vk];
				DRL_Craft_Component compRef = GetComponentByKey(compKey);
				if (compRef)
				{
					array<string> compVars = DRL_CraftMergedHelper.GetVariationsForComponent(compRef);
					if (compVars && compVars.Find(chosenVariant) != -1)
						m_SelectedComponentVariations.Set(compKey, chosenVariant);
					else if (compVars && compVars.Count() > 0)
						m_SelectedComponentVariations.Set(compKey, compVars[0]);
					else
						m_SelectedComponentVariations.Set(compKey, compRef.Classname);
				}
				else
				{
					m_SelectedComponentVariations.Set(compKey, chosenVariant);
				}
			}
		}
		HideVariationList();
		FillDetails();
	}

	protected void HideVariationList()
	{
		if (m_Popover && m_Popover.GetKind() == "var")
			ClosePopover();
		m_ActiveVariationMerged = null;
	}

	protected void ClosePopover()
	{
		if (m_Popover)
			m_Popover.Close();
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		if (m_Grid)
			m_Grid.HideModels(false);
		HideHoverCard();
	}

	// Закрыта крестиком.
	protected void OnPopoverClosed()
	{
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		m_Grid.HideModels(false);
		HideHoverCard();
	}

	protected void OnPopAction(int index, string action, int arg, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d || (action != "row" && action != "info"))
			return;
		string kind = m_Popover.GetKind();
		if (kind == "var")
		{
			ApplyVariation(d.m_Tag);
			return;
		}
		if (kind == "req" && d.m_Tag >= 0 && d.m_Tag < m_ReqTargets.Count())
		{
			DRL_Craft_Recipe target = m_ReqTargets[d.m_Tag];
			if (target)
				SelectRecipeDirectly(target);
		}
	}

	protected void OnPopHover(int index, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d || m_Popover.GetKind() != "var" || d.m_Tag < 0 || d.m_Tag >= m_VarInfos.Count())
		{
			HideHoverCard();
			return;
		}
		DRL_VariationClickInfo info = m_VarInfos[d.m_Tag];
		if (!info || !info.MergedComp)
		{
			HideHoverCard();
			return;
		}
		ShowHoverCard(info.VariantClass, info.MergedComp.ToCraftComponent());
	}

	// --- требования (= DRL_CraftRequirementsController) ---------------------------------
	protected void OnReqClick(INT_UIF_Button b)
	{
		if (m_Popover.GetKind() == "req")
		{
			ClosePopover();
			return;
		}
		OpenRequirements();
	}

	protected void OpenRequirements()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		int hue = INT_UIF.Color(INT_UIF_ThemeKeys.RED);
		if (AreAllRecipeRequirementsMet(r))
			hue = INT_UIF.Color(INT_UIF_ThemeKeys.GREEN);
		array<ref DRL_UIRowData> rows = BuildRequirementRows(r);
		string popTitle = DRL_UIText.Tr("#STR_DRL_UI_REQS");
		m_Popover.Open("req", popTitle, r.RecipeName, "shield", hue, rows);
		m_Grid.HideModels(true);
	}

	bool AreAllRecipeRequirementsMet(DRL_Craft_Recipe r)
	{
		if (!r)
			return true;
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (lcfg && lcfg.RecipeRequiresLearning(r.RecipeId))
		{
			if (!DRL_LearnState.HasLearned(r.RecipeId))
				return false;
		}
		if (lcfg)
		{
			DRL_Learning_Recipe lr = lcfg.FindByPrimaryId(r.RecipeId);
			if (!lr)
				lr = lcfg.FindByAnyRecipeId(r.RecipeId);
			if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
			{
				for (int i = 0; i < lr.RequiredRecipeIds.Count(); i++)
				{
					if (!DRL_LearnState.HasLearned(lr.RequiredRecipeIds[i]))
						return false;
				}
			}
		}
		if (!HasRequiredAttachments(r))
			return false;
		if (!CanCraftRecipe(r))
			return false;
		return true;
	}

	protected void AddReqRow(array<ref DRL_UIRowData> rows, string title, string subtitle, string previewClass, bool isMet, string statusText, DRL_Craft_Recipe target)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (statusText == "")
		{
			if (isMet)
				statusText = "#STR_DRL_ATTACHMENT_INSTALLED";
			else
				statusText = "#STR_DRL_ATTACHMENT_MISSING";
		}
		int col = tm.Color(INT_UIF_ThemeKeys.RED);
		if (isMet)
			col = tm.Color(INT_UIF_ThemeKeys.GREEN);
		DRL_UIRowData d = new DRL_UIRowData();
		d.m_Title = DRL_UIText.Tr(title);
		d.m_Sub = DRL_UIText.Tr(subtitle);
		d.m_Right = DRL_UIText.Tr(statusText);
		d.m_RightColor = col;
		d.m_Accent = col;
		d.m_PreviewClass = previewClass;
		d.m_Glyph = "info";
		d.m_Clickable = target != null;
		d.m_Info = target != null;
		d.m_Tag = m_ReqTargets.Count();
		m_ReqTargets.Insert(target);
		rows.Insert(d);
	}

	// = DRL_CraftRequirementsController.BuildUI
	protected array<ref DRL_UIRowData> BuildRequirementRows(DRL_Craft_Recipe r)
	{
		array<ref DRL_UIRowData> rows = new array<ref DRL_UIRowData>();
		m_ReqTargets.Clear();
		if (!r)
			return rows;

		int totalReqCount = 0;
		DRL_Learning_Config lcfg = GetLearning_Config();
		bool recipeNeedsLearning = false;
		if (lcfg)
			recipeNeedsLearning = lcfg.RecipeRequiresLearning(r.RecipeId);
		DRL_Learning_Recipe lr = null;
		if (lcfg)
		{
			lr = lcfg.FindByPrimaryId(r.RecipeId);
			if (!lr)
				lr = lcfg.FindByAnyRecipeId(r.RecipeId);
		}
		bool hasPrereqs = lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0;

		if (recipeNeedsLearning || hasPrereqs)
		{
			rows.Insert(DRL_UIRowData.Header("#STR_Studying_Recipes"));
			if (recipeNeedsLearning)
			{
				bool isLearned = DRL_LearnState.HasLearned(r.RecipeId);
				string studyPrevCls = r.PreviewClassname;
				if (studyPrevCls == "")
					studyPrevCls = r.Result;
				string studyStatusStr = "#STR_LearningLockTitel";
				if (isLearned)
					studyStatusStr = "#STR_DRL_LEARNED";
				AddReqRow(rows, r.RecipeName, "#STR_DRL_RECIPE_STUDY", studyPrevCls, isLearned, studyStatusStr, null);
				totalReqCount++;
			}
			if (hasPrereqs)
			{
				DRL_Craft_Config ccfg = Cfg();
				for (int pi = 0; pi < lr.RequiredRecipeIds.Count(); pi++)
				{
					int prereqId = lr.RequiredRecipeIds[pi];
					DRL_Craft_Recipe prereqR = null;
					if (ccfg)
						prereqR = ccfg.FindById(prereqId);
					string pTitle = DRL_UIText.Tr("#STR_Recipe_name") + " #" + prereqId.ToString();
					string pPrev = "";
					if (prereqR)
					{
						pTitle = prereqR.RecipeName;
						pPrev = prereqR.PreviewClassname;
						if (pPrev == "")
							pPrev = prereqR.Result;
					}
					bool pLearned = DRL_LearnState.HasLearned(prereqId);
					string pStatusStr = "#STR_RequiredToStudy";
					if (pLearned)
						pStatusStr = "#STR_DRL_LEARNED";
					AddReqRow(rows, pTitle, "#STR_DRL_PREREQUISITE_RECIPE", pPrev, pLearned, pStatusStr, prereqR);
					totalReqCount++;
				}
			}
		}

		if (r.CraftComponents && r.CraftComponents.Count() > 0)
		{
			bool addedAttachHeader = false;
			string activeReqRes = m_SelectedResultClass;
			PlayerBase pbReq = PlayerBase.Cast(GetGame().GetPlayer());
			for (int ci = 0; ci < r.CraftComponents.Count(); ci++)
			{
				DRL_Craft_Component comp = r.CraftComponents[ci];
				if (!comp || comp.AttachmentsEx <= 0)
					continue;
				if (!comp.IsApplicableForResult(activeReqRes))
					continue;
				if (!addedAttachHeader)
				{
					rows.Insert(DRL_UIRowData.Header("#STR_WB_TOOLS"));
					addedAttachHeader = true;
				}
				string token = comp.Classname;
				if (token == "" && comp.Classnames && comp.Classnames.Count() > 0)
					token = comp.Classnames[0];
				bool okSlot = DRL_CraftComponentResolver.HasComponentRequirement(comp, pbReq, m_ContextWorkbench, activeReqRes, 1);
				string attStatus = "#STR_DRL_ATTACHMENT_MISSING";
				if (okSlot)
					attStatus = "#STR_DRL_ATTACHMENT_INSTALLED";
				AddReqRow(rows, ToolName(token), "#STR_DRL_WB_EQUIPMENT", token, okSlot, attStatus, null);
				totalReqCount++;
			}
		}

		if (totalReqCount == 0)
		{
			if (!CanCraftRecipe(r))
			{
				rows.Insert(DRL_UIRowData.Header("#STR_DRL_Craft_Ingridient_Titel"));
				array<EntityAI> cachedItems = new array<EntityAI>();
				GetInventorySnapshot(cachedItems);
				int craftCount = Math.Max(1, r.CurrentCraftCount);
				bool addedAnyMissing = false;
				string required = DRL_UIText.Tr("#STR_DRL_REQUIRED");
				required = required.Trim();
				if (r == m_SelectedRecipe)
				{
					array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(r);
					for (int m = 0; m < mergedList.Count(); m++)
					{
						DRL_CraftMergedComponent mc = mergedList[m];
						if (!mc)
							continue;
						DRL_Craft_Component cMerged = mc.ToCraftComponent();
						if (!cMerged || cMerged.AttachmentsEx > 0)
							continue;
						string activeMergedCls = mc.Classname;
						if (activeMergedCls == "")
							activeMergedCls = cMerged.Classname;
						int haveMerged = ComputeHaveForClass(activeMergedCls, cMerged.MinHealthFrac, cachedItems, cMerged.QuantityFrac, cMerged.UseInheritance == 1);
						int needMerged = NeedForClassCount(activeMergedCls, cMerged, craftCount);
						if (haveMerged < needMerged)
						{
							string mStatus = DRL_CraftInventoryHelper.FormatIngredientAmount(activeMergedCls, cMerged, haveMerged, needMerged, true);
							string mSub = required + " " + DRL_CraftInventoryHelper.FormatIngredientNeed(activeMergedCls, cMerged, needMerged);
							AddReqRow(rows, DRL_UIText.DisplayName(activeMergedCls), mSub, activeMergedCls, false, mStatus, null);
							addedAnyMissing = true;
						}
					}
				}
				else if (r.CraftComponents)
				{
					for (int mi = 0; mi < r.CraftComponents.Count(); mi++)
					{
						DRL_Craft_Component cIngr = r.CraftComponents[mi];
						if (!cIngr || cIngr.AttachmentsEx > 0)
							continue;
						int haveQty = ComputeHaveForComponent(cIngr, mi, cachedItems);
						int needQty = NeedForCount(cIngr, craftCount, mi);
						if (haveQty < needQty)
						{
							string ingrStatus = DRL_CraftInventoryHelper.FormatIngredientAmount(cIngr.Classname, cIngr, haveQty, needQty, true);
							string needSub = required + " " + DRL_CraftInventoryHelper.FormatIngredientNeed(cIngr.Classname, cIngr, needQty);
							AddReqRow(rows, DRL_UIText.DisplayName(cIngr.Classname), needSub, cIngr.Classname, false, ingrStatus, null);
							addedAnyMissing = true;
						}
					}
				}
				if (!addedAnyMissing)
					AddReqRow(rows, "#STR_DRL_NOT_ENOUGH_INGREDIENTS", "#STR_DRL_CHECK_RECIPE_INGREDIENTS", "", false, "#STR_DRL_ATTACHMENT_MISSING", null);
			}
			else
			{
				rows.Insert(DRL_UIRowData.Header("#STR_DRL_AdditionalTasks_Titel"));
				AddReqRow(rows, "#STR_DRL_NO_ADDITIONAL_REQUIREMENTS", "#STR_DRL_READY_TO_CRAFT", "", true, "#STR_DRL_ATTACHMENT_INSTALLED", null);
			}
		}
		return rows;
	}

	// = UpdateAdditionalTasksButtonState
	protected void UpdateReqButton()
	{
		if (!m_ReqBtn || !m_SelectedRecipe)
			return;
		int variant = INT_UIF_Variant.DANGER;
		if (AreAllRecipeRequirementsMet(m_SelectedRecipe))
			variant = INT_UIF_Variant.SUCCESS;
		if (m_ReqBtn.GetVariant() != variant)
			m_ReqBtn.SetVariant(variant);
		if (m_Popover.GetKind() == "req")
		{
			array<ref DRL_UIRowData> rows = BuildRequirementRows(m_SelectedRecipe);
			m_Popover.SetRows(rows);
		}
	}

	// --- «Описание» -------------------------------------------------------------------
	protected void FillInfoPage(DRL_Craft_Recipe r)
	{
		string chosen = ChosenResult();
		m_InfoName.SetText(DRL_UIText.Label("#STR_DRL_NAME", DRL_UIText.DisplayName(chosen)));
		m_InfoState.SetText(DRL_UIText.Label("#STR_DRL_STATE", DRL_UIText.StateFromDamage(r.ResultDamageFrac)));
		string qs = "—";
		if (r.ResultQuantityFrac > 0.0)
			qs = DRL_UIText.Pct(r.ResultQuantityFrac);
		m_InfoFill.SetText(DRL_UIText.Label("#STR_DRL_FILL", qs));

		string body = "";
		if (r.Description != "")
		{
			body = r.Description;
		}
		else if (r.Result != "")
		{
			string cfgDesc = "";
			if (DRL_CraftUIHelper.GetDescriptionShortForClass(r.Result, cfgDesc))
				body = cfgDesc;
		}
		if (body == "")
			body = "#STR_DRL_Craft_Recipe_Detail";
		m_RecipeDesc.SetText(body);

		FillStats(chosen);
		string attachClass = chosen;
		if (attachClass == "")
		{
			attachClass = r.PreviewClassname;
			if (attachClass == "")
				attachClass = r.Result;
		}
		FillSlotIcons(attachClass);
	}

	// = DRL_CraftStatsHelper.UpdateResultStats, значения по плиткам.
	protected void FillStats(string cls)
	{
		string basePath = "";
		if (cls != "")
		{
			if (GetGame().ConfigIsExisting("CfgVehicles " + cls))
				basePath = "CfgVehicles " + cls;
			else if (GetGame().ConfigIsExisting("CfgWeapons " + cls))
				basePath = "CfgWeapons " + cls;
			else if (GetGame().ConfigIsExisting("CfgMagazines " + cls))
				basePath = "CfgMagazines " + cls;
		}
		m_StatsBox.Show(basePath != "");
		if (basePath == "")
			return;
		int hp = -1;
		int tmpInt = 0;
		if (DRL_CraftStatsHelper.ReadInt(basePath + " DamageSystem GlobalHealth Health hitpoints", tmpInt))
			hp = tmpInt;
		string hpText = "—";
		if (hp >= 0)
			hpText = hp.ToString() + " hp";
		m_StatValues[0].SetText(hpText);
		m_StatValues[1].SetText(ProtText(ReadArmor(basePath, "Projectile", "", "")));
		m_StatValues[2].SetText(ProtText(ReadArmor(basePath, "Melee", "", "")));
		m_StatValues[3].SetText(ProtText(ReadArmor(basePath, "Infected", "Zombie", "MeleeZombie")));
		m_StatValues[4].SetText(ProtText(ReadArmor(basePath, "FragGrenade", "Explosion", "")));
	}

	protected float ReadArmor(string basePath, string a, string b, string c)
	{
		float v;
		if (DRL_CraftStatsHelper.ReadFloat(basePath + " DamageSystem GlobalArmor " + a + " Health damage", v))
			return v;
		if (b != "" && DRL_CraftStatsHelper.ReadFloat(basePath + " DamageSystem GlobalArmor " + b + " Health damage", v))
			return v;
		if (c != "" && DRL_CraftStatsHelper.ReadFloat(basePath + " DamageSystem GlobalArmor " + c + " Health damage", v))
			return v;
		return -1.0;
	}

	protected string ProtText(float coef)
	{
		if (coef < 0.0)
			return "—";
		float p = Math.Clamp((1.0 - coef) * 100.0, 0, 100);
		int pi = Math.Round(p);
		return pi.ToString() + "%";
	}

	// = RenderAttachmentIconsForClass
	protected void FillSlotIcons(string cls)
	{
		INT_UIF_Widgets.ClearChildren(m_SlotIcons);
		array<string> slots;
		DRL_CraftStatsHelper.GetAttachmentSlotsForClass(cls, slots);
		int n = 0;
		if (slots)
			n = slots.Count();
		m_SlotsNone.Show(n == 0);
		float maxW = DETAILS_W - 160;
		float x = 0;
		for (int i = 0; i < n; i++)
		{
			if (x + 24 > maxW)
				break;
			string path = "set:dayz_inventory image:missing";
			string iconPath;
			if (DRL_CraftStatsHelper.GetSlotIconPath(slots[i], iconPath))
				path = DRL_CraftUIHelper.ResolveImagePath(iconPath);
			INT_UIF_Draw.Texture(m_SlotIcons, x, 0, 24, 24, path, ARGB(210, 255, 255, 255));
			x = x + 28;
		}
	}

	// --- «Разбор» (= BuildReCraftList / UpdateReCraftButtonState) ----------------------
	protected void BuildRecraftRows(DRL_Craft_Recipe r, bool keepScroll)
	{
		array<ref DRL_UIRowData> rows = new array<ref DRL_UIRowData>();
		string targetRes = ChosenResult();
		if (r && r.CraftComponents)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c)
					continue;
				if (!c.IsApplicableForResult(targetRes))
					continue;
				if (c.RecraftDropChance <= 0.0)
					continue;
				if (rows.Count() == 0)
					rows.Insert(DRL_UIRowData.Header("#STR_DRL_UI_RECRAFT_GET"));
				DRL_UIRowData d = new DRL_UIRowData();
				d.m_Title = DRL_UIText.DisplayName(c.Classname);
				if (c.RecraftDropChance < 1.0)
				{
					string chance = DRL_UIText.Pct(c.RecraftDropChance);
					d.m_Sub = DRL_UIText.Fmt("#STR_DRL_UI_RECRAFT_CHANCE", chance);
				}
				string t = "";
				if (DRL_CraftInventoryHelper.IsFillingByClass(c.Classname) && c.RecraftQuantityFrac > 0.0)
				{
					t = DRL_UIText.Pct(c.RecraftQuantityFrac);
				}
				else
				{
					int cnt = c.RecraftAmount;
					if (cnt <= 0)
						cnt = 1;
					t = "x" + cnt.ToString();
				}
				d.m_Right = t;
				d.m_RightColor = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
				d.m_PreviewClass = c.Classname;
				d.m_Clickable = false;
				d.m_Tag = i;
				rows.Insert(d);
			}
		}
		m_RecraftList.SetItems(rows, keepScroll);
		bool hasDrops = rows.Count() > 0;
		m_RecraftFoot.Show(hasDrops);
		if (r)
		{
			string recraftTime = DRL_UIText.Time(r.GetRecraftTimeSec());
			m_RecraftTime.SetText(DRL_UIText.Fmt("#STR_DRL_UI_RECRAFT_TIME", recraftTime));
		}
	}

	protected void UpdateRecraftButton()
	{
		if (!m_RecraftBtn)
			return;
		if (m_ReCraftActive)
		{
			SetRecraftButton(false, "#STR_DRL_IN_PROGRESS");
			return;
		}
		if (!m_SelectedRecipe)
		{
			SetRecraftButton(false, "#STR_DRL_NO_INGREDIENTS");
			return;
		}
		if (HasResultItemForRecraft(m_SelectedRecipe))
			SetRecraftButton(true, "#STR_DRL_RECRAFT");
		else
			SetRecraftButton(false, "#STR_DRL_NO_INGREDIENTS");
	}

	protected void SetRecraftButton(bool enabled, string text)
	{
		m_RecraftText = DRL_UIText.Tr(text);
		m_RecraftBtn.SetEnabled(enabled);
		m_RecraftBtn.SetText(text);
		int variant = INT_UIF_Variant.SECONDARY;
		if (enabled)
			variant = INT_UIF_Variant.WARNING;
		if (m_RecraftBtn.GetVariant() != variant)
			m_RecraftBtn.SetVariant(variant);
	}

	protected bool HasResultItemForRecraft(DRL_Craft_Recipe r)
	{
		if (!r)
			return false;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return false;
		string target = r.Result;
		if (m_SelectedResultClass != "" && r.ResultClasses && r.ResultClasses.Find(m_SelectedResultClass) > -1)
			target = m_SelectedResultClass;
		array<ItemBase> pool;
		p.DRL_CollectItemsByClass(target, 3, pool);
		if (!pool || pool.Count() == 0)
			return false;
		return HasRequiredRecraftAttachments(r);
	}

	protected bool HasRequiredRecraftAttachments(DRL_Craft_Recipe r)
	{
		if (!r || !r.CraftComponents)
			return true;
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0 || c.RecraftNeedAttachments != 1)
				continue;
			if (!FindAttachment(c))
				return false;
		}
		return true;
	}

	protected void OnRecraftClick(INT_UIF_Button b)
	{
		if (m_ReCraftActive)
			return;
		TryStartReCraft();
	}

	protected void TryStartReCraft()
	{
		if (!m_SelectedRecipe || m_ReCraftActive)
			return;
		if (!HasResultItemForRecraft(m_SelectedRecipe))
		{
			m_RecraftBtn.SetEnabled(false);
			return;
		}
		if (!m_ContextWorkbench)
			PlayCraftSound(m_SelectedRecipe);
		StartReCraftVisuals(m_SelectedRecipe.GetRecraftTimeSec());

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		string chosen = ChosenResult();
		int netLow = 0;
		int netHigh = 0;
		if (m_ContextWorkbench)
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		Param4<int, string, int, int> pdata4 = new Param4<int, string, int, int>(m_SelectedRecipe.RecipeId, chosen, netLow, netHigh);
		GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_RECRAFT, pdata4, true, player.GetIdentity());
	}

	protected void StartReCraftVisuals(float durationSec)
	{
		if (durationSec <= 0.0)
			durationSec = 0.01;
		m_ReCraftActive = true;
		m_ReCraftElapsed = 0.0;
		m_ReCraftDuration = durationSec;
		m_RecraftBar.Show(true);
		m_RecraftBar.SetValue(0, false);
		UpdateRecraftButton();
		UpdateStageTint();
	}

	protected void TickReCraft(float dt)
	{
		if (!m_ReCraftActive)
			return;
		m_ReCraftElapsed = m_ReCraftElapsed + dt;
		m_RecraftBar.SetValue(m_ReCraftElapsed / m_ReCraftDuration, false);
		if (m_ReCraftElapsed < m_ReCraftDuration)
			return;
		m_ReCraftActive = false;
		m_RecraftBar.SetValue(1, false);
		m_RecraftBar.Show(false);
		StopCraftSound();
		UpdateRecraftButton();
		if (m_SelectedRecipe)
			FillDetails();
		else
			RefreshStatuses();
		UpdateCraftButtonState();
	}

	// --- количество и время -----------------------------------------------------------
	protected void OnCountPlus(INT_UIF_IconButton b)
	{
		if (!m_SelectedRecipe)
			return;
		int maxN = GetMaxCraftable(m_SelectedRecipe);
		int newN = Math.Clamp(m_SelectedRecipe.CurrentCraftCount + 1, 1, Math.Max(1, maxN));
		m_SelectedRecipe.CurrentCraftCount = newN;
		FillDetails();
	}

	protected void OnCountMinus(INT_UIF_IconButton b)
	{
		if (!m_SelectedRecipe)
			return;
		m_SelectedRecipe.CurrentCraftCount = Math.Max(1, m_SelectedRecipe.CurrentCraftCount - 1);
		FillDetails();
	}

	// = RefreshCraftCountText / RefreshTimeTextForCount
	protected void RefreshCountUI()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		int n = Math.Max(1, r.CurrentCraftCount);
		m_CountText.SetText(n.ToString());
		int maxN = GetMaxCraftable(r);
		m_CountMinus.SetEnabled(n > 1);
		m_CountPlus.SetEnabled(n < Math.Max(1, maxN));
		int total = r.ResultCount * n;
		m_TotalText.SetText(DRL_UIText.Fmt("#STR_DRL_UI_TOTAL", total.ToString()));
		int t = Math.Max(0, GetTotalCraftTimeSec(r) * n);
		m_FootTime.SetText(DRL_UIText.Time(t));
	}

	// =================================================================================
	// Кнопка крафта (= UpdateCraftButtonState / TryStartCraft)
	// =================================================================================
	void UpdateCraftButtonState()
	{
		UpdateReqButton();
		UpdateStageTint();
		if (!m_CraftBtn)
			return;

		DRL_UIProc proc = GetProcForCurrentRecipe();
		if (proc)
		{
			m_CraftBtn.SetEnabled(true);
			if (proc.IsQueued())
			{
				SetCraftButton("#STR_DRL_IN_QUEUE_CANCEL", INT_UIF_Variant.WARNING, "clock");
				m_CraftBar.Show(false);
			}
			else
			{
				SetCraftButton("#STR_CANCEL", INT_UIF_Variant.DANGER, "close");
				m_CraftBar.Show(true);
				m_CraftBar.SetValue(proc.Percent() / 100.0, false);
			}
			return;
		}
		m_CraftBar.Show(false);

		if (!m_SelectedRecipe)
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_No_Recipe", INT_UIF_Variant.SECONDARY, "error");
			return;
		}
		if (!DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId))
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_LearningLockTitel", INT_UIF_Variant.DANGER, "lock");
			return;
		}
		if (!HasRequiredAttachments(m_SelectedRecipe))
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_DRL_NEED_ATTACHMENT", INT_UIF_Variant.DANGER, "warning");
			return;
		}
		if (!CanCraftRecipe(m_SelectedRecipe))
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_DRL_NO_INGREDIENTS", INT_UIF_Variant.DANGER, "close");
			return;
		}
		int maxSlots = MaxQueueSlots();
		int cur = m_Procs.Count();
		if (cur >= maxSlots)
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton(DRL_UIText.Tr("#STR_DRL_QUEUE_FULL") + " (" + cur.ToString() + "/" + maxSlots.ToString() + ")", INT_UIF_Variant.DANGER, "error");
			return;
		}
		m_CraftBtn.SetEnabled(true);
		if (cur > 0)
		{
			int next = cur + 1;
			SetCraftButton(DRL_UIText.Tr("#STR_DRL_TO_QUEUE") + " (" + next.ToString() + "/" + maxSlots.ToString() + ")", INT_UIF_Variant.PRIMARY, "plus");
		}
		else
		{
			SetCraftButton("#STR_DRL_START", INT_UIF_Variant.PRIMARY, "gear");
		}
	}

	protected void SetCraftButton(string text, int variant, string icon)
	{
		m_CraftText = DRL_UIText.Tr(text);
		m_CraftBtn.SetText(text);
		m_CraftBtn.SetIcon(icon);
		if (m_CraftBtn.GetVariant() != variant)
			m_CraftBtn.SetVariant(variant);
	}

	// = UpdateCraftFonColor: идёт крафт или разбор — подсветка сцены краснеет.
	protected void UpdateStageTint()
	{
		if (!m_Stage)
			return;
		bool isRunning = m_ReCraftActive;
		DRL_UIProc proc = GetProcForCurrentRecipe();
		if (proc && !proc.IsQueued())
			isRunning = true;
		if (isRunning)
			m_Stage.SetAccent(INT_UIF.Color(INT_UIF_ThemeKeys.RED));
		else
			m_Stage.SetAccent(INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));
	}

	protected void OnCraftClick(INT_UIF_Button b)
	{
		DRL_UIProc proc = GetProcForCurrentRecipe();
		if (proc)
		{
			CancelProcess(proc.GetProcessId());
			return;
		}
		if (!CanCraftSelected())
		{
			m_CraftBtn.SetEnabled(false);
			return;
		}
		TryStartCraft();
	}

	protected void TryStartCraft()
	{
		if (!m_SelectedRecipe)
			return;
		if (GetProcForCurrentRecipe() != null)
			return;
		if (m_Procs.Count() >= MaxQueueSlots())
			return;
		if (!DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId))
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_LearningLockTitel", INT_UIF_Variant.DANGER, "lock");
			return;
		}
		if (!HasRequiredAttachments(m_SelectedRecipe))
		{
			m_CraftBtn.SetEnabled(false);
			SetCraftButton("#STR_DRL_NEED_ATTACHMENT", INT_UIF_Variant.DANGER, "warning");
			return;
		}

		array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(m_SelectedRecipe);
		for (int i = 0; i < mergedList.Count(); i++)
		{
			DRL_CraftMergedComponent mc = mergedList[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			string activeCls = mc.Classname;
			if (activeCls == "")
				activeCls = c.Classname;
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, null, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need)
				return;
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		string chosenCls = ChosenResult();
		int craftCount = Math.Max(1, m_SelectedRecipe.CurrentCraftCount);
		int netLow = 0;
		int netHigh = 0;
		if (m_ContextWorkbench)
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		vector aimPos = player.GetPosition();

		// Выбранные вариации компонентов: ключ RecipeId * 100 + индекс компонента.
		array<int> compVarKeys = new array<int>();
		array<string> compVarClasses = new array<string>();
		if (m_SelectedRecipe.CraftComponents)
		{
			for (int ci = 0; ci < m_SelectedRecipe.CraftComponents.Count(); ci++)
			{
				DRL_Craft_Component cComp = m_SelectedRecipe.CraftComponents[ci];
				if (cComp && cComp.Classnames && cComp.Classnames.Count() > 1)
				{
					int cKey = m_SelectedRecipe.RecipeId * 100 + ci;
					string activeVarCls = GetActiveComponentClass(cComp, cKey);
					if (activeVarCls != "")
					{
						compVarKeys.Insert(cKey);
						compVarClasses.Insert(activeVarCls);
					}
				}
			}
		}

		array<int> selectedAttached = GetSelectedAttachedRecipeIdsFlattened();
		DRL_Craft_Config cfg = Cfg();
		if (selectedAttached.Count() > 0 && cfg)
		{
			for (int saIdx = 0; saIdx < selectedAttached.Count(); saIdx++)
			{
				DRL_Craft_Recipe saR = cfg.FindById(selectedAttached[saIdx]);
				if (!saR || !saR.CraftComponents)
					continue;
				for (int sai = 0; sai < saR.CraftComponents.Count(); sai++)
				{
					DRL_Craft_Component saComp = saR.CraftComponents[sai];
					if (saComp && saComp.Classnames && saComp.Classnames.Count() > 1)
					{
						int saKey = saR.RecipeId * 100 + sai;
						string saActiveCls = GetActiveComponentClass(saComp, saKey);
						if (saActiveCls != "")
						{
							compVarKeys.Insert(saKey);
							compVarClasses.Insert(saActiveCls);
						}
					}
				}
			}

			vector netWb = Vector(netLow, netHigh, 0);
			array<string> attVariantClasses = new array<string>();
			for (int a = 0; a < selectedAttached.Count(); a++)
			{
				int dopId = selectedAttached[a];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				attVariantClasses.Insert(GetSelectedAttachedClass(dopId, dopR));
			}
			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CRAFT_WITH_ATTACHMENTS, new Param9<int, string, int, vector, array<int>, vector, array<string>, array<int>, array<string>>(m_SelectedRecipe.RecipeId, chosenCls, craftCount, netWb, selectedAttached, aimPos, attVariantClasses, compVarKeys, compVarClasses), true, null);
		}
		else
		{
			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CRAFT, new Param8<int, string, int, int, int, vector, array<int>, array<string>>(m_SelectedRecipe.RecipeId, chosenCls, craftCount, netLow, netHigh, aimPos, compVarKeys, compVarClasses), true, null);
		}
	}

	// =================================================================================
	// Процессы крафта (= OnProcessSync/Started/Finished + DRL_CraftProcessEntryWidget)
	// =================================================================================
	protected void HookProcesses()
	{
		UnhookProcesses();
		DRL_CraftProcessBridge.s_OnProcessesSync.Insert(OnProcessSyncReceived);
		DRL_CraftProcessBridge.s_OnProcessStarted.Insert(OnProcessStartedReceived);
		DRL_CraftProcessBridge.s_OnProcessFinished.Insert(OnProcessFinishedReceived);
	}

	protected void UnhookProcesses()
	{
		if (DRL_CraftProcessBridge.s_OnProcessesSync)
			DRL_CraftProcessBridge.s_OnProcessesSync.Remove(OnProcessSyncReceived);
		if (DRL_CraftProcessBridge.s_OnProcessStarted)
			DRL_CraftProcessBridge.s_OnProcessStarted.Remove(OnProcessStartedReceived);
		if (DRL_CraftProcessBridge.s_OnProcessFinished)
			DRL_CraftProcessBridge.s_OnProcessFinished.Remove(OnProcessFinishedReceived);
	}

	void RequestActiveCraftProcesses()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		int netLow = 0;
		int netHigh = 0;
		if (m_ContextWorkbench)
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		p.RPCSingleParam(DRL_CraftRPC.CTS_REQUEST_ACTIVE_PROCESSES, new Param2<int, int>(netLow, netHigh), true, null);
	}

	void OnProcessSyncReceived()
	{
		m_Procs.Clear();
		array<ref DRL_CraftProcessSyncInfo> activeList = DRL_CraftProcessBridge.GetActiveProcesses();
		if (activeList)
		{
			foreach (DRL_CraftProcessSyncInfo info : activeList)
			{
				if (info)
					m_Procs.Insert(new DRL_UIProc(info));
			}
			if (!m_ContextWorkbench)
			{
				foreach (DRL_CraftProcessSyncInfo runInfo : activeList)
				{
					if (!runInfo || runInfo.m_ProcessStatus != DRL_CraftProcessStatus.RUNNING)
						continue;
					if (!m_CraftSound || !m_CraftSound.IsSoundPlaying())
					{
						DRL_Craft_Config cfg = Cfg();
						if (cfg)
						{
							DRL_Craft_Recipe runRecipe = cfg.FindById(runInfo.m_RecipeId);
							if (runRecipe)
								PlayCraftSound(runRecipe);
						}
					}
					break;
				}
			}
		}
		RefreshQueue();
		UpdateCraftButtonState();
	}

	void OnProcessStartedReceived(DRL_CraftProcessSyncInfo syncInfo)
	{
		if (!syncInfo)
			return;
		for (int i = m_Procs.Count() - 1; i >= 0; i--)
		{
			if (m_Procs[i] && m_Procs[i].GetProcessId() == syncInfo.m_ProcessId)
				m_Procs.Remove(i);
		}
		m_Procs.Insert(new DRL_UIProc(syncInfo));
		if (!m_ContextWorkbench && syncInfo.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
		{
			DRL_Craft_Config cfg = Cfg();
			if (cfg)
			{
				DRL_Craft_Recipe rStart = cfg.FindById(syncInfo.m_RecipeId);
				if (rStart)
					PlayCraftSound(rStart);
			}
		}
		RefreshQueue();
		UpdateCraftButtonState();
	}

	void OnProcessFinishedReceived(string processId, bool cancelled)
	{
		for (int i = m_Procs.Count() - 1; i >= 0; i--)
		{
			if (m_Procs[i] && m_Procs[i].GetProcessId() == processId)
				m_Procs.Remove(i);
		}
		StopCraftSound();
		RefreshQueue();
		if (m_SelectedRecipe)
			FillDetails();
		else
			RefreshStatuses();
		UpdateCraftButtonState();
	}

	protected DRL_UIProc GetProcForCurrentRecipe()
	{
		if (!m_SelectedRecipe)
			return null;
		foreach (DRL_UIProc p : m_Procs)
		{
			if (p && p.GetRecipeId() == m_SelectedRecipe.RecipeId)
				return p;
		}
		return null;
	}

	void CancelProcess(string processId)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || processId == "")
			return;
		GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CANCEL_CRAFT, new Param1<string>(processId), true, null);
		StopCraftSound();
	}

	protected void RefreshQueue()
	{
		if (m_Queue)
			m_Queue.SetProcs(m_Procs, MaxQueueSlots());
	}

	protected void TickProcesses(float dt)
	{
		if (m_Procs.Count() == 0)
			return;
		foreach (DRL_UIProc p : m_Procs)
		{
			if (p)
				p.Tick(dt);
		}
		m_Queue.Tick();
		DRL_UIProc cur = GetProcForCurrentRecipe();
		if (cur && !cur.IsQueued() && m_CraftBar)
			m_CraftBar.SetValue(cur.Percent() / 100.0, false);
	}

	// Плитка очереди: выбрать рецепт процесса или отменить его.
	protected void OnQueueAction(int index, string action, DRL_UIQueueBar bar)
	{
		if (index < 0 || index >= m_Procs.Count())
			return;
		DRL_UIProc p = m_Procs[index];
		if (!p)
			return;
		if (action == "cancel")
		{
			CancelProcess(p.GetProcessId());
			return;
		}
		DRL_Craft_Config cfg = Cfg();
		if (!cfg)
			return;
		DRL_Craft_Recipe target = cfg.FindById(p.GetRecipeId());
		if (target)
			SelectRecipeDirectly(target);
	}

	// --- звук крафта (= DRL_GetCraftSoundSetForRecipe / DRL_PlayCraftSoundForRecipe) ---
	protected string GetCraftSoundSetForRecipe(DRL_Craft_Recipe r)
	{
		DRL_Craft_Config cfg = Cfg();
		if (!cfg || !cfg.Settings)
			return "";
		string snd = cfg.Settings.CraftSoundSet;
		DRL_Craft_Category cat = null;
		if (cfg.Categories && m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg.Categories.Count())
		{
			DRL_Craft_Category selectedCat = cfg.Categories[m_CurrentCategoryIdx];
			if (selectedCat && selectedCat.ShowAll != 1)
				cat = selectedCat;
			else
				cat = FindCategoryForRecipe(r);
		}
		if (cat && cat.CategoryCraftSoundSet != "")
			snd = cat.CategoryCraftSoundSet;
		return snd;
	}

	protected void PlayCraftSound(DRL_Craft_Recipe r)
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
			return;
		string snd = GetCraftSoundSetForRecipe(r);
		if (snd == "")
			return;
		if (m_CraftSound && m_CraftSound.IsSoundPlaying())
			m_CraftSound.SoundStop();
		pb.PlaySoundSetLoop(m_CraftSound, snd, 0, 0);
	}

	protected void StopCraftSound()
	{
		if (m_CraftSound)
			m_CraftSound.SoundStop();
	}

	// =================================================================================
	// Тестовый хук (DRL_UITest)
	// =================================================================================
	int TestCategoryCount()
	{
		return m_Sidebar.Count();
	}

	void TestSelectCategory(int entryIndex)
	{
		m_Sidebar.Select(entryIndex, true);
	}

	int TestRecipeCount()
	{
		return m_Grid.Count();
	}

	void TestSelectRecipe(int index)
	{
		m_Grid.Select(index, true);
	}

	void TestSearch(string text)
	{
		if (m_Search)
			m_Search.SetText(text);
		OnSearchChanged(text, m_Search);
	}

	void TestHover(int slot)
	{
		m_Grid.PinHover(slot);
	}

	// Команды деталей; "" — команда не наша.
	string TestCommand(string cmd, string arg)
	{
		int n = arg.ToInt();
		if (cmd == "cat")
		{
			TestSelectCategory(n);
			return "categories=" + TestCategoryCount().ToString() + " recipes=" + TestRecipeCount().ToString();
		}
		if (cmd == "recipe")
		{
			TestSelectRecipe(n);
			return "of " + TestRecipeCount().ToString() + " " + TestState();
		}
		if (cmd == "search")
		{
			TestSearch(arg);
			return "recipes=" + TestRecipeCount().ToString();
		}
		if (cmd == "hover")
		{
			TestHover(n);
			return "hover";
		}
		if (cmd == "tab")
		{
			m_Tabs.Select(n, true);
			return "tab=" + m_Tabs.GetActive().ToString();
		}
		if (cmd == "var")
		{
			m_Variants.TestSelect(n);
			return "result=" + m_SelectedResultClass + " " + TestState();
		}
		if (cmd == "row" || cmd == "chk" || cmd == "qplus" || cmd == "qminus" || cmd == "info")
		{
			string act = "row";
			if (cmd == "chk")
				act = "check";
			else if (cmd == "qplus")
				act = "plus";
			else if (cmd == "qminus")
				act = "minus";
			else if (cmd == "info")
				act = "info";
			m_CompList.TestAction(n, act, 0);
			return TestState();
		}
		if (cmd == "dv")
		{
			DRL_UIRowData d = m_CompList.GetItem(n);
			int nextVar = 0;
			if (d && d.m_VarCount > 1)
				nextVar = (d.m_VarSelected + 1) % d.m_VarCount;
			m_CompList.TestAction(n, "var", nextVar);
			return "var=" + nextVar.ToString() + " " + TestState();
		}
		if (cmd == "prow")
		{
			m_Popover.GetList().TestAction(n, "row", 0);
			return TestState();
		}
		if (cmd == "pclose")
		{
			ClosePopover();
			return TestState();
		}
		if (cmd == "reqs")
		{
			OnReqClick(m_ReqBtn);
			return TestState() + " poprows=" + m_Popover.GetList().Count().ToString();
		}
		if (cmd == "plus")
		{
			OnCountPlus(null);
			return TestState();
		}
		if (cmd == "minus")
		{
			OnCountMinus(null);
			return TestState();
		}
		if (cmd == "craft")
		{
			if (m_CraftBtn.IsEnabled())
				OnCraftClick(m_CraftBtn);
			return TestState();
		}
		if (cmd == "recraft")
		{
			if (m_RecraftBtn.IsEnabled())
				OnRecraftClick(m_RecraftBtn);
			return "recraft='" + m_RecraftText + "' active=" + m_ReCraftActive.ToString();
		}
		if (cmd == "hrow")
		{
			OnCompHover(n, m_CompList);
			return "card=" + m_HoverCard.IsShown().ToString();
		}
		if (cmd == "hoff")
		{
			HideHoverCard();
			return "card=off";
		}
		if (cmd == "qsel")
		{
			m_Queue.TestAction(n, "select");
			return TestState();
		}
		if (cmd == "qcancel")
		{
			m_Queue.TestAction(n, "cancel");
			return TestState();
		}
		if (cmd == "state")
			return TestState();
		return "";
	}

	string TestState()
	{
		string name = "-";
		int count = 0;
		if (m_SelectedRecipe)
		{
			name = m_SelectedRecipe.RecipeName;
			count = m_SelectedRecipe.CurrentCraftCount;
		}
		string s = "recipe='" + name + "' count=" + count.ToString();
		s = s + " button='" + m_CraftText + "' rows=" + m_CompList.Count().ToString();
		s = s + " attached=" + m_SelectedAttachedRecipeIds.Count().ToString();
		s = s + " procs=" + m_Procs.Count().ToString() + " pop=" + m_Popover.GetKind();
		return s;
	}

	override void OnTestCommand(string command)
	{
		DRL_UITest.Apply(this, command);
	}

	// Мышь попадает в хит-зону, у которой свой обработчик (сетка, сайдбар).
	// dayz-mcp ui_click вызывает обработчик меню — пробрасываем клик дальше.
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w)
		{
			string n = w.GetName();
			if (n.IndexOf("DRL_Card_") == 0 && m_Grid && m_Grid.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Cat_") == 0 && m_Sidebar && m_Sidebar.OnClick(w, x, y, button))
				return true;
		}
		return super.OnClick(w, x, y, button);
	}
}
