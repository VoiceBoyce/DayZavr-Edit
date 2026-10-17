/**
 * DRL_CraftComponentResolver.c
 * Unified component resolver for DRL_CraftMenu.
 * Handles item matching, location constraints (AttachmentsEx 0, 1, 2),
 * and requirement verification across player and workbench.
 */
class DRL_CraftComponentResolver
{
    static bool DoesItemMatchComponent(ItemBase item, DRL_Craft_Component c)
    {
        if (!item || item.IsRuined() || !c)
        {
            return false;
        }

        int lvl = item.GetHealthLevel();
        if (lvl > c.MinHealthFrac)
        {
            return false;
        }

        string itemType = item.GetType();
        string itemTypeLower = itemType;
        itemTypeLower.ToLower();

        if (c.Classnames && c.Classnames.Count() > 0)
        {
            for (int i = 0; i < c.Classnames.Count(); i++)
            {
                string req = c.Classnames[i];
                if (req == "")
                {
                    continue;
                }

                int slotId = InventorySlots.GetSlotIdFromString(req);
                if (slotId > -1 && DoesItemFitSlot(item, slotId))
                {
                    return true;
                }

                if (c.UseInheritance == 1)
                {
                    if (item.IsKindOf(req))
                    {
                        return true;
                    }
                }
                else
                {
                    string reqLower = req;
                    reqLower.ToLower();
                    if (itemTypeLower == reqLower)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        if (c.Classname != "")
        {
            int slotIdSingle = InventorySlots.GetSlotIdFromString(c.Classname);
            if (slotIdSingle > -1 && DoesItemFitSlot(item, slotIdSingle))
            {
                return true;
            }

            if (c.UseInheritance == 1)
            {
                if (item.IsKindOf(c.Classname))
                {
                    return true;
                }
            }
            else
            {
                string reqSingleLower = c.Classname;
                reqSingleLower.ToLower();
                if (itemTypeLower == reqSingleLower)
                {
                    return true;
                }
            }
            return false;
        }

        return false;
    }

    static bool DoesItemFitSlot(ItemBase item, int targetSlotId)
    {
        if (!item || !item.GetInventory() || targetSlotId < 0)
        {
            return false;
        }

        int slotCount = item.GetInventory().GetSlotIdCount();
        for (int i = 0; i < slotCount; i++)
        {
            if (item.GetInventory().GetSlotId(i) == targetSlotId)
            {
                return true;
            }
        }
        return false;
    }

    static bool IsDirectAttachment(EntityAI owner, ItemBase item)
    {
        if (!owner || !item || !item.GetInventory())
        {
            return false;
        }

        InventoryLocation il = new InventoryLocation();
        if (item.GetInventory().GetCurrentInventoryLocation(il))
        {
            if (il.GetType() == InventoryLocationType.ATTACHMENT)
            {
                EntityAI parent = il.GetParent();
                if (parent == owner)
                {
                    return true;
                }

                EntityAI rootOwner = owner.GetHierarchyRoot();
                if (rootOwner && parent == rootOwner)
                {
                    return true;
                }
            }
        }
        return false;
    }

    static bool IsAllocated(EntityAI item, array<EntityAI> allocatedItems)
    {
        if (!item)
        {
            return true;
        }

        ItemBase ib = ItemBase.Cast(item);
        if (ib && ib.DRL_IsCraftLocked())
        {
            return true;
        }

        if (allocatedItems && allocatedItems.Find(item) != -1)
        {
            return true;
        }

        return false;
    }

    static ItemBase FindItemForComponent(DRL_Craft_Component c, PlayerBase player, EntityAI workbench, string activeResultCls = "", bool onlyUnallocated = false, array<EntityAI> allocatedItems = null)
    {
        if (!c)
        {
            return null;
        }

        if (activeResultCls != "" && !c.IsApplicableForResult(activeResultCls))
        {
            return null;
        }


        if (workbench && workbench.GetInventory())
        {
            EntityAI rootWb = workbench;
            EntityAI hRoot = rootWb.GetHierarchyRoot();
            if (hRoot && !hRoot.IsInherited(Man))
            {
                rootWb = hRoot;
            }

            int acWb = rootWb.GetInventory().AttachmentCount();
            for (int i = 0; i < acWb; i++)
            {
                ItemBase ibWbAtt = ItemBase.Cast(rootWb.GetInventory().GetAttachmentFromIndex(i));
                if (ibWbAtt && (!onlyUnallocated || !IsAllocated(ibWbAtt, allocatedItems)))
                {
                    if (DoesItemMatchComponent(ibWbAtt, c))
                    {
                        return ibWbAtt;
                    }
                }
            }

            if (workbench != rootWb && workbench.GetInventory())
            {
                int acSubWb = workbench.GetInventory().AttachmentCount();
                for (int s = 0; s < acSubWb; s++)
                {
                    ItemBase ibSubWbAtt = ItemBase.Cast(workbench.GetInventory().GetAttachmentFromIndex(s));
                    if (ibSubWbAtt && (!onlyUnallocated || !IsAllocated(ibSubWbAtt, allocatedItems)))
                    {
                        if (DoesItemMatchComponent(ibSubWbAtt, c))
                        {
                            return ibSubWbAtt;
                        }
                    }
                }
            }
        }

        if (player && player.GetInventory())
        {
            int acPlayer = player.GetInventory().AttachmentCount();
            for (int p = 0; p < acPlayer; p++)
            {
                ItemBase ibPlayerAtt = ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(p));
                if (ibPlayerAtt && (!onlyUnallocated || !IsAllocated(ibPlayerAtt, allocatedItems)))
                {
                    if (c.AttachmentsEx == 0 && IsDirectAttachment(player, ibPlayerAtt))
                    {
                        continue;
                    }

                    if (DoesItemMatchComponent(ibPlayerAtt, c))
                    {
                        return ibPlayerAtt;
                    }
                }
            }
        }

        if (c.AttachmentsEx == 1)
        {
            return null;
        }

        if (workbench && workbench.GetInventory())
        {
            EntityAI rootWbInv = workbench;
            EntityAI hRootInv = rootWbInv.GetHierarchyRoot();
            if (hRootInv && !hRootInv.IsInherited(Man))
            {
                rootWbInv = hRootInv;
            }

            array<EntityAI> wbItems = new array<EntityAI>();
            rootWbInv.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, wbItems);
            for (int w = 0; w < wbItems.Count(); w++)
            {
                ItemBase ibWbCargo = ItemBase.Cast(wbItems[w]);
                if (ibWbCargo && ibWbCargo != rootWbInv && (!onlyUnallocated || !IsAllocated(ibWbCargo, allocatedItems)))
                {
                    if (DoesItemMatchComponent(ibWbCargo, c))
                    {
                        return ibWbCargo;
                    }
                }
            }
        }

        if (player && player.GetInventory())
        {
            array<EntityAI> playerItems = new array<EntityAI>();
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, playerItems);
            for (int pl = 0; pl < playerItems.Count(); pl++)
            {
                ItemBase ibPlayerInv = ItemBase.Cast(playerItems[pl]);
                if (ibPlayerInv && ibPlayerInv != player && (!onlyUnallocated || !IsAllocated(ibPlayerInv, allocatedItems)))
                {
                    if (c.AttachmentsEx == 0 && IsDirectAttachment(player, ibPlayerInv))
                    {
                        continue;
                    }

                    if (DoesItemMatchComponent(ibPlayerInv, c))
                    {
                        return ibPlayerInv;
                    }
                }
            }
        }

        return null;
    }

    static void CollectMatchingItems(DRL_Craft_Component c, PlayerBase player, EntityAI workbench, string activeResultCls, out array<ItemBase> outItems, bool excludeAllocated = false, array<EntityAI> allocatedItems = null)
    {
        if (!outItems)
        {
            outItems = new array<ItemBase>();
        }

        if (!c)
        {
            return;
        }

        if (activeResultCls != "" && !c.IsApplicableForResult(activeResultCls))
        {
            return;
        }

        if (workbench && workbench.GetInventory())
        {
            EntityAI rootWb = workbench;
            EntityAI hRoot = rootWb.GetHierarchyRoot();
            if (hRoot && !hRoot.IsInherited(Man))
            {
                rootWb = hRoot;
            }

            int acWb = rootWb.GetInventory().AttachmentCount();
            for (int i = 0; i < acWb; i++)
            {
                ItemBase ibWbAtt = ItemBase.Cast(rootWb.GetInventory().GetAttachmentFromIndex(i));
                if (ibWbAtt && (!excludeAllocated || !IsAllocated(ibWbAtt, allocatedItems)))
                {
                    if (DoesItemMatchComponent(ibWbAtt, c) && outItems.Find(ibWbAtt) == -1)
                    {
                        outItems.Insert(ibWbAtt);
                    }
                }
            }

            if (workbench != rootWb && workbench.GetInventory())
            {
                int acSubWb = workbench.GetInventory().AttachmentCount();
                for (int s = 0; s < acSubWb; s++)
                {
                    ItemBase ibSubWbAtt = ItemBase.Cast(workbench.GetInventory().GetAttachmentFromIndex(s));
                    if (ibSubWbAtt && (!excludeAllocated || !IsAllocated(ibSubWbAtt, allocatedItems)))
                    {
                        if (DoesItemMatchComponent(ibSubWbAtt, c) && outItems.Find(ibSubWbAtt) == -1)
                        {
                            outItems.Insert(ibSubWbAtt);
                        }
                    }
                }
            }
        }

        if (player && player.GetInventory())
        {
            int acPlayer = player.GetInventory().AttachmentCount();
            for (int p = 0; p < acPlayer; p++)
            {
                ItemBase ibPlayerAtt = ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(p));
                if (ibPlayerAtt && (!excludeAllocated || !IsAllocated(ibPlayerAtt, allocatedItems)))
                {
                    if (c.AttachmentsEx == 0 && IsDirectAttachment(player, ibPlayerAtt))
                    {
                        continue;
                    }

                    if (DoesItemMatchComponent(ibPlayerAtt, c) && outItems.Find(ibPlayerAtt) == -1)
                    {
                        outItems.Insert(ibPlayerAtt);
                    }
                }
            }
        }

        if (c.AttachmentsEx == 1)
        {
            return;
        }

        if (workbench && workbench.GetInventory())
        {
            EntityAI rootWbInv = workbench;
            EntityAI hRootInv = rootWbInv.GetHierarchyRoot();
            if (hRootInv && !hRootInv.IsInherited(Man))
            {
                rootWbInv = hRootInv;
            }

            array<EntityAI> wbItems = new array<EntityAI>();
            rootWbInv.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, wbItems);
            for (int w = 0; w < wbItems.Count(); w++)
            {
                ItemBase ibWbCargo = ItemBase.Cast(wbItems[w]);
                if (ibWbCargo && ibWbCargo != rootWbInv && (!excludeAllocated || !IsAllocated(ibWbCargo, allocatedItems)))
                {
                    if (DoesItemMatchComponent(ibWbCargo, c) && outItems.Find(ibWbCargo) == -1)
                    {
                        outItems.Insert(ibWbCargo);
                    }
                }
            }
        }

        if (player && player.GetInventory())
        {
            array<EntityAI> playerItems = new array<EntityAI>();
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, playerItems);
            for (int pl = 0; pl < playerItems.Count(); pl++)
            {
                ItemBase ibPlayerInv = ItemBase.Cast(playerItems[pl]);
                if (ibPlayerInv && ibPlayerInv != player && (!excludeAllocated || !IsAllocated(ibPlayerInv, allocatedItems)))
                {
                    if (c.AttachmentsEx == 0 && IsDirectAttachment(player, ibPlayerInv))
                    {
                        continue;
                    }

                    if (DoesItemMatchComponent(ibPlayerInv, c) && outItems.Find(ibPlayerInv) == -1)
                    {
                        outItems.Insert(ibPlayerInv);
                    }
                }
            }
        }
    }

    static bool HasComponentRequirement(DRL_Craft_Component c, PlayerBase player, EntityAI workbench, string activeResultCls, int craftCount = 1, array<EntityAI> allocatedItems = null)
    {
        if (!c)
        {
            return true;
        }

        if (activeResultCls != "" && !c.IsApplicableForResult(activeResultCls))
        {
            return true;
        }

        if (c.IsTool() && !c.Delete && c.QuantityFrac <= 0.0)
        {
            ItemBase foundTool = FindItemForComponent(c, player, workbench, activeResultCls, true, allocatedItems);
            return (foundTool != null);
        }

        int neededAmount = c.Amount * craftCount;
        array<ItemBase> matchingItems = new array<ItemBase>();
        CollectMatchingItems(c, player, workbench, activeResultCls, matchingItems, true, allocatedItems);
        if (matchingItems.Count() == 0)
        {
            return false;
        }

        int firstQType = DRL_ItemQuantityHelper.GetQuantityType(matchingItems[0]);
        bool isFillingComp = (c.QuantityFrac > 0.0 && DRL_ItemQuantityHelper.IsFillingType(firstQType));

        if (isFillingComp)
        {
            float totalQuantity = 0.0;
            float totalUnitsNeeded = 0.0;
            bool unitsResolved = false;

            for (int i = 0; i < matchingItems.Count(); i++)
            {
                ItemBase ib = matchingItems[i];
                if (!unitsResolved)
                {
                    float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(ib);
                    if (qMax <= 0.0)
                    {
                        qMax = 1.0;
                    }
                    float perUnitFrac = Math.Round(qMax * c.QuantityFrac);
                    if (perUnitFrac < 1.0)
                    {
                        perUnitFrac = 1.0;
                    }
                    totalUnitsNeeded = perUnitFrac * (float)neededAmount;
                    unitsResolved = true;
                }

                totalQuantity += DRL_ItemQuantityHelper.GetQuantityRaw(ib);
                if (totalQuantity >= totalUnitsNeeded)
                {
                    return true;
                }
            }

            if (!unitsResolved)
            {
                return false;
            }

            return (totalQuantity >= totalUnitsNeeded);
        }

        int totalCount = 0;
        for (int j = 0; j < matchingItems.Count(); j++)
        {
            totalCount += DRL_ItemQuantityHelper.GetStackAmount(matchingItems[j]);
            if (totalCount >= neededAmount)
            {
                return true;
            }
        }

        return (totalCount >= neededAmount);
    }

    static int ComputeAvailableCount(DRL_Craft_Component c, PlayerBase player, EntityAI workbench, string activeResultCls = "")
    {
        if (!c)
        {
            return 0;
        }

        if (activeResultCls != "" && !c.IsApplicableForResult(activeResultCls))
        {
            return 0;
        }

        if (c.IsTool() && !c.Delete && c.QuantityFrac <= 0.0)
        {
            ItemBase tool = FindItemForComponent(c, player, workbench, activeResultCls, false, null);
            if (tool)
            {
                return 1;
            }
            return 0;
        }

        array<ItemBase> matchingItems = new array<ItemBase>();
        CollectMatchingItems(c, player, workbench, activeResultCls, matchingItems, false, null);
        if (matchingItems.Count() == 0)
        {
            return 0;
        }

        int firstQTypeAvail = DRL_ItemQuantityHelper.GetQuantityType(matchingItems[0]);
        bool isFillingCompAvail = (c.QuantityFrac > 0.0 && DRL_ItemQuantityHelper.IsFillingType(firstQTypeAvail));

        if (isFillingCompAvail)
        {
            float totalQuantity = 0.0;
            float perUnitUnits = 0.0;
            for (int i = 0; i < matchingItems.Count(); i++)
            {
                ItemBase ib = matchingItems[i];
                if (perUnitUnits <= 0.0)
                {
                    float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(ib);
                    if (qMax <= 0.0)
                    {
                        qMax = 1.0;
                    }
                    perUnitUnits = Math.Round(qMax * c.QuantityFrac);
                    if (perUnitUnits < 1.0)
                    {
                        perUnitUnits = 1.0;
                    }
                }
                totalQuantity += DRL_ItemQuantityHelper.GetQuantityRaw(ib);
            }

            if (perUnitUnits <= 0.0)
            {
                return 0;
            }

            return (int)(totalQuantity / perUnitUnits);
        }

        int totalCount = 0;
        for (int k = 0; k < matchingItems.Count(); k++)
        {
            totalCount += DRL_ItemQuantityHelper.GetStackAmount(matchingItems[k]);
        }
        return totalCount;
    }

    static bool ExecuteComponentConsumption(DRL_Craft_Component c, PlayerBase player, EntityAI workbench, string activeResultCls, int craftCount = 1)
    {
        if (!c)
        {
            return true;
        }

        if (activeResultCls != "" && !c.IsApplicableForResult(activeResultCls))
        {
            return true;
        }

        array<ItemBase> matchingItems = new array<ItemBase>();
        CollectMatchingItems(c, player, workbench, activeResultCls, matchingItems, false, null);
        if (matchingItems.Count() == 0)
        {
            return false;
        }

        int firstQType = DRL_ItemQuantityHelper.GetQuantityType(matchingItems[0]);
        bool isFillingComp = (c.QuantityFrac > 0.0 && DRL_ItemQuantityHelper.IsFillingType(firstQType));

        if (isFillingComp)
        {
            float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(matchingItems[0]);
            if (qMax <= 0.0)
            {
                qMax = 1.0;
            }
            float perUnit = Math.Round(qMax * c.QuantityFrac);
            if (perUnit < 1.0)
            {
                perUnit = 1.0;
            }
            float unitsRemainingToConsume = perUnit * (float)(Math.Max(1, c.Amount) * craftCount);

            for (int i = 0; i < matchingItems.Count(); i++)
            {
                ItemBase ibQ = matchingItems[i];
                if (!ibQ || ibQ.IsRuined())
                {
                    continue;
                }

                int qType = DRL_ItemQuantityHelper.GetQuantityType(ibQ);
                if (qType == EDRLItemQuantityType.ENERGY)
                {
                    ComponentEnergyManager em = ibQ.GetCompEM();
                    if (em)
                    {
                        float currE = em.GetEnergy();
                        float takeE = Math.Min(currE, unitsRemainingToConsume);
                        em.AddEnergy(-takeE);
                        unitsRemainingToConsume -= takeE;
                    }
                }
                else
                {
                    float currQ = DRL_ItemQuantityHelper.GetQuantityRaw(ibQ);
                    float takeQ = Math.Min(currQ, unitsRemainingToConsume);
                    float newQ = currQ - takeQ;
                    unitsRemainingToConsume -= takeQ;

                    if (newQ <= 0.001)
                    {
                        if (c.Delete)
                        {
                            GetGame().ObjectDelete(ibQ);
                        }
                        else
                        {
                            DRL_ItemQuantityHelper.SetFilling(ibQ, 0.0, 0);
                        }
                    }
                    else
                    {
                        if (qType == EDRLItemQuantityType.MAGAZINE)
                        {
                            Magazine mag = Magazine.Cast(ibQ);
                            if (mag)
                            {
                                mag.ServerSetAmmoCount((int)Math.Round(newQ));
                            }
                        }
                        else
                        {
                            ibQ.SetQuantity((int)Math.Round(newQ));
                        }
                    }
                }

                if (unitsRemainingToConsume <= 0.001)
                {
                    break;
                }
            }
        }

        if (c.ApplyDamageFrac > 0.0)
        {
            for (int d = 0; d < matchingItems.Count(); d++)
            {
                ItemBase toolDmg = matchingItems[d];
                if (!toolDmg || toolDmg.IsRuined())
                {
                    continue;
                }

                float maxH = toolDmg.GetMaxHealth("", "Health");
                if (maxH > 0.0)
                {
                    float dmg = maxH * c.ApplyDamageFrac * (float)craftCount;
                    toolDmg.AddHealth("", "Health", -dmg);
                }
                break;
            }
        }

        if (isFillingComp || (!c.Delete && !DRL_ItemQuantityHelper.IsStackable(matchingItems[0])))
        {
            return true;
        }

        int remainingAmount = Math.Max(1, c.Amount) * craftCount;
        for (int s = 0; s < matchingItems.Count(); s++)
        {
            ItemBase ibItem = matchingItems[s];
            if (!ibItem || ibItem.IsRuined())
            {
                continue;
            }

            if (DRL_ItemQuantityHelper.IsStackable(ibItem))
            {
                int stackAmount = DRL_ItemQuantityHelper.GetStackAmount(ibItem);
                if (stackAmount <= remainingAmount)
                {
                    remainingAmount -= stackAmount;
                    if (c.Delete)
                    {
                        GetGame().ObjectDelete(ibItem);
                    }
                    else
                    {
                        Magazine magStackEmpty = Magazine.Cast(ibItem);
                        if (magStackEmpty && magStackEmpty.IsAmmoPile())
                        {
                            magStackEmpty.ServerSetAmmoCount(0);
                        }
                        else
                        {
                            ibItem.SetQuantity(0);
                        }
                    }
                }
                else
                {
                    int newStack = stackAmount - remainingAmount;
                    Magazine magStack = Magazine.Cast(ibItem);
                    if (magStack && magStack.IsAmmoPile())
                    {
                        magStack.ServerSetAmmoCount(newStack);
                    }
                    else
                    {
                        ibItem.SetQuantity(newStack);
                    }
                    remainingAmount = 0;
                }
            }
            else
            {
                remainingAmount -= 1;
                if (c.Delete)
                {
                    GetGame().ObjectDelete(ibItem);
                }
            }

            if (remainingAmount <= 0)
            {
                break;
            }
        }

        return true;
    }
}
