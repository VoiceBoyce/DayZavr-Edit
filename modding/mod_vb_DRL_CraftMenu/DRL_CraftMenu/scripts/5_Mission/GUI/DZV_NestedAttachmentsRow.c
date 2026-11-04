// DayZavr: показ вложенных слотов у станций DRL.
// Ванильный AttachmentCategoriesRow (станок с группами слотов) показывает у предмета в слоте только его карго,
// а слоты самого предмета — нет. Поэтому у швейного стола на станке не видно слота под машинку,
// а у машинки на столе — слотов ножниц и шпули.
// Здесь под строкой категории добавляется обычный ванильный блок ContainerWithCargoAndAttachments
// для предмета без карго, но со своими слотами. Он сам показывает следующий уровень (машинку: её слоты и карго).
// Только для станций DRL (DRL_Craft_WB и наследники), машины/забор/вышки не затрагиваются.
modded class AttachmentCategoriesRow
{
	protected ref map<int, ref ContainerWithCargoAndAttachments> m_DZV_SubAttachments;

	protected bool DZV_WantsSubAttachments(EntityAI item, int slotId)
	{
		if (!item || !m_Entity)
			return false;
		if (!m_Entity.IsInherited(DRL_Craft_WB))
			return false;
		if (!m_Entity.CanDisplayAttachmentSlot(slotId))
			return false;
		if (!item.GetInventory())
			return false;
		if (item.GetInventory().GetCargo())
			return false; // предмет с карго ваниль уже показывает своим блоком
		if (item.GetInventory().GetAttachmentSlotsCount() <= 0)
			return false;
		return item.CanDisplayAnyAttachmentSlot();
	}

	override void RefreshSlot(int row, int column, int slot_id, string slot_name)
	{
		super.RefreshSlot(row, column, slot_id, slot_name);

		if (!m_Entity || !m_Entity.IsInherited(DRL_Craft_WB))
			return;

		SlotsIcon icon = m_AttachmentsContainer.GetSlotsIcon(row, column);
		if (!icon)
			return;

		if (!m_DZV_SubAttachments)
			m_DZV_SubAttachments = new map<int, ref ContainerWithCargoAndAttachments>;

		int slotId = icon.GetSlotID();
		EntityAI item = m_Entity.GetInventory().FindAttachment(slotId);
		bool want = DZV_WantsSubAttachments(item, slotId);

		ContainerWithCargoAndAttachments sub = m_DZV_SubAttachments.Get(slotId);
		if (sub && (!want || sub.GetEntity() != item))
		{
			Remove(sub);
			m_DZV_SubAttachments.Remove(slotId); // ref уходит — блок и его виджеты удаляются
			sub = null;
		}

		if (want && !sub)
		{
			sub = new ContainerWithCargoAndAttachments(this, -1);
			m_DZV_SubAttachments.Insert(slotId, sub);
			sub.SetEntity(item, false); // сам вставится в эту строку (m_Parent.Insert)
		}
	}
}
