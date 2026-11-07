// INT Banking (server) - the bank operations. Every operation runs completely inside one call
// (one server frame): validate -> plan -> change the world -> write the account -> log -> answer.
//
// Order of changes (money is never created twice):
//   withdraw: balance - amount written FIRST, then the notes are given; whatever could not be given
//             goes back to the balance (logged as an error).
//   deposit:  the notes are removed FIRST, then the balance is written; when the write fails the
//             removed value is given back in cash. Change that cannot be given in cash is credited.
//   transfer: sender debited and written, then the recipient credited and written; when the second
//             write fails the sender is refunded.
//
// Code style note: call results are stored in locals before they are combined (several call results in
// one expression or argument list can be overwritten - INT_Trader, 2026-10-12).

// Answer of one operation (fields as in INT_Bank_Op / INT_Bank_Status).
class INT_BankSE_Out
{
	int Status;
	int A;
	int B;
	int C;
	int D;
	string S1;
	string S2;

	void Set(int status, int a = 0, int b = 0, int c = 0)
	{
		Status = status;
		A = a;
		B = b;
		C = c;
	}
}

// Per-player session data (kept for the whole server run, keyed by Steam64).
class INT_BankSE_Session
{
	int LastDeposit = -1000000;		// mission time, ms
	int LastWithdraw = -1000000;
	int LastTransfer = -1000000;
	int LastMutation = -1000000;
	int ReadWindowStart = -1000000;
	int ReadCount;
}

class INT_BankSE_Service
{
	static const int MUTATION_GAP_MS = 400;
	// read-only requests (state, history, lookup): at most READ_MAX per READ_WINDOW_MS
	static const int READ_WINDOW_MS = 2000;
	static const int READ_MAX = 12;

	protected static ref map<string, ref INT_BankSE_Session> s_Sessions;

	static void Init()
	{
		s_Sessions = new map<string, ref INT_BankSE_Session>();
	}

	static INT_BankSE_Session Session(string uid)
	{
		if (!s_Sessions)
			Init();
		INT_BankSE_Session s;
		if (!s_Sessions.Find(uid, s))
		{
			s = new INT_BankSE_Session();
			s_Sessions.Set(uid, s);
		}
		return s;
	}

	static int NowMs()
	{
		return GetGame().GetTime();
	}

	// Seconds left of a cooldown that started at 'since' (ms) and lasts 'sec' seconds.
	static int Left(int since, int sec)
	{
		if (sec <= 0)
			return 0;
		int end = since + sec * 1000;
		int now = NowMs();
		int left = end - now;
		if (left <= 0)
			return 0;
		return (left + 999) / 1000;
	}

	static int DepositCooldown(string uid)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		INT_BankSE_Session s = Session(uid);
		int a = Left(s.LastDeposit, cfg.depositCooldownSec);
		int b = Left(s.LastWithdraw, cfg.depositWithdrawGapSec);
		if (b > a)
			return b;
		return a;
	}

	static int WithdrawCooldown(string uid)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		INT_BankSE_Session s = Session(uid);
		int a = Left(s.LastWithdraw, cfg.withdrawCooldownSec);
		int b = Left(s.LastDeposit, cfg.depositWithdrawGapSec);
		if (b > a)
			return b;
		return a;
	}

	static int TransferCooldown(string uid)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		INT_BankSE_Session s = Session(uid);
		return Left(s.LastTransfer, cfg.transferCooldownSec);
	}

	// Request spam guard. false = too fast.
	static bool AllowRequest(string uid, bool mutation)
	{
		INT_BankSE_Session s = Session(uid);
		int now = NowMs();
		if (mutation)
		{
			if (now - s.LastMutation < MUTATION_GAP_MS)
				return false;
			s.LastMutation = now;
			return true;
		}
		if (now - s.ReadWindowStart > READ_WINDOW_MS)
		{
			s.ReadWindowStart = now;
			s.ReadCount = 0;
		}
		if (s.ReadCount >= READ_MAX)
			return false;
		s.ReadCount++;
		return true;
	}

	// ------------------------------------------------------------------------------------------------
	// Checks
	// ------------------------------------------------------------------------------------------------
	static int CheckPlayer(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return INT_Bank_Status.PLAYER_STATE;
		if (!player.IsAlive())
			return INT_Bank_Status.PLAYER_STATE;
		if (player.IsUnconscious())
			return INT_Bank_Status.PLAYER_STATE;
		if (player.IsRestrained())
			return INT_Bank_Status.PLAYER_STATE;
		if (!NearAtm(player))
			return INT_Bank_Status.NOT_NEAR;
		return INT_Bank_Status.OK;
	}

	static bool NearAtm(PlayerBase player)
	{
		float dist = INT_BankSE_Config.Get().atmUseDistance;
		if (dist <= 0)
			return true;
		vector pos = player.GetPosition();
		array<Object> objects = new array<Object>();
		GetGame().GetObjectsAtPosition(pos, dist, objects, null);
		foreach (Object o : objects)
		{
			if (MF_BankObjectBase.Cast(o))
				return true;
		}
		return false;
	}

	static void FillState(PlayerBase player, string uid, INT_BankSE_AccountData acc, INT_Bank_State st)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		st.Balance = 0;
		st.HistoryCount = 0;
		if (acc)
		{
			st.Balance = acc.balance;
			if (acc.history)
				st.HistoryCount = acc.history.Count();
		}
		st.Cash = INT_BankSE_Money.CountCash(player);
		st.MaxBalance = cfg.maxBalance;
		st.MaxDeposit = cfg.maxDepositPerOperation;
		st.MaxWithdraw = cfg.maxWithdrawPerOperation;
		st.MaxTransfer = cfg.maxTransferPerOperation;
		st.CdDeposit = DepositCooldown(uid);
		st.CdWithdraw = WithdrawCooldown(uid);
		st.CdTransfer = TransferCooldown(uid);
	}

	// "<kind> | Игрок: <who> | <details> | ОТКАЗ: <reason>" (only when logRefusals = 1)
	protected static void Refuse(string kind, string who, string details, string reason)
	{
		if (INT_BankSE_Config.Get().logRefusals == 0)
			return;
		string msg = "Игрок: " + who;
		if (details != "")
			msg = msg + " | " + details;
		msg = msg + " | ОТКАЗ: " + reason;
		INT_BankSE_Log.Op(kind, msg);
	}

	protected static string M(int v)
	{
		return INT_Bank_Util.Money(v);
	}

	// ------------------------------------------------------------------------------------------------
	// HELLO
	// ------------------------------------------------------------------------------------------------
	static INT_BankSE_AccountData Hello(PlayerBase player, string uid, string name, INT_BankSE_Out o)
	{
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		o.Set(st);
		if (!acc)
			return null;
		// remember the nick for people who send money here (one write per open at most)
		string clean = INT_Bank_Util.CleanName(name, 32);
		if (clean != "" && acc.name != clean)
		{
			acc.name = clean;
			INT_BankSE_Accounts.Save(acc);
		}
		return acc;
	}

	// ------------------------------------------------------------------------------------------------
	// DEPOSIT
	// ------------------------------------------------------------------------------------------------
	static INT_BankSE_AccountData Deposit(PlayerBase player, string uid, string name, int amountIn, bool all, bool allowGround, INT_BankSE_Out o)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		string who = INT_BankSE_Log.Who(name, uid);
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		if (!acc)
		{
			o.Set(st);
			return null;
		}
		int cd = DepositCooldown(uid);
		if (cd > 0)
		{
			o.Set(INT_Bank_Status.COOLDOWN, cd);
			return acc;
		}
		int cash = INT_BankSE_Money.CountCash(player);
		int amount = amountIn;
		int room = int.MAX;
		if (cfg.maxBalance > 0)
			room = cfg.maxBalance - acc.balance;
		string sMaxBal = M(cfg.maxBalance);
		if (all)
		{
			amount = cash;
			if (cfg.maxDepositPerOperation > 0 && amount > cfg.maxDepositPerOperation)
				amount = cfg.maxDepositPerOperation;
			if (amount > room)
				amount = room;
			if (cash <= 0)
			{
				o.Set(INT_Bank_Status.NO_CASH);
				Refuse("ВНЕСЕНИЕ", who, "Внести всё", "нет наличных в инвентаре");
				return acc;
			}
			if (amount <= 0)
			{
				o.Set(INT_Bank_Status.LIMIT_BALANCE, cfg.maxBalance);
				Refuse("ВНЕСЕНИЕ", who, "Внести всё", "достигнут максимальный баланс " + sMaxBal);
				return acc;
			}
		}
		if (amount <= 0)
		{
			o.Set(INT_Bank_Status.BAD_INPUT);
			return acc;
		}
		string sAmount = M(amount);
		string dAmount = "Сумма: " + sAmount;
		if (cfg.maxDepositPerOperation > 0 && amount > cfg.maxDepositPerOperation)
		{
			o.Set(INT_Bank_Status.LIMIT_OP, cfg.maxDepositPerOperation);
			string sMaxDep = M(cfg.maxDepositPerOperation);
			Refuse("ВНЕСЕНИЕ", who, dAmount, "превышен лимит за операцию " + sMaxDep);
			return acc;
		}
		if (amount > room)
		{
			o.Set(INT_Bank_Status.LIMIT_BALANCE, cfg.maxBalance);
			string sBalNow = M(acc.balance);
			string rRoom = "будет превышен максимальный баланс " + sMaxBal + " (сейчас " + sBalNow + ")";
			Refuse("ВНЕСЕНИЕ", who, dAmount, rRoom);
			return acc;
		}
		if (cash < amount)
		{
			o.Set(INT_Bank_Status.NO_CASH, cash);
			string sCash = M(cash);
			Refuse("ВНЕСЕНИЕ", who, dAmount, "недостаточно наличных (при себе " + sCash + ")");
			return acc;
		}

		INT_BankSE_ChargePlan plan = new INT_BankSE_ChargePlan();
		if (!INT_BankSE_Money.PlanCharge(player, amount, plan))
		{
			o.Set(INT_Bank_Status.NOT_PAYABLE);
			Refuse("ВНЕСЕНИЕ", who, dAmount, "нельзя собрать сумму из купюр игрока");
			return acc;
		}

		// change: plan it and check it fits before anything is touched
		INT_BankSE_PayPlan change = null;
		bool changeGround = false;
		if (plan.Change > 0)
		{
			change = new INT_BankSE_PayPlan();
			if (INT_BankSE_Money.PlanPay(player, plan.Change, change))
			{
				if (change.FitValue < plan.Change)
				{
					if (!allowGround)
					{
						o.Set(INT_Bank_Status.NEED_GROUND, change.FitValue, plan.Change, plan.Change);
						return acc;
					}
					changeGround = true;
				}
			}
			else
			{
				change = null;	// not payable in notes: the change is credited to the account
			}
		}

		// 1. take the notes
		int before = acc.balance;
		string notesTaken = plan.Describe();
		int removed = INT_BankSE_Money.ExecuteCharge(plan);
		if (removed <= 0)
		{
			o.Set(INT_Bank_Status.INTERNAL);
			INT_BankSE_Log.Op("ВНЕСЕНИЕ", "Игрок: " + who + " | " + dAmount + " | ОШИБКА: купюры не удалось забрать из инвентаря, счёт не изменён");
			return acc;
		}
		int credit = amount;
		int giveBack = plan.Change;
		string note = "";
		if (removed != plan.Taken)
		{
			// something changed under the plan: credit exactly what was taken, no change
			credit = removed;
			giveBack = 0;
			change = null;
			string sRemoved = M(removed);
			string sTaken = M(plan.Taken);
			note = "забрано " + sRemoved + " вместо " + sTaken + ", зачислено всё забранное, сдача не выдавалась";
		}

		// 2. write the balance
		acc.balance = INT_BankSE_Money.AddSat(acc.balance, credit);
		acc.AddHistory(INT_Bank_Hist.DEPOSIT, credit, "", "", "");
		if (!INT_BankSE_Accounts.Save(acc))
		{
			acc.balance = before;
			INT_BankSE_Accounts.Forget(uid);
			// put the cash back
			INT_BankSE_PayPlan back = new INT_BankSE_PayPlan();
			INT_BankSE_PayResult br = new INT_BankSE_PayResult();
			string backInfo = "не удалось";
			if (INT_BankSE_Money.PlanPay(player, removed, back))
			{
				bool backGround = back.FitValue < removed;
				INT_BankSE_Money.Deliver(player, back, backGround, br);
				string bInv = M(br.ToInventory);
				string bGround = M(br.ToGround);
				string bFail = M(br.Failed);
				backInfo = "в инвентарь " + bInv + ", на землю " + bGround + ", не выдано " + bFail;
			}
			o.Set(INT_Bank_Status.STORAGE);
			INT_BankSE_Log.Op("ВНЕСЕНИЕ", "Игрок: " + who + " | " + dAmount + " | ОШИБКА: файл счёта не записан, баланс не изменён | Возврат наличных: " + backInfo);
			return null;
		}
		INT_BankSE_Session sess = Session(uid);
		sess.LastDeposit = NowMs();

		// 3. change
		string changeInfo = "нет";
		int changeOnGround = 0;
		int changeCredited = 0;
		if (giveBack > 0)
		{
			INT_BankSE_PayResult cr = new INT_BankSE_PayResult();
			if (change)
			{
				INT_BankSE_Money.Deliver(player, change, changeGround, cr);
				changeCredited = cr.Failed;
			}
			else
			{
				changeCredited = giveBack;
			}
			changeOnGround = cr.ToGround;
			string cBack = M(giveBack);
			string cInv = M(cr.ToInventory);
			string cGround = M(cr.ToGround);
			changeInfo = cBack + " (в инвентарь " + cInv + ", на землю " + cGround;
			if (cr.GroundPos != "")
				changeInfo = changeInfo + " у " + cr.GroundPos;
			changeInfo = changeInfo + ")";
			if (changeCredited > 0)
			{
				acc.balance = INT_BankSE_Money.AddSat(acc.balance, changeCredited);
				acc.AddHistory(INT_Bank_Hist.DEPOSIT, changeCredited, "", "", "change");
				string cCred = M(changeCredited);
				if (INT_BankSE_Accounts.Save(acc))
					changeInfo = changeInfo + ", не выдано наличными " + cCred + " - зачислено на счёт";
				else
					changeInfo = changeInfo + ", не выдано наличными " + cCred + " - ЗАЧИСЛИТЬ НЕ УДАЛОСЬ (файл не записан)";
				if (cr.Error != "")
					changeInfo = changeInfo + " [" + cr.Error + "]";
			}
		}

		o.Set(INT_Bank_Status.OK, credit, giveBack, changeOnGround);
		o.D = changeCredited;
		string sCredit = M(credit);
		string sBefore = M(before);
		string sAfter = M(acc.balance);
		string sPos = INT_BankSE_Log.Pos(player.GetPosition());
		string line = "Игрок: " + who + " | Внесено: " + sCredit + " | Баланс: " + sBefore + " -> " + sAfter;
		line = line + " | Принято купюр: " + notesTaken;
		line = line + " | Сдача: " + changeInfo;
		line = line + " | Позиция: " + sPos;
		if (note != "")
			line = line + " | ВНИМАНИЕ: " + note;
		line = line + " | УСПЕШНО";
		INT_BankSE_Log.Op("ВНЕСЕНИЕ", line);
		return acc;
	}

	// ------------------------------------------------------------------------------------------------
	// WITHDRAW
	// ------------------------------------------------------------------------------------------------
	static INT_BankSE_AccountData Withdraw(PlayerBase player, string uid, string name, int amount, bool allowGround, INT_BankSE_Out o)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		string who = INT_BankSE_Log.Who(name, uid);
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		if (!acc)
		{
			o.Set(st);
			return null;
		}
		if (amount <= 0)
		{
			o.Set(INT_Bank_Status.BAD_INPUT);
			return acc;
		}
		int cd = WithdrawCooldown(uid);
		if (cd > 0)
		{
			o.Set(INT_Bank_Status.COOLDOWN, cd);
			return acc;
		}
		string sAmount = M(amount);
		string dAmount = "Сумма: " + sAmount;
		if (cfg.maxWithdrawPerOperation > 0 && amount > cfg.maxWithdrawPerOperation)
		{
			o.Set(INT_Bank_Status.LIMIT_OP, cfg.maxWithdrawPerOperation);
			string sMax = M(cfg.maxWithdrawPerOperation);
			Refuse("СНЯТИЕ", who, dAmount, "превышен лимит за операцию " + sMax);
			return acc;
		}
		if (acc.balance < amount)
		{
			o.Set(INT_Bank_Status.NO_FUNDS, acc.balance);
			string sBal = M(acc.balance);
			Refuse("СНЯТИЕ", who, dAmount, "недостаточно средств на счёте (баланс " + sBal + ")");
			return acc;
		}
		INT_BankSE_PayPlan plan = new INT_BankSE_PayPlan();
		if (!INT_BankSE_Money.PlanPay(player, amount, plan))
		{
			o.Set(INT_Bank_Status.NOT_PAYABLE);
			Refuse("СНЯТИЕ", who, dAmount, "сумму нельзя выдать имеющимися номиналами");
			return acc;
		}
		if (plan.FitValue < amount && !allowGround)
		{
			// nothing changed: the client asks "it does not fit - give everything on the ground?"
			o.Set(INT_Bank_Status.NEED_GROUND, plan.FitValue, amount, amount);
			return acc;
		}

		// 1. debit and write
		int before = acc.balance;
		acc.balance = acc.balance - amount;
		acc.AddHistory(INT_Bank_Hist.WITHDRAW, amount, "", "", "");
		if (!INT_BankSE_Accounts.Save(acc))
		{
			acc.balance = before;
			INT_BankSE_Accounts.Forget(uid);
			o.Set(INT_Bank_Status.STORAGE);
			INT_BankSE_Log.Op("СНЯТИЕ", "Игрок: " + who + " | " + dAmount + " | ОШИБКА: файл счёта не записан, деньги не выдавались, баланс не изменён");
			return null;
		}
		INT_BankSE_Session sess = Session(uid);
		sess.LastWithdraw = NowMs();

		// 2. give the notes: all into the inventory, or (it does not all fit, the player agreed) all on the ground
		bool groundOnly = plan.FitValue < amount;
		string notes = plan.Describe();
		INT_BankSE_PayResult res = new INT_BankSE_PayResult();
		INT_BankSE_Money.Deliver(player, plan, groundOnly, res);
		string gInv = M(res.ToInventory);
		string gGround = M(res.ToGround);
		string given = "в инвентарь " + gInv + ", на землю " + gGround;
		if (res.ToGround > 0)
		{
			string stacks = res.GroundStacks.ToString();
			given = given + " (" + stacks + " пачк. у " + res.GroundPos + ", в инвентарь всё не помещалось - выдано на землю с согласия игрока)";
		}
		string sBefore = M(before);
		string sPos = INT_BankSE_Log.Pos(player.GetPosition());
		string line = "Игрок: " + who + " | Снято: " + sAmount + " | Купюры: " + notes + " | Выдано: " + given;

		// 3. refund what could not be given
		if (res.Failed > 0)
		{
			acc.balance = INT_BankSE_Money.AddSat(acc.balance, res.Failed);
			acc.AddHistory(INT_Bank_Hist.DEPOSIT, res.Failed, "", "", "refund");
			bool saved = INT_BankSE_Accounts.Save(acc);
			string sAfterR = M(acc.balance);
			string sFailed = M(res.Failed);
			line = line + " | Баланс: " + sBefore + " -> " + sAfterR;
			line = line + " | ОШИБКА ВЫДАЧИ: " + res.Error + "; не выдано " + sFailed;
			if (saved)
				line = line + " - возвращено на счёт";
			else
				line = line + " - ВОЗВРАТ НА СЧЁТ НЕ ЗАПИСАН (проверьте вручную)";
			line = line + " | Позиция: " + sPos;
			INT_BankSE_Log.Op("СНЯТИЕ", line);
			int paid = amount - res.Failed;
			o.Set(INT_Bank_Status.OK, paid, res.ToGround, res.Failed);
			return acc;
		}
		string sAfter = M(acc.balance);
		line = line + " | Баланс: " + sBefore + " -> " + sAfter;
		line = line + " | Позиция: " + sPos + " | УСПЕШНО";
		INT_BankSE_Log.Op("СНЯТИЕ", line);
		o.Set(INT_Bank_Status.OK, amount, res.ToGround, 0);
		return acc;
	}

	// ------------------------------------------------------------------------------------------------
	// LOOKUP / TRANSFER
	// ------------------------------------------------------------------------------------------------
	static void Lookup(string uid, string target, INT_BankSE_Out o)
	{
		o.S2 = target;
		if (!INT_Bank_Util.IsSteam64(target))
		{
			o.Set(INT_Bank_Status.TARGET_INVALID);
			return;
		}
		if (target == uid)
		{
			o.Set(INT_Bank_Status.TARGET_SELF);
			return;
		}
		if (!INT_BankSE_Accounts.Exists(target))
		{
			o.Set(INT_Bank_Status.TARGET_NOT_FOUND);
			return;
		}
		// the name saved in the account only: a live (online) name would tell the sender who is online
		string name = "";
		int st;
		INT_BankSE_AccountData t = INT_BankSE_Accounts.Get(target, false, "", st);
		if (t)
			name = t.name;
		else if (st == INT_Bank_Status.ACCOUNT_BROKEN)
		{
			o.Set(INT_Bank_Status.ACCOUNT_BROKEN);
			return;
		}
		o.Set(INT_Bank_Status.OK, 1);
		o.S1 = name;
	}

	static INT_BankSE_AccountData Transfer(PlayerBase player, string uid, string name, string target, int amount, INT_BankSE_Out o)
	{
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		string who = INT_BankSE_Log.Who(name, uid);
		int st;
		INT_BankSE_AccountData acc = INT_BankSE_Accounts.Get(uid, true, name, st);
		if (!acc)
		{
			o.Set(st);
			return null;
		}
		o.S2 = target;
		if (amount <= 0)
		{
			o.Set(INT_Bank_Status.BAD_INPUT);
			return acc;
		}
		if (!INT_Bank_Util.IsSteam64(target))
		{
			o.Set(INT_Bank_Status.TARGET_INVALID);
			return acc;
		}
		if (target == uid)
		{
			o.Set(INT_Bank_Status.TARGET_SELF);
			return acc;
		}
		int cd = TransferCooldown(uid);
		if (cd > 0)
		{
			o.Set(INT_Bank_Status.COOLDOWN, cd);
			return acc;
		}
		string sAmount = M(amount);
		string dTo = "Кому: " + target + " | Сумма: " + sAmount;
		if (cfg.maxTransferPerOperation > 0 && amount > cfg.maxTransferPerOperation)
		{
			o.Set(INT_Bank_Status.LIMIT_OP, cfg.maxTransferPerOperation);
			string sMax = M(cfg.maxTransferPerOperation);
			Refuse("ПЕРЕВОД", who, dTo, "превышен лимит за операцию " + sMax);
			return acc;
		}
		if (acc.balance < amount)
		{
			o.Set(INT_Bank_Status.NO_FUNDS, acc.balance);
			string sBal = M(acc.balance);
			Refuse("ПЕРЕВОД", who, dTo, "недостаточно средств на счёте (баланс " + sBal + ")");
			return acc;
		}
		int tst;
		INT_BankSE_AccountData dst = INT_BankSE_Accounts.Get(target, false, "", tst);
		if (!dst)
		{
			if (tst == INT_Bank_Status.ACCOUNT_BROKEN)
				o.Set(INT_Bank_Status.ACCOUNT_BROKEN);
			else
				o.Set(INT_Bank_Status.TARGET_NOT_FOUND);
			Refuse("ПЕРЕВОД", who, dTo, "счёт получателя не найден или повреждён");
			return acc;
		}
		// saved account name (see Lookup); the admin log identifies the recipient by Steam64 anyway
		string targetName = dst.name;
		string whom = INT_BankSE_Log.Who(targetName, target);
		string dWhom = "Кому: " + whom + " | Сумма: " + sAmount;
		if (cfg.maxBalance > 0 && dst.balance > cfg.maxBalance - amount)
		{
			o.Set(INT_Bank_Status.TARGET_LIMIT, cfg.maxBalance);
			Refuse("ПЕРЕВОД", who, dWhom, "у получателя будет превышен максимальный баланс");
			return acc;
		}

		// 1. sender
		int senderBefore = acc.balance;
		acc.balance = acc.balance - amount;
		acc.AddHistory(INT_Bank_Hist.TRANSFER_OUT, amount, target, targetName, "");
		if (!INT_BankSE_Accounts.Save(acc))
		{
			acc.balance = senderBefore;
			INT_BankSE_Accounts.Forget(uid);
			o.Set(INT_Bank_Status.STORAGE);
			INT_BankSE_Log.Op("ПЕРЕВОД", "От: " + who + " | " + dWhom + " | ОШИБКА: файл счёта отправителя не записан, перевод не выполнен");
			return null;
		}
		// 2. recipient
		int targetBefore = dst.balance;
		dst.balance = dst.balance + amount;
		dst.AddHistory(INT_Bank_Hist.TRANSFER_IN, amount, uid, name, "");
		if (!INT_BankSE_Accounts.Save(dst))
		{
			dst.balance = targetBefore;
			INT_BankSE_Accounts.Forget(target);
			acc.balance = senderBefore;
			acc.AddHistory(INT_Bank_Hist.TRANSFER_IN, amount, target, targetName, "refund");
			string refund = "возвращено отправителю";
			if (!INT_BankSE_Accounts.Save(acc))
			{
				string sSB = M(senderBefore);
				refund = "ВОЗВРАТ ОТПРАВИТЕЛЮ НЕ ЗАПИСАН (проверьте вручную, баланс отправителя должен быть " + sSB + ")";
			}
			o.Set(INT_Bank_Status.STORAGE);
			INT_BankSE_Log.Op("ПЕРЕВОД", "От: " + who + " | " + dWhom + " | ОШИБКА: файл счёта получателя не записан - " + refund);
			return acc;
		}
		INT_BankSE_Session sess = Session(uid);
		sess.LastTransfer = NowMs();
		string sSB1 = M(senderBefore);
		string sSA1 = M(acc.balance);
		string sTB1 = M(targetBefore);
		string sTA1 = M(dst.balance);
		string line = "От: " + who + " | " + dWhom;
		line = line + " | Баланс отправителя: " + sSB1 + " -> " + sSA1;
		line = line + " | Баланс получателя: " + sTB1 + " -> " + sTA1 + " | УСПЕШНО";
		INT_BankSE_Log.Op("ПЕРЕВОД", line);
		o.Set(INT_Bank_Status.OK, amount);
		o.S1 = targetName;
		NotifyRecipient(target, amount, name, uid, dst.balance);
		return acc;
	}

	static PlayerBase FindOnline(string uid)
	{
		if (uid == "")
			return null;
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man m : players)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (!p || !p.GetIdentity())
				continue;
			string pid = p.GetIdentity().GetPlainId();
			if (pid == uid)
				return p;
		}
		return null;
	}

	static string OnlineName(string uid)
	{
		PlayerBase p = FindOnline(uid);
		if (!p)
			return "";
		string raw = p.GetIdentity().GetName();
		return INT_Bank_Util.CleanName(raw, 32);
	}

	protected static void NotifyRecipient(string target, int amount, string fromName, string fromUid, int newBalance)
	{
		if (INT_BankSE_Config.Get().notifyRecipient == 0)
			return;
		PlayerBase p = FindOnline(target);
		if (!p)
			return;
		INT_BankSE_Out o = new INT_BankSE_Out();
		o.Set(INT_Bank_Status.OK, amount, newBalance);
		o.S1 = INT_Bank_Util.CleanName(fromName, 32);
		o.S2 = fromUid;
		INT_BankSE_Server.Send(p, INT_Bank_Op.PUSH_TRANSFER_IN, 0, o, null, null);
	}

	// ------------------------------------------------------------------------------------------------
	// HISTORY
	// ------------------------------------------------------------------------------------------------
	static void ClearHistory(string uid, string name, INT_BankSE_AccountData acc, INT_BankSE_Out o)
	{
		int count = acc.history.Count();
		acc.history.Clear();
		if (!INT_BankSE_Accounts.Save(acc))
		{
			INT_BankSE_Accounts.Forget(uid);
			o.Set(INT_Bank_Status.STORAGE);
			return;
		}
		string who = INT_BankSE_Log.Who(name, uid);
		string sCount = count.ToString();
		INT_BankSE_Log.Op("ИСТОРИЯ", "Игрок: " + who + " очистил историю операций (" + sCount + " записей)");
		o.Set(INT_Bank_Status.OK);
	}
}
