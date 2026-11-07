// INT_PDA — script log (owner rules 16, 19). One short line per call (the script log cuts every Print at 255
// characters); '%' is made harmless because a bare '%' in Print kills the client (measured 2026-10-13).
// Tag "[INT_PDA]" on the client, "[INT_PDA_SE]" on the server. ОТЛАДКА lines only with "DebugMode": 1 in the
// server config (clients follow the debug flag of WELCOME). Times are the local time of the machine (rule 19).

class INT_PDA_Log
{
	static bool s_Debug;
	static string s_Tag = "[INT_PDA]";

	static string Safe(string s)
	{
		string r = INT_PDA_Util.ReplaceByte(s, "%", " pct");
		r = INT_PDA_Util.ReplaceByte(r, "\n", " ");
		return INT_PDA_Util.ReplaceByte(r, "\r", " ");
	}

	// Player-controlled text for a log line: one line, at most maxChars characters.
	static string Clip(string s, int maxChars)
	{
		string r = s + "";
		if (r.LengthUtf8() > maxChars)
			r = r.SubstringUtf8(0, maxChars) + "~";
		return r;
	}

	static void Write(string level, string text)
	{
		string line = s_Tag + " " + INT_PDA_Util.Stamp() + " " + level + " | " + text;
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

class INT_PDA_Util
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

	// "YYYY-MM-DD" (log file names).
	static string Day()
	{
		int y;
		int mo;
		int d;
		GetYearMonthDay(y, mo, d);
		return y.ToString() + "-" + mo.ToStringLen(2) + "-" + d.ToStringLen(2);
	}

	// UTC seconds since 2020-01-01 00:00 (lifetimes that must survive restarts: GetTime() starts again at 0).
	static int NowSec()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDayUTC(y, mo, d);
		GetHourMinuteSecondUTC(h, mi, s);
		return DaysSince2020(y, mo, d) * 86400 + h * 3600 + mi * 60 + s;
	}

	static int DaysSince2020(int y, int mo, int d)
	{
		int days = 0;
		int yy;
		for (yy = 2020; yy < y; yy++)
		{
			days = days + 365;
			if (IsLeap(yy))
				days = days + 1;
		}
		array<int> monthDays = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		for (int m = 1; m < mo && m <= 12; m++)
		{
			days = days + monthDays[m - 1];
			if (m == 2 && IsLeap(y))
				days = days + 1;
		}
		return days + d - 1;
	}

	static bool IsLeap(int y)
	{
		if (y % 400 == 0)
			return true;
		if (y % 100 == 0)
			return false;
		return y % 4 == 0;
	}

	// "YYYY-MM-DD_HH-MM-SS" for file names.
	static string FileStamp()
	{
		string s = Stamp();
		s.Replace(" ", "_");
		s.Replace(":", "-");
		return s;
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

	// One line of player text: control characters to spaces, trimmed, at most maxChars characters and maxBytes bytes.
	static string CleanLine(string s, int maxChars, int maxBytes)
	{
		string r = FitBytes(s, 4096);
		r = ReplaceByte(r, "\r", " ");
		r = ReplaceByte(r, "\n", " ");
		r = ReplaceByte(r, "\t", " ");
		// invisible look-alikes: NBSP becomes a space; zero-width space / non-joiner / joiner, word joiner and BOM go
		// (the literals below hold those characters) - a message made only of them stays empty and is refused
		r = ReplaceByte(r, " ", " ");
		r = ReplaceByte(r, "​", "");
		r = ReplaceByte(r, "‌", "");
		r = ReplaceByte(r, "‍", "");
		r = ReplaceByte(r, "⁠", "");
		r = ReplaceByte(r, "﻿", "");
		r.TrimInPlace();
		if (r.LengthUtf8() > maxChars)
			r = r.SubstringUtf8(0, maxChars);
		return FitBytes(r, maxBytes);
	}

	// Callsign rule (owner 2026-10-22): Latin letters A-Z only, single spaces or hyphens between letters allowed,
	// no digits, no Cyrillic, CALLSIGN_MIN..CALLSIGN_MAX characters.
	static bool IsCallsign(string s)
	{
		int n = s.Length();
		if (n < INT_PDA_Const.CALLSIGN_MIN || n > INT_PDA_Const.CALLSIGN_MAX)
			return false;
		int i;
		int c;
		int prev = 32;
		for (i = 0; i < n; i++)
		{
			c = s.Get(i).ToAscii();
			if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
			{
				prev = c;
				continue;
			}
			if (c == 32 || c == 45)
			{
				if (i == 0 || i == n - 1 || prev == 32 || prev == 45)
					return false;
				prev = c;
				continue;
			}
			return false;
		}
		return true;
	}

	// Key of a callsign for the name index: lower case, spaces as '_'.
	// Lower case for comparisons of free text (faction names): string.ToLower is ASCII only, Cyrillic is mapped here.
	protected static ref array<string> s_Up;
	protected static ref array<string> s_Low;

	static string LowerAll(string s)
	{
		string r = s + "";
		r.ToLower();
		if (!s_Up)
		{
			s_Up = new array<string>();
			s_Low = new array<string>();
			string up = "А Б В Г Д Е Ё Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я І Ї Є Ґ Ў";
			string low = "а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я і ї є ґ ў";
			up.Split(" ", s_Up);
			low.Split(" ", s_Low);
		}
		int n = s_Up.Count();
		if (s_Low.Count() < n)
			n = s_Low.Count();
		for (int i = 0; i < n; i++)
		{
			if (r.IndexOf(s_Up[i]) >= 0)
				r.Replace(s_Up[i], s_Low[i]);
		}
		r.TrimInPlace();
		return r;
	}

	static string CallsignKey(string s)
	{
		string k = s + "";
		k.ToLower();
		k.Replace(" ", "_");
		return k;
	}

	// Password: printable ASCII without spaces (the owner's admins read it in the admin tab), PASSWORD_MIN..MAX.
	static bool IsPassword(string s)
	{
		int n = s.Length();
		if (n < INT_PDA_Const.PASSWORD_MIN || n > INT_PDA_Const.PASSWORD_MAX)
			return false;
		int i;
		int c;
		for (i = 0; i < n; i++)
		{
			c = s.Get(i).ToAscii();
			if (c < 33 || c > 126)
				return false;
		}
		return true;
	}

	// Steam64: 17 digits starting with 7656.
	static bool IsSteamId(string s)
	{
		if (s.Length() != 17 || s.IndexOf("7656") != 0)
			return false;
		int i;
		int c;
		for (i = 0; i < 17; i++)
		{
			c = s.Get(i).ToAscii();
			if (c < 48 || c > 57)
				return false;
		}
		return true;
	}

	// NaN-safe float (NaN comparisons are not IEEE in Enforce: `NaN >= 0` is true; only `v != v` detects it).
	static float Num(float v, float fallback)
	{
		if (v != v)
			return fallback;
		return v;
	}

	static float ClampF(float v, float lo, float hi, float fallback)
	{
		float x = Num(v, fallback);
		if (x < lo)
			return lo;
		if (x > hi)
			return hi;
		return x;
	}

	static int ClampI(int v, int lo, int hi)
	{
		if (v < lo)
			return lo;
		if (v > hi)
			return hi;
		return v;
	}

	// 0/1 from any int (JSON bools are written as numbers).
	static int Bit(int v)
	{
		if (v != 0)
			return 1;
		return 0;
	}
}
