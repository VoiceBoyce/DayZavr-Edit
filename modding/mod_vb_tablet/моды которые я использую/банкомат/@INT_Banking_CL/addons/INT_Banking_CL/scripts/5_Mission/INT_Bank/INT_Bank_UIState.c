// INT Banking - client-side state: preview mode with sample data (screenshots without a server) and
// small display helpers.

class INT_Bank_UIState
{
	protected static bool s_Preview;
	protected static string s_PreviewMode = "";

	static void SetPreview(bool on, string mode)
	{
		s_Preview = on;
		s_PreviewMode = mode;
	}

	static bool IsPreview()
	{
		return s_Preview;
	}

	static string PreviewMode()
	{
		return s_PreviewMode;
	}

	// "5 000" for a note class (the value of one piece), the class name when unknown.
	static string NoteName(string cls)
	{
		INT_Bank_Client c = INT_Bank_Client.Get();
		int n = c.CurClasses.Count();
		for (int i = 0; i < n; i++)
		{
			string k = c.CurClasses.Get(i);
			if (k != cls)
				continue;
			int value = c.CurValues.Get(i);
			int unit = c.CurUnits.Get(i);
			if (unit > 1)
				value = value / unit;
			return INT_Bank_Util.Money(value);
		}
		return cls;
	}

	// ---- sample data ---------------------------------------------------------------------------
	static void FillSample()
	{
		INT_Bank_Client c = INT_Bank_Client.Get();
		INT_Bank_State st = c.State;
		st.Balance = 1284650;
		st.Cash = 23400;
		st.MaxBalance = 30000000;
		st.MaxDeposit = 0;
		st.MaxWithdraw = 500000;
		st.MaxTransfer = 0;
		st.CdDeposit = 0;
		st.CdWithdraw = 0;
		st.CdTransfer = 7;
		st.HistoryCount = 23;
		c.HasState = true;
		c.CurClasses.Clear();
		c.CurValues.Clear();
		c.CurUnits.Clear();
		array<string> cls = {"Bablo5000Rubley", "Bablo2000Rubley", "Bablo1000Rubley", "Bablo500Rubley", "Bablo200Rubley", "Bablo100Rubley", "Bablo50Rubley", "MoneyRuble25", "Bablo10Rubley", "Bablo5Rubley", "MoneyRuble1"};
		array<int> val = {5000, 2000, 1000, 500, 200, 100, 50, 25, 10, 5, 1};
		int n = cls.Count();
		for (int i = 0; i < n; i++)
		{
			c.CurClasses.Insert(cls.Get(i));
			c.CurValues.Insert(val.Get(i));
			c.CurUnits.Insert(1);
		}
	}

	static INT_Bank_Reply SampleHistory(int page)
	{
		INT_Bank_Reply r = new INT_Bank_Reply();
		r.Op = INT_Bank_Op.HISTORY;
		r.Status = INT_Bank_Status.OK;
		array<int> types = {INT_Bank_Hist.WITHDRAW, INT_Bank_Hist.TRANSFER_IN, INT_Bank_Hist.DEPOSIT, INT_Bank_Hist.TRANSFER_OUT, INT_Bank_Hist.P2P_IN, INT_Bank_Hist.WITHDRAW, INT_Bank_Hist.DEPOSIT, INT_Bank_Hist.P2P_OUT};
		array<int> amounts = {17385, 50000, 120000, 25000, 8400, 5000, 300000, 12000};
		array<string> names = {"", "Babka_Zina", "", "Serega", "Kolyan", "", "", "Kolyan"};
		array<string> times = {"16.10.2026 00:21", "15.10.2026 23:58", "15.10.2026 23:40", "15.10.2026 22:05", "15.10.2026 21:47", "15.10.2026 20:11", "14.10.2026 19:30", "14.10.2026 18:02"};
		int n = types.Count();
		for (int i = 0; i < n; i++)
		{
			INT_Bank_HistRow row = new INT_Bank_HistRow();
			row.Time = times.Get(i);
			row.Type = types.Get(i);
			row.Amount = amounts.Get(i);
			row.PeerName = names.Get(i);
			if (row.PeerName != "")
				row.PeerId = "76561198000000042";
			r.Rows.Insert(row);
		}
		r.A = page;
		r.B = 3;
		r.C = 23;
		return r;
	}

	static INT_Bank_Reply SampleOp(int op, int a, int b, int c, string s)
	{
		INT_Bank_Reply r = new INT_Bank_Reply();
		r.Op = op;
		r.Status = INT_Bank_Status.OK;
		INT_Bank_State st = INT_Bank_Client.Get().State;
		if (op == INT_Bank_Op.DEPOSIT)
		{
			int amount = a;
			if (b != 0)
				amount = st.Cash;
			st.Balance = st.Balance + amount;
			st.CdDeposit = 5;
			st.CdWithdraw = 10;
			r.A = amount;
			r.B = 0;
		}
		else if (op == INT_Bank_Op.WITHDRAW)
		{
			st.Balance = st.Balance - a;
			st.CdWithdraw = 5;
			st.CdDeposit = 10;
			r.A = a;
		}
		else if (op == INT_Bank_Op.TRANSFER)
		{
			st.Balance = st.Balance - a;
			st.CdTransfer = 10;
			r.A = a;
			r.S1 = "Serega";
			r.S2 = s;
		}
		r.State.CopyFrom(st);
		return r;
	}

	static INT_Bank_Reply SampleLookup(string id)
	{
		INT_Bank_Reply r = new INT_Bank_Reply();
		r.Op = INT_Bank_Op.LOOKUP;
		r.Status = INT_Bank_Status.OK;
		r.A = 1;
		r.S1 = "Serega";
		r.S2 = id;
		return r;
	}
}
