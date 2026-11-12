class ActionSFP_ScannerMutantBurnKillAll : ActionInteractBase
{
	void ActionSFP_ScannerMutantBurnKillAll()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text = "Включить выжигатель";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;

		SFP_Item_scanner_mutantType2 scanner;
		if (!Class.CastTo(scanner, target.GetObject()))
			return false;

		// Only on ground (like other interactions with the device)
		InventoryLocation loc = new InventoryLocation;
		if (scanner.GetInventory().GetCurrentInventoryLocation(loc) && loc.GetType() != InventoryLocationType.GROUND)
			return false;

		return scanner.SFP_IsBurnReady();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		SFP_Item_scanner_mutantType2 scanner;
		if (!Class.CastTo(scanner, action_data.m_Target.GetObject()))
			return;

		scanner.SFP_DoBurnKillAll();
	}
};
