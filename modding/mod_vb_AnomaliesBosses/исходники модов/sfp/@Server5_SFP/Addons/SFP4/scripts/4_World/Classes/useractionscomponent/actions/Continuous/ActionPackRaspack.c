class ActionPackRaspackCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionPackRaspack : ActionContinuousBase
{
	void ActionPackRaspack()
	{
		m_CallbackClass = ActionPackRaspackCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#unbox";
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		// Только в руках
		if (player.GetItemInHands() != item)
			return false;

		return PackRaspackHelper.IsSupportedPack(item);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		ItemBase pack = action_data.m_MainItem;

		if (!player || !pack)
			return;

		// на всякий случай
		if (player.GetItemInHands() != pack)
			return;

		string spawnType;
		int spawnCount;
		if (!PackRaspackHelper.GetPackContent(pack.GetType(), spawnType, spawnCount))
			return;

		vector center = player.GetPosition();

		for (int i = 0; i < spawnCount; i++)
		{
			vector pos = PackRaspackHelper.GetRandomSpawnPos(center);
			ItemBase spawned = ItemBase.Cast(GetGame().CreateObjectEx(spawnType, pos, ECE_PLACE_ON_SURFACE));
			PackRaspackHelper.FillSpawnedItemForPack(spawned, pack.GetType());
		}

		pack.DeleteSafe();
	}
};
