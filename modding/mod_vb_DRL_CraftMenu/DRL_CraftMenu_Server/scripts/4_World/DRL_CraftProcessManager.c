class DRL_CraftProcessManager
{
    private static ref DRL_CraftProcessManager s_Instance;

    private const string DIR_BASE = "$profile:\\DRL_CraftMenu\\Base\\";
    protected ref map<string, ref DRL_CraftProcessData> m_ActiveProcesses;

    void DRL_CraftProcessManager()
    {
        m_ActiveProcesses = new map<string, ref DRL_CraftProcessData>();
        InitOnce();
    }

    static DRL_CraftProcessManager GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = new DRL_CraftProcessManager();
        }
        return s_Instance;
    }

    protected void InitOnce()
    {
        if (!FileExist(DIR_BASE))
        {
            MakeDirectory(DIR_BASE);
        }

        ScanAndRestoreProcesses();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.RestoreAllItemLocks, 3000, false);
    }

    void RestoreAllItemLocks()
    {
        if (!m_ActiveProcesses) return;

        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && data.m_LockedEntityNetLows)
            {
                for (int k = 0; k < data.m_LockedEntityNetLows.Count(); k++)
                {
                    int netLow = data.m_LockedEntityNetLows[k];
                    int netHigh = data.m_LockedEntityNetHighs[k];

                    Object obj = GetGame().GetObjectByNetworkId(netLow, netHigh);
                    ItemBase ib = ItemBase.Cast(obj);
                    if (ib)
                    {
                        ib.DRL_SetCraftLocked(true);
                    }
                }
            }

            if (data && data.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING && data.m_IsWorkbenchCraft)
            {
                EntityAI wbObj = FindWorkbenchForProcess(data);
                DRL_Craft_WB cWb = DRL_Craft_WB.Cast(wbObj);
                if (cWb)
                {
                    DRL_Craft_Recipe r = null;
                    if (GetCraft_Config())
                    {
                        r = GetCraft_Config().FindById(data.m_RecipeId);
                    }
                    string sndSet = DRL_CraftSoundHelper.GetSoundSetForRecipe(r);
                    cWb.DRL_StartCraftSound(DRL_CraftSoundHelper.GetSoundId(sndSet));
                }
            }
        }
    }

    protected void ScanAndRestoreProcesses()
    {
        string fileName;
        FileAttr fileAttr;
        FindFileHandle fh = FindFile(DIR_BASE + "*.bin", fileName, fileAttr, 0);
        if (!fh)
        {
            return;
        }

        TStringArray filesToLoad = new TStringArray();
        if (!(fileAttr & FileAttr.DIRECTORY))
        {
            filesToLoad.Insert(fileName);
        }

        while (FindNextFile(fh, fileName, fileAttr))
        {
            if (!(fileAttr & FileAttr.DIRECTORY))
            {
                filesToLoad.Insert(fileName);
            }
        }
        CloseFindFile(fh);

        for (int i = 0; i < filesToLoad.Count(); i++)
        {
            string fName = filesToLoad[i];
            LoadProcessFile(fName);
        }
    }

    protected void LoadProcessFile(string fileName)
    {
        string fullPath = DIR_BASE + fileName;
        if (!FileExist(fullPath))
        {
            return;
        }

        FileSerializer file = new FileSerializer();
        if (!file.Open(fullPath, FileMode.READ))
        {
            DRL_CraftLogger.Error("[DRL_CraftProcessManager] Failed to open bin file: " + fullPath);
            return;
        }

        ref DRL_CraftProcessData data = new DRL_CraftProcessData();
        bool readOk = data.OnRead(file);
        file.Close();

        if (!readOk)
        {
            DRL_CraftLogger.Error("[DRL_CraftProcessManager] Corrupted bin file: " + fullPath + ", deleting.");
            DeleteFile(fullPath);
            return;
        }

        m_ActiveProcesses.Set(data.m_ProcessId, data);

        if (data.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
        {
            float remaining = data.GetRemainingSec();
            if (remaining <= 0.0)
            {
                DRL_CraftLogger.Info("[DRL_CraftProcessManager] Process " + data.m_ProcessId + " expired during server offline. Cleaning up: " + fullPath);
                DeleteFile(fullPath);
                m_ActiveProcesses.Remove(data.m_ProcessId);
                PromoteNextInQueue(data);
                return;
            }

            int delayMs = Math.Max(100, Math.Round(remaining * 1000));
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.OnProcessTimerComplete, delayMs, false, data.m_ProcessId);
            DRL_CraftLogger.Info("[DRL_CraftProcessManager] Resumed active craft process: " + data.m_ProcessId + " (" + remaining.ToString() + "s remaining)");
        }
        else
        {
            DRL_CraftLogger.Info("[DRL_CraftProcessManager] Restored queued craft process: " + data.m_ProcessId + " (Position: " + data.m_QueuePosition.ToString() + ")");
        }
    }

    int GetProcessesCountForPlayer(string steamId)
    {
        if (steamId == "") return 0;
        int count = 0;
        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && !data.m_IsWorkbenchCraft && data.m_PlayerSteamId == steamId)
            {
                count++;
            }
        }
        return count;
    }

    int GetProcessesCountForWorkbench(EntityAI wb)
    {
        if (!wb) return 0;
        int netLow, netHigh;
        wb.GetNetworkID(netLow, netHigh);
        vector wbPos = wb.GetPosition();

        int count = 0;
        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && data.m_IsWorkbenchCraft)
            {
                if ((data.m_WorkbenchNetLow == netLow && data.m_WorkbenchNetHigh == netHigh) || vector.DistanceSq(data.m_WorkbenchPos, wbPos) < 1.0)
                {
                    count++;
                }
            }
        }
        return count;
    }

    void GetProcessesForWorkbench(EntityAI wb, out array<ref DRL_CraftProcessData> outList)
    {
        outList = new array<ref DRL_CraftProcessData>();
        if (!wb) return;

        int netLow, netHigh;
        wb.GetNetworkID(netLow, netHigh);
        vector wbPos = wb.GetPosition();

        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && data.m_IsWorkbenchCraft)
            {
                if ((data.m_WorkbenchNetLow == netLow && data.m_WorkbenchNetHigh == netHigh) || vector.DistanceSq(data.m_WorkbenchPos, wbPos) < 1.0)
                {
                    outList.Insert(data);
                }
            }
        }
        SortProcessesByQueue(outList);
    }

    void GetProcessesForPlayer(string steamId, out array<ref DRL_CraftProcessData> outList)
    {
        outList = new array<ref DRL_CraftProcessData>();
        if (steamId == "") return;

        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && !data.m_IsWorkbenchCraft && data.m_PlayerSteamId == steamId)
            {
                outList.Insert(data);
            }
        }
        SortProcessesByQueue(outList);
    }

    protected void SortProcessesByQueue(array<ref DRL_CraftProcessData> list)
    {
        if (!list || list.Count() <= 1) return;
        for (int i = 0; i < list.Count() - 1; i++)
        {
            for (int j = 0; j < list.Count() - i - 1; j++)
            {
                if (list[j].m_QueuePosition > list[j + 1].m_QueuePosition)
                {
                    DRL_CraftProcessData tmp = list[j];
                    list[j] = list[j + 1];
                    list[j + 1] = tmp;
                }
            }
        }
    }

    bool HasActiveProcessForPlayer(string steamId)
    {
        return (GetActiveProcessForPlayer(steamId) != null);
    }

    bool HasActiveProcessForWorkbench(EntityAI wb)
    {
        return (GetActiveProcessForWorkbench(wb) != null);
    }

    DRL_CraftProcessData GetActiveProcessForWorkbench(EntityAI wb)
    {
        if (!wb)
        {
            return null;
        }

        int netLow, netHigh;
        wb.GetNetworkID(netLow, netHigh);
        vector wbPos = wb.GetPosition();

        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && data.m_IsWorkbenchCraft && data.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
            {
                if ((data.m_WorkbenchNetLow == netLow && data.m_WorkbenchNetHigh == netHigh) || vector.DistanceSq(data.m_WorkbenchPos, wbPos) < 1.0)
                {
                    return data;
                }
            }
        }
        return null;
    }

    DRL_CraftProcessData GetActiveProcessForPlayer(string steamId)
    {
        if (steamId == "")
        {
            return null;
        }

        for (int i = 0; i < m_ActiveProcesses.Count(); i++)
        {
            DRL_CraftProcessData data = m_ActiveProcesses.GetElement(i);
            if (data && !data.m_IsWorkbenchCraft && data.m_PlayerSteamId == steamId && data.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
            {
                return data;
            }
        }
        return null;
    }

    DRL_CraftProcessData FindProcessById(string processId)
    {
        DRL_CraftProcessData data;
        if (m_ActiveProcesses.Find(processId, data))
        {
            return data;
        }
        return null;
    }

    void SendCraftNotification(PlayerBase player, int notifType, string itemClass = "", string extraParam = "", string customTitle = "", string customText = "")
    {
        if (!player || !player.GetIdentity())
        {
            return;
        }

        player.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_NOTIFICATION, new Param5<int, string, string, string, string>(notifType, itemClass, extraParam, customTitle, customText), true, player.GetIdentity());
    }

    protected void LockProcessItems(DRL_CraftProcessData data, array<ref DRL_Craft_Component> comps, PlayerBase player, EntityAI wb)
    {
        if (!data || !comps)
        {
            return;
        }

        data.m_LockedEntityNetLows.Clear();
        data.m_LockedEntityNetHighs.Clear();

        array<EntityAI> availableItems = new array<EntityAI>();
        EntityAI rootWb = wb;
        if (rootWb)
        {
            EntityAI hRoot = rootWb.GetHierarchyRoot();
            if (hRoot && !hRoot.IsInherited(Man))
            {
                rootWb = hRoot;
            }
        }
        if (rootWb && rootWb.GetInventory())
        {
            rootWb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, availableItems);
        }
        if (player && player.GetInventory())
        {
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, availableItems);
        }

        int crafts = Math.Max(1, data.m_CraftCount);

        for (int cIdx = 0; cIdx < comps.Count(); cIdx++)
        {
            DRL_Craft_Component c = comps[cIdx];
            if (!c)
                continue;

            if (c.IsTool())
            {
                ItemBase toolItem = DRL_CraftComponentResolver.FindItemForComponent(c, player, wb, data.m_ResultClass);
                if (toolItem)
                {
                    EntityAI hRootTool = toolItem.GetHierarchyRoot();
                    if (hRootTool && hRootTool.IsInherited(Man))
                    {
                        data.m_HasPlayerIngredients = true;
                    }

                    if (c.Delete || c.ApplyDamageFrac > 0.0)
                    {
                        int netLowT, netHighT;
                        toolItem.GetNetworkID(netLowT, netHighT);
                        toolItem.DRL_SetCraftLocked(true);
                        data.m_LockedEntityNetLows.Insert(netLowT);
                        data.m_LockedEntityNetHighs.Insert(netHighT);
                    }
                }
                continue;
            }

            int amountReq = Math.Max(1, c.Amount);

            int qTypeLock = DRL_ItemQuantityHelper.GetQuantityTypeByClass(c.Classname);
            bool isFillingLock = (c.QuantityFrac > 0.0 && DRL_ItemQuantityHelper.IsFillingType(qTypeLock));

            if (isFillingLock)
            {
                float totalUnitsNeeded = 0.0;
                bool unitsResolved = false;
                float haveUnitsTotal = 0.0;

                for (int itQ = 0; itQ < availableItems.Count(); itQ++)
                {
                    ItemBase ibQ = ItemBase.Cast(availableItems[itQ]);
                    if (!ibQ || ibQ.DRL_IsCraftLocked())
                        continue;

                    if (player)
                    {
                        InventoryLocation ilQ = new InventoryLocation();
                        if (ibQ.GetInventory() && ibQ.GetInventory().GetCurrentInventoryLocation(ilQ))
                        {
                            if (ilQ.GetType() == InventoryLocationType.ATTACHMENT && ilQ.GetParent() == player)
                                continue;
                        }
                    }

                    if (!DoesItemMatchComponent(ibQ, c))
                        continue;

                    if (!unitsResolved)
                    {
                        float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(ibQ);
                        if (qMax <= 0.0) qMax = 1.0;
                        float perUnitFrac = Math.Round(qMax * c.QuantityFrac);
                        if (perUnitFrac < 1.0) perUnitFrac = 1.0;
                        totalUnitsNeeded = perUnitFrac * amountReq * crafts;
                        unitsResolved = true;
                    }

                    int netLowQ, netHighQ;
                    ibQ.GetNetworkID(netLowQ, netHighQ);

                    ibQ.DRL_SetCraftLocked(true);
                    data.m_LockedEntityNetLows.Insert(netLowQ);
                    data.m_LockedEntityNetHighs.Insert(netHighQ);

                    EntityAI hRootQ = ibQ.GetHierarchyRoot();
                    if (hRootQ && hRootQ.IsInherited(Man))
                    {
                        data.m_HasPlayerIngredients = true;
                    }

                    haveUnitsTotal += DRL_ItemQuantityHelper.GetQuantityRaw(ibQ);
                    if (haveUnitsTotal >= totalUnitsNeeded)
                        break;
                }
            }
            else
            {
                int needTotal = amountReq * crafts;
                int haveTotal = 0;

                for (int itS = 0; itS < availableItems.Count(); itS++)
                {
                    ItemBase ibS = ItemBase.Cast(availableItems[itS]);
                    if (!ibS || ibS.DRL_IsCraftLocked())
                        continue;

                    if (player)
                    {
                        InventoryLocation ilS = new InventoryLocation();
                        if (ibS.GetInventory() && ibS.GetInventory().GetCurrentInventoryLocation(ilS))
                        {
                            if (ilS.GetType() == InventoryLocationType.ATTACHMENT && ilS.GetParent() == player)
                                continue;
                        }
                    }

                    if (!DoesItemMatchComponent(ibS, c))
                        continue;

                    int netLowS, netHighS;
                    ibS.GetNetworkID(netLowS, netHighS);

                    ibS.DRL_SetCraftLocked(true);
                    data.m_LockedEntityNetLows.Insert(netLowS);
                    data.m_LockedEntityNetHighs.Insert(netHighS);

                    EntityAI hRootS = ibS.GetHierarchyRoot();
                    if (hRootS && hRootS.IsInherited(Man))
                    {
                        data.m_HasPlayerIngredients = true;
                    }

                    haveTotal += DRL_ItemQuantityHelper.GetStackAmount(ibS);
                    if (haveTotal >= needTotal)
                        break;
                }
            }
        }
    }

    protected bool DoesItemMatchComponent(ItemBase ib, DRL_Craft_Component c)
    {
        return DRL_CraftComponentResolver.DoesItemMatchComponent(ib, c);
    }



    protected bool DRL_IsWorkbenchMatchingRecipe(EntityAI wb, DRL_Craft_Recipe r)
    {
        if (!r)
            return false;

        if (r.ShowInFreeMenu == 1)
            return true;

        if (!r.WorkbenchClasses || r.WorkbenchClasses.Count() == 0)
            return true;

        if (!wb)
            return false;

        string wbType = wb.GetType();
        EntityAI rootWb = wb.GetHierarchyRoot();
        string rootType = "";
        if (rootWb && !rootWb.IsInherited(Man))
        {
            rootType = rootWb.GetType();
        }

        for (int j = 0; j < r.WorkbenchClasses.Count(); j++)
        {
            string reqWb = r.WorkbenchClasses[j];
            if (wbType == reqWb || wb.IsKindOf(reqWb))
                return true;
            if (rootType != "" && (rootType == reqWb || rootWb.IsKindOf(reqWb)))
                return true;
        }

        return false;
    }




    protected void DRL_WorkbenchSpawnResultExactNTimes(EntityAI wb, DRL_Craft_Recipe r, string outCls, int crafts, DRL_CraftProcessData data)
    {
        DRL_Craft_WB cWb = DRL_Craft_WB.Cast(wb);
        if (!cWb && wb)
        {
            cWb = DRL_Craft_WB.Cast(wb.GetHierarchyRoot());
        }
        if (!cWb) return;

        int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(outCls);
        int perCraftSpawns = r.ResultCount;
        if (qType == EDRLItemQuantityType.STACK || qType == EDRLItemQuantityType.MAGAZINE || DRL_ItemQuantityHelper.IsFillingType(qType))
        {
            perCraftSpawns = 1;
        }

        vector wbSpawnPos = "0 0 0";
        if (data) wbSpawnPos = data.m_CustomSpawnPos;

        int craftsClamped = Math.Max(1, crafts);
        for (int k = 0; k < craftsClamped; k++)
        {
            for (int i = 0; i < perCraftSpawns; i++)
            {
                EntityAI created = cWb.DRL_CreateCraftResult(outCls, null, false, wbSpawnPos);
                if (!created) continue;

                ItemBase ib = ItemBase.Cast(created);
                if (ib)
                {
                    DRL_ItemQuantityHelper.SetFilling(ib, r.ResultQuantityFrac, r.ResultCount);

                    if (r.ResultDamageFrac > 0.0)
                    {
                        float health01 = Math.Clamp(1.0 - r.ResultDamageFrac, 0.0, 1.0);
                        ib.SetHealth01("", "Health", health01);
                    }
                }

                if (data && data.m_AttachedRecipeIds && data.m_AttachedRecipeIds.Count() > 0 && GetCraft_Config())
                {
                    for (int a = 0; a < data.m_AttachedRecipeIds.Count(); a++)
                    {
                        int dopId = data.m_AttachedRecipeIds[a];
                        DRL_Craft_Recipe dopR = GetCraft_Config().FindById(dopId);
                        if (!dopR) continue;

                        string dopCls = dopR.Result;
                        if (data.m_AttachedVariants && data.m_AttachedVariants.Contains(dopId))
                        {
                            dopCls = data.m_AttachedVariants.Get(dopId);
                        }
                        if (dopCls == "") continue;

                        EntityAI attItem = null;
                        if (created.GetInventory())
                        {
                            attItem = created.GetInventory().CreateAttachment(dopCls);
                        }
                        if (!attItem)
                        {
                            attItem = cWb.DRL_CreateCraftResult(dopCls, null, false, wbSpawnPos);
                        }

                        if (attItem)
                        {
                            ItemBase attIb = ItemBase.Cast(attItem);
                            if (attIb)
                            {
                                DRL_ItemQuantityHelper.SetFilling(attIb, dopR.ResultQuantityFrac, dopR.ResultCount);
                                if (dopR.ResultDamageFrac > 0.0)
                                {
                                    float attHealth01 = Math.Clamp(1.0 - dopR.ResultDamageFrac, 0.0, 1.0);
                                    attIb.SetHealth01("", "Health", attHealth01);
                                }
                            }
                        }
                    }
                }
            }
        }

        cWb.DRL_OnCraftFinish(outCls, null, null);
    }

    protected void UnlockProcessItems(DRL_CraftProcessData data)
    {
        if (!data || !data.m_LockedEntityNetLows)
        {
            return;
        }

        for (int i = 0; i < data.m_LockedEntityNetLows.Count(); i++)
        {
            int netLow = data.m_LockedEntityNetLows[i];
            int netHigh = data.m_LockedEntityNetHighs[i];

            Object obj = GetGame().GetObjectByNetworkId(netLow, netHigh);
            ItemBase ib = ItemBase.Cast(obj);
            if (ib)
            {
                ib.DRL_SetCraftLocked(false);
            }
        }

        data.m_LockedEntityNetLows.Clear();
        data.m_LockedEntityNetHighs.Clear();
    }

    protected void ShiftQueueAfterRemoval(DRL_CraftProcessData removedData)
    {
        if (!removedData) return;

        array<ref DRL_CraftProcessData> list = new array<ref DRL_CraftProcessData>();
        if (removedData.m_IsWorkbenchCraft)
        {
            EntityAI wb = FindWorkbenchForProcess(removedData);
            if (wb)
            {
                GetProcessesForWorkbench(wb, list);
            }
        }
        else
        {
            GetProcessesForPlayer(removedData.m_PlayerSteamId, list);
        }

        for (int i = 0; i < list.Count(); i++)
        {
            DRL_CraftProcessData p = list[i];
            if (p && p.m_ProcessStatus == DRL_CraftProcessStatus.QUEUED && p.m_QueuePosition > removedData.m_QueuePosition)
            {
                p.m_QueuePosition = Math.Max(1, p.m_QueuePosition - 1);
                SaveProcessBin(p);
            }
        }
    }

    protected void PromoteNextInQueue(DRL_CraftProcessData finishedData)
    {
        if (!finishedData)
            return;

        array<ref DRL_CraftProcessData> queueList = new array<ref DRL_CraftProcessData>();
        if (finishedData.m_IsWorkbenchCraft)
        {
            EntityAI wb = FindWorkbenchForProcess(finishedData);
            if (wb)
            {
                GetProcessesForWorkbench(wb, queueList);
            }
        }
        else
        {
            GetProcessesForPlayer(finishedData.m_PlayerSteamId, queueList);
        }

        DRL_CraftProcessData nextProc = null;
        int lowestPos = 999999;
        for (int i = 0; i < queueList.Count(); i++)
        {
            DRL_CraftProcessData cand = queueList[i];
            if (cand && cand.m_ProcessStatus == DRL_CraftProcessStatus.QUEUED && cand.m_QueuePosition < lowestPos)
            {
                lowestPos = cand.m_QueuePosition;
                nextProc = cand;
            }
        }

        if (!nextProc)
        {
            if (finishedData && finishedData.m_IsWorkbenchCraft)
            {
                EntityAI finWb = FindWorkbenchForProcess(finishedData);
                DRL_Craft_WB finCraftWb = DRL_Craft_WB.Cast(finWb);
                if (!finCraftWb && finWb)
                {
                    finCraftWb = DRL_Craft_WB.Cast(finWb.GetHierarchyRoot());
                }
                if (finCraftWb)
                {
                    finCraftWb.DRL_StopCraftSound();
                }
            }
            return;
        }

        nextProc.m_ProcessStatus = DRL_CraftProcessStatus.RUNNING;
        nextProc.m_QueuePosition = 0;
        nextProc.m_StartTimeEpoch = GetGame().GetTime() / 1000;
        SaveProcessBin(nextProc);

        int delayMs = Math.Max(100, Math.Round(nextProc.m_TotalDurationSec * 1000));
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.OnProcessTimerComplete, delayMs, false, nextProc.m_ProcessId);

        for (int k = 0; k < queueList.Count(); k++)
        {
            DRL_CraftProcessData rem = queueList[k];
            if (rem && rem.m_ProcessId != nextProc.m_ProcessId && rem.m_ProcessStatus == DRL_CraftProcessStatus.QUEUED)
            {
                rem.m_QueuePosition = Math.Max(1, rem.m_QueuePosition - 1);
                SaveProcessBin(rem);
            }
        }

        PlayerBase pTarget = FindPlayerBySteamId(nextProc.m_PlayerSteamId);
        EntityAI nextWb = FindWorkbenchForProcess(nextProc);
        ref DRL_CraftProcessSyncInfo nextSync = new DRL_CraftProcessSyncInfo(nextProc);

        if (nextWb)
        {
            DRL_Craft_WB cWb = DRL_Craft_WB.Cast(nextWb);
            if (!cWb && nextWb)
            {
                cWb = DRL_Craft_WB.Cast(nextWb.GetHierarchyRoot());
            }
            if (cWb)
            {
                cWb.DRL_OnCraftStartEx(nextProc.m_RecipeId, nextProc.m_ResultClass, pTarget);
                DRL_Craft_Recipe nextRecipe = null;
                if (GetCraft_Config())
                {
                    nextRecipe = GetCraft_Config().FindById(nextProc.m_RecipeId);
                }
                string nextSoundSet = DRL_CraftSoundHelper.GetSoundSetForRecipe(nextRecipe);
                cWb.DRL_StartCraftSound(DRL_CraftSoundHelper.GetSoundId(nextSoundSet));
            }
            BroadcastWorkbenchProcessStarted(nextWb, nextSync);
            BroadcastWorkbenchActiveProcesses(nextWb);
        }
        else if (pTarget && pTarget.GetIdentity())
        {
            pTarget.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_STARTED, new Param1<DRL_CraftProcessSyncInfo>(nextSync), true, pTarget.GetIdentity());
            BroadcastPlayerActiveProcesses(pTarget);
        }

        if (pTarget)
        {
            SendCraftNotification(pTarget, EDRLCraftNotifType.PROMOTED_FROM_QUEUE, nextProc.m_ResultClass);
        }
    }

    void BroadcastWorkbenchActiveProcesses(EntityAI wb)
    {
        if (!wb) return;
        array<ref DRL_CraftProcessSyncInfo> activeList;
        GetActiveProcessesForPlayerOrWorkbench(null, wb, activeList);

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        vector pos = wb.GetPosition();

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (pb && pb.GetIdentity() && vector.DistanceSq(pb.GetPosition(), pos) < 100.0)
            {
                pb.RPCSingleParam(DRL_CraftRPC.STC_SYNC_ACTIVE_PROCESSES, new Param1<array<ref DRL_CraftProcessSyncInfo>>(activeList), true, pb.GetIdentity());
            }
        }
    }

    void BroadcastPlayerActiveProcesses(PlayerBase player)
    {
        if (!player || !player.GetIdentity()) return;
        array<ref DRL_CraftProcessSyncInfo> activeList;
        GetActiveProcessesForPlayerOrWorkbench(player, null, activeList);
        player.RPCSingleParam(DRL_CraftRPC.STC_SYNC_ACTIVE_PROCESSES, new Param1<array<ref DRL_CraftProcessSyncInfo>>(activeList), true, player.GetIdentity());
    }

    bool StartProcess(PlayerBase player, DRL_Craft_Recipe recipe, string resultCls, int craftCount, EntityAI workbench, array<int> attachedRecipeIds, map<int, string> selectedVariations, float totalDurationSec, vector customSpawnPos = "0 0 0", map<int, string> attachedVariants = null)
    {
        if (!player || !recipe)
        {
            return false;
        }

        string steamId = player.DRL_GetPlayerIdSafe();
        int maxSlots = 5;
        if (GetCraft_Config() && GetCraft_Config().Settings)
        {
            maxSlots = Math.Max(1, GetCraft_Config().Settings.MaxQueueSlots);
        }

        int currentCount = 0;
        if (workbench)
        {
            currentCount = GetProcessesCountForWorkbench(workbench);
        }
        else
        {
            currentCount = GetProcessesCountForPlayer(steamId);
        }

        if (currentCount >= maxSlots)
        {
            SendCraftNotification(player, EDRLCraftNotifType.QUEUE_FULL, "", string.Format("%1/%2", currentCount, maxSlots));
            return false;
        }

        bool isRunningNow = (currentCount == 0);

        ref DRL_CraftProcessData data = new DRL_CraftProcessData();
        data.m_ProcessId = steamId + "_" + recipe.RecipeId.ToString() + "_" + (GetGame().GetTime() / 1000).ToString();
        data.m_RecipeId = recipe.RecipeId;
        data.m_ResultClass = resultCls;
        data.m_CraftCount = craftCount;
        data.m_PlayerSteamId = steamId;
        if (player.GetIdentity())
        {
            data.m_PlayerName = player.GetIdentity().GetName();
        }
        else
        {
            data.m_PlayerName = "Unknown";
        }

        if (workbench)
        {
            data.m_IsWorkbenchCraft = true;
            data.m_HasPlayerIngredients = false;
            workbench.GetNetworkID(data.m_WorkbenchNetLow, data.m_WorkbenchNetHigh);
            data.m_WorkbenchPos = workbench.GetPosition();
            data.m_WorkbenchClass = workbench.GetType();
        }
        else
        {
            data.m_IsWorkbenchCraft = false;
            data.m_HasPlayerIngredients = true;
            data.m_WorkbenchNetLow = 0;
            data.m_WorkbenchNetHigh = 0;
            data.m_WorkbenchPos = player.GetPosition();
            data.m_WorkbenchClass = "";
        }

        data.m_CustomSpawnPos = customSpawnPos;
        data.m_TotalDurationSec = totalDurationSec;

        if (isRunningNow)
        {
            data.m_ProcessStatus = DRL_CraftProcessStatus.RUNNING;
            data.m_QueuePosition = 0;
            data.m_StartTimeEpoch = GetGame().GetTime() / 1000;
        }
        else
        {
            data.m_ProcessStatus = DRL_CraftProcessStatus.QUEUED;
            data.m_QueuePosition = currentCount;
            data.m_StartTimeEpoch = 0;
        }

        if (attachedRecipeIds)
        {
            for (int a = 0; a < attachedRecipeIds.Count(); a++)
            {
                data.m_AttachedRecipeIds.Insert(attachedRecipeIds[a]);
            }
        }

        if (selectedVariations)
        {
            for (int v = 0; v < selectedVariations.Count(); v++)
            {
                data.m_SelectedVariations.Set(selectedVariations.GetKey(v), selectedVariations.GetElement(v));
            }
        }

        if (attachedVariants)
        {
            for (int av = 0; av < attachedVariants.Count(); av++)
            {
                data.m_AttachedVariants.Set(attachedVariants.GetKey(av), attachedVariants.GetElement(av));
            }
        }

        array<ref DRL_Craft_Component> compsToLock = GetFinalComponentsForData(recipe, data);
        LockProcessItems(data, compsToLock, player, workbench);

        SaveProcessBin(data);
        m_ActiveProcesses.Set(data.m_ProcessId, data);

        if (isRunningNow)
        {
            DRL_Craft_WB wb = DRL_Craft_WB.Cast(workbench);
            if (!wb && workbench)
            {
                wb = DRL_Craft_WB.Cast(workbench.GetHierarchyRoot());
            }
            if (wb)
            {
                wb.DRL_OnCraftStartEx(recipe.RecipeId, resultCls, player);
                string sndSet = DRL_CraftSoundHelper.GetSoundSetForRecipe(recipe);
                wb.DRL_StartCraftSound(DRL_CraftSoundHelper.GetSoundId(sndSet));
            }

            int delayMs = Math.Max(100, Math.Round(totalDurationSec * 1000));
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.OnProcessTimerComplete, delayMs, false, data.m_ProcessId);
        }

        DRL_CraftLogger.Info(string.Format("[DRL_CraftProcessManager] Craft started: Process=%1, Recipe=%2, Result=%3, Player=%4 (%5), Time=%6s, Status=%7, QueuePos=%8", data.m_ProcessId, recipe.RecipeId, resultCls, data.m_PlayerName, steamId, totalDurationSec, data.m_ProcessStatus, data.m_QueuePosition));

        ref DRL_CraftProcessSyncInfo syncInfo = new DRL_CraftProcessSyncInfo(data);
        if (workbench)
        {
            BroadcastWorkbenchProcessStarted(workbench, syncInfo);
            BroadcastWorkbenchActiveProcesses(workbench);
        }
        else if (player.GetIdentity())
        {
            player.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_STARTED, new Param1<DRL_CraftProcessSyncInfo>(syncInfo), true, player.GetIdentity());
            BroadcastPlayerActiveProcesses(player);
        }

        if (isRunningNow)
        {
            SendCraftNotification(player, EDRLCraftNotifType.STARTED, resultCls);
        }
        else
        {
            SendCraftNotification(player, EDRLCraftNotifType.QUEUED, resultCls, data.m_QueuePosition.ToString());
        }

        return true;
    }

    bool CancelProcess(string processId, PlayerBase initiator)
    {
        DRL_CraftProcessData data;
        if (!m_ActiveProcesses.Find(processId, data) || !data)
        {
            return false;
        }

        UnlockProcessItems(data);
        m_ActiveProcesses.Remove(processId);
        DeleteProcessBin(data);

        string initiatorName = "Unknown";
        if (initiator && initiator.GetIdentity())
        {
            initiatorName = initiator.GetIdentity().GetName();
            SendCraftNotification(initiator, EDRLCraftNotifType.CANCELLED_MANUAL, data.m_ResultClass);
        }

        DRL_CraftLogger.Info(string.Format("[DRL_CraftProcessManager] Craft cancelled: Process=%1, Recipe=%2, Initiator=%3, Status=%4", processId, data.m_RecipeId, initiatorName, data.m_ProcessStatus));

        EntityAI wb = FindWorkbenchForProcess(data);
        if (data.m_ProcessStatus == DRL_CraftProcessStatus.RUNNING)
        {
            DRL_Craft_WB craftWb = DRL_Craft_WB.Cast(wb);
            if (craftWb)
            {
                craftWb.DRL_OnCraftFinish(data.m_ResultClass, null, initiator);
            }

            if (data.m_IsWorkbenchCraft && wb)
            {
                BroadcastWorkbenchProcessFinished(wb, processId, true);
            }
            else
            {
                PlayerBase pTarget = initiator;
                if (!pTarget)
                {
                    pTarget = FindPlayerBySteamId(data.m_PlayerSteamId);
                }
                if (pTarget && pTarget.GetIdentity())
                {
                    pTarget.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED, new Param2<string, bool>(processId, true), true, pTarget.GetIdentity());
                }
            }

            PromoteNextInQueue(data);
        }
        else
        {
            ShiftQueueAfterRemoval(data);
            if (data.m_IsWorkbenchCraft && wb)
            {
                BroadcastWorkbenchProcessFinished(wb, processId, true);
                BroadcastWorkbenchActiveProcesses(wb);
            }
            else
            {
                PlayerBase pTargetQ = initiator;
                if (!pTargetQ)
                {
                    pTargetQ = FindPlayerBySteamId(data.m_PlayerSteamId);
                }
                if (pTargetQ && pTargetQ.GetIdentity())
                {
                    pTargetQ.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED, new Param2<string, bool>(processId, true), true, pTargetQ.GetIdentity());
                    BroadcastPlayerActiveProcesses(pTargetQ);
                }
            }
        }

        return true;
    }

    void OnProcessTimerComplete(string processId)
    {
        DRL_CraftProcessData data;
        if (!m_ActiveProcesses.Find(processId, data) || !data)
        {
            return;
        }

        UnlockProcessItems(data);
        m_ActiveProcesses.Remove(processId);
        DeleteProcessBin(data);

        PlayerBase player = FindPlayerBySteamId(data.m_PlayerSteamId);
        EntityAI wb = FindWorkbenchForProcess(data);

        DRL_Craft_Config cfg = GetCraft_Config();
        if (!cfg)
        {
            DRL_CraftLogger.Error("[DRL_CraftProcessManager] No craft config found on process finish!");
            return;
        }

        DRL_Craft_Recipe r = cfg.FindById(data.m_RecipeId);
        if (!r)
        {
            DRL_CraftLogger.Error("[DRL_CraftProcessManager] Recipe " + data.m_RecipeId.ToString() + " not found on finish!");
            return;
        }

        if (player)
        {
            player.DRL_SetCurrentWorkbench(wb);
        }

        int failReason = -1;
        bool craftSuccess = false;
        if (player || (data.m_IsWorkbenchCraft && wb && !data.m_HasPlayerIngredients))
        {
            failReason = CheckProcessIngredientsWithReason(player, r, data);
            if (failReason == -1)
            {
                craftSuccess = ExecuteProcessCompletion(player, wb, r, data);
                if (!craftSuccess)
                {
                    failReason = EDRLCraftNotifType.NO_INGREDIENTS;
                }
            }
        }
        else
        {
            failReason = EDRLCraftNotifType.NO_INGREDIENTS;
        }

        if (craftSuccess)
        {
            DRL_CraftLogger.Info(string.Format("[DRL_CraftProcessManager] Craft finished successfully: Process=%1, Recipe=%2, Result=%3", processId, r.RecipeId, data.m_ResultClass));

            if (data.m_IsWorkbenchCraft && wb)
            {
                BroadcastWorkbenchProcessFinished(wb, processId, false);
                BroadcastWorkbenchActiveProcesses(wb);
            }
            else if (player && player.GetIdentity())
            {
                player.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED, new Param2<string, bool>(processId, false), true, player.GetIdentity());
                BroadcastPlayerActiveProcesses(player);
            }
            if (player)
            {
                SendCraftNotification(player, EDRLCraftNotifType.SUCCESS, data.m_ResultClass);
            }
        }
        else
        {
            DRL_Craft_WB craftWb = DRL_Craft_WB.Cast(wb);
            if (!craftWb && wb)
            {
                craftWb = DRL_Craft_WB.Cast(wb.GetHierarchyRoot());
            }
            if (craftWb)
            {
                craftWb.DRL_OnCraftFinish(data.m_ResultClass, null, player);
            }

            DRL_CraftLogger.Warn(string.Format("[DRL_CraftProcessManager] Craft cancelled (missing ingredients or conditions not met, reason=%1): Process=%2, Recipe=%3", failReason, processId, r.RecipeId));

            if (data.m_IsWorkbenchCraft && wb)
            {
                BroadcastWorkbenchProcessFinished(wb, processId, true);
                BroadcastWorkbenchActiveProcesses(wb);
            }
            else if (player && player.GetIdentity())
            {
                player.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED, new Param2<string, bool>(processId, true), true, player.GetIdentity());
                BroadcastPlayerActiveProcesses(player);
            }
            if (player)
            {
                int notifFail = failReason;
                if (notifFail < 0)
                {
                    notifFail = EDRLCraftNotifType.NO_INGREDIENTS;
                }
                SendCraftNotification(player, notifFail, data.m_ResultClass);
            }
        }

        if (player)
        {
            player.DRL_SetCurrentWorkbench(null);
        }

        PromoteNextInQueue(data);
    }

    protected int CheckProcessIngredientsWithReason(PlayerBase player, DRL_Craft_Recipe r, DRL_CraftProcessData data)
    {
        if (!r)
        {
            return EDRLCraftNotifType.NO_INGREDIENTS;
        }

        EntityAI targetWb = null;
        if (data.m_IsWorkbenchCraft)
        {
            targetWb = FindWorkbenchForProcess(data);
            if (!targetWb || targetWb.IsDamageDestroyed())
            {
                return EDRLCraftNotifType.NO_INGREDIENTS;
            }

            if (data.m_HasPlayerIngredients)
            {
                if (!player || vector.Distance(player.GetPosition(), targetWb.GetPosition()) > 6.0)
                {
                    return EDRLCraftNotifType.CANCELLED_DISTANCE;
                }
            }
        }
        else
        {
            if (!player)
            {
                return EDRLCraftNotifType.NO_INGREDIENTS;
            }
        }

        if (data.m_IsWorkbenchCraft && targetWb)
        {
            if (!DRL_IsWorkbenchMatchingRecipe(targetWb, r))
            {
                return EDRLCraftNotifType.MISSING_TOOLS;
            }
        }
        else if (player)
        {
            if (!player.DRL_HasRequiredWorkbenchForRecipe(r))
            {
                return EDRLCraftNotifType.MISSING_TOOLS;
            }
        }

        array<ref DRL_Craft_Component> compsToCheck = GetFinalComponentsForData(r, data);
        int craftCount = Math.Max(1, data.m_CraftCount);

        string resultCls = data.m_ResultClass;
        if (resultCls == "")
        {
            resultCls = r.Result;
        }

        for (int tIdx = 0; tIdx < compsToCheck.Count(); tIdx++)
        {
            DRL_Craft_Component toolComp = compsToCheck[tIdx];
            if (!toolComp || !toolComp.IsTool())
            {
                continue;
            }

            int neededToolCrafts = 1;
            if (toolComp.QuantityFrac > 0.0 || toolComp.Delete)
            {
                neededToolCrafts = craftCount;
            }

            if (!DRL_CraftComponentResolver.HasComponentRequirement(toolComp, player, targetWb, resultCls, neededToolCrafts))
            {
                return EDRLCraftNotifType.MISSING_TOOLS;
            }
        }

        for (int cIdx = 0; cIdx < compsToCheck.Count(); cIdx++)
        {
            DRL_Craft_Component c = compsToCheck[cIdx];
            if (!c || c.IsTool())
            {
                continue;
            }

            if (!DRL_CraftComponentResolver.HasComponentRequirement(c, player, targetWb, resultCls, craftCount))
            {
                return EDRLCraftNotifType.NO_INGREDIENTS;
            }
        }

        return -1;
    }

    protected bool CheckProcessIngredients(PlayerBase player, DRL_Craft_Recipe r, DRL_CraftProcessData data)
    {
        return CheckProcessIngredientsWithReason(player, r, data) == -1;
    }

    protected bool DRL_ExecuteComponentConsumption(DRL_Craft_Component c, PlayerBase player, EntityAI wb, int craftCount, string activeResultCls)
    {
        return DRL_CraftComponentResolver.ExecuteComponentConsumption(c, player, wb, activeResultCls, craftCount);
    }

    protected bool ExecuteProcessCompletion(PlayerBase player, EntityAI wb, DRL_Craft_Recipe r, DRL_CraftProcessData data)
    {
        array<ref DRL_Craft_Component> compsToConsume = GetFinalComponentsForData(r, data);
        int craftCount = Math.Max(1, data.m_CraftCount);

        string outCls = data.m_ResultClass;
        if (outCls == "")
        {
            outCls = r.Result;
        }

        for (int cIdx = 0; cIdx < compsToConsume.Count(); cIdx++)
        {
            DRL_Craft_Component c = compsToConsume[cIdx];
            if (!c)
            {
                continue;
            }

            bool consumed = DRL_ExecuteComponentConsumption(c, player, wb, craftCount, outCls);
            if (!consumed)
            {
                DRL_CraftLogger.Warn(string.Format("[DRL_CraftProcessManager] Failed to consume component %1 for process %2. Aborting result spawn!", c.Classname, data.m_ProcessId));
                return false;
            }
        }

        if (player)
        {
            player.DRL_SetCurrentWorkbench(wb);
            player.DRL_SetCustomSpawnPos(data.m_CustomSpawnPos);
            player.DRL_SetAttachedVariantsOverride(data.m_AttachedVariants);
            if (data.m_AttachedRecipeIds && data.m_AttachedRecipeIds.Count() > 0)
            {
                player.DRL_CompleteCraftWithAttachments(r, outCls, craftCount, data.m_AttachedRecipeIds);
            }
            else
            {
                player.DRL_CompleteCraftExactNTimes(r, outCls, craftCount);
            }
            player.DRL_SetAttachedVariantsOverride(null);
            player.DRL_SetCustomSpawnPos("0 0 0");
        }
        else if (wb)
        {
            DRL_WorkbenchSpawnResultExactNTimes(wb, r, outCls, craftCount, data);
        }
        return true;
    }

    protected array<ref DRL_Craft_Component> GetFinalComponentsForData(DRL_Craft_Recipe r, DRL_CraftProcessData data)
    {
        array<ref DRL_CraftMergedComponent> mergedList = new array<ref DRL_CraftMergedComponent>();
        string targetCls = "";
        if (data && data.m_ResultClass != "")
        {
            targetCls = data.m_ResultClass;
        }
        else if (r)
        {
            targetCls = r.Result;
        }

        if (r && r.CraftComponents)
        {
            for (int i = 0; i < r.CraftComponents.Count(); i++)
            {
                DRL_Craft_Component comp = r.CraftComponents[i];
                if (!comp)
                {
                    continue;
                }
                if (!comp.IsApplicableForResult(targetCls))
                {
                    continue;
                }

                DRL_Craft_Component compEffective = comp;
                int keyMain = r.RecipeId * 100 + i;
                if (data && data.m_SelectedVariations)
                {
                    string chosenVar = "";
                    if (data.m_SelectedVariations.Contains(keyMain))
                    {
                        chosenVar = data.m_SelectedVariations.Get(keyMain);
                    }
                    else if (data.m_SelectedVariations.Contains(i))
                    {
                        chosenVar = data.m_SelectedVariations.Get(i);
                    }

                    if (chosenVar != "")
                    {
                        compEffective = comp.CloneWithVariation(chosenVar);
                    }
                }

                mergedList.Insert(new DRL_CraftMergedComponent(compEffective, keyMain));
            }
        }

        if (data && data.m_AttachedRecipeIds && data.m_AttachedRecipeIds.Count() > 0)
        {
            for (int a = 0; a < data.m_AttachedRecipeIds.Count(); a++)
            {
                int dopId = data.m_AttachedRecipeIds[a];
                DRL_Craft_Recipe dopR = GetCraft_Config().FindById(dopId);
                if (!dopR || !dopR.CraftComponents)
                {
                    continue;
                }

                string dopCls = dopR.Result;
                if (data.m_AttachedVariants && data.m_AttachedVariants.Contains(dopId))
                {
                    dopCls = data.m_AttachedVariants.Get(dopId);
                }

                for (int k = 0; k < dopR.CraftComponents.Count(); k++)
                {
                    DRL_Craft_Component attC = dopR.CraftComponents[k];
                    if (!attC)
                    {
                        continue;
                    }
                    if (!attC.IsApplicableForResult(dopCls))
                    {
                        continue;
                    }

                    DRL_Craft_Component attEffective = attC;
                    int keyDop = dopR.RecipeId * 100 + k;
                    if (data && data.m_SelectedVariations)
                    {
                        string chosenAttVar = "";
                        if (data.m_SelectedVariations.Contains(keyDop))
                        {
                            chosenAttVar = data.m_SelectedVariations.Get(keyDop);
                        }
                        else if (data.m_SelectedVariations.Contains(k))
                        {
                            chosenAttVar = data.m_SelectedVariations.Get(k);
                        }

                        if (chosenAttVar != "")
                        {
                            attEffective = attC.CloneWithVariation(chosenAttVar);
                        }
                    }

                    bool found = false;
                    for (int m = 0; m < mergedList.Count(); m++)
                    {
                        DRL_CraftMergedComponent ex = mergedList[m];
                        if (ex && ex.Classname == attEffective.Classname && ex.AttachmentsEx == attEffective.AttachmentsEx)
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
        }

        array<ref DRL_Craft_Component> resultList = new array<ref DRL_Craft_Component>();
        for (int f = 0; f < mergedList.Count(); f++)
        {
            DRL_CraftMergedComponent mcFinal = mergedList[f];
            if (mcFinal)
            {
                resultList.Insert(mcFinal.ToCraftComponent());
            }
        }
        return resultList;
    }

    void GetActiveProcessesForPlayerOrWorkbench(PlayerBase player, EntityAI wb, out array<ref DRL_CraftProcessSyncInfo> outList)
    {
        outList = new array<ref DRL_CraftProcessSyncInfo>();

        if (wb)
        {
            array<ref DRL_CraftProcessData> wbProcesses;
            GetProcessesForWorkbench(wb, wbProcesses);
            for (int i = 0; i < wbProcesses.Count(); i++)
            {
                if (wbProcesses[i])
                {
                    outList.Insert(new DRL_CraftProcessSyncInfo(wbProcesses[i]));
                }
            }
            return;
        }

        if (player)
        {
            string steamId = player.DRL_GetPlayerIdSafe();
            array<ref DRL_CraftProcessData> pProcesses;
            GetProcessesForPlayer(steamId, pProcesses);
            for (int j = 0; j < pProcesses.Count(); j++)
            {
                if (pProcesses[j])
                {
                    outList.Insert(new DRL_CraftProcessSyncInfo(pProcesses[j]));
                }
            }
        }
    }

    protected void BroadcastWorkbenchProcessStarted(EntityAI wb, DRL_CraftProcessSyncInfo syncInfo)
    {
        if (!wb || !syncInfo)
        {
            return;
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        vector pos = wb.GetPosition();

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (pb && pb.GetIdentity() && vector.DistanceSq(pb.GetPosition(), pos) < 100.0)
            {
                pb.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_STARTED, new Param1<DRL_CraftProcessSyncInfo>(syncInfo), true, pb.GetIdentity());
            }
        }
    }

    protected void BroadcastWorkbenchProcessFinished(EntityAI wb, string processId, bool cancelled)
    {
        if (!wb)
        {
            return;
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        vector pos = wb.GetPosition();

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (pb && pb.GetIdentity() && vector.DistanceSq(pb.GetPosition(), pos) < 100.0)
            {
                pb.RPCSingleParam(DRL_CraftRPC.STC_CRAFT_PROCESS_FINISHED, new Param2<string, bool>(processId, cancelled), true, pb.GetIdentity());
            }
        }
    }

    protected PlayerBase FindPlayerBySteamId(string steamId)
    {
        if (steamId == "")
        {
            return null;
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (pb && pb.DRL_GetPlayerIdSafe() == steamId)
            {
                return pb;
            }
        }
        return null;
    }

    protected EntityAI FindWorkbenchForProcess(DRL_CraftProcessData data)
    {
        if (!data || !data.m_IsWorkbenchCraft)
        {
            return null;
        }

        if (data.m_WorkbenchNetLow != 0 || data.m_WorkbenchNetHigh != 0)
        {
            Object obj = GetGame().GetObjectByNetworkId(data.m_WorkbenchNetLow, data.m_WorkbenchNetHigh);
            EntityAI ea = EntityAI.Cast(obj);
            if (ea)
            {
                return ea;
            }
        }

        array<Object> nearObjects = new array<Object>();
        GetGame().GetObjectsAtPosition(data.m_WorkbenchPos, 2.0, nearObjects, null);
        for (int i = 0; i < nearObjects.Count(); i++)
        {
            EntityAI nearEa = EntityAI.Cast(nearObjects[i]);
            if (nearEa && (nearEa.GetType() == data.m_WorkbenchClass || nearEa.IsKindOf(data.m_WorkbenchClass)))
            {
                return nearEa;
            }
        }
        return null;
    }

    protected void SaveProcessBin(DRL_CraftProcessData data)
    {
        if (!data)
        {
            return;
        }

        string filePath = DIR_BASE + data.GetBinFileName();
        FileSerializer file = new FileSerializer();
        if (file.Open(filePath, FileMode.WRITE))
        {
            data.OnWrite(file);
            file.Close();
        }
        else
        {
            DRL_CraftLogger.Error("[DRL_CraftProcessManager] Failed to write bin file: " + filePath);
        }
    }

    protected void DeleteProcessBin(DRL_CraftProcessData data)
    {
        if (!data)
        {
            return;
        }

        string filePath = DIR_BASE + data.GetBinFileName();
        if (FileExist(filePath))
        {
            DeleteFile(filePath);
        }
    }
}
