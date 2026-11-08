// Start-up: profile folders, the log of this run, config, currency list, then open for requests.
// Called from MissionServer.OnInit AND lazily by INT_Banking_API, so another server mod (INT_P2P_Trading
// crash recovery at its own boot) never sees "bank not ready" just because its OnInit ran first.
class INT_BankSE_Boot
{
	protected static bool s_Done;

	static void Init()
	{
		if (s_Done)
			return;
		if (!GetGame() || !GetGame().IsDedicatedServer())
			return;
		s_Done = true;
		INT_BankSE_Paths.EnsureAll();
		INT_BankSE_Log.Init();
		INT_BankSE_Config.Load();
		INT_BankSE_Accounts.Init();
		INT_BankSE_Service.Init();
		INT_BankSE_Money.Load();
		INT_BankSE_Server.SetReady(true);
		INT_BankSE_Log.Info("настройки: " + INT_BankSE_Config.Describe());
		INT_BankSE_Log.Info("счета: " + INT_BankSE_Paths.AccDir() + " | лог: " + INT_BankSE_Log.File());
		Print("[INT_BANK_SE] ready " + INT_BANK_VERSION);
	}
}
