//==============================================================================
// Action: Toggle SFP_Alarm_Stolb (On/Off)
//==============================================================================

class ActionSFP_AlarmStolbToggle : ActionInteractBase
{
	void ActionSFP_AlarmStolbToggle()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "Включить";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;

		SFP_Alarm_Stolb pole = SFP_Alarm_Stolb.Cast(target.GetObject());
		return pole != NULL;
	}

	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		SFP_Alarm_Stolb pole = SFP_Alarm_Stolb.Cast(target.GetObject());
		if (pole && pole.IsAlarmOn())
			m_Text = "Выключить";
		else
			m_Text = "Включить";
	}

	override void OnExecuteServer(ActionData action_data)
	{
		SFP_Alarm_Stolb pole = SFP_Alarm_Stolb.Cast(action_data.m_Target.GetObject());
		if (pole)
			pole.ToggleAlarmServer();
	}
};
