// Банкомат и склад клана. Каждая операция выполняется целиком за один вызов на сервере.
// Порядок денег как в INT_BankSE_Service: деньги не появляются дважды.
//   Снятие: баланс уменьшен и записан, потом выдача купюр; не поместилось, баланс возвращается.
//   Пополнение: купюры изъяты, потом баланс записан; запись не удалась, купюры возвращаются.
// Сверх того двойной клик отсекает пауза между операциями одного игрока.
class VB_TabletBank
{
	static const int GAP_MS = 400;
	static const float PLACE_DIST = 12.0;	// голограмма не дальше этого от игрока

	protected static ref map<string, VB_TabletAtm> s_Atms = new map<string, VB_TabletAtm>();
	protected static ref map<string, int> s_LastOp = new map<string, int>();

	// Операции 40..59 приходят сюда из VB_TabletClanOps.Handle.
	static void Handle(PlayerBase player, PlayerIdentity id, int op, ParamsReadContext ctx)
	{
		float x;
		float y;
		float z;
		float yaw;
		int n;
		switch (op)
		{
			case VB_TabletOp.ATM_PLACE:
				if (ctx.Read(x) && ctx.Read(y) && ctx.Read(z) && ctx.Read(yaw))
					Place(player, id, x, y, z, yaw);
				break;
			case VB_TabletOp.ATM_DEPOSIT:
				if (ctx.Read(n))
					Deposit(player, id, n);
				break;
			case VB_TabletOp.ATM_WITHDRAW:
				if (ctx.Read(n))
					Withdraw(player, id, n);
				break;
			case VB_TabletOp.CONTRACTS:
				VB_TabletContracts.Send(player, id);
				break;
			case VB_TabletOp.CONTRACT_CLAIM:
				if (ctx.Read(n))
					VB_TabletContracts.Claim(player, id, n);
				break;
			case VB_TabletOp.CONTRACT_REPLACE:
				if (ctx.Read(n))
					VB_TabletContracts.Replace(player, id, n);
				break;
			default:
				VB_TabletAdmin.Handle(player, id, op, ctx);
				break;
		}
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

	// --- объект в мире --------------------------------------------------------------------
	protected static VB_TabletAtm Create(vector at, float yaw)
	{
		// Объект живёт только в файлах клана: без хранилища мира и таймера лута.
		int flags = ECE_SETUP | ECE_CREATEPHYSICS | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN | ECE_UPDATEPATHGRAPH | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD;
		Object o = GetGame().CreateObjectEx("VB_TabletAtm", at, flags, RF_IGNORE);
		VB_TabletAtm atm = VB_TabletAtm.Cast(o);
		if (!atm)
		{
			if (o)
				GetGame().ObjectDelete(o);
			Print(VB_TABLET_TAG + " не создан объект банкомата");
			return null;
		}
		atm.SetOrientation(Vector(yaw, 0, 0));
		atm.SetAllowDamage(false);
		GetGame().GetWorld().ProcessMarkedObjectsForPathgraphUpdate();
		return atm;
	}

	static void RemoveAtm(string clanId)
	{
		VB_TabletAtm atm;
		if (s_Atms.Find(clanId, atm) && atm)
			GetGame().ObjectDelete(atm);
		s_Atms.Remove(clanId);
	}

	// Старт сервера: банкоматы из файлов кланов.
	static void SpawnAll()
	{
		map<string, ref VB_Clan> all = VB_TabletClans.All();
		foreach (string key, VB_Clan c : all)
		{
			if (!c.HasAtm)
				continue;
			float y = c.AtmY;
			if (y == 0)
				y = GetGame().SurfaceY(c.AtmX, c.AtmZ);
			VB_TabletAtm atm = Create(Vector(c.AtmX, y, c.AtmZ), c.AtmYaw);
			if (atm)
				s_Atms.Set(c.Id, atm);
		}
	}

	protected static void Place(PlayerBase player, PlayerIdentity id, float x, float y, float z, float yaw)
	{
		string uid = id.GetPlainId();
		int res = VB_TabletResult.OK;
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		vector at = Vector(x, y, z);
		float dist = vector.Distance(player.GetPosition(), at);
		if (!Allow(uid))
			res = VB_TabletResult.RATE;
		else if (!c || !c.MemberCan(uid, VB_ClanPerm.ATM) || !CanAct(player))
			res = VB_TabletResult.DENIED;
		else if (!(dist <= PLACE_DIST) || !(yaw >= -720 && yaw <= 720))
			res = VB_TabletResult.NOT_NEAR;
		VB_TabletAtm atm = null;
		if (res == VB_TabletResult.OK)
		{
			atm = Create(at, yaw);
			if (!atm)
				res = VB_TabletResult.STORAGE;
		}
		if (res != VB_TabletResult.OK)
		{
			Notify(player, id, VBT_ResultKey(res), "", 2);
			return;
		}
		bool moved = c.HasAtm;
		RemoveAtm(c.Id);
		s_Atms.Set(c.Id, atm);
		c.HasAtm = true;
		c.AtmX = x;
		c.AtmY = y;
		c.AtmZ = z;
		c.AtmYaw = yaw;
		if (moved)
		{
			VB_TabletClans.Log(c, VB_ClanEvent.ATM_MOVE, id.GetName());
			Notify(player, id, "#STR_VB_TAB_C_ATM_MOVED", "", 1);
		}
		else
		{
			VB_TabletClans.Log(c, VB_ClanEvent.ATM_SET, id.GetName());
			Notify(player, id, "#STR_VB_TAB_C_ATM_DONE", "", 1);
		}
		VB_TabletClans.Save(c);
		VB_TabletServer.PushClan(c);
	}

	protected static string VBT_ResultKey(int code)
	{
		return "#STR_VB_TAB_R_" + code.ToString();
	}

	// --- деньги ---------------------------------------------------------------------------
	protected static bool NearAtm(PlayerBase player, VB_Clan c)
	{
		vector at = Vector(c.AtmX, c.AtmY, c.AtmZ);
		VB_TabletAtm atm;
		if (s_Atms.Find(c.Id, atm) && atm)
			at = atm.GetPosition();
		return vector.Distance(player.GetPosition(), at) <= VB_TabletConfig.Get().AtmDist;
	}

	// Проверки до любых изменений: игрок, право ранга, банкомат своего клана рядом, сумма.
	protected static int Check(PlayerBase player, string uid, int amount, out VB_Clan c)
	{
		if (!Allow(uid))
			return VB_TabletResult.RATE;
		c = VB_TabletClans.OfPlayer(uid);
		if (!c || !c.MemberCan(uid, VB_ClanPerm.BANK) || !CanAct(player))
			return VB_TabletResult.DENIED;
		if (!c.HasAtm)
			return VB_TabletResult.NO_ATM;
		if (!NearAtm(player, c))
			return VB_TabletResult.NOT_NEAR;
		if (amount <= 0)
			return VB_TabletResult.BAD_INPUT;
		return VB_TabletResult.OK;
	}

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
		// 2. Запись баланса. Не записалось: купюры возвращаются.
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
		// 1. Баланс уменьшен и записан.
		int before = c.Bank;
		c.Bank = before - amount;
		if (!VB_TabletClans.Save(c))
		{
			c.Bank = before;
			VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			return;
		}
		// 2. Выдача только в инвентарь: не поместилось, созданное удалено, баланс возвращается.
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
