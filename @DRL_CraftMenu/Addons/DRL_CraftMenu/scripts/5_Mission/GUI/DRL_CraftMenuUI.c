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
	protected Widget 				m_AllRecipesListWrap;
	protected Widget 				m_RecipesListSpacer;
	protected ButtonWidget 			m_BtnClose;
	
	protected WrapSpacerWidget 		m_SelectCraftListWrap;
	protected Widget           		m_SelectCraftPanel;
	protected string           		m_SelectedResultClass;
	
	protected ref array<Widget>     m_SelectRows = new array<Widget>(); 
	
	protected Widget               	m_IngrDetailPanel;
	protected TextWidget           	m_IngrDetailName;
	protected MultilineTextWidget  	m_IngrDetailDesc;
	
	protected TextWidget 			m_StatsTextW;
	
	protected Widget 				m_SelectCraftRecipeTitle;
	protected Widget 				m_AttachTitle;
	
	protected Widget 				m_AtachIngridientListWrap;
	protected ref array<EntityAI> 	m_AttachPrevDummies;
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
	
	protected Widget             	m_ReCraftPanel;
	protected Widget             	m_ReCraftListWrap;
		
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
		
	protected Widget 				m_CraftButtonPanel;
	protected int    				m_CraftBtnColorDefault;
		
	protected float             	m_CraftElapsed;
	protected float             	m_CraftDuration;
	protected bool              	m_CraftActive;
	
	protected ItemPreviewWidget 	m_ItemPreviewWidget0;
	
	protected int   				m_PrevMouseX;
	protected int   				m_PrevMouseY;
	protected vector 				m_PreviewOrientation;
	
	protected ref array	<EntityAI> 	m_DRL_PreviewTrash;
	protected EntityAI 				m_DRL_MainPreview;

	protected ref DRL_Craft_Recipe 	m_SelectedRecipe;
	
	protected EffectSound 			m_CraftSound;
	
	protected ImageWidget           m_FonImage;
	protected ImageWidget           m_CraftFonImage;
	
	protected EffectSound			m_DRL_CraftSound;
	
	protected string CATEGORY_TEMPLATE 		= "DRL_CraftMenu/UI/layout/Category.layout";
	protected string RECRAFT_TEMPLATE 		= "DRL_CraftMenu/UI/layout/ReCraft.layout";
	protected string RECIPE_TEMPLATE 		= "DRL_CraftMenu/UI/layout/Recipe.layout";
	protected string INGR_TEMPLATE   		= "DRL_CraftMenu/UI/layout/Ingredient.layout";
	static const string DRL_ATTACHMENT_CELL = "DRL_CraftMenu/UI/layout/Attachment.layout";
	
	protected ref map<string, EntityAI> m_PreviewCache = new map<string, EntityAI>();
	protected ref array<ItemPreviewWidget> m_AttachedPreviews = new array<ItemPreviewWidget>();

	override Widget Init()
	{
		m_Root 					= GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/drl_craft_manu.layout", null);
		
		m_FonImage       = ImageWidget.Cast(m_Root.FindAnyWidget("FonImageWidget"));
		m_CraftFonImage  = ImageWidget.Cast(m_Root.FindAnyWidget("CraftFonImageWidget"));
		m_WBNameText = TextWidget.Cast(m_Root.FindAnyWidget("WB_Name"));
	
		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg && cfg.Settings)
		{
			if (m_FonImage)
			{
				string fonPath = cfg.Settings.FonImagePath;
				if (fonPath != "")
				{
					m_FonImage.LoadImageFile(0, fonPath);
					m_FonImage.SetImage(0);
				}
			}
			
			if (m_CraftFonImage)
			{
				string craftFonPath = cfg.Settings.CraftFonImagePath;
				if (craftFonPath != "")
				{
					m_CraftFonImage.LoadImageFile(0, craftFonPath);
					m_CraftFonImage.SetImage(0);
				}
			}
		}
		
		m_SelectCraftPanel   	= m_Root.FindAnyWidget("SelectCraftPanelWidget");
		m_SelectCraftListWrap 	= WrapSpacerWidget.Cast(m_Root.FindAnyWidget("SelectCraftListWrap"));
		
		m_PlayerNameW 			= TextWidget.Cast(m_Root.FindAnyWidget("PlayerName"));
		m_SlotInfoW   			= TextWidget.Cast(m_Root.FindAnyWidget("SlotInfoText"));
		
		UpdatePlayerName();
		UpdateSlotInfo();
		UpdateWBName();
		
		m_StatsTextW 			= TextWidget.Cast(m_Root.FindAnyWidget("StatsText"));
	
		if (!m_SlotInfoTimer) m_SlotInfoTimer = new Timer(CALL_CATEGORY_GUI);
		m_SlotInfoTimer.Run(1.0, this, "UpdateSlotInfo", null, true); 
		
		m_CategoryListWrap 		= m_Root.FindAnyWidget("SpacerCategoryListWidget");
		m_CategoryTitle    		= TextWidget.Cast(m_Root.FindAnyWidget("AllCategoryTitel"));
		
		BuildCategoriesPanel();
		SelectCategory(FindDefaultCategory());
		
		m_ItemPreviewWidget0  	= ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));
		
		m_DRL_PreviewTrash 		= new array<EntityAI>();
		m_DRL_MainPreview  		= null;
		
		m_BtnClose 				= ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonOut"));
		m_RecipesListSpacer 	= m_Root.FindAnyWidget("SpacerAllRecipesListWidget");
		m_SearchBox 			= EditBoxWidget.Cast(m_Root.FindAnyWidget("SearchEditBox"));
		m_RightPanel 			= m_Root.FindAnyWidget("RigthPanelWidget");
		if (m_RightPanel) m_RightPanel.Show(false);
		
		m_SelectCraftRecipeTitle = m_Root.FindAnyWidget("SelectCraftRecipeTitelWidget");
		m_AttachTitle            = m_Root.FindAnyWidget("AtachIngridientListWidget");
		
		m_AtachIngridientListWrap = m_Root.FindAnyWidget("AtachIngridientListWrap");
		m_AttachPrevDummies 	= new array<EntityAI>();
		
		m_AttachmentIconsWrap 	= m_RightPanel.FindAnyWidget("AttachmentIconsWrap");
		
		m_ButtonCraftText 		= TextWidget.Cast(m_RightPanel.FindAnyWidget("ButtonCraftText"));
		m_ButtonCraftIcon 		= ImageWidget.Cast(m_RightPanel.FindAnyWidget("ButtonImageWidget"));
		
		m_IngrDetailPanel 		= m_Root.FindAnyWidget("CraftIngridientDetail");
		m_IngrDetailName  		= TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetailText"));
		m_IngrDetailDesc  		= MultilineTextWidget.Cast(m_Root.FindAnyWidget("IngridientDetailMultiline"));
		if (m_IngrDetailPanel) m_IngrDetailPanel.Show(false);
		
		Widget ingrWrap 		= m_Root.FindAnyWidget("CraftIngridientListWrap");
		m_AllRecipesListWrap 	= m_Root.FindAnyWidget("SpacerAllRecipesListWidget");
		
		m_CraftCountText 		= TextWidget.Cast(m_RightPanel.FindAnyWidget("CraftCountText"));
		m_CraftCountBtnPlus 	= ButtonWidget.Cast(m_Root.FindAnyWidget("CraftCountButtonPlus"));
		m_CraftCountBtnMin  	= ButtonWidget.Cast(m_Root.FindAnyWidget("CraftCountButtonMin"));
		
		m_ReCraftPanel        	= m_Root.FindAnyWidget("ReCraftRecipeDetail");
		m_ReCraftListWrap     	= m_Root.FindAnyWidget("ReCraftIngridientListWrap");
			
		m_ReCraftButton       	= ButtonWidget.Cast(m_Root.FindAnyWidget("ReCraftButton"));
		m_ReCraftPB           	= ProgressBarWidget.Cast(m_Root.FindAnyWidget("ReCraftButtonProgressBar"));
		m_ReCraftButtonPanel  	= m_Root.FindAnyWidget("ReButtonCraftPanelWidget");
		m_ReCraftButtonText   	= TextWidget.Cast(m_Root.FindAnyWidget("ReButtonCraftText"));
		m_ReCraftButtonIcon   	= ImageWidget.Cast(m_Root.FindAnyWidget("ReButtonImageWidget"));
		if (m_ReCraftPB)
		{
			m_ReCraftPB.SetCurrent(0);
			m_ReCraftPB.Show(true);
		}
		if (m_ReCraftPanel)
			m_ReCraftPanel.Show(false);
		
		m_WarningTextW 			= MultilineTextWidget.Cast(m_Root.FindAnyWidget("WarningText"));
		if (m_WarningTextW)
		{
			m_WarningTextW.SetText("#STR_DRL_WARNING_TEXT");
			m_WarningTextW.SetColor(ARGB(200, 120, 30, 30));
			m_WarningTextW.Show(true);
		}
		

		m_CraftButton 			= ButtonWidget.Cast(m_Root.FindAnyWidget("CraftButton"));
		m_CraftPB 				= ProgressBarWidget.Cast(m_Root.FindAnyWidget("CraftButtonProgressBar"));
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

		BuildRecipesList("");
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
	}

	override void OnShow()
	{
		super.OnShow();
		UpdatePlayerName();
		UpdateSlotInfo();
		UpdateWBName();
	
		MissionGameplay m = MissionGameplay.Cast(GetGame().GetMission());
		if (m && m.GetHud())
		{
			m.GetHud().ShowHudUI(false);
			m.GetHud().ShowQuickbarUI(false);
		}

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
	
		GetGame().GetInput().ChangeGameFocus(1);
		SetFocus(m_Root);
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
		
		if (m_SlotInfoTimer)
			m_SlotInfoTimer.Stop();
		
		DRL_CleanupPreview();
		ClearAttachDummies();
		
		DRL_StopCraftSound();
	}
	
	void ~DRL_CraftMenuUI()
	{
		DRL_StopCraftSound();
		DRL_CleanupPreview();
	}
	
	protected void UpdateOptionalTitles(DRL_Craft_Recipe r)
	{
		bool hasVariants = false;
		bool hasAttach   = false;
	
		if (r)
		{
			if (r.ResultClasses && r.ResultClasses.Count() > 1)
				hasVariants = true;
	
			if (r.GetAttachReqCount() > 0)
				hasAttach = true;
		}
	
		if (m_SelectCraftRecipeTitle)
			m_SelectCraftRecipeTitle.Show(hasVariants);
	
		if (m_AttachTitle)
			m_AttachTitle.Show(hasAttach);
	}
	
	protected bool DRL_IsFillingByClassUI(string cls)
	{
		if (cls == "") return false;
	
		string lower = cls;
		lower.ToLower();
	
		if (lower.Contains("ammo")) return false;
	
		int qbar = 0;
		if (DRL_GetConfigIntForClass(cls, "quantityBar", qbar))
		{
			if (qbar == 1)
				return true;
		}
	
		string unit;
		if (DRL_GetConfigStringForClass(cls, "stackedUnit", unit))
		{
			unit.ToLower();
			if (unit == "percentage")
				return true;
		}
	
		return false;
	}
	
	protected bool DRL_GetConfigIntForClass(string cls, string prop, out int val)
	{
		val = 0;
		if (cls == "") return false;
	
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
		if (cls == "") return false;
	
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
	
	protected void UpdateSlotInfo()
	{
		if (!m_SlotInfoW) return;
	
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
		{
			m_SlotInfoW.SetText("#STR_DRL_INV_SLOTS_EMPTY");
			return;
		}
	
		int totalCells = 0;
		int usedCells  = 0;
		
		array<EntityAI> items = new array<EntityAI>();
		pb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		
		if (m_ContextWorkbench && m_ContextWorkbench.GetInventory())
		{
			m_ContextWorkbench.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}
	
		for (int i = 0; i < items.Count(); i++)
		{
			EntityAI ent = items[i];
			if (!ent) continue;
	
			CargoBase cargo = ent.GetInventory().GetCargo();
			if (!cargo) continue;
			
			int w = cargo.GetWidth();
			int h = cargo.GetHeight();
			totalCells += (w * h);
			
			int cnt = cargo.GetItemCount();
			for (int k = 0; k < cnt; k++)
			{
				EntityAI item = cargo.GetItem(k);
				if (!item) continue;
			
				string cls = item.GetType();
				
				int sx = 1;
				int sy = 1;
				
				array<int> dims = new array<int>();
			
				string p = "CfgVehicles " + cls + " itemSize";
				if (GetGame().ConfigIsExisting(p))
				{
					GetGame().ConfigGetIntArray(p, dims);
				}
				if (dims.Count() < 2)
				{
					dims.Clear();
					p = "CfgWeapons " + cls + " itemSize";
					if (GetGame().ConfigIsExisting(p))
						GetGame().ConfigGetIntArray(p, dims);
				}
				if (dims.Count() < 2)
				{
					dims.Clear();
					p = "CfgMagazines " + cls + " itemSize";
					if (GetGame().ConfigIsExisting(p))
						GetGame().ConfigGetIntArray(p, dims);
				}
			
				if (dims.Count() >= 2)
				{
					sx = dims[0];
					sy = dims[1];
				}
			
				usedCells += (sx * sy);
			}
		}
		
		int freeCells = Math.Max(totalCells - usedCells, 0);
		m_SlotInfoW.SetText(" #STR_DRL_INV_TOTAL " + totalCells + " #STR_DRL_FREE_ONLY " + freeCells);
	}
	
	protected void DRL_CleanupPreview()
	{
		if (m_DRL_MainPreview)
		{
			GetGame().ObjectDelete(m_DRL_MainPreview);
			m_DRL_MainPreview = null;
		}
		
		if (m_DRL_PreviewTrash)
		{
			for (int i = 0; i < m_DRL_PreviewTrash.Count(); i++)
			{
				EntityAI e = m_DRL_PreviewTrash[i];
				if (e) GetGame().ObjectDelete(e);
			}
			m_DRL_PreviewTrash.Clear();
		}
		
		if (m_AttachedPreviews)
		{
			for (int ij = 0; ij < m_AttachedPreviews.Count(); ij++)
			{
				ItemPreviewWidget ipw = m_AttachedPreviews[ij];
				if (ipw) ipw.SetItem(null);
			}
			m_AttachedPreviews.Clear();
		}
		
		if (m_PreviewCache)
		{
			foreach (string k, EntityAI ej : m_PreviewCache)
			{
				if (ej) GetGame().ObjectDelete(ej);
			}
			m_PreviewCache.Clear();
		}
		
		array<Object> around = new array<Object>();
		GetGame().GetObjectsAtPosition("0 0 0", 1.5, around, NULL);
		for (int kj = 0; kj < around.Count(); kj++)
		{
			EntityAI ea = EntityAI.Cast(around[kj]);
			if (ea) GetGame().ObjectDelete(ea);
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
		for (int i = 0; i < m_AttachedPreviews.Count(); i++)
		{
			ItemPreviewWidget ipw = m_AttachedPreviews[i];
			if (ipw) ipw.SetItem(null);
		}
		m_AttachedPreviews.Clear();

		if (m_PreviewCache)
		{
			foreach (string k, EntityAI e : m_PreviewCache)
			{
				if (e) GetGame().ObjectDelete(e);
			}
			m_PreviewCache.Clear();
		}
		
		if (m_CraftTimer) m_CraftTimer.Stop();
		if (m_CraftProgressTimer) m_CraftProgressTimer.Stop();
		m_CraftActive = false;
		if (m_CraftPB) m_CraftPB.SetCurrent(0);
		
		Widget previewPanel2 = m_Root.FindAnyWidget("PreviewItemWidgetPanel");
		if (previewPanel2)
		{
			previewPanel2.Show(false);
		}
		
		if (m_ReCraftProgressTimer) m_ReCraftProgressTimer.Stop();
		m_ReCraftActive = false;
		UpdateReCraftButtonState();
		if (m_ReCraftPB) m_ReCraftPB.SetCurrent(0);
		
		DRL_StopCraftSound();
		
		PlayerBase pb2 = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb2)
		{
			pb2.DRL_SetCurrentWorkbench(null);
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
	
	protected EntityAI DRL_SpawnPreviewLocal(string cls)
	{
		if (!cls || cls == "") return null;
		EntityAI ent = EntityAI.Cast(GetGame().CreateObject(cls, "0 0 0", true, false, false));
		if (ent) m_DRL_PreviewTrash.Insert(ent);
		return ent;
	}
	
	void BuildCategoriesPanel()
	{
		if (!m_CategoryListWrap) return;
	
		ClearChildren(m_CategoryListWrap);
	
		auto cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories) return;
	
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
					icon.LoadImageFile(0, cfg.Categories[i].Icon);
					icon.SetImage(0);
				}
			}
	
			ButtonWidget btn = ButtonWidget.Cast(row.FindAnyWidget("CategorySelectionButtonWidget"));
			if (btn) btn.SetUserID(visibleIdx);
		}
	
		m_CategoryListWrap.Update();
	}
	
	int FindDefaultCategory()
	{
		auto cfg = GetCraft_Config();
		if (!cfg || !cfg.Categories) return -1;
		if (!m_VisibleCategories || m_VisibleCategories.Count() == 0) return -1;
		
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
		m_CurrentCategoryIdx = idx;
	
		auto cfg = GetCraft_Config();
		DRL_Craft_Category cat = null;
		if (cfg && idx >= 0 && idx < cfg.Categories.Count())
			cat = cfg.Categories[idx];
		
		if (m_CategoryTitle)
		{
			string title = "#STR_DRL_ALL_RECIPES";
			if (cat && cat.Name != "") title = cat.Name;
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
			m_FonImage.LoadImageFile(0, path);
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
			m_CraftFonImage.LoadImageFile(0, path);
			m_CraftFonImage.SetImage(0);
		}
	}
	
	void RebuildRecipesList_CurrentCategory()
	{
		BuildRecipesList();
	}
	
	protected bool RecipeAllowedByContext(DRL_Craft_Recipe r)
	{
		if (!r) return false;
		return r.ShowInFreeMenu == 1;
	}
	
	bool RecipePassesCategory(DRL_Craft_Recipe r)
	{
		auto cfg = GetCraft_Config();
		if (!cfg || m_CurrentCategoryIdx < 0 || m_CurrentCategoryIdx >= cfg.Categories.Count())
			return true;
	
		DRL_Craft_Category cat = cfg.Categories[m_CurrentCategoryIdx];
		if (!cat) return true;
	
		if (cat.ShowAll == 1) return true;

		if (cat.UseList == 1)
		{
			if (!cat.List) return false;
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
		if (!cat) return true;
	
		if (cat.ShowAll == 1) return true;
	
		if (cat.UseList == 1)
		{
			if (!cat.List) return false;
			for (int i = 0; i < cat.List.Count(); i++)
				if (r && r.GetId() == cat.List[i])
					return true;
			return false;
		}
		return true;
	}
	
	bool CategoryHasAnyRecipe(int catIdx)
	{
		if (!g_Craft_Config) return false;
		auto all = g_Craft_Config.GetAll();
		if (!all) return false;
	
		for (int i = 0; i < all.Count(); i++)
		{
			DRL_Craft_Recipe r = all[i];
			if (!r) continue;
			
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
		if (m_SelectCraftListWrap) ClearChildren(m_SelectCraftListWrap);
		
		if (m_SelectRows)
			m_SelectRows.Clear();
	
		if (!r || !r.ResultClasses || r.ResultClasses.Count() == 0)
		{
			if (m_SelectCraftPanel) m_SelectCraftPanel.Show(false);
			m_SelectedResultClass = "";
			UpdateResultPreview(r);
			return;
		}
	
		if (m_SelectCraftPanel) m_SelectCraftPanel.Show(true);
	
		for (int k = 0; k < r.ResultClasses.Count(); k++)
		{
			string cls = r.ResultClasses[k];
	
			Widget row = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/SelectCraft.layout", m_SelectCraftListWrap);
			
			m_SelectRows.Insert(row);

			ScriptedWidgetEventHandler h = null;
			Class.CastTo(h, this);
			if (h) row.SetHandler(h);
			
			row.SetUserID(k);
			
			ItemPreviewWidget prev = ItemPreviewWidget.Cast(row.FindAnyWidget("SelectCraftItemPreview"));
			if (prev) {
				EntityAI itm = GetPreviewEntity(cls);
				if (itm) prev.SetItem(itm);
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
			if (btn) {
				ScriptedWidgetEventHandler h2 = null;
				Class.CastTo(h2, this);
				if (h2) btn.SetHandler(h2);
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
				if (b) idx = b.GetUserID();
				return idx >= 0;
			}
	
			int id = cur.GetUserID();
			if (id >= 0) {
				idx = id;
				return true;
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
		if (!m_ReCraftButton) return;
		
		if (m_ReCraftActive)
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText) m_ReCraftButtonText.SetText("#STR_DRL_IN_PROGRESS");
			return;
		}
		
		if (!m_SelectedRecipe)
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText) m_ReCraftButtonText.SetText("#STR_DRL_NO_INGREDIENTS");
			return;
		}
		
		bool hasTarget = HasResultItemForRecraft(m_SelectedRecipe);
	
		if (hasTarget)
		{
			m_ReCraftButton.Enable(true);
			if (m_ReCraftButtonText) m_ReCraftButtonText.SetText("#STR_DRL_RECRAFT");
		}
		else
		{
			m_ReCraftButton.Enable(false);
			if (m_ReCraftButtonText) m_ReCraftButtonText.SetText("#STR_DRL_NO_INGREDIENTS");
		}
	}
	
	protected bool HasResultItemForRecraft(DRL_Craft_Recipe r)
	{
		if (!r) return false;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p) return false;
	
		string target = r.Result;
		if (m_SelectedResultClass != "" && r.ResultClasses && r.ResultClasses.Find(m_SelectedResultClass) > -1)
			target = m_SelectedResultClass;
	
		array<ItemBase> pool;
		p.DRL_CollectItemsByClass(target, 3, pool);
		return pool && pool.Count() > 0;
	}
	
	protected Widget DRL_FindDescendantByName(Widget root, string name)
	{
		if (!root) return null;
		if (root.GetName() == name) return root;
	
		Widget ch = root.GetChildren();
		while (ch)
		{
			Widget got = DRL_FindDescendantByName(ch, name);
			if (got) return got;
			ch = ch.GetSibling();
		}
		return null;
	}
	
	protected void ColorizeSelectButton(Widget rowOrBtn, bool selected)
	{
		if (!rowOrBtn) return;
		
		Widget tile = DRL_FindDescendantByName(rowOrBtn, "SelectCraftItemPanelW");
		if (!tile) return;
		
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
		if (!m_SelectedRecipe) return -1;
		if (!m_SelectedRecipe.ResultClasses) return -1;
	
		for (int i = 0; i < m_SelectedRecipe.ResultClasses.Count(); i++)
		{
			string c = m_SelectedRecipe.ResultClasses[i];
			if (c == cls) return i;
		}
		return -1;
	}
	
	protected void UpdateSelectCraftHighlight()
	{
		if (!m_SelectedRecipe) return;
		
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
			if (i == selIndex) selected = true;
			ColorizeSelectButton(m_SelectRows[i], selected);
		}
	}
	
	void BuildReCraftList(DRL_Craft_Recipe r)
	{
		if (!m_ReCraftListWrap) return;
	
		ClearChildren(m_ReCraftListWrap);
	
		bool hasDrops = false;
	
		if (r)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c) continue;
	
				if (c.RecraftDropChance <= 0.0)
					continue;
	
				hasDrops = true;
	
				Widget slot = GetGame().GetWorkspace().CreateWidgets(RECRAFT_TEMPLATE, m_ReCraftListWrap);
				if (!slot) continue;
	
				slot.SetUserData(c);
	
				ItemPreviewWidget prev = ItemPreviewWidget.Cast(slot.FindAnyWidget("ReCraftIngridientItemPreview"));
				TextWidget qty         = TextWidget.Cast(slot.FindAnyWidget("ReCraftIngridientItemQuantity"));
	
				if (prev && c.Classname && c.Classname != "")
				{
					EntityAI e = GetPreviewEntity(c.Classname);
					if (e)
					{
						prev.SetItem(e);
						prev.SetView(0);
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
						if (cnt <= 0) cnt = 1;
						t = string.Format("x%1", cnt);
					}
					qty.SetText(t);
				}
			}
		}
		
		if (m_ReCraftPanel)
			m_ReCraftPanel.Show(hasDrops);
		
		UpdateReCraftButtonState();
	
		if (m_ReCraftListWrap)
			m_ReCraftListWrap.Update();
	}
	
	protected void StartReCraftVisuals(float durationSec)
	{
		if (durationSec <= 0.0) durationSec = 0.01;
	
		m_ReCraftActive   = true;
		m_ReCraftElapsed  = 0.0;
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
		if (!m_ReCraftActive) return;
	
		m_ReCraftElapsed += 0.05;
		float pct = (m_ReCraftElapsed / m_ReCraftDuration) * 100.0;
		int cur = Math.Clamp(Math.Round(pct), 0, 100);
		if (m_ReCraftPB)
			m_ReCraftPB.SetCurrent(cur);
	
		if (m_ReCraftElapsed >= m_ReCraftDuration)
		{
			m_ReCraftActive = false;
			if (m_ReCraftProgressTimer) m_ReCraftProgressTimer.Stop();
			if (m_ReCraftPB) m_ReCraftPB.SetCurrent(100);
			
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
		if (!m_SelectedRecipe) return;
		if (m_ReCraftActive) return;
		if (!HasResultItemForRecraft(m_SelectedRecipe))
		{
			if (m_ReCraftButton) m_ReCraftButton.Enable(false);
			return;
		}
		
		DRL_PlayCraftSoundForRecipe(m_SelectedRecipe);
	
		StartReCraftVisuals(m_SelectedRecipe.CraftTimeSec);
		UpdateReCraftButtonState();
	
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
	
		string chosen = m_SelectedRecipe.Result;
		if (m_SelectedResultClass != "" && m_SelectedRecipe.ResultClasses && m_SelectedRecipe.ResultClasses.Find(m_SelectedResultClass) > -1)
			chosen = m_SelectedResultClass;
	
		Param2<int,string> pdata = new Param2<int,string>(m_SelectedRecipe.RecipeId, chosen);
		GetGame().RPCSingleParam(player, 57864579, pdata, true, player.GetIdentity());
	}
	
	protected void TickCraftProgress()
	{
		if (!m_CraftActive) return;
	
		m_CraftElapsed += 0.05;
		float pct = (m_CraftElapsed / m_CraftDuration) * 100.0;
		int cur = Math.Clamp(Math.Round(pct), 0, 100);
		if (m_CraftPB)
			m_CraftPB.SetCurrent(cur);
	
		if (m_CraftElapsed >= m_CraftDuration)
		{
			m_CraftActive = false;
			if (m_CraftProgressTimer) m_CraftProgressTimer.Stop();
			if (m_CraftPB) m_CraftPB.SetCurrent(100);
			
			DRL_StopCraftSound();
		}
	}
	
	protected void StartCraftVisuals(float durationSec)
	{
		if (durationSec <= 0.0) durationSec = 0.01;

		if (m_CraftTimer)          m_CraftTimer.Stop();
		if (m_CraftProgressTimer)  m_CraftProgressTimer.Stop();
	
		m_CraftDuration = durationSec;
		m_CraftElapsed  = 0.0;
		m_CraftActive   = true;

		if (m_CraftPB) m_CraftPB.SetCurrent(0);
		
		UpdateCraftButtonState();

		if (m_ButtonCraftText)
        m_ButtonCraftText.SetText("#STR_DRL_IN_PROGRESS");

		m_RemainTime = Math.Ceil(durationSec);
		UpdateCraftCountdown();
		if (!m_CraftTimer) m_CraftTimer = new Timer();
		m_CraftTimer.Run(1.0, this, "UpdateCraftCountdown", null, true);

		if (!m_CraftProgressTimer) m_CraftProgressTimer = new Timer();
		m_CraftProgressTimer.Run(0.05, this, "TickCraftProgress", null, true);
	}

	void BuildRecipesList(string queryLower = "")
	{
		if (!m_RecipesListSpacer) return;
		ClearChildren(m_RecipesListSpacer);
	
		if (!g_Craft_Config) return;
		ref array<ref DRL_Craft_Recipe> all = g_Craft_Config.GetAll();
		if (!all) return;
	
		for (int i = 0; i < all.Count(); i++)
		{
			DRL_Craft_Recipe r = all[i];
			if (!r) continue;
	
			string rn = r.RecipeName;
			rn.ToLower();
			
			if (queryLower != "" && rn != "" && rn.IndexOf(queryLower) == -1)
				continue;
			
			if (!RecipeAllowedByContext(r))
				continue;
			
			if (!RecipePassesCategory(r))
				continue;
	
			Widget row = GetGame().GetWorkspace().CreateWidgets(RECIPE_TEMPLATE, m_RecipesListSpacer);
			FillRecipeRow(row, r);
		}
		m_RecipesListSpacer.Update();
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_SearchBox)
		{
			string q = m_SearchBox.GetText();
			q.ToLower();
			BuildRecipesList(q);
			return true;
		}
		return super.OnChange(w, x, y, finished);
	}

	protected int NeedPerCraft(DRL_Craft_Component c)
	{
		if (!c) return 0;
	
		if (c.QuantityFrac > 0.0 && DRL_IsFillingByClassUI(c.Classname))
			return ComputeNeedTotalForComponent(c);
	
		return c.Amount;
	}
	
	protected int NeedForCount(DRL_Craft_Component c, int countN)
	{
		return NeedPerCraft(c) * Math.Max(1, countN);
	}

	protected int GetMaxCraftable(DRL_Craft_Recipe r)
	{
		if (!r) return 0;
		int maxN = 1000000;
	
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
	
			int have = ComputeHaveForComponent(c);
			int need1 = NeedPerCraft(c);
			if (need1 <= 0) return 0;
	
			int n = have / need1;
			if (n < maxN) maxN = n;
		}
		if (maxN < 0) maxN = 0;
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
		if (frac < 0.0) frac = 0.0;
		if (frac > 1.0) frac = 1.0;
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
		if (frac < 0.0) frac = 0.0;
		if (frac > 1.0) frac = 1.0;
		int p = Math.Round(frac * 100.0);
		return string.Format("%1%%", p);
	}
	
	string DRL_HealthLevelToText(int lvl)
	{
		int cl = Math.Clamp(lvl, 0, 3);
		if (cl == 0) return "#STR_DRL_UNTOUCHED";
		if (cl == 1) return "#STR_DRL_WORN";
		if (cl == 2) return "#STR_DRL_DAMAGED";
		return "#STR_DRL_BADLY_DAMAGED";
	}
	
	string DRL_TextFromMinHealthFrac(int minHF)
	{
		int lvl = 3 - minHF;
		if (lvl < 0) lvl = 0;
		if (lvl > 3) lvl = 3;
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
			if (cnt <= 0) cnt = 1;
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
		if (c.Delete) delText = "#STR_DRL_DELETE_YES";
	
		if (wDmg)
		{
			wDmg.SetText(dmgText);
			if (wDel) wDel.SetText(delText);
		}
		else
		{
			if (wDel) wDel.SetText(string.Format("%1 | %2", dmgText, delText));
		}
	}
	
	protected void UpdateResultPreview(DRL_Craft_Recipe r)
	{
		if (!r) return;
		
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
				EntityAI e = GetPreviewEntity(cls);
				if (e)
				{
					ipwResult.SetItem(e);
					ipwResult.SetView(0);
					ipwResult.SetModelPosition(Vector(0,0,0));
					ipwResult.SetModelOrientation(Vector(0,0,0));
					m_AttachedPreviews.Insert(ipwResult);
				}
			}
		}
		
		ItemPreviewWidget ipwDefault = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));
		if (ipwDefault)
		{
			ipwDefault.SetItem(null);
			if (cls != "")
			{
				EntityAI e2 = GetPreviewEntity(cls);
				if (e2)
				{
					ipwDefault.SetItem(e2);
					ipwDefault.SetView(0);
					ipwDefault.SetModelPosition(Vector(0,0,0.5));
					ipwDefault.SetModelOrientation(Vector(0,0,0));
					m_AttachedPreviews.Insert(ipwDefault);
				}
			}
		}
		
		RenderAttachmentIconsForClass(cls);
	}
	
	void FillRecipeDetails(DRL_Craft_Recipe r)
	{
		if (!r) return;
		
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
		
		if (m_IngrDetailPanel)
			m_IngrDetailPanel.Show(false);

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
			else
			{
				if (r.Result && r.Result != "")
				{
					string cfgDesc = "";
					if (GetDescriptionShortForClass(r.Result, cfgDesc))
					{
						body = cfgDesc;
					}
				}
			}

			if (body == "")
			{
				body = "#STR_DRL_Craft_Recipe_Detail";
			}
		
			descW.SetText(body);
		}
		
		Widget infoPanel = m_Root.FindAnyWidget("PreviewItemInfo");
		if (infoPanel)
		{
			infoPanel.Show(true);

			TextWidget wName = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoName"));
			if (wName)
			{
				string dn = "";
				if (chosenResult && chosenResult != "")
				{
					if (!GetDisplayNameForClass(chosenResult, dn))
						dn = chosenResult;
				}
				wName.SetText(string.Format("#STR_DRL_NAME %1", dn));
			}

			TextWidget wTime = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoTime"));
			if (wTime)
			{
				int t_cfg = r.CraftTimeSec;
				wTime.SetText(string.Format("#STR_DRL_TIME %1", DRL_FormatTimeSecs(t_cfg)));
			}

			TextWidget wCount = TextWidget.Cast(m_Root.FindAnyWidget("PreviewInfoCount"));
			if (wCount)
			{
				int cnt = r.ResultCount;
				if (cnt <= 0) cnt = 1;
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
		
		ItemPreviewWidget ipwDefault = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));
		if (ipwDefault)
		{
			string clsDefault = "";
			if (r.PreviewClassname && r.PreviewClassname != "")
			{
				clsDefault = r.PreviewClassname;
			}
			else
			{
				clsDefault = r.Result;
			}
		
			if (clsDefault != "")
			{
				EntityAI e1 = GetPreviewEntity(clsDefault);
				if (e1)
				{
					ipwDefault.SetItem(e1);
					ipwDefault.SetView(0);
					ipwDefault.SetModelPosition(Vector(0,0, 0.5));
					ipwDefault.SetModelOrientation(Vector(0,0,0));
					m_AttachedPreviews.Insert(ipwDefault);
				}
			}
			
			string attachClass = "";
			if (r.PreviewClassname && r.PreviewClassname != "")
			{
				attachClass = r.PreviewClassname;
			}
			else
			{
				attachClass = r.Result;
			}
			RenderAttachmentIconsForClass(attachClass);
		}

		Widget previewPanel = m_Root.FindAnyWidget("PreviewItemWidgetPanel");
		if (previewPanel)
		{
			previewPanel.Show(true);
		}
		
		ItemPreviewWidget ipwResult = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("PreviewItemWidget"));
		if (ipwResult)
		{
			string resultClass = chosenResult;
			if (resultClass != "")
			{
				EntityAI e2 = GetPreviewEntity(resultClass);
				if (e2)
				{
					ipwResult.SetItem(e2);
					ipwResult.SetView(0);
					ipwResult.SetModelPosition(Vector(0,0,0));
					ipwResult.SetModelOrientation(Vector(0,0,0));
					m_AttachedPreviews.Insert(ipwResult);
				}
			}
		}
		
		m_ItemPreviewWidget0 = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget0"));
		if (m_ItemPreviewWidget0)
		{
			m_PreviewOrientation = Vector(0,0,0);
			m_ItemPreviewWidget0.SetModelOrientation(m_PreviewOrientation);
		}

		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (timeW)
		{
			int t_total = r.CraftTimeSec;
			timeW.SetText(DRL_CraftTimeLabel(t_total));
		}

		Widget ingrWrap = m_Root.FindAnyWidget("CraftIngridientListWrap");
		if (!ingrWrap) return;
	
		ClearChildren(ingrWrap);

		int m = r.CraftComponents.Count();
		for (int i = 0; i < m; i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
		
			Widget slot = GetGame().GetWorkspace().CreateWidgets(INGR_TEMPLATE, ingrWrap);
			slot.SetUserData(c);
			
			ItemPreviewWidget prev = ItemPreviewWidget.Cast(slot.FindAnyWidget("CraftIngridientItemPreview"));
			TextWidget        qty  = TextWidget.Cast(slot.FindAnyWidget("CraftIngridientItemQuantity"));
			Widget            panelW = slot.FindAnyWidget("CraftIngridientItemPanelWidget");
		
			if (prev && c.Classname && c.Classname != "")
			{
				EntityAI ew = GetPreviewEntity(c.Classname);
				if (ew)
				{
					prev.SetItem(ew);
					prev.SetView(0);
					m_AttachedPreviews.Insert(prev);
				}
			}
		
			if (qty)
			{
				int have = ComputeHaveForComponent(c);
				int need = NeedForCount(c, m_SelectedRecipe.CurrentCraftCount);
				qty.SetText(string.Format("%1/%2", have, need));
		
				Widget panel = panelW;

				int colorPanelOK   = ARGB(10, 120, 200,  80);
				int colorPanelPart = ARGB(10, 230, 180,  60);
				int colorPanelNone = ARGB(10, 200,  60,  60);
				
				int colorTextOK    = ARGB(255, 255, 255, 255);
				int colorTextPart  = ARGB(255, 230, 180,  60);
				int colorTextNone  = ARGB(255, 200,  60,  60);
				
				if (qty) qty.Show(true);
				
				if (have >= need)
				{
					if (panel) panel.SetColor(colorPanelOK);
					if (qty)   qty.SetColor(colorTextOK);
				}
				else if (have > 0)
				{
					if (panel) panel.SetColor(colorPanelPart);
					if (qty)   qty.SetColor(colorTextPart);
				}
				else
				{
					if (panel) panel.SetColor(colorPanelNone);
					if (qty)   qty.SetColor(colorTextNone);
				}
			}
		}
		
		ingrWrap.Update();
		BuildAtachIngridientListUI(r);
		BuildSelectResultList(r);
		
		bool hasAll = true;
		bool hasAny = false;
		
		for (int j = 0; j < r.CraftComponents.Count(); j++)
		{
			DRL_Craft_Component comp = r.CraftComponents[j];
			if (!comp) continue;
		
			int haveC, needC;
			haveC = ComputeHaveForComponent(comp);
			needC = NeedForCount(comp, m_SelectedRecipe.CurrentCraftCount);
		
			if (haveC > 0) hasAny = true;
			if (haveC < needC) hasAll = false;
		}

		UpdateCraftButtonState();
		
		bool canClient = CanCraftRecipe(r) && DRL_ClientHasRequiredAttachments(r);
		UpdateCraftButtonColor(canClient);
		
		RefreshTimeTextForCount();
		RefreshCraftCountText();
		RefreshRecipeListColors();
		BuildReCraftList(r);
		UpdateReCraftButtonState();
		UpdateResultStats(chosenResult);
	}
	
	protected bool DRL_IsHealthyAttachment(ItemBase ib)
	{
		if (!ib) return false;
		if (ib.IsRuined()) return false;
		return ib.GetHealthLevel() <= 2;
	}
	
	protected bool DRL_ClientHasRequiredAttachments(DRL_Craft_Recipe r)
	{
		if (!r || r.GetAttachReqCount() == 0)
			return true;
	
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
	
		EntityAI bench = null;
		if (m_Workbench)
		{
			bench = m_Workbench;
		}
		else
		{
			bench = m_ContextWorkbench;
		}
	
		for (int i = 0; i < r.GetAttachReqCount(); i++)
		{
			string token; float dmg;
			r.GetAttachReqAt(i, token, dmg);
			if (token == "") return false;
			
			if (InventorySlots.GetSlotIdFromString(token) > -1)
			{
				bool okSlot = false;
	
				if (pb)
				{
					ItemBase ibP = ItemBase.Cast(pb.FindAttachmentBySlotName(token));
					if (ibP && DRL_IsHealthyAttachment(ibP)) okSlot = true;
				}
	
				if (!okSlot && bench)
				{
					ItemBase ibW = ItemBase.Cast(bench.FindAttachmentBySlotName(token));
					if (ibW && DRL_IsHealthyAttachment(ibW)) okSlot = true;
				}
	
				if (!okSlot) return false;
				continue;
			}
			
			bool okClass = false;
			if (pb && pb.GetInventory())
			{
				int ac = pb.GetInventory().AttachmentCount();
				for (int a = 0; a < ac; a++)
				{
					ItemBase ibA = ItemBase.Cast(pb.GetInventory().GetAttachmentFromIndex(a));
					if (!ibA) continue;
					if (ibA.GetType() != token) continue;
					if (DRL_IsHealthyAttachment(ibA)) { okClass = true; break; }
				}
			}
			if (!okClass && bench && bench.GetInventory())
			{
				int ac2 = bench.GetInventory().AttachmentCount();
				for (int b = 0; b < ac2; b++)
				{
					ItemBase ibB = ItemBase.Cast(bench.GetInventory().GetAttachmentFromIndex(b));
					if (!ibB) continue;
					if (ibB.GetType() != token) continue;
					if (DRL_IsHealthyAttachment(ibB)) { okClass = true; break; }
				}
			}
	
			if (!okClass) return false;
		}
		return true;
	}
	
	protected bool IsSlotName(string token)
	{
		return InventorySlots.GetSlotIdFromString(token) > -1;
	}
	
	protected EntityAI GetAttachmentBySlotName(EntityAI owner, string slotName)
	{
		if (!owner) return null;
		return owner.FindAttachmentBySlotName(slotName);
	}
	
	protected string DefaultClassForSlot(string slotName)
	{
		if (slotName == "DRL_Craftattach")  return "DRL_Craft_Grinder";
		if (slotName == "DRL_Craftattach1") return "DRL_Craft_Vise";
		if (slotName == "DRL_Craftattach2") return "DRL_Craft_Anvil";
		if (slotName == "DRL_Craftattach3") return "DRL_Craft_Drill";
		if (slotName == "DRL_Craftattach4") return "DRL_Craft_Micro";
		if (slotName == "DRL_Craftattach5") return "DRL_Craft_lab_bunsen";
		if (slotName == "Gloves") return "WorkingGloves_Beige";
		return "";
	}
	
	protected EntityAI SpawnAttachDummy(string cls)
	{
		if (cls == "") return null;
		vector z = "0 0 0";
		EntityAI obj = EntityAI.Cast(GetGame().CreateObject(cls, z, true, false, false));
		if (obj)
		{
			m_AttachPrevDummies.Insert(obj);
		}
		return obj;
	}
	
	protected void ClearAttachDummies()
	{
		int i = 0;
		while (i < m_AttachPrevDummies.Count())
		{
			GetGame().ObjectDelete(m_AttachPrevDummies[i]);
			i = i + 1;
		}
		m_AttachPrevDummies.Clear();
	}
	
	protected void BuildAtachIngridientListUI(DRL_Craft_Recipe r)
	{
		if (!m_AtachIngridientListWrap) return;
	
		ClearChildren(m_AtachIngridientListWrap);
		ClearAttachDummies();
	
		m_AttachTokens          = new array<string>();
		m_AttachPreviewClasses  = new array<string>();
		m_AttachPresentItems    = new array<EntityAI>();
		m_AttachDamages         = new array<float>(); 
	
		if (!r || r.GetAttachReqCount() == 0)
			return;
	
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
	
		int idx = 0;
		while (idx < r.GetAttachReqCount())
		{
			string token; float dmg;
			r.GetAttachReqAt(idx, token, dmg);
			
			if (token == "")
			{
				idx = idx + 1;
					continue;
			}
			
			m_AttachDamages.Insert(dmg);
	
			Widget cell = GetGame().GetWorkspace().CreateWidgets(DRL_ATTACHMENT_CELL, m_AtachIngridientListWrap);

			ButtonWidget btn = ButtonWidget.Cast(cell.FindAnyWidget("AtachIngridientButton"));
			ItemPreviewWidget prev = ItemPreviewWidget.Cast(cell.FindAnyWidget("AtachIngridientItemPreview"));
			Widget panelW = cell.FindAnyWidget("AtachIngridientItemPanelW");
			
			ImageWidget icon = ImageWidget.Cast(cell.FindAnyWidget("AttachmentIconImage"));
			if (icon) icon.Show(false);
			
			int id = idx;
			cell.SetUserID(id);
			if (btn)  btn.SetUserID(id);
			if (prev) prev.SetUserID(id);
			if (panelW) panelW.SetUserID(id);
	
			bool present = false;
			EntityAI presentItem = null;
			string previewClass = "";
			
			if (IsSlotName(token))
			{
				if (pb)
				{
					presentItem = GetAttachmentBySlotName(pb, token);
					ItemBase ibP = ItemBase.Cast(presentItem);
					if (ibP && DRL_IsHealthyAttachment(ibP)) { present = true; }
					else { presentItem = null; }
				}
				if (!present && m_ContextWorkbench)
				{
					presentItem = GetAttachmentBySlotName(m_ContextWorkbench, token);
					ItemBase ibW = ItemBase.Cast(presentItem);
					if (ibW && DRL_IsHealthyAttachment(ibW)) { present = true; }
					else { presentItem = null; }
				}
	
				if (presentItem) previewClass = presentItem.GetType();
				else previewClass = DefaultClassForSlot(token);
			}
			
			else
			{
				previewClass = token;
				present = false;
				presentItem = null;
	
				if (pb && pb.GetInventory())
				{
					int ac = pb.GetInventory().AttachmentCount();
					for (int a = 0; a < ac; a++)
					{
						ItemBase ibA = ItemBase.Cast(pb.GetInventory().GetAttachmentFromIndex(a));
						if (!ibA) continue;
						if (ibA.GetType() != token) continue;
						if (DRL_IsHealthyAttachment(ibA)) { present = true; presentItem = ibA; break; }
					}
				}
				if (!present && m_ContextWorkbench && m_ContextWorkbench.GetInventory())
				{
					int ac2 = m_ContextWorkbench.GetInventory().AttachmentCount();
					for (int b = 0; b < ac2; b++)
					{
						ItemBase ibB = ItemBase.Cast(m_ContextWorkbench.GetInventory().GetAttachmentFromIndex(b));
						if (!ibB) continue;
						if (ibB.GetType() != token) continue;
						if (DRL_IsHealthyAttachment(ibB)) { present = true; presentItem = ibB; break; }
					}
				}
			}
	
			if (panelW)
			{
				if (present) panelW.SetColor(ARGB(255, 40,160, 60));
				else         panelW.SetColor(ARGB(255,170, 40, 40));
			}
	
			if (prev)
			{
				if (presentItem)
				{
					prev.SetItem(presentItem);
				}
				else
				{
					EntityAI dummy = SpawnAttachDummy(previewClass);
					if (dummy) prev.SetItem(dummy);
					else if (icon) icon.Show(true);
				}
			}
	
			m_AttachTokens.Insert(token);
			m_AttachPreviewClasses.Insert(previewClass);
			m_AttachPresentItems.Insert(presentItem);
	
			idx = idx + 1;
		}
	
		m_AtachIngridientListWrap.Update();
	}
	
	protected bool ReadInt(string path, out int val)
	{
		if (GetGame().ConfigIsExisting(path))
		{
			val = GetGame().ConfigGetInt(path);
			return true;
		}
		return false;
	}
	
	protected bool ReadFloat(string path, out float val)
	{
		if (GetGame().ConfigIsExisting(path))
		{
			val = GetGame().ConfigGetFloat(path);
			return true;
		}
		return false;
	}
	
	protected void UpdateResultStats(string cls)
	{
		if (!m_StatsTextW)
			return;
	
		if (!cls || cls == "")
		{
			m_StatsTextW.SetText("");
			return;
		}
		
		string basePath = "CfgVehicles " + cls;
		if (!GetGame().ConfigIsExisting(basePath))
		{
			basePath = "CfgWeapons " + cls;
			if (!GetGame().ConfigIsExisting(basePath))
			{
				basePath = "CfgMagazines " + cls;
				if (!GetGame().ConfigIsExisting(basePath))
				{
					m_StatsTextW.SetText("");
					return;
				}
			}
		}
		
		int hpMax = -1;
		int tmpInt = 0;
		if (ReadInt(basePath + " DamageSystem GlobalHealth Health hitpoints", tmpInt))
			hpMax = tmpInt;
		
		float projCoef = -1.0;
		float meleeCoef = -1.0;
		float infectCoef = -1.0;
		float grenadeCoef = -1.0;
		float tmp;
		
		if (ReadFloat(basePath + " DamageSystem GlobalArmor Projectile Health damage", tmp))
			projCoef = tmp;
		
		if (ReadFloat(basePath + " DamageSystem GlobalArmor Melee Health damage", tmp))
			meleeCoef = tmp;
		
		if (ReadFloat(basePath + " DamageSystem GlobalArmor Infected Health damage", tmp))
			infectCoef = tmp;
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor Zombie Health damage", tmp))
			infectCoef = tmp;
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor MeleeZombie Health damage", tmp))
			infectCoef = tmp;
		
		if (ReadFloat(basePath + " DamageSystem GlobalArmor FragGrenade Health damage", tmp))
			grenadeCoef = tmp;
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor Explosion Health damage", tmp))
			grenadeCoef = tmp;
		
		int projProt = -1;
		int meleeProt = -1;
		int infectProt = -1;
		int grenadeProt = -1;
	
		if (projCoef >= 0.0)
		{
			float p = (1.0 - projCoef) * 100.0;
			if (p < 0.0) p = 0.0;
			if (p > 100.0) p = 100.0;
			projProt = Math.Round(p);
		}
	
		if (meleeCoef >= 0.0)
		{
			float p2 = (1.0 - meleeCoef) * 100.0;
			if (p2 < 0.0) p2 = 0.0;
			if (p2 > 100.0) p2 = 100.0;
			meleeProt = Math.Round(p2);
		}
	
		if (infectCoef >= 0.0)
		{
			float p3 = (1.0 - infectCoef) * 100.0;
			if (p3 < 0.0) p3 = 0.0;
			if (p3 > 100.0) p3 = 100.0;
			infectProt = Math.Round(p3);
		}
	
		if (grenadeCoef >= 0.0)
		{
			float p4 = (1.0 - grenadeCoef) * 100.0;
			if (p4 < 0.0) p4 = 0.0;
			if (p4 > 100.0) p4 = 100.0;
			grenadeProt = Math.Round(p4);
		}
		
		string t = "#STR_DRL_STATS_TITLE\n";
		
		if (hpMax >= 0)
			t += "#STR_DRL_STATS_DURABILITY " + hpMax.ToString() + " hp\n";
		else
			t += "#STR_DRL_STATS_DURABILITY_NONE\n";
		
		if (projProt >= 0)
			t += "#STR_DRL_STATS_BULLET " + projProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_BULLET_NONE\n";
		
		if (meleeProt >= 0)
			t += "#STR_DRL_STATS_KNIFE " + meleeProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_KNIFE_NONE\n";
		
		if (infectProt >= 0)
			t += "#STR_DRL_STATS_INFECTED " + infectProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_INFECTED_NONE\n";
		
		if (grenadeProt >= 0)
			t += "#STR_DRL_STATS_GRENADE " + grenadeProt.ToString() + "%";
		else
			t += "#STR_DRL_STATS_GRENADE_NONE\n";
	
		m_StatsTextW.SetText(t);
	}

	protected void GetAttachmentSlotsForClass(string cls, out array<string> outSlots)
	{
		outSlots = new array<string>();
		if (cls == "") return;
	
		string p;

		p = "CfgWeapons " + cls + " attachments";
		if (GetGame().ConfigIsExisting(p))
		{
			array<string> tmp = new array<string>();
			GetGame().ConfigGetTextArray(p, tmp);
			for (int i = 0; i < tmp.Count(); i++)
				outSlots.Insert(tmp[i]);
		}

		p = "CfgVehicles " + cls + " attachments";
		if (GetGame().ConfigIsExisting(p))
		{
			array<string> tmp2 = new array<string>();
			GetGame().ConfigGetTextArray(p, tmp2);
			for (int j = 0; j < tmp2.Count(); j++)
				outSlots.Insert(tmp2[j]);
		}

		outSlots.Sort();
		int k = 1;
		while (k < outSlots.Count())
		{
			if (outSlots[k] == outSlots[k - 1]) outSlots.Remove(k);
			else k++;
		}
	}

	protected bool GetSlotIconPath(string slotName, out string outPath)
	{
		outPath = "";
		if (slotName == "") return false;
	
		string base = "CfgSlots Slot_" + slotName + " ";
		string val = "";

		if (GetGame().ConfigIsExisting(base + "ghostIcon"))
			GetGame().ConfigGetText(base + "ghostIcon", val);

		if (val == "" && GetGame().ConfigIsExisting(base + "icon"))
			GetGame().ConfigGetText(base + "icon", val);
	
		if (val == "")
			return false;

		if (val.IndexOf("set:") == 0)
			outPath = val;
		else
			outPath = "set:dayz_inventory image:" + val;
	
		return true;
	}

	protected void RenderAttachmentIconsForClass(string cls)
	{
		if (!m_AttachmentIconsWrap) return;

		ClearChildren(m_AttachmentIconsWrap);
	
		if (cls == "") return;
	
		array<string> slots;
		GetAttachmentSlotsForClass(cls, slots);
		if (slots.Count() == 0) return;
	
		for (int i = 0; i < slots.Count(); i++)
		{
			string slot = slots[i];

			Widget row = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/AttachmentIcon.layout", m_AttachmentIconsWrap);
			ImageWidget img = ImageWidget.Cast(row.FindAnyWidget("AttachmentIconImage"));
			if (!img) continue;
	
			string iconPath;
			if (GetSlotIconPath(slot, iconPath))
			{
				img.LoadImageFile(0, iconPath);
				img.SetImage(0);
			}
			else
			{
				img.LoadImageFile(0, "set:dayz_inventory image:missing");
				img.SetImage(0);
			}
			
			img.Show(true);
			row.Show(true);
		}

		m_AttachmentIconsWrap.Update();
	}

	protected bool CanCraftRecipeCount(DRL_Craft_Recipe r, int countN)
	{
		if (!r) return false;
		if (m_CraftActive) return false;
	
		int cnt = Math.Max(1, countN);
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
	
			int have = ComputeHaveForComponent(c);
			int need = NeedForCount(c, cnt);
			if (have < need) return false;
		}
		return true;
	}
	
	protected void UpdateRecipeSelectionHighlight()
	{
		Widget parent = m_AllRecipesListWrap;
		if (!parent) return;
	
		int defaultCol  = ARGB(50, 22, 23, 19);
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
		if (!m_RecipesListSpacer) return;
	
		Widget row = m_RecipesListSpacer.GetChildren();
		while (row)
		{
			DRL_Craft_Recipe r;
			row.GetUserData(r);
	
			if (r)
			{
				bool can = CanCraftRecipe(r);
	
				Widget prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget");
				if (!prevPanel) prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget0");
	
				if (prevPanel)
				{
					if (can)
						prevPanel.SetColor(ARGB(30, 120, 200, 80));
					else
						prevPanel.SetColor(ARGB(30, 200, 60, 60));
				}
			}
	
			row = row.GetSibling();
		}
	}
	
	protected void UpdateCraftCountdown()
	{
		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (!timeW) return;

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
	
		if (m_CraftTimer) m_CraftTimer.Stop();
		
		DRL_StopCraftSound();
	
		m_CraftActive = false;
		if (m_CraftProgressTimer) m_CraftProgressTimer.Stop();

		if (m_CraftPB) m_CraftPB.SetCurrent(0);
		
		bool can = CanCraftSelected();
		RefreshCraftCountText();
		UpdateCraftButtonState();
		UpdateCraftButtonColor(can);
		
		RefreshRecipeListColors();
		
		if (m_SelectedRecipe)
		{
			FillRecipeDetails(m_SelectedRecipe);
			RefreshTimeTextForCount();
		}
	}

	protected void FillRecipeRow(Widget row, DRL_Craft_Recipe r)
	{
		if (!row || !r) return;

		RichTextWidget nameW = RichTextWidget.Cast(row.FindAnyWidget("NameRecipesButton"));
		ImageWidget iconW    = ImageWidget.Cast(row.FindAnyWidget("RecipesImagePreview"));
		ItemPreviewWidget prevW = ItemPreviewWidget.Cast(row.FindAnyWidget("RecipesItemPreview"));

		if (nameW) nameW.SetText(r.RecipeName);

		bool iconSet = false;
		if (iconW)
		{
			if (r.CraftIcon && r.CraftIcon != "")
			{
				iconW.LoadImageFile(0, r.CraftIcon);
				iconW.Show(true);
				iconSet = true;
			}
			else
			{
				iconW.Show(false);
			}
		}
		if (prevW)
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
				EntityAI e = GetPreviewEntity(cls);
				if (e)
				{
					prevW.SetItem(e);
					prevW.SetView(0);
					prevW.SetModelPosition(Vector(0,0,0));
					prevW.SetModelOrientation(Vector(0,0,0));
					prevW.Show(!iconSet);
					m_AttachedPreviews.Insert(prevW);
				}
			}
		}
		
		bool can = CanCraftRecipe(r);

		Widget prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget");
		
		if (!prevPanel) prevPanel = row.FindAnyWidget("RecipesPreviewPanelWidget0");
		
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

		row.SetUserData(r);
		row.SetFlags(WidgetFlags.VISIBLE);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w == m_ItemPreviewWidget0)
		{
			GetGame().GetDragQueue().Call(this, "UpRotation");
			g_Game.GetMousePos(m_PrevMouseX, m_PrevMouseY);
			return true;
		}
		
		if (button == MouseState.LEFT && w)
		{
			DRL_Craft_Recipe r = null;
			Widget cur = w;
			while (cur && !r)
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
				m_SelectedRecipe = r;
				m_SelectedRecipe.CurrentCraftCount = 1;
				m_SelectedResultClass = "";
				BuildSelectResultList(m_SelectedRecipe);
				UpdateSelectCraftHighlight();
				UpdateResultPreview(m_SelectedRecipe);
				UpdateResultStats(m_SelectedRecipe.Result);
				UpdateCraftButtonState();
				
				FillRecipeDetails(r);
				UpdateOptionalTitles(r);
				RefreshCraftCountText();
				RefreshTimeTextForCount();
				return true;
			}
		}
		
		if (button == MouseState.LEFT && w)
		{
			ButtonWidget sb = FindSelectButtonFrom(w);
			if (sb) return OnClick(sb, x, y, MouseState.LEFT);
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
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		
		if (w == m_CraftCountBtnPlus || (w && w.GetName() == "CraftCountButtonPlus"))
		{
			if (!m_SelectedRecipe) return true;
			int maxN = GetMaxCraftable(m_SelectedRecipe);
			int newN = Math.Clamp(m_SelectedRecipe.CurrentCraftCount + 1, 1, Math.Max(1, maxN));
			m_SelectedRecipe.CurrentCraftCount = newN;
			FillRecipeDetails(m_SelectedRecipe);
			return true;
		}
		
		if (w == m_CraftCountBtnMin || (w && w.GetName() == "CraftCountButtonMin"))
		{
			if (!m_SelectedRecipe) return true;
			int newNMin = Math.Max(1, m_SelectedRecipe.CurrentCraftCount - 1);
			m_SelectedRecipe.CurrentCraftCount = newNMin;
			FillRecipeDetails(m_SelectedRecipe);
			return true;
		}
		
		if (button == MouseState.LEFT && w)
		{
			Widget cur = w;
			while (cur)
			{
				if (cur == m_CraftButton || cur.GetName() == "CraftButton")
				{
					if (m_CraftActive) return true;
					if (!CanCraftSelected())
					{
						if (m_CraftButton) m_CraftButton.Enable(false);
						return true;
					}
					TryStartCraft();
					return true;
				}
				if (cur.GetName() == "ButtonOut" || cur == m_BtnClose)
				{
					CloseMenuSelf();
					return true;
				}
				cur = cur.GetParent();
			}
			
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
			
			if (w.GetName() == "RecipesSelectionButtonWidget")
			{
				DRL_Craft_Recipe r;
				w.GetUserData(r);
				if (!r && w.GetParent()) w.GetParent().GetUserData(r);
				if (r)
				{
					m_SelectedRecipe = r;
					if (m_RightPanel) m_RightPanel.Show(true);
					
					DRL_Craft_Category cat2 = null;
					DRL_Craft_Config cfg2 = GetCraft_Config();
					
					if (cfg2 && m_CurrentCategoryIdx >= 0 && m_CurrentCategoryIdx < cfg2.Categories.Count())
					{
						DRL_Craft_Category selectedCat = cfg2.Categories[m_CurrentCategoryIdx];
						
						if (selectedCat && selectedCat.ShowAll != 1)
						{
							cat2 = selectedCat;
						}
						else
						{
							cat2 = DRL_FindCategoryForRecipe(r);
						}
					}
					
					DRL_ApplyFonBackground(cat2);
					DRL_ApplyCraftFonBackground(cat2);
					
					m_SelectedRecipe.CurrentCraftCount = 1;
					
					m_SelectedResultClass = "";
					
					FillRecipeDetails(r);
					UpdateOptionalTitles(r);
					UpdateRecipeSelectionHighlight();
					RefreshCraftCountText();
					RefreshTimeTextForCount();
				}
				return true;
			}
			
			Widget cur2 = w;
			while (cur2)
			{
				if (cur2 == m_ReCraftButton || cur2.GetName() == "ReCraftButton")
				{
					if (m_ReCraftActive) return true;
					TryStartReCraft();
					return true;
				}
				cur2 = cur2.GetParent();
			}
		}
		
		if (button == MouseState.LEFT && w)
		{
			int idx; string cls;
			if (DRL_TryPickResultFromWidget(w, idx, cls))
			{
				if (m_SelectedRecipe && m_SelectedRecipe.ResultClasses && idx >= 0 && idx < m_SelectedRecipe.ResultClasses.Count())
					cls = m_SelectedRecipe.ResultClasses[idx];
		
				if (cls != "")
				{
					m_SelectedResultClass = cls;
					
					for (int i = 0; i < m_SelectRows.Count(); i++)
					{
						bool sel = false;
						if (i == idx)
						{
							sel = true;
						}
						ColorizeSelectButton(m_SelectRows[i], sel);
					}
					
					UpdateSelectCraftHighlight();
					UpdateResultPreview(m_SelectedRecipe);
					UpdateResultStats(m_SelectedResultClass);
					UpdateReCraftButtonState();
					UpdateCraftButtonState();
				}
				return true;
			}
		}
	
		return super.OnClick(w, x, y, button);
	}
	
	protected void RefreshCraftCountText()
	{
		if (!m_CraftCountText) return;
		if (m_SelectedRecipe)
			m_CraftCountText.SetText(string.Format("%1", m_SelectedRecipe.ResultCount * Math.Max(1, m_SelectedRecipe.CurrentCraftCount)));
		else
			m_CraftCountText.SetText("");
	}
	
	protected void RefreshTimeTextForCount()
	{
		RichTextWidget timeW = RichTextWidget.Cast(m_Root.FindAnyWidget("CraftInfoTimeText"));
		if (!timeW) return;
	
		int t = 0;
		if (m_SelectedRecipe)
			t = Math.Max(0, m_SelectedRecipe.CraftTimeSec * Math.Max(1, m_SelectedRecipe.CurrentCraftCount));
	
		timeW.SetText(DRL_CraftTimeLabel(t));
	}

	protected int ComputeHaveForComponent(DRL_Craft_Component c)
	{
		if (!c) return 0;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p) return 0;
	
		array<EntityAI> items = new array<EntityAI>;
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		
		if (m_ContextWorkbench && m_ContextWorkbench.GetInventory())
		{
			m_ContextWorkbench.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}
	
		int haveTotal = 0;
		for (int i = 0; i < items.Count(); i++)
		{
			EntityAI ent = items[i];
			if (!ent) continue;
			
			InventoryLocation il = new InventoryLocation();
			if (ent.GetInventory() && ent.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == p)
						continue;
				}
			}
			
			bool matchUI = false;
			if (c.UseInheritance == 1)
			{
				matchUI = ent.IsKindOf(c.Classname);
			}
			else
			{
				string typeEnt = ent.GetType();
				typeEnt.ToLower();
				string reqClsUI = c.Classname;
				reqClsUI.ToLower();
				matchUI = (typeEnt == reqClsUI);
			}

			if (!matchUI) continue;
	
			ItemBase ib = ItemBase.Cast(ent);
			if (!ib) continue;
	
			if (ib.IsRuined()) continue;
			int lvl = ib.GetHealthLevel();
			if (lvl > c.MinHealthFrac) continue;
			
			if (ib && ib.IsKindOf("Ammunition_Base"))
			{
				Magazine mgzAmmo = Magazine.Cast(ib);
				int qty = 0;
				if (mgzAmmo)
				{
					qty = mgzAmmo.GetAmmoCount();
				}
				else
				{
					qty = ib.GetQuantity();
				}
			
				if (qty <= 0)
				{
					qty = 1;
				}
			
				haveTotal = haveTotal + qty;
				continue;
			}
			
			Magazine mgz = Magazine.Cast(ib);
			if (mgz)
			{
				haveTotal += 1;
				continue;
			}
			
			if (ib.HasQuantity())
			{
				haveTotal += ib.GetQuantity();
				continue;
			}
			
			int qmaxProbeUI = ib.GetQuantityMax();
			if (qmaxProbeUI > 0)
			{
				haveTotal += ib.GetQuantity();
				continue;
			}
			
			haveTotal += 1;
		}
		return haveTotal;
	}
	
	protected bool CanCraftSelected()
	{
		if (m_CraftActive) return false;
		if (!m_SelectedRecipe) return false;
	
		for (int i = 0; i < m_SelectedRecipe.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = m_SelectedRecipe.CraftComponents[i];
			if (!c) continue;
	
			int have = ComputeHaveForComponent(c);
			int need = NeedForCount(c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need) return false;
		}
		return true;
	}
	
	protected bool CanCraftRecipe(DRL_Craft_Recipe r)
	{
		if (!r) return false;
	
		int cnt = 1;
		if (r == m_SelectedRecipe)
			cnt = Math.Max(1, r.CurrentCraftCount);
	
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
	
			int have = ComputeHaveForComponent(c);
			int need = NeedForCount(c, cnt);
			if (have < need) return false;
		}
		return true;
	}
	
	protected void UpdateCraftButtonColor(bool canCraft)
	{
		if (!m_CraftButtonPanel) return;

		if (canCraft && !m_CraftActive)
		{
			m_CraftButtonPanel.SetColor(m_CraftBtnColorDefault);
		}
		else
		{
			m_CraftButtonPanel.SetColor(ARGB(30, 200, 60, 60));
		}
	}

	protected int ComputeNeedTotalForComponent(DRL_Craft_Component c)
	{
		if (!c) return 0;

		if (c.QuantityFrac <= 0.0)
		{
			return c.Amount;
		}

		EntityAI protoEnt;
		protoEnt = EntityAI.Cast(GetPreviewEntity(c.Classname));
		if (!protoEnt)
		{
			return c.Amount;
		}
	
		int qMax = 0;

		Magazine pmgz = Magazine.Cast(protoEnt);
		if (pmgz)
		{
			qMax = pmgz.GetAmmoMax();
		}
		else
		{
			ItemBase pib = ItemBase.Cast(protoEnt);
			if (pib)
			{
				qMax = pib.GetQuantityMax();
			}
		}
	
		if (qMax <= 0)
		{
			return c.Amount;
		}
	
		int perUnit = Math.Round(qMax * c.QuantityFrac);
		if (perUnit < 1)
		{
			perUnit = 1;
		}
	
		return c.Amount * perUnit;
	}

	protected bool GetDescriptionShortForClass(string cls, out string outText)
	{
		outText = "";
		if (cls == "") return false;
	
		string path;

		path = "CfgVehicles " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "") return true;
		}

		path = "CfgWeapons " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "") return true;
		}

		path = "CfgMagazines " + cls + " descriptionShort";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outText);
			if (outText != "") return true;
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
		if (cls == "") return false;
	
		string path;
	
		path = "CfgVehicles " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "") return true;
		}
	
		path = "CfgWeapons " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "") return true;
		}
	
		path = "CfgMagazines " + cls + " displayName";
		if (GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetText(path, outName);
			if (outName != "") return true;
		}
	
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (!w) return super.OnMouseEnter(w, x, y);

		DRL_Craft_Component comp = null;
		Widget owner = null;
	
		Widget cur = w;
		while (cur)
		{
			Class ud = null;
			cur.GetUserData(ud);
			if (ud)
			{
				DRL_Craft_Component tmp = DRL_Craft_Component.Cast(ud);
				if (tmp)
				{
					comp = tmp;
					owner = cur;
					break;
				}
			}
			cur = cur.GetParent();
		}

		if (comp && owner && owner.GetName() == "CraftIngridientButton")
		{
			string cls = comp.Classname;
	
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
		
		if (w)
		{
			Widget curA = w;
			while (curA)
			{
				if (curA.GetName() == "AtachIngridientButton")
				{
					int id = curA.GetUserID();
					
					if (id >= 0 && m_AttachTokens && id < m_AttachTokens.Count())
					{
						EntityAI realItem = null;
						string attCls = "";
		
						if (m_AttachPresentItems && id < m_AttachPresentItems.Count())
							realItem = m_AttachPresentItems[id];
		
						if (realItem)
							attCls = realItem.GetType();
						else if (m_AttachPreviewClasses && id < m_AttachPreviewClasses.Count())
							attCls = m_AttachPreviewClasses[id];
		
						if (m_IngrDetailPanel)
						{
							if (m_IngrDetailName)
							{
								string attDn = attCls;
								GetDisplayNameForClass(attCls, attDn);
								m_IngrDetailName.SetText(attDn);
							}
							if (m_IngrDetailDesc)
							{
								string attDs = "";
								GetDescriptionShortForClass(attCls, attDs);
								m_IngrDetailDesc.SetText(attDs);
							}
							
							TextWidget aCount = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiCount"));
							TextWidget aQty   = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiQuantity"));
							TextWidget aDmg   = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiDamage"));
							TextWidget aDel   = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiDelete"));
							if (aCount) aCount.SetText("");
							if (aQty)   aQty.SetText("");
							if (aDmg)
							{
								int pct = 0;
								if (m_AttachDamages && id >= 0 && id < m_AttachDamages.Count())
									pct = Math.Round(m_AttachDamages[id] * 100.0);
								aDmg.SetText("#STR_CRAFT_URON " + pct.ToString() + "%");
							}
							
							if (aDel)   aDel.SetText("");
							
							TextWidget aState = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiHealth"));
							if (!aState)
								aState = TextWidget.Cast(m_Root.FindAnyWidget("IngridientDetaiState"));
							
							if (aState)
							{
								string stateKey = "#STR_DRL_STATE_DESC_NONE";
							
								ItemBase ib = ItemBase.Cast(realItem);
								if (ib)
								{
									if (ib.IsRuined())
									{
										stateKey = "#STR_DRL_STATE_DESC_RUINED";
									}
									else
									{
										int hl = ib.GetHealthLevel();
										if (hl == 0) stateKey = "#STR_DRL_STATE_DESC_PRISTINE";
										else if (hl == 1) stateKey = "#STR_DRL_STATE_DESC_WORN";
										else if (hl == 2) stateKey = "#STR_DRL_STATE_DESC_DAMAGED";
										else if (hl == 3) stateKey = "#STR_DRL_STATE_DESC_BADLY_DAMAGED";
										else             stateKey = "#STR_DRL_STATE_DESC_RUINED";
									}
								}
							
								aState.SetText(stateKey);
							}
		
							m_IngrDetailPanel.Show(true);
						}
					}
					return true;
				}
				curA = curA.GetParent();
			}
		}
	
		return super.OnMouseEnter(w, x, y);
	}
		
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (!w) return super.OnMouseLeave(w, enterW, x, y);

		DRL_Craft_Component nextComp = null;
		Widget nextOwner = null;
	
		if (enterW)
		{
			Widget cur = enterW;
			while (cur)
			{
				Class ud = null;
				cur.GetUserData(ud);
				if (ud)
				{
					DRL_Craft_Component tmp = DRL_Craft_Component.Cast(ud);
					if (tmp)
					{
						nextComp = tmp;
						nextOwner = cur;
						break;
					}
				}
				cur = cur.GetParent();
			}
		}

		if (!nextComp || !nextOwner || nextOwner.GetName() != "CraftIngridientButton")
		{
			if (m_IngrDetailPanel)
				m_IngrDetailPanel.Show(false);
		}
		
		if (w && w.GetName() == "AtachIngridientButton")
		{
			bool stayingOnAttach = false;
			if (enterW)
			{
				Widget jcur = enterW;
				while (jcur)
				{
					if (jcur.GetName() == "AtachIngridientButton") { stayingOnAttach = true; break; }
					jcur = jcur.GetParent();
				}
			}
			if (!stayingOnAttach && m_IngrDetailPanel)
				m_IngrDetailPanel.Show(false);
		
			return true;
		}
	
		return super.OnMouseLeave(w, enterW, x, y);
	}
	
	protected EntityAI GetPreviewEntity(string clsname)
	{
		if (clsname == "") return null;
	
		EntityAI ent;
		if (m_PreviewCache && m_PreviewCache.Find(clsname, ent))
			return ent;

		ent = EntityAI.Cast(GetGame().CreateObject(clsname, "0 0 0", true, false, false));
		if (ent)
		{
			m_PreviewCache.Insert(clsname, ent);
		}
		return ent;
	}
	
	protected void UpdateCraftButtonState()
	{
		if (!m_CraftButton || !m_ButtonCraftText) return;
		
		if (m_CraftActive)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_DRL_IN_PROGRESS");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/deseasesshow.edds");
			return;
		}
		
		if (!m_SelectedRecipe)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("Нет рецепта");
			if (m_ButtonCraftIcon)
				m_ButtonCraftIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/error.edds");
			return;
		}
		
		bool lackAttachments = !DRL_ClientHasRequiredAttachments(m_SelectedRecipe);
		if (lackAttachments)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_DRL_NEED_ATTACHMENT");
			if (m_ButtonCraftIcon) m_ButtonCraftIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/error.edds");
			if (m_CraftButtonPanel) m_CraftButtonPanel.SetColor(ARGB(255,170,40,40));
			return;
		}
		
		bool lackIngredients = !CanCraftRecipe(m_SelectedRecipe);
		if (lackIngredients)
		{
			m_CraftButton.Enable(false);
			m_ButtonCraftText.SetText("#STR_DRL_NO_INGREDIENTS");
			if (m_ButtonCraftIcon) m_ButtonCraftIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/error.edds");
			if (m_CraftButtonPanel) m_CraftButtonPanel.SetColor(ARGB(30, 200, 60, 60));
			return;
		}
		
		m_CraftButton.Enable(true);
		m_ButtonCraftText.SetText("#STR_DRL_START");
		if (m_ButtonCraftIcon) m_ButtonCraftIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/recycling.edds");
		if (m_CraftButtonPanel) m_CraftButtonPanel.SetColor(m_CraftBtnColorDefault);
	}
	protected void TryStartCraft()
	{
		if (!m_SelectedRecipe) return;
		if (m_CraftActive) return;
		
		if (!DRL_ClientHasRequiredAttachments(m_SelectedRecipe))
		{
			if (m_CraftButton) m_CraftButton.Enable(false);
			if (m_ButtonCraftText) m_ButtonCraftText.SetText("#STR_DRL_NEED_ATTACHMENT");
			return;
		}
	
		bool hasAll = true;
		for (int i = 0; i < m_SelectedRecipe.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = m_SelectedRecipe.CraftComponents[i];
			if (!c) continue;
	
			int have = ComputeHaveForComponent(c);
			int need = NeedForCount(c, m_SelectedRecipe.CurrentCraftCount);
			if (have < need) { hasAll = false; break; }
		}
		if (!hasAll) return;
	
		if (m_CraftButton) m_CraftButton.Enable(false);
	
		int N = Math.Max(1, m_SelectedRecipe.CurrentCraftCount);
		int totalTime = m_SelectedRecipe.CraftTimeSec * N;
	
		m_RemainTime = totalTime;
		UpdateCraftCountdown();
		if (!m_CraftTimer) m_CraftTimer = new Timer();
		m_CraftTimer.Run(1.0, this, "UpdateCraftCountdown", null, true);
		
		DRL_PlayCraftSoundForRecipe(m_SelectedRecipe);
		
		StartCraftVisuals(totalTime);
	
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
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
		
		GetGame().RPCSingleParam(player, 57864578, new Param5<int,string,int,int,int>(m_SelectedRecipe.RecipeId, chosenCls, craftCount, netLow, netHigh), true, player.GetIdentity());
	}
	
	protected void PlayCraftSound()
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb) return;
		
		if (m_CraftSound && m_CraftSound.IsSoundPlaying())
			m_CraftSound.SoundStop();
	
		pb.PlaySoundSet(m_CraftSound, "DRLCraft_SoundSet", 0, 0);
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
		if (!parent) return;
		Widget c = parent.GetChildren();
		while (c)
		{
			Widget n = c.GetSibling();
			c.Unlink();
			delete c;
			c = n;
		}
	}
}