// INT PartyMode (client) — network + state model (spec.md §4, §5, §8). What the client knows and where from:
//   friends roster          FRIENDS / FRIEND_ADDED / FRIEND_REMOVED from the server
//   online / joined         the vanilla player list every client gets anyway (ClientData.SyncEvent_OnPlayerListUpdate)
//   position in the bubble  the friend's own replicated PlayerBase (ClientData.m_PlayerBaseList, matched by Steam64)
//   HP, status, far pos     PULL every PullIntervalSec — only the SHOWN friends (≤ limit), only if one is online
//   death                   FRIEND_DEAD push
//   pings                   PING_ADD / PING_CLEAR_OF; my own pings are drawn at once, before the server echo
// Everything runs from MissionGameplay.OnUpdate (one tick, nothing registered in the call queue).

class INT_PTY_Friend
{
	string Id;
	string Nick;
	string Since;
	bool Online;
	bool HasState;
	int Flags;
	int Hp;
	vector LastPos;
	float PosAt;
	bool Dead;
	vector DeathPos;
	PlayerBase Body;		// replicated to us (inside the network bubble), else null

	bool HasFlag(int f)
	{
		return (Flags & f) != 0;
	}

	// Best known world position: the live entity, the death spot, the last server position.
	vector WorldPos()
	{
		if (Body && !Dead)
			return Body.GetPosition();
		if (Dead)
			return DeathPos;
		return LastPos;
	}

	bool KnowsPos()
	{
		if (Body || Dead)
			return true;
		return HasState && HasFlag(INT_PTY_Const.F_HAS_POS);
	}
}

class INT_PTY_PingMark
{
	string Owner;
	int SlotIdx;
	vector Where;
	float Born;			// client clock (s) when it appeared — drives the appear animation
	float Gone;			// 0 = alive, else client clock when the remove animation started
}

class INT_PTY_SearchRow
{
	string Id;
	string Name;
	int Secs;
	bool Invited;
	float At;
}

class INT_PTY_InviteRow
{
	string Id;
	string Name;
	int State;			// outgoing only: INT_PTY_Const.OUT_*
	float At;
}

class INT_PTY_What
{
	static const int FRIENDS = 1;
	static const int STATE = 2;
	static const int PINGS = 4;
	static const int INVITES = 8;
	static const int SEARCH = 16;
	static const int SEARCH_LIST = 32;
	static const int BLOCK = 64;
}

class INT_PTY_Client
{
	static const float HELLO_RETRY_S = 5.0;
	static const float RESOLVE_S = 0.5;
	static const float FINAL_ROW_KEEP_S = 60.0;
	static const float PING_FADE_OUT_S = 0.6;

	protected static bool s_Hooked;
	protected static bool s_Welcomed;
	protected static float s_Clock;			// seconds since init (client-side animation / timers)
	protected static float s_HelloAt = -100;
	protected static float s_ResolveAcc;
	protected static float s_PullAcc;
	protected static float s_LastPullAt = -100;
	protected static bool s_PullWanted;
	protected static float s_LastPingAt = -100;
	protected static bool s_ListKnown;
	protected static int s_MyPingNext;

	// from WELCOME
	static string s_MySteam;
	static int s_DisplayLimit = 10;
	static int s_MaxTagDistance = 900;
	static int s_PingLimit = 4;
	static int s_MaxPingDistance = 2000;
	static int s_PingCooldownMs = 400;
	static int s_PullIntervalSec = 3;
	static int s_PullMinSec = 2;

	static bool s_Searching;
	static float s_BlockedUntil;

	static ref array<ref INT_PTY_Friend> s_Friends;
	protected static ref map<string, INT_PTY_Friend> s_ById;
	static ref array<ref INT_PTY_PingMark> s_Pings;
	static ref array<ref INT_PTY_SearchRow> s_SearchList;
	static ref array<ref INT_PTY_InviteRow> s_Incoming;
	static ref array<ref INT_PTY_InviteRow> s_Outgoing;
	protected static ref array<string> s_PendingRemove;
	protected static ref ScriptInvoker s_OnChanged;

	// statistics (debug / self-test)
	static int s_PullsSent;
	static int s_StatesIn;
	static int s_PacketsIn;

	static void Init()
	{
		if (!s_Friends)
			s_Friends = new array<ref INT_PTY_Friend>();
		if (!s_ById)
			s_ById = new map<string, INT_PTY_Friend>();
		if (!s_Pings)
			s_Pings = new array<ref INT_PTY_PingMark>();
		if (!s_SearchList)
			s_SearchList = new array<ref INT_PTY_SearchRow>();
		if (!s_Incoming)
			s_Incoming = new array<ref INT_PTY_InviteRow>();
		if (!s_Outgoing)
			s_Outgoing = new array<ref INT_PTY_InviteRow>();
		if (!s_PendingRemove)
			s_PendingRemove = new array<string>();
		if (s_Hooked || !GetGame() || GetGame().IsDedicatedServer())
			return;
		if (DayZGame.Event_OnRPC.Count(INT_PTY_Client.OnRPC) == 0)
			DayZGame.Event_OnRPC.Insert(INT_PTY_Client.OnRPC);
		if (ClientData.SyncEvent_OnPlayerListUpdate.Count(INT_PTY_Client.OnPlayerList) == 0)
			ClientData.SyncEvent_OnPlayerListUpdate.Insert(INT_PTY_Client.OnPlayerList);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked)
		{
			if (DayZGame.Event_OnRPC)
				DayZGame.Event_OnRPC.Remove(INT_PTY_Client.OnRPC);
			if (ClientData.SyncEvent_OnPlayerListUpdate)
				ClientData.SyncEvent_OnPlayerListUpdate.Remove(INT_PTY_Client.OnPlayerList);
		}
		s_Hooked = false;
		s_Welcomed = false;
		s_ListKnown = false;
		s_Friends = null;
		s_ById = null;
		s_Pings = null;
		s_SearchList = null;
		s_Incoming = null;
		s_Outgoing = null;
		s_PendingRemove = null;
		INT_PTY_SettingsStore.Flush();
	}

	static ScriptInvoker OnChanged()
	{
		if (!s_OnChanged)
			s_OnChanged = new ScriptInvoker();
		return s_OnChanged;
	}

	protected static void Changed(int what)
	{
		if (s_OnChanged)
			s_OnChanged.Invoke(what);
	}

	static float Clock()
	{
		return s_Clock;
	}

	static bool IsWelcomed()
	{
		return s_Welcomed;
	}

	static bool IsBlocked()
	{
		return s_Clock < s_BlockedUntil;
	}

	static int BlockedSeconds()
	{
		if (!IsBlocked())
			return 0;
		return (int)Math.Ceil(s_BlockedUntil - s_Clock);
	}

	static INT_PTY_Friend Friend(string id)
	{
		if (!s_ById)
			return null;
		INT_PTY_Friend f;
		s_ById.Find(id, f);
		return f;
	}

	static INT_PTY_Settings Settings()
	{
		return INT_PTY_SettingsStore.Get();
	}

	// The name the player sees for a friend: his own alias, else the nick.
	static string NameOf(string id)
	{
		INT_PTY_FriendPref p = Settings().Pref(id);
		if (p && p.Alias != "")
			return p.Alias;
		INT_PTY_Friend f = Friend(id);
		if (f)
			return f.Nick;
		return id;
	}

	static bool IsShown(string id)
	{
		INT_PTY_FriendPref p = Settings().Pref(id);
		return p && p.Shown == 1;
	}

	static int ShownCount()
	{
		int n = 0;
		foreach (INT_PTY_Friend f : s_Friends)
		{
			if (IsShown(f.Id))
				n++;
		}
		return n;
	}

	// --- tick ---------------------------------------------------------------------------------------------------
	static void Tick(float dt)
	{
		if (!s_Hooked)
			return;
		s_Clock = s_Clock + dt;
		// preload gate: nothing that touches the camera / world before the local player exists
		if (!GetGame().GetPlayer())
			return;
		if (!s_Welcomed)
		{
			if (s_Clock - s_HelloAt >= HELLO_RETRY_S)
			{
				s_HelloAt = s_Clock;
				Send(INT_PTY_Proto.Begin(INT_PTY_Const.OP_HELLO));
			}
			return;
		}
		s_ResolveAcc = s_ResolveAcc + dt;
		if (s_ResolveAcc >= RESOLVE_S)
		{
			s_ResolveAcc = 0;
			ResolveEntities();
			DropOldRows();
		}
		s_PullAcc = s_PullAcc + dt;
		bool pullDue = s_PullAcc >= s_PullIntervalSec;
		if (s_PullWanted && s_Clock - s_LastPullAt >= s_PullMinSec + 0.1)
			pullDue = true;
		if (pullDue)
		{
			s_PullAcc = 0;
			s_PullWanted = false;
			Pull();
		}
		DropFinishedPings();
	}

	// Matches the replicated players of our bubble to friends by Steam64 (identity of a remote player is
	// available on the client: vanilla reads GetPlainId there, clientdata.c:39-47).
	protected static void ResolveEntities()
	{
		foreach (INT_PTY_Friend f : s_Friends)
		{
			f.Body = null;
		}
		Man me = GetGame().GetPlayer();
		foreach (Man m : ClientData.m_PlayerBaseList)
		{
			if (!m || m == me)
				continue;
			PlayerIdentity ident = m.GetIdentity();
			if (!ident)
				continue;
			INT_PTY_Friend fr = Friend(ident.GetPlainId());
			if (!fr)
				continue;
			PlayerBase pb = PlayerBase.Cast(m);
			if (!pb)
				continue;
			// a fresh body after a respawn is alive; the old corpse is not ours any more
			if (pb.IsAlive())
			{
				fr.Body = pb;
				if (fr.Dead)
					fr.Dead = false;
			}
		}
	}

	protected static void Pull()
	{
		array<string> ids = new array<string>();
		bool anyOnline = false;
		foreach (INT_PTY_Friend f : s_Friends)
		{
			if (!IsShown(f.Id))
				continue;
			if (ids.Count() >= s_DisplayLimit)
				break;
			ids.Insert(f.Id);
			if (f.Online || !s_ListKnown)
				anyOnline = true;
		}
		if (ids.Count() == 0 || !anyOnline)
			return;
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PULL);
		rpc.Write(ids.Count());
		foreach (string id : ids)
		{
			INT_PTY_Proto.WriteId(rpc, id);
		}
		Send(rpc);
		s_PullsSent++;
		s_LastPullAt = s_Clock;
	}

	protected static void DropOldRows()
	{
		for (int i = s_Outgoing.Count() - 1; i >= 0; i--)
		{
			INT_PTY_InviteRow r = s_Outgoing.Get(i);
			if (r.State != INT_PTY_Const.OUT_SENT && s_Clock - r.At > FINAL_ROW_KEEP_S)
			{
				s_Outgoing.RemoveOrdered(i);
				Changed(INT_PTY_What.INVITES);
			}
		}
	}

	protected static void DropFinishedPings()
	{
		for (int i = s_Pings.Count() - 1; i >= 0; i--)
		{
			INT_PTY_PingMark p = s_Pings.Get(i);
			if (p.Gone > 0 && s_Clock - p.Gone > PING_FADE_OUT_S)
				s_Pings.RemoveOrdered(i);
		}
	}

	// --- vanilla player list: online state + "friend joined" notice --------------------------------------------
	static void OnPlayerList(SyncPlayerList list)
	{
		RefreshOnline(true);
	}

	static void RefreshOnline(bool notify)
	{
		if (!s_Friends || !ClientData.m_PlayerList || !ClientData.m_PlayerList.m_PlayerList)
			return;
		array<string> online = new array<string>();
		foreach (SyncPlayer sp : ClientData.m_PlayerList.m_PlayerList)
		{
			if (sp)
				online.Insert(sp.m_UID);
		}
		bool first = !s_ListKnown;
		s_ListKnown = true;
		bool changed = false;
		foreach (INT_PTY_Friend f : s_Friends)
		{
			bool now = online.Find(f.Id) >= 0;
			if (now == f.Online)
				continue;
			f.Online = now;
			changed = true;
			// a hidden friend is hidden completely (brief 5, review PTY-07)
			if (now && notify && !first && Settings().NotifyOnline == 1 && IsShown(f.Id))
				INT_PTY_Notify.Online(NameOf(f.Id));
			if (!now)
				ForgetPingsOf(f.Id);
		}
		if (changed)
		{
			s_PullWanted = true;		// fresh state as soon as the server minimum allows (review PTY-13)
			Changed(INT_PTY_What.STATE);
		}
	}

	// --- network in ---------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_PTY_Const.RPC_S2C || target)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int op;
		if (!INT_PTY_Proto.ReadHeader(ctx, op))
		{
			INT_PTY_Log.Warn("пакет сервера не прочитан (другая версия мода на сервере?)");
			return;
		}
		s_PacketsIn++;
		Handle(op, ctx);
	}

	static void Handle(int op, ParamsReadContext ctx)
	{
		if (op == INT_PTY_Const.OP_WELCOME)
			OnWelcome(ctx);
		else if (op == INT_PTY_Const.OP_FRIENDS)
			OnFriends(ctx);
		else if (op == INT_PTY_Const.OP_STATE)
			OnState(ctx);
		else if (op == INT_PTY_Const.OP_PING_ADD)
			OnPingAdd(ctx);
		else if (op == INT_PTY_Const.OP_PING_CLEAR_OF)
			OnPingClearOf(ctx);
		else if (op == INT_PTY_Const.OP_SEARCH_STATE)
			OnSearchState(ctx);
		else if (op == INT_PTY_Const.OP_SEARCH_ROWS)
			OnSearchList(ctx);
		else if (op == INT_PTY_Const.OP_INVITE_IN)
			OnInviteIn(ctx);
		else if (op == INT_PTY_Const.OP_INVITE_GONE)
			OnInviteGone(ctx);
		else if (op == INT_PTY_Const.OP_INVITE_OUT)
			OnInviteOut(ctx);
		else if (op == INT_PTY_Const.OP_FRIEND_ADDED)
			OnFriendAdded(ctx);
		else if (op == INT_PTY_Const.OP_FRIEND_REMOVED)
			OnFriendRemoved(ctx);
		else if (op == INT_PTY_Const.OP_FRIEND_DEAD)
			OnFriendDead(ctx);
		else if (op == INT_PTY_Const.OP_RESULT)
			OnResult(ctx);
	}

	protected static void OnWelcome(ParamsReadContext ctx)
	{
		int limit;
		int tagDist;
		int pingLimit;
		int pingDist;
		int pingCd;
		int pullSec;
		int pullMin;
		bool dbgOn;
		bool searching;
		string me;
		if (!ctx.Read(limit) || !ctx.Read(tagDist) || !ctx.Read(pingLimit) || !ctx.Read(pingDist) || !ctx.Read(pingCd))
			return;
		if (!ctx.Read(pullSec) || !ctx.Read(pullMin) || !ctx.Read(dbgOn) || !ctx.Read(searching) || !ctx.Read(me))
			return;
		s_DisplayLimit = Math.Clamp(limit, 1, INT_PTY_Const.HARD_DISPLAY_LIMIT);
		s_MaxTagDistance = tagDist;
		s_PingLimit = Math.Clamp(pingLimit, 1, INT_PTY_Const.HARD_PING_LIMIT);
		s_MaxPingDistance = pingDist;
		s_PingCooldownMs = pingCd;
		s_PullIntervalSec = Math.Max(1, pullSec);
		s_PullMinSec = Math.Clamp(pullMin, 1, s_PullIntervalSec);
		s_Searching = searching;
		s_MySteam = me;
		INT_PTY_Log.s_Debug = dbgOn;
		s_Welcomed = true;
		s_PullAcc = 0;
		INT_PTY_Log.Info("сервер принял: лимит показа " + s_DisplayLimit.ToString() + ", пингов " + s_PingLimit.ToString());
		Changed(INT_PTY_What.SEARCH);
	}

	protected static void OnFriends(ParamsReadContext ctx)
	{
		bool first;
		int n;
		if (!ctx.Read(first) || !ctx.Read(n) || n < 0 || n > INT_PTY_Const.FRIENDS_PER_PART)
			return;
		if (first)
		{
			s_Friends.Clear();
			s_ById.Clear();
		}
		for (int i = 0; i < n; i++)
		{
			string id;
			string nick;
			string since;
			if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadName(ctx, nick) || !ctx.Read(since))
				return;
			AddFriendLocal(id, nick, since);
		}
		EnforceShownLimit();
		RefreshOnline(false);
		Changed(INT_PTY_What.FRIENDS);
	}

	protected static INT_PTY_Friend AddFriendLocal(string id, string nick, string since)
	{
		INT_PTY_Friend f = Friend(id);
		if (!f)
		{
			f = new INT_PTY_Friend();
			f.Id = id;
			s_Friends.Insert(f);
			s_ById.Set(id, f);
		}
		f.Nick = nick;
		f.Since = since;
		INT_PTY_FriendPref p = Settings().Pref(id);
		if (!p)
		{
			p = Settings().PrefOrNew(id);
			p.Shown = 0;
			if (ShownCount() < s_DisplayLimit)
				p.Shown = 1;
			INT_PTY_SettingsStore.MarkDirty();
		}
		return f;
	}

	// More shown friends than the server allows (limit lowered by the admin): the first ones stay shown.
	protected static void EnforceShownLimit()
	{
		int shown = 0;
		foreach (INT_PTY_Friend f : s_Friends)
		{
			INT_PTY_FriendPref p = Settings().Pref(f.Id);
			if (!p || p.Shown != 1)
				continue;
			shown++;
			if (shown > s_DisplayLimit)
			{
				p.Shown = 0;
				INT_PTY_SettingsStore.MarkDirty();
			}
		}
	}

	protected static void OnState(ParamsReadContext ctx)
	{
		int n;
		if (!ctx.Read(n) || n < 0 || n > INT_PTY_Const.MAX_PULL_IDS)
			return;
		s_StatesIn++;
		for (int i = 0; i < n; i++)
		{
			string id;
			int flags;
			int hp;
			vector pos;
			if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !ctx.Read(flags) || !ctx.Read(hp) || !ctx.Read(pos))
				return;
			INT_PTY_Friend f = Friend(id);
			if (!f)
				continue;
			f.HasState = true;
			f.Flags = flags;
			f.Hp = Math.Clamp(hp, 0, 100);
			bool online = (flags & INT_PTY_Const.F_ONLINE) != 0;
			if (!s_ListKnown)
				f.Online = online;
			if ((flags & INT_PTY_Const.F_HAS_POS) != 0 && INT_PTY_Util.SanePos(pos))
			{
				f.LastPos = pos;
				f.PosAt = s_Clock;
			}
			if (online && (flags & INT_PTY_Const.F_ALIVE) == 0 && (flags & INT_PTY_Const.F_HAS_POS) != 0)
			{
				if (!f.Dead)
					f.DeathPos = f.LastPos;
				f.Dead = true;
			}
			else if (online && (flags & INT_PTY_Const.F_ALIVE) != 0)
			{
				f.Dead = false;
			}
		}
		Changed(INT_PTY_What.STATE);
	}

	protected static void OnFriendDead(ParamsReadContext ctx)
	{
		string id;
		vector pos;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadPos(ctx, pos))
			return;
		INT_PTY_Friend f = Friend(id);
		if (!f)
			return;
		f.Dead = true;
		f.DeathPos = pos;
		f.Hp = 0;
		f.Flags = f.Flags & ~INT_PTY_Const.F_ALIVE;
		if (IsShown(id))
			INT_PTY_Notify.Dead(NameOf(id));
		Changed(INT_PTY_What.STATE);
	}

	// --- pings in -----------------------------------------------------------------------------------------------
	protected static void OnPingAdd(ParamsReadContext ctx)
	{
		string owner;
		int slot;
		vector pos;
		if (!INT_PTY_Proto.ReadSteamId(ctx, owner) || !ctx.Read(slot) || !INT_PTY_Proto.ReadPos(ctx, pos))
			return;
		if (slot < 0 || slot >= INT_PTY_Const.HARD_PING_LIMIT)
			return;
		if (owner != s_MySteam && !Friend(owner))
			return;
		PutPing(owner, slot, pos);
	}

	protected static void OnPingClearOf(ParamsReadContext ctx)
	{
		string owner;
		if (!INT_PTY_Proto.ReadSteamId(ctx, owner))
			return;
		ForgetPingsOf(owner);
	}

	protected static void PutPing(string owner, int slot, vector pos)
	{
		INT_PTY_PingMark m = null;
		foreach (INT_PTY_PingMark p : s_Pings)
		{
			if (p.Owner == owner && p.SlotIdx == slot && p.Gone == 0)
			{
				m = p;
				break;
			}
		}
		if (!m)
		{
			m = new INT_PTY_PingMark();
			m.Owner = owner;
			m.SlotIdx = slot;
			s_Pings.Insert(m);
		}
		m.Where = pos;
		m.Born = s_Clock;
		m.Gone = 0;
		if (owner == s_MySteam)
			INT_PTY_Sfx.Ping(true);
		else if (IsShown(owner) && PingInView(pos))
			INT_PTY_Sfx.Ping(false);
		Changed(INT_PTY_What.PINGS);
	}

	// A friend's ping farther than PingViewM is not drawn, so it makes no sound either (owner 2026-11-01).
	protected static bool PingInView(vector pos)
	{
		Man me = GetGame().GetPlayer();
		if (!me)
			return true;
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		return vector.Distance(me.GetPosition(), pos) < st.PingViewM;
	}

	// The remove animation of every live ping of `owner`.
	static void ForgetPingsOf(string owner)
	{
		if (!s_Pings)
			return;
		bool any = false;
		foreach (INT_PTY_PingMark p : s_Pings)
		{
			if (p.Owner == owner && p.Gone == 0)
			{
				p.Gone = s_Clock;
				any = true;
			}
		}
		if (any)
			Changed(INT_PTY_What.PINGS);
	}

	// --- search / invites in -------------------------------------------------------------------------------------
	protected static void OnSearchState(ParamsReadContext ctx)
	{
		bool on;
		if (!ctx.Read(on))
			return;
		s_Searching = on;
		if (!on)
			s_Incoming.Clear();
		Changed(INT_PTY_What.SEARCH | INT_PTY_What.INVITES);
	}

	protected static void OnSearchList(ParamsReadContext ctx)
	{
		int n;
		if (!ctx.Read(n) || n < 0 || n > INT_PTY_Const.MAX_SEARCH_LIST)
			return;
		s_SearchList.Clear();
		string id;
		string name;
		int secs;
		bool invited;
		for (int i = 0; i < n; i++)
		{
			if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadName(ctx, name) || !ctx.Read(secs) || !ctx.Read(invited))
				return;
			INT_PTY_SearchRow r = new INT_PTY_SearchRow();
			r.Id = id;
			r.Name = name;
			r.Secs = secs;
			r.Invited = invited;
			r.At = s_Clock;
			s_SearchList.Insert(r);
		}
		Changed(INT_PTY_What.SEARCH_LIST);
	}

	protected static INT_PTY_InviteRow FindRow(array<ref INT_PTY_InviteRow> rows, string id)
	{
		foreach (INT_PTY_InviteRow r : rows)
		{
			if (r.Id == id)
				return r;
		}
		return null;
	}

	protected static void RemoveRow(array<ref INT_PTY_InviteRow> rows, string id)
	{
		for (int i = rows.Count() - 1; i >= 0; i--)
		{
			if (rows.Get(i).Id == id)
				rows.RemoveOrdered(i);
		}
	}

	protected static void OnInviteIn(ParamsReadContext ctx)
	{
		string id;
		string name;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadName(ctx, name))
			return;
		bool fresh = FindRow(s_Incoming, id) == null;
		RemoveRow(s_Incoming, id);
		INT_PTY_InviteRow r = new INT_PTY_InviteRow();
		r.Id = id;
		r.Name = name;
		r.At = s_Clock;
		s_Incoming.InsertAt(r, 0);
		if (fresh && Settings().NotifyInvite == 1)
			INT_PTY_Notify.InviteIn(name);
		Changed(INT_PTY_What.INVITES);
	}

	protected static void OnInviteGone(ParamsReadContext ctx)
	{
		string id;
		int reason;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !ctx.Read(reason))
			return;
		INT_PTY_InviteRow r = FindRow(s_Incoming, id);
		string name = id;
		if (r)
			name = r.Name;
		RemoveRow(s_Incoming, id);
		if (r && reason == INT_PTY_Const.GONE_CANCELLED)
			INT_PTY_Notify.Text("STR_INT_PTY_T_IN_CANCELLED", name, INT_UIF_ToastKind.INFO);
		else if (r && reason == INT_PTY_Const.GONE_INVITER_LEFT)
			INT_PTY_Notify.Text("STR_INT_PTY_T_IN_LEFT", name, INT_UIF_ToastKind.INFO);
		Changed(INT_PTY_What.INVITES);
	}

	protected static void OnInviteOut(ParamsReadContext ctx)
	{
		string id;
		string name;
		int state;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadName(ctx, name) || !ctx.Read(state))
			return;
		if (state == INT_PTY_Const.OUT_CANCELLED || state == INT_PTY_Const.OUT_ACCEPTED)
		{
			RemoveRow(s_Outgoing, id);
		}
		else
		{
			INT_PTY_InviteRow r = FindRow(s_Outgoing, id);
			bool fresh = r == null;
			if (fresh)
			{
				r = new INT_PTY_InviteRow();
				r.Id = id;
				s_Outgoing.InsertAt(r, 0);
			}
			r.Name = name;
			r.State = state;
			r.At = s_Clock;
			if (state == INT_PTY_Const.OUT_SENT && fresh)
				INT_PTY_Notify.Text("STR_INT_PTY_T_SENT", name, INT_UIF_ToastKind.SUCCESS);
			else if (state == INT_PTY_Const.OUT_DECLINED)
				INT_PTY_Notify.Text("STR_INT_PTY_T_OUT_DECLINED", name, INT_UIF_ToastKind.WARNING);
			else if (state == INT_PTY_Const.OUT_SEARCH_OFF)
				INT_PTY_Notify.Text("STR_INT_PTY_T_OUT_SEARCH_OFF", name, INT_UIF_ToastKind.WARNING);
			else if (state == INT_PTY_Const.OUT_TARGET_LEFT)
				INT_PTY_Notify.Text("STR_INT_PTY_T_OUT_LEFT", name, INT_UIF_ToastKind.INFO);
		}
		foreach (INT_PTY_SearchRow s : s_SearchList)
		{
			if (s.Id == id)
				s.Invited = state == INT_PTY_Const.OUT_SENT;
		}
		Changed(INT_PTY_What.INVITES | INT_PTY_What.SEARCH_LIST);
	}

	protected static void OnFriendAdded(ParamsReadContext ctx)
	{
		string id;
		string name;
		string since;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id) || !INT_PTY_Proto.ReadName(ctx, name) || !ctx.Read(since))
			return;
		AddFriendLocal(id, name, since);
		EnforceShownLimit();
		RemoveRow(s_Incoming, id);
		RemoveRow(s_Outgoing, id);
		for (int i = s_SearchList.Count() - 1; i >= 0; i--)
		{
			if (s_SearchList.Get(i).Id == id)
				s_SearchList.RemoveOrdered(i);
		}
		RefreshOnline(false);
		INT_PTY_SettingsStore.Flush();
		INT_PTY_Notify.Text("STR_INT_PTY_T_FRIEND_ADDED", name, INT_UIF_ToastKind.SUCCESS);
		s_PullWanted = true;
		Changed(INT_PTY_What.FRIENDS | INT_PTY_What.INVITES | INT_PTY_What.SEARCH_LIST);
	}

	// A removed friend: everything about him goes away at once (brief 22е).
	protected static void OnFriendRemoved(ParamsReadContext ctx)
	{
		string id;
		if (!INT_PTY_Proto.ReadSteamId(ctx, id))
			return;
		string name = NameOf(id);
		INT_PTY_Friend f = Friend(id);
		if (f)
		{
			s_Friends.RemoveItem(f);
			s_ById.Remove(id);
		}
		// his alias / colour / shown flag go too: a re-added friend starts fresh under the display limit (PTY-08)
		INT_PTY_SettingsStore.ForgetFriend(id);
		for (int i = s_Pings.Count() - 1; i >= 0; i--)
		{
			if (s_Pings.Get(i).Owner == id)
				s_Pings.RemoveOrdered(i);
		}
		if (s_PendingRemove.Find(id) >= 0)
		{
			s_PendingRemove.RemoveItem(id);
			INT_PTY_Notify.Text("STR_INT_PTY_T_FRIEND_REMOVED", name, INT_UIF_ToastKind.INFO);
		}
		else
		{
			INT_PTY_Notify.Text("STR_INT_PTY_T_FRIEND_REMOVED_BY", name, INT_UIF_ToastKind.WARNING);
		}
		Changed(INT_PTY_What.FRIENDS | INT_PTY_What.PINGS);
	}

	protected static void OnResult(ParamsReadContext ctx)
	{
		int code;
		int value;
		if (!ctx.Read(code) || !ctx.Read(value))
			return;
		if (code == INT_PTY_Const.RES_RATE_LIMITED)
		{
			s_BlockedUntil = s_Clock + Math.Clamp(value, 1, 120);
			Changed(INT_PTY_What.BLOCK);
		}
		INT_PTY_Notify.Result(code, value);
	}

	// --- actions (UI, keys) -------------------------------------------------------------------------------------
	protected static bool CanAct()
	{
		if (!s_Welcomed)
			return false;
		if (IsBlocked())
		{
			INT_PTY_Notify.Result(INT_PTY_Const.RES_RATE_LIMITED, BlockedSeconds());
			return false;
		}
		return true;
	}

	static void SetSearch(bool on)
	{
		if (!CanAct())
			return;
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_SEARCH_SET);
		rpc.Write(on);
		Send(rpc);
	}

	static void RequestSearchList()
	{
		if (!CanAct())
			return;
		Send(INT_PTY_Proto.Begin(INT_PTY_Const.OP_SEARCH_LIST));
	}

	protected static void SendWithId(int op, string id)
	{
		if (!CanAct() || !INT_PTY_Util.IsSteamId(id))
			return;
		ScriptRPC rpc = INT_PTY_Proto.Begin(op);
		INT_PTY_Proto.WriteId(rpc, id);
		Send(rpc);
	}

	static void Invite(string id)
	{
		SendWithId(INT_PTY_Const.OP_INVITE, id);
	}

	static void CancelInvite(string id)
	{
		SendWithId(INT_PTY_Const.OP_INVITE_CANCEL, id);
	}

	static void Accept(string id)
	{
		SendWithId(INT_PTY_Const.OP_INVITE_ACCEPT, id);
	}

	static void Decline(string id)
	{
		SendWithId(INT_PTY_Const.OP_INVITE_DECLINE, id);
	}

	static void RemoveFriend(string id)
	{
		if (!Friend(id))
			return;
		if (s_PendingRemove.Find(id) < 0)
			s_PendingRemove.Insert(id);
		SendWithId(INT_PTY_Const.OP_FRIEND_REMOVE, id);
	}

	// Shown on the HUD or not (client only, brief 3: never more than the server limit).
	static bool SetShown(string id, bool shown)
	{
		INT_PTY_FriendPref p = Settings().PrefOrNew(id);
		if (shown && p.Shown != 1 && ShownCount() >= s_DisplayLimit)
		{
			INT_PTY_Notify.Text("STR_INT_PTY_T_LIMIT", s_DisplayLimit.ToString(), INT_UIF_ToastKind.WARNING);
			return false;
		}
		p.Shown = 0;
		if (shown)
			p.Shown = 1;
		if (!shown)
			ForgetPingsOf(id);
		INT_PTY_SettingsStore.MarkDirty();
		s_PullWanted = true;
		Changed(INT_PTY_What.FRIENDS);
		return true;
	}

	// The point the camera looks at. From a vehicle the first hit is often the own car (1PP glass, 3PP body): the ray
	// steps past the own vehicle / own body and goes on (review PTY-10), at most 4 times.
	protected static bool INT_PTY_PingRay(PlayerBase me, vector from, vector to, vector dir, out vector hitPos)
	{
		hitPos = vector.Zero;
		Transport veh = null;
		HumanCommandVehicle hcv = me.GetCommand_Vehicle();
		if (hcv)
			veh = hcv.GetTransport();
		vector start = from;
		set<Object> hits = new set<Object>();
		for (int step = 0; step < 4; step++)
		{
			Object skip = me;
			if (veh && step % 2 == 1)
				skip = veh;
			hits.Clear();
			vector at;
			vector nrm;
			int comp;
			if (!DayZPhysics.RaycastRV(start, to, at, nrm, comp, hits, null, skip, false, false, ObjIntersectView, 0.0))
				return false;
			bool own = false;
			for (int h = 0; h < hits.Count(); h++)
			{
				Object o = hits.Get(h);
				if (o && (o == me || (veh && o == veh)))
					own = true;
			}
			if (!own)
			{
				hitPos = at;
				return true;
			}
			start = at + dir * 0.15;
		}
		return false;
	}

	// T: a ping where the camera looks (raycast from the camera, up to the server ping distance).
	static bool PlacePing()
	{
		if (!s_Welcomed)
			return false;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me || !me.IsAlive() || me.IsUnconscious())
			return false;
		if ((s_Clock - s_LastPingAt) * 1000 < s_PingCooldownMs)
			return false;
		vector from = GetGame().GetCurrentCameraPosition();
		vector dir = GetGame().GetCurrentCameraDirection();
		float range = s_MaxPingDistance;
		vector to = from + dir * range;
		vector hitPos;
		if (!INT_PTY_PingRay(me, from, to, dir, hitPos))
			return false;
		if (vector.Distance(me.GetPosition(), hitPos) > s_MaxPingDistance)
			return false;
		s_LastPingAt = s_Clock;
		int slot = s_MyPingNext;
		s_MyPingNext = (s_MyPingNext + 1) % s_PingLimit;
		PutPing(s_MySteam, slot, hitPos);
		ScriptRPC rpc = INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING);
		rpc.Write(slot);
		rpc.Write(hitPos);
		Send(rpc);
		return true;
	}

	// Y: my pings go away for everyone. Held Y (allToo): friends' pings also leave MY screen.
	static void ClearPings(bool allToo)
	{
		if (!s_Welcomed)
			return;
		bool mine = false;
		foreach (INT_PTY_PingMark p : s_Pings)
		{
			if (p.Gone != 0)
				continue;
			if (p.Owner == s_MySteam)
			{
				p.Gone = s_Clock;
				mine = true;
			}
			else if (allToo)
			{
				p.Gone = s_Clock;
			}
		}
		if (mine)
		{
			s_MyPingNext = 0;
			Send(INT_PTY_Proto.Begin(INT_PTY_Const.OP_PING_CLEAR));
		}
		Changed(INT_PTY_What.PINGS);
	}

	// --- network out --------------------------------------------------------------------------------------------
	protected static void Send(ScriptRPC rpc)
	{
		rpc.Send(null, INT_PTY_Const.RPC_C2S, true, null);
	}
}
