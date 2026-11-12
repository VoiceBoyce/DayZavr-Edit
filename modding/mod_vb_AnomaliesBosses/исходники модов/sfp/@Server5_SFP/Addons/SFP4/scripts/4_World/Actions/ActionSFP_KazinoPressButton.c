class ActionSFP_KazinoPressButton : ActionInteractBase
{
	void ActionSFP_KazinoPressButton()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Нажать кнопку";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		if (!targetObject)
			return false;

		SFP_Kazino_terminal_01 kazino = SFP_Kazino_terminal_01.Cast(targetObject);
		if (!kazino)
			return false;

		if (kazino.IsBusy())
			return false;

		// ВАЖНО:
		// Конфиг казино находится в $profile и на КЛИЕНТЕ будет иметь свои (локальные) значения.
		// Поэтому нельзя фильтровать действие по AcceptItemClass на клиенте.
		// Иначе при смене предмета в СЕРВЕРНОМ JSON действие пропадёт.
		// Сервер всё равно проверит и спишет нужный предмет (см. TryPlayServer).
		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
		SFP_Kazino_terminal_01 kazino = SFP_Kazino_terminal_01.Cast(targetObject);

		if (kazino)
			kazino.TryPlayServer(action_data.m_Player);
	}
};
