// INT Banking (server) - player accounts, $profile:INT_Mods/INT_Banking/PlayersAccounts/<Steam64>.json
//
// {
//   "steamId": "76561198000000000",
//   "name": "last known nick",
//   "balance": 125000,
//   "history": [ { "time": "15.10.2026 21:30", "unix": 1760563800, "type": "WITHDRAW", "amount": 5000,
//                  "balanceAfter": 125000, "peerId": "", "peerName": "", "note": "" } ]
// }
//
// liveAccountReload = 1: the file is read again on every ATM open and before every operation, so an
// admin may edit "balance" while the server runs. A file that does not parse is NEVER overwritten:
// the player gets "account damaged, contact an admin" until the file is fixed.

class INT_BankSE_HistEntry
{
	string time;
	int unix;
	string type;
	int amount;
	int balanceAfter;
	string peerId;
	string peerName;
	string note;
}

class INT_BankSE_AccountData
{
	string steamId;
	string name;
	int balance;
	ref array<ref INT_BankSE_HistEntry> history;

	void INT_BankSE_AccountData()
	{
		history = new array<ref INT_BankSE_HistEntry>();
	}

	void AddHistory(int type, int amount, string peerId, string peerName, string note)
	{
		if (!history)
			history = new array<ref INT_BankSE_HistEntry>();
		INT_BankSE_HistEntry e = new INT_BankSE_HistEntry();
		e.time = INT_BankSE_Time.Short();
		e.unix = INT_BankSE_Time.Unix();
		e.type = INT_Bank_Util.HistKey(type);
		e.amount = amount;
		e.balanceAfter = balance;
		e.peerId = peerId;
		e.peerName = INT_Bank_Util.CleanName(peerName, 32);
		e.note = note;
		history.Insert(e);
		int maxEntries = INT_BankSE_Config.Get().maxHistoryEntries;
		if (maxEntries <= 0)
		{
			history.Clear();
			return;
		}
		while (history.Count() > maxEntries)
			history.RemoveOrdered(0);
	}
}

class INT_BankSE_Accounts
{
	protected static ref map<string, ref INT_BankSE_AccountData> s_Cache;
	// Broken files already reported this session: uid -> file text (reported again only when it changes).
	protected static ref map<string, string> s_Broken;

	static void Init()
	{
		s_Cache = new map<string, ref INT_BankSE_AccountData>();
		s_Broken = new map<string, string>();
	}

	static bool Exists(string uid)
	{
		if (!INT_Bank_Util.IsSteam64(uid))
			return false;
		if (s_Cache && s_Cache.Contains(uid))
			return true;
		string path = INT_BankSE_Paths.AccFile(uid);
		if (FileExist(path))
			return true;
		// an interrupted save leaves only .bak or .tmp
		if (FileExist(path + ".bak") || FileExist(path + ".tmp"))
			return true;
		return false;
	}

	static bool IsLive()
	{
		return INT_BankSE_Config.Get().liveAccountReload != 0;
	}

	// Account for an operation. status: OK, TARGET_NOT_FOUND (missing, create = false), ACCOUNT_BROKEN, STORAGE.
	// With liveAccountReload the file is always read again; otherwise the cached object is used.
	static INT_BankSE_AccountData Get(string uid, bool create, string name, out int status)
	{
		status = INT_Bank_Status.OK;
		if (!s_Cache)
			Init();
		if (!IsLive() && s_Cache.Contains(uid))
		{
			INT_BankSE_AccountData cached = s_Cache.Get(uid);
			Touch(cached, uid, name);
			return cached;
		}
		string path = INT_BankSE_Paths.AccFile(uid);
		// A file reported broken before and not changed since is not read (and not copied) again.
		if (s_Broken.Contains(uid))
		{
			string now;
			INT_BankSE_File.ReadText(path, now);
			if (now == s_Broken.Get(uid))
			{
				status = INT_Bank_Status.ACCOUNT_BROKEN;
				return null;
			}
			s_Broken.Remove(uid);
		}
		INT_BankSE_AccountData data = new INT_BankSE_AccountData();
		int res = INT_BankSE_Json<INT_BankSE_AccountData>.Load(path, data);
		if (res == INT_BANKSE_LOAD_OK)
		{
			if (!data.history)
				data.history = new array<ref INT_BankSE_HistEntry>();
			if (data.balance < 0)
			{
				string neg = data.balance.ToString();
				INT_BankSE_Log.Warn("отрицательный баланс в файле счёта " + uid + " (" + neg + ") - считаю как 0");
				data.balance = 0;
			}
			if (data.steamId != uid)
				data.steamId = uid;
			Touch(data, uid, name);
			s_Cache.Set(uid, data);
			return data;
		}
		if (res == INT_BANKSE_LOAD_BROKEN)
		{
			string txt;
			INT_BankSE_File.ReadText(path, txt);
			s_Broken.Set(uid, txt);
			s_Cache.Remove(uid);
			INT_BankSE_Log.Critical("файл счёта не читается, операции игрока заблокированы до исправления: " + path);
			status = INT_Bank_Status.ACCOUNT_BROKEN;
			return null;
		}
		// missing
		if (!create)
		{
			status = INT_Bank_Status.TARGET_NOT_FOUND;
			return null;
		}
		data = new INT_BankSE_AccountData();
		data.steamId = uid;
		data.name = INT_Bank_Util.CleanName(name, 32);
		data.balance = 0;
		if (!Save(data))
		{
			status = INT_Bank_Status.STORAGE;
			return null;
		}
		s_Cache.Set(uid, data);
		string who = INT_BankSE_Log.Who(name, uid);
		INT_BankSE_Log.Op("НОВЫЙ СЧЁТ", "Игрок: " + who + " | Файл: " + path);
		return data;
	}

	// Remembers the latest nick (shown to people sending money to this account).
	protected static void Touch(INT_BankSE_AccountData data, string uid, string name)
	{
		if (name == "")
			return;
		string clean = INT_Bank_Util.CleanName(name, 32);
		if (clean == "" || clean == data.name)
			return;
		data.name = clean;
		// The name alone is not worth a write; it is stored with the next balance change.
	}

	static bool Save(INT_BankSE_AccountData data)
	{
		if (!data)
			return false;
		string path = INT_BankSE_Paths.AccFile(data.steamId);
		return INT_BankSE_Json<INT_BankSE_AccountData>.Save(path, data);
	}

	// Drops the cached copy (the next Get reads the file).
	static void Forget(string uid)
	{
		if (s_Cache)
			s_Cache.Remove(uid);
	}

	static string NameOf(string uid)
	{
		if (s_Cache && s_Cache.Contains(uid))
		{
			INT_BankSE_AccountData d = s_Cache.Get(uid);
			return d.name;
		}
		return "";
	}
}
