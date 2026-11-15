// Файлы гнёзд Изнанки на сервере, $profile:VB_Mods\VB_AncientRoot\ (формат прежний: файлы боевого сервера читаются как есть):
//   settings.json — сколько гнёзд ставить, доля корней, расстояние, прорастание, админы редактора
//   nests.json    — все гнёзда (пишет внутриигровой редактор; можно править руками при выключенном сервере
//                   или «Перебросить гнёзда» в редакторе — он перечитывает файлы с диска)
//   state.json    — какие гнёзда стояли на прошлом рестарте (чтобы не повторять подряд)
//   nests_backup.json — копия nests.json на каждом старте сервера, до любых правок
//   nests_tmp.json    — новая версия nests.json при записи; остаётся, только если запись прервалась
// Классы гнезда (VB_AR_Nest, VB_AR_NestObj) лежат в клиентском аддоне VB_AncientRoot: их же получает редактор.
// Лежит в 4_World, а не в 3_Game: VB_AR_NestFile ссылается на VB_AR_Nest из 4_World клиента.

class VB_AR_NestFile
{
	int Version = 1;
	int NextId = 1;			// id объектов и гнёзд, общий счётчик
	int NextNumber = 1;		// номер в имени нового гнезда («Гнездо 7»)
	ref array<ref VB_AR_Nest> Nests;
	ref array<ref VB_AR_NestObj> Decor;	// постоянный декор: щупальца вдали от сердец, стоят на каждом рестарте

	void VB_AR_NestFile()
	{
		Nests = new array<ref VB_AR_Nest>;
		Decor = new array<ref VB_AR_NestObj>;
	}
}

class VB_AR_NestSettings
{
	ref TStringArray Admins;		// SteamID64 тех, кому можно открыть редактор (команда /grove)
	int ActiveNests = 5;			// гнёзд с сердцем за рестарт
	float MinNestDistance = 300;	// м: выбранные гнёзда не ближе этого друг к другу
	int RootsPercentMin = 70;		// сколько процентов корней гнезда встаёт (случайно от Min до Max)
	int RootsPercentMax = 100;
	int NoRepeat = 1;				// 1 — не ставить подряд те же гнёзда, что на прошлом рестарте (если есть из чего выбрать)
	int RegrowMinutes = 0;			// через столько минут после вырезания сердца прорастает новое гнездо (0 — выключено)

	void VB_AR_NestSettings()
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

class VB_AR_NestState
{
	ref TIntArray LastActive;
	ref TIntArray Seeded;	// гнёзда, где посадили Семя Древнего корня: на следующем рестарте встают первыми

	void VB_AR_NestState()
	{
		LastActive = new TIntArray;
		Seeded = new TIntArray;
	}
}

// Сетка сердец для «ближайшее сердце не дальше радиуса»: клетка размером с радиус гнезда, смотрим 3×3 клетки.
// VB_Regroup раньше для каждого корня перебирал все гнёзда и в каждом искал сердце через IsKindOf
// (на 16: ~800 объектов × 73 гнезда ≈ 58 тыс. IsKindOf) — рывок сервера на каждой правке сердца (аудит №38).
// Живёт только внутри одного прохода: держит гнёзда без ref, их держит VB_AR_NestFile.
class VB_AR_NestHeartIndex
{
	static const float CELL = 60.0;			// = VB_AR_NestNet.NEST_RADIUS

	protected ref map<int, ref TIntArray> m_VB_Cells;
	protected ref array<vector> m_VB_Pos;
	protected ref array<VB_AR_Nest> m_VB_Nests;

	void VB_AR_NestHeartIndex()
	{
		m_VB_Cells = new map<int, ref TIntArray>;
		m_VB_Pos = new array<vector>;
		m_VB_Nests = new array<VB_AR_Nest>;
	}

	protected static int VB_Key(int cx, int cz)
	{
		return cx * 10000 + cz;
	}

	// все гнёзда с сердцем в нормальных координатах (VB_AR_Nests.VB_PosOk: только 0..20000, ключи клеток не пересекаются)
	void VB_Build(array<ref VB_AR_Nest> nests)
	{
		VB_AR_NestObj heart;
		int cx;
		int cz;
		int key;
		TIntArray cell;
		foreach (VB_AR_Nest nest : nests)
		{
			heart = nest.VB_FindHeart();
			if (!heart || !VB_AR_Nests.VB_PosOk(heart.Position))
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
	VB_AR_Nest VB_Nearest(vector pos, float maxDist)
	{
		if (!VB_AR_Nests.VB_PosOk(pos))
			return null;

		int cx = Math.Floor(pos[0] / CELL);
		int cz = Math.Floor(pos[2] / CELL);
		VB_AR_Nest best;
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
					d = VB_AR_NestUtil.VB_DistXZ(m_VB_Pos[i], pos);
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
