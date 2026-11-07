// Действия игроков с кланом. Права проверяются здесь по рангу на сервере, что бы ни прислал клиент.
class VB_TabletClanOps
{
	static const float CHAT_GAP_SEC = 2.0;
	protected static ref map<string, float> s_ChatAt;

	static void Handle(PlayerBase player, PlayerIdentity id, int op, ParamsReadContext ctx)
	{
		// Банкомат, контракты, администратор: диапазон 40..59.
		if (op >= VB_TabletOp.ATM_PLACE && op < 60)
		{
			VB_TabletBank.Handle(player, id, op, ctx);
			return;
		}
		string uid = id.GetPlainId();
		string a;
		string b;
		string c1;
		string d;
		string e;
		int n;
		bool flag;
		switch (op)
		{
			case VB_TabletOp.CLAN_CREATE:
				if (ctx.Read(a) && ctx.Read(b) && ctx.Read(c1) && ctx.Read(d) && ctx.Read(e) && ctx.Read(n))
					VB_TabletClans.Create(player, id, a, b, c1, d, e, n);
				break;
			case VB_TabletOp.CLAN_APPLY:
				if (ctx.Read(a) && ctx.Read(b) && ctx.Read(c1))
					Apply(player, id, a, b, c1);
				break;
			case VB_TabletOp.CLAN_APPLY_CANCEL:
				VB_TabletPlayers.ClearApply(uid);
				VB_TabletServer.SendClan(player, id);
				break;
			case VB_TabletOp.CLAN_REQUEST:
				if (ctx.Read(a) && ctx.Read(flag))
					AnswerRequest(player, id, a, flag);
				break;
			case VB_TabletOp.CLAN_INVITE:
				if (ctx.Read(a))
					Invite(player, id, a);
				break;
			case VB_TabletOp.CLAN_INVITE_ANSWER:
				if (ctx.Read(a) && ctx.Read(flag))
					AnswerInvite(player, id, a, flag);
				break;
			case VB_TabletOp.CLAN_KICK:
				if (ctx.Read(a))
					Kick(player, id, a);
				break;
			case VB_TabletOp.CLAN_SET_RANK:
				if (ctx.Read(a) && ctx.Read(b))
					SetRank(player, id, a, b);
				break;
			case VB_TabletOp.CLAN_LEAVE:
				Leave(player, id);
				break;
			case VB_TabletOp.CLAN_DISSOLVE:
				if (ctx.Read(a))
					DissolveByLeader(player, id, a);
				break;
			case VB_TabletOp.CLAN_LEADER:
				if (ctx.Read(a))
					GiveLeader(player, id, a);
				break;
			case VB_TabletOp.CLAN_JOURNAL:
				SendJournal(player, id);
				break;
			case VB_TabletOp.CLAN_NEWS:
				SendNews(player, id);
				break;
			case VB_TabletOp.CLAN_CHAT:
				SendChat(player, id);
				break;
			case VB_TabletOp.CLAN_BANK:
				SendBank(player, id);
				break;
			case VB_TabletOp.CHAT_SEND:
				if (ctx.Read(a))
					ChatSend(player, id, a);
				break;
			case VB_TabletOp.DISCORD_SET:
				if (ctx.Read(a))
					SetDiscord(player, id, a);
				break;
			case VB_TabletOp.NEWS_DELETE:
				if (ctx.Read(n))
					NewsDelete(player, id, n);
				break;
			case VB_TabletOp.NEWS_PIN:
				if (ctx.Read(n))
					NewsPin(player, id, n);
				break;
			case VB_TabletOp.NEWS_SAVE:
				NewsSave(player, id, ctx);
				break;
			case VB_TabletOp.CLAN_SETTINGS:
				Settings(player, id, ctx);
				break;
			case VB_TabletOp.CLAN_RANKS:
				SaveRanks(player, id, ctx);
				break;
		}
	}

	protected static void Done(VB_Clan c)
	{
		VB_TabletClans.Save(c);
		VB_TabletServer.PushClan(c);
	}

	protected static void Apply(PlayerBase player, PlayerIdentity id, string clanId, string text, string discord)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.Find(clanId);
		if (!c)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		if (VB_TabletClans.OfPlayer(uid))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.HAS_CLAN);
			return;
		}
		if (!c.Open)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_OPEN);
			return;
		}
		if (c.Members.Count() >= VB_TabletConfig.Get().ClanMaxMembers)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.CLAN_FULL);
			return;
		}
		int len = text.LengthUtf8();
		if (len < 10 || len > 300)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		VB_TabletPlayers.ClearApply(uid);
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		VB_ClanRequest q = new VB_ClanRequest();
		q.Uid = uid;
		q.PlayerName = id.GetName();
		q.Discord = discord;
		q.Msg = text;
		q.Time = VB_TabletClans.Stamp();
		q.Xp = rec.ToStats().Xp(VB_TabletConfig.Get().Rates);
		c.Requests.Insert(q);
		rec.AppliedClan = c.Id;
		rec.Dirty = true;
		Done(c);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.SENT);
		VB_TabletServer.SendClan(player, id);
	}

	protected static void AnswerRequest(PlayerBase player, PlayerIdentity id, string target, bool accept)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.INVITE))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_ClanRequest q = null;
		foreach (VB_ClanRequest r : c.Requests)
		{
			if (r.Uid == target)
				q = r;
		}
		if (!q)
			return;
		if (accept && c.Members.Count() >= VB_TabletConfig.Get().ClanMaxMembers)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.CLAN_FULL);
			return;
		}
		c.Requests.RemoveItem(q);
		VB_TabletRecord rec = VB_TabletPlayers.Get(target, q.PlayerName);
		rec.AppliedClan = "";
		rec.Dirty = true;
		if (accept && !VB_TabletClans.OfPlayer(target))
		{
			VB_TabletClans.AddMember(c, target, q.PlayerName, "member");
			VB_ClanMember m = c.Member(target);
			m.Online = VB_TabletServer.FindPlayer(target) != null;
			if (q.Discord != "")
				m.Discord = q.Discord;
			VB_TabletClans.Log(c, VB_ClanEvent.JOINED, q.PlayerName);
		}
		Done(c);
		VB_TabletServer.SendNoClan(target);
	}

	protected static void Invite(PlayerBase player, PlayerIdentity id, string nick)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.INVITE))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		if (c.Members.Count() >= VB_TabletConfig.Get().ClanMaxMembers)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.CLAN_FULL);
			return;
		}
		PlayerBase target = VB_TabletServer.FindByName(nick);
		if (!target)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		string tu = target.GetIdentity().GetPlainId();
		if (VB_TabletClans.OfPlayer(tu))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.HAS_CLAN);
			return;
		}
		VB_TabletClans.AddInvite(tu, c, id.GetName());
		VB_TabletServer.SendClan(target, target.GetIdentity());
		VB_TabletServer.SendResult(player, id, VB_TabletResult.SENT);
	}

	protected static void AnswerInvite(PlayerBase player, PlayerIdentity id, string clanId, bool accept)
	{
		string uid = id.GetPlainId();
		VB_TabletClans.DropInvite(uid, clanId);
		VB_Clan c = VB_TabletClans.Find(clanId);
		if (!accept || !c)
		{
			VB_TabletServer.SendClan(player, id);
			return;
		}
		if (VB_TabletClans.OfPlayer(uid))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.HAS_CLAN);
			return;
		}
		if (c.Members.Count() >= VB_TabletConfig.Get().ClanMaxMembers)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.CLAN_FULL);
			return;
		}
		VB_TabletPlayers.ClearApply(uid);
		VB_TabletClans.AddMember(c, uid, id.GetName(), "member");
		VB_TabletClans.Log(c, VB_ClanEvent.JOINED, id.GetName());
		Done(c);
	}

	protected static void Kick(PlayerBase player, PlayerIdentity id, string target)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.KICK) || !c.Outranks(uid, target))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		string name = c.Member(target).PlayerName;
		VB_TabletClans.RemoveMember(c, target);
		VB_TabletClans.Log(c, VB_ClanEvent.KICKED, name);
		Done(c);
		VB_TabletServer.SendNoClan(target);
	}

	protected static void SetRank(PlayerBase player, PlayerIdentity id, string target, string rank)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.ASSIGN) || !c.Outranks(uid, target))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_ClanMember me = c.Member(uid);
		VB_ClanRank r = c.Rank(rank);
		if (r.Id != rank || rank == "leader" || c.RankIndex(rank) <= c.RankIndex(me.RankId))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_ClanMember m = c.Member(target);
		m.RankId = rank;
		VB_TabletClans.Log(c, VB_ClanEvent.RANK, m.PlayerName, r.Title);
		Done(c);
	}

	protected static void Leave(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
			return;
		// Лидер не уходит молча: сначала передаёт лидерство или распускает клан.
		if (c.IsLeader(uid))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_TabletClans.RemoveMember(c, uid);
		VB_TabletClans.Log(c, VB_ClanEvent.LEFT, id.GetName());
		Done(c);
		VB_TabletServer.SendClan(player, id);
	}

	protected static void DissolveByLeader(PlayerBase player, PlayerIdentity id, string confirm)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.IsLeader(uid) || confirm != c.Title)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_TabletClans.Dissolve(c);
	}

	protected static void GiveLeader(PlayerBase player, PlayerIdentity id, string target)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.IsLeader(uid) || !c.Member(target) || target == uid)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_ClanMember m = c.Member(target);
		m.RankId = "leader";
		c.Member(uid).RankId = c.Ranks.Get(1).Id;
		VB_TabletClans.Log(c, VB_ClanEvent.LEADER, m.PlayerName);
		Done(c);
	}

	protected static void SetDiscord(PlayerBase player, PlayerIdentity id, string name)
	{
		string uid = id.GetPlainId();
		if (name != "" && VB_TabletCheck.DiscordError(name) != 0)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		VB_TabletRecord rec = VB_TabletPlayers.Get(uid, id.GetName());
		rec.Discord = name;
		rec.Dirty = true;
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (c)
		{
			c.Member(uid).Discord = name;
			Done(c);
		}
		VB_TabletServer.SendProfile(player, id);
	}

	protected static void ChatSend(PlayerBase player, PlayerIdentity id, string text)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
			return;
		if (!s_ChatAt)
			s_ChatAt = new map<string, float>();
		float now = GetGame().GetTickTime();
		float last;
		if (s_ChatAt.Find(uid, last) && now - last < CHAT_GAP_SEC)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.RATE);
			return;
		}
		if (text == "" || text.LengthUtf8() > 200)
			return;
		if (VB_TabletClans.HasBanned(text))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BANNED_WORD);
			return;
		}
		s_ChatAt.Set(uid, now);
		VB_ClanChatRow row = new VB_ClanChatRow();
		row.Who = id.GetName();
		row.Msg = text;
		string st = VB_TabletClans.Stamp();
		row.Day = st.Substring(0, 5);
		row.Time = st.Substring(6, 5);
		c.Chat.Insert(row);
		c.ChatTotal = c.ChatTotal + 1;
		while (c.Chat.Count() > 100)
			c.Chat.RemoveOrdered(0);
		Print(VB_TABLET_TAG + " чат " + c.Tag + " " + id.GetName() + ": " + text);
		VB_TabletClans.Save(c);
		foreach (VB_ClanMember m : c.Members)
		{
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (p)
				SendChat(p, p.GetIdentity());
		}
	}

	protected static void SendJournal(PlayerBase player, PlayerIdentity id)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(id.GetPlainId());
		if (!c)
			return;
		int op = VB_TabletOp.CLAN_JOURNAL;
		int n = c.Journal.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (VB_ClanJournalRow r : c.Journal)
			r.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void SendNews(PlayerBase player, PlayerIdentity id)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(id.GetPlainId());
		if (!c)
			return;
		int op = VB_TabletOp.CLAN_NEWS;
		int n = c.News.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (VB_ClanNews r : c.News)
			r.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void SendChat(PlayerBase player, PlayerIdentity id)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(id.GetPlainId());
		if (!c)
			return;
		int op = VB_TabletOp.CLAN_CHAT;
		int n = c.Chat.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (VB_ClanChatRow r : c.Chat)
			r.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void SendBank(PlayerBase player, PlayerIdentity id)
	{
		VB_Clan c = VB_TabletClans.OfPlayer(id.GetPlainId());
		if (!c)
			return;
		int op = VB_TabletOp.CLAN_BANK;
		int n = c.History.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (VB_ClanBankRow r : c.History)
			r.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void PushNews(VB_Clan c)
	{
		VB_TabletClans.Save(c);
		foreach (VB_ClanMember m : c.Members)
		{
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (p)
				SendNews(p, p.GetIdentity());
		}
	}

	protected static void NewsSave(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		int newsId;
		int tag;
		string head;
		string body;
		bool pin;
		if (!ctx.Read(newsId) || !ctx.Read(tag) || !ctx.Read(head) || !ctx.Read(body) || !ctx.Read(pin))
			return;
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.NEWS))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		if (head == "" || head.LengthUtf8() > 60 || body.LengthUtf8() > 500 || tag < 0 || tag > 3)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		if (VB_TabletClans.HasBanned(head) || VB_TabletClans.HasBanned(body))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BANNED_WORD);
			return;
		}
		VB_ClanNews item = null;
		int top = 0;
		foreach (VB_ClanNews r : c.News)
		{
			top = Math.Max(top, r.NewsId);
			if (r.NewsId == newsId)
				item = r;
		}
		if (!item)
		{
			item = new VB_ClanNews();
			item.NewsId = top + 1;
			item.Author = id.GetName();
			item.Time = VB_TabletClans.Stamp();
			c.News.InsertAt(item, 0);
			c.NewsTotal = c.NewsTotal + 1;
			VB_TabletClans.Log(c, VB_ClanEvent.NEWS, id.GetName(), head);
			while (c.News.Count() > VB_CLAN_NEWS_MAX)
				c.News.Remove(c.News.Count() - 1);
		}
		item.TagIdx = tag;
		item.Head = head;
		item.Body = body;
		if (pin)
		{
			foreach (VB_ClanNews o : c.News)
				o.Pin = false;
		}
		item.Pin = pin;
		PushNews(c);
	}

	protected static void NewsDelete(PlayerBase player, PlayerIdentity id, int newsId)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.NEWS))
			return;
		for (int i = c.News.Count() - 1; i >= 0; i--)
		{
			if (c.News.Get(i).NewsId == newsId)
				c.News.RemoveOrdered(i);
		}
		PushNews(c);
	}

	protected static void NewsPin(PlayerBase player, PlayerIdentity id, int newsId)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.NEWS))
			return;
		foreach (VB_ClanNews r : c.News)
		{
			if (r.NewsId == newsId)
				r.Pin = !r.Pin;
			else
				r.Pin = false;
		}
		PushNews(c);
	}

	// Настройки: платные поля списываются со склада, право «Настройки клана» даёт и эту трату.
	protected static void Settings(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		string title;
		string tag;
		string slogan;
		string shape;
		string symbol;
		int color;
		bool open;
		string discord;
		if (!ctx.Read(title) || !ctx.Read(tag) || !ctx.Read(slogan) || !ctx.Read(shape) || !ctx.Read(symbol) || !ctx.Read(color) || !ctx.Read(open) || !ctx.Read(discord))
			return;
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.SETTINGS))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		string invite = VB_TabletCheck.ClanInvite(discord);
		if (slogan.LengthUtf8() > 60 || (discord != "" && invite == ""))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		int sum = 0;
		if (title != c.Title)
		{
			int len = title.LengthUtf8();
			if (len < 3 || len > 24)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
				return;
			}
			if (VB_TabletClans.HasBanned(title))
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.BANNED_WORD);
				return;
			}
			if (!VB_TabletClans.NameFree(title, c))
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.NAME_TAKEN);
				return;
			}
			sum += cfg.PriceName;
		}
		if (tag != c.Tag)
		{
			if (tag.Length() < 2 || tag.Length() > 4)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
				return;
			}
			if (VB_TabletClans.HasBanned(tag))
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.BANNED_WORD);
				return;
			}
			if (!VB_TabletClans.TagFree(tag, c))
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.TAG_TAKEN);
				return;
			}
			sum += cfg.PriceTag;
		}
		if (shape != c.EmbShape || symbol != c.EmbSymbol)
		{
			if (!VB_TabletClans.EmblemFree(shape, symbol, c))
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.EMBLEM_TAKEN);
				return;
			}
			sum += cfg.PriceEmblem;
		}
		if (color != c.Color)
			sum += cfg.PriceColor;
		if (sum > c.Bank)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NO_FUNDS);
			return;
		}
		c.Title = title;
		c.Tag = tag;
		c.Slogan = slogan;
		c.EmbShape = shape;
		c.EmbSymbol = symbol;
		c.Color = color;
		c.Open = open;
		c.Discord = invite;
		if (sum > 0)
		{
			c.Bank = c.Bank - sum;
			VB_TabletClans.AddHistory(c, id.GetName(), VB_ClanEvent.SETTINGS, -sum);
		}
		VB_TabletClans.Log(c, VB_ClanEvent.SETTINGS, id.GetName());
		Done(c);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.SAVED);
	}

	// Ранги: менять только младше своего и выдавать только свои права. Лидер всегда первый и со всеми правами.
	protected static void SaveRanks(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		int n;
		if (!ctx.Read(n) || n < 2 || n > VB_CLAN_RANKS_MAX)
			return;
		array<ref VB_ClanRank> list = new array<ref VB_ClanRank>();
		for (int i = 0; i < n; i++)
		{
			VB_ClanRank r = new VB_ClanRank();
			if (!r.Read(ctx))
				return;
			list.Insert(r);
		}
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.RANKS))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_ClanMember me = c.Member(uid);
		VB_ClanRank mine = c.Rank(me.RankId);
		int myIndex = c.RankIndex(me.RankId);
		bool lead = me.RankId == "leader";
		if (list.Get(0).Id != "leader" || list.Get(n - 1).Id != "member" || (!lead && n <= myIndex))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		// Ранги выше и равные своему остаются как были и на своих местах.
		for (int k = 0; k <= myIndex && !lead; k++)
		{
			if (list.Get(k).Id != c.Ranks.Get(k).Id)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
				return;
			}
			list.Set(k, c.Ranks.Get(k));
		}
		// Название и цвет ранга Лидера меняет сам лидер, права у ранга всегда все.
		list.Get(0).Perms = VB_ClanPerm.ALL;
		for (int j = 0; j < n; j++)
		{
			VB_ClanRank q = list.Get(j);
			VB_ClanRank was = c.Rank(q.Id);
			if (q.Title.LengthUtf8() > 20 || q.Title == "")
				q.Title = was.Title;
			// Чужие права игрок не выдаёт и не снимает: они остаются как были. У нового ранга их нет.
			if (!lead && j > myIndex)
			{
				int kept = 0;
				if (was.Id == q.Id)
					kept = was.Perms & (VB_ClanPerm.ALL - mine.Perms);
				q.Perms = (q.Perms & mine.Perms) | kept;
			}
		}
		c.Ranks = list;
		foreach (VB_ClanMember m : c.Members)
		{
			bool found = false;
			foreach (VB_ClanRank x : list)
			{
				if (x.Id == m.RankId)
					found = true;
			}
			if (!found)
				m.RankId = "member";
		}
		// Без тоста: экран шлёт ранги после каждого клика, а новая шапка клана и есть ответ.
		Done(c);
	}
}
