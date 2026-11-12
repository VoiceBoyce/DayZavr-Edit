class ActionSFP_Unpack2CigarettePacksCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionSFP_Unpack2CigarettePacks : ActionContinuousBase
{
	void ActionSFP_Unpack2CigarettePacks()
	{
		m_CallbackClass = ActionSFP_Unpack2CigarettePacksCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
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

	protected bool IsCigPackType(string pack_type)
	{
		switch (pack_type)
		{
			case "SFP_2cigarettes_pack_camel":
			case "SFP_2cigarettes_pack_kent":
			case "SFP_2cigarettes_pack_magna":
			case "SFP_2cigarettes_pack_marlboro":
			case "SFP_2cigarettes_pack_parliament":
			case "SFP_2cigarettes_pack_PhilipMor":
			case "SFP_2cigarettes_pack_RusSlyle":
			case "SFP_2cigarettes_pack_SouyzAppo":
			case "SFP_2cigarettes_pack_winston":
			case "SFP_2cigarettes_pack_goldYava":
				return true;
		}

		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		// strictly when the pack is in hands
		if (player.GetItemInHands() != item)
			return false;

		return IsCigPackType(item.GetType());
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

	override void OnFinishProgressServer(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);

		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return;

		// sanity: still in player's hands
		if (action_data.m_MainItem.GetHierarchyRootPlayer() != action_data.m_Player)
			return;

		if (!IsCigPackType(action_data.m_MainItem.GetType()))
			return;

		// create exactly 2 cigarettes
		for (int i = 0; i < 2; i++)
		{
			SpawnToPlayerInventoryOrGround(action_data.m_Player, "SFP_SigaPack_Single");
		}

		// delete the pack
		action_data.m_MainItem.DeleteSafe();
	}

	override void OnFinishProgressClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
	}
};
