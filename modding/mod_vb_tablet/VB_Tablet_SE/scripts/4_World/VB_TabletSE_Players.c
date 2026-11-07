// Данные игрока: один файл на игрока, $profile:VB_Mods/VB_Tablet/players/<steam64>.json.
// Рядом лежит копия .bak: если запись оборвалась, читается она.
class VB_TabletRecord
{
	string PlayerName;
	string Discord;
	int Zombies;
	int Mutants;
	int Animals;
	int DistM;
	int RecordSec;
	int Days;
	int LastDay;
	int Contracts;
	int ExtraXp;
	int AchBoss;
	int AchCar;
	int AchGather;
	int AchFish;
	int AchCraft;
	int AchBuild;
	int AchTrophy;
	int AchCard;
	string AppliedClan;
	int MyDay;			// день, на который выдан набор личных контрактов
	int MyReplaceDay;	// день, когда истрачена бесплатная замена
	ref array<ref VB_ClanContract> MyContracts = new array<ref VB_ClanContract>();

	float CarLeft;		// метры поездки до целого километра
	[NonSerialized()]
	bool CarKnown;
	[NonSerialized()]
	vector CarAt;
	[NonSerialized()]
	string Uid;
	[NonSerialized()]
	bool Dirty;
	[NonSerialized()]
	float LastDist = -1;
	[NonSerialized()]
	float WalkLeft;		// метры до следующего целого километра для контрактов
	[NonSerialized()]
	bool MyDirty;		// личные контракты изменились, клиенту ещё не отправлено

	VB_TabletStats ToStats()
	{
		VB_TabletStats s = new VB_TabletStats();
		s.PlayerName = PlayerName;
		s.Discord = Discord;
		s.Zombies = Zombies;
		s.Mutants = Mutants;
		s.Animals = Animals;
		s.DistM = DistM;
		s.RecordSec = RecordSec;
		s.Days = Days;
		s.Contracts = Contracts;
		s.ExtraXp = ExtraXp;
		s.AchBoss = AchBoss;
		s.AchCar = AchCar;
		s.AchGather = AchGather;
		s.AchFish = AchFish;
		s.AchCraft = AchCraft;
		s.AchBuild = AchBuild;
		s.AchTrophy = AchTrophy;
		s.AchCard = AchCard;
		return s;
	}
}

class VB_TabletRatingRow
{
	string Uid;
	string PlayerName;
	int Xp;
}

// Сводка для рейтинга: обновляется вместе с записью игрока, чтобы не читать все файлы.
class VB_TabletRatingFile
{
	ref array<ref VB_TabletRatingRow> Rows = new array<ref VB_TabletRatingRow>();
}

class VB_TabletPlayers
{
	static const string DIR_PLAYERS = "$profile:VB_Mods/VB_Tablet/players";
	static const string FILE_RATING = "$profile:VB_Mods/VB_Tablet/rating.json";

	protected static ref map<string, ref VB_TabletRecord> s_Online = new map<string, ref VB_TabletRecord>();
	protected static ref VB_TabletRatingFile s_Rating;
	protected static bool s_RatingDirty;

	protected static string PathOf(string uid)
	{
		return DIR_PLAYERS + "/" + uid + ".json";
	}

	static int DayKey()
	{
		int y;
		int m;
		int d;
		GetYearMonthDay(y, m, d);
		// Сплошной счёт дней: разница двух дат равна числу прошедших дней и на стыке месяцев.
		if (m <= 2)
		{
			y = y - 1;
			m = m + 12;
		}
		return 365 * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + d - 719469;
	}

	static VB_TabletRecord Get(string uid, string name)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
			return rec;
		rec = new VB_TabletRecord();
		string path = PathOf(uid);
		string error;
		bool ok = false;
		if (FileExist(path))
			ok = JsonFileLoader<VB_TabletRecord>.LoadFile(path, rec, error);
		if (!ok && FileExist(path + ".bak"))
		{
			rec = new VB_TabletRecord();
			ok = JsonFileLoader<VB_TabletRecord>.LoadFile(path + ".bak", rec, error);
			if (ok)
				Print(VB_TABLET_TAG + " запись игрока прочитана из копии: " + uid);
		}
		rec.Uid = uid;
		rec.PlayerName = name;
		int today = DayKey();
		if (rec.LastDay != today)
		{
			rec.LastDay = today;
			rec.Days++;
			rec.Dirty = true;
		}
		s_Online.Set(uid, rec);
		return rec;
	}

	// Путь и рекорд из статистики движка. Статистика своя у каждого персонажа и обнуляется после смерти.
	static void UpdateFromPlayer(VB_TabletRecord rec, PlayerBase player)
	{
		if (!player)
			return;
		float dist = player.StatGet(AnalyticsManagerServer.STAT_DISTANCE);
		if (rec.LastDist >= 0 && dist > rec.LastDist)
		{
			rec.DistM = rec.DistM + (dist - rec.LastDist);
			rec.Dirty = true;
			VB_TabletContracts.OnWalk(rec, dist - rec.LastDist);
		}
		rec.LastDist = dist;
		int life = player.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		VB_TabletContracts.OnLife(rec, life);
		if (life > rec.RecordSec)
		{
			rec.RecordSec = life;
			rec.Dirty = true;
		}
	}

	// Контракт засчитан. Игрок мог выйти: тогда правится файл, счётчик дней не трогаем.
	static void AddContract(string uid, int extraXp)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
		{
			rec.Contracts++;
			rec.ExtraXp = rec.ExtraXp + extraXp;
			rec.Dirty = true;
			return;
		}
		string path = PathOf(uid);
		if (!FileExist(path))
			return;
		rec = new VB_TabletRecord();
		string error;
		if (!JsonFileLoader<VB_TabletRecord>.LoadFile(path, rec, error))
		{
			Print(VB_TABLET_TAG + " не прочитан игрок " + uid + ": " + error);
			return;
		}
		rec.Uid = uid;
		rec.Contracts++;
		rec.ExtraXp = rec.ExtraXp + extraXp;
		rec.Dirty = true;
		Save(rec);
	}

	// Запись игрока только для чтения: онлайн из памяти, остальные из файла, в память не кладётся.
	static VB_TabletRecord Peek(string uid)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
			return rec;
		string path = PathOf(uid);
		if (!FileExist(path))
			return null;
		rec = new VB_TabletRecord();
		string error;
		if (!JsonFileLoader<VB_TabletRecord>.LoadFile(path, rec, error))
			return null;
		rec.Uid = uid;
		return rec;
	}

	static void ClearApply(string uid)
	{
		VB_TabletRecord rec;
		if (!s_Online.Find(uid, rec) || rec.AppliedClan == "")
			return;
		VB_Clan c = VB_TabletClans.Find(rec.AppliedClan);
		if (c)
		{
			for (int i = c.Requests.Count() - 1; i >= 0; i--)
			{
				if (c.Requests.Get(i).Uid == uid)
					c.Requests.Remove(i);
			}
			VB_TabletClans.Save(c);
			VB_TabletServer.PushClan(c);
		}
		rec.AppliedClan = "";
		rec.Dirty = true;
	}

	static void Save(VB_TabletRecord rec)
	{
		if (!rec.Dirty)
			return;
		MakeDirectory(VB_TabletConfig.DIR_ROOT);
		MakeDirectory(VB_TabletConfig.DIR_MOD);
		MakeDirectory(DIR_PLAYERS);
		string path = PathOf(rec.Uid);
		if (FileExist(path))
			CopyFile(path, path + ".bak");
		string error;
		if (!JsonFileLoader<VB_TabletRecord>.SaveFile(path, rec, error))
		{
			Print(VB_TABLET_TAG + " не записан игрок " + rec.Uid + ": " + error);
			return;
		}
		rec.Dirty = false;
		PutRating(rec);
	}

	protected static void LoadRating()
	{
		if (s_Rating)
			return;
		s_Rating = new VB_TabletRatingFile();
		string error;
		if (FileExist(FILE_RATING) && !JsonFileLoader<VB_TabletRatingFile>.LoadFile(FILE_RATING, s_Rating, error))
		{
			Print(VB_TABLET_TAG + " ошибка в rating.json: " + error);
			s_Rating = new VB_TabletRatingFile();
		}
		if (!s_Rating.Rows)
			s_Rating.Rows = new array<ref VB_TabletRatingRow>();
	}

	protected static void PutRating(VB_TabletRecord rec)
	{
		LoadRating();
		VB_TabletRates rates = VB_TabletConfig.Get().Rates;
		VB_TabletRatingRow row = null;
		foreach (VB_TabletRatingRow r : s_Rating.Rows)
		{
			if (r.Uid == rec.Uid)
				row = r;
		}
		if (!row)
		{
			row = new VB_TabletRatingRow();
			row.Uid = rec.Uid;
			s_Rating.Rows.Insert(row);
		}
		row.PlayerName = rec.PlayerName;
		row.Xp = rec.ToStats().Xp(rates);
		s_RatingDirty = true;
	}

	// Список по убыванию опыта. Игроков в файле сотни, поэтому простая вставка.
	static array<ref VB_TabletRatingRow> Sorted()
	{
		foreach (string uid, VB_TabletRecord rec : s_Online)
			PutRating(rec);
		array<ref VB_TabletRatingRow> list = new array<ref VB_TabletRatingRow>();
		foreach (VB_TabletRatingRow r : s_Rating.Rows)
		{
			int at = list.Count();
			for (int i = 0; i < list.Count(); i++)
			{
				if (r.Xp > list.Get(i).Xp)
				{
					at = i;
					break;
				}
			}
			list.InsertAt(r, at);
		}
		return list;
	}

	static int PlaceOf(string uid)
	{
		array<ref VB_TabletRatingRow> list = Sorted();
		for (int i = 0; i < list.Count(); i++)
		{
			if (list.Get(i).Uid == uid)
				return i + 1;
		}
		return 0;
	}

	// Раз в 30 секунд: путь и рекорд у всех в сети, запись изменившихся.
	static void Tick()
	{
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player || !player.GetIdentity())
				continue;
			PlayerIdentity id = player.GetIdentity();
			VB_TabletRecord rec = Get(id.GetPlainId(), id.GetName());
			UpdateFromPlayer(rec, player);
			Save(rec);
		}
		if (s_RatingDirty)
		{
			s_RatingDirty = false;
			string error;
			if (!JsonFileLoader<VB_TabletRatingFile>.SaveFile(FILE_RATING, s_Rating, error))
				Print(VB_TABLET_TAG + " не записан rating.json: " + error);
		}
	}

	static void Release(PlayerBase player, string uid)
	{
		VB_TabletRecord rec;
		if (!s_Online.Find(uid, rec))
			return;
		UpdateFromPlayer(rec, player);
		rec.Dirty = true;
		Save(rec);
		s_Online.Remove(uid);
		VB_TabletMap.Release(uid);
		VB_TabletServer.MemberOnline(uid, false);
	}
}
