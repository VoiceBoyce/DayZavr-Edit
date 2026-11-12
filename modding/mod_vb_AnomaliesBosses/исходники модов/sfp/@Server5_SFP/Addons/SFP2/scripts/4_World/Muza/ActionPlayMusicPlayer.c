class ActionPlayMusicMUZCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(30);
	}
};

class ActionPlayMyzMusicPlayer: ActionContinuousBase
{
	
	void ActionPlayMyzMusicPlayer()
	{
		m_CallbackClass = ActionPlayMusicMUZCB;
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
		return "Play";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_MusicPlayerKidsMYZ horn = SFP_MusicPlayerKidsMYZ.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_MusicPlayerKidsMYZ horn = SFP_MusicPlayerKidsMYZ.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};