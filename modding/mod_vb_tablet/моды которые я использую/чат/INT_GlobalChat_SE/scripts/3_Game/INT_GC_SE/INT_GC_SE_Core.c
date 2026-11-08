// INT GlobalChat (server) — the whole server logic (spec.md §4.3-4.5). Event-driven only: a client packet, a
// disconnect or a call of the server API. No timers, no OnUpdate, no scans (owner rules 1-3). Every check is a
// map lookup, except /pm which walks the player list once when a private message is sent.
// The RPC comes through the static DayZGame.Event_OnRPC (no modded DayZGame). The nick is always taken from the
// sender's PlayerIdentity, never from the packet.

class INT_GC_SE_Online
{
	string Uid;			// PlayerIdentity.GetId()
	string Steam;		// PlayerIdentity.GetPlainId()
	string Name;
	int LastHello = -100000;
	bool Announced;
}

class INT_GC_SE_Core
{
	protected static bool s_Hooked;
	protected static ref map<string, ref INT_GC_SE_Online> s_Online;	// by GetId(): join/leave + HELLO guard
	protected static ref map<string, int> s_Strikes;					// by SteamID64, this restart only

	// counters for Debug and the self-test
	static int s_In;
	static int s_Relayed;
	static int s_Dropped;
	static int s_Pm;
	static int s_Violations;
	static int s_MutesGiven;

	// Self-test capture: when set, every outgoing packet is recorded as "op:who"; with s_NoSend the packet is not
	// sent at all (pure logic tests without a client).
	static ref array<string> s_Capture;
	static bool s_NoSend;

	static void Init()
	{
		if (!s_Online)
			s_Online = new map<string, ref INT_GC_SE_Online>();
		if (!s_Strikes)
			s_Strikes = new map<string, int>();
		if (s_Hooked || !GetGame() || !GetGame().IsDedicatedServer())
			return;
		if (DayZGame.Event_OnRPC.Count(INT_GC_SE_Core.OnRPC) == 0)
			DayZGame.Event_OnRPC.Insert(INT_GC_SE_Core.OnRPC);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (!s_Hooked)
			return;
		if (DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(INT_GC_SE_Core.OnRPC);
		s_Hooked = false;
	}

	static int Strikes(string steamId)
	{
		int n = 0;
		if (s_Strikes)
			s_Strikes.Find(steamId, n);
		return n;
	}

	static int OnlineCount()
	{
		if (!s_Online)
			return 0;
		return s_Online.Count();
	}

	// --- network in ---------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_GC_Const.RPC_C2S || target)
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
		if (!INT_GC_Proto.ReadHeader(ctx, op))
		{
			s_Dropped++;
			INT_GC_Log.Warn("пакет не прочитан (другая версия мода у клиента?) от " + INT_GC_Log.Clip(sender.GetName(), 32));
			return;
		}
		if (op == INT_GC_Const.OP_HELLO)
		{
			OnHello(sender);
			return;
		}
		if (op == INT_GC_Const.OP_MSG)
		{
			OnMsg(sender, ctx);
			return;
		}
		if (op == INT_GC_Const.OP_PM)
		{
			OnPm(sender, ctx);
			return;
		}
		if (op == INT_GC_Const.OP_VIOL)
		{
			OnViolation(sender, ctx);
			return;
		}
		s_Dropped++;
		INT_GC_Log.Dbg("неизвестная операция " + op.ToString());
	}

	protected static bool TextFits(string text, int maxBytes)
	{
		return text.Length() <= maxBytes;
	}

	// --- HELLO: welcome bundle + word list; join line once per session ------------------------------------------
	protected static void OnHello(PlayerIdentity sender)
	{
		string uid = sender.GetId();
		string steam = sender.GetPlainId();
		string name = sender.GetName();
		int now = GetGame().GetTime();
		INT_GC_SE_Online o;
		if (!s_Online.Find(uid, o))
		{
			o = new INT_GC_SE_Online();
			o.Uid = uid;
			s_Online.Set(uid, o);
		}
		if (now - o.LastHello < INT_GC_Const.HELLO_GAP_MS)
		{
			s_Dropped++;
			INT_GC_Log.Dbg("HELLO слишком часто, пропущен: " + INT_GC_Log.Clip(name, 32));
			return;
		}
		o.LastHello = now;
		o.Steam = steam;
		o.Name = name;
		SendWelcome(sender, steam);
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		if (!o.Announced)
		{
			o.Announced = true;
			if (cfg.ShowJoinLeave == 1)
				SendSys(null, INT_GC_Const.SYS_JOIN, name, null);
		}
		if (!INT_GC_Log.s_Debug)
			return;
		string shortName = INT_GC_Log.Clip(name, 32);
		bool muted = INT_GC_SE_Config.IsMuted(steam);
		int strikes = Strikes(steam);
		INT_GC_Log.Dbg("HELLO " + shortName + " мут=" + muted.ToString() + " страйков=" + strikes.ToString());
	}

	static void SendWelcome(PlayerIdentity to, string steam)
	{
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		array<string> words = INT_GC_SE_Config.Words();
		array<ref array<string>> parts = new array<ref array<string>>();
		INT_GC_Proto.SplitParts(words, INT_GC_Const.WORDS_PART_BYTES, parts);
		int flags = 0;
		if (cfg.DebugMode == 1)
			flags = flags | INT_GC_Const.FLAG_DEBUG;
		if (INT_GC_SE_Config.IsMuted(steam))
			flags = flags | INT_GC_Const.FLAG_MUTED;
		if (INT_GC_SE_Config.IsAdmin(steam))
			flags = flags | INT_GC_Const.FLAG_ADMIN;
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		int strikes = Strikes(steam);
		int nParts = parts.Count();
		ScriptRPC rpc = INT_GC_Proto.Begin(INT_GC_Const.OP_WELCOME);
		rpc.Write(flags);
		rpc.Write(strikes);
		rpc.Write(cfg.StrikesForPermanentMute);
		rpc.Write(cfg.MaxMessageLength);
		rpc.Write(cfg.MinSecondsBetweenMessages);
		rpc.Write(cfg.SameMessageCooldownSeconds);
		rpc.Write(h);
		rpc.Write(mi);
		rpc.Write(s);
		INT_GC_Proto.WriteStrings(rpc, cfg.VanillaCommandPrefixes);
		rpc.Write(nParts);
		Send(rpc, to, INT_GC_Const.OP_WELCOME);
		for (int i = 0; i < nParts; i++)
		{
			ScriptRPC w = INT_GC_Proto.Begin(INT_GC_Const.OP_WORDS);
			w.Write(i);
			w.Write(nParts);
			INT_GC_Proto.WriteStrings(w, parts.Get(i));
			Send(w, to, INT_GC_Const.OP_WORDS);
		}
	}

	// --- MSG: one broadcast ------------------------------------------------------------------------------------
	protected static void OnMsg(PlayerIdentity sender, ParamsReadContext ctx)
	{
		int colorIdx;
		string text;
		if (!ctx.Read(colorIdx) || !ctx.Read(text))
		{
			s_Dropped++;
			return;
		}
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		if (!TextFits(text, INT_GC_Const.MaxTextBytes(cfg.MaxMessageLength)) || text == "")
		{
			s_Dropped++;
			int bytes = text.Length();
			INT_GC_Log.Dbg("сообщение отброшено (длина " + bytes.ToString() + " байт)");
			return;
		}
		string steam = sender.GetPlainId();
		if (INT_GC_SE_Config.IsMuted(steam))
		{
			SendNotice(sender, INT_GC_Const.NOTICE_IS_MUTED, 0, 0);
			INT_GC_Log.Dbg("мут, не разослано: " + INT_GC_Log.Clip(sender.GetName(), 32));
			return;
		}
		Relay(sender.GetName(), sender.GetId(), colorIdx, INT_GC_SE_Config.IsAdmin(steam), text);
	}

	// Sends one CHAT line to every client. Public for the self-test.
	static void Relay(string name, string uid, int colorIdx, bool admin, string text)
	{
		int flags = 0;
		if (admin)
			flags = INT_GC_Const.FLAG_ADMIN;
		ScriptRPC rpc = INT_GC_Proto.Begin(INT_GC_Const.OP_CHAT);
		rpc.Write(name);
		rpc.Write(uid);
		rpc.Write(colorIdx);
		rpc.Write(flags);
		rpc.Write(text);
		Send(rpc, null, INT_GC_Const.OP_CHAT);
		s_Relayed++;
		if (!INT_GC_Log.s_Debug)
			return;
		string shortName = INT_GC_Log.Clip(name, 32);
		string shortText = INT_GC_Log.Clip(text, 80);
		INT_GC_Log.Dbg("ГЛОБАЛ " + shortName + ": " + shortText);
	}

	// --- PM: find the addressee by nick once --------------------------------------------------------------------
	protected static void OnPm(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string targetName;
		int colorIdx;
		string text;
		if (!ctx.Read(targetName) || !ctx.Read(colorIdx) || !ctx.Read(text))
		{
			s_Dropped++;
			return;
		}
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		if (!TextFits(text, INT_GC_Const.MaxTextBytes(cfg.MaxMessageLength)) || text == "" || !TextFits(targetName, INT_GC_Const.MAX_NAME_BYTES))
		{
			s_Dropped++;
			return;
		}
		string steam = sender.GetPlainId();
		if (INT_GC_SE_Config.IsMuted(steam))
		{
			SendNotice(sender, INT_GC_Const.NOTICE_IS_MUTED, 0, 0);
			return;
		}
		PlayerIdentity to = FindByName(targetName);
		if (!to)
		{
			ScriptRPC fail = INT_GC_Proto.Begin(INT_GC_Const.OP_PM_FAIL);
			fail.Write(targetName);
			Send(fail, sender, INT_GC_Const.OP_PM_FAIL);
			INT_GC_Log.Dbg("ЛС: адресат не найден '" + INT_GC_Log.Clip(targetName, 32) + "'");
			return;
		}
		int flags = 0;
		if (INT_GC_SE_Config.IsAdmin(steam))
			flags = INT_GC_Const.FLAG_ADMIN;
		string fromName = sender.GetName();
		string fromId = sender.GetId();
		ScriptRPC inRpc = INT_GC_Proto.Begin(INT_GC_Const.OP_PM_IN);
		inRpc.Write(fromName);
		inRpc.Write(fromId);
		inRpc.Write(colorIdx);
		inRpc.Write(flags);
		inRpc.Write(text);
		Send(inRpc, to, INT_GC_Const.OP_PM_IN);
		string toName = to.GetName();
		string toId = to.GetId();
		ScriptRPC outRpc = INT_GC_Proto.Begin(INT_GC_Const.OP_PM_OUT);
		outRpc.Write(toName);
		outRpc.Write(toId);
		outRpc.Write(colorIdx);
		outRpc.Write(flags);
		outRpc.Write(text);
		Send(outRpc, sender, INT_GC_Const.OP_PM_OUT);
		s_Pm++;
		if (!INT_GC_Log.s_Debug)
			return;
		string a = INT_GC_Log.Clip(fromName, 32);
		string b = INT_GC_Log.Clip(toName, 32);
		string c = INT_GC_Log.Clip(text, 60);
		INT_GC_Log.Dbg("ЛС " + a + " -> " + b + ": " + c);
	}

	// Exact nick, case-insensitive (Russian and ASCII). The first exact-case match wins over a case-insensitive one.
	static PlayerIdentity FindByName(string name)
	{
		if (name == "")
			return null;
		string want = INT_GC_Util.Lower(name);
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		PlayerIdentity loose = null;
		foreach (Man m : players)
		{
			if (!m)
				continue;
			PlayerIdentity id = m.GetIdentity();
			if (!id)
				continue;
			string n = id.GetName();
			if (n == name)
				return id;
			if (!loose && INT_GC_Util.Lower(n) == want)
				loose = id;
		}
		return loose;
	}

	// --- VIOL: strikes this restart, permanent mute at N --------------------------------------------------------
	protected static void OnViolation(PlayerIdentity sender, ParamsReadContext ctx)
	{
		int channel;
		string text;
		if (!ctx.Read(channel) || !ctx.Read(text))
		{
			s_Dropped++;
			return;
		}
		if (!TextFits(text, INT_GC_Const.MaxTextBytes(500)))
			text = "";
		string steam = sender.GetPlainId();
		string name = sender.GetName();
		if (INT_GC_Log.s_Debug)
		{
			string shortName = INT_GC_Log.Clip(name, 32);
			string shortText = INT_GC_Log.Clip(text, 80);
			INT_GC_Log.Dbg("нарушение " + shortName + " канал " + channel.ToString() + ": " + shortText);
		}
		Violation(sender, steam, name);
	}

	// to = the violator (null in tests). Returns the strike count after this violation (0 = already muted).
	static int Violation(PlayerIdentity to, string steam, string name)
	{
		if (steam == "" || INT_GC_SE_Config.IsMuted(steam))
			return 0;
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		int n = Strikes(steam) + 1;
		s_Strikes.Set(steam, n);
		s_Violations++;
		int maxStrikes = cfg.StrikesForPermanentMute;
		if (n < maxStrikes)
		{
			SendSys(null, INT_GC_Const.SYS_VIOLATION, name, null);
			if (to)
				SendNotice(to, INT_GC_Const.NOTICE_WARN, n, maxStrikes);
			string who = INT_GC_Log.Clip(name, 32);
			INT_GC_Log.Info("автомодерация: " + who + " нарушение " + n.ToString() + " из " + maxStrikes.ToString());
			return n;
		}
		string reason = "автомодерация, нарушений за рестарт: " + n.ToString();
		INT_GC_SE_Config.AddMute(steam, name, reason);
		s_MutesGiven++;
		SendSys(null, INT_GC_Const.SYS_MUTED, name, null);
		if (to)
			SendNotice(to, INT_GC_Const.NOTICE_MUTED, n, maxStrikes);
		string whoMuted = INT_GC_Log.Clip(name, 32);
		INT_GC_Log.Info("автомодерация: " + whoMuted + " получил перманентный бан чата (" + steam + ")");
		return n;
	}

	// --- disconnect ---------------------------------------------------------------------------------------------
	static void OnDisconnected(string uid)
	{
		if (!s_Online || uid == "")
			return;
		INT_GC_SE_Online o;
		if (!s_Online.Find(uid, o))
			return;
		s_Online.Remove(uid);
		if (!o || !o.Announced)
			return;
		INT_GC_SE_ConfigData cfg = INT_GC_SE_Config.Data();
		if (cfg.ShowJoinLeave == 1)
			SendSys(null, INT_GC_Const.SYS_LEAVE, o.Name, null);
	}

	// --- network out --------------------------------------------------------------------------------------------
	// to = null -> every client.
	static void SendSys(PlayerIdentity to, int kind, string nameOrKey, array<string> args)
	{
		ScriptRPC rpc = INT_GC_Proto.Begin(INT_GC_Const.OP_SYS);
		rpc.Write(kind);
		rpc.Write(nameOrKey);
		INT_GC_Proto.WriteStrings(rpc, args);
		Send(rpc, to, INT_GC_Const.OP_SYS);
	}

	static void SendNotice(PlayerIdentity to, int kind, int a, int b)
	{
		if (!to)
			return;
		ScriptRPC rpc = INT_GC_Proto.Begin(INT_GC_Const.OP_NOTICE);
		rpc.Write(kind);
		rpc.Write(a);
		rpc.Write(b);
		Send(rpc, to, INT_GC_Const.OP_NOTICE);
	}

	protected static void Send(ScriptRPC rpc, PlayerIdentity to, int op)
	{
		if (s_Capture)
		{
			string who = "all";
			if (to)
				who = to.GetName();
			s_Capture.Insert(op.ToString() + ":" + who);
		}
		if (s_NoSend)
			return;
		rpc.Send(null, INT_GC_Const.RPC_S2C, true, to);
	}

	// Self-test only: forget strikes and the online list.
	static void ResetState()
	{
		s_Online = new map<string, ref INT_GC_SE_Online>();
		s_Strikes = new map<string, int>();
	}
}
