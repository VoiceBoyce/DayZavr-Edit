// INT Quests — constants shared by both sides (spec.md §6, §11). IDs 922xxxx are free in the INT mods (survey
// 2026-10-22: 911..921 are taken). The wire is versioned: a packet of another PROTO is dropped by the reader.

class INT_QST_Const
{
	static const string VERSION = "1.0.0";
	static const int PROTO = 2;		// 2 (2026-10-31): HELLO / GIVERS without the sign fields (SIGNS-OFF)

	static const int RPC_C2S = 9220211;
	static const int RPC_S2C = 9220212;

	static const int MENU_JOURNAL = 9220201;
	static const int MENU_GIVER = 9220202;
	static const int MENU_BOARD = 9220203;
	static const int MENU_MOVER = 9220204;		// the player moves the HUD tracker
	static const int MENU_GIVERS = 9220205;		// admin: quest givers
	static const int MENU_EXCHANGE = 9220206;	// admin: export / import

	// A string field on the wire stays far below the ~1000-byte limit measured on 1.30 (INT_GlobalChat).
	static const int PIECE_BYTES = 600;
	// Catalog pieces per packet; the client pulls the next packet only after the previous one arrived.
	static const int PIECES_PER_PACKET = 16;

	static const int MAX_TARGETS = 40;
	static const int MAX_REWARDS = 30;
	static const int MAX_QUEST_ITEMS = 10;
	static const int MAX_REQUIRES = 60;
	// one item = one entity on the server: a typo of 999999 would spawn a million of them per turn-in (review 2026-10-22)
	static const int MAX_ITEM_COUNT = 1000;
	static const int MAX_TARGET_COUNT = 100000;
	static const int MAX_TIME_LIMIT = 2592000;	// 30 days
	static const int MAX_COOLDOWN = 31536000;	// 365 days
	static const int MAX_NAME_BYTES = 200;
	static const int MAX_TEXT_BYTES = 12000;
	static const int JSON_STRING_LIMIT = 1023;	// bytes of one JSON string the engine reader keeps (measured)
	static const int TEXT_PIECE_BYTES = 900;	// Description pieces written by the converter / the editor
	static const int MAX_GIVERS = 200;
	static const int MAX_CLOTHES = 20;
	static const int MAX_GREETING_BYTES = 500;	// one RPC string (corrupts from ~1000 B)

	static const float DEFAULT_EXPLORE_RADIUS = 5.0;
	static const int TRACK_MAX = 5;			// quests shown in the HUD tracker at once (survey: up to 5)
}

// Target types (JSON "Type" string <-> int).
class INT_QST_TT
{
	static const int NONE = 0;
	static const int KILL = 1;
	static const int DELIVER = 2;		// items are taken at turn-in
	static const int SEARCH = 3;		// items are checked, not taken
	static const int CRAFT = 4;
	static const int FISH = 5;
	static const int HARVEST = 6;
	static const int EXPLORE = 7;
	static const int ACTION = 8;
	static const int COUNT = 9;

	static int FromString(string s)
	{
		string u = s + "";
		u.ToUpper();
		u.TrimInPlace();
		if (u == "KILL")
			return KILL;
		if (u == "DELIVER")
			return DELIVER;
		if (u == "SEARCH")
			return SEARCH;
		if (u == "CRAFT")
			return CRAFT;
		if (u == "FISH")
			return FISH;
		if (u == "HARVEST")
			return HARVEST;
		if (u == "EXPLORE")
			return EXPLORE;
		if (u == "ACTION")
			return ACTION;
		return NONE;
	}

	static string ToKey(int t)
	{
		switch (t)
		{
			case KILL:
				return "KILL";
			case DELIVER:
				return "DELIVER";
			case SEARCH:
				return "SEARCH";
			case CRAFT:
				return "CRAFT";
			case FISH:
				return "FISH";
			case HARVEST:
				return "HARVEST";
			case EXPLORE:
				return "EXPLORE";
			case ACTION:
				return "ACTION";
		}
		return "";
	}

	// Progress kept by the server (event counters). DELIVER/SEARCH are read from the inventory at turn-in.
	static bool IsCounted(int t)
	{
		return t == KILL || t == CRAFT || t == FISH || t == HARVEST || t == EXPLORE || t == ACTION;
	}

	static bool IsInventory(int t)
	{
		return t == DELIVER || t == SEARCH;
	}
}

// Reward types.
class INT_QST_RT
{
	static const int NONE = 0;
	static const int ITEM = 1;
	static const int MONEY = 2;		// INT_Banking account (soft dependency)

	static int FromString(string s)
	{
		string u = s + "";
		u.ToUpper();
		u.TrimInPlace();
		if (u == "ITEM")
			return ITEM;
		if (u == "MONEY")
			return MONEY;
		return NONE;
	}
}

// Repeat modes (JSON "RepeatMode").
class INT_QST_RM
{
	static const int ONCE = 0;
	static const int COOLDOWN = 1;		// "Cooldown" seconds after the turn-in
	static const int DAILY = 2;		// resets every day at ResetHour:ResetMinute, server machine time
	static const int WEEKLY = 3;		// resets on ResetWeekDay (1 = Monday) at ResetHour:ResetMinute

	static int FromString(string s)
	{
		string u = s + "";
		u.ToUpper();
		u.TrimInPlace();
		if (u == "COOLDOWN")
			return COOLDOWN;
		if (u == "DAILY")
			return DAILY;
		if (u == "WEEKLY")
			return WEEKLY;
		return ONCE;
	}

	static string ToKey(int m)
	{
		if (m == COOLDOWN)
			return "COOLDOWN";
		if (m == DAILY)
			return "DAILY";
		if (m == WEEKLY)
			return "WEEKLY";
		return "ONCE";
	}
}

// Item state limits ("" = any). Health levels of the engine: 0 pristine .. 4 ruined.
class INT_QST_State
{
	static int LevelFromString(string s)
	{
		string u = s + "";
		u.ToUpper();
		u.TrimInPlace();
		if (u == "PRISTINE")
			return 0;
		if (u == "WORN")
			return 1;
		if (u == "DAMAGED")
			return 2;
		if (u == "BADLY_DAMAGED")
			return 3;
		if (u == "RUINED")
			return 4;
		return -1;
	}

	static string KeyFromLevel(int lvl)
	{
		switch (lvl)
		{
			case 0:
				return "PRISTINE";
			case 1:
				return "WORN";
			case 2:
				return "DAMAGED";
			case 3:
				return "BADLY_DAMAGED";
			case 4:
				return "RUINED";
		}
		return "";
	}
}

// Wire operations. Client -> server.
class INT_QST_C2S
{
	static const int HELLO = 1;		// client is ready: send me the state
	static const int CATALOG_PULL = 2;	// (hash, packetIndex): next catalog packet
	static const int ACCEPT = 3;		// (questId, giverKey)
	static const int TURNIN = 4;		// (questId, giverKey)
	static const int CANCEL = 5;		// (questId)
	static const int EXPLORED = 6;		// (questId, targetIndex)
	static const int DISMISS = 7;		// (questId): drop an expired quest
	static const int ADMIN = 20;		// (adminOp, ...)
}

// Server -> client.
class INT_QST_S2C
{
	static const int HELLO = 1;		// config, admin flag, catalog hash, givers, player state
	static const int CATALOG = 2;		// (hash, packetIndex, packetCount, pieces...)
	static const int STATE = 3;		// full player state
	static const int PROGRESS = 4;		// (questId, targetIndex, value)
	static const int RESULT = 5;		// (forOp, code, questId)
	static const int NOTE = 6;		// (kind, questId, a, text): banners
	static const int GIVERS = 7;		// giver list changed
	static const int CATALOG_CHANGED = 8;	// (hash): quests were reloaded/edited
	static const int CATALOG_PATCH = 9;	// (oldHash, newHash, kind, delete, key, pieces): one record changed
	static const int ADMIN = 20;		// (adminOp, code, payload...)
}

// Results of requests (RESULT.code). The client turns them into texts from the stringtable.
class INT_QST_Res
{
	static const int OK = 0;
	static const int NOT_FOUND = 1;
	static const int DISABLED = 2;
	static const int TOO_FAR = 3;
	static const int WRONG_GIVER = 4;
	static const int NO_SLOTS = 5;
	static const int LOCKED = 6;		// requirements not met
	static const int COOLDOWN = 7;
	static const int ALREADY_ACTIVE = 8;
	static const int NOT_ACTIVE = 9;
	static const int NOT_DONE = 10;
	static const int MISSING_ITEMS = 11;
	static const int BANK_FULL = 12;
	static const int BANK_UNAVAILABLE = 13;
	static const int CANNOT_CANCEL = 14;
	static const int EXPIRED = 15;
	static const int RATE = 16;
	static const int NOT_ADMIN = 17;
	static const int BAD_DATA = 18;
	static const int STORAGE = 19;
	static const int DONE_ONCE = 20;	// non-repeatable and already done
	static const int DEAD = 21;		// player is dead / not in game
	static const int BUSY = 22;		// inventory busy (action, throw) — try again
	static const int CONFLICT = 23;		// admin: someone changed this quest after you opened it
	static const int ITEM_NOT_EMPTY = 24;	// turn-in: the items to hand over still hold cargo/attachments
}

// Banner kinds (NOTE.kind).
class INT_QST_Note
{
	static const int ACCEPTED = 1;
	static const int TARGET_DONE = 2;
	static const int READY = 3;		// all targets done, go turn it in
	static const int COMPLETED = 4;		// turned in, rewards given
	static const int FAILED = 5;		// time limit
	static const int CANCELLED = 6;
	static const int REWARD_GROUND = 7;	// no room: rewards dropped next to the player
	static const int MONEY = 8;		// a = amount credited to the bank account
	static const int QUEST_ITEM = 9;	// quest item received
	static const int CATALOG = 10;		// quests were updated by an admin
	static const int RESET = 11;		// an admin changed the targets of a taken quest: its progress starts again
}
