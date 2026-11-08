// INT GlobalChat — client core without UI (spec.md §5.2-5.6). Receives server packets (static
// DayZGame.Event_OnRPC, no modded DayZGame), vanilla lines (from the modded Chat.Add in 5_Mission) and the player's
// input; owns the filter, anti-flood, mute state, block list, colour cache, mentions, server time and history.
// The UI (5_Mission) only listens to OnLine / OnState and calls Submit. Never builds widgets.

class INT_GC_Client
{
	// Submit results
	static const int R_SENT = 0;
	static const int R_EMPTY = 1;
	static const int R_MUTED = 2;
	static const int R_FLOOD = 3;
	static const int R_BANNED = 4;
	static const int R_COMMAND = 5;
	static const int R_NOGLOBAL = 6;
	static const int R_PM_USAGE = 7;
	static const int R_HELP = 8;

	static const int HISTORY_LIMIT = 200;
	static const int SENT_LIMIT = 30;
	static const float HELLO_RETRY = 10.0;
	static const int HELLO_TRIES = 4;

	protected static bool s_Hooked;
	protected static bool s_Welcome;
	protected static bool s_Muted;
	protected static bool s_Admin;
	protected static int s_Strikes;
	protected static int s_StrikesMax = 3;
	protected static int s_MaxLen = 200;
	protected static int s_MinInterval = 2;
	protected static int s_DupInterval = 10;
	protected static ref array<string> s_Prefixes;
	protected static ref INT_GC_Filter s_Filter;
	protected static ref array<string> s_WordsBuf;
	protected static int s_WordsParts;
	protected static int s_WordsGot;
	protected static int s_TimeOffset;			// server seconds of day - local seconds of day
	protected static float s_HelloAt = -1000;
	protected static int s_HelloTries;

	protected static ref array<ref INT_GC_Line> s_History;
	protected static int s_Seq;
	protected static ref map<string, int> s_NickColor;		// lower nick -> colour index (learnt from global lines)
	protected static ref map<string, bool> s_NickAdmin;
	protected static ref map<string, string> s_NickCase;	// lower nick -> nick as written
	protected static float s_LastSendAt = -1000;
	protected static string s_LastText;
	protected static float s_LastTextAt = -1000;
	protected static ref array<string> s_Sent;

	protected static ref ScriptInvoker s_OnLine;
	protected static ref ScriptInvoker s_OnState;

	// counters (Debug / self-test)
	static int s_Received;
	static int s_Dropped;
	static int s_Blocked;

	// --- life cycle ---------------------------------------------------------------------------------------------
	static void Init()
	{
		Ensure();
		if (s_Hooked || !GetGame() || GetGame().IsDedicatedServer())
			return;
		if (DayZGame.Event_OnRPC.Count(INT_GC_Client.OnRPC) == 0)
			DayZGame.Event_OnRPC.Insert(INT_GC_Client.OnRPC);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked && DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(INT_GC_Client.OnRPC);
		s_Hooked = false;
		s_Welcome = false;
		s_HelloTries = 0;
		s_HelloAt = -1000;
	}

	protected static void Ensure()
	{
		if (s_History)
			return;
		s_History = new array<ref INT_GC_Line>();
		s_Filter = new INT_GC_Filter();
		s_WordsBuf = new array<string>();
		s_Prefixes = new array<string>();
		s_Prefixes.Insert("#");
		s_Prefixes.Insert("@fpromo");
		s_NickColor = new map<string, int>();
		s_NickAdmin = new map<string, bool>();
		s_NickCase = new map<string, string>();
		s_Sent = new array<string>();
	}

	protected static bool Has(int flags, int flag)
	{
		if ((flags & flag) != 0)
			return true;
		return false;
	}

	// Called every frame by the mission: HELLO once the local player exists, retried a few times (client side only).
	static void Tick(float dt)
	{
		if (s_Welcome || s_HelloTries >= HELLO_TRIES || !s_Hooked)
			return;
		if (!GetGame() || !GetGame().GetPlayer())
			return;
		float now = GetGame().GetTickTime();
		if (now - s_HelloAt < HELLO_RETRY)
			return;
		s_HelloAt = now;
		s_HelloTries++;
		ScriptRPC rpc = INT_GC_Proto.Begin(INT_GC_Const.OP_HELLO);
		rpc.Send(null, INT_GC_Const.RPC_C2S, true, null);
		INT_GC_Log.Dbg("HELLO отправлен, попытка " + s_HelloTries.ToString());
		if (s_HelloTries == HELLO_TRIES)
			INT_GC_Log.Warn("сервер не ответил на HELLO: нет серверной части INT_GlobalChat_SE? Глобальный канал недоступен");
	}

	// --- state --------------------------------------------------------------------------------------------------
	static bool IsWelcome()
	{
		return s_Welcome;
	}

	static bool IsMuted()
	{
		return s_Muted;
	}

	static bool IsAdmin()
	{
		return s_Admin;
	}

	static int Strikes()
	{
		return s_Strikes;
	}

	static int StrikesMax()
	{
		return s_StrikesMax;
	}

	static int MaxLen()
	{
		return s_MaxLen;
	}

	static INT_GC_Filter Filter()
	{
		Ensure();
		return s_Filter;
	}

	static array<ref INT_GC_Line> History()
	{
		Ensure();
		return s_History;
	}

	static array<string> SentHistory()
	{
		Ensure();
		return s_Sent;
	}

	static ScriptInvoker OnLine()			// (INT_GC_Line line)
	{
		if (!s_OnLine)
			s_OnLine = new ScriptInvoker();
		return s_OnLine;
	}

	static ScriptInvoker OnState()			// () mute / welcome / strikes changed
	{
		if (!s_OnState)
			s_OnState = new ScriptInvoker();
		return s_OnState;
	}

	static string MyName()
	{
		string n = "";
		if (GetGame())
		{
			Man me = GetGame().GetPlayer();
			if (me && me.GetIdentity())
				n = me.GetIdentity().GetName();
			if (n == "")
				GetGame().GetPlayerName(n);
		}
		return n;
	}

	// Server time "HH:MM" (local time before WELCOME).
	static void ServerTime(out int h, out int m)
	{
		int lh;
		int lm;
		int ls;
		GetHourMinuteSecond(lh, lm, ls);
		int sec = lh * 3600 + lm * 60 + ls + s_TimeOffset;
		while (sec < 0)
			sec = sec + 86400;
		while (sec >= 86400)
			sec = sec - 86400;
		h = sec / 3600;
		m = (sec / 60) - h * 60;
	}

	// --- network in ---------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_GC_Const.RPC_S2C || target)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int op;
		if (!INT_GC_Proto.ReadHeader(ctx, op))
		{
			s_Dropped++;
			INT_GC_Log.Warn("пакет сервера не прочитан (другая версия мода на сервере?)");
			return;
		}
		s_Received++;
		bool ok = false;
		if (op == INT_GC_Const.OP_WELCOME)
			ok = ReadWelcome(ctx);
		else if (op == INT_GC_Const.OP_WORDS)
			ok = ReadWords(ctx);
		else if (op == INT_GC_Const.OP_CHAT)
			ok = ReadChat(ctx);
		else if (op == INT_GC_Const.OP_PM_IN)
			ok = ReadPm(ctx, true);
		else if (op == INT_GC_Const.OP_PM_OUT)
			ok = ReadPm(ctx, false);
		else if (op == INT_GC_Const.OP_PM_FAIL)
			ok = ReadPmFail(ctx);
		else if (op == INT_GC_Const.OP_SYS)
			ok = ReadSys(ctx);
		else if (op == INT_GC_Const.OP_NOTICE)
			ok = ReadNotice(ctx);
		if (!ok)
		{
			s_Dropped++;
			INT_GC_Log.Warn("пакет сервера " + op.ToString() + " не прочитан");
		}
	}

	protected static bool ReadWelcome(ParamsReadContext ctx)
	{
		int flags;
		int strikes;
		int strikesMax;
		int maxLen;
		int minInterval;
		int dupInterval;
		int h;
		int mi;
		int s;
		int parts;
		if (!ctx.Read(flags) || !ctx.Read(strikes) || !ctx.Read(strikesMax) || !ctx.Read(maxLen))
			return false;
		if (!ctx.Read(minInterval) || !ctx.Read(dupInterval) || !ctx.Read(h) || !ctx.Read(mi) || !ctx.Read(s))
			return false;
		array<string> pre = new array<string>();
		if (!INT_GC_Proto.ReadStrings(ctx, pre, INT_GC_Const.MAX_PREFIXES))
			return false;
		if (!ctx.Read(parts))
			return false;
		Ensure();
		s_Welcome = true;
		bool muted = Has(flags, INT_GC_Const.FLAG_MUTED);
		bool admin = Has(flags, INT_GC_Const.FLAG_ADMIN);
		s_Muted = muted;
		s_Admin = admin;
		if (Has(flags, INT_GC_Const.FLAG_DEBUG))
			INT_GC_Log.s_Debug = true;
		s_Strikes = strikes;
		s_StrikesMax = Math.Max(1, strikesMax);
		s_MaxLen = Math.Clamp(maxLen, 20, 500);
		s_MinInterval = Math.Clamp(minInterval, 0, 60);
		s_DupInterval = Math.Clamp(dupInterval, 0, 600);
		s_Prefixes.Clear();
		foreach (string p : pre)
		{
			if (p != "")
				s_Prefixes.Insert(p);
		}
		int lh;
		int lm;
		int ls;
		GetHourMinuteSecond(lh, lm, ls);
		int diff = (h * 3600 + mi * 60 + s) - (lh * 3600 + lm * 60 + ls);
		if (diff > 43200)
			diff = diff - 86400;
		if (diff < -43200)
			diff = diff + 86400;
		s_TimeOffset = diff;
		s_WordsBuf.Clear();
		s_WordsParts = Math.Clamp(parts, 0, 64);
		s_WordsGot = 0;
		if (s_WordsParts == 0)
			s_Filter.Clear();
		INT_GC_Log.Dbg("WELCOME мут=" + s_Muted.ToString() + " админ=" + s_Admin.ToString() + " страйков=" + s_Strikes.ToString() + " частей слов=" + s_WordsParts.ToString());
		OnState().Invoke();
		return true;
	}

	protected static bool ReadWords(ParamsReadContext ctx)
	{
		int part;
		int parts;
		if (!ctx.Read(part) || !ctx.Read(parts))
			return false;
		array<string> words = new array<string>();
		if (!INT_GC_Proto.ReadStrings(ctx, words, INT_GC_Const.MAX_WORDS))
			return false;
		Ensure();
		foreach (string w : words)
		{
			if (s_WordsBuf.Count() < INT_GC_Const.MAX_WORDS)
				s_WordsBuf.Insert(w);
		}
		s_WordsGot++;
		if (s_WordsGot >= s_WordsParts)
		{
			s_Filter.SetWords(s_WordsBuf);
			INT_GC_Log.Dbg("список слов принят: " + s_Filter.WordCount().ToString() + " слов, форм " + s_Filter.FormCount().ToString());
		}
		return true;
	}

	protected static bool ReadChat(ParamsReadContext ctx)
	{
		string name;
		string uid;
		int colorIdx;
		int flags;
		string text;
		if (!ctx.Read(name) || !ctx.Read(uid) || !ctx.Read(colorIdx) || !ctx.Read(flags) || !ctx.Read(text))
			return false;
		string cleanName = INT_GC_Util.CleanText(name, 64);
		string cleanText = INT_GC_Util.CleanText(text, s_MaxLen);
		int ci = INT_GC_Palette.Clamp(colorIdx);
		bool admin = Has(flags, INT_GC_Const.FLAG_ADMIN);
		Learn(cleanName, ci, admin);
		bool blocked = INT_GC_UI.Settings().IsBlocked(uid, cleanName);
		if (blocked)
		{
			s_Blocked++;
			return true;
		}
		INT_GC_Line l = new INT_GC_Line();
		l.Kind = INT_GC_Line.KIND_GLOBAL;
		l.Name = cleanName;
		l.SenderId = uid;
		l.ColorIdx = ci;
		l.Admin = admin;
		l.Text = cleanText;
		AddLine(l);
		return true;
	}

	protected static bool ReadPm(ParamsReadContext ctx, bool incoming)
	{
		string name;
		string uid;
		int colorIdx;
		int flags;
		string text;
		if (!ctx.Read(name) || !ctx.Read(uid) || !ctx.Read(colorIdx) || !ctx.Read(flags) || !ctx.Read(text))
			return false;
		string cleanName = INT_GC_Util.CleanText(name, 64);
		string cleanText = INT_GC_Util.CleanText(text, s_MaxLen);
		int ci = INT_GC_Palette.Clamp(colorIdx);
		bool admin = Has(flags, INT_GC_Const.FLAG_ADMIN);
		if (incoming)
		{
			Learn(cleanName, ci, admin);
			bool blocked = INT_GC_UI.Settings().IsBlocked(uid, cleanName);
			if (blocked)
			{
				s_Blocked++;
				return true;
			}
		}
		else
		{
			ci = INT_GC_UI.Settings().ColorIdx;
		}
		INT_GC_Line l = new INT_GC_Line();
		l.Kind = INT_GC_Line.KIND_PM_OUT;
		if (incoming)
			l.Kind = INT_GC_Line.KIND_PM_IN;
		else
			l.Mine = true;
		l.Name = cleanName;
		l.SenderId = uid;
		l.ColorIdx = ci;
		l.Admin = admin;
		l.Text = cleanText;
		AddLine(l);
		return true;
	}

	protected static bool ReadPmFail(ParamsReadContext ctx)
	{
		string name;
		if (!ctx.Read(name))
			return false;
		string clean = INT_GC_Util.CleanText(name, 64);
		AddNotice(Fmt1("#STR_INT_GC_PM_NOTFOUND", clean), 0);
		return true;
	}

	protected static bool ReadSys(ParamsReadContext ctx)
	{
		int kind;
		string nameOrKey;
		if (!ctx.Read(kind) || !ctx.Read(nameOrKey))
			return false;
		array<string> args = new array<string>();
		if (!INT_GC_Proto.ReadStrings(ctx, args, 8))
			return false;
		INT_GC_Line l = new INT_GC_Line();
		string who = INT_GC_Util.CleanText(nameOrKey, 64);
		if (kind == INT_GC_Const.SYS_VIOLATION)
		{
			l.Kind = INT_GC_Line.KIND_MOD;
			l.Text = Fmt1("#STR_INT_GC_MOD_VIOLATION", who);
		}
		else if (kind == INT_GC_Const.SYS_MUTED)
		{
			l.Kind = INT_GC_Line.KIND_MOD;
			l.Text = Fmt1("#STR_INT_GC_MOD_MUTED", who);
		}
		else if (kind == INT_GC_Const.SYS_JOIN)
		{
			l.Kind = INT_GC_Line.KIND_JOIN;
			l.Text = Fmt1("#STR_INT_GC_JOIN", who);
		}
		else if (kind == INT_GC_Const.SYS_LEAVE)
		{
			l.Kind = INT_GC_Line.KIND_JOIN;
			l.Text = Fmt1("#STR_INT_GC_LEAVE", who);
		}
		else if (kind == INT_GC_Const.SYS_TEXT)
		{
			l.Kind = INT_GC_Line.KIND_SYSTEM;
			l.Text = INT_GC_Util.CleanText(nameOrKey, 1000);
		}
		else if (kind == INT_GC_Const.SYS_KEY)
		{
			l.Kind = INT_GC_Line.KIND_SYSTEM;
			l.Text = FmtKey(nameOrKey, args);
		}
		else
		{
			return true;
		}
		if (l.Kind == INT_GC_Line.KIND_SYSTEM)
			l.Tag = "#STR_INT_GC_TAG_SERVER";
		AddLine(l);
		return true;
	}

	protected static bool ReadNotice(ParamsReadContext ctx)
	{
		int kind;
		int a;
		int b;
		if (!ctx.Read(kind) || !ctx.Read(a) || !ctx.Read(b))
			return false;
		if (kind == INT_GC_Const.NOTICE_WARN)
		{
			s_Strikes = a;
			s_StrikesMax = Math.Max(1, b);
			AddNotice(Fmt2("#STR_INT_GC_NOTICE_WARN", a.ToString(), b.ToString()), 1);
		}
		else if (kind == INT_GC_Const.NOTICE_MUTED)
		{
			s_Muted = true;
			s_Strikes = a;
			AddNotice(Translate("#STR_INT_GC_NOTICE_MUTED"), 2);
		}
		else if (kind == INT_GC_Const.NOTICE_IS_MUTED)
		{
			s_Muted = true;
			AddNotice(Translate("#STR_INT_GC_NOTICE_IS_MUTED"), 2);
		}
		OnState().Invoke();
		return true;
	}

	// --- vanilla lines (modded Chat.Add) ------------------------------------------------------------------------
	// channel = vanilla bit mask (CCSystem 1, CCAdmin 2, CCDirect 4, CCMegaphone 8, CCTransmitter 16, CCPublicAddress 32,
	// CCBattlEye 64, 0 = local message to self). colorClass = vanilla colour config name ("colorAction"...).
	static void PushVanilla(int channel, string from, string text, string colorClass)
	{
		Ensure();
		string name = INT_GC_Util.CleanText(from, 64);
		string body = INT_GC_Util.CleanText(text, 1000);
		if (body == "" && name == "")
			return;
		INT_GC_Line l = new INT_GC_Line();
		bool isLocal = false;
		if ((channel & (4 | 8 | 16 | 32)) != 0)
			isLocal = true;
		if (channel == 0 && name != "")
			isLocal = true;
		if (isLocal)
		{
			string me = MyName();
			bool mine = false;
			if (name != "" && name == me)
				mine = true;
			// commands to vanilla (#..., @fpromo ...) of other players are never shown: promo codes stay private
			bool command = IsCommand(body);
			if (!mine && command)
				return;
			bool blocked = INT_GC_UI.Settings().IsBlocked("", name);
			if (!mine && blocked)
			{
				s_Blocked++;
				return;
			}
			string lname = INT_GC_Util.Lower(name);
			int ci = -1;
			s_NickColor.Find(lname, ci);
			bool adm = false;
			s_NickAdmin.Find(lname, adm);
			if (mine)
				ci = INT_GC_UI.Settings().ColorIdx;
			l.Kind = INT_GC_Line.KIND_LOCAL;
			l.Name = name;
			l.Text = body;
			l.Mine = mine;
			l.ColorIdx = ci;
			l.Admin = adm;
			if ((channel & 16) != 0)
				l.Tag = "#STR_INT_GC_TAG_RADIO";
			else if ((channel & (8 | 32)) != 0)
				l.Tag = "#STR_INT_GC_TAG_MEGAPHONE";
			else
				l.Tag = "#STR_INT_GC_TAG_LOCAL";
		}
		else
		{
			// Server, admin, BattlEye and other mods' lines: text only. Mods put their own name in `from`
			// (SPObjectSearch: CCBattlEye + "SPObjectSearch"), the owner wants it never shown; BattlEye gets SERVER.
			// Mods also send stringtable keys (StashSearch: MessageAction("#ss_searched")): vanilla ChatLine
			// translates them in TextWidget.SetText, so the line gets the translated text here, as in vanilla.
			if (body.IndexOf("#") == 0)
			{
				string translated = Widget.TranslateString(body);
				body = INT_GC_Util.CleanText(translated, 1000);
			}
			if (body == "")
				return;
			l.Kind = INT_GC_Line.KIND_SYSTEM;
			l.Text = body;
			if ((channel & 2) != 0)
				l.Tag = "#STR_INT_GC_TAG_ADMIN";
			else
				l.Tag = "#STR_INT_GC_TAG_SERVER";
			int vc = INT_GC_UI.VanillaColor(colorClass);
			l.Color = vc;
		}
		AddLine(l);
	}

	// A line made on this client (public API, other client mods).
	static void PushLocal(string text, int color, string tag)
	{
		INT_GC_Line l = new INT_GC_Line();
		l.Kind = INT_GC_Line.KIND_SYSTEM;
		l.Text = INT_GC_Util.CleanText(text, 1000);
		l.Color = color;
		l.Tag = tag;
		if (l.Text == "")
			return;
		AddLine(l);
	}

	// --- history ------------------------------------------------------------------------------------------------
	protected static void Learn(string name, int colorIdx, bool admin)
	{
		if (name == "")
			return;
		string ln = INT_GC_Util.Lower(name);
		s_NickColor.Set(ln, colorIdx);
		s_NickAdmin.Set(ln, admin);
		s_NickCase.Set(ln, name);
	}

	protected static void AddNotice(string text, int level)
	{
		INT_GC_Line l = new INT_GC_Line();
		l.Kind = INT_GC_Line.KIND_NOTICE;
		l.Text = text;
		l.Tag = "#STR_INT_GC_TAG_NOTICE";
		l.Level = level;
		AddLine(l);
	}

	static void AddLine(INT_GC_Line l)
	{
		Ensure();
		int h;
		int m;
		ServerTime(h, m);
		l.Hour = h;
		l.Minute = m;
		if (GetGame())
			l.Born = GetGame().GetTickTime();
		s_Seq++;
		l.Seq = s_Seq;
		bool talk = false;
		if (l.Kind == INT_GC_Line.KIND_GLOBAL || l.Kind == INT_GC_Line.KIND_LOCAL || l.Kind == INT_GC_Line.KIND_PM_IN)
			talk = true;
		if (talk && !l.Mine && l.Name != "")
		{
			string me = MyName();
			if (me != "" && l.Name == me)
			{
				l.Mine = true;
			}
			else
			{
				bool mention = Mentions(l.Text, me);
				l.Mention = mention;
			}
		}
		s_History.Insert(l);
		while (s_History.Count() > HISTORY_LIMIT)
			s_History.RemoveOrdered(0);
		INT_GC_Log.Dbg("строка " + l.Describe());
		OnLine().Invoke(l);
	}

	static void ClearHistory()
	{
		Ensure();
		s_History.Clear();
	}

	// Drops the lines of a player who was just blocked (by id, or by nick for vanilla lines). Returns the count.
	static int RemoveFrom(string id, string name)
	{
		Ensure();
		int removed = 0;
		string lname = INT_GC_Util.Lower(name);
		for (int i = s_History.Count() - 1; i >= 0; i--)
		{
			INT_GC_Line l = s_History.Get(i);
			if (!l || l.Mine)
				continue;
			bool same = false;
			if (id != "" && l.SenderId == id)
				same = true;
			if (name != "" && l.Name != "" && INT_GC_Util.Lower(l.Name) == lname)
			{
				if (l.Kind == INT_GC_Line.KIND_GLOBAL || l.Kind == INT_GC_Line.KIND_LOCAL || l.Kind == INT_GC_Line.KIND_PM_IN)
					same = true;
			}
			if (same)
			{
				s_History.RemoveOrdered(i);
				removed++;
			}
		}
		return removed;
	}

	// A personal line from the UI (block notices etc.).
	static void Notice(string text, int level)
	{
		AddNotice(text, level);
	}

	// Whole-word occurrence of my nick (with or without '@'), case-insensitive.
	static bool Mentions(string text, string me)
	{
		if (me == "" || text == "")
			return false;
		string lt = INT_GC_Util.Lower(text);
		string lm = INT_GC_Util.Lower(me);
		int from = 0;
		int len = lm.Length();
		int total = lt.Length();
		while (from <= total - len)
		{
			int at = lt.IndexOfFrom(from, lm);
			if (at < 0)
				return false;
			bool leftOk = at == 0 || IsSep(lt.Get(at - 1));
			bool rightOk = at + len >= total || IsSep(lt.Get(at + len));
			if (leftOk && rightOk)
				return true;
			from = at + 1;
		}
		return false;
	}

	protected static bool IsSep(string c)
	{
		return " ,.!?:;@-()[]{}\"'/".IndexOf(c) >= 0;
	}

	// Known nick as written, "" when this client never saw it.
	static string KnownNick(string lowerName)
	{
		string n = "";
		if (s_NickCase)
			s_NickCase.Find(lowerName, n);
		return n;
	}

	// --- sending ------------------------------------------------------------------------------------------------
	static bool IsCommand(string text)
	{
		Ensure();
		foreach (string p : s_Prefixes)
		{
			if (INT_GC_Util.StartsWith(text, p))
				return true;
		}
		return false;
	}

	// Remaining seconds of the anti-flood wait for this text (0 = may send).
	static int FloodWait(string text)
	{
		if (!GetGame())
			return 0;
		float now = GetGame().GetTickTime();
		float wait = s_MinInterval - (now - s_LastSendAt);
		if (s_DupInterval > 0 && text == s_LastText)
		{
			float dup = s_DupInterval - (now - s_LastTextAt);
			if (dup > wait)
				wait = dup;
		}
		if (wait <= 0)
			return 0;
		int r = (int)Math.Ceil(wait);
		return r;
	}

	protected static void MarkSent(string text)
	{
		float now = GetGame().GetTickTime();
		s_LastSendAt = now;
		s_LastText = text;
		s_LastTextAt = now;
		if (s_Sent.Count() == 0 || s_Sent.Get(s_Sent.Count() - 1) != text)
			s_Sent.Insert(text);
		while (s_Sent.Count() > SENT_LIMIT)
			s_Sent.RemoveOrdered(0);
	}

	// The player pressed Enter. channel = the window's channel (CH_GLOBAL / CH_LOCAL).
	static int Submit(string raw, int channel)
	{
		Ensure();
		string text = INT_GC_Util.CleanText(raw, 0);
		if (text == "")
			return R_EMPTY;
		// commands to vanilla: admin "#", RF_PROMOCODES "@fpromo" — always, from any channel, never filtered
		if (IsCommand(text))
		{
			GetGame().ChatPlayer(text);
			INT_GC_Log.Dbg("команда в ваниль: " + INT_GC_Log.Clip(text, 40));
			return R_COMMAND;
		}
		string lower = INT_GC_Util.Lower(text);
		if (lower == "/help" || lower == "/?" || lower == "/помощь")
		{
			AddNotice(Translate("#STR_INT_GC_HELP"), 0);
			return R_HELP;
		}
		int ch = channel;
		string pmTo = "";
		string body = text;
		if (StartsWithWord(lower, "/pm") || StartsWithWord(lower, "/лс"))
		{
			ch = INT_GC_Const.CH_PM;
			if (!ParsePm(text, pmTo, body))
			{
				AddNotice(Translate("#STR_INT_GC_PM_USAGE"), 1);
				return R_PM_USAGE;
			}
		}
		else if (StartsWithWord(lower, "/g") || StartsWithWord(lower, "/г"))
		{
			ch = INT_GC_Const.CH_GLOBAL;
			body = AfterWord(text);
		}
		else if (StartsWithWord(lower, "/l") || StartsWithWord(lower, "/л") || StartsWithWord(lower, "/р"))
		{
			ch = INT_GC_Const.CH_LOCAL;
			body = AfterWord(text);
		}
		body = INT_GC_Util.CleanText(body, s_MaxLen);
		if (body == "")
			return R_EMPTY;
		if (s_Muted)
		{
			AddNotice(Translate("#STR_INT_GC_NOTICE_IS_MUTED"), 2);
			return R_MUTED;
		}
		int wait = FloodWait(body);
		if (wait > 0)
		{
			AddNotice(Fmt1("#STR_INT_GC_FLOOD", wait.ToString()), 1);
			return R_FLOOD;
		}
		if ((ch == INT_GC_Const.CH_GLOBAL || ch == INT_GC_Const.CH_PM) && !s_Welcome)
		{
			AddNotice(Translate("#STR_INT_GC_NO_GLOBAL"), 1);
			return R_NOGLOBAL;
		}
		string matched;
		if (s_Filter.Check(body, matched))
		{
			MarkSent(body);
			INT_GC_Log.Dbg("фильтр: найдено '" + matched + "'");
			if (s_Welcome)
			{
				ScriptRPC v = INT_GC_Proto.Begin(INT_GC_Const.OP_VIOL);
				v.Write(ch);
				v.Write(body);
				v.Send(null, INT_GC_Const.RPC_C2S, true, null);
			}
			else
			{
				AddNotice(Translate("#STR_INT_GC_BLOCKED_LOCAL"), 2);
			}
			return R_BANNED;
		}
		MarkSent(body);
		int color = INT_GC_UI.Settings().ColorIdx;
		if (ch == INT_GC_Const.CH_LOCAL)
		{
			GetGame().ChatPlayer(body);
			return R_SENT;
		}
		if (ch == INT_GC_Const.CH_PM)
		{
			ScriptRPC pm = INT_GC_Proto.Begin(INT_GC_Const.OP_PM);
			pm.Write(pmTo);
			pm.Write(color);
			pm.Write(body);
			pm.Send(null, INT_GC_Const.RPC_C2S, true, null);
			return R_SENT;
		}
		ScriptRPC msg = INT_GC_Proto.Begin(INT_GC_Const.OP_MSG);
		msg.Write(color);
		msg.Write(body);
		msg.Send(null, INT_GC_Const.RPC_C2S, true, null);
		return R_SENT;
	}

	protected static bool StartsWithWord(string lower, string word)
	{
		if (lower == word)
			return true;
		return lower.IndexOf(word + " ") == 0;
	}

	protected static string AfterWord(string text)
	{
		int sp = text.IndexOf(" ");
		if (sp < 0)
			return "";
		string r = text.Substring(sp + 1, text.Length() - sp - 1);
		r.TrimInPlace();
		return r;
	}

	// "/pm Nick text", "/pm "Nick with spaces" text"; a nick with spaces is also found among nicks seen this session.
	static bool ParsePm(string text, out string to, out string body)
	{
		to = "";
		body = "";
		string rest = AfterWord(text);
		if (rest == "")
			return false;
		if (rest.IndexOf("\"") == 0)
		{
			int close = rest.IndexOfFrom(1, "\"");
			if (close < 2)
				return false;
			to = rest.Substring(1, close - 1);
			body = rest.Substring(close + 1, rest.Length() - close - 1);
			body.TrimInPlace();
			return to != "" && body != "";
		}
		string lrest = INT_GC_Util.Lower(rest);
		string best = "";
		if (s_NickCase)
		{
			foreach (string ln, string nick : s_NickCase)
			{
				if (ln.Length() <= best.Length())
					continue;
				if (lrest.IndexOf(ln + " ") == 0)
					best = ln;
			}
		}
		if (best != "")
		{
			to = KnownNick(best);
			body = rest.Substring(best.Length(), rest.Length() - best.Length());
			body.TrimInPlace();
			return body != "";
		}
		int sp = rest.IndexOf(" ");
		if (sp <= 0)
			return false;
		to = rest.Substring(0, sp);
		body = rest.Substring(sp + 1, rest.Length() - sp - 1);
		body.TrimInPlace();
		return body != "";
	}

	// --- texts --------------------------------------------------------------------------------------------------
	static string Translate(string key)
	{
		return Widget.TranslateString(key);
	}

	static string Fmt1(string key, string a)
	{
		string t = Widget.TranslateString(key);
		t.Replace("%1", a);
		return t;
	}

	static string Fmt2(string key, string a, string b)
	{
		string t = Widget.TranslateString(key);
		t.Replace("%1", a);
		t.Replace("%2", b);
		return t;
	}

	protected static string FmtKey(string key, array<string> args)
	{
		string k = key + "";
		if (k.IndexOf("#") != 0)
			k = "#" + k;
		string t = Widget.TranslateString(k);
		for (int i = 0; i < args.Count() && i < 3; i++)
		{
			// at most 400 B per argument: three of them keep the line below the 2048 B Replace limit
			string raw = args.Get(i);
			string cleaned = INT_GC_Util.CleanText(raw, 200);
			string a = INT_GC_Util.FitBytes(cleaned, 400);
			int n = i + 1;
			t.Replace("%" + n.ToString(), a);
		}
		return t;
	}

	// --- self-test hooks ----------------------------------------------------------------------------------------
	static void TestSetState(bool welcome, bool muted, int minInterval, int dupInterval)
	{
		Ensure();
		s_Welcome = welcome;
		s_Muted = muted;
		s_MinInterval = minInterval;
		s_DupInterval = dupInterval;
		s_LastSendAt = -1000;
		s_LastText = "";
		s_LastTextAt = -1000;
	}
}
