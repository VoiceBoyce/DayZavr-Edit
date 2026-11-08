// Сервер принимает запросы планшета. Каждый запрос проверяется здесь: клиенту не доверяем.
class VB_TabletServer
{
	static const int MAX_PER_SEC = 20;

	protected static ref map<string, float> s_LastAt;
	protected static ref map<string, int> s_Count;

	static void Handle(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!player || !sender)
			return;
		// Запрос принимается только от самого игрока этого персонажа.
		PlayerIdentity owner = player.GetIdentity();
		if (!owner || owner.GetPlainId() != sender.GetPlainId())
			return;
		if (!s_LastAt)
		{
			s_LastAt = new map<string, float>();
			s_Count = new map<string, int>();
		}
		string uid = sender.GetPlainId();
		float now = GetGame().GetTickTime();
		// Окно в секунду: экран может спросить сразу несколько вещей, но не сотню.
		float start;
		int count;
		if (s_LastAt.Find(uid, start) && now - start < 1.0)
		{
			s_Count.Find(uid, count);
			if (count >= MAX_PER_SEC)
				return;
			s_Count.Set(uid, count + 1);
		}
		else
		{
			s_LastAt.Set(uid, now);
			s_Count.Set(uid, 1);
		}

		int op;
		if (!ctx.Read(op))
			return;
		switch (op)
		{
			case VB_TabletOp.HELLO:
				SendHello(player, sender);
				SendProfile(player, sender);
				MemberOnline(uid, true, sender.GetName());
				SendClan(player, sender);
				VB_TabletBank.OnHello(player, sender);
				break;
			case VB_TabletOp.PROFILE:
				SendProfile(player, sender);
				break;
			case VB_TabletOp.RATING:
				SendRating(player, sender);
				break;
			case VB_TabletOp.NAMETAGS:
				SendNameTags(player, sender);
				break;
			case VB_TabletOp.CLAN:
				SendClan(player, sender);
				break;
			case VB_TabletOp.CLAN_LIST:
				SendClanList(player, sender);
				break;
			case VB_TabletOp.PLAYER_CARD:
				SendCard(player, sender, ctx);
				break;
			default:
				if (VB_MapState.IsMapOp(op))
					VB_TabletMap.Handle(player, sender, op, ctx);
				else
					VB_TabletClanOps.Handle(player, sender, op, ctx);
				break;
		}
	}

	// Игрок ушёл: счётчики частоты по нему больше не нужны.
	static void Forget(string uid)
	{
		if (s_LastAt)
			s_LastAt.Remove(uid);
		if (s_Count)
			s_Count.Remove(uid);
	}

	static void SendResult(PlayerBase player, PlayerIdentity id, int code)
	{
		int op = VB_TabletOp.RESULT;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(code);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	// Персонаж игрока в сети по Steam64.
	static PlayerBase FindPlayer(string uid)
	{
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase p = PlayerBase.Cast(man);
			if (p && p.GetIdentity() && p.GetIdentity().GetPlainId() == uid)
				return p;
		}
		return null;
	}

	static PlayerBase FindByName(string name)
	{
		string want = name + "";
		want.ToLower();
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase p = PlayerBase.Cast(man);
			if (!p || !p.GetIdentity())
				continue;
			string n = p.GetIdentity().GetName();
			n.ToLower();
			if (n == want)
				return p;
		}
		return null;
	}

	// Вход и выход участника. name при входе: игрок мог сменить ник, список клана показывает текущий.
	static void MemberOnline(string uid, bool online, string name = "")
	{
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
			return;
		VB_ClanMember m = c.Member(uid);
		if (!m)
			return;
		bool renamed = online && name != "" && m.PlayerName != name;
		if (m.Online == online && !renamed)
			return;
		m.Online = online;
		if (!online)
			m.LastSeen = VB_TabletClans.Stamp();
		if (renamed)
		{
			m.PlayerName = name;
			VB_TabletClans.SaveSoon(c);
		}
		PushClan(c);
	}

	static void SendClan(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		int op = VB_TabletOp.CLAN;
		bool has = c != null;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(has);
		if (c)
			c.WriteHead(rpc);
		array<ref VB_ClanInvite> invites = VB_TabletClans.InvitesOf(uid);
		int n = invites.Count();
		rpc.Write(n);
		foreach (VB_ClanInvite inv : invites)
			inv.Write(rpc);
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		string applied = rec.AppliedClan;
		// Клана, куда подана заявка, уже нет (роспуск): пометка снимается, клиент не покажет «заявка в —».
		if (applied != "" && !VB_TabletClans.Find(applied))
		{
			rec.AppliedClan = "";
			rec.Dirty = true;
			applied = "";
		}
		rpc.Write(applied);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	static void PushClan(VB_Clan c)
	{
		foreach (VB_ClanMember m : c.Members)
		{
			if (!m.Online)
				continue;
			PlayerBase p = FindPlayer(m.Uid);
			if (p)
				SendClan(p, p.GetIdentity());
		}
	}

	static void SendNoClan(string uid)
	{
		PlayerBase p = FindPlayer(uid);
		if (p)
			SendClan(p, p.GetIdentity());
	}

	// Показатели другого игрока. Discord виден только у тех, кто в клане.
	static void SendCard(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		string uid;
		string name;
		if (!ctx.Read(uid) || !ctx.Read(name))
			return;
		// uid попадает в путь файла: только Steam64.
		if (uid != "" && !VB_TabletCheck.IsSteam64(uid))
			return;
		if (name.LengthUtf8() > 64)
			return;
		if (uid == "")
		{
			array<ref VB_TabletRatingRow> rows = VB_TabletPlayers.Sorted();
			foreach (VB_TabletRatingRow row : rows)
			{
				if (row.PlayerName == name)
				{
					uid = row.Uid;
					break;
				}
			}
		}
		if (uid == "")
			return;
		VB_TabletRecord rec = VB_TabletPlayers.Peek(uid);
		if (!rec)
			return;
		VB_TabletStats st = rec.ToStats();
		st.PlayerName = name;
		st.Place = VB_TabletPlayers.PlaceOf(uid);
		PlayerBase target = FindPlayer(uid);
		if (target)
			st.LifeSec = target.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		if (!VB_TabletClans.OfPlayer(uid))
			st.Discord = "";
		int op = VB_TabletOp.PLAYER_CARD;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(name);
		st.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	static void SendClanList(PlayerBase player, PlayerIdentity id)
	{
		bool admin = VB_TabletConfig.Get().IsAdmin(id.GetPlainId());
		map<string, ref VB_Clan> all = VB_TabletClans.All();
		int op = VB_TabletOp.CLAN_LIST;
		int n = all.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (string key, VB_Clan c : all)
		{
			VB_ClanRow row = new VB_ClanRow();
			row.Id = c.Id;
			row.Title = c.Title;
			row.Tag = c.Tag;
			row.Slogan = c.Slogan;
			row.EmbShape = c.EmbShape;
			row.EmbSymbol = c.EmbSymbol;
			row.Color = c.Color;
			row.Level = c.Level;
			row.MemberCount = c.Members.Count();
			row.WeekInfluence = c.WeekInfluence;
			row.Influence = c.Influence;
			row.Open = c.Open;
			row.Leader = c.LeaderName();
			row.Discord = c.Discord;
			if (admin)
			{
				row.Bank = c.Bank;
				row.TaxFree = c.TaxFree;
			}
			row.Write(rpc);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void SendHello(PlayerBase player, PlayerIdentity sender)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		int op = VB_TabletOp.HELLO;
		string name = cfg.ServerName;
		bool admin = cfg.IsAdmin(sender.GetPlainId());
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(name);
		rpc.Write(admin);
		cfg.Rates.Write(rpc);
		int priceName = cfg.PriceName;
		int priceTag = cfg.PriceTag;
		int priceEmblem = cfg.PriceEmblem;
		int priceColor = cfg.PriceColor;
		rpc.Write(priceName);
		rpc.Write(priceTag);
		rpc.Write(priceEmblem);
		rpc.Write(priceColor);
		int clanPrice = cfg.ClanPrice;
		int clanRank = cfg.ClanRank;
		int clanMax = cfg.ClanMaxMembers;
		int maxClans = cfg.MaxClans;
		int bankMax = cfg.BankMax;
		int clanTax = cfg.ClanTax;
		rpc.Write(clanPrice);
		rpc.Write(clanRank);
		rpc.Write(clanMax);
		rpc.Write(maxClans);
		rpc.Write(bankMax);
		rpc.Write(clanTax);
		// Награды для экранов рейтинга и контрактов: не вшиты в клиент, берутся из конфига.
		array<int> prizes = cfg.WeekPrizes;
		array<int> rewardMy = cfg.RewardMy;
		rpc.Write(prizes);
		rpc.Write(rewardMy);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, sender);
	}

	static void SendProfile(PlayerBase player, PlayerIdentity sender)
	{
		VB_TabletRecord rec = VB_TabletPlayers.Get(sender.GetPlainId(), sender.GetName());
		VB_TabletPlayers.UpdateFromPlayer(rec, player);
		VB_TabletStats st = rec.ToStats();
		st.LifeSec = player.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		st.Place = VB_TabletPlayers.PlaceOf(sender.GetPlainId());
		st.Cash = VB_TabletMoney.Count(player);
		int op = VB_TabletOp.PROFILE;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		st.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, sender);
	}

	// Таблица ников: собирается не чаще раза в 10 секунд и отдаётся всем, кто спросит.
	protected static const int TAGS_CACHE_MS = 10000;
	protected static ref array<string> s_TagIds;
	protected static ref array<int> s_TagRanks;
	protected static ref array<string> s_TagClanOf;
	protected static ref array<string> s_TagClans;
	protected static ref array<string> s_TagShapes;
	protected static ref array<string> s_TagSymbols;
	protected static ref array<int> s_TagColors;
	protected static int s_TagAt;

	protected static void BuildNameTags()
	{
		s_TagAt = GetGame().GetTime();
		s_TagIds = new array<string>();
		s_TagRanks = new array<int>();
		s_TagClanOf = new array<string>();
		s_TagClans = new array<string>();
		s_TagShapes = new array<string>();
		s_TagSymbols = new array<string>();
		s_TagColors = new array<int>();
		VB_TabletRates rates = VB_TabletConfig.Get().Rates;
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerIdentity ident = man.GetIdentity();
			if (!ident)
				continue;
			string uid = ident.GetPlainId();
			int rank = 0;
			VB_TabletRecord rec = VB_TabletPlayers.Peek(uid);
			if (rec)
			{
				int xp = rec.ToStats().Xp(rates);
				rank = rates.RankOf(xp);
			}
			string cid = "";
			VB_Clan c = VB_TabletClans.OfPlayer(uid);
			if (c)
			{
				cid = c.Id;
				if (s_TagClans.Find(cid) < 0)
				{
					s_TagClans.Insert(cid);
					s_TagShapes.Insert(c.EmbShape);
					s_TagSymbols.Insert(c.EmbSymbol);
					s_TagColors.Insert(c.Color);
				}
			}
			s_TagIds.Insert(uid);
			s_TagRanks.Insert(rank);
			s_TagClanOf.Insert(cid);
		}
	}

	protected static void SendNameTags(PlayerBase player, PlayerIdentity sender)
	{
		int now = GetGame().GetTime();
		if (!s_TagIds || now - s_TagAt > TAGS_CACHE_MS || now < s_TagAt)
			BuildNameTags();
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		int op = VB_TabletOp.NAMETAGS;
		bool on = cfg.NameTags != 0;
		float range = cfg.NameTagRange;
		int n = s_TagIds.Count();
		int m = s_TagClans.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(on);
		rpc.Write(range);
		rpc.Write(n);
		int i;
		for (i = 0; i < n; i++)
		{
			string id = s_TagIds.Get(i);
			int rank = s_TagRanks.Get(i);
			string cid = s_TagClanOf.Get(i);
			rpc.Write(id);
			rpc.Write(rank);
			rpc.Write(cid);
		}
		rpc.Write(m);
		for (i = 0; i < m; i++)
		{
			string key = s_TagClans.Get(i);
			string shape = s_TagShapes.Get(i);
			string symbol = s_TagSymbols.Get(i);
			int color = s_TagColors.Get(i);
			rpc.Write(key);
			rpc.Write(shape);
			rpc.Write(symbol);
			rpc.Write(color);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, sender);
	}

	protected static void SendRating(PlayerBase player, PlayerIdentity sender)
	{
		const int ROWS = 13;
		VB_TabletRecord rec = VB_TabletPlayers.Get(sender.GetPlainId(), sender.GetName());
		VB_TabletPlayers.UpdateFromPlayer(rec, player);
		array<ref VB_TabletRatingRow> list = VB_TabletPlayers.Sorted();
		int total = list.Count();
		int count = Math.Min(ROWS, total);
		int op = VB_TabletOp.RATING;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(total);
		rpc.Write(count);
		array<int> perRank = new array<int>();
		for (int r = 0; r < VB_RANK_COUNT; r++)
			perRank.Insert(0);
		VB_TabletRates rates = VB_TabletConfig.Get().Rates;
		for (int i = 0; i < total; i++)
		{
			VB_TabletRatingRow row = list.Get(i);
			int ri = rates.RankOf(row.Xp);
			perRank.Set(ri, perRank.Get(ri) + 1);
			if (i < count)
			{
				string name = row.PlayerName;
				int xp = row.Xp;
				string tag = "";
				VB_Clan rowClan = VB_TabletClans.OfPlayer(row.Uid);
				if (rowClan)
					tag = rowClan.Tag;
				rpc.Write(name);
				rpc.Write(xp);
				rpc.Write(tag);
			}
		}
		for (int k = 0; k < VB_RANK_COUNT; k++)
		{
			int n = perRank.Get(k);
			rpc.Write(n);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, sender);
	}

	// Игрок за убийцей или источником удара: сам игрок, его оружие, нож в руках.
	static PlayerBase PlayerOf(Object o)
	{
		if (!o)
			return null;
		PlayerBase player = PlayerBase.Cast(o);
		if (player)
			return player;
		EntityAI e = EntityAI.Cast(o);
		if (e)
			return PlayerBase.Cast(e.GetHierarchyRootPlayer());
		return null;
	}

	// Счётчики добычи: мутанты проверяются первыми, они часто наследуются от заражённых.
	static void OnCreatureKilled(EntityAI victim, PlayerBase player)
	{
		if (!victim || !player || !player.GetIdentity())
			return;
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		PlayerIdentity id = player.GetIdentity();
		VB_TabletRecord rec = VB_TabletPlayers.Get(id.GetPlainId(), id.GetName());
		foreach (VB_TabletKillGroup group : cfg.KillGroups)
		{
			if (IsListed(victim, group.Classes))
				VB_TabletContracts.OnKill(rec, group.Kind);
		}
		if (IsListed(victim, cfg.Bosses))
			rec.AchBoss++;
		string kind;
		if (IsListed(victim, cfg.Mutants))
		{
			rec.Mutants++;
			kind = "mutant";
		}
		else if (IsListed(victim, cfg.Zombies))
		{
			rec.Zombies++;
			kind = "zombie";
		}
		else if (IsListed(victim, cfg.Animals))
		{
			rec.Animals++;
			kind = "animal";
		}
		else
			return;
		rec.Dirty = true;
		VB_TabletContracts.OnKill(rec, kind);
	}

	static bool IsListed(EntityAI victim, array<string> classes)
	{
		return IsListedObj(victim, classes);
	}

	// То же для любого объекта (цель действия может быть не EntityAI).
	static bool IsListedObj(Object victim, array<string> classes)
	{
		string type = victim.GetType();
		foreach (string cls : classes)
		{
			// Звёздочка на конце: совпадение по началу имени, годится и для будущих существ.
			int star = cls.Length() - 1;
			if (star > 0 && cls.Get(star) == "*")
			{
				if (type.IndexOf(cls.Substring(0, star)) == 0)
					return true;
			}
			else if (victim.IsKindOf(cls))
			{
				return true;
			}
		}
		return false;
	}
}

// Кто убил. Животным движок передаёт в EEKilled убийцей само животное (проверено на оленях 1.30),
// поэтому стрелок запоминается по попаданиям. Порядок EEHitBy и EEKilled на смертельном ударе не важен:
// смерть без известного стрелка дожидается следующего EEHitBy.
modded class ZombieBase
{
	protected PlayerBase m_VbHitBy;		// без ref: игрок мог выйти, тогда null
	protected bool m_VbKillOpen;
	protected bool m_VbCounted;

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!GetGame().IsDedicatedServer())
			return;
		PlayerBase p = VB_TabletServer.PlayerOf(source);
		if (p)
			m_VbHitBy = p;
		if (m_VbKillOpen && !m_VbCounted && m_VbHitBy)
		{
			m_VbKillOpen = false;
			m_VbCounted = true;
			VB_TabletServer.OnCreatureKilled(this, m_VbHitBy);
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!GetGame().IsDedicatedServer() || m_VbCounted)
			return;
		PlayerBase p = VB_TabletServer.PlayerOf(killer);
		if (!p)
			p = m_VbHitBy;
		if (!p)
		{
			m_VbKillOpen = true;
			return;
		}
		m_VbCounted = true;
		VB_TabletServer.OnCreatureKilled(this, p);
	}
}

modded class AnimalBase
{
	protected PlayerBase m_VbHitBy;
	protected bool m_VbKillOpen;
	protected bool m_VbCounted;

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!GetGame().IsDedicatedServer())
			return;
		PlayerBase p = VB_TabletServer.PlayerOf(source);
		if (p)
			m_VbHitBy = p;
		if (m_VbKillOpen && !m_VbCounted && m_VbHitBy)
		{
			m_VbKillOpen = false;
			m_VbCounted = true;
			VB_TabletServer.OnCreatureKilled(this, m_VbHitBy);
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!GetGame().IsDedicatedServer() || m_VbCounted)
			return;
		PlayerBase p = VB_TabletServer.PlayerOf(killer);
		if (!p)
			p = m_VbHitBy;
		if (!p)
		{
			m_VbKillOpen = true;
			return;
		}
		m_VbCounted = true;
		VB_TabletServer.OnCreatureKilled(this, p);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == VB_TABLET_RPC_C2S)
		{
			if (GetGame().IsDedicatedServer())
				VB_TabletServer.Handle(this, sender, ctx);
			return;
		}
		super.OnRPC(sender, rpc_type, ctx);
	}
}
