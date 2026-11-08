// INT PartyMode — constants shared by both sides (spec.md §5). IDs: block 925xxxx (free across INT mods and the
// F: client mods, research/01). One RPC id per direction, the first int of a packet is the operation.

class INT_PTY_Const
{
	static const string VERSION = "1.1.0";
	static const int PROTOCOL = 2;			// 2: client -> server ids as two ints (review PTY-12)

	static const int RPC_C2S = 9250211;
	static const int RPC_S2C = 9250212;
	static const int MENU_MAIN = 9250201;
	static const int MENU_MOVER = 9250202;

	// client -> server
	static const int OP_HELLO = 1;
	static const int OP_PULL = 2;
	static const int OP_PING = 3;
	static const int OP_PING_CLEAR = 4;
	static const int OP_SEARCH_SET = 5;
	static const int OP_SEARCH_LIST = 6;
	static const int OP_INVITE = 7;
	static const int OP_INVITE_CANCEL = 8;
	static const int OP_INVITE_ACCEPT = 9;
	static const int OP_INVITE_DECLINE = 10;
	static const int OP_FRIEND_REMOVE = 11;

	// server -> client
	static const int OP_WELCOME = 101;
	static const int OP_FRIENDS = 102;			// roster part (first part resets the list)
	static const int OP_STATE = 103;			// answer to PULL
	static const int OP_PING_ADD = 104;
	static const int OP_PING_CLEAR_OF = 105;	// all pings of one owner are gone
	static const int OP_SEARCH_STATE = 106;
	static const int OP_SEARCH_ROWS = 107;
	static const int OP_INVITE_IN = 108;		// a new incoming invite
	static const int OP_INVITE_GONE = 109;		// an incoming invite vanished (reason)
	static const int OP_INVITE_OUT = 110;		// state of one of my outgoing invites
	static const int OP_FRIEND_ADDED = 111;
	static const int OP_FRIEND_REMOVED = 112;
	static const int OP_FRIEND_DEAD = 113;
	static const int OP_RESULT = 114;			// a request was refused (code, seconds)

	// states of an outgoing invite (OP_INVITE_OUT)
	static const int OUT_SENT = 0;
	static const int OUT_ACCEPTED = 1;
	static const int OUT_DECLINED = 2;
	static const int OUT_SEARCH_OFF = 3;		// the target switched search off — declined
	static const int OUT_TARGET_LEFT = 4;		// the target left the server
	static const int OUT_CANCELLED = 5;			// I withdrew it

	// why an incoming invite vanished (OP_INVITE_GONE)
	static const int GONE_CANCELLED = 1;
	static const int GONE_INVITER_LEFT = 2;
	static const int GONE_SEARCH_OFF = 3;		// I switched search off myself
	static const int GONE_FRIENDS = 4;			// we became friends another way
	static const int GONE_DECLINED = 5;			// I declined it (ack)

	// refusals (OP_RESULT)
	static const int RES_RATE_LIMITED = 1;
	static const int RES_NOT_SEARCHING = 2;
	static const int RES_ALREADY_FRIENDS = 3;
	static const int RES_ALREADY_INVITED = 4;
	static const int RES_TOO_MANY_INVITES = 5;
	static const int RES_MY_FRIEND_LIMIT = 6;
	static const int RES_HIS_FRIEND_LIMIT = 7;
	static const int RES_NOT_ONLINE = 8;
	static const int RES_NO_INVITE = 9;
	static const int RES_SELF = 10;
	static const int RES_SAVE_FAILED = 11;
	static const int RES_NOT_FRIEND = 12;

	// friend state flags (OP_STATE)
	static const int F_ONLINE = 1;
	static const int F_ALIVE = 2;
	static const int F_UNCONSCIOUS = 4;
	static const int F_BLEEDING = 8;
	static const int F_BROKEN_LEG = 16;
	static const int F_HAS_POS = 32;

	// wire limits
	static const int STEAM_ID_LEN = 17;
	static const int MAX_NAME_CHARS = 32;		// nicks are cut to this on the wire and in files
	static const int MAX_SEARCH_LIST = 60;		// entries in one OP_SEARCH_LIST
	static const int FRIENDS_PER_PART = 40;		// entries in one OP_FRIENDS part
	static const int MAX_PULL_IDS = 50;			// hard cap of a PULL request (config limit applies first)
	static const int HELLO_GAP_MS = 4000;
	static const int SEARCH_LIST_GAP_MS = 1500;

	// config clamps (server)
	static const int HARD_DISPLAY_LIMIT = 50;
	static const int HARD_PING_LIMIT = 10;

	// client settings clamps: how far a friend's ping is still drawn (owner 2026-11-01)
	static const int PING_VIEW_MIN = 100;
	static const int PING_VIEW_MAX = 2000;
}
