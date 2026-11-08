// INT GlobalChat — banned-word filter (spec.md §5.6, owner D4/D5: client side, whole words).
// The server sends its BannedWords.json list once (WELCOME + WORDS); every word and every token of a message go
// through the same normalisation, then a token form must EQUAL a word form (map lookup):
//   lower case (ASCII + Russian, ё -> е) -> two forms per token: Cyrillic (Latin look-alikes and 0 @ 3 6 4 mapped
//   to Cyrillic) and Latin (Cyrillic look-alikes and 0 @ 1 ! $ 3 4 5 7 mapped to Latin) -> repeated letters
//   collapsed ("маааать" -> "мать"). A token is checked whole without its punctuation ("м.а.т.ь") and part by part
//   ("мать,отца"); three or more one-letter tokens in a row are glued ("м а т ь").
// Cyrillic list words land in the Cyrillic set, Latin words in the Latin set, so "геймер" or "moment" never match
// "гей" / "mom". Client only; the server never reads message text (owner D4).

class INT_GC_Filter
{
	protected static ref map<string, string> s_Lower;		// one upper-case character -> lower case
	protected static ref map<string, string> s_ToCyr;		// lower-case character -> Cyrillic form ("" = drop)
	protected static ref map<string, string> s_ToLat;		// lower-case character -> Latin form
	protected static ref map<string, bool> s_CyrLetters;
	protected static ref map<string, bool> s_LatLetters;

	protected ref map<string, bool> m_Cyr;
	protected ref map<string, bool> m_Lat;
	protected int m_Words;

	void INT_GC_Filter()
	{
		m_Cyr = new map<string, bool>();
		m_Lat = new map<string, bool>();
	}

	// --- tables -------------------------------------------------------------------------------------------------
	protected static void Pair(map<string, string> m, string from, string to)
	{
		int n = from.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string a = from.SubstringUtf8(i, 1);
			string b = to.SubstringUtf8(i, 1);
			m.Set(a, b);
		}
	}

	protected static void Letters(map<string, bool> m, string chars)
	{
		int n = chars.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = chars.SubstringUtf8(i, 1);
			m.Set(c, true);
		}
	}

	static void EnsureTables()
	{
		if (s_Lower)
			return;
		s_Lower = new map<string, string>();
		s_ToCyr = new map<string, string>();
		s_ToLat = new map<string, string>();
		s_CyrLetters = new map<string, bool>();
		s_LatLetters = new map<string, bool>();
		Pair(s_Lower, "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ", "абвгдеежзийклмнопрстуфхцчшщъыьэюя");
		Pair(s_Lower, "ABCDEFGHIJKLMNOPQRSTUVWXYZё", "abcdefghijklmnopqrstuvwxyzе");
		Letters(s_CyrLetters, "абвгдежзийклмнопрстуфхцчшщъыьэюя");
		Letters(s_LatLetters, "abcdefghijklmnopqrstuvwxyz");
		// Latin and digit look-alikes written inside a Cyrillic word
		Pair(s_ToCyr, "acekmhoptxy0@364", "асекмнортхуоазбч");
		// Cyrillic and digit look-alikes written inside a Latin word
		Pair(s_ToLat, "асеокмрхутн0@1!$3457", "aceokmpxythoaiiseast");
	}

	// --- normalisation ------------------------------------------------------------------------------------------
	static string LowerChar(string c)
	{
		string r;
		if (s_Lower.Find(c, r))
			return r;
		return c;
	}

	static string LowerText(string s)
	{
		EnsureTables();
		string r = "";
		int n = s.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = s.SubstringUtf8(i, 1);
			r = r + LowerChar(c);
		}
		return r;
	}

	// One form of a lower-case chunk: map look-alikes into the script, drop everything else, collapse repeats.
	protected static string Form(string lower, bool cyr)
	{
		string r = "";
		string last = "";
		int n = lower.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = lower.SubstringUtf8(i, 1);
			string m = c;
			string mapped;
			if (cyr)
			{
				if (s_ToCyr.Find(c, mapped))
					m = mapped;
				if (!s_CyrLetters.Contains(m))
					continue;
			}
			else
			{
				if (s_ToLat.Find(c, mapped))
					m = mapped;
				if (!s_LatLetters.Contains(m))
					continue;
			}
			if (m == last)
				continue;
			r = r + m;
			last = m;
		}
		return r;
	}

	protected static bool IsWordChar(string c)
	{
		if (s_CyrLetters.Contains(c) || s_LatLetters.Contains(c))
			return true;
		if (s_ToCyr.Contains(c) || s_ToLat.Contains(c))
			return true;
		return false;
	}

	protected static bool HasScript(string lower, bool cyr)
	{
		int n = lower.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = lower.SubstringUtf8(i, 1);
			if (cyr && s_CyrLetters.Contains(c))
				return true;
			if (!cyr && s_LatLetters.Contains(c))
				return true;
		}
		return false;
	}

	// Splits a lower-case whitespace token at characters that are not letters / look-alikes.
	protected static void Parts(string lower, array<string> outParts)
	{
		string cur = "";
		int n = lower.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string c = lower.SubstringUtf8(i, 1);
			if (IsWordChar(c))
			{
				cur = cur + c;
				continue;
			}
			if (cur != "")
				outParts.Insert(cur);
			cur = "";
		}
		if (cur != "")
			outParts.Insert(cur);
	}

	// --- word list ----------------------------------------------------------------------------------------------
	void Clear()
	{
		m_Cyr.Clear();
		m_Lat.Clear();
		m_Words = 0;
	}

	// Adds one list word (any case). A word written in Cyrillic goes to the Cyrillic set, a Latin one to the Latin set.
	bool AddWord(string word)
	{
		EnsureTables();
		string lower = LowerText(word);
		lower.TrimInPlace();
		if (lower == "")
			return false;
		bool added = false;
		if (HasScript(lower, true))
		{
			string fc = Form(lower, true);
			if (fc.LengthUtf8() >= 2)
			{
				m_Cyr.Set(fc, true);
				added = true;
			}
		}
		else if (HasScript(lower, false))
		{
			string fl = Form(lower, false);
			if (fl.Length() >= 2)
			{
				m_Lat.Set(fl, true);
				added = true;
			}
		}
		if (added)
			m_Words++;
		return added;
	}

	void SetWords(array<string> words)
	{
		Clear();
		if (!words)
			return;
		foreach (string w : words)
			AddWord(w);
	}

	int WordCount()
	{
		return m_Words;
	}

	int FormCount()
	{
		return m_Cyr.Count() + m_Lat.Count();
	}

	// --- check --------------------------------------------------------------------------------------------------
	protected bool Hit(string lowerChunk, out string matched)
	{
		string fc = Form(lowerChunk, true);
		if (fc != "" && m_Cyr.Contains(fc))
		{
			matched = fc;
			return true;
		}
		string fl = Form(lowerChunk, false);
		if (fl != "" && m_Lat.Contains(fl))
		{
			matched = fl;
			return true;
		}
		return false;
	}

	// true = the text contains a banned word; `matched` = its normalised form (for the debug log only).
	bool Check(string text, out string matched)
	{
		matched = "";
		if (m_Words == 0 || text == "")
			return false;
		EnsureTables();
		string lowered = LowerText(text);
		string lower = INT_GC_Util.ReplaceByte(lowered, "\t", " ");
		array<string> tokens = new array<string>();
		lower.Split(" ", tokens);
		array<string> parts = new array<string>();
		string glued = "";
		int gluedCount = 0;
		foreach (string tok : tokens)
		{
			if (tok == "")
				continue;
			// whole token without punctuation: "м.а.т.ь", "m@ma"
			if (Hit(tok, matched))
				return true;
			// parts between punctuation: "мать,отца"
			parts.Clear();
			Parts(tok, parts);
			if (parts.Count() > 1)
			{
				foreach (string p : parts)
				{
					if (Hit(p, matched))
						return true;
				}
			}
			// spaced letters: "м а т ь"
			string single = Form(tok, true);
			string singleLat = Form(tok, false);
			if (single.LengthUtf8() == 1 || singleLat.Length() == 1)
			{
				glued = glued + tok;
				gluedCount++;
				continue;
			}
			if (gluedCount >= 3 && Hit(glued, matched))
				return true;
			glued = "";
			gluedCount = 0;
		}
		if (gluedCount >= 3 && Hit(glued, matched))
			return true;
		return false;
	}
}
