class DRL_CraftAttachmentsController
{
	protected DRL_CraftMenuUI m_Menu;
	protected Widget m_DopVarWidget;
	protected WrapSpacerWidget m_DopVarWrap;
	protected Widget m_AttachWrap;

	protected ref array<ref DRL_RecipeVarWidget> m_DopWidgets;
	protected ref map<Widget, DRL_RecipeVarWidget> m_DopWidgetMap;
	protected ref array<int> m_SelectedAttachedRecipeIds;
	protected ref map<int, string> m_SelectedAttachedVariants;
	protected ref map<int, int> m_SelectedAttachedQuantities;

	protected ref array<float> m_AttachDamages;
	protected ref array<string> m_AttachTokens;
	protected ref array<string> m_AttachPreviewClasses;
	protected ref array<EntityAI> m_AttachPresentItems;

	void DRL_CraftAttachmentsController(DRL_CraftMenuUI menu, Widget dopVarWidget, WrapSpacerWidget dopVarWrap, Widget attachWrap)
	{
		m_Menu = menu;
		m_DopVarWidget = dopVarWidget;
		m_DopVarWrap = dopVarWrap;
		m_AttachWrap = attachWrap;

		m_DopWidgets = new array<ref DRL_RecipeVarWidget>();
		m_DopWidgetMap = new map<Widget, DRL_RecipeVarWidget>();
		m_SelectedAttachedRecipeIds = new array<int>();
		m_SelectedAttachedVariants = new map<int, string>();
		m_SelectedAttachedQuantities = new map<int, int>();

		m_AttachDamages = new array<float>();
		m_AttachTokens = new array<string>();
		m_AttachPreviewClasses = new array<string>();
		m_AttachPresentItems = new array<EntityAI>();
	}

	array<int> GetSelectedAttachedRecipeIds()
	{
		return m_SelectedAttachedRecipeIds;
	}

	map<int, string> GetSelectedAttachedVariants()
	{
		return m_SelectedAttachedVariants;
	}

	map<int, int> GetSelectedAttachedQuantities()
	{
		return m_SelectedAttachedQuantities;
	}

	int GetSelectedAttachedQuantity(int recipeId)
	{
		if (m_SelectedAttachedQuantities && m_SelectedAttachedQuantities.Contains(recipeId))
		{
			return m_SelectedAttachedQuantities.Get(recipeId);
		}
		return 1;
	}

	array<int> GetSelectedAttachedRecipeIdsFlattened()
	{
		array<int> result = new array<int>();
		if (!m_SelectedAttachedRecipeIds)
		{
			return result;
		}

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

	void SetSelectedAttachedQuantity(int recipeId, int qty)
	{
		if (!m_SelectedAttachedQuantities)
		{
			m_SelectedAttachedQuantities = new map<int, int>();
		}
		if (qty < 1)
		{
			qty = 1;
		}
		m_SelectedAttachedQuantities.Set(recipeId, qty);
	}

	string GetSelectedAttachedClass(int recipeId, DRL_Craft_Recipe dopR)
	{
		if (m_SelectedAttachedVariants && m_SelectedAttachedVariants.Contains(recipeId))
		{
			return m_SelectedAttachedVariants.Get(recipeId);
		}
		if (dopR)
		{
			return dopR.Result;
		}
		return "";
	}

	void ClearSelectedIds()
	{
		if (m_SelectedAttachedRecipeIds)
		{
			m_SelectedAttachedRecipeIds.Clear();
		}
		if (m_SelectedAttachedVariants)
		{
			m_SelectedAttachedVariants.Clear();
		}
		if (m_SelectedAttachedQuantities)
		{
			m_SelectedAttachedQuantities.Clear();
		}
	}

	bool ItemMatchesComponent(ItemBase ib, DRL_Craft_Component c, string token, bool isSlot)
	{
		if (!ib || !c)
			return false;

		if (isSlot)
		{
			int slotId = InventorySlots.GetSlotIdFromString(token);
			if (m_Menu.DRL_ItemFitsSlotClient(ib, slotId))
			{
				return true;
			}
		}

		if (c.Classnames && c.Classnames.Count() > 0)
		{
			string ibType = ib.GetType();
			ibType.ToLower();
			for (int i = 0; i < c.Classnames.Count(); i++)
			{
				string req = c.Classnames[i];
				if (c.UseInheritance == 1)
				{
					if (ib.IsKindOf(req))
						return true;
				}
				else
				{
					string reqLow = req;
					reqLow.ToLower();
					if (ibType == reqLow)
						return true;
				}
			}
			return false;
		}

		if (c.UseInheritance == 1)
		{
			return ib.IsKindOf(token);
		}

		string tMatch = ib.GetType();
		tMatch.ToLower();
		string rMatch = token;
		rMatch.ToLower();
		return tMatch == rMatch;
	}

	EntityAI FindAttachment(DRL_Craft_Component c)
	{
		if (!c)
			return null;

		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		EntityAI bench = m_Menu.GetContextWorkbench();
		string activeResult = m_Menu.GetSelectedResultClass();

		return DRL_CraftComponentResolver.FindItemForComponent(c, pb, bench, activeResult);
	}

	bool HasRequiredAttachments(DRL_Craft_Recipe r)
	{
		if (!r || !r.CraftComponents)
			return true;

		string activeResult = m_Menu.GetSelectedResultClass();
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		EntityAI bench = m_Menu.GetContextWorkbench();

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0)
				continue;

			if (!c.IsApplicableForResult(activeResult))
				continue;

			if (!DRL_CraftComponentResolver.HasComponentRequirement(c, pb, bench, activeResult, 1))
			{
				return false;
			}
		}
		return true;
	}

	void BuildAtachIngridientListUI(DRL_Craft_Recipe r)
	{
		if (!m_AttachWrap)
			return;

		DRL_CraftUIHelper.ClearChildren(m_AttachWrap);

		m_AttachTokens.Clear();
		m_AttachPreviewClasses.Clear();
		m_AttachPresentItems.Clear();
		m_AttachDamages.Clear();

		if (!r || !r.CraftComponents || r.CraftComponents.Count() == 0)
			return;

		string activeResult = m_Menu.GetSelectedResultClass();

		int idx = 0;
		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0)
				continue;

			if (!c.IsApplicableForResult(activeResult))
				continue;

			string token = c.Classname;
			if (token == "" && c.Classnames && c.Classnames.Count() > 0)
			{
				token = c.Classnames[0];
			}
			if (token == "")
				continue;

			float dmg = c.ApplyDamageFrac;
			m_AttachDamages.Insert(dmg);

			Widget cell = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/Attachment.layout", m_AttachWrap);
			cell.SetUserData(c);

			ButtonWidget btn = ButtonWidget.Cast(cell.FindAnyWidget("AtachIngridientButton"));
			if (btn)
				btn.SetUserData(c);

			ItemPreviewWidget prev = ItemPreviewWidget.Cast(cell.FindAnyWidget("AtachIngridientItemPreview"));
			Widget panelW = cell.FindAnyWidget("AtachIngridientItemPanelW");

			ImageWidget icon = ImageWidget.Cast(cell.FindAnyWidget("AttachmentIconImage"));
			if (icon)
				icon.Show(false);

			int id = idx;
			cell.SetUserID(id);
			if (btn)
				btn.SetUserID(id);
			if (prev)
				prev.SetUserID(id);
			if (panelW)
				panelW.SetUserID(id);

			EntityAI presentItem = FindAttachment(c);
			bool present = (presentItem != null);
			string previewClass = "";

			if (presentItem)
			{
				previewClass = presentItem.GetType();
			}
			else
			{
				previewClass = token;
				previewClass = DRL_ItemTypeHelper.GetSafePreviewClass(previewClass);
			}

			if (panelW)
			{
				if (present)
					panelW.SetColor(ARGB(255, 40, 160, 60));
				else
					panelW.SetColor(ARGB(255, 170, 40, 40));
			}

			if (prev)
			{
				if (presentItem)
				{
					prev.SetItem(presentItem);
					prev.Show(true);
					if (icon)
						icon.Show(false);
					m_Menu.AddAttachedPreview(prev);
				}
				else
				{
					EntityAI dummy = m_Menu.GetPreviewEntity(previewClass);
					if (dummy)
					{
						prev.SetItem(dummy);
						prev.Show(true);
						if (icon)
							icon.Show(false);
						m_Menu.AddAttachedPreview(prev);
					}
					else if (icon)
					{
						icon.Show(true);
					}
				}
			}

			m_AttachTokens.Insert(token);
			m_AttachPreviewClasses.Insert(previewClass);
			m_AttachPresentItems.Insert(presentItem);

			idx = idx + 1;
		}

		m_AttachWrap.Update();
	}

	int FindConflictingAttachedRecipe(DRL_Craft_Recipe newDopR)
	{
		if (!newDopR || !m_SelectedAttachedRecipeIds || m_SelectedAttachedRecipeIds.Count() == 0)
			return -1;

		string newCls = newDopR.Result;
		if (newCls == "")
			return -1;

		DRL_Craft_Config cfg = m_Menu.GetCraft_Config();
		if (!cfg)
			return -1;

		EntityAI mainPrev = m_Menu.GetMainPreview();
		DRL_Craft_Recipe selRecipe = m_Menu.GetSelectedRecipe();

		bool isWeaponParent = false;
		if (mainPrev && mainPrev.IsWeapon())
		{
			isWeaponParent = true;
		}
		else if (selRecipe)
		{
			string pCls = selRecipe.PreviewClassname;
			if (pCls == "")
				pCls = selRecipe.Result;
			if (g_Game.IsKindOf(pCls, "Weapon_Base"))
			{
				isWeaponParent = true;
			}
		}

		EDRLWeaponAttachType newWeaponType = EDRLWeaponAttachType.NONE;
		if (isWeaponParent)
		{
			newWeaponType = m_Menu.DRL_GetWeaponAttachmentType(newCls);
		}

		TStringArray newSlots = new TStringArray();
		m_Menu.DRL_GetAttachmentSlots(newCls, newSlots);

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
				EDRLWeaponAttachType existWeaponType = m_Menu.DRL_GetWeaponAttachmentType(existCls);
				if (newWeaponType == existWeaponType)
				{
					return existId;
				}
			}

			TStringArray existSlots = new TStringArray();
			m_Menu.DRL_GetAttachmentSlots(existCls, existSlots);

			if (newSlots.Count() > 0 && existSlots.Count() > 0)
			{
				for (int ns = 0; ns < newSlots.Count(); ns++)
				{
					if (existSlots.Find(newSlots[ns]) > -1)
					{
						return existId;
					}
				}
			}
		}

		return -1;
	}

	bool CanAttachToCurrentBuild(string targetCls)
	{
		EntityAI mainPrev = m_Menu.GetMainPreview();
		if (targetCls == "" || !mainPrev || !mainPrev.GetInventory())
		{
			return false;
		}

		if (mainPrev.IsWeapon() && (GetGame().IsKindOf(targetCls, "Magazine_Base") || m_Menu.DRL_GetWeaponAttachmentType(targetCls) == EDRLWeaponAttachType.MAGAZINE))
		{
			int magSlotId = InventorySlots.GetSlotIdFromString("magazine");
			if (magSlotId != InventorySlots.INVALID && mainPrev.GetInventory().HasAttachmentSlot(magSlotId))
			{
				return true;
			}
		}

		TStringArray targetSlots = new TStringArray();
		m_Menu.DRL_GetAttachmentSlots(targetCls, targetSlots);

		if (targetSlots && targetSlots.Count() > 0)
		{
			for (int s = 0; s < targetSlots.Count(); s++)
			{
				int sId = InventorySlots.GetSlotIdFromString(targetSlots[s]);
				if (sId != InventorySlots.INVALID && mainPrev.GetInventory().HasAttachmentSlot(sId))
				{
					return true;
				}
			}

			array<EntityAI> children = new array<EntityAI>();
			mainPrev.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, children);
			for (int c = 0; c < children.Count(); c++)
			{
				EntityAI ch = children[c];
				if (ch && ch != mainPrev && ch.GetInventory())
				{
					for (int cs = 0; cs < targetSlots.Count(); cs++)
					{
						int csId = InventorySlots.GetSlotIdFromString(targetSlots[cs]);
						if (csId != InventorySlots.INVALID && ch.GetInventory().HasAttachmentSlot(csId))
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void BuildAttachedRecipesList(DRL_Craft_Recipe r)
	{
		if (m_DopVarWrap)
		{
			DRL_CraftUIHelper.ClearChildren(m_DopVarWrap);
		}
		m_DopWidgets.Clear();
		m_DopWidgetMap.Clear();

		if (!r || !r.HasAttachedRecipes())
		{
			if (m_DopVarWidget)
			{
				m_DopVarWidget.Show(false);
			}
			return;
		}

		if (m_DopVarWidget)
		{
			m_DopVarWidget.Show(true);
			if (m_DopVarWrap)
			{
				m_DopVarWrap.Show(true);
			}
		}

		DRL_Craft_Config cfg = m_Menu.GetCraft_Config();
		if (!cfg)
		{
			return;
		}

		for (int i = 0; i < r.AttachedRecipeIds.Count(); i++)
		{
			int attachedId = r.AttachedRecipeIds[i];
			DRL_Craft_Recipe attachedR = cfg.FindById(attachedId);
			if (!attachedR)
			{
				continue;
			}

			DRL_RecipeVarWidget ctrl = new DRL_RecipeVarWidget();
			Widget rootW = ctrl.Init(m_DopVarWrap, attachedR, m_Menu);
			if (!rootW)
			{
				continue;
			}

			bool isChecked = (m_SelectedAttachedRecipeIds.Find(attachedId) > -1);
			ctrl.SetChecked(isChecked);

			bool isAllowed = m_Menu.RecipeAllowedByContext(attachedR);
			ctrl.SetRecipeAllowed(isAllowed);

			bool isLearned = DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(attachedId);
			ctrl.SetLearned(isLearned);

			m_DopWidgets.Insert(ctrl);
			if (ctrl.GetRoot())
			{
				m_DopWidgetMap.Insert(ctrl.GetRoot(), ctrl);
			}
			if (ctrl.GetButton())
			{
				m_DopWidgetMap.Insert(ctrl.GetButton(), ctrl);
			}
			if (ctrl.GetCheckBox())
			{
				m_DopWidgetMap.Insert(ctrl.GetCheckBox(), ctrl);
			}
			if (ctrl.GetInfoButton())
			{
				m_DopWidgetMap.Insert(ctrl.GetInfoButton(), ctrl);
			}
			if (ctrl.GetMinusQuantityButton())
			{
				m_DopWidgetMap.Insert(ctrl.GetMinusQuantityButton(), ctrl);
			}
			if (ctrl.GetPlusQuantityButton())
			{
				m_DopWidgetMap.Insert(ctrl.GetPlusQuantityButton(), ctrl);
			}

			array<ButtonWidget> varBtns = ctrl.GetVariantButtons();
			if (varBtns)
			{
				for (int vb = 0; vb < varBtns.Count(); vb++)
				{
					if (varBtns[vb])
					{
						m_DopWidgetMap.Insert(varBtns[vb], ctrl);
					}
				}
			}
		}

		m_DopVarWrap.Update();
		UpdateAttachedSlotsAvailability();
	}

	void UpdateAttachedSlotsAvailability()
	{
		for (int i = 0; i < m_DopWidgets.Count(); i++)
		{
			DRL_RecipeVarWidget ctrl = m_DopWidgets[i];
			if (!ctrl)
			{
				continue;
			}

			DRL_Craft_Recipe dopR = ctrl.GetRecipe();
			if (!dopR)
			{
				continue;
			}

			bool isSelected = (m_SelectedAttachedRecipeIds.Find(dopR.RecipeId) > -1);
			bool canAttach = isSelected;
			if (!canAttach)
			{
				canAttach = CanAttachToCurrentBuild(dopR.Result);
			}

			ctrl.SetSlotAvailable(canAttach);
		}
	}

	void RebuildMainPreviewWithAttachments()
	{
		DRL_Craft_Recipe selRecipe = m_Menu.GetSelectedRecipe();
		if (!selRecipe)
		{
			return;
		}

		string mainCls = selRecipe.PreviewClassname;
		if (mainCls == "")
		{
			mainCls = selRecipe.Result;
		}

		string selRes = m_Menu.GetSelectedResultClass();
		if (selRes != "" && ((selRecipe.ResultClasses && selRecipe.ResultClasses.Find(selRes) > -1) || selRes == selRecipe.Result))
		{
			mainCls = selRes;
		}

		if (mainCls == "")
		{
			return;
		}

		EntityAI mainPrev = m_Menu.GetMainPreview();
		DRL_CraftPreviewHelper.CreatePreviewObject(mainCls, mainPrev);
		if (!mainPrev)
		{
			m_Menu.SetMainPreview(null);
			return;
		}

		m_Menu.SetMainPreview(mainPrev);

		bool isTransport = (mainPrev && mainPrev.IsInherited(Transport));
		DRL_Craft_Config cfg = m_Menu.GetCraft_Config();
		if (!isTransport && cfg && m_SelectedAttachedRecipeIds && mainPrev.GetInventory())
		{
			for (int i = 0; i < m_SelectedAttachedRecipeIds.Count(); i++)
			{
				int dopId = m_SelectedAttachedRecipeIds[i];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				if (!dopR)
				{
					continue;
				}

				string dopCls = GetSelectedAttachedClass(dopId, dopR);
				if (dopCls == "")
				{
					continue;
				}

				int attQty = GetSelectedAttachedQuantity(dopId);
				for (int q = 0; q < attQty; q++)
				{
					DRL_CraftPreviewHelper.SpawnAttachment(mainPrev, dopCls);
				}
			}
		}

		Weapon_Base wGun = Weapon_Base.Cast(mainPrev);
		if (wGun)
		{
			wGun.ShowMagazine();
		}

		ItemPreviewWidget prevW = m_Menu.GetItemPreviewWidget0();
		if (prevW)
		{
			prevW.SetItem(null);
			prevW.SetItem(mainPrev);
			int vIdx = mainPrev.GetViewIndex();
			if (vIdx < 0)
			{
				vIdx = 0;
			}
			prevW.SetView(vIdx);
			prevW.SetModelPosition(Vector(0, 0, 0.5));
			prevW.SetModelOrientation(m_Menu.GetPreviewOrientation());
			prevW.Show(true);
			m_Menu.AddAttachedPreview(prevW);
		}
	}

	void OnAttachedSelectionChanged()
	{
		RebuildMainPreviewWithAttachments();
		DRL_Craft_Recipe r = m_Menu.GetSelectedRecipe();
		if (r)
		{
			m_Menu.FillRecipeDetails(r);
			m_Menu.RefreshCraftCountText();
			m_Menu.RefreshTimeTextForCount();
			m_Menu.UpdateCraftButtonState();
			UpdateAttachedSlotsAvailability();
		}
	}

	bool HandleDopWidgetClick(Widget w)
	{
		DRL_RecipeVarWidget dopCtrl = null;
		if (m_DopWidgetMap)
		{
			Widget curW = w;
			while (curW && !dopCtrl)
			{
				m_DopWidgetMap.Find(curW, dopCtrl);
				curW = curW.GetParent();
			}
		}

		if (!dopCtrl)
			return false;

		DRL_Craft_Recipe dopR = dopCtrl.GetRecipe();
		if (!dopR)
			return false;

		int qtyDelta = 0;
		if (dopCtrl.IsQuantityButton(w, qtyDelta))
		{
			if (dopCtrl.IsChecked() && dopCtrl.ChangeQuantity(qtyDelta))
			{
				m_SelectedAttachedQuantities.Set(dopR.RecipeId, dopCtrl.GetQuantity());
				OnAttachedSelectionChanged();
			}
			return true;
		}

		int varIdx = -1;
		if (dopCtrl.IsVariantButton(w, varIdx))
		{
			dopCtrl.SelectVariant(varIdx);
			string selectedVarCls = dopCtrl.GetSelectedResultClass();
			m_SelectedAttachedVariants.Set(dopR.RecipeId, selectedVarCls);

			if (dopCtrl.IsChecked())
			{
				RebuildMainPreviewWithAttachments();
			}
			return true;
		}

		if (w == dopCtrl.GetInfoButton() || w.GetName() == "InfoRecipesDopButton")
		{
			if (dopCtrl.IsRecipeAllowed())
			{
				m_Menu.SelectRecipeDirectly(dopR);
			}
			return true;
		}

		if (w == dopCtrl.GetCheckBox() || w.GetName() == "RecipesDopCheckBox" || w == dopCtrl.GetButton() || w.GetName() == "RecipesDopButton")
		{
			if (!dopCtrl.IsLearned())
			{
				return true;
			}

			if (dopCtrl.IsChecked())
			{
				int remIdx = m_SelectedAttachedRecipeIds.Find(dopR.RecipeId);
				if (remIdx > -1)
				{
					m_SelectedAttachedRecipeIds.Remove(remIdx);
				}
				if (m_SelectedAttachedQuantities)
				{
					m_SelectedAttachedQuantities.Remove(dopR.RecipeId);
				}
				dopCtrl.SetChecked(false);
				OnAttachedSelectionChanged();
			}
			else if (dopCtrl.IsSlotAvailable())
			{
				int conflictId = FindConflictingAttachedRecipe(dopR);
				if (conflictId != -1)
				{
					int cIdx = m_SelectedAttachedRecipeIds.Find(conflictId);
					if (cIdx > -1)
					{
						m_SelectedAttachedRecipeIds.Remove(cIdx);
					}
					if (m_SelectedAttachedQuantities)
					{
						m_SelectedAttachedQuantities.Remove(conflictId);
					}
					for (int wIdx = 0; wIdx < m_DopWidgets.Count(); wIdx++)
					{
						DRL_RecipeVarWidget otherW = m_DopWidgets[wIdx];
						if (otherW && otherW.GetRecipe() && otherW.GetRecipe().RecipeId == conflictId)
						{
							otherW.SetChecked(false);
							break;
						}
					}
				}

				m_SelectedAttachedRecipeIds.Insert(dopR.RecipeId);
				m_SelectedAttachedQuantities.Set(dopR.RecipeId, dopCtrl.GetQuantity());
				dopCtrl.SetChecked(true);
				OnAttachedSelectionChanged();
			}
			else
			{
				dopCtrl.SetChecked(false);
			}
			return true;
		}

		return false;
	}
}
