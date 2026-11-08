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

	protected static void LoadOne(string file)
	{
		string path = DIR_CLANS + "/" + file;
		VB_Clan c = new VB_Clan();
		string error;
		if (!JsonFileLoader<VB_Clan>.LoadFile(path, c, error) || c.Id == "")
		{
			Print(VB_TABLET_TAG + " ошибка в файле клана " + file + ": " + error);
			return;
		}
		s_Clans.Set(c.Id, c);
		foreach (VB_ClanMember m : c.Members)
		{
			m.Online = false;
			s_ByUid.Set(m.Uid, c.Id);
		}
		string num = c.Id.Substring(1, c.Id.Length() - 1);
		s_NextId = Math.Max(s_NextId, num.ToInt() + 1);
	}

	static bool Save(VB_Clan c)
	{
		string path = DIR_CLANS + "/" + c.Id + ".json";
		if (FileExist(path))
			CopyFile(path, path + ".bak");
		string error;
		if (!JsonFileLoader<VB_Clan>.SaveFile(path, c, error))
		{
			Print(VB_TABLET_TAG + " не записан клан " + c.Id + ": " + error);
			return false;
		}
		return true;
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
		StoreWeek(path, week);
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
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		for (int place = 0; place < top.Count(); place++)
		{
			VB_Clan t = top.Get(place);
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
		}
	}

	protected static string VBT_NumText(int v)
	{
		return v.ToString();
	}

	protected static void StoreWeek(string path, int week)
	{
		FileHandle h = OpenFile(path, FileMode.WRITE);
		if (!h)
			return;
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
		string low = title + "";
		low.ToLower();
		foreach (string id, VB_Clan c : All())
		{
			if (c == except)
				continue;
			string other = c.Title + "";
			other.ToLower();
			if (other == low)
				return false;
		}
		return true;
	}

	static bool TagFree(string tag, VB_Clan except)
	{
		foreach (string id, VB_Clan c : All())
		{
			if (c != except && c.Tag == tag)
				return false;
		}
		return true;
	}

	static bool EmblemFree(string shape, string symbol, VB_Clan except)
	{
		foreach (string id, VB_Clan c : All())
		{
			if (c != except && c.EmbShape == shape && c.EmbSymbol == symbol)
				return false;
		}
		return true;
	}

	static bool HasBanned(string text)
	{
		string low = text + "";
		low.ToLower();
		foreach (string w : VB_TabletConfig.Get().BannedWords)
		{
			string bad = w + "";
			bad.ToLower();
			if (bad != "" && low.Contains(bad))
				return true;
		}
		return false;
	}

	// Клан игрока изменился: всем в сети новая шапка.
	static void Create(PlayerBase player, PlayerIdentity id, string title, string tag, string slogan, string shape, string symbol, int color)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		string uid = id.GetPlainId();
		int res = CheckCreate(player, uid, title, tag, slogan, shape, symbol);
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
		Save(c);
		VB_TabletPlayers.ClearApply(uid);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.CREATED);
		VB_TabletServer.PushClan(c);
		VB_TabletServer.SendProfile(player, id);
	}

	protected static int CheckCreate(PlayerBase player, string uid, string title, string tag, string slogan, string shape, string symbol)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		if (OfPlayer(uid))
			return VB_TabletResult.HAS_CLAN;
		int len = title.LengthUtf8();
		if (len < 3 || len > 24 || tag.Length() < 2 || tag.Length() > 4 || slogan.LengthUtf8() > 60)
			return VB_TabletResult.BAD_INPUT;
		if (HasBanned(title) || HasBanned(tag))
			return VB_TabletResult.BANNED_WORD;
		if (All().Count() >= cfg.MaxClans)
			return VB_TabletResult.SERVER_FULL;
		if (!NameFree(title, null))
			return VB_TabletResult.NAME_TAKEN;
		if (!TagFree(tag, null))
			return VB_TabletResult.TAG_TAKEN;
		if (!EmblemFree(shape, symbol, null))
			return VB_TabletResult.EMBLEM_TAKEN;
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, "");
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
		m.Online = true;
		m.Discord = VB_TabletPlayers.Get(uid, name).Discord;
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
		string path = DIR_CLANS + "/" + c.Id + ".json";
		DeleteFile(path);
		DeleteFile(path + ".bak");
		s_Clans.Remove(c.Id);
		foreach (string gone : uids)
			VB_TabletServer.SendNoClan(gone);
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
		return list;
	}

	static void DropInvite(string uid, string clanId)
	{
		array<ref VB_ClanInvite> list = InvitesOf(uid);
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			if (list.Get(i).ClanId == clanId)
				list.Remove(i);
		}
	}

	// Новый день: налог и обнуление счётчиков «сегодня». Пропущенные дни доначисляются.
	static void DailyTick()
	{
		Load();
		int today = VB_TabletPlayers.DayKey();
		CheckWeek(today);
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		array<ref VB_Clan> gone = new array<ref VB_Clan>();
		foreach (string id, VB_Clan c : s_Clans)
		{
			if (c.LastTaxDay == today)
				continue;
			int days = today - c.LastTaxDay;
			if (days < 1 || days > 365)
				days = 1;
			c.LastTaxDay = today;
			foreach (VB_ClanMember m : c.Members)
				m.Today = 0;
			for (int i = 0; i < days; i++)
			{
				if (c.TaxFree > 0)
				{
					c.TaxFree = c.TaxFree - 1;
					continue;
				}
				int pay = Math.Min(c.Bank, cfg.ClanTax);
				c.Bank = c.Bank - pay;
				AddHistory(c, "", VB_ClanEvent.TAX, -pay);
				Log(c, VB_ClanEvent.TAX, "", "", "-" + pay.ToString());
				if (c.Bank <= 0)
				{
					gone.Insert(c);
					break;
				}
			}
			if (gone.Find(c) < 0)
			{
				Save(c);
				VB_TabletServer.PushClan(c);
			}
		}
		foreach (VB_Clan dead : gone)
		{
			Print(VB_TABLET_TAG + " клан распущен за неуплату налога: " + dead.Title);
			Dissolve(dead);
		}
	}
}
