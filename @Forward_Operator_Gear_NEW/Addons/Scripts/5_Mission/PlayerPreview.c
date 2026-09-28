modded class PlayerPreview
{
    override void UpdateInterval()
    {
        super.UpdateInterval();
        
        DayZPlayer dummy_player = m_CharacterPanelWidget.GetDummyPlayer();
        EntityAI leg_item = dummy_player.GetInventory().FindAttachmentByName("Legs");
        EntityAI dummy_legs = dummy_player.GetInventory().FindPlaceholderForSlot(InventorySlots.GetSlotIdFromString("Legs"));
        if (dummy_legs) {
            dummy_legs.SetInvisible(leg_item && leg_item.IsKindOf("FOG_FlightSuit_ColorBase"));
        }
    }
}