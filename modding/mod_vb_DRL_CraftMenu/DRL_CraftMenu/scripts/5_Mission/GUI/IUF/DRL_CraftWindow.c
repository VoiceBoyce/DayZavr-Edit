// ============================================================================
// DRL_CraftMenu - окно крафта на INT_UIFramework (замена DRL_CraftMenuUI).
//
// Раскладка v2 — четыре колонки, как в старом меню (окно 1680x920):
//   шапка      — название, верстак/свободный крафт, очередь процессов, слоты, «Изучение»;
//   рецепты    — поиск, полоса иконок категорий, список рецептов, «только доступные»;
//   ингредиенты — плитки ингредиентов и доп. предметов (инструменты, AttachmentsEx),
//                карточка наведённого ингредиента;
//   предмет    — сцена с 3D-превью и колонкой вариантов, слоты навесного,
//                описание, плитки доп. навесного (поверх колонки выезжает
//                панель вариаций ингредиента / требований);
//   результат  — что получится, характеристики, количество, разбор,
//                время и кнопка крафта с прогрессом.
// Логика выбора, фильтров, проверок, состояний кнопки и RPC повторяет старое
// меню 1:1 и идёт через те же хелперы (DRL_CraftInventoryHelper,
// DRL_CraftMergedHelper, DRL_CraftComponentResolver и т.д.).
// Сервер (обфусцированный DRL_CraftMenu_Server.pbo) не меняется.
// ============================================================================

class DRL_CraftWindow extends INT_UIF_Menu
{
	// Колонки тела (1648 x 814 при окне 1680x920).
	static const float COL1_W = 336;
	static const float COL2_W = 352;
	static const float COL3_W = 600;
	static const float COL4_W = 324;
	static const float COL_GAP = 12;
	static const float CPAD = 14;
	static const float STAGE_H = 268;
	static const float VARIANT_W = 84;
	static const float VARIANT_TILE_H = 56;
	static const float RECIPE_ROW_H = 60;
	static const float RECIPE_ROW_GAP = 6;
	static const float INGR_TILE_H = 88;
	static const float DOP_TILE_H = 118;
	static const float RET_TILE_H = 52;
	static const float CARD_H = 206;

	// Теги плиток ингредиентов: ингредиент (индекс в m_Merged) или инструмент.
	static const int TAG_TOOL = 1000;

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
	protected ref DRL_UICategoryRail m_Rail;
	protected ref DRL_UIRecipeGrid m_Grid;
	protected ref DRL_UIStage m_Stage;
	protected ref DRL_UIVariantStrip m_Variants;
	protected ref DRL_UITileList m_IngrList;
	protected ref DRL_UITileList m_DopList;
	protected ref DRL_UITileList m_RetList;
	protected ref DRL_UIIngrCard m_IngrCard;
	protected ref DRL_UISlotStrip m_SlotStrip;
	protected ref DRL_UIItemView m_ResView;
	protected ref DRL_UIPopover m_Popover;
	protected ref DRL_UIQueueBar m_Queue;
	protected INT_UIF_TextInput m_Search;
	protected INT_UIF_Toggle m_OnlyAvail;
	protected INT_UIF_Button m_LearningBtn;
	protected INT_UIF_Badge m_SlotsBadge;
	protected TextWidget m_ListTitle;
	protected string m_CatTitle;
	protected bool m_OnlyAvailable;

	// Колонки: тело показывается, когда выбран рецепт.
	protected Widget m_IngrBody;
	protected Widget m_ItemBody;
	protected Widget m_ItemEmpty;
	protected Widget m_ResBody;

	// Ингредиенты.
	protected TextWidget m_IngrSummary;
	protected int m_CardTag = -1;

	// Предмет.
	protected TextWidget m_RecipeTitle;
	protected INT_UIF_Button m_ReqBtn;
	protected TextWidget m_SlotsFill;
	protected MultilineTextWidget m_RecipeDesc;
	protected TextWidget m_DopSummary;

	// Результат.
	protected Widget m_ResThumbGlyph;
	protected TextWidget m_ResName;
	protected TextWidget m_ResSub;
	protected ref array<TextWidget> m_ResValues;
	protected Widget m_StatsBox;
	protected ref array<TextWidget> m_StatValues;

	// Разбор.
	protected TextWidget m_RecraftHead;
	protected INT_UIF_Button m_RecraftBtn;
	protected INT_UIF_ProgressBar m_RecraftBar;
	protected string m_RecraftText;

	// Количество и кнопка крафта.
	protected INT_UIF_IconButton m_CountMinus;
	protected INT_UIF_IconButton m_CountPlus;
	protected INT_UIF_Slider m_CountSlider;
	protected TextWidget m_CountText;
	protected TextWidget m_CountMax;
	protected TextWidget m_TotalText;
	protected TextWidget m_FootTime;
	protected INT_UIF_Button m_CraftBtn;
	protected INT_UIF_ProgressBar m_CraftBar;
	protected string m_CraftText;

	protected float m_SlotTimer;

	void DRL_CraftWindow()
	{
		m_Rail = new DRL_UICategoryRail();
		m_Grid = new DRL_UIRecipeGrid();
		m_Stage = new DRL_UIStage();
		m_Variants = new DRL_UIVariantStrip();
		m_IngrList = new DRL_UITileList();
		m_DopList = new DRL_UITileList();
		m_RetList = new DRL_UITileList();
		m_IngrCard = new DRL_UIIngrCard();
		m_SlotStrip = new DRL_UISlotStrip();
		m_ResView = new DRL_UIItemView();
		m_Popover = new DRL_UIPopover();
		m_Queue = new DRL_UIQueueBar();
		m_ResValues = new array<TextWidget>();

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
		w = 1680;
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
		float bh = GetBodyH();
		float x2 = COL1_W + COL_GAP;
		float x3 = x2 + COL2_W + COL_GAP;
		float x4 = x3 + COL3_W + COL_GAP;

		BuildRecipesCol(Column(body, 0, COL1_W, bh, "DRL_ColRecipes"), COL1_W, bh);
		BuildIngrCol(Column(body, x2, COL2_W, bh, "DRL_ColIngr"), COL2_W, bh);
		BuildItemCol(Column(body, x3, COL3_W, bh, "DRL_ColItem"), COL3_W, bh);
		BuildResultCol(Column(body, x4, COL4_W, bh, "DRL_ColResult"), COL4_W, bh);

		// Панель вариаций ингредиента / требований выезжает поверх колонки предмета.
		m_Popover.Build(m_Bag, body, x3, 0, COL3_W, bh);
		m_Popover.OnClose().Insert(OnPopoverClosed);
		m_Popover.GetList().OnAction().Insert(OnPopAction);
		m_Popover.GetList().OnHover().Insert(OnPopHover);

		ShowDetails(false, false);

		// Подсказки клавиш под окном не нужны (решение 2026-10-27). «Esc» фреймворк
		// добавляет сам; сам контейнер оставляем — его анимирует открытие/закрытие.
		if (m_Hints)
			INT_UIF_Widgets.ClearChildren(m_Hints);
	}

	// Колонка — панель на всю высоту тела.
	protected Widget Column(Widget body, float x, float w, float h, string name)
	{
		Widget col = INT_UIF_Draw.Frame(body, x, 0, w, h, name);
		int panelFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		int panelBorder = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL_BORDER);
		INT_UIF_Surface.Box(col, 0, 0, w, h, 12, panelFill, panelBorder);
		RevealOnOpen(col);
		return col;
	}

	protected TextWidget ColTitle(Widget col, string text)
	{
		TextWidget t = INT_UIF_Draw.Text(col, CPAD, CPAD - 2, 220, 24, text, 16, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		t.SetBold(true);
		return t;
	}

	// --- 1. рецепты: поиск, полоса категорий, список, «только доступные» -------------
	protected void BuildRecipesCol(Widget col, float w, float h)
	{
		float cw = w - 2 * CPAD;
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		ColTitle(col, "#STR_DRL_UI_COL_RECIPES");
		m_ListTitle = INT_UIF_Draw.Text(col, CPAD + 110, CPAD, cw - 110, 20, "", 12, muted, INT_UIF_Align.RIGHT);
		m_Search = INT_UIF_TextInput.Create(m_Bag, col, CPAD, 46, cw, 40, "#STR_SEARCH_RECIPE", 40, "search", "");
		m_Search.OnChanged().Insert(OnSearchChanged);
		m_Search.GetRoot().SetName("DRL_Search");

		float ly = 98;
		float toggleH = 32;
		float lh = h - ly - CPAD - toggleH - 12;
		m_Rail.Build(col, CPAD, ly, 40, lh);
		m_Rail.OnSelected().Insert(OnCategorySelected);

		float gx = CPAD + 40 + 12;
		float gw = w - CPAD - gx + 6;
		int rows = (int)((lh + RECIPE_ROW_GAP) / (RECIPE_ROW_H + RECIPE_ROW_GAP));
		if (rows < 1)
			rows = 1;
		float gh = rows * RECIPE_ROW_H + (rows - 1) * RECIPE_ROW_GAP;
		m_Grid.SetRowMode(true);
		m_Grid.Build(col, gx, ly, gw, gh, 1, rows, RECIPE_ROW_GAP);
		m_Grid.OnSelected().Insert(OnGridSelected);

		float ty = h - CPAD - toggleH;
		INT_UIF_Draw.Line(col, CPAD, ty - 7, cw, 1, INT_UIF.Color(INT_UIF_ThemeKeys.DIVIDER));
		m_OnlyAvail = INT_UIF_Toggle.Create(m_Bag, col, CPAD, ty, cw, "#STR_DRL_UI_ONLY_AVAILABLE", false);
		m_OnlyAvail.OnChanged().Insert(OnOnlyAvailChanged);
		DRL_UIKitUtil.Expose(m_OnlyAvail, "DRL_OnlyAvail");
	}

	// --- 2. ингредиенты и доп. предметы, карточка ингредиента -----------------------
	protected void BuildIngrCol(Widget col, float w, float h)
	{
		float cw = w - 2 * CPAD;
		ColTitle(col, "#STR_DRL_Craft_Ingridient_Titel");
		m_IngrBody = INT_UIF_Draw.Frame(col, 0, 0, w, h, "DRL_IngrBody");
		m_IngrSummary = INT_UIF_Draw.Text(m_IngrBody, CPAD + 120, CPAD, cw - 120, 20, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.RIGHT);
		float ly = 48;
		float lh = h - ly - CPAD - CARD_H - 12;
		m_IngrList.Build(m_IngrBody, CPAD, ly, cw + 8, lh, 4, INGR_TILE_H, 8, false, "DRL_Ingr");
		m_IngrList.SetTooltips(true);
		m_IngrList.SetEmptyText("#STR_DRL_UI_NO_INGR");
		m_IngrList.OnAction().Insert(OnIngrAction);
		m_IngrList.OnHover().Insert(OnIngrHover);
		m_IngrCard.Build(m_IngrBody, CPAD, h - CPAD - CARD_H, cw, CARD_H);
	}

	// --- 3. предмет: сцена и варианты, слоты, описание, доп. навесное ---------------
	protected void BuildItemCol(Widget col, float w, float h)
	{
		float cw = w - 2 * CPAD;
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);

		// Пусто: ничего не выбрано.
		m_ItemEmpty = INT_UIF_Draw.Frame(col, 0, 0, w, h, "DRL_ItemEmpty");
		INT_UIF_Draw.Image(m_ItemEmpty, w * 0.5 - 120, h * 0.5 - 150, 240, 240, "iuf_radial", ARGB(16, 255, 255, 255));
		INT_UIF_Draw.Icon(m_ItemEmpty, w * 0.5 - 22, h * 0.5 - 56, 44, "gear", faint);
		INT_UIF_Draw.Text(m_ItemEmpty, 0, h * 0.5 + 4, w, 24, "#STR_DRL_UI_SELECT_HINT", 16, muted, INT_UIF_Align.CENTER);

		m_ItemBody = INT_UIF_Draw.Frame(col, 0, 0, w, h, "DRL_ItemBody");
		m_RecipeTitle = INT_UIF_Draw.Text(m_ItemBody, CPAD, CPAD - 3, cw - 166, 26, "", 18, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_RecipeTitle.SetBold(true);
		m_ReqBtn = INT_UIF_Button.Create(m_Bag, m_ItemBody, w - CPAD - 150, CPAD - 6, 150, 34, "#STR_DRL_UI_REQS", INT_UIF_Variant.SUCCESS, "shield");
		m_ReqBtn.OnClick().Insert(OnReqClick);
		DRL_UIKitUtil.Expose(m_ReqBtn, "DRL_ReqBtn");

		float y = 50;
		m_Stage.Build(m_ItemBody, CPAD, y, cw, STAGE_H);
		m_Stage.SetAccent(INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));
		m_Variants.Build(m_Stage.GetRoot(), cw - VARIANT_W - 10, 10, VARIANT_W, STAGE_H - 20, VARIANT_TILE_H);
		m_Variants.OnSelected().Insert(OnVariantSelected);
		m_Variants.Show(false);

		y = y + STAGE_H + 14;
		INT_UIF_Draw.Caption(m_ItemBody, CPAD + 2, y, cw - 200, "#STR_DRL_UI_SLOTS_TITLE", "list");
		m_SlotsFill = INT_UIF_Draw.Text(m_ItemBody, CPAD + cw - 240, y - 2, 240, 18, "", 12, muted, INT_UIF_Align.RIGHT);
		m_SlotStrip.Build(m_ItemBody, CPAD, y + 22, cw, 38);

		y = y + 22 + 38 + 14;
		INT_UIF_Draw.Caption(m_ItemBody, CPAD + 2, y, cw, "#STR_DRL_UI_RECIPE_DETAILS", "info");
		m_RecipeDesc = INT_UIF_Draw.MText(m_ItemBody, CPAD + 2, y + 22, cw - 4, 60, "", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));

		y = y + 22 + 60 + 12;
		INT_UIF_Draw.Caption(m_ItemBody, CPAD + 2, y, cw - 220, "#STR_DRL_UI_SEC_DOP", "plus");
		m_DopSummary = INT_UIF_Draw.Text(m_ItemBody, CPAD + cw - 260, y - 2, 260, 18, "", 12, muted, INT_UIF_Align.RIGHT);
		float dy = y + 24;
		m_DopList.Build(m_ItemBody, CPAD, dy, cw + 8, h - CPAD - dy, 4, DOP_TILE_H, 8, true, "DRL_Dop");
		m_DopList.SetTooltips(true);
		m_DopList.SetEmptyText("#STR_DRL_UI_NO_DOP");
		m_DopList.OnAction().Insert(OnDopAction);
	}

	// --- 4. результат: предмет, характеристики, количество, разбор, кнопка ----------
	protected void BuildResultCol(Widget col, float w, float h)
	{
		float cw = w - 2 * CPAD;
		int text = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int tileFill = INT_UIF.Color(INT_UIF_ThemeKeys.TILE);
		int tileBorder = INT_UIF.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		ColTitle(col, "#STR_DRL_UI_COL_RESULT");
		m_ResBody = INT_UIF_Draw.Frame(col, 0, 0, w, h, "DRL_ResBody");

		// Что получится.
		float y = 48;
		INT_UIF_Surface.Box(m_ResBody, CPAD, y, 92, 64, 9, tileFill, tileBorder);
		Widget clip = INT_UIF_Draw.Clip(m_ResBody, CPAD + 3, y + 3, 86, 58, "DRL_ResClip");
		m_ResView.Create(clip, true);
		m_ResView.SetOrientation(Vector(-8, 30, 0));
		m_ResThumbGlyph = INT_UIF_Draw.Icon(m_ResBody, CPAD + 36, y + 22, 20, "info", faint);
		m_ResThumbGlyph.Show(false);
		m_ResName = INT_UIF_Draw.Text(m_ResBody, CPAD + 104, y + 8, cw - 104, 24, "", 16, text);
		m_ResName.SetBold(true);
		m_ResSub = INT_UIF_Draw.Text(m_ResBody, CPAD + 104, y + 34, cw - 104, 18, "", 12, muted);

		y = y + 64 + 14;
		TStringArray resLabels = {"#STR_DRL_UI_RES_TIME", "#STR_DRL_UI_RES_COUNT", "#STR_DRL_UI_ING_FILL", "#STR_DRL_UI_RES_STATE"};
		for (int i = 0; i < resLabels.Count(); i++)
		{
			INT_UIF_Draw.Text(m_ResBody, CPAD, y, cw * 0.5, 20, resLabels.Get(i), 13, muted);
			TextWidget rv = INT_UIF_Draw.Text(m_ResBody, CPAD + cw * 0.4, y, cw * 0.6, 20, "", 13, text, INT_UIF_Align.RIGHT);
			rv.SetBold(true);
			m_ResValues.Insert(rv);
			y = y + 22;
		}

		// Характеристики (= DRL_CraftStatsHelper.UpdateResultStats).
		y = y + 10;
		m_StatsBox = INT_UIF_Draw.Frame(m_ResBody, CPAD, y, cw, 138, "DRL_Stats");
		INT_UIF_Surface.Box(m_StatsBox, 0, 0, cw, 138, 10, tileFill, tileBorder);
		INT_UIF_Draw.Caption(m_StatsBox, 12, 10, cw - 24, "#STR_DRL_UI_STATS", "shield");
		TStringArray labels = {"#STR_DRL_UI_STAT_HP", "#STR_DRL_UI_STAT_BULLET", "#STR_DRL_UI_STAT_MELEE", "#STR_DRL_UI_STAT_INFECTED", "#STR_DRL_UI_STAT_GRENADE"};
		for (int s = 0; s < labels.Count(); s++)
		{
			float sy = 32 + s * 20;
			INT_UIF_Draw.Text(m_StatsBox, 12, sy, cw * 0.6, 19, labels.Get(s), 13, muted);
			TextWidget val = INT_UIF_Draw.Text(m_StatsBox, cw * 0.4, sy, cw * 0.6 - 12, 19, "—", 13, text, INT_UIF_Align.RIGHT);
			val.SetBold(true);
			m_StatValues.Insert(val);
		}

		// Количество к созданию.
		y = y + 138 + 14;
		INT_UIF_Draw.Text(m_ResBody, CPAD, y, cw * 0.7, 18, "#STR_DRL_UI_COUNT_TO_MAKE", 13, muted);
		m_CountMax = INT_UIF_Draw.Text(m_ResBody, CPAD + cw * 0.5, y, cw * 0.5, 18, "", 12, faint, INT_UIF_Align.RIGHT);
		float qy = y + 24;
		m_CountMinus = INT_UIF_IconButton.Create(m_Bag, m_ResBody, CPAD, qy, 44, "minus", INT_UIF_Variant.SECONDARY);
		m_CountMinus.OnClick().Insert(OnCountMinus);
		DRL_UIKitUtil.Expose(m_CountMinus, "DRL_CountMinus");
		INT_UIF_Surface.Box(m_ResBody, CPAD + 52, qy, cw - 104, 44, 9, tileFill, ARGB(24, 255, 255, 255));
		m_CountText = INT_UIF_Draw.Text(m_ResBody, CPAD + 52, qy, cw - 104, 44, "1", 22, text, INT_UIF_Align.CENTER, true);
		m_CountText.SetBold(true);
		m_CountPlus = INT_UIF_IconButton.Create(m_Bag, m_ResBody, CPAD + cw - 44, qy, 44, "plus", INT_UIF_Variant.SECONDARY);
		m_CountPlus.OnClick().Insert(OnCountPlus);
		DRL_UIKitUtil.Expose(m_CountPlus, "DRL_CountPlus");
		m_CountSlider = INT_UIF_Slider.Create(m_Bag, m_ResBody, CPAD, qy + 50, cw, 1, 2, 1, 1, "", 24);
		m_CountSlider.OnChanged().Insert(OnCountSlider);
		m_TotalText = INT_UIF_Draw.Text(m_ResBody, CPAD, qy + 76, cw, 18, "", 12, muted);

		// Разбор (= BuildReCraftList / UpdateReCraftButtonState).
		y = qy + 104;
		INT_UIF_Draw.Line(m_ResBody, CPAD, y, cw, 1, INT_UIF.Color(INT_UIF_ThemeKeys.DIVIDER));
		y = y + 10;
		INT_UIF_Draw.Icon(m_ResBody, CPAD, y + 8, 14, "refresh", muted);
		m_RecraftHead = INT_UIF_Draw.Text(m_ResBody, CPAD + 20, y, cw - 160, 30, "", 13, text);
		m_RecraftHead.SetBold(true);
		m_RecraftBtn = INT_UIF_Button.Create(m_Bag, m_ResBody, CPAD + cw - 136, y, 136, 30, "#STR_DRL_RECRAFT", INT_UIF_Variant.WARNING, "refresh");
		m_RecraftBtn.OnClick().Insert(OnRecraftClick);
		DRL_UIKitUtil.Expose(m_RecraftBtn, "DRL_RecraftBtn");
		m_RecraftBar = INT_UIF_ProgressBar.Create(m_Bag, m_ResBody, CPAD, y + 34, cw, 3, 0, "amber");
		m_RecraftBar.Show(false);
		m_RetList.Build(m_ResBody, CPAD, y + 40, cw + 8, RET_TILE_H, 5, RET_TILE_H, 6, false, "DRL_Ret");
		m_RetList.SetTooltips(true);
		m_RetList.SetEmptyText("#STR_DRL_UI_RECRAFT_EMPTY");

		// Низ: общее время, прогресс и кнопка крафта.
		float by = h - CPAD - 54;
		m_CraftBtn = INT_UIF_Button.Create(m_Bag, m_ResBody, CPAD, by, cw, 54, "#STR_DRL_START", INT_UIF_Variant.PRIMARY, "gear");
		m_CraftBtn.OnClick().Insert(OnCraftClick);
		DRL_UIKitUtil.Expose(m_CraftBtn, "DRL_CraftBtn");
		m_CraftBar = INT_UIF_ProgressBar.Create(m_Bag, m_ResBody, CPAD, by - 10, cw, 4, 0, "teal");
		m_CraftBar.Show(false);
		INT_UIF_Draw.Icon(m_ResBody, CPAD, by - 36, 14, "clock", muted);
		INT_UIF_Draw.Text(m_ResBody, CPAD + 20, by - 40, cw * 0.6, 22, "#STR_DRL_UI_CRAFT_TIME", 13, muted);
		m_FootTime = INT_UIF_Draw.Text(m_ResBody, CPAD + cw * 0.4, by - 41, cw * 0.6, 24, "", 16, text, INT_UIF_Align.RIGHT);
		m_FootTime.SetBold(true);
	}

	// Тело колонок: пусто или выбран рецепт.
	protected void ShowDetails(bool show, bool animate)
	{
		m_ItemEmpty.Show(!show);
		m_IngrBody.Show(show);
		m_ItemBody.Show(show);
		m_ResBody.Show(show);
		if (!show || !animate)
			return;
		DRL_UIAnim.Reveal(m_IngrBody, 0, 0, 0, 8, 0.24);
		DRL_UIAnim.Reveal(m_ItemBody, 0, 0, 0.04, 8, 0.24);
		DRL_UIAnim.Reveal(m_ResBody, 0, 0, 0.08, 8, 0.24);
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
		INT_UIF_Tooltip.HideTip();
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
		if (m_IngrList)
			m_IngrList.ClearModels();
		if (m_DopList)
			m_DopList.ClearModels();
		if (m_RetList)
			m_RetList.ClearModels();
		if (m_IngrCard)
			m_IngrCard.Clear();
		if (m_ResView)
			m_ResView.Clear();
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
			m_Rail.SetEntries(entries, -1);
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
		m_Rail.SetEntries(entries, defaultEntry);
		int key = -1;
		DRL_UICategoryEntry def = m_Rail.GetEntry(defaultEntry);
		if (def)
			key = def.m_Key;
		SelectCategory(key);
	}

	protected void OnCategorySelected(int entryIndex, DRL_UICategoryRail rail)
	{
		DRL_UICategoryEntry e = rail.GetEntry(entryIndex);
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
		m_CatTitle = DRL_UIText.Tr(title);
		RebuildRecipes();
	}

	// «Только доступные к крафту»: в списке остаются рецепты со статусом «можно».
	protected void OnOnlyAvailChanged(bool on, INT_UIF_Toggle t)
	{
		if (m_OnlyAvailable == on)
			return;
		m_OnlyAvailable = on;
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
					DRL_UIRecipeEntry entry = MakeEntry(r, cachedItems);
					if (m_OnlyAvailable && entry.m_Status != DRL_UI_STATUS_CAN)
						continue;
					entries.Insert(entry);
				}
			}
		}
		m_Grid.SetItems(entries, m_SelectedRecipe);
		m_ListTitle.SetText(m_CatTitle + " · " + entries.Count().ToString());
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
			{
				RebuildMainPreview();
				FillSlots(m_SelectedRecipe);
			}
			BuildDopTiles(true);
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
		m_CardTag = -1;
		if (m_Stage)
			m_Stage.Clear();
		if (m_Variants)
			m_Variants.Clear();
		if (m_IngrList)
			m_IngrList.ClearModels();
		if (m_DopList)
			m_DopList.ClearModels();
		if (m_RetList)
			m_RetList.ClearModels();
		if (m_IngrCard)
			m_IngrCard.ShowEmpty();
		if (m_ResView)
			m_ResView.Clear();
		if (m_CraftBar)
			m_CraftBar.Show(false);
		if (m_ItemBody)
			ShowDetails(false, false);
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
		if (!sameRecipe)
			m_CardTag = -1;
		if (!sameRecipe || !m_ItemBody.IsVisible())
			ShowDetails(true, true);
		HideVariationList();

		m_RecipeTitle.SetText(r.RecipeName);
		RebuildMainPreview();
		ApplyBackdrop(r);
		FillVariants(r);
		BuildIngrTiles(sameRecipe);
		BuildDopTiles(sameRecipe);
		FillItemInfo(r);
		FillResult(r);
		BuildRecraftTiles(r, sameRecipe);
		UpdateRecraftButton();
		RefreshCountUI();
		UpdateCraftButtonState();
		RefreshStatuses();
	}

	// Класс главного превью: PreviewClassname / Result или выбранный вариант.
	protected string MainPreviewClass()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return "";
		string mainCls = r.PreviewClassname;
		if (mainCls == "")
			mainCls = r.Result;
		string selRes = m_SelectedResultClass;
		if (selRes != "" && ((r.ResultClasses && r.ResultClasses.Find(selRes) > -1) || selRes == r.Result))
			mainCls = selRes;
		return mainCls;
	}

	// Главное превью с выбранным доп. навесным (= RebuildMainPreviewWithAttachments).
	protected void RebuildMainPreview()
	{
		if (!m_SelectedRecipe)
			return;
		string mainCls = MainPreviewClass();
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

	// Варианты результата (= BuildSelectResultList / UpdateSelectCraftHighlight):
	// колонка справа на сцене, модель сдвигается левее.
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
			m_Stage.SetModelInset(VARIANT_W + 10);
		}
		else
		{
			m_Variants.Clear();
			m_Stage.SetModelInset(0);
		}
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

	// Выезжающая панель лежит поверх колонки предмета: 3D-модели под ней прячутся
	// (рисуются поверх 2D).
	protected void SetCovered(bool covered)
	{
		if (m_Stage)
			m_Stage.SetHidden(DRL_UIStage.HIDE_POPOVER, covered);
		if (m_Variants)
			m_Variants.HideModels(covered);
		if (m_DopList)
			m_DopList.HideModels(covered);
	}

	// --- ингредиенты и доп. предметы (= BuildIngredientsListUI + BuildAtachIngridientListUI)
	protected void BuildIngrTiles(bool keepScroll)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		array<ref DRL_UITileData> items = new array<ref DRL_UITileData>();
		m_ToolComps.Clear();
		if (!r)
		{
			m_Merged.Clear();
			m_IngrList.SetItems(items);
			return;
		}
		m_Merged = BuildMergedComponents(r);
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int green = tm.Color(INT_UIF_ThemeKeys.GREEN);
		int amber = tm.Color(INT_UIF_ThemeKeys.AMBER);
		int red = tm.Color(INT_UIF_ThemeKeys.RED);

		int total = 0;
		int ready = 0;
		for (int i = 0; i < m_Merged.Count(); i++)
		{
			DRL_CraftMergedComponent mc = m_Merged[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			string activeCls = mc.Classname;
			if (activeCls == "")
				activeCls = c.Classname;
			array<string> variations = DRL_CraftMergedHelper.GetVariationsForComponent(c);
			bool hasVariations = variations && variations.Count() > 1 && c.EnableVariationSelection == 1;
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, cachedItems, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, r.CurrentCraftCount);
			int col = red;
			if (have >= need)
				col = green;
			else if (have > 0)
				col = amber;

			DRL_UITileData d = new DRL_UITileData();
			d.m_Title = DRL_UIText.DisplayName(activeCls);
			d.m_Count = DRL_CraftInventoryHelper.FormatIngredientAmount(activeCls, c, have, need);
			d.m_CountColor = col;
			d.m_State = col;
			d.m_PreviewClass = activeCls;
			if (hasVariations)
				d.m_Badge = "swap";
			d.m_Tag = i;
			items.Insert(d);
			total++;
			if (have >= need)
				ready++;
		}
		if (total == 0)
			items.Insert(DRL_UITileData.Note("#STR_DRL_UI_NO_INGR"));
		if (total > 0)
		{
			m_IngrSummary.SetText(DRL_UIText.Fmt("#STR_DRL_UI_IN_STOCK", ready.ToString(), total.ToString()));
			if (ready == total)
				m_IngrSummary.SetColor(green);
			else
				m_IngrSummary.SetColor(red);
		}
		else
		{
			m_IngrSummary.SetText("");
		}

		// Инструменты и оснащение (AttachmentsEx) — «Дополнительные предметы».
		string activeResult = m_SelectedResultClass;
		array<ref DRL_UITileData> tools = new array<ref DRL_UITileData>();
		int toolsOk = 0;
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
				DRL_UITileData td = new DRL_UITileData();
				td.m_Title = ToolName(token);
				int tcol = red;
				if (present)
				{
					td.m_Count = "1/1";
					tcol = green;
					toolsOk++;
				}
				else
				{
					td.m_Count = "0/1";
				}
				td.m_CountColor = tcol;
				td.m_State = tcol;
				td.m_PreviewClass = prevCls;
				td.m_Glyph = "key";
				td.m_Tag = TAG_TOOL + m_ToolComps.Count();
				m_ToolComps.Insert(tc);
				tools.Insert(td);
			}
		}
		string toolsRight = "";
		int toolsColor = 0;
		if (tools.Count() > 0)
		{
			toolsRight = DRL_UIText.Fmt("#STR_DRL_UI_OF", toolsOk.ToString(), tools.Count().ToString());
			toolsColor = red;
			if (toolsOk == tools.Count())
				toolsColor = green;
		}
		items.Insert(DRL_UITileData.Header("#STR_DRL_UI_COL_TOOLS", toolsRight, toolsColor));
		if (tools.Count() == 0)
			items.Insert(DRL_UITileData.Note("#STR_DRL_UI_NO_TOOLS"));
		foreach (DRL_UITileData tool : tools)
		{
			items.Insert(tool);
		}
		m_IngrList.SetItems(items, keepScroll);
		RefreshIngrCard();
	}

	// Выделить ингредиент, чьи вариации открыты в выезжающей панели.
	protected void RefreshIngrSelection()
	{
		for (int i = 0; i < m_IngrList.Count(); i++)
		{
			DRL_UITileData d = m_IngrList.GetItem(i);
			if (!d || d.m_Kind != DRL_UI_TILE_ITEM)
				continue;
			bool sel = false;
			if (m_ActiveVariationMerged && d.m_Tag >= 0 && d.m_Tag < TAG_TOOL && d.m_Tag < m_Merged.Count())
				sel = m_Merged[d.m_Tag] == m_ActiveVariationMerged;
			d.m_Selected = sel;
		}
		m_IngrList.Refresh();
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

	// Клик по ингредиенту с вариациями — панель выбора варианта (как в старом меню).
	protected void OnIngrAction(int index, string action, int arg, DRL_UITileList list)
	{
		DRL_UITileData d = list.GetItem(index);
		if (!d || d.m_Tag < 0)
			return;
		m_CardTag = d.m_Tag;
		ShowIngrCardForTag(d.m_Tag);
		if (d.m_Tag >= TAG_TOOL || action != "tile")
			return;
		if (d.m_Tag < m_Merged.Count())
			ToggleVariationList(m_Merged[d.m_Tag]);
	}

	// Наведение — карточка ингредиента; курсор ушёл — карточка остаётся.
	protected void OnIngrHover(int index, DRL_UITileList list)
	{
		if (index < 0)
			return;
		DRL_UITileData d = list.GetItem(index);
		if (!d || d.m_Kind != DRL_UI_TILE_ITEM || d.m_Tag < 0)
			return;
		m_CardTag = d.m_Tag;
		ShowIngrCardForTag(d.m_Tag);
	}

	// Карточка — на последней наведённой плитке, если она осталась, иначе на первой.
	protected void RefreshIngrCard()
	{
		int first = -1;
		bool found = false;
		for (int i = 0; i < m_IngrList.Count(); i++)
		{
			DRL_UITileData d = m_IngrList.GetItem(i);
			if (!d || d.m_Kind != DRL_UI_TILE_ITEM)
				continue;
			if (first < 0)
				first = d.m_Tag;
			if (d.m_Tag == m_CardTag)
			{
				found = true;
				break;
			}
		}
		if (!found)
			m_CardTag = first;
		ShowIngrCardForTag(m_CardTag);
	}

	// = FillIngredientDetailInfo для плитки (ингредиент или инструмент).
	protected void ShowIngrCardForTag(int tag)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r || tag < 0)
		{
			m_IngrCard.ShowEmpty();
			return;
		}
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		if (tag >= TAG_TOOL)
		{
			int ti = tag - TAG_TOOL;
			if (ti >= m_ToolComps.Count() || !m_ToolComps[ti])
			{
				m_IngrCard.ShowEmpty();
				return;
			}
			DRL_Craft_Component tc = m_ToolComps[ti];
			string token = tc.Classname;
			if (token == "" && tc.Classnames && tc.Classnames.Count() > 0)
				token = tc.Classnames[0];
			EntityAI present = FindAttachment(tc);
			string toolCls = "";
			string haveT = "";
			int haveC = 0;
			if (present)
			{
				toolCls = present.GetType();
				haveT = DRL_UIText.Tr("#STR_DRL_ATTACHMENT_INSTALLED");
				haveC = tm.Color(INT_UIF_ThemeKeys.GREEN);
			}
			else
			{
				toolCls = DRL_ItemTypeHelper.GetSafePreviewClass(token);
				haveT = DRL_UIText.Tr("#STR_DRL_ATTACHMENT_MISSING");
				haveC = tm.Color(INT_UIF_ThemeKeys.RED);
			}
			m_IngrCard.ShowFor(toolCls, tc, "1", haveT, haveC, false);
			return;
		}
		if (tag >= m_Merged.Count() || !m_Merged[tag])
		{
			m_IngrCard.ShowEmpty();
			return;
		}
		DRL_CraftMergedComponent mc = m_Merged[tag];
		DRL_Craft_Component c = mc.ToCraftComponent();
		if (!c)
		{
			m_IngrCard.ShowEmpty();
			return;
		}
		string activeCls = mc.Classname;
		if (activeCls == "")
			activeCls = c.Classname;
		int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, null, c.QuantityFrac, c.UseInheritance == 1);
		int need = NeedForClassCount(activeCls, c, r.CurrentCraftCount);
		array<string> variations = DRL_CraftMergedHelper.GetVariationsForComponent(c);
		bool hasVariations = variations && variations.Count() > 1 && c.EnableVariationSelection == 1;
		ShowIngrCard(activeCls, c, have, need, hasVariations);
	}

	protected void ShowIngrCard(string cls, DRL_Craft_Component c, int have, int need, bool hasVariations)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int col = tm.Color(INT_UIF_ThemeKeys.RED);
		if (have >= need)
			col = tm.Color(INT_UIF_ThemeKeys.GREEN);
		else if (have > 0)
			col = tm.Color(INT_UIF_ThemeKeys.AMBER);
		string needT = DRL_CraftInventoryHelper.FormatIngredientNeed(cls, c, need);
		string haveT = DRL_CraftInventoryHelper.FormatIngredientNeed(cls, c, have);
		m_IngrCard.ShowFor(cls, c, needT, haveT, col, hasVariations);
	}

	// --- доп. рецепты-навесное (= BuildAttachedRecipesList + UpdateAttachedSlotsAvailability)
	protected void BuildDopTiles(bool keepScroll)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		array<ref DRL_UITileData> items = new array<ref DRL_UITileData>();
		m_DopRecipes.Clear();
		DRL_Craft_Config cfg = Cfg();
		int picked = 0;
		if (r && r.HasAttachedRecipes() && cfg)
		{
			int red = INT_UIF.Color(INT_UIF_ThemeKeys.RED);
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

				DRL_UITileData dd = new DRL_UITileData();
				string sub = "";
				if (!learned)
				{
					sub = DRL_UIText.Tr("#STR_DRL_UI_CHIP_LOCKED");
				}
				else if (!slotOk)
				{
					sub = DRL_UIText.Tr("#STR_DRL_UI_NO_SLOT");
				}
				else
				{
					string dopTime = DRL_UIText.Time(dopR.CraftTimeSec);
					sub = DRL_UIText.Fmt("#STR_DRL_UI_DOP_TIME", dopTime);
				}
				dd.m_Title = dopR.RecipeName + " · " + sub;
				if (learned)
				{
					dd.m_Check = 0;
					if (isChecked)
						dd.m_Check = 1;
				}
				else
				{
					dd.m_Locked = true;
					dd.m_State = red;
				}
				if (isChecked)
				{
					dd.m_Qty = GetSelectedAttachedQuantity(attachedId);
					picked++;
				}
				if (dvars.Count() > 1)
				{
					int vc = dvars.Count();
					if (vc > DRL_UI_TILE_MAX_VARS)
						vc = DRL_UI_TILE_MAX_VARS;
					dd.m_VarCount = vc;
					dd.m_VarSelected = dvi;
				}
				dd.m_Info = true;
				dd.m_InfoEnabled = allowed;
				dd.m_Dim = learned && !slotOk;
				if (dopR.CraftIcon != "")
					dd.m_IconPath = DRL_CraftUIHelper.ResolveImagePath(dopR.CraftIcon);
				else if (dvi < dvars.Count())
					dd.m_PreviewClass = dvars[dvi];
				dd.m_Clickable = learned;
				dd.m_Tag = m_DopRecipes.Count();
				m_DopRecipes.Insert(dopR);
				items.Insert(dd);
			}
		}
		m_DopList.SetItems(items, keepScroll);
		if (items.Count() > 0)
			m_DopSummary.SetText(DRL_UIText.Fmt("#STR_DRL_UI_DOP_PICKED", picked.ToString(), items.Count().ToString()));
		else
			m_DopSummary.SetText("");
	}

	protected void OnDopAction(int index, string action, int arg, DRL_UITileList list)
	{
		DRL_UITileData d = list.GetItem(index);
		if (!d || d.m_Tag < 0)
			return;
		HandleDopAction(d.m_Tag, action, arg);
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
		SetCovered(true);
		RefreshIngrSelection();
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
		bool wasOpen = m_Popover && m_Popover.IsOpen();
		if (m_Popover)
			m_Popover.Close();
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		SetCovered(false);
		if (wasOpen && m_IngrList)
		{
			RefreshIngrSelection();
			RefreshIngrCard();
		}
	}

	// Закрыта крестиком.
	protected void OnPopoverClosed()
	{
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		SetCovered(false);
		RefreshIngrSelection();
		RefreshIngrCard();
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

	// Наведение на вариацию — карточка ингредиента показывает её; ушли — снова свою плитку.
	protected void OnPopHover(int index, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d || m_Popover.GetKind() != "var" || d.m_Tag < 0 || d.m_Tag >= m_VarInfos.Count())
		{
			RefreshIngrCard();
			return;
		}
		DRL_VariationClickInfo info = m_VarInfos[d.m_Tag];
		if (!info || !info.MergedComp)
		{
			RefreshIngrCard();
			return;
		}
		DRL_Craft_Component comp = null;
		if (info.TargetKey >= 0)
			comp = GetComponentByKey(info.TargetKey);
		if (!comp)
			comp = info.MergedComp.ToCraftComponent();
		if (!comp)
		{
			RefreshIngrCard();
			return;
		}
		int craftCount = 1;
		if (m_SelectedRecipe)
			craftCount = Math.Max(1, m_SelectedRecipe.CurrentCraftCount);
		string varCls = info.VariantClass;
		int have = ComputeHaveForClass(varCls, comp.MinHealthFrac, null, comp.QuantityFrac);
		int need = NeedForClassCount(varCls, comp, craftCount);
		ShowIngrCard(varCls, comp, have, need, false);
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
		SetCovered(true);
		RefreshIngrSelection();
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

	// --- предмет: описание и слоты навесного --------------------------------------------
	protected void FillItemInfo(DRL_Craft_Recipe r)
	{
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
		FillSlots(r);
	}

	// Слоты навесного результата (= RenderAttachmentIconsForClass); бирюзовые —
	// их займёт выбранное доп. навесное.
	protected void FillSlots(DRL_Craft_Recipe r)
	{
		if (!r)
			return;
		string attachClass = ChosenResult();
		if (attachClass == "")
		{
			attachClass = r.PreviewClassname;
			if (attachClass == "")
				attachClass = r.Result;
		}
		array<string> slots;
		DRL_CraftStatsHelper.GetAttachmentSlotsForClass(attachClass, slots);

		TStringArray taken = new TStringArray();
		DRL_Craft_Config cfg = Cfg();
		if (cfg)
		{
			for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
			{
				int dopId = m_SelectedAttachedRecipeIds[i];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				string dopCls = GetSelectedAttachedClass(dopId, dopR);
				TStringArray dslots;
				if (dopCls != "" && DRL_AttachmentHelper.GetAttachmentSlots(dopCls, dslots))
				{
					foreach (string ds : dslots)
					{
						taken.Insert(ds);
					}
				}
			}
		}
		array<bool> filled = new array<bool>();
		int nFilled = 0;
		for (int s = 0; s < slots.Count(); s++)
		{
			string low = slots[s];
			low.ToLower();
			bool isTaken = taken.Find(low) > -1;
			filled.Insert(isTaken);
			if (isTaken)
				nFilled++;
		}
		m_SlotStrip.SetSlots(slots, filled);
		if (slots.Count() > 0)
			m_SlotsFill.SetText(DRL_UIText.Fmt("#STR_DRL_UI_SLOTS_FILL", nFilled.ToString(), slots.Count().ToString()));
		else
			m_SlotsFill.SetText("");
	}

	// --- результат: что получится, характеристики ---------------------------------------
	protected void FillResult(DRL_Craft_Recipe r)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		string chosen = ChosenResult();
		bool ok = m_ResView.SetClass(MainPreviewClass());
		m_ResThumbGlyph.Show(!ok);
		m_ResName.SetText(DRL_UIText.DisplayName(chosen));
		string sub = "";
		if (r.ResultClasses && r.ResultClasses.Count() > 1)
		{
			int vi = r.ResultClasses.Find(chosen);
			if (vi < 0)
				vi = 0;
			int vn = vi + 1;
			sub = DRL_UIText.Fmt("#STR_DRL_UI_RES_VARIANT", vn.ToString(), r.ResultClasses.Count().ToString());
		}
		m_ResSub.SetText(sub);

		int cnt = r.ResultCount;
		if (cnt <= 0)
			cnt = 1;
		string fill = "—";
		if (r.ResultQuantityFrac > 0.0)
			fill = DRL_UIText.Pct(r.ResultQuantityFrac);
		string timeText = DRL_UIText.Time(GetTotalCraftTimeSec(r));
		string stateText = DRL_UIText.StateFromDamage(r.ResultDamageFrac);
		m_ResValues[0].SetText(timeText);
		m_ResValues[1].SetText(cnt.ToString());
		m_ResValues[2].SetText(fill);
		m_ResValues[3].SetText(stateText);
		int stateCol = tm.Color(INT_UIF_ThemeKeys.TEXT);
		if (r.ResultDamageFrac <= 0.0)
			stateCol = tm.Color(INT_UIF_ThemeKeys.GREEN);
		m_ResValues[3].SetColor(stateCol);
		FillStats(chosen);
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
	// --- «Разбор» (= BuildReCraftList / UpdateReCraftButtonState) ----------------------
	protected void BuildRecraftTiles(DRL_Craft_Recipe r, bool keepScroll)
	{
		array<ref DRL_UITileData> items = new array<ref DRL_UITileData>();
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
				DRL_UITileData d = new DRL_UITileData();
				string name = DRL_UIText.DisplayName(c.Classname);
				d.m_Title = name;
				if (c.RecraftDropChance < 1.0)
				{
					string chance = DRL_UIText.Pct(c.RecraftDropChance);
					string chanceText = DRL_UIText.Fmt("#STR_DRL_UI_RECRAFT_CHANCE", chance);
					d.m_Corner = chance;
					d.m_Title = name + " · " + chanceText;
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
				d.m_Count = t;
				d.m_PreviewClass = c.Classname;
				d.m_Clickable = false;
				d.m_Tag = i;
				items.Insert(d);
			}
		}
		m_RetList.SetItems(items, keepScroll);
		bool hasDrops = items.Count() > 0;
		m_RecraftBtn.Show(hasDrops);
		string head = DRL_UIText.Tr("#STR_DRL_UI_TAB_RECRAFT");
		if (r && hasDrops)
		{
			string recraftTime = DRL_UIText.Time(r.GetRecraftTimeSec());
			head = head + " · " + recraftTime;
		}
		m_RecraftHead.SetText(head);
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
			SetRecraftButton(false, "#STR_DRL_UI_NO_ITEM");
			return;
		}
		if (HasResultItemForRecraft(m_SelectedRecipe))
			SetRecraftButton(true, "#STR_DRL_RECRAFT");
		else
			SetRecraftButton(false, "#STR_DRL_UI_NO_ITEM");
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
		int top = Math.Max(1, maxN);
		m_CountMinus.SetEnabled(n > 1);
		m_CountPlus.SetEnabled(n < top);
		m_CountMax.SetText(DRL_UIText.Fmt("#STR_DRL_UI_COUNT_MAX", top.ToString()));
		// Слайдер повторяет ±: диапазон 1..максимум (у слайдера максимум > минимума).
		m_CountSlider.SetRange(1, Math.Max(2, top), 1);
		m_CountSlider.SetValue(n, false, false);
		m_CountSlider.SetEnabled(top > 1);
		int total = r.ResultCount * n;
		m_TotalText.SetText(DRL_UIText.Fmt("#STR_DRL_UI_TOTAL", total.ToString()));
		int t = Math.Max(0, GetTotalCraftTimeSec(r) * n);
		m_FootTime.SetText(DRL_UIText.Time(t));
	}

	// Перетаскивание — только число на экране; отпустили — пересчёт деталей.
	protected void OnCountSlider(float value, INT_UIF_Slider s, bool isFinal)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		int top = Math.Max(1, GetMaxCraftable(r));
		int n = Math.Clamp(Math.Round(value), 1, top);
		if (!isFinal)
		{
			m_CountText.SetText(n.ToString());
			return;
		}
		if (n == r.CurrentCraftCount)
		{
			RefreshCountUI();
			return;
		}
		r.CurrentCraftCount = n;
		FillDetails();
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
		return m_Rail.Count();
	}

	void TestSelectCategory(int entryIndex)
	{
		m_Rail.Select(entryIndex, true);
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
		if (cmd == "avail")
		{
			bool on = n != 0;
			m_OnlyAvail.SetOn(on, false, false);
			OnOnlyAvailChanged(on, m_OnlyAvail);
			return "only=" + m_OnlyAvailable.ToString() + " recipes=" + TestRecipeCount().ToString();
		}
		if (cmd == "var")
		{
			m_Variants.TestSelect(n);
			return "result=" + m_SelectedResultClass + " " + TestState();
		}
		// Плитка ингредиента: клик (вариации) и наведение (карточка).
		if (cmd == "row")
		{
			m_IngrList.TestAction(n, "tile", 0);
			return TestState();
		}
		if (cmd == "hrow")
		{
			OnIngrHover(n, m_IngrList);
			return "card=" + m_CardTag.ToString();
		}
		// Плитка доп. навесного.
		if (cmd == "drow" || cmd == "chk" || cmd == "qplus" || cmd == "qminus" || cmd == "info")
		{
			string act = "tile";
			if (cmd == "chk")
				act = "check";
			else if (cmd == "qplus")
				act = "plus";
			else if (cmd == "qminus")
				act = "minus";
			else if (cmd == "info")
				act = "info";
			m_DopList.TestAction(n, act, 0);
			return TestState();
		}
		if (cmd == "dv")
		{
			DRL_UITileData d = m_DopList.GetItem(n);
			int nextVar = 0;
			if (d && d.m_VarCount > 1)
				nextVar = (d.m_VarSelected + 1) % d.m_VarCount;
			m_DopList.TestAction(n, "var", nextVar);
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
		if (cmd == "count")
		{
			OnCountSlider(n, m_CountSlider, true);
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
		s = s + " button='" + m_CraftText + "' ingr=" + m_IngrList.Count().ToString();
		s = s + " dop=" + m_DopList.Count().ToString() + " ret=" + m_RetList.Count().ToString();
		s = s + " attached=" + m_SelectedAttachedRecipeIds.Count().ToString();
		s = s + " procs=" + m_Procs.Count().ToString() + " pop=" + m_Popover.GetKind();
		return s;
	}

	override void OnTestCommand(string command)
	{
		DRL_UITest.Apply(this, command);
	}

	// Мышь попадает в хит-зону, у которой свой обработчик (список рецептов, категории).
	// dayz-mcp ui_click вызывает обработчик меню — пробрасываем клик дальше.
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w)
		{
			string n = w.GetName();
			if (n.IndexOf("DRL_Card_") == 0 && m_Grid && m_Grid.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Cat_") == 0 && m_Rail && m_Rail.OnClick(w, x, y, button))
				return true;
		}
		return super.OnClick(w, x, y, button);
	}
}
