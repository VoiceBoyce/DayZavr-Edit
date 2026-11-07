// INT_PDA — constants shared by client and server (spec.md §7). Ids live in the free 924xxxx block (checked against
// every INT mod and every PBO under F:\Games\DayZavr on 2026-10-22).

class INT_PDA_Const
{
	static const string VERSION = "1.1.0";
	static const int PROTOCOL = 2;					// 2: chat lines carry an id and a reply quote (2026-10-23)

	static const int MENU_PDA = 9240201;
	static const int MENU_MOVER = 9240202;
	static const int RPC_C2S = 9240211;
	static const int RPC_S2C = 9240212;
	static const int RPC_BEEP = 9240213;		// on the PDA item: the engine delivers it to clients near the item

	// client -> server ------------------------------------------------------------------------------------------------
	static const int OP_HELLO = 1;				// client mission started: settings + own account state please
	static const int OP_REGISTER = 2;			// callsign, password, discord, avatar
	static const int OP_LOGIN = 3;				// callsign, password
	static const int OP_LOGOUT = 4;
	static const int OP_SET_CALLSIGN = 5;		// new callsign
	static const int OP_SET_PASSWORD = 6;		// old password, new password
	static const int OP_SET_AVATAR = 7;			// avatar index
	static const int OP_LISTEN = 8;				// bool: own PDA is on with battery + chat module (chat delivery on/off)
	static const int OP_CHAT_SEND = 10;			// channel, flags, text
	static const int OP_PM_SEND = 11;			// to callsign, text
	static const int OP_CHAT_HISTORY = 12;		// channel («Обновить чаты»)
	static const int OP_NEWS = 13;				// news list
	static const int OP_PDA_OWNER = 14;			// who owns this PDA (looted PDA trophy screen)
	static const int OP_CHAR = 15;				// «Персонаж» tab data please
	static const int OP_CHAR_SAVE = 16;			// survivor class: the character of every new spawn of this player
	static const int OP_CHAR_RESPAWN = 17;		// survivor class: saved, the current character dies, the client respawns
	static const int OP_FACTION = 20;			// own faction data
	static const int OP_INVITE = 21;			// steam id
	static const int OP_INVITE_ANSWER = 22;		// faction id, accept
	static const int OP_KICK = 23;				// steam id
	static const int OP_SET_RANK = 24;			// steam id, rank title
	static const int OP_LEAVE = 25;
	static const int OP_MATES = 26;				// online faction mates' positions (one shot, map open / refresh)
	static const int OP_SOS = 27;				// own position to online faction mates
	static const int OP_MARK_ADD = 28;			// faction map mark (leader / deputy)
	static const int OP_MARK_DEL = 29;
	static const int OP_ADMIN = 40;				// admin request: sub-op + payload (see ADM_*)

	// server -> client ------------------------------------------------------------------------------------------------
	static const int OP_WELCOME = 101;			// settings, admin flag, registered flag, callsign
	static const int OP_AUTH_RESULT = 102;		// result code + profile on success
	static const int OP_PROFILE = 103;			// own profile changed (callsign / avatar / faction / rank)
	static const int OP_RESULT = 104;			// generic result of a request: request op, code, text
	static const int OP_CHAT_LINE = 110;		// one chat line (public / faction / private / system)
	static const int OP_CHAT_HISTORY_DATA = 111;
	static const int OP_PM_NOTICE = 112;		// «Вам пришло сообщение от X» (toast + beep)
	static const int OP_NEWS_DATA = 113;
	static const int OP_NEWS_FLASH = 114;		// new admin news: banner
	static const int OP_PDA_OWNER_DATA = 115;
	static const int OP_CHAR_DATA = 116;		// INT_PDA_CharData: offered classes, saved class, respawn wait, spawn clothes
	static const int OP_FACTION_DATA = 120;
	static const int OP_INVITE_IN = 121;		// faction id, faction name, from callsign
	static const int OP_MATES_DATA = 122;
	static const int OP_SOS_IN = 123;
	static const int OP_ADMIN_DATA = 140;		// admin answer: sub-op + payload

	// admin sub-ops ---------------------------------------------------------------------------------------------------
	static const int ADM_PLAYERS = 1;			// page of accounts (offset, filter)
	static const int ADM_PLAYER = 2;			// one account by steam id or callsign
	static const int ADM_SET_PASSWORD = 3;
	static const int ADM_DELETE_ACCOUNT = 4;
	static const int ADM_FACTIONS = 10;			// all factions (summary)
	static const int ADM_FACTION_SAVE = 11;		// create / edit faction (name, emblem, motto, base, spawn point)
	static const int ADM_FACTION_DELETE = 12;
	static const int ADM_SET_ROLE = 13;			// leader / deputy / member
	static const int ADM_KICK = 14;
	static const int ADM_JOIN = 15;				// admin joins a faction
	static const int ADM_RANKS = 16;			// rank list of a faction
	static const int ADM_RELATION = 17;			// relation between two factions + reason
	static const int ADM_RANK_RENAME = 18;		// faction id, old rank, new rank: the holders keep it under the new name
	static const int ADM_RELOAD_DB = 20;		// re-read accounts (drop caches)
	static const int ADM_RELOAD_FACTIONS = 21;
	static const int ADM_RELOAD_CONFIG = 22;	// Ctrl+Alt+P
	static const int ADM_NEWS_POST = 23;
	static const int ADM_NEWS_DELETE = 24;
	static const int ADM_SETTINGS = 25;			// nearby invite radius, news line rotation, default news lifetime
	static const int ADM_NEWS_EDIT = 26;		// id, title, text, minutes: a published news item changed (lifetime from now)

	// chat channels
	static const int CH_PUBLIC = 0;
	static const int CH_FACTION = 1;
	static const int CH_PRIVATE = 2;
	static const int CH_SYSTEM = 3;

	// chat line flags
	static const int FL_ANON = 1;				// author hidden (admins still get the real author)
	static const int FL_AS_FACTION = 2;			// written in the name of the faction
	static const int FL_ADMIN_VIEW = 4;			// this copy carries the real author of an anonymous line

	// faction roles
	static const int ROLE_NONE = 0;
	static const int ROLE_MEMBER = 1;
	static const int ROLE_DEPUTY = 2;
	static const int ROLE_LEADER = 3;

	// relations
	static const int REL_NEUTRAL = 0;
	static const int REL_ALLY = 1;
	static const int REL_ENEMY = 2;

	// mark kinds (faction / personal map marks)
	static const int MARK_BASE = 0;
	static const int MARK_RALLY = 1;
	static const int MARK_DANGER = 2;
	static const int MARK_STASH = 3;
	static const int MARK_ANOMALY = 4;
	static const int MARK_NOTE = 5;

	// result codes (0 = ok, negative = error; texts via stringtable STR_INT_PDA_ERR_*)
	static const int R_OK = 0;
	static const int R_BAD_DATA = -1;
	static const int R_CALLSIGN_FORMAT = -2;	// not Latin letters / length
	static const int R_CALLSIGN_TAKEN = -3;
	static const int R_PASSWORD_FORMAT = -4;
	static const int R_DISCORD_FORMAT = -5;
	static const int R_ALREADY_REGISTERED = -6;
	static const int R_NOT_REGISTERED = -7;
	static const int R_WRONG_LOGIN = -8;		// callsign or password wrong
	static const int R_LOCKED = -9;				// too many wrong passwords, wait
	static const int R_NOT_LOGGED_IN = -10;
	static const int R_NO_RIGHTS = -11;
	static const int R_NOT_FOUND = -12;
	static const int R_NO_MODULE = -13;
	static const int R_WRITE_FAILED = -14;
	static const int R_TOO_FAST = -15;
	static const int R_OFFLINE = -16;
	static const int R_IN_FACTION = -17;		// target already in a faction
	static const int R_NOT_IN_FACTION = -18;
	static const int R_TEXT = -19;				// empty / too long text
	static const int R_FULL = -20;				// list limit reached
	static const int R_NAME_TAKEN = -21;		// another faction already has this name
	static const int R_NO_RECIPIENTS = -22;		// SOS: no faction mate online with his own working PDA + GP module
	static const int R_WAIT = -23;				// «Переродиться» again only after CharacterRespawnMinutes

	// limits and pacing
	static const int CALLSIGN_MIN = 3;
	static const int CALLSIGN_MAX = 20;
	static const int PASSWORD_MIN = 4;
	static const int PASSWORD_MAX = 32;
	static const int DISCORD_MAX = 40;
	static const int AVATARS = 73;				// int_pda_avatars a00..a72 (S.T.A.L.K.E.R. build NPC icons, owner 2026-10-23)
	static const int EMBLEMS = 45;				// faction emblems int_pda_emblemsq q01..q45 (owner's 32 + 13 Clear Sky icons,
												// unified squares; the GUI draws the vertical card with the faction name)
	static const int MAX_WIRE_BYTES = 1000;			// one RPC string (≥1024 B throws «String CORRUPTED», measured 1.30)
	static const int MAX_NAME_BYTES = 80;
	static const int HELLO_GAP_MS = 4000;			// server: at most one WELCOME per player per 4 s
	static const int HELLO_RETRY_MS = 8000;			// client: resend HELLO when no WELCOME came
	static const int LOGIN_FAILS = 5;				// wrong passwords before a lock
	static const int LOGIN_LOCK_MS = 60000;
	static const int REQUEST_GAP_MS = 250;			// server: generic requests per player
	static const int MATES_GAP_MS = 5000;			// server: faction mates' positions per player
	static const int SOS_GAP_MS = 30000;
}
