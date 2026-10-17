class DRL_CraftVariationsController
{
	protected DRL_CraftMenuUI m_Menu;
	protected Widget m_RootWidget;
	protected WrapSpacerWidget m_ListWrap;

	void DRL_CraftVariationsController(DRL_CraftMenuUI menu, Widget rootWidget, WrapSpacerWidget listWrap)
	{
		m_Menu = menu;
		m_RootWidget = rootWidget;
		m_ListWrap = listWrap;
	}

	void SetWidgets(Widget rootWidget, WrapSpacerWidget listWrap)
	{
		m_RootWidget = rootWidget;
		m_ListWrap = listWrap;
	}

	void Hide()
	{
		if (m_RootWidget && m_RootWidget.IsVisible())
		{
			m_RootWidget.Show(false);
			if (m_Menu)
			{
				m_Menu.SetActiveVariationComponentIndex(-1);
				m_Menu.UpdateUnderlyingOverlaysState();
			}
		}
	}

	bool IsVisible()
	{
		if (m_RootWidget)
		{
			return m_RootWidget.IsVisible();
		}
		return false;
	}

	void AddVariationHeader(string text, string templatePath = "DRL_CraftMenu/UI/layout/VariationHeader.layout")
	{
		if (!m_ListWrap)
		{
			return;
		}

		Widget head = GetGame().GetWorkspace().CreateWidgets(templatePath, m_ListWrap);
		if (head)
		{
			RichTextWidget txt = RichTextWidget.Cast(head.FindAnyWidget("VariationHeaderText"));
			if (txt)
			{
				txt.SetText(text);
				txt.SetColor(ARGB(255, 226, 152, 59));
			}
		}
	}

	void CreateVariationTile(Widget parentWrap, DRL_CraftMergedComponent mc, int targetKey, string varCls, int varIdx, string activeCls, DRL_Craft_Component compRef, array<EntityAI> cachedItems, string templatePath = "DRL_CraftMenu/UI/layout/Ingredient.layout")
	{
		if (!parentWrap || !compRef || !m_Menu)
		{
			return;
		}

		Widget row = GetGame().GetWorkspace().CreateWidgets(templatePath, parentWrap);
		if (!row)
		{
			return;
		}

		DRL_VariationClickInfo info = new DRL_VariationClickInfo(mc, targetKey, varCls, varIdx);
		m_Menu.GetActiveVariationClickInfos().Insert(info);

		row.SetUserData(info);
		row.SetUserID(varIdx);

		ButtonWidget btn = ButtonWidget.Cast(row);
		if (!btn)
		{
			btn = ButtonWidget.Cast(row.FindAnyWidget("CraftIngridientButton"));
		}
		if (btn)
		{
			btn.SetUserID(varIdx);
			btn.SetUserData(info);
		}

		ImageWidget vIcon = ImageWidget.Cast(row.FindAnyWidget("VariationIcon"));
		if (vIcon)
		{
			vIcon.Show(false);
		}

		ItemPreviewWidget prev = ItemPreviewWidget.Cast(row.FindAnyWidget("CraftIngridientItemPreview"));
		TextWidget qty = TextWidget.Cast(row.FindAnyWidget("CraftIngridientItemQuantity"));
		Widget panelW = row.FindAnyWidget("CraftIngridientItemPanelWidget");
		Widget panelBorder = row.FindAnyWidget("CraftIngridientItemPanelW");

		if (prev && varCls != "")
		{
			EntityAI ew = m_Menu.GetPreviewEntity(varCls);
			if (ew)
			{
				DRL_CraftPreviewHelper.SetupPreviewWidget(prev, ew);
				m_Menu.AddAttachedPreview(prev);
			}
			else
			{
				prev.Show(false);
			}
		}

		int have = m_Menu.ComputeHaveForClass(varCls, compRef.MinHealthFrac, cachedItems, compRef.QuantityFrac);
		int need = 0;

		DRL_Craft_Recipe selRecipe = m_Menu.GetSelectedRecipe();
		int craftCount = 1;
		if (selRecipe)
		{
			craftCount = selRecipe.CurrentCraftCount;
		}

		if (targetKey >= 0)
		{
			need = m_Menu.NeedForClassCount(varCls, compRef, craftCount);
		}
		else
		{
			int totalNeed = 0;
			if (mc.ComponentKeys && mc.ComponentKeys.Count() > 0)
			{
				for (int ck = 0; ck < mc.ComponentKeys.Count(); ck++)
				{
					int keyK = mc.ComponentKeys[ck];
					DRL_Craft_Component cK = m_Menu.GetComponentByKey(keyK);
					if (cK)
					{
						array<string> cKVars = m_Menu.GetVariationsForComponent(cK);
						if (cKVars && cKVars.Find(varCls) != -1)
						{
							totalNeed += m_Menu.NeedForClassCount(varCls, cK, craftCount);
						}
						else if (varCls == cK.Classname)
						{
							totalNeed += m_Menu.NeedForClassCount(varCls, cK, craftCount);
						}
					}
				}
			}
			if (totalNeed <= 0)
			{
				totalNeed = m_Menu.NeedForClassCount(varCls, compRef, craftCount);
			}
			need = totalNeed;
		}

		if (qty)
		{
			qty.SetText(DRL_CraftInventoryHelper.FormatIngredientAmount(varCls, compRef, have, need));
			qty.Show(true);
		}

		bool isCurrentSelected = (varCls == activeCls);
		if (panelBorder)
		{
			if (isCurrentSelected)
			{
				panelBorder.SetColor(ARGB(255, 60, 160, 255));
			}
			else if (have >= need && need > 0)
			{
				panelBorder.SetColor(ARGB(255, 60, 200, 60));
			}
			else
			{
				panelBorder.SetColor(ARGB(255, 60, 60, 60));
			}
		}

		if (panelW)
		{
			if (isCurrentSelected)
			{
				panelW.SetColor(ARGB(10, 30, 120, 220));
			}
			else if (have >= need)
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

	void BuildList(DRL_CraftMergedComponent mc, array<EntityAI> cachedItems)
	{
		if (!m_RootWidget || !m_ListWrap || !mc || !m_Menu)
		{
			return;
		}

		DRL_CraftUIHelper.ClearChildren(m_ListWrap);
		m_Menu.GetActiveVariationClickInfos().Clear();

		array<EntityAI> items;
		if (cachedItems)
		{
			items = cachedItems;
		}
		else
		{
			items = new array<EntityAI>();
			m_Menu.DRL_GetInventorySnapshot(items);
		}

		DRL_Craft_Component comp = mc.ToCraftComponent();
		if (!comp)
		{
			return;
		}

		array<string> variations = m_Menu.GetVariationsForMergedComponent(mc);
		if (!variations || variations.Count() <= 1)
		{
			Hide();
			return;
		}

		m_RootWidget.Show(true);

		string activeCls = mc.Classname;
		if (activeCls == "")
		{
			activeCls = comp.Classname;
		}

		bool hasIndividualSubComps = (mc.ComponentKeys && mc.ComponentKeys.Count() > 1);

		if (hasIndividualSubComps)
		{
			AddVariationHeader("#STR_DRL_APPLY_TO_ALL_SAME_TYPE");

			for (int gk = 0; gk < variations.Count(); gk++)
			{
				string globalVarCls = variations[gk];
				CreateVariationTile(m_ListWrap, mc, -1, globalVarCls, gk, activeCls, comp, items);
			}

			AddVariationHeader("#STR_DRL_SELECT_BY_INDIVIDUAL_PARTS");

			for (int sk = 0; sk < mc.ComponentKeys.Count(); sk++)
			{
				int targetKey = mc.ComponentKeys[sk];
				DRL_Craft_Component subComp = m_Menu.GetComponentByKey(targetKey);
				if (!subComp)
				{
					continue;
				}

				int rId = targetKey / 100;
				DRL_Craft_Recipe srcRecipe = null;
				DRL_Craft_Recipe curSel = m_Menu.GetSelectedRecipe();
				if (curSel && curSel.RecipeId == rId)
				{
					srcRecipe = curSel;
				}
				else if (GetCraft_Config())
				{
					srcRecipe = GetCraft_Config().FindById(rId);
				}

				string partTitle = Widget.TranslateString("#STR_DRL_PART") + " " + (sk + 1).ToString();
				if (srcRecipe)
				{
					partTitle = srcRecipe.RecipeName;
				}

				string subActiveCls = m_Menu.GetActiveComponentClass(subComp, targetKey);
				if (subActiveCls == "")
				{
					subActiveCls = subComp.Classname;
				}

				Widget partCard = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/VariationPartSection.layout", m_ListWrap);
				if (partCard)
				{
					RichTextWidget pTitle = RichTextWidget.Cast(partCard.FindAnyWidget("VariationPartTitle"));
					if (pTitle)
					{
						pTitle.SetText("• " + partTitle);
						pTitle.SetColor(ARGB(255, 210, 185, 145));
					}

					RichTextWidget pBadge = RichTextWidget.Cast(partCard.FindAnyWidget("VariationPartQtyBadge"));
					if (pBadge)
					{
						if (subComp.QuantityFrac > 0.0)
						{
							int partPct = Math.Round(subComp.QuantityFrac * 100);
							pBadge.SetText(string.Format("[ %1% ]", partPct));
						}
						else
						{
							string subQtyFormat = Widget.TranslateString("#STR_DRL_QTY_PIECES_FORMAT");
							pBadge.SetText(string.Format(subQtyFormat, subComp.Amount * Math.Max(1, curSel.CurrentCraftCount)));
						}
						pBadge.SetColor(ARGB(200, 170, 160, 140));
					}

					WrapSpacerWidget pWrap = WrapSpacerWidget.Cast(partCard.FindAnyWidget("VariationPartTilesWrap"));
					if (!pWrap)
					{
						pWrap = m_ListWrap;
					}

					array<string> subVars = m_Menu.GetVariationsForComponent(subComp);
					for (int sv = 0; sv < subVars.Count(); sv++)
					{
						string subVarCls = subVars[sv];
						CreateVariationTile(pWrap, mc, targetKey, subVarCls, sv, subActiveCls, subComp, items);
					}
				}
			}
		}
		else
		{
			int singleKey = -1;
			if (mc.ComponentKeys && mc.ComponentKeys.Count() > 0)
			{
				singleKey = mc.ComponentKeys[0];
			}
			for (int sk0 = 0; sk0 < variations.Count(); sk0++)
			{
				string varCls0 = variations[sk0];
				CreateVariationTile(m_ListWrap, mc, singleKey, varCls0, sk0, activeCls, comp, items);
			}
		}

		m_RootWidget.Update();
		m_ListWrap.Update();
		m_Menu.UpdateUnderlyingOverlaysState();
	}
}
