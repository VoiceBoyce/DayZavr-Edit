// INT_PDA (server) — network side (spec.md §7). Event-driven only: a client packet answers a client packet; the
// server sends on its own only as a result of another player's packet (chat line, invite, SOS) or an admin action.
// No timers, no scans: sessions exist only for online players, accounts are cached only for them. The RPC comes
// through the static DayZGame.Event_OnRPC (no modded DayZGame). Names always come from the server's own data,
// never from the packet; admins are checked by the sender's SteamID64 against the config. Module checks are O(1):
// the client names its PDA by network id and the server checks that this item is in the sender's inventory.

class INT_PDA_SE_Session
{
	PlayerIdentity Identity;	// weak: refreshed by every packet, the session goes away on disconnect
	string Steam;
	string GameName;
	bool Admin;
	bool LoggedIn;
	bool Listening;
	int LastHelloMs = -100000000;
	int LastReqMs = -100000000;
	float Tokens = 8;			// request bucket: a burst of 8, one token back per REQUEST_GAP_MS
	int TokenMs;
	int LastTooFastMs = -100000000;
	int LastChatMs = -100000000;
	int LastMatesMs = -100000000;
	int LastSosMs = -100000000;
	int LastHistPubMs = -100000000;	// chat history: one request answers with up to 25 packets — 2 s per channel
	int LastHistFacMs = -100000000;
	int Fails;
	int LockUntilMs = -100000000;
	string InviteFaction;		// pending invitation
	string InviteFrom;
	int PdaLow;					// network id of the PDA he listens with (for the world beep)
	int PdaHigh;
	bool TestOnline;			// self-test seam (INT_PDA_TestKit only): online without a PlayerIdentity, packets only captured
}

class INT_PDA_SE_Core
{
	static const string SE_BUILD = "1.1.1";		// server-only fixes on top of the shared VERSION (the client is unchanged)
	static const float BURST = 8;
	protected static bool s_Hooked;
	static ref map<string, ref INT_PDA_SE_Session> s_Sessions = new map<string, ref INT_PDA_SE_Session>();

	// counters for Debug and the self-test
	static int s_In;
	static int s_Dropped;
	static int s_Out;

	// Self-test capture: outgoing packets are recorded as "op:steam" (with s_NoSend nothing is sent).
	static ref array<string> s_Capture;
	static bool s_NoSend;
	// Self-test: the last answer code the server gave (any recipient) — op (OP_RESULT request op / OP_AUTH_RESULT /
	// OP_ADMIN), admin sub-op, code — and how many answers were given.
	static int s_LastOp;
	static int s_LastSub;
	static int s_LastCode;
	static int s_Results;

	static void Init()
	{
		if (s_Hooked || !GetGame() || !GetGame().IsDedicatedServer())
			return;
		if (DayZGame.Event_OnRPC.Count(INT_PDA_SE_Core.OnRPC) == 0)
			DayZGame.Event_OnRPC.Insert(INT_PDA_SE_Core.OnRPC);
		if (INT_PDA_Events.s_ServerWorkStop.Count(INT_PDA_SE_Core.OnPdaStopped) == 0)
			INT_PDA_Events.s_ServerWorkStop.Insert(INT_PDA_SE_Core.OnPdaStopped);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked && DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(INT_PDA_SE_Core.OnRPC);
		if (s_Hooked && INT_PDA_Events.s_ServerWorkStop)
			INT_PDA_Events.s_ServerWorkStop.Remove(INT_PDA_SE_Core.OnPdaStopped);
		s_Hooked = false;
	}

	static INT_PDA_SE_Config Cfg()
	{
		return INT_PDA_SE_Store.Cfg();
	}

	static INT_PDA_SE_Session Session(string steam)
	{
		return s_Sessions.Get(steam);
	}

	static INT_PDA_SE_Session SessionOf(PlayerIdentity id)
	{
		if (!id)
			return null;
		string steam = id.GetPlainId();
		INT_PDA_SE_Session s;
		if (s_Sessions.Find(steam, s))
		{
			s.Identity = id;
			return s;
		}
		s = new INT_PDA_SE_Session();
		s.Identity = id;
		s.Steam = steam;
		s.GameName = id.GetName();
		s.Admin = Cfg().IsAdmin(steam);
		s_Sessions.Set(steam, s);
		return s;
	}

	static bool IsAdmin(PlayerIdentity id)
	{
		return id && Cfg().IsAdmin(id.GetPlainId());
	}

	// --- connect / disconnect (called from the mission) -------------------------------------------------------------
	static void OnDisconnect(string steam)
	{
		INT_PDA_SE_Session s = s_Sessions.Get(steam);
		if (!s)
			return;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
		if (a && a.InFaction())
			INT_PDA_SE_Faction.SetOnline(a, false);
		s_Sessions.Remove(steam);
		INT_PDA_SE_Store.ForgetAccount(steam);
		INT_PDA_Log.Dbg("отключение " + steam);
	}

	// Server event: a PDA stopped (switched off / battery empty) — its holder stops receiving chat.
	static void OnPdaStopped(EntityAI pda)
	{
		if (!pda)
			return;
		Man root = pda.GetHierarchyRootPlayer();
		if (!root || !root.GetIdentity())
			return;
		INT_PDA_SE_Session s = s_Sessions.Get(root.GetIdentity().GetPlainId());
		if (!s || !s.Listening || s.Admin)
			return;
		// only the PDA he listens with counts: a second PDA (a looted one) going off must not cut his chat
		int low;
		int high;
		pda.GetNetworkID(low, high);
		if ((s.PdaLow != 0 || s.PdaHigh != 0) && (low != s.PdaLow || high != s.PdaHigh))
			return;
		s.Listening = false;
		INT_PDA_Log.Dbg("КПК выключился — без чата: " + s.Steam);
	}

	// --- network in -------------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_PDA_Const.RPC_C2S)
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
		if (!INT_PDA_Proto.ReadHeader(ctx, op))
		{
			s_Dropped++;
			INT_PDA_Log.Warn("пакет не прочитан (другая версия мода у клиента?) от " + INT_PDA_Log.Clip(sender.GetName(), 32));
			return;
		}
		PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
		if (!player)
		{
			s_Dropped++;
			return;
		}
		INT_PDA_SE_Session s = SessionOf(sender);
		int now = GetGame().GetTime();
		if (op == INT_PDA_Const.OP_HELLO)
		{
			OnHello(sender, s, now);
			return;
		}
		Dispatch(sender, player, s, op, ctx, now);
	}

	// Pacing + routing of one request after the header (also the self-test entry: INT_PDA_TestKit drives it with its
	// own sessions; sender / player are null there).
	static void Dispatch(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, int op, ParamsReadContext ctx, int now)
	{
		if (!s)
		{
			s_Dropped++;
			return;
		}
		// Pacing: a token bucket (burst 8, refill one per REQUEST_GAP_MS) instead of a hard gap, so the client's own
		// automatic requests (profile -> faction -> history) never eat a player's click. Chat has its own floor.
		if (op != INT_PDA_Const.OP_CHAT_SEND && op != INT_PDA_Const.OP_PM_SEND)
		{
			float passed = now - s.TokenMs;
			s.TokenMs = now;
			s.Tokens = s.Tokens + passed / INT_PDA_Const.REQUEST_GAP_MS;
			if (s.Tokens > BURST)
				s.Tokens = BURST;
			if (s.Tokens < 1)
			{
				s_Dropped++;
				if (now - s.LastTooFastMs >= 1000)
				{
					s.LastTooFastMs = now;
					SendResult(sender, op, INT_PDA_Const.R_TOO_FAST);
				}
				return;
			}
			s.Tokens = s.Tokens - 1;
		}
		s.LastReqMs = now;
		INT_PDA_Log.Dbg("op " + op.ToString() + " от " + s.Steam);
		switch (op)
		{
			case INT_PDA_Const.OP_REGISTER:
				INT_PDA_SE_Auth.OnRegister(sender, player, s, ctx);
				return;
			case INT_PDA_Const.OP_LOGIN:
				INT_PDA_SE_Auth.OnLogin(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_LOGOUT:
				INT_PDA_SE_Auth.OnLogout(sender, s);
				return;
			case INT_PDA_Const.OP_SET_CALLSIGN:
				INT_PDA_SE_Auth.OnSetCallsign(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_SET_PASSWORD:
				INT_PDA_SE_Auth.OnSetPassword(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_SET_AVATAR:
				INT_PDA_SE_Auth.OnSetAvatar(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_LISTEN:
				INT_PDA_SE_Chat.OnListen(sender, player, s, ctx);
				return;
			case INT_PDA_Const.OP_CHAT_SEND:
				INT_PDA_SE_Chat.OnChatSend(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_PM_SEND:
				INT_PDA_SE_Chat.OnPmSend(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_CHAT_HISTORY:
				INT_PDA_SE_Chat.OnHistory(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_NEWS:
				INT_PDA_SE_Chat.OnNews(sender, s);
				return;
			case INT_PDA_Const.OP_PDA_OWNER:
				INT_PDA_SE_Chat.OnPdaOwner(sender, player, s, ctx);
				return;
			case INT_PDA_Const.OP_FACTION:
				INT_PDA_SE_Faction.OnFaction(sender, s);
				return;
			case INT_PDA_Const.OP_INVITE:
				INT_PDA_SE_Faction.OnInvite(sender, player, s, ctx);
				return;
			case INT_PDA_Const.OP_INVITE_ANSWER:
				INT_PDA_SE_Faction.OnInviteAnswer(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_KICK:
				INT_PDA_SE_Faction.OnKick(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_SET_RANK:
				INT_PDA_SE_Faction.OnSetRank(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_LEAVE:
				INT_PDA_SE_Faction.OnLeave(sender, s);
				return;
			case INT_PDA_Const.OP_MATES:
				INT_PDA_SE_Faction.OnMates(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_SOS:
				INT_PDA_SE_Faction.OnSos(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_MARK_ADD:
				INT_PDA_SE_Faction.OnMarkAdd(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_MARK_DEL:
				INT_PDA_SE_Faction.OnMarkDel(sender, s, ctx);
				return;
			case INT_PDA_Const.OP_CHAR:
				INT_PDA_SE_Char.OnGet(sender, s, now);
				return;
			case INT_PDA_Const.OP_CHAR_SAVE:
				INT_PDA_SE_Char.OnSave(sender, s, ctx, now);
				return;
			case INT_PDA_Const.OP_CHAR_RESPAWN:
				INT_PDA_SE_Char.OnRespawn(sender, player, s, ctx, now);
				return;
			case INT_PDA_Const.OP_ADMIN:
				INT_PDA_SE_Admin.OnAdmin(sender, player, s, ctx);
				return;
		}
		s_Dropped++;
		INT_PDA_Log.Dbg("неизвестная операция " + op.ToString());
	}

	protected static void OnHello(PlayerIdentity sender, INT_PDA_SE_Session s, int now)
	{
		if (now - s.LastHelloMs < INT_PDA_Const.HELLO_GAP_MS)
			return;
		s.LastHelloMs = now;
		s.GameName = sender.GetName();
		INT_PDA_Account a = INT_PDA_SE_Store.Account(s.Steam);
		if (a && a.InFaction())
			INT_PDA_SE_Faction.SetOnline(a, true);
		SendWelcome(sender, s, a);
	}

	static void SendWelcome(PlayerIdentity to, INT_PDA_SE_Session s, INT_PDA_Account a)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_WELCOME);
		Cfg().ToSettings().Write(rpc);
		rpc.Write(s.Admin);
		bool registered = a != null;
		rpc.Write(registered);
		string callsign = "";
		int avatar = -1;
		if (a)
		{
			callsign = a.Callsign;
			avatar = a.Avatar;
		}
		INT_PDA_Proto.WriteStr(rpc, callsign);
		rpc.Write(avatar);			// login screen portrait (added after the first build: clients read it optionally)
		if (to)
			SendTo(rpc, to, INT_PDA_Const.OP_WELCOME);
		else
			SendToSession(rpc, s, INT_PDA_Const.OP_WELCOME);
	}

	// --- helpers ----------------------------------------------------------------------------------------------------
	// The PDA the client names (network id) if it is in the sender's inventory; null otherwise.
	static INT_PDA PdaOf(PlayerBase player, int low, int high)
	{
		if (low == 0 && high == 0)
			return null;
		INT_PDA pda = INT_PDA.Cast(GetGame().GetObjectByNetworkId(low, high));
		if (!pda || pda.GetHierarchyRootPlayer() != player)
			return null;
		return pda;
	}

	// Reads the PDA id from the packet and checks a module. Admins (Alt+L) need no PDA.
	static bool CheckPda(PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, string slot, out INT_PDA pda)
	{
		int low;
		int high;
		pda = null;
		if (!ctx.Read(low) || !ctx.Read(high))
			return false;
		pda = PdaOf(player, low, high);
		if (s.Admin)
			return true;
		if (!pda || !pda.IsWorkingPda())
			return false;
		return slot == "" || pda.HasModule(slot);
	}

	// The PDA a listener reported (still in his inventory), or null.
	static INT_PDA ListenPda(INT_PDA_SE_Session s)
	{
		if (!s || !s.Identity)
			return null;
		PlayerBase player = PlayerBase.Cast(s.Identity.GetPlayer());
		if (!player)
			return null;
		return PdaOf(player, s.PdaLow, s.PdaHigh);
	}

	static INT_PDA_Account LoggedAccount(INT_PDA_SE_Session s)
	{
		if (!s || !s.LoggedIn)
			return null;
		return INT_PDA_SE_Store.Account(s.Steam);
	}

	static PlayerIdentity IdentityOf(string steam)
	{
		INT_PDA_SE_Session s = s_Sessions.Get(steam);
		if (!s)
			return null;
		return s.Identity;
	}

	// DayZ nick of a sender ("" without an identity: self-test packets).
	static string NameOf(PlayerIdentity id)
	{
		if (!id)
			return "";
		return id.GetName();
	}

	// A session that can receive packets: a connected player (or a self-test session, TestOnline).
	static bool Online(INT_PDA_SE_Session s)
	{
		if (!s)
			return false;
		if (s.Identity)
			return true;
		return s.TestOnline;
	}

	// Packet to a session: the player; a self-test session only lands in the capture as "op:steam".
	static void SendToSession(ScriptRPC rpc, INT_PDA_SE_Session s, int op)
	{
		if (!s)
			return;
		if (s.Identity)
		{
			SendTo(rpc, s.Identity, op);
			return;
		}
		if (s.TestOnline && s_Capture)
			s_Capture.Insert(op.ToString() + ":" + s.Steam);
	}

	static void SendTo(ScriptRPC rpc, PlayerIdentity to, int op)
	{
		if (s_Capture)
		{
			string who = "-";
			if (to)
				who = to.GetPlainId();
			s_Capture.Insert(op.ToString() + ":" + who);
		}
		if (s_NoSend || !to)
			return;
		rpc.Send(null, INT_PDA_Const.RPC_S2C, true, to);
		s_Out++;
	}

	// Self-test bookkeeping of every answer code (two int writes, no logic).
	static void NoteResult(int op, int sub, int code)
	{
		s_LastOp = op;
		s_LastSub = sub;
		s_LastCode = code;
		s_Results++;
	}

	static void SendResult(PlayerIdentity to, int reqOp, int code)
	{
		NoteResult(reqOp, 0, code);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_RESULT);
		rpc.Write(reqOp);
		rpc.Write(code);
		SendTo(rpc, to, INT_PDA_Const.OP_RESULT);
	}

	static INT_PDA_Profile ProfileOf(INT_PDA_Account a)
	{
		INT_PDA_Profile p = new INT_PDA_Profile();
		p.SteamId = a.SteamId;
		p.Callsign = a.Callsign;
		p.Discord = a.Discord;
		p.Avatar = a.Avatar;
		p.Registered = a.Registered;
		p.DaysInZone = INT_PDA_SE_Clock.DaysSince(a.Registered);
		p.FactionId = a.FactionId;
		p.Role = a.Role;
		p.Rank = a.Rank;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (f)
		{
			p.FactionName = f.Name;
			p.FactionEmblem = f.Emblem;
		}
		// the play-time stat lives on the server only (always 0 on clients)
		INT_PDA_SE_Session s = s_Sessions.Get(a.SteamId);
		if (s && s.Identity)
		{
			PlayerBase player = PlayerBase.Cast(s.Identity.GetPlayer());
			if (player)
				p.LifeSec = (int)player.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		}
		return p;
	}

	static void SendProfile(PlayerIdentity to, INT_PDA_Account a)
	{
		if (!to || !a)
			return;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PROFILE);
		ProfileOf(a).Write(rpc);
		SendTo(rpc, to, INT_PDA_Const.OP_PROFILE);
	}

	// Profile push to a player only if he is online (after an admin or faction change).
	static void PushProfile(string steam)
	{
		INT_PDA_SE_Session s = s_Sessions.Get(steam);
		if (!s || !s.LoggedIn || !Online(s))
			return;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
		if (!a)
			return;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PROFILE);
		ProfileOf(a).Write(rpc);
		SendToSession(rpc, s, INT_PDA_Const.OP_PROFILE);
	}
}
