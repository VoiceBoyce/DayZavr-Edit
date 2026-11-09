// Крафт DRL_CraftMenu в контрактах планшета (сервер):
//   - завершённый крафт засчитывается тому, кто его запустил, в группы CraftGroups (config/contracts.json планшета);
//   - всё, что создаёт крафт (результат, возврат при разборке), помечается как «не находка»: такой предмет,
//     забранный из верстака или с земли, не идёт в ItemGroups («найти артефакт»).
// Если игрок вышел до конца крафта, результат ложится в верстак, а в контракт не идёт.

modded class DRL_CraftProcessManager
{
	override protected bool ExecuteProcessCompletion(PlayerBase player, EntityAI wb, DRL_Craft_Recipe r, DRL_CraftProcessData data)
	{
		bool ok = super.ExecuteProcessCompletion(player, wb, r, data);
		if (ok && player && r && data)
		{
			string cls = data.m_ResultClass;
			if (cls == "")
				cls = r.Result;
			VB_TabletContracts.OnCrafted(player, cls, Math.Max(1, data.m_CraftCount));
		}
		return ok;
	}
}

// Старый путь без очереди процессов (TryCraftRecipe): результат один.
modded class PlayerBase
{
	override void DRL_CompleteCraft(DRL_Craft_Recipe r)
	{
		super.DRL_CompleteCraft(r);
		if (r)
			VB_TabletContracts.OnCrafted(this, r.Result, 1);
	}

	override protected EntityAI DRL_SpawnCraftResult(string cls, bool groundOnly = false, vector customSpawnPos = "0 0 0")
	{
		EntityAI e = super.DRL_SpawnCraftResult(cls, groundOnly, customSpawnPos);
		ItemBase ib = ItemBase.Cast(e);
		if (ib)
			ib.VbMarkTaken();
		return e;
	}
}

modded class DRL_Craft_WB
{
	override EntityAI DRL_CreateCraftResult(string resultCls, PlayerBase player, bool groundOnly, vector customSpawnPos = "0 0 0")
	{
		EntityAI e = super.DRL_CreateCraftResult(resultCls, player, groundOnly, customSpawnPos);
		ItemBase ib = ItemBase.Cast(e);
		if (ib)
			ib.VbMarkTaken();
		return e;
	}
}
