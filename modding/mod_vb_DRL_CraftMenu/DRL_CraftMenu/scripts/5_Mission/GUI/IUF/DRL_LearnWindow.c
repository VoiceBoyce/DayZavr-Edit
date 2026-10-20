// ============================================================================
// DRL_CraftMenu - окно изучения рецептов на INT_UIFramework
// (замена DRL_LearningMenuUI).
//
// Раскладка как у окна крафта (1640x920):
//   слева  — «Изученные» и категории со счётчиком изучаемых рецептов;
//   центр  — заголовок, поиск, сетка карточек (чип: изучен / можно изучить / недоступен);
//   справа — превью, описание, «Требуется изучить» или «Способы изучения»
//            + «Результат», кнопка «Изучить».
// Логика 1:1 со старым меню: те же проверки состояния, фильтры категорий,
// списание предметов на клиенте и RPC CTS_STUDY_RECIPE (Param2: recipeId, индекс способа).
// ============================================================================

class DRL_LearnWindow extends INT_UIF_Menu
{
	static const float SIDE_W = 250;
	static const float DETAILS_W = 540;
	static const float GAP = 16;
	static const float STAGE_H = 240;
	static const float BTN_H = 52;
	static const float ROW_H = 50;
	static const float ROW_GAP = 6;

	// Состояние изучения (= DRL_LearningMenuUI.GetRecipeLearningState).
	static const int LS_LOCKED = 0;
	static const int LS_AVAILABLE = 1;
	static const int LS_LEARNED = 2;

	// = CATEGORY_STUDIED_INDEX старого меню.
	static const int CAT_STUDIED = -2;

	// Теги строк списка деталей: способ изучения, требуемый рецепт, результат.
	static const int TAG_REQ = 1000;
	static const int TAG_RESULT = 2000;

	protected int m_CategoryKey = -1;
	protected DRL_Craft_Recipe m_SelectedRecipe;
	protected DRL_Learning_Recipe m_SelectedLearningRecipe;
	protected int m_SelectedVariationIndex = -1;
	protected bool m_StudyPending;
	protected string m_Query;
	protected ref array<DRL_Craft_Recipe> m_ReqRecipes;

	protected ref DRL_UISidebar m_Sidebar;
	protected ref DRL_UIRecipeGrid m_Grid;
	protected ref DRL_UIStage m_Stage;
	protected ref DRL_UIRowList m_List;
	protected INT_UIF_TextInput m_Search;
	protected TextWidget m_ListTitle;
	protected TextWidget m_ListSub;

	protected Widget m_DetailsEmpty;
	protected Widget m_DetailsBody;
	protected TextWidget m_RecipeTitle;
	protected ref DRL_UIChip m_ChipState;
	protected ref DRL_UIChip m_ChipInfo;
	protected float m_ChipY;
	protected MultilineTextWidget m_RecipeDesc;
	protected INT_UIF_Button m_StudyBtn;
	protected string m_StudyText;

	void DRL_LearnWindow()
	{
		m_Sidebar = new DRL_UISidebar();
		m_Grid = new DRL_UIRecipeGrid();
		m_Stage = new DRL_UIStage();
		m_List = new DRL_UIRowList();
		m_ReqRecipes = new array<DRL_Craft_Recipe>();
	}

	void ~DRL_LearnWindow()
	{
		UnhookBridge();
		ReleaseModels();
	}

	// =================================================================================
	// Окно
	// =================================================================================
	override string GetTitle()
	{
		return "#STR_DRL_UI_LEARN_TITLE";
	}

	override string GetSubtitle()
	{
		return LearnedSubtitle();
	}

	override string GetIcon()
	{
		return "star";
	}

	override void GetWindowSize(out float w, out float h)
	{
		w = 1640;
		h = 920;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override void OnBuild(Widget body)
	{
		float bw = GetBodyW();
		float bh = GetBodyH();
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int textColor = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT);

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
		m_ListTitle = INT_UIF_Draw.Text(mid, 2, 0, gw - 340, 24, "", 19, textColor);
		m_ListTitle.SetBold(true);
		m_ListSub = INT_UIF_Draw.Text(mid, 2, 24, gw - 340, 16, "", 11, faint);
		m_Search = INT_UIF_TextInput.Create(m_Bag, mid, gw - 320, 0, 320, 40, "#STR_SEARCH_RECIPE", 40, "search", "");
		m_Search.OnChanged().Insert(OnSearchChanged);
		m_Search.GetRoot().SetName("DRL_Search");
		m_Grid.Build(mid, 0, 52, gw, bh - 52, 4, 3, 12);
		m_Grid.OnSelected().Insert(OnGridSelected);

		// --- справа: детали -------------------------------------------------------------
		BuildDetails(body, bw - DETAILS_W, 0, DETAILS_W, bh);

		AddHint("#STR_DRL_UI_KEY_LMB", "#STR_DRL_UI_HINT_SELECT");
		AddHint("#STR_DRL_UI_KEY_WHEEL", "#STR_DRL_UI_HINT_SCROLL");
		AddHint("#STR_DRL_UI_KEY_DRAG", "#STR_DRL_UI_HINT_ROTATE");
	}

	protected void BuildDetails(Widget parent, float x, float y, float w, float h)
	{
		Widget details = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Details");
		RevealOnOpen(details);
		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int muted = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED);

		m_DetailsEmpty = INT_UIF_Draw.Frame(details, 0, 0, w, h, "DRL_DetailsEmpty");
		int panelFill = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL);
		int panelBorder = INT_UIF.Color(INT_UIF_ThemeKeys.PANEL_BORDER);
		INT_UIF_Surface.Box(m_DetailsEmpty, 0, 0, w, h, 12, panelFill, panelBorder);
		INT_UIF_Draw.Image(m_DetailsEmpty, w * 0.5 - 120, h * 0.5 - 150, 240, 240, "iuf_radial", ARGB(16, 255, 255, 255));
		INT_UIF_Draw.Icon(m_DetailsEmpty, w * 0.5 - 22, h * 0.5 - 56, 44, "star", faint);
		INT_UIF_Draw.Text(m_DetailsEmpty, 0, h * 0.5 + 4, w, 24, "#STR_DRL_UI_SELECT_HINT", 16, muted, INT_UIF_Align.CENTER);

		m_DetailsBody = INT_UIF_Draw.Frame(details, 0, 0, w, h, "DRL_DetailsBody");
		m_Stage.Build(m_DetailsBody, 0, 0, w, STAGE_H);
		m_Stage.SetAccent(INT_UIF.Color(INT_UIF_ThemeKeys.AMBER));

		float ty = STAGE_H + 12;
		m_RecipeTitle = INT_UIF_Draw.Text(m_DetailsBody, 2, ty, w - 4, 26, "", 20, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_RecipeTitle.SetBold(true);
		m_ChipY = ty + 34;
		m_ChipState = new DRL_UIChip();
		m_ChipState.Build(m_DetailsBody, 2, m_ChipY, 22, "DRL_LearnState");
		m_ChipInfo = new DRL_UIChip();
		m_ChipInfo.Build(m_DetailsBody, 140, m_ChipY, 22, "DRL_LearnInfo");

		float dy = m_ChipY + 32;
		m_RecipeDesc = INT_UIF_Draw.MText(m_DetailsBody, 2, dy, w - 4, 64, "", 13, muted);

		float ly = dy + 74;
		float btnY = h - BTN_H;
		m_List.Build(m_DetailsBody, 0, ly, w, btnY - 10 - ly, ROW_H, ROW_GAP, "DRL_Learn");
		m_List.OnAction().Insert(OnListAction);

		m_StudyBtn = INT_UIF_Button.Create(m_Bag, m_DetailsBody, 0, btnY, w, BTN_H, "#STR_VIEW_RECIPE", INT_UIF_Variant.PRIMARY, "star");
		m_StudyBtn.OnClick().Insert(OnStudyClick);
		DRL_UIKitUtil.Expose(m_StudyBtn, "DRL_StudyBtn");

		m_DetailsBody.Show(false);
	}

	override void OnReady()
	{
		BuildCategories(true);
	}

	override void OnShow()
	{
		super.OnShow();
		HookBridge();
	}

	override void OnHide()
	{
		super.OnHide();
		UnhookBridge();
		ReleaseModels();
	}

	protected void ReleaseModels()
	{
		if (m_Grid)
			m_Grid.ClearModels();
		if (m_Stage)
			m_Stage.Clear();
		if (m_List)
			m_List.ClearModels();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_Grid)
			m_Grid.Tick(timeslice);
		if (m_Stage)
			m_Stage.Tick(timeslice);
	}

	protected void HookBridge()
	{
		UnhookBridge();
		if (DRL_LearningBridge.s_OnStudyResponse)
			DRL_LearningBridge.s_OnStudyResponse.Insert(OnStudyResponse);
	}

	protected void UnhookBridge()
	{
		if (DRL_LearningBridge.s_OnStudyResponse)
			DRL_LearningBridge.s_OnStudyResponse.Remove(OnStudyResponse);
	}

	// =================================================================================
	// Состояния изучения (= DRL_LearningMenuUI)
	// =================================================================================
	bool IsRecipeStudyable(int recipeId)
	{
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg || !lcfg.LearningRecipes)
			return false;
		return lcfg.FindByAnyRecipeId(recipeId) != null;
	}

	protected DRL_Learning_Recipe FindLearning(int recipeId)
	{
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
			return null;
		DRL_Learning_Recipe lr = lcfg.FindByPrimaryId(recipeId);
		if (!lr)
			lr = lcfg.FindByAnyRecipeId(recipeId);
		return lr;
	}

	int GetRecipeLearningState(int recipeId)
	{
		if (DRL_LearnState.HasLearned(recipeId))
			return LS_LEARNED;
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
			return LS_AVAILABLE;
		DRL_Learning_Recipe lr = FindLearning(recipeId);
		if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
		{
			for (int i = 0; i < lr.RequiredRecipeIds.Count(); i++)
			{
				if (!DRL_LearnState.HasLearned(lr.RequiredRecipeIds[i]))
					return LS_LOCKED;
			}
		}
		// Своих требований в старом меню нет (DRL_CheckLearningCustomRequirements всегда true).
		return LS_AVAILABLE;
	}

	bool IsRecipeAvailableToStudyNow(int recipeId)
	{
		if (!IsRecipeStudyable(recipeId))
			return false;
		if (DRL_LearnState.HasLearned(recipeId))
			return false;
		return GetRecipeLearningState(recipeId) == LS_AVAILABLE;
	}

	// Показывать ли рецепт в списках (StudyMain показывает только основной).
	protected bool ShownInLearningList(int recipeId)
	{
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
			return true;
		DRL_Learning_Recipe lr = lcfg.FindByAnyRecipeId(recipeId);
		if (lr && !lr.ShouldShowInLearningList(recipeId))
			return false;
		return true;
	}

	// Рецепты категории для изучения (= GetCategoryStudyableCount / BuildRecipeList).
	protected void CollectCategoryRecipes(DRL_Craft_Category cat, array<DRL_Craft_Recipe> outList)
	{
		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cat || !cfg)
			return;
		if (cat.ShowAll == 1)
		{
			if (!cfg.CraftItems)
				return;
			for (int i = 0; i < cfg.CraftItems.Count(); i++)
			{
				DRL_Craft_Recipe r = cfg.CraftItems[i];
				if (!r || !IsRecipeStudyable(r.RecipeId))
					continue;
				if (cat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(r.RecipeId))
					continue;
				if (!ShownInLearningList(r.RecipeId))
					continue;
				outList.Insert(r);
			}
		}
		else if (cat.UseList == 1 && cat.List)
		{
			for (int j = 0; j < cat.List.Count(); j++)
			{
				int rId = cat.List[j];
				if (!IsRecipeStudyable(rId))
					continue;
				if (cat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(rId))
					continue;
				if (!ShownInLearningList(rId))
					continue;
				DRL_Craft_Recipe rec = cfg.FindById(rId);
				if (rec)
					outList.Insert(rec);
			}
		}
	}

	// Изученные (= категория «Изученные рецепты»).
	protected void CollectStudiedRecipes(array<DRL_Craft_Recipe> outList)
	{
		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.CraftItems)
			return;
		for (int i = 0; i < cfg.CraftItems.Count(); i++)
		{
			DRL_Craft_Recipe r = cfg.CraftItems[i];
			if (!r || !IsRecipeStudyable(r.RecipeId))
				continue;
			if (!ShownInLearningList(r.RecipeId))
				continue;
			if (GetRecipeLearningState(r.RecipeId) == LS_LEARNED)
				outList.Insert(r);
		}
	}

	protected int CountStudyable()
	{
		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.CraftItems)
			return 0;
		int n = 0;
		for (int i = 0; i < cfg.CraftItems.Count(); i++)
		{
			DRL_Craft_Recipe r = cfg.CraftItems[i];
			if (r && IsRecipeStudyable(r.RecipeId) && ShownInLearningList(r.RecipeId))
				n++;
		}
		return n;
	}

	protected string LearnedSubtitle()
	{
		array<DRL_Craft_Recipe> studied = new array<DRL_Craft_Recipe>();
		CollectStudiedRecipes(studied);
		string learned = studied.Count().ToString();
		string total = CountStudyable().ToString();
		return DRL_UIText.Fmt("#STR_DRL_UI_LEARN_SUB", learned, total);
	}

	// =================================================================================
	// Категории
	// =================================================================================
	protected void BuildCategories(bool selectDefault)
	{
		array<ref DRL_UICategoryEntry> entries = new array<ref DRL_UICategoryEntry>();
		array<DRL_Craft_Recipe> studied = new array<DRL_Craft_Recipe>();
		CollectStudiedRecipes(studied);
		entries.Insert(new DRL_UICategoryEntry(CAT_STUDIED, "#STR_DRL_UI_LEARNED_CAT", "", studied.Count(), "check"));

		int firstCategory = -1;
		DRL_Craft_Config cfg = g_Craft_Config;
		if (cfg && cfg.Categories)
		{
			for (int i = 0; i < cfg.Categories.Count(); i++)
			{
				DRL_Craft_Category cat = cfg.Categories[i];
				if (!cat)
					continue;
				array<DRL_Craft_Recipe> recs = new array<DRL_Craft_Recipe>();
				CollectCategoryRecipes(cat, recs);
				if (recs.Count() <= 0)
					continue;
				string icon = "";
				if (cat.Icon != "")
					icon = DRL_CraftUIHelper.ResolveImagePath(cat.Icon);
				string name = cat.Name;
				if (name == "")
					name = "#STR_DRL_ALL_RECIPES";
				entries.Insert(new DRL_UICategoryEntry(i, name, icon, recs.Count()));
				if (firstCategory < 0)
					firstCategory = entries.Count() - 1;
			}
		}

		// Выбор сохраняется при перестройке (после изучения), иначе — первая категория.
		int keep = -1;
		if (!selectDefault)
		{
			for (int e = 0; e < entries.Count(); e++)
			{
				if (entries[e].m_Key == m_CategoryKey)
				{
					keep = e;
					break;
				}
			}
		}
		if (keep < 0)
			keep = firstCategory;
		if (keep < 0)
			keep = 0;
		m_Sidebar.SetEntries(entries, keep);
		if (selectDefault)
		{
			DRL_UICategoryEntry def = m_Sidebar.GetEntry(keep);
			int key = CAT_STUDIED;
			if (def)
				key = def.m_Key;
			SelectCategory(key);
		}
	}

	protected void OnCategorySelected(int entryIndex, DRL_UISidebar sidebar)
	{
		DRL_UICategoryEntry e = sidebar.GetEntry(entryIndex);
		if (e)
			SelectCategory(e.m_Key);
	}

	void SelectCategory(int key)
	{
		m_CategoryKey = key;
		DeselectRecipe();
		string title = "#STR_DRL_UI_LEARNED_CAT";
		if (key != CAT_STUDIED && g_Craft_Config && g_Craft_Config.Categories && key >= 0 && key < g_Craft_Config.Categories.Count())
		{
			DRL_Craft_Category cat = g_Craft_Config.Categories[key];
			title = "#STR_DRL_ALL_RECIPES";
			if (cat && cat.Name != "")
				title = cat.Name;
		}
		m_ListTitle.SetText(title);
		RebuildRecipes(true);
	}

	// =================================================================================
	// Рецепты
	// =================================================================================
	protected void OnSearchChanged(string text, INT_UIF_TextInput input)
	{
		string q = DRL_CraftUIHelper.CleanSearchQuery(text, "#STR_SEARCH_RECIPE");
		q.ToLower();
		if (q == m_Query)
			return;
		m_Query = q;
		RebuildRecipes(true);
	}

	protected void RebuildRecipes(bool reveal)
	{
		array<DRL_Craft_Recipe> recs = new array<DRL_Craft_Recipe>();
		if (m_CategoryKey == CAT_STUDIED)
		{
			CollectStudiedRecipes(recs);
		}
		else if (g_Craft_Config && g_Craft_Config.Categories && m_CategoryKey >= 0 && m_CategoryKey < g_Craft_Config.Categories.Count())
		{
			CollectCategoryRecipes(g_Craft_Config.Categories[m_CategoryKey], recs);
		}

		array<ref DRL_UIRecipeEntry> entries = new array<ref DRL_UIRecipeEntry>();
		foreach (DRL_Craft_Recipe r : recs)
		{
			if (m_Query != "")
			{
				string rn = r.RecipeName;
				rn.ToLower();
				if (rn.IndexOf(m_Query) == -1)
					continue;
			}
			entries.Insert(MakeEntry(r));
		}
		m_Grid.SetItems(entries, m_SelectedRecipe, reveal);
		m_ListSub.SetText(DRL_UIText.Fmt("#STR_DRL_UI_RECIPES_COUNT", entries.Count().ToString()));
	}

	protected DRL_UIRecipeEntry MakeEntry(DRL_Craft_Recipe r)
	{
		DRL_UIRecipeEntry e = new DRL_UIRecipeEntry();
		e.m_Recipe = r;
		e.m_Name = r.RecipeName;
		e.m_PreviewClass = r.Result;
		if (r.PreviewClassname != "")
			e.m_PreviewClass = r.PreviewClassname;
		e.m_IconPath = "";
		e.m_ResultCount = 0;
		FillEntryState(e);
		return e;
	}

	// Чип и подпись карточки по состоянию изучения.
	protected void FillEntryState(DRL_UIRecipeEntry e)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int state = GetRecipeLearningState(e.m_Recipe.RecipeId);
		if (state == LS_LEARNED)
		{
			e.m_Status = DRL_UI_STATUS_CAN;
			e.m_ChipText = "#STR_DRL_UI_LS_LEARNED";
			e.m_ChipIcon = "check";
			e.m_ChipHue = tm.Color(INT_UIF_ThemeKeys.GREEN);
		}
		else if (state == LS_AVAILABLE)
		{
			e.m_Status = DRL_UI_STATUS_CANNOT;
			e.m_ChipText = "#STR_DRL_UI_LS_AVAILABLE";
			e.m_ChipIcon = "key";
			e.m_ChipHue = tm.Color(INT_UIF_ThemeKeys.AMBER);
		}
		else
		{
			e.m_Status = DRL_UI_STATUS_LOCKED;
			e.m_ChipText = "#STR_DRL_UI_LS_LOCKED";
			e.m_ChipIcon = "lock";
			e.m_ChipHue = tm.Color(INT_UIF_ThemeKeys.RED);
		}
		int methods = 0;
		DRL_Learning_Recipe lr = FindLearning(e.m_Recipe.RecipeId);
		if (lr && lr.StudyVariations)
			methods = lr.StudyVariations.Count();
		e.m_FootText = DRL_UIText.Fmt("#STR_DRL_UI_LEARN_METHODS", methods.ToString());
		e.m_FootIcon = "key";
	}

	protected void RefreshStatuses()
	{
		int n = m_Grid.Count();
		for (int i = 0; i < n; i++)
		{
			DRL_UIRecipeEntry e = m_Grid.GetItem(i);
			if (e && e.m_Recipe)
				FillEntryState(e);
		}
		m_Grid.RefreshValues();
	}

	protected void OnGridSelected(int index, DRL_UIRecipeGrid grid)
	{
		DRL_UIRecipeEntry e = grid.GetItem(index);
		if (e)
			SelectRecipe(e.m_Recipe);
		else
			DeselectRecipe();
	}

	// =================================================================================
	// Детали (= SelectRecipe / BuildLockStudyRecipes / BuildStudyVariations / BuildStudyRecipes)
	// =================================================================================
	void SelectRecipe(DRL_Craft_Recipe r)
	{
		if (!r)
			return;
		bool changed = r != m_SelectedRecipe;
		m_SelectedRecipe = r;
		m_SelectedLearningRecipe = FindLearning(r.RecipeId);
		m_SelectedVariationIndex = -1;
		m_StudyPending = false;
		m_Grid.Select(m_Grid.IndexOfRecipe(r), false);
		FillDetails(changed);
	}

	void DeselectRecipe()
	{
		m_SelectedRecipe = null;
		m_SelectedLearningRecipe = null;
		m_SelectedVariationIndex = -1;
		m_StudyPending = false;
		if (m_Stage)
			m_Stage.Clear();
		if (m_List)
			m_List.ClearModels();
		if (m_DetailsBody)
			m_DetailsBody.Show(false);
		if (m_DetailsEmpty)
			m_DetailsEmpty.Show(true);
	}

	// Состояние выбранного рецепта: пока ждём ответ сервера — уже «изучен» (как старое меню).
	protected int SelectedState()
	{
		if (!m_SelectedRecipe)
			return LS_LOCKED;
		if (m_StudyPending)
			return LS_LEARNED;
		return GetRecipeLearningState(m_SelectedRecipe.RecipeId);
	}

	protected void FillDetails(bool reveal)
	{
		DRL_Craft_Recipe r = m_SelectedRecipe;
		if (!r)
			return;
		m_DetailsEmpty.Show(false);
		if (reveal || !m_DetailsBody.IsVisible())
		{
			m_DetailsBody.Show(true);
			DRL_UIAnim.Reveal(m_DetailsBody, 0, 0, 0, 8, 0.24);
		}

		string previewCls = r.PreviewClassname;
		if (previewCls == "")
			previewCls = r.Result;
		m_Stage.SetClass(previewCls);
		m_RecipeTitle.SetText(r.RecipeName);
		string desc = r.Description;
		if (desc == "")
			desc = "#STR_DRL_Craft_Recipe_Detail";
		m_RecipeDesc.SetText(desc);

		int state = SelectedState();
		FillChips(state);
		BuildRows(state, !reveal);
		UpdateStudyButtonState();
	}

	protected void FillChips(int state)
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		float w = 0;
		if (state == LS_LEARNED)
		{
			w = m_ChipState.Set("#STR_DRL_UI_LS_LEARNED", "check", tm.Color(INT_UIF_ThemeKeys.GREEN));
			m_Stage.SetAccent(tm.Color(INT_UIF_ThemeKeys.GREEN));
		}
		else if (state == LS_AVAILABLE)
		{
			w = m_ChipState.Set("#STR_DRL_UI_LS_AVAILABLE", "key", tm.Color(INT_UIF_ThemeKeys.AMBER));
			m_Stage.SetAccent(tm.Color(INT_UIF_ThemeKeys.AMBER));
		}
		else
		{
			w = m_ChipState.Set("#STR_DRL_UI_LS_LOCKED", "lock", tm.Color(INT_UIF_ThemeKeys.RED));
			m_Stage.SetAccent(tm.Color(INT_UIF_ThemeKeys.RED));
		}
		// Второй чип: сколько рецептов открывает изучение (StudyMain учит группу).
		int count = 1;
		if (m_SelectedLearningRecipe && m_SelectedLearningRecipe.IsStudyMain() && m_SelectedLearningRecipe.RecipeIds)
			count = m_SelectedLearningRecipe.RecipeIds.Count();
		INT_UIF_Draw.Move(m_ChipInfo.m_Root, 2 + w + 8, m_ChipY);
		string infoText = DRL_UIText.Fmt("#STR_DRL_UI_LEARN_GIVES", count.ToString());
		m_ChipInfo.Set(infoText, "list", tm.Color(INT_UIF_ThemeKeys.TEAL));
	}

	protected void BuildRows(int state, bool keepScroll)
	{
		array<ref DRL_UIRowData> rows = new array<ref DRL_UIRowData>();
		m_ReqRecipes.Clear();
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int green = tm.Color(INT_UIF_ThemeKeys.GREEN);
		int amber = tm.Color(INT_UIF_ThemeKeys.AMBER);
		int red = tm.Color(INT_UIF_ThemeKeys.RED);
		DRL_Craft_Config cfg = g_Craft_Config;
		DRL_Learning_Recipe lr = m_SelectedLearningRecipe;

		if (state == LS_LOCKED)
		{
			// = BuildLockStudyRecipes: какие рецепты нужно изучить раньше.
			if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0 && cfg)
			{
				rows.Insert(DRL_UIRowData.Header("#STR_ToStudyRecipe"));
				for (int i = 0; i < lr.RequiredRecipeIds.Count(); i++)
				{
					int reqId = lr.RequiredRecipeIds[i];
					DRL_Craft_Recipe target = cfg.FindById(reqId);
					if (!target)
						continue;
					int st = GetRecipeLearningState(reqId);
					DRL_UIRowData rq = new DRL_UIRowData();
					rq.m_Title = target.RecipeName;
					rq.m_PreviewClass = PreviewOf(target);
					if (st == LS_LEARNED)
					{
						rq.m_Right = DRL_UIText.Tr("#STR_DRL_UI_LS_LEARNED");
						rq.m_RightColor = green;
					}
					else if (st == LS_AVAILABLE)
					{
						rq.m_Right = DRL_UIText.Tr("#STR_DRL_UI_LS_AVAILABLE");
						rq.m_RightColor = amber;
					}
					else
					{
						rq.m_Right = DRL_UIText.Tr("#STR_DRL_UI_LS_LOCKED");
						rq.m_RightColor = red;
					}
					rq.m_Accent = rq.m_RightColor;
					rq.m_Info = true;
					rq.m_Tag = TAG_REQ + m_ReqRecipes.Count();
					m_ReqRecipes.Insert(target);
					rows.Insert(rq);
				}
			}
			m_List.SetItems(rows, keepScroll);
			return;
		}

		// = BuildStudyVariations: способы изучения (выбор одного).
		bool learned = state == LS_LEARNED;
		rows.Insert(DRL_UIRowData.Header("#STR_Methods_of_study"));
		if (lr && lr.StudyVariations && lr.StudyVariations.Count() > 0)
		{
			for (int v = 0; v < lr.StudyVariations.Count(); v++)
			{
				DRL_Study_Variation sv = lr.StudyVariations[v];
				if (!sv)
					continue;
				int have = GetPlayerInventoryCount(sv.RequiredItem);
				DRL_UIRowData vd = new DRL_UIRowData();
				if (sv.DisplayName != "")
					vd.m_Title = sv.DisplayName;
				else
					vd.m_Title = GetItemDisplayName(sv.RequiredItem) + " x" + sv.Amount.ToString();
				string avail = DRL_UIText.Tr("#STR_DRL_AVAILABLE");
				avail = avail.Trim();
				vd.m_Right = avail + " " + have.ToString() + "/" + sv.Amount.ToString();
				if (have >= sv.Amount)
					vd.m_RightColor = green;
				else
					vd.m_RightColor = red;
				vd.m_PreviewClass = sv.RequiredItem;
				vd.m_Check = 0;
				if (v == m_SelectedVariationIndex)
				{
					vd.m_Check = 1;
					vd.m_Selected = true;
				}
				// После изучения способы не выбираются (как Enable(false) в старом меню).
				vd.m_Clickable = !learned;
				vd.m_Tag = v;
				rows.Insert(vd);
			}
		}
		else
		{
			rows.Insert(DRL_UIRowData.Note("#STR_DRL_UI_NO_METHODS"));
		}

		// = BuildStudyRecipes: что откроется после изучения.
		if (cfg && m_SelectedRecipe)
		{
			array<int> idsToShow = new array<int>();
			if (lr && lr.IsStudyMain() && lr.RecipeIds)
			{
				foreach (int rid : lr.RecipeIds)
				{
					idsToShow.Insert(rid);
				}
			}
			else
			{
				idsToShow.Insert(m_SelectedRecipe.RecipeId);
			}
			rows.Insert(DRL_UIRowData.Header("#STR_StudyRecipe"));
			for (int j = 0; j < idsToShow.Count(); j++)
			{
				DRL_Craft_Recipe res = cfg.FindById(idsToShow[j]);
				if (!res)
					continue;
				DRL_UIRowData rd = new DRL_UIRowData();
				rd.m_Title = res.RecipeName;
				rd.m_PreviewClass = PreviewOf(res);
				rd.m_Clickable = false;
				rd.m_Tag = TAG_RESULT + j;
				rows.Insert(rd);
			}
		}
		m_List.SetItems(rows, keepScroll);
	}

	protected string PreviewOf(DRL_Craft_Recipe r)
	{
		string cls = r.PreviewClassname;
		if (cls == "")
			cls = r.Result;
		return cls;
	}

	protected void OnListAction(int index, string action, int arg, DRL_UIRowList list)
	{
		DRL_UIRowData d = list.GetItem(index);
		if (!d || d.m_Tag < 0)
			return;
		if (d.m_Tag >= TAG_RESULT)
			return;
		if (d.m_Tag >= TAG_REQ)
		{
			int k = d.m_Tag - TAG_REQ;
			if (k < m_ReqRecipes.Count() && m_ReqRecipes[k])
				SelectRecipe(m_ReqRecipes[k]);
			return;
		}
		if ((action == "row" || action == "check") && SelectedState() != LS_LEARNED)
			SelectVariation(d.m_Tag);
	}

	// = SelectVariation
	void SelectVariation(int index)
	{
		m_SelectedVariationIndex = index;
		BuildRows(SelectedState(), true);
		UpdateStudyButtonState();
	}

	// =================================================================================
	// Кнопка «Изучить» (= UpdateStudyButtonState / TryStudySelected)
	// =================================================================================
	// Неактивная кнопка — не PRIMARY: полупрозрачность почти не гасит яркий
	// янтарный, и «Изучить» выглядело бы доступным.
	protected void SetStudyButton(bool enabled, string text, int variant, string icon)
	{
		m_StudyText = DRL_UIText.Tr(text);
		m_StudyBtn.SetEnabled(enabled);
		m_StudyBtn.SetText(text);
		m_StudyBtn.SetIcon(icon);
		if (m_StudyBtn.GetVariant() != variant)
			m_StudyBtn.SetVariant(variant);
	}

	void UpdateStudyButtonState()
	{
		if (!m_StudyBtn)
			return;
		if (!m_SelectedRecipe)
		{
			SetStudyButton(false, "#STR_VIEW_RECIPE", INT_UIF_Variant.SECONDARY, "star");
			return;
		}
		int state = SelectedState();
		if (state == LS_LEARNED)
		{
			SetStudyButton(false, "#STR_DRL_LEARNED", INT_UIF_Variant.SUCCESS, "check");
			return;
		}
		// Старое меню при недоступных требованиях прячет панель изучения с кнопкой.
		if (state == LS_LOCKED)
		{
			SetStudyButton(false, "#STR_DRL_NOT_AVAILABLE", INT_UIF_Variant.DANGER, "lock");
			return;
		}
		DRL_Learning_Recipe lr = m_SelectedLearningRecipe;
		if (m_SelectedVariationIndex < 0 || !lr || !lr.StudyVariations || m_SelectedVariationIndex >= lr.StudyVariations.Count())
		{
			SetStudyButton(false, "#STR_VIEW_RECIPE", INT_UIF_Variant.SECONDARY, "star");
			return;
		}
		DRL_Study_Variation v = lr.StudyVariations[m_SelectedVariationIndex];
		if (!v)
		{
			SetStudyButton(false, "#STR_VIEW_RECIPE", INT_UIF_Variant.SECONDARY, "star");
			return;
		}
		int have = GetPlayerInventoryCount(v.RequiredItem);
		if (have >= v.Amount)
			SetStudyButton(true, "#STR_VIEW_RECIPE", INT_UIF_Variant.PRIMARY, "star");
		else
			SetStudyButton(false, "#STR_VIEW_RECIPE", INT_UIF_Variant.SECONDARY, "star");
	}

	protected void OnStudyClick(INT_UIF_Button b)
	{
		TryStudySelected();
	}

	void TryStudySelected()
	{
		if (!m_SelectedRecipe || m_SelectedVariationIndex < 0 || m_StudyPending)
			return;
		if (DRL_LearnState.HasLearned(m_SelectedRecipe.RecipeId))
			return;

		DRL_Study_Variation studiedVar = null;
		DRL_Learning_Recipe lr = m_SelectedLearningRecipe;
		if (lr && lr.StudyVariations && m_SelectedVariationIndex < lr.StudyVariations.Count())
			studiedVar = lr.StudyVariations[m_SelectedVariationIndex];
		// Как в старом меню: предметы списываются на клиенте сразу, сервер решает сам.
		if (studiedVar && studiedVar.Delete == 1)
			DRL_ClientDeductInventoryItems(studiedVar.RequiredItem, studiedVar.Amount);

		m_StudyPending = true;
		FillDetails(false);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_STUDY_RECIPE, new Param2<int, int>(m_SelectedRecipe.RecipeId, m_SelectedVariationIndex), true, null);
	}

	void DRL_ClientDeductInventoryItems(string clsName, int count)
	{
		if (clsName == "" || count <= 0)
			return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetInventory())
			return;
		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		string targetClsLower = clsName;
		targetClsLower.ToLower();
		int leftToConsume = count;
		for (int i = items.Count() - 1; i >= 0; i--)
		{
			if (leftToConsume <= 0)
				break;
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib || ib.IsRuined())
				continue;
			string t = ib.GetType();
			t.ToLower();
			if (t != targetClsLower && !ib.IsKindOf(clsName))
				continue;
			if (ib.IsKindOf("Ammunition_Base"))
			{
				Magazine mgzAmmo = Magazine.Cast(ib);
				if (mgzAmmo)
				{
					int ammo = mgzAmmo.GetAmmoCount();
					if (ammo <= leftToConsume)
					{
						leftToConsume -= ammo;
						mgzAmmo.LocalSetAmmoCount(0);
					}
					else
					{
						mgzAmmo.LocalSetAmmoCount(ammo - leftToConsume);
						leftToConsume = 0;
					}
				}
			}
			else if (ib.HasQuantity() && !ib.IsLiquidContainer())
			{
				int q = ib.GetQuantity();
				if (q <= 0 && ib.GetQuantityMax() <= 0)
					q = 1;
				if (q <= leftToConsume)
				{
					leftToConsume -= q;
					ib.SetQuantity(0);
				}
				else
				{
					ib.SetQuantity(q - leftToConsume);
					leftToConsume = 0;
				}
			}
		}
	}

	// = OnStudyResponse
	void OnStudyResponse(bool success, int recipeId, array<int> learnedIds)
	{
		m_StudyPending = false;
		if (success)
		{
			DRL_LearningBridge.SetClientLearnedRecipes(learnedIds);
			string soundName = "DRLCraft_SoundSet";
			if (g_Craft_Config && g_Craft_Config.Settings && g_Craft_Config.Settings.LearningSoundSet != "")
			{
				soundName = g_Craft_Config.Settings.LearningSoundSet;
			}
			else
			{
				DRL_Learning_Config lcfg = GetLearning_Config();
				if (lcfg && lcfg.LearningSoundSet != "")
					soundName = lcfg.LearningSoundSet;
			}
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player)
			{
				EffectSound s = SEffectManager.PlaySoundOnObject(soundName, player);
				if (s)
					s.SetSoundAutodestroy(true);
			}
			SetSubtitle(LearnedSubtitle());
			BuildCategories(false);
			RebuildRecipes(false);
		}
		if (m_SelectedRecipe)
		{
			m_Grid.Select(m_Grid.IndexOfRecipe(m_SelectedRecipe), false);
			FillDetails(false);
		}
		else
		{
			UpdateStudyButtonState();
		}
	}

	// =================================================================================
	// Инвентарь (= GetPlayerInventoryCount / GetItemDisplayName)
	// =================================================================================
	int GetPlayerInventoryCount(string clsName)
	{
		if (clsName == "")
			return 0;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return 0;
		array<EntityAI> items = new array<EntityAI>();
		if (player.GetInventory())
			player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		string targetLower = clsName;
		targetLower.ToLower();
		int count = 0;
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib || ib.IsRuined())
				continue;
			string t = ib.GetType();
			t.ToLower();
			if (t != targetLower && !ib.IsKindOf(clsName))
				continue;
			if (ib.IsKindOf("Ammunition_Base"))
			{
				Magazine mgzAmmo = Magazine.Cast(ib);
				if (mgzAmmo)
					count += mgzAmmo.GetAmmoCount();
				else
					count += ib.GetQuantity();
			}
			else if (ib.HasQuantity() && !ib.IsLiquidContainer())
			{
				int q = ib.GetQuantity();
				if (q <= 0 && ib.GetQuantityMax() <= 0)
					q = 1;
				count += q;
			}
			else
			{
				count += 1;
			}
		}
		return count;
	}

	string GetItemDisplayName(string clsName)
	{
		if (clsName == "")
			return "";
		string name = "";
		if (DRL_CraftUIHelper.GetDisplayNameForClass(clsName, name) && name != "")
			return Widget.TranslateString(name);
		return clsName;
	}

	// =================================================================================
	// Тестовый хук (DRL_UITest)
	// =================================================================================
	string TestCommand(string cmd, string arg)
	{
		int n = arg.ToInt();
		if (cmd == "cat")
		{
			m_Sidebar.Select(n, true);
			return "categories=" + m_Sidebar.Count().ToString() + " recipes=" + m_Grid.Count().ToString();
		}
		if (cmd == "recipe")
		{
			m_Grid.Select(n, true);
			return TestState();
		}
		if (cmd == "search")
		{
			if (m_Search)
				m_Search.SetText(arg);
			OnSearchChanged(arg, m_Search);
			return "recipes=" + m_Grid.Count().ToString();
		}
		if (cmd == "hover")
		{
			m_Grid.PinHover(n);
			return "hover";
		}
		if (cmd == "var")
		{
			SelectVariation(n);
			return TestState();
		}
		if (cmd == "row")
		{
			m_List.TestAction(n, "row", 0);
			return TestState();
		}
		if (cmd == "study")
		{
			if (m_StudyBtn.IsEnabled())
				TryStudySelected();
			return TestState();
		}
		if (cmd == "state")
			return TestState();
		return "";
	}

	string TestState()
	{
		string name = "-";
		int state = -1;
		if (m_SelectedRecipe)
		{
			name = m_SelectedRecipe.RecipeName;
			state = SelectedState();
		}
		string s = "recipe='" + name + "' state=" + state.ToString() + " var=" + m_SelectedVariationIndex.ToString();
		s = s + " button='" + m_StudyText + "' rows=" + m_List.Count().ToString() + " pending=" + m_StudyPending.ToString();
		return s;
	}

	override void OnTestCommand(string command)
	{
		DRL_UITest.Apply(this, command);
	}
}
