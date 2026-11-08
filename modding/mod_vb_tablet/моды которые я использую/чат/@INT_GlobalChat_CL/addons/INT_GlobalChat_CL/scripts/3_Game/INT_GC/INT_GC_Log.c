// INT GlobalChat — script log (spec.md §6). One short line per call (the script log cuts every Print at
// 255 characters), '%' made harmless: a bare '%' in Print kills the client (measured 2026-10-13) and players type
// '%' in chat. Tag: "[INT_GC]" client / shared, "[INT_GC_SE]" server. ОТЛАДКА lines only with "Debug": 1 in the
// server config (clients follow the DEBUG flag of WELCOME).

class INT_GC_Log
{
	static bool s_Debug;
	static string s_Tag = "[INT_GC]";

	static string Safe(string s)
	{
		string r = INT_GC_Util.ReplaceByte(s, "%", " pct");
		r = INT_GC_Util.ReplaceByte(r, "\n", " ");
		return INT_GC_Util.ReplaceByte(r, "\r", " ");
	}

	// Player text for a log line: one line, at most maxChars characters.
	static string Clip(string s, int maxChars)
	{
		string r = s + "";
		if (r.LengthUtf8() > maxChars)
			r = r.SubstringUtf8(0, maxChars) + "~";
		r = INT_GC_Util.ReplaceByte(r, "\n", " ");
		r = INT_GC_Util.ReplaceByte(r, "\r", " ");
		return INT_GC_Util.ReplaceByte(r, "\t", " ");
	}

	static void Write(string level, string text)
	{
		string line = s_Tag + " " + level + " | " + text;
		if (line.LengthUtf8() > 250)
			line = line.SubstringUtf8(0, 250);
		Print(Safe(line));
	}

	static void Dbg(string text)
	{
		if (!s_Debug)
			return;
		Write("ОТЛАДКА", text);
	}

	static void Info(string text)
	{
		Write("ИНФО", text);
	}

	static void Warn(string text)
	{
		Write("ВНИМАНИЕ", text);
	}

	static void Err(string text)
	{
		Write("ОШИБКА", text);
	}

	static void Crit(string text)
	{
		Write("КРИТИЧНО", text);
	}

	// Plain line without a level (ready lines, self-test lines).
	static void Raw(string text)
	{
		Print(Safe(s_Tag + " " + text));
	}
}

class INT_GC_Util
{
	// Local time of this machine (owner rule 19): "YYYY-MM-DD HH:MM:SS".
	static string Stamp()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		string date = y.ToString() + "-" + mo.ToStringLen(2) + "-" + d.ToStringLen(2);
		return date + " " + h.ToStringLen(2) + ":" + mi.ToStringLen(2) + ":" + s.ToStringLen(2);
	}

	// "YYYY-MM-DD_HH-MM-SS" for file names.
	static string FileStamp()
	{
		string s = Stamp();
		s.Replace(" ", "_");
		s.Replace(":", "-");
		return s;
	}

	// Player-controlled text kept on one line, control characters removed, trimmed, at most maxChars characters
	// and never more than MAX_WIRE_BYTES bytes (so it always fits one RPC string).
	static string CleanText(string s, int maxChars)
	{
		string r = FitBytes(s, CLEAN_INPUT_BYTES);
		r = ReplaceByte(r, "\r", " ");
		r = ReplaceByte(r, "\n", " ");
		r = ReplaceByte(r, "\t", " ");
		r.TrimInPlace();
		if (maxChars > 0 && r.LengthUtf8() > maxChars)
			r = r.SubstringUtf8(0, maxChars);
		return FitBytes(r, INT_GC_Const.MAX_WIRE_BYTES);
	}

	// string.Replace on a string longer than 2560 bytes cuts it to 5120 - length bytes, even with nothing to
	// replace (measured on 1.30.164078: 2561 B -> 2559, 3000 -> 2120, 5120 and more -> empty, ASCII or Cyrillic
	// alike). Long text is replaced in byte chunks well below that size; `from`
	// must be one ASCII byte, so a chunk border never splits a match and UTF-8 bytes pass through unchanged.
	static const int REPLACE_SAFE_BYTES = 1024;
	static const int CLEAN_INPUT_BYTES = 8192;	// CleanText input beyond this is cut before cleaning

	static string ReplaceByte(string s, string from, string to)
	{
		int total = s.Length();
		if (total <= REPLACE_SAFE_BYTES)
		{
			string small = s + "";
			small.Replace(from, to);
			return small;
		}
		string r = "";
		int pos = 0;
		int n;
		string chunk;
		while (pos < total)
		{
			n = Math.Min(REPLACE_SAFE_BYTES, total - pos);
			chunk = s.Substring(pos, n);
			chunk.Replace(from, to);
			r = r + chunk;
			pos = pos + n;
		}
		return r;
	}

	// Longest prefix of whole UTF-8 characters that takes at most maxBytes bytes.
	static string FitBytes(string s, int maxBytes)
	{
		if (s.Length() <= maxBytes)
			return s;
		int lo = 0;
		int hi = s.LengthUtf8();
		int mid;
		string part;
		while (lo < hi)
		{
			mid = (lo + hi + 1) / 2;
			part = s.SubstringUtf8(0, mid);
			if (part.Length() <= maxBytes)
				lo = mid;
			else
				hi = mid - 1;
		}
		return s.SubstringUtf8(0, lo);
	}

	// Lower case for ASCII and Russian Cyrillic (ё -> е); used for nick comparisons.
	static string Lower(string s)
	{
		return INT_GC_Filter.LowerText(s);
	}

	static bool StartsWith(string s, string prefix)
	{
		if (prefix == "")
			return false;
		return s.IndexOf(prefix) == 0;
	}
}
