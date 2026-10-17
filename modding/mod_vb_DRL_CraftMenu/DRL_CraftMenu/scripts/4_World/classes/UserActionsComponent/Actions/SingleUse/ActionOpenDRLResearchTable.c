class ActionOpenDRLResearchTable : ActionInteractBase
{
	void ActionOpenDRLResearchTable()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#STR_STUDY_RECIPES";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player)
		{
			return false;
		}
		if (!target)
		{
			return false;
		}
		Object obj = target.GetObject();
		if (!obj)
		{
			return false;
		}

		return obj.IsKindOf("DRL_Craft_Research_Table") || obj.IsKindOf("DRL_Craft_Research_Book");
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		EntityAI table = EntityAI.Cast(action_data.m_Target.GetObject());
		if (table)
		{
			GetGame().GetUIManager().EnterScriptedMenu(9503, null);
		}
	}
}
