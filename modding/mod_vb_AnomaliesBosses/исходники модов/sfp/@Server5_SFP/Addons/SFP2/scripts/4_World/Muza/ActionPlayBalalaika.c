class ActionPlayMyzBalalaikaCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(40);
	}
};

class ActionPlayMyzBalalaika: ActionContinuousBase
{
	void ActionPlayMyzBalalaika()
	{
		m_CallbackClass = ActionPlaySigaSmokeCB;
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
		return "#STR_PlayMyz";// сижу такой делаю что то, а кушать хочу.. бля шаву заебенить бы ммм. У меня тут на райончике фуфел один, в другой конец города не хочу  ехать.
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_BalalaikaMYZ horn = SFP_BalalaikaMYZ.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_BalalaikaMYZ horn = SFP_BalalaikaMYZ.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};