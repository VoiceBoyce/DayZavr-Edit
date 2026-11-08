// INT PartyMode — text helpers of the C «ЭКСПЕДИЦИЯ» look (ported from the INT_FrameworkSkills util class).

class INT_PTY_Text
{
	protected static ref array<string> s_Lower;
	protected static ref array<string> s_Upper;

	// Upper case for the Expedition captions: ASCII via ToUpper(), then Cyrillic and the European letters of
	// the 13 stringtable languages by table (ToUpper() is not documented to handle UTF-8).
	static string Upper(string value)
	{
		string r = value + "";
		if (r.IndexOf("#STR_") == 0)
			r = Widget.TranslateString(r);
		r.ToUpper();
		EnsureCaseTable();
		int n = s_Lower.Count();
		for (int i = 0; i < n; i++)
		{
			string lo = s_Lower.Get(i);
			string up = s_Upper.Get(i);
			r.Replace(lo, up);
		}
		return r;
	}

	protected static void EnsureCaseTable()
	{
		if (s_Lower)
			return;
		s_Lower = new array<string>();
		s_Upper = new array<string>();
		string lo = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяàáâãäåæçèéêëìíîïñòóôõöøùúûüýÿąćęłńśźżčďěňřšťůžőűğışі";
		string up = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝŸĄĆĘŁŃŚŹŻČĎĚŇŘŠŤŮŽŐŰĞIŞІ";
		int n = lo.LengthUtf8();
		int m = up.LengthUtf8();
		if (m < n)
			n = m;
		for (int i = 0; i < n; i++)
		{
			s_Lower.Insert(lo.SubstringUtf8(i, 1));
			s_Upper.Insert(up.SubstringUtf8(i, 1));
		}
	}

	// "НАВЫКИ" -> "Н А В Ы К И": DayZ text widgets have no letter spacing, a space between letters gives
	// the tracked capitals of the Expedition design at small sizes; a word gap becomes three spaces.
	static string Spaced(string value)
	{
		string t = value;
		if (t.IndexOf("#STR_") == 0)
			t = Widget.TranslateString(t);
		string r = "";
		int n = t.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			if (i > 0)
				r = r + " ";
			if (ch == " ")
				r = r + " ";
			else
				r = r + ch;
		}
		return r;
	}

	static string Caps(string value)
	{
		return Spaced(Upper(value));
	}

	static string Pad2(int v)
	{
		if (v >= 0 && v < 10)
			return "0" + v.ToString();
		return v.ToString();
	}

	// Full-width glyphs (Japanese / Chinese UI) are ~1 em wide (measured on the stand 2026-10-23: 1.0 em at 16 and
	// 23 px), while every width estimate of the mod assumed European letters (0.56–0.65 em) and laid CJK texts on top
	// of each other. A 3-byte UTF-8 character is full-width unless it is a narrow punctuation mark of the European
	// translations.
	static const string NARROW3 = "—…„“”‘’‚–•→←№€™";

	static bool IsWide(string ch)
	{
		if (ch.Length() < 3)
			return false;
		return NARROW3.IndexOf(ch) < 0;
	}

	// How many full-width characters `t` has (0 at once for plain ASCII).
	static int WideCount(string t)
	{
		int n = t.LengthUtf8();
		if (t.Length() == n)
			return 0;
		int wide = 0;
		for (int i = 0; i < n; i++)
		{
			string ch = t.SubstringUtf8(i, 1);
			if (IsWide(ch))
				wide++;
		}
		return wide;
	}

	// "#STR_X" -> translated text, plain text unchanged.
	static string Tr(string value)
	{
		if (value.IndexOf("#STR_") == 0)
			return Widget.TranslateString(value);
		return value;
	}

	// Translate a key and fill %1 / %2.
	static string Fmt(string value, string a1, string a2 = "")
	{
		string t = Tr(value);
		t.Replace("%1", a1);
		t.Replace("%2", a2);
		return t;
	}
}
