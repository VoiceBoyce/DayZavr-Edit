class ActionTurnOffLamp : ActionInteractBase
{
	void ActionTurnOffLamp()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#switch_off";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		DRL_Craft_Lamp lamp = DRL_Craft_Lamp.Cast(target.GetObject());
		if (!lamp) return false;

		if (!lamp.HasEnergyManager()) return false;

		if (lamp.GetCompEM().CanSwitchOff())
		{
			return true;
		}

		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
		if (!targetObject) return;

		DRL_Craft_Lamp lamp = DRL_Craft_Lamp.Cast(targetObject);
		if (!lamp) return;

		if (!lamp.HasEnergyManager()) return;

		lamp.GetCompEM().SwitchOff();
		lamp.GetCompEM().InteractBranch(lamp);
	}
};