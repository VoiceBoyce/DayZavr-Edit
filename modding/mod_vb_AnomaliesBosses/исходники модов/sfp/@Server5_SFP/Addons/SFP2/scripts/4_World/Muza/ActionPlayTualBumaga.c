/*class ActionTualBumagaCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(40);
	}
};

class ActionTualBumaga: ActionContinuousBase
{
	/*void ActionPlayMyzGitara()
	{
		m_CallbackClass = ActionPlayGirataMUZCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING;	
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	*/
	/*void ActionTualBumaga()
	{
		m_CallbackClass = ActionTualBumagaCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF;	
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
	}
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
		
	override string GetText()
	{
		return "Использовать по назначению";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_TualBumaga horn = SFP_TualBumaga.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_TualBumaga horn = SFP_TualBumaga.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};