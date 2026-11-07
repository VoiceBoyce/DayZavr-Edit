// INT_PDA (client) — data cache and network of the client (spec.md §7). The GUI reads only from here and asks for
// changes through the Send* methods; every answer of the server updates the cache and raises s_OnChanged(what).
// Owner rule 1: everything that can live on the client lives here — the «listening» state of the chat (PDA on +
// battery + chat module) is decided here from engine events, the saved password, the private history, the
// «Рядом» list, unread counters and the trophy check of a looted PDA.

class INT_PDA_ClientPrefs
{
	static const int VERSION = 2;
	int Version = 2;							// = VERSION
	int HudFeed = -1;							// PDA feed on the HUD: 1 on, 0 off, -1 not chosen = the server's HudFeedDefault
	int Sounds = 1;								// PDA sounds: 1 on, 0 off
	ref array<ref INT_PDA_SavedLogin> Logins;	// one per server address

	static INT_PDA_ClientPrefs ForRead()
	{
		INT_PDA_ClientPrefs p = new INT_PDA_ClientPrefs();
		p.Logins = new array<ref INT_PDA_SavedLogin>();
		return p;
	}
}

class INT_PDA_SavedLogin
{
	string Server;
	string Callsign;
	string Password;
}

class INT_PDA_SosMark
{
	string Callsign;
	float X;
	float Z;
	int TimeMs;
}

class INT_PDA_Client
{
	// what changed (s_OnChanged argument)
	static const int CH_WELCOME = 1;
	static const int CH_AUTH = 2;
	static const int CH_PROFILE = 3;
	static const int CH_CHAT = 4;
	static const int CH_FACTION = 5;
	static const int CH_NEWS = 6;
	static const int CH_MATES = 7;
	static const int CH_SOS = 8;
	static const int CH_ADMIN = 9;
	static const int CH_OWNER = 10;
	static const int CH_INVITE = 11;
	static const int CH_PDA = 12;
	static const int CH_CHAR = 13;

	static const int PUBLIC_KEEP = 150;
	static const int PRIVATE_KEEP = 60;
	static const string PREFS_DIR = "$profile:INT_Mods";
	static const string PREFS_ROOT = "$profile:INT_Mods/INT_PDA";
	static const string PREFS_FILE = "$profile:INT_Mods/INT_PDA/INT_PDA_Client.json";

	static ref ScriptInvoker s_OnChanged = new ScriptInvoker();		// (int what)
	static ref ScriptInvoker s_OnResult = new ScriptInvoker();		// (int requestOp, int code)
	static ref ScriptInvoker s_OnLine = new ScriptInvoker();		// (INT_PDA_ChatLine line) — HUD feed, sounds
	static ref ScriptInvoker s_OnNotice = new ScriptInvoker();		// (int kind, string a, string b) — HUD toasts

	static ref INT_PDA_Settings s_Settings = new INT_PDA_Settings();
	static bool s_Welcomed;
	static bool s_IsAdmin;
	static bool s_Registered;
	static string s_KnownCallsign;
	static bool s_LoggedIn;
	static ref INT_PDA_Profile s_Profile;
	static int s_ProfileAtMs;					// when s_Profile arrived (LifeSec runs on from there)
	static ref INT_PDA_FactionView s_Faction;
	static ref array<ref INT_PDA_ChatLine> s_Public = new array<ref INT_PDA_ChatLine>();
	static ref array<ref INT_PDA_ChatLine> s_FactionChat = new array<ref INT_PDA_ChatLine>();
	static ref map<string, ref array<ref INT_PDA_ChatLine>> s_Private = new map<string, ref array<ref INT_PDA_ChatLine>>();
	static ref array<ref INT_PDA_News> s_News = new array<ref INT_PDA_News>();
	static ref array<ref INT_PDA_MatePos> s_Mates = new array<ref INT_PDA_MatePos>();
	static ref array<ref INT_PDA_SosMark> s_Sos = new array<ref INT_PDA_SosMark>();
	static ref map<string, int> s_Unread = new map<string, int>();	// "public", "faction", "news", "pm:<callsign>"
	static ref map<string, string> s_OwnerNames = new map<string, string>();	// "low:high" -> owner callsign
	static ref INT_PDA_ClientPrefs s_Prefs;
	// invite waiting for an answer
	static string s_InviteFaction;
	static string s_InviteName;
	static int s_InviteEmblem;
	static string s_InviteFrom;
	// admin tab data (filled by OP_ADMIN_DATA)
	static ref array<ref INT_PDA_AdminRow> s_AdmRows = new array<ref INT_PDA_AdminRow>();
	static int s_AdmOffset;
	static int s_AdmTotal;
	static ref array<ref INT_PDA_FactionSummary> s_AdmFactions = new array<ref INT_PDA_FactionSummary>();
	static ref INT_PDA_FactionView s_AdmFaction;
	// «Персонаж» tab (OP_CHAR_DATA) and the rebirth it asked for
	static ref INT_PDA_CharData s_Char;
	static int s_CharAtMs;					// when s_Char arrived (WaitSec runs down from there)
	static bool s_RebornPending;				// the server killed the character for «Переродиться»: respawn once he is dead
	static int s_RebornAtMs;
	static int s_AdmLastSub;
	static int s_AdmLastCode;
	static string s_AdmLastId;

	protected static bool s_Hooked;
	protected static bool s_Listening;
	protected static int s_HelloMs = -100000000;
	protected static bool s_AutoLoginTried;
	static int s_KnownAvatar = -1;				// avatar of this SteamID's account (WELCOME), -1 = unknown
	// The saved login changes only when the server says OK (a typo must not overwrite a working password).
	protected static bool s_PendAuth;			// a login / registration waits for OP_AUTH_RESULT
	protected static bool s_PendAuto;			// ... sent by the silent auto-login
	protected static string s_PendCallsign;
	protected static string s_PendPass;
	protected static bool s_PendKeep;
	protected static string s_PendNewPass;		// a password change waits for its result
	protected static string s_PendNewCallsign;	// a callsign change waits for its result
	protected static bool s_AdminMode;			// opened with Alt+L
	protected static bool s_HadCharacter;		// OnNewCharacter saw the first character of this connection
	protected static Man s_Character;			// that character (weak)

	// --- life cycle ---------------------------------------------------------------------------------------------------
	static void Init()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (!s_Hooked)
		{
			DayZGame.Event_OnRPC.Insert(INT_PDA_Client.OnRPC);
			INT_PDA_Events.s_PdaChanged.Insert(INT_PDA_Client.OnPdaChanged);
			s_Hooked = true;
		}
		LoadPrefs();
		Reset();
	}

	static void Shutdown()
	{
		if (s_Hooked)
		{
			if (DayZGame.Event_OnRPC)
				DayZGame.Event_OnRPC.Remove(INT_PDA_Client.OnRPC);
			if (INT_PDA_Events.s_PdaChanged)
				INT_PDA_Events.s_PdaChanged.Remove(INT_PDA_Client.OnPdaChanged);
		}
		s_Hooked = false;
		Reset();
	}

	static void Reset()
	{
		s_Welcomed = false;
		s_IsAdmin = false;
		s_Registered = false;
		s_LoggedIn = false;
		s_Listening = false;
		s_AutoLoginTried = false;
		s_KnownAvatar = -1;
		s_PendAuth = false;
		s_PendNewPass = "";
		s_PendNewCallsign = "";
		s_Profile = null;
		s_Faction = null;
		s_Public.Clear();
		s_FactionChat.Clear();
		s_Private.Clear();
		s_News.Clear();
		s_Mates.Clear();
		s_Sos.Clear();
		s_Unread.Clear();
		s_OwnerNames.Clear();
		s_InviteFaction = "";
		s_HelloMs = -100000000;
		s_HadCharacter = false;
		s_Character = null;
		s_Char = null;
		s_RebornPending = false;
	}

	// Called every frame by the mission: the radio of a dead character, HELLO until the server answers (no server
	// timer is involved).
	static void Tick()
	{
		WatchDeath();
		if (s_Welcomed || !GetGame() || !GetGame().GetPlayer())
			return;
		int now = GetGame().GetTime();
		if (now - s_HelloMs < INT_PDA_Const.HELLO_RETRY_MS)
			return;
		s_HelloMs = now;
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_HELLO));
	}

	static bool VanillaChatOff()
	{
		return s_Settings.DisableVanillaChat;
	}

	static bool IsAdminMode()
	{
		return s_AdminMode && s_IsAdmin;
	}

	static void SetAdminMode(bool on)
	{
		s_AdminMode = on;
	}

	// The PDA can be used at all: a living, conscious, unrestrained local character (also the admin PDA).
	static bool PlayerCanUse()
	{
		if (!GetGame())
			return false;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive() || p.IsUnconscious() || p.IsRestrained())
			return false;
		return true;
	}

	// --- PDA of the local player -------------------------------------------------------------------------------------
	static string MySteam()
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.GetIdentity())
			return "";
		return p.GetIdentity().GetPlainId();
	}

	// The PDA to use: an own PDA first (in hands, then anywhere in the inventory; a working one before a switched-off
	// one), a looted one (trophy) only when there is no own PDA at all — the one in hands first among those.
	static INT_PDA FindPda(bool workingOnly)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return null;
		string me = MySteam();
		INT_PDA hands = INT_PDA.Cast(p.GetHumanInventory().GetEntityInHands());
		if (hands && workingOnly && !hands.IsWorkingPda())
			hands = null;
		if (hands && hands.IsOwnedBy(me) && hands.IsWorkingPda())
			return hands;
		array<EntityAI> items = new array<EntityAI>();
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		INT_PDA ownOff;			// own, switched off (only when !workingOnly)
		INT_PDA trophy;
		foreach (EntityAI e : items)
		{
			INT_PDA pda = INT_PDA.Cast(e);
			if (!pda || pda == hands)
				continue;
			if (workingOnly && !pda.IsWorkingPda())
				continue;
			if (pda.IsOwnedBy(me))
			{
				if (pda.IsWorkingPda())
					return pda;
				if (!ownOff)
					ownOff = pda;
			}
			else if (!trophy)
			{
				trophy = pda;
			}
		}
		if (hands && hands.IsOwnedBy(me))
			return hands;
		if (ownOff)
			return ownOff;
		if (hands)
			return hands;
		return trophy;
	}

	static bool IsTrophy(INT_PDA pda)
	{
		return pda && !pda.IsOwnedBy(MySteam());
	}

	static void WritePda(ScriptRPC rpc, INT_PDA pda)
	{
		int low = 0;
		int high = 0;
		if (pda)
			pda.GetNetworkID(low, high);
		rpc.Write(low);
		rpc.Write(high);
	}

	// Engine event of a PDA (on/off, battery, module, moved): recompute whether this client should get chat lines.
	static void OnPdaChanged(EntityAI pda)
	{
		RefreshListening();
		s_OnChanged.Invoke(CH_PDA);
	}

	static void RefreshListening()
	{
		if (!s_Welcomed)
			return;
		bool want = false;
		INT_PDA pda = FindPda(true);
		// a dead character does not listen (his body keeps the PDA working)
		Man me = GetGame().GetPlayer();
		bool alive = false;
		if (me && me.IsAlive())
			alive = true;
		if (!alive)
			pda = null;
		if (s_LoggedIn && alive)
		{
			if (s_AdminMode && s_IsAdmin)
				want = true;
			else if (pda && pda.HasChat() && !IsTrophy(pda))
				want = true;
		}
		else if (pda && !IsTrophy(pda))
		{
			TryAutoLogin(pda);
		}
		if (want == s_Listening)
			return;
		s_Listening = want;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_LISTEN);
		rpc.Write(want);
		if (want)
			WritePda(rpc, pda);
		Send(rpc);
	}

	static bool IsListening()
	{
		return s_Listening;
	}

	// Tells the server to stop sending chat to this client. The server forgets a listener only when the listened PDA
	// stops; a dead body's PDA keeps working, so death and respawn must say it themselves.
	protected static void StopListening()
	{
		bool was = s_Listening;
		s_Listening = false;
		if (!was || !s_Welcomed)
			return;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_LISTEN);
		rpc.Write(false);
		Send(rpc);
	}

	// Every frame (cheap): the local character died while listening -> the radio goes off.
	protected static void WatchDeath()
	{
		if (!s_Listening || !GetGame())
			return;
		Man me = GetGame().GetPlayer();
		if (me && !me.IsAlive())
			StopListening();
	}

	// MissionGameplay.OnPlayerRespawned: a new character of this client. The first one of the connection only gets
	// remembered; after a respawn the radio is switched off on the server and checked again for the new character's
	// own PDA (a working one sends OP_LISTEN again), and «Пробыл в Зоне» starts from zero (the server counts the
	// play time of the character, STAT_PLAYTIME).
	static void OnNewCharacter(Man player)
	{
		if (!player || player == s_Character)
			return;
		bool respawn = s_HadCharacter;
		s_HadCharacter = true;
		s_Character = player;
		if (!respawn)
			return;
		StopListening();
		if (s_Profile)
		{
			s_Profile.LifeSec = 0;
			s_ProfileAtMs = GetGame().GetTime();
			s_OnChanged.Invoke(CH_PROFILE);
		}
		RefreshListening();
	}

	// --- saved password -------------------------------------------------------------------------------------------
	static string ServerKey()
	{
		string address;
		int port;
		if (GetGame().GetHostAddress(address, port))
			return address + ":" + port.ToString();
		return "local";
	}

	static INT_PDA_SavedLogin SavedLogin()
	{
		if (!s_Prefs)
			return null;
		string key = ServerKey();
		foreach (INT_PDA_SavedLogin l : s_Prefs.Logins)
		{
			if (l.Server == key)
				return l;
		}
		return null;
	}

	static void RememberLogin(string callsign, string password, bool keep)
	{
		if (!s_Prefs)
			s_Prefs = INT_PDA_ClientPrefs.ForRead();
		string key = ServerKey();
		for (int i = s_Prefs.Logins.Count() - 1; i >= 0; i--)
		{
			if (s_Prefs.Logins[i].Server == key)
				s_Prefs.Logins.Remove(i);
		}
		if (keep)
		{
			INT_PDA_SavedLogin l = new INT_PDA_SavedLogin();
			l.Server = key;
			l.Callsign = callsign;
			l.Password = password;
			s_Prefs.Logins.Insert(l);
		}
		SavePrefs();
	}

	static void LoadPrefs()
	{
		s_Prefs = INT_PDA_ClientPrefs.ForRead();
		if (!FileExist(PREFS_FILE))
			return;
		INT_PDA_ClientPrefs p = INT_PDA_ClientPrefs.ForRead();
		string err;
		string txt;
		FileHandle fh = OpenFile(PREFS_FILE, FileMode.READ);
		if (fh == 0)
			return;
		ReadFile(fh, txt, 200000);
		CloseFile(fh);
		JsonSerializer js = new JsonSerializer();
		if (js.ReadFromString(p, txt, err))
		{
			if (!p.Logins)
				p.Logins = new array<ref INT_PDA_SavedLogin>();
			// version 1 always wrote HudFeed 1 (there was no switch yet): «not chosen», the server default decides
			if (p.Version < 2)
			{
				p.HudFeed = -1;
				p.Version = INT_PDA_ClientPrefs.VERSION;
			}
			s_Prefs = p;
		}
	}

	static void SavePrefs()
	{
		if (!s_Prefs)
			return;
		if (!FileExist(PREFS_DIR))
			MakeDirectory(PREFS_DIR);
		if (!FileExist(PREFS_ROOT))
			MakeDirectory(PREFS_ROOT);
		JsonSerializer js = new JsonSerializer();
		string txt;
		if (!js.WriteToString(s_Prefs, true, txt))
			return;
		FileHandle fh = OpenFile(PREFS_FILE, FileMode.WRITE);
		if (fh == 0)
			return;
		FPrint(fh, txt);
		CloseFile(fh);
	}

	// The PDA feed on the HUD: the player's choice (profile page), else the server's default for new players.
	static bool HudFeedOn()
	{
		if (!s_Prefs || s_Prefs.HudFeed < 0)
			return s_Settings.HudFeedDefault;
		return s_Prefs.HudFeed != 0;
	}

	static void SetHudFeed(bool on)
	{
		if (!s_Prefs)
			s_Prefs = INT_PDA_ClientPrefs.ForRead();
		s_Prefs.HudFeed = 0;
		if (on)
			s_Prefs.HudFeed = 1;
		SavePrefs();
	}

	// PDA sounds (INT_PDA_Sfx reads the same field).
	static bool SoundsOn()
	{
		return !s_Prefs || s_Prefs.Sounds != 0;
	}

	static void SetSounds(bool on)
	{
		if (!s_Prefs)
			s_Prefs = INT_PDA_ClientPrefs.ForRead();
		s_Prefs.Sounds = 0;
		if (on)
			s_Prefs.Sounds = 1;
		SavePrefs();
	}

	// Silent login with the saved password when a working own PDA appears (once per connection).
	protected static void TryAutoLogin(INT_PDA pda)
	{
		if (s_AutoLoginTried || !s_Registered)
			return;
		INT_PDA_SavedLogin l = SavedLogin();
		if (!l || l.Password == "")
			return;
		s_AutoLoginTried = true;
		StageLogin(l.Callsign, l.Password, true);
		s_PendAuto = true;
		SendLogin(l.Callsign, l.Password, pda);
	}

	// What to save when the login / registration just sent succeeds (the auth page calls it with «Сохранить пароль»).
	static void StageLogin(string callsign, string password, bool keep)
	{
		s_PendAuth = true;
		s_PendAuto = false;
		s_PendCallsign = callsign;
		s_PendPass = password;
		s_PendKeep = keep;
	}

	protected static void CommitLogin(int code)
	{
		if (!s_PendAuth)
			return;
		s_PendAuth = false;
		if (code == INT_PDA_Const.R_OK)
		{
			if (!s_PendAuto)
				RememberLogin(s_PendCallsign, s_PendPass, s_PendKeep);
			return;
		}
		// a saved password the server refuses is dropped, so it is not retried (and not counted) on every connect
		if (s_PendAuto && code == INT_PDA_Const.R_WRONG_LOGIN)
			RememberLogin(s_PendCallsign, "", false);
	}

	protected static void CommitChange(int reqOp, int code)
	{
		if (reqOp == INT_PDA_Const.OP_SET_PASSWORD)
		{
			string newPass = s_PendNewPass;
			s_PendNewPass = "";
			INT_PDA_SavedLogin saved = SavedLogin();
			if (code == INT_PDA_Const.R_OK && saved && newPass != "")
				RememberLogin(saved.Callsign, newPass, true);
		}
		else if (reqOp == INT_PDA_Const.OP_SET_CALLSIGN)
		{
			string newCallsign = s_PendNewCallsign;
			s_PendNewCallsign = "";
			INT_PDA_SavedLogin had = SavedLogin();
			if (code == INT_PDA_Const.R_OK && had && newCallsign != "")
				RememberLogin(newCallsign, had.Password, true);
		}
	}

	// --- requests -------------------------------------------------------------------------------------------------
	static void Send(ScriptRPC rpc)
	{
		if (!rpc)
			return;
		rpc.Send(null, INT_PDA_Const.RPC_C2S, true, null);
	}

	static void SendRegister(string callsign, string password, string discord, int avatar, INT_PDA pda)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_REGISTER);
		INT_PDA_Proto.WriteStr(rpc, callsign);
		INT_PDA_Proto.WriteStr(rpc, password);
		INT_PDA_Proto.WriteStr(rpc, discord);
		rpc.Write(avatar);
		WritePda(rpc, pda);
		Send(rpc);
	}

	static void SendLogin(string callsign, string password, INT_PDA pda)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_LOGIN);
		INT_PDA_Proto.WriteStr(rpc, callsign);
		INT_PDA_Proto.WriteStr(rpc, password);
		WritePda(rpc, pda);
		Send(rpc);
	}

	static void SendLogout()
	{
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_LOGOUT));
		s_LoggedIn = false;
		s_Listening = false;
		s_Profile = null;
		s_Faction = null;
		s_OnChanged.Invoke(CH_AUTH);
	}

	static void SendSetCallsign(string callsign)
	{
		s_PendNewCallsign = callsign;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SET_CALLSIGN);
		INT_PDA_Proto.WriteStr(rpc, callsign);
		Send(rpc);
	}

	static void SendSetPassword(string oldPw, string newPw)
	{
		s_PendNewPass = newPw;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SET_PASSWORD);
		INT_PDA_Proto.WriteStr(rpc, oldPw);
		INT_PDA_Proto.WriteStr(rpc, newPw);
		Send(rpc);
	}

	static void SendSetAvatar(int avatar)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SET_AVATAR);
		rpc.Write(avatar);
		Send(rpc);
	}

	// replyId: server id of the line this one answers (0 = none); the server builds the quote from its own history
	static void SendChat(int channel, int flags, string text, int replyId = 0)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAT_SEND);
		rpc.Write(channel);
		rpc.Write(flags);
		INT_PDA_Proto.WriteStr(rpc, text);
		WritePda(rpc, FindPda(true));
		rpc.Write(replyId);
		Send(rpc);
	}

	static void SendPm(string to, string text)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PM_SEND);
		INT_PDA_Proto.WriteStr(rpc, to);
		INT_PDA_Proto.WriteStr(rpc, text);
		WritePda(rpc, FindPda(true));
		Send(rpc);
	}

	static void RequestHistory(int channel)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAT_HISTORY);
		rpc.Write(channel);
		Send(rpc);
	}

	static void RequestNews()
	{
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_NEWS));
	}

	static void RequestOwner(INT_PDA pda)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PDA_OWNER);
		WritePda(rpc, pda);
		Send(rpc);
	}

	static void RequestChar()
	{
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAR));
	}

	static void SendCharSave(string type)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAR_SAVE);
		INT_PDA_Proto.WriteStr(rpc, type);
		Send(rpc);
	}

	static void SendCharRespawn(string type)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAR_RESPAWN);
		INT_PDA_Proto.WriteStr(rpc, type);
		Send(rpc);
	}

	// Seconds until «Переродиться» is allowed again (the server's count, run down on this side).
	static int CharWaitSec()
	{
		if (!s_Char || s_Char.WaitSec <= 0)
			return 0;
		int left = s_Char.WaitSec - (GetGame().GetTime() - s_CharAtMs) / 1000;
		if (left < 0)
			return 0;
		return left;
	}

	static void RequestFaction()
	{
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_FACTION));
	}

	static void SendInvite(string steam)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_INVITE);
		INT_PDA_Proto.WriteStr(rpc, steam);
		Send(rpc);
	}

	static void SendInviteAnswer(bool accept)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_INVITE_ANSWER);
		INT_PDA_Proto.WriteStr(rpc, s_InviteFaction);
		rpc.Write(accept);
		Send(rpc);
		s_InviteFaction = "";
		s_OnChanged.Invoke(CH_INVITE);
	}

	static void SendKick(string steam)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_KICK);
		INT_PDA_Proto.WriteStr(rpc, steam);
		Send(rpc);
	}

	static void SendSetRank(string steam, string rank)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SET_RANK);
		INT_PDA_Proto.WriteStr(rpc, steam);
		INT_PDA_Proto.WriteStr(rpc, rank);
		Send(rpc);
	}

	static void SendLeave()
	{
		Send(INT_PDA_Proto.Begin(INT_PDA_Const.OP_LEAVE));
	}

	static void RequestMates()
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_MATES);
		WritePda(rpc, FindPda(true));
		Send(rpc);
	}

	static void SendSos()
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SOS);
		WritePda(rpc, FindPda(true));
		Send(rpc);
	}

	static void SendMarkAdd(int kind, string name, float x, float z)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_MARK_ADD);
		rpc.Write(kind);
		INT_PDA_Proto.WriteStr(rpc, name);
		rpc.Write(x);
		rpc.Write(z);
		Send(rpc);
	}

	static void SendMarkDel(int id)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_MARK_DEL);
		rpc.Write(id);
		Send(rpc);
	}

	// Admin request: the caller writes the payload after Admin(sub) and sends it with Send().
	static ScriptRPC Admin(int sub)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_ADMIN);
		rpc.Write(sub);
		return rpc;
	}

	// --- unread ---------------------------------------------------------------------------------------------------
	static int Unread(string key)
	{
		return s_Unread.Get(key);
	}

	static int UnreadPrivate()
	{
		int n = 0;
		foreach (string k, int v : s_Unread)
		{
			if (k.IndexOf("pm:") == 0)
				n = n + v;
		}
		return n;
	}

	static void MarkRead(string key)
	{
		s_Unread.Remove(key);
		s_OnChanged.Invoke(CH_CHAT);
	}

	// The same without an event (for a caller that is already inside an s_OnChanged handler).
	static void ForgetUnread(string key)
	{
		s_Unread.Remove(key);
	}

	// The player deleted a private dialog (chat tab): its lines and its unread counter go; a new line from the same
	// callsign starts the dialog again.
	static void ForgetDialog(string key)
	{
		s_Private.Remove(key);
		s_Unread.Remove("pm:" + key);
		s_OnChanged.Invoke(CH_CHAT);
	}

	protected static void Bump(string key)
	{
		s_Unread.Set(key, s_Unread.Get(key) + 1);
	}

	// --- network in -----------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_PDA_Const.RPC_S2C)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		int op;
		if (!INT_PDA_Proto.ReadHeader(ctx, op))
		{
			INT_PDA_Log.Warn("пакет сервера не прочитан (другая версия мода?)");
			return;
		}
		switch (op)
		{
			case INT_PDA_Const.OP_WELCOME:
				OnWelcome(ctx);
				return;
			case INT_PDA_Const.OP_AUTH_RESULT:
				OnAuth(ctx);
				return;
			case INT_PDA_Const.OP_PROFILE:
				OnProfile(ctx);
				return;
			case INT_PDA_Const.OP_RESULT:
				OnResult(ctx);
				return;
			case INT_PDA_Const.OP_CHAT_LINE:
				OnLine(ctx);
				return;
			case INT_PDA_Const.OP_CHAT_HISTORY_DATA:
				OnHistory(ctx);
				return;
			case INT_PDA_Const.OP_PM_NOTICE:
				OnPmNotice(ctx);
				return;
			case INT_PDA_Const.OP_NEWS_DATA:
				OnNewsData(ctx);
				return;
			case INT_PDA_Const.OP_NEWS_FLASH:
				OnNewsFlash(ctx);
				return;
			case INT_PDA_Const.OP_PDA_OWNER_DATA:
				OnOwner(ctx);
				return;
			case INT_PDA_Const.OP_CHAR_DATA:
				OnCharData(ctx);
				return;
			case INT_PDA_Const.OP_FACTION_DATA:
				OnFaction(ctx);
				return;
			case INT_PDA_Const.OP_INVITE_IN:
				OnInvite(ctx);
				return;
			case INT_PDA_Const.OP_MATES_DATA:
				OnMates(ctx);
				return;
			case INT_PDA_Const.OP_SOS_IN:
				OnSos(ctx);
				return;
			case INT_PDA_Const.OP_ADMIN_DATA:
				OnAdminData(ctx);
				return;
		}
	}

	protected static void OnWelcome(ParamsReadContext ctx)
	{
		INT_PDA_Settings st = new INT_PDA_Settings();
		bool admin;
		bool registered;
		string callsign;
		if (!st.Read(ctx) || !ctx.Read(admin) || !ctx.Read(registered) || !ctx.Read(callsign))
			return;
		s_Settings = st;
		INT_PDA_Log.s_Debug = st.DebugLog;
		bool first = !s_Welcomed;
		s_Welcomed = true;
		s_IsAdmin = admin;
		s_Registered = registered;
		s_KnownCallsign = callsign;
		// the account's portrait for the login screen (a newer field: an older server simply does not send it)
		int avatar;
		if (ctx.Read(avatar))
			s_KnownAvatar = avatar;
		bool deleted = !registered && s_LoggedIn;
		if (deleted)
		{
			// the admin deleted the account: nothing of it stays on this client (private dialogs, faction channel,
			// unread counters, SOS marks, mates, a pending invitation)
			s_LoggedIn = false;
			s_Listening = false;
			s_Profile = null;
			s_Faction = null;
			s_Private.Clear();
			s_FactionChat.Clear();
			s_Unread.Clear();
			s_Sos.Clear();
			s_Mates.Clear();
			s_InviteFaction = "";
		}
		INT_PDA_Log.Dbg("WELCOME: админ " + admin.ToString() + ", зарегистрирован " + registered.ToString());
		s_OnChanged.Invoke(CH_WELCOME);
		if (deleted)
		{
			s_OnChanged.Invoke(CH_CHAT);
			s_OnChanged.Invoke(CH_SOS);
			s_OnChanged.Invoke(CH_MATES);
			s_OnChanged.Invoke(CH_INVITE);
		}
		if (first)
			RefreshListening();
	}

	protected static void OnAuth(ParamsReadContext ctx)
	{
		int code;
		bool ok;
		if (!ctx.Read(code) || !ctx.Read(ok))
			return;
		if (ok)
		{
			INT_PDA_Profile p = new INT_PDA_Profile();
			if (!p.Read(ctx))
				return;
			s_Profile = p;
			s_ProfileAtMs = GetGame().GetTime();	// LifeSec runs on from here (as after OP_PROFILE)
			s_LoggedIn = true;
			s_Registered = true;
			s_KnownCallsign = p.Callsign;
			s_Listening = false;
			RefreshListening();
			RequestHistory(INT_PDA_Const.CH_PUBLIC);
			RequestNews();
			s_KnownAvatar = p.Avatar;
		}
		CommitLogin(code);
		s_OnResult.Invoke(INT_PDA_Const.OP_AUTH_RESULT, code);
		s_OnChanged.Invoke(CH_AUTH);
	}

	protected static void OnProfile(ParamsReadContext ctx)
	{
		INT_PDA_Profile p = new INT_PDA_Profile();
		if (!p.Read(ctx))
			return;
		bool factionChanged = !s_Profile || s_Profile.FactionId != p.FactionId;
		bool switched = false;
		if (s_Profile && s_Profile.FactionId != "" && s_Profile.FactionId != p.FactionId)
			switched = true;
		s_Profile = p;
		s_ProfileAtMs = GetGame().GetTime();
		s_KnownCallsign = p.Callsign;
		s_KnownAvatar = p.Avatar;
		// another faction (or none): the old one's SOS marks and mates' positions mean nothing any more
		bool marksGone = false;
		if (factionChanged && (s_Sos.Count() > 0 || s_Mates.Count() > 0))
		{
			s_Sos.Clear();
			s_Mates.Clear();
			marksGone = true;
		}
		// moved straight from one faction to another: drop the old faction's view and channel (OnFaction then takes
		// the new one as the first and loads its history)
		if (switched)
		{
			s_Faction = null;
			s_FactionChat.Clear();
			s_Unread.Remove("faction");
		}
		// joined a faction: a pending invitation (from this or another faction) is answered by that
		bool inFaction = p.InFaction();
		bool joined = factionChanged && inFaction;
		bool inviteGone = false;
		if (joined && s_InviteFaction != "")
		{
			s_InviteFaction = "";
			inviteGone = true;
		}
		if (!inFaction)
		{
			s_Faction = null;
			s_FactionChat.Clear();
		}
		else if (factionChanged)
		{
			RequestFaction();
		}
		s_OnChanged.Invoke(CH_PROFILE);
		if (marksGone)
		{
			s_OnChanged.Invoke(CH_SOS);
			s_OnChanged.Invoke(CH_MATES);
		}
		if (inviteGone)
			s_OnChanged.Invoke(CH_INVITE);
	}

	protected static void OnResult(ParamsReadContext ctx)
	{
		int reqOp;
		int code;
		if (!ctx.Read(reqOp) || !ctx.Read(code))
			return;
		CommitChange(reqOp, code);
		// «Переродиться» accepted: the server killed the character; the mission asks for the respawn once he is dead
		if (reqOp == INT_PDA_Const.OP_CHAR_RESPAWN && code == INT_PDA_Const.R_OK)
		{
			s_RebornPending = true;
			s_RebornAtMs = GetGame().GetTime();
		}
		// a login / registration refused before the server looked at it (R_TOO_FAST): nothing to save, and a silent
		// auto-login may try again later
		if ((reqOp == INT_PDA_Const.OP_LOGIN || reqOp == INT_PDA_Const.OP_REGISTER) && code != INT_PDA_Const.R_OK)
		{
			if (s_PendAuth && s_PendAuto)
				s_AutoLoginTried = false;
			s_PendAuth = false;
		}
		s_OnResult.Invoke(reqOp, code);
	}

	protected static void OnLine(ParamsReadContext ctx)
	{
		INT_PDA_ChatLine line = new INT_PDA_ChatLine();
		if (!line.Read(ctx))
			return;
		AddLine(line, true);
	}

	// Server system lines come as @KEY|arg1|arg2 (language-neutral): KEY -> #STR_INT_PDA_SYS_<KEY> with %1, %2.
	static string SystemText(string text)
	{
		if (text.IndexOf("@") != 0)
			return text;
		array<string> parts = new array<string>();
		string body = text.Substring(1, text.Length() - 1);
		body.Split("|", parts);
		if (parts.Count() == 0)
			return text;
		string fmt = Widget.TranslateString("#STR_INT_PDA_SYS_" + parts[0]);
		string a = "";
		string b = "";
		if (parts.Count() > 1)
			a = parts[1];
		if (parts.Count() > 2)
			b = parts[2];
		fmt.Replace("%1", a);
		fmt.Replace("%2", b);
		return fmt;
	}

	protected static void AddLine(INT_PDA_ChatLine line, bool live)
	{
		string me = s_KnownCallsign;
		if (line.Channel == INT_PDA_Const.CH_SYSTEM)
			line.Text = SystemText(line.Text);
		if (line.Channel == INT_PDA_Const.CH_PRIVATE)
		{
			string other = line.Author;
			if (line.Author == me)
				other = line.To;
			string key = INT_PDA_Util.CallsignKey(other);
			array<ref INT_PDA_ChatLine> list;
			if (!s_Private.Find(key, list))
			{
				list = new array<ref INT_PDA_ChatLine>();
				s_Private.Set(key, list);
			}
			list.Insert(line);
			while (list.Count() > PRIVATE_KEEP)
				list.RemoveOrdered(0);
			if (live && line.Author != me)
				Bump("pm:" + key);
		}
		else if (line.Channel == INT_PDA_Const.CH_FACTION || (line.Channel == INT_PDA_Const.CH_SYSTEM && line.FactionId != ""))
		{
			s_FactionChat.Insert(line);
			while (s_FactionChat.Count() > PUBLIC_KEEP)
				s_FactionChat.RemoveOrdered(0);
			if (live)
				Bump("faction");
		}
		else
		{
			s_Public.Insert(line);
			while (s_Public.Count() > PUBLIC_KEEP)
				s_Public.RemoveOrdered(0);
			if (live)
				Bump("public");
		}
		if (!live)
			return;
		s_OnLine.Invoke(line);
		s_OnChanged.Invoke(CH_CHAT);
	}

	protected static void OnHistory(ParamsReadContext ctx)
	{
		int channel;
		bool first;
		int n;
		if (!ctx.Read(channel) || !ctx.Read(first) || !INT_PDA_Proto.ReadCount(ctx, n, 100))
			return;
		if (channel != INT_PDA_Const.CH_PUBLIC && channel != INT_PDA_Const.CH_FACTION)
			return;
		if (first)
		{
			if (channel == INT_PDA_Const.CH_FACTION)
				s_FactionChat.Clear();
			else
				s_Public.Clear();
		}
		for (int i = 0; i < n; i++)
		{
			INT_PDA_ChatLine line = new INT_PDA_ChatLine();
			if (!line.Read(ctx))
				return;
			AddLine(line, false);
		}
		s_OnChanged.Invoke(CH_CHAT);
	}

	protected static void OnPmNotice(ParamsReadContext ctx)
	{
		string from;
		int count;
		if (!ctx.Read(from) || !ctx.Read(count))
			return;
		s_OnNotice.Invoke(INT_PDA_Const.OP_PM_NOTICE, from, count.ToString());
	}

	protected static void OnNewsData(ParamsReadContext ctx)
	{
		int n;
		if (!INT_PDA_Proto.ReadCount(ctx, n, 100))
			return;
		s_News.Clear();
		for (int i = 0; i < n; i++)
		{
			INT_PDA_News item = new INT_PDA_News();
			if (!item.Read(ctx))
				return;
			s_News.Insert(item);
		}
		s_OnChanged.Invoke(CH_NEWS);
	}

	// Live news (an admin lifetime ends them), oldest first.
	static void LiveNews(array<INT_PDA_News> outList)
	{
		outList.Clear();
		foreach (INT_PDA_News n : s_News)
		{
			if (n && n.IsLive())
				outList.Insert(n);
		}
	}

	protected static void OnNewsFlash(ParamsReadContext ctx)
	{
		INT_PDA_News item = new INT_PDA_News();
		if (!item.Read(ctx))
			return;
		// an edited item comes again under its id: replace it
		for (int i = s_News.Count() - 1; i >= 0; i--)
		{
			if (s_News[i] && s_News[i].Id == item.Id)
				s_News.RemoveOrdered(i);
		}
		s_News.Insert(item);
		while (s_News.Count() > 50)
			s_News.RemoveOrdered(0);
		Bump("news");
		s_OnNotice.Invoke(INT_PDA_Const.OP_NEWS_FLASH, item.Title, item.Text);
		s_OnChanged.Invoke(CH_NEWS);
	}

	protected static void OnCharData(ParamsReadContext ctx)
	{
		INT_PDA_CharData d = new INT_PDA_CharData();
		if (!d.Read(ctx))
			return;
		s_Char = d;
		s_CharAtMs = GetGame().GetTime();
		s_OnChanged.Invoke(CH_CHAR);
	}

	protected static void OnOwner(ParamsReadContext ctx)
	{
		int low;
		int high;
		string callsign;
		if (!ctx.Read(low) || !ctx.Read(high) || !ctx.Read(callsign))
			return;
		s_OwnerNames.Set(low.ToString() + ":" + high.ToString(), callsign);
		s_OnChanged.Invoke(CH_OWNER);
	}

	static string OwnerName(INT_PDA pda)
	{
		if (!pda)
			return "";
		int low;
		int high;
		pda.GetNetworkID(low, high);
		return s_OwnerNames.Get(low.ToString() + ":" + high.ToString());
	}

	protected static void OnFaction(ParamsReadContext ctx)
	{
		bool has;
		if (!ctx.Read(has))
			return;
		if (!has)
		{
			s_Faction = null;
			s_OnChanged.Invoke(CH_FACTION);
			return;
		}
		INT_PDA_FactionView v = new INT_PDA_FactionView();
		if (!v.Read(ctx))
			return;
		bool first = !s_Faction;
		s_Faction = v;
		if (first)
			RequestHistory(INT_PDA_Const.CH_FACTION);
		s_OnChanged.Invoke(CH_FACTION);
	}

	protected static void OnInvite(ParamsReadContext ctx)
	{
		string id;
		string name;
		int emblem;
		string from;
		if (!ctx.Read(id) || !ctx.Read(name) || !ctx.Read(emblem) || !ctx.Read(from))
			return;
		s_InviteFaction = id;
		s_InviteName = name;
		s_InviteEmblem = emblem;
		s_InviteFrom = from;
		s_OnNotice.Invoke(INT_PDA_Const.OP_INVITE_IN, name, from);
		s_OnChanged.Invoke(CH_INVITE);
	}

	protected static void OnMates(ParamsReadContext ctx)
	{
		int n;
		if (!INT_PDA_Proto.ReadCount(ctx, n, 200))
			return;
		s_Mates.Clear();
		for (int i = 0; i < n; i++)
		{
			INT_PDA_MatePos m = new INT_PDA_MatePos();
			if (!m.Read(ctx))
				return;
			s_Mates.Insert(m);
		}
		s_OnChanged.Invoke(CH_MATES);
	}

	protected static void OnSos(ParamsReadContext ctx)
	{
		string from;
		float x;
		float z;
		if (!ctx.Read(from) || !ctx.Read(x) || !ctx.Read(z))
			return;
		INT_PDA_SosMark s = new INT_PDA_SosMark();
		s.Callsign = from;
		s.X = x;
		s.Z = z;
		s.TimeMs = GetGame().GetTime();
		for (int i = s_Sos.Count() - 1; i >= 0; i--)
		{
			if (s_Sos[i].Callsign == from)
				s_Sos.Remove(i);
		}
		s_Sos.Insert(s);
		s_OnNotice.Invoke(INT_PDA_Const.OP_SOS_IN, from, DistanceText(x, z));
		s_OnChanged.Invoke(CH_SOS);
	}

	// "340 м · СВ" from the local player to a point.
	static string DistanceText(float x, float z)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return "";
		vector me = p.GetPosition();
		float dx = x - me[0];
		float dz = z - me[2];
		int dist = (int)Math.Round(Math.Sqrt(dx * dx + dz * dz));
		float ang = Math.Atan2(dx, dz) * Math.RAD2DEG;
		if (ang < 0)
			ang = ang + 360;
		int idx = (int)Math.Round(ang / 45.0);
		if (idx >= 8)
			idx = 0;
		string unit = Widget.TranslateString("#STR_INT_PDA_UNIT_M");
		string dir = Widget.TranslateString("#STR_INT_PDA_DIR_" + idx.ToString());
		return dist.ToString() + " " + unit + " · " + dir;
	}

	protected static void OnAdminData(ParamsReadContext ctx)
	{
		int sub;
		int code;
		if (!ctx.Read(sub) || !ctx.Read(code))
			return;
		s_AdmLastSub = sub;
		s_AdmLastCode = code;
		s_AdmLastId = "";
		if (code == INT_PDA_Const.R_OK)
		{
			if (sub == INT_PDA_Const.ADM_PLAYERS)
				ReadAdminPlayers(ctx);
			else if (sub == INT_PDA_Const.ADM_FACTIONS)
				ReadAdminFactions(ctx);
			else if (sub == INT_PDA_Const.ADM_FACTION_SAVE)
				ctx.Read(s_AdmLastId);
		}
		s_OnResult.Invoke(INT_PDA_Const.OP_ADMIN, code);
		s_OnChanged.Invoke(CH_ADMIN);
	}

	protected static void ReadAdminPlayers(ParamsReadContext ctx)
	{
		int n;
		if (!ctx.Read(s_AdmOffset) || !ctx.Read(s_AdmTotal) || !INT_PDA_Proto.ReadCount(ctx, n, 500))
			return;
		s_AdmRows.Clear();
		for (int i = 0; i < n; i++)
		{
			INT_PDA_AdminRow r = new INT_PDA_AdminRow();
			if (!r.Read(ctx))
				return;
			s_AdmRows.Insert(r);
		}
	}

	protected static void ReadAdminFactions(ParamsReadContext ctx)
	{
		bool full;
		if (!ctx.Read(full))
			return;
		if (full)
		{
			INT_PDA_FactionView v = new INT_PDA_FactionView();
			if (v.Read(ctx))
				s_AdmFaction = v;
			return;
		}
		int n;
		if (!INT_PDA_Proto.ReadCount(ctx, n, 500))
			return;
		s_AdmFactions.Clear();
		for (int i = 0; i < n; i++)
		{
			INT_PDA_FactionSummary s = new INT_PDA_FactionSummary();
			if (!s.Read(ctx))
				return;
			s_AdmFactions.Insert(s);
		}
	}
}
