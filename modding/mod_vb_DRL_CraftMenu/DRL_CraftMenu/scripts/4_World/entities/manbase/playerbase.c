modded class PlayerBase
{
	protected EntityAI m_DRL_CurrentWorkbench;

	void DRL_SetCurrentWorkbench(EntityAI wb)
	{
		m_DRL_CurrentWorkbench = wb;
	}

	EntityAI DRL_GetCurrentWorkbench()
	{
		return m_DRL_CurrentWorkbench;
	}

	EntityAI DRL_GetCurrentWorkbenchRoot()
	{
		if (!m_DRL_CurrentWorkbench) return null;
		EntityAI root = m_DRL_CurrentWorkbench.GetHierarchyRoot();
		if (root && !root.IsInherited(Man))
		{
			return root;
		}
		return m_DRL_CurrentWorkbench;
	}
	
	void DRL_CollectItemsByClass(string classname, float minHealthFrac, out array<ItemBase> outItems)
	{
		outItems = new array<ItemBase>();
		if (classname == "") return;

		int minLevel = minHealthFrac;
		minLevel = Math.Clamp(minLevel, 0, 3);
	
		array<EntityAI> items = new array<EntityAI>();
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		
		EntityAI wb = DRL_GetCurrentWorkbenchRoot();
		if (wb && wb.GetInventory())
		{
			wb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}
	
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

			int lvl = ib.GetHealthLevel();
			if (lvl > minLevel) continue;
	
			outItems.Insert(ib);
		}
	}

	override bool CanDropEntity(notnull EntityAI item)
	{
		ItemBase ib = ItemBase.Cast(item);
		if (ib && ib.DRL_IsCraftLocked())
		{
			return false;
		}
		return super.CanDropEntity(item);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		switch (rpc_type)
		{
			case DRL_CraftRPC.STC_SYNC_LEARNED_RECIPES:
			{
				Param1<array<int>> pSync;
				if (ctx.Read(pSync))
				{
					DRL_LearningBridge.SetClientLearnedRecipes(pSync.param1);
					Print("[DRL_CraftMenu][CLIENT] Learned recipes synced via Native RPC: " + pSync.param1.Count().ToString());
				}
				break;
			}

			case DRL_CraftRPC.STC_STUDY_RECIPE_RESPONSE:
			{
				Param3<bool, int, array<int>> pResp;
				if (ctx.Read(pResp))
				{
					DRL_LearningBridge.SetClientLearnedRecipes(pResp.param3);
					if (DRL_LearningBridge.s_OnStudyResponse)
					{
						DRL_LearningBridge.s_OnStudyResponse.Invoke(pResp.param1, pResp.param2, pResp.param3);
					}
				}
				break;
			}

			case DRL_CraftRPC.STC_SYNC_ACTIVE_PROCESSES:
			{
				Param1<array<ref DRL_CraftProcessSyncInfo>> pSyncList;
				if (ctx.Read(pSyncList))
				{
					DRL_CraftProcessBridge.SetActiveProcesses(pSyncList.param1);
					if (DRL_CraftProcessBridge.s_OnProcessesSync)
					{
						DRL_CraftProcessBridge.s_OnProcessesSync.Invoke();
					}
				}
				break;
			}

			case DRL_CraftRPC.STC_CRAFT_PROCESS_STARTED:
			{
				Param1<DRL_CraftProcessSyncInfo> pProcStarted;
				if (ctx.Read(pProcStarted))
				{
					if (DRL_CraftProcessBridge.s_OnProcessStarted)
					{
						DRL_CraftProcessBridge.s_OnProcessStarted.Invoke(pProcStarted.param1);
					}
				}
				break;
			}

			case DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED:
			{
				Param2<string, bool> pProcFin;
				if (ctx.Read(pProcFin))
				{
					if (DRL_CraftProcessBridge.s_OnProcessFinished)
					{
						DRL_CraftProcessBridge.s_OnProcessFinished.Invoke(pProcFin.param1, pProcFin.param2);
					}
				}
				break;
			}

			case DRL_CraftRPC.STC_CRAFT_NOTIFICATION:
			{
				Param5<int, string, string, string, string> pNotif5;
				if (ctx.Read(pNotif5))
				{
					DRL_CraftNotificationManager.ShowByType(pNotif5.param1, pNotif5.param2, pNotif5.param3, pNotif5.param4, pNotif5.param5);
					break;
				}

				Param4<string, string, string, string> pNotif4;
				if (ctx.Read(pNotif4))
				{
					DRL_CraftNotificationManager.Show(pNotif4.param1, pNotif4.param2, pNotif4.param3, pNotif4.param4);
					break;
				}
				break;
			}
		}
	}

	bool DRL_CheckStudyRequirements(DRL_Learning_Recipe lr, DRL_Craft_Recipe r)
	{
		return true;
	}

	void DRL_OnStudySuccessPostActions(DRL_Learning_Recipe lr, DRL_Craft_Recipe r)
	{
	}
}