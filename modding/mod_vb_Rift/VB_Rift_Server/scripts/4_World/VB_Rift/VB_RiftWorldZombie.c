// Лут с зомби Изнанки — разделкой (только сервер, VB_Rift_Server). Решение пользователя 2026-11-13:
// «резать зомби как мутантов, с шансом выпадает тот самый лут». Раньше лут сам падал в инвентарь трупа при смерти.
// Клиентская часть (VB_Rift, VB_RiftZombie.c) разрешает ванильное «Разделать» (ActionSkinning) на мёртвом зомби
// Изнанки: ZombieBase.CanBeSkinned. Ванильное действие на сервере помечает тело разделанным, зовёт SpawnItems
// и удаляет тело. Здесь SpawnItems для зомби Изнанки: вместо конфига Skinning (его у зомби нет) — свой лут
// (корень ROOT_CHANCE, споры SPORES_CHANCE, броски независимые) на землю у ног резавшего.
// Зомби волн фонаря-сердца и топливной капсулы (VB_RiftHorde с loot = false) при разделке ничего не дают.

modded class VB_RiftZombie
{
	static const string ROOT_TYPE = "VB_RiftRootPiece";
	static const float ROOT_CHANCE = 0.25;
	static const string SPORES_TYPE = "VB_RiftSpores";
	static const float SPORES_CHANCE = 0.05;

	// сервер: зомби Изнанки разделан игроком player
	static void VB_SkinServer(EntityAI body, PlayerBase player)
	{
		VB_DropBodyInventory(body);

		string who = VB_RiftItemsServer.VB_Who(player);
		if (!player)
			return;

		if (!VB_RiftHorde.VB_HasLoot(body))
		{
			Print("[VB_Rift] разделал зомби: " + who + " получил: ничего (зомби волны без лута)");
			return;
		}

		string got = "";
		EntityAI drop;
		float rollRoot = Math.RandomFloat01();
		float rollSpores = Math.RandomFloat01();
		if (rollRoot < ROOT_CHANCE)
		{
			drop = VB_RiftItemsServer.VB_DropAtFeet(player, ROOT_TYPE);
			if (drop)
				got = ROOT_TYPE;
		}
		if (rollSpores < SPORES_CHANCE)
		{
			drop = VB_RiftItemsServer.VB_DropAtFeet(player, SPORES_TYPE);
			if (drop)
			{
				if (got != "")
					got += ", ";
				got += SPORES_TYPE;
			}
		}
		if (got == "")
			got = "ничего";
		Print("[VB_Rift] разделал зомби: " + who + " получил: " + got);
	}

	// Ванильная разделка удаляет тело вместе с инвентарём. Если в трупе что-то лежит (положил игрок или
	// добавил чужой мод лута с зомби), выложить на землю — чтобы разделка ничего не съела.
	protected static void VB_DropBodyInventory(EntityAI body)
	{
		if (!body || !body.GetInventory())
			return;

		array<EntityAI> items = new array<EntityAI>;
		body.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);
		foreach (EntityAI item : items)
		{
			if (item && item != body && item.GetHierarchyParent() == body)
				body.GetInventory().DropEntity(InventoryMode.SERVER, body, item);
		}
	}
}

modded class ActionSkinning
{
	// Ванильная разделка (ActionSkinning.OnFinishProgressServer -> SpawnItems): у зомби Изнанки свой лут,
	// у всех остальных (животные, тела игроков, чужие мутанты) — как в ванили
	override void SpawnItems(ActionData action_data)
	{
		EntityAI body;
		if (action_data && action_data.m_Target)
			body = EntityAI.Cast(action_data.m_Target.GetObject());

		if (!body || !VB_RiftZombie.VB_Is(body))
		{
			super.SpawnItems(action_data);
			return;
		}

		VB_RiftZombie.VB_SkinServer(body, action_data.m_Player);
	}
}
