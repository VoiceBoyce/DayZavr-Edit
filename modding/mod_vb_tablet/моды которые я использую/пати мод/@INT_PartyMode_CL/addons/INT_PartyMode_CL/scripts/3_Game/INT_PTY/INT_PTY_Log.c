// INT PartyMode — script log (spec.md §7). One short line per call (the script log cuts every Print at 255
// characters), '%' made harmless (a bare '%' in Print kills the client, measured 2026-10-13). Tag "[INT_PTY]"
// client / shared, "[INT_PTY_SE]" server. ОТЛАДКА lines only with "DebugMode": 1 in the server config (clients
// follow the debug flag of WELCOME).

class INT_PTY_Log
{
	static bool s_Debug;
	static string s_Tag = "[INT_PTY]";

	static string Safe(string s)
	{
		string r = s + "";
		r.Replace("%", " pct");
		r.Replace("\n", " ");
		r.Replace("\r", " ");
		return r;
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

class INT_PTY_Util
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

	// "HH:MM:SS" of this machine.
	static string Clock()
	{
		int h;
		int mi;
		int s;
		GetHourMinuteSecond(h, mi, s);
		return h.ToStringLen(2) + ":" + mi.ToStringLen(2) + ":" + s.ToStringLen(2);
	}

	// "YYYY-MM-DD_HH-MM-SS" for file names.
	static string FileStamp()
	{
		string s = Stamp();
		s.Replace(" ", "_");
		s.Replace(":", "-");
		return s;
	}

	static bool IsSteamId(string s)
	{
		if (s.Length() != INT_PTY_Const.STEAM_ID_LEN)
			return false;
		string digits = "0123456789";
		for (int i = 0; i < INT_PTY_Const.STEAM_ID_LEN; i++)
		{
			if (digits.IndexOf(s.Get(i)) < 0)
				return false;
		}
		return true;
	}

	// A player name kept on one line, control characters removed, trimmed, at most MAX_NAME_CHARS characters.
	// A leading '#' becomes '№': widgets would translate "#STR_…" as a stringtable key (review W-5).
	static string CleanName(string s)
	{
		string r = s + "";
		if (r.Length() > INT_PTY_Const.MAX_NAME_CHARS * 8)
			r = r.Substring(0, INT_PTY_Const.MAX_NAME_CHARS * 8);
		r.Replace("\r", " ");
		r.Replace("\n", " ");
		r.Replace("\t", " ");
		r.Replace("%", " ");
		r.TrimInPlace();
		if (r.IndexOf("#") == 0)
			r = "№" + r.Substring(1, r.Length() - 1);
		if (r.LengthUtf8() > INT_PTY_Const.MAX_NAME_CHARS)
			r = r.SubstringUtf8(0, INT_PTY_Const.MAX_NAME_CHARS);
		return r;
	}

	// string.Replace on a string longer than 2560 bytes corrupts it, even with nothing to replace (measured on 1.30,
	// INT_GlobalChat). Long text is replaced in chunks well below that size; `from` must be ONE ASCII byte, so a chunk
	// border never splits a match and UTF-8 bytes pass through unchanged.
	static const int REPLACE_SAFE_BYTES = 1024;

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
		while (pos < total)
		{
			int n = Math.Min(REPLACE_SAFE_BYTES, total - pos);
			string chunk = s.Substring(pos, n);
			chunk.Replace(from, to);
			r = r + chunk;
			pos = pos + n;
		}
		return r;
	}

	// Two file texts are the same (CR ignored; surrounding whitespace ignored only for short texts, where Trim is safe).
	static bool SameText(string a, string b)
	{
		if (a == b)
			return true;
		string ca = ReplaceByte(a, "\r", "");
		string cb = ReplaceByte(b, "\r", "");
		if (ca == cb)
			return true;
		if (ca.Length() > 2000 || cb.Length() > 2000)
			return false;
		ca.TrimInPlace();
		cb.TrimInPlace();
		return ca == cb;
	}

	// Finite and inside a generous world box (largest DayZ maps are < 25.6 km).
	static bool SanePos(vector v)
	{
		for (int i = 0; i < 3; i++)
		{
			float c = v[i];
			if (c != c)
				return false;
			if (c < -1000 || c > 40000)
				return false;
		}
		return true;
	}

	protected static string s_UnitM;
	protected static string s_UnitKm;

	// Distance as text in the player's language: "124 м", "1,2 км".
	static string DistText(float m)
	{
		if (s_UnitM == "")
		{
			s_UnitM = Widget.TranslateString("#STR_INT_PTY_UNIT_M");
			s_UnitKm = Widget.TranslateString("#STR_INT_PTY_UNIT_KM");
		}
		if (m < 999.5)
		{
			int whole = (int)Math.Round(m);
			return whole.ToString() + " " + s_UnitM;
		}
		int tenths = (int)Math.Round(m / 100.0);
		int km = tenths / 10;
		int frac = tenths - km * 10;
		return km.ToString() + "," + frac.ToString() + " " + s_UnitKm;
	}
}
