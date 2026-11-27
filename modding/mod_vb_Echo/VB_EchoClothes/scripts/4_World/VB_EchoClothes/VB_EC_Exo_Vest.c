// Бронежилет экзо Эхо (все расцветки): 3 подсумка под магазины вместо гранат (2026-11-27) — ванильные слоты
// magazine/magazine2/magazine3, как у «Берилла» OOTS. Их же наследуют пачки патронов
// (Ammunition_Base: Magazine_Base) — их сюда не пускаем, только магазины.
class VB_EC_Exo_Vest_Base: PlateCarrierVest
{
	protected static int s_VB_Mag1 = -2;
	protected static int s_VB_Mag2;
	protected static int s_VB_Mag3;

	protected bool VB_IsMagSlot(int slotId)
	{
		if (s_VB_Mag1 == -2)
		{
			s_VB_Mag1 = InventorySlots.GetSlotIdFromString("magazine");
			s_VB_Mag2 = InventorySlots.GetSlotIdFromString("magazine2");
			s_VB_Mag3 = InventorySlots.GetSlotIdFromString("magazine3");
		}
		if (slotId == s_VB_Mag1 || slotId == s_VB_Mag2)
			return true;
		if (slotId == s_VB_Mag3)
			return true;
		return false;
	}

	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		if (!VB_IsMagSlot(slotId))
			return true;
		if (!attachment || !attachment.IsInherited(Magazine))
			return false;
		if (attachment.IsInherited(Ammunition_Base))
			return false;
		return true;
	}
}
