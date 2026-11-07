// Client session: sends requests, reads the server answers, queues them and dispatches them OUTSIDE the
// RPC handler (no UI is built inside OnRPC). Keeps the latest state, the currency list and the online
// players page for the window.

class INT_Bank_Client
{
	static const float TIMEOUT_SEC = 10.0;
	static const float OPEN_GAP_SEC = 1.0;

	protected static ref INT_Bank_Client s_Instance;

	protected int m_Seq;
	protected ref array<ref INT_Bank_Reply> m_Queue;
	protected ref map<int, float> m_Pending;		// seq -> tick time sent
	protected ref map<int, int> m_PendingOp;		// seq -> op
	protected ref array<int> m_Expired;
	protected ref ScriptInvoker m_OnReply;		// (INT_Bank_Reply r)
	protected ref ScriptInvoker m_OnPush;		// (INT_Bank_Reply r)
	protected float m_LastOpenAt = -100;

	// cache
	ref INT_Bank_State State;
	bool HasState;
	ref array<string> CurClasses;
	ref array<int> CurValues;
	ref array<int> CurUnits;

	static INT_Bank_Client Get()
	{
		if (!s_Instance)
			s_Instance = new INT_Bank_Client();
		return s_Instance;
	}

	static void Reset()
	{
		s_Instance = null;
	}

	void INT_Bank_Client()
	{
		m_Seq = 0;
		m_Queue = new array<ref INT_Bank_Reply>();
		m_Pending = new map<int, float>();
		m_PendingOp = new map<int, int>();
		m_Expired = new array<int>();
		m_OnReply = new ScriptInvoker();
		m_OnPush = new ScriptInvoker();
		State = new INT_Bank_State();
		CurClasses = new array<string>();
		CurValues = new array<int>();
		CurUnits = new array<int>();
	}

	ScriptInvoker OnReply()
	{
		return m_OnReply;
	}

	ScriptInvoker OnPush()
	{
		return m_OnPush;
	}

	// The open action: at most one open per OPEN_GAP_SEC (the server limits requests anyway).
	void RequestOpen()
	{
		if (GetGame().IsDedicatedServer())
			return;
		float now = GetGame().GetTickTime();
		if (now - m_LastOpenAt < OPEN_GAP_SEC)
			return;
		m_LastOpenAt = now;
		INT_UIF_Menu.Open(INT_BANK_MENU_ID);
	}

	bool IsPending(int seq)
	{
		return m_Pending.Contains(seq);
	}

	int PendingCount()
	{
		return m_Pending.Count();
	}

	// Returns the request sequence number (0 = not sent).
	int Send(int op, int a = 0, int b = 0, int c = 0, string s = "")
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return 0;
		m_Seq++;
		if (m_Seq > 1000000000)
			m_Seq = 1;
		int seq = m_Seq;
		float now = GetGame().GetTickTime();
		m_Pending.Set(seq, now);
		m_PendingOp.Set(seq, op);
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(seq);
		rpc.Write(a);
		rpc.Write(b);
		rpc.Write(c);
		rpc.Write(s);
		rpc.Send(player, INT_BANK_RPC_C2S, true, null);
		return seq;
	}

	// From PlayerBase.OnRPC (client). Reads the whole answer, only queues it.
	void OnServerMessage(ParamsReadContext ctx)
	{
		INT_Bank_Reply r = new INT_Bank_Reply();
		if (!ctx.Read(r.Op) || !ctx.Read(r.Seq) || !ctx.Read(r.Status))
			return;
		if (!ctx.Read(r.A) || !ctx.Read(r.B) || !ctx.Read(r.C) || !ctx.Read(r.D))
			return;
		if (!ctx.Read(r.S1) || !ctx.Read(r.S2))
			return;
		if (!r.State.Read(ctx))
			return;
		if (r.Op == INT_Bank_Op.HELLO && r.Status == INT_Bank_Status.OK)
		{
			if (!ctx.Read(r.Classes) || !ctx.Read(r.Values) || !ctx.Read(r.Units))
				return;
		}
		else if (r.Op == INT_Bank_Op.HISTORY && r.Status == INT_Bank_Status.OK)
		{
			array<string> times = new array<string>();
			array<int> types = new array<int>();
			array<int> amounts = new array<int>();
			array<int> after = new array<int>();
			array<string> ids = new array<string>();
			array<string> names = new array<string>();
			if (!ctx.Read(times) || !ctx.Read(types) || !ctx.Read(amounts) || !ctx.Read(after) || !ctx.Read(ids) || !ctx.Read(names))
				return;
			int n = times.Count();
			for (int i = 0; i < n; i++)
			{
				INT_Bank_HistRow row = new INT_Bank_HistRow();
				row.Time = times.Get(i);
				row.Type = types.Get(i);
				row.Amount = amounts.Get(i);
				row.BalanceAfter = after.Get(i);
				row.PeerId = ids.Get(i);
				row.PeerName = names.Get(i);
				r.Rows.Insert(row);
			}
		}
		m_Queue.Insert(r);
	}

	// Every frame from the mission (5_Mission): dispatches answers, turns lost requests into TIMEOUT.
	void Pump()
	{
		if (m_Queue.Count() > 0)
		{
			array<ref INT_Bank_Reply> batch = new array<ref INT_Bank_Reply>();
			foreach (INT_Bank_Reply q : m_Queue)
				batch.Insert(q);
			m_Queue.Clear();
			foreach (INT_Bank_Reply r : batch)
				Dispatch(r);
		}
		if (m_Pending.Count() == 0)
			return;
		float now = GetGame().GetTickTime();
		m_Expired.Clear();
		foreach (int seq, float at : m_Pending)
		{
			if (now - at > TIMEOUT_SEC)
				m_Expired.Insert(seq);
		}
		foreach (int ex : m_Expired)
		{
			int op = m_PendingOp.Get(ex);
			m_Pending.Remove(ex);
			m_PendingOp.Remove(ex);
			INT_Bank_Reply t = new INT_Bank_Reply();
			t.Op = op;
			t.Seq = ex;
			t.Status = INT_Bank_Status.TIMEOUT;
			m_OnReply.Invoke(t);
		}
	}

	protected void Dispatch(INT_Bank_Reply r)
	{
		// every server answer carries the account state (NO_SERVER answers carry zeros)
		if (r.Status != INT_Bank_Status.NO_SERVER && r.Status != INT_Bank_Status.ACCOUNT_BROKEN)
		{
			State.CopyFrom(r.State);
			HasState = true;
		}
		if (r.Op == INT_Bank_Op.HELLO && r.Status == INT_Bank_Status.OK)
		{
			CurClasses.Copy(r.Classes);
			CurValues.Copy(r.Values);
			CurUnits.Copy(r.Units);
		}
		if (r.Op == INT_Bank_Op.PUSH_TRANSFER_IN)
		{
			m_OnPush.Invoke(r);
			return;
		}
		m_Pending.Remove(r.Seq);
		m_PendingOp.Remove(r.Seq);
		m_OnReply.Invoke(r);
	}

	// Value of the money the local player carries (the inventory is replicated to the owner).
	int CountCashLocal()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || CurClasses.Count() == 0)
			return -1;
		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		int total = 0;
		int n = CurClasses.Count();
		foreach (EntityAI e : items)
		{
			if (!e || e == player)
				continue;
			string t = e.GetType();
			t.ToLower();
			for (int i = 0; i < n; i++)
			{
				string c = CurClasses.Get(i);
				c.ToLower();
				if (c != t)
					continue;
				int pieces = 1;
				ItemBase ib = ItemBase.Cast(e);
				if (ib && ib.HasQuantity())
					pieces = Math.Round(ib.GetQuantity());
				int unit = CurUnits.Get(i);
				if (unit < 1)
					unit = 1;
				int units = pieces / unit;
				int value = CurValues.Get(i);
				total = total + units * value;
				break;
			}
		}
		return total;
	}

	// Greedy split of a payout into notes (for the "you will receive" preview; the server decides).
	// Fills pairs (class, pieces); false when the amount cannot be paid with these notes.
	bool PreviewNotes(int amount, array<string> classes, array<int> pieces)
	{
		classes.Clear();
		pieces.Clear();
		int remaining = amount;
		int n = CurClasses.Count();
		// currency list from the server is sorted descending by value per piece
		for (int i = 0; i < n; i++)
		{
			if (remaining <= 0)
				break;
			int value = CurValues.Get(i);
			if (value <= 0)
				continue;
			int units = remaining / value;
			if (units <= 0)
				continue;
			int unit = CurUnits.Get(i);
			classes.Insert(CurClasses.Get(i));
			pieces.Insert(units * unit);
			remaining = remaining - units * value;
		}
		return remaining == 0;
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == INT_BANK_RPC_S2C)
		{
			if (!GetGame().IsDedicatedServer())
			{
				PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
				if (localPlayer == this)
					INT_Bank_Client.Get().OnServerMessage(ctx);
			}
			return;
		}
		if (rpc_type == INT_BANK_RPC_C2S)
		{
			// Reached only when INT_Banking_SE is NOT loaded: its modded OnRPC handles C2S first and
			// never calls super for it.
			if (GetGame().IsDedicatedServer())
				INT_Bank_NoServerReply(sender, ctx);
			return;
		}
		super.OnRPC(sender, rpc_type, ctx);
	}

	protected void INT_Bank_NoServerReply(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender || !GetIdentity() || sender.GetId() != GetIdentity().GetId())
			return;
		int op;
		int seq;
		if (!ctx.Read(op) || !ctx.Read(seq))
			return;
		INT_Bank_State st = new INT_Bank_State();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(seq);
		rpc.Write(INT_Bank_Status.NO_SERVER);
		rpc.Write(0);
		rpc.Write(0);
		rpc.Write(0);
		rpc.Write(0);
		rpc.Write("");
		rpc.Write("");
		st.Write(rpc);
		rpc.Send(this, INT_BANK_RPC_S2C, true, sender);
	}
}
