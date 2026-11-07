// Контракты: клановые и личные. Список в конфиге, набор новый каждый день в 00:00 по серверу.
// Клановые лежат в файле клана и зачисляются сами. Личные лежат в файле игрока, награду забирают кнопкой;
// не забранное до обновления зачисляется при смене набора.
class VB_TabletContracts
{
	protected static ref array<string> s_Dirty = new array<string>();	// кланы с прогрессом, файл ещё не записан

	// Сколько контрактов каждой сложности на уровне клана. По 3 числа на уровень.
	protected static int MixOf(bool my, int level, int tier)
	{
		array<int> clan = {6, 0, 0, 5, 1, 0, 4, 2, 0, 3, 3, 0, 3, 2, 1};
		array<int> mine = {3, 0, 0, 3, 0, 0, 2, 1, 0, 2, 1, 0, 1, 1, 1};
		int lv = Math.Clamp(level, 1, VB_CLAN_LEVELS) - 1;
		if (my)
			return mine.Get(lv * 3 + tier);
		return clan.Get(lv * 3 + tier);
	}

	// Контракт без счётчика никогда не выполнится, такой не выдаём.
	protected static bool Usable(VB_TabletContractDef d)
	{
		if (d.Need <= 0)
			return false;
		if (d.Kind == "mutant")
			return VB_TabletConfig.Get().Mutants.Count() > 0;
		return d.Kind == "zombie" || d.Kind == "animal" || d.Kind == "km" || d.Kind == "hours";
	}

	protected static VB_ClanContract Make(VB_TabletContractDef d, bool my)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		VB_ClanContract k = new VB_ClanContract();
		k.Code = d.Code;
		k.Tier = Math.Clamp(d.Tier, 0, 2);
		k.Kind = d.Kind;
		k.Need = d.Need;
		k.Reward = d.Reward;
		k.Ico = d.Ico;
		k.Title = d.Title;
		k.Hint = d.Hint;
		if (my)
			k.Xp = cfg.XpMy.Get(k.Tier);
		else
		{
			k.Cash = d.Cash;
			k.Xp = cfg.Rates.XpContract;
		}
		return k;
	}

	protected static void Pick(array<ref VB_TabletContractDef> defs, int tier, int count, bool my, array<ref VB_ClanContract> into)
	{
		array<ref VB_TabletContractDef> pool = new array<ref VB_TabletContractDef>();
		foreach (VB_TabletContractDef d : defs)
		{
			if (d.Tier == tier && Usable(d))
				pool.Insert(d);
		}
		for (int i = 0; i < count && pool.Count() > 0; i++)
		{
			int at = Math.RandomInt(0, pool.Count());
			into.Insert(Make(pool.Get(at), my));
			pool.Remove(at);
		}
	}

	protected static void Fill(array<ref VB_TabletContractDef> defs, int level, bool my, array<ref VB_ClanContract> into)
	{
		into.Clear();
		for (int tier = 0; tier < 3; tier++)
			Pick(defs, tier, MixOf(my, level, tier), my, into);
	}

	protected static void MarkDirty(VB_Clan c)
	{
		if (s_Dirty.Find(c.Id) < 0)
			s_Dirty.Insert(c.Id);
	}

	protected static void EnsureDay(VB_Clan c)
	{
		int today = VB_TabletPlayers.DayKey();
		if (c.ContractsDay == today)
			return;
		c.ContractsDay = today;
		Fill(VB_TabletConfig.Get().ClanContracts, c.Level, false, c.Contracts);
		MarkDirty(c);
	}

	// Набор личных привязан к игроку на день: выход и вступление в клан его не обновляют.
	protected static void EnsureMy(VB_TabletRecord rec, VB_Clan c)
	{
		int today = VB_TabletPlayers.DayKey();
		if (rec.MyDay == today)
			return;
		foreach (VB_ClanContract k : rec.MyContracts)
		{
			if (k.Stage == VB_ContractStage.READY)
				PayMy(c, rec, k);
		}
		rec.MyDay = today;
		Fill(VB_TabletConfig.Get().MyContracts, c.Level, true, rec.MyContracts);
		rec.Dirty = true;
		rec.MyDirty = true;
	}

	protected static void AddPart(VB_ClanContract k, string uid, int amount)
	{
		int i = k.Uids.Find(uid);
		if (i < 0)
		{
			k.Uids.Insert(uid);
			k.Parts.Insert(amount);
			return;
		}
		k.Parts.Set(i, k.Parts.Get(i) + amount);
	}

	protected static void LevelUp(VB_Clan c)
	{
		array<int> levels = VB_TabletRates.Levels();
		bool up = false;
		while (c.Level < levels.Count() && c.Influence >= levels.Get(c.Level))
		{
			c.Level = c.Level + 1;
			up = true;
		}
		if (!up)
			return;
		VB_TabletClans.Log(c, VB_ClanEvent.LEVEL, "", c.Level.ToString());
		VB_TabletBank.NotifyClan(c, "#STR_VB_TAB_C_LEVEL_UP", c.Level.ToString(), 0);
	}

	protected static void AddInfluence(VB_Clan c, int value)
	{
		c.Influence = c.Influence + value;
		c.WeekInfluence = c.WeekInfluence + value;
		LevelUp(c);
	}

	// Клановый контракт выполнен: влияние по вкладу, деньги на склад до лимита, опыт всем с долей.
	protected static void Complete(VB_Clan c, VB_ClanContract k)
	{
		k.Have = k.Need;
		k.Stage = VB_ContractStage.DONE;
		int sum = 0;
		int i;
		for (i = 0; i < k.Parts.Count(); i++)
			sum += k.Parts.Get(i);
		int given = 0;
		int top = -1;
		int best = -1;
		for (i = 0; i < k.Uids.Count(); i++)
		{
			VB_ClanMember m = c.Member(k.Uids.Get(i));
			if (!m)
				continue;
			int part = k.Parts.Get(i);
			int share = 0;
			if (sum > 0)
				share = k.Reward * part / sum;
			m.AddToday(share, VB_TabletClans.TodayIdx());
			m.Total = m.Total + share;
			given += share;
			if (part > best)
			{
				best = part;
				top = i;
			}
			VB_TabletPlayers.AddContract(m.Uid, 0);
		}
		if (top >= 0 && given < k.Reward)
		{
			VB_ClanMember lead = c.Member(k.Uids.Get(top));
			lead.AddToday(k.Reward - given, VB_TabletClans.TodayIdx());
			lead.Total = lead.Total + k.Reward - given;
		}
		int paid = Math.Min(k.Cash, Math.Max(0, VB_TabletConfig.Get().BankMax - c.Bank));
		if (paid > 0)
		{
			c.Bank = c.Bank + paid;
			VB_TabletClans.AddHistory(c, "", VB_ClanEvent.CONTRACT, paid);
		}
		VB_TabletClans.Log(c, VB_ClanEvent.CONTRACT, "", k.Title, "+" + k.Reward.ToString());
		AddInfluence(c, k.Reward);
		VB_TabletBank.NotifyClan(c, "#STR_VB_TAB_C_N_CLAN_DONE", k.Title, 1);
		if (paid < k.Cash)
		{
			int lost = k.Cash - paid;
			VB_TabletBank.NotifyClan(c, "#STR_VB_TAB_C_N_BANK_FULL", lost.ToString(), 2);
		}
		VB_TabletClans.Save(c);
		VB_TabletServer.PushClan(c);
		VB_TabletBank.PushHistory(c);
		PushClan(c);
	}

	// Личная награда: влияние клану, столько же вклада игроку и опыт в общий ранг. Денег нет.
	protected static void PayMy(VB_Clan c, VB_TabletRecord rec, VB_ClanContract k)
	{
		k.Stage = VB_ContractStage.DONE;
		VB_ClanMember m = c.Member(rec.Uid);
		if (m)
		{
			m.AddToday(k.Reward, VB_TabletClans.TodayIdx());
			m.Total = m.Total + k.Reward;
		}
		AddInfluence(c, k.Reward);
		VB_TabletPlayers.AddContract(rec.Uid, k.Xp - VB_TabletConfig.Get().Rates.XpContract);
		rec.Dirty = true;
		rec.MyDirty = true;
		MarkDirty(c);
	}

	// Прогресс. absolute: value это текущее значение показателя, а не прибавка.
	protected static void Add(VB_TabletRecord rec, string kind, int value, bool absolute)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(rec.Uid);
		if (!c)
			return;
		EnsureDay(c);
		EnsureMy(rec, c);
		int add;
		foreach (VB_ClanContract k : c.Contracts)
		{
			if (k.Kind != kind || k.Stage != VB_ContractStage.ACTIVE)
				continue;
			add = value;
			if (absolute)
				add = value - k.Have;
			add = Math.Min(add, k.Need - k.Have);
			if (add <= 0)
				continue;
			k.Have = k.Have + add;
			AddPart(k, rec.Uid, add);
			MarkDirty(c);
			if (k.Have >= k.Need)
				Complete(c, k);
		}
		foreach (VB_ClanContract q : rec.MyContracts)
		{
			if (q.Kind != kind || q.Stage != VB_ContractStage.ACTIVE)
				continue;
			add = value;
			if (absolute)
				add = value - q.Have;
			add = Math.Min(add, q.Need - q.Have);
			if (add <= 0)
				continue;
			q.Have = q.Have + add;
			rec.Dirty = true;
			rec.MyDirty = true;
			if (q.Have >= q.Need)
			{
				q.Stage = VB_ContractStage.READY;
				PlayerBase p = VB_TabletServer.FindPlayer(rec.Uid);
				if (p)
				{
					VB_TabletBank.Notify(p, p.GetIdentity(), "#STR_VB_TAB_C_N_MY_READY", q.Title, 1);
					Send(p, p.GetIdentity());
				}
			}
		}
	}

	static void OnKill(VB_TabletRecord rec, string kind)
	{
		Add(rec, kind, 1, false);
	}

	// Метры из статистики движка копятся до целых километров.
	static void OnWalk(VB_TabletRecord rec, float meters)
	{
		rec.WalkLeft = rec.WalkLeft + meters;
		if (rec.WalkLeft < 1000)
			return;
		int km = rec.WalkLeft / 1000;
		rec.WalkLeft = rec.WalkLeft - km * 1000;
		Add(rec, "km", km, false);
	}

	// Прожить без смерти: засчитывается лучшая жизнь за день.
	static void OnLife(VB_TabletRecord rec, int lifeSec)
	{
		int hours = lifeSec / 3600;
		if (hours > 0)
			Add(rec, "hours", hours, true);
	}

	protected static int SecondsLeft()
	{
		int h;
		int m;
		int s;
		GetHourMinuteSecond(h, m, s);
		return 86400 - (h * 3600 + m * 60 + s);
	}

	static void Send(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (c)
		{
			EnsureDay(c);
			EnsureMy(rec, c);
		}
		int op = VB_TabletOp.CONTRACTS;
		int left = SecondsLeft();
		bool free = rec.MyReplaceDay != VB_TabletPlayers.DayKey();
		int n = 0;
		if (c)
			n = c.Contracts.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(left);
		rpc.Write(free);
		rpc.Write(n);
		if (c)
		{
			foreach (VB_ClanContract k : c.Contracts)
				k.Write(rpc);
		}
		// Без клана личные контракты недоступны.
		n = 0;
		if (c)
			n = rec.MyContracts.Count();
		rpc.Write(n);
		if (c)
		{
			foreach (VB_ClanContract q : rec.MyContracts)
				q.Write(rpc);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
		rec.MyDirty = false;
	}

	protected static void PushClan(VB_Clan c)
	{
		foreach (VB_ClanMember m : c.Members)
		{
			if (!m.Online)
				continue;
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (p)
				Send(p, p.GetIdentity());
		}
	}

	static void Claim(PlayerBase player, PlayerIdentity id, int index)
	{
		string uid = id.GetPlainId();
		if (!VB_TabletBank.Allow(uid))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.RATE);
			return;
		}
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		EnsureDay(c);
		EnsureMy(rec, c);
		if (index < 0 || index >= rec.MyContracts.Count() || rec.MyContracts.Get(index).Stage != VB_ContractStage.READY)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		VB_ClanContract k = rec.MyContracts.Get(index);
		PayMy(c, rec, k);
		// Сначала файл игрока: после обрыва награда не будет выдана второй раз.
		VB_TabletPlayers.Save(rec);
		VB_TabletClans.Save(c);
		VB_TabletBank.Notify(player, id, "#STR_VB_TAB_C_N_GOT", k.Reward.ToString(), 1);
		VB_TabletServer.PushClan(c);
		Send(player, id);
		VB_TabletServer.SendProfile(player, id);
	}

	// Одна бесплатная замена в день, только для незавершённого задания.
	static void Replace(PlayerBase player, PlayerIdentity id, int index)
	{
		string uid = id.GetPlainId();
		if (!VB_TabletBank.Allow(uid))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.RATE);
			return;
		}
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		EnsureMy(rec, c);
		int today = VB_TabletPlayers.DayKey();
		if (rec.MyReplaceDay == today)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		if (index < 0 || index >= rec.MyContracts.Count() || rec.MyContracts.Get(index).Stage != VB_ContractStage.ACTIVE)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		int tier = rec.MyContracts.Get(index).Tier;
		VB_TabletContractDef fresh = null;
		array<ref VB_TabletContractDef> defs = VB_TabletConfig.Get().MyContracts;
		foreach (VB_TabletContractDef d : defs)
		{
			if (fresh || d.Tier != tier || !Usable(d))
				continue;
			bool used = false;
			foreach (VB_ClanContract k : rec.MyContracts)
			{
				if (k.Code == d.Code)
					used = true;
			}
			if (!used)
				fresh = d;
		}
		if (!fresh)
		{
			VB_TabletBank.Notify(player, id, "#STR_VB_TAB_C_N_NO_REPLACE", "", 2);
			return;
		}
		rec.MyContracts.Set(index, Make(fresh, true));
		rec.MyReplaceDay = today;
		rec.Dirty = true;
		VB_TabletBank.Notify(player, id, "#STR_VB_TAB_C_N_REPLACED", "", 1);
		Send(player, id);
	}

	// Раз в 30 секунд: смена дня у игроков в сети, запись прогресса, рассылка.
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
			string uid = id.GetPlainId();
			VB_Clan c = VB_TabletClans.OfPlayer(uid);
			if (!c)
				continue;
			VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
			EnsureDay(c);
			EnsureMy(rec, c);
			if (rec.MyDirty && s_Dirty.Find(c.Id) < 0)
				Send(player, id);
		}
		foreach (string clanId : s_Dirty)
		{
			VB_Clan dirty = VB_TabletClans.Find(clanId);
			if (!dirty)
				continue;
			VB_TabletClans.Save(dirty);
			VB_TabletServer.PushClan(dirty);
			PushClan(dirty);
		}
		s_Dirty.Clear();
	}
}
