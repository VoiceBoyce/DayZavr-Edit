// INT PartyMode (server) — the whole server logic (spec.md §4, §5). Event-driven only: a client packet, a connect,
// a disconnect, a death. No timers, no OnUpdate, no scans (owner rules 1-3). The RPC comes through the static
// DayZGame.Event_OnRPC (no modded DayZGame). The sender is always the RPC identity, never an id from the packet.
// Security (brief 22e/22ж): data about a player goes only to his current friends (checked against the in-memory
// list on every request); a removed friend stops getting anything at once.

class INT_PTY_SE_Online
{
	string Steam;
	string Uid;						// PlayerIdentity.GetId() — PlayerDisconnected gets only this
	string Name;
	PlayerIdentity Identity;		// null for self-test players
	PlayerBase Body;
	ref INT_PTY_SE_PlayerFile INT_File;	// INT_ prefix: a field 'File' clashes with Dabs Framework class File (predeploy server 1, 2026-10-25)
	bool Welcomed;
	bool Searching;
	int SearchSince;
	ref array<string> OutInvites;	// players I invited (pending)
	ref array<string> InInvites;	// players who invited me (pending, arrival order)
	ref array<vector> PingPos;
	ref array<bool> PingOn;
	ref array<int> Actions;			// times (ms) of counted actions inside the last minute
	int LastHello = -100000;
	int LastPull = -100000;
	int LastPing = -100000;
	int LastSearchList = -100000;
	bool Virtual;					// self-test player: never sent to

	void INT_PTY_SE_Online()
	{
		OutInvites = new array<string>();
		InInvites = new array<string>();
		PingPos = new array<vector>();
		PingOn = new array<bool>();
		Actions = new array<int>();
	}

	int PingCount()
	{
		int n = 0;
		foreach (bool on : PingOn)
		{
			if (on)
				n++;
		}
		return n;
	}

	void ClearPings()
	{
		PingPos.Clear();
		PingOn.Clear();
	}
}

class INT_PTY_SE_Core
{
	protected static bool s_Hooked;
	protected static ref map<string, ref INT_PTY_SE_Online> s_BySteam;
	protected static ref map<string, string> s_SteamByUid;
	protected static ref array<string> s_Searchers;		// in the order they switched search on

	// statistics (debug / self-test)
	static int s_In;
	static int s_Out;
	static int s_Dropped;
	static int s_Denied;			// requests about non-friends, wrong slots, too far pings
	static int s_RateLimited;
	static int s_Pulls;
	static int s_PullsEarly;
	static int s_Pings;
	static int s_LastStateRows;

	// Self-test capture: when set, every outgoing packet is recorded as "op>steam"; with s_NoSend nothing is sent.
	static ref array<string> s_Capture;
	static bool s_NoSend;

	static void Init()
	{
		if (!s_BySteam)
			s_BySteam = new map<string, ref INT_PTY_SE_Online>();
		if (!s_SteamByUid)
			s_SteamByUid = new map<string, string>();
		if (!s_Searchers)
			s_Searchers = new array<string>();
		if (s_Hooked || !GetGame() || !GetGame().IsDedicatedServer())
			return;
		if (DayZGame.Event_OnRPC.Count(INT_PTY_SE_Core.OnRPC) == 0)
			DayZGame.Event_OnRPC.Insert(INT_PTY_SE_Core.OnRPC);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked && DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(INT_PTY_SE_Core.OnRPC);
		s_Hooked = false;
	}

	static INT_PTY_SE_ConfigData Cfg()
	{
		return INT_PTY_SE_Config.Get();
	}

	static INT_PTY_SE_Online Find(string steam)
	{
		if (!s_BySteam)
			return null;
		INT_PTY_SE_Online o;
		s_BySteam.Find(steam, o);
		return o;
	}

	static int OnlineCount()
	{
		if (!s_BySteam)
			return 0;
		return s_BySteam.Count();
	}

	static int SearcherCount()
	{
		if (!s_Searchers)
			return 0;
		return s_Searchers.Count();
	}

	static int s_TestNow = -1;		// self-test clock (ms); -1 = the game clock

	protected static int Now()
	{
		if (s_TestNow >= 0)
			return s_TestNow;
		return GetGame().GetTime();
	}

	// --- connect / disconnect / death ---------------------------------------------------------------------------
	static void OnConnect(PlayerBase player, PlayerIdentity identity)
	{
		if (!identity)
			return;
		string steam = identity.GetPlainId();
		if (!INT_PTY_Util.IsSteamId(steam))
		{
			INT_PTY_Log.Warn("вход: Steam ID не распознан, игрок пропущен: " + steam);
			return;
		}
		// argument temporaries can be clobbered in Enforce: every call result goes into a local first
		string uid = identity.GetId();
		string rawName = identity.GetPlainName();
		string name = INT_PTY_Util.CleanName(rawName);
		INT_PTY_SE_Online o = Register(steam, uid, name, identity, player);
		INT_PTY_Log.Dbg("вход " + o.Name + " " + steam + " друзей " + o.INT_File.Friends.Count().ToString());
	}

	// Shared by the real connect and the self-test (identity = null for virtual players).
	static INT_PTY_SE_Online Register(string steam, string uid, string name, PlayerIdentity identity, PlayerBase player)
	{
		Init();
		INT_PTY_SE_Online o = Find(steam);
		if (!o)
		{
			o = new INT_PTY_SE_Online();
			o.Steam = steam;
			s_BySteam.Set(steam, o);
		}
		o.Uid = uid;
		o.Identity = identity;
		o.Virtual = identity == null;
		if (o.Body != player)
		{
			o.Body = player;
			WatchDeath(player);
		}
		if (uid != "")
			s_SteamByUid.Set(uid, steam);
		if (!o.INT_File)
			o.INT_File = INT_PTY_SE_Store.Acquire(steam);
		SyncName(o, name);
		DropOneSided(o);
		return o;
	}

	// The nick changed since the files were written: own file + the entries in ONLINE friends' files.
	protected static void SyncName(INT_PTY_SE_Online o, string name)
	{
		if (name == "")
			name = o.Name;
		o.Name = name;
		if (o.INT_File.Friends.Count() == 0)
			return;
		if (o.INT_File.Name != name)
		{
			o.INT_File.Name = name;
			INT_PTY_SE_Store.Save(o.INT_File);
		}
		foreach (INT_PTY_SE_FriendEntry e : o.INT_File.Friends)
		{
			INT_PTY_SE_Online fr = Find(e.Id);
			if (!fr)
				continue;
			if (e.Name != fr.Name && fr.Name != "")
			{
				e.Name = fr.Name;
				o.INT_File.Name = name;
				INT_PTY_SE_Store.Save(o.INT_File);
			}
			INT_PTY_SE_FriendEntry back = fr.INT_File.Find(o.Steam);
			if (back && back.Name != name)
			{
				back.Name = name;
				INT_PTY_SE_Store.Save(fr.INT_File);
			}
		}
	}

	protected static void WatchDeath(PlayerBase p)
	{
		if (!p)
			return;
		ScriptInvoker killed = p.GetOnKilledInvoker();
		if (killed && killed.Count(INT_PTY_SE_Core.OnKilled) == 0)
			killed.Insert(INT_PTY_SE_Core.OnKilled);
	}

	static void OnKilled(EntityAI victim, Object killer)
	{
		PlayerBase p = PlayerBase.Cast(victim);
		if (!p || !p.GetIdentity())
			return;
		INT_PTY_SE_Online o = Find(p.GetIdentity().GetPlainId());
		if (!o || o.Body != p)
			return;
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_DEAD);
		rpc.Write(o.Steam);
		rpc.Write(p.GetPosition());
		SendToFriends(o, rpc, "dead");
		INT_PTY_Log.Dbg("смерть " + o.Name + ", друзьям онлайн отправлено");
	}

	static void OnDisconnected(string uid)
	{
		if (!s_SteamByUid)
			return;
		string steam;
		if (!s_SteamByUid.Find(uid, steam))
			return;
		s_SteamByUid.Remove(uid);
		Leave(steam);
	}

	// The player leaves: pings, search and every pending invite of his go away with notices. Shared with tests.
	static void Leave(string steam)
	{
		INT_PTY_SE_Online o = Find(steam);
		if (!o)
			return;
		if (o.PingCount() > 0)
		{
			o.ClearPings();
			ScriptRPC rpcPing = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING_CLEAR_OF);
			rpcPing.Write(o.Steam);
			SendToFriends(o, rpcPing, "pingclear");
		}
		DropIncoming(o, INT_PTY_Const.OUT_TARGET_LEFT);
		foreach (string target : o.OutInvites)
		{
			INT_PTY_SE_Online t = Find(target);
			if (!t)
				continue;
			t.InInvites.RemoveItem(o.Steam);
			SendInviteGone(t, o.Steam, INT_PTY_Const.GONE_INVITER_LEFT);
			INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " вышел с сервера — его заявка → " + INT_PTY_SE_History.Who(t.Name, t.Steam) + " снята");
		}
		o.OutInvites.Clear();
		SetSearching(o, false);
		INT_PTY_SE_Store.Release(steam);
		s_BySteam.Remove(steam);
		INT_PTY_Log.Dbg("выход " + o.Name + " " + steam);
	}

	// Every invite TO this player is declined; the inviters are told why.
	protected static void DropIncoming(INT_PTY_SE_Online o, int outState)
	{
		foreach (string from : o.InInvites)
		{
			INT_PTY_SE_Online inviter = Find(from);
			if (!inviter)
				continue;
			inviter.OutInvites.RemoveItem(o.Steam);
			SendInviteOut(inviter, o.Steam, o.Name, outState);
			string why = " вышел с сервера — заявка от ";
			if (outState == INT_PTY_Const.OUT_SEARCH_OFF)
				why = " выключил поиск — заявка от ";
			INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + why + INT_PTY_SE_History.Who(inviter.Name, inviter.Steam) + " отклонена");
		}
		o.InInvites.Clear();
	}

	// --- network in ---------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_PTY_Const.RPC_C2S || target)
			return;
		if (!GetGame() || !GetGame().IsDedicatedServer())
			return;
		if (!sender)
		{
			s_Dropped++;
			return;
		}
		s_In++;
		int op;
		if (!INT_PTY_Proto.ReadHeader(ctx, op))
		{
			s_Dropped++;
			INT_PTY_Log.Dbg("пакет не прочитан (другая версия мода у клиента?) от " + sender.GetPlainId());
			return;
		}
		INT_PTY_SE_Online o = Find(sender.GetPlainId());
		if (!o)
		{
			s_Dropped++;
			INT_PTY_Log.Dbg("пакет от игрока без входа: " + sender.GetPlainId());
			return;
		}
		Handle(o, op, ctx);
	}

	// One packet of an online player (also the entry point of the self-test).
	static void Handle(INT_PTY_SE_Online o, int op, ParamsReadContext ctx)
	{
		if (op == INT_PTY_Const.OP_HELLO)
		{
			OnHello(o);
			return;
		}
		if (!o.Welcomed)
		{
			s_Dropped++;
			return;
		}
		if (op == INT_PTY_Const.OP_PULL)
		{
			OnPull(o, ctx);
			return;
		}
		if (op == INT_PTY_Const.OP_PING)
		{
			OnPing(o, ctx);
			return;
		}
		if (op == INT_PTY_Const.OP_PING_CLEAR)
		{
			OnPingClear(o);
			return;
		}
		if (op == INT_PTY_Const.OP_SEARCH_LIST)
		{
			OnSearchList(o);
			return;
		}
		// everything below is a counted action (brief 28)
		if (op < INT_PTY_Const.OP_SEARCH_SET || op > INT_PTY_Const.OP_FRIEND_REMOVE)
		{
			s_Dropped++;
			INT_PTY_Log.Dbg("неизвестная операция " + op.ToString());
			return;
		}
		if (!AllowAction(o))
			return;
		if (op == INT_PTY_Const.OP_SEARCH_SET)
		{
			OnSearchSet(o, ctx);
			return;
		}
		string other;
		if (!INT_PTY_Proto.ReadId(ctx, other))
		{
			s_Dropped++;
			return;
		}
		if (op == INT_PTY_Const.OP_INVITE)
			OnInvite(o, other);
		else if (op == INT_PTY_Const.OP_INVITE_CANCEL)
			OnInviteCancel(o, other);
		else if (op == INT_PTY_Const.OP_INVITE_ACCEPT)
			OnInviteAccept(o, other);
		else if (op == INT_PTY_Const.OP_INVITE_DECLINE)
			OnInviteDecline(o, other);
		else if (op == INT_PTY_Const.OP_FRIEND_REMOVE)
			OnFriendRemove(o, other);
	}

	// Sliding one-minute window (brief 28, config MaxRequestsPerMinute). No timer: old marks drop on the next call.
	protected static bool AllowAction(INT_PTY_SE_Online o)
	{
		int now = Now();
		while (o.Actions.Count() > 0 && now - o.Actions.Get(0) >= 60000)
		{
			o.Actions.RemoveOrdered(0);
		}
		if (o.Actions.Count() >= Cfg().MaxRequestsPerMinute)
		{
			s_RateLimited++;
			int wait = (o.Actions.Get(0) + 60000 - now) / 1000 + 1;
			SendResult(o, INT_PTY_Const.RES_RATE_LIMITED, wait);
			INT_PTY_Log.Dbg("лимит запросов: " + o.Name + " ждать " + wait.ToString() + " с");
			return false;
		}
		o.Actions.Insert(now);
		return true;
	}

	// --- HELLO: everything the client needs after (re)loading ----------------------------------------------------
	protected static void OnHello(INT_PTY_SE_Online o)
	{
		int now = Now();
		if (now - o.LastHello < INT_PTY_Const.HELLO_GAP_MS)
		{
			s_Dropped++;
			return;
		}
		o.LastHello = now;
		INT_PTY_SE_ConfigData c = Cfg();
		ScriptRPC w = INT_PTY_Proto.Begin(INT_PTY_Const.OP_WELCOME);
		w.Write(c.DisplayLimit);
		w.Write(c.MaxTagDistance);
		w.Write(c.PingLimit);
		w.Write(c.MaxPingDistance);
		w.Write(c.PingCooldownMs);
		w.Write(c.PullIntervalSec);
		w.Write(c.PullMinIntervalSec);
		w.Write(c.DebugMode == 1);
		w.Write(o.Searching);
		w.Write(o.Steam);
		Send(o, w, "welcome");
		SendRoster(o);
		foreach (string from : o.InInvites)
		{
			INT_PTY_SE_Online inviter = Find(from);
			if (inviter)
				SendInviteIn(o, inviter);
		}
		foreach (string to : o.OutInvites)
		{
			INT_PTY_SE_Online tgt = Find(to);
			if (tgt)
				SendInviteOut(o, tgt.Steam, tgt.Name, INT_PTY_Const.OUT_SENT);
		}
		o.Welcomed = true;
		foreach (INT_PTY_SE_FriendEntry e : o.INT_File.Friends)
		{
			INT_PTY_SE_Online fr = Find(e.Id);
			if (fr && Mutual(o, fr))
				SendPingsOf(fr, o);
		}
		SendPingsOf(o, o);
		INT_PTY_Log.Dbg("HELLO " + o.Name + ": друзей " + o.INT_File.Friends.Count().ToString());
	}

	protected static void SendRoster(INT_PTY_SE_Online o)
	{
		int total = o.INT_File.Friends.Count();
		int start = 0;
		bool first = true;
		while (first || start < total)
		{
			int n = Math.Min(INT_PTY_Const.FRIENDS_PER_PART, total - start);
			ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIENDS);
			rpc.Write(first);
			rpc.Write(n);
			for (int i = 0; i < n; i++)
			{
				INT_PTY_SE_FriendEntry e = o.INT_File.Friends.Get(start + i);
				string name = e.Name;
				INT_PTY_SE_Online fr = Find(e.Id);
				if (fr && fr.Name != "")
					name = fr.Name;
				rpc.Write(e.Id);
				rpc.Write(name);
				rpc.Write(e.Since);
			}
			Send(o, rpc, "friends");
			start = start + n;
			first = false;
		}
	}

	// --- PULL: live state of the friends the client shows (≤ DisplayLimit) ----------------------------------------
	protected static void OnPull(INT_PTY_SE_Online o, ParamsReadContext ctx)
	{
		int now = Now();
		if (now - o.LastPull < Cfg().PullMinIntervalSec * 1000 - 250)
		{
			s_PullsEarly++;
			return;
		}
		int n;
		if (!ctx.Read(n) || n < 0 || n > INT_PTY_Const.MAX_PULL_IDS)
		{
			s_Dropped++;
			return;
		}
		o.LastPull = now;
		s_Pulls++;
		int limit = Math.Min(n, Cfg().DisplayLimit);
		array<INT_PTY_SE_Online> rows = new array<INT_PTY_SE_Online>();
		array<string> offline = new array<string>();
		for (int i = 0; i < n; i++)
		{
			string id;
			if (!INT_PTY_Proto.ReadId(ctx, id))
			{
				s_Dropped++;
				return;
			}
			if (i >= limit)
				continue;
			if (!o.INT_File.Has(id))
			{
				s_Denied++;
				continue;
			}
			INT_PTY_SE_Online fr = Find(id);
			if (fr && !Mutual(o, fr))
			{
				s_Denied++;
				continue;
			}
			if (fr)
				rows.Insert(fr);
			else
				offline.Insert(id);
		}
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_STATE);
		s_LastStateRows = rows.Count() + offline.Count();
		rpc.Write(s_LastStateRows);
		foreach (INT_PTY_SE_Online r : rows)
		{
			WriteState(rpc, r);
		}
		foreach (string off : offline)
		{
			rpc.Write(off);
			rpc.Write(0);
			rpc.Write(0);
			rpc.Write(vector.Zero);
		}
		Send(o, rpc, "state");
	}

	protected static void WriteState(ScriptRPC rpc, INT_PTY_SE_Online fr)
	{
		int flags = INT_PTY_Const.F_ONLINE;
		int hp = 0;
		vector pos = vector.Zero;
		PlayerBase p = fr.Body;
		if (p)
		{
			flags = flags | INT_PTY_Const.F_HAS_POS;
			pos = p.GetPosition();
			if (p.IsAlive())
			{
				flags = flags | INT_PTY_Const.F_ALIVE;
				float maxHp = p.GetMaxHealth("", "Health");
				if (maxHp <= 0)
					maxHp = 100;
				hp = (int)Math.Round(p.GetHealth("", "Health") / maxHp * 100.0);
				hp = Math.Clamp(hp, 1, 100);
				if (p.IsUnconscious())
					flags = flags | INT_PTY_Const.F_UNCONSCIOUS;
				if (p.GetBleedingBits() != 0)
					flags = flags | INT_PTY_Const.F_BLEEDING;
				if (p.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
					flags = flags | INT_PTY_Const.F_BROKEN_LEG;
			}
		}
		rpc.Write(fr.Steam);
		rpc.Write(flags);
		rpc.Write(hp);
		rpc.Write(pos);
	}

	// --- pings ----------------------------------------------------------------------------------------------------
	protected static void OnPing(INT_PTY_SE_Online o, ParamsReadContext ctx)
	{
		int slot;
		vector pos;
		if (!ctx.Read(slot) || !INT_PTY_Proto.ReadPos(ctx, pos))
		{
			s_Dropped++;
			return;
		}
		int now = Now();
		INT_PTY_SE_ConfigData c = Cfg();
		// network jitter: two pings sent PingCooldownMs apart may arrive closer (review PTY-05)
		if (now - o.LastPing < c.PingCooldownMs - 150)
		{
			s_Denied++;
			return;
		}
		if (slot < 0 || slot >= c.PingLimit)
		{
			s_Denied++;
			return;
		}
		if (!o.Virtual && (!o.Body || !o.Body.IsAlive() || o.Body.IsUnconscious()))
		{
			s_Denied++;
			return;
		}
		if (o.Body && vector.Distance(o.Body.GetPosition(), pos) > c.MaxPingDistance + 25)
		{
			s_Denied++;
			INT_PTY_Log.Dbg("пинг дальше лимита отклонён: " + o.Name);
			return;
		}
		o.LastPing = now;
		while (o.PingPos.Count() < c.PingLimit)
		{
			o.PingPos.Insert(vector.Zero);
			o.PingOn.Insert(false);
		}
		o.PingPos.Set(slot, pos);
		o.PingOn.Set(slot, true);
		s_Pings++;
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING_ADD);
		rpc.Write(o.Steam);
		rpc.Write(slot);
		rpc.Write(pos);
		SendToFriends(o, rpc, "ping");
	}

	// No cooldown here: Y right after T (inside PingCooldownMs) must still reach the friends — the client has already
	// removed the ping locally. A clear without live pings sends nothing, so clears cannot flood (one per placed ping).
	protected static void OnPingClear(INT_PTY_SE_Online o)
	{
		if (o.PingCount() == 0)
			return;
		o.ClearPings();
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING_CLEAR_OF);
		rpc.Write(o.Steam);
		SendToFriends(o, rpc, "pingclear");
	}

	// All live pings of `owner` to `to` (HELLO, a new friendship).
	protected static void SendPingsOf(INT_PTY_SE_Online owner, INT_PTY_SE_Online to)
	{
		int n = owner.PingOn.Count();
		for (int i = 0; i < n; i++)
		{
			if (!owner.PingOn.Get(i))
				continue;
			ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING_ADD);
			rpc.Write(owner.Steam);
			rpc.Write(i);
			rpc.Write(owner.PingPos.Get(i));
			Send(to, rpc, "ping");
		}
	}

	// --- search ---------------------------------------------------------------------------------------------------
	protected static void OnSearchSet(INT_PTY_SE_Online o, ParamsReadContext ctx)
	{
		bool on;
		if (!ctx.Read(on))
		{
			s_Dropped++;
			return;
		}
		SetSearching(o, on);
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_SEARCH_STATE);
		rpc.Write(o.Searching);
		Send(o, rpc, "search");
	}

	protected static void SetSearching(INT_PTY_SE_Online o, bool on)
	{
		if (on == o.Searching)
			return;
		o.Searching = on;
		if (on)
		{
			o.SearchSince = Now();
			s_Searchers.Insert(o.Steam);
			INT_PTY_Log.Dbg("поиск включён: " + o.Name);
			return;
		}
		s_Searchers.RemoveItem(o.Steam);
		// brief 22г: switching search off declines every pending invite to this player, inviters are told
		foreach (string from : o.InInvites)
		{
			SendInviteGone(o, from, INT_PTY_Const.GONE_SEARCH_OFF);
		}
		DropIncoming(o, INT_PTY_Const.OUT_SEARCH_OFF);
		INT_PTY_Log.Dbg("поиск выключен: " + o.Name);
	}

	protected static void OnSearchList(INT_PTY_SE_Online o)
	{
		int now = Now();
		if (now - o.LastSearchList < INT_PTY_Const.SEARCH_LIST_GAP_MS)
		{
			s_Denied++;
			return;
		}
		if (!AllowAction(o))
			return;
		o.LastSearchList = now;
		array<INT_PTY_SE_Online> list = new array<INT_PTY_SE_Online>();
		for (int i = s_Searchers.Count() - 1; i >= 0; i--)
		{
			INT_PTY_SE_Online s = Find(s_Searchers.Get(i));
			if (!s || s == o || o.INT_File.Has(s.Steam))
				continue;
			list.Insert(s);
			if (list.Count() >= INT_PTY_Const.MAX_SEARCH_LIST)
				break;
		}
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_SEARCH_ROWS);
		rpc.Write(list.Count());
		foreach (INT_PTY_SE_Online e : list)
		{
			int secs = (now - e.SearchSince) / 1000;
			rpc.Write(e.Steam);
			rpc.Write(e.Name);
			rpc.Write(secs);
			rpc.Write(o.OutInvites.Find(e.Steam) >= 0);
		}
		Send(o, rpc, "searchlist");
	}

	// --- invites --------------------------------------------------------------------------------------------------
	protected static bool FriendLimitOk(INT_PTY_SE_Online a, INT_PTY_SE_Online b)
	{
		int max = Cfg().MaxFriends;
		if (max <= 0)
			return true;
		if (a.INT_File.Friends.Count() >= max)
		{
			SendResult(a, INT_PTY_Const.RES_MY_FRIEND_LIMIT, max);
			return false;
		}
		if (b.INT_File.Friends.Count() >= max)
		{
			SendResult(a, INT_PTY_Const.RES_HIS_FRIEND_LIMIT, max);
			return false;
		}
		return true;
	}

	protected static void OnInvite(INT_PTY_SE_Online o, string target)
	{
		if (target == o.Steam)
		{
			SendResult(o, INT_PTY_Const.RES_SELF, 0);
			return;
		}
		INT_PTY_SE_Online t = Find(target);
		if (!t || !t.Welcomed)
		{
			SendResult(o, INT_PTY_Const.RES_NOT_ONLINE, 0);
			return;
		}
		if (o.INT_File.Has(target))
		{
			SendResult(o, INT_PTY_Const.RES_ALREADY_FRIENDS, 0);
			return;
		}
		// he already invited me: two invites meet = friendship at once
		if (o.InInvites.Find(target) >= 0)
		{
			if (FriendLimitOk(o, t))
				MakeFriends(t, o, false);
			return;
		}
		if (!t.Searching)
		{
			SendResult(o, INT_PTY_Const.RES_NOT_SEARCHING, 0);
			return;
		}
		if (o.OutInvites.Find(target) >= 0)
		{
			SendResult(o, INT_PTY_Const.RES_ALREADY_INVITED, 0);
			return;
		}
		if (o.OutInvites.Count() >= Cfg().MaxOutgoingInvites)
		{
			SendResult(o, INT_PTY_Const.RES_TOO_MANY_INVITES, Cfg().MaxOutgoingInvites);
			return;
		}
		if (!FriendLimitOk(o, t))
			return;
		o.OutInvites.Insert(target);
		t.InInvites.Insert(o.Steam);
		SendInviteIn(t, o);
		SendInviteOut(o, t.Steam, t.Name, INT_PTY_Const.OUT_SENT);
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " отправил заявку → " + INT_PTY_SE_History.Who(t.Name, t.Steam));
	}

	protected static void OnInviteCancel(INT_PTY_SE_Online o, string target)
	{
		if (o.OutInvites.Find(target) < 0)
		{
			SendResult(o, INT_PTY_Const.RES_NO_INVITE, 0);
			return;
		}
		o.OutInvites.RemoveItem(target);
		INT_PTY_SE_Online t = Find(target);
		string tName = target;
		if (t)
		{
			t.InInvites.RemoveItem(o.Steam);
			SendInviteGone(t, o.Steam, INT_PTY_Const.GONE_CANCELLED);
			tName = t.Name;
		}
		SendInviteOut(o, target, tName, INT_PTY_Const.OUT_CANCELLED);
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " отозвал заявку → " + INT_PTY_SE_History.Who(tName, target));
	}

	protected static void OnInviteAccept(INT_PTY_SE_Online o, string from)
	{
		if (o.InInvites.Find(from) < 0)
		{
			SendResult(o, INT_PTY_Const.RES_NO_INVITE, 0);
			return;
		}
		INT_PTY_SE_Online inviter = Find(from);
		if (!inviter)
		{
			o.InInvites.RemoveItem(from);
			SendInviteGone(o, from, INT_PTY_Const.GONE_INVITER_LEFT);
			SendResult(o, INT_PTY_Const.RES_NOT_ONLINE, 0);
			return;
		}
		if (!FriendLimitOk(o, inviter))
			return;
		MakeFriends(inviter, o, true);
	}

	protected static void OnInviteDecline(INT_PTY_SE_Online o, string from)
	{
		if (o.InInvites.Find(from) < 0)
		{
			SendResult(o, INT_PTY_Const.RES_NO_INVITE, 0);
			return;
		}
		o.InInvites.RemoveItem(from);
		SendInviteGone(o, from, INT_PTY_Const.GONE_DECLINED);
		INT_PTY_SE_Online inviter = Find(from);
		string iName = from;
		if (inviter)
		{
			inviter.OutInvites.RemoveItem(o.Steam);
			SendInviteOut(inviter, o.Steam, o.Name, INT_PTY_Const.OUT_DECLINED);
			iName = inviter.Name;
		}
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " отклонил заявку от " + INT_PTY_SE_History.Who(iName, from));
	}

	// Both files change together; when the second write fails, the first is rolled back.
	static bool MakeFriends(INT_PTY_SE_Online inviter, INT_PTY_SE_Online acceptor, bool byAccept)
	{
		string since = INT_PTY_Util.Stamp();
		inviter.INT_File.Name = inviter.Name;
		acceptor.INT_File.Name = acceptor.Name;
		inviter.INT_File.Add(acceptor.Steam, acceptor.Name, since);
		acceptor.INT_File.Add(inviter.Steam, inviter.Name, since);
		if (!INT_PTY_SE_Store.Save(inviter.INT_File))
		{
			inviter.INT_File.Remove(acceptor.Steam);
			acceptor.INT_File.Remove(inviter.Steam);
			SendResult(acceptor, INT_PTY_Const.RES_SAVE_FAILED, 0);
			INT_PTY_Log.Err("дружба не сохранена (файл " + inviter.Steam + ")");
			return false;
		}
		if (!INT_PTY_SE_Store.Save(acceptor.INT_File))
		{
			inviter.INT_File.Remove(acceptor.Steam);
			acceptor.INT_File.Remove(inviter.Steam);
			if (!INT_PTY_SE_Store.Save(inviter.INT_File))
				INT_PTY_Log.Crit("откат дружбы не записан (файл " + inviter.Steam + ") — проверка взаимности не даст данных");
			SendResult(acceptor, INT_PTY_Const.RES_SAVE_FAILED, 0);
			INT_PTY_Log.Err("дружба не сохранена (файл " + acceptor.Steam + "), откат");
			return false;
		}
		// the pending invites between the two are done (both directions)
		inviter.OutInvites.RemoveItem(acceptor.Steam);
		acceptor.InInvites.RemoveItem(inviter.Steam);
		bool reverse = acceptor.OutInvites.Find(inviter.Steam) >= 0;
		acceptor.OutInvites.RemoveItem(inviter.Steam);
		inviter.InInvites.RemoveItem(acceptor.Steam);
		if (reverse)
			SendInviteGone(inviter, acceptor.Steam, INT_PTY_Const.GONE_FRIENDS);
		SendFriendAdded(inviter, acceptor, since);
		SendFriendAdded(acceptor, inviter, since);
		SendInviteOut(inviter, acceptor.Steam, acceptor.Name, INT_PTY_Const.OUT_ACCEPTED);
		SendPingsOf(inviter, acceptor);
		SendPingsOf(acceptor, inviter);
		if (byAccept)
			INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(acceptor.Name, acceptor.Steam) + " принял заявку от " + INT_PTY_SE_History.Who(inviter.Name, inviter.Steam));
		else
			INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(acceptor.Name, acceptor.Steam) + " и " + INT_PTY_SE_History.Who(inviter.Name, inviter.Steam) + " стали друзьями (встречные заявки)");
		INT_PTY_SE_API.Notify(inviter.Steam, acceptor.Steam, true);
		return true;
	}

	// --- remove a friend ------------------------------------------------------------------------------------------
	protected static void OnFriendRemove(INT_PTY_SE_Online o, string id)
	{
		if (!o.INT_File.Has(id))
		{
			RemoveOneSided(o, id);
			return;
		}
		INT_PTY_SE_Online other = Find(id);
		INT_PTY_SE_PlayerFile otherFile;
		string otherName = "";
		if (other)
		{
			otherFile = other.INT_File;
			otherName = other.Name;
		}
		else
		{
			otherFile = INT_PTY_SE_Store.Peek(id);
			INT_PTY_SE_FriendEntry entry = o.INT_File.Find(id);
			if (entry)
				otherName = entry.Name;
		}
		INT_PTY_SE_FriendEntry mine = o.INT_File.Find(id);
		INT_PTY_SE_FriendEntry his = otherFile.Find(o.Steam);
		string since = "";
		if (mine)
			since = mine.Since;
		o.INT_File.Remove(id);
		otherFile.Remove(o.Steam);
		if (!INT_PTY_SE_Store.Save(o.INT_File))
		{
			o.INT_File.Add(id, otherName, since);
			if (his)
				otherFile.Add(o.Steam, o.Name, his.Since);
			SendResult(o, INT_PTY_Const.RES_SAVE_FAILED, 0);
			return;
		}
		if (!INT_PTY_SE_Store.Save(otherFile))
		{
			o.INT_File.Add(id, otherName, since);
			if (!INT_PTY_SE_Store.Save(o.INT_File))
				INT_PTY_Log.Crit("откат удаления не записан (файл " + o.Steam + ") — проверка взаимности не даст данных");
			if (his)
				otherFile.Add(o.Steam, o.Name, his.Since);
			SendResult(o, INT_PTY_Const.RES_SAVE_FAILED, 0);
			INT_PTY_Log.Err("удаление из друзей не сохранено (файл " + id + "), откат");
			return;
		}
		ScriptRPC mineRpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_REMOVED);
		mineRpc.Write(id);
		Send(o, mineRpc, "removed");
		if (other)
		{
			ScriptRPC hisRpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_REMOVED);
			hisRpc.Write(o.Steam);
			Send(other, hisRpc, "removed");
		}
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " удалил из друзей " + INT_PTY_SE_History.Who(otherName, id));
		INT_PTY_SE_API.Notify(o.Steam, id, false);
	}

	// Friendship is mutual by design; a one-sided entry (a lost write, a restored backup, a broken file) never gives
	// data to anybody (review PTY-01). Both records are in memory for online players: two list lookups.
	protected static bool Mutual(INT_PTY_SE_Online a, INT_PTY_SE_Online b)
	{
		if (!a || !b || !a.INT_File || !b.INT_File)
			return false;
		return a.INT_File.Has(b.Steam) && b.INT_File.Has(a.Steam);
	}

	// `id` lists `o` but `o` does not list `id`: `o` may still cut it (his client never showed it, the other did).
	protected static void RemoveOneSided(INT_PTY_SE_Online o, string id)
	{
		INT_PTY_SE_Online other = Find(id);
		INT_PTY_SE_PlayerFile otherFile;
		if (other)
			otherFile = other.INT_File;
		else
			otherFile = INT_PTY_SE_Store.Peek(id);
		if (!otherFile || !otherFile.Has(o.Steam))
		{
			SendResult(o, INT_PTY_Const.RES_NOT_FRIEND, 0);
			return;
		}
		otherFile.Remove(o.Steam);
		if (!INT_PTY_SE_Store.Save(otherFile))
		{
			SendResult(o, INT_PTY_Const.RES_SAVE_FAILED, 0);
			return;
		}
		ScriptRPC mineRpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_REMOVED);
		mineRpc.Write(id);
		Send(o, mineRpc, "removed");
		if (other)
		{
			ScriptRPC hisRpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_REMOVED);
			hisRpc.Write(o.Steam);
			Send(other, hisRpc, "removed");
		}
		INT_PTY_Log.Warn("односторонняя запись дружбы убрана: " + id + " -> " + o.Steam);
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(o.Name, o.Steam) + " удалил из друзей (односторонняя запись) " + id);
		INT_PTY_SE_API.Notify(o.Steam, id, false);
	}

	// At connect: entries of `o` whose (online, readable) friend does not list `o` back are one-sided — dropped.
	protected static void DropOneSided(INT_PTY_SE_Online o)
	{
		if (o.INT_File.Untrusted)
			return;
		array<string> gone = new array<string>();
		foreach (INT_PTY_SE_FriendEntry e : o.INT_File.Friends)
		{
			INT_PTY_SE_Online fr = Find(e.Id);
			if (fr && fr.INT_File && !fr.INT_File.Untrusted && !fr.INT_File.Has(o.Steam))
				gone.Insert(e.Id);
		}
		if (gone.Count() == 0)
			return;
		foreach (string id : gone)
		{
			o.INT_File.Remove(id);
			INT_PTY_Log.Warn("односторонняя запись дружбы убрана при входе: " + o.Steam + " -> " + id);
		}
		INT_PTY_SE_Store.Save(o.INT_File);
	}

	// --- network out ----------------------------------------------------------------------------------------------
	static void Send(INT_PTY_SE_Online to, ScriptRPC rpc, string tag)
	{
		if (!to)
			return;
		if (s_Capture)
			s_Capture.Insert(tag + ">" + to.Steam);
		if (s_NoSend || to.Virtual || !to.Identity)
			return;
		rpc.Send(null, INT_PTY_Const.RPC_S2C, true, to.Identity);
		s_Out++;
	}

	// One packet to every online, welcomed friend of `o` (the buffer is not reset by Send).
	protected static void SendToFriends(INT_PTY_SE_Online o, ScriptRPC rpc, string tag)
	{
		foreach (INT_PTY_SE_FriendEntry e : o.INT_File.Friends)
		{
			INT_PTY_SE_Online fr = Find(e.Id);
			if (fr && fr.Welcomed && Mutual(o, fr))
				Send(fr, rpc, tag);
		}
	}

	protected static void SendResult(INT_PTY_SE_Online o, int code, int value)
	{
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_RESULT);
		rpc.Write(code);
		rpc.Write(value);
		Send(o, rpc, "result" + code.ToString());
	}

	protected static void SendInviteIn(INT_PTY_SE_Online to, INT_PTY_SE_Online from)
	{
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_INVITE_IN);
		rpc.Write(from.Steam);
		rpc.Write(from.Name);
		Send(to, rpc, "invitein");
	}

	protected static void SendInviteGone(INT_PTY_SE_Online to, string from, int reason)
	{
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_INVITE_GONE);
		rpc.Write(from);
		rpc.Write(reason);
		Send(to, rpc, "invitegone");
	}

	protected static void SendInviteOut(INT_PTY_SE_Online to, string target, string name, int state)
	{
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_INVITE_OUT);
		rpc.Write(target);
		rpc.Write(name);
		rpc.Write(state);
		Send(to, rpc, "inviteout" + state.ToString());
	}

	protected static void SendFriendAdded(INT_PTY_SE_Online to, INT_PTY_SE_Online friend, string since)
	{
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_FRIEND_ADDED);
		rpc.Write(friend.Steam);
		rpc.Write(friend.Name);
		rpc.Write(since);
		Send(to, rpc, "added");
	}

	// Self-test: forget every online record and searcher (the files stay).
	static void ResetForTest()
	{
		s_BySteam = new map<string, ref INT_PTY_SE_Online>();
		s_SteamByUid = new map<string, string>();
		s_Searchers = new array<string>();
	}

	protected static ref map<string, ref INT_PTY_SE_Online> s_SavedBySteam;
	protected static ref map<string, string> s_SavedUid;
	protected static ref array<string> s_SavedSearchers;

	// Self-test: the real online players are put aside during a test run and come back afterwards.
	static void PushStateForTest()
	{
		Init();
		s_SavedBySteam = s_BySteam;
		s_SavedUid = s_SteamByUid;
		s_SavedSearchers = s_Searchers;
		ResetForTest();
	}

	static void PopStateForTest()
	{
		if (!s_SavedBySteam)
			return;
		s_BySteam = s_SavedBySteam;
		s_SteamByUid = s_SavedUid;
		s_Searchers = s_SavedSearchers;
		s_SavedBySteam = null;
		s_SavedUid = null;
		s_SavedSearchers = null;
	}
}
