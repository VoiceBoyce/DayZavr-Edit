// Общие проверки для гнёзд Изнанки: тип объекта гнезда и расстояние по земле. Нужны и клиенту (редактор, «мир»),
// и серверу (VB_AncientRoot_Server). Лежит в 3_Game: g_Game.IsKindOf и ConfigGetInt доступны уже здесь.
//
// IsKindOf дорогой (полный путь класса в конфиге и сравнение строк), а гнёзд сотни объектов, поэтому вид типа
// считается один раз и кэшируется (аудит 2026-11-13, №38).

class VB_AR_NestUtil
{
	static const int KIND_HEART = 1;		// сердце (VB_AR_Heart и наследники)
	static const int KIND_HANG = 2;			// щупальца (VB_AR_HangBase)
	static const int KIND_ROOTS = 4;		// корни на земле (VB_AR_Roots_*)
	static const int KIND_NEST = 8;			// можно ставить в гнездо: одно из трёх выше и scope 2 (не абстрактная база)
	static const int CACHE_MAX = 512;		// типов больше не бывает; если набралось — кто-то шлёт мусор, кэш сбрасываем

	protected static ref map<string, int> s_VB_Kinds;

	// вид типа: набор флагов KIND_*; 0 — не объект гнезда (обвивки, корни на домах, чужие классы, опечатки)
	static int VB_Kind(string type)
	{
		if (!s_VB_Kinds)
			s_VB_Kinds = new map<string, int>;

		int kind;
		if (s_VB_Kinds.Find(type, kind))
			return kind;

		kind = 0;
		if (type != "" && g_Game && g_Game.ConfigIsExisting("CfgVehicles " + type))
		{
			bool heart = g_Game.IsKindOf(type, "VB_AR_Heart");
			bool hang = g_Game.IsKindOf(type, "VB_AR_HangBase");
			bool grove = g_Game.IsKindOf(type, "VB_AR_Base");
			if (heart)
				kind = kind | KIND_HEART;
			if (hang)
				kind = kind | KIND_HANG;
			// корни гнезда — VB_AR_Roots_*; обвивки деревьев (Wrap) и корни на домах (Wall) — локальные, не для гнёзд
			if (grove && type.IndexOf("VB_AR_Roots_") == 0)
				kind = kind | KIND_ROOTS;

			int scope = g_Game.ConfigGetInt("CfgVehicles " + type + " scope");
			if (scope == 2 && kind != 0)
				kind = kind | KIND_NEST;
		}

		if (s_VB_Kinds.Count() >= CACHE_MAX)
			s_VB_Kinds.Clear();
		s_VB_Kinds.Set(type, kind);
		return kind;
	}

	static bool VB_IsHeartType(string type)
	{
		int kind = VB_Kind(type);
		return (kind & KIND_HEART) != 0;
	}

	static bool VB_IsHangType(string type)
	{
		int kind = VB_Kind(type);
		return (kind & KIND_HANG) != 0;
	}

	// тип из списка редактора: сердце, корни VB_AR_Roots_*, щупальца; scope 2
	static bool VB_IsNestType(string type)
	{
		int kind = VB_Kind(type);
		return (kind & KIND_NEST) != 0;
	}

	// расстояние по земле (без высоты)
	static float VB_DistXZ(vector a, vector b)
	{
		float dx = a[0] - b[0];
		float dz = a[2] - b[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}
}
