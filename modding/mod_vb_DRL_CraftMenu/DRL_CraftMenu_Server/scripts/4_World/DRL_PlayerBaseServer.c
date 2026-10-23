modded class PlayerBase
{
    protected bool m_DRL_ReCraftActive = false;
    protected vector m_DRL_CustomSpawnPos = "0 0 0";
    protected ref map<int, string> m_DRL_AttachedVariantsOverride;
    protected ref map<int, string> m_DRL_SelectedVariationsOverride;

    void DRL_SetCustomSpawnPos(vector pos)
    {
        m_DRL_CustomSpawnPos = pos;
    }

    vector DRL_GetCustomSpawnPos()
    {
        return m_DRL_CustomSpawnPos;
    }

    void DRL_SetAttachedVariantsOverride(map<int, string> overrides)
    {
        m_DRL_AttachedVariantsOverride = overrides;
    }

    map<int, string> DRL_GetAttachedVariantsOverride()
    {
        return m_DRL_AttachedVariantsOverride;
    }

    void DRL_SetSelectedVariationsOverride(map<int, string> overrides)
    {
        m_DRL_SelectedVariationsOverride = overrides;
    }

    map<int, string> DRL_GetSelectedVariationsOverride()
    {
        return m_DRL_SelectedVariationsOverride;
    }
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
		
        if (rpc_type == DRL_CraftRPC.CTS_CRAFT)
		{
			Param8<int, string, int, int, int, vector, array<int>, array<string>> p8;
			if (ctx.Read(p8))
			{
				int netLow8 = p8.param4;
				int netHigh8 = p8.param5;
				vector aimPos8 = p8.param6;
		
				EntityAI wb8 = null;
				if (netLow8 != 0 || netHigh8 != 0)
				{
					Object o8 = GetGame().GetObjectByNetworkId(netLow8, netHigh8);
					wb8 = EntityAI.Cast(o8);
				}

				map<int, string> selectedVars8 = new map<int, string>();
				if (p8.param7 && p8.param8)
				{
					for (int v8 = 0; v8 < p8.param7.Count(); v8++)
					{
						if (v8 < p8.param8.Count())
						{
							selectedVars8.Set(p8.param7[v8], p8.param8[v8]);
						}
					}
				}
		
				DRL_SetCurrentWorkbench(wb8);
				DRL_SetCustomSpawnPos(aimPos8);
				DRL_SetSelectedVariationsOverride(selectedVars8);
				TryCraftRecipeWithResultCount(p8.param1, p8.param2, p8.param3);
				DRL_SetSelectedVariationsOverride(null);
				DRL_SetCustomSpawnPos("0 0 0");
				return;
			}

			Param6<int,string,int,int,int,vector> p6;
			if (ctx.Read(p6))
			{
				int netLow6 = p6.param4;
				int netHigh6 = p6.param5;
				vector aimPos6 = p6.param6;
		
				EntityAI wb6 = null;
				if (netLow6 != 0 || netHigh6 != 0)
				{
					Object o6 = GetGame().GetObjectByNetworkId(netLow6, netHigh6);
					wb6 = EntityAI.Cast(o6);
				}
		
				DRL_SetCurrentWorkbench(wb6);
				DRL_SetCustomSpawnPos(aimPos6);
				TryCraftRecipeWithResultCount(p6.param1, p6.param2, p6.param3);
				DRL_SetCustomSpawnPos("0 0 0");
				return;
			}

			Param5<int,string,int,int,int> p5;
			if (ctx.Read(p5))
			{
				int netLow = p5.param4;
				int netHigh = p5.param5;
		
				EntityAI wb = null;
				if (netLow != 0 || netHigh != 0)
				{
					Object o = GetGame().GetObjectByNetworkId(netLow, netHigh);
					wb = EntityAI.Cast(o);
				}
		
				DRL_SetCurrentWorkbench(wb);
				TryCraftRecipeWithResultCount(p5.param1, p5.param2, p5.param3);
				return;
			}
			
			DRL_SetCurrentWorkbench(null);
		
			Param3<int,string,int> p3;
			if (ctx.Read(p3))
			{
				TryCraftRecipeWithResultCount(p3.param1, p3.param2, p3.param3);
				return;
			}
		
			Param2<int,string> p2v;
			if (ctx.Read(p2v))
			{
				TryCraftRecipeWithResultCount(p2v.param1, p2v.param2, 1);
				return;
			}
		
			Param1<int> p1v;
			if (ctx.Read(p1v))
			{
				TryCraftRecipeWithResultCount(p1v.param1, "", 1);
				return;
			}
		}
		
		if (rpc_type == DRL_CraftRPC.CTS_RECRAFT)
		{
			Param4<int, string, int, int> data4;
			if (ctx.Read(data4))
			{
				int netLowRecraft = data4.param3;
				int netHighRecraft = data4.param4;
				EntityAI wbRecraft = null;
				if (netLowRecraft != 0 || netHighRecraft != 0)
				{
					Object oRecraft = GetGame().GetObjectByNetworkId(netLowRecraft, netHighRecraft);
					wbRecraft = EntityAI.Cast(oRecraft);
				}
				DRL_SetCurrentWorkbench(wbRecraft);
				TryReCraftRecipeExact(data4.param1, data4.param2);
				return;
			}
		}

		if (rpc_type == DRL_CraftRPC.CTS_CRAFT_WITH_ATTACHMENTS)
		{
			Param9<int, string, int, vector, array<int>, vector, array<string>, array<int>, array<string>> pAtt9;
			if (ctx.Read(pAtt9))
			{
				vector netPosAtt9 = pAtt9.param4;
				int netLowAtt9 = Math.Round(netPosAtt9[0]);
				int netHighAtt9 = Math.Round(netPosAtt9[1]);

				EntityAI wbAtt9 = null;
				if (netLowAtt9 != 0 || netHighAtt9 != 0)
				{
					Object oAtt9 = GetGame().GetObjectByNetworkId(netLowAtt9, netHighAtt9);
					wbAtt9 = EntityAI.Cast(oAtt9);
				}

				map<int, string> varMap9 = new map<int, string>();
				if (pAtt9.param5 && pAtt9.param7)
				{
					for (int vIdx9 = 0; vIdx9 < pAtt9.param5.Count(); vIdx9++)
					{
						if (vIdx9 < pAtt9.param7.Count())
						{
							varMap9.Set(pAtt9.param5[vIdx9], pAtt9.param7[vIdx9]);
						}
					}
				}

				map<int, string> compVarMap9 = new map<int, string>();
				if (pAtt9.param8 && pAtt9.param9)
				{
					for (int cv9 = 0; cv9 < pAtt9.param8.Count(); cv9++)
					{
						if (cv9 < pAtt9.param9.Count())
						{
							compVarMap9.Set(pAtt9.param8[cv9], pAtt9.param9[cv9]);
						}
					}
				}

				DRL_SetCurrentWorkbench(wbAtt9);
				DRL_SetCustomSpawnPos(pAtt9.param6);
				DRL_SetAttachedVariantsOverride(varMap9);
				DRL_SetSelectedVariationsOverride(compVarMap9);
				TryCraftRecipeWithAttachments(pAtt9.param1, pAtt9.param2, pAtt9.param3, pAtt9.param5, compVarMap9, varMap9);
				DRL_SetSelectedVariationsOverride(null);
				DRL_SetAttachedVariantsOverride(null);
				DRL_SetCustomSpawnPos("0 0 0");
				return;
			}

			Param7<int, string, int, vector, array<int>, vector, array<string>> pAtt7;
			if (ctx.Read(pAtt7))
			{
				vector netPosAtt7 = pAtt7.param4;
				int netLowAtt7 = Math.Round(netPosAtt7[0]);
				int netHighAtt7 = Math.Round(netPosAtt7[1]);

				EntityAI wbAtt7 = null;
				if (netLowAtt7 != 0 || netHighAtt7 != 0)
				{
					Object oAtt7 = GetGame().GetObjectByNetworkId(netLowAtt7, netHighAtt7);
					wbAtt7 = EntityAI.Cast(oAtt7);
				}

				map<int, string> varMap = new map<int, string>();
				if (pAtt7.param5 && pAtt7.param7)
				{
					for (int vIdx = 0; vIdx < pAtt7.param5.Count(); vIdx++)
					{
						if (vIdx < pAtt7.param7.Count())
						{
							varMap.Set(pAtt7.param5[vIdx], pAtt7.param7[vIdx]);
						}
					}
				}

				DRL_SetCurrentWorkbench(wbAtt7);
				DRL_SetCustomSpawnPos(pAtt7.param6);
				DRL_SetAttachedVariantsOverride(varMap);
				TryCraftRecipeWithAttachments(pAtt7.param1, pAtt7.param2, pAtt7.param3, pAtt7.param5, null, varMap);
				DRL_SetAttachedVariantsOverride(null);
				DRL_SetCustomSpawnPos("0 0 0");
				return;
			}

			Param6<int, string, int, vector, array<int>, vector> pAtt6;
			if (ctx.Read(pAtt6))
			{
				vector netPosAtt6 = pAtt6.param4;
				int netLowAtt6 = Math.Round(netPosAtt6[0]);
				int netHighAtt6 = Math.Round(netPosAtt6[1]);

				EntityAI wbAtt6 = null;
				if (netLowAtt6 != 0 || netHighAtt6 != 0)
				{
					Object oAtt6 = GetGame().GetObjectByNetworkId(netLowAtt6, netHighAtt6);
					wbAtt6 = EntityAI.Cast(oAtt6);
				}

				DRL_SetCurrentWorkbench(wbAtt6);
				DRL_SetCustomSpawnPos(pAtt6.param6);
				TryCraftRecipeWithAttachments(pAtt6.param1, pAtt6.param2, pAtt6.param3, pAtt6.param5);
				DRL_SetCustomSpawnPos("0 0 0");
				return;
			}

			Param5<int, string, int, vector, array<int>> pAtt;
			if (ctx.Read(pAtt))
			{
				vector netPos = pAtt.param4;
				int netLow2 = Math.Round(netPos[0]);
				int netHigh2 = Math.Round(netPos[1]);

				EntityAI wb2 = null;
				if (netLow2 != 0 || netHigh2 != 0)
				{
					Object o2 = GetGame().GetObjectByNetworkId(netLow2, netHigh2);
					wb2 = EntityAI.Cast(o2);
				}

				DRL_SetCurrentWorkbench(wb2);
				TryCraftRecipeWithAttachments(pAtt.param1, pAtt.param2, pAtt.param3, pAtt.param5);
				return;
			}
		}

		if (rpc_type == DRL_CraftRPC.CTS_STUDY_RECIPE)
		{
			Param2<int, int> pStudy;
			if (ctx.Read(pStudy))
			{
				TryStudyRecipe(pStudy.param1, pStudy.param2);
				return;
			}
		}

		if (rpc_type == DRL_CraftRPC.CTS_CANCEL_CRAFT)
		{
			Param1<string> pCancel;
			if (ctx.Read(pCancel))
			{
				DRL_CraftProcessManager.GetInstance().CancelProcess(pCancel.param1, this);
				return;
			}
		}

		if (rpc_type == DRL_CraftRPC.CTS_REQUEST_ACTIVE_PROCESSES)
		{
			Param2<int, int> pReq;
			if (ctx.Read(pReq))
			{
				EntityAI reqWb = null;
				if (pReq.param1 != 0 || pReq.param2 != 0)
				{
					Object oReq = GetGame().GetObjectByNetworkId(pReq.param1, pReq.param2);
					reqWb = EntityAI.Cast(oReq);
				}

				array<ref DRL_CraftProcessSyncInfo> activeList;
				DRL_CraftProcessManager.GetInstance().GetActiveProcessesForPlayerOrWorkbench(this, reqWb, activeList);
				RPCSingleParam(DRL_CraftRPC.STC_SYNC_ACTIVE_PROCESSES, new Param1<array<ref DRL_CraftProcessSyncInfo>>(activeList), true, sender);
				return;
			}
		}
    }

	void TryCraftRecipeWithResultCount(int id, string overrideCls, int count)
	{
		DRL_Craft_Recipe r = GetCraft_Config().FindById(id);
		if (!r) return;

		if (!DRL_HasLearnedRecipe(id))
		{
			DRL_CraftLogger.Warn("Player " + DRL_GetPlayerIdSafe() + " tried to craft recipe " + id.ToString() + " without learning it first!");
			return;
		}
	
		int want = count;
		if (want < 1) want = 1;
	
		string resultCls = r.Result;
		if (overrideCls != "" && r.ResultClasses && r.ResultClasses.Find(overrideCls) > -1)
			resultCls = overrideCls;
		
		if (!DRL_HasRequiredWorkbenchForRecipe(r)) return;
		if (!DRL_HasRequiredAttachments(r)) return;

		DRL_Craft_WB wb = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());
		string blockReason;
		if (wb && !wb.DRL_CanCraftResult(resultCls, this, blockReason))
		{
			DRL_CraftProcessManager.GetInstance().SendCraftNotification(this, EDRLCraftNotifType.SPACE_BLOCKED, resultCls, "", "", blockReason);
			return;
		}
	
		map<int, string> selectedVariations = m_DRL_SelectedVariationsOverride;

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
			if (!c.IsApplicableForResult(resultCls)) continue;

			DRL_Craft_Component compCheck = c;
			int varKey = r.RecipeId * 100 + i;
			if (selectedVariations)
			{
				string chosenVar = "";
				if (selectedVariations.Contains(varKey))
				{
					chosenVar = selectedVariations.Get(varKey);
				}
				else if (selectedVariations.Contains(i))
				{
					chosenVar = selectedVariations.Get(i);
				}

				if (chosenVar != "")
				{
					compCheck = c.CloneWithVariation(chosenVar);
				}
			}

			if (!DRL_HasComponent(compCheck, want))
			{
				DRL_CraftProcessManager.GetInstance().SendCraftNotification(this, EDRLCraftNotifType.NO_INGREDIENTS, resultCls);
				return;
			}
		}

		float totalDuration = r.CraftTimeSec * want;
		DRL_CraftProcessManager.GetInstance().StartProcess(this, r, resultCls, want, wb, null, selectedVariations, totalDuration, m_DRL_CustomSpawnPos);
	}
	
	void DRL_CompleteCraftExactNTimes(DRL_Craft_Recipe r, string cls, int crafts)
	{
		if (!r) return;
	
		string outCls = cls;
		if (outCls == "")
			outCls = r.Result;
	
		int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(outCls);
		int perCraftSpawns = r.ResultCount;
		if (qType == EDRLItemQuantityType.STACK || qType == EDRLItemQuantityType.MAGAZINE || DRL_ItemQuantityHelper.IsFillingType(qType))
		{
			perCraftSpawns = 1;
		}
	
		int craftsClamped = crafts;
		if (craftsClamped < 1) craftsClamped = 1;
	
		for (int k = 0; k < craftsClamped; k++)
		{
			for (int i = 0; i < perCraftSpawns; i++)
			{
				EntityAI created = DRL_SpawnCraftResult(outCls, false, m_DRL_CustomSpawnPos);
				if (!created) continue;
	
				ItemBase ib = ItemBase.Cast(created);
				if (ib)
				{
					DRL_SetResultAmount(ib, r);
	
					if (r.ResultDamageFrac > 0.0)
					{
						float health01 = 1.0 - r.ResultDamageFrac;
						if (health01 < 0.0) health01 = 0.0;
						if (health01 > 1.0) health01 = 1.0;
						ib.SetHealth01("", "Health", health01);
					}
				}
			}
		}
		
		DRL_Craft_WB wbFinish = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());
		if (wbFinish)
		{
			wbFinish.DRL_OnCraftFinish(outCls, null, this);
		}

		DRL_SetCurrentWorkbench(null);
	}

	void TryCraftRecipeWithAttachments(int id, string overrideCls, int count, array<int> attachedRecipeIds, map<int, string> selectedVariations = null, map<int, string> attachedVariants = null)
	{
		if (!selectedVariations)
		{
			selectedVariations = m_DRL_SelectedVariationsOverride;
		}
		if (!attachedVariants)
		{
			attachedVariants = m_DRL_AttachedVariantsOverride;
		}

		DRL_Craft_Recipe r = GetCraft_Config().FindById(id);
		if (!r) return;

		if (!DRL_HasLearnedRecipe(id))
		{
			DRL_CraftLogger.Warn("Player " + DRL_GetPlayerIdSafe() + " tried to craft recipe " + id.ToString() + " with attachments without learning it first!");
			return;
		}

		int want = count;
		if (want < 1) want = 1;

		string resultCls = r.Result;
		if (overrideCls != "" && r.ResultClasses && r.ResultClasses.Find(overrideCls) > -1)
			resultCls = overrideCls;

		if (!DRL_HasRequiredWorkbenchForRecipe(r)) return;
		if (!DRL_HasRequiredAttachments(r)) return;

		DRL_Craft_WB wbAtt = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());
		string blockReasonAtt;
		if (wbAtt && !wbAtt.DRL_CanCraftResult(resultCls, this, blockReasonAtt))
		{
			DRL_CraftProcessManager.GetInstance().SendCraftNotification(this, EDRLCraftNotifType.SPACE_BLOCKED, resultCls, "", "", blockReasonAtt);
			return;
		}

		array<int> validAttachedIds = new array<int>();
		array<DRL_Craft_Recipe> validAttachedRecipes = new array<DRL_Craft_Recipe>();

		if (attachedRecipeIds && r.AttachedRecipeIds)
		{
			for (int a = 0; a < attachedRecipeIds.Count(); a++)
			{
				int dopId = attachedRecipeIds[a];
				if (r.AttachedRecipeIds.Find(dopId) > -1)
				{
					DRL_Craft_Recipe dopR = GetCraft_Config().FindById(dopId);
					if (dopR)
					{
						validAttachedIds.Insert(dopId);
						validAttachedRecipes.Insert(dopR);
					}
				}
			}
		}

		if (validAttachedRecipes.Count() == 0)
		{
			TryCraftRecipeWithResultCount(id, overrideCls, count);
			return;
		}

		array<ref DRL_CraftMergedComponent> mergedList = new array<ref DRL_CraftMergedComponent>();
		if (r.CraftComponents)
		{
			for (int i = 0; i < r.CraftComponents.Count(); i++)
			{
				DRL_Craft_Component c = r.CraftComponents[i];
				if (!c) continue;
				if (!c.IsApplicableForResult(resultCls)) continue;

				DRL_Craft_Component compEffective = c;
				int keyMain = r.RecipeId * 100 + i;
				if (selectedVariations)
				{
					string chosenMain = "";
					if (selectedVariations.Contains(keyMain))
					{
						chosenMain = selectedVariations.Get(keyMain);
					}
					else if (selectedVariations.Contains(i))
					{
						chosenMain = selectedVariations.Get(i);
					}

					if (chosenMain != "")
					{
						compEffective = c.CloneWithVariation(chosenMain);
					}
				}
				mergedList.Insert(new DRL_CraftMergedComponent(compEffective, keyMain));
			}
		}

		int totalTimeSec = r.CraftTimeSec;

		for (int j = 0; j < validAttachedRecipes.Count(); j++)
		{
			DRL_Craft_Recipe attR = validAttachedRecipes[j];
			totalTimeSec += attR.CraftTimeSec;

			if (!attR.CraftComponents) continue;
			for (int k = 0; k < attR.CraftComponents.Count(); k++)
			{
				DRL_Craft_Component attC = attR.CraftComponents[k];
				if (!attC) continue;

				DRL_Craft_Component attEffective = attC;
				int keyDop = attR.RecipeId * 100 + k;
				if (selectedVariations)
				{
					string chosenDop = "";
					if (selectedVariations.Contains(keyDop))
					{
						chosenDop = selectedVariations.Get(keyDop);
					}
					else if (selectedVariations.Contains(k))
					{
						chosenDop = selectedVariations.Get(k);
					}

					if (chosenDop != "")
					{
						attEffective = attC.CloneWithVariation(chosenDop);
					}
				}

				bool found = false;
				for (int m = 0; m < mergedList.Count(); m++)
				{
					DRL_CraftMergedComponent ex = mergedList[m];
					if (ex && ex.Classname == attEffective.Classname)
					{
						ex.Merge(attEffective, keyDop);
						found = true;
						break;
					}
				}
				if (!found)
				{
					mergedList.Insert(new DRL_CraftMergedComponent(attEffective, keyDop));
				}
			}
		}

		array<ref DRL_Craft_Component> finalComps = new array<ref DRL_Craft_Component>();
		for (int f = 0; f < mergedList.Count(); f++)
		{
			DRL_CraftMergedComponent mcFinal = mergedList[f];
			if (mcFinal)
			{
				finalComps.Insert(mcFinal.ToCraftComponent());
			}
		}

		for (int compIdx = 0; compIdx < finalComps.Count(); compIdx++)
		{
			DRL_Craft_Component chkComp = finalComps[compIdx];
			if (!chkComp) continue;
			if (!DRL_HasComponent(chkComp, want))
			{
				DRL_CraftProcessManager.GetInstance().SendCraftNotification(this, EDRLCraftNotifType.NO_INGREDIENTS, resultCls);
				return;
			}
		}

		float totalAttDuration = totalTimeSec * want;
		DRL_CraftProcessManager.GetInstance().StartProcess(this, r, resultCls, want, wbAtt, validAttachedIds, selectedVariations, totalAttDuration, m_DRL_CustomSpawnPos, attachedVariants);
	}

	EntityAI DRL_SpawnAttachmentServer(EntityAI parent, string itemClass, int depth = 0)
	{
		if (!parent || !parent.GetInventory() || itemClass == "")
			return null;

		EntityAI attEntity = null;

		Weapon_Base wGun = Weapon_Base.Cast(parent);
		if (wGun && (GetGame().IsKindOf(itemClass, "Magazine_Base") || DRL_AttachmentHelper.GetWeaponAttachmentType(itemClass) == EDRLWeaponAttachType.MAGAZINE))
		{
			int magSlotId = InventorySlots.GetSlotIdFromString("magazine");
			if (magSlotId != InventorySlots.INVALID && parent.GetInventory().HasAttachmentSlot(magSlotId))
			{
				attEntity = parent.GetInventory().CreateAttachmentEx(itemClass, magSlotId);
			}
			if (!attEntity)
			{
				attEntity = parent.GetInventory().CreateAttachment(itemClass);
			}

			if (attEntity)
			{
				return attEntity;
			}
		}

		TStringArray candidateSlots = new TStringArray();
		DRL_AttachmentHelper.GetAttachmentSlots(itemClass, candidateSlots);

		if (candidateSlots && candidateSlots.Count() > 0)
		{
			for (int s = 0; s < candidateSlots.Count(); s++)
			{
				int slotId = InventorySlots.GetSlotIdFromString(candidateSlots[s]);
				if (slotId != InventorySlots.INVALID && parent.GetInventory().HasAttachmentSlot(slotId))
				{
					if (!parent.GetInventory().FindAttachment(slotId))
					{
						attEntity = parent.GetInventory().CreateAttachmentEx(itemClass, slotId);
						if (attEntity)
						{
							return attEntity;
						}
					}
				}
			}
		}

		attEntity = parent.GetInventory().CreateAttachment(itemClass);
		if (attEntity)
		{
			return attEntity;
		}

		if (depth < 2)
		{
			int childCount = parent.GetInventory().AttachmentCount();
			for (int c = 0; c < childCount; c++)
			{
				EntityAI child = parent.GetInventory().GetAttachmentFromIndex(c);
				if (child && child != parent)
				{
					attEntity = DRL_SpawnAttachmentServer(child, itemClass, depth + 1);
					if (attEntity)
					{
						return attEntity;
					}
				}
			}
		}

		return null;
	}

	void DRL_CompleteCraftWithAttachments(DRL_Craft_Recipe r, string cls, int crafts, array<int> attachedRecipeIds)
	{
		if (!r) return;

		string outCls = cls;
		if (outCls == "")
			outCls = r.Result;

		int craftsClamped = crafts;
		if (craftsClamped < 1) craftsClamped = 1;

		for (int k = 0; k < craftsClamped; k++)
		{
			EntityAI created = DRL_SpawnCraftResult(outCls, false, m_DRL_CustomSpawnPos);
			if (!created) continue;

			ItemBase ib = ItemBase.Cast(created);
			if (ib)
			{
				DRL_SetResultAmount(ib, r);

				if (r.ResultDamageFrac > 0.0)
				{
					float health01 = 1.0 - r.ResultDamageFrac;
					if (health01 < 0.0) health01 = 0.0;
					if (health01 > 1.0) health01 = 1.0;
					ib.SetHealth01("", "Health", health01);
				}
			}

			if (attachedRecipeIds)
			{
				for (int a = 0; a < attachedRecipeIds.Count(); a++)
				{
					int dopId = attachedRecipeIds[a];
					DRL_Craft_Recipe dopR = GetCraft_Config().FindById(dopId);
					if (!dopR) continue;

					string dopCls = dopR.Result;
					if (m_DRL_AttachedVariantsOverride && m_DRL_AttachedVariantsOverride.Contains(dopId))
					{
						dopCls = m_DRL_AttachedVariantsOverride.Get(dopId);
					}
					if (dopCls == "") continue;

					EntityAI attItem = DRL_SpawnAttachmentServer(created, dopCls);

					if (!attItem)
					{
						attItem = DRL_SpawnCraftResult(dopCls);
					}

					if (attItem)
					{
						ItemBase attIb = ItemBase.Cast(attItem);
						if (attIb)
						{
							DRL_SetResultAmount(attIb, dopR);
							if (dopR.ResultDamageFrac > 0.0)
							{
								float attHealth01 = 1.0 - dopR.ResultDamageFrac;
								if (attHealth01 < 0.0) attHealth01 = 0.0;
								if (attHealth01 > 1.0) attHealth01 = 1.0;
								attIb.SetHealth01("", "Health", attHealth01);
							}
						}
					}
				}
			}
		}

		DRL_Craft_WB liftAttFinish = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());
		if (liftAttFinish)
		{
			liftAttFinish.DRL_OnCraftFinish(outCls, null, this);
		}

		DRL_SetCurrentWorkbench(null);
	}
	
	protected bool DRL_IsFillingItem(ItemBase ib)
	{
		if (!ib) return false;
		int qType = DRL_ItemQuantityHelper.GetQuantityType(ib);
		return DRL_ItemQuantityHelper.IsFillingType(qType);
	}
	
	protected bool DRL_ShouldUseQuantityBar(ItemBase ib)
	{
		if (!ib) return false;
		int qType = DRL_ItemQuantityHelper.GetQuantityType(ib);
		return (qType == EDRLItemQuantityType.RESOURCE_BAR || qType == EDRLItemQuantityType.LIQUID);
	}
	
	protected void DRL_SetResultAmount(EntityAI resultEnt, DRL_Craft_Recipe r)
	{
		if (!resultEnt || !r) return;
		DRL_ItemQuantityHelper.SetFilling(resultEnt, r.ResultQuantityFrac, r.ResultCount);
	}

	protected bool DRL_IsFillingClassname(string clsLower)
	{
		if (clsLower == "") return false;
		int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(clsLower);
		return DRL_ItemQuantityHelper.IsFillingType(qType);
	}
	
	protected bool DRL_IsStackPieceItem(ItemBase ib)
	{
		if (!ib) return false;
		return (DRL_ItemQuantityHelper.GetQuantityType(ib) == EDRLItemQuantityType.STACK);
	}
	
	bool DRL_HasRequiredWorkbenchForRecipe(DRL_Craft_Recipe r)
	{
		if (!r) return false;
		
		if (r.ShowInFreeMenu == 1)
			return true;
		
		if (!r.WorkbenchClasses || r.WorkbenchClasses.Count() == 0)
			return true;
	
		float maxDist = 6.0;
		vector pos = GetPosition();
	
		array<Object> objs = new array<Object>();
		array<CargoBase> prx = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(pos, maxDist, objs, prx);
	
		for (int i = 0; i < objs.Count(); i++)
		{
			EntityAI ea = EntityAI.Cast(objs[i]);
			if (!ea) continue;
	
			string t = ea.GetType();
			for (int j = 0; j < r.WorkbenchClasses.Count(); j++)
			{
				if (t == r.WorkbenchClasses[j])
					return true;
			}
		}
		return false;
	}
	
	protected void DRL_DamageItemByPct(EntityAI item, float pct)
	{
		if (!item || pct <= 0.0) return;
	
		float maxH = item.GetMaxHealth("", "Health");
		if (maxH <= 0) return;
	
		float dmg = maxH * pct;
		item.AddHealth("", "Health", -dmg);
	}
	
	bool DRL_IsComponentHealthyAttachmentSrv(ItemBase ib, DRL_Craft_Component c)
	{
		if (!ib || !c) return false;
		if (ib.IsRuined()) return false;
		int maxLevel = c.MinHealthFrac;
		if (maxLevel < 0) maxLevel = 0;
		if (maxLevel > 3) maxLevel = 3;
		return ib.GetHealthLevel() <= maxLevel;
	}

	bool DRL_DoesItemMatchComponent(ItemBase ib, DRL_Craft_Component c)
	{
		if (!ib || !c) return false;

		if (c.Classnames && c.Classnames.Count() > 0)
		{
			string ibType = ib.GetType();
			ibType.ToLower();
			for (int ci = 0; ci < c.Classnames.Count(); ci++)
			{
				string cReq = c.Classnames[ci];
				if (c.UseInheritance == 1)
				{
					if (ib.IsKindOf(cReq))
					{
						return true;
					}
				}
				else
				{
					string cReqLower = cReq;
					cReqLower.ToLower();
					if (ibType == cReqLower)
					{
						return true;
					}
				}
			}
			return false;
		}

		if (c.UseInheritance == 1)
		{
			return ib.IsKindOf(c.Classname);
		}
		
		string typeMatch = ib.GetType();
		typeMatch.ToLower();
		string reqMatch = c.Classname;
		reqMatch.ToLower();
		return (typeMatch == reqMatch);
	}

	bool DRL_DoesItemFitSlotSrv(ItemBase ib, int targetSlotId)
	{
		if (!ib || !ib.GetInventory() || targetSlotId < 0) return false;
		int slotCount = ib.GetInventory().GetSlotIdCount();
		for (int i = 0; i < slotCount; i++)
		{
			if (ib.GetInventory().GetSlotId(i) == targetSlotId)
				return true;
		}
		return false;
	}

	ItemBase DRL_FindMatchingAttachmentSrv(EntityAI owner, DRL_Craft_Component c, bool slotsOnly = false)
	{
		if (!owner || !c || !owner.GetInventory()) return null;
		
		string token = c.Classname;
		if (token == "" && c.Classnames && c.Classnames.Count() > 0)
		{
			token = c.Classnames[0];
		}
		if (token == "") return null;

		if (InventorySlots.GetSlotIdFromString(token) > -1)
		{
			ItemBase ibSlot = ItemBase.Cast(owner.FindAttachmentBySlotName(token));
			if (ibSlot && DRL_IsComponentHealthyAttachmentSrv(ibSlot, c))
			{
				return ibSlot;
			}
		}
		else
		{
			int ac = owner.GetInventory().AttachmentCount();
			for (int i = 0; i < ac; i++)
			{
				ItemBase ib = ItemBase.Cast(owner.GetInventory().GetAttachmentFromIndex(i));
				if (!ib) continue;

				bool match = DRL_DoesItemMatchComponent(ib, c);
				if (match && DRL_IsComponentHealthyAttachmentSrv(ib, c))
				{
					return ib;
				}
			}
		}

		if (slotsOnly) return null;

		if (c.AttachmentsEx == 2)
		{
			array<EntityAI> items = new array<EntityAI>();
			owner.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
			for (int j = 0; j < items.Count(); j++)
			{
				ItemBase ibInv = ItemBase.Cast(items[j]);
				if (!ibInv || ibInv == owner) continue;

				InventoryLocation il = new InventoryLocation();
				if (ibInv.GetInventory() && ibInv.GetInventory().GetCurrentInventoryLocation(il))
				{
					if (il.GetType() == InventoryLocationType.ATTACHMENT && il.GetParent() == owner)
					{
						continue;
					}
				}

				bool matchInv = false;
				int sId = InventorySlots.GetSlotIdFromString(token);
				if (sId > -1)
				{
					if (DRL_DoesItemFitSlotSrv(ibInv, sId))
					{
						matchInv = true;
					}
					else if (c.Classnames && c.Classnames.Count() > 0)
					{
						matchInv = DRL_DoesItemMatchComponent(ibInv, c);
					}
				}
				else
				{
					matchInv = DRL_DoesItemMatchComponent(ibInv, c);
				}

				if (matchInv && DRL_IsComponentHealthyAttachmentSrv(ibInv, c))
				{
					return ibInv;
				}
			}
		}

		return null;
	}

	ItemBase DRL_FindAttachmentComponentSrv(DRL_Craft_Component c, EntityAI nearWB)
	{
		if (!c) return null;

		ItemBase att = null;
		if (nearWB)
		{
			att = DRL_FindMatchingAttachmentSrv(nearWB, c, true);
			if (att) return att;
		}

		att = DRL_FindMatchingAttachmentSrv(this, c, true);
		if (att) return att;

		if (c.AttachmentsEx == 2)
		{
			if (nearWB)
			{
				att = DRL_FindMatchingAttachmentSrv(nearWB, c, false);
				if (att) return att;
			}

			att = DRL_FindMatchingAttachmentSrv(this, c, false);
			if (att) return att;
		}

		return null;
	}

	bool DRL_IsHealthyAttachmentSrv(ItemBase ib)
	{
		if (!ib) return false;
		if (ib.IsRuined()) return false;
		int hl = ib.GetHealthLevel();
		return hl <= 2;
	}
	
	bool DRL_ServerHasAttachmentByClassHealthy(EntityAI owner, string cls)
	{
		if (!owner || !owner.GetInventory()) return false;
		int ac = owner.GetInventory().AttachmentCount();
		for (int i = 0; i < ac; i++)
		{
			ItemBase ib = ItemBase.Cast(owner.GetInventory().GetAttachmentFromIndex(i));
			if (!ib) continue;
			if (ib.GetType() != cls) continue;
			if (DRL_IsHealthyAttachmentSrv(ib)) return true;
		}
		return false;
	}
	
	void DRL_ApplyRequiredAttachmentsWear(DRL_Craft_Recipe r, string resultCls = "")
	{
		if (!r || !r.CraftComponents) return;

		EntityAI nearWB = DRL_FindNearbyWorkbenchForRecipe(r, 6.0);
		if (!nearWB) nearWB = DRL_GetCurrentWorkbench();

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0) continue;
			if (!c.IsApplicableForResult(resultCls)) continue;

			ItemBase target = DRL_CraftComponentResolver.FindItemForComponent(c, this, nearWB, resultCls);
			if (target)
			{
				if (c.Delete)
				{
					GetGame().ObjectDelete(target);
				}
				else if (c.ApplyDamageFrac > 0.0)
				{
					DRL_DamageItemByPct(target, c.ApplyDamageFrac);
				}
			}
		}
	}
	
	protected EntityAI DRL_FindNearbyWorkbenchForRecipe(DRL_Craft_Recipe r, float maxDist = 6.0)
	{
		if (!r || !r.WorkbenchClasses || r.WorkbenchClasses.Count() == 0) return null;
	
		vector pos = GetPosition();
		array<Object> objs = new array<Object>();
		array<CargoBase> prx = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(pos, maxDist, objs, prx);
	
		for (int i = 0; i < objs.Count(); i++)
		{
			Object o = objs[i];
			if (!o) continue;
			string t = o.GetType(); t.Trim();
	
			for (int j = 0; j < r.WorkbenchClasses.Count(); j++)
			{
				string reqWb = r.WorkbenchClasses[j];
				reqWb.Trim();
				if (reqWb == "") continue;
				if (o.IsKindOf(reqWb) || t == reqWb)
					return EntityAI.Cast(o);
			}
		}
		return null;
	}
	
	protected bool DRL_AttachmentExistsInSlotsByClass(EntityAI owner, string cls)
	{
		if (!owner || !owner.GetInventory()) return false;
		int ac = owner.GetInventory().AttachmentCount();
		for (int i = 0; i < ac; i++)
		{
			EntityAI a = owner.GetInventory().GetAttachmentFromIndex(i);
			if (a && a.GetType() == cls) return true;
		}
		return false;
	}
	
	bool DRL_HasRequiredAttachments(DRL_Craft_Recipe r, string resultCls = "")
	{
		if (!r || !r.CraftComponents) return true;

		EntityAI nearWB = DRL_FindNearbyWorkbenchForRecipe(r, 6.0);
		if (!nearWB) nearWB = DRL_GetCurrentWorkbench();

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0) continue;
			if (!c.IsApplicableForResult(resultCls)) continue;

			ItemBase att = DRL_CraftComponentResolver.FindItemForComponent(c, this, nearWB, resultCls);
			if (!att)
			{
				return false;
			}
		}
		return true;
	}

	bool DRL_HasRequiredRecraftAttachments(DRL_Craft_Recipe r, string resultCls = "")
	{
		if (!r || !r.CraftComponents) return true;

		EntityAI nearWB = DRL_FindNearbyWorkbenchForRecipe(r, 6.0);
		if (!nearWB) nearWB = DRL_GetCurrentWorkbench();

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0 || c.RecraftNeedAttachments != 1) continue;
			if (!c.IsApplicableForResult(resultCls)) continue;

			ItemBase att = DRL_CraftComponentResolver.FindItemForComponent(c, this, nearWB, resultCls);
			if (!att)
			{
				return false;
			}
		}
		return true;
	}

	void DRL_ApplyRequiredRecraftAttachmentsWear(DRL_Craft_Recipe r, string resultCls = "")
	{
		if (!r || !r.CraftComponents) return;

		EntityAI nearWB = DRL_FindNearbyWorkbenchForRecipe(r, 6.0);
		if (!nearWB) nearWB = DRL_GetCurrentWorkbench();

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c || c.AttachmentsEx <= 0 || c.RecraftNeedAttachments != 1) continue;
			if (!c.IsApplicableForResult(resultCls)) continue;

			ItemBase att = DRL_CraftComponentResolver.FindItemForComponent(c, this, nearWB, resultCls);
			if (att)
			{
				if (c.Delete)
				{
					GetGame().ObjectDelete(att);
				}
				else if (c.ApplyDamageFrac > 0.0)
				{
					DRL_DamageItemByPct(att, c.ApplyDamageFrac);
				}
			}
		}
	}
	
	bool DRL_HasAttachments(array<string> atts)
	{
		if (!atts || atts.Count() == 0) return true;
		for (int i = 0; i < atts.Count(); i++)
		{
			string cls = atts[i];
			array<ItemBase> pool;
			DRL_CollectItemsByClass(cls, 3, pool);
			if (pool.Count() == 0) return false;
		}
		return true;
	}
	
	void TryCraftRecipe(int id)
	{
		DRL_Craft_Recipe r = GetCraft_Config().FindById(id);
		if (!r) return;

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
			if (!DRL_HasComponent(c, 1))
				return;
		}

		for (int j = 0; j < r.CraftComponents.Count(); j++)
		{
			DRL_Craft_Component c2 = r.CraftComponents[j];
			if (!c2) continue;
			if (!DRL_ConsumeComponent(c2, 1))
				return;
		}
	
		int delayMs = r.CraftTimeSec * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DRL_CompleteCraft, delayMs, false, r);
		
		if (r.WorkbenchClasses && r.WorkbenchClasses.Count() > 0)
		{
			float maxDist = 6.0;
			vector pos = GetPosition();
			array<Object> objs = new array<Object>();
			array<CargoBase> prx = new array<CargoBase>();
			GetGame().GetObjectsAtPosition(pos, maxDist, objs, prx);
		
			bool ok = false;
			for (int ip = 0; ip < objs.Count(); ip++)
			{
				string t = objs[ip].GetType(); t.Trim();
				for (int jp = 0; jp < r.WorkbenchClasses.Count(); jp++)
				{
					if (t == r.WorkbenchClasses[jp]) { ok = true; break; }
				}
				if (ok) break;
			}
			if (!ok) return;
		}
		
		if (!DRL_HasRequiredWorkbenchForRecipe(r))
		{
			return;
		}
		
		if (!DRL_HasRequiredAttachments(r))
			return;
		
		DRL_ApplyRequiredAttachmentsWear(r);
	}
	
	void TryCraftRecipeWithResult(int id, string overrideCls)
	{
		DRL_Craft_Recipe r = GetCraft_Config().FindById(id);
		if (!r) return;
		
		string resultCls = r.Result;
		if (overrideCls && overrideCls != "" && r.ResultClasses && r.ResultClasses.Find(overrideCls) > -1)
			resultCls = overrideCls;
		
		for (int i2 = 0; i2 < r.CraftComponents.Count(); i2++)
		{
			DRL_Craft_Component cComp = r.CraftComponents[i2];
			if (!cComp) continue;
			if (!DRL_HasComponent(cComp, 1))
				return;
		}
	
		for (int j2 = 0; j2 < r.CraftComponents.Count(); j2++)
		{
			DRL_Craft_Component c2Comp = r.CraftComponents[j2];
			if (!c2Comp) continue;
			if (!DRL_ConsumeComponent(c2Comp, 1))
				return;
		}
	
		if (!DRL_HasRequiredWorkbenchForRecipe(r))
			return;
	
		if (!DRL_HasRequiredAttachments(r))
			return;
		
		int delayMs = r.CraftTimeSec * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DRL_CompleteCraftExact, delayMs, false, r, resultCls);
	
		DRL_ApplyRequiredAttachmentsWear(r);
	}
	
	void TryReCraftRecipe(int id)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg) return;
	
		if (m_DRL_ReCraftActive) return;
		m_DRL_ReCraftActive = true;
	
		DRL_Craft_Recipe r = cfg.FindById(id);
		if (!r) { m_DRL_ReCraftActive = false; return; }
		if (!r.Result || r.Result == "") { m_DRL_ReCraftActive = false; return; }
	
		if (!DRL_HasRequiredRecraftAttachments(r))
		{
			m_DRL_ReCraftActive = false;
			return;
		}

		int delay = Math.Max(1, Math.Round(r.GetRecraftTimeSec() * 1000));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DRL_DoReCraftNow, delay, false, r, "");
	}
	
	protected DRL_Craft_Recipe DRL_FindRecipeByIdAndResult(int id, string resultCls)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg) return null;
		
		DRL_Craft_Recipe r = cfg.FindById(id);
		if (!r) return null;

		if (resultCls == "") return r;

		string targetCls = resultCls;
		targetCls.ToLower();

		string rRes = r.Result;
		rRes.ToLower();
		if (rRes == targetCls) return r;

		if (r.ResultClasses)
		{
			foreach (string rc : r.ResultClasses)
			{
				string rcL = rc;
				rcL.ToLower();
				if (rcL == targetCls) return r;
			}
		}
		
		return r;
	}
	
	void TryReCraftRecipeExact(int id, string resultCls)
	{
		if (m_DRL_ReCraftActive) return;
		m_DRL_ReCraftActive = true;
	
		DRL_Craft_Recipe r = DRL_FindRecipeByIdAndResult(id, resultCls);
		if (!r) { m_DRL_ReCraftActive = false; return; }
	
		if (!DRL_HasRequiredRecraftAttachments(r, resultCls))
		{
			m_DRL_ReCraftActive = false;
			return;
		}

		int delay = Math.Max(1, Math.Round(r.GetRecraftTimeSec() * 1000));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.DRL_DoReCraftNow, delay, false, r, resultCls);
	}

	bool DRL_HasComponent(DRL_Craft_Component c, int craftCount)
	{
		if (!c)
		{
			return false;
		}

		EntityAI wb = DRL_GetCurrentWorkbenchRoot();
		return DRL_CraftComponentResolver.HasComponentRequirement(c, this, wb, "", craftCount);
	}

	bool DRL_ConsumeComponent(DRL_Craft_Component c, int craftCount)
	{
		if (!c)
		{
			return false;
		}

		EntityAI wb = DRL_GetCurrentWorkbenchRoot();
		return DRL_CraftComponentResolver.ExecuteComponentConsumption(c, this, wb, "", craftCount);
	}



	void DRL_CompleteCraft(DRL_Craft_Recipe r)
	{
		if (!r) return;
	
		int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(r.Result);
		int perCraftSpawns = r.ResultCount;
		if (qType == EDRLItemQuantityType.STACK || qType == EDRLItemQuantityType.MAGAZINE || DRL_ItemQuantityHelper.IsFillingType(qType))
		{
			perCraftSpawns = 1;
		}
	
		for (int i = 0; i < perCraftSpawns; i++)
		{
			EntityAI created = DRL_SpawnCraftResult(r.Result);
			if (!created) continue;
	
			ItemBase ib = ItemBase.Cast(created);
			if (ib)
			{
				DRL_SetResultAmount(ib, r);
	
				if (r.ResultDamageFrac > 0.0)
				{
					float health01 = 1.0 - r.ResultDamageFrac;
					if (health01 < 0.0) health01 = 0.0;
					if (health01 > 1.0) health01 = 1.0;
					ib.SetHealth01("", "", health01);
				}
			}
		}
	}
	
	void DRL_CompleteCraftExact(DRL_Craft_Recipe r, string cls)
	{
		if (!r) return;
	
		string outCls = cls;
		if (!outCls || outCls == "")
			outCls = r.Result;
	
		int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(outCls);
		int perCraftSpawns = r.ResultCount;
		if (qType == EDRLItemQuantityType.STACK || qType == EDRLItemQuantityType.MAGAZINE || DRL_ItemQuantityHelper.IsFillingType(qType))
		{
			perCraftSpawns = 1;
		}
	
		for (int i = 0; i < perCraftSpawns; i++)
		{
			EntityAI created = DRL_SpawnCraftResult(outCls);
			if (!created) continue;
	
			ItemBase ib = ItemBase.Cast(created);
			if (ib)
			{
				DRL_SetResultAmount(ib, r);
	
				if (r.ResultDamageFrac > 0.0)
				{
					float health01 = 1.0 - r.ResultDamageFrac;
					if (health01 < 0.0) health01 = 0.0;
					if (health01 > 1.0) health01 = 1.0;
					ib.SetHealth01("", "", health01);
				}
			}
		}
		
		DRL_SetCurrentWorkbench(null);
	}
	
	protected void DRL_GiveRecraftComponent(DRL_Craft_WB wb, DRL_Craft_Component c, array<EntityAI> wbItems)
	{
		if (!c || c.Classname == "")
		{
			return;
		}

		int cQType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(c.Classname);
		int qMaxProbe = DRL_ItemQuantityHelper.GetStackMax(null, c.Classname);
		string targetCls = c.Classname;
		targetCls.ToLower();

		if (cQType == EDRLItemQuantityType.STACK)
		{
			int totalUnits = Math.Max(1, c.RecraftAmount);
			if (c.RecraftQuantityFrac > 0.0 && qMaxProbe > 0)
			{
				int perUnit = Math.Clamp(Math.Round(qMaxProbe * c.RecraftQuantityFrac), 1, qMaxProbe);
				totalUnits = totalUnits * perUnit;
			}

			if (wb)
			{
				if (wbItems)
				{
					for (int j = 0; j < wbItems.Count(); j++)
					{
						ItemBase ibExist = ItemBase.Cast(wbItems[j]);
						if (!ibExist || ibExist.IsRuined() || ibExist.DRL_IsCraftLocked())
						{
							continue;
						}

						string existType = ibExist.GetType();
						existType.ToLower();
						if (existType != targetCls && !ibExist.IsKindOf(c.Classname))
						{
							continue;
						}

						if (!DRL_ItemQuantityHelper.IsStackable(ibExist))
						{
							continue;
						}

						int curQ = ibExist.GetQuantity();
						int maxQ = ibExist.GetQuantityMax();
						if (maxQ <= 0)
						{
							maxQ = qMaxProbe;
						}

						if (curQ < maxQ)
						{
							int space = maxQ - curQ;
							int addQ = Math.Min(totalUnits, space);
							ibExist.AddQuantity(addQ);
							totalUnits -= addQ;
							if (totalUnits <= 0)
							{
								break;
							}
						}
					}
				}

				while (totalUnits > 0)
				{
					int give = totalUnits;
					if (qMaxProbe > 0 && give > qMaxProbe)
					{
						give = qMaxProbe;
					}

					EntityAI created = wb.DRL_CreateCraftResult(c.Classname, null, false);
					if (created)
					{
						ItemBase ibCreated = ItemBase.Cast(created);
						if (ibCreated && ibCreated.HasQuantity())
						{
							ibCreated.SetQuantity(give);
						}
						if (wbItems)
						{
							wbItems.Insert(created);
						}
					}
					else
					{
						EntityAI flb = DRL_SpawnCraftResult(c.Classname, true);
						if (flb)
						{
							ItemBase ibFlb = ItemBase.Cast(flb);
							if (ibFlb && ibFlb.HasQuantity())
							{
								ibFlb.SetQuantity(give);
							}
						}
					}

					totalUnits -= give;
					if (qMaxProbe <= 0)
					{
						break;
					}
				}
			}
			else
			{
				while (totalUnits > 0)
				{
					int giveG = totalUnits;
					if (qMaxProbe > 0 && giveG > qMaxProbe)
					{
						giveG = qMaxProbe;
					}
					DRL_SpawnWithQty(this, c.Classname, giveG, true);
					totalUnits -= giveG;
					if (qMaxProbe <= 0)
					{
						break;
					}
				}
			}
			return;
		}

		bool isFilling = DRL_ItemQuantityHelper.IsFillingType(cQType) || (c.RecraftQuantityFrac > 0.0);
		if (isFilling)
		{
			float qMaxRaw = DRL_ItemQuantityHelper.GetQuantityMaxRaw(null, c.Classname);
			int qMaxFill = Math.Round(qMaxRaw);
			if (qMaxFill <= 0)
			{
				qMaxFill = 1;
			}

			float fillFrac = c.RecraftQuantityFrac;
			if (fillFrac <= 0.0)
			{
				fillFrac = 1.0;
			}

			int plainCount = Math.Max(1, c.RecraftAmount);
			int totalFillUnits = Math.Round(qMaxFill * fillFrac) * plainCount;
			if (totalFillUnits <= 0)
			{
				totalFillUnits = 1;
			}

			if (wb)
			{
				if (wbItems)
				{
					for (int k = 0; k < wbItems.Count(); k++)
					{
						ItemBase ibFillExist = ItemBase.Cast(wbItems[k]);
						if (!ibFillExist || ibFillExist.IsRuined() || ibFillExist.DRL_IsCraftLocked())
						{
							continue;
						}

						string fillType = ibFillExist.GetType();
						fillType.ToLower();
						if (fillType != targetCls && !ibFillExist.IsKindOf(c.Classname))
						{
							continue;
						}

						int curQFill = ibFillExist.GetQuantity();
						int maxQFill = ibFillExist.GetQuantityMax();
						if (maxQFill <= 0)
						{
							maxQFill = qMaxFill;
						}

						if (curQFill < maxQFill)
						{
							int spaceFill = maxQFill - curQFill;
							int addFill = Math.Min(totalFillUnits, spaceFill);
							ibFillExist.AddQuantity(addFill);
							totalFillUnits -= addFill;
							if (totalFillUnits <= 0)
							{
								break;
							}
						}
					}
				}

				while (totalFillUnits > 0)
				{
					int giveFill = totalFillUnits;
					if (qMaxFill > 0 && giveFill > qMaxFill)
					{
						giveFill = qMaxFill;
					}

					EntityAI createdFill = wb.DRL_CreateCraftResult(c.Classname, null, false);
					if (!createdFill)
					{
						createdFill = DRL_SpawnCraftResult(c.Classname, true);
					}

					if (createdFill)
					{
						ItemBase ibCreatedFill = ItemBase.Cast(createdFill);
						if (ibCreatedFill && ibCreatedFill.HasQuantity())
						{
							ibCreatedFill.SetQuantity(giveFill);
						}
						else
						{
							float fracVal = (float)giveFill / (float)Math.Max(1, qMaxFill);
							DRL_ItemQuantityHelper.SetFilling(createdFill, fracVal, 1);
						}

						if (wbItems)
						{
							wbItems.Insert(createdFill);
						}
					}

					totalFillUnits -= giveFill;
					if (qMaxFill <= 0)
					{
						break;
					}
				}
			}
			else
			{
				for (int nW = 0; nW < plainCount; nW++)
				{
					EntityAI spawned = DRL_SpawnCraftResult(c.Classname, true);
					if (spawned)
					{
						DRL_ItemQuantityHelper.SetFilling(spawned, c.RecraftQuantityFrac, 1);
					}
				}
			}
			return;
		}

		int discreteCount = Math.Max(1, c.RecraftAmount);
		for (int d = 0; d < discreteCount; d++)
		{
			if (wb)
			{
				EntityAI createdDisc = wb.DRL_CreateCraftResult(c.Classname, null, false);
				if (!createdDisc)
				{
					createdDisc = DRL_SpawnCraftResult(c.Classname, true);
				}
				if (createdDisc && wbItems)
				{
					wbItems.Insert(createdDisc);
				}
			}
			else
			{
				DRL_SpawnCraftResult(c.Classname, true);
			}
		}
	}
	
	void DRL_DoReCraftNow(DRL_Craft_Recipe r, string chosenCls)
	{
		if (!r) { m_DRL_ReCraftActive = false; return; }
		if (!r.Result || r.Result == "") { m_DRL_ReCraftActive = false; return; }
		
		string targetCls = r.Result;
		if (chosenCls != "")
			targetCls = chosenCls;
		
		array<ItemBase> pool;
		DRL_CollectItemsByClass(targetCls, 3, pool);
		
		ItemBase resIB = null;
		if (pool && pool.Count() > 0)
		{
			resIB = pool[0];
		}
		else
		{
			EntityAI inHands = GetHumanInventory().GetEntityInHands();
			ItemBase ibHands = ItemBase.Cast(inHands);
			if (ibHands && ibHands.IsKindOf(targetCls) && !ibHands.IsRuined())
				resIB = ibHands;
		}
		if (!resIB) { m_DRL_ReCraftActive = false; return; }
		int sourceUnits = 1;
		int consumeQty  = 0;
		int resQType = DRL_ItemQuantityHelper.GetQuantityType(resIB);
		if (resIB && resIB.HasQuantity() && resQType != EDRLItemQuantityType.DISCRETE)
		{
			int qMaxRes = resIB.GetQuantityMax();
			int unitQty = 1;

			if (DRL_ItemQuantityHelper.IsFillingType(resQType) && r.ResultQuantityFrac > 0.0 && qMaxRes > 0)
			{
				unitQty = Math.Max(1, Math.Round(qMaxRes * r.ResultQuantityFrac));
			}

			if (unitQty <= 0) unitQty = 1;

			sourceUnits = Math.Max(1, resIB.GetQuantity() / unitQty);
			consumeQty  = sourceUnits * unitQty;
		}
		
		DRL_ApplyRequiredRecraftAttachmentsWear(r, chosenCls);

		DRL_Craft_WB wb = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());

		array<EntityAI> wbItems = new array<EntityAI>();
		if (wb)
		{
			EntityAI rootWb = wb.GetHierarchyRoot();
			if (!rootWb || rootWb.IsInherited(Man))
			{
				rootWb = wb;
			}

			if (rootWb && rootWb.GetInventory())
			{
				rootWb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, wbItems);
			}
			else if (wb.GetInventory())
			{
				wb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, wbItems);
			}
		}

		for (int i = 0; i < r.CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = r.CraftComponents[i];
			if (!c) continue;
			
			float chanceClamped = c.RecraftDropChance;
			if (chanceClamped < 0.0) chanceClamped = 0.0;
			if (chanceClamped > 1.0) chanceClamped = 1.0;
			if (chanceClamped <= 0.0) continue;
			
			float roll = Math.RandomFloat01();
			if (roll >= chanceClamped) continue;
			
			DRL_GiveRecraftComponent(wb, c, wbItems);
		}
		
		if (resIB && resIB.HasQuantity())
		{
			if (consumeQty <= 0) consumeQty = 1;
			resIB.AddQuantity(-consumeQty);
			if (resIB.GetQuantity() <= 0)
				GetGame().ObjectDelete(resIB);
		}
		else
		{
			GetGame().ObjectDelete(resIB);
		}
	
		m_DRL_ReCraftActive = false;
		DRL_SetCurrentWorkbench(null);
	}
	
	protected void DRL_ApplyRecraftFill(EntityAI ent, float frac)
	{
		if (!ent) return;
		DRL_ItemQuantityHelper.SetFilling(ent, frac, 1);
	}
	
	protected void DRL_SafeDelete(EntityAI e)
	{
		if (!e) return;
		GetGame().ObjectDelete(e);
	}
	
	ItemBase DRL_FindUnderfilledStack(PlayerBase pb, string cls)
	{
		array<EntityAI> items = new array<EntityAI>();
		pb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
	
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib) continue;
			if (!ib.IsKindOf(cls)) continue;
			if (!ib.HasQuantity()) continue;
	
			int qty_cur = ib.GetQuantity();
			int qty_max = ib.GetQuantityMax();
			if (qty_cur < qty_max) return ib;
		}
		return null;
	}
	
	void DRL_FindItemsByClassname(string classname, out array<EntityAI> outItems)
	{
		outItems = new array<EntityAI>();
		if (classname == "") return;
	
		array<EntityAI> items = new array<EntityAI>();
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
	
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib) continue;
			
			InventoryLocation il = new InventoryLocation();
			if (ib.GetInventory() && ib.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == this)
						continue;
				}
			}
	
			if (!ib.IsKindOf(classname)) continue;
			if (ib.IsRuined()) continue;
	
			outItems.Insert(ib);
		}
	}
	
	ItemBase DRL_SpawnWithQty(PlayerBase pb, string cls, int qty, bool groundOnly = false)
	{
		EntityAI e = DRL_SpawnCraftResult(cls, groundOnly);
		if (!e)
		{
			return null;
		}
	
		ItemBase ib = ItemBase.Cast(e);
		if (ib && ib.HasQuantity())
		{
			if (qty < 1)
			{
				qty = 1;
			}
			ib.SetQuantity(qty);
		}
	
		return ib;
	}
	
	int DRL_GetQMaxForClass(PlayerBase pb, string cls)
	{
		return Math.Round(DRL_ItemQuantityHelper.GetQuantityMaxRaw(null, cls));
	}
	
	protected EntityAI DRL_SpawnCraftResult(string cls, bool groundOnly = false, vector customSpawnPos = "0 0 0")
	{
		if (!this) return null;
		if (cls == "") return null;

		vector spawnPos = customSpawnPos;
		if (spawnPos == "0 0 0")
		{
			spawnPos = m_DRL_CustomSpawnPos;
		}

		DRL_Craft_WB wb = DRL_Craft_WB.Cast(DRL_GetCurrentWorkbench());
		if (wb)
		{
			return wb.DRL_CreateCraftResult(cls, this, groundOnly, spawnPos);
		}

		return DRL_SpawnCraftResultDefault(cls, groundOnly, spawnPos);
	}

	protected EntityAI DRL_SpawnCraftResultDefault(string cls, bool groundOnly = false, vector customSpawnPos = "0 0 0")
	{
		if (!this) return null;
		if (cls == "") return null;

		if (GetGame().IsKindOf(cls, "Transport"))
		{
			vector vehSpawnPos = customSpawnPos;
			vector playerPos = GetPosition();
			if (vehSpawnPos == "0 0 0" || vector.DistanceSq(vehSpawnPos, playerPos) > 1225.0)
			{
				vector fwd = GetDirection();
				fwd.Normalize();
				vehSpawnPos = playerPos + Vector(fwd[0] * 5.0, 0.2, fwd[2] * 5.0);
			}

			int vehFlags = ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_KEEPHEIGHT;
			EntityAI veh = EntityAI.Cast(GetGame().CreateObjectEx(cls, vehSpawnPos, vehFlags));
			if (veh)
			{
				veh.SetOrientation(GetOrientation());
				dBodyApplyImpulse(veh, vector.Up);
				return veh;
			}
		}

		if (!groundOnly && GetHumanInventory())
		{
			EntityAI inHands = GetHumanInventory().CreateInHands(cls);
			if (inHands)
			{
				return inHands;
			}
		}

		int iFlags = ECE_SETUP | ECE_KEEPHEIGHT | ECE_PLACE_ON_SURFACE;
		vector dropPos = GetPosition();
		EntityAI groundItem = EntityAI.Cast(GetGame().CreateObjectEx(cls, dropPos, iFlags));
		if (groundItem)
		{
			groundItem.SetOrientation(GetOrientation());
			return groundItem;
		}
		return null;
	}

	string DRL_GetPlayerIdSafe()
	{
		PlayerIdentity id = GetIdentity();
		if (id)
		{
			string plainId = id.GetPlainId();
			if (plainId != "")
			{
				return plainId;
			}
		}
		return "offline_player";
	}

	bool DRL_HasLearnedRecipe(int recipeId)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg)
		{
			return true;
		}

		if (!cfg.IsLearningEnabled())
		{
			return true;
		}

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg || !lcfg.RecipeRequiresLearning(recipeId))
		{
			return true;
		}

		string pId = DRL_GetPlayerIdSafe();
		DRL_PlayerLearningData pData = GetPlayerLearningManager().GetPlayerData(pId);
		if (!pData)
		{
			return false;
		}

		return pData.HasLearned(recipeId);
	}

	int DRL_CountItemsInInventory(string clsName)
	{
		if (clsName == "")
		{
			return 0;
		}

		array<EntityAI> items = new array<EntityAI>();
		if (GetInventory())
		{
			GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}

		string targetClsLower = clsName;
		targetClsLower.ToLower();

		int total = 0;
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib)
			{
				continue;
			}

			if (ib.IsRuined())
			{
				continue;
			}

			InventoryLocation il = new InventoryLocation();
			if (ib.GetInventory() && ib.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == this)
					{
						string ibType = ib.GetType();
						ibType.ToLower();
						if (ibType != targetClsLower && !ib.IsKindOf(clsName))
						{
							continue;
						}
					}
				}
			}

			string t = ib.GetType();
			t.ToLower();
			if (t == targetClsLower || ib.IsKindOf(clsName))
			{
				total += DRL_ItemQuantityHelper.GetStackAmount(ib);
			}
		}

		return total;
	}

	bool DRL_ConsumeItemsFromInventory(string clsName, int count)
	{
		if (clsName == "" || count <= 0)
		{
			return true;
		}

		array<EntityAI> items = new array<EntityAI>();
		if (GetInventory())
		{
			GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}

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
				int qType = DRL_ItemQuantityHelper.GetQuantityType(ib);
				if (qType == EDRLItemQuantityType.STACK)
				{
					Magazine mgzAmmo = Magazine.Cast(ib);
					if (mgzAmmo && mgzAmmo.IsAmmoPile())
					{
						int ammo = mgzAmmo.GetAmmoCount();
						if (ammo <= leftToConsume)
						{
							leftToConsume -= ammo;
							GetGame().ObjectDelete(mgzAmmo);
						}
						else
						{
							mgzAmmo.ServerSetAmmoCount(ammo - leftToConsume);
							leftToConsume = 0;
						}
					}
					else if (ib.HasQuantity())
					{
						int q = ib.GetQuantity();
						if (q <= leftToConsume)
						{
							leftToConsume -= q;
							GetGame().ObjectDelete(ib);
						}
						else
						{
							ib.SetQuantity(q - leftToConsume);
							leftToConsume = 0;
						}
					}
				}
				else
				{
					leftToConsume -= 1;
					GetGame().ObjectDelete(ib);
				}
			}
		}

		return (leftToConsume <= 0);
	}

	void TryStudyRecipe(int primaryRecipeId, int variationIndex)
	{
		DRL_Craft_Config cfg = GetCraft_Config();
		DRL_Learning_Config lConfig = GetLearning_Config();
		if (!lConfig || lConfig.EnableLearning != 1)
		{
			return;
		}

		DRL_Learning_Recipe lr = lConfig.FindByPrimaryId(primaryRecipeId);
		if (!lr)
		{
			lr = lConfig.FindByAnyRecipeId(primaryRecipeId);
		}
		if (!lr)
		{
			DRL_CraftLogger.Warn("TryStudyRecipe: recipe " + primaryRecipeId.ToString() + " not found in learning config!");
			return;
		}

		if (cfg && !cfg.IsFreeLearningMenuEnabled())
		{
			float maxDist = 6.0;
			vector pos = GetPosition();
			array<Object> objs = new array<Object>();
			array<CargoBase> prx = new array<CargoBase>();
			GetGame().GetObjectsAtPosition(pos, maxDist, objs, prx);

			bool nearTable = false;
			for (int oi = 0; oi < objs.Count(); oi++)
			{
				EntityAI ent = EntityAI.Cast(objs[oi]);
				if (ent && ent.IsKindOf("DRL_Craft_Research_Table"))
				{
					nearTable = true;
					break;
				}
			}

			if (!nearTable)
			{
				DRL_CraftLogger.Warn("Player " + DRL_GetPlayerIdSafe() + " tried to study recipe without being near Research_Table!");
				return;
			}
		}

		string pId = DRL_GetPlayerIdSafe();
		DRL_PlayerLearningData pData = GetPlayerLearningManager().GetPlayerData(pId);
		if (!pData)
		{
			return;
		}

		if (pData.HasLearned(primaryRecipeId))
		{
			DRL_CraftLogger.Info("Player " + pId + " has already learned recipe " + primaryRecipeId.ToString());
			if (GetIdentity())
			{
				RPCSingleParam(DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE, new Param3<bool, int, array<int>>(true, primaryRecipeId, pData.LearnedRecipeIds), true, GetIdentity());
			}
			return;
		}

		if (lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
		{
			for (int reqIdx = 0; reqIdx < lr.RequiredRecipeIds.Count(); reqIdx++)
			{
				int reqId = lr.RequiredRecipeIds[reqIdx];
				if (!pData.HasLearned(reqId))
				{
					DRL_CraftLogger.Warn("Player " + pId + " tried to study recipe " + primaryRecipeId.ToString() + " without prerequisite recipe " + reqId.ToString());
					if (GetIdentity())
					{
						RPCSingleParam(DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE, new Param3<bool, int, array<int>>(false, primaryRecipeId, pData.LearnedRecipeIds), true, GetIdentity());
					}
					return;
				}
			}
		}

		DRL_Craft_Recipe targetRec = null;
		if (cfg)
		{
			targetRec = cfg.FindById(primaryRecipeId);
		}

		if (!DRL_CheckStudyRequirements(lr, targetRec))
		{
			DRL_CraftLogger.Warn("Player " + pId + " failed custom study requirements for recipe " + primaryRecipeId.ToString());
			if (GetIdentity())
			{
				RPCSingleParam(DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE, new Param3<bool, int, array<int>>(false, primaryRecipeId, pData.LearnedRecipeIds), true, GetIdentity());
			}
			return;
		}

		if (variationIndex < 0 || variationIndex >= lr.StudyVariations.Count())
		{
			DRL_CraftLogger.Warn("TryStudyRecipe: invalid variation index " + variationIndex.ToString());
			return;
		}

		DRL_Study_Variation v = lr.StudyVariations[variationIndex];
		if (!v)
		{
			return;
		}

		int haveCount = DRL_CountItemsInInventory(v.RequiredItem);
		if (haveCount < v.Amount)
		{
			DRL_CraftLogger.Warn("Player " + pId + " lacks required item " + v.RequiredItem + " (" + haveCount.ToString() + "/" + v.Amount.ToString() + ")");
			if (GetIdentity())
			{
				RPCSingleParam(DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE, new Param3<bool, int, array<int>>(false, primaryRecipeId, pData.LearnedRecipeIds), true, GetIdentity());
			}
			return;
		}

		if (v.Delete == 1)
		{
			DRL_ConsumeItemsFromInventory(v.RequiredItem, v.Amount);
		}

		if (lr.IsStudyMain())
		{
			for (int rIdx = 0; rIdx < lr.RecipeIds.Count(); rIdx++)
			{
				pData.AddRecipe(lr.RecipeIds[rIdx]);
			}
			DRL_CraftLogger.Info("Player " + pId + " successfully studied recipe group for main " + primaryRecipeId.ToString());
		}
		else
		{
			pData.AddRecipe(primaryRecipeId);
			DRL_CraftLogger.Info("Player " + pId + " successfully studied individual recipe " + primaryRecipeId.ToString());
		}

		GetPlayerLearningManager().SavePlayerData(pData);

		DRL_OnStudySuccessPostActions(lr, targetRec);

		if (GetIdentity())
		{
			RPCSingleParam(DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE, new Param3<bool, int, array<int>>(true, primaryRecipeId, pData.LearnedRecipeIds), true, GetIdentity());

			string learnedResultCls = "";
			if (targetRec)
			{
				learnedResultCls = targetRec.Result;
			}
			DRL_CraftProcessManager.GetInstance().SendCraftNotification(this, EDRLCraftNotifType.RECIPE_LEARNED, learnedResultCls);
		}
	}

	override bool DRL_CheckStudyRequirements(DRL_Learning_Recipe lr, DRL_Craft_Recipe r)
	{
		return super.DRL_CheckStudyRequirements(lr, r);
	}

	override void DRL_OnStudySuccessPostActions(DRL_Learning_Recipe lr, DRL_Craft_Recipe r)
	{
		super.DRL_OnStudySuccessPostActions(lr, r);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		DRL_Learning_Config lcfg = GetLearning_Config();
		if (lcfg && lcfg.ResetLearnedOnDeath == 1)
		{
			string pId = DRL_GetPlayerIdSafe();
			DRL_PlayerLearningData pData = GetPlayerLearningManager().GetPlayerData(pId);
			if (pData)
			{
				pData.LearnedRecipeIds.Clear();
				GetPlayerLearningManager().SavePlayerData(pData);
				if (GetIdentity())
				{
					RPCSingleParam(DRL_CraftRPC.STC_SYNC_LEARNED_RECIPES, new Param1<array<int>>(pData.LearnedRecipeIds), true, GetIdentity());
				}
			}
		}
	}
}
