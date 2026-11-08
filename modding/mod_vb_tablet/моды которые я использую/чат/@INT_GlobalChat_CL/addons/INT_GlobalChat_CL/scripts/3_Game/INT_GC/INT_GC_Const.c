// INT GlobalChat — shared constants (spec.md §4.3, §14). The CL part is loaded on both sides.

class INT_GC_Const
{
	static const string VERSION = "1.0.0";
	static const int PROTOCOL = 1;

	// Two global RPCs (ScriptRPC target null): client -> server and server -> client(s).
	static const int RPC_C2S = 9190211;
	static const int RPC_S2C = 9190212;

	static const int MENU_CHAT = 9190201;
	static const int MENU_EDITOR = 9190202;

	// client -> server operations
	static const int OP_HELLO = 1;
	static const int OP_MSG = 2;
	static const int OP_PM = 3;
	static const int OP_VIOL = 4;

	// server -> client operations
	static const int OP_WELCOME = 20;
	static const int OP_WORDS = 21;
	static const int OP_CHAT = 22;
	static const int OP_PM_IN = 23;
	static const int OP_PM_OUT = 24;
	static const int OP_PM_FAIL = 25;
	static const int OP_SYS = 26;
	static const int OP_NOTICE = 27;

	// channels
	static const int CH_GLOBAL = 0;
	static const int CH_LOCAL = 1;
	static const int CH_PM = 2;

	// flags (WELCOME: about me; CHAT / PM: about the sender)
	static const int FLAG_DEBUG = 1;
	static const int FLAG_MUTED = 2;
	static const int FLAG_ADMIN = 4;

	// SYS kinds (shown to everybody who receives them)
	static const int SYS_VIOLATION = 1;		// name broke the rules (strike 1..N-1)
	static const int SYS_MUTED = 2;			// name got the permanent chat ban
	static const int SYS_JOIN = 3;
	static const int SYS_LEAVE = 4;
	static const int SYS_TEXT = 5;			// plain text from the server API
	static const int SYS_KEY = 6;			// stringtable key + up to 3 arguments from the server API

	// NOTICE kinds (only for the addressee)
	static const int NOTICE_WARN = 1;		// a = strikes, b = strikes for the ban
	static const int NOTICE_MUTED = 2;		// the ban was given just now
	static const int NOTICE_IS_MUTED = 3;	// a message was dropped because of the ban

	// limits
	static const int DEFAULT_MAX_LEN = 200;
	static const int MAX_NAME_BYTES = 128;
	static const int MAX_KEY_BYTES = 96;
	static const int WORDS_PART_BYTES = 480;		// one WORDS packet
	// Largest string that goes into one RPC field. Measured on 1.30.164078: ctx.Read of a string of 1024 bytes or
	// more throws «String CORRUPTED» (VM exception, handler aborted); 1023 still reads.
	static const int MAX_WIRE_BYTES = 1000;
	static const int MAX_WORDS = 2000;
	static const int MAX_PREFIXES = 16;
	static const int HELLO_GAP_MS = 5000;
	static const int PALETTE_COUNT = 30;
	static const int DEFAULT_COLOR = 15;			// «Бирюза»

	// Largest text (bytes) a client may send for a given character limit: Cyrillic is 2 bytes, other
	// scripts up to 4; never above MAX_WIRE_BYTES (the client cuts there too).
	static int MaxTextBytes(int maxChars)
	{
		int bytes = maxChars * 4 + 8;
		return Math.Min(bytes, MAX_WIRE_BYTES);
	}
}

// One chat line as the client keeps it (history, feed, public API events).
class INT_GC_Line
{
	static const int KIND_GLOBAL = 0;
	static const int KIND_LOCAL = 1;
	static const int KIND_PM_IN = 2;
	static const int KIND_PM_OUT = 3;
	static const int KIND_SYSTEM = 4;		// vanilla system / admin / BattlEye lines, server API
	static const int KIND_MOD = 5;			// automoderation lines (public)
	static const int KIND_NOTICE = 6;		// personal notices (only for me)
	static const int KIND_JOIN = 7;			// join / leave

	int Kind;
	string Name;		// sender (or addressee for PM_OUT)
	string SenderId;	// GetId() hash; "" for vanilla lines
	int ColorIdx = -1;	// -1 = default colour of the kind
	bool Admin;
	bool Mine;
	bool Mention;
	string Text;
	string Tag;			// small caption before the text (system kinds)
	int Color;			// explicit text colour (0 = by kind)
	int Level;			// notices: 0 info, 1 warning, 2 error
	int Hour;
	int Minute;
	float Born;			// GetTickTime() when it arrived (feed fade)
	int Seq;

	string Describe()
	{
		return "k=" + Kind.ToString() + " n=" + Name + " c=" + ColorIdx.ToString() + " t=" + INT_GC_Log.Clip(Text, 60);
	}
}
