// Файлы гнёзд Изнанки на сервере, $profile:VB_Mods\VB_AncientRoot\ (формат прежний: файлы боевого сервера читаются как есть):
//   settings.json — сколько гнёзд ставить, доля корней, расстояние, прорастание, админы редактора
//   nests.json    — все гнёзда (пишет внутриигровой редактор; можно править руками при выключенном сервере
//                   или «Перебросить гнёзда» в редакторе — он перечитывает файлы с диска)
//   state.json    — живые рощи со ступенями (Groves, ТЗ 2б.4), гнёзда прошлого рестарта, засеянные Семенем,
//                   начало и «жив до» текущего запуска (длина запуска — для роста)
//   nests_backup.json — копия nests.json на старте сервера, до любых правок (файл без гнёзд её не затирает);
//   nests_backup2.json — прежняя отличающаяся копия
//   nests_tmp.json, state_tmp.json, settings_tmp.json — новая версия при записи; остаётся, только если запись прервалась
//   state_backup.json — копия state.json удачного старта; *_broken.json — файл с ошибкой, сохранённый до перезаписи
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
	// рост рощ (ТЗ 2б.4, шаг 5): ActiveNests — сколько живых рощ держать на карте
	int GrowRestarts = 2;			// очков роста на ступень (очко — пережитый невырезанным рестарт)
	int FeedOffers = 2;				// приняла за рестарт столько подношений — ещё +1 очко (росток — свой единственный)
	int AncientLife = 4;			// древняя живёт столько рестартов и уходит в землю
	int MaxAncient = 1;				// древних одновременно не больше
	int GrowMinSessionMinutes = 60;	// запуск короче — не рестарт: рост и старение не считаются (падения подряд); 0 — всегда
	// «Защита рощи» — нашествие пауков (ТЗ 2в п.1, VB_AR_WorldSiege.c)
	int SiegeCheckMinutes = 10;		// как часто ночью бросать жребий
	float SiegeChance = 0.15;		// шанс нашествия на проверке (0 — выключено)
	int SiegeCooldownMinutes = 120;	// на одну рощу не чаще
	float SiegePlayerDistance = 200;	// м: нападают только на рощу, у которой есть игрок

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
		GrowRestarts = -1;
		FeedOffers = -1;
		AncientLife = -1;
		MaxAncient = -1;
		GrowMinSessionMinutes = -1;
		SiegeCheckMinutes = -1;
		SiegeChance = -1;
		SiegeCooldownMinutes = -1;
		SiegePlayerDistance = -1;
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
		if (GrowRestarts < 1)
			GrowRestarts = 2;
		if (GrowRestarts > 100)
			GrowRestarts = 100;
		if (FeedOffers < 1)
			FeedOffers = 2;
		if (AncientLife < 1)
			AncientLife = 4;
		if (AncientLife > 1000)
			AncientLife = 1000;
		if (MaxAncient < 0)
			MaxAncient = 1;
		if (GrowMinSessionMinutes < 0)
			GrowMinSessionMinutes = 60;
		if (GrowMinSessionMinutes > 1440)
			GrowMinSessionMinutes = 1440;
		if (SiegeCheckMinutes < 1)
			SiegeCheckMinutes = 10;
		if (SiegeCheckMinutes > 1440)
			SiegeCheckMinutes = 1440;
		if (!(SiegeChance >= 0))		// -1 (ключа нет) и NaN
			SiegeChance = 0.15;
		if (SiegeChance > 1)
			SiegeChance = 1;
		if (SiegeCooldownMinutes < 0)
			SiegeCooldownMinutes = 120;
		if (SiegeCooldownMinutes > 10080)
			SiegeCooldownMinutes = 10080;		// неделя: больше — переполнение мс в int (WorldSiege)
		if (!(SiegePlayerDistance >= 0))
			SiegePlayerDistance = 200;
		if (SiegePlayerDistance > 5000)
			SiegePlayerDistance = 5000;
	}
}

// Живая роща (ТЗ 2б.4): гнездо и его ступень. Offers и Disturbed — за текущий рестарт, по ним на следующем старте
// считается рост (VB_AR_Nests.VB_AdvanceGroves), потом обнуляются. Ключа нет в JSON — поле 0 (E57): ступень
// приводится к 1..3 при загрузке.
class VB_AR_Grove
{
	int Nest;			// id гнезда в nests.json
	int Stage = 1;		// 1 росток, 2 роща, 3 древняя
	int Growth;			// очки роста на текущей ступени
	int Age;			// сколько рестартов прожила древней (−1 — продлила век, VB_GroveDefended)
	int Offers;			// подношений за текущий рестарт
	int Disturbed;		// 1 — в этот рестарт сбором подняли волну (роста не будет)
	int Extended;		// 1 — древняя уже раз продлила век (ритуал роста, нашествие): второй раз нельзя
}

class VB_AR_NestState
{
	int StartMin;		// минута UTC (от 1970) старта текущего запуска
	int AliveMin;		// и последняя отметка «жив» (раз в 5 мин): разница — длина запуска
	ref TIntArray LastActive;
	ref TIntArray Seeded;	// гнёзда, где посадили Семя Древнего корня: на следующем рестарте встают первыми (ростком)
	ref array<ref VB_AR_Grove> Groves;	// живые рощи

	void VB_AR_NestState()
	{
		LastActive = new TIntArray;
		Seeded = new TIntArray;
		Groves = new array<ref VB_AR_Grove>;
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
