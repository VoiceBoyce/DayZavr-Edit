// INT GlobalChat — wire helpers shared by both sides (spec.md §4.3). Every packet starts with the operation code
// and the protocol version; a packet of another version is dropped by the reader.

class INT_GC_Proto
{
	static ScriptRPC Begin(int op)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(INT_GC_Const.PROTOCOL);
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
		return ver == INT_GC_Const.PROTOCOL;
	}

	static void WriteStrings(ParamsWriteContext ctx, array<string> list)
	{
		int n = 0;
		if (list)
			n = list.Count();
		ctx.Write(n);
		for (int i = 0; i < n; i++)
		{
			string s = list.Get(i);
			ctx.Write(s);
		}
	}

	static bool ReadStrings(ParamsReadContext ctx, array<string> list, int maxCount)
	{
		int n = 0;
		if (!ctx.Read(n))
			return false;
		if (n < 0 || n > maxCount)
			return false;
		for (int i = 0; i < n; i++)
		{
			string s;
			if (!ctx.Read(s))
				return false;
			list.Insert(s);
		}
		return true;
	}

	// Splits words into parts of at most maxBytes bytes each (a word longer than that gets a part of its own).
	static void SplitParts(array<string> words, int maxBytes, array<ref array<string>> parts)
	{
		array<string> cur = new array<string>();
		int bytes = 0;
		foreach (string w : words)
		{
			int wb = w.Length() + 4;
			if (cur.Count() > 0 && bytes + wb > maxBytes)
			{
				parts.Insert(cur);
				cur = new array<string>();
				bytes = 0;
			}
			cur.Insert(w);
			bytes = bytes + wb;
		}
		if (cur.Count() > 0)
			parts.Insert(cur);
	}
}
