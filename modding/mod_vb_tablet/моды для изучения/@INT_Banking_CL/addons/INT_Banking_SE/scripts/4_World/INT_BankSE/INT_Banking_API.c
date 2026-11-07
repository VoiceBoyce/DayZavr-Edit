// INT Banking (server) - API for other server mods (INT_P2P_Trading pays and pays out through it).
//
// Soft dependency from another mod:
//   #ifdef INT_BANKING_SE
//       int bal = INT_Banking_API.GetBalance(uid);
//       int st  = INT_Banking_API.Move(uid, -price, sellerUid, txnKey, "P2P");
//   #endif
// INT_BANKING_SE is defined by INT_Banking_SE's CfgMods (defines[]).
//
// Every movement is written to the account file, the account history (note "<source> <key>") and the
// admin log. Results are INT_Bank_Status values (OK = 0).
//
// History kind by source: INT_BANK_SRC_BBA (INT_BaseBoxAuthorization) -> BBA_OUT / BBA_IN,
// every other source -> P2P_OUT / P2P_IN.

const string INT_BANK_SRC_BBA = "BBA";

class INT_Banking_API
{
	static bool IsReady()
	{
		if (!INT_BankSE_Server.IsReady())
			INT_BankSE_Boot.Init();
		return INT_BankSE_Server.IsReady();
	}

	static int MaxBalance()
	{
		return INT_BankSE_Config.Get().maxBalance;
	}

	// Balance; 0 when the player has no account yet; -1 when the bank is not ready or the file is broken.
	static int GetBalance(string uid)
	{
		if (!IsReady() || !INT_Bank_Util.IsSteam64(uid))
			return -1;
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, false, "", st);
		if (acc)
			return acc.balance;
		if (st == INT_Bank_Status.TARGET_NOT_FOUND)
			return 0;
		return -1;
	}

	// Would a credit of 'amount' stay under the max balance?
	static bool CanCredit(string uid, int amount)
	{
		if (!IsReady() || amount <= 0)
			return false;
		int bal = GetBalance(uid);
		if (bal < 0)
			return false;
		if (bal > int.MAX - amount)
			return false;
		int maxBal = MaxBalance();
		if (maxBal > 0 && bal + amount > maxBal)
			return false;
		return true;
	}

	// delta > 0 credits (creates the account when needed), delta < 0 debits.
	// source = short tag of the calling mod ("P2P", "BBA"), key = its transaction id (HasKey finds it later).
	static int Move(string uid, int delta, string peerUid, string key, string source)
	{
		if (!IsReady())
			return INT_Bank_Status.INTERNAL;
		if (!INT_Bank_Util.IsSteam64(uid) || delta == 0)
			return INT_Bank_Status.BAD_INPUT;
		string name = INT_BankSE_Service.OnlineName(uid);
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, delta > 0, name, st);
		if (!acc)
		{
			if (st == INT_Bank_Status.TARGET_NOT_FOUND)
				return INT_Bank_Status.NO_FUNDS;
			return st;
		}
		if (name == "")
			name = acc.name;
		int amount = delta;
		if (amount < 0)
			amount = -amount;
		int before = acc.balance;
		bool isBba = source == INT_BANK_SRC_BBA;
		int kind;
		if (delta < 0)
		{
			if (acc.balance < amount)
				return INT_Bank_Status.NO_FUNDS;
			acc.balance = acc.balance - amount;
			kind = INT_Bank_Hist.P2P_OUT;
			if (isBba)
				kind = INT_Bank_Hist.BBA_OUT;
		}
		else
		{
			if (acc.balance > int.MAX - amount)
				return INT_Bank_Status.LIMIT_BALANCE;
			int maxBal = MaxBalance();
			if (maxBal > 0 && acc.balance + amount > maxBal)
				return INT_Bank_Status.LIMIT_BALANCE;
			acc.balance = acc.balance + amount;
			kind = INT_Bank_Hist.P2P_IN;
			if (isBba)
				kind = INT_Bank_Hist.BBA_IN;
		}
		string note = source + " " + key;
		string peerName = INT_BankSE_Service.OnlineName(peerUid);
		acc.AddHistory(kind, amount, peerUid, peerName, note);
		string who = INT_BankSE_Log.Who(name, uid);
		if (!INT_BankSE_Accounts.Save(acc))
		{
			acc.balance = before;
			INT_BankSE_Accounts.Forget(uid);
			string sDelta = INT_Bank_Util.Money(delta);
			INT_BankSE_Log.Op(source, "Игрок: " + who + " | Сумма: " + sDelta + " | Основание: " + note + " | ОШИБКА: файл счёта не записан, баланс не изменён");
			return INT_Bank_Status.STORAGE;
		}
		string what = "Зачислено";
		if (delta < 0)
			what = "Списано";
		string sAmount = INT_Bank_Util.Money(amount);
		string sBefore = INT_Bank_Util.Money(before);
		string sAfter = INT_Bank_Util.Money(acc.balance);
		string line = "Игрок: " + who + " | " + what + ": " + sAmount + " | Баланс: " + sBefore + " -> " + sAfter;
		if (peerUid != "")
		{
			string peerWho = INT_BankSE_Log.Who(peerName, peerUid);
			line = line + " | Второй участник: " + peerWho;
		}
		line = line + " | Основание: " + note + " | УСПЕШНО";
		INT_BankSE_Log.Op(source, line);
		return INT_Bank_Status.OK;
	}

	// Was a movement with this key written to the account history? (crash recovery of the caller)
	static bool HasKey(string uid, string key, string source)
	{
		if (!IsReady())
			return false;
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, false, "", st);
		if (!acc || !acc.history)
			return false;
		string want = source + " " + key;
		foreach (INT_BankSE_HistEntry e : acc.history)
		{
			if (e.note == want)
				return true;
		}
		return false;
	}
}
