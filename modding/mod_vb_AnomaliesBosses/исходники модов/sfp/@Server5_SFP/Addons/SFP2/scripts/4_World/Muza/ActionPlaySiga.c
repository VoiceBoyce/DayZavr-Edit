/*class ActionPlaySigaSmokeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(20);
	}
};

class ActionPlaySigaSmoke: ActionContinuousBase
{
	
	void ActionPlaySigaSmoke()
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
		return "Сделать Тягу";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
		{
            horn.BlowHorn();
			horn.ShowSmoke();
		}
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
		{
            horn.StopBlowingHorn();
			horn.HideSmoke();
		}
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};
*/
class ActionPlaySigaSmokeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(60);
	}
};

class ActionPlaySigaSmoke: ActionContinuousBase
{
	/*void ActionPlayMyzGitara()
	{
		m_CallbackClass = ActionPlayGirataMUZCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING;	
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	*/
	void ActionPlaySigaSmoke()
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
		return "#STR_VapeTaga";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};
/*
class ActionPlaySigaSmokeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(30);
	}
};

class ActionPlaySigaSmoke: ActionContinuousBase
{
	
	void ActionPlaySigaSmoke()
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
		return "Сделать Тягу";
	}

	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);

        if(horn)
		{
            horn.BlowHorn();
		}
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);

        if(horn)
		{
            horn.StopBlowingHorn();
		}
	}

	override void OnStartAnimationLoopClient( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);

		if(horn)
		{
			horn.ShowSmoke();
		}
	}

	override void OnEndAnimationLoopClient( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);

		if(horn)
		{
			horn.HideSmoke();
		}
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}
};
*/