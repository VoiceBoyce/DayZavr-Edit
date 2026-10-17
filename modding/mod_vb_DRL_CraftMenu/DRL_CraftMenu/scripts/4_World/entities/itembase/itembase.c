modded class ItemBase
{
	protected bool m_DRL_CraftLocked;

	void ItemBase()
	{
		RegisterNetSyncVariableBool("m_DRL_CraftLocked");
	}

	void DRL_SetCraftLocked(bool state)
	{
		if (m_DRL_CraftLocked != state)
		{
			m_DRL_CraftLocked = state;
			SetSynchDirty();
		}
	}

	bool DRL_IsCraftLocked()
	{
		return m_DRL_CraftLocked;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		return super.CanPutIntoHands(parent);
	}

	override bool CanRemoveFromCargo(EntityAI parent)
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		return super.CanRemoveFromCargo(parent);
	}

	override bool CanRemoveFromHands(EntityAI parent)
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		return super.CanRemoveFromHands(parent);
	}

	override bool CanDetachAttachment(EntityAI parent)
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		return super.CanDetachAttachment(parent);
	}

	override bool CanBeSplit()
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		return super.CanBeSplit();
	}

	override bool CanBeCombined(EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false)
	{
		if (m_DRL_CraftLocked)
		{
			return false;
		}
		ItemBase otherIb = ItemBase.Cast(other_item);
		if (otherIb && otherIb.DRL_IsCraftLocked())
		{
			return false;
		}
		return super.CanBeCombined(other_item, reservation_check, stack_max_limit);
	}
}