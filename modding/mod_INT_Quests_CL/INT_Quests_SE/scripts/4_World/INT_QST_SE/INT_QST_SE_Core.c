// INT Quests (server) — requests from clients and the quest operations (spec.md §6). Event-driven only: nothing here
// runs unless a client sends something or a game event happens (owner rules 1-3). Every request: protocol version,
// rate limit per player, the player must be alive; accept/turn-in check the giver key and the distance to the giver
// on the server's own data. Time limits and cooldowns are evaluated right here, on demand (no timers).

class INT_QST_SE_Rate
{
	float m_Start;
	int m_Count;
	float m_PullStart;
	int m_Pulls;
}

class INT_QST_SE_Core
{
	static const float RATE_WINDOW = 10.0;
	static const int RATE_MAX = 40;
	static const float PULL_WINDOW = 60.0;
	// one turn-in per player per second: the delete of a carried item is a juncture, for a moment the handed-in item
	// still exists and could be handed in again (review 2026-10-22)
	static const float TURNIN_GAP = 1.0;

	protected static bool s_Hooked;
	protected static ref map<string, ref INT_QST_SE_Rate> s_Rates = new map<string, ref INT_QST_SE_Rate>();

	// self-test
	static int s_In;
	static int s_Dropped;
	static ref array<string> s_Capture;
	static bool s_NoSend;

	static void Init()
	{
		if (!s_Hooked)
		{
			DayZGame.Event_OnRPC.Insert(OnRPC);
			s_Hooked = true;
		}
	}

	static void Shutdown()
	{
		if (s_Hooked && DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(OnRPC);
		s_Hooked = false;
	}

	static void Forget(string uid)
	{
		s_Rates.Remove(uid);
		INT_QST_SE_Admin.Forget(uid);
	}

	protected static INT_QST_SE_Rate Rate(string uid)
	{
		INT_QST_SE_Rate r;
		if (!s_Rates.Find(uid, r))
		{
			r = new INT_QST_SE_Rate();
			float now = GetGame().GetTickTime();
			r.m_Start = now;
			r.m_PullStart = now;
			s_Rates.Set(uid, r);
		}
		return r;
	}

	protected static bool RateOk(string uid)
	{
		INT_QST_SE_Rate r = Rate(uid);
		float now = GetGame().GetTickTime();
		if (now - r.m_Start > RATE_WINDOW)
		{
			r.m_Start = now;
			r.m_Count = 0;
		}
		r.m_Count++;
		return r.m_Count <= RATE_MAX;
	}

	// A new catalog: every client may download it once more in this minute.
	static void ResetPulls()
	{
		foreach (string uid, INT_QST_SE_Rate r : s_Rates)
			r.m_Pulls = 0;
	}

	// Catalog pulls: the whole catalog plus a margin per minute.
	protected static bool PullOk(string uid)
	{
		INT_QST_SE_Rate r = Rate(uid);
		float now = GetGame().GetTickTime();
		if (now - r.m_PullStart > PULL_WINDOW)
		{
			r.m_PullStart = now;
			r.m_Pulls = 0;
		}
		r.m_Pulls++;
		return r.m_Pulls <= INT_QST_SE_Store.PacketCount() + 20;
	}

	// ---- network in --------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_QST_Const.RPC_C2S)
			return;
		if (!GetGame() || !GetGame().IsDedicatedServer())
			return;
		s_In++;
		if (!sender)
		{
			s_Dropped++;
			return;
		}
		int op;
		if (!INT_QST_Wire.ReadHeader(ctx, op))
		{
			s_Dropped++;
			INT_QST_Log.Warn("пакет не прочитан (другая версия мода у клиента?) от " + INT_QST_Log.Clip(sender.GetName(), 32));
			return;
		}
		string uid = sender.GetPlainId();
		if (op == INT_QST_C2S.CATALOG_PULL)
		{
			if (!PullOk(uid))
			{
				s_Dropped++;
				return;
			}
			HandlePull(sender, ctx);
			return;
		}
		// admins edit in bursts (a save is several packets): their admin packets skip the player rate limit
		bool adminOp = op == INT_QST_C2S.ADMIN && INT_QST_SE_Store.IsAdmin(uid);
		if (!adminOp && !RateOk(uid))
		{
			s_Dropped++;
			return;
		}
		INT_QST_SE_Session s = INT_QST_SE_Players.Get(uid);
		if (!s)
			s = INT_QST_SE_Players.Open(sender);
		if (!s)
			return;
		PlayerBase p = PlayerBase.Cast(sender.GetPlayer());
		int id;
		string key;
		int code;
		switch (op)
		{
			case INT_QST_C2S.HELLO:
				s.m_Hello = true;
				ExpireAll(s, p);
				SendHello(s);
				INT_QST_SE_Players.FlushResetNotes(s);
				RetryAutoComplete(s, p);
				return;
			case INT_QST_C2S.ACCEPT:
				if (!ctx.Read(id) || !ctx.Read(key))
					return;
				code = Accept(s, p, id, key);
				SendResult(s, op, code, id);
				return;
			case INT_QST_C2S.TURNIN:
				if (!ctx.Read(id) || !ctx.Read(key))
					return;
				code = TurnIn(s, p, id, key);
				SendResult(s, op, code, id);
				return;
			case INT_QST_C2S.CANCEL:
				if (!ctx.Read(id))
					return;
				code = Cancel(s, p, id);
				SendResult(s, op, code, id);
				return;
			case INT_QST_C2S.EXPLORED:
				int ti;
				if (!ctx.Read(id) || !ctx.Read(ti))
					return;
				code = Explored(s, p, id, ti);
				if (code != INT_QST_Res.OK)
					SendResult(s, op, code, id);
				return;
			case INT_QST_C2S.DISMISS:
				if (!ctx.Read(id))
					return;
				code = Dismiss(s, p, id);
				SendResult(s, op, code, id);
				return;
			case INT_QST_C2S.ADMIN:
				if (!INT_QST_SE_Store.IsAdmin(uid))
				{
					SendResult(s, op, INT_QST_Res.NOT_ADMIN, 0);
					INT_QST_SE_FileLog.Line("ОТКАЗ: админ-команда не от админа " + INT_QST_Log.Clip(s.m_Name, 32) + " (" + uid + ")");
					return;
				}
				INT_QST_SE_Admin.Handle(s, p, ctx);
				return;
		}
		s_Dropped++;
	}

	// ---- operations ----------------------------------------------------------------------------------------------
	static int Accept(INT_QST_SE_Session s, PlayerBase p, int id, string key)
	{
		if (!p || !p.IsAlive())
			return INT_QST_Res.DEAD;
		INT_QST_Config cfg = INT_QST_SE_Store.s_Config;
		INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
		if (!q)
			return INT_QST_Res.NOT_FOUND;
		if (q.Enabled == 0)
			return INT_QST_Res.DISABLED;
		if (s.m_St.FindActive(id))
			return INT_QST_Res.ALREADY_ACTIVE;
		if (q.GiverStart != "")
		{
			if (key != q.GiverStart)
				return INT_QST_Res.WRONG_GIVER;
			if (!INT_QST_SE_Givers.Near(p, key, cfg.TalkDistance))
				return INT_QST_Res.TOO_FAR;
		}
		int now = INT_QST_Util.UnixNow();
		int offset = INT_QST_Util.LocalOffset();
		int wait;
		int st = INT_QST_Rules.Status(q, s.m_St, INT_QST_SE_Store.s_Catalog, now, offset, wait);
		if (st == INT_QST_St.DONE)
			return INT_QST_Res.DONE_ONCE;
		if (st == INT_QST_St.COOLDOWN)
			return INT_QST_Res.COOLDOWN;
		if (st == INT_QST_St.LOCKED)
			return INT_QST_Res.LOCKED;
		if (st != INT_QST_St.AVAILABLE)
			return INT_QST_Res.DISABLED;
		if (INT_QST_Rules.FreeSlots(s.m_St, cfg.QuestSlots) <= 0)
			return INT_QST_Res.NO_SLOTS;
		INT_QST_Active a = new INT_QST_Active();
		a.Id = id;
		a.Start = now;
		a.Sig = INT_QST_Rules.TargetSig(q);
		a.Ensure(q.Targets.Count());
		s.m_St.Active.Insert(a);
		s.m_St.Reindex();
		// quest items once: not again while the ones of an earlier try are still out (cancel/fail with them stashed)
		bool ground = false;
		bool gave = false;
		if (q.QuestItems.Count() > 0 && s.m_St.ItemsOut.Find(id) < 0)
		{
			INT_QST_SE_Items.GiveQuestItems(p, q, ground);
			s.m_St.ItemsOut.Insert(id);
			gave = true;
		}
		if (!INT_QST_SE_Players.Save(s))
		{
			s.m_St.RemoveActive(id);
			if (gave)
			{
				INT_QST_SE_Items.TakeQuestItems(p, q);
				s.m_St.ItemsOut.RemoveItem(id);
			}
			return INT_QST_Res.STORAGE;
		}
		INT_QST_SE_Players.Reindex(s);
		INT_QST_SE_FileLog.Line(Who(s) + " взял задание [" + id.ToString() + "] " + q.Name);
		SendState(s);
		SendNote(s, INT_QST_Note.ACCEPTED, id, 0, "");
		if (gave)
			SendNote(s, INT_QST_Note.QUEST_ITEM, id, 0, "");
		if (ground)
			SendNote(s, INT_QST_Note.REWARD_GROUND, id, 0, "");
		// a quest with nothing to count and no giver to visit completes at once (talk-only, giver-less finish)
		TryAutoComplete(s, p, q);
		return INT_QST_Res.OK;
	}

	static int TurnIn(INT_QST_SE_Session s, PlayerBase p, int id, string key)
	{
		if (!p || !p.IsAlive())
			return INT_QST_Res.DEAD;
		float tick = GetGame().GetTickTime();
		if (s.m_LastTurnIn > 0 && tick - s.m_LastTurnIn < TURNIN_GAP)
			return INT_QST_Res.BUSY;
		INT_QST_Active a = s.m_St.FindActive(id);
		if (!a)
			return INT_QST_Res.NOT_ACTIVE;
		INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
		if (!q)
		{
			s.m_St.RemoveActive(id);
			INT_QST_SE_Players.Save(s);
			INT_QST_SE_Players.Reindex(s);
			SendState(s);
			return INT_QST_Res.NOT_FOUND;
		}
		int now = INT_QST_Util.UnixNow();
		if (INT_QST_Rules.IsExpired(q, a, now))
		{
			Fail(s, p, q);
			return INT_QST_Res.EXPIRED;
		}
		INT_QST_Config cfg = INT_QST_SE_Store.s_Config;
		if (q.GiverFinish != "")
		{
			if (key != q.GiverFinish)
				return INT_QST_Res.WRONG_GIVER;
			if (!INT_QST_SE_Givers.Near(p, key, cfg.TalkDistance))
				return INT_QST_Res.TOO_FAR;
		}
		if (!INT_QST_Rules.CountedDone(q, a))
			return INT_QST_Res.NOT_DONE;
		bool inv = INT_QST_Rules.HasInventoryTargets(q);
		if (inv)
		{
			if (!p.CanDeleteItems())
				return INT_QST_Res.BUSY;
			if (!INT_QST_Inv.AllCovered(p, q))
				return INT_QST_Res.MISSING_ITEMS;
			// only items that hold something would do: handing them over would delete their cargo/attachments
			if (!INT_QST_Inv.AllCovered(p, q, true))
				return INT_QST_Res.ITEM_NOT_EMPTY;
		}
		int money = 0;
		foreach (INT_QST_RewardDef r : q.Rewards)
		{
			if (r.m_Type == INT_QST_RT.MONEY)
				money = money + r.Amount;
		}
		int bank = INT_QST_SE_Bank.CanCredit(s.m_Uid, money);
		if (bank != INT_QST_Res.OK)
			return bank;
		// ---- commit ----
		s.m_LastTurnIn = tick;
		if (inv)
			INT_QST_SE_Items.TakeDelivered(p, q);
		INT_QST_SE_Items.TakeQuestItems(p, q);
		s.m_St.ItemsOut.RemoveItem(id);
		bool ground = false;
		int failed = INT_QST_SE_Items.GiveItems(p, q, ground);
		bool paid = true;
		if (money > 0)
			paid = INT_QST_SE_Bank.Credit(s.m_Uid, money, "QST_" + id.ToString() + "_" + now.ToString());
		INT_QST_Done d = s.m_St.FindDone(id);
		if (!d)
		{
			d = new INT_QST_Done();
			d.Id = id;
			s.m_St.Done.Insert(d);
			s.m_St.Reindex();
		}
		d.Last = now;
		d.Times = d.Times + 1;
		s.m_St.RemoveActive(id);
		INT_QST_SE_Players.Save(s);
		INT_QST_SE_Players.Reindex(s);
		string line = Who(s) + " сдал задание [" + id.ToString() + "] " + q.Name;
		if (money > 0)
			line = line + ", деньги на счёт " + money.ToString() + " (" + paid.ToString() + ")";
		if (failed > 0)
			line = line + ", не создано наград " + failed.ToString();
		INT_QST_SE_FileLog.Line(line);
		if (!paid)
			INT_QST_Log.Err("банк не принял зачисление " + money.ToString() + " игроку " + s.m_Uid + " за квест " + id.ToString());
		SendState(s);
		SendNote(s, INT_QST_Note.COMPLETED, id, 0, "");
		if (money > 0 && paid)
			SendNote(s, INT_QST_Note.MONEY, id, money, "");
		if (ground)
			SendNote(s, INT_QST_Note.REWARD_GROUND, id, 0, "");
		return INT_QST_Res.OK;
	}

	static int Cancel(INT_QST_SE_Session s, PlayerBase p, int id)
	{
		// a shop-like quest may always be dropped: the giver window takes it back when the exchange's turn-in failed
		if (INT_QST_SE_Store.s_Config.CanCancel == 0)
		{
			INT_QST_QuestDef cq = INT_QST_SE_Store.s_Catalog.Get(id);
			if (!cq || !cq.m_Instant)
				return INT_QST_Res.CANNOT_CANCEL;
		}
		INT_QST_Active a = s.m_St.FindActive(id);
		if (!a)
			return INT_QST_Res.NOT_ACTIVE;
		INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
		s.m_St.RemoveActive(id);
		if (q && p && INT_QST_SE_Items.TakeQuestItems(p, q))
			s.m_St.ItemsOut.RemoveItem(id);
		INT_QST_SE_Players.Save(s);
		INT_QST_SE_Players.Reindex(s);
		string name = "?";
		if (q)
			name = q.Name;
		INT_QST_SE_FileLog.Line(Who(s) + " отказался от задания [" + id.ToString() + "] " + name);
		SendState(s);
		SendNote(s, INT_QST_Note.CANCELLED, id, 0, "");
		return INT_QST_Res.OK;
	}

	static int Dismiss(INT_QST_SE_Session s, PlayerBase p, int id)
	{
		INT_QST_Active a = s.m_St.FindActive(id);
		if (!a)
			return INT_QST_Res.NOT_ACTIVE;
		INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
		if (q && !INT_QST_Rules.IsExpired(q, a, INT_QST_Util.UnixNow()))
			return INT_QST_Res.NOT_DONE;
		if (q)
			Fail(s, p, q);
		else
		{
			s.m_St.RemoveActive(id);
			INT_QST_SE_Players.Save(s);
			INT_QST_SE_Players.Reindex(s);
			SendState(s);
		}
		return INT_QST_Res.OK;
	}

	static int Explored(INT_QST_SE_Session s, PlayerBase p, int id, int ti)
	{
		if (!p || !p.IsAlive())
			return INT_QST_Res.DEAD;
		INT_QST_Active a = s.m_St.FindActive(id);
		if (!a)
			return INT_QST_Res.NOT_ACTIVE;
		INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
		if (!q || ti < 0 || ti >= q.Targets.Count())
			return INT_QST_Res.BAD_DATA;
		INT_QST_TargetDef t = q.Targets[ti];
		if (t.m_Type != INT_QST_TT.EXPLORE || !t.m_HasPos)
			return INT_QST_Res.BAD_DATA;
		a.Ensure(q.Targets.Count());
		if (a.Progress[ti] >= 1)
			return INT_QST_Res.OK;
		if (INT_QST_Rules.IsExpired(q, a, INT_QST_Util.UnixNow()))
		{
			Fail(s, p, q);
			return INT_QST_Res.EXPIRED;
		}
		vector pp = p.GetPosition();
		float dist;
		if (t.GroundY == 1)
		{
			float dx = pp[0] - t.m_Pos[0];
			float dz = pp[2] - t.m_Pos[2];
			dist = Math.Sqrt(dx * dx + dz * dz);
		}
		else
		{
			dist = vector.Distance(pp, t.m_Pos);
		}
		if (dist > t.Radius + INT_QST_SE_Store.s_Config.ExploreTolerance)
		{
			INT_QST_Log.Dbg("исследование отклонено: " + s.m_Uid + " квест " + id.ToString() + " дистанция " + dist.ToString());
			return INT_QST_Res.TOO_FAR;
		}
		a.Progress[ti] = 1;
		INT_QST_SE_Players.Save(s);
		INT_QST_SE_Players.Reindex(s);
		SendProgress(s, id, ti, 1);
		SendNote(s, INT_QST_Note.TARGET_DONE, id, ti, "");
		AfterProgress(s, p, q, a);
		return INT_QST_Res.OK;
	}

	// After counted progress: READY banner once, or complete on the spot when there is no finish giver.
	static void AfterProgress(INT_QST_SE_Session s, PlayerBase p, INT_QST_QuestDef q, INT_QST_Active a)
	{
		if (!INT_QST_Rules.CountedDone(q, a))
			return;
		if (TryAutoComplete(s, p, q))
			return;
		SendNote(s, INT_QST_Note.READY, q.Id, 0, "");
	}

	// Giver-less finish (owner decision O13): completes by itself when nothing must be handed in.
	static bool TryAutoComplete(INT_QST_SE_Session s, PlayerBase p, INT_QST_QuestDef q)
	{
		if (q.GiverFinish != "")
			return false;
		if (INT_QST_Rules.HasInventoryTargets(q))
			return false;
		INT_QST_Active a = s.m_St.FindActive(q.Id);
		if (!a || !INT_QST_Rules.CountedDone(q, a))
			return false;
		int code = TurnIn(s, p, q.Id, "");
		return code == INT_QST_Res.OK;
	}

	// On HELLO: giver-less quests whose auto turn-in failed earlier (dead, bank full, inventory busy) try again.
	static void RetryAutoComplete(INT_QST_SE_Session s, PlayerBase p)
	{
		array<int> ids = new array<int>();
		foreach (INT_QST_Active a : s.m_St.Active)
			ids.Insert(a.Id);
		foreach (int id : ids)
		{
			INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(id);
			if (q)
				TryAutoComplete(s, p, q);
		}
	}

	static void Fail(INT_QST_SE_Session s, PlayerBase p, INT_QST_QuestDef q)
	{
		s.m_St.RemoveActive(q.Id);
		if (p && INT_QST_SE_Items.TakeQuestItems(p, q))
			s.m_St.ItemsOut.RemoveItem(q.Id);
		INT_QST_SE_Players.Save(s);
		INT_QST_SE_Players.Reindex(s);
		INT_QST_SE_FileLog.Line(Who(s) + " провалил задание (время вышло) [" + q.Id.ToString() + "] " + q.Name);
		SendState(s);
		SendNote(s, INT_QST_Note.FAILED, q.Id, 0, "");
	}

	// On HELLO: quests whose time ran out while the player was away fail now (old mod did it at login).
	static void ExpireAll(INT_QST_SE_Session s, PlayerBase p)
	{
		int now = INT_QST_Util.UnixNow();
		array<int> gone = new array<int>();
		foreach (INT_QST_Active a : s.m_St.Active)
		{
			INT_QST_QuestDef q = INT_QST_SE_Store.s_Catalog.Get(a.Id);
			if (q && INT_QST_Rules.IsExpired(q, a, now))
				gone.Insert(a.Id);
		}
		foreach (int id : gone)
			Fail(s, p, INT_QST_SE_Store.s_Catalog.Get(id));
	}

	static string Who(INT_QST_SE_Session s)
	{
		return "Игрок " + INT_QST_Log.Clip(s.m_Name, 32) + " (" + s.m_Uid + ")";
	}

	// ---- network out -------------------------------------------------------------------------------------------
	static void SendTo(ScriptRPC rpc, PlayerIdentity to, string what)
	{
		if (s_Capture)
		{
			string who = "all";
			if (to)
				who = to.GetPlainId();
			s_Capture.Insert(what + ":" + who);
		}
		if (s_NoSend)
			return;
		rpc.Send(null, INT_QST_Const.RPC_S2C, true, to);
	}

	static void SendHello(INT_QST_SE_Session s)
	{
		if (!s.m_Identity)
			return;
		INT_QST_Config cfg = INT_QST_SE_Store.s_Config;
		INT_QST_Hello h = new INT_QST_Hello();
		h.IsAdmin = INT_QST_SE_Store.IsAdmin(s.m_Uid);
		h.DebugOn = cfg.DebugMode == 1;
		h.QuestSlots = cfg.QuestSlots;
		h.CanCancel = cfg.CanCancel == 1;
		h.ShowLocked = cfg.ShowLockedAtGiver == 1;
		h.AutoTrackMax = cfg.AutoTrackMax;
		h.TalkDistance = cfg.TalkDistance;
		h.Money = INT_QST_SE_Bank.IsAvailable();
		h.ServerUnix = INT_QST_Util.UnixNow();
		h.LocalOffset = INT_QST_Util.LocalOffset();
		h.CatalogHash = INT_QST_SE_Store.s_Catalog.m_Hash;
		h.CatalogCount = INT_QST_SE_Store.s_Catalog.m_List.Count();
		// SIGNS-OFF (owner 2026-10-31): signs above the givers
		// h.ShowSigns = cfg.ShowSigns != 0;
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.HELLO);
		h.Write(rpc);
		WriteGivers(rpc);
		INT_QST_Wire.WriteState(rpc, s.m_St);
		SendTo(rpc, s.m_Identity, "HELLO");
	}

	static void WriteGivers(ScriptRPC rpc)
	{
		array<ref INT_QST_GiverView> views = new array<ref INT_QST_GiverView>();
		INT_QST_SE_Givers.Views(views);
		int n = Math.Min(views.Count(), INT_QST_Const.MAX_GIVERS);
		rpc.Write(n);
		for (int i = 0; i < n; i++)
			views[i].Write(rpc);
	}

	static void SendState(INT_QST_SE_Session s)
	{
		if (!s.m_Identity || !s.m_Hello)
			return;
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.STATE);
		int now = INT_QST_Util.UnixNow();
		rpc.Write(now);
		INT_QST_Wire.WriteState(rpc, s.m_St);
		SendTo(rpc, s.m_Identity, "STATE");
	}

	static void SendProgress(INT_QST_SE_Session s, int id, int ti, float value)
	{
		if (!s.m_Identity || !s.m_Hello)
			return;
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.PROGRESS);
		rpc.Write(id);
		rpc.Write(ti);
		rpc.Write(value);
		SendTo(rpc, s.m_Identity, "PROGRESS");
	}

	static void SendNote(INT_QST_SE_Session s, int kind, int id, int a, string text)
	{
		if (!s.m_Identity || !s.m_Hello)
			return;
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.NOTE);
		rpc.Write(kind);
		rpc.Write(id);
		rpc.Write(a);
		rpc.Write(INT_QST_Util.FitBytes(text, INT_QST_Const.PIECE_BYTES));
		SendTo(rpc, s.m_Identity, "NOTE" + kind.ToString());
	}

	static void SendResult(INT_QST_SE_Session s, int forOp, int code, int id)
	{
		if (!s.m_Identity)
			return;
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.RESULT);
		rpc.Write(forOp);
		rpc.Write(code);
		rpc.Write(id);
		SendTo(rpc, s.m_Identity, "RESULT" + code.ToString());
	}

	static void HandlePull(PlayerIdentity to, ParamsReadContext ctx)
	{
		int hash;
		int k;
		if (!ctx.Read(hash) || !ctx.Read(k))
			return;
		ScriptRPC rpc = new ScriptRPC();
		int cur = INT_QST_SE_Store.s_Catalog.m_Hash;
		if (hash != cur)
		{
			INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.CATALOG_CHANGED);
			rpc.Write(cur);
			SendTo(rpc, to, "CHANGED");
			return;
		}
		int packets = INT_QST_SE_Store.PacketCount();
		if (k < 0 || k >= packets)
			return;
		int first = k * INT_QST_Const.PIECES_PER_PACKET;
		int last = Math.Min(first + INT_QST_Const.PIECES_PER_PACKET, INT_QST_SE_Store.s_PieceText.Count());
		INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.CATALOG);
		rpc.Write(cur);
		rpc.Write(k);
		rpc.Write(packets);
		int recs = INT_QST_SE_Store.s_RecordCount;
		rpc.Write(recs);
		int n = last - first;
		rpc.Write(n);
		int rec;
		int kind;
		int pidx;
		int pcnt;
		string text;
		for (int i = first; i < last; i++)
		{
			rec = INT_QST_SE_Store.s_PieceRec[i];
			kind = INT_QST_SE_Store.s_PieceKind[i];
			pidx = INT_QST_SE_Store.s_PieceIdx[i];
			pcnt = INT_QST_SE_Store.s_PieceCnt[i];
			text = INT_QST_SE_Store.s_PieceText[i];
			rpc.Write(rec);
			rpc.Write(kind);
			rpc.Write(pidx);
			rpc.Write(pcnt);
			rpc.Write(text);
		}
		SendTo(rpc, to, "CATALOG");
	}

	// Everybody online: the catalog changed (admin edit / reload) — clients pull again.
	static void BroadcastCatalogChanged()
	{
		ResetPulls();
		foreach (string uid, INT_QST_SE_Session s : INT_QST_SE_Players.s_ByUid)
		{
			if (!s.m_Identity || !s.m_Hello)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.CATALOG_CHANGED);
			rpc.Write(INT_QST_SE_Store.s_Catalog.m_Hash);
			SendTo(rpc, s.m_Identity, "CHANGED");
		}
	}

	// One changed catalog record to every client (it checks oldTotal against its copy, else re-downloads).
	static void BroadcastPatch(int oldTotal, int newTotal, int kind, int del, string key, string text)
	{
		array<string> pieces = new array<string>();
		if (del == 0)
			INT_QST_Util.SplitBytes(text, INT_QST_Const.PIECE_BYTES, pieces);
		if (pieces.Count() > 40)
		{
			BroadcastCatalogChanged();
			return;
		}
		int n = pieces.Count();
		foreach (string uid, INT_QST_SE_Session s : INT_QST_SE_Players.s_ByUid)
		{
			if (!s.m_Identity || !s.m_Hello)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.CATALOG_PATCH);
			rpc.Write(oldTotal);
			rpc.Write(newTotal);
			rpc.Write(kind);
			rpc.Write(del);
			rpc.Write(key);
			rpc.Write(n);
			for (int i = 0; i < n; i++)
			{
				string piece = pieces[i];
				rpc.Write(piece);
			}
			SendTo(rpc, s.m_Identity, "PATCH");
		}
	}

	static void BroadcastGivers()
	{
		foreach (string uid, INT_QST_SE_Session s : INT_QST_SE_Players.s_ByUid)
		{
			if (!s.m_Identity || !s.m_Hello)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.GIVERS);
			WriteGivers(rpc);
			SendTo(rpc, s.m_Identity, "GIVERS");
		}
	}
}


