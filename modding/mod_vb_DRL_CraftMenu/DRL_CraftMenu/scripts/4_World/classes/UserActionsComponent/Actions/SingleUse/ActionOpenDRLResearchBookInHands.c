class ActionOpenDRLResearchBookInHands : ActionSingleUseBase
{
	void ActionOpenDRLResearchBookInHands()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE;
		m_Text = "#STR_STUDY_RECIPES";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
		{
			return false;
		}

		return item.IsKindOf("DRL_Craft_Research_Book");
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		GetGame().GetUIManager().EnterScriptedMenu(9503, null);
	}
}
