
modded class PlayerBase
{
    protected FOG_NVG_Base FOG_GetWornNvgFromItem(EntityAI item)
    {
        if (!item)
            return null;

        FOG_NVG_Base nvgDirect = FOG_NVG_Base.Cast(item);
        if (nvgDirect)
            return nvgDirect;

        int nvgSlot = InventorySlots.GetSlotIdFromString("NVG");
        if (nvgSlot == -1)
            return null;

        if (!item.GetInventory())
            return null;

        return FOG_NVG_Base.Cast(item.GetInventory().FindAttachment(nvgSlot));
    }

    override void SwitchItemSelectionTexture(EntityAI item, string slot_name)
    {
        super.SwitchItemSelectionTexture(item, slot_name);

        if (slot_name == "Mask" || slot_name == "Eyewear" || slot_name == "Headgear")
        {
            FOG_Hooded_Parka_ColorBase jacket;
            if (Class.CastTo(jacket, FindAttachmentBySlotName("Body")))
                jacket.HandleSelections();
        }
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        FOG_Hooded_Parka_ColorBase jacket;
        if (Class.CastTo(jacket, FindAttachmentBySlotName("Body")))
            if (!jacket.CanWearWith(attachment, slotId))
                return false;

        return super.CanReceiveAttachment(attachment, slotId);
    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);
        if (item && (slot_name == "Headgear" || slot_name == "Eyewear" || slot_name == "Mask"))
        {
            FOG_NVG_Base nvgOn = FOG_GetWornNvgFromItem(item);
            if (nvgOn)
                nvgOn.FOG_OnParentWornStateChanged(true);
        }
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);
        if (item && (slot_name == "Headgear" || slot_name == "Eyewear" || slot_name == "Mask"))
        {
            FOG_NVG_Base nvgOff = FOG_GetWornNvgFromItem(item);
            if (nvgOff)
                nvgOff.FOG_OnParentWornStateChanged(false);
        }
    }
};