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

// Каждая порция дров. Снятие коры (ActionMineTreeBark наследует это действие) не считается: у коры
// нет предела, и один ствол давал бы счётчик без конца.
modded class ActionMineTree
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		if (ActionMineTreeBark.Cast(this))
			return;
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

// Костёр засчитывается, только если он загорелся этим действием: не неудачная попытка
// и не факел, зажжённый от уже горящего костра.
modded class ActionLightItemOnFire
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		FireplaceBase fire = FireplaceBase.Cast(action_data.m_Target.GetObject());
		bool was = fire && fire.IsBurning();
		super.OnFinishProgressServer(action_data);
		if (fire && !was && fire.IsBurning())
			VB_TabletContracts.OnAction(action_data.m_Player, "fire");
	}
}

// Только рецепт, который действительно выполнился (ванильный PerformRecipe молча выходит, если проверка не прошла).
modded class RecipeBase
{
	override void PerformRecipe(ItemBase item1, ItemBase item2, PlayerBase player)
	{
		bool ok = CheckRecipe(item1, item2, player);
		super.PerformRecipe(item1, item2, player);
		if (ok)
			VB_TabletContracts.OnAction(player, "craft");
	}
}

// Действия из других модов (ActionGroups в конфиге): каждое завершение длительного действия.
// OnFinishProgress вызывается у всех ActionContinuousBase, моды обычно переопределяют только OnFinishProgressServer.
modded class ActionContinuousBase
{
	override void OnFinishProgress(ActionData action_data)
	{
		super.OnFinishProgress(action_data);
		if (GetGame().IsDedicatedServer())
			VB_TabletContracts.OnActionDone(this, action_data);
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
