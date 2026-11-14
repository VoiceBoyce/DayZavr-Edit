// INT Quests (server) — the only vanilla hooks of the server part (spec.md §4.3). Each one calls super first and
// returns after one int compare when nobody needs that event type. Kills come from INT_QST_SE_Hub (3_Game).
//   RecipeBase.SpawnItems            craft results, once per successful craft (no vanilla recipe overrides it)
//   ActionFishingNew.TrySpawnCatch   the caught entity, server only (old ActionFishing is commented out in 1.30)
//   PlantBase.Harvest                garden harvest (plant and crop class read before super)
//   ActionContinuousBase.OnFinishProgress  "do action X" (old mod's hook; once per cycle for repeating actions)

modded class RecipeBase
{
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		super.SpawnItems(ingredients, player, spawned_objects);
		if (!player || !spawned_objects || !INT_QST_SE_Players.Needs(INT_QST_TT.CRAFT))
			return;
		foreach (ItemBase it : spawned_objects)
		{
			if (it)
				INT_QST_SE_Progress.OnSimple(player, INT_QST_TT.CRAFT, it.GetType());
		}
	}
}

modded class ActionFishingNew
{
	override protected EntityAI TrySpawnCatch(FishingActionData action_data)
	{
		EntityAI c = super.TrySpawnCatch(action_data);
		if (c && action_data && action_data.m_Player && INT_QST_SE_Players.Needs(INT_QST_TT.FISH))
			INT_QST_SE_Progress.OnSimple(action_data.m_Player, INT_QST_TT.FISH, c.GetType());
		return c;
	}
}

modded class PlantBase
{
	override void Harvest(PlayerBase player)
	{
		string crop = "";
		bool counted = player && INT_QST_SE_Players.Needs(INT_QST_TT.HARVEST) && IsHarvestable();
		if (counted)
			crop = GetCropsType();
		string plant = GetType();
		super.Harvest(player);
		if (counted)
			INT_QST_SE_Progress.OnHarvest(player, plant, crop);
	}
}

modded class ActionContinuousBase
{
	override void OnFinishProgress(ActionData action_data)
	{
		super.OnFinishProgress(action_data);
		if (!GetGame().IsDedicatedServer() || !INT_QST_SE_Players.Needs(INT_QST_TT.ACTION))
			return;
		if (!action_data || !action_data.m_Player)
			return;
		string targetType = "";
		if (action_data.m_Target && action_data.m_Target.GetObject())
			targetType = action_data.m_Target.GetObject().GetType();
		INT_QST_SE_Progress.OnAction(action_data.m_Player, this, targetType);
	}
}
