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
				MemberOnline(uid, true);
				SendClan(player, sender);
				VB_TabletBank.OnHello(player, sender);
				break;
			case VB_TabletOp.PROFILE:
				SendProfile(player, sender);
				break;
			case VB_TabletOp.RATING:
				SendRating(player, sender);
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

	static void MemberOnline(string uid, bool online)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
			return;
		VB_ClanMember m = c.Member(uid);
		if (!m || m.Online == online)
			return;
		m.Online = online;
		if (!online)
			m.LastSeen = VB_TabletClans.Stamp();
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

	// Счётчики добычи: мутанты проверяются первыми, они часто наследуются от заражённых.
	static void OnCreatureKilled(EntityAI victim, Object killer)
	{
		if (!victim || !killer)
			return;
		PlayerBase player = PlayerBase.Cast(killer);
		EntityAI source = EntityAI.Cast(killer);
		if (!player && source)
			player = PlayerBase.Cast(source.GetHierarchyRootPlayer());
		if (!player || !player.GetIdentity())
			return;
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		PlayerIdentity id = player.GetIdentity();
		VB_TabletRecord rec = VB_TabletPlayers.Get(id.GetPlainId(), id.GetName());
		foreach (VB_TabletKillGroup group : cfg.KillGroups)
		{
			if (IsListed(victim, group.Classes))
				VB_TabletContracts.OnKill(rec, group.Kind);
		}
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

modded class ZombieBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (GetGame().IsDedicatedServer())
			VB_TabletServer.OnCreatureKilled(this, killer);
	}
}

modded class AnimalBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (GetGame().IsDedicatedServer())
			VB_TabletServer.OnCreatureKilled(this, killer);
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
