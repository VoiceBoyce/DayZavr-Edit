// INT PartyMode — wire helpers shared by both sides (spec.md §5). Every packet starts with the operation code and
// the protocol version; a packet of another version is dropped by the reader. RPC target is always null.

class INT_PTY_Proto
{
	static ScriptRPC Begin(int op)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(INT_PTY_Const.PROTOCOL);
		return rpc;
	}

	// false = unreadable or another protocol version.
	static bool ReadHeader(ParamsReadContext ctx, out int op)
	{
		op = 0;
		int ver = 0;
		if (!ctx.Read(op))
			return false;
		if (!ctx.Read(ver))
			return false;
		return ver == INT_PTY_Const.PROTOCOL;
	}

	// Client -> server Steam64 ids travel as TWO ints (first 8 digits, last 9): the server never reads a string from a
	// client, so a modified client cannot send a string of 1024+ bytes that throws a VM exception (review PTY-12).
	static void WriteId(ParamsWriteContext ctx, string id)
	{
		int hi = 0;
		int lo = 0;
		if (INT_PTY_Util.IsSteamId(id))
		{
			string head = id.Substring(0, 8);
			string tail = id.Substring(8, 9);
			hi = head.ToInt();
			lo = tail.ToInt();
		}
		ctx.Write(hi);
		ctx.Write(lo);
	}

	static bool ReadId(ParamsReadContext ctx, out string id)
	{
		id = "";
		int hi;
		int lo;
		if (!ctx.Read(hi) || !ctx.Read(lo))
			return false;
		if (hi < 10000000 || hi > 99999999 || lo < 0 || lo > 999999999)
			return false;
		// one call result per statement (argument temporaries trap); int.ToStringLen pads at most 7 zeros
		string head = hi.ToString();
		string tail = lo.ToString();
		while (tail.Length() < 9)
		{
			tail = "0" + tail;
		}
		string s = head + tail;
		if (!INT_PTY_Util.IsSteamId(s))
			return false;
		id = s;
		return true;
	}

	// A Steam64 id from the server (string): exactly 17 decimal digits (anything else is dropped).
	static bool ReadSteamId(ParamsReadContext ctx, out string id)
	{
		id = "";
		string s;
		if (!ctx.Read(s))
			return false;
		if (!INT_PTY_Util.IsSteamId(s))
			return false;
		id = s;
		return true;
	}

	// A name from the wire: one line, at most MAX_NAME_CHARS characters.
	static bool ReadName(ParamsReadContext ctx, out string name)
	{
		name = "";
		string s;
		if (!ctx.Read(s))
			return false;
		if (s.Length() > INT_PTY_Const.MAX_NAME_CHARS * 4)
			return false;
		name = INT_PTY_Util.CleanName(s);
		return true;
	}

	// A world position from the wire: finite and inside a sane world box.
	static bool ReadPos(ParamsReadContext ctx, out vector pos)
	{
		pos = vector.Zero;
		vector v;
		if (!ctx.Read(v))
			return false;
		if (!INT_PTY_Util.SanePos(v))
			return false;
		pos = v;
		return true;
	}
}
