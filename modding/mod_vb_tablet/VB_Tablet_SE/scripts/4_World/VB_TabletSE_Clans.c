// Кланы на сервере: файлы $profile:VB_Mods/VB_Tablet/clans/<id>.json, индекс в памяти, все проверки.
class VB_TabletClans
{
	static const string DIR_CLANS = "$profile:VB_Mods/VB_Tablet/clans";
	protected static ref map<string, ref VB_Clan> s_Clans;
	protected static ref map<string, string> s_ByUid;	// игрок -> клан
	protected static ref ScriptInvoker s_OnJoin;
	protected static ref ScriptInvoker s_OnLeave;
	protected static ref map<string, ref array<ref VB_ClanInvite>> s_Invites;
	protected static int s_NextId = 1;
	protected static bool s_Loaded;
	protected static int s_Week = -1;

	static void Load()
	{
		if (s_Loaded)
			return;
		s_Loaded = true;
		s_Clans = new map<string, ref VB_Clan>();
		s_ByUid = new map<string, string>();
		s_Invites = new map<string, ref array<ref VB_ClanInvite>>();
		MakeDirectory(VB_TabletConfig.DIR_ROOT);
		MakeDirectory(VB_TabletConfig.DIR_MOD);
		MakeDirectory(DIR_CLANS);
		string file;
		FileAttr attr;
		FindFileHandle h = FindFile(DIR_CLANS + "/*.json", file, attr, FindFileFlags.ALL);
		if (!h)
			return;
		bool more = true;
		while (more)
		{
			LoadOne(file);
			more = FindNextFile(h, file, attr);
		}
		CloseFindFile(h);
		Print(VB_TABLET_TAG + " кланов загружено: " + s_Clans.Count());
	}

	// Номер из имени файла cN.json занят, даже если файл не прочитался: новый клан не получит тот же id
	// и не затрёт его копию.
	protected static void ReserveId(string file)
	{
		int dot = file.IndexOf(".");
		if (dot < 2 || file.Get(0) != "c")
			return;
		string num = file.Substring(1, dot - 1);
		int n = num.ToInt();
		if (n + 1 > s_NextId)
			s_NextId = n + 1;
	}

	protected static void LoadOne(string file)
	{
		ReserveId(file);
		string path = DIR_CLANS + "/" + file;
		VB_Clan c = null;
		string error;
		for (int n = 0; n < 3; n++)
		{
			VB_Clan read = new VB_Clan();
			if (VB_SafeJson<VB_Clan>.LoadFrom(VB_TabletFiles.Attempt(path, n), read, error) && read.Id != "")
			{
				c = read;
				VB_TabletFiles.Recovered(path, n);
				break;
			}
			if (n == 0)
				Print(VB_TABLET_TAG + " ошибка в файле клана " + file + ": " + error);
		}
		if (!c)
		{
			Print(VB_TABLET_TAG + " ВНИМАНИЕ: клан " + file + " не загружен, его копий нет или они тоже повреждены. Файл не трогаем.");
			return;
		}
		s_Clans.Set(c.Id, c);
		foreach (VB_ClanMember m : c.Members)
		{
			m.Online = false;
			s_ByUid.Set(m.Uid, c.Id);
		}
		ReserveId(c.Id + ".json");
	}

	static bool Save(VB_Clan c)
	{
		string path = DIR_CLANS + "/" + c.Id + ".json";
		string error;
		if (!VB_SafeJson<VB_Clan>.Save(path, c, error))
		{
			Print(VB_TABLET_TAG + " не записан клан " + c.Id + ": " + error);
			c.Dirty = true;
			return false;
		}
		c.Dirty = false;
		return true;
	}

	// Не срочные изменения (чат): файл пишется пачкой раз в несколько секунд, а не на каждое сообщение.
	static void SaveSoon(VB_Clan c)
	{
		c.Dirty = true;
	}

	// Из таймера миссии и при остановке сервера.
	static void Flush()
	{
		if (!s_Clans)
			return;
		foreach (string id, VB_Clan c : s_Clans)
		{
			if (c.Dirty)
				Save(c);
		}
	}

	static VB_Clan Find(string id)
	{
		Load();
		VB_Clan c;
		s_Clans.Find(id, c);
		return c;
	}

	static VB_Clan OfPlayer(string uid)
	{
		Load();
		string id;
		if (!s_ByUid.Find(uid, id))
			return null;
		return Find(id);
	}

	static map<string, ref VB_Clan> All()
	{
		Load();
		return s_Clans;
	}

	static string Stamp()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		return Pad(d) + "." + Pad(mo) + " " + Pad(h) + ":" + Pad(mi);
	}

	protected static string Pad(int v)
	{
		string r = v.ToString();
		if (v < 10)
			r = "0" + r;
		return r;
	}

	static void Log(VB_Clan c, int kind, string who, string arg = "", string val = "")
	{
		VB_ClanJournalRow row = new VB_ClanJournalRow();
		row.Kind = kind;
		row.Who = who;
		row.Arg = arg;
		row.Val = val;
		string st = Stamp();
		row.Day = st.Substring(0, 5);
		row.Time = st.Substring(6, 5);
		c.Journal.InsertAt(row, 0);
		while (c.Journal.Count() > VB_CLAN_JOURNAL_MAX)
			c.Journal.Remove(c.Journal.Count() - 1);
		VB_TabletFeed.Event(c, row);
	}

	static void AddHistory(VB_Clan c, string who, int kind, int sum)
	{
		VB_ClanBankRow row = new VB_ClanBankRow();
		row.Who = who;
		row.Kind = kind;
		row.Sum = sum;
		row.Time = Stamp();
		c.History.InsertAt(row, 0);
		while (c.History.Count() > VB_CLAN_HISTORY_MAX)
			c.History.Remove(c.History.Count() - 1);
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		int today = VB_TabletClans.WeekDay(y, mo, d);
		c.Week.Set(today, c.Week.Get(today) + sum);
	}

	// Номер сегодняшнего дня недели с понедельника.
	static int TodayIdx()
	{
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		return WeekDay(y, mo, d);
	}

	// Итоги недели в понедельник: награда трём лучшим и обнуление счётчиков.
	protected static void CheckWeek(int today)
	{
		int week = (today + 3) / 7;
		string path = VB_TabletConfig.DIR_MOD + "/week.txt";
		if (s_Week < 0)
		{
			s_Week = week;
			if (FileExist(path))
			{
				FileHandle h = OpenFile(path, FileMode.READ);
				string line;
				if (h && FGets(h, line) > 0)
					s_Week = line.ToInt();
				if (h)
					CloseFile(h);
			}
		}
		if (s_Week == week)
		{
			if (!FileExist(path))
				StoreWeek(path, week);
			return;
		}
		s_Week = week;
		array<ref VB_Clan> top = new array<ref VB_Clan>();
		foreach (string id, VB_Clan c : s_Clans)
		{
			int at = top.Count();
			for (int i = 0; i < top.Count(); i++)
			{
				if (c.WeekInfluence > top.Get(i).WeekInfluence)
				{
					at = i;
					break;
				}
			}
			top.InsertAt(c, at);
		}
		// Итоги каждого клана отмечаются в его файле (WeekDone): если сервер упадёт посреди раздачи
		// или week.txt не запишется, второй раз приз никто не получит.
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		for (int place = 0; place < top.Count(); place++)
		{
			VB_Clan t = top.Get(place);
			if (t.WeekDone == week)
				continue;
			t.WeekDone = week;
			int prize = 0;
			if (t.WeekInfluence > 0 && place < cfg.WeekPrizes.Count())
				prize = Math.Min(cfg.WeekPrizes.Get(place), cfg.BankMax - t.Bank);
			t.WeekInfluence = 0;
			for (int w = 0; w < 7; w++)
				t.Week.Set(w, 0);
			foreach (VB_ClanMember m : t.Members)
			{
				for (int k = 0; k < 7; k++)
					m.Week.Set(k, 0);
			}
			if (prize > 0)
			{
				t.Bank = t.Bank + prize;
				int shown = place + 1;
				AddHistory(t, "", VB_ClanEvent.WEEK_PRIZE, prize);
				Log(t, VB_ClanEvent.WEEK_PRIZE, "", shown.ToString(), VBT_NumText(prize));
			}
			Save(t);
			VB_TabletServer.PushClan(t);
			VB_TabletBank.PushHistory(t);
		}
		StoreWeek(path, week);
	}

	protected static string VBT_NumText(int v)
	{
		return v.ToString();
	}

	protected static void StoreWeek(string path, int week)
	{
		FileHandle h = OpenFile(path, FileMode.WRITE);
		if (!h)
		{
			Print(VB_TABLET_TAG + " не записан week.txt: итоги недели защищены отметкой в файлах кланов");
			return;
		}
		FPrint(h, week.ToString());
		CloseFile(h);
	}

	// День недели с понедельника: 0..6, формула Зеллера.
	static int WeekDay(int y, int m, int d)
	{
		if (m < 3)
		{
			m = m + 12;
			y = y - 1;
		}
		int k = y % 100;
		int j = y / 100;
		int v = d + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j;
		int r = v % 7;
		return (r + 5) % 7;
	}

	static array<ref VB_ClanRank> DefaultRanks()
	{
		array<ref VB_ClanRank> list = new array<ref VB_ClanRank>();
		list.Insert(new VB_ClanRank("leader", "#STR_VB_TAB_RK_LEADER", 0xFFFF7A1A, VB_ClanPerm.ALL));
		list.Insert(new VB_ClanRank("deputy", "#STR_VB_TAB_RK_DEPUTY", 0xFFE9C55B, VB_ClanPerm.INVITE | VB_ClanPerm.KICK | VB_ClanPerm.ASSIGN | VB_ClanPerm.MAP | VB_ClanPerm.NEWS));
		list.Insert(new VB_ClanRank("member", "#STR_VB_TAB_RK_MEMBER", 0xFFCFCAC2, 0));
		return list;
	}

	static bool NameFree(string title, VB_Clan except)
	{
		string low = VB_TabletCheck.Lower(title);
		low.TrimInPlace();
		map<string, ref VB_Clan> all = All();
		foreach (string id, VB_Clan c : all)
		{
			if (c == except)
				continue;
			string other = VB_TabletCheck.Lower(c.Title);
			other.TrimInPlace();
			if (other == low)
				return false;
		}
		return true;
	}

	static bool TagFree(string tag, VB_Clan except)
	{
		string want = tag + "";
		want.ToUpper();
		map<string, ref VB_Clan> all = All();
		foreach (string id, VB_Clan c : all)
		{
			string other = c.Tag + "";
			other.ToUpper();
			if (c != except && other == want)
				return false;
		}
		return true;
	}

	// Символ эмблемы уникален на сервере (решение 2026-11-09): кланов не больше, чем символов (36).
	// Форма и цвет могут повторяться. shape оставлен для совместимости вызовов.
	static bool EmblemFree(string shape, string symbol, VB_Clan except)
	{
		map<string, ref VB_Clan> all = All();
		foreach (string id, VB_Clan c : all)
		{
			if (c != except && c.EmbSymbol == symbol)
				return false;
		}
		return true;
	}

	// Запрещённые слова без учёта регистра, в том числе кириллицы.
	static bool HasBanned(string text)
	{
		string low = VB_TabletCheck.Lower(text);
		array<string> words = VB_TabletConfig.Get().BannedWords;
		foreach (string w : words)
		{
			string bad = VB_TabletCheck.Lower(w);
			bad.TrimInPlace();
			if (bad != "" && low.Contains(bad))
				return true;
		}
		return false;
	}

	// Название: без пробелов по краям, 3..24 знака.
	static bool TitleValid(string title)
	{
		string t = title + "";
		t.TrimInPlace();
		int len = title.LengthUtf8();
		return t == title && len >= 3 && len <= 24;
	}

	// Клан игрока изменился: всем в сети новая шапка.
	static void Create(PlayerBase player, PlayerIdentity id, string title, string tag, string slogan, string shape, string symbol, int color)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		string uid = id.GetPlainId();
		int res = CheckCreate(player, uid, title, tag, slogan, shape, symbol, color);
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		res = VB_TabletMoney.Charge(player, cfg.ClanPrice);
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		VB_Clan c = new VB_Clan();
		c.Id = "c" + s_NextId.ToString();
		s_NextId++;
		c.Title = title;
		c.Tag = tag;
		c.Slogan = slogan;
		c.EmbShape = shape;
		c.EmbSymbol = symbol;
		c.Color = color;
		c.TaxFree = cfg.TaxFreeDays;
		c.LastTaxDay = VB_TabletPlayers.DayKey();
		c.Ranks = DefaultRanks();
		AddMember(c, uid, id.GetName(), "leader");
		Log(c, VB_ClanEvent.CREATED, id.GetName());
		s_Clans.Set(c.Id, c);
		// Клан, которого нет в файле, исчез бы после рестарта вместе с оплатой: деньги возвращаются.
		if (!Save(c))
		{
			s_Clans.Remove(c.Id);
			s_ByUid.Remove(uid);
			if (!VB_TabletMoney.Give(player, cfg.ClanPrice))
				Print(VB_TABLET_TAG + " СОЗДАНИЕ КЛАНА: файл не записан и оплату не вернуть, проверьте вручную: " + uid + " должен получить " + cfg.ClanPrice.ToString());
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			VB_TabletServer.SendProfile(player, id);
			return;
		}
		VB_TabletPlayers.ClearApply(uid);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.CREATED);
		VB_TabletServer.PushClan(c);
		VB_TabletServer.SendProfile(player, id);
	}

	protected static int CheckCreate(PlayerBase player, string uid, string title, string tag, string slogan, string shape, string symbol, int color)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		if (OfPlayer(uid))
			return VB_TabletResult.HAS_CLAN;
		if (!TitleValid(title) || !VB_TabletCheck.TagValid(tag) || slogan.LengthUtf8() > 60)
			return VB_TabletResult.BAD_INPUT;
		if (!VB_ClanLook.Valid(shape, symbol, color))
			return VB_TabletResult.BAD_INPUT;
		if (HasBanned(title) || HasBanned(tag) || HasBanned(slogan))
			return VB_TabletResult.BANNED_WORD;
		if (All().Count() >= cfg.MaxClans)
			return VB_TabletResult.SERVER_FULL;
		if (!NameFree(title, null))
			return VB_TabletResult.NAME_TAKEN;
		if (!TagFree(tag, null))
			return VB_TabletResult.TAG_TAKEN;
		if (!EmblemFree(shape, symbol, null))
			return VB_TabletResult.EMBLEM_TAKEN;
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, player.GetIdentity().GetName());
		int xp = rec.ToStats().Xp(cfg.Rates);
		if (cfg.Rates.RankOf(xp) < cfg.ClanRank)
			return VB_TabletResult.NO_RANK;
		if (VB_TabletMoney.Count(player) < cfg.ClanPrice)
			return VB_TabletResult.NO_MONEY;
		return VB_TabletResult.OK;
	}

	// Для дополнений: (VB_Clan клан, string uid). Вступление после добавления в состав, выход после удаления.
	static ScriptInvoker OnMemberJoin()
	{
		if (!s_OnJoin)
			s_OnJoin = new ScriptInvoker();
		return s_OnJoin;
	}

	static ScriptInvoker OnMemberLeave()
	{
		if (!s_OnLeave)
			s_OnLeave = new ScriptInvoker();
		return s_OnLeave;
	}

	static void AddMember(VB_Clan c, string uid, string name, string rank)
	{
		VB_ClanMember m = new VB_ClanMember();
		m.Uid = uid;
		m.PlayerName = name;
		m.RankId = rank;
		m.Online = VB_TabletServer.FindPlayer(uid) != null;
		m.LastDay = VB_TabletPlayers.DayKey();
		// Запись игрока только читается: офлайн-игрок не попадает в кэш тех, кто в сети.
		VB_TabletRecord rec = VB_TabletPlayers.Peek(uid);
		if (rec)
			m.Discord = rec.Discord;
		c.Members.Insert(m);
		s_ByUid.Set(uid, c.Id);
		s_Invites.Remove(uid);
		if (s_OnJoin)
			s_OnJoin.Invoke(c, uid);
	}

	static void RemoveMember(VB_Clan c, string uid)
	{
		for (int i = c.Members.Count() - 1; i >= 0; i--)
		{
			if (c.Members.Get(i).Uid == uid)
				c.Members.Remove(i);
		}
		s_ByUid.Remove(uid);
		if (s_OnLeave)
			s_OnLeave.Invoke(c, uid);
	}

	// Роспуск: удаляется всё, что связано с кланом. Копия .bak тоже.
	static void Dissolve(VB_Clan c)
	{
		array<string> uids = new array<string>();
		foreach (VB_ClanMember m : c.Members)
			uids.Insert(m.Uid);
		foreach (string uid : uids)
			s_ByUid.Remove(uid);
		// Роспуск для дополнений это выход каждого, пока состав ещё на месте.
		if (s_OnLeave)
		{
			foreach (string left : uids)
				s_OnLeave.Invoke(c, left);
		}
		// Поданные заявки больше некуда рассматривать: у игроков снимается пометка «заявка подана».
		array<string> applicants = new array<string>();
		foreach (VB_ClanRequest q : c.Requests)
			applicants.Insert(q.Uid);
		string path = DIR_CLANS + "/" + c.Id + ".json";
		DeleteFile(path);
		DeleteFile(path + ".bak");
		DeleteFile(path + ".tmp");
		s_Clans.Remove(c.Id);
		foreach (string gone : uids)
			VB_TabletServer.SendNoClan(gone);
		foreach (string who : applicants)
		{
			VB_TabletPlayers.ClearApplyOf(who, c.Id);
			VB_TabletServer.SendNoClan(who);
		}
	}

	static void AddInvite(string uid, VB_Clan c, string by)
	{
		array<ref VB_ClanInvite> list;
		if (!s_Invites.Find(uid, list))
		{
			list = new array<ref VB_ClanInvite>();
			s_Invites.Set(uid, list);
		}
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			if (list.Get(i).ClanId == c.Id)
				list.Remove(i);
		}
		VB_ClanInvite inv = new VB_ClanInvite();
		inv.ClanId = c.Id;
		inv.Title = c.Title;
		inv.Tag = c.Tag;
		inv.Slogan = c.Slogan;
		inv.EmbShape = c.EmbShape;
		inv.EmbSymbol = c.EmbSymbol;
		inv.Color = c.Color;
		inv.Level = c.Level;
		inv.MemberCount = c.Members.Count();
		inv.By = by;
		inv.ExpiresAt = GetGame().GetTickTime() + 86400;
		list.Insert(inv);
	}

	static array<ref VB_ClanInvite> InvitesOf(string uid)
	{
		array<ref VB_ClanInvite> list;
		if (!s_Invites || !s_Invites.Find(uid, list))
			return new array<ref VB_ClanInvite>();
		float now = GetGame().GetTickTime();
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			if (list.Get(i).ExpiresAt < now || !Find(list.Get(i).ClanId))
				list.Remove(i);
		}
		if (list.Count() == 0)
		{
			s_Invites.Remove(uid);
			return new array<ref VB_ClanInvite>();
		}
		return list;
	}

	static bool HasInvite(string uid, string clanId)
	{
		array<ref VB_ClanInvite> list = InvitesOf(uid);
		foreach (VB_ClanInvite inv : list)
		{
			if (inv.ClanId == clanId)
				return true;
		}
		return false;
	}

	static void DropInvite(string uid, string clanId)
	{
		array<ref VB_ClanInvite> list;
		if (!s_Invites || !s_Invites.Find(uid, list))
			return;
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			if (list.Get(i).ClanId == clanId)
				list.Remove(i);
		}
		if (list.Count() == 0)
			s_Invites.Remove(uid);
	}

	// Новый день: неактивность, пропавший лидер, налог и обнуление счётчиков «сегодня».
	// Пропущенные дни налога доначисляются. Решения пользователя 2026-11-09: налог по уровню клана без скидок;
	// никто из участников не заходил 7 дней — налог ×3, 14 дней — роспуск, склад сгорает; лидер не заходил
	// 14 дней — лидерство активному участнику со старшим рангом, активных нет — клан распустится по неактивности.
	static void DailyTick()
	{
		Load();
		int today = VB_TabletPlayers.DayKey();
		CheckWeek(today);
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		array<ref VB_Clan> gone = new array<ref VB_Clan>();
		array<ref VB_Clan> idleGone = new array<ref VB_Clan>();
		foreach (string id, VB_Clan c : s_Clans)
		{
			if (c.LastTaxDay == today)
				continue;
			int days = today - c.LastTaxDay;
			c.LastTaxDay = today;
			int lastActive = 0;
			foreach (VB_ClanMember m : c.Members)
			{
				m.Today = 0;
				// В сети сейчас — активен сегодня. Запись без даты (до обновления) считается сегодняшней.
				if (m.Online || m.LastDay <= 0)
					m.LastDay = today;
				if (m.LastDay > lastActive)
					lastActive = m.LastDay;
			}
			// Часы сервера ушли назад: день уже оплачен, налог не берётся второй раз.
			if (days < 1)
			{
				Save(c);
				continue;
			}
			int idle = today - lastActive;
			if (cfg.InactiveDissolveDays > 0 && idle >= cfg.InactiveDissolveDays)
			{
				idleGone.Insert(c);
				continue;
			}
			CheckLeader(c, today, cfg);
			if (days > 365)
				days = 1;
			int tax = cfg.TaxOf(c.Level);
			if (cfg.InactiveTaxDays > 0 && idle >= cfg.InactiveTaxDays)
				tax = tax * cfg.InactiveTaxMult;
			for (int i = 0; i < days; i++)
			{
				if (c.TaxFree > 0)
				{
					c.TaxFree = c.TaxFree - 1;
					continue;
				}
				// Налог выключен (0 в конфиге): склад не трогается, клан не распускается.
				if (tax <= 0)
					break;
				// Клан распускается, только когда заплатить полностью уже нечем. Остаток уходит в налог.
				// Так совпадает с подсказкой «налога хватит на N дней»: дни без налога + склад / налог.
				int pay = Math.Min(c.Bank, tax);
				c.Bank = c.Bank - pay;
				AddHistory(c, "", VB_ClanEvent.TAX, -pay);
				Log(c, VB_ClanEvent.TAX, "", "", "-" + pay.ToString());
				if (pay < tax)
				{
					gone.Insert(c);
					break;
				}
			}
			if (gone.Find(c) < 0)
			{
				Save(c);
				VB_TabletServer.PushClan(c);
				VB_TabletBank.PushHistory(c);
			}
		}
		foreach (VB_Clan dead : gone)
		{
			Print(VB_TABLET_TAG + " клан распущен за неуплату налога: " + dead.Title);
			Dissolve(dead);
		}
		foreach (VB_Clan idleClan : idleGone)
		{
			Print(VB_TABLET_TAG + " клан распущен за неактивность (никто не заходил " + cfg.InactiveDissolveDays.ToString() + " дней): " + idleClan.Title + ", склад сгорел: " + idleClan.Bank.ToString());
			Dissolve(idleClan);
		}
	}

	// Лидер не заходил LeaderAwayDays дней: лидерство переходит к участнику, заходившему за этот срок,
	// со старшим рангом (при равном ранге — с большим вкладом). Прежний лидер получает второй ранг сверху.
	protected static void CheckLeader(VB_Clan c, int today, VB_TabletConfig cfg)
	{
		if (cfg.LeaderAwayDays <= 0 || c.Ranks.Count() < 2)
			return;
		VB_ClanMember lead = null;
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.RankId == "leader")
			{
				lead = m;
				break;
			}
		}
		if (!lead || lead.Online || today - lead.LastDay < cfg.LeaderAwayDays)
			return;
		VB_ClanMember best = null;
		int bestRank = 0;
		foreach (VB_ClanMember o : c.Members)
		{
			if (o == lead)
				continue;
			if (!o.Online && today - o.LastDay >= cfg.LeaderAwayDays)
				continue;
			int r = c.RankIndex(o.RankId);
			if (!best || r < bestRank || (r == bestRank && o.Total > best.Total))
			{
				best = o;
				bestRank = r;
			}
		}
		if (!best)
			return;
		int away = today - lead.LastDay;
		lead.RankId = c.Ranks.Get(1).Id;
		best.RankId = "leader";
		Log(c, VB_ClanEvent.LEADER, best.PlayerName);
		Print(VB_TABLET_TAG + " клан " + c.Title + ": лидер " + lead.PlayerName + " не заходил " + away.ToString() + " дн., лидерство передано " + best.PlayerName);
	}
}
