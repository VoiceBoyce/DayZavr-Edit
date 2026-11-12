class ActionSFP_ScannerMutantToggleMode : ActionInteractBase
{
	void ActionSFP_ScannerMutantToggleMode()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text = "Сменить режим";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;

		SFP_Item_scanner_mutantType2 scanner;
		if (!Class.CastTo(scanner, target.GetObject()))
			return false;

		// Must be working to switch repel mode
		if (!scanner.GetCompEM() || !scanner.GetCompEM().IsWorking())
			return false;

		// Only on ground
		InventoryLocation loc = new InventoryLocation;
		if (scanner.GetInventory().GetCurrentInventoryLocation(loc) && loc.GetType() != InventoryLocationType.GROUND)
			return false;

		// Dynamic text
		if (scanner.SFP_GetMode() == SFP_Item_scanner_mutantType2.SFP_MODE_STANDARD)
			m_Text = "Усиленный режим";
		else
			m_Text = "Стандартный режим";

		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		SFP_Item_scanner_mutantType2 scanner;
		if (!Class.CastTo(scanner, action_data.m_Target.GetObject()))
			return;

		if (scanner.SFP_GetMode() == SFP_Item_scanner_mutantType2.SFP_MODE_STANDARD)
			scanner.SFP_SetMode(SFP_Item_scanner_mutantType2.SFP_MODE_BOOST);
		else
			scanner.SFP_SetMode(SFP_Item_scanner_mutantType2.SFP_MODE_STANDARD);
	}
};
