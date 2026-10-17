class DRL_CraftMenuUI extends UIScriptedMenu
{
	protected Widget 				m_Root;

	protected EntityAI 				m_Workbench;
	protected string   				m_WorkbenchCls;

	protected ref array<int> 		m_VisibleCategories;

	protected TextWidget 			m_PlayerNameW;
	protected TextWidget 			m_WBNameText;

	protected MultilineTextWidget 	m_WarningTextW;

	protected TextWidget 			m_SlotInfoW;
	protected ref Timer  			m_SlotInfoTimer;

	protected Widget     			m_CategoryListWrap;
	protected TextWidget 			m_CategoryTitle;
	protected int        			m_CurrentCategoryIdx = -1;

	protected Widget 				m_RightPanel;
	protected EditBoxWidget 		m_SearchBox;
	const string 					SEARCH_DEFAULT_STR = "#STR_SEARCH_RECIPE";

	protected string CleanSearchQuery(string text)
	{
		return DRL_CraftUIHelper.CleanSearchQuery(text, SEARCH_DEFAULT_STR);
	}

	protected bool IsSearchDefaultText(string text)
	{
		return DRL_CraftUIHelper.IsSearchDefaultText(text, SEARCH_DEFAULT_STR);
	}

	protected Widget 				m_RecipesListSpacer;
	protected ButtonWidget 			m_BtnClose;
	protected Widget 				m_LearningButtonWidget;
	protected ButtonWidget 			m_LearningBtn;
	protected ref DRL_CraftQueuePopulator m_QueuePopulator;
	protected ref DRL_CraftRequirementsController m_RequirementsController;
	protected ref DRL_CraftVariationsController   m_VariationsController;
	protected ref DRL_CraftAttachmentsController  m_AttachmentsController;

	DRL_Craft_Recipe GetSelectedRecipe()
	{
		return m_SelectedRecipe;
	}

	array<ref DRL_VariationClickInfo> GetActiveVariationClickInfos() { return m_ActiveVariationClickInfos; }

	void SetActiveVariationComponentIndex(int idx) { m_ActiveVariationComponentIndex = idx; }

	EntityAI GetContextWorkbench() { return m_ContextWorkbench; }

	EntityAI GetMainPreview() { return m_DRL_MainPreview; }

	void SetMainPreview(EntityAI p) { m_DRL_MainPreview = p; }

	ItemPreviewWidget GetItemPreviewWidget0() { return m_ItemPreviewWidget0; }

	string GetSelectedResultClass() { return m_SelectedResultClass; }

	vector GetPreviewOrientation() { return m_PreviewOrientation; }

	DRL_Craft_Config GetCraft_Config() { return g_Craft_Config; }

	DRL_CraftAttachmentsController GetAttachmentsController() { return m_AttachmentsController; }

	array<int> GetSelectedAttachedRecipeIds()
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.GetSelectedAttachedRecipeIds();
		return m_SelectedAttachedRecipeIds;
	}

	array<int> GetSelectedAttachedRecipeIdsFlattened()
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.GetSelectedAttachedRecipeIdsFlattened();
		return GetSelectedAttachedRecipeIds();
	}

	static bool DRL_ClientHasLearnedRecipe(int recipeId)
	{
		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.IsLearningEnabled())
		{
			return true;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg || !lcfg.RecipeRequiresLearning(recipeId))
		{
			return true;
		}

		return DRL_LearningBridge.HasLearnedRecipe(recipeId);
	}

	protected WrapSpacerWidget 		m_SelectCraftListWrap;
	protected Widget           		m_SelectCraftPanel;
	protected string           		m_SelectedResultClass;

	protected ref array<Widget>     m_SelectRows = new array<Widget>();

	protected Widget               	m_IngrDetailPanel;
	protected TextWidget           	m_IngrDetailName;
	protected MultilineTextWidget  	m_IngrDetailDesc;

	protected Widget               	m_VariationIngridientListWidget;
	protected WrapSpacerWidget        	m_VariationCraftIngridientListWrap;
	protected int                  	m_ActiveVariationComponentIndex = -1;

	protected ref map<int, string> 	m_SelectedComponentVariations = new map<int, string>();

	protected TextWidget 			m_StatsTextW;

	static string ResolveImagePath(string path)
	{
		return DRL_CraftUIHelper.ResolveImagePath(path);
	}

	protected Widget 				m_SelectCraftRecipeTitle;
	protected Widget 				m_AttachTitle;

	protected Widget 				m_AtachIngridientListWrap;
	protected EntityAI 				m_ContextWorkbench;

	protected ref array<float> 		m_AttachDamages;
	protected ref array<string>   	m_AttachTokens;
	protected ref array<string>   	m_AttachPreviewClasses;
	protected ref array<EntityAI> 	m_AttachPresentItems;

	protected Widget 				m_AttachmentIconsWrap;
	protected ImageWidget 			m_ButtonCraftIcon;

	protected TextWidget 			m_CraftCountText;
	protected TextWidget 			m_ButtonCraftText;

	protected ButtonWidget 			m_CraftCountBtnPlus;
	protected ButtonWidget 			m_CraftCountBtnMin;
	protected ButtonWidget 			m_CraftButton;

	protected Widget 				m_AdditionalTasksWidget;
	protected ScrollWidget			m_AdditionalTasksCraftScroll;
	protected WrapSpacerWidget		m_AdditionalTasksCraftWrap;
	protected ButtonWidget 			m_AdditionalTasksButton;
	protected Widget 				m_AdditionalTasksButtonPanel;

	protected Widget             	m_ReCraftPanel;
	protected Widget             	m_ReCraftListWrap;
	protected RichTextWidget     	m_ReCraftRecipeDetailText;

	protected ButtonWidget       	m_ReCraftButton;
	protected ProgressBarWidget  	m_ReCraftPB;
	protected Widget             	m_ReCraftButtonPanel;
	protected TextWidget         	m_ReCraftButtonText;
	protected ImageWidget        	m_ReCraftButtonIcon;

	protected ref Timer          	m_ReCraftProgressTimer;
	protected bool               	m_ReCraftActive;
	protected float              	m_ReCraftElapsed;
	protected float              	m_ReCraftDuration;

	protected ProgressBarWidget 	m_CraftPB;
	protected ref Timer 			m_CraftProgressTimer;
	protected ref Timer 			m_CraftTimer;
	protected int 					m_RemainTime;

	protected Widget 				m_CraftProcessWidget;
	protected Widget 				m_CraftProcessGridSpacer;

	protected ref array<ref DRL_CraftProcessEntryWidget> m_ProcessWidgets = new array<ref DRL_CraftProcessEntryWidget>();
	protected ref Timer 			m_ProcessUpdateTimer;

	protected Widget 				m_CraftButtonPanel;
	protected int    				m_CraftBtnColorDefault;

	protected float             	m_CraftElapsed;
	protected float             	m_CraftDuration;
	protected bool              	m_CraftActive;

	protected ItemPreviewWidget 	m_ItemPreviewWidget0;

	protected int   				m_PrevMouseX;
	protected int   				m_PrevMouseY;
	protected vector 				m_PreviewOrientation;
	protected EntityAI 				m_DRL_MainPreview;

	protected ref DRL_Craft_Recipe 	m_SelectedRecipe;

	protected EffectSound 			m_CraftSound;

	protected ImageWidget           m_FonImage;
	protected ImageWidget           m_CraftFonImage;

	static const int COLOR_CRAFT_FON_DEFAULT = ARGB(255, 255, 255, 255);
	static const int COLOR_CRAFT_FON_ACTIVE  = ARGB(255, 152, 72, 72);

	protected string CATEGORY_TEMPLATE = "DRL_CraftMenu/UI/layout/Category.layout";
	protected string RECRAFT_TEMPLATE = "DRL_CraftMenu/UI/layout/ReCraft.layout";
	protected string RECIPE_TEMPLATE = "DRL_CraftMenu/UI/layout/Recipe.layout";
	protected string INGR_TEMPLATE = "DRL_CraftMenu/UI/layout/Ingredient.layout";
	protected string VARIATION_HEADER_TEMPLATE = "DRL_CraftMenu/UI/layout/VariationHeader.layout";
	protected string VARIATION_PART_SECTION_TEMPLATE = "DRL_CraftMenu/UI/layout/VariationPartSection.layout";
	protected string REQ_HEADER_TEMPLATE = "DRL_CraftMenu/UI/layout/RequirementHeader.layout";
	protected string REQ_ITEM_TEMPLATE = "DRL_CraftMenu/UI/layout/RequirementItem.layout";
	protected string REQ_INFO_TEMPLATE = "DRL_CraftMenu/UI/layout/RequirementInfo.layout";
	static const string DRL_ATTACHMENT_CELL = "DRL_CraftMenu/UI/layout/Attachment.layout";

	protected ref map<string, EntityAI> m_PreviewCache = new map<string, EntityAI>();

	protected ref array<ItemPreviewWidget> m_AttachedPreviews = new array<ItemPreviewWidget>();

	protected Widget 					m_RecipesDopVarListWidget;
	protected WrapSpacerWidget 			m_RecipesDopVarListWrap;

	protected ref array<ref DRL_RecipeVarWidget> m_DopWidgets = new array<ref DRL_RecipeVarWidget>();

	protected ref map<Widget, DRL_RecipeVarWidget> m_DopWidgetMap = new map<Widget, DRL_RecipeVarWidget>();

	protected ref array<int> 			m_SelectedAttachedRecipeIds = new array<int>();

	protected ref array<ref DRL_Craft_Component> m_ActiveDisplayComponents = new array<ref DRL_Craft_Component>();

	protected ref array<ref DRL_CraftMergedComponent> m_ActiveDisplayMergedComponents = new array<ref DRL_CraftMergedComponent>();
	protected ref DRL_CraftMergedComponent m_ActiveVariationMergedComponent;

	protected ref array<ref DRL_VariationClickInfo> m_ActiveVariationClickInfos = new array<ref DRL_VariationClickInfo>();

	void AddAttachedPreview(ItemPreviewWidget prev)
	{
		if (prev && m_AttachedPreviews)
		{
			m_AttachedPreviews.Insert(prev);
		}
	}

	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/drl_craft_manu.layout", null);

		m_FonImage = ImageWidget.Cast(m_Root.FindAnyWidget("FonImageWidget"));
		m_CraftFonImage = ImageWidget.Cast(m_Root.FindAnyWidget("CraftFonImageWidget"));
		m_WBNameText = TextWidget.Cast(m_Root.FindAnyWidget("WB_Name"));

		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg && cfg.Settings)
		{
			if (m_FonImage)
			{
				string fonPath = cfg.Settings.FonImagePath;
				if (fonPath != "")
				{
					m_FonImage.LoadImageFile(0, ResolveImagePath(fonPath));
					m_FonImage.SetImage(0);
				}
			}

			if (m_CraftFonImage)
			{
				string craftFonPath = cfg.Settings.CraftFonImagePath;
				if (craftFonPath != "")
				{
					m_CraftFonImage.LoadImageFile(0, ResolveImagePath(craftFonPath));
					m_CraftFonImage.SetImage(0);
				}
			}
		}

		m_SelectCraftPanel = m_Root.FindAnyWidget("SelectCraftPanelWidget");
		m_SelectCraftListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("SelectCraftListWrap"));

		m_RecipesDopVarListWidget = m_Root.FindAnyWidget("RecipesDopVarListWidget");
		m_RecipesDopVarListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("RecipesDopVarListWrap"));
		if (m_RecipesDopVarListWrap)
		{
			ClearChildren(m_RecipesDopVarListWrap);
		}
		if (m_RecipesDopVarListWidget)
		{
			m_RecipesDopVarListWidget.Show(false);
		}

		m_PlayerNameW = TextWidget.Cast(m_Root.FindAnyWidget("PlayerName"));
		m_SlotInfoW = TextWidget.Cast(m_Root.FindAnyWidget("SlotInfoText"));

		UpdatePlayerName();
		UpdateSlotInfo();
		UpdateWBName();

		m_StatsTextW = TextWidget.Cast(m_Root.FindAnyWidget("StatsText"));

		if (!m_SlotInfoTimer)
			m_SlotInfoTimer = new Timer(CALL_CATEGORY_GUI);
		m_SlotInfoTimer.Run(1.0, this, "UpdateSlotInfo", null, true);

		m_ItemPreviewWidget0 = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));

		m_DRL_MainPreview = null;

		m_BtnClose = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonOut"));
		m_LearningButtonWidget = m_Root.FindAnyWidget("LearningButtonWidget");
		m_LearningBtn = ButtonWidget.Cast(m_Root.FindAnyWidget("LearningButton"));
		if (m_LearningButtonWidget)
		{
			bool showLearning = false;
			if (cfg && cfg.IsLearningEnabled() && cfg.IsFreeLearningMenuEnabled())
			{
				showLearning = true;
			}
			m_LearningButtonWidget.Show(showLearning);
		}
		m_RecipesListSpacer = m_Root.FindAnyWidget("SpacerAllRecipesListWidget");
		m_SearchBox = EditBoxWidget.Cast(m_Root.FindAnyWidget("SearchEditBox"));
		if (m_SearchBox)
		{
			m_SearchBox.SetText(SEARCH_DEFAULT_STR);
		}
		m_RightPanel = m_Root.FindAnyWidget("RigthPanelWidget");
		if (m_RightPanel)
			m_RightPanel.Show(false);

		m_SelectCraftRecipeTitle = m_Root.FindAnyWidget("SelectCraftRecipeTitelWidget");
		m_AttachTitle = m_Root.FindAnyWidget("AtachIngridientListWidget");

		m_AtachIngridientListWrap = m_Root.FindAnyWidget("AtachIngridientListWrap");

		m_AttachmentIconsWrap = m_RightPanel.FindAnyWidget("AttachmentIconsWrap");

		m_ButtonCraftText = TextWidget.Cast(m_RightPanel.FindAnyWidget("ButtonCraftText"));
		m_ButtonCraftIcon = ImageWidget.Cast(m_RightPanel.FindAnyWidget("ButtonImageWidget"));

		m_IngrDetailPanel = m_Root.FindAnyWidget("CraftIngridientDetail");
		m_IngrDetailName = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetailText"));
		m_IngrDetailDesc = MultilineTextWidget.Cast(m_Root.FindAnyWidget("IngridientDetailMultiline"));
		if (m_IngrDetailPanel)
			m_IngrDetailPanel.Show(false);

		Widget ingrWrap = m_Root.FindAnyWidget("CraftIngridientListWrap");

		m_CraftCountText = TextWidget.Cast(m_RightPanel.FindAnyWidget("CraftCountText"));
		m_CraftCountBtnPlus = ButtonWidget.Cast(m_Root.FindAnyWidget("CraftCountButtonPlus"));
		m_CraftCountBtnMin = ButtonWidget.Cast(m_Root.FindAnyWidget("CraftCountButtonMin"));

		m_ReCraftPanel = m_Root.FindAnyWidget("ReCraftRecipeDetail");
		m_ReCraftListWrap = m_Root.FindAnyWidget("ReCraftIngridientListWrap");

		m_ReCraftButton = ButtonWidget.Cast(m_Root.FindAnyWidget("ReCraftButton"));
		m_ReCraftPB = ProgressBarWidget.Cast(m_Root.FindAnyWidget("ReCraftButtonProgressBar"));
		m_ReCraftButtonPanel = m_Root.FindAnyWidget("ReButtonCraftPanelWidget");
		m_ReCraftButtonText = TextWidget.Cast(m_Root.FindAnyWidget("ReButtonCraftText"));
		m_ReCraftButtonIcon = ImageWidget.Cast(m_Root.FindAnyWidget("ReButtonImageWidget"));
		m_ReCraftRecipeDetailText = RichTextWidget.Cast(m_Root.FindAnyWidget("ReCraftRecipeDetailText"));
		if (m_ReCraftPB)
		{
			m_ReCraftPB.SetCurrent(0);
			m_ReCraftPB.Show(true);
		}
		if (m_ReCraftPanel)
			m_ReCraftPanel.Show(false);

		m_WarningTextW = MultilineTextWidget.Cast(m_Root.FindAnyWidget("WarningText"));
		if (m_WarningTextW)
		{
			m_WarningTextW.SetText("#STR_DRL_WARNING_TEXT");
			m_WarningTextW.SetColor(ARGB(200, 120, 30, 30));
			m_WarningTextW.Show(true);
		}

		m_CraftButton = ButtonWidget.Cast(m_Root.FindAnyWidget("CraftButton"));
		m_CraftPB = ProgressBarWidget.Cast(m_Root.FindAnyWidget("CraftButtonProgressBar"));
		if (m_CraftPB)
		{
			m_CraftPB.SetCurrent(0);
			m_CraftPB.Show(true);
		}

		m_CraftButtonPanel = m_Root.FindAnyWidget("ButtonCraftPanelWidget");
		if (m_CraftButtonPanel)
		{
			m_CraftBtnColorDefault = m_CraftButtonPanel.GetColor();
		}

		m_VariationIngridientListWidget = m_Root.FindAnyWidget("VariationIngridientListWidget");
		m_VariationCraftIngridientListWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("VariationCraftIngridientListWrap"));
		if (m_VariationIngridientListWidget)
		{
			m_VariationIngridientListWidget.Show(false);
		}

		m_AdditionalTasksWidget = m_Root.FindAnyWidget("AdditionalTasksCraftWidget");
		m_AdditionalTasksCraftScroll = ScrollWidget.Cast(m_Root.FindAnyWidget("AdditionalTasksCraftScrollWidget"));
		m_AdditionalTasksCraftWrap = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("AdditionalTasksCraftWrap"));
		if (m_AdditionalTasksWidget)
		{
			m_AdditionalTasksWidget.Show(false);
		}
		m_AdditionalTasksButton = ButtonWidget.Cast(m_Root.FindAnyWidget("AdditionalTasksButton"));
		m_AdditionalTasksButtonPanel = m_Root.FindAnyWidget("AdditionalTasksButtonPanel");
		if (m_AdditionalTasksButton)
		{
			m_AdditionalTasksButton.Show(false);
		}

		m_RequirementsController = new DRL_CraftRequirementsController(this, m_AdditionalTasksCraftWrap, m_AdditionalTasksCraftScroll);
		m_VariationsController = new DRL_CraftVariationsController(this, m_VariationIngridientListWidget, m_VariationCraftIngridientListWrap);
		m_AttachmentsController = new DRL_CraftAttachmentsController(this, m_RecipesDopVarListWidget, m_RecipesDopVarListWrap, m_AtachIngridientListWrap);

		m_CraftProcessWidget = m_Root.FindAnyWidget("CraftProcessWidget");
		m_CraftProcessGridSpacer = m_Root.FindAnyWidget("CraftProcessGridSpacer");
		if (m_CraftProcessGridSpacer)
		{
			ClearChildren(m_CraftProcessGridSpacer);
		}

		m_CategoryListWrap = m_Root.FindAnyWidget("SpacerCategoryListWidget");
		m_CategoryTitle = TextWidget.Cast(m_Root.FindAnyWidget("AllCategoryTitel"));

		BuildCategoriesPanel();
		SelectCategory(FindDefaultCategory());

		return m_Root;
	}

	void DRL_CraftMenuUI(EntityAI workbench = null)
	{
		m_Workbench = workbench;
		if (m_Workbench)
		{
			m_WorkbenchCls = m_Workbench.GetType();
			m_WorkbenchCls.Trim();
		}
		m_QueuePopulator = new DRL_CraftQueuePopulator(this);
	}

	override void OnShow()
	{
		super.OnShow();
		UpdatePlayerName();
		UpdateWBName();
		UpdateSlotInfo();

		if (m_SearchBox)
		{
			m_SearchBox.SetText(SEARCH_DEFAULT_STR);
		}
		DeselectRecipe();

		if (!m_VisibleCategories || m_VisibleCategories.Count() == 0)
		{
			BuildCategoriesPanel();
			SelectCategory(FindDefaultCategory());
		}
		else
		{
			DRL_Craft_Category curCat = GetCurrentCategory();
			if (curCat && curCat.ShowOnlyAvailableCraft == 1)
			{
				RebuildRecipesList_CurrentCategory();
			}
			else
			{
				RefreshRecipeListColors();
			}
		}

		MissionGameplay m = MissionGameplay.Cast(GetGame().GetMission());
		if (m && m.GetHud())
		{
			m.GetHud().ShowHudUI(false);
			m.GetHud().ShowQuickbarUI(false);
		}

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);

		GetGame().GetInput().ChangeGameFocus(1);
		SetFocus(m_Root);

		DRL_CraftProcessBridge.s_OnProcessesSync.Insert(OnProcessSyncReceived);
		DRL_CraftProcessBridge.s_OnProcessStarted.Insert(OnProcessStartedReceived);
		DRL_CraftProcessBridge.s_OnProcessFinished.Insert(OnProcessFinishedReceived);

		RequestActiveCraftProcesses();

		if (!m_ProcessUpdateTimer)
		{
			m_ProcessUpdateTimer = new Timer(CALL_CATEGORY_GUI);
		}
		m_ProcessUpdateTimer.Run(0.05, this, "OnProcessTimerTick", null, true);
	}

	override void OnHide()
	{
		super.OnHide();

		DRL_CraftProcessBridge.s_OnProcessesSync.Remove(OnProcessSyncReceived);
		DRL_CraftProcessBridge.s_OnProcessStarted.Remove(OnProcessStartedReceived);
		DRL_CraftProcessBridge.s_OnProcessFinished.Remove(OnProcessFinishedReceived);

		if (m_ProcessUpdateTimer)
		{
			m_ProcessUpdateTimer.Stop();
		}
		ClearProcessWidgets();

		GetGame().GetMission().PlayerControlEnable(true);
		MissionGameplay m = MissionGameplay.Cast(GetGame().GetMission());
		if (m && m.GetHud())
		{
			m.GetHud().ShowHudUI(true);
			m.GetHud().ShowQuickbarUI(true);
		}
		GetGame().GetInput().ResetGameFocus();

		if (m_SlotInfoTimer)
			m_SlotInfoTimer.Stop();

		if (m_QueuePopulator)
			m_QueuePopulator.Stop();

		if (m_AdditionalTasksWidget)
			m_AdditionalTasksWidget.Show(false);

		DRL_CleanupPreview();
		DRL_StopCraftSound();
	}

	void ~DRL_CraftMenuUI()
	{
		if (m_QueuePopulator)
		{
			m_QueuePopulator.Stop();
		}
		if (m_ProcessUpdateTimer)
		{
			m_ProcessUpdateTimer.Stop();
		}
		ClearProcessWidgets();
		DRL_StopCraftSound();
		DRL_CleanupPreview();
	}

	void RequestActiveCraftProcesses()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
		{
			return;
		}

		int netLow = 0, netHigh = 0;
		if (m_ContextWorkbench)
		{
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		}

		p.RPCSingleParam(DRL_CraftRPC.CTS_REQUEST_ACTIVE_PROCESSES, new Param2<int, int>(netLow, netHigh), true, null);
	}

	void ClearProcessWidgets()
	{
		if (m_ProcessWidgets)
		{
			for (int i = 0; i < m_ProcessWidgets.Count(); i++)
			{
				DRL_CraftProcessEntryWidget ew = m_ProcessWidgets[i];
				if (ew)
				{
					ew.Destroy();
				}
			}
			m_ProcessWidgets.Clear();
		}

		if (m_CraftProcessGridSpacer)
		{
			ClearChildren(m_CraftProcessGridSpacer);
		}

		if (m_CraftPB)
		{
			m_CraftPB.SetCurrent(0);
		}
	}

	void OnProcessSyncReceived()
	{
		ClearProcessWidgets();

		if (!m_CraftProcessGridSpacer)
		{
			UpdateCraftButtonState();
			return;
		}

		array<ref DRL_CraftProcessSyncInfo> activeList = DRL_CraftProcessBridge.GetActiveProcesses();
		if (activeList)
		{
			for (int i = 0; i < activeList.Count(); i++)
			{
				DRL_CraftProcessSyncInfo info = activeList.Get(i);
				if (info)
				{
					DRL_CraftProcessEntryWidget newEntry = new DRL_CraftProcessEntryWidget(m_CraftProcessGridSpacer, info, this);
					m_ProcessWidgets.Insert(newEntry);
				}
			}

			if (!m_ContextWorkbench)
			{
				for (int j = 0; j < activeList.Count(); j++)
				{
					DRL_CraftProcessSyncInfo runInfo = activeList.Get(j);
					if (runInfo && runInfo.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
					{
						if (!m_CraftSound || !m_CraftSound.IsSoundPlaying())
						{
							DRL_Craft_Config cfg = GetCraft_Config();
							if (cfg)
							{
								DRL_Craft_Recipe runRecipe = cfg.FindById(runInfo.m_RecipeId);
								if (runRecipe)
								{
									DRL_PlayCraftSoundForRecipe(runRecipe);
								}
							}
						}
						break;
					}
				}
			}
		}

		UpdateCraftButtonState();
	}

	void OnProcessStartedReceived(DRL_CraftProcessSyncInfo syncInfo)
	{
		if (!syncInfo)
		{
			return;
		}

		for (int i = m_ProcessWidgets.Count() - 1; i >= 0; i--)
		{
			if (m_ProcessWidgets[i] && m_ProcessWidgets[i].GetProcessId() == syncInfo.m_ProcessId)
			{
				m_ProcessWidgets[i].Destroy();
				m_ProcessWidgets.Remove(i);
			}
		}

		if (m_CraftProcessGridSpacer)
		{
			DRL_CraftProcessEntryWidget newEntry = new DRL_CraftProcessEntryWidget(m_CraftProcessGridSpacer, syncInfo, this);
			m_ProcessWidgets.Insert(newEntry);
		}

		if (!m_ContextWorkbench && syncInfo.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
		{
			DRL_Craft_Config cfgSync = GetCraft_Config();
			if (cfgSync)
			{
				DRL_Craft_Recipe rStart = cfgSync.FindById(syncInfo.m_RecipeId);
				if (rStart)
				{
					DRL_PlayCraftSoundForRecipe(rStart);
				}
			}
		}

		UpdateCraftButtonState();
	}

	void OnProcessFinishedReceived(string processId, bool cancelled)
	{
		for (int i = m_ProcessWidgets.Count() - 1; i >= 0; i--)
		{
			DRL_CraftProcessEntryWidget ew = m_ProcessWidgets[i];
			if (ew && ew.GetProcessId() == processId)
			{
				ew.Destroy();
				m_ProcessWidgets.Remove(i);
			}
		}

		DRL_StopCraftSound();

		if (m_SelectedRecipe)
		{
			FillRecipeDetails(m_SelectedRecipe);
		}
		RefreshRecipeListColors();
		UpdateCraftButtonState();
	}

	void OnProcessTimerTick()
	{
		if (!m_ProcessWidgets || m_ProcessWidgets.Count() == 0)
		{
			if (m_CraftPB)
			{
				m_CraftPB.SetCurrent(0);
			}
			return;
		}

		for (int i = 0; i < m_ProcessWidgets.Count(); i++)
		{
			DRL_CraftProcessEntryWidget ew = m_ProcessWidgets[i];
			if (ew)
			{
				ew.UpdateTick(0.05);
			}
		}

		UpdateCraftButtonProgress();
	}

	void UpdateCraftButtonProgress()
	{
		if (!m_CraftPB)
		{
			return;
		}

		DRL_CraftProcessEntryWidget activeEntry = GetProcessEntryForCurrentRecipe();
		if (activeEntry)
		{
			m_CraftPB.SetCurrent(activeEntry.GetCurrentProgressPercent());
			m_CraftPB.Show(true);
		}
		else
		{
			m_CraftPB.SetCurrent(0);
		}
	}

	DRL_CraftProcessEntryWidget GetProcessEntryForCurrentRecipe()
	{
		if (!m_SelectedRecipe || !m_ProcessWidgets)
		{
			return null;
		}

		for (int i = 0; i < m_ProcessWidgets.Count(); i++)
		{
			DRL_CraftProcessEntryWidget ew = m_ProcessWidgets[i];
			if (ew && ew.GetRecipeId() == m_SelectedRecipe.RecipeId)
			{
				return ew;
			}
		}
		return null;
	}

	void CancelActiveCraftProcess(string processId)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || processId == "")
		{
			return;
		}

		GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CANCEL_CRAFT, new Param1<string>(processId), true, null);
		DRL_StopCraftSound();
	}

	protected bool HandleCraftProcessItemClick(Widget w)
	{
		Widget cur = w;
		bool isCancel = false;
		while (cur)
		{
			if (cur.GetName() == "CancelButton")
			{
				isCancel = true;
			}

			Class ud = null;
			cur.GetUserData(ud);
			DRL_CraftProcessEntryWidget entry = DRL_CraftProcessEntryWidget.Cast(ud);
			if (entry)
			{
				if (isCancel)
				{
					CancelActiveCraftProcess(entry.GetProcessId());
					return true;
				}

				int recId = entry.GetRecipeId();
				DRL_Craft_Config cfg = GetCraft_Config();
				if (cfg)
				{
					DRL_Craft_Recipe targetR = cfg.FindById(recId);
					if (targetR)
					{
						SelectRecipeDirectly(targetR);
						return true;
					}
				}
				return true;
			}
			cur = cur.GetParent();
		}
		return false;
	}

	protected bool IsWidgetChildOf(Widget parent, Widget child)
	{
		if (!parent || !child)
			return false;
		Widget cur = child;
		int depth = 0;
		while (cur && depth < 40)
		{
			if (cur == parent)
				return true;
			cur = cur.GetParent();
			depth++;
		}
		return false;
	}

	void UpdateUnderlyingOverlaysState()
	{
		bool shouldHideDop = false;
		if (m_AdditionalTasksWidget && m_AdditionalTasksWidget.IsVisible())
		{
			shouldHideDop = true;
		}
		if (m_VariationIngridientListWidget && m_VariationIngridientListWidget.IsVisible())
		{
			shouldHideDop = true;
		}

		if (shouldHideDop)
		{
			if (m_RecipesDopVarListWidget)
			{
				m_RecipesDopVarListWidget.Show(false);
			}
			if (m_RecipesDopVarListWrap)
			{
				m_RecipesDopVarListWrap.Show(false);
			}
		}
		else
		{
			bool hasAttached = false;
			if (m_SelectedRecipe && m_SelectedRecipe.HasAttachedRecipes())
			{
				hasAttached = true;
			}

			if (m_RecipesDopVarListWidget)
			{
				m_RecipesDopVarListWidget.Show(hasAttached);
			}
			if (m_RecipesDopVarListWrap)
			{
				m_RecipesDopVarListWrap.Show(hasAttached);
			}
		}
	}

	void HideVariationList()
	{
		if (m_VariationIngridientListWidget && m_VariationIngridientListWidget.IsVisible())
		{
			m_VariationIngridientListWidget.Show(false);
		}
		m_ActiveVariationMergedComponent = null;
		m_ActiveVariationComponentIndex = -1;
		UpdateUnderlyingOverlaysState();
	}

	void HideAdditionalTasks()
	{
		if (m_AdditionalTasksWidget && m_AdditionalTasksWidget.IsVisible())
		{
			m_AdditionalTasksWidget.Show(false);
		}
		UpdateUnderlyingOverlaysState();
	}

	void DeselectRecipe()
	{
		m_SelectedRecipe = null;
		m_SelectedResultClass = "";
		m_ActiveVariationComponentIndex = -1;
		if (m_SelectedComponentVariations)
		{
			m_SelectedComponentVariations.Clear();
		}
		HideVariationList();
		HideAdditionalTasks();
		if (m_RightPanel)
		{
			m_RightPanel.Show(false);
		}
		if (m_IngrDetailPanel)
		{
			m_IngrDetailPanel.Show(false);
		}
		if (m_DRL_MainPreview)
		{
			GetGame().ObjectDelete(m_DRL_MainPreview);
			m_DRL_MainPreview = null;
		}
		if (m_ItemPreviewWidget0)
		{
			m_ItemPreviewWidget0.SetItem(null);
		}
		if (m_AdditionalTasksButton)
		{
			m_AdditionalTasksButton.Show(false);
		}
		if (m_CraftPB)
		{
			m_CraftPB.SetCurrent(0);
		}
		UpdateRecipeSelectionHighlight();
	}

	protected void UpdateOptionalTitles(DRL_Craft_Recipe r)
	{
		bool hasVariants = false;
		bool hasAttach = false;

		if (r)
		{
			if (r.ResultClasses && r.ResultClasses.Count() > 1)
				hasVariants = true;

			if (r.GetAttachReqCount() > 0)
				hasAttach = true;
		}

		if (m_SelectCraftRecipeTitle)
		{
			m_SelectCraftRecipeTitle.Show(hasVariants);
		}

		if (m_AttachTitle)
		{
			m_AttachTitle.Show(hasAttach);
		}
	}

	protected bool DRL_IsFillingByClassUI(string cls)
	{
		return DRL_CraftInventoryHelper.IsFillingByClass(cls);
	}

	protected bool DRL_GetConfigIntForClass(string cls, string prop, out int val)
	{
		val = 0;
		if (cls == "")
			return false;

		string p = "CfgVehicles " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			val = GetGame().ConfigGetInt(p);
			return true;
		}
		p = "CfgMagazines " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			val = GetGame().ConfigGetInt(p);
			return true;
		}
		p = "CfgWeapons " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			val = GetGame().ConfigGetInt(p);
			return true;
		}
		return false;
	}

	protected bool DRL_GetConfigStringForClass(string cls, string prop, out string val)
	{
		val = "";
		if (cls == "")
			return false;

		string p = "CfgVehicles " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			GetGame().ConfigGetText(p, val);
			return true;
		}

		p = "CfgMagazines " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			GetGame().ConfigGetText(p, val);
			return true;
		}

		p = "CfgWeapons " + cls + " " + prop;
		if (GetGame().ConfigIsExisting(p))
		{
			GetGame().ConfigGetText(p, val);
			return true;
		}

		return false;
	}

	void DRL_GetInventorySnapshot(out array<EntityAI> items)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		DRL_CraftInventoryHelper.GetInventorySnapshot(p, m_ContextWorkbench, items);
	}

	protected void UpdateSlotInfo(array<EntityAI> cachedItems = null)
	{
		if (!m_SlotInfoW)
			return;

		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
		{
			m_SlotInfoW.SetText("#STR_DRL_INV_SLOTS_EMPTY");
			return;
		}

		int totalCells = 0;
		int usedCells = 0;
		DRL_CraftInventoryHelper.CalculateInventorySlots(pb, m_ContextWorkbench, totalCells, usedCells, cachedItems);

		int freeCells = Math.Max(totalCells - usedCells, 0);
		m_SlotInfoW.SetText(" #STR_DRL_INV_TOTAL " + totalCells + " #STR_DRL_FREE_ONLY " + freeCells);
	}

	protected void DRL_CleanupPreview()
	{
		if (m_ItemPreviewWidget0)
		{
			m_ItemPreviewWidget0.SetItem(null);
		}

		if (m_Root)
		{
			Widget previewPanel2 = m_Root.FindAnyWidget("PreviewItemWidget");
			if (previewPanel2)
			{
				ItemPreviewWidget ipwR = ItemPreviewWidget.Cast(previewPanel2);
				if (ipwR)
				{
					ipwR.SetItem(null);
				}
			}

			Widget wbPrev = m_Root.FindAnyWidget("WBPreview");
			if (wbPrev)
			{
				ItemPreviewWidget ipwWb = ItemPreviewWidget.Cast(wbPrev);
				if (ipwWb)
				{
					ipwWb.SetItem(null);
				}
			}
		}

		if (m_AttachedPreviews)
		{
			for (int ij = 0; ij < m_AttachedPreviews.Count(); ij++)
			{
				ItemPreviewWidget ipw = m_AttachedPreviews[ij];
				if (ipw)
				{
					ipw.SetItem(null);
				}
			}
			m_AttachedPreviews.Clear();
		}

		if (m_DRL_MainPreview)
		{
			GetGame().ObjectDelete(m_DRL_MainPreview);
			m_DRL_MainPreview = null;
		}

		if (m_PreviewCache)
		{
			DRL_CraftPreviewHelper.ClearCache(m_PreviewCache);
		}
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseMenuSelf();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseMenuSelf();
			return true;
		}
		return super.OnKeyPress(w, x, y, key);
	}

	void CloseMenuSelf()
	{
		DRL_CleanupPreview();

		if (m_CraftTimer)
			m_CraftTimer.Stop();
		if (m_CraftProgressTimer)
			m_CraftProgressTimer.Stop();
		m_CraftActive = false;
		if (m_CraftPB)
			m_CraftPB.SetCurrent(0);

		Widget previewPanel2 = m_Root.FindAnyWidget("PreviewItemWidgetPanel");
		if (previewPanel2)
		{
			previewPanel2.Show(false);
		}

		if (m_ReCraftProgressTimer)
			m_ReCraftProgressTimer.Stop();
		m_ReCraftActive = false;
		UpdateReCraftButtonState();
		if (m_ReCraftPB)
			m_ReCraftPB.SetCurrent(0);

		DRL_StopCraftSound();

		PlayerBase pb2 = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb2)
		{
			pb2.DRL_SetCurrentWorkbench(null);
		}

		DeselectRecipe();
		if (m_VariationCraftIngridientListWrap)
		{
			ClearChildren(m_VariationCraftIngridientListWrap);
		}
		if (m_ActiveVariationClickInfos)
		{
			m_ActiveVariationClickInfos.Clear();
		}
		if (m_SearchBox)
		{
			m_SearchBox.SetText(SEARCH_DEFAULT_STR);
		}

		GetGame().GetUIManager().HideScriptedMenu(this);
	}

	protected void UpdatePlayerName()
	{
		string name = "Player";

		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb)
		{
			PlayerIdentity id = pb.GetIdentity();
			if (id)
				name = id.GetName();
		}

		if (m_PlayerNameW)
		{
			m_PlayerNameW.SetText(name);
		}
	}

	EntityAI GetPreviewEntity(string clsname)
	{
		EntityAI dummy = null;
		if (!m_PreviewCache)
		{
			m_PreviewCache = new map<string, EntityAI>();
		}
		return DRL_CraftPreviewHelper.GetOrCreatePreviewEntity(clsname, dummy, m_PreviewCache);
	}

	void BuildCategoriesPanel()
	{
		if (!m_CategoryListWrap)
			return;

		ClearChildren(m_CategoryListWrap);

		auto cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories)
			return;

		m_VisibleCategories = new array<int>();

		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			if (!CategoryHasAnyRecipe(i))
				continue;

			int visibleIdx = m_VisibleCategories.Count();
			m_VisibleCategories.Insert(i);

			Widget row = GetGame().GetWorkspace().CreateWidgets(CATEGORY_TEMPLATE, m_CategoryListWrap);
			row.SetUserID(visibleIdx);

			ImageWidget icon = ImageWidget.Cast(row.FindAnyWidget("CategoryImagePreview"));
			if (icon)
			{
				if (cfg.Categories[i].Icon != "")
				{
					icon.LoadImageFile(0, ResolveImagePath(cfg.Categories[i].Icon));
					icon.SetImage(0);
				}
			}

			ButtonWidget btn = ButtonWidget.Cast(row.FindAnyWidget("CategorySelectionButtonWidget"));
			if (btn)
				btn.SetUserID(visibleIdx);
		}

		m_CategoryListWrap.Update();
	}

	int FindDefaultCategory()
	{
		auto cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories)
			return -1;
		if (!m_VisibleCategories || m_VisibleCategories.Count() == 0)
			return -1;

		for (int k = 0; k < m_VisibleCategories.Count(); k++)
		{
			int realIdx = m_VisibleCategories[k];
			if (cfg.Categories[realIdx] && cfg.Categories[realIdx].ShowAll == 1)
				return realIdx;
		}

		return m_VisibleCategories[0];
	}

	void SelectCategory(int idx)
	{
		DeselectRecipe();
		HideVariationList();
		HideAdditionalTasks();
		m_CurrentCategoryIdx = idx;

		auto cfg = GetCraft_Config();
		DRL_Craft_Category cat = null;
		if (cfg && idx >= 0 && idx < cfg.Categories.Count())
			cat = cfg.Categories[idx];

		if (m_CategoryTitle)
		{
			string title = "#STR_DRL_ALL_RECIPES";
			if (cat && cat.Name != "")
				title = cat.Name;
			m_CategoryTitle.SetText(title);
		}

		DRL_ApplyFonBackground(cat);
		DRL_ApplyCraftFonBackground(cat);

		RebuildRecipesList_CurrentCategory();
	}

	protected void DRL_ApplyFonBackground(DRL_Craft_Category cat)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.Settings)
		{
			return;
		}

		if (!m_FonImage)
		{
			return;
		}

		string path = cfg.Settings.FonImagePath;

		if (cat)
		{
			if (cat.CategoryFonImagePath != "")
			{
				path = cat.CategoryFonImagePath;
			}
		}

		if (path != "")
		{
			m_FonImage.LoadImageFile(0, ResolveImagePath(path));
			m_FonImage.SetImage(0);
		}
	}

	protected void DRL_ApplyCraftFonBackground(DRL_Craft_Category cat)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.Settings)
		{
			return;
		}

		if (!m_CraftFonImage)
		{
			return;
		}

		string path = cfg.Settings.CraftFonImagePath;

		if (cat)
		{
			if (cat.CategoryCraftFonImagePath != "")
			{
				path = cat.CategoryCraftFonImagePath;
			}
		}

		if (path != "")
		{
			m_CraftFonImage.LoadImageFile(0, ResolveImagePath(path));
			m_CraftFonImage.SetImage(0);
		}

		UpdateCraftFonColor();
	}

	void RebuildRecipesList_CurrentCategory()
	{
		string q = "";
		if (m_SearchBox)
		{
			q = CleanSearchQuery(m_SearchBox.GetText());
			q.ToLower();
		}
		BuildRecipesList(q);
	}

	bool RecipeAllowedByContext(DRL_Craft_Recipe r)
	{
		if (!r)
			return false;
		return r.ShowInFreeMenu == 1;
	}

	DRL_Craft_Category GetCurrentCategory()
	{
		auto cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories)
			return null;
		if (m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg.Categories.Count())
			return cfg.Categories[m_CurrentCategoryIdx];
		return null;
	}

	bool DRL_IsRecipeCraftableNow(DRL_Craft_Recipe r, array<EntityAI> cachedItems = null)
	{
		if (!r)
			return false;
		if (!RecipeAllowedByContext(r))
			return false;
		if (!DRL_ClientHasLearnedRecipe(r.RecipeId))
			return false;
		return true;
	}

	bool RecipePassesCategory(DRL_Craft_Recipe r)
	{
		auto cfg = GetCraft_Config();
		if (!cfg || m_CurrentCategoryIdx < 0 || m_CurrentCategoryIdx >= cfg.Categories.Count())
			return true;

		DRL_Craft_Category cat = cfg.Categories[m_CurrentCategoryIdx];
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

	bool RecipePassesCategoryIdx(DRL_Craft_Recipe r, int catIdx)
	{
		auto cfg = GetCraft_Config();
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
				if (r && r.GetId() == cat.List[i])
					return true;
			return false;
		}
		return true;
	}

	bool CategoryHasAnyRecipe(int catIdx)
	{
		if (!g_Craft_Config)
			return false;
		auto all = g_Craft_Config.GetAll();
		if (!all)
			return false;

		for (int i = 0; i < all.Count(); i++)
		{
			DRL_Craft_Recipe r = all[i];
			if (!r)
				continue;

			if (!RecipeAllowedByContext(r))
				continue;

			if (!RecipePassesCategoryIdx(r, catIdx))
				continue;

			return true;
		}
		return false;
	}

	protected void BuildSelectResultList(DRL_Craft_Recipe r)
	{
		if (m_SelectCraftListWrap)
			ClearChildren(m_SelectCraftListWrap);

		if (m_SelectRows)
			m_SelectRows.Clear();

		if (!r || !r.ResultClasses || r.ResultClasses.Count() == 0)
		{
			if (m_SelectCraftPanel)
				m_SelectCraftPanel.Show(false);
			m_SelectedResultClass = "";
			UpdateResultPreview(r);
			return;
		}

		if (m_SelectCraftPanel)
		{
			m_SelectCraftPanel.Show(true);
		}

		for (int k = 0; k < r.ResultClasses.Count(); k++)
		{
			string cls = r.ResultClasses[k];

			Widget row = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/SelectCraft.layout", m_SelectCraftListWrap);

			m_SelectRows.Insert(row);

			ScriptedWidgetEventHandler h = null;
			Class.CastTo(h, this);
			if (h)
				row.SetHandler(h);

			row.SetUserID(k);

			ItemPreviewWidget prev = ItemPreviewWidget.Cast(row.FindAnyWidget("SelectCraftItemPreview"));
			if (prev)
			{
				EntityAI itm = GetPreviewEntity(cls);
				if (itm)
				{
					prev.SetItem(itm);
					int vIdxItm = itm.GetViewIndex();
					if (vIdxItm < 0)
						vIdxItm = 0;
					prev.SetView(vIdxItm);
					prev.SetModelPosition(Vector(0, 0, 0));
					prev.SetModelOrientation(Vector(0, 0, 0));
					prev.Show(true);
					m_AttachedPreviews.Insert(prev);
				}
			}

			bool isSelected = false;
			if (m_SelectedResultClass != "" && m_SelectedResultClass == cls)
			{
				isSelected = true;
			}
			else
			{
				if (m_SelectedResultClass == "" && cls == r.Result)
				{
					isSelected = true;
				}
			}

			ColorizeSelectButton(row, isSelected);

			ButtonWidget btn = ButtonWidget.Cast(row.FindAnyWidget("SelectCraftButton"));
			if (btn)
			{
				ScriptedWidgetEventHandler h2 = null;

				Class.CastTo(h2, this);
				if (h2)
					btn.SetHandler(h2);

				btn.Enable(true);

				btn.Show(true);

				btn.SetUserID(k);
			}
		}

		UpdateResultPreview(r);
		UpdateResultStats(m_SelectedResultClass);
		UpdateSelectCraftHighlight();
	}

	protected bool DRL_TryPickResultFromWidget(Widget w, out int idx, out string cls)
	{
		idx = -1;
		cls = "";
		Widget cur = w;
		while (cur)
		{
			string nm = cur.GetName();
			if (nm.Contains("SelectCraftButton"))
			{
				ButtonWidget b = ButtonWidget.Cast(cur);
				if (b)
					idx = b.GetUserID();
				return idx >= 0;
			}

			cur = cur.GetParent();
		}
		return false;
	}

	protected ButtonWidget FindSelectButtonFrom(Widget w)
	{
		Widget cur = w;
		while (cur)
		{
			if (cur.GetName() == "SelectCraftButton")
				return ButtonWidget.Cast(cur);
			cur = cur.GetParent();
		}
		return null;
	}

	protected void UpdateReCraftButtonState()
	{
		if (!m_ReCraftButton)
			return;

		if (m_ReCraftActive)
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText)
				m_ReCraftButtonText.SetText("#STR_DRL_IN_PROGRESS");
			return;
		}

		if (!m_SelectedRecipe)
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText)
				m_ReCraftButtonText.SetText("#STR_DRL_NO_INGREDIENTS");
			return;
		}

		bool hasTarget = HasResultItemForRecraft(m_SelectedRecipe);

		if (hasTarget)
		{
			m_ReCraftButton.Enable(true);
			if (m_ReCraftButtonText)
				m_ReCraftButtonText.SetText("#STR_DRL_RECRAFT");
		}
		else
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText)
				m_ReCraftButtonText.SetText("#STR_DRL_NO_INGREDIENTS");
		}
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

		if (!DRL_ClientHasRequiredRecraftAttachments(r))
			return false;

		return true;
	}

	protected bool DRL_ClientHasRequiredRecraftAttachments(DRL_Craft_Recipe r)
	{
		if (!r || !r.CraftComponents)
			return true;

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0 || c.RecraftNeedAttachments != 1)
				continue;

			if (!DRL_ClientFindAttachment(c))
			{
				return false;
			}
		}

		return true;
	}

	protected Widget DRL_FindDescendantByName(Widget root, string name)
	{
		if (!root)
			return null;
		if (root.GetName() == name)
			return root;

		Widget ch = root.GetChildren();
		while (ch)
		{
			Widget got = DRL_FindDescendantByName(ch, name);
			if (got)
				return got;
			ch = ch.GetSibling();
		}
		return null;
	}

	protected void ColorizeSelectButton(Widget rowOrBtn, bool selected)
	{
		if (!rowOrBtn)
			return;

		Widget tile = DRL_FindDescendantByName(rowOrBtn, "SelectCraftItemPanelW");
		if (!tile)
			return;

		int normal = ARGB(255, 38, 34, 32);
		int active = ARGB(255, 137, 91, 36);

		if (selected)
		{
			tile.SetColor(active);
		}
		else
		{
			tile.SetColor(normal);
		}
	}

	protected int DRL_FindResultIndex(string cls)
	{
		if (!m_SelectedRecipe)
			return -1;
		if (!m_SelectedRecipe.ResultClasses)
			return -1;

		for (int i = 0; i < m_SelectedRecipe.ResultClasses.Count(); i++)
		{
			string c = m_SelectedRecipe.ResultClasses[i];
			if (c == cls)
				return i;
		}
		return -1;
	}

	protected void UpdateSelectCraftHighlight()
	{
		if (!m_SelectedRecipe)
			return;

		int selIndex = -1;
		if (m_SelectedResultClass != "")
			selIndex = DRL_FindResultIndex(m_SelectedResultClass);

		int defaultIndex = -1;
		if (m_SelectedRecipe.Result != "")
			defaultIndex = DRL_FindResultIndex(m_SelectedRecipe.Result);

		if (selIndex < 0)
			selIndex = defaultIndex;

		if (selIndex < 0)
			selIndex = 0;

		for (int i = 0; i < m_SelectRows.Count(); i++)
		{
			bool selected = false;
			if (i == selIndex)
				selected = true;
			ColorizeSelectButton(m_SelectRows[i], selected);
		}
	}

	void BuildReCraftList(DRL_Craft_Recipe r)
	{
		if (!m_ReCraftListWrap)
			return;

		ClearChildren(m_ReCraftListWrap);

		bool hasDrops = false;

		if (r)
		{
			string targetRes = "";
			if (m_SelectedResultClass != "" && r.ResultClasses && r.ResultClasses.Find(m_SelectedResultClass) > -1)
			{
				targetRes = m_SelectedResultClass;
			}
			else
			{
				targetRes = r.Result;
			}

			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c)
					continue;

				if (!c.IsApplicableForResult(targetRes))
					continue;

				if (c.RecraftDropChance <= 0.0)
					continue;

				hasDrops = true;

				Widget slot = GetGame().GetWorkspace().CreateWidgets(RECRAFT_TEMPLATE, m_ReCraftListWrap);
				if (!slot)
					continue;

				slot.SetUserData(c);

				ItemPreviewWidget prev = ItemPreviewWidget.Cast(slot.FindAnyWidget("ReCraftIngridientItemPreview"));
				TextWidget qty = TextWidget.Cast(slot.FindAnyWidget("ReCraftIngridientItemQuantity"));

				if (prev && c.Classname && c.Classname != "")
				{
					EntityAI e = GetPreviewEntity(c.Classname);
					if (e)
					{
						prev.SetItem(e);
						int vIdxE = e.GetViewIndex();
						if (vIdxE < 0)
							vIdxE = 0;
						prev.SetView(vIdxE);
						prev.SetModelPosition(Vector(0, 0, 0));
						prev.SetModelOrientation(Vector(0, 0, 0));
						prev.Show(true);
						m_AttachedPreviews.Insert(prev);
					}
				}

				if (qty)
				{
					string t = "";
					if (DRL_IsFillingByClassUI(c.Classname) && c.RecraftQuantityFrac > 0.0)
					{
						t = DRL_PercentText(c.RecraftQuantityFrac);
					}
					else
					{
						int cnt = c.RecraftAmount;
						if (cnt <= 0)
							cnt = 1;
						t = string.Format("x%1", cnt);
					}
					qty.SetText(t);
				}
			}
		}

		if (m_ReCraftPanel)
			m_ReCraftPanel.Show(hasDrops);

		if (m_ReCraftRecipeDetailText && r)
		{
			int rSec = r.GetRecraftTimeSec();
			m_ReCraftRecipeDetailText.SetText(string.Format("#STR_DRL_ReCraft_Recipe (%1)", DRL_FormatTimeSecs(rSec)));
		}

		UpdateReCraftButtonState();

		if (m_ReCraftListWrap)
			m_ReCraftListWrap.Update();
	}

	protected void StartReCraftVisuals(float durationSec)
	{
		if (durationSec <= 0.0)
			durationSec = 0.01;

		m_ReCraftActive = true;
		m_ReCraftElapsed = 0.0;
		m_ReCraftDuration = durationSec;

		UpdateReCraftButtonState();

		if (!m_ReCraftProgressTimer)
			m_ReCraftProgressTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		m_ReCraftProgressTimer.Run(0.05, this, "TickReCraftProgress", null, true);

		if (m_ReCraftPB)
		{
			m_ReCraftPB.SetCurrent(0);
			m_ReCraftPB.Show(true);
		}
	}

	protected void TickReCraftProgress()
	{
		if (!m_ReCraftActive)
			return;

		m_ReCraftElapsed += 0.05;
		float pct = (m_ReCraftElapsed / m_ReCraftDuration) * 100.0;
		int cur = Math.Clamp(Math.Round(pct), 0, 100);
		if (m_ReCraftPB)
			m_ReCraftPB.SetCurrent(cur);

		if (m_ReCraftElapsed >= m_ReCraftDuration)
		{
			m_ReCraftActive = false;
			if (m_ReCraftProgressTimer)
				m_ReCraftProgressTimer.Stop();
			if (m_ReCraftPB)
				m_ReCraftPB.SetCurrent(100);

			DRL_StopCraftSound();

			UpdateReCraftButtonState();

			if (m_SelectedRecipe)
				FillRecipeDetails(m_SelectedRecipe);
			RefreshRecipeListColors();
			bool can = CanCraftSelected();
			UpdateCraftButtonState();
			UpdateCraftButtonColor(can);
		}
	}

	protected void TryStartReCraft()
	{
		if (!m_SelectedRecipe)
			return;
		if (m_ReCraftActive)
			return;
		if (!HasResultItemForRecraft(m_SelectedRecipe))
		{
			if (m_ReCraftButton)
				m_ReCraftButton.Enable(false);
			return;
		}

		if (!m_ContextWorkbench)
		{
			DRL_PlayCraftSoundForRecipe(m_SelectedRecipe);
		}

		StartReCraftVisuals(m_SelectedRecipe.GetRecraftTimeSec());
		UpdateReCraftButtonState();

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		string chosen = m_SelectedRecipe.Result;
		if (m_SelectedResultClass != "" && m_SelectedRecipe.ResultClasses && m_SelectedRecipe.ResultClasses.Find(m_SelectedResultClass) > -1)
			chosen = m_SelectedResultClass;

		int netLow = 0;
		int netHigh = 0;
		if (m_ContextWorkbench)
		{
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		}

		Param4<int, string, int, int> pdata4 = new Param4<int, string, int, int>(m_SelectedRecipe.RecipeId, chosen, netLow, netHigh);
		GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_RECRAFT, pdata4, true, player.GetIdentity());
	}

	protected void TickCraftProgress()
	{
		if (!m_CraftActive)
			return;

		m_CraftElapsed += 0.05;
		float pct = (m_CraftElapsed / m_CraftDuration) * 100.0;
		int cur = Math.Clamp(Math.Round(pct), 0, 100);
		if (m_CraftPB)
			m_CraftPB.SetCurrent(cur);

		if (m_CraftElapsed >= m_CraftDuration)
		{
			m_CraftActive = false;
			if (m_CraftProgressTimer)
				m_CraftProgressTimer.Stop();
			if (m_CraftPB)
				m_CraftPB.SetCurrent(100);

			DRL_StopCraftSound();

			if (m_SelectedRecipe)
				FillRecipeDetails(m_SelectedRecipe);
			RefreshRecipeListColors();
			bool can = CanCraftSelected();
			UpdateCraftButtonState();
			UpdateCraftButtonColor(can);
		}
	}

	protected void StartCraftVisuals(float durationSec)
	{
		if (durationSec <= 0.0)
			durationSec = 0.01;

		if (m_CraftTimer)
			m_CraftTimer.Stop();
		if (m_CraftProgressTimer)
			m_CraftProgressTimer.Stop();

		m_CraftDuration = durationSec;
		m_CraftElapsed = 0.0;
		m_CraftActive = true;

		if (m_CraftPB)
			m_CraftPB.SetCurrent(0);

		UpdateCraftButtonState();

		if (m_ButtonCraftText)
			m_ButtonCraftText.SetText("#STR_DRL_IN_PROGRESS");

		m_RemainTime = Math.Ceil(durationSec);
		UpdateCraftCountdown();
		if (!m_CraftTimer)
			m_CraftTimer = new Timer();
		m_CraftTimer.Run(1.0, this, "UpdateCraftCountdown", null, true);

		if (!m_CraftProgressTimer)
			m_CraftProgressTimer = new Timer();
		m_CraftProgressTimer.Run(0.05, this, "TickCraftProgress", null, true);
	}

	void BuildRecipesList(string queryLower = "")
	{
		if (!m_RecipesListSpacer)
			return;

		if (m_QueuePopulator)
		{
			m_QueuePopulator.Stop();
		}

		ClearChildren(m_RecipesListSpacer);

		if (!g_Craft_Config)
			return;
		ref array<ref DRL_Craft_Recipe> all = g_Craft_Config.GetAll();
		if (!all)
			return;

		array<EntityAI> cachedItems = new array<EntityAI>();
		DRL_GetInventorySnapshot(cachedItems);

		array<DRL_Craft_Recipe> filteredQueue = new array<DRL_Craft_Recipe>();
		DRL_Craft_Category curCat = GetCurrentCategory();

		for (int i = 0; i < all.Count(); i++)
		{
			DRL_Craft_Recipe r = all[i];
			if (!r)
				continue;

			string rn = r.RecipeName;
			rn.ToLower();

			if (queryLower != "" && rn != "" && rn.IndexOf(queryLower) == -1)
				continue;

			if (!RecipeAllowedByContext(r))
				continue;

			if (!RecipePassesCategory(r))
				continue;

			if (curCat && curCat.ShowOnlyAvailableCraft == 1)
			{
				if (!DRL_IsRecipeCraftableNow(r, cachedItems))
					continue;
			}

			filteredQueue.Insert(r);
		}

		if (m_QueuePopulator)
		{
			m_QueuePopulator.Start(filteredQueue, cachedItems);
		}
	}

	void CreateRecipeRowAsync(DRL_Craft_Recipe r, array<EntityAI> cachedItems)
	{
		if (!m_RecipesListSpacer || !r)
			return;

		Widget row = GetGame().GetWorkspace().CreateWidgets(RECIPE_TEMPLATE, m_RecipesListSpacer);
		if (row)
		{
			FillRecipeRow(row, r, cachedItems);
			m_RecipesListSpacer.Update();
		}
	}

	void OnRecipePopulationFinished()
	{
		if (m_RecipesListSpacer)
		{
			m_RecipesListSpacer.Update();
		}
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_SearchBox)
		{
			string raw = m_SearchBox.GetText();
			string cleaned = CleanSearchQuery(raw);
			if (cleaned != raw)
			{
				m_SearchBox.SetText(cleaned);
			}

			string q = cleaned;
			q.ToLower();
			BuildRecipesList(q);
			return true;
		}
		return super.OnChange(w, x, y, finished);
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w == m_SearchBox)
		{
			if (IsSearchDefaultText(m_SearchBox.GetText()))
			{
				m_SearchBox.SetText("");
			}
		}
		return super.OnFocus(w, x, y);
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w == m_SearchBox)
		{
			if (m_SearchBox.GetText() == "")
			{
				m_SearchBox.SetText(SEARCH_DEFAULT_STR);
			}
		}
		return super.OnFocusLost(w, x, y);
	}

	int NeedPerCraft(DRL_Craft_Component c, int compIdx = -1)
	{
		string activeCls = "";
		if (compIdx >= 0)
		{
			activeCls = GetActiveComponentClass(c, compIdx);
		}
		return DRL_CraftInventoryHelper.NeedPerCraft(c, activeCls);
	}

	int NeedForCount(DRL_Craft_Component c, int countN, int compIdx = -1)
	{
		string activeCls = "";
		if (compIdx >= 0)
		{
			activeCls = GetActiveComponentClass(c, compIdx);
		}
		return DRL_CraftInventoryHelper.NeedForCount(c, countN, activeCls);
	}

	int NeedForClassCount(string cls, DRL_Craft_Component c, int countN)
	{
		return DRL_CraftInventoryHelper.NeedForClassCount(cls, c, countN);
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

	string DRL_FormatTimeSecs(int t)
	{
		int mm = t / 60;
		int ss = t % 60;

		if (mm > 0 && ss > 0)
			return string.Format("%1 #STR_DRL_MIN %2 #STR_DRL_SEC", mm, ss);

		if (mm > 0)
			return string.Format("%1 #STR_DRL_MIN", mm);

		return string.Format("%1 #STR_DRL_SEC", ss);
	}

	string DRL_CraftTimeLabel(int secs)
	{
		return string.Format("#STR_DRL_CRAFT_TIME  %1", DRL_FormatTimeSecs(secs));
	}

	string DRL_FormatPercent(float frac)
	{
		if (frac < 0.0)
			frac = 0.0;
		if (frac > 1.0)
			frac = 1.0;
		int p = Math.Round(frac * 100.0);
		return string.Format("%1%%", p);
	}

	string DRL_FormatStateFromDamageFrac(float dmgFrac)
	{
		if (dmgFrac <= 0.0)
			return "#STR_DRL_UNTOUCHED";

		if (dmgFrac >= 1.0)
			return "#STR_DRL_BROKEN";

		int p = Math.Round(dmgFrac * 100.0);
		return string.Format("#STR_DRL_DMG_PERCENT %1%%", p);
	}

	string DRL_PercentText(float frac)
	{
		if (frac < 0.0)
			frac = 0.0;
		if (frac > 1.0)
			frac = 1.0;
		int p = Math.Round(frac * 100.0);
		return string.Format("%1%%", p);
	}

	string DRL_HealthLevelToText(int lvl)
	{
		int cl = Math.Clamp(lvl, 0, 3);
		if (cl == 0)
			return "#STR_DRL_UNTOUCHED";
		if (cl == 1)
			return "#STR_DRL_WORN";
		if (cl == 2)
			return "#STR_DRL_DAMAGED";
		return "#STR_DRL_BADLY_DAMAGED";
	}

	string DRL_TextFromMinHealthFrac(int minHF)
	{
		int lvl = 3 - minHF;
		if (lvl < 0)
			lvl = 0;
		if (lvl > 3)
			lvl = 3;
		return DRL_HealthLevelToText(lvl);
	}

	protected void FillIngredientDetailInfo(DRL_Craft_Component c)
	{
		TextWidget wHealth = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiHealth"));
		if (!wHealth)
			wHealth = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiState"));
		if (wHealth)
		{
			string ht = DRL_TextFromMinHealthFrac(c.MinHealthFrac);
			wHealth.SetText(string.Format("#STR_DRL_STATE %1", ht));
		}

		TextWidget wCount = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiCount"));
		if (wCount)
		{
			int cnt = c.Amount;
			if (cnt <= 0)
				cnt = 1;
			wCount.SetText(string.Format("#STR_DRL_QUANTITY %1", cnt));
		}

		TextWidget wQty = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiQuantity"));
		if (wQty)
		{
			string qs = "—";
			if (c.QuantityFrac > 0.0)
				qs = DRL_PercentText(c.QuantityFrac);
			wQty.SetText(string.Format("#STR_DRL_FILL %1", qs));
		}

		TextWidget wDmg = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiDamage"));
		TextWidget wDel = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiDelete"));

		string dmgText = string.Format("#STR_DRL_DAMAGE - %1", DRL_PercentText(c.ApplyDamageFrac));
		string delText = "#STR_DRL_DELETE_NO";
		if (c.Delete)
			delText = "#STR_DRL_DELETE_YES";

		if (wDmg)
		{
			wDmg.SetText(dmgText);
			if (wDel)
				wDel.SetText(delText);
		}
		else
		{
			if (wDel)
				wDel.SetText(string.Format("%1 | %2", dmgText, delText));
		}
	}

	protected void UpdateResultPreview(DRL_Craft_Recipe r)
	{
		if (!r)
			return;

		string cls = r.Result;
		if (m_SelectedResultClass && m_SelectedResultClass != "")
		{
			if (r.ResultClasses && r.ResultClasses.Find(m_SelectedResultClass) > -1)
				cls = m_SelectedResultClass;
		}

		ItemPreviewWidget ipwResult = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("PreviewItemWidget"));
		if (ipwResult)
		{
			ipwResult.SetItem(null);
			if (cls != "")
			{
				EntityAI eRes = GetPreviewEntity(cls);
				if (eRes)
				{
					ipwResult.SetItem(eRes);
					int vIdxRes = eRes.GetViewIndex();
					if (vIdxRes < 0)
						vIdxRes = 0;
					ipwResult.SetView(vIdxRes);
					ipwResult.SetModelPosition(Vector(0, 0, 0));
					ipwResult.SetModelOrientation(Vector(0, 0, 0));
					ipwResult.Show(true);
					m_AttachedPreviews.Insert(ipwResult);
				}
			}
		}

		RebuildMainPreviewWithAttachments();
		RenderAttachmentIconsForClass(cls);
	}

	protected void FillPreviewInfoPanel(DRL_Craft_Recipe r, string chosenResult)
	{
		Widget infoPanel = m_Root.FindAnyWidget("PreviewItemInfo");
		if (!infoPanel)
			return;

		infoPanel.Show(true);

		TextWidget wName = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoName"));
		if (wName)
		{
			string dn = "";
			if (chosenResult != "")
			{
				if (!GetDisplayNameForClass(chosenResult, dn))
					dn = chosenResult;
			}
			wName.SetText(string.Format("#STR_DRL_NAME %1", dn));
		}

		TextWidget wTime = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoTime"));
		if (wTime)
		{
			int t_cfg = GetTotalCraftTimeSec(r);
			wTime.SetText(string.Format("#STR_DRL_TIME %1", DRL_FormatTimeSecs(t_cfg)));
		}

		TextWidget wCount = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoCount"));
		if (wCount)
		{
			int cnt = r.ResultCount;
			if (cnt <= 0)
				cnt = 1;
			wCount.SetText(string.Format("#STR_DRL_QUANTITY %1", cnt));
		}

		TextWidget wQty = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoQuantity"));
		if (wQty)
		{
			string qs = "—";
			if (r.ResultQuantityFrac > 0.0)
				qs = DRL_FormatPercent(r.ResultQuantityFrac);
			wQty.SetText(string.Format("#STR_DRL_FILL %1", qs));
		}

		TextWidget wState = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoState"));
		if (wState)
		{
			wState.SetText(string.Format("#STR_DRL_STATE %1", DRL_FormatStateFromDamageFrac(r.ResultDamageFrac)));
		}
	}

	protected void BuildIngredientsListUI(DRL_Craft_Recipe r, Widget ingrWrap, array<ref DRL_CraftMergedComponent> mergedList, array<EntityAI> cachedItems)
	{
		ClearChildren(ingrWrap);
		m_ActiveDisplayComponents.Clear();
		m_ActiveDisplayMergedComponents.Clear();

		for (int i = 0; i < mergedList.Count(); i++)
		{
			DRL_CraftMergedComponent mc = mergedList[i];
			if (!mc)
				continue;
			DRL_Craft_Component c = mc.ToCraftComponent();
			if (!c || c.AttachmentsEx > 0)
				continue;
			m_ActiveDisplayComponents.Insert(c);
			m_ActiveDisplayMergedComponents.Insert(mc);

			string activeCls = mc.Classname;
			if (activeCls == "")
			{
				activeCls = c.Classname;
			}
			array<string> variations = GetVariationsForComponent(c);
			bool hasVariations = (variations && variations.Count() > 1 && c.EnableVariationSelection == 1);

			Widget slot = GetGame().GetWorkspace().CreateWidgets(INGR_TEMPLATE, ingrWrap);
			slot.SetUserData(mc);
			slot.SetUserID(i);

			ButtonWidget slotBtn = ButtonWidget.Cast(slot.FindAnyWidget("CraftIngridientButton"));
			if (slotBtn)
			{
				slotBtn.SetUserData(mc);
				slotBtn.SetUserID(i);
			}

			ImageWidget varIcon = ImageWidget.Cast(slot.FindAnyWidget("VariationIcon"));
			if (varIcon)
			{
				varIcon.Show(hasVariations);
			}

			ItemPreviewWidget prev = ItemPreviewWidget.Cast(slot.FindAnyWidget("CraftIngridientItemPreview"));
			TextWidget        qty = TextWidget.Cast(slot.FindAnyWidget("CraftIngridientItemQuantity"));
			Widget            panelW = slot.FindAnyWidget("CraftIngridientItemPanelWidget");

			if (prev && activeCls != "")
			{
				prev.SetItem(null);
				EntityAI ew = GetPreviewEntity(activeCls);
				if (ew)
				{
					prev.SetItem(ew);
					int vIdx = ew.GetViewIndex();
					if (vIdx < 0)
						vIdx = 0;
					prev.SetView(vIdx);
					prev.SetModelPosition(Vector(0, 0, 0));
					prev.SetModelOrientation(Vector(0, 0, 0));
					prev.Show(true);
					m_AttachedPreviews.Insert(prev);
				}
			}

			if (qty)
			{
				int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, cachedItems, c.QuantityFrac, c.UseInheritance == 1);
				int need = NeedForClassCount(activeCls, c, m_SelectedRecipe.CurrentCraftCount);
				qty.SetText(DRL_CraftInventoryHelper.FormatIngredientAmount(activeCls, c, have, need));
				qty.Show(true);

				if (panelW)
				{
					if (have >= need)
					{
						panelW.SetColor(ARGB(10, 120, 200, 80));
					}
					else if (have > 0)
					{
						panelW.SetColor(ARGB(10, 230, 180, 60));
					}
					else
					{
						panelW.SetColor(ARGB(10, 200, 60, 60));
					}
				}
			}
		}

		ingrWrap.Update();
	}

	void FillRecipeDetails(DRL_Craft_Recipe r)
	{
		if (!r)
			return;

		string chosenResult = r.Result;
		if (m_SelectedResultClass && m_SelectedResultClass != "")
		{
			if (r.ResultClasses && r.ResultClasses.Find(m_SelectedResultClass) > -1)
				chosenResult = m_SelectedResultClass;
		}

		if (!m_RightPanel)
			m_RightPanel = m_Root.FindAnyWidget("RigthPanelWidget");
		if (m_RightPanel)
			m_RightPanel.Show(true);

		UpdateAdditionalTasksButtonState();

		if (m_IngrDetailPanel)
			m_IngrDetailPanel.Show(false);

		if (m_VariationIngridientListWidget)
			m_VariationIngridientListWidget.Show(false);
		m_ActiveVariationComponentIndex = -1;

		RichTextWidget titleW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftRecipeTitel"));
		if (titleW)
			titleW.SetText(r.RecipeName);

		MultilineTextWidget descW = MultilineTextWidget.Cast(m_Root.FindAnyWidget("CraftRecipeDetailMultiline"));
		if (descW)
		{
			string body = "";
			if (r.Description && r.Description != "")
			{
				body = r.Description;
			}
			else if (r.Result && r.Result != "")
			{
				string cfgDesc = "";
				if (GetDescriptionShortForClass(r.Result, cfgDesc))
					body = cfgDesc;
			}
			if (body == "")
				body = "#STR_DRL_Craft_Recipe_Detail";
			descW.SetText(body);
		}

		FillPreviewInfoPanel(r, chosenResult);
		RebuildMainPreviewWithAttachments();

		string attachClass = chosenResult;
		if (attachClass == "")
		{
			if (r.PreviewClassname && r.PreviewClassname != "")
				attachClass = r.PreviewClassname;
			else
				attachClass = r.Result;
		}
		RenderAttachmentIconsForClass(attachClass);

		Widget previewPanel = m_Root.FindAnyWidget("PreviewItemWidgetPanel");
		if (previewPanel)
			previewPanel.Show(true);

		ItemPreviewWidget ipwResult = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("PreviewItemWidget"));
		if (ipwResult && chosenResult != "")
		{
			EntityAI e2 = GetPreviewEntity(chosenResult);
			if (e2)
			{
				ipwResult.SetItem(e2);
				int vIdx2 = e2.GetViewIndex();
				if (vIdx2 < 0)
					vIdx2 = 0;
				ipwResult.SetView(vIdx2);
				ipwResult.SetModelPosition(Vector(0, 0, 0));
				ipwResult.SetModelOrientation(Vector(0, 0, 0));
				ipwResult.Show(true);
				m_AttachedPreviews.Insert(ipwResult);
			}
		}

		m_ItemPreviewWidget0 = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));
		if (m_ItemPreviewWidget0)
		{
			m_PreviewOrientation = Vector(0, 0, 0);
			m_ItemPreviewWidget0.SetModelOrientation(m_PreviewOrientation);
		}

		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (timeW)
		{
			int t_total = GetTotalCraftTimeSec(r);
			timeW.SetText(DRL_CraftTimeLabel(t_total));
		}

		Widget ingrWrap = m_Root.FindAnyWidget("CraftIngridientListWrap");
		if (!ingrWrap)
			return;

		array<EntityAI> cachedItems;
		DRL_GetInventorySnapshot(cachedItems);
		array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(r);

		BuildIngredientsListUI(r, ingrWrap, mergedList, cachedItems);
		BuildAtachIngridientListUI(r);
		BuildSelectResultList(r);

		UpdateCraftButtonState();
		bool canClient = CanCraftRecipe(r, cachedItems) && DRL_ClientHasRequiredAttachments(r);
		UpdateCraftButtonColor(canClient);

		RefreshTimeTextForCount();
		RefreshCraftCountText();
		RefreshRecipeListColors();
		BuildReCraftList(r);
		UpdateReCraftButtonState();
		UpdateResultStats(chosenResult);
	}

	array<string> GetVariationsForComponent(DRL_Craft_Component comp)
	{
		return DRL_CraftMergedHelper.GetVariationsForComponent(comp);
	}

	DRL_Craft_Component GetComponentByKey(int key)
	{
		return DRL_CraftMergedHelper.GetComponentByKey(key, m_SelectedRecipe, GetCraft_Config());
	}

	array<string> GetVariationsForMergedComponent(DRL_CraftMergedComponent mc)
	{
		return DRL_CraftMergedHelper.GetVariationsForMergedComponent(mc, m_SelectedRecipe, GetCraft_Config());
	}

	string GetActiveComponentClass(DRL_Craft_Component comp, int compIdx)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftMergedHelper.GetActiveComponentClass(comp, compIdx, m_SelectedComponentVariations, m_SelectedRecipe, p, m_ContextWorkbench);
	}

	protected void AddVariationHeader(string text)
	{
		if (m_VariationsController)
		{
			m_VariationsController.AddVariationHeader(text, VARIATION_HEADER_TEMPLATE);
		}
	}

	protected void CreateVariationTile(Widget parentWrap, DRL_CraftMergedComponent mc, int targetKey, string varCls, int varIdx, string activeCls, DRL_Craft_Component compRef, array<EntityAI> cachedItems)
	{
		if (m_VariationsController)
		{
			m_VariationsController.CreateVariationTile(parentWrap, mc, targetKey, varCls, varIdx, activeCls, compRef, cachedItems, INGR_TEMPLATE);
		}
	}

	protected void BuildVariationListUI(DRL_CraftMergedComponent mc, int compIdx)
	{
		m_ActiveVariationComponentIndex = compIdx;
		m_ActiveVariationMergedComponent = mc;
		if (m_VariationsController)
		{
			m_VariationsController.BuildList(mc, null);
		}
	}

	void DRL_AddRequirementHeader(string title)
	{
		if (m_RequirementsController)
		{
			m_RequirementsController.AddRequirementHeader(title, REQ_HEADER_TEMPLATE);
		}
	}

	Widget DRL_AddRequirementItem(string title, string subtitle, string iconPath, string previewClass, bool isMet, string statusText = "", DRL_Craft_Recipe targetRecipe = null)
	{
		if (m_RequirementsController)
		{
			return m_RequirementsController.AddRequirementItem(title, subtitle, iconPath, previewClass, isMet, statusText, targetRecipe, REQ_ITEM_TEMPLATE);
		}
		return null;
	}

	void DRL_AddRequirementInfo(string text, int color = 0)
	{
		if (m_RequirementsController)
		{
			m_RequirementsController.AddRequirementInfo(text, color, REQ_INFO_TEMPLATE);
		}
	}

	bool DRL_AreAllRecipeRequirementsMet(DRL_Craft_Recipe r)
	{
		if (m_RequirementsController)
		{
			return m_RequirementsController.AreAllRecipeRequirementsMet(r);
		}
		return true;
	}

	void BuildAdditionalTasksUI(DRL_Craft_Recipe r)
	{
		if (m_RequirementsController)
		{
			m_RequirementsController.BuildUI(r);
		}
	}

	bool DRL_IsComponentHealthyAttachment(ItemBase ib, DRL_Craft_Component c)
	{
		if (!ib || !c)
			return false;
		if (ib.IsRuined())
			return false;
		int maxLevel = c.MinHealthFrac;
		if (maxLevel < 0)
			maxLevel = 0;
		if (maxLevel > 3)
			maxLevel = 3;
		return ib.GetHealthLevel() <= maxLevel;
	}

	bool DRL_IsHealthyAttachment(ItemBase ib)
	{
		if (!ib)
			return false;
		if (ib.IsRuined())
			return false;
		return ib.GetHealthLevel() <= 2;
	}

	bool DRL_ItemFitsSlotClient(ItemBase ib, int targetSlotId)
	{
		if (!ib || !ib.GetInventory() || targetSlotId < 0)
			return false;
		int slotCount = ib.GetInventory().GetSlotIdCount();
		for (int i = 0; i < slotCount; i++)
		{
			if (ib.GetInventory().GetSlotId(i) == targetSlotId)
				return true;
		}
		return false;
	}

	protected bool DRL_ClientItemMatchesComponent(ItemBase ib, DRL_Craft_Component c, string token, bool isSlot)
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.ItemMatchesComponent(ib, c, token, isSlot);
		return false;
	}

	EntityAI DRL_ClientFindAttachment(DRL_Craft_Component c)
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.FindAttachment(c);
		return null;
	}

	bool DRL_ClientHasRequiredAttachments(DRL_Craft_Recipe r)
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.HasRequiredAttachments(r);
		return true;
	}



	protected void BuildAtachIngridientListUI(DRL_Craft_Recipe r)
	{
		if (m_AttachmentsController)
			m_AttachmentsController.BuildAtachIngridientListUI(r);
	}

	protected bool ReadInt(string path, out int val)
	{
		return DRL_CraftStatsHelper.ReadInt(path, val);
	}

	protected bool ReadFloat(string path, out float val)
	{
		return DRL_CraftStatsHelper.ReadFloat(path, val);
	}

	protected void UpdateResultStats(string cls)
	{
		DRL_CraftStatsHelper.UpdateResultStats(cls, m_StatsTextW);
	}

	protected void GetAttachmentSlotsForClass(string cls, out array<string> outSlots)
	{
		DRL_CraftStatsHelper.GetAttachmentSlotsForClass(cls, outSlots);
	}

	protected bool GetSlotIconPath(string slotName, out string outPath)
	{
		return DRL_CraftStatsHelper.GetSlotIconPath(slotName, outPath);
	}

	protected void RenderAttachmentIconsForClass(string cls)
	{
		DRL_CraftStatsHelper.RenderAttachmentIconsForClass(cls, m_AttachmentIconsWrap);
	}

	bool CanCraftRecipeCount(DRL_Craft_Recipe r, int countN)
	{
		if (!r)
			return false;

		string activeRes = "";
		if (r == m_SelectedRecipe && m_SelectedResultClass != "")
		{
			activeRes = m_SelectedResultClass;
		}
		else
		{
			activeRes = r.Result;
		}

		int cnt = Math.Max(1, countN);
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx > 0)
				continue;

			if (!c.IsApplicableForResult(activeRes))
				continue;

			int compIdxParam = -1;
			if (r == m_SelectedRecipe)
			{
				compIdxParam = i;
			}
			int have = ComputeHaveForComponent(c, compIdxParam);
			int need = NeedForCount(c, cnt, compIdxParam);
			if (have < need)
				return false;
		}
		if (!DRL_ClientHasRequiredAttachments(r))
			return false;
		return true;
	}

	protected void UpdateRecipeSelectionHighlight()
	{
		Widget parent = m_RecipesListSpacer;
		if (!parent)
			return;

		int defaultCol = ARGB(50, 22, 23, 19);
		int selectedCol = ARGB(80, 88, 64, 31);

		Widget child = parent.GetChildren();
		while (child)
		{
			DRL_Craft_Recipe r;
			child.GetUserData(r);

			Widget frame = child.FindAnyWidget("BoardSelectionButtonWidget");
			if (frame)
			{
				if (r && r == m_SelectedRecipe)
					frame.SetColor(selectedCol);
				else
					frame.SetColor(defaultCol);
			}

			child = child.GetSibling();
		}
	}

	protected void RefreshRecipeListColors()
	{
		if (!m_RecipesListSpacer)
			return;

		array<EntityAI> cachedItems;
		DRL_GetInventorySnapshot(cachedItems);

		Widget row = m_RecipesListSpacer.GetChildren();
		while (row)
		{
			DRL_Craft_Recipe r;
			row.GetUserData(r);

			if (r)
			{
				bool can = CanCraftRecipe(r, cachedItems);

				Widget prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget");
				if (!prevPanel)
					prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget0");

				if (prevPanel)
				{
					if (can)
						prevPanel.SetColor(ARGB(30, 120, 200, 80));
					else
						prevPanel.SetColor(ARGB(30, 200, 60, 60));
				}

				ImageWidget lockIcon = ImageWidget.Cast(row.FindAnyWidget("LockIcon"));
				if (lockIcon)
				{
					bool isLearned = DRL_ClientHasLearnedRecipe(r.RecipeId);
					lockIcon.Show(!isLearned);
				}
			}

			row = row.GetSibling();
		}
	}

	protected void UpdateCraftCountdown()
	{
		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (!timeW)
			return;

		if (m_RemainTime > 0)
		{
			timeW.SetText(DRL_CraftTimeLabel(m_RemainTime));

			if (m_ButtonCraftText)
				m_ButtonCraftText.SetText("#STR_DRL_IN_PROGRESS");

			m_RemainTime--;
			return;
		}

		if (m_SelectedRecipe)
		{
			int t_total = m_SelectedRecipe.CraftTimeSec;
			timeW.SetText(DRL_CraftTimeLabel(t_total));
		}
		else
		{
			timeW.SetText("");
		}

		if (m_CraftTimer)
			m_CraftTimer.Stop();

		DRL_StopCraftSound();

		m_CraftActive = false;
		if (m_CraftProgressTimer)
			m_CraftProgressTimer.Stop();

		if (m_CraftPB)
			m_CraftPB.SetCurrent(0);

		bool can = CanCraftSelected();
		RefreshCraftCountText();
		UpdateCraftButtonState();
		UpdateCraftButtonColor(can);
	}

	int GetTotalCraftTimeSec(DRL_Craft_Recipe r)
	{
		if (!r)
		{
			return 0;
		}

		int total = r.CraftTimeSec;
		DRL_Craft_Config cfg = GetCraft_Config();
		array<int> attTimeIds = GetSelectedAttachedRecipeIds();
		if (cfg && attTimeIds)
		{
			for (int i = 0; i < attTimeIds.Count(); i++)
			{
				int dopId = attTimeIds[i];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				if (dopR)
				{
					int attQty = 1;
					if (m_AttachmentsController)
					{
						attQty = m_AttachmentsController.GetSelectedAttachedQuantity(dopId);
					}
					total += (dopR.CraftTimeSec * attQty);
				}
			}
		}
		return total;
	}

	static bool DRL_GetAttachmentSlots(string className, out TStringArray outSlots)
	{
		return DRL_AttachmentHelper.GetAttachmentSlots(className, outSlots);
	}

	static EDRLWeaponAttachType DRL_GetWeaponAttachmentType(string className, string slotName = "")
	{
		return DRL_AttachmentHelper.GetWeaponAttachmentType(className, slotName);
	}

	int FindConflictingAttachedRecipe(DRL_Craft_Recipe newDopR)
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.FindConflictingAttachedRecipe(newDopR);
		return -1;
	}

	EntityAI DRL_SpawnAttachment(EntityAI parent, string itemClass, int depth = 0)
	{
		return DRL_CraftPreviewHelper.SpawnAttachment(parent, itemClass, depth);
	}

	bool CanAttachToCurrentBuild(string targetCls)
	{
		if (m_AttachmentsController)
			return m_AttachmentsController.CanAttachToCurrentBuild(targetCls);
		return false;
	}

	void BuildAttachedRecipesList(DRL_Craft_Recipe r)
	{
		if (m_AttachmentsController)
			m_AttachmentsController.BuildAttachedRecipesList(r);
	}

	void UpdateAttachedSlotsAvailability()
	{
		if (m_AttachmentsController)
			m_AttachmentsController.UpdateAttachedSlotsAvailability();
	}

	void RebuildMainPreviewWithAttachments()
	{
		if (m_AttachmentsController)
			m_AttachmentsController.RebuildMainPreviewWithAttachments();
	}

	void OnAttachedSelectionChanged()
	{
		if (m_AttachmentsController)
			m_AttachmentsController.OnAttachedSelectionChanged();
	}

	void SelectRecipeDirectly(DRL_Craft_Recipe r)
	{
		if (!r)
		{
			return;
		}

		if (m_SelectedComponentVariations)
		{
			m_SelectedComponentVariations.Clear();
		}

		m_SelectedRecipe = r;
		m_SelectedAttachedRecipeIds.Clear();
		m_SelectedResultClass = "";

		HideVariationList();
		HideAdditionalTasks();

		if (m_RightPanel)
		{
			m_RightPanel.Show(true);
		}

		DRL_Craft_Category cat = DRL_FindCategoryForRecipe(r);
		DRL_ApplyFonBackground(cat);
		DRL_ApplyCraftFonBackground(cat);

		m_SelectedRecipe.CurrentCraftCount = 1;

		RebuildMainPreviewWithAttachments();
		FillRecipeDetails(r);
		BuildAttachedRecipesList(r);
		UpdateOptionalTitles(r);
		UpdateRecipeSelectionHighlight();
		RefreshCraftCountText();
		RefreshTimeTextForCount();
	}

	array<ref DRL_CraftMergedComponent> BuildMergedComponents(DRL_Craft_Recipe r)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		array<int> attIds = null;
		if (m_AttachmentsController)
		{
			attIds = m_AttachmentsController.GetSelectedAttachedRecipeIdsFlattened();
		}
		string resCls = "";
		if (r == m_SelectedRecipe && m_SelectedResultClass != "")
		{
			resCls = m_SelectedResultClass;
		}
		else if (r)
		{
			resCls = r.Result;
		}
		return DRL_CraftMergedHelper.BuildMergedComponents(r, m_SelectedComponentVariations, attIds, GetCraft_Config(), p, m_ContextWorkbench, resCls);
	}

	protected void FillRecipeRow(Widget row, DRL_Craft_Recipe r, array<EntityAI> cachedItems = null)
	{
		if (!row || !r)
			return;

		RichTextWidget nameW = RichTextWidget.Cast(row.FindAnyWidget("NameRecipesButton"));
		ImageWidget iconW = ImageWidget.Cast(row.FindAnyWidget("RecipesImagePreview"));
		ItemPreviewWidget prevW = ItemPreviewWidget.Cast(row.FindAnyWidget("RecipesItemPreview"));

		if (nameW)
			nameW.SetText(r.RecipeName);

		bool iconSet = false;
		if (iconW)
		{
			if (r.CraftIcon && r.CraftIcon != "")
			{
				iconW.LoadImageFile(0, ResolveImagePath(r.CraftIcon));
				iconW.Show(true);
				iconSet = true;
			}
			else
			{
				iconW.Show(false);
			}
		}
		if (!iconSet && prevW)
		{
			string cls = "";
			if (r.PreviewClassname && r.PreviewClassname != "")
			{
				cls = r.PreviewClassname;
			}
			else
			{
				cls = r.Result;
			}

			if (cls != "")
			{
				EntityAI dummyEnt = null;
				EntityAI e = DRL_CraftPreviewHelper.GetOrCreatePreviewEntity(cls, dummyEnt, m_PreviewCache);
				if (e)
				{
					DRL_CraftPreviewHelper.SetupPreviewWidget(prevW, e);
					m_AttachedPreviews.Insert(prevW);
				}
				else
				{
					prevW.Show(false);
				}
			}
			else
			{
				prevW.Show(false);
			}
		}
		else if (prevW)
		{
			prevW.Show(false);
		}

		bool can = CanCraftRecipe(r, cachedItems);

		Widget prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget");

		if (!prevPanel)
			prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget0");

		if (prevPanel)
		{
			if (can)
			{
				prevPanel.SetColor(ARGB(30, 120, 200, 80));
			}
			else
			{
				prevPanel.SetColor(ARGB(30, 200, 60, 60));
			}
		}

		ImageWidget rowLockIcon = ImageWidget.Cast(row.FindAnyWidget("LockIcon"));
		if (rowLockIcon)
		{
			bool rowLearned = DRL_ClientHasLearnedRecipe(r.RecipeId);
			rowLockIcon.Show(!rowLearned);
		}

		row.SetUserData(r);
		row.SetFlags(WidgetFlags.VISIBLE);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w == m_ItemPreviewWidget0)
		{
			GetGame().GetDragQueue().Call(this, "UpRotation");
			GetMousePos(m_PrevMouseX, m_PrevMouseY);
			return true;
		}

		if (button == MouseState.LEFT && w)
		{
			if (w == m_SearchBox)
			{
				if (IsSearchDefaultText(m_SearchBox.GetText()))
				{
					m_SearchBox.SetText("");
				}
			}

			bool isDopWidget = false;
			if (m_RecipesDopVarListWrap && IsWidgetChildOf(m_RecipesDopVarListWrap, w))
			{
				isDopWidget = true;
			}
			else if (m_RecipesDopVarListWidget && IsWidgetChildOf(m_RecipesDopVarListWidget, w))
			{
				isDopWidget = true;
			}

			DRL_Craft_Recipe r = null;
			Widget cur = w;
			while (cur && !r && !isDopWidget)
			{
				Class ud = null;
				cur.GetUserData(ud);
				if (ud)
				{
					DRL_Craft_Recipe tmp = DRL_Craft_Recipe.Cast(ud);
					if (tmp)
					{
						r = tmp;
						break;
					}
				}
				cur = cur.GetParent();
			}

			if (r)
			{
				if (m_SelectedRecipe != r && m_SelectedComponentVariations)
				{
					m_SelectedComponentVariations.Clear();
				}
				m_SelectedRecipe = r;
				m_SelectedRecipe.CurrentCraftCount = 1;
				m_SelectedAttachedRecipeIds.Clear();
				m_SelectedResultClass = "";
				HideVariationList();
				HideAdditionalTasks();
				BuildSelectResultList(m_SelectedRecipe);
				UpdateSelectCraftHighlight();
				UpdateResultPreview(m_SelectedRecipe);
				UpdateResultStats(m_SelectedRecipe.Result);
				UpdateCraftButtonState();

				FillRecipeDetails(r);
				BuildAttachedRecipesList(r);
				UpdateOptionalTitles(r);
				RefreshCraftCountText();
				RefreshTimeTextForCount();
				return true;
			}

			bool isCategoryBtn = false;
			Widget chkCat = w;
			while (chkCat)
			{
				if (chkCat.GetName() == "CategorySelectionButtonWidget")
				{
					isCategoryBtn = true;
					break;
				}
				chkCat = chkCat.GetParent();
			}

			if (!isCategoryBtn && m_RightPanel && m_RightPanel.IsVisible())
			{
				if (!IsWidgetChildOf(m_RightPanel, w))
				{
					DeselectRecipe();
				}
			}
		}

		if (button == MouseState.LEFT && w)
		{
			ButtonWidget sb = FindSelectButtonFrom(w);
			if (sb)
				return OnClick(sb, x, y, MouseState.LEFT);
		}

		return super.OnMouseButtonDown(w, x, y, button);
	}

	void UpRotation(int mouse_x, int mouse_y, bool is_dragging)
	{
		vector o = m_PreviewOrientation;

		o[0] = o[0] + (m_PrevMouseX - mouse_y);
		o[1] = o[1] - (m_PrevMouseY - mouse_x);

		m_ItemPreviewWidget0.SetModelOrientation(o);

		if (!is_dragging)
		{
			m_PreviewOrientation = o;
		}
	}

	protected void ShowRecipeDetails(DRL_Craft_Recipe r)
	{
		FillRecipeDetails(r);
		UpdateOptionalTitles(r);
	}

	protected bool HandleCraftCountClick(Widget w)
	{
		if (w == m_CraftCountBtnPlus || (w && w.GetName() == "CraftCountButtonPlus"))
		{
			if (!m_SelectedRecipe)
				return true;
			int maxN = GetMaxCraftable(m_SelectedRecipe);
			int newN = Math.Clamp(m_SelectedRecipe.CurrentCraftCount + 1, 1, Math.Max(1, maxN));
			m_SelectedRecipe.CurrentCraftCount = newN;
			FillRecipeDetails(m_SelectedRecipe);
			return true;
		}

		if (w == m_CraftCountBtnMin || (w && w.GetName() == "CraftCountButtonMin"))
		{
			if (!m_SelectedRecipe)
				return true;
			int newNMin = Math.Max(1, m_SelectedRecipe.CurrentCraftCount - 1);
			m_SelectedRecipe.CurrentCraftCount = newNMin;
			FillRecipeDetails(m_SelectedRecipe);
			return true;
		}
		return false;
	}

	protected bool HandleActionButtonsClick(Widget w)
	{
		Widget cur = w;
		while (cur)
		{
			if (cur == m_CraftButton || cur.GetName() == "CraftButton")
			{
				DRL_CraftProcessEntryWidget curEntry = GetProcessEntryForCurrentRecipe();
				if (curEntry)
				{
					CancelActiveCraftProcess(curEntry.GetProcessId());
					return true;
				}
				if (!CanCraftSelected())
				{
					if (m_CraftButton)
						m_CraftButton.Enable(false);
					return true;
				}
				TryStartCraft();
				return true;
			}
			if (cur == m_AdditionalTasksButton || cur.GetName() == "AdditionalTasksButton")
			{
				if (m_AdditionalTasksWidget)
				{
					bool showTasks = !m_AdditionalTasksWidget.IsVisible();
					m_AdditionalTasksWidget.Show(showTasks);
					if (showTasks)
					{
						HideVariationList();
						BuildAdditionalTasksUI(m_SelectedRecipe);
					}
					UpdateUnderlyingOverlaysState();
				}
				return true;
			}
			if (cur.GetName() == "ReqButton")
			{
				DRL_Craft_Recipe navRec;
				cur.GetUserData(navRec);
				if (navRec)
				{
					SelectRecipeDirectly(navRec);
					return true;
				}
			}
			if (cur.GetName() == "LearningButton" || cur == m_LearningBtn)
			{
				CloseMenuSelf();
				GetGame().GetUIManager().EnterScriptedMenu(9503, null);
				return true;
			}
			if (cur.GetName() == "ButtonOut" || cur == m_BtnClose)
			{
				CloseMenuSelf();
				return true;
			}
			if (cur == m_ReCraftButton || cur.GetName() == "ReCraftButton")
			{
				if (m_ReCraftActive)
					return true;
				TryStartReCraft();
				return true;
			}
			cur = cur.GetParent();
		}
		return false;
	}

	protected bool HandleCategorySelectionClick(Widget w)
	{
		Widget hit = w;
		while (hit)
		{
			if (hit.GetName() == "CategorySelectionButtonWidget")
			{
				int visIdx = hit.GetUserID();
				int realIdx = visIdx;
				if (m_VisibleCategories && visIdx >= 0 && visIdx < m_VisibleCategories.Count())
					realIdx = m_VisibleCategories[visIdx];
				SelectCategory(realIdx);
				return true;
			}
			hit = hit.GetParent();
		}
		return false;
	}

	protected bool HandleRecipeSelectionClick(Widget w)
	{
		if (w.GetName() != "RecipesSelectionButtonWidget")
			return false;

		DRL_Craft_Recipe r;
		w.GetUserData(r);
		if (!r && w.GetParent())
			w.GetParent().GetUserData(r);
		if (!r)
			return false;

		if (m_SelectedRecipe != r)
		{
			if (m_SelectedComponentVariations)
			{
				m_SelectedComponentVariations.Clear();
			}
		}
		m_SelectedRecipe = r;
		if (m_AttachmentsController)
			m_AttachmentsController.ClearSelectedIds();
		HideVariationList();
		HideAdditionalTasks();
		if (m_RightPanel)
			m_RightPanel.Show(true);

		DRL_Craft_Category cat2 = null;
		DRL_Craft_Config cfg2 = GetCraft_Config();
		if (cfg2 && m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg2.Categories.Count())
		{
			DRL_Craft_Category selectedCat = cfg2.Categories[m_CurrentCategoryIdx];
			if (selectedCat && selectedCat.ShowAll != 1)
				cat2 = selectedCat;
			else
				cat2 = DRL_FindCategoryForRecipe(r);
		}

		DRL_ApplyFonBackground(cat2);
		DRL_ApplyCraftFonBackground(cat2);

		m_SelectedRecipe.CurrentCraftCount = 1;
		m_SelectedResultClass = "";

		FillRecipeDetails(r);
		BuildAttachedRecipesList(r);
		UpdateOptionalTitles(r);
		UpdateRecipeSelectionHighlight();
		RefreshCraftCountText();
		RefreshTimeTextForCount();
		return true;
	}

	protected bool HandleVariationIngrClick(Widget w)
	{
		Widget curIngr = w;
		int depth = 0;
		while (curIngr && depth < 30)
		{
			if (curIngr.GetName() == "CraftIngridientButton")
			{
				if (m_VariationCraftIngridientListWrap && IsWidgetChildOf(m_VariationCraftIngridientListWrap, curIngr))
				{
					Class udVar = null;
					curIngr.GetUserData(udVar);
					DRL_VariationClickInfo clickInfo = DRL_VariationClickInfo.Cast(udVar);

					if (clickInfo && clickInfo.MergedComp)
					{
						string chosenVariant = clickInfo.VariantClass;
						int targetKey = clickInfo.TargetKey;

						if (targetKey >= 0)
						{
							m_SelectedComponentVariations.Set(targetKey, chosenVariant);
						}
						else if (clickInfo.MergedComp.ComponentKeys)
						{
							for (int vk = 0; vk < clickInfo.MergedComp.ComponentKeys.Count(); vk++)
							{
								int compKey = clickInfo.MergedComp.ComponentKeys[vk];
								DRL_Craft_Component compRef = GetComponentByKey(compKey);
								if (compRef)
								{
									array<string> compVars = GetVariationsForComponent(compRef);
									if (compVars && compVars.Find(chosenVariant) != -1)
									{
										m_SelectedComponentVariations.Set(compKey, chosenVariant);
									}
									else
									{
										if (compVars && compVars.Count() > 0)
											m_SelectedComponentVariations.Set(compKey, compVars[0]);
										else
											m_SelectedComponentVariations.Set(compKey, compRef.Classname);
									}
								}
								else
								{
									m_SelectedComponentVariations.Set(compKey, chosenVariant);
								}
							}
						}

						HideVariationList();
						FillRecipeDetails(m_SelectedRecipe);
						RefreshCraftCountText();
						RefreshTimeTextForCount();
						UpdateCraftButtonState();
						return true;
					}
				}
				else
				{
					Class udMain = null;
					curIngr.GetUserData(udMain);
					DRL_CraftMergedComponent mcMain = DRL_CraftMergedComponent.Cast(udMain);
					if (mcMain && mcMain.EnableVariationSelection == 1)
					{
						array<string> variationsMain = GetVariationsForMergedComponent(mcMain);
						if (variationsMain && variationsMain.Count() > 1)
						{
							if (m_VariationIngridientListWidget && m_VariationIngridientListWidget.IsVisible() && m_ActiveVariationMergedComponent == mcMain)
							{
								HideVariationList();
							}
							else
							{
								HideAdditionalTasks();
								m_ActiveVariationMergedComponent = mcMain;
								int firstKey = 0;
								if (mcMain.ComponentKeys && mcMain.ComponentKeys.Count() > 0)
								{
									firstKey = mcMain.ComponentKeys[0];
								}
								BuildVariationListUI(mcMain, firstKey);
							}
							return true;
						}
					}
				}
			}
			curIngr = curIngr.GetParent();
			depth++;
		}
		return false;
	}

	protected bool HandleSelectResultClick(Widget w)
	{
		int idx; string cls;
		if (!DRL_TryPickResultFromWidget(w, idx, cls))
			return false;

		if (m_SelectedRecipe && m_SelectedRecipe.ResultClasses && idx >= 0 && idx < m_SelectedRecipe.ResultClasses.Count())
			cls = m_SelectedRecipe.ResultClasses[idx];

		if (cls == "")
			return true;

		m_SelectedResultClass = cls;
		for (int i = 0; i < m_SelectRows.Count(); i++)
		{
			bool sel = (i == idx);
			ColorizeSelectButton(m_SelectRows[i], sel);
		}

		UpdateSelectCraftHighlight();
		UpdateResultPreview(m_SelectedRecipe);
		UpdateResultStats(m_SelectedResultClass);
		RebuildMainPreviewWithAttachments();
		RenderAttachmentIconsForClass(m_SelectedResultClass);
		TextWidget wName = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoName"));
		if (wName)
		{
			string dn = "";
			if (!GetDisplayNameForClass(m_SelectedResultClass, dn))
				dn = m_SelectedResultClass;
			wName.SetText(string.Format("#STR_DRL_NAME %1", dn));
		}
		UpdateReCraftButtonState();
		Widget ingrWrap = m_Root.FindAnyWidget("CraftIngridientListWrap");
		if (ingrWrap && m_SelectedRecipe)
		{
			array<EntityAI> cachedItems;
			DRL_GetInventorySnapshot(cachedItems);
			array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(m_SelectedRecipe);
			BuildIngredientsListUI(m_SelectedRecipe, ingrWrap, mergedList, cachedItems);
		}
		UpdateCraftButtonState();
		bool canClient = CanCraftRecipe(m_SelectedRecipe) && DRL_ClientHasRequiredAttachments(m_SelectedRecipe);
		UpdateCraftButtonColor(canClient);
		if (m_RequirementsController && m_AdditionalTasksWidget && m_AdditionalTasksWidget.IsVisible())
		{
			m_RequirementsController.BuildUI(m_SelectedRecipe);
		}
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (HandleCraftCountClick(w))
			return true;

		if (button == MouseState.LEFT && w)
		{
			if (m_AdditionalTasksWidget && m_AdditionalTasksWidget.IsVisible())
			{
				bool isInsideTasks = (w == m_AdditionalTasksWidget || IsWidgetChildOf(m_AdditionalTasksWidget, w));
				bool isTasksBtn = false;
				Widget curT = w;
				while (curT)
				{
					if (curT == m_AdditionalTasksButton || curT.GetName() == "AdditionalTasksButton")
					{
						isTasksBtn = true;
						break;
					}
					curT = curT.GetParent();
				}
				if (!isInsideTasks && !isTasksBtn)
				{
					HideAdditionalTasks();
				}
			}

			if (m_VariationIngridientListWidget && m_VariationIngridientListWidget.IsVisible())
			{
				bool isInsideVar = (w == m_VariationIngridientListWidget || IsWidgetChildOf(m_VariationIngridientListWidget, w));
				bool isIngrBtn = false;
				Widget curV = w;
				while (curV)
				{
					if (curV.GetName() == "CraftIngridientButton")
					{
						isIngrBtn = true;
						break;
					}
					curV = curV.GetParent();
				}
				if (!isInsideVar && !isIngrBtn)
				{
					HideVariationList();
				}
			}

			if (m_AttachmentsController && m_AttachmentsController.HandleDopWidgetClick(w))
				return true;
			if (HandleCraftProcessItemClick(w))
				return true;
			if (HandleActionButtonsClick(w))
				return true;
			if (HandleCategorySelectionClick(w))
				return true;
			if (HandleRecipeSelectionClick(w))
				return true;
			if (HandleVariationIngrClick(w))
				return true;
			if (HandleSelectResultClick(w))
				return true;
		}

		return super.OnClick(w, x, y, button);
	}

	void RefreshCraftCountText()
	{
		if (!m_CraftCountText)
			return;
		if (m_SelectedRecipe)
			m_CraftCountText.SetText(string.Format("%1", m_SelectedRecipe.ResultCount * Math.Max(1, m_SelectedRecipe.CurrentCraftCount)));
		else
			m_CraftCountText.SetText("");
	}

	void RefreshTimeTextForCount()
	{
		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (!timeW)
			return;

		int t = 0;
		if (m_SelectedRecipe)
			t = Math.Max(0, GetTotalCraftTimeSec(m_SelectedRecipe) * Math.Max(1, m_SelectedRecipe.CurrentCraftCount));

		timeW.SetText(DRL_CraftTimeLabel(t));
	}

	int ComputeHaveForClass(string cls, int minHealthFrac, array<EntityAI> cachedItems = null, float quantityFrac = 0.0, bool useInheritance = false)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftInventoryHelper.ComputeHaveForClass(cls, minHealthFrac, cachedItems, quantityFrac, useInheritance, p, m_ContextWorkbench);
	}

	int ComputeHaveForComponent(DRL_Craft_Component c, int compIdx = -1, array<EntityAI> cachedItems = null)
	{
		string activeCls = "";
		if (compIdx >= 0)
		{
			activeCls = GetActiveComponentClass(c, compIdx);
		}
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return DRL_CraftInventoryHelper.ComputeHaveForComponent(c, activeCls, cachedItems, p, m_ContextWorkbench);
	}

	bool CanCraftSelected()
	{
		if (GetProcessEntryForCurrentRecipe() != null)
			return false;
		if (!m_SelectedRecipe)
			return false;

		int maxQueueSlots = 5;
		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg && cfg.Settings && cfg.Settings.MaxQueueSlots > 0)
		{
			maxQueueSlots = cfg.Settings.MaxQueueSlots;
		}

		if (m_ProcessWidgets && m_ProcessWidgets.Count() >= maxQueueSlots)
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
			{
				activeCls = c.Classname;
			}
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, null, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need)
				return false;
		}
		if (!DRL_ClientHasRequiredAttachments(m_SelectedRecipe))
			return false;
		return true;
	}

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
				{
					activeMergedCls = cMerged.Classname;
				}
				int haveMerged = ComputeHaveForClass(activeMergedCls, cMerged.MinHealthFrac, cachedItems, cMerged.QuantityFrac, cMerged.UseInheritance == 1);
				int needMerged = NeedForClassCount(activeMergedCls, cMerged, cnt);
				if (haveMerged < needMerged)
					return false;
			}
		}
		else
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
		if (!DRL_ClientHasRequiredAttachments(r))
			return false;
		return true;
	}

	protected void UpdateCraftButtonColor(bool canCraft)
	{
		if (!m_CraftButtonPanel)
			return;

		DRL_CraftProcessEntryWidget activeEntry = GetProcessEntryForCurrentRecipe();
		if (activeEntry)
		{
			if (activeEntry.IsQueued())
				m_CraftButtonPanel.SetColor(ARGB(255, 180, 70, 40));
			else
				m_CraftButtonPanel.SetColor(ARGB(255, 200, 60, 60));
			return;
		}

		if (canCraft)
		{
			m_CraftButtonPanel.SetColor(m_CraftBtnColorDefault);
		}
		else
		{
			m_CraftButtonPanel.SetColor(ARGB(30, 200, 60, 60));
		}
	}

	protected int ComputeNeedTotalForClass(string cls, DRL_Craft_Component c)
	{
		return DRL_CraftInventoryHelper.ComputeNeedTotalForClass(cls, c);
	}

	protected int ComputeNeedTotalForComponent(DRL_Craft_Component c)
	{
		return DRL_CraftInventoryHelper.ComputeNeedTotalForComponent(c);
	}

	protected bool GetDescriptionShortForClass(string cls, out string outText)
	{
		outText = "";
		if (cls == "")
			return false;

		string path;

		path = "CfgVehicles " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "")
				return true;
		}

		path = "CfgWeapons " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "")
				return true;
		}

		path = "CfgMagazines " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "")
				return true;
		}

		return false;
	}

	protected DRL_Craft_Category DRL_FindCategoryForRecipe(DRL_Craft_Recipe recipe)
	{
		if (!recipe)
		{
			return null;
		}

		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories)
		{
			return null;
		}

		for (int i = 0; i < cfg.Categories.Count(); i++)
		{
			DRL_Craft_Category cat = cfg.Categories[i];
			if (!cat)
			{
				continue;
			}

			if (cat.UseList != 1)
			{
				continue;
			}

			if (!cat.List)
			{
				continue;
			}

			for (int j = 0; j < cat.List.Count(); j++)
			{
				if (cat.List[j] == recipe.RecipeId)
				{
					return cat;
				}
			}
		}

		return null;
	}

	protected string DRL_GetCraftSoundSetForRecipe(DRL_Craft_Recipe r)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.Settings)
		{
			return "";
		}

		string snd = cfg.Settings.CraftSoundSet;

		DRL_Craft_Category cat = null;

		if (cfg.Categories && m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg.Categories.Count())
		{
			DRL_Craft_Category selectedCat = cfg.Categories[m_CurrentCategoryIdx];
			if (selectedCat && selectedCat.ShowAll != 1)
			{
				cat = selectedCat;
			}
			else
			{
				cat = DRL_FindCategoryForRecipe(r);
			}
		}

		if (cat)
		{
			if (cat.CategoryCraftSoundSet != "")
			{
				snd = cat.CategoryCraftSoundSet;
			}
		}

		return snd;
	}

	protected bool GetDisplayNameForClass(string cls, out string outName)
	{
		outName = "";
		if (cls == "")
			return false;

		string path;

		path = "CfgVehicles " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "")
				return true;
		}

		path = "CfgWeapons " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "")
				return true;
		}

		path = "CfgMagazines " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "")
				return true;
		}

		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (!w)
			return super.OnMouseEnter(w, x, y);

		if (m_CraftProcessGridSpacer && IsWidgetChildOf(m_CraftProcessGridSpacer, w))
		{
			Widget curProc = w;
			int depthProc = 0;
			while (curProc && curProc != m_CraftProcessGridSpacer && depthProc < 30)
			{
				Class udProc = null;
				curProc.GetUserData(udProc);
				DRL_CraftProcessEntryWidget procEntry = DRL_CraftProcessEntryWidget.Cast(udProc);
				if (procEntry)
				{
					procEntry.SetCancelButtonVisible(true);
					break;
				}
				curProc = curProc.GetParent();
				depthProc++;
			}
		}

		DRL_Craft_Component comp = null;
		Widget owner = null;

		Widget cur = w;
		int depth = 0;
		while (cur && depth < 30)
		{
			Class ud = null;
			cur.GetUserData(ud);
			if (ud)
			{
				DRL_CraftMergedComponent mcTmp = DRL_CraftMergedComponent.Cast(ud);
				if (mcTmp)
				{
					comp = mcTmp.ToCraftComponent();
					owner = cur;
					break;
				}
				DRL_Craft_Component tmp = DRL_Craft_Component.Cast(ud);
				if (tmp)
				{
					comp = tmp;
					owner = cur;
					break;
				}
				DRL_VariationClickInfo vci = DRL_VariationClickInfo.Cast(ud);
				if (vci && vci.MergedComp)
				{
					comp = vci.MergedComp.ToCraftComponent();
					owner = cur;
					break;
				}
			}
			cur = cur.GetParent();
			depth++;
		}

		if (comp && owner && (owner.GetName() == "CraftIngridientButton" || owner.GetName() == "AtachIngridientButton"))
		{
			string cls = comp.Classname;
			if (owner.GetName() == "CraftIngridientButton")
			{
				if (m_VariationCraftIngridientListWrap && IsWidgetChildOf(m_VariationCraftIngridientListWrap, owner))
				{
					Class udOwner = null;
					owner.GetUserData(udOwner);
					DRL_VariationClickInfo clickInfoOwner = DRL_VariationClickInfo.Cast(udOwner);
					if (clickInfoOwner && clickInfoOwner.VariantClass != "")
					{
						cls = clickInfoOwner.VariantClass;
					}
					else
					{
						int varIdx = owner.GetUserID();
						array<string> vList = GetVariationsForComponent(comp);
						if (vList && varIdx >= 0 && varIdx < vList.Count())
						{
							cls = vList[varIdx];
						}
					}
				}
				else
				{
					cls = comp.Classname;
				}
			}

			if (m_IngrDetailPanel)
			{
				if (m_IngrDetailName)
				{
					string dn = cls;
					GetDisplayNameForClass(cls, dn);
					m_IngrDetailName.SetText(dn);
				}

				if (m_IngrDetailDesc)
				{
					string ds = "";
					if (!GetDescriptionShortForClass(cls, ds))
						ds = "";
					m_IngrDetailDesc.SetText(ds);
				}

				FillIngredientDetailInfo(comp);

				m_IngrDetailPanel.Show(true);
			}
			return true;
		}

		return super.OnMouseEnter(w, x, y);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (!w)
			return super.OnMouseLeave(w, enterW, x, y);

		DRL_CraftProcessEntryWidget leaveEntry = null;
		if (m_CraftProcessGridSpacer && IsWidgetChildOf(m_CraftProcessGridSpacer, w))
		{
			Widget curLeave = w;
			int depthLeave = 0;
			while (curLeave && curLeave != m_CraftProcessGridSpacer && depthLeave < 30)
			{
				Class udL = null;
				curLeave.GetUserData(udL);
				leaveEntry = DRL_CraftProcessEntryWidget.Cast(udL);
				if (leaveEntry)
					break;
				curLeave = curLeave.GetParent();
				depthLeave++;
			}

			if (leaveEntry)
			{
				if (!enterW || !IsWidgetChildOf(leaveEntry.GetRoot(), enterW))
				{
					leaveEntry.SetCancelButtonVisible(false);
				}
			}
		}

		DRL_Craft_Component nextComp = null;
		Widget nextOwner = null;

		if (enterW)
		{
			Widget curNext = enterW;
			int depthEnter = 0;
			while (curNext && depthEnter < 30)
			{
				Class ud = null;
				curNext.GetUserData(ud);
				if (ud)
				{
					DRL_CraftMergedComponent mcNext = DRL_CraftMergedComponent.Cast(ud);
					if (mcNext)
					{
						nextComp = mcNext.ToCraftComponent();
						nextOwner = curNext;
						break;
					}
					DRL_Craft_Component tmp = DRL_Craft_Component.Cast(ud);
					if (tmp)
					{
						nextComp = tmp;
						nextOwner = curNext;
						break;
					}
					DRL_VariationClickInfo vciNext = DRL_VariationClickInfo.Cast(ud);
					if (vciNext && vciNext.MergedComp)
					{
						nextComp = vciNext.MergedComp.ToCraftComponent();
						nextOwner = curNext;
						break;
					}
				}
				curNext = curNext.GetParent();
				depthEnter++;
			}
		}

		if (!nextComp || !nextOwner || (nextOwner.GetName() != "CraftIngridientButton" && nextOwner.GetName() != "AtachIngridientButton"))
		{
			if (m_IngrDetailPanel)
				m_IngrDetailPanel.Show(false);
		}

		return super.OnMouseLeave(w, enterW, x, y);
	}

	protected void UpdateCraftFonColor()
	{
		if (!m_CraftFonImage)
		{
			return;
		}

		bool isRunning = false;
		DRL_CraftProcessEntryWidget activeEntry = GetProcessEntryForCurrentRecipe();
		if (activeEntry && !activeEntry.IsQueued())
		{
			isRunning = true;
		}

		if (m_ReCraftActive)
		{
			isRunning = true;
		}

		if (isRunning)
		{
			m_CraftFonImage.SetColor(COLOR_CRAFT_FON_ACTIVE);
		}
		else
		{
			m_CraftFonImage.SetColor(COLOR_CRAFT_FON_DEFAULT);
		}
	}

	void UpdateCraftButtonState()
	{
		UpdateAdditionalTasksButtonState();
		UpdateCraftFonColor();

		if (!m_CraftButton || !m_ButtonCraftText)
			return;

		DRL_CraftProcessEntryWidget activeEntry = GetProcessEntryForCurrentRecipe();
		m_CraftActive = (activeEntry != null);
		if (activeEntry)
		{
			m_CraftButton.Enable(true);
			if (activeEntry.IsQueued())
			{
				m_ButtonCraftText.SetText("#STR_DRL_IN_QUEUE_CANCEL");
				if (m_CraftButtonPanel)
					m_CraftButtonPanel.SetColor(ARGB(255, 180, 70, 40));
				if (m_CraftPB)
					m_CraftPB.Show(false);
			}
			else
			{
				m_ButtonCraftText.SetText(Widget.TranslateString("#STR_CANCEL"));
				if (m_CraftButtonPanel)
					m_CraftButtonPanel.SetColor(ARGB(255, 200, 60, 60));
				if (m_CraftPB)
				{
					m_CraftPB.SetCurrent(activeEntry.GetCurrentProgressPercent());
					m_CraftPB.Show(true);
				}
			}
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			return;
		}

		if (m_CraftPB)
		{
			m_CraftPB.SetCurrent(0);
		}

		if (!m_SelectedRecipe)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_No_Recipe");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			return;
		}

		bool isLearned = DRL_ClientHasLearnedRecipe(m_SelectedRecipe.RecipeId);
		if (!isLearned)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_LearningLockTitel");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(ARGB(255, 170, 40, 40));
			return;
		}

		bool lackAttachments = !DRL_ClientHasRequiredAttachments(m_SelectedRecipe);
		if (lackAttachments)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_DRL_NEED_ATTACHMENT");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(ARGB(255, 170, 40, 40));
			return;
		}

		bool lackIngredients = !CanCraftRecipe(m_SelectedRecipe);
		if (lackIngredients)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_DRL_NO_INGREDIENTS");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(ARGB(30, 200, 60, 60));
			return;
		}

		int maxQueueSlots = 5;
		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg && cfg.Settings && cfg.Settings.MaxQueueSlots > 0)
		{
			maxQueueSlots = cfg.Settings.MaxQueueSlots;
		}

		int currentActiveCount = 0;
		if (m_ProcessWidgets)
		{
			currentActiveCount = m_ProcessWidgets.Count();
		}

		if (currentActiveCount >= maxQueueSlots)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText(string.Format("#STR_DRL_QUEUE_FULL (%1/%2)", currentActiveCount, maxQueueSlots));
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_error image:error");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(ARGB(255, 170, 40, 40));
			return;
		}

		m_CraftButton.Enable(true);
		if (currentActiveCount > 0)
		{
			m_ButtonCraftText.SetText(string.Format("#STR_DRL_TO_QUEUE (%1/%2)", currentActiveCount + 1, maxQueueSlots));
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_recycling image:recycling");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(m_CraftBtnColorDefault);
		}
		else
		{
			m_ButtonCraftText.SetText("#STR_DRL_START");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "set:drl_craft_recycling image:recycling");
			if (m_CraftButtonPanel)
				m_CraftButtonPanel.SetColor(m_CraftBtnColorDefault);
		}
	}

	protected void UpdateAdditionalTasksButtonState()
	{
		if (!m_AdditionalTasksButton)
			return;

		if (!m_SelectedRecipe)
		{
			m_AdditionalTasksButton.Show(false);
			return;
		}

		m_AdditionalTasksButton.Show(true);

		bool allMet = DRL_AreAllRecipeRequirementsMet(m_SelectedRecipe);
		if (m_AdditionalTasksButtonPanel)
		{
			if (allMet)
			{
				m_AdditionalTasksButtonPanel.SetColor(ARGB(255, 60, 200, 60));
			}
			else
			{
				m_AdditionalTasksButtonPanel.SetColor(ARGB(255, 200, 60, 60));
			}
		}

		if (m_AdditionalTasksWidget && m_AdditionalTasksWidget.IsVisible())
		{
			BuildAdditionalTasksUI(m_SelectedRecipe);
		}
	}

	protected void TryStartCraft()
	{
		if (!m_SelectedRecipe)
			return;
		if (GetProcessEntryForCurrentRecipe() != null)
			return;

		int maxQueueSlots = 5;
		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg && cfg.Settings && cfg.Settings.MaxQueueSlots > 0)
		{
			maxQueueSlots = cfg.Settings.MaxQueueSlots;
		}

		if (m_ProcessWidgets && m_ProcessWidgets.Count() >= maxQueueSlots)
			return;

		if (!DRL_ClientHasLearnedRecipe(m_SelectedRecipe.RecipeId))
		{
			if (m_CraftButton)
				m_CraftButton.Enable(false);
			if (m_ButtonCraftText)
				m_ButtonCraftText.SetText("#STR_LearningLockTitel");
			return;
		}

		if (!DRL_ClientHasRequiredAttachments(m_SelectedRecipe))
		{
			if (m_CraftButton)
				m_CraftButton.Enable(false);
			if (m_ButtonCraftText)
				m_ButtonCraftText.SetText("#STR_DRL_NEED_ATTACHMENT");
			return;
		}

		array<ref DRL_CraftMergedComponent> mergedList = BuildMergedComponents(m_SelectedRecipe);
		bool hasAll = true;
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
			{
				activeCls = c.Classname;
			}
			int have = ComputeHaveForClass(activeCls, c.MinHealthFrac, null, c.QuantityFrac, c.UseInheritance == 1);
			int need = NeedForClassCount(activeCls, c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need) { hasAll = false; break; }
		}
		if (!hasAll)
			return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		string chosenCls = m_SelectedRecipe.Result;
		if (m_SelectedResultClass != "" && m_SelectedRecipe.ResultClasses && m_SelectedRecipe.ResultClasses.Find(m_SelectedResultClass) > -1)
			chosenCls = m_SelectedResultClass;

		int craftCount = Math.Max(1, m_SelectedRecipe.CurrentCraftCount);
		int netLow = 0;
		int netHigh = 0;

		if (m_ContextWorkbench)
		{
			m_ContextWorkbench.GetNetworkID(netLow, netHigh);
		}

		vector aimPos = DRL_GetCrosshairAimPosition();

		array<int> compVarKeys = new array<int>();
		array<string> compVarClasses = new array<string>();
		if (m_SelectedRecipe && m_SelectedRecipe.CraftComponents)
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
		if (selectedAttached && selectedAttached.Count() > 0)
		{
			if (GetCraft_Config())
			{
				for (int saIdx = 0; saIdx < selectedAttached.Count(); saIdx++)
				{
					int saId = selectedAttached[saIdx];
					DRL_Craft_Recipe saR = GetCraft_Config().FindById(saId);
					if (saR && saR.CraftComponents)
					{
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
				}
			}

			vector netWb = Vector(netLow, netHigh, 0);
			array<string> attVariantClasses = new array<string>();
			for (int a = 0; a < selectedAttached.Count(); a++)
			{
				int dopId = selectedAttached[a];
				DRL_Craft_Recipe dopR = GetCraft_Config().FindById(dopId);
				string attCls = "";
				if (m_AttachmentsController)
				{
					attCls = m_AttachmentsController.GetSelectedAttachedClass(dopId, dopR);
				}
				else if (dopR)
				{
					attCls = dopR.Result;
				}
				attVariantClasses.Insert(attCls);
			}

			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CRAFT_WITH_ATTACHMENTS, new Param9<int, string, int, vector, array<int>, vector, array<string>, array<int>, array<string>>(m_SelectedRecipe.RecipeId, chosenCls, craftCount, netWb, selectedAttached, aimPos, attVariantClasses, compVarKeys, compVarClasses), true, null);
		}
		else
		{
			GetGame().RPCSingleParam(player, DRL_CraftRPC.CTS_CRAFT, new Param8<int, string, int, int, int, vector, array<int>, array<string>>(m_SelectedRecipe.RecipeId, chosenCls, craftCount, netLow, netHigh, aimPos, compVarKeys, compVarClasses), true, null);
		}
	}

	vector DRL_GetCrosshairAimPosition()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
		{
			return "0 0 0";
		}

		return p.GetPosition();
	}

	protected void DRL_StopCraftSound()
	{
		if (m_CraftSound)
		{
			m_CraftSound.SoundStop();
		}
	}

	protected void DRL_PlayCraftSoundForRecipe(DRL_Craft_Recipe r)
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
		{
			return;
		}

		string snd = DRL_GetCraftSoundSetForRecipe(r);
		if (snd == "")
		{
			return;
		}

		if (m_CraftSound && m_CraftSound.IsSoundPlaying())
		{
			m_CraftSound.SoundStop();
		}

		pb.PlaySoundSetLoop(m_CraftSound, snd, 0, 0);
	}

	protected void UpdateWBName()
	{
		if (!m_WBNameText)
		{
			return;
		}

		string txt = "#STR_WBName";

		if (m_ContextWorkbench)
		{
			string dn = m_ContextWorkbench.GetDisplayName();
			if (dn != string.Empty)
			{
				txt = dn;
			}
		}

		m_WBNameText.SetText(txt);
	}

	void ClearChildren(Widget parent)
	{
		DRL_CraftUIHelper.ClearChildren(parent);
	}
}