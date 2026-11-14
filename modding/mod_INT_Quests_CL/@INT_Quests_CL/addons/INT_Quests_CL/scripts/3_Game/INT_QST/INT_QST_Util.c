// INT Quests — script log, time and small helpers for both sides (spec.md §9).
// One short line per Print (the script log cuts lines at 255 characters); '%' is made harmless because a bare '%'
// in Print kills the client (measured 2026-10-13). ОТЛАДКА lines only with "DebugMode": 1 in the server config
// (clients follow the flag sent in HELLO). Log times are the local time of the machine (owner rule 19).
// Durable timestamps are unix seconds in UTC built from GetYearMonthDayUTC/GetHourMinuteSecondUTC (no CF).

class INT_QST_Log
{
	static bool s_Debug;
	static string s_Tag = "[INT_QST]";

	static string Safe(string s)
	{
		string r = INT_QST_Util.ReplaceByte(s, "%", " pct");
		r = INT_QST_Util.ReplaceByte(r, "\n", " ");
		return INT_QST_Util.ReplaceByte(r, "\r", " ");
	}

	// Player- or admin-controlled text for a log line: one line, at most maxChars characters.
	static string Clip(string s, int maxChars)
	{
		string r = s + "";
		if (r.LengthUtf8() > maxChars)
			r = r.SubstringUtf8(0, maxChars) + "~";
		return r;
	}

	static void Write(string level, string text)
	{
		string line = s_Tag + " " + INT_QST_Util.Stamp() + " " + level + " | " + text;
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

	// Plain line without time or level (ready lines, self-test lines).
	static void Raw(string text)
	{
		Print(Safe(s_Tag + " " + text));
	}
}

class INT_QST_Util
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

	// "HH:MM:SS" (local machine time) for log lines.
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

	// Unix time in UTC seconds.
	static int UnixNow()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDayUTC(y, mo, d);
		GetHourMinuteSecondUTC(h, mi, s);
		return DaysFromCivil(y, mo, d) * 86400 + h * 3600 + mi * 60 + s;
	}

	// Machine local time minus UTC, in seconds, rounded to 15 minutes (daily/weekly resets use machine time).
	static int LocalOffset()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		int locSec = DaysFromCivil(y, mo, d) * 86400 + h * 3600 + mi * 60 + s;
		int diff = locSec - UnixNow();
		float q = diff / 900.0;
		int steps = (int)Math.Round(q);
		return steps * 900;
	}

	// Days since 1970-01-01 of a proleptic Gregorian date (H. Hinnant's algorithm, years >= 1).
	static int DaysFromCivil(int y, int m, int d)
	{
		int yy = y;
		if (m <= 2)
			yy = yy - 1;
		int era = yy / 400;
		int yoe = yy - era * 400;
		int mp = m - 3;
		if (m <= 2)
			mp = m + 9;
		int doy = (153 * mp + 2) / 5 + d - 1;
		int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
		return era * 146097 + doe - 719468;
	}

	// First daily reset strictly after `afterUtc` (local time hh:mm of a machine with the given offset), as UTC.
	static int NextDaily(int afterUtc, int hour, int minute, int offset)
	{
		int loc = afterUtc + offset;
		int dayStart = (loc / 86400) * 86400;
		int reset = dayStart + hour * 3600 + minute * 60;
		if (reset <= loc)
			reset = reset + 86400;
		return reset - offset;
	}

	// First weekly reset strictly after `afterUtc`: weekDay 1 = Monday .. 7 = Sunday, local hh:mm.
	static int NextWeekly(int afterUtc, int weekDay, int hour, int minute, int offset)
	{
		int loc = afterUtc + offset;
		int days = loc / 86400;
		int dayStart = days * 86400;
		int today = ((days + 3) % 7) + 1;		// 1970-01-01 was a Thursday (ISO 4)
		int wd = ClampI(weekDay, 1, 7);
		int delta = (wd - today + 7) % 7;
		int reset = dayStart + delta * 86400 + hour * 3600 + minute * 60;
		if (reset <= loc)
			reset = reset + 7 * 86400;
		return reset - offset;
	}

	// "2д 14ч", "3ч 12м", "45с" (units are stringtable texts the caller passes translated).
	static string DurationShort(int seconds, string dUnit, string hUnit, string mUnit, string sUnit)
	{
		int t = seconds;
		if (t < 0)
			t = 0;
		int days = t / 86400;
		int hours = (t - days * 86400) / 3600;
		int mins = (t - days * 86400 - hours * 3600) / 60;
		int secs = t - days * 86400 - hours * 3600 - mins * 60;
		if (days > 0)
		{
			if (hours > 0)
				return days.ToString() + dUnit + " " + hours.ToString() + hUnit;
			return days.ToString() + dUnit;
		}
		if (hours > 0)
		{
			if (mins > 0)
				return hours.ToString() + hUnit + " " + mins.ToString() + mUnit;
			return hours.ToString() + hUnit;
		}
		if (mins > 0)
			return mins.ToString() + mUnit;
		return secs.ToString() + sUnit;
	}

	// 1234567 -> "1 234 567".
	static string Money(int v)
	{
		string sign = "";
		int a = v;
		if (a < 0)
		{
			sign = "-";
			a = -a;
		}
		string digits = a.ToString();
		string r = "";
		int n = digits.Length();
		for (int i = 0; i < n; i++)
		{
			if (i > 0 && (n - i) % 3 == 0)
				r = r + " ";
			r = r + digits.Get(i);
		}
		return sign + r;
	}

	// string.Replace corrupts strings longer than 2560 bytes (measured on 1.30, INT_GlobalChat): long text is
	// replaced in byte chunks; `from` must be one ASCII byte so a chunk border never splits a match.
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

	// Cuts a long text into pieces of at most maxBytes bytes on UTF-8 character borders (wire pieces). Always the
	// byte splitter: the old SubstringUtf8 version returned "" past ~8192 characters, so a long catalog record lost its
	// tail and the quest vanished for every client (review 2026-10-22).
	static void SplitBytes(string s, int maxBytes, array<string> outPieces)
	{
		SplitLong(s, maxBytes, outPieces);
	}

	// Long text (any length) -> pieces of at most maxBytes bytes, never cutting a UTF-8 character. Byte based: a
	// SubstringUtf8 whose start passes ~8192 characters returns "" (measured in INT_P2P), so the long admin payloads
	// (board layout, quest batches) are cut with the byte Substring and a continuation-byte check.
	static void SplitLong(string text, int maxBytes, array<string> outPieces)
	{
		outPieces.Clear();
		int total = text.Length();
		int pos = 0;
		while (pos < total)
		{
			int len = total - pos;
			if (len > maxBytes)
			{
				len = maxBytes;
				// step back while the first byte of the next piece is a UTF-8 continuation byte (10xxxxxx)
				while (len > 1 && IsContinuation(text, pos + len))
					len--;
			}
			outPieces.Insert(text.Substring(pos, len));
			pos = pos + len;
		}
		if (outPieces.Count() == 0)
			outPieces.Insert("");
	}

	static bool IsContinuation(string text, int byteIndex)
	{
		string ch = text.Substring(byteIndex, 1);
		int b = ch.ToAscii();
		if (b < 0)
			b = b + 256;
		return (b & 0xC0) == 0x80;
	}

	// Text -> Description pieces (joined as they are on load).
	static void TextToPieces(string text, array<string> outPieces)
	{
		if (text == "")
		{
			outPieces.Clear();
			return;
		}
		SplitBytes(text, INT_QST_Const.TEXT_PIECE_BYTES, outPieces);
	}

	// One-line text for names and labels: no line breaks, trimmed, at most maxBytes bytes.
	static string OneLine(string s, int maxBytes)
	{
		string r = FitBytes(s, 4096);
		r = ReplaceByte(r, "\r", " ");
		r = ReplaceByte(r, "\n", " ");
		r = ReplaceByte(r, "\t", " ");
		r.TrimInPlace();
		return FitBytes(r, maxBytes);
	}

	static bool IsSteam64(string uid)
	{
		if (uid.Length() != 17)
			return false;
		string c;
		for (int i = 0; i < 17; i++)
		{
			c = uid.Get(i);
			if (c < "0" || c > "9")
				return false;
		}
		return true;
	}

	// "x y z" or "x, y, z" -> vector; false when not three numbers.
	static bool ParseVec(string s, out vector v)
	{
		v = vector.Zero;
		string t = ReplaceByte(s, ",", " ");
		t.TrimInPlace();
		array<string> parts = new array<string>();
		t.Split(" ", parts);
		array<string> nums = new array<string>();
		foreach (string p : parts)
		{
			if (p != "")
				nums.Insert(p);
		}
		if (nums.Count() != 3)
			return false;
		v[0] = nums[0].ToFloat();
		v[1] = nums[1].ToFloat();
		v[2] = nums[2].ToFloat();
		return true;
	}

	static string VecText(vector v)
	{
		return FloatText(v[0]) + " " + FloatText(v[1]) + " " + FloatText(v[2]);
	}

	// Two decimals, no trailing zeros noise for positions.
	static string FloatText(float f)
	{
		float r = Math.Round(f * 100.0) / 100.0;
		return r.ToString();
	}

	static int ClampI(int v, int lo, int hi)
	{
		if (v < lo)
			return lo;
		if (v > hi)
			return hi;
		return v;
	}

	// NaN-safe float clamp (NaN comparisons are not IEEE in Enforce: only `v != v` detects it).
	static float ClampF(float v, float lo, float hi, float fallback)
	{
		float x = v;
		if (x != x)
			x = fallback;
		if (x < lo)
			return lo;
		if (x > hi)
			return hi;
		return x;
	}

	static string Lower(string s)
	{
		string r = s + "";
		r.ToLower();
		return r;
	}

	// Display name of a config class (vehicles, weapons, magazines, ammo); the class name when nothing is found.
	static string DisplayName(string type)
	{
		string name = "";
		string path = "CfgVehicles " + type + " displayName";
		if (GetGame().ConfigIsExisting(path))
			GetGame().ConfigGetText(path, name);
		if (name == "")
		{
			path = "CfgWeapons " + type + " displayName";
			if (GetGame().ConfigIsExisting(path))
				GetGame().ConfigGetText(path, name);
		}
		if (name == "")
		{
			path = "CfgMagazines " + type + " displayName";
			if (GetGame().ConfigIsExisting(path))
				GetGame().ConfigGetText(path, name);
		}
		if (name == "")
			name = type;
		return name;
	}

	// True when the class exists in any of the item/entity config roots.
	static bool ClassExists(string type)
	{
		if (type == "")
			return false;
		if (GetGame().ConfigIsExisting("CfgVehicles " + type))
			return true;
		if (GetGame().ConfigIsExisting("CfgWeapons " + type))
			return true;
		if (GetGame().ConfigIsExisting("CfgMagazines " + type))
			return true;
		return false;
	}
}
