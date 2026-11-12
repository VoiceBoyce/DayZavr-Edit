/**
 * Pack_Raspack
 * Универсальная логика распаковки для заданных типов упаковок.
 */
class PackRaspackHelper
{
	static const float SPAWN_RADIUS = 0.35;

	// Возвращает тип предмета и количество, которое должно выпасть из упаковки
	static bool GetPackContent(string packType, out string itemType, out int count)
	{
		itemType = string.Empty;
		count = 0;

		switch (packType)
		{
			case "SFP_VodkaPack12":
				itemType = "GlassBottle";
				count = 12;
				return true;

			case "SFP_VodkaSFPPack12":
				itemType = "SFP_Vodka_Pseno";
				count = 12;
				return true;

			case "SFP_CanTunaPack15":
				itemType = "TunaCan";
				count = 15;
				return true;

			case "SFP_PipsiPack12":
				itemType = "SodaCan_Pipsi";
				count = 12;
				return true;

			case "SFP_CanPack6":
				itemType = "BakedBeansCan";
				count = 6;
				return true;

			case "SFP_BootleWaterPack6":
				itemType = "WaterBottle";
				count = 6;
				return true;

			case "SFP_BootleWaterPack12":
				itemType = "WaterBottle";
				count = 12;
				return true;
		}

		return false;
	}

	static bool IsSupportedPack(ItemBase pack)
	{
		if (!pack)
			return false;

		string dummyType;
		int dummyCount;
		return GetPackContent(pack.GetType(), dummyType, dummyCount);
	}

	static vector GetRandomSpawnPos(vector center)
	{
		float rx = Math.RandomFloatInclusive(-SPAWN_RADIUS, SPAWN_RADIUS);
		float rz = Math.RandomFloatInclusive(-SPAWN_RADIUS, SPAWN_RADIUS);
		return center + Vector(rx, 0, rz);
	}

		// Возвращает override liquidType для конкретной упаковки. -1 если override не нужен.
	static int GetLiquidOverride(string packType)
	{
		switch (packType)
		{
			// Кастомная жидкость для SFP_VodkaSFPPack12
			case "SFP_VodkaSFPPack12":
				return 33554432; // id кастомной жидкости
		}
		return -1;
	}

	// Заполняет предмет (если у него есть quantity). Для жидкостей:
	// - если есть override для данной упаковки — ставим его
	// - иначе не ломаем дефолтный liquidType из конфига (например, газировка),
	//   но если тип не задан — считаем что это вода
	static void FillSpawnedItemForPack(ItemBase spawned, string packType)
	{
		if (!spawned)
			return;

		if (spawned.IsLiquidContainer())
		{
			int forced = GetLiquidOverride(packType);
			if (forced != -1)
			{
				spawned.SetLiquidType(forced);
			}
			else
			{
				if (spawned.GetLiquidType() == LIQUID_NONE)
					spawned.SetLiquidType(LIQUID_WATER);
			}
		}

		if (spawned.HasQuantity())
			spawned.SetQuantityMax();
	}

	// Backward-compatible: старый вызов без packType
	static void FillSpawnedItem(ItemBase spawned)
	{
		FillSpawnedItemForPack(spawned, string.Empty);
	}
}

