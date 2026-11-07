// INT Banking (server) - self-test for the test stand.
//
// Marker: $profile:INT_Mods/INT_Banking/int_bank_selftest.txt (server profile). Polled every 2 s; a new
// first line starts a run. Pure tests run at once; the live part waits for the first online player and
// works directly on the service (no ATM distance check). The player's account file is copied first and
// written back at the end; spawned filler items are deleted. Banknotes the player received stay.
// Lines: "[INT_BANK_SE] [ТЕСТ] PASS <name>" / "FAIL <name> <details>", then "DONE pass=<n> fail=<n>".
// Remove the marker after the run.

class INT_BankSE_SelfTest
{
	static const string MARKER = "$profile:INT_Mods/INT_Banking/int_bank_selftest.txt";
	static const string FAKE_A = "76561190000000001";
	static const string FAKE_B = "76561190000000002";

	protected static float s_Poll;
	protected static string s_Line = "";
	protected static bool s_WaitPlayer;
	protected static bool s_Cleaned;
	protected static int s_Pass;
	protected static int s_Fail;

	static void Tick(float dt)
	{
		s_Poll = s_Poll + dt;
		if (s_Poll < 2.0)
			return;
		s_Poll = 0;
		if (!INT_BankSE_Server.IsReady())
			return;
		if (s_WaitPlayer)
		{
			PlayerBase p = AnyPlayer();
			if (p && !s_Cleaned)
			{
				// start from an empty wallet (test stand character): earlier runs leave notes that
				// fill the cargo; the run starts on the next poll, when the slots are free again
				CleanWallet(p);
				s_Cleaned = true;
				return;
			}
			if (p)
			{
				s_WaitPlayer = false;
				s_Cleaned = false;
				RunLive(p);
				Done();
			}
			return;
		}
		if (!FileExist(MARKER))
			return;
		string line = ReadLine(MARKER);
		if (line == s_Line)
			return;
		s_Line = line;
		s_Pass = 0;
		s_Fail = 0;
		INT_BankSE_Log.Test("старт самотеста '" + line + "'");
		RunPure();
		if (line.IndexOf("pure") != -1)
		{
			Done();
			return;
		}
		s_WaitPlayer = true;
		INT_BankSE_Log.Test("жду игрока онлайн для живой части");
	}

	static string ReadLine(string path)
	{
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return "";
		string line = "";
		FGets(fh, line);
		CloseFile(fh);
		line.TrimInPlace();
		if (line == "")
			line = "run";
		return line;
	}

	protected static void Done()
	{
		string sp = s_Pass.ToString();
		string sf = s_Fail.ToString();
		string done = "DONE pass=" + sp + " fail=" + sf;
		Print("[INT_BANK_SE] [TEST] " + done);
		INT_BankSE_Log.Test(done);
	}

	protected static void Check(string name, bool ok, string details = "")
	{
		if (ok)
		{
			s_Pass++;
			Print("[INT_BANK_SE] [TEST] PASS " + name);
			return;
		}
		s_Fail++;
		string d = details;
		d.Replace("%", " pct");
		Print("[INT_BANK_SE] [TEST] FAIL " + name + " " + d);
		INT_BankSE_Log.Test("FAIL " + name + " " + d);
	}

	protected static void CleanWallet(PlayerBase p)
	{
		array<ref INT_BankSE_CashItem> old = new array<ref INT_BankSE_CashItem>();
		INT_BankSE_Money.Collect(p, old);
		foreach (INT_BankSE_CashItem oc0 : old)
		{
			if (oc0.Item)
				GetGame().ObjectDelete(oc0.Item);
		}
	}

	protected static PlayerBase AnyPlayer()
	{
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man m : players)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (p && p.GetIdentity() && p.IsAlive())
				return p;
		}
		return null;
	}

	protected static void RemoveFake(string uid)
	{
		string path = INT_BankSE_Paths.AccFile(uid);
		DeleteFile(path);
		DeleteFile(path + ".bak");
		DeleteFile(path + ".tmp");
		INT_BankSE_Accounts.Forget(uid);
	}

	// ------------------------------------------------------------------------------------------------
	// VPP Object Builder lists a CfgVehicles class only when its immediate config parent is HouseNoDestruct
	// (MenuObjectManager: ConfigGetBaseName == "housenodestruct"); the bank logic needs the script class.
	protected static void CheckAtmClasses()
	{
		string listed = "";
		string wrong = "";
		int n = GetGame().ConfigGetChildrenCount("CfgVehicles");
		for (int i = 0; i < n; i++)
		{
			string cls;
			GetGame().ConfigGetChildName("CfgVehicles", i, cls);
			typename t = cls.ToType();
			if (!t || !t.IsInherited(MF_BankObjectBase))
				continue;
			string baseName;
			GetGame().ConfigGetBaseName("CfgVehicles " + cls, baseName);
			baseName.ToLower();
			int scope = GetGame().ConfigGetInt("CfgVehicles " + cls + " scope");
			if (baseName == "housenodestruct" && scope == 2)
				listed = listed + cls + " ";
			else if (cls != "MF_BankObjectBase")
				wrong = wrong + cls + " ";
		}
		int hasOrange = listed.IndexOf("BankingATMOrange ");
		int hasKiosk = listed.IndexOf("sm_atm_DayZavr ");
		bool atmOk = wrong == "" && hasOrange != -1 && hasKiosk != -1;
		string atmInfo = "wrong=" + wrong;
		Check("atm.object_builder", atmOk, atmInfo);
		Print("[INT_BANK_SE] [TEST] atm classes: " + listed);
	}

	protected static void RunPure()
	{
		string big = INT_Bank_Util.Money(1234567);
		Check("fmt.money", big == "1 234 567", big);
		CheckAtmClasses();
		Check("fmt.money_small", INT_Bank_Util.Money(500) == "500");
		Check("fmt.money_zero", INT_Bank_Util.Money(0) == "0");
		Check("steam64.ok", INT_Bank_Util.IsSteam64("76561198000000000"));
		Check("steam64.short", !INT_Bank_Util.IsSteam64("7656119800000000"));
		Check("steam64.new_range", INT_Bank_Util.IsSteam64("76561202007535715"));
		Check("steam64.letters", !INT_Bank_Util.IsSteam64("7656119800000000a"));
		Check("amount.parse", INT_Bank_Util.ParseAmount("00 12 500") == 12500);
		Check("amount.too_long", INT_Bank_Util.ParseAmount("1234567890") == -1);

		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		Check("config.live_default", cfg.liveAccountReload == 1);
		Check("config.currencies", cfg.currencies.Count() >= 1);
		int dn = INT_BankSE_Money.Denoms().Count();
		Check("money.denoms_loaded", dn >= 1, "count=" + dn.ToString());

		INT_BankSE_PayPlan plan = new INT_BankSE_PayPlan();
		bool payable = INT_BankSE_Money.PlanPayGreedy(12345, plan);
		int sum = 0;
		foreach (INT_BankSE_PayLine l : plan.Lines)
			sum = sum + l.Value();
		string planText0 = plan.Describe();
		Check("money.greedy_12345", payable && sum == 12345, planText0);
		INT_BankSE_Denom top = INT_BankSE_Money.Denoms().Get(0);
		bool topFirst = false;
		if (plan.Lines.Count() > 0)
		{
			INT_BankSE_PayLine first = plan.Lines.Get(0);
			topFirst = first.Denom == top;
		}
		string planText = plan.Describe();
		Check("money.greedy_top_first", topFirst, planText);

		// accounts through the API
		RemoveFake(FAKE_A);
		RemoveFake(FAKE_B);
		Check("api.no_account_zero", INT_Banking_API.GetBalance(FAKE_A) == 0);
		Check("api.debit_no_account", INT_Banking_API.Move(FAKE_A, -10, "", "t1", "TEST") == INT_Bank_Status.NO_FUNDS);
		Check("api.credit", INT_Banking_API.Move(FAKE_A, 1000, FAKE_B, "t2", "TEST") == INT_Bank_Status.OK);
		INT_BankSE_Accounts.Forget(FAKE_A);
		Check("api.balance_after_credit", INT_Banking_API.GetBalance(FAKE_A) == 1000);
		Check("api.debit", INT_Banking_API.Move(FAKE_A, -400, FAKE_B, "t3", "TEST") == INT_Bank_Status.OK);
		Check("api.balance_after_debit", INT_Banking_API.GetBalance(FAKE_A) == 600);
		Check("api.no_funds", INT_Banking_API.Move(FAKE_A, -601, FAKE_B, "t4", "TEST") == INT_Bank_Status.NO_FUNDS);
		Check("api.haskey", INT_Banking_API.HasKey(FAKE_A, "t3", "TEST"));
		Check("api.haskey_missing", !INT_Banking_API.HasKey(FAKE_A, "t4", "TEST"));
		if (cfg.maxBalance > 0)
			Check("api.limit_balance", INT_Banking_API.Move(FAKE_A, cfg.maxBalance, "", "t5", "TEST") == INT_Bank_Status.LIMIT_BALANCE);
		Check("accounts.exists", INT_BankSE_Accounts.Exists(FAKE_A));
		Check("accounts.not_exists", !INT_BankSE_Accounts.Exists(FAKE_B));

		// a broken file is reported and never overwritten
		string path = INT_BankSE_Paths.AccFile(FAKE_B);
		INT_BankSE_File.WriteText(path, "{ \"balance\": 5, broken");
		INT_BankSE_Accounts.Forget(FAKE_B);
		int st;
		INT_BankSE_AccountData broken = INT_BankSE_Accounts.Get(FAKE_B, true, "x", st);
		string still;
		INT_BankSE_File.ReadText(path, still);
		Check("accounts.broken_detected", !broken && st == INT_Bank_Status.ACCOUNT_BROKEN, "st=" + st.ToString());
		Check("accounts.broken_kept", still.IndexOf("broken") != -1);
		RemoveFake(FAKE_A);
		RemoveFake(FAKE_B);
	}

	// ------------------------------------------------------------------------------------------------
	protected static void ResetCooldowns(string uid)
	{
		INT_BankSE_Session s = INT_BankSE_Service.Session(uid);
		s.LastDeposit = -1000000;
		s.LastWithdraw = -1000000;
		s.LastTransfer = -1000000;
	}

	protected static INT_BankSE_Denom DenomNotHeld(PlayerBase p)
	{
		array<ref INT_BankSE_CashItem> items = new array<ref INT_BankSE_CashItem>();
		INT_BankSE_Money.Collect(p, items);
		array<ref INT_BankSE_Denom> denomList = INT_BankSE_Money.Denoms();
		foreach (INT_BankSE_Denom d : denomList)
		{
			bool held = false;
			foreach (INT_BankSE_CashItem ci : items)
			{
				if (ci.Denom == d)
				{
					held = true;
					break;
				}
			}
			if (!held)
				return d;
		}
		return null;
	}

	protected static void RunLive(PlayerBase p)
	{
		string uid = p.GetIdentity().GetPlainId();
		string rawName = p.GetIdentity().GetName();
		string name = INT_Bank_Util.CleanName(rawName, 32);
		string accPath = INT_BankSE_Paths.AccFile(uid);
		string backup = "";
		bool hadFile = INT_BankSE_File.ReadText(accPath, backup);
		string whoTest = INT_BankSE_Log.Who(name, uid);
		INT_BankSE_Log.Test("живая часть на игроке " + whoTest);

		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		Check("live.account", acc != null, "st=" + st.ToString());
		if (!acc)
			return;
		acc.balance = 0;
		acc.history.Clear();
		INT_BankSE_Accounts.Save(acc);

		// 15 000 in one stack of the top note
		INT_BankSE_Denom top = INT_BankSE_Money.Denoms().Get(0);
		int pieces = 15000 / top.Value;
		EntityAI stack = INT_BankSE_Money.CreateInCargo(p, top.ClassName);
		ItemBase sb = ItemBase.Cast(stack);
		if (sb && sb.HasQuantity())
			sb.SetQuantity(pieces, false);
		int cash0 = INT_BankSE_Money.CountCash(p);
		Check("live.spawn_cash", cash0 >= 15000, "cash=" + cash0.ToString());

		// deposit 12 000 (change expected when only big notes are carried)
		ResetCooldowns(uid);
		INT_BankSE_Out o = new INT_BankSE_Out();
		INT_BankSE_Service.Deposit(p, uid, name, 12000, false, true, o);
		acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		int cash1 = INT_BankSE_Money.CountCash(p);
		Check("live.deposit_ok", o.Status == INT_Bank_Status.OK, "status=" + INT_Bank_Util.StatusName(o.Status));
		Check("live.deposit_balance", acc.balance == 12000, "balance=" + acc.balance.ToString());
		string c0 = cash0.ToString();
		string c1 = cash1.ToString();
		string ob = o.B.ToString();
		Check("live.deposit_cash", cash1 == cash0 - 12000, "cash " + c0 + " -> " + c1 + " change=" + ob);

		// cooldown: withdraw right after a deposit is refused
		INT_BankSE_Out oc = new INT_BankSE_Out();
		INT_BankSE_Service.Withdraw(p, uid, name, 100, false, oc);
		string ocs = INT_Bank_Util.StatusName(oc.Status);
		string oca = oc.A.ToString();
		Check("live.gap_cooldown", oc.Status == INT_Bank_Status.COOLDOWN && oc.A > 0, "status=" + ocs + " a=" + oca);

		// withdraw 7 385
		ResetCooldowns(uid);
		INT_BankSE_Out ow = new INT_BankSE_Out();
		INT_BankSE_Service.Withdraw(p, uid, name, 7385, true, ow);
		acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		int cash2 = INT_BankSE_Money.CountCash(p);
		Check("live.withdraw_ok", ow.Status == INT_Bank_Status.OK, "status=" + INT_Bank_Util.StatusName(ow.Status));
		Check("live.withdraw_balance", acc.balance == 12000 - 7385, "balance=" + acc.balance.ToString());
		string c2 = cash2.ToString();
		// all in the inventory, or (inventory too full) all on the ground - never split
		bool allInv = ow.B == 0 && cash2 == cash1 + 7385;
		bool allGround = ow.B == 7385 && cash2 == cash1;
		string sGround = ow.B.ToString();
		Check("live.withdraw_cash", allInv || allGround, "cash " + c1 + " -> " + c2 + " ground=" + sGround);

		// no funds
		ResetCooldowns(uid);
		INT_BankSE_Out onf = new INT_BankSE_Out();
		INT_BankSE_Service.Withdraw(p, uid, name, 999999, true, onf);
		bool nfOk = onf.Status == INT_Bank_Status.NO_FUNDS || onf.Status == INT_Bank_Status.LIMIT_OP;
		Check("live.withdraw_no_funds", nfOk, "status=" + INT_Bank_Util.StatusName(onf.Status));

		// deposit all
		ResetCooldowns(uid);
		int cashAll = INT_BankSE_Money.CountCash(p);
		acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		int balAll = acc.balance;
		INT_BankSE_Out oa = new INT_BankSE_Out();
		INT_BankSE_Service.Deposit(p, uid, name, 0, true, true, oa);
		acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		Check("live.deposit_all_ok", oa.Status == INT_Bank_Status.OK, "status=" + INT_Bank_Util.StatusName(oa.Status));
		int wantAll = balAll + cashAll;
		string sBalAll = acc.balance.ToString();
		string sWantAll = wantAll.ToString();
		Check("live.deposit_all_balance", acc.balance == wantAll, "balance=" + sBalAll + " want=" + sWantAll);
		int cashLeft = INT_BankSE_Money.CountCash(p);
		Check("live.deposit_all_cash", cashLeft == 0);

		// full inventory: a note the player does not hold needs a new slot
		array<EntityAI> fillers = new array<EntityAI>();
		for (int i = 0; i < 400; i++)
		{
			EntityAI f = INT_BankSE_Money.CreateInCargo(p, "Rag");
			if (!f)
				break;
			fillers.Insert(f);
		}
		// single free cells left between the rags still take a note: close them with a 1x1 item
		for (int i1 = 0; i1 < 400; i1++)
		{
			EntityAI f1 = INT_BankSE_Money.CreateInCargo(p, "Battery9V");
			if (!f1)
				break;
			fillers.Insert(f1);
		}
		INT_BankSE_Denom freeDenom = DenomNotHeld(p);
		if (freeDenom && acc.balance >= freeDenom.Value)
		{
			ResetCooldowns(uid);
			INT_BankSE_Out og = new INT_BankSE_Out();
			INT_BankSE_Service.Withdraw(p, uid, name, freeDenom.Value, false, og);
			string ogs = INT_Bank_Util.StatusName(og.Status);
			string oga = og.A.ToString();
			string ogb = og.B.ToString();
			Check("live.full_need_ground", og.Status == INT_Bank_Status.NEED_GROUND && og.A == 0 && og.B == freeDenom.Value, "status=" + ogs + " fit=" + oga + " total=" + ogb);
			acc = INT_BankSE_Accounts.Get(uid, true, name, st);
			int balBefore = acc.balance;
			Check("live.full_nothing_changed", balBefore == wantAll, "balance=" + balBefore.ToString());
			ResetCooldowns(uid);
			INT_BankSE_Out og2 = new INT_BankSE_Out();
			INT_BankSE_Service.Withdraw(p, uid, name, freeDenom.Value, true, og2);
			acc = INT_BankSE_Accounts.Get(uid, true, name, st);
			string og2s = INT_Bank_Util.StatusName(og2.Status);
			string og2b = og2.B.ToString();
			Check("live.full_ground_ok", og2.Status == INT_Bank_Status.OK && og2.B == freeDenom.Value, "status=" + og2s + " ground=" + og2b);
			Check("live.full_ground_balance", acc.balance == balBefore - freeDenom.Value, "balance=" + acc.balance.ToString());
			// pick the dropped stack up again (delete it)
			array<Object> near = new array<Object>();
			GetGame().GetObjectsAtPosition(p.GetPosition(), 3.0, near, null);
			int removedGround = 0;
			foreach (Object no : near)
			{
				EntityAI ne = EntityAI.Cast(no);
				if (!ne || ne.GetHierarchyParent() || ne.GetType() != freeDenom.ClassName)
					continue;
				GetGame().ObjectDelete(ne);
				removedGround++;
			}
			Check("live.full_ground_found", removedGround >= 1, "found=" + removedGround.ToString());
		}
		else
		{
			Check("live.full_skipped", true);
		}
		foreach (EntityAI fe : fillers)
		{
			if (fe)
				GetGame().ObjectDelete(fe);
		}

		// transfer to a fake account
		RemoveFake(FAKE_A);
		INT_Banking_API.Move(FAKE_A, 1, "", "seed", "TEST");
		ResetCooldowns(uid);
		INT_BankSE_Out ot = new INT_BankSE_Out();
		int balT = acc.balance;
		INT_BankSE_Service.Transfer(p, uid, name, FAKE_A, 500, ot);
		acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		Check("live.transfer_ok", ot.Status == INT_Bank_Status.OK, "status=" + INT_Bank_Util.StatusName(ot.Status));
		Check("live.transfer_sender", acc.balance == balT - 500, "balance=" + acc.balance.ToString());
		int fakeBal = INT_Banking_API.GetBalance(FAKE_A);
		Check("live.transfer_recipient", fakeBal == 501);
		INT_BankSE_Out ots = new INT_BankSE_Out();
		ResetCooldowns(uid);
		INT_BankSE_Service.Transfer(p, uid, name, uid, 1, ots);
		Check("live.transfer_self", ots.Status == INT_Bank_Status.TARGET_SELF);
		INT_BankSE_Out otn = new INT_BankSE_Out();
		INT_BankSE_Service.Transfer(p, uid, name, FAKE_B, 1, otn);
		Check("live.transfer_unknown", otn.Status == INT_Bank_Status.TARGET_NOT_FOUND, "status=" + INT_Bank_Util.StatusName(otn.Status));
		int histCount = acc.history.Count();
		Check("live.history_written", histCount >= 4, "count=" + histCount.ToString());
		RemoveFake(FAKE_A);
		RemoveFake(FAKE_B);

		// restore the player's account
		INT_BankSE_Accounts.Forget(uid);
		if (hadFile)
			INT_BankSE_File.SaveText(accPath, backup);
		else
			RemoveFake(uid);
		ResetCooldowns(uid);
		INT_BankSE_Log.Test("счёт игрока восстановлен из копии");
	}
}
