// ============================================================================
// DRL_CraftMenu - окно крафта на INT_UIFramework (замена DRL_CraftMenuUI).
//
// Раскладка v3 по макету «макет верстака DRL» (окно 1680x920, шапка
// фреймворка скрыта, тело на всё окно):
//   слева  (350)  - станция, поиск, сетка категорий, список рецептов со статусом;
//   центр  (1014) - лист: номер и название рецепта, штамп (время, выход,
//                   состояние), «Требования»; «Чертёж» (DRL_UIBlueprint) с
//                   изделием, выносками, обвесами, вариантами и навесным;
//                   внизу количество, «получите / время» и кнопка крафта;
//   справа (316)  - очередь карточками, «Изучение», обвесы станции, разбор,
//                   свободные слоты.
// Акцент зависит от станции (верстак, швейная машинка, подъёмник); свой вид
// листа у швейной машинки и подъёмника - следующий шаг, сейчас у всех «Чертёж».
// Логика выбора, фильтров, проверок, состояний кнопки и RPC повторяет старое
// меню 1:1 и идёт через те же хелперы (DRL_CraftInventoryHelper,
// DRL_CraftMergedHelper, DRL_CraftComponentResolver и т.д.).
// Сервер (обфусцированный DRL_CraftMenu_Server.pbo) не меняется.
// ============================================================================

// Вид станции: от него акцент окна и вид листа.
const int DRL_ST_FREE = 0;
const int DRL_ST_WB = 1;
const int DRL_ST_SEW = 2;
const int DRL_ST_LIFT = 3;

class DRL_CraftWindow extends INT_UIF_Menu
{
	static const float WIN_W = 1680;
	static const float WIN_H = 920;
	static const float LEFT_W = 350;
	static const float RIGHT_W = 316;
	static const float LPAD = 16;
	static const float SHEET_HEAD_H = 112;
	static const float BAR_H = 72;
	static const float RECIPE_ROW_H = 52;
	static const float RECIPE_ROW_GAP = 4;
	static const float CAT_CELL_H = 36;
	static const float CAT_Y = 118;
	static const float RET_TILE_H = 50;
	static const int MAX_QUEUE_ROWS = 5;
	static const int MAX_STATION_SLOTS = 12;
	static const int MAX_CATS = 16;
	// Кнопка «Изучение» в правой колонке (пока не нужна - скрыта).
	static const bool SHOW_LEARNING = false;

	// Теги выносок: ингредиент (индекс в m_Merged) или инструмент/обвес.
	static const int TAG_TOOL = 1000;

	// = DRL_RecipeVarWidget.MAX_ATTACHED_QUANTITY
	static const int MAX_ATTACHED_QUANTITY = 10;

	// Контекст: верстак (null - свободное меню F9).
	protected EntityAI m_ContextWorkbench;
	protected string m_WorkbenchCls;
	protected int m_StationKind;

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

	// Данные за выносками и списками.
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
	protected ref DRL_UIBlueprint m_Bp;
	protected DRL_UIStage m_Stage;            // сцена изделия внутри чертежа (владелец - m_Bp)
	protected ref DRL_UITileList m_RetList;
	protected ref DRL_UIPopover m_Popover;
	protected ref DRL_UIQueueList m_Queue;
	protected ref DRL_UIStationSlots m_StSlots;
	protected ref DRL_UIPill m_LearnPill;
	protected ref DRL_UIPill m_ClosePill;
	protected ref DRL_UIPill m_ReqPill;
	protected ref DRL_UIPill m_RecraftPill;
	protected ref DRL_UIPill m_StepMinus;
	protected ref DRL_UIPill m_StepPlus;
	protected ref array<ref DRL_UIPill> m_CountChips;
	protected ref array<int> m_CountChipValues;
	protected ref DRL_UIBigBtn m_CraftBtn;
	protected INT_UIF_TextInput m_Search;
	protected INT_UIF_Toggle m_OnlyAvail;
	protected string m_CatTitle;
	protected bool m_OnlyAvailable;
	protected bool m_HasLearning;

	// Левая колонка.
	protected Widget m_Left;
	protected TextWidget m_SecTitle;
	protected TextWidget m_SecCount;

	// Лист.
	protected Widget m_Sheet;
	protected float m_SheetW;
	protected Widget m_SheetEmpty;
	protected Widget m_SheetBody;
	protected TextWidget m_DocLine;
	protected TextWidget m_RecipeTitle;
	protected MultilineTextWidget m_RecipeDesc;
	protected Widget m_Stamp;
	protected ref INT_UIF_Surface m_StampBg;
	protected ref array<TextWidget> m_StampLabels;
	protected ref array<TextWidget> m_StampValues;
	protected ref array<ImageWidget> m_StampSeps;
	protected Widget m_Bar;
	protected TextWidget m_CountText;
	protected TextWidget m_TotalCount;
	protected TextWidget m_TotalTime;
	protected ImageWidget m_TotalClock;
	protected float m_TotalRight;

	// Правая колонка.
	protected Widget m_Right;
	protected TextWidget m_QueueCount;
	protected TextWidget m_QueueMax;
	protected Widget m_RecraftCard;
	protected TextWidget m_RecraftName;
	protected TextWidget m_RecraftTime;
	protected ImageWidget m_RecraftTrack;
	protected ImageWidget m_RecraftFill;
	protected TextWidget m_InvText;
	protected TextWidget m_InvValue;
	protected float m_RecraftBarW;
	protected float m_RecraftY;

	// Характеристики (ячейки внизу чертежа).
	protected Widget m_StatsBox;
	protected ref array<TextWidget> m_StatValues;
	protected ref array<TextWidget> m_StatLabels;

	protected string m_RecraftText;
	protected string m_CraftText;
	protected float m_SlotTimer;
	protected int m_RunRemain = -1;
	protected float m_LateRefresh;

	void DRL_CraftWindow()
	{
		m_Rail = new DRL_UICategoryRail();
		m_Grid = new DRL_UIRecipeGrid();
		m_Bp = new DRL_UIBlueprint();
		m_Stage = m_Bp.GetStage();
		m_RetList = new DRL_UITileList();
		m_Popover = new DRL_UIPopover();
		m_Queue = new DRL_UIQueueList();
		m_StSlots = new DRL_UIStationSlots();
		m_LearnPill = new DRL_UIPill();
		m_ClosePill = new DRL_UIPill();
		m_ReqPill = new DRL_UIPill();
		m_RecraftPill = new DRL_UIPill();
		m_StepMinus = new DRL_UIPill();
		m_StepPlus = new DRL_UIPill();
		m_CountChips = new array<ref DRL_UIPill>();
		m_CountChipValues = new array<int>();
		m_CraftBtn = new DRL_UIBigBtn();
		m_StampLabels = new array<TextWidget>();
		m_StampValues = new array<TextWidget>();
		m_StampSeps = new array<ImageWidget>();

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
		m_StatLabels = new array<TextWidget>();
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
		w = WIN_W;
		h = WIN_H;
	}

	// Поле поиска требует клавиатуру.
	override bool WantsKeyboard()
	{
		return true;
	}

	// Шапка фреймворка не используется: станция - в левой колонке, очередь - в правой.
	override void OnBuildHeader(Widget center, Widget right)
	{
	}

	protected int DetectStation()
	{
		if (!m_ContextWorkbench)
			return DRL_ST_FREE;
		if (m_ContextWorkbench.IsKindOf("DRL_Craft_Sewing"))
			return DRL_ST_SEW;
		if (m_ContextWorkbench.IsKindOf("DRL_Craft_Truck_Lift"))
			return DRL_ST_LIFT;
		return DRL_ST_WB;
	}

	override void OnBuild(Widget body)
	{
		m_StationKind = DetectStation();
		int palette = 0;
		if (m_StationKind == DRL_ST_SEW)
			palette = 1;
		else if (m_StationKind == DRL_ST_LIFT)
			palette = 2;
		DRL_UIC.SetStation(palette);

		// Тело на всё окно: шапку фреймворка заменяют колонки.
		if (m_Header)
			m_Header.Show(false);
		INT_UIF_Draw.Place(body, 0, 0, WIN_W, WIN_H);

		m_SheetW = WIN_W - LEFT_W - RIGHT_W;
		m_Left = INT_UIF_Draw.Frame(body, 0, 0, LEFT_W, WIN_H, "DRL_ColRecipes");
		m_Sheet = INT_UIF_Draw.Frame(body, LEFT_W, 0, m_SheetW, WIN_H, "DRL_Sheet");
		m_Right = INT_UIF_Draw.Frame(body, LEFT_W + m_SheetW, 0, RIGHT_W, WIN_H, "DRL_ColRight");
		BuildSheet(m_Sheet, m_SheetW, WIN_H);
		BuildLeft(m_Left, LEFT_W, WIN_H);
		BuildRight(m_Right, RIGHT_W, WIN_H);
		RevealOnOpen(m_Left);
		RevealOnOpen(m_Sheet);
		RevealOnOpen(m_Right);

		// Панель вариаций ингредиента / требований - поверх чертежа.
		float popW = 600;
		float popY = SHEET_HEAD_H + 6;
		float popH = WIN_H - popY - BAR_H - 10;
		m_Popover.Build(m_Bag, m_Sheet, (m_SheetW - popW) * 0.5, popY, popW, popH);
		m_Popover.OnClose().Insert(OnPopoverClosed);
		m_Popover.GetList().OnAction().Insert(OnPopAction);
		m_Popover.GetList().OnHover().Insert(OnPopHover);

		ShowDetails(false, false);

		// Подсказки клавиш под окном не нужны (решение 2026-10-27).
		if (m_Hints)
			INT_UIF_Widgets.ClearChildren(m_Hints);
	}

	// --- слева: станция, поиск, категории, список ------------------------------------------
	protected string StationIcon()
	{
		if (m_StationKind == DRL_ST_FREE)
			return "list";
		if (m_StationKind == DRL_ST_SEW)
			return "pin";
		if (m_StationKind == DRL_ST_LIFT)
			return "bolt";
		return "gear";
	}

	protected string StationCaption()
	{
		string cap = "#STR_DRL_UI_ST_CAP_WB";
		if (m_StationKind == DRL_ST_FREE)
			cap = "#STR_DRL_UI_ST_CAP_FREE";
		else if (m_StationKind == DRL_ST_SEW)
			cap = "#STR_DRL_UI_ST_CAP_SEW";
		else if (m_StationKind == DRL_ST_LIFT)
			cap = "#STR_DRL_UI_ST_CAP_LIFT";
		string s = DRL_UIText.Tr(cap);
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb && pb.GetIdentity())
		{
			string pn = pb.GetIdentity().GetName();
			if (pn != "")
				s = s + " · " + pn;
		}
		s.ToUpper();
		return s;
	}

	protected string StationTitle()
	{
		if (!m_ContextWorkbench)
			return DRL_UIText.Tr("#STR_DRL_UI_FREE_TITLE");
		string dn = m_ContextWorkbench.GetDisplayName();
		if (dn == "")
			dn = DRL_UIText.Tr("#STR_WBName");
		return dn;
	}

	// Сколько категорий с рецептами в этом контексте (для высоты сетки).
	protected int CountShownCategories()
	{
		DRL_Craft_Config cfg = Cfg();
		if (!cfg || !cfg.Categories)
			return 0;
		int n = 0;
		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			if (cfg.Categories[i] && CountCategoryRecipes(i) > 0)
				n++;
		}
		return n;
	}

	protected float CatGridHeight(int count)
	{
		if (count > MAX_CATS)
			count = MAX_CATS;
		int cols = count;
		if (cols > DRL_UICategoryRail.GRID_COLS)
			cols = DRL_UICategoryRail.GRID_COLS;
		if (cols < 1)
			cols = 1;
		int rows = (count + cols - 1) / cols;
		if (rows < 1)
			rows = 1;
		return rows * CAT_CELL_H + (rows - 1) * DRL_UICategoryRail.GRID_GAP + 6;
	}

	protected void BuildLeft(Widget col, float w, float h)
	{
		float cw = w - 2 * LPAD;
		int accent = DRL_UIC.A();
		int txt = DRL_UIC.Txt();
		int dim = DRL_UIC.Dim();

		int tileFill = DRL_UIC.A(36);
		int tileBorder = DRL_UIC.A(82);
		INT_UIF_Surface.Box(col, LPAD, 16, 42, 42, 10, tileFill, tileBorder);
		INT_UIF_Draw.Icon(col, LPAD + 11, 27, 20, StationIcon(), accent);
		TextWidget cap = INT_UIF_Draw.Text(col, LPAD + 53, 17, cw - 53, 14, StationCaption(), 10, accent);
		cap.SetBold(true);
		// Длинное название станции - шрифтом мельче, чтобы влезло целиком.
		string stTitle = StationTitle();
		float titleSize = 21;
		float avail = cw - 53;
		if (DRL_UIMeasure.TextW(col, stTitle, titleSize, true) > avail)
			titleSize = 17;
		if (DRL_UIMeasure.TextW(col, stTitle, titleSize, true) > avail)
			titleSize = 14;
		TextWidget title = INT_UIF_Draw.Text(col, LPAD + 53, 31, avail, 28, stTitle, titleSize, txt);
		title.SetBold(true);

		m_Search = INT_UIF_TextInput.Create(m_Bag, col, LPAD, 70, cw, 38, "#STR_SEARCH_RECIPE", 40, "search", "");
		m_Search.OnChanged().Insert(OnSearchChanged);
		m_Search.GetRoot().SetName("DRL_Search");

		m_Rail.BuildGrid(col, LPAD, CAT_Y, cw, CAT_CELL_H, MAX_CATS);
		m_Rail.OnSelected().Insert(OnCategorySelected);
		float catH = CatGridHeight(CountShownCategories());

		float sy = CAT_Y + catH + 12;
		m_SecTitle = INT_UIF_Draw.Text(col, LPAD, sy, cw * 0.62, 18, "", 11, txt);
		m_SecTitle.SetBold(true);
		m_SecCount = INT_UIF_Draw.Text(col, LPAD + cw * 0.4, sy, cw * 0.6, 18, "", 11, dim, INT_UIF_Align.RIGHT);
		m_SecCount.SetBold(true);
		m_OnlyAvail = INT_UIF_Toggle.Create(m_Bag, col, LPAD, sy + 22, cw, "#STR_DRL_UI_ONLY_AVAILABLE", false, "", 26);
		m_OnlyAvail.OnChanged().Insert(OnOnlyAvailChanged);
		DRL_UIKitUtil.Expose(m_OnlyAvail, "DRL_OnlyAvail");

		float ly = sy + 22 + 26 + 8;
		float lh = h - ly - LPAD;
		int rows = (int)((lh + RECIPE_ROW_GAP) / (RECIPE_ROW_H + RECIPE_ROW_GAP));
		if (rows < 1)
			rows = 1;
		float gh = rows * RECIPE_ROW_H + (rows - 1) * RECIPE_ROW_GAP;
		m_Grid.SetLineMode(true);
		m_Grid.Build(col, LPAD, ly, cw + 10, gh, 1, rows, RECIPE_ROW_GAP);
		m_Grid.OnSelected().Insert(OnGridSelected);
	}

	// --- центр: лист ------------------------------------------------------------------------
	protected void BuildSheet(Widget sheet, float w, float h)
	{
		// Вид листа по станции: швейная машинка - «Раскрой», остальные - «Чертёж».
		bool sew = m_StationKind == DRL_ST_SEW;
		if (sew)
		{
			m_Bp = new DRL_UICutSheet();
			m_Stage = m_Bp.GetStage();
		}

		// Подложка и сетка листа (с отступом в пиксель - рамка окна не перекрывается).
		int bg = ARGB(245, 6, 16, 18);
		float cellSize = 24;
		int minor = DRL_UIC.BP(9);
		int major = DRL_UIC.BP(18);
		int glow = DRL_UIC.BP(24);
		if (sew)
		{
			// Раскройный стол: тёмно-зелёное сукно, крупная клетка.
			bg = ARGB(245, 15, 26, 21);
			cellSize = 40;
			minor = DRL_UIC.BP(14);
			major = DRL_UIC.BP(14);
			glow = ARGB(90, 60, 90, 70);
		}
		INT_UIF_Draw.Image(sheet, 0, 1, w, h - 2, "iuf_white", bg);
		DRL_UICanvas grid = new DRL_UICanvas();
		if (grid.Build(sheet, 0, 1, w, h - 2))
			grid.Grid(w, h - 2, cellSize, minor, major);
		INT_UIF_Draw.Image(sheet, w * 0.5 - w * 0.6, h * 0.48 - h * 0.55, w * 1.2, h * 1.1, "iuf_radial", glow);
		int border = DRL_UIC.BP(56);
		INT_UIF_Draw.Line(sheet, 0, 1, 1, h - 2, border);
		INT_UIF_Draw.Line(sheet, w - 1, 1, 1, h - 2, border);

		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int dim = DRL_UIC.Dim();

		// Пусто: рецепт не выбран.
		m_SheetEmpty = INT_UIF_Draw.Frame(sheet, 0, 0, w, h, "DRL_SheetEmpty");
		int emptyGlow = DRL_UIC.BP(20);
		INT_UIF_Draw.Image(m_SheetEmpty, w * 0.5 - 140, h * 0.5 - 170, 280, 280, "iuf_radial", emptyGlow);
		int bp = DRL_UIC.BP(150);
		INT_UIF_Draw.Icon(m_SheetEmpty, w * 0.5 - 24, h * 0.5 - 60, 48, "gear", bp);
		INT_UIF_Draw.Text(m_SheetEmpty, 0, h * 0.5 + 4, w, 24, "#STR_DRL_UI_SELECT_HINT", 16, muted, INT_UIF_Align.CENTER);

		m_SheetBody = INT_UIF_Draw.Frame(sheet, 0, 0, w, h, "DRL_SheetBody");
		int docColor = DRL_UIC.BP(224);
		m_DocLine = INT_UIF_Draw.Text(m_SheetBody, 20, 16, w * 0.6, 14, "", 10, docColor);
		m_DocLine.SetBold(true);
		m_RecipeTitle = INT_UIF_Draw.Text(m_SheetBody, 20, 32, w - 400, 32, "", 24, txt);
		m_RecipeTitle.SetBold(true);
		m_RecipeDesc = INT_UIF_Draw.MText(m_SheetBody, 20, 66, 560, 36, "", 12, muted);

		// Штамп: время, выход, состояние, наполнение.
		m_Stamp = INT_UIF_Draw.Frame(m_SheetBody, w - 20 - 300, 16, 300, 46, "DRL_Stamp");
		int stampFill = ARGB(160, 4, 12, 14);
		int stampBorder = DRL_UIC.BP(90);
		m_StampBg = INT_UIF_Surface.Box(m_Stamp, 0, 0, 300, 46, 8, stampFill, stampBorder);
		int sep = DRL_UIC.BP(56);
		int stampLabel = DRL_UIC.BP(190);
		for (int i = 0; i < 4; i++)
		{
			TextWidget sl = INT_UIF_Draw.Text(m_Stamp, 0, 6, 80, 14, "", 9, stampLabel);
			sl.SetBold(true);
			m_StampLabels.Insert(sl);
			TextWidget sv = INT_UIF_Draw.Text(m_Stamp, 0, 20, 80, 20, "", 14, txt);
			sv.SetBold(true);
			m_StampValues.Insert(sv);
			ImageWidget ss = INT_UIF_Draw.Line(m_Stamp, 0, 0, 1, 46, sep);
			m_StampSeps.Insert(ss);
		}
		m_ReqPill.Build(m_SheetBody, w - 20 - 200, 70, 30, 8, 12, "DRL_ReqBtn");
		m_ReqPill.OnClick().Insert(OnReqPill);

		// Чертёж.
		float dy = SHEET_HEAD_H;
		float dh = h - dy - BAR_H;
		m_Bp.Build(m_SheetBody, 0, dy, w, dh);
		m_Bp.OnAction().Insert(OnBpAction);
		m_StatsBox = m_Bp.GetStatsRoot();
		m_StatLabels = m_Bp.GetStatLabels();
		m_StatValues = m_Bp.GetStatValues();

		BuildBar(m_SheetBody, w, h);
	}

	// Низ листа: количество, «получите / время», кнопка крафта.
	protected void BuildBar(Widget body, float w, float h)
	{
		m_Bar = INT_UIF_Draw.Frame(body, 0, h - BAR_H, w, BAR_H, "DRL_Bar");
		int barFill = ARGB(225, 4, 10, 12);
		INT_UIF_Draw.Image(m_Bar, 1, 0, w - 2, BAR_H - 1, "iuf_white", barFill);
		int line = DRL_UIC.BP(56);
		INT_UIF_Draw.Line(m_Bar, 1, 0, w - 2, 1, line);
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int dim = DRL_UIC.Dim();

		int stepFill = DRL_UIC.White(15);
		INT_UIF_Surface.Box(m_Bar, 16, 17, 118, 38, 10, stepFill, 0);
		m_StepMinus.Build(m_Bar, 19, 20, 32, 8, 16, "DRL_CountMinus");
		m_StepMinus.Set("", "minus", 32);
		m_StepMinus.SetColors(0, 0, muted);
		m_StepMinus.OnClick().Insert(OnStepMinus);
		m_CountText = INT_UIF_Draw.Text(m_Bar, 53, 17, 44, 38, "1", 16, txt, INT_UIF_Align.CENTER);
		m_CountText.SetBold(true);
		m_StepPlus.Build(m_Bar, 99, 20, 32, 8, 16, "DRL_CountPlus");
		m_StepPlus.Set("", "plus", 32);
		m_StepPlus.SetColors(0, 0, muted);
		m_StepPlus.OnClick().Insert(OnStepPlus);

		// ×1 ×5 ×10 «Макс N»: значения ставит RefreshCountUI.
		for (int i = 0; i < 4; i++)
		{
			DRL_UIPill chip = new DRL_UIPill();
			chip.Build(m_Bar, 0, 17, 38, 8, 12, "DRL_CountChip_" + i.ToString());
			chip.OnClick().Insert(OnCountChip);
			m_CountChips.Insert(chip);
			m_CountChipValues.Insert(1);
		}

		float btnW = 212;
		m_TotalRight = w - 16 - btnW - 14;
		TextWidget cap = INT_UIF_Draw.Text(m_Bar, m_TotalRight - 220, 15, 220, 14, "#STR_DRL_UI_GET_TIME", 10, dim, INT_UIF_Align.RIGHT);
		cap.SetBold(true);
		m_TotalTime = INT_UIF_Draw.Text(m_Bar, m_TotalRight - 120, 30, 120, 24, "", 16, txt, INT_UIF_Align.RIGHT);
		m_TotalTime.SetBold(true);
		int bp = DRL_UIC.BP();
		m_TotalClock = INT_UIF_Draw.Icon(m_Bar, m_TotalRight - 140, 35, 14, "clock", bp);
		m_TotalCount = INT_UIF_Draw.Text(m_Bar, m_TotalRight - 360, 30, 200, 24, "", 16, txt, INT_UIF_Align.RIGHT);
		m_TotalCount.SetBold(true);

		m_CraftBtn.Build(m_Bar, w - 16 - btnW, 12, btnW, 48, "DRL_CraftBtn");
		m_CraftBtn.OnClick().Insert(OnCraftBtn);
	}

	// --- справа: очередь, обвесы станции, разбор ---------------------------------------------
	protected void BuildRight(Widget col, float w, float h)
	{
		float cw = w - 2 * LPAD;
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int dim = DRL_UIC.Dim();

		TextWidget cap = INT_UIF_Draw.Text(col, LPAD, 17, 140, 14, "#STR_DRL_UI_QUEUE_CAP", 10, dim);
		cap.SetBold(true);
		m_QueueCount = INT_UIF_Draw.Text(col, LPAD, 31, 40, 28, "0", 21, txt);
		m_QueueCount.SetBold(true);
		m_QueueMax = INT_UIF_Draw.Text(col, LPAD + 20, 37, 80, 20, "", 13, dim);
		m_QueueMax.SetBold(true);

		// Крестик как у окон фреймворка: плитка 32x32, значок 12 по центру.
		m_ClosePill.Build(col, w - LPAD - 32, 21, 32, 10, 12, "DRL_CloseBtn");
		m_ClosePill.Set("", "close", 32);
		int closeFill = INT_UIF.Color(INT_UIF_ThemeKeys.TILE);
		int closeBorder = DRL_UIC.White(20);
		m_ClosePill.SetColors(closeFill, closeBorder, muted);
		m_ClosePill.OnClick().Insert(OnCloseClick);

		// Кнопка «Изучение» пока скрыта (решение 2026-11-09): изучение на серверах не используется.
		DRL_Craft_Config cfg = Cfg();
		m_HasLearning = false;
		if (SHOW_LEARNING && cfg && cfg.IsLearningEnabled() && cfg.IsFreeLearningMenuEnabled())
			m_HasLearning = true;
		if (m_HasLearning)
		{
			m_LearnPill.Build(col, 0, 20, 34, 9, 12, "DRL_LearningBtn");
			float lw = m_LearnPill.Set("#STR_DRL_UI_LEARNING", "star");
			m_LearnPill.Move(w - LPAD - 34 - 8 - lw, 20);
			int learnFill = DRL_UIC.Lk(30);
			int learnBorder = DRL_UIC.Lk(102);
			int learnFg = DRL_UIC.Lk();
			m_LearnPill.SetColors(learnFill, learnBorder, learnFg);
			m_LearnPill.OnClick().Insert(OnLearningPill);
		}

		m_Queue.Build(col, LPAD, 74, cw, 52, 5, MAX_QUEUE_ROWS);
		m_Queue.OnAction().Insert(OnQueueAction);
		m_StSlots.Build(col, LPAD, 200, cw, MAX_STATION_SLOTS);

		BuildRecraftCard(col, w, h);

		int blue = DRL_UIC.Blue();
		INT_UIF_Draw.Icon(col, LPAD, h - 31, 12, "list", dim);
		m_InvText = INT_UIF_Draw.Text(col, LPAD + 18, h - 34, cw - 90, 18, "", 11, dim);
		m_InvText.SetBold(true);
		m_InvValue = INT_UIF_Draw.Text(col, LPAD + cw - 72, h - 34, 72, 18, "", 11, blue, INT_UIF_Align.RIGHT);
		m_InvValue.SetBold(true);
	}

	protected void BuildRecraftCard(Widget col, float w, float h)
	{
		float cw = w - 2 * LPAD;
		float tilesH = RET_TILE_H * 2 + 5;
		float ch = 12 + 36 + 8 + tilesH + 10 + 36 + 6 + 3 + 12;
		float y = h - 44 - ch;
		m_RecraftY = y;
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int dim = DRL_UIC.Dim();
		m_RecraftCard = INT_UIF_Draw.Frame(col, LPAD, y, cw, ch, "DRL_Recraft");
		int cardFill = ARGB(77, 0, 0, 0);
		int cardBorder = DRL_UIC.White(20);
		INT_UIF_Surface.Box(m_RecraftCard, 0, 0, cw, ch, 12, cardFill, cardBorder);
		INT_UIF_Draw.Icon(m_RecraftCard, 12, 22, 18, "refresh", muted);
		TextWidget cap = INT_UIF_Draw.Text(m_RecraftCard, 40, 11, 140, 14, "#STR_DRL_UI_RC_CAP", 10, dim);
		cap.SetBold(true);
		m_RecraftName = INT_UIF_Draw.Text(m_RecraftCard, 40, 25, cw - 40 - 74, 22, "", 14, txt);
		m_RecraftName.SetBold(true);
		m_RecraftTime = INT_UIF_Draw.Text(m_RecraftCard, cw - 86, 22, 74, 20, "", 12, muted, INT_UIF_Align.RIGHT);
		m_RecraftTime.SetBold(true);

		m_RetList.Build(m_RecraftCard, 12, 56, cw - 14, tilesH, 5, RET_TILE_H, 5, false, "DRL_Ret");
		m_RetList.SetTooltips(true);
		m_RetList.SetEmptyText("#STR_DRL_UI_RECRAFT_EMPTY");

		float by = 56 + tilesH + 10;
		m_RecraftPill.Build(m_RecraftCard, 12, by, 36, 9, 13, "DRL_RecraftBtn");
		m_RecraftPill.Set("#STR_DRL_RECRAFT", "refresh", cw - 24);
		m_RecraftPill.OnClick().Insert(OnRecraftPill);
		m_RecraftBarW = cw - 24;
		int track = DRL_UIC.White(20);
		m_RecraftTrack = INT_UIF_Draw.Line(m_RecraftCard, 12, by + 42, m_RecraftBarW, 3, track);
		int lk = DRL_UIC.Lk();
		m_RecraftFill = INT_UIF_Draw.Line(m_RecraftCard, 12, by + 42, 2, 3, lk);
		m_RecraftTrack.Show(false);
		m_RecraftFill.Show(false);
	}

	// Обвесы станции под очередью; высота очереди зависит от MaxQueueSlots.
	protected void LayoutRight(float queueH)
	{
		float y = 74 + queueH + 18;
		INT_UIF_Draw.Move(m_StSlots.GetRoot(), LPAD, y);
		RefreshStationSlots();
	}

	protected void RefreshStationSlots()
	{
		array<DRL_Craft_Component> need = new array<DRL_Craft_Component>();
		if (m_SelectedRecipe && m_SelectedRecipe.CraftComponents)
		{
			string activeResult = m_SelectedResultClass;
			foreach (DRL_Craft_Component c : m_SelectedRecipe.CraftComponents)
			{
				if (c && c.AttachmentsEx == 1 && c.IsApplicableForResult(activeResult))
					need.Insert(c);
			}
		}
		string title = "#STR_DRL_UI_ST_TITLE_WB";
		if (m_StationKind == DRL_ST_SEW)
			title = "#STR_DRL_UI_ST_TITLE_SEW";
		else if (m_StationKind == DRL_ST_LIFT)
			title = "#STR_DRL_UI_ST_TITLE_LIFT";
		else if (m_StationKind == DRL_ST_FREE)
			title = "#STR_DRL_UI_NOST_CAP";
		string titleText = DRL_UIText.Tr(title);
		string note = DRL_UIText.Tr("#STR_DRL_UI_ST_NOTE");
		if (m_StationKind == DRL_ST_FREE)
			note = DRL_UIText.Tr("#STR_DRL_UI_NOST_NOTE");
		m_StSlots.SetStation(m_ContextWorkbench, need, titleText, note);
	}

	// Тело листа: пусто или выбран рецепт.
	protected void ShowDetails(bool show, bool animate)
	{
		m_SheetEmpty.Show(!show);
		m_SheetBody.Show(show);
		m_RecraftCard.Show(show);
		if (!show)
		{
			m_Bp.HideAll();
			return;
		}
		if (!animate)
			return;
		DRL_UIAnim.Reveal(m_SheetBody, 0, 0, 0, 8, 0.24);
		DRL_UIAnim.Reveal(m_RecraftCard, LPAD, m_RecraftY, 0.06, 8, 0.24);
	}

	override void OnReady()
	{
		BuildCategories();
		RefreshQueue();
		UpdateSlotInfo();
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
		if (m_Bp)
			m_Bp.ClearModels();
		if (m_RetList)
			m_RetList.ClearModels();
		if (m_Popover)
			m_Popover.Close();
		if (m_Queue)
			m_Queue.Clear();
		if (m_StSlots)
			m_StSlots.Clear();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_Grid)
			m_Grid.Tick(timeslice);
		if (m_Bp)
			m_Bp.Tick(timeslice);
		TickProcesses(timeslice);
		TickReCraft(timeslice);
		TickLateRefresh(timeslice);
		m_SlotTimer = m_SlotTimer + timeslice;
		if (m_SlotTimer >= 1.0)
		{
			m_SlotTimer = 0;
			UpdateSlotInfo();
			RefreshStationSlots();
		}
	}

	// =================================================================================
	// Свободные слоты и кнопки колонок
	// =================================================================================
	protected void UpdateSlotInfo()
	{
		if (!m_InvValue)
			return;
		string label = "#STR_DRL_UI_INV";
		if (m_ContextWorkbench)
			label = "#STR_DRL_UI_INV_ST";
		m_InvText.SetText(DRL_UIText.Tr(label));
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
		{
			m_InvValue.SetText("—");
			return;
		}
		int totalCells = 0;
		int usedCells = 0;
		DRL_CraftInventoryHelper.CalculateInventorySlots(pb, m_ContextWorkbench, totalCells, usedCells);
		int freeCells = Math.Max(totalCells - usedCells, 0);
		m_InvValue.SetText(DRL_UIText.Fmt("#STR_DRL_UI_INV_VAL", freeCells.ToString(), totalCells.ToString()));
	}

	protected void OnLearningPill(DRL_UIPill p)
	{
		// Новое окно крафта открывает новое окно изучения.
		CloseNow();
		GetGame().GetUIManager().EnterScriptedMenu(DRL_UI_MENU_LEARNING, null);
	}

	protected void OnCloseClick(DRL_UIPill p)
	{
		RequestClose();
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
		UpdateSectionLine();
	}

	// «Артефакты · можно: 3 из 58» над списком.
	protected void UpdateSectionLine()
	{
		if (!m_SecTitle)
			return;
		int can = 0;
		int n = m_Grid.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRecipeEntry e = m_Grid.GetItem(i);
			if (e && e.m_Status == DRL_UI_STATUS_CAN)
				can++;
		}
		m_SecTitle.SetText(m_CatTitle);
		m_SecCount.SetText(DRL_UIText.Fmt("#STR_DRL_UI_CAN_OF", can.ToString(), n.ToString()));
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
		FillStatusLine(e, r, cachedItems);
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

	protected bool IsRecipeQueued(int recipeId)
	{
		foreach (DRL_UIProc p : m_Procs)
		{
			if (p && p.GetRecipeId() == recipeId)
				return true;
		}
		return false;
	}

	// Первый недостающий обвес станции (AttachmentsEx = 1) рецепта.
	protected DRL_Craft_Component FirstMissingAttachment(DRL_Craft_Recipe r, bool stationOnly)
	{
		if (!r || !r.CraftComponents)
			return null;
		string activeResult = m_SelectedResultClass;
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0)
				continue;
			if (stationOnly && c.AttachmentsEx != 1)
				continue;
			if (!c.IsApplicableForResult(activeResult))
				continue;
			if (!DRL_CraftComponentResolver.HasComponentRequirement(c, pb, m_ContextWorkbench, activeResult, 1))
				return c;
		}
		return null;
	}

	protected string ComponentToken(DRL_Craft_Component c)
	{
		if (!c)
			return "";
		string token = c.Classname;
		if (token == "" && c.Classnames && c.Classnames.Count() > 0)
			token = c.Classnames[0];
		return token;
	}

	// Строка статуса под названием (макет): в очереди, не изучен, нужен обвес,
	// не хватает, можно скрафтить N.
	protected void FillStatusLine(DRL_UIRecipeEntry e, DRL_Craft_Recipe r, array<EntityAI> cachedItems)
	{
		if (IsRecipeQueued(r.RecipeId))
		{
			e.m_LineText = DRL_UIText.Tr("#STR_DRL_UI_RL_QUEUE");
			e.m_LineIcon = "clock";
			e.m_LineColor = DRL_UIC.Blue();
			return;
		}
		if (e.m_Status == DRL_UI_STATUS_LOCKED)
		{
			e.m_LineText = DRL_UIText.Tr("#STR_DRL_UI_RL_LOCKED");
			e.m_LineIcon = "lock";
			e.m_LineColor = DRL_UIC.Lk();
			return;
		}
		if (e.m_Status != DRL_UI_STATUS_CAN)
		{
			DRL_Craft_Component att = FirstMissingAttachment(r, true);
			if (att)
			{
				string attKey = "#STR_DRL_UI_RL_NEEDATT";
				if (!m_ContextWorkbench)
					attKey = "#STR_DRL_UI_RL_ONLYST";
				e.m_LineText = DRL_UIText.Fmt(attKey, ToolName(ComponentToken(att)));
				e.m_LineIcon = "gear";
				e.m_LineColor = DRL_UIC.Tl();
				return;
			}
		}

		// Ингредиенты на один крафт: сколько раз хватит и чего нет.
		int maxN = 1000000;
		int lack = 0;
		string firstLack = "";
		if (r.CraftComponents)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c || c.AttachmentsEx > 0)
					continue;
				if (!c.IsApplicableForResult(r.Result))
					continue;
				int have = ComputeHaveForComponent(c, -1, cachedItems);
				int need1 = NeedForCount(c, 1, -1);
				if (need1 <= 0)
					continue;
				int times = have / need1;
				if (times < maxN)
					maxN = times;
				if (have < need1)
				{
					lack++;
					if (firstLack == "")
						firstLack = DRL_UIText.DisplayName(ComponentToken(c));
				}
			}
		}
		if (lack == 0 && e.m_Status != DRL_UI_STATUS_CAN)
		{
			DRL_Craft_Component tool = FirstMissingAttachment(r, false);
			if (tool)
			{
				lack = 1;
				firstLack = ToolName(ComponentToken(tool));
			}
		}
		if (lack > 0)
		{
			string lackText = DRL_UIText.Fmt("#STR_DRL_UI_RL_LACK", firstLack);
			if (lack > 1)
			{
				int more = lack - 1;
				lackText = lackText + " +" + more.ToString();
			}
			e.m_LineText = lackText;
			e.m_LineIcon = "warning";
			e.m_LineColor = DRL_UIC.NoText();
			return;
		}
		if (maxN == 1000000)
			maxN = 1;
		e.m_LineText = DRL_UIText.Fmt("#STR_DRL_UI_RL_CAN", maxN.ToString());
		e.m_LineIcon = "check";
		e.m_LineColor = DRL_UIC.Ok();
	}

	// = RefreshRecipeListColors: статусы строк после изменения инвентаря, очереди, выбора.
	protected void RefreshStatuses()
	{
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		int n = m_Grid.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRecipeEntry e = m_Grid.GetItem(i);
			if (e && e.m_Recipe)
			{
				e.m_Status = RecipeStatus(e.m_Recipe, cachedItems);
				FillStatusLine(e, e.m_Recipe, cachedItems);
			}
		}
		m_Grid.RefreshValues();
		UpdateSectionLine();
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
			// Превью, слоты навесного и чипы пересобираются вместе с листом.
			FillDetails();
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
		if (m_RetList)
			m_RetList.ClearModels();
		if (m_SheetBody)
			ShowDetails(false, false);
		if (m_StSlots && m_SheetBody)
			RefreshStationSlots();
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
		if (!sameRecipe || !m_SheetBody.IsVisible())
			ShowDetails(true, true);
		HideVariationList();

		m_RecipeTitle.SetText(r.RecipeName);
		FillDocLine(r);
		RebuildMainPreview();
		FillVariants(r);
		BuildCallouts();
		BuildDopTiles(sameRecipe);
		FillItemInfo(r);
		FillResult(r);
		BuildRecraftTiles(r, sameRecipe);
		UpdateRecraftButton();
		RefreshCountUI();
		UpdateCraftButtonState();
		RefreshStatuses();
		RefreshStationSlots();
		FillSetInfo(r);
		m_Bp.SetLocked(!DRL_LearnState.HasLearned(r.RecipeId));
		m_Bp.Relayout();
	}

	// --- сет для «Раскроя»: части и во что одеть манекен --------------------------------
	// Слоты одежды по порядку сверху вниз (порядок меток на манекене).
	protected array<string> BodySlots()
	{
		array<string> s = {"headgear", "eyewear", "mask", "body", "vest", "armband", "back", "hips", "gloves", "legs", "feet"};
		return s;
	}

	protected string FirstSlotOf(string cls)
	{
		string path = "CfgVehicles " + cls + " inventorySlot";
		if (cls == "" || !GetGame().ConfigIsExisting(path))
			return "";
		TStringArray slots = new TStringArray();
		GetGame().ConfigGetTextArray(path, slots);
		string first = "";
		if (slots.Count() > 0)
			first = slots[0];
		else
			GetGame().ConfigGetText(path, first);
		first.ToLower();
		return first;
	}

	// Название части сета по слоту вещи («Жилет», «Штаны»...), иначе название рецепта.
	protected string PartName(string cls, DRL_Craft_Recipe r)
	{
		string slot = FirstSlotOf(cls);
		if (slot != "" && BodySlots().Find(slot) > -1)
		{
			string key = "#STR_DRL_UI_PART_" + slot;
			key.ToUpper();
			return DRL_UIText.Tr(key);
		}
		if (r)
			return r.RecipeName;
		return DRL_UIText.DisplayName(cls);
	}

	protected int PartOrder(string cls)
	{
		int i = BodySlots().Find(FirstSlotOf(cls));
		if (i < 0)
			return 100;
		return i;
	}

	// Слова расцветки выбранного варианта, которых нет у основного результата
	// (Vest_Silver_Away против Vest_Silver -> «away»): по ним подбираются части того же цвета.
	protected array<string> VariantMarks(DRL_Craft_Recipe r, string chosen)
	{
		array<string> marks = new array<string>();
		if (!r || chosen == "" || chosen == r.Result)
			return marks;
		array<string> mine = ClassWords(chosen);
		array<string> baseWords = ClassWords(r.Result);
		foreach (string wd : mine)
		{
			if (baseWords.Find(wd) < 0)
				marks.Insert(wd);
		}
		return marks;
	}

	protected string PickVariant(DRL_Craft_Recipe pr, array<string> marks)
	{
		if (!pr.ResultClasses || pr.ResultClasses.Count() < 2 || marks.Count() == 0)
			return pr.Result;
		foreach (string cls : pr.ResultClasses)
		{
			array<string> words = ClassWords(cls);
			bool allMatch = true;
			foreach (string m : marks)
			{
				if (words.Find(m) < 0)
				{
					allMatch = false;
					break;
				}
			}
			if (allMatch)
				return cls;
		}
		return pr.Result;
	}

	protected void FillSetInfo(DRL_Craft_Recipe r)
	{
		if (m_StationKind != DRL_ST_SEW)
			return;
		DRL_Craft_Config cfg = Cfg();
		DRL_Craft_Category cat = ContextCategoryFor(r);
		array<ref DRL_UISetPart> parts = new array<ref DRL_UISetPart>();
		array<string> outfit = new array<string>();
		array<int> orders = new array<int>();
		string setName = "";
		string chosen = ChosenResult();
		array<string> marks = VariantMarks(r, chosen);
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);
		if (cfg && cat && cat.ShowAll != 1 && cat.UseList == 1 && cat.List)
		{
			setName = DRL_UIText.Tr(cat.Name);
			foreach (int id : cat.List)
			{
				DRL_Craft_Recipe pr = cfg.FindById(id);
				if (!pr || !RecipeAllowedByContext(pr))
					continue;
				string cls = PickVariant(pr, marks);
				if (pr == r)
					cls = chosen;
				DRL_UISetPart p = new DRL_UISetPart();
				p.m_RecipeId = pr.RecipeId;
				p.m_Name = PartName(cls, pr);
				p.m_Selected = pr == r;
				p.m_Locked = !DRL_LearnState.HasLearned(pr.RecipeId);
				p.m_Ok = RecipeStatus(pr, cachedItems) == DRL_UI_STATUS_CAN;
				// Вставка по порядку слотов сверху вниз.
				int order = PartOrder(cls);
				int at = 0;
				while (at < orders.Count() && orders[at] <= order)
				{
					at++;
				}
				parts.InsertAt(p, at);
				orders.InsertAt(order, at);
				outfit.Insert(cls);
			}
		}
		// Две вещи на один слот (два жилета) - «Жилет», «Жилет 2».
		array<string> seen = new array<string>();
		foreach (DRL_UISetPart sp : parts)
		{
			string baseName = sp.m_Name;
			int dup = 1;
			while (seen.Find(sp.m_Name) > -1)
			{
				dup++;
				sp.m_Name = baseName + " " + dup.ToString();
			}
			seen.Insert(sp.m_Name);
		}
		if (parts.Count() == 0)
		{
			DRL_UISetPart only = new DRL_UISetPart();
			only.m_RecipeId = r.RecipeId;
			only.m_Name = PartName(chosen, r);
			only.m_Selected = true;
			only.m_Ok = RecipeStatus(r, cachedItems) == DRL_UI_STATUS_CAN;
			parts.Insert(only);
			outfit.Insert(chosen);
		}
		m_Bp.SetSet(setName, parts, outfit);
	}

	// «ЧЕРТЁЖ № 800 - АРТЕФАКТЫ»: вид листа, номер рецепта и категория.
	protected void FillDocLine(DRL_Craft_Recipe r)
	{
		string doc = "#STR_DRL_UI_DOC_WB";
		if (m_StationKind == DRL_ST_SEW)
			doc = "#STR_DRL_UI_DOC_SEW";
		else if (m_StationKind == DRL_ST_LIFT)
			doc = "#STR_DRL_UI_DOC_LIFT";
		string line = DRL_UIText.Tr(doc) + "  №  " + r.RecipeId.ToString();
		DRL_Craft_Category cat = ContextCategoryFor(r);
		if (cat && cat.Name != "" && cat.ShowAll != 1)
			line = line + "  -  " + DRL_UIText.Tr(cat.Name);
		m_DocLine.SetText(line);
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

	// Варианты результата (= BuildSelectResultList / UpdateSelectCraftHighlight):
	// чипы под изделием на чертеже.
	protected void FillVariants(DRL_Craft_Recipe r)
	{
		array<string> names = new array<string>();
		array<int> colors = new array<int>();
		int sel = 0;
		if (r.ResultClasses && r.ResultClasses.Count() > 1)
		{
			sel = -1;
			if (m_SelectedResultClass != "")
				sel = r.ResultClasses.Find(m_SelectedResultClass);
			if (sel < 0 && r.Result != "")
				sel = r.ResultClasses.Find(r.Result);
			if (sel < 0)
				sel = 0;
			bool sameNames = VariantNamesEqual(r.ResultClasses);
			for (int i = 0; i < r.ResultClasses.Count(); i++)
			{
				string cls = r.ResultClasses[i];
				string vn = VariantName(cls, i, sameNames);
				// Совпавшие подписи (оба «Серебро») различаются номером.
				if (names.Find(vn) > -1)
				{
					int num = i + 1;
					vn = vn + " " + num.ToString();
				}
				names.Insert(vn);
				colors.Insert(VariantColor(cls));
			}
		}
		string label = DRL_UIText.Fmt("#STR_DRL_UI_BP_VARS", names.Count().ToString());
		m_Bp.SetVariants(names, colors, sel, label);
	}

	protected bool VariantNamesEqual(array<string> classes)
	{
		string first = "";
		for (int i = 0; i < classes.Count(); i++)
		{
			string dn = DRL_UIText.DisplayName(classes[i]);
			if (i == 0)
				first = dn;
			else if (dn != first)
				return false;
		}
		return true;
	}

	// Слова цвета из имени класса (Vest_Black_Pink -> «Чёрный / розовый»).
	protected array<string> ColorTokens()
	{
		array<string> t = {"black", "white", "red", "blue", "green", "yellow", "orange", "pink", "purple", "brown", "grey", "gray", "olive", "khaki", "tan", "beige", "camo", "desert", "pixel", "winter", "snow", "woodland", "urban", "digital", "multicam", "navy", "gold", "silver", "dark", "light", "notail", "tail"};
		return t;
	}

	protected array<string> ClassWords(string cls)
	{
		string low = cls;
		low.ToLower();
		low.Replace("no_tail", "notail");
		array<string> parts = new array<string>();
		low.Split("_", parts);
		return parts;
	}

	protected string VariantName(string cls, int index, bool sameNames)
	{
		// Своё название у варианта есть и оно отличается от соседних - его и пишем.
		string dn = DRL_UIText.DisplayName(cls);
		if (!sameNames && dn != cls)
			return dn;
		array<string> tokens = ColorTokens();
		array<string> parts = ClassWords(cls);
		string name = "";
		foreach (string p : parts)
		{
			if (tokens.Find(p) < 0)
				continue;
			string key = "#STR_DRL_UI_C_" + p;
			key.ToUpper();
			string word = DRL_UIText.Tr(key);
			if (name == "")
				name = word;
			else
				name = name + " / " + word;
		}
		if (name != "")
			return name;
		int n = index + 1;
		return DRL_UIText.Fmt("#STR_DRL_UI_VAR_N", n.ToString());
	}

	protected int VariantColor(string cls)
	{
		array<string> parts = ClassWords(cls);
		foreach (string p : parts)
		{
			if (p == "black" || p == "dark")
				return ARGB(255, 29, 31, 34);
			if (p == "white" || p == "snow" || p == "winter" || p == "light")
				return ARGB(255, 232, 232, 232);
			if (p == "red")
				return ARGB(255, 199, 58, 50);
			if (p == "blue" || p == "navy")
				return ARGB(255, 47, 98, 201);
			if (p == "green" || p == "woodland")
				return ARGB(255, 70, 130, 60);
			if (p == "olive" || p == "khaki")
				return ARGB(255, 110, 112, 60);
			if (p == "yellow" || p == "gold")
				return ARGB(255, 217, 169, 46);
			if (p == "orange")
				return ARGB(255, 230, 120, 40);
			if (p == "pink")
				return ARGB(255, 230, 120, 190);
			if (p == "purple")
				return ARGB(255, 130, 80, 190);
			if (p == "brown")
				return ARGB(255, 120, 80, 50);
			if (p == "grey" || p == "gray" || p == "silver" || p == "urban")
				return ARGB(255, 150, 156, 162);
			if (p == "tan" || p == "beige" || p == "desert")
				return ARGB(255, 214, 190, 140);
			if (p == "camo" || p == "pixel" || p == "digital" || p == "multicam")
				return ARGB(255, 100, 110, 80);
		}
		return ARGB(255, 120, 126, 132);
	}

	// = HandleSelectResultClick
	protected void OnVariantSelected(int index)
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

	// Панель поверх чертежа: 3D-модели под ней прячутся (рисуются поверх 2D).
	protected void SetCovered(bool covered)
	{
		if (m_Bp)
			m_Bp.SetCovered(covered);
	}

	// --- выноски чертежа (= BuildIngredientsListUI + BuildAtachIngridientListUI) --------
	// Ингредиенты и инструменты (AttachmentsEx 2) - выносками слева и справа,
	// обвесы станции (AttachmentsEx 1) - строкой сверху.
	protected void BuildCallouts()
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		array<ref DRL_UIBpItem> side = new array<ref DRL_UIBpItem>();
		array<ref DRL_UIBpItem> top = new array<ref DRL_UIBpItem>();
		m_ToolComps.Clear();
		if (!r)
		{
			m_Merged.Clear();
			m_Bp.SetItems(side, top);
			return;
		}
		m_Merged = BuildMergedComponents(r);
		array<EntityAI> cachedItems = new array<EntityAI>();
		GetInventorySnapshot(cachedItems);

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

			DRL_UIBpItem d = new DRL_UIBpItem();
			d.m_Kind = DRL_BP_INGR;
			d.m_Tag = i;
			string nm = DRL_UIText.DisplayName(activeCls);
			d.m_Name = nm;
			d.m_Preview = activeCls;
			string amount = DRL_CraftInventoryHelper.FormatIngredientAmount(activeCls, c, have, need);
			int slash = amount.IndexOf("/");
			if (slash > 0)
			{
				d.m_Have = amount.Substring(0, slash);
				int restLen = amount.Length() - slash - 1;
				d.m_Need = "/ " + amount.Substring(slash + 1, restLen);
			}
			else
			{
				d.m_Have = amount;
				d.m_Need = "";
			}
			d.m_Ok = have >= need;
			d.m_Variants = hasVariations;
			d.m_Selected = m_ActiveVariationMerged != null && m_ActiveVariationMerged == mc;
			if (!d.m_Ok)
				d.m_GoRecipe = FindLocalRecipeFor(activeCls);
			string tip = nm + " · " + amount;
			if (c.MinHealthFrac < 3)
				tip = tip + " · " + DRL_UIText.Fmt("#STR_DRL_UI_BP_TIP_MINHP", DRL_UIText.MinHealth(c.MinHealthFrac));
			if (hasVariations)
				tip = tip + " · " + DRL_UIText.Tr("#STR_DRL_UI_BP_TIP_VARS");
			d.m_Tip = tip;
			side.Insert(d);
		}

		string activeResult = m_SelectedResultClass;
		if (r.CraftComponents)
		{
			for (int t = 0; t < r.CraftComponents.Count(); t++)
			{
				DRL_Craft_Component tc = r.CraftComponents[t];
				if (!tc || tc.AttachmentsEx <= 0)
					continue;
				if (!tc.IsApplicableForResult(activeResult))
					continue;
				string token = ComponentToken(tc);
				if (token == "")
					continue;
				EntityAI present = FindAttachment(tc);
				string prevCls = "";
				if (present)
					prevCls = present.GetType();
				else
					prevCls = DRL_ItemTypeHelper.GetSafePreviewClass(token);
				DRL_UIBpItem td = new DRL_UIBpItem();
				td.m_Tag = TAG_TOOL + m_ToolComps.Count();
				m_ToolComps.Insert(tc);
				td.m_Name = ToolName(token);
				td.m_Preview = prevCls;
				td.m_Ok = present != null;
				int wear = Math.Round(tc.ApplyDamageFrac * 100.0);
				if (tc.AttachmentsEx == 1)
				{
					td.m_Kind = DRL_BP_ATT;
					td.m_Glyph = "gear";
					if (present)
					{
						int lvl = present.GetHealthLevel();
						td.m_Health = DRL_UIC.HealthPct(lvl);
						td.m_Sub = DRL_UIText.Fmt("#STR_DRL_UI_BP_ATT", wear.ToString(), DRL_UIC.HealthName(lvl));
					}
					else if (!m_ContextWorkbench)
					{
						td.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_BP_ATT_FREE");
					}
					else
					{
						td.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_BP_ATT_NO");
					}
					top.Insert(td);
				}
				else
				{
					td.m_Kind = DRL_BP_TOOL;
					td.m_Glyph = "key";
					if (present)
						td.m_Sub = DRL_UIText.Fmt("#STR_DRL_UI_BP_TOOL", wear.ToString());
					else
						td.m_Sub = DRL_UIText.Tr("#STR_DRL_UI_BP_TOOL_NO");
					side.Insert(td);
				}
				td.m_Tip = td.m_Name + " · " + td.m_Sub;
			}
		}
		m_Bp.SetItems(side, top);

		int made = r.ResultCount;
		if (made <= 0)
			made = 1;
		made = made * Math.Max(1, r.CurrentCraftCount);
		if (made > 1)
			m_Bp.SetBadge("x" + made.ToString());
		else
			m_Bp.SetBadge("");
	}

	// Рецепт этой же станции, который делает недостающий предмет («Сделать»).
	protected int FindLocalRecipeFor(string cls)
	{
		DRL_Craft_Config cfg = Cfg();
		if (!cfg || cls == "")
			return -1;
		array<ref DRL_Craft_Recipe> all = cfg.GetAll();
		if (!all)
			return -1;
		foreach (DRL_Craft_Recipe r : all)
		{
			if (!r || r == m_SelectedRecipe || !RecipeAllowedByContext(r))
				continue;
			if (r.Result == cls || (r.ResultClasses && r.ResultClasses.Find(cls) > -1))
				return r.RecipeId;
		}
		return -1;
	}

	// Выделить ингредиент, чьи вариации открыты в панели.
	protected void RefreshIngrSelection()
	{
		if (!m_Bp || !m_SelectedRecipe)
			return;
		int i = 0;
		DRL_UIBpItem d = m_Bp.GetSideItem(i);
		while (d)
		{
			bool sel = false;
			if (m_ActiveVariationMerged && d.m_Kind == DRL_BP_INGR && d.m_Tag >= 0 && d.m_Tag < m_Merged.Count())
				sel = m_Merged[d.m_Tag] == m_ActiveVariationMerged;
			d.m_Selected = sel;
			i++;
			d = m_Bp.GetSideItem(i);
		}
		m_Bp.Relayout();
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

	// Действия чертежа: выноска (панель вариантов), «Сделать», «ещё N»,
	// вариант результата, доп. навесное.
	protected void OnBpAction(string what, int a, int b)
	{
		if (what == "item")
		{
			if (a >= 0 && a < TAG_TOOL && a < m_Merged.Count())
				ToggleVariationList(m_Merged[a]);
			return;
		}
		if (what == "go")
		{
			DRL_Craft_Config cfg = Cfg();
			if (!cfg)
				return;
			DRL_Craft_Recipe target = cfg.FindById(a);
			if (target)
				SelectRecipeDirectly(target);
			return;
		}
		if (what == "more")
		{
			OpenRequirements();
			return;
		}
		// Метка части сета на манекене: рецепт этой части.
		if (what == "part")
		{
			DRL_Craft_Config partCfg = Cfg();
			if (!partCfg)
				return;
			DRL_Craft_Recipe partR = partCfg.FindById(a);
			if (partR && partR != m_SelectedRecipe)
				SelectRecipeDirectly(partR);
			return;
		}
		if (what == "var")
		{
			OnVariantSelected(a);
			return;
		}
		if (what == "dop")
		{
			string act = "check";
			int arg = 0;
			if (b == 1)
				act = "minus";
			else if (b == 2)
				act = "plus";
			else if (b == 3)
				act = "var";
			if (b == 3 && a >= 0 && a < m_DopRecipes.Count())
			{
				DRL_Craft_Recipe dopR = m_DopRecipes[a];
				array<string> dvars = DopVariants(dopR);
				int cur = DopVariantIndex(dopR.RecipeId, dvars);
				if (dvars.Count() > 0)
					arg = (cur + 1) % dvars.Count();
			}
			HandleDopAction(a, act, arg);
		}
	}

	// --- доп. рецепты-навесное (= BuildAttachedRecipesList + UpdateAttachedSlotsAvailability)
	protected void BuildDopTiles(bool keepScroll)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		array<ref DRL_UIBpDopData> items = new array<ref DRL_UIBpDopData>();
		m_DopRecipes.Clear();
		DRL_Craft_Config cfg = Cfg();
		int picked = 0;
		if (r && r.HasAttachedRecipes() && cfg)
		{
			for (int k = 0; k < r.AttachedRecipeIds.Count(); k++)
			{
				int attachedId = r.AttachedRecipeIds[k];
				DRL_Craft_Recipe dopR = cfg.FindById(attachedId);
				if (!dopR)
					continue;
				bool isChecked = m_SelectedAttachedRecipeIds.Find(attachedId) > -1;
				bool learned = DRL_LearnState.HasLearned(attachedId);
				array<string> dvars = DopVariants(dopR);
				int dvi = DopVariantIndex(attachedId, dvars);
				bool slotOk = isChecked;
				if (!slotOk)
					slotOk = CanAttachToCurrentBuild(dopR.Result);

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
				DRL_UIBpDopData dd = new DRL_UIBpDopData();
				dd.m_Name = dopR.RecipeName;
				dd.m_Checked = isChecked;
				dd.m_Locked = !learned;
				dd.m_Dim = learned && !slotOk;
				if (isChecked)
				{
					dd.m_Qty = GetSelectedAttachedQuantity(attachedId);
					picked++;
				}
				dd.m_VarCount = dvars.Count();
				string tip = dopR.RecipeName + " · " + sub;
				if (dvars.Count() > 1 && dvi < dvars.Count())
					tip = tip + " · " + DRL_UIText.DisplayName(dvars[dvi]);
				dd.m_Tip = tip;
				m_DopRecipes.Insert(dopR);
				items.Insert(dd);
			}
		}
		string label = DRL_UIText.Fmt("#STR_DRL_UI_BP_DOP", picked.ToString(), items.Count().ToString());
		m_Bp.SetDop(items, label);
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
		if (wasOpen)
			RefreshIngrSelection();
	}

	// Закрыта крестиком.
	protected void OnPopoverClosed()
	{
		m_ActiveVariationMerged = null;
		m_VarInfos.Clear();
		SetCovered(false);
		RefreshIngrSelection();
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

	// Наведение на строку панели: подробности уже в самой строке.
	protected void OnPopHover(int index, DRL_UIRowList list)
	{
	}

	// --- требования (= DRL_CraftRequirementsController) ---------------------------------
	protected void OnReqPill(DRL_UIPill p)
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

	// = UpdateAdditionalTasksButtonState: пилюля «Требования выполнены / не выполнены».
	protected void UpdateReqButton()
	{
		if (!m_ReqPill || !m_SelectedRecipe)
			return;
		bool ok = AreAllRecipeRequirementsMet(m_SelectedRecipe);
		string text = "#STR_DRL_UI_REQ_NO";
		int fill = DRL_UIC.No(31);
		int border = DRL_UIC.No(115);
		int fg = ARGB(255, 255, 123, 123);
		if (ok)
		{
			text = "#STR_DRL_UI_REQ_OK";
			fill = DRL_UIC.Ok(31);
			border = DRL_UIC.Ok(102);
			fg = DRL_UIC.Ok();
		}
		float pw = m_ReqPill.Set(text, "shield");
		m_ReqPill.Move(m_SheetW - 20 - pw, 70);
		m_ReqPill.SetColors(fill, border, fg);
		if (m_Popover.GetKind() == "req")
		{
			array<ref DRL_UIRowData> rows = BuildRequirementRows(m_SelectedRecipe);
			m_Popover.SetRows(rows);
		}
	}

	// --- описание и навесное --------------------------------------------------------------
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
		m_RecipeDesc.SetText(DRL_UIText.Tr(body));
		FillSlots(r);
	}

	// Слоты навесного результата (= RenderAttachmentIconsForClass) под изделием;
	// бирюзовые - их займёт выбранное доп. навесное.
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
		if (!slots)
			slots = new array<string>();

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
		string label = DRL_UIText.Fmt("#STR_DRL_UI_BP_SLOTS", nFilled.ToString(), slots.Count().ToString());
		m_Bp.SetSlots(slots, filled, label);
	}

	// --- штамп листа: время, выход, состояние, наполнение --------------------------------
	protected void FillResult(DRL_Craft_Recipe r)
	{
		string chosen = ChosenResult();
		int cnt = r.ResultCount;
		if (cnt <= 0)
			cnt = 1;
		array<string> labels = new array<string>();
		array<string> values = new array<string>();
		array<int> colors = new array<int>();
		int txt = DRL_UIC.Txt();

		labels.Insert("#STR_DRL_UI_STP_TIME");
		values.Insert(DRL_UIText.Time(GetTotalCraftTimeSec(r)));
		colors.Insert(txt);

		if (m_StationKind == DRL_ST_SEW)
		{
			// «Раскрой»: какая это часть сета и сколько у неё расцветок.
			labels.Insert("#STR_DRL_UI_STP_PART");
			values.Insert(PartName(chosen, r));
			colors.Insert(txt);
			int nColors = 1;
			if (r.ResultClasses && r.ResultClasses.Count() > 1)
				nColors = r.ResultClasses.Count();
			labels.Insert("#STR_DRL_UI_STP_COLORS");
			values.Insert(nColors.ToString());
			colors.Insert(txt);
		}
		else
		{
			labels.Insert("#STR_DRL_UI_STP_OUT");
			values.Insert("x" + cnt.ToString());
			colors.Insert(txt);
		}

		labels.Insert("#STR_DRL_UI_STP_STATE");
		values.Insert(DRL_UIText.StateFromDamage(r.ResultDamageFrac));
		int stateCol = txt;
		if (r.ResultDamageFrac <= 0.0)
			stateCol = DRL_UIC.Ok();
		colors.Insert(stateCol);

		if (r.ResultQuantityFrac > 0.0)
		{
			labels.Insert("#STR_DRL_UI_STP_FILL");
			values.Insert(DRL_UIText.Pct(r.ResultQuantityFrac));
			colors.Insert(txt);
		}
		SetStamp(labels, values, colors);
		FillStats(chosen);
	}

	protected void SetStamp(array<string> labels, array<string> values, array<int> colors)
	{
		array<float> widths = new array<float>();
		float total = 0;
		int n = labels.Count();
		if (n > m_StampLabels.Count())
			n = m_StampLabels.Count();
		for (int i = 0; i < n; i++)
		{
			string lt = DRL_UIText.Tr(labels[i]);
			float lw = DRL_UIMeasure.TextW(m_Stamp, lt, 9, true);
			float vw = DRL_UIMeasure.TextW(m_Stamp, values[i], 14, true);
			float cw = lw;
			if (vw > cw)
				cw = vw;
			cw = cw + 22;
			widths.Insert(cw);
			total = total + cw;
		}
		INT_UIF_Draw.Place(m_Stamp, m_SheetW - 20 - total, 16, total, 46);
		INT_UIF_Draw.Resize(m_StampBg.GetRoot(), total, 46);
		float x = 0;
		for (int c = 0; c < m_StampLabels.Count(); c++)
		{
			bool on = c < n;
			m_StampLabels[c].Show(on);
			m_StampValues[c].Show(on);
			m_StampSeps[c].Show(on && c > 0);
			if (!on)
				continue;
			float wc = widths[c];
			INT_UIF_Draw.Place(m_StampLabels[c], x + 11, 7, wc - 14, 14);
			INT_UIF_Draw.Place(m_StampValues[c], x + 11, 20, wc - 14, 20);
			INT_UIF_Draw.Place(m_StampSeps[c], x, 0, 1, 46);
			m_StampLabels[c].SetText(DRL_UIText.Tr(labels[c]));
			m_StampValues[c].SetText(values[c]);
			m_StampValues[c].SetColor(colors[c]);
			x = x + wc;
		}
		// Название не залезает под штамп.
		float titleW = m_SheetW - 20 - total - 40;
		INT_UIF_Draw.Resize(m_RecipeTitle, titleW, 32);
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

		// Набор строк зависит от типа предмета: огнестрел, одежда и броня (защита),
		// холодное оружие, боеприпасы. Одежда проверяется до холодного оружия: у неё
		// тоже есть MeleeModes (удар предметом), и вместо защиты показывался урон.
		if (basePath.IndexOf("CfgWeapons ") == 0 && GetGame().ConfigIsExisting(basePath + " modes"))
		{
			FillStatsFirearm(basePath, hpText);
			return;
		}
		bool isClothing = GetGame().IsKindOf(cls, "Clothing_Base") || GetGame().ConfigIsExisting(basePath + " DamageSystem GlobalArmor");
		if (!isClothing && GetGame().ConfigIsExisting(basePath + " MeleeModes Default ammo"))
		{
			FillStatsMelee(basePath, hpText);
			return;
		}
		if (basePath.IndexOf("CfgMagazines ") == 0 && GetGame().ConfigIsExisting(basePath + " ammo"))
		{
			FillStatsAmmo(basePath, hpText);
			return;
		}

		// Защиту показываем только одежде и броне; обычным предметам панель не нужна.
		if (!isClothing)
		{
			m_StatsBox.Show(false);
			return;
		}

		SetStatRow(0, "#STR_DRL_UI_STAT_HP", hpText);
		SetStatRow(1, "#STR_DRL_UI_STAT_BULLET", ProtText(ReadArmor(basePath, "Projectile", "", "")));
		SetStatRow(2, "#STR_DRL_UI_STAT_MELEE", ProtText(ReadArmor(basePath, "Melee", "", "")));
		SetStatRow(3, "#STR_DRL_UI_STAT_INFECTED", ProtText(ReadArmor(basePath, "Infected", "Zombie", "MeleeZombie")));
		SetStatRow(4, "#STR_DRL_UI_STAT_GRENADE", ProtText(ReadArmor(basePath, "FragGrenade", "Explosion", "")));
	}

	protected void SetStatRow(int idx, string label, string value)
	{
		m_Bp.SetStat(idx, DRL_UIText.Tr(label), value);
	}

	protected void ClearStatRow(int idx)
	{
		m_Bp.SetStat(idx, "", "");
	}

	// Урон по здоровью из CfgAmmo (DamageApplied Health damage); -1, если не задан.
	protected float AmmoHealthDamage(string ammo)
	{
		float v;
		if (ammo != "" && DRL_CraftStatsHelper.ReadFloat("CfgAmmo " + ammo + " DamageApplied Health damage", v))
			return v;
		return -1.0;
	}

	protected string DmgText(float dmg)
	{
		if (dmg < 0.0)
			return "—";
		int d = Math.Round(dmg);
		return d.ToString();
	}

	// Холодное оружие и инструменты: урон лёгкого/тяжёлого удара, дальность, кровотечение.
	protected void FillStatsMelee(string basePath, string hpText)
	{
		string lightAmmo = GetGame().ConfigGetTextOut(basePath + " MeleeModes Default ammo");
		string heavyAmmo = GetGame().ConfigGetTextOut(basePath + " MeleeModes Heavy ammo");
		float range;
		string rangeText = "—";
		if (DRL_CraftStatsHelper.ReadFloat(basePath + " MeleeModes Default range", range))
			rangeText = range.ToString() + " " + DRL_UIText.Tr("#STR_DRL_UI_UNIT_M");

		float bleed;
		string bleedText = DRL_UIText.Tr("#STR_DRL_UI_NO");
		if (DRL_CraftStatsHelper.ReadFloat("CfgAmmo " + heavyAmmo + " DamageApplied bleedThreshold", bleed) && bleed > 0.0)
			bleedText = DRL_UIText.Tr("#STR_DRL_UI_YES");

		SetStatRow(0, "#STR_DRL_UI_STAT_HP", hpText);
		SetStatRow(1, "#STR_DRL_UI_STAT_DMG_LIGHT", DmgText(AmmoHealthDamage(lightAmmo)));
		SetStatRow(2, "#STR_DRL_UI_STAT_DMG_HEAVY", DmgText(AmmoHealthDamage(heavyAmmo)));
		SetStatRow(3, "#STR_DRL_UI_STAT_RANGE", rangeText);
		SetStatRow(4, "#STR_DRL_UI_STAT_BLEED", bleedText);
	}

	// Огнестрел и арбалеты: урон выстрела (с учётом скорости), скорострельность, режимы, калибр.
	protected void FillStatsFirearm(string basePath, string hpText)
	{
		TStringArray rounds = new TStringArray;
		GetGame().ConfigGetTextArray(basePath + " chamberableFrom", rounds);
		if (rounds.Count() == 0)
			GetGame().ConfigGetTextArray(basePath + " magazines", rounds);

		string roundCls = "";
		if (rounds.Count() > 0)
			roundCls = rounds[0];

		float mult = 1.0;
		float multCfg;
		if (DRL_CraftStatsHelper.ReadFloat(basePath + " initSpeedMultiplier", multCfg) && multCfg > 0.0)
			mult = multCfg;
		float dmg = RoundDamage(roundCls, mult);

		// скорострельность — по самому быстрому режиму; режимы — по именам классов режимов
		TStringArray modes = new TStringArray;
		GetGame().ConfigGetTextArray(basePath + " modes", modes);
		float bestReload = -1.0;
		bool isSingle = false;
		bool isBurst = false;
		bool isAuto = false;
		foreach (string mode : modes)
		{
			float rt;
			if (DRL_CraftStatsHelper.ReadFloat(basePath + " " + mode + " reloadTime", rt) && rt > 0.0)
			{
				if (bestReload < 0.0 || rt < bestReload)
					bestReload = rt;
			}
			string ml = mode;
			ml.ToLower();
			if (ml.Contains("burst"))
				isBurst = true;
			else if (ml.Contains("full") || (ml.Contains("auto") && !ml.Contains("semi")))
				isAuto = true;
			else
				isSingle = true;
		}
		string rofText = "—";
		if (bestReload > 0.0)
		{
			int rof = Math.Round(60.0 / bestReload);
			rofText = rof.ToString() + " " + DRL_UIText.Tr("#STR_DRL_UI_UNIT_RPM");
		}
		string modesText = "";
		if (isSingle)
			modesText = DRL_UIText.Tr("#STR_DRL_UI_MODE_SINGLE");
		if (isBurst)
		{
			if (modesText != "")
				modesText = modesText + ", ";
			modesText = modesText + DRL_UIText.Tr("#STR_DRL_UI_MODE_BURST");
		}
		if (isAuto)
		{
			if (modesText != "")
				modesText = modesText + ", ";
			modesText = modesText + DRL_UIText.Tr("#STR_DRL_UI_MODE_AUTO");
		}
		if (modesText == "")
			modesText = "—";

		string caliberText = "—";
		if (roundCls != "")
			caliberText = DRL_UIText.DisplayName(roundCls);

		SetStatRow(0, "#STR_DRL_UI_STAT_HP", hpText);
		SetStatRow(1, "#STR_DRL_UI_STAT_DMG_SHOT", DmgText(dmg));
		SetStatRow(2, "#STR_DRL_UI_STAT_ROF", rofText);
		SetStatRow(3, "#STR_DRL_UI_STAT_MODES", modesText);
		SetStatRow(4, "#STR_DRL_UI_STAT_CALIBER", caliberText);
	}

	// Боеприпасы и магазины: урон и ёмкость.
	protected void FillStatsAmmo(string basePath, string hpText)
	{
		string cls = basePath.Substring(13, basePath.Length() - 13); // после "CfgMagazines "
		int count = 0;
		string countText = "—";
		if (DRL_CraftStatsHelper.ReadInt(basePath + " count", count) && count > 0)
			countText = count.ToString();

		SetStatRow(0, "#STR_DRL_UI_STAT_HP", hpText);
		SetStatRow(1, "#STR_DRL_UI_STAT_DMG", DmgText(RoundDamage(cls, 1.0)));
		SetStatRow(2, "#STR_DRL_UI_STAT_CAPACITY", countText);
		ClearStatRow(3);
		ClearStatRow(4);
	}

	// Урон выстрела патроном/болтом из класса CfgMagazines с поправкой на скорость:
	// урон в DayZ пропорционален скорости, initSpeed (магазина или CfgAmmo) * множитель оружия / typicalSpeed.
	protected float RoundDamage(string magCls, float weaponMult)
	{
		if (magCls == "")
			return -1.0;
		string ammo = GetGame().ConfigGetTextOut("CfgMagazines " + magCls + " ammo");
		float dmg = AmmoHealthDamage(ammo);
		if (dmg < 0.0)
			return -1.0;
		float typical;
		if (!DRL_CraftStatsHelper.ReadFloat("CfgAmmo " + ammo + " typicalSpeed", typical) || typical <= 0.0)
			return dmg * weaponMult;
		float speed = typical;
		float magSpeed;
		float ammoSpeed;
		if (DRL_CraftStatsHelper.ReadFloat("CfgMagazines " + magCls + " initSpeed", magSpeed) && magSpeed > 0.0)
			speed = magSpeed;
		else if (DRL_CraftStatsHelper.ReadFloat("CfgAmmo " + ammo + " initSpeed", ammoSpeed) && ammoSpeed > 0.0)
			speed = ammoSpeed;
		return dmg * speed * weaponMult / typical;
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
		m_RecraftPill.Show(hasDrops);
		if (r && hasDrops)
		{
			m_RecraftName.SetText(r.RecipeName);
			m_RecraftTime.SetText(DRL_UIText.Time(r.GetRecraftTimeSec()));
		}
		else
		{
			m_RecraftName.SetText(DRL_UIText.Tr("#STR_DRL_UI_RC_NONE"));
			m_RecraftTime.SetText("");
		}
	}

	protected void UpdateRecraftButton()
	{
		if (!m_RecraftPill)
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
			SetRecraftButton(false, "#STR_DRL_UI_NO_ITEM_INV");
	}

	protected void SetRecraftButton(bool enabled, string text)
	{
		m_RecraftText = DRL_UIText.Tr(text);
		string icon = "";
		if (enabled)
			icon = "refresh";
		m_RecraftPill.Set(text, icon, m_RecraftBarW);
		int fill = DRL_UIC.Lk();
		int fg = ARGB(255, 43, 26, 0);
		m_RecraftPill.SetColors(fill, 0, fg);
		m_RecraftPill.SetEnabled(enabled);
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

	protected void OnRecraftPill(DRL_UIPill p)
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
			UpdateRecraftButton();
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
		m_RecraftTrack.Show(true);
		m_RecraftFill.Show(true);
		INT_UIF_Draw.Resize(m_RecraftFill, 2, 3);
		UpdateRecraftButton();
		UpdateStageTint();
	}

	protected void TickReCraft(float dt)
	{
		if (!m_ReCraftActive)
			return;
		m_ReCraftElapsed = m_ReCraftElapsed + dt;
		float frac = Math.Clamp(m_ReCraftElapsed / m_ReCraftDuration, 0, 1);
		float fw = m_RecraftBarW * frac;
		if (fw < 2)
			fw = 2;
		INT_UIF_Draw.Resize(m_RecraftFill, fw, 3);
		if (m_ReCraftElapsed < m_ReCraftDuration)
			return;
		m_ReCraftActive = false;
		m_RecraftTrack.Show(false);
		m_RecraftFill.Show(false);
		StopCraftSound();
		UpdateRecraftButton();
		if (m_SelectedRecipe)
			FillDetails();
		else
			RefreshStatuses();
		UpdateCraftButtonState();
	}

	// --- количество и время -----------------------------------------------------------
	protected void OnStepPlus(DRL_UIPill p)
	{
		if (!m_SelectedRecipe)
			return;
		SetCount(m_SelectedRecipe.CurrentCraftCount + 1);
	}

	protected void OnStepMinus(DRL_UIPill p)
	{
		if (!m_SelectedRecipe)
			return;
		SetCount(m_SelectedRecipe.CurrentCraftCount - 1);
	}

	protected void OnCountChip(DRL_UIPill p)
	{
		int idx = m_CountChips.Find(p);
		if (idx < 0 || idx >= m_CountChipValues.Count())
			return;
		SetCount(m_CountChipValues[idx]);
	}

	// Количество в пределах 1..сколько хватает ингредиентов.
	protected void SetCount(int n)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		int top = Math.Max(1, GetMaxCraftable(r));
		n = Math.Clamp(n, 1, top);
		if (n == r.CurrentCraftCount)
		{
			RefreshCountUI();
			return;
		}
		r.CurrentCraftCount = n;
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
		m_StepMinus.SetEnabled(n > 1);
		m_StepPlus.SetEnabled(n < top);

		// x1 x5 x10 «Макс N».
		m_CountChipValues.Clear();
		m_CountChipValues.Insert(1);
		m_CountChipValues.Insert(5);
		m_CountChipValues.Insert(10);
		m_CountChipValues.Insert(top);
		float x = 144;
		for (int i = 0; i < m_CountChips.Count(); i++)
		{
			DRL_UIPill chip = m_CountChips[i];
			int v = m_CountChipValues[i];
			string label = "x" + v.ToString();
			if (i == 3)
				label = DRL_UIText.Fmt("#STR_DRL_UI_MAX_N", v.ToString());
			float cw = chip.Set(label, "");
			chip.Move(x, 17);
			x = x + cw + 4;
			int fill = DRL_UIC.White(13);
			int border = 0;
			int fg = DRL_UIC.Muted();
			if (v == n)
			{
				fill = DRL_UIC.A(41);
				border = DRL_UIC.A(128);
				fg = DRL_UIC.A();
			}
			chip.SetColors(fill, border, fg);
			chip.SetEnabled(i == 3 || v <= top);
		}

		int total = r.ResultCount * n;
		if (total <= 0)
			total = n;
		int t = Math.Max(0, GetTotalCraftTimeSec(r) * n);
		string timeText = DRL_UIText.Time(t);
		m_TotalTime.SetText(timeText);
		float tw = DRL_UIMeasure.TextW(m_Bar, timeText, 16, true);
		INT_UIF_Draw.Move(m_TotalClock, m_TotalRight - tw - 20, 35);
		INT_UIF_Draw.Place(m_TotalCount, m_TotalRight - tw - 230, 30, 200, 24);
		m_TotalCount.SetText("x" + total.ToString());
	}

	// =================================================================================
	// Кнопка крафта (= UpdateCraftButtonState / TryStartCraft)
	// =================================================================================
	protected string VerbKey()
	{
		if (m_StationKind == DRL_ST_SEW)
			return "#STR_DRL_UI_VERB_SEW";
		if (m_StationKind == DRL_ST_LIFT)
			return "#STR_DRL_UI_VERB_LIFT";
		return "#STR_DRL_UI_VERB_CRAFT";
	}

	void UpdateCraftButtonState()
	{
		UpdateReqButton();
		UpdateStageTint();
		if (!m_CraftBtn)
			return;

		DRL_UIProc proc = GetProcForCurrentRecipe();
		if (proc)
		{
			if (proc.IsQueued())
			{
				SetCraftButton("#STR_DRL_IN_QUEUE_CANCEL", "", DRL_BTN_WAIT, "clock", true);
				m_CraftBtn.SetProgress(-1);
			}
			else
			{
				UpdateRunningButton(proc, true);
			}
			return;
		}
		m_CraftBtn.SetProgress(-1);
		m_RunRemain = -1;

		if (!m_SelectedRecipe)
		{
			SetCraftButton("#STR_No_Recipe", "", DRL_BTN_OFF, "error", false);
			return;
		}
		if (!DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId))
		{
			SetCraftButton("#STR_LearningLockTitel", "", DRL_BTN_OFF, "lock", false);
			return;
		}
		if (!HasRequiredAttachments(m_SelectedRecipe))
		{
			DRL_Craft_Component miss = FirstMissingAttachment(m_SelectedRecipe, false);
			string missName = "";
			if (miss)
				missName = ToolName(ComponentToken(miss));
			string attKey = "#STR_DRL_UI_BTN_SETATT";
			if (!m_ContextWorkbench)
				attKey = "#STR_DRL_UI_BTN_ONLYST";
			if (miss && miss.AttachmentsEx == 2)
				attKey = "#STR_DRL_UI_BTN_NEEDTOOL";
			SetCraftButton(attKey, missName, DRL_BTN_OFF, "warning", false);
			return;
		}
		if (!CanCraftRecipe(m_SelectedRecipe))
		{
			SetCraftButton("#STR_DRL_NO_INGREDIENTS", "", DRL_BTN_OFF, "close", false);
			return;
		}
		int maxSlots = MaxQueueSlots();
		int cur = m_Procs.Count();
		if (cur >= maxSlots)
		{
			string full = DRL_UIText.Tr("#STR_DRL_QUEUE_FULL") + " (" + cur.ToString() + "/" + maxSlots.ToString() + ")";
			SetCraftButton(full, "", DRL_BTN_OFF, "error", false);
			return;
		}
		if (cur > 0)
		{
			int next = cur + 1;
			string toQueue = DRL_UIText.Tr("#STR_DRL_TO_QUEUE") + " (" + next.ToString() + "/" + maxSlots.ToString() + ")";
			SetCraftButton(toQueue, "", DRL_BTN_QUEUE, "plus", true);
		}
		else
		{
			SetCraftButton(VerbKey(), "", DRL_BTN_GO, StationIcon(), true);
		}
	}

	// Идёт крафт выбранного рецепта: «Отменить», ниже процент и сколько осталось.
	protected void UpdateRunningButton(DRL_UIProc proc, bool force)
	{
		int remain = proc.RemainingSec();
		m_CraftBtn.SetProgress(proc.Percent() / 100.0);
		if (!force && remain == m_RunRemain)
			return;
		m_RunRemain = remain;
		int mm = remain / 60;
		int ss = remain - mm * 60;
		string sec = ss.ToString();
		if (ss < 10)
			sec = "0" + sec;
		string clockText = mm.ToString() + ":" + sec;
		int pct = proc.Percent();
		string sub = DRL_UIText.Fmt("#STR_DRL_UI_BTN_RUN", pct.ToString(), clockText);
		SetCraftButton("#STR_CANCEL", sub, DRL_BTN_CANCEL, "close", true);
	}

	protected void SetCraftButton(string text, string sub, int style, string icon, bool enabled)
	{
		m_CraftText = DRL_UIText.Tr(text);
		m_CraftBtn.SetState(style, text, sub, icon, enabled);
	}

	// Всё готово - круг изделия на чертеже зелёный.
	protected void UpdateStageTint()
	{
		if (!m_Bp)
			return;
		bool ok = false;
		if (m_SelectedRecipe && DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId))
			ok = CanCraftRecipe(m_SelectedRecipe);
		m_Bp.SetAllOk(ok);
	}

	protected void OnCraftBtn(DRL_UIBigBtn b)
	{
		DRL_UIProc proc = GetProcForCurrentRecipe();
		if (proc)
		{
			CancelProcess(proc.GetProcessId());
			return;
		}
		if (!CanCraftSelected())
		{
			UpdateCraftButtonState();
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
		if (!DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId) || !HasRequiredAttachments(m_SelectedRecipe))
		{
			UpdateCraftButtonState();
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
		// Тост о запуске не нужен: сервер сам присылает «Начался процесс изготовления».
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
		ScheduleLateRefresh();
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
		ScheduleLateRefresh();
	}

	// Сервер снимает блокировку предметов крафта (DRL_IsCraftLocked) и меняет
	// инвентарь чуть позже, чем приходит сообщение о процессе: без повторного
	// пересчёта обвесы и инструменты висели бы «нужен» до следующего клика.
	protected void ScheduleLateRefresh()
	{
		m_LateRefresh = 1.5;
	}

	protected void TickLateRefresh(float dt)
	{
		if (m_LateRefresh <= 0)
			return;
		m_LateRefresh = m_LateRefresh - dt;
		if (m_LateRefresh > 0)
			return;
		m_LateRefresh = 0;
		if (m_SelectedRecipe)
			FillDetails();
		else
			RefreshStatuses();
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
		if (!m_Queue || !m_QueueCount)
			return;
		int maxSlots = MaxQueueSlots();
		float qh = m_Queue.SetProcs(m_Procs, maxSlots);
		string cnt = m_Procs.Count().ToString();
		m_QueueCount.SetText(cnt);
		float cw = DRL_UIMeasure.TextW(m_Right, cnt, 21, true);
		INT_UIF_Draw.Move(m_QueueMax, LPAD + cw + 6, 37);
		m_QueueMax.SetText("/ " + maxSlots.ToString());
		LayoutRight(qh);
		if (m_Grid && m_Grid.Count() > 0)
			RefreshStatuses();
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
		if (cur && !cur.IsQueued() && m_CraftBtn)
			UpdateRunningButton(cur, false);
	}

	// Карточка очереди: выбрать рецепт процесса или отменить его.
	protected void OnQueueAction(int index, string action, DRL_UIQueueList list)
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

	// Команды деталей; "" - команда не наша.
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
		// Вариант результата (чип под изделием).
		if (cmd == "var")
		{
			m_Bp.TestClick("var", n, 0);
			return "result=" + m_SelectedResultClass + " " + TestState();
		}
		// Выноска чертежа: клик (панель вариантов ингредиента) и «Сделать».
		if (cmd == "row")
		{
			m_Bp.TestClick("card", n, 0);
			return TestState();
		}
		if (cmd == "go")
		{
			m_Bp.TestClick("go", n, 0);
			return TestState();
		}
		// «Раскрой»: метка части сета на манекене.
		if (cmd == "part")
		{
			m_Bp.TestClick("part", n, 0);
			return TestState();
		}
		// Доп. навесное: галочка, минус, плюс, вариант, переход к рецепту.
		if (cmd == "drow" || cmd == "chk")
		{
			m_Bp.TestClick("dop", n, 0);
			return TestState();
		}
		if (cmd == "qminus")
		{
			m_Bp.TestClick("dop", n, 1);
			return TestState();
		}
		if (cmd == "qplus")
		{
			m_Bp.TestClick("dop", n, 2);
			return TestState();
		}
		if (cmd == "dv")
		{
			m_Bp.TestClick("dop", n, 3);
			return TestState();
		}
		if (cmd == "info")
		{
			HandleDopAction(n, "info", 0);
			return TestState();
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
			OnReqPill(m_ReqPill);
			return TestState() + " poprows=" + m_Popover.GetList().Count().ToString();
		}
		if (cmd == "plus")
		{
			OnStepPlus(null);
			return TestState();
		}
		if (cmd == "minus")
		{
			OnStepMinus(null);
			return TestState();
		}
		if (cmd == "count")
		{
			SetCount(n);
			return TestState();
		}
		if (cmd == "craft")
		{
			if (m_CraftBtn.IsEnabled())
				OnCraftBtn(m_CraftBtn);
			return TestState();
		}
		if (cmd == "recraft")
		{
			if (m_RecraftPill.IsEnabled())
				OnRecraftPill(m_RecraftPill);
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
		s = s + " button='" + m_CraftText + "' cards=" + m_Bp.VisibleCards().ToString();
		s = s + " dop=" + m_DopRecipes.Count().ToString() + " ret=" + m_RetList.Count().ToString();
		s = s + " attached=" + m_SelectedAttachedRecipeIds.Count().ToString();
		s = s + " procs=" + m_Procs.Count().ToString() + " pop=" + m_Popover.GetKind();
		return s;
	}

	override void OnTestCommand(string command)
	{
		DRL_UITest.Apply(this, command);
	}

	// Мышь попадает в хит-зону, у которой свой обработчик (список рецептов,
	// категории, чертёж, очередь). dayz-mcp ui_click вызывает обработчик меню -
	// пробрасываем клик дальше.
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w)
		{
			string n = w.GetName();
			if (n.IndexOf("DRL_Card_") == 0 && m_Grid && m_Grid.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Cat_") == 0 && m_Rail && m_Rail.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Bp") == 0 && m_Bp && m_Bp.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Cut") == 0 && m_Bp && m_Bp.OnClick(w, x, y, button))
				return true;
			if (n.IndexOf("DRL_Q") == 0 && m_Queue && m_Queue.OnClick(w, x, y, button))
				return true;
		}
		return super.OnClick(w, x, y, button);
	}
}
