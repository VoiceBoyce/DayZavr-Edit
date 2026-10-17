class DRL_CraftMergedHelper
{
	static array<string> GetVariationsForComponent(DRL_Craft_Component comp)
	{
		array<string> res = new array<string>();
		if (!comp)
		{
			return res;
		}

		if (comp.Classnames && comp.Classnames.Count() > 0)
		{
			for (int i = 0; i < comp.Classnames.Count(); i++)
			{
				string clsItem = comp.Classnames[i];
				if (clsItem == "")
				{
					continue;
				}

				if (comp.UseInheritance == 1 || !DRL_ItemTypeHelper.IsClassPlayable(clsItem))
				{
					array<string> variants = DRL_ItemTypeHelper.GetPlayableVariants(clsItem);
					if (variants)
					{
						for (int v = 0; v < variants.Count(); v++)
						{
							string vCls = variants[v];
							if (vCls != "" && res.Find(vCls) == -1)
							{
								res.Insert(vCls);
							}
						}
					}
				}
				else if (res.Find(clsItem) == -1)
				{
					res.Insert(clsItem);
				}
			}
			return res;
		}

		if (comp.Classname != "")
		{
			if (comp.UseInheritance == 1 || !DRL_ItemTypeHelper.IsClassPlayable(comp.Classname))
			{
				array<string> variantsSingle = DRL_ItemTypeHelper.GetPlayableVariants(comp.Classname);
				if (variantsSingle)
				{
					for (int sv = 0; sv < variantsSingle.Count(); sv++)
					{
						string svCls = variantsSingle[sv];
						if (svCls != "" && res.Find(svCls) == -1)
						{
							res.Insert(svCls);
						}
					}
				}
			}
			else
			{
				res.Insert(comp.Classname);
			}
		}

		return res;
	}

	static DRL_Craft_Component GetComponentByKey(int key, DRL_Craft_Recipe selectedRecipe, DRL_Craft_Config cfg)
	{
		int rId = key / 100;
		int cIdx = key % 100;
		DRL_Craft_Recipe r = null;
		if (selectedRecipe && selectedRecipe.RecipeId == rId)
		{
			r = selectedRecipe;
		}
		else if (cfg)
		{
			r = cfg.FindById(rId);
		}

		if (r && r.CraftComponents && cIdx >= 0 && cIdx < r.CraftComponents.Count())
		{
			return r.CraftComponents[cIdx];
		}
		return null;
	}

	static array<string> GetVariationsForMergedComponent(DRL_CraftMergedComponent mc, DRL_Craft_Recipe selectedRecipe, DRL_Craft_Config cfg)
	{
		array<string> allVars = new array<string>();
		if (!mc) return allVars;

		if (mc.ComponentKeys && mc.ComponentKeys.Count() > 0)
		{
			for (int k = 0; k < mc.ComponentKeys.Count(); k++)
			{
				int key = mc.ComponentKeys[k];
				DRL_Craft_Component c = GetComponentByKey(key, selectedRecipe, cfg);
				if (c)
				{
					array<string> cVars = GetVariationsForComponent(c);
					for (int v = 0; v < cVars.Count(); v++)
					{
						if (allVars.Find(cVars[v]) == -1)
						{
							allVars.Insert(cVars[v]);
						}
					}
				}
			}
		}

		if (allVars.Count() == 0)
		{
			DRL_Craft_Component cFallback = mc.ToCraftComponent();
			allVars = GetVariationsForComponent(cFallback);
		}

		return allVars;
	}

	static string GetActiveComponentClass(DRL_Craft_Component comp, int compIdx, map<int, string> selectedVariations, DRL_Craft_Recipe selectedRecipe, PlayerBase player, EntityAI contextWorkbench)
	{
		if (!comp)
		{
			return "";
		}

		if (selectedVariations)
		{
			if (selectedVariations.Contains(compIdx))
			{
				string selCls = selectedVariations.Get(compIdx);
				if (selCls != "")
				{
					return selCls;
				}
			}
			if (selectedRecipe && compIdx >= 0 && compIdx < 100)
			{
				int calcKey = selectedRecipe.RecipeId * 100 + compIdx;
				if (selectedVariations.Contains(calcKey))
				{
					string selCls2 = selectedVariations.Get(calcKey);
					if (selCls2 != "")
					{
						return selCls2;
					}
				}
			}
		}

		array<string> vars = GetVariationsForComponent(comp);
		if (vars && vars.Count() > 0)
		{
			if (player)
			{
				for (int v = 0; v < vars.Count(); v++)
				{
					string candidate = vars[v];
					if (DRL_CraftInventoryHelper.ComputeHaveForClass(candidate, comp.MinHealthFrac, null, 0.0, false, player, contextWorkbench) > 0)
					{
						return candidate;
					}
				}
			}
			if (vars.Find(comp.Classname) != -1)
			{
				return comp.Classname;
			}
			return vars[0];
		}

		return DRL_ItemTypeHelper.GetSafePreviewClass(comp.Classname);
	}

	static array<ref DRL_CraftMergedComponent> BuildMergedComponents(DRL_Craft_Recipe r, map<int, string> selectedVariations, array<int> selectedAttachedRecipeIds, DRL_Craft_Config cfg, PlayerBase player, EntityAI contextWorkbench, string targetResultClass = "")
	{
		array<ref DRL_CraftMergedComponent> mergedList = new array<ref DRL_CraftMergedComponent>();
		if (!r)
		{
			return mergedList;
		}

		string activeTargetRes = targetResultClass;
		if (activeTargetRes == "" && r)
		{
			activeTargetRes = r.Result;
		}

		if (r.CraftComponents)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c || c.AttachmentsEx > 0)
				{
					continue;
				}

				if (!c.IsApplicableForResult(activeTargetRes))
				{
					continue;
				}

				int keyMain = r.RecipeId * 100 + i;
				string mainActiveCls = GetActiveComponentClass(c, keyMain, selectedVariations, r, player, contextWorkbench);
				if (mainActiveCls == "")
				{
					mainActiveCls = c.Classname;
				}

				bool foundMain = false;
				for (int mm = 0; mm < mergedList.Count(); mm++)
				{
					DRL_CraftMergedComponent exMain = mergedList[mm];
					if (exMain && exMain.Classname == mainActiveCls)
					{
						exMain.Merge(c, keyMain);
						foundMain = true;
						break;
					}
				}

				if (!foundMain)
				{
					DRL_CraftMergedComponent mcMain = new DRL_CraftMergedComponent(c, keyMain);
					mcMain.Classname = mainActiveCls;
					mergedList.Insert(mcMain);
				}
			}
		}

		if (cfg && selectedAttachedRecipeIds)
		{
			for (int j = 0; j < selectedAttachedRecipeIds.Count(); j++)
			{
				int dopId = selectedAttachedRecipeIds[j];
				DRL_Craft_Recipe dopR = cfg.FindById(dopId);
				if (!dopR || !dopR.CraftComponents)
				{
					continue;
				}

				for (int k = 0; k < dopR.CraftComponents.Count(); k++)
				{
					DRL_Craft_Component dopC = dopR.CraftComponents[k];
					if (!dopC || dopC.AttachmentsEx > 0)
					{
						continue;
					}

					int keyDop = dopR.RecipeId * 100 + k;
					string dopActiveCls = GetActiveComponentClass(dopC, keyDop, selectedVariations, r, player, contextWorkbench);
					if (dopActiveCls == "")
					{
						dopActiveCls = dopC.Classname;
					}

					bool foundExisting = false;
					for (int m = 0; m < mergedList.Count(); m++)
					{
						DRL_CraftMergedComponent existing = mergedList[m];
						if (existing && existing.Classname == dopActiveCls)
						{
							existing.Merge(dopC, keyDop);
							foundExisting = true;
							break;
						}
					}

					if (!foundExisting)
					{
						DRL_CraftMergedComponent mcDop = new DRL_CraftMergedComponent(dopC, keyDop);
						mcDop.Classname = dopActiveCls;
						mergedList.Insert(mcDop);
					}
				}
			}
		}

		return mergedList;
	}
}
