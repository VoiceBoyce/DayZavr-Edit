class ActionChetkiCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(60);
	}
};

class ActionChetki: ActionContinuousBase
{
	/*void ActionPlayMyzGitara()
	{
		m_CallbackClass = ActionPlayGirataMUZCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING;	
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	*/
	void ActionChetki()
	{
		m_CallbackClass = ActionChetkiCB;
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
		return "#STR_ActionChetki";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_item_Chetki_01 horn = SFP_item_Chetki_01.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_item_Chetki_01 horn = SFP_item_Chetki_01.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};