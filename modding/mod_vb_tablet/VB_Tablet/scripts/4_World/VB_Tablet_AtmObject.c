// Банкомат клана в мире. Создаёт только сервер, от HouseNoDestruct: сломать и поднять нельзя.
class VB_TabletAtm extends BuildingBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionVB_TabletAtmOpen);
	}
}

// Сервер только показывает действие. Каждую операцию он проверяет отдельно: право, сумму, расстояние.
class ActionVB_TabletAtmOpen : ActionInteractBase
{
	void ActionVB_TabletAtmOpen()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_VB_TAB_C_ATM_OPEN";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(3.0);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !target)
			return false;
		VB_TabletAtm atm = VB_TabletAtm.Cast(target.GetObject());
		if (!atm)
			return false;
		if (GetGame().IsDedicatedServer())
			return true;
		// Чужой банкомат не открывается: на клиенте точка своего клана из шапки клана.
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c || !c.HasAtm)
			return false;
		vector at = atm.GetPosition();
		return Math.AbsFloat(at[0] - c.AtmX) < 1 && Math.AbsFloat(at[2] - c.AtmZ) < 1;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		VB_TabletClient.Get().AtmWanted = true;
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionVB_TabletAtmOpen);
	}
}
