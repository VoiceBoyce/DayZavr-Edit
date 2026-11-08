// INT Banking (server) - $profile:INT_Mods/INT_Banking/INT_BankingConfig.json
// Written with the defaults below on the first start. Field names are the JSON keys.
// Limits: 0 means "no limit". Cooldowns are in seconds.

class INT_BankSE_CurrencyCfg
{
	string itemClassname;
	int itemValue;			// value of requiredQuantity pieces of this item
	int requiredQuantity;	// pieces that make one itemValue (1 = every piece is worth itemValue)
	int acceptPartial;		// 1 = a deposit may take part of a stack, 0 = only whole stacks (change is given)

	void INT_BankSE_CurrencyCfg(string cls = "", int value = 1, int quantity = 1, int partial = 1)
	{
		itemClassname = cls;
		itemValue = value;
		requiredQuantity = quantity;
		acceptPartial = partial;
	}
}

class INT_BankSE_ConfigData
{
	int configVersion;
	// 1 = the account file is read again every time the ATM is opened and before every operation,
	// so an admin can edit a balance in PlayersAccounts/<id>.json without a restart.
	int liveAccountReload;
	int maxBalance;
	int maxDepositPerOperation;
	int maxWithdrawPerOperation;
	int maxTransferPerOperation;
	int depositCooldownSec;
	int withdrawCooldownSec;
	int transferCooldownSec;
	// Minimum time between a deposit and a withdraw (in either order).
	int depositWithdrawGapSec;
	int maxHistoryEntries;
	// The player must stand this close to an ATM for any operation (0 = not checked).
	float atmUseDistance;
	// A recipient who is online gets a notification about an incoming transfer.
	int notifyRecipient;
	// Refused operations (no money, cooldown, ...) are written to the admin log too.
	int logRefusals;
	ref array<ref INT_BankSE_CurrencyCfg> currencies;

	void INT_BankSE_ConfigData()
	{
		configVersion = INT_BankSE_Config.VERSION;
		liveAccountReload = 1;
		maxBalance = 30000000;
		maxDepositPerOperation = 0;
		maxWithdrawPerOperation = 500000;
		maxTransferPerOperation = 0;
		depositCooldownSec = 5;
		withdrawCooldownSec = 5;
		transferCooldownSec = 10;
		depositWithdrawGapSec = 10;
		maxHistoryEntries = 50;
		atmUseDistance = 4.0;
		notifyRecipient = 1;
		logRefusals = 1;
		currencies = new array<ref INT_BankSE_CurrencyCfg>();
	}

	void SeedCurrencies()
	{
		currencies.Clear();
		currencies.Insert(new INT_BankSE_CurrencyCfg("MoneyRuble1", 1, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo5Rubley", 5, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo10Rubley", 10, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("MoneyRuble25", 25, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo50Rubley", 50, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo100Rubley", 100, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo200Rubley", 200, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo500Rubley", 500, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo1000Rubley", 1000, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo2000Rubley", 2000, 1, 1));
		currencies.Insert(new INT_BankSE_CurrencyCfg("Bablo5000Rubley", 5000, 1, 1));
	}
}

class INT_BankSE_Config
{
	static const int VERSION = 1;
	protected static ref INT_BankSE_ConfigData s_Data;

	static INT_BankSE_ConfigData Get()
	{
		if (!s_Data)
			Load();
		return s_Data;
	}

	static void Load()
	{
		INT_BankSE_Paths.EnsureAll();
		string path = INT_BankSE_Paths.Config();
		INT_BankSE_ConfigData data = new INT_BankSE_ConfigData();
		int res = INT_BankSE_Json<INT_BankSE_ConfigData>.Load(path, data);
		if (res == INT_BANKSE_LOAD_MISSING)
		{
			data = new INT_BankSE_ConfigData();
			data.SeedCurrencies();
			if (INT_BankSE_Json<INT_BankSE_ConfigData>.Save(path, data))
				INT_BankSE_Log.Info("создан конфиг по умолчанию: " + path);
			else
				INT_BankSE_Log.Err("не удалось записать конфиг по умолчанию: " + path);
		}
		else if (res == INT_BANKSE_LOAD_BROKEN)
		{
			// Keep the admin's file as it is (a copy is kept as .broken-<stamp>); run on defaults.
			data = new INT_BankSE_ConfigData();
			data.SeedCurrencies();
			INT_BankSE_Log.Critical("конфиг не читается (ошибка JSON), работаю на настройках по умолчанию до исправления: " + path);
		}
		else
		{
			if (!data.currencies)
				data.currencies = new array<ref INT_BankSE_CurrencyCfg>();
			if (data.currencies.Count() == 0)
			{
				data.SeedCurrencies();
				INT_BankSE_Log.Warn("в конфиге нет валют - использую список по умолчанию");
			}
			if (data.configVersion < VERSION)
			{
				data.configVersion = VERSION;
				INT_BankSE_Json<INT_BankSE_ConfigData>.Save(path, data);
			}
		}
		Sanitize(data);
		s_Data = data;
	}

	protected static void Sanitize(INT_BankSE_ConfigData d)
	{
		if (d.maxBalance < 0)
			d.maxBalance = 0;
		if (d.maxDepositPerOperation < 0)
			d.maxDepositPerOperation = 0;
		if (d.maxWithdrawPerOperation < 0)
			d.maxWithdrawPerOperation = 0;
		if (d.maxTransferPerOperation < 0)
			d.maxTransferPerOperation = 0;
		if (d.depositCooldownSec < 0)
			d.depositCooldownSec = 0;
		if (d.withdrawCooldownSec < 0)
			d.withdrawCooldownSec = 0;
		if (d.transferCooldownSec < 0)
			d.transferCooldownSec = 0;
		if (d.depositWithdrawGapSec < 0)
			d.depositWithdrawGapSec = 0;
		if (d.maxHistoryEntries < 0)
			d.maxHistoryEntries = 0;
		if (d.maxHistoryEntries > 500)
			d.maxHistoryEntries = 500;
		if (d.atmUseDistance < 0)
			d.atmUseDistance = 0;
		foreach (INT_BankSE_CurrencyCfg c : d.currencies)
		{
			if (c.requiredQuantity < 1)
				c.requiredQuantity = 1;
		}
	}

	static string Describe()
	{
		INT_BankSE_ConfigData d = Get();
		string s = "liveAccountReload=" + d.liveAccountReload.ToString();
		s = s + " maxBalance=" + d.maxBalance.ToString();
		s = s + " maxDeposit=" + d.maxDepositPerOperation.ToString();
		s = s + " maxWithdraw=" + d.maxWithdrawPerOperation.ToString();
		s = s + " maxTransfer=" + d.maxTransferPerOperation.ToString();
		s = s + " cooldown dep/wd/tr/gap=" + d.depositCooldownSec.ToString();
		s = s + "/" + d.withdrawCooldownSec.ToString();
		s = s + "/" + d.transferCooldownSec.ToString();
		s = s + "/" + d.depositWithdrawGapSec.ToString();
		s = s + " history=" + d.maxHistoryEntries.ToString();
		s = s + " atmDistance=" + d.atmUseDistance.ToString();
		return s;
	}
}
