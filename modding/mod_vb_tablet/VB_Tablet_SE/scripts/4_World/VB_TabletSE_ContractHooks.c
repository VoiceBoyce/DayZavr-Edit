// Счётчики контрактов на ванильных действиях. Срабатывают только на сервере: файл лежит в серверном PBO.
modded class ActionFishingNew
{
	override protected EntityAI TrySpawnCatch(FishingActionData action_data)
	{
		EntityAI caught = super.TrySpawnCatch(action_data);
		if (caught)
			VB_TabletContracts.OnAction(action_data.m_Player, "fish");
		return caught;
	}
}

modded class ActionSkinning
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "skin");
	}
}

modded class ActionMineTree
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "wood");
	}
}

modded class ActionMineBush
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "bush");
	}
}

modded class ActionMineRock
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "rock");
	}
}

modded class ActionRepairVehiclePartBase
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "repair");
	}
}

modded class ActionRepairCarPart
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "repair");
	}
}

modded class ActionBuildPart
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		VB_TabletContracts.OnAction(action_data.m_Player, "build");
	}
}

modded class ActionLightItemOnFire
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		if (FireplaceBase.Cast(action_data.m_Target.GetObject()))
			VB_TabletContracts.OnAction(action_data.m_Player, "fire");
	}
}

modded class RecipeBase
{
	override void PerformRecipe(ItemBase item1, ItemBase item2, PlayerBase player)
	{
		super.PerformRecipe(item1, item2, player);
		VB_TabletContracts.OnAction(player, "craft");
	}
}

modded class ItemBase
{
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		VB_TabletContracts.OnItemMoved(oldLoc, newLoc);
	}
}
