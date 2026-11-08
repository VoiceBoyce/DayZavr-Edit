// ATM objects and the "open" action.
// MF_BankObjectBase keeps the script class name of the old MF Banking mod. In config.cpp every placeable
// ATM derives DIRECTLY from HouseNoDestruct (VPP Object Builder lists only such classes), so the bank logic
// is attached here: one script class per config class name, all extending MF_BankObjectBase.

class MF_BankObjectBase extends BuildingBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionINT_BankOpen);
	}
}

class ActionINT_BankOpen : ActionInteractBase
{
	void ActionINT_BankOpen()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_INT_BANK_ACTION_OPEN";
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
		Object obj = target.GetObject();
		if (!obj)
			return false;
		// script class of the target (config classes derive from HouseNoDestruct directly)
		return MF_BankObjectBase.Cast(obj) != null;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		INT_Bank_Client.Get().RequestOpen();
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionINT_BankOpen);
	}
}

// ---- script classes of the placeable ATMs (names = config.cpp CfgVehicles, keep both lists in sync) ----
class MF_BankObjectTest extends MF_BankObjectBase {}
class MF_BankObject_Test extends MF_BankObjectBase {}
class DC_BankingATM extends MF_BankObjectBase {}
class DC_BankingATMGreen extends MF_BankObjectBase {}
class DC_BankingATMBlue extends MF_BankObjectBase {}
class DC_BankingATMRed extends MF_BankObjectBase {}
class DC_BankingATMOrange extends MF_BankObjectBase {}
class DC_BankingATMYellow extends MF_BankObjectBase {}
class DC_BankingATMPurple extends MF_BankObjectBase {}
class DC_BankingATMDarkGreen extends MF_BankObjectBase {}
class DC_BankingATMDarkBlue extends MF_BankObjectBase {}
class DC_BankingATMWhite extends MF_BankObjectBase {}
class DC_BankingATMGray extends MF_BankObjectBase {}
class BankingATM extends MF_BankObjectBase {}
class BankingATMGreen extends MF_BankObjectBase {}
class BankingATMBlue extends MF_BankObjectBase {}
class BankingATMRed extends MF_BankObjectBase {}
class BankingATMOrange extends MF_BankObjectBase {}
class BankingATMYellow extends MF_BankObjectBase {}
class BankingATMPurple extends MF_BankObjectBase {}
class BankingATMDarkGreen extends MF_BankObjectBase {}
class BankingATMDarkBlue extends MF_BankObjectBase {}
class BankingATMWhite extends MF_BankObjectBase {}
class BankingATMGray extends MF_BankObjectBase {}
class INT_BankingATM extends MF_BankObjectBase {}
class INT_BankingATMGreen extends MF_BankObjectBase {}
class INT_BankingATMBlue extends MF_BankObjectBase {}
class INT_BankingATMRed extends MF_BankObjectBase {}
class INT_BankingATMOrange extends MF_BankObjectBase {}
class INT_BankingATMYellow extends MF_BankObjectBase {}
class INT_BankingATMPurple extends MF_BankObjectBase {}
class INT_BankingATMDarkGreen extends MF_BankObjectBase {}
class INT_BankingATMDarkBlue extends MF_BankObjectBase {}
class INT_BankingATMWhite extends MF_BankObjectBase {}
class INT_BankingATMGray extends MF_BankObjectBase {}
class DC_BankingLocker extends MF_BankObjectBase {}
class BankingLocker extends MF_BankObjectBase {}
class INT_BankingLocker extends MF_BankObjectBase {}
class sm_atm_DayZilla extends MF_BankObjectBase {}
class INT_atm_DayZilla extends MF_BankObjectBase {}
class sm_atm_DayZavr extends MF_BankObjectBase {}
class INT_atm_DayZavr extends MF_BankObjectBase {}
