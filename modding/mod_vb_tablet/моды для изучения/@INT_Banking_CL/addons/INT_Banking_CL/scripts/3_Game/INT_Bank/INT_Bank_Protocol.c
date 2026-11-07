// INT Banking - protocol shared by the client part (CL) and the server part (SE).
// readme.md "Протокол" is the human-readable version of this file. Change both together.
//
// Transport: ScriptRPC on the player entity (PlayerBase), fields written one by one.
//   C2S  INT_BANK_RPC_C2S: int op, int seq, int a, int b, int c, string s
//   S2C  INT_BANK_RPC_S2C: int op, int seq, int status, int a, int b, int c, int d, string s1, string s2,
//                          INT_Bank_State, then the op payload (history page / player page).
// Strings stay short (ids, names <= 32 chars); lists travel in pages, never as one long string.

const int INT_BANK_MENU_ID = 9130201;
const int INT_BANK_RPC_C2S = 9130211;
const int INT_BANK_RPC_S2C = 9130212;
const string INT_BANK_VERSION = "1.0.5";
const string INT_BANK_TAG = "[INT_BANK]";
const string INT_BANK_TITLE = "DAYZAVR BANKING SYSTEM";
const int INT_BANK_HISTORY_PAGE = 8;
const int INT_BANK_AMOUNT_DIGITS = 9;

enum INT_Bank_Op
{
	NONE = 0,
	HELLO = 1,			// open: currency list + state (+ the account is created / name refreshed)
	STATE = 2,			// refresh state only
	DEPOSIT = 10,		// a = amount, b = 1 "deposit all" (c unused since 1.0.5: change that does not fit -> account)
	WITHDRAW = 11,		// a = amount (c unused since 1.0.5: no payout on the ground)
	LOOKUP = 12,		// s = Steam64 of a recipient -> s1 = name, a = 1 has account
	TRANSFER = 13,		// a = amount, s = Steam64 of the recipient
	HISTORY = 20,		// a = page
	CLEAR_HISTORY = 21,
	// 22 was PLAYERS (online list) - removed 2026-10-16 on players' request: it let anyone see who is online
	PUSH_TRANSFER_IN = 60	// server -> recipient: a = amount, s1 = sender name, s2 = sender id
}

// Every status except OK has a string STR_INT_BANK_ERR_<NAME>.
enum INT_Bank_Status
{
	OK = 0,
	NO_SERVER = 1,			// CL on the server, SE missing
	RATE = 2,				// requests too fast
	BUSY = 3,				// previous operation still running
	NOT_NEAR = 4,			// no ATM within reach
	PLAYER_STATE = 5,		// dead / unconscious / restrained
	BAD_INPUT = 6,
	COOLDOWN = 7,			// a = seconds left
	NO_FUNDS = 8,			// not enough on the account
	NO_CASH = 9,			// not enough cash in the inventory
	LIMIT_BALANCE = 10,		// a = max balance
	LIMIT_OP = 11,			// a = max per operation
	TARGET_INVALID = 12,	// not a Steam64
	TARGET_NOT_FOUND = 13,	// recipient has no account
	TARGET_SELF = 14,
	NEED_GROUND = 15,		// unused since 1.0.5 (was: "give on the ground?"), the value stays reserved
	NOT_PAYABLE = 16,		// amount cannot be paid with the configured notes
	STORAGE = 17,			// account file could not be written, nothing changed
	ACCOUNT_BROKEN = 18,	// account file unreadable: an admin must look at it
	INTERNAL = 19,
	TARGET_LIMIT = 20,		// recipient would exceed the max balance
	TIMEOUT = 21,			// client only: no answer
	NO_ROOM = 22			// 1.0.5: not all notes fit into the inventory, nothing given; a = value that fits, b = total
}

enum INT_Bank_Hist
{
	DEPOSIT = 0,
	WITHDRAW = 1,
	TRANSFER_IN = 2,
	TRANSFER_OUT = 3,
	P2P_IN = 4,
	P2P_OUT = 5,
	ADMIN = 6,
	// 2026-10-22: INT_BaseBoxAuthorization (API source "BBA"). New values only APPENDED, never renumber:
	// the account file stores the key string (HistKey), the RPC sends the number.
	BBA_IN = 7,		// refund
	BBA_OUT = 8,	// box upgrade charge
	// 2026-10-22: INT_Quests (API source "QST"). Quests only credit money, so there is no QST_OUT
	// (a QST debit is refused by INT_Banking_API.Move); 10 stays free.
	QST_IN = 9		// quest reward
}

// Account state the server attaches to every answer.
class INT_Bank_State
{
	int Balance;
	int Cash;				// cash value the player carries
	int MaxBalance;			// 0 = no limit
	int MaxDeposit;			// per operation, 0 = no limit
	int MaxWithdraw;
	int MaxTransfer;
	int CdDeposit;			// seconds until the next deposit is allowed
	int CdWithdraw;
	int CdTransfer;
	int HistoryCount;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Balance);
		rpc.Write(Cash);
		rpc.Write(MaxBalance);
		rpc.Write(MaxDeposit);
		rpc.Write(MaxWithdraw);
		rpc.Write(MaxTransfer);
		rpc.Write(CdDeposit);
		rpc.Write(CdWithdraw);
		rpc.Write(CdTransfer);
		rpc.Write(HistoryCount);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Balance))
			return false;
		if (!ctx.Read(Cash))
			return false;
		if (!ctx.Read(MaxBalance))
			return false;
		if (!ctx.Read(MaxDeposit))
			return false;
		if (!ctx.Read(MaxWithdraw))
			return false;
		if (!ctx.Read(MaxTransfer))
			return false;
		if (!ctx.Read(CdDeposit))
			return false;
		if (!ctx.Read(CdWithdraw))
			return false;
		if (!ctx.Read(CdTransfer))
			return false;
		if (!ctx.Read(HistoryCount))
			return false;
		return true;
	}

	void CopyFrom(INT_Bank_State o)
	{
		Balance = o.Balance;
		Cash = o.Cash;
		MaxBalance = o.MaxBalance;
		MaxDeposit = o.MaxDeposit;
		MaxWithdraw = o.MaxWithdraw;
		MaxTransfer = o.MaxTransfer;
		CdDeposit = o.CdDeposit;
		CdWithdraw = o.CdWithdraw;
		CdTransfer = o.CdTransfer;
		HistoryCount = o.HistoryCount;
	}
}

// One history row as the client shows it.
class INT_Bank_HistRow
{
	string Time;			// "15.10.2026 21:30" server local time
	int Type;				// INT_Bank_Hist
	int Amount;
	int BalanceAfter;
	string PeerId;
	string PeerName;
}

// One server answer (client side, queued and dispatched outside the RPC handler).
class INT_Bank_Reply
{
	int Op;
	int Seq;
	int Status;
	int A;
	int B;
	int C;
	int D;
	string S1;
	string S2;
	ref INT_Bank_State State;
	// HELLO: currency
	ref array<string> Classes;
	ref array<int> Values;
	ref array<int> Units;
	// HISTORY
	ref array<ref INT_Bank_HistRow> Rows;

	void INT_Bank_Reply()
	{
		State = new INT_Bank_State();
		Classes = new array<string>();
		Values = new array<int>();
		Units = new array<int>();
		Rows = new array<ref INT_Bank_HistRow>();
	}
}

class INT_Bank_Util
{
	// "1 234 567" (thin groups by a plain space - the game fonts have no narrow no-break space).
	static string Money(int value)
	{
		bool neg = value < 0;
		int v = value;
		if (neg)
			v = -v;
		string digits = v.ToString();
		string res = "";
		int n = digits.Length();
		for (int i = 0; i < n; i++)
		{
			int fromEnd = n - i;
			if (i > 0 && fromEnd % 3 == 0)
				res = res + " ";
			res = res + digits.Get(i);
		}
		if (neg)
			res = "-" + res;
		return res;
	}

	// Only 0-9 survive.
	static string Digits(string raw)
	{
		string res = "";
		int n = raw.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = raw.Get(i);
			if (ch == "0" || ch == "1" || ch == "2" || ch == "3" || ch == "4" || ch == "5" || ch == "6" || ch == "7" || ch == "8" || ch == "9")
				res = res + ch;
		}
		return res;
	}

	// Steam64 of a user account: 17 digits starting with 7656 (7656119... for older accounts,
	// 765612... for newer ones - measured on the test stand: 76561202007535715).
	static bool IsSteam64(string id)
	{
		if (id.Length() != 17)
			return false;
		string digits = Digits(id);
		if (digits != id)
			return false;
		return id.IndexOf("7656") == 0;
	}

	// Positive amount from digits, -1 when it does not fit (more than 9 digits).
	static int ParseAmount(string raw)
	{
		string d = Digits(raw);
		if (d == "")
			return 0;
		while (d.Length() > 1 && d.Get(0) == "0")
			d = d.Substring(1, d.Length() - 1);
		if (d.Length() > INT_BANK_AMOUNT_DIGITS)
			return -1;
		return d.ToInt();
	}

	// A player name safe for a log line or a label: no line breaks, at most maxChars characters.
	static string CleanName(string raw, int maxChars)
	{
		string s = raw + "";
		s.Replace("\r", " ");
		s.Replace("\n", " ");
		s.Replace("\t", " ");
		s.Replace("%", " ");
		s.TrimInPlace();
		if (s.LengthUtf8() > maxChars)
			s = s.SubstringUtf8(0, maxChars);
		return s;
	}

	static string HistKey(int type)
	{
		switch (type)
		{
			case INT_Bank_Hist.DEPOSIT:
				return "DEPOSIT";
			case INT_Bank_Hist.WITHDRAW:
				return "WITHDRAW";
			case INT_Bank_Hist.TRANSFER_IN:
				return "TRANSFER_IN";
			case INT_Bank_Hist.TRANSFER_OUT:
				return "TRANSFER_OUT";
			case INT_Bank_Hist.P2P_IN:
				return "P2P_IN";
			case INT_Bank_Hist.P2P_OUT:
				return "P2P_OUT";
			case INT_Bank_Hist.ADMIN:
				return "ADMIN";
			case INT_Bank_Hist.BBA_IN:
				return "BBA_IN";
			case INT_Bank_Hist.BBA_OUT:
				return "BBA_OUT";
			case INT_Bank_Hist.QST_IN:
				return "QST_IN";
		}
		return "DEPOSIT";
	}

	static int HistFromKey(string key)
	{
		string k = key + "";
		k.ToUpper();
		if (k == "WITHDRAW")
			return INT_Bank_Hist.WITHDRAW;
		if (k == "TRANSFER_IN")
			return INT_Bank_Hist.TRANSFER_IN;
		if (k == "TRANSFER_OUT")
			return INT_Bank_Hist.TRANSFER_OUT;
		if (k == "P2P_IN")
			return INT_Bank_Hist.P2P_IN;
		if (k == "P2P_OUT")
			return INT_Bank_Hist.P2P_OUT;
		if (k == "ADMIN")
			return INT_Bank_Hist.ADMIN;
		if (k == "BBA_IN")
			return INT_Bank_Hist.BBA_IN;
		if (k == "BBA_OUT")
			return INT_Bank_Hist.BBA_OUT;
		if (k == "QST_IN")
			return INT_Bank_Hist.QST_IN;
		return INT_Bank_Hist.DEPOSIT;
	}

	// Money arriving on the account (shown green, with a plus).
	static bool HistIsIncome(int type)
	{
		if (type == INT_Bank_Hist.DEPOSIT || type == INT_Bank_Hist.TRANSFER_IN || type == INT_Bank_Hist.P2P_IN || type == INT_Bank_Hist.BBA_IN || type == INT_Bank_Hist.QST_IN)
			return true;
		return false;
	}

	static string StatusName(int status)
	{
		return typename.EnumToString(INT_Bank_Status, status);
	}
}
