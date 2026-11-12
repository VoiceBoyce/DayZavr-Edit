class ActionSFP_UnpackBattery9VPacksCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionSFP_UnpackBattery9VPacks : ActionContinuousBase
{
	void ActionSFP_UnpackBattery9VPacks()
	{
		m_CallbackClass = ActionSFP_UnpackBattery9VPacksCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#open";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}

	protected bool GetBatteryOutputType(string pack_type, out string battery_type)
	{
		switch (pack_type)
		{
			case "SFP_Battery9V_pack_Energ":
				battery_type = "SFP_Battery9V_Energ";
				return true;

			case "SFP_Battery9V_pack_GP":
				battery_type = "SFP_Battery9V_GP";
				return true;

			case "SFP_Battery9V_pack_DW":
				battery_type = "SFP_Battery9V_DW";
				return true;

			case "SFP_Battery9V_pack_Panason":
				battery_type = "SFP_Battery9V_Panason";
				return true;

			case "SFP_Battery9V_pack_Kosmos":
				battery_type = "SFP_Battery9V_Kosmos";
				return true;

			case "SFP_Battery9V_pack_Dur":
				battery_type = "SFP_Battery9V_Dur";
				return true;
		}

		battery_type = string.Empty;
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		// strictly when the pack is in hands
		if (player.GetItemInHands() != item)
			return false;

		string out_type;
		return GetBatteryOutputType(item.GetType(), out_type);
	}

	protected ItemBase SpawnToPlayerInventoryOrGround(PlayerBase player, string type)
	{
		EntityAI created = player.GetInventory().CreateInInventory(type);
		if (!created)
		{
			Object obj = GetGame().CreateObjectEx(type, player.GetPosition(), ECE_PLACE_ON_SURFACE);
			created = EntityAI.Cast(obj);
		}

		return ItemBase.Cast(created);
	}

	protected void SetItemEnergyFull(ItemBase item)
	{
		if (!item)
			return;

		if (item.HasEnergyManager() && item.GetCompEM())
		{
			item.GetCompEM().SetEnergy(item.GetCompEM().GetEnergyMax());
		}
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);

		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return;

		// sanity: still in player's hands
		if (action_data.m_MainItem.GetHierarchyRootPlayer() != action_data.m_Player)
			return;

		string battery_type;
		if (!GetBatteryOutputType(action_data.m_MainItem.GetType(), battery_type))
			return;

		// create exactly 2 batteries with full energy
		for (int i = 0; i < 2; i++)
		{
			ItemBase batt = SpawnToPlayerInventoryOrGround(action_data.m_Player, battery_type);
			SetItemEnergyFull(batt);
		}

		// delete the pack
		action_data.m_MainItem.DeleteSafe();
	}

	override void OnFinishProgressClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
	}
};
