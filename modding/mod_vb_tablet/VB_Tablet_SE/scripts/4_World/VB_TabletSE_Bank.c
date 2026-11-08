// Склад клана в банкоматах INT_Banking. Своего банкомата у клана нет: окно банкомата банка с аддоном
// VB_Tablet_BANK шлёт сюда операции склада, а сервер проверяет, что рядом стоит банкомат банка.
// Каждая операция выполняется целиком за один вызов на сервере. Порядок денег как в INT_BankSE_Service:
//   Снятие: склад уменьшен и записан, потом выдача купюр; не поместилось, склад возвращается.
//   Пополнение: купюры изъяты, потом склад записан; запись не удалась, купюры возвращаются.
//   Со счёта на склад: счёт списан через INT_Banking_API, потом склад записан; не записался, счёт возвращается.
//   Со склада на счёт: склад уменьшен и записан, потом зачисление на счёт; банк отказал, склад возвращается.
// Сверх того двойной клик отсекает пауза между операциями одного игрока.
class VB_TabletBank
{
	static const int GAP_MS = 400;
	// Источник перевода в истории счёта и логе банка. «Второй участник» VB_BANK_PEER + тег клана.
	static const string BANK_SOURCE = "CLAN";

	protected static ref map<string, int> s_LastOp = new map<string, int>();
	protected static int s_TxnNo;

	// Операции 40..59 приходят сюда из VB_TabletClanOps.Handle.
	static void Handle(PlayerBase player, PlayerIdentity id, int op, ParamsReadContext ctx)
	{
		int n;
		string code;
		switch (op)
		{
			case VB_TabletOp.ATM_DEPOSIT:
				if (ctx.Read(n))
					Deposit(player, id, n);
				break;
			case VB_TabletOp.ATM_WITHDRAW:
				if (ctx.Read(n))
					Withdraw(player, id, n);
				break;
			case VB_TabletOp.ATM_FROM_ACCOUNT:
				if (ctx.Read(n))
					FromAccount(player, id, n);
				break;
			case VB_TabletOp.ATM_TO_ACCOUNT:
				if (ctx.Read(n))
					ToAccount(player, id, n);
				break;
			case VB_TabletOp.CONTRACTS:
				VB_TabletContracts.Send(player, id);
				break;
			case VB_TabletOp.CONTRACT_CLAIM:
				if (ctx.Read(n) && ctx.Read(code))
					VB_TabletContracts.Claim(player, id, n, code);
				break;
			case VB_TabletOp.CONTRACT_REPLACE:
				if (ctx.Read(n) && ctx.Read(code))
					VB_TabletContracts.Replace(player, id, n, code);
				break;
			default:
				VB_TabletAdmin.Handle(player, id, op, ctx);
				break;
		}
	}

	static void Forget(string uid)
	{
		s_LastOp.Remove(uid);
	}

	// Пауза между изменяющими операциями одного игрока.
	static bool Allow(string uid)
	{
		int now = GetGame().GetTime();
		int last;
		if (s_LastOp.Find(uid, last) && now - last < GAP_MS)
			return false;
		s_LastOp.Set(uid, now);
		return true;
	}

	static bool CanAct(PlayerBase player)
	{
		if (!player || !player.GetIdentity() || !player.IsAlive())
			return false;
		return !player.IsUnconscious() && !player.IsRestrained();
	}

	// Сообщение поверх игры. arg подставляется вместо {arg}.
	static void Notify(PlayerBase player, PlayerIdentity id, string key, string arg, int kind)
	{
		int op = VB_TabletOp.NOTIFY;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(key);
		rpc.Write(arg);
		rpc.Write(kind);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	static void NotifyClan(VB_Clan c, string key, string arg, int kind)
	{
		foreach (VB_ClanMember m : c.Members)
		{
			if (!m.Online)
				continue;
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (p)
				Notify(p, p.GetIdentity(), key, arg, kind);
		}
	}

	// Клиент прислал HELLO: это и есть вход в игру. Предупреждение тем, кто работает со складом.
	static void OnHello(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.BANK))
			return;
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		if (cfg.ClanTax <= 0)
			return;
		int days = c.TaxFree + c.Bank / cfg.ClanTax;
		if (days <= cfg.TaxWarnDays)
			Notify(player, id, "#STR_VB_TAB_C_TAX_WARN", days.ToString(), 2);
	}

	// Рядом банкомат банка. Классы банкоматов в конфиге наследуют HouseNoDestruct напрямую, поэтому проверка
	// по скриптовому классу MF_BankObjectBase и с расстоянием из конфига банка, как в INT_BankSE_Service.NearAtm.
	// Без серверной части банка банкоматов нет: склад только смотрят в планшете.
	static bool NearAtm(PlayerBase player)
	{
#ifdef INT_BANKING_SE
		float dist = INT_BankSE_Config.Get().atmUseDistance;
		if (dist <= 0)
			return true;
		array<Object> objects = new array<Object>();
		GetGame().GetObjectsAtPosition(player.GetPosition(), dist, objects, null);
		foreach (Object o : objects)
		{
			if (MF_BankObjectBase.Cast(o))
				return true;
		}
#endif
		return false;
	}

	// Проверки до любых изменений: игрок, право ранга на склад, банкомат рядом, сумма.
	protected static int Check(PlayerBase player, string uid, int amount, out VB_Clan c)
	{
		if (!Allow(uid))
			return VB_TabletResult.RATE;
		c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.BANK) || !CanAct(player))
			return VB_TabletResult.DENIED;
		if (!NearAtm(player))
			return VB_TabletResult.NO_ATM;
		if (amount <= 0)
			return VB_TabletResult.BAD_INPUT;
		return VB_TabletResult.OK;
	}

	// --- наличные ---------------------------------------------------------------------------
	protected static void Deposit(PlayerBase player, PlayerIdentity id, int amount)
	{
		string uid = id.GetPlainId();
		string who = id.GetName();
		VB_Clan c = null;
		int res = Check(player, uid, amount, c);
		if (res == VB_TabletResult.OK && amount > VB_TabletConfig.Get().BankMax - c.Bank)
			res = VB_TabletResult.BANK_FULL;
		// 1. Купюры изымаются. Сдача создаётся раньше, не поместилась: ничего не изменилось.
		if (res == VB_TabletResult.OK)
			res = VB_TabletMoney.Charge(player, amount);
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		// 2. Запись склада. Не записалось: купюры возвращаются.
		int before = c.Bank;
		c.Bank = before + amount;
		if (!VB_TabletClans.Save(c))
		{
			if (VB_TabletMoney.Give(player, amount))
				c.Bank = before;
			else
				Print(VB_TABLET_TAG + " ПОПОЛНЕНИЕ: файл не записан и купюры не вернуть, сумма оставлена на складе в памяти: " + c.Id + " " + amount.ToString());
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			VB_TabletServer.SendProfile(player, id);
			return;
		}
		VB_TabletClans.AddHistory(c, who, VB_ClanEvent.DEPOSIT, amount);
		VB_TabletClans.Log(c, VB_ClanEvent.DEPOSIT, who, "", "+" + amount.ToString());
		Print(VB_TABLET_TAG + " склад " + c.Id + ": " + who + " (" + uid + ") внёс " + amount.ToString() + ", стало " + c.Bank.ToString());
		Finish(player, id, c);
	}

	protected static void Withdraw(PlayerBase player, PlayerIdentity id, int amount)
	{
		string uid = id.GetPlainId();
		string who = id.GetName();
		VB_Clan c = null;
		int res = Check(player, uid, amount, c);
		if (res == VB_TabletResult.OK && c.Bank < amount)
			res = VB_TabletResult.NO_FUNDS;
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		// 1. Склад уменьшен и записан.
		int before = c.Bank;
		c.Bank = before - amount;
		if (!VB_TabletClans.Save(c))
		{
			c.Bank = before;
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			return;
		}
		// 2. Выдача только в инвентарь: не поместилось, созданное удалено, склад возвращается.
		if (!VB_TabletMoney.Give(player, amount))
		{
			c.Bank = before;
			if (!VB_TabletClans.Save(c))
				Print(VB_TABLET_TAG + " СНЯТИЕ: возврат на склад не записан, проверьте вручную: " + c.Id + " должно быть " + before.ToString());
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NO_ROOM);
			return;
		}
		VB_TabletClans.AddHistory(c, who, VB_ClanEvent.WITHDRAW, -amount);
		VB_TabletClans.Log(c, VB_ClanEvent.WITHDRAW, who, "", "-" + amount.ToString());
		Print(VB_TABLET_TAG + " склад " + c.Id + ": " + who + " (" + uid + ") снял " + amount.ToString() + ", стало " + c.Bank.ToString());
		Finish(player, id, c);
	}

	// --- личный счёт INT_Banking ------------------------------------------------------------
	protected static void FromAccount(PlayerBase player, PlayerIdentity id, int amount)
	{
#ifdef INT_BANKING_SE
		string uid = id.GetPlainId();
		string who = id.GetName();
		VB_Clan c = null;
		int res = Check(player, uid, amount, c);
		if (res == VB_TabletResult.OK && amount > VB_TabletConfig.Get().BankMax - c.Bank)
			res = VB_TabletResult.BANK_FULL;
		string key = "";
		// 1. Списание со счёта: банк сам проверит остаток и запишет файл счёта.
		if (res == VB_TabletResult.OK)
		{
			key = NewKey(c);
			int st = INT_Banking_API.Move(uid, -amount, VB_BANK_PEER + c.Tag, key, BANK_SOURCE);
			res = FromBank(st);
		}
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		// 2. Склад. Не записался: деньги возвращаются на счёт.
		int before = c.Bank;
		c.Bank = before + amount;
		if (!VB_TabletClans.Save(c))
		{
			c.Bank = before;
			int back = INT_Banking_API.Move(uid, amount, VB_BANK_PEER + c.Tag, key + "-R", BANK_SOURCE);
			if (back != INT_Bank_Status.OK)
				Print(VB_TABLET_TAG + " СО СЧЁТА НА СКЛАД: склад не записан и счёт не вернуть, проверьте вручную: " + uid + " должен получить " + amount.ToString());
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			return;
		}
		VB_TabletClans.AddHistory(c, who, VB_ClanEvent.FROM_ACCOUNT, amount);
		VB_TabletClans.Log(c, VB_ClanEvent.FROM_ACCOUNT, who, "", "+" + amount.ToString());
		Print(VB_TABLET_TAG + " склад " + c.Id + ": " + who + " (" + uid + ") перевёл со счёта " + amount.ToString() + ", стало " + c.Bank.ToString() + ", ключ " + key);
		Finish(player, id, c);
#else
		VB_TabletServer.SendResult(player, id, VB_TabletResult.NO_BANK);
#endif
	}

	protected static void ToAccount(PlayerBase player, PlayerIdentity id, int amount)
	{
#ifdef INT_BANKING_SE
		string uid = id.GetPlainId();
		string who = id.GetName();
		VB_Clan c = null;
		int res = Check(player, uid, amount, c);
		if (res == VB_TabletResult.OK && c.Bank < amount)
			res = VB_TabletResult.NO_FUNDS;
		if (res == VB_TabletResult.OK)
		{
			// -1: банк не готов или файл счёта сломан. Счёта ещё нет: 0, банк создаст его при зачислении.
			if (INT_Banking_API.GetBalance(uid) < 0)
				res = VB_TabletResult.NO_BANK;
			else if (!INT_Banking_API.CanCredit(uid, amount))
				res = VB_TabletResult.ACC_FULL;
		}
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		// 1. Склад уменьшен и записан.
		int before = c.Bank;
		c.Bank = before - amount;
		if (!VB_TabletClans.Save(c))
		{
			c.Bank = before;
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			return;
		}
		// 2. Зачисление. Банк отказал: склад возвращается.
		string key = NewKey(c);
		int st = INT_Banking_API.Move(uid, amount, VB_BANK_PEER + c.Tag, key, BANK_SOURCE);
		if (st != INT_Bank_Status.OK)
		{
			c.Bank = before;
			if (!VB_TabletClans.Save(c))
				Print(VB_TABLET_TAG + " СО СКЛАДА НА СЧЁТ: возврат на склад не записан, проверьте вручную: " + c.Id + " должно быть " + before.ToString());
			VB_TabletServer.SendResult(player, id, FromBank(st));
			return;
		}
		VB_TabletClans.AddHistory(c, who, VB_ClanEvent.TO_ACCOUNT, -amount);
		VB_TabletClans.Log(c, VB_ClanEvent.TO_ACCOUNT, who, "", "-" + amount.ToString());
		Print(VB_TABLET_TAG + " склад " + c.Id + ": " + who + " (" + uid + ") перевёл на счёт " + amount.ToString() + ", стало " + c.Bank.ToString() + ", ключ " + key);
		Finish(player, id, c);
#else
		VB_TabletServer.SendResult(player, id, VB_TabletResult.NO_BANK);
#endif
	}

#ifdef INT_BANKING_SE
	// Ключ перевода для банка: по нему INT_Banking_API.HasKey найдёт запись в истории счёта.
	// Дата и время в ключе: номер клана и счётчик повторяются после рестарта.
	protected static string NewKey(VB_Clan c)
	{
		s_TxnNo++;
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		string when = y.ToString() + mo.ToString() + d.ToString() + "-" + h.ToString() + mi.ToString() + s.ToString();
		return c.Id + "-" + when + "-" + s_TxnNo.ToString();
	}

	// Ответ банка в ответ планшета.
	protected static int FromBank(int st)
	{
		if (st == INT_Bank_Status.OK)
			return VB_TabletResult.OK;
		if (st == INT_Bank_Status.NO_FUNDS)
			return VB_TabletResult.ACC_NO_FUNDS;
		if (st == INT_Bank_Status.LIMIT_BALANCE)
			return VB_TabletResult.ACC_FULL;
		if (st == INT_Bank_Status.STORAGE)
			return VB_TabletResult.STORAGE;
		return VB_TabletResult.NO_BANK;
	}
#endif

	protected static void Finish(PlayerBase player, PlayerIdentity id, VB_Clan c)
	{
		VB_TabletClans.Save(c);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.OK);
		VB_TabletServer.PushClan(c);
		PushHistory(c);
		VB_TabletServer.SendProfile(player, id);
	}

	// История всем в сети: у кого открыт склад, увидит операцию сразу.
	static void PushHistory(VB_Clan c)
	{
		foreach (VB_ClanMember m : c.Members)
		{
			if (!m.Online)
				continue;
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (!p)
				continue;
			int op = VB_TabletOp.CLAN_BANK;
			int n = c.History.Count();
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(op);
			rpc.Write(n);
			foreach (VB_ClanBankRow r : c.History)
				r.Write(rpc);
			rpc.Send(p, VB_TABLET_RPC_S2C, true, p.GetIdentity());
		}
	}
}
