// INT Quests (server) — money rewards to the INT_Banking account (owner decisions O3, O10, O16). SOFT dependency:
// the API is compiled only when INT_Banking_SE is loaded (#ifdef INT_BANKING_SE, define from its config.cpp);
// INT_Banking is deliberately not in requiredAddons. Without the bank, money rewards are reported as unavailable
// and the turn-in is refused BEFORE any item is taken. History source "QST" = «Награда за задание» in the bank.

class INT_QST_SE_Bank
{
	static const string SOURCE = "QST";

	static bool Compiled()
	{
		#ifdef INT_BANKING_SE
		return true;
		#else
		return false;
		#endif
	}

	static bool IsAvailable()
	{
		if (!INT_QST_SE_Store.s_Config || INT_QST_SE_Store.s_Config.MoneyRewards == 0)
			return false;
		#ifdef INT_BANKING_SE
		return INT_Banking_API.IsReady();
		#else
		return false;
		#endif
	}

	// INT_QST_Res.OK, BANK_UNAVAILABLE or BANK_FULL (balance would pass the bank limit).
	static int CanCredit(string uid, int amount)
	{
		if (amount <= 0)
			return INT_QST_Res.OK;
		if (!IsAvailable())
			return INT_QST_Res.BANK_UNAVAILABLE;
		#ifdef INT_BANKING_SE
		if (!INT_Banking_API.CanCredit(uid, amount))
			return INT_QST_Res.BANK_FULL;
		return INT_QST_Res.OK;
		#else
		return INT_QST_Res.BANK_UNAVAILABLE;
		#endif
	}

	// Credit after the checks; false = the bank refused (logged by the caller).
	static bool Credit(string uid, int amount, string key)
	{
		if (amount <= 0)
			return true;
		#ifdef INT_BANKING_SE
		if (!INT_Banking_API.IsReady())
			return false;
		int st = INT_Banking_API.Move(uid, amount, "", key, SOURCE);
		return st == INT_Bank_Status.OK;
		#else
		return false;
		#endif
	}
}
