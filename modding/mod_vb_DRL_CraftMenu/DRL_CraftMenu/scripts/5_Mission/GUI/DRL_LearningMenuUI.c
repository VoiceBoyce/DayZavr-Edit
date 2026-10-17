class DRL_LearningMenuUI : UIScriptedMenu
{
	protected Widget                   m_Root;
	protected ButtonWidget             m_BtnClose;

	protected Widget                   m_LeftPanelWidget;
	protected WrapSpacerWidget         m_SpacerAllLearningCategoryList;

	protected Widget                   m_RightPanelWidget;
	protected Widget                   m_LearningListPanelWidget;
	protected EditBoxWidget            m_SearchEditBox;
	protected WrapSpacerWidget         m_LearningCrafttListWrap;

	protected Widget                   m_LearningPanelWidget;
	protected ImageWidget              m_LearningFonImage;
	protected ImageWidget              m_LearningLockIcon;
	protected RichTextWidget           m_LearningRecipeTitel;
	protected ItemPreviewWidget        m_LearningPreview;
	protected RichTextWidget           m_LearningNameTite;
	protected MultilineTextWidget      m_LearningDescriptionMultiline;
	protected WrapSpacerWidget         m_StudyRecipeListWrap;
	protected WrapSpacerWidget         m_StudyVariationListWrap;
	protected ButtonWidget             m_StudyButton;
	protected TextWidget               m_StudyButtonText;
	protected Widget                   m_StudyButtonFon;
	protected int                      m_StudyButtonFonDefaultColor = 0;

	protected Widget                   m_StudyPanel;
	protected Widget                   m_LockStudyPanel;
	protected WrapSpacerWidget         m_LockStudyRecipeListWrap;
	protected ref array<Widget>        m_LockStudyRecipeWidgets = new array<Widget>();

	const int                          CATEGORY_STUDIED_INDEX = -2;

	protected int                      m_CurrentCategoryIndex = -1;
	protected ref DRL_Craft_Recipe     m_SelectedRecipe;
	protected ref DRL_Learning_Recipe  m_SelectedLearningRecipe;
	protected int                      m_SelectedVariationIndex = -1;

	protected ButtonWidget             m_StudiedCategoryButton;
	protected Widget                   m_StudiedCategoryColorWidget;
	protected RichTextWidget           m_AmountStudiedCategory;
	protected Widget                   m_StudiedCategoryBPanel;

	protected ref map<string, EntityAI> m_PreviewEntities = new map<string, EntityAI>();
	protected ref array<EntityAI>       m_PreviewTrash = new array<EntityAI>();
	protected ref array<Widget>         m_CategoryWidgets = new array<Widget>();
	protected ref array<int>            m_VisibleCategoryIndices = new array<int>();
	protected ref array<Widget>         m_RecipeWidgets = new array<Widget>();
	protected ref array<Widget>         m_VariationWidgets = new array<Widget>();
	protected ref array<Widget>         m_StudyRecipeWidgets = new array<Widget>();
	protected int                       m_BPanelDefaultColor = 0;
	protected int                       m_LearningCraftPanelDefaultColor = 0;
	protected int                       m_LearningCraftBorderDefaultColor = 0;

	protected ref Timer                 m_LearningQueueTimer;
	protected ref array<DRL_Craft_Recipe> m_LearningQueueRecipes;
	protected int                       m_LearningQueueIndex = 0;
	static const int                    LEARNING_RECIPES_PER_FRAME = 2;
	static const float                  LEARNING_QUEUE_TICK_RATE = 0.016666;

	const string MAIN_LAYOUT        = "DRL_CraftMenu/UI/layout/LearningMain.layout";
	const string CATEGORY_TEMPLATE  = "DRL_CraftMenu/UI/layout/CategorySelection.layout";
	const string RECIPE_TEMPLATE    = "DRL_CraftMenu/UI/layout/LearningCraft.layout";
	const string VARIATION_TEMPLATE = "DRL_CraftMenu/UI/layout/StudyVariatio.layout";
	const string STUDY_RECIPE_TEMPLATE = "DRL_CraftMenu/UI/layout/StudyRecipe.layout";
	const string REQ_HEADER_TEMPLATE   = "DRL_CraftMenu/UI/layout/RequirementHeader.layout";
	const string REQ_ITEM_TEMPLATE     = "DRL_CraftMenu/UI/layout/RequirementItem.layout";
	const string REQ_INFO_TEMPLATE     = "DRL_CraftMenu/UI/layout/RequirementInfo.layout";

	const string SEARCH_DEFAULT_STR = "Search recipes";

	void ~DRL_LearningMenuUI()
	{
		StopRecipeQueue();
		if (DRL_LearningBridge.s_OnStudyResponse)
		{
			DRL_LearningBridge.s_OnStudyResponse.Remove(OnStudyResponse);
		}
		CleanupPreviews();
	}

	override Widget Init()
	{
		if (DRL_LearningBridge.s_OnStudyResponse)
		{
			DRL_LearningBridge.s_OnStudyResponse.Remove(OnStudyResponse);
			DRL_LearningBridge.s_OnStudyResponse.Insert(OnStudyResponse);
		}

		m_Root = GetGame().GetWorkspace().CreateWidgets(MAIN_LAYOUT);
		if (!m_Root)
		{
			return null;
		}

		m_BtnClose = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonOut"));

		m_StudiedCategoryButton = ButtonWidget.Cast(m_Root.FindAnyWidget("StudiedCategoryButton"));
		if (m_StudiedCategoryButton)
		{
			m_StudiedCategoryColorWidget = m_StudiedCategoryButton.FindAnyWidget("StudiedCategoryColorWidget");
			m_AmountStudiedCategory = RichTextWidget.Cast(m_StudiedCategoryButton.FindAnyWidget("AmountStudiedCategory"));
			m_StudiedCategoryBPanel = m_StudiedCategoryButton.FindAnyWidget("BPanelWidget");
		}

		m_LeftPanelWidget = m_Root.FindAnyWidget("LeftPanelWidget");
		m_SpacerAllLearningCategoryList = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("SpacerAllLearningCategoryList"));

		m_RightPanelWidget = m_Root.FindAnyWidget("RightPanelWidget");
		if (m_RightPanelWidget)
		{
			m_RightPanelWidget.Show(false);
		}

		m_LearningListPanelWidget = m_Root.FindAnyWidget("LearningListPanelWidget");
		m_SearchEditBox = EditBoxWidget.Cast(m_Root.FindAnyWidget("SearchEditBox"));
		if (m_SearchEditBox)
		{
			m_SearchEditBox.SetText(SEARCH_DEFAULT_STR);
		}
		m_LearningCrafttListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("LearningCrafttListWrap"));

		m_LearningPanelWidget = m_Root.FindAnyWidget("LearningPanelWidget");
		if (m_LearningPanelWidget)
		{
			m_LearningPanelWidget.Show(false);
		}

		m_LearningLockIcon = ImageWidget.Cast(m_Root.FindAnyWidget("LearningLockIcon"));
		m_LearningRecipeTitel = RichTextWidget.Cast(m_Root.FindAnyWidget("LearningRecipeTitel"));
		m_LearningPreview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("LearningPreview"));
		m_LearningNameTite = RichTextWidget.Cast(m_Root.FindAnyWidget("LearningNameTite"));
		m_LearningDescriptionMultiline = MultilineTextWidget.Cast(m_Root.FindAnyWidget("LearningDescriptionMultiline"));
		m_StudyPanel = m_Root.FindAnyWidget("StudyPanel");
		if (m_StudyPanel)
		{
			m_StudyPanel.Show(false);
		}

		m_LockStudyPanel = m_Root.FindAnyWidget("LockStudyPanel");
		if (m_LockStudyPanel)
		{
			m_LockStudyPanel.Show(false);
		}

		m_LockStudyRecipeListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("LockStudyRecipeListWrap"));
		if (m_LockStudyRecipeListWrap)
		{
			ClearChildren(m_LockStudyRecipeListWrap);
		}

		m_StudyRecipeListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("StudyRecipeListWrap"));
		if (m_StudyRecipeListWrap)
		{
			ClearChildren(m_StudyRecipeListWrap);
		}
		m_StudyVariationListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("StudyVariationListWrap"));
		m_StudyButton = ButtonWidget.Cast(m_Root.FindAnyWidget("StudyButton"));
		m_StudyButtonText = TextWidget.Cast(m_Root.FindAnyWidget("StudyButtonText"));
		m_StudyButtonFon = m_Root.FindAnyWidget("StudyButtonFon");
		if (m_StudyButtonFon)
		{
			m_StudyButtonFonDefaultColor = m_StudyButtonFon.GetColor();
			if (m_StudyButtonFonDefaultColor == 0)
			{
				m_StudyButtonFonDefaultColor = ARGB(90, 213, 137, 49);
			}
		}

		m_LearningFonImage = ImageWidget.Cast(m_Root.FindAnyWidget("LearningFonImage"));
		if (!m_LearningFonImage)
		{
			m_LearningFonImage = ImageWidget.Cast(m_Root.FindAnyWidget("ImageWidget0"));
		}
		ApplyLearningFon();

		BuildCategories();

		return m_Root;
	}

	override void OnShow()
	{
		super.OnShow();

		MissionGameplay m = MissionGameplay.Cast(GetGame().GetMission());
		if (m && m.GetHud())
		{
			m.GetHud().ShowHudUI(false);
			m.GetHud().ShowQuickbarUI(false);
		}

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);
		SetFocus(m_Root);

		ApplyLearningFon();
		BuildCategories();
		if (m_CurrentCategoryIndex >= 0)
		{
			SelectCategory(m_CurrentCategoryIndex);
		}
		else if (m_CurrentCategoryIndex == CATEGORY_STUDIED_INDEX)
		{
			SelectStudiedCategory();
		}
		else
		{
			if (m_RightPanelWidget)
			{
				m_RightPanelWidget.Show(false);
			}
			if (m_LearningPanelWidget)
			{
				m_LearningPanelWidget.Show(false);
			}
		}
	}

	override void OnHide()
	{
		super.OnHide();

		GetGame().GetMission().PlayerControlEnable(true);
		MissionGameplay m = MissionGameplay.Cast(GetGame().GetMission());
		if (m && m.GetHud())
		{
			m.GetHud().ShowHudUI(true);
			m.GetHud().ShowQuickbarUI(true);
		}
		GetGame().GetInput().ResetGameFocus();
		GetGame().GetUIManager().ShowCursor(false);
		StopRecipeQueue();
		CleanupPreviews();
		m_CurrentCategoryIndex = -1;
		UpdateCategoryHighlights();
		if (m_LearningCrafttListWrap)
		{
			ClearChildren(m_LearningCrafttListWrap);
		}
		if (m_StudyRecipeListWrap)
		{
			ClearChildren(m_StudyRecipeListWrap);
		}
		if (m_LockStudyRecipeListWrap)
		{
			ClearChildren(m_LockStudyRecipeListWrap);
		}
		if (m_StudyVariationListWrap)
		{
			ClearChildren(m_StudyVariationListWrap);
		}
		if (m_RightPanelWidget)
		{
			m_RightPanelWidget.Show(false);
		}
		if (m_LearningPanelWidget)
		{
			m_LearningPanelWidget.Show(false);
		}
		if (m_StudyPanel)
		{
			m_StudyPanel.Show(false);
		}
		if (m_LockStudyPanel)
		{
			m_LockStudyPanel.Show(false);
		}
	}

	protected void DetachPreviewsFromList(array<Widget> widgetList, string previewName)
	{
		if (!widgetList)
		{
			return;
		}

		for (int i = 0; i < widgetList.Count(); i++)
		{
			Widget w = widgetList[i];
			if (w)
			{
				ItemPreviewWidget prev = ItemPreviewWidget.Cast(w.FindAnyWidget(previewName));
				if (prev)
				{
					prev.SetItem(null);
				}
			}
		}
	}

	void CleanupPreviews()
	{
		if (m_LearningPreview)
		{
			m_LearningPreview.SetItem(null);
		}

		DetachPreviewsFromList(m_RecipeWidgets, "LearningCraftItemPreview");
		DetachPreviewsFromList(m_LockStudyRecipeWidgets, "LearningCraftItemPreview");
		DetachPreviewsFromList(m_StudyRecipeWidgets, "StudyRecipeItemPreview");
		DetachPreviewsFromList(m_VariationWidgets, "StudyVariationItemPreview");

		if (m_RecipeWidgets)
		{
			m_RecipeWidgets.Clear();
		}

		if (m_VariationWidgets)
		{
			m_VariationWidgets.Clear();
		}

		if (m_StudyRecipeWidgets)
		{
			m_StudyRecipeWidgets.Clear();
		}

		if (m_LockStudyRecipeWidgets)
		{
			m_LockStudyRecipeWidgets.Clear();
		}

		if (m_PreviewEntities)
		{
			foreach (string cls, EntityAI ent : m_PreviewEntities)
			{
				if (ent)
				{
					GetGame().ObjectDelete(ent);
				}
			}
			m_PreviewEntities.Clear();
		}

		if (m_PreviewTrash)
		{
			for (int i = 0; i < m_PreviewTrash.Count(); i++)
			{
				EntityAI tent = m_PreviewTrash[i];
				if (tent)
				{
					GetGame().ObjectDelete(tent);
				}
			}
			m_PreviewTrash.Clear();
		}
	}

	EntityAI GetOrCreatePreviewEntity(string className)
	{
		if (className == "")
		{
			return null;
		}

		EntityAI existing;
		if (m_PreviewEntities.Find(className, existing))
		{
			if (existing)
			{
				return existing;
			}
		}

		EntityAI created = EntityAI.Cast(GetGame().CreateObjectEx(className, vector.Zero, ECE_LOCAL|ECE_NOLIFETIME));
		if (created)
		{
			m_PreviewEntities.Set(className, created);
			return created;
		}

		return null;
	}

	bool IsRecipeStudyable(int recipeId)
	{
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg || !lcfg.LearningRecipes)
		{
			return false;
		}

		return (lcfg.FindByAnyRecipeId(recipeId) != null);
	}

	int GetRecipeLearningState(int recipeId)
	{
		if (DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(recipeId))
		{
			return 2;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
		{
			return 1;
		}

		DRL_Learning_Recipe lr = lcfg.FindByPrimaryId(recipeId);
		if (!lr)
		{
			lr = lcfg.FindByAnyRecipeId(recipeId);
		}

		if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
		{
			for (int i = 0; i < lr.RequiredRecipeIds.Count(); i++)
			{
				int reqId = lr.RequiredRecipeIds[i];
				if (!DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(reqId))
				{
					return 0;
				}
			}
		}

		if (lr)
		{
			DRL_Craft_Recipe checkRec;
			if (g_Craft_Config)
			{
				checkRec = g_Craft_Config.FindById(recipeId);
			}

			string customFailReason;
			if (!DRL_CheckLearningCustomRequirements(lr, checkRec, customFailReason))
			{
				return 0;
			}
		}

		return 1;
	}

	bool DRL_CheckLearningCustomRequirements(DRL_Learning_Recipe lr, DRL_Craft_Recipe r, out string failReason)
	{
		failReason = "";
		return true;
	}

	void DRL_ShowLearningCustomRequirements(DRL_Learning_Recipe lr, DRL_Craft_Recipe r)
	{
	}

	bool IsRecipeAvailableToStudyNow(int recipeId)
	{
		if (!IsRecipeStudyable(recipeId))
		{
			return false;
		}

		if (DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(recipeId))
		{
			return false;
		}

		return (GetRecipeLearningState(recipeId) == 1);
	}

	void UpdateLockIconVisual(ImageWidget iconWidget, int state)
	{
		if (!iconWidget)
		{
			return;
		}

		iconWidget.Show(true);
		if (state == 2)
		{
			iconWidget.LoadImageFile(0, "DRL_CraftMenu/UI/data/baitinfo.edds");
			iconWidget.SetColor(ARGB(255, 80, 210, 80));
		}
		else if (state == 1)
		{
			iconWidget.LoadImageFile(0, "DRL_CraftMenu/UI/data/unlock.edds");
			iconWidget.SetColor(ARGB(255, 213, 137, 49));
		}
		else
		{
			iconWidget.LoadImageFile(0, "DRL_CraftMenu/UI/data/lock1.edds");
			iconWidget.SetColor(ARGB(255, 230, 230, 230));
		}
	}

	int GetCategoryStudyableCount(DRL_Craft_Category cat, DRL_Craft_Config cfg)
	{
		if (!cat || !cfg)
		{
			return 0;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();

		int count = 0;
		if (cat.ShowAll == 1)
		{
			if (!cfg.CraftItems)
			{
				return 0;
			}

			for (int i = 0; i < cfg.CraftItems.Count(); i++)
			{
				DRL_Craft_Recipe r = cfg.CraftItems[i];
				if (r && IsRecipeStudyable(r.RecipeId))
				{
					if (cat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(r.RecipeId))
					{
						continue;
					}

					if (lcfg)
					{
						DRL_Learning_Recipe lr = lcfg.FindByAnyRecipeId(r.RecipeId);
						if (lr && !lr.ShouldShowInLearningList(r.RecipeId))
						{
							continue;
						}
					}
					count++;
				}
			}
		}
		else if (cat.UseList == 1 && cat.List)
		{
			for (int j = 0; j < cat.List.Count(); j++)
			{
				int rId = cat.List[j];
				if (IsRecipeStudyable(rId))
				{
					if (cat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(rId))
					{
						continue;
					}

					if (lcfg)
					{
						DRL_Learning_Recipe lr2 = lcfg.FindByAnyRecipeId(rId);
						if (lr2 && !lr2.ShouldShowInLearningList(rId))
						{
							continue;
						}
					}
					DRL_Craft_Recipe rec = cfg.FindById(rId);
					if (rec)
					{
						count++;
					}
				}
			}
		}

		return count;
	}

	void ApplyLearningFon()
	{
		if (!m_LearningFonImage)
		{
			return;
		}

		string fonPath = "set:drl_craft_learning_fon image:learning_fon";
		if (g_Craft_Config && g_Craft_Config.Settings && g_Craft_Config.Settings.LearningFonImagePath != "")
		{
			fonPath = g_Craft_Config.Settings.LearningFonImagePath;
		}

		m_LearningFonImage.LoadImageFile(0, DRL_CraftMenuUI.ResolveImagePath(fonPath));
	}

	void BuildCategories()
	{
		if (!m_SpacerAllLearningCategoryList)
		{
			return;
		}

		m_CategoryWidgets.Clear();
		if (!m_VisibleCategoryIndices)
		{
			m_VisibleCategoryIndices = new array<int>();
		}
		m_VisibleCategoryIndices.Clear();
		ClearChildren(m_SpacerAllLearningCategoryList);

		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.Categories)
		{
			return;
		}

		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			DRL_Craft_Category cat = cfg.Categories[i];
			if (!cat)
			{
				continue;
			}

			int catCount = GetCategoryStudyableCount(cat, cfg);
			if (catCount <= 0)
			{
				continue;
			}

			int visibleIdx = m_VisibleCategoryIndices.Count();
			m_VisibleCategoryIndices.Insert(i);

			Widget catW = GetGame().GetWorkspace().CreateWidgets(CATEGORY_TEMPLATE, m_SpacerAllLearningCategoryList);
			if (!catW)
			{
				continue;
			}

			if (m_BPanelDefaultColor == 0)
			{
				Widget bpCheck = catW.FindAnyWidget("BPanelWidget");
				if (bpCheck)
				{
					m_BPanelDefaultColor = bpCheck.GetColor();
				}
				if (m_BPanelDefaultColor == 0)
				{
					m_BPanelDefaultColor = ARGB(255, 68, 68, 68);
				}
			}

			RichTextWidget catName = RichTextWidget.Cast(catW.FindAnyWidget("NameCategoryButton"));
			if (catName)
			{
				catName.SetText(cat.Name);
			}

			RichTextWidget catAmount = RichTextWidget.Cast(catW.FindAnyWidget("AmountCategory"));
			if (catAmount)
			{
				catAmount.SetText(catCount.ToString());
			}

			ImageWidget catImg = ImageWidget.Cast(catW.FindAnyWidget("CategoryImagePreview"));
			if (catImg && cat.Icon != "")
			{
				catImg.LoadImageFile(0, DRL_CraftMenuUI.ResolveImagePath(cat.Icon));
			}

			ButtonWidget catBtn = ButtonWidget.Cast(catW);
			if (!catBtn)
			{
				catBtn = ButtonWidget.Cast(catW.FindAnyWidget("CategorySelectionButton"));
			}
			if (catBtn)
			{
				catBtn.SetUserID(visibleIdx);
			}

			m_CategoryWidgets.Insert(catW);
		}

		UpdateStudiedCategoryCounter();
		UpdateCategoryHighlights();
	}

	void UpdateStudiedCategoryCounter()
	{
		if (!m_AmountStudiedCategory || !g_Craft_Config || !g_Craft_Config.CraftItems)
		{
			return;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();
		int count = 0;

		for (int i = 0; i < g_Craft_Config.CraftItems.Count(); i++)
		{
			DRL_Craft_Recipe r = g_Craft_Config.CraftItems[i];
			if (!r || !IsRecipeStudyable(r.RecipeId))
			{
				continue;
			}

			if (lcfg)
			{
				DRL_Learning_Recipe lr = lcfg.FindByAnyRecipeId(r.RecipeId);
				if (lr && !lr.ShouldShowInLearningList(r.RecipeId))
				{
					continue;
				}
			}

			if (GetRecipeLearningState(r.RecipeId) == 2)
			{
				count++;
			}
		}

		m_AmountStudiedCategory.SetText(count.ToString());
	}

	void UpdateCategoryHighlights()
	{
		for (int i = 0; i < m_CategoryWidgets.Count(); i++)
		{
			Widget cw = m_CategoryWidgets[i];
			if (!cw)
			{
				continue;
			}

			bool isSelected = (m_CurrentCategoryIndex >= 0 && i == m_CurrentCategoryIndex);

			Widget colorBar = cw.FindAnyWidget("AllCategoryColorWidget");
			if (colorBar)
			{
				if (isSelected)
				{
					colorBar.SetColor(ARGB(255, 213, 137, 49));
				}
				else
				{
					colorBar.SetColor(ARGB(0, 0, 0, 0));
				}
			}

			Widget bPanel = cw.FindAnyWidget("BPanelWidget");
			if (bPanel)
			{
				if (isSelected)
				{
					bPanel.SetColor(ARGB(255, 213, 137, 49));
				}
				else
				{
					bPanel.SetColor(m_BPanelDefaultColor);
				}
			}
		}

		if (m_StudiedCategoryButton)
		{
			bool isStudiedSelected = (m_CurrentCategoryIndex == CATEGORY_STUDIED_INDEX);

			if (m_StudiedCategoryColorWidget)
			{
				if (isStudiedSelected)
				{
					m_StudiedCategoryColorWidget.SetColor(ARGB(255, 213, 137, 49));
				}
				else
				{
					m_StudiedCategoryColorWidget.SetColor(ARGB(0, 0, 0, 0));
				}
			}

			if (m_StudiedCategoryBPanel)
			{
				if (isStudiedSelected)
				{
					m_StudiedCategoryBPanel.SetColor(ARGB(255, 213, 137, 49));
				}
				else
				{
					m_StudiedCategoryBPanel.SetColor(m_BPanelDefaultColor);
				}
			}
		}
	}

	void SelectStudiedCategory()
	{
		m_CurrentCategoryIndex = CATEGORY_STUDIED_INDEX;
		UpdateCategoryHighlights();

		if (m_RightPanelWidget)
		{
			m_RightPanelWidget.Show(true);
		}

		if (m_LearningListPanelWidget)
		{
			m_LearningListPanelWidget.SetSize(1.0, 1.0);
		}

		if (m_LearningPanelWidget)
		{
			m_LearningPanelWidget.Show(false);
		}

		m_SelectedRecipe = null;
		m_SelectedLearningRecipe = null;
		m_SelectedVariationIndex = -1;

		BuildRecipeList();
	}

	void SelectCategory(int visibleIndex)
	{
		m_CurrentCategoryIndex = visibleIndex;
		UpdateCategoryHighlights();

		if (m_RightPanelWidget)
		{
			m_RightPanelWidget.Show(true);
		}

		if (m_LearningListPanelWidget)
		{
			m_LearningListPanelWidget.SetSize(1.0, 1.0);
		}

		if (m_LearningPanelWidget)
		{
			m_LearningPanelWidget.Show(false);
		}

		m_SelectedRecipe = null;
		m_SelectedLearningRecipe = null;
		m_SelectedVariationIndex = -1;

		BuildRecipeList();
	}

	void BuildRecipeList()
	{
		StopRecipeQueue();

		if (!m_LearningCrafttListWrap)
		{
			return;
		}

		DetachPreviewsFromList(m_RecipeWidgets, "LearningCraftItemPreview");
		m_RecipeWidgets.Clear();
		ClearChildren(m_LearningCrafttListWrap);

		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.CraftItems)
		{
			return;
		}

		string filter = "";
		if (m_SearchEditBox)
		{
			filter = m_SearchEditBox.GetText();
			if (filter == SEARCH_DEFAULT_STR)
			{
				filter = "";
			}
			filter.ToLower();
		}

		array<ref DRL_Craft_Recipe> recipesToShow = new array<ref DRL_Craft_Recipe>();
		DRL_Learning_Config lcfg = GetLearning_Config();

		if (m_CurrentCategoryIndex == CATEGORY_STUDIED_INDEX)
		{
			for (int sIdx = 0; sIdx < cfg.CraftItems.Count(); sIdx++)
			{
				DRL_Craft_Recipe sRec = cfg.CraftItems[sIdx];
				if (!sRec || !IsRecipeStudyable(sRec.RecipeId))
				{
					continue;
				}

				if (lcfg)
				{
					DRL_Learning_Recipe lrS = lcfg.FindByAnyRecipeId(sRec.RecipeId);
					if (lrS && !lrS.ShouldShowInLearningList(sRec.RecipeId))
					{
						continue;
					}
				}

				if (GetRecipeLearningState(sRec.RecipeId) == 2)
				{
					recipesToShow.Insert(sRec);
				}
			}
		}
		else
		{
			if (!m_VisibleCategoryIndices || m_CurrentCategoryIndex < 0 || m_CurrentCategoryIndex >= m_VisibleCategoryIndices.Count())
			{
				return;
			}

			int realCatIdx = m_VisibleCategoryIndices[m_CurrentCategoryIndex];
			if (!cfg.Categories || realCatIdx < 0 || realCatIdx >= cfg.Categories.Count())
			{
				return;
			}

			DRL_Craft_Category currentCat = cfg.Categories[realCatIdx];
			if (currentCat)
			{
				if (currentCat.ShowAll == 1)
				{
					for (int allIdx = 0; allIdx < cfg.CraftItems.Count(); allIdx++)
					{
						DRL_Craft_Recipe allRec = cfg.CraftItems[allIdx];
						if (allRec && IsRecipeStudyable(allRec.RecipeId))
						{
							if (currentCat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(allRec.RecipeId))
							{
								continue;
							}

							if (lcfg)
							{
								DRL_Learning_Recipe lrAll = lcfg.FindByAnyRecipeId(allRec.RecipeId);
								if (lrAll && !lrAll.ShouldShowInLearningList(allRec.RecipeId))
								{
									continue;
								}
							}
							recipesToShow.Insert(allRec);
						}
					}
				}
				else if (currentCat.UseList == 1 && currentCat.List)
				{
					for (int li = 0; li < currentCat.List.Count(); li++)
					{
						int rId = currentCat.List[li];
						if (IsRecipeStudyable(rId))
						{
							if (currentCat.ShowOnlyAvailableStudy == 1 && !IsRecipeAvailableToStudyNow(rId))
							{
								continue;
							}

							if (lcfg)
							{
								DRL_Learning_Recipe lrLi = lcfg.FindByAnyRecipeId(rId);
								if (lrLi && !lrLi.ShouldShowInLearningList(rId))
								{
									continue;
								}
							}
							DRL_Craft_Recipe rec = cfg.FindById(rId);
							if (rec)
							{
								recipesToShow.Insert(rec);
							}
						}
					}
				}
			}
		}

		if (!m_LearningQueueRecipes)
		{
			m_LearningQueueRecipes = new array<DRL_Craft_Recipe>();
		}
		m_LearningQueueRecipes.Clear();

		for (int i = 0; i < recipesToShow.Count(); i++)
		{
			DRL_Craft_Recipe rFilter = recipesToShow[i];
			if (!rFilter)
			{
				continue;
			}

			if (filter != "")
			{
				string rNameLower = rFilter.RecipeName;
				rNameLower.ToLower();
				if (rNameLower.IndexOf(filter) == -1)
				{
					continue;
				}
			}

			m_LearningQueueRecipes.Insert(rFilter);
		}

		m_LearningQueueIndex = 0;
		if (m_LearningQueueRecipes.Count() > 0)
		{
			if (!m_LearningQueueTimer)
			{
				m_LearningQueueTimer = new Timer(CALL_CATEGORY_GUI);
			}
			m_LearningQueueTimer.Run(LEARNING_QUEUE_TICK_RATE, this, "OnLearningQueueTick", null, true);
		}
		else
		{
			UpdateRecipeHighlights();
		}
	}

	void StopRecipeQueue()
	{
		if (m_LearningQueueTimer)
		{
			m_LearningQueueTimer.Stop();
		}
		if (m_LearningQueueRecipes)
		{
			m_LearningQueueRecipes.Clear();
		}
		m_LearningQueueIndex = 0;
	}

	void OnLearningQueueTick()
	{
		if (!m_LearningCrafttListWrap || !m_LearningQueueRecipes || m_LearningQueueIndex >= m_LearningQueueRecipes.Count())
		{
			StopRecipeQueue();
			if (m_LearningCrafttListWrap)
			{
				m_LearningCrafttListWrap.Update();
			}
			UpdateRecipeHighlights();
			return;
		}

		int countThisFrame = 0;
		while (m_LearningQueueIndex < m_LearningQueueRecipes.Count() && countThisFrame < LEARNING_RECIPES_PER_FRAME)
		{
			DRL_Craft_Recipe r = m_LearningQueueRecipes[m_LearningQueueIndex];
			m_LearningQueueIndex++;

			if (r)
			{
				CreateLearningRecipeTile(r);
			}

			countThisFrame++;
		}

		if (m_LearningCrafttListWrap)
		{
			m_LearningCrafttListWrap.Update();
		}

		if (m_LearningQueueIndex >= m_LearningQueueRecipes.Count())
		{
			StopRecipeQueue();
			UpdateRecipeHighlights();
		}
	}

	void CreateLearningRecipeTile(DRL_Craft_Recipe r)
	{
		if (!m_LearningCrafttListWrap || !r)
		{
			return;
		}

		Widget rw = GetGame().GetWorkspace().CreateWidgets(RECIPE_TEMPLATE, m_LearningCrafttListWrap);
		if (!rw)
		{
			return;
		}

		if (m_LearningCraftPanelDefaultColor == 0)
		{
			Widget pBgTest = rw.FindAnyWidget("LearningCraftItemPanelWidget");
			if (!pBgTest)
			{
				pBgTest = rw.FindAnyWidget("RecipeLearningItemPanelWidget");
			}
			if (pBgTest)
			{
				m_LearningCraftPanelDefaultColor = pBgTest.GetColor();
			}
			if (m_LearningCraftPanelDefaultColor == 0)
			{
				m_LearningCraftPanelDefaultColor = ARGB(200, 29, 22, 13);
			}
		}

		if (m_LearningCraftBorderDefaultColor == 0)
		{
			Widget pBrdTest = rw.FindAnyWidget("LearningCraftItemPanelW");
			if (!pBrdTest)
			{
				pBrdTest = rw.FindAnyWidget("RecipeLearningItemPanelW");
			}
			if (pBrdTest)
			{
				m_LearningCraftBorderDefaultColor = pBrdTest.GetColor();
			}
			if (m_LearningCraftBorderDefaultColor == 0)
			{
				m_LearningCraftBorderDefaultColor = ARGB(255, 68, 68, 68);
			}
		}

		MultilineTextWidget titleW = MultilineTextWidget.Cast(rw.FindAnyWidget("LearningCraftTitleText"));
		if (titleW)
		{
			titleW.SetText(r.RecipeName);
		}

		string previewCls = r.PreviewClassname;
		if (previewCls == "" && r.Result != "")
		{
			previewCls = r.Result;
		}

		ItemPreviewWidget prevW = ItemPreviewWidget.Cast(rw.FindAnyWidget("LearningCraftItemPreview"));
		if (prevW && previewCls != "")
		{
			EntityAI ent = GetOrCreatePreviewEntity(previewCls);
			if (ent)
			{
				prevW.SetItem(ent);
				prevW.SetModelPosition(Vector(0, 0, 0.5));
				prevW.SetModelOrientation(Vector(0, 0, 0));
			}
		}

		ImageWidget lockW = ImageWidget.Cast(rw.FindAnyWidget("LockIcon"));
		if (lockW)
		{
			int st = GetRecipeLearningState(r.RecipeId);
			UpdateLockIconVisual(lockW, st);
		}

		ButtonWidget btn = ButtonWidget.Cast(rw);
		if (!btn)
		{
			btn = ButtonWidget.Cast(rw.FindAnyWidget("LearningCraftButton"));
		}
		if (btn)
		{
			btn.SetUserID(r.RecipeId);
			btn.SetUserData(r);
		}

		m_RecipeWidgets.Insert(rw);
	}

	void UpdateRecipeHighlights()
	{
		for (int i = 0; i < m_RecipeWidgets.Count(); i++)
		{
			Widget rw = m_RecipeWidgets[i];
			if (!rw)
			{
				continue;
			}

			bool isSelected = false;
			if (m_SelectedRecipe)
			{
				ButtonWidget btn = ButtonWidget.Cast(rw);
				if (!btn)
				{
					btn = ButtonWidget.Cast(rw.FindAnyWidget("LearningCraftButton"));
					if (!btn)
					{
						btn = ButtonWidget.Cast(rw.FindAnyWidget("RecipeLearningButton"));
					}
				}
				if (btn && btn.GetUserID() == m_SelectedRecipe.RecipeId)
				{
					isSelected = true;
				}
			}

			Widget panelBg = rw.FindAnyWidget("LearningCraftItemPanelWidget");
			if (!panelBg)
			{
				panelBg = rw.FindAnyWidget("RecipeLearningItemPanelWidget");
			}
			if (panelBg)
			{
				if (isSelected)
				{
					panelBg.SetColor(ARGB(8, 213, 137, 49));
				}
				else
				{
					panelBg.SetColor(m_LearningCraftPanelDefaultColor);
				}
			}

			Widget panelBorder = rw.FindAnyWidget("LearningCraftItemPanelW");
			if (!panelBorder)
			{
				panelBorder = rw.FindAnyWidget("RecipeLearningItemPanelW");
			}
			if (panelBorder)
			{
				if (isSelected)
				{
					panelBorder.SetColor(ARGB(255, 213, 137, 49));
				}
				else
				{
					panelBorder.SetColor(m_LearningCraftBorderDefaultColor);
				}
			}
		}
	}

	void SelectRecipe(DRL_Craft_Recipe r)
	{
		if (!r)
		{
			return;
		}

		m_SelectedRecipe = r;
		m_SelectedVariationIndex = -1;
		UpdateRecipeHighlights();

		if (m_LearningListPanelWidget)
		{
			m_LearningListPanelWidget.SetSize(0.71, 1.0);
		}

		if (m_LearningPanelWidget)
		{
			m_LearningPanelWidget.Show(true);
		}

		int state = GetRecipeLearningState(r.RecipeId);

		if (m_LearningLockIcon)
		{
			UpdateLockIconVisual(m_LearningLockIcon, state);
		}

		if (m_LearningRecipeTitel)
		{
			if (state == 2)
			{
				m_LearningRecipeTitel.SetText("#STR_DRL_LEARNED");
			}
			else if (state == 1)
			{
				m_LearningRecipeTitel.SetText("#STR_LearningLockTitel");
			}
			else
			{
				m_LearningRecipeTitel.SetText("#STR_DRL_NOT_AVAILABLE");
			}
		}

		if (m_LearningNameTite)
		{
			m_LearningNameTite.SetText(r.RecipeName);
		}

		if (m_LearningDescriptionMultiline)
		{
			m_LearningDescriptionMultiline.SetText(r.Description);
		}

		string previewCls = r.PreviewClassname;
		if (previewCls == "" && r.Result != "")
		{
			previewCls = r.Result;
		}

		if (m_LearningPreview)
		{
			if (previewCls != "")
			{
				EntityAI ent = GetOrCreatePreviewEntity(previewCls);
				if (ent)
				{
					m_LearningPreview.SetItem(ent);
					m_LearningPreview.SetModelPosition(Vector(0, 0, 0.5));
					m_LearningPreview.SetModelOrientation(Vector(0, 0, 0));
				}
				else
				{
					m_LearningPreview.SetItem(null);
				}
			}
			else
			{
				m_LearningPreview.SetItem(null);
			}
		}

		if (state == 0)
		{
			if (m_StudyPanel)
			{
				m_StudyPanel.Show(false);
			}
			if (m_LockStudyPanel)
			{
				m_LockStudyPanel.Show(true);
			}
			BuildLockStudyRecipes();
		}
		else
		{
			if (m_LockStudyPanel)
			{
				m_LockStudyPanel.Show(false);
			}
			if (m_StudyPanel)
			{
				m_StudyPanel.Show(true);
			}
			BuildStudyVariations();
			BuildStudyRecipes();
			DRL_ShowLearningCustomRequirements(m_SelectedLearningRecipe, r);
		}
	}

	void DRL_AddRequirementHeader(string title)
	{
		if (!m_LockStudyRecipeListWrap) return;
		Widget head = GetGame().GetWorkspace().CreateWidgets(REQ_HEADER_TEMPLATE, m_LockStudyRecipeListWrap);
		if (!head) return;
		RichTextWidget txt = RichTextWidget.Cast(head.FindAnyWidget("RequirementHeaderText"));
		if (txt) txt.SetText(title);
	}

	Widget DRL_AddRequirementItem(string title, string subtitle, string iconPath, string previewClass, bool isMet, string statusText = "", DRL_Craft_Recipe targetRecipe = null)
	{
		if (!m_LockStudyRecipeListWrap) return null;
		Widget row = GetGame().GetWorkspace().CreateWidgets(REQ_ITEM_TEMPLATE, m_LockStudyRecipeListWrap);
		if (!row) return null;

		RichTextWidget titleW = RichTextWidget.Cast(row.FindAnyWidget("ReqTitle"));
		if (titleW) titleW.SetText(title);

		TextWidget subW = TextWidget.Cast(row.FindAnyWidget("ReqSubtitle"));
		if (subW) subW.SetText(subtitle);

		ItemPreviewWidget prevW = ItemPreviewWidget.Cast(row.FindAnyWidget("ReqItemPreview"));
		ImageWidget iconW = ImageWidget.Cast(row.FindAnyWidget("ReqIcon"));

		if (previewClass != "" && prevW)
		{
			EntityAI prevEnt = GetOrCreatePreviewEntity(previewClass);
			if (prevEnt)
			{
				prevW.SetItem(prevEnt);
				int vIdx = prevEnt.GetViewIndex();
				if (vIdx < 0) vIdx = 0;
				prevW.SetView(vIdx);
				prevW.SetModelPosition(Vector(0, 0, 0));
				prevW.SetModelOrientation(Vector(0, 0, 0));
				prevW.Show(true);
			}
		}
		else if (iconPath != "" && iconW)
		{
			iconW.LoadImageFile(0, iconPath);
			iconW.Show(true);
		}
		else if (iconW)
		{
			iconW.LoadImageFile(0, "DRL_CraftMenu/UI/data/info.edds");
			iconW.Show(true);
		}

		ImageWidget statusIcon = ImageWidget.Cast(row.FindAnyWidget("ReqStatusIcon"));
		TextWidget statusTxt = TextWidget.Cast(row.FindAnyWidget("ReqStatusText"));

		if (isMet)
		{
			if (statusIcon)
			{
				statusIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/baitinfo.edds");
				statusIcon.SetColor(ARGB(255, 80, 210, 80));
			}
			if (statusTxt)
			{
				if (statusText == "") statusText = "#STR_DRL_ATTACHMENT_INSTALLED";
				statusTxt.SetText(statusText);
				statusTxt.SetColor(ARGB(255, 80, 210, 80));
			}
		}
		else
		{
			if (statusIcon)
			{
				statusIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/lock1.edds");
				statusIcon.SetColor(ARGB(255, 210, 70, 70));
			}
			if (statusTxt)
			{
				if (statusText == "") statusText = "#STR_DRL_ATTACHMENT_MISSING";
				statusTxt.SetText(statusText);
				statusTxt.SetColor(ARGB(255, 210, 70, 70));
			}
		}

		ButtonWidget btn = ButtonWidget.Cast(row.FindAnyWidget("ReqButton"));
		if (btn && targetRecipe)
		{
			btn.SetUserData(targetRecipe);
			btn.SetUserID(targetRecipe.RecipeId);
		}

		return row;
	}

	void DRL_AddRequirementInfo(string text, int color = 0)
	{
		if (!m_LockStudyRecipeListWrap) return;
		Widget infoW = GetGame().GetWorkspace().CreateWidgets(REQ_INFO_TEMPLATE, m_LockStudyRecipeListWrap);
		if (!infoW) return;
		RichTextWidget t = RichTextWidget.Cast(infoW.FindAnyWidget("RequirementInfoText"));
		if (t)
		{
			t.SetText(text);
			if (color != 0) t.SetColor(color);
		}
	}

	protected void BuildLearningAdditionalTasksCustom(DRL_Learning_Recipe lr, DRL_Craft_Recipe r, out int reqCount)
	{
	}

	void BuildLockStudyRecipes()
	{
		if (!m_LockStudyRecipeListWrap)
		{
			return;
		}

		DetachPreviewsFromList(m_LockStudyRecipeWidgets, "LearningCraftItemPreview");
		m_LockStudyRecipeWidgets.Clear();
		ClearChildren(m_LockStudyRecipeListWrap);

		if (!m_SelectedRecipe)
		{
			return;
		}

		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg)
		{
			return;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
		{
			return;
		}

		m_SelectedLearningRecipe = lcfg.FindByPrimaryId(m_SelectedRecipe.RecipeId);
		if (!m_SelectedLearningRecipe)
		{
			m_SelectedLearningRecipe = lcfg.FindByAnyRecipeId(m_SelectedRecipe.RecipeId);
		}

		int totalReqCount = 0;

		if (m_SelectedLearningRecipe && m_SelectedLearningRecipe.RequiredRecipeIds && m_SelectedLearningRecipe.RequiredRecipeIds.Count() > 0)
		{
			DRL_AddRequirementHeader("#STR_ToStudyRecipe");

			for (int i = 0; i < m_SelectedLearningRecipe.RequiredRecipeIds.Count(); i++)
			{
				int reqId = m_SelectedLearningRecipe.RequiredRecipeIds[i];
				DRL_Craft_Recipe targetRec = cfg.FindById(reqId);
				if (!targetRec)
				{
					continue;
				}

				Widget rw = GetGame().GetWorkspace().CreateWidgets(RECIPE_TEMPLATE, m_LockStudyRecipeListWrap);
				if (!rw)
				{
					continue;
				}

				MultilineTextWidget titleW = MultilineTextWidget.Cast(rw.FindAnyWidget("LearningCraftTitleText"));
				if (titleW)
				{
					titleW.SetText(targetRec.RecipeName);
				}

				string previewCls = targetRec.PreviewClassname;
				if (previewCls == "" && targetRec.Result != "")
				{
					previewCls = targetRec.Result;
				}

				ItemPreviewWidget prevW = ItemPreviewWidget.Cast(rw.FindAnyWidget("LearningCraftItemPreview"));
				if (prevW && previewCls != "")
				{
					EntityAI ent = GetOrCreatePreviewEntity(previewCls);
					if (ent)
					{
						prevW.SetItem(ent);
						prevW.SetModelPosition(Vector(0, 0, 0.5));
						prevW.SetModelOrientation(Vector(0, 0, 0));
					}
				}

				ImageWidget lockW = ImageWidget.Cast(rw.FindAnyWidget("LockIcon"));
				if (lockW)
				{
					int st = GetRecipeLearningState(targetRec.RecipeId);
					UpdateLockIconVisual(lockW, st);
				}

				ButtonWidget btn = ButtonWidget.Cast(rw);
				if (!btn)
				{
					btn = ButtonWidget.Cast(rw.FindAnyWidget("LearningCraftButton"));
				}
				if (btn)
				{
					btn.SetUserID(reqId);
					btn.SetUserData(targetRec);
				}

				m_LockStudyRecipeWidgets.Insert(rw);
				totalReqCount++;
			}
		}

		BuildLearningAdditionalTasksCustom(m_SelectedLearningRecipe, m_SelectedRecipe, totalReqCount);

		m_LockStudyRecipeListWrap.Update();
	}

	void BuildStudyRecipes()
	{
		if (!m_StudyRecipeListWrap)
		{
			return;
		}

		DetachPreviewsFromList(m_StudyRecipeWidgets, "StudyRecipeItemPreview");
		m_StudyRecipeWidgets.Clear();
		ClearChildren(m_StudyRecipeListWrap);

		if (!m_SelectedRecipe)
		{
			return;
		}

		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg)
		{
			return;
		}

		if (!m_SelectedLearningRecipe)
		{
			DRL_Learning_Config lcfg = GetLearning_Config();
			if (lcfg)
			{
				m_SelectedLearningRecipe = lcfg.FindByPrimaryId(m_SelectedRecipe.RecipeId);
				if (!m_SelectedLearningRecipe)
				{
					m_SelectedLearningRecipe = lcfg.FindByAnyRecipeId(m_SelectedRecipe.RecipeId);
				}
			}
		}

		array<int> idsToShow = new array<int>();
		if (m_SelectedLearningRecipe && m_SelectedLearningRecipe.IsStudyMain() && m_SelectedLearningRecipe.RecipeIds)
		{
			for (int i = 0; i < m_SelectedLearningRecipe.RecipeIds.Count(); i++)
			{
				idsToShow.Insert(m_SelectedLearningRecipe.RecipeIds[i]);
			}
		}
		else
		{
			idsToShow.Insert(m_SelectedRecipe.RecipeId);
		}

		for (int j = 0; j < idsToShow.Count(); j++)
		{
			int recId = idsToShow[j];
			DRL_Craft_Recipe targetRec = cfg.FindById(recId);
			if (!targetRec)
			{
				continue;
			}

			Widget srw = GetGame().GetWorkspace().CreateWidgets(STUDY_RECIPE_TEMPLATE, m_StudyRecipeListWrap);
			if (!srw)
			{
				continue;
			}

			string previewCls = targetRec.PreviewClassname;
			if (previewCls == "" && targetRec.Result != "")
			{
				previewCls = targetRec.Result;
			}

			ItemPreviewWidget prevW = ItemPreviewWidget.Cast(srw.FindAnyWidget("StudyRecipeItemPreview"));
			if (prevW && previewCls != "")
			{
				EntityAI ent = GetOrCreatePreviewEntity(previewCls);
				if (ent)
				{
					prevW.SetItem(ent);
					prevW.SetModelPosition(Vector(0, 0, 0.5));
					prevW.SetModelOrientation(Vector(0, 0, 0));
				}
			}

			ButtonWidget btn = ButtonWidget.Cast(srw);
			if (!btn)
			{
				btn = ButtonWidget.Cast(srw.FindAnyWidget("StudyRecipeButton"));
			}
			if (btn)
			{
				btn.SetUserID(recId);
				btn.SetUserData(targetRec);
			}

			m_StudyRecipeWidgets.Insert(srw);
		}
	}

	void BuildStudyVariations()
	{
		if (!m_StudyVariationListWrap)
		{
			return;
		}

		DetachPreviewsFromList(m_VariationWidgets, "StudyVariationItemPreview");
		m_VariationWidgets.Clear();
		ClearChildren(m_StudyVariationListWrap);

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
		{
			UpdateStudyButtonState();
			return;
		}

		m_SelectedLearningRecipe = lcfg.FindByPrimaryId(m_SelectedRecipe.RecipeId);
		if (!m_SelectedLearningRecipe)
		{
			m_SelectedLearningRecipe = lcfg.FindByAnyRecipeId(m_SelectedRecipe.RecipeId);
		}

		if (!m_SelectedLearningRecipe || !m_SelectedLearningRecipe.StudyVariations)
		{
			UpdateStudyButtonState();
			return;
		}

		bool alreadyLearned = DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(m_SelectedRecipe.RecipeId);

		for (int i = 0; i < m_SelectedLearningRecipe.StudyVariations.Count(); i++)
		{
			DRL_Study_Variation v = m_SelectedLearningRecipe.StudyVariations[i];
			if (!v)
			{
				continue;
			}

			Widget vw = GetGame().GetWorkspace().CreateWidgets(VARIATION_TEMPLATE, m_StudyVariationListWrap);
			if (!vw)
			{
				continue;
			}

			ItemPreviewWidget prevW = ItemPreviewWidget.Cast(vw.FindAnyWidget("StudyVariationItemPreview"));
			if (prevW && v.RequiredItem != "")
			{
				EntityAI vent = GetOrCreatePreviewEntity(v.RequiredItem);
				if (vent)
				{
					prevW.SetItem(vent);
					prevW.SetModelPosition(Vector(0, 0, 0.5));
					prevW.SetModelOrientation(Vector(0, 0, 0));
				}
			}

			TextWidget nameTitleW = TextWidget.Cast(vw.FindAnyWidget("NameItemTitle"));
			if (nameTitleW)
			{
				string titleText = "";
				if (v.DisplayName != "")
				{
					titleText = v.DisplayName;
				}
				else
				{
					titleText = GetItemDisplayName(v.RequiredItem) + " x" + v.Amount.ToString();
				}
				nameTitleW.SetText(titleText);
			}

			int have = GetPlayerInventoryCount(v.RequiredItem);
			TextWidget availW = TextWidget.Cast(vw.FindAnyWidget("AvailableText"));
			if (availW)
			{
				string availText = Widget.TranslateString("#STR_DRL_AVAILABLE") + " " + have.ToString() + "/" + v.Amount.ToString();
				availW.SetText(availText);
				if (have >= v.Amount)
				{
					availW.SetColor(ARGB(255, 80, 210, 80));
				}
				else
				{
					availW.SetColor(ARGB(255, 200, 70, 70));
				}
			}

			CheckBoxWidget cb = CheckBoxWidget.Cast(vw.FindAnyWidget("StudyVariationCheckBox"));
			if (cb)
			{
				cb.SetChecked(i == m_SelectedVariationIndex);
				if (alreadyLearned)
				{
					cb.Enable(false);
				}
			}

			ButtonWidget varBtn = ButtonWidget.Cast(vw);
			if (!varBtn)
			{
				varBtn = ButtonWidget.Cast(vw.FindAnyWidget("StudyVariationButton"));
			}
			if (varBtn)
			{
				varBtn.SetUserID(i);
				if (alreadyLearned)
				{
					varBtn.Enable(false);
				}
			}

			m_VariationWidgets.Insert(vw);
		}

		UpdateStudyButtonState();
	}

	void SelectVariation(int index)
	{
		m_SelectedVariationIndex = index;

		for (int i = 0; i < m_VariationWidgets.Count(); i++)
		{
			Widget vw = m_VariationWidgets[i];
			if (!vw)
			{
				continue;
			}

			CheckBoxWidget cb = CheckBoxWidget.Cast(vw.FindAnyWidget("StudyVariationCheckBox"));
			if (cb)
			{
				cb.SetChecked(i == m_SelectedVariationIndex);
			}
		}

		UpdateStudyButtonState();
	}

	void UpdateStudyButtonState()
	{
		if (!m_StudyButton || !m_StudyButtonText)
		{
			return;
		}

		if (m_StudyButtonFon)
		{
			m_StudyButtonFon.SetColor(m_StudyButtonFonDefaultColor);
		}

		if (!m_SelectedRecipe)
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
			return;
		}

		bool alreadyLearned = DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(m_SelectedRecipe.RecipeId);
		if (alreadyLearned)
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_DRL_LEARNED");
			if (m_StudyButtonFon)
			{
				m_StudyButtonFon.SetColor(ARGB(90, 80, 79, 78));
			}
			return;
		}

		string customFailReason = "";
		if (!DRL_CheckLearningCustomRequirements(m_SelectedLearningRecipe, m_SelectedRecipe, customFailReason))
		{
			m_StudyButton.Enable(false);
			if (customFailReason != "")
			{
				m_StudyButtonText.SetText(customFailReason);
			}
			else
			{
				m_StudyButtonText.SetText("#STR_DRL_NOT_AVAILABLE");
			}
			if (m_StudyButtonFon)
			{
				m_StudyButtonFon.SetColor(ARGB(90, 80, 79, 78));
			}
			return;
		}

		if (m_SelectedVariationIndex < 0 || !m_SelectedLearningRecipe || !m_SelectedLearningRecipe.StudyVariations)
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
			return;
		}

		if (m_SelectedVariationIndex >= m_SelectedLearningRecipe.StudyVariations.Count())
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
			return;
		}

		DRL_Study_Variation v = m_SelectedLearningRecipe.StudyVariations[m_SelectedVariationIndex];
		if (!v)
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
			return;
		}

		int have = GetPlayerInventoryCount(v.RequiredItem);
		if (have >= v.Amount)
		{
			m_StudyButton.Enable(true);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
		}
		else
		{
			m_StudyButton.Enable(false);
			m_StudyButtonText.SetText("#STR_VIEW_RECIPE");
		}
	}

	void TryStudySelected()
	{
		if (!m_SelectedRecipe)
		{
			return;
		}

		if (m_SelectedVariationIndex < 0)
		{
			return;
		}

		if (DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(m_SelectedRecipe.RecipeId))
		{
			return;
		}

		string customFailReason = "";
		if (!DRL_CheckLearningCustomRequirements(m_SelectedLearningRecipe, m_SelectedRecipe, customFailReason))
		{
			return;
		}

		m_StudyButton.Enable(false);

		DRL_Study_Variation studiedVar = null;
		if (m_SelectedLearningRecipe && m_SelectedLearningRecipe.StudyVariations && m_SelectedVariationIndex < m_SelectedLearningRecipe.StudyVariations.Count())
		{
			studiedVar = m_SelectedLearningRecipe.StudyVariations[m_SelectedVariationIndex];
		}

		if (studiedVar && studiedVar.Delete == 1)
		{
			if (m_SelectedVariationIndex < m_VariationWidgets.Count())
			{
				Widget vw = m_VariationWidgets[m_SelectedVariationIndex];
				if (vw)
				{
					TextWidget availW = TextWidget.Cast(vw.FindAnyWidget("AvailableText"));
					if (availW)
					{
						int curHave = GetPlayerInventoryCount(studiedVar.RequiredItem);
						int newHave = curHave - studiedVar.Amount;
						if (newHave < 0)
						{
							newHave = 0;
						}

						string availText = Widget.TranslateString("#STR_DRL_AVAILABLE") + " " + newHave.ToString() + "/" + studiedVar.Amount.ToString();
						availW.SetText(availText);
						if (newHave >= studiedVar.Amount)
						{
							availW.SetColor(ARGB(255, 80, 210, 80));
						}
						else
						{
							availW.SetColor(ARGB(255, 200, 70, 70));
						}
					}
				}
			}

			DRL_ClientDeductInventoryItems(studiedVar.RequiredItem, studiedVar.Amount);
		}

		if (m_StudyButtonText)
		{
			m_StudyButtonText.SetText("#STR_DRL_LEARNED");
		}
		if (m_StudyButtonFon)
		{
			m_StudyButtonFon.SetColor(ARGB(90, 80, 79, 78));
		}
		if (m_LearningRecipeTitel)
		{
			m_LearningRecipeTitel.SetText("#STR_DRL_LEARNED");
		}
		if (m_LearningLockIcon)
		{
			UpdateLockIconVisual(m_LearningLockIcon, 2);
		}

		for (int vi = 0; vi < m_VariationWidgets.Count(); vi++)
		{
			Widget varW = m_VariationWidgets[vi];
			if (varW)
			{
				CheckBoxWidget cb = CheckBoxWidget.Cast(varW.FindAnyWidget("StudyVariationCheckBox"));
				if (cb)
				{
					cb.Enable(false);
				}
				ButtonWidget b = ButtonWidget.Cast(varW);
				if (!b)
				{
					b = ButtonWidget.Cast(varW.FindAnyWidget("StudyVariationButton"));
				}
				if (b)
				{
					b.Enable(false);
				}
			}
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_STUDY_RECIPE, new Param2<int, int>(m_SelectedRecipe.RecipeId, m_SelectedVariationIndex), true, null);
		}
	}

	void DRL_ClientDeductInventoryItems(string clsName, int count)
	{
		if (clsName == "" || count <= 0)
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetInventory())
		{
			return;
		}

		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		string targetClsLower = clsName;
		targetClsLower.ToLower();

		int leftToConsume = count;

		for (int i = items.Count() - 1; i >= 0; i--)
		{
			if (leftToConsume <= 0)
			{
				break;
			}

			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib || ib.IsRuined())
			{
				continue;
			}

			string t = ib.GetType();
			t.ToLower();
			if (t == targetClsLower || ib.IsKindOf(clsName))
			{
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
					{
						q = 1;
					}

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
	}

	void OnStudyResponse(bool success, int recipeId, array<int> learnedIds)
	{
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
				{
					soundName = lcfg.LearningSoundSet;
				}
			}

			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player)
			{
				EffectSound s = SEffectManager.PlaySoundOnObject(soundName, player);
				if (s)
				{
					s.SetSoundAutodestroy(true);
				}
			}

			BuildCategories();
			BuildRecipeList();
		}
		else
		{
			if (m_SelectedRecipe)
			{
				SelectRecipe(m_SelectedRecipe);
			}
			UpdateStudyButtonState();
		}
	}

	int GetPlayerInventoryCount(string clsName)
	{
		if (clsName == "")
		{
			return 0;
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			return 0;
		}

		array<EntityAI> items = new array<EntityAI>();
		if (player.GetInventory())
		{
			player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}

		string targetLower = clsName;
		targetLower.ToLower();

		int count = 0;
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib || ib.IsRuined())
			{
				continue;
			}

			InventoryLocation il = new InventoryLocation();
			if (ib.GetInventory() && ib.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == player)
					{
						string ibType = ib.GetType();
						ibType.ToLower();
						if (ibType != targetLower && !ib.IsKindOf(clsName))
						{
							continue;
						}
					}
				}
			}

			string t = ib.GetType();
			t.ToLower();
			if (t == targetLower || ib.IsKindOf(clsName))
			{
				if (ib.IsKindOf("Ammunition_Base"))
				{
					Magazine mgzAmmo = Magazine.Cast(ib);
					if (mgzAmmo)
					{
						count += mgzAmmo.GetAmmoCount();
					}
					else
					{
						count += ib.GetQuantity();
					}
				}
				else if (ib.HasQuantity() && !ib.IsLiquidContainer())
				{
					int q = ib.GetQuantity();
					if (q <= 0 && ib.GetQuantityMax() <= 0)
					{
						q = 1;
					}
					count += q;
				}
				else
				{
					count += 1;
				}
			}
		}

		return count;
	}

	string GetItemDisplayName(string clsName)
	{
		if (clsName == "")
		{
			return "";
		}

		string cfgPath = "CfgVehicles " + clsName + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			string name;
			GetGame().ConfigGetText(cfgPath, name);
			return Widget.TranslateString(name);
		}

		cfgPath = "CfgWeapons " + clsName + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			string wName;
			GetGame().ConfigGetText(cfgPath, wName);
			return Widget.TranslateString(wName);
		}

		cfgPath = "CfgMagazines " + clsName + " displayName";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			string mName;
			GetGame().ConfigGetText(cfgPath, mName);
			return Widget.TranslateString(mName);
		}

		return clsName;
	}

	void ClearChildren(Widget parent)
	{
		if (!parent)
		{
			return;
		}

		Widget child = parent.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_BtnClose || (w && w.GetName() == "ButtonOut"))
		{
			GetGame().GetUIManager().Back();
			return true;
		}

		if (w == m_StudyButton || (w && w.GetName() == "StudyButton"))
		{
			TryStudySelected();
			return true;
		}

		Widget cur = w;
		while (cur)
		{
			if (cur == m_StudiedCategoryButton || cur.GetName() == "StudiedCategoryButton")
			{
				SelectStudiedCategory();
				return true;
			}

			if (cur.GetName() == "CategorySelectionButton")
			{
				int catId = cur.GetUserID();
				SelectCategory(catId);
				return true;
			}

			if (cur.GetName() == "LearningCraftButton" || cur.GetName() == "ReqButton")
			{
				DRL_Craft_Recipe r;
				cur.GetUserData(r);
				if (!r)
				{
					int recId = cur.GetUserID();
					if (g_Craft_Config)
					{
						r = g_Craft_Config.FindById(recId);
					}
				}
				if (r)
				{
					SelectRecipe(r);
				}
				return true;
			}

			if (cur.GetName() == "StudyVariationButton" || cur.GetName() == "StudyVariationCheckBox")
			{
				int varIdx = cur.GetUserID();
				SelectVariation(varIdx);
				return true;
			}

			cur = cur.GetParent();
		}

		return super.OnClick(w, x, y, button);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_SearchEditBox)
		{
			BuildRecipeList();
			return true;
		}

		return super.OnChange(w, x, y, finished);
	}
}
