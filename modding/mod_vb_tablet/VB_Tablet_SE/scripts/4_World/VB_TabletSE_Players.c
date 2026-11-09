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
	int LastDay;	// день последнего захода: по нему игрок забывается через PlayerForgetDays
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
	static const int SORT_GAP_MS = 30000;		// порядок рейтинга пересчитывается не чаще
	static const int RATING_SAVE_MS = 300000;	// rating.json пишется не чаще (это сводка, записи игроков главнее)

	protected static ref map<string, ref VB_TabletRecord> s_Online = new map<string, ref VB_TabletRecord>();
	protected static ref VB_TabletRatingFile s_Rating;
	protected static ref map<string, ref VB_TabletRatingRow> s_RowOf;	// uid -> строка рейтинга
	protected static ref array<ref VB_TabletRatingRow> s_Sorted;		// по убыванию опыта
	protected static ref map<string, int> s_Place;						// uid -> место, с 1
	protected static bool s_RatingDirty;
	protected static bool s_OrderDirty = true;
	protected static int s_SortedAt = -1000000;
	protected static int s_RatingSavedAt;
	protected static int s_PurgeDay = -1;

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

	// Запись с диска: основной файл, копия .bak, недописанный .tmp. null: файла нет или он не читается.
	// uid проверяется: он попадает в путь файла.
	protected static VB_TabletRecord ReadFile(string uid)
	{
		if (!VB_TabletCheck.IsSteam64(uid))
			return null;
		string path = PathOf(uid);
		string error;
		for (int n = 0; n < 3; n++)
		{
			string file = VB_TabletFiles.Attempt(path, n);
			if (!FileExist(file))
				continue;
			VB_TabletRecord rec = new VB_TabletRecord();
			if (JsonFileLoader<VB_TabletRecord>.LoadFile(file, rec, error))
			{
				VB_TabletFiles.Recovered(path, n);
				rec.Uid = uid;
				return rec;
			}
			Print(VB_TABLET_TAG + " не прочитан игрок " + file + ": " + error);
		}
		return null;
	}

	// Игрок в сети: запись живёт в памяти до выхода. Для офлайн-игроков Peek (чтение) и Edit/Commit (правка).
	static VB_TabletRecord Get(string uid, string name)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
		{
			if (name != "")
				rec.PlayerName = name;
			return rec;
		}
		rec = ReadFile(uid);
		if (!rec)
		{
			rec = new VB_TabletRecord();
			rec.Uid = uid;
		}
		if (name != "")
			rec.PlayerName = name;
		NewDay(rec);
		s_Online.Set(uid, rec);
		return rec;
	}

	// День на сервере засчитывается и тому, кто играет через полночь.
	protected static void NewDay(VB_TabletRecord rec)
	{
		int today = DayKey();
		if (rec.LastDay == today)
			return;
		rec.LastDay = today;
		rec.Days++;
		rec.Dirty = true;
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

	// Запись для правки: из памяти, если игрок в сети, иначе с диска (в память не кладётся). После правки Commit.
	static VB_TabletRecord Edit(string uid)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
			return rec;
		return ReadFile(uid);
	}

	// После Edit: запись офлайн-игрока пишется сразу, запись того, кто в сети, запишет Tick.
	static void Commit(VB_TabletRecord rec)
	{
		if (!rec)
			return;
		rec.Dirty = true;
		if (!s_Online.Contains(rec.Uid))
			Save(rec);
	}

	// Контракт засчитан. Игрок мог выйти: тогда правится файл, счётчик дней не трогаем.
	static void AddContract(string uid, int extraXp)
	{
		VB_TabletRecord rec = Edit(uid);
		if (!rec)
			return;
		rec.Contracts++;
		rec.ExtraXp = rec.ExtraXp + extraXp;
		Commit(rec);
	}

	// Запись игрока только для чтения: онлайн из памяти, остальные из файла, в память не кладётся.
	static VB_TabletRecord Peek(string uid)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
			return rec;
		return ReadFile(uid);
	}

	// Игрок отменил заявку, вступил, создал клан или подал новую: прежняя заявка снимается и в клане.
	static void ClearApply(string uid)
	{
		VB_TabletRecord rec = Edit(uid);
		if (!rec || rec.AppliedClan == "")
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
		Commit(rec);
	}

	// Заявку рассмотрели или клан распущен: пометка снимается, только если она про этот клан.
	static void ClearApplyOf(string uid, string clanId)
	{
		VB_TabletRecord rec = Edit(uid);
		if (!rec || rec.AppliedClan != clanId)
			return;
		rec.AppliedClan = "";
		Commit(rec);
	}

	// true: на диске то же, что в памяти.
	static bool Save(VB_TabletRecord rec)
	{
		if (!rec.Dirty)
			return true;
		if (!VB_TabletCheck.IsSteam64(rec.Uid))
			return false;
		VB_TabletFiles.Dirs();
		MakeDirectory(DIR_PLAYERS);
		string error;
		if (!VB_SafeJson<VB_TabletRecord>.Save(PathOf(rec.Uid), rec, error))
		{
			Print(VB_TABLET_TAG + " не записан игрок " + rec.Uid + ": " + error);
			return false;
		}
		rec.Dirty = false;
		PutRating(rec);
		return true;
	}

	// --- рейтинг -----------------------------------------------------------------------------------------
	protected static void LoadRating()
	{
		if (s_Rating)
			return;
		s_RowOf = new map<string, ref VB_TabletRatingRow>();
		s_Rating = null;
		string error;
		for (int n = 0; n < 3; n++)
		{
			string file = VB_TabletFiles.Attempt(FILE_RATING, n);
			if (!FileExist(file))
				continue;
			VB_TabletRatingFile read = new VB_TabletRatingFile();
			if (JsonFileLoader<VB_TabletRatingFile>.LoadFile(file, read, error))
			{
				VB_TabletFiles.Recovered(FILE_RATING, n);
				s_Rating = read;
				break;
			}
			Print(VB_TABLET_TAG + " ошибка в " + file + ": " + error);
		}
		if (!s_Rating)
			s_Rating = new VB_TabletRatingFile();
		if (!s_Rating.Rows)
			s_Rating.Rows = new array<ref VB_TabletRatingRow>();
		// Дубли uid (старые файлы) схлопываются: остаётся строка с большим опытом.
		array<ref VB_TabletRatingRow> clean = new array<ref VB_TabletRatingRow>();
		foreach (VB_TabletRatingRow r : s_Rating.Rows)
		{
			if (!r || r.Uid == "")
				continue;
			VB_TabletRatingRow have;
			if (s_RowOf.Find(r.Uid, have))
			{
				if (r.Xp > have.Xp)
				{
					have.Xp = r.Xp;
					have.PlayerName = r.PlayerName;
				}
				continue;
			}
			s_RowOf.Set(r.Uid, r);
			clean.Insert(r);
		}
		s_Rating.Rows = clean;
		s_OrderDirty = true;
	}

	protected static void PutRating(VB_TabletRecord rec)
	{
		LoadRating();
		VB_TabletRates rates = VB_TabletConfig.Get().Rates;
		VB_TabletRatingRow row;
		if (!s_RowOf.Find(rec.Uid, row))
		{
			row = new VB_TabletRatingRow();
			row.Uid = rec.Uid;
			s_Rating.Rows.Insert(row);
			s_RowOf.Set(rec.Uid, row);
		}
		if (row.LastDay != rec.LastDay)
		{
			row.LastDay = rec.LastDay;
			s_RatingDirty = true;
		}
		int xp = rec.ToStats().Xp(rates);
		if (row.PlayerName != rec.PlayerName || row.Xp != xp)
		{
			row.PlayerName = rec.PlayerName;
			row.Xp = xp;
			s_RatingDirty = true;
			s_OrderDirty = true;
		}
	}

	// Список по убыванию опыта. Пересчитывается не чаще раза в SORT_GAP_MS, сортировка слиянием:
	// игроков за всё время бывают тысячи, а спрашивают рейтинг и место на каждом открытии профиля.
	static array<ref VB_TabletRatingRow> Sorted()
	{
		LoadRating();
		int now = GetGame().GetTime();
		if (s_Sorted && (!s_OrderDirty || now - s_SortedAt < SORT_GAP_MS))
			return s_Sorted;
		s_SortedAt = now;
		s_OrderDirty = false;
		s_Sorted = new array<ref VB_TabletRatingRow>();
		foreach (VB_TabletRatingRow r : s_Rating.Rows)
			s_Sorted.Insert(r);
		SortRows(s_Sorted);
		s_Place = new map<string, int>();
		int n = s_Sorted.Count();
		for (int i = 0; i < n; i++)
			s_Place.Set(s_Sorted.Get(i).Uid, i + 1);
		return s_Sorted;
	}

	protected static void SortRows(array<ref VB_TabletRatingRow> list)
	{
		int n = list.Count();
		if (n < 2)
			return;
		array<ref VB_TabletRatingRow> tmp = new array<ref VB_TabletRatingRow>();
		for (int f = 0; f < n; f++)
			tmp.Insert(null);
		int width = 1;
		while (width < n)
		{
			int lo = 0;
			while (lo < n)
			{
				int mid = lo + width;
				if (mid > n)
					mid = n;
				int hi = lo + width * 2;
				if (hi > n)
					hi = n;
				int a = lo;
				int b = mid;
				int k = lo;
				while (a < mid && b < hi)
				{
					if (list.Get(b).Xp > list.Get(a).Xp)
					{
						tmp.Set(k, list.Get(b));
						b++;
					}
					else
					{
						tmp.Set(k, list.Get(a));
						a++;
					}
					k++;
				}
				while (a < mid)
				{
					tmp.Set(k, list.Get(a));
					a++;
					k++;
				}
				while (b < hi)
				{
					tmp.Set(k, list.Get(b));
					b++;
					k++;
				}
				lo = hi;
			}
			for (int i = 0; i < n; i++)
				list.Set(i, tmp.Get(i));
			width = width * 2;
		}
	}

	static int PlaceOf(string uid)
	{
		Sorted();
		int place;
		if (s_Place && s_Place.Find(uid, place))
			return place;
		return 0;
	}

	protected static void SaveRating(bool force)
	{
		if (!s_Rating || !s_RatingDirty)
			return;
		int now = GetGame().GetTime();
		if (!force && now - s_RatingSavedAt < RATING_SAVE_MS)
			return;
		VB_TabletFiles.Dirs();
		string error;
		if (!VB_SafeJson<VB_TabletRatingFile>.Save(FILE_RATING, s_Rating, error))
		{
			Print(VB_TABLET_TAG + " не записан rating.json: " + error);
			return;
		}
		s_RatingDirty = false;
		s_RatingSavedAt = now;
	}

	// Раз в 30 секунд: путь, рекорд и смена дня у всех в сети, запись изменившихся.
	// Записи тех, кого уже нет на сервере (выход без identity), сохраняются и выгружаются.
	static void Tick()
	{
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		map<string, PlayerBase> here = new map<string, PlayerBase>();
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player || !player.GetIdentity())
				continue;
			PlayerIdentity id = player.GetIdentity();
			string uid = id.GetPlainId();
			here.Set(uid, player);
			VB_TabletRecord rec = Get(uid, id.GetName());
			NewDay(rec);
			UpdateFromPlayer(rec, player);
			Save(rec);
		}
		array<string> stale = new array<string>();
		foreach (string key, VB_TabletRecord r : s_Online)
		{
			if (!here.Contains(key))
				stale.Insert(key);
		}
		foreach (string gone : stale)
			Release(null, gone);
		int today = DayKey();
		if (today != s_PurgeDay)
		{
			s_PurgeDay = today;
			Purge(today);
		}
		SaveRating(false);
	}

	// Игрок не заходил PlayerForgetDays дней (решение 2026-11-09: 90): данные планшета удаляются, при возвращении
	// он начинает с нуля. Раз в сутки и при старте. Строки рейтинга без даты (до обновления) отсчитываются с сегодня.
	protected static void Purge(int today)
	{
		int days = VB_TabletConfig.Get().PlayerForgetDays;
		if (days <= 0)
			return;
		LoadRating();
		array<ref VB_TabletRatingRow> keep = new array<ref VB_TabletRatingRow>();
		int removed = 0;
		foreach (VB_TabletRatingRow r : s_Rating.Rows)
		{
			if (r.LastDay <= 0)
			{
				r.LastDay = today;
				s_RatingDirty = true;
			}
			if (today - r.LastDay < days || s_Online.Contains(r.Uid) || !Forget(r.Uid))
			{
				keep.Insert(r);
				continue;
			}
			s_RowOf.Remove(r.Uid);
			removed++;
			Print(VB_TABLET_TAG + " игрок удалён из планшета (не заходил " + days.ToString() + "+ дней): " + r.PlayerName + " " + r.Uid);
		}
		if (removed == 0)
			return;
		s_Rating.Rows = keep;
		s_RatingDirty = true;
		s_OrderDirty = true;
		SaveRating(true);
	}

	// Удаление данных игрока: запись, личная карта, место в клане. false: оставить (лидер клана: такой клан
	// уже передал бы лидерство или распустился по неактивности, трогать его здесь нельзя).
	protected static bool Forget(string uid)
	{
		if (!VB_TabletCheck.IsSteam64(uid))
			return false;
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (c)
		{
			if (c.IsLeader(uid))
				return false;
			string name = uid;
			VB_ClanMember m = c.Member(uid);
			if (m)
				name = m.PlayerName;
			VB_TabletClans.RemoveMember(c, uid);
			VB_TabletClans.Log(c, VB_ClanEvent.LEFT, name);
			VB_TabletClans.Save(c);
			VB_TabletServer.PushClan(c);
		}
		DeleteWithCopies(PathOf(uid));
		VB_TabletMap.Forget(uid);
		return true;
	}

	// Файл и его копии .bak и .tmp.
	static void DeleteWithCopies(string path)
	{
		array<string> files = {path, path + ".bak", path + ".tmp"};
		foreach (string f : files)
		{
			if (FileExist(f))
				DeleteFile(f);
		}
	}

	// Остановка сервера: всё, что в памяти, на диск.
	static void SaveAll()
	{
		foreach (string uid, VB_TabletRecord rec : s_Online)
		{
			rec.Dirty = true;
			Save(rec);
		}
		SaveRating(true);
	}

	// player может быть null: игрок уже ушёл, путь и рекорд не обновляются.
	static void Release(PlayerBase player, string uid)
	{
		VB_TabletRecord rec;
		if (s_Online.Find(uid, rec))
		{
			UpdateFromPlayer(rec, player);
			rec.Dirty = true;
			Save(rec);
			s_Online.Remove(uid);
		}
		VB_TabletMap.Release(uid);
		VB_TabletServer.MemberOnline(uid, false);
		VB_TabletServer.Forget(uid);
		VB_TabletClanOps.Forget(uid);
		VB_TabletBank.Forget(uid);
	}
}
