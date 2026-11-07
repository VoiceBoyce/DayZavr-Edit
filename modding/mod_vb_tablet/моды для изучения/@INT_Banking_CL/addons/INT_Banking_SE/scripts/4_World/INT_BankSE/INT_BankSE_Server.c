// INT Banking (server) - RPC entry point and answers. Wire format: INT_Bank_Protocol.c (CL).

class INT_BankSE_Server
{
	protected static bool s_Ready;

	static void SetReady(bool ready)
	{
		s_Ready = ready;
	}

	static bool IsReady()
	{
		return s_Ready;
	}

	// C2S from PlayerBase.OnRPC. The sender must be the player the RPC targets.
	static void OnC2S(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!player || !sender)
			return;
		PlayerIdentity own = player.GetIdentity();
		if (!own || own.GetId() != sender.GetId())
			return;
		int op;
		int seq;
		int a;
		int b;
		int c;
		string s;
		if (!ctx.Read(op) || !ctx.Read(seq) || !ctx.Read(a) || !ctx.Read(b) || !ctx.Read(c) || !ctx.Read(s))
			return;
		if (s.Length() > 64)
			s = s.Substring(0, 64);
		Handle(player, sender, op, seq, a, b, c, s);
	}

	protected static bool IsMutation(int op)
	{
		if (op == INT_Bank_Op.DEPOSIT || op == INT_Bank_Op.WITHDRAW || op == INT_Bank_Op.TRANSFER || op == INT_Bank_Op.CLEAR_HISTORY)
			return true;
		return false;
	}

	protected static void Handle(PlayerBase player, PlayerIdentity id, int op, int seq, int a, int b, int c, string s)
	{
		string uid = id.GetPlainId();
		string name = INT_Bank_Util.CleanName(id.GetName(), 32);
		INT_BankSE_Out o = new INT_BankSE_Out();
		INT_BankSE_AccountData acc = null;

		if (!s_Ready)
		{
			o.Set(INT_Bank_Status.INTERNAL);
			Send(player, op, seq, o, null, null);
			return;
		}
		if (!INT_BankSE_Service.AllowRequest(uid, IsMutation(op)))
		{
			o.Set(INT_Bank_Status.RATE);
			Send(player, op, seq, o, null, null);
			return;
		}
		int check = INT_BankSE_Service.CheckPlayer(player);
		if (check != INT_Bank_Status.OK)
		{
			o.Set(check);
			Send(player, op, seq, o, null, null);
			return;
		}

		switch (op)
		{
			case INT_Bank_Op.HELLO:
			{
				acc = INT_BankSE_Service.Hello(player, uid, name, o);
				SendHello(player, seq, o, acc);
				return;
			}
			case INT_Bank_Op.STATE:
			{
				int st;
				acc = INT_BankSE_Accounts.Get(uid, true, name, st);
				o.Set(st);
				break;
			}
			case INT_Bank_Op.DEPOSIT:
			{
				acc = INT_BankSE_Service.Deposit(player, uid, name, a, b != 0, c != 0, o);
				break;
			}
			case INT_Bank_Op.WITHDRAW:
			{
				acc = INT_BankSE_Service.Withdraw(player, uid, name, a, c != 0, o);
				break;
			}
			case INT_Bank_Op.LOOKUP:
			{
				string target = INT_Bank_Util.Digits(s);
				INT_BankSE_Service.Lookup(uid, target, o);
				int lst;
				acc = INT_BankSE_Accounts.Get(uid, true, name, lst);
				break;
			}
			case INT_Bank_Op.TRANSFER:
			{
				string to = INT_Bank_Util.Digits(s);
				acc = INT_BankSE_Service.Transfer(player, uid, name, to, a, o);
				break;
			}
			case INT_Bank_Op.HISTORY:
			{
				int hst;
				acc = INT_BankSE_Accounts.Get(uid, true, name, hst);
				o.Set(hst);
				SendHistory(player, seq, o, acc, a);
				return;
			}
			case INT_Bank_Op.CLEAR_HISTORY:
			{
				int cst;
				acc = INT_BankSE_Accounts.Get(uid, true, name, cst);
				o.Set(cst);
				if (acc)
					INT_BankSE_Service.ClearHistory(uid, name, acc, o);
				break;
			}
			default:
			{
				o.Set(INT_Bank_Status.BAD_INPUT);
				break;
			}
		}
		Send(player, op, seq, o, acc, null);
	}

	// Common header + state. payload (optional) writes op data after the state.
	static void Send(PlayerBase player, int op, int seq, INT_BankSE_Out o, INT_BankSE_AccountData acc, INT_BankSE_Payload payload)
	{
		if (!player || !player.GetIdentity())
			return;
		string uid = player.GetIdentity().GetPlainId();
		// early refusals (rate, distance, ...) carry no account: read it so the state is real
		if (!acc && s_Ready && o.Status != INT_Bank_Status.ACCOUNT_BROKEN)
		{
			int ast;
			acc = INT_BankSE_Accounts.Get(uid, false, "", ast);
		}
		INT_Bank_State st = new INT_Bank_State();
		INT_BankSE_Service.FillState(player, uid, acc, st);
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(seq);
		rpc.Write(o.Status);
		rpc.Write(o.A);
		rpc.Write(o.B);
		rpc.Write(o.C);
		rpc.Write(o.D);
		rpc.Write(o.S1);
		rpc.Write(o.S2);
		st.Write(rpc);
		if (payload)
			payload.Write(rpc);
		rpc.Send(player, INT_BANK_RPC_S2C, true, player.GetIdentity());
	}

	protected static void SendHello(PlayerBase player, int seq, INT_BankSE_Out o, INT_BankSE_AccountData acc)
	{
		INT_BankSE_HelloPayload p = new INT_BankSE_HelloPayload();
		Send(player, INT_Bank_Op.HELLO, seq, o, acc, p);
	}

	protected static void SendHistory(PlayerBase player, int seq, INT_BankSE_Out o, INT_BankSE_AccountData acc, int page)
	{
		INT_BankSE_HistoryPayload p = new INT_BankSE_HistoryPayload();
		int total = 0;
		if (acc && acc.history)
			total = acc.history.Count();
		int pages = (total + INT_BANK_HISTORY_PAGE - 1) / INT_BANK_HISTORY_PAGE;
		if (pages < 1)
			pages = 1;
		if (page < 0)
			page = 0;
		if (page > pages - 1)
			page = pages - 1;
		// newest first
		int index = total - 1 - page * INT_BANK_HISTORY_PAGE;
		int got = 0;
		while (acc && index >= 0 && got < INT_BANK_HISTORY_PAGE)
		{
			INT_BankSE_HistEntry e = acc.history.Get(index);
			p.Times.Insert(e.time);
			p.Types.Insert(INT_Bank_Util.HistFromKey(e.type));
			p.Amounts.Insert(e.amount);
			p.After.Insert(e.balanceAfter);
			p.PeerIds.Insert(e.peerId);
			p.PeerNames.Insert(INT_Bank_Util.CleanName(e.peerName, 32));
			index--;
			got++;
		}
		o.A = page;
		o.B = pages;
		o.C = total;
		Send(player, INT_Bank_Op.HISTORY, seq, o, acc, p);
	}

}

// Op data written after the state block.
class INT_BankSE_Payload
{
	void Write(ScriptRPC rpc);
}

class INT_BankSE_HelloPayload extends INT_BankSE_Payload
{
	override void Write(ScriptRPC rpc)
	{
		array<string> classes = new array<string>();
		array<int> values = new array<int>();
		array<int> units = new array<int>();
		array<ref INT_BankSE_Denom> denomList = INT_BankSE_Money.Denoms();
		foreach (INT_BankSE_Denom d : denomList)
		{
			classes.Insert(d.ClassName);
			values.Insert(d.Value);
			units.Insert(d.Unit);
		}
		rpc.Write(classes);
		rpc.Write(values);
		rpc.Write(units);
	}
}

class INT_BankSE_HistoryPayload extends INT_BankSE_Payload
{
	ref array<string> Times = new array<string>();
	ref array<int> Types = new array<int>();
	ref array<int> Amounts = new array<int>();
	ref array<int> After = new array<int>();
	ref array<string> PeerIds = new array<string>();
	ref array<string> PeerNames = new array<string>();

	override void Write(ScriptRPC rpc)
	{
		rpc.Write(Times);
		rpc.Write(Types);
		rpc.Write(Amounts);
		rpc.Write(After);
		rpc.Write(PeerIds);
		rpc.Write(PeerNames);
	}
}

// C2S handling. Replaces the CL fallback (which answers NO_SERVER) and never calls super for C2S.
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == INT_BANK_RPC_C2S)
		{
			if (GetGame().IsDedicatedServer())
				INT_BankSE_Server.OnC2S(this, sender, ctx);
			return;
		}
		super.OnRPC(sender, rpc_type, ctx);
	}
}
