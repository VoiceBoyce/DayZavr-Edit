class DRL_CraftPreviewHelper
{
    static void CreatePreviewObject(string className, inout EntityAI preview)
    {
        className = DRL_ItemTypeHelper.GetSafePreviewClass(className);
        if (className == "")
        {
            if (preview)
            {
                GetGame().ObjectDelete(preview);
                preview = null;
            }
            return;
        }

        if (GetGame().IsKindOf(className, "DZ_LightAI"))
        {
            if (preview)
            {
                GetGame().ObjectDelete(preview);
                preview = null;
            }
            return;
        }

        if (preview)
        {
            PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
            if (pb && preview.GetHierarchyRootPlayer() == pb)
            {
                return;
            }

            if (preview.GetType() == className)
            {
                if (preview.GetInventory())
                {
                    for (int i = preview.GetInventory().AttachmentCount() - 1; i >= 0; i--)
                    {
                        EntityAI attachment = preview.GetInventory().GetAttachmentFromIndex(i);
                        if (attachment)
                        {
                            GetGame().ObjectDelete(attachment);
                        }
                    }
                }
                return;
            }
            else
            {
                GetGame().ObjectDelete(preview);
                preview = null;
            }
        }

        preview = EntityAI.Cast(GetGame().CreateObjectEx(className, vector.Zero, ECE_LOCAL|ECE_NOLIFETIME));
        if (preview && !GetGame().IsServer())
        {
            Transport transport;
            if (Class.CastTo(transport, preview))
            {
                dBodyActive(preview, ActiveState.INACTIVE);
                dBodyDynamic(preview, false);
                transport.DisableSimulation(true);
            }

            Weapon_Base weapon;
            if (Class.CastTo(weapon, preview))
            {
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(weapon.AssembleGun);
            }
        }
    }

    static EntityAI GetOrCreatePreviewEntity(string clsname, inout EntityAI existingEntity, map<string, EntityAI> cache = null)
    {
        clsname = DRL_ItemTypeHelper.GetSafePreviewClass(clsname);
        if (clsname == "")
        {
            return null;
        }

        if (cache)
        {
            EntityAI cachedEnt;
            if (cache.Find(clsname, cachedEnt) && cachedEnt)
            {
                return cachedEnt;
            }
        }

        CreatePreviewObject(clsname, existingEntity);

        if (existingEntity && cache)
        {
            cache.Insert(clsname, existingEntity);
        }

        return existingEntity;
    }

    static EntityAI SpawnAttachment(EntityAI parent, string itemClass, int depth = 0)
    {
        if (!parent || !parent.GetInventory() || itemClass == "")
        {
            return null;
        }

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
                wGun.ShowMagazine();
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
                    attEntity = SpawnAttachment(child, itemClass, depth + 1);
                    if (attEntity)
                    {
                        return attEntity;
                    }
                }
            }
        }

        return null;
    }

    static void SetupPreviewWidget(ItemPreviewWidget prevW, EntityAI ent, vector orientation = vector.Zero)
    {
        if (!prevW)
        {
            return;
        }

        prevW.SetItem(ent);

        if (ent)
        {
            int vIdx = ent.GetViewIndex();
            if (vIdx < 0)
            {
                vIdx = 0;
            }
            prevW.SetView(vIdx);
            prevW.SetModelPosition(Vector(0, 0, 0));
            prevW.SetModelOrientation(orientation);
            prevW.Show(true);
        }
        else
        {
            prevW.Show(false);
        }
    }

    static void SafeDeleteEntity(inout EntityAI ent)
    {
        if (ent)
        {
            GetGame().ObjectDelete(ent);
            ent = null;
        }
    }

    static void ClearCache(map<string, EntityAI> cache)
    {
        if (!cache)
        {
            return;
        }

        foreach (string cls, EntityAI ent : cache)
        {
            if (ent)
            {
                GetGame().ObjectDelete(ent);
            }
        }
        cache.Clear();
    }
}
