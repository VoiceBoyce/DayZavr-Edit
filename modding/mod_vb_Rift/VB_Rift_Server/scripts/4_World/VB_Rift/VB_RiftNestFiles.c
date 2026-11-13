// Файлы гнёзд Изнанки на сервере, $profile:VB_Mods\VB_Rift\ (формат прежний: файлы боевого сервера читаются как есть):
//   settings.json — сколько гнёзд ставить, доля корней, расстояние, прорастание, админы редактора
//   nests.json    — все гнёзда (пишет внутриигровой редактор; можно править руками при выключенном сервере
//                   или «Перебросить гнёзда» в редакторе — он перечитывает файлы с диска)
//   state.json    — какие гнёзда стояли на прошлом рестарте (чтобы не повторять подряд)
//   nests_backup.json — копия nests.json на каждом старте сервера, до любых правок
//   nests_tmp.json    — новая версия nests.json при записи; остаётся, только если запись прервалась
// Классы гнезда (VB_RiftNest, VB_RiftNestObj) лежат в клиентском аддоне VB_Rift: их же получает редактор.
// Лежит в 4_World, а не в 3_Game: VB_RiftNestFile ссылается на VB_RiftNest из 4_World клиента.

class VB_RiftNestFile
{
	int Version = 1;
	int NextId = 1;			// id объектов и гнёзд, общий счётчик
	int NextNumber = 1;		// номер в имени нового гнезда («Гнездо 7»)
	ref array<ref VB_RiftNest> Nests;
	ref array<ref VB_RiftNestObj> Decor;	// постоянный декор: щупальца вдали от сердец, стоят на каждом рестарте

	void VB_RiftNestFile()
	{
		Nests = new array<ref VB_RiftNest>;
		Decor = new array<ref VB_RiftNestObj>;
	}
}

class VB_RiftNestSettings
{
	ref TStringArray Admins;		// SteamID64 тех, кому можно открыть редактор (Ctrl+F8)
	int ActiveNests = 5;			// гнёзд с сердцем за рестарт
	float MinNestDistance = 300;	// м: выбранные гнёзда не ближе этого друг к другу
	int RootsPercentMin = 70;		// сколько процентов корней гнезда встаёт (случайно от Min до Max)
	int RootsPercentMax = 100;
	int NoRepeat = 1;				// 1 — не ставить подряд те же гнёзда, что на прошлом рестарте (если есть из чего выбрать)
	int RegrowMinutes = 0;			// через столько минут после вырезания сердца прорастает новое гнездо (0 — выключено)

	void VB_RiftNestSettings()
	{
		Admins = new TStringArray;
	}

	// Перед чтением файла: ключа нет в JSON — поле остаётся -1 и VB_Norm ставит значение по умолчанию.
	// Без этого отсутствующий ключ мог стать нулём (E57): «ActiveNests» 0 — ни одного гнезда.
	void VB_MarkUnset()
	{
		ActiveNests = -1;
		MinNestDistance = -1;
		RootsPercentMin = -1;
		RootsPercentMax = -1;
		NoRepeat = -1;
		RegrowMinutes = -1;
	}

	// значения по умолчанию для пропущенных ключей и пределы для ручных правок
	void VB_Norm()
	{
		if (!Admins)
			Admins = new TStringArray;
		if (ActiveNests < 0)
			ActiveNests = 5;
		if (ActiveNests > 500)
			ActiveNests = 500;
		if (!(MinNestDistance >= 0))		// так ловится и NaN
			MinNestDistance = 300;
		if (MinNestDistance > 20000)
			MinNestDistance = 20000;
		if (RootsPercentMin < 0)
			RootsPercentMin = 70;
		if (RootsPercentMax < 0)
			RootsPercentMax = 100;
		if (RootsPercentMin > 100)
			RootsPercentMin = 100;
		if (RootsPercentMax > 100)
			RootsPercentMax = 100;
		if (RootsPercentMax < RootsPercentMin)
			RootsPercentMax = RootsPercentMin;
		if (NoRepeat < 0)
			NoRepeat = 1;
		if (RegrowMinutes < 0)
			RegrowMinutes = 0;
		if (RegrowMinutes > 1440)
			RegrowMinutes = 1440;
	}
}

class VB_RiftNestState
{
	ref TIntArray LastActive;

	void VB_RiftNestState()
	{
		LastActive = new TIntArray;
	}
}

// Сетка сердец для «ближайшее сердце не дальше радиуса»: клетка размером с радиус гнезда, смотрим 3×3 клетки.
// VB_Regroup раньше для каждого корня перебирал все гнёзда и в каждом искал сердце через IsKindOf
// (на 16: ~800 объектов × 73 гнезда ≈ 58 тыс. IsKindOf) — рывок сервера на каждой правке сердца (аудит №38).
// Живёт только внутри одного прохода: держит гнёзда без ref, их держит VB_RiftNestFile.
class VB_RiftNestHeartIndex
{
	static const float CELL = 60.0;			// = VB_RiftNestNet.NEST_RADIUS

	protected ref map<int, ref TIntArray> m_VB_Cells;
	protected ref array<vector> m_VB_Pos;
	protected ref array<VB_RiftNest> m_VB_Nests;

	void VB_RiftNestHeartIndex()
	{
		m_VB_Cells = new map<int, ref TIntArray>;
		m_VB_Pos = new array<vector>;
		m_VB_Nests = new array<VB_RiftNest>;
	}

	protected static int VB_Key(int cx, int cz)
	{
		return cx * 10000 + cz;
	}

	// все гнёзда с сердцем в нормальных координатах (VB_RiftNests.VB_PosOk: только 0..20000, ключи клеток не пересекаются)
	void VB_Build(array<ref VB_RiftNest> nests)
	{
		VB_RiftNestObj heart;
		int cx;
		int cz;
		int key;
		TIntArray cell;
		foreach (VB_RiftNest nest : nests)
		{
			heart = nest.VB_FindHeart();
			if (!heart || !VB_RiftNests.VB_PosOk(heart.Position))
				continue;

			cx = Math.Floor(heart.Position[0] / CELL);
			cz = Math.Floor(heart.Position[2] / CELL);
			key = VB_Key(cx, cz);
			cell = m_VB_Cells.Get(key);
			if (!cell)
			{
				cell = new TIntArray;
				m_VB_Cells.Set(key, cell);
			}
			cell.Insert(m_VB_Pos.Count());
			m_VB_Pos.Insert(heart.Position);
			m_VB_Nests.Insert(nest);
		}
	}

	// гнездо, чьё сердце ближе всех к pos и не дальше maxDist (maxDist не больше CELL); null — нет
	VB_RiftNest VB_Nearest(vector pos, float maxDist)
	{
		if (!VB_RiftNests.VB_PosOk(pos))
			return null;

		int cx = Math.Floor(pos[0] / CELL);
		int cz = Math.Floor(pos[2] / CELL);
		VB_RiftNest best;
		float bestD = maxDist;
		float d;
		int key;
		TIntArray cell;
		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dz = -1; dz <= 1; dz++)
			{
				key = VB_Key(cx + dx, cz + dz);
				cell = m_VB_Cells.Get(key);
				if (!cell)
					continue;
				foreach (int i : cell)
				{
					d = VB_RiftNestUtil.VB_DistXZ(m_VB_Pos[i], pos);
					if (d < bestD)
					{
						bestD = d;
						best = m_VB_Nests[i];
					}
				}
			}
		}
		return best;
	}
}
