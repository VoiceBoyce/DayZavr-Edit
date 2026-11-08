// INT GlobalChat — text metrics and line layout (spec.md §5.4). A chat line is wrapped in script into visual rows
// with engine metrics: a probe TextWidget measures every word once (cached per font size), rows are whole
// TextWidgets, so scrolling moves whole rows (clipchildren does not cut text, measured 2026-10-12).

class INT_GC_Measure
{
	protected static TextWidget s_Probe;
	protected static ref map<string, float> s_Cache;
	protected static float s_Factor;
	static int s_Measured;		// engine measurements (self-test / perf)

	// The probe lives in the HUD root (transparent, wide box so long words are not clipped).
	static void Attach(Widget host)
	{
		if (s_Probe)
			s_Probe.Unlink();
		s_Probe = null;
		if (!host)
			return;
		s_Probe = INT_UIF_Draw.Text(host, 0, 0, 4000, 40, "", 16, ARGB(0, 0, 0, 0));
		if (s_Probe)
			s_Probe.SetName("INT_GC_Probe");
		ClearCache();
	}

	static void Detach()
	{
		if (s_Probe)
			s_Probe.Unlink();
		s_Probe = null;
	}

	static void ClearCache()
	{
		s_Cache = new map<string, float>();
		s_Factor = INT_UIF_Scale.Factor();
	}

	// Width of a single-line string in design px at glyph size `size`.
	static float Width(string s, float size)
	{
		if (s == "")
			return 0;
		if (!s_Cache || s_Factor != INT_UIF_Scale.Factor())
			ClearCache();
		int sz = Math.Round(size * 10);
		string key = sz.ToString() + "|" + s;
		float cached;
		if (s_Cache.Find(key, cached))
			return cached;
		float w = INT_UIF_Draw.TextWidth(s, size) * 1.04;
		if (s_Probe)
		{
			int px = INT_UIF_Scale.TextPx(size);
			s_Probe.SetTextExactSize(px);
			s_Probe.SetText(s);
			s_Probe.Update();
			int mw;
			int mh;
			s_Probe.GetTextSize(mw, mh);
			float f = INT_UIF_Scale.Factor();
			if (mw > 0 && f > 0)
				w = mw / f;
			s_Measured++;
		}
		if (s_Cache.Count() > 6000)
			ClearCache();
		s_Cache.Set(key, w);
		return w;
	}

	static float SpaceWidth(float size)
	{
		float ab = Width("ab", size);
		float spaced = Width("a b", size);
		float sp = spaced - ab;
		if (sp < size * 0.18)
			sp = size * 0.28;
		return sp;
	}

	// Largest number of characters of `s` (from the start) whose width fits `maxW`; at least 1.
	static int FitChars(string s, float size, float maxW)
	{
		int n = s.LengthUtf8();
		if (n <= 1)
			return n;
		int lo = 1;
		int hi = n;
		while (lo < hi)
		{
			int mid = (lo + hi + 1) / 2;
			string part = s.SubstringUtf8(0, mid);
			float pw = Width(part, size);
			if (pw <= maxW)
				lo = mid;
			else
				hi = mid - 1;
		}
		return lo;
	}
}

class INT_GC_Row
{
	string Text;
	float X;
}

// The wrapped form of one INT_GC_Line for a given width / font / time setting.
class INT_GC_LineLayout
{
	static const float PLATE_PAD = 3;

	ref INT_GC_Line Line;
	float Size;
	float Width;
	float RowH;
	bool Plate;			// system-like kinds sit on a soft plate
	string TimeText;
	float TimeX;
	float TimeW;
	string TagText;
	string TagIcon;
	float TagX;
	float TagW;
	bool HasAdmin;
	float AdminX;
	float AdminW;
	string NickText;
	float NickX;
	float NickW;
	float Indent;
	ref array<ref INT_GC_Row> Rows;

	void INT_GC_LineLayout()
	{
		Rows = new array<ref INT_GC_Row>();
	}

	int RowCount()
	{
		return Rows.Count();
	}

	float Height()
	{
		float h = Rows.Count() * RowH;
		if (Plate)
			h = h + PLATE_PAD * 2;
		return h;
	}

	float TopPad()
	{
		if (Plate)
			return PLATE_PAD;
		return 0;
	}

	static float TimeSize(float size)
	{
		return size * 0.76;
	}

	static float TagSize(float size)
	{
		return size * 0.66;
	}

	static bool IsTalk(int kind)
	{
		if (kind == INT_GC_Line.KIND_GLOBAL || kind == INT_GC_Line.KIND_LOCAL)
			return true;
		if (kind == INT_GC_Line.KIND_PM_IN || kind == INT_GC_Line.KIND_PM_OUT)
			return true;
		return false;
	}

	static INT_GC_LineLayout Build(INT_GC_Line line, float width, float size, bool showTime)
	{
		INT_GC_LineLayout L = new INT_GC_LineLayout();
		L.Line = line;
		L.Size = size;
		L.Width = width;
		L.RowH = Math.Round(size * 1.42);
		int kind = line.Kind;
		if (kind == INT_GC_Line.KIND_MOD || kind == INT_GC_Line.KIND_SYSTEM || kind == INT_GC_Line.KIND_NOTICE)
			L.Plate = true;
		// lines with a left bar (plates, mentions, own lines) keep their text off the bar
		float pad = 0;
		if (L.Plate || line.Mention || line.Mine)
			pad = 8;
		float x = pad;
		if (showTime)
		{
			L.TimeText = line.Hour.ToStringLen(2) + ":" + line.Minute.ToStringLen(2);
			L.TimeX = x;
			L.TimeW = INT_GC_Measure.Width(L.TimeText, TimeSize(size));
			x = x + L.TimeW + 8;
		}
		L.Indent = x;
		string tag = "";
		if (line.Tag != "")
			tag = Widget.TranslateString(line.Tag);
		if (kind == INT_GC_Line.KIND_PM_IN || kind == INT_GC_Line.KIND_PM_OUT)
			tag = Widget.TranslateString("#STR_INT_GC_TAG_PM");
		if (kind == INT_GC_Line.KIND_MOD)
			tag = Widget.TranslateString("#STR_INT_GC_TAG_MOD");
		// tags are written in capitals in the stringtable (ToUpper is ASCII only)
		if (tag != "")
		{
			L.TagText = tag;
			L.TagIcon = TagIconOf(line);
			float iconW = 0;
			if (L.TagIcon != "")
				iconW = TagSize(size) + 4;
			L.TagX = x;
			L.TagW = INT_GC_Measure.Width(tag, TagSize(size)) + iconW + 10;
			x = x + L.TagW + 6;
		}
		if (line.Admin && IsTalk(kind))
		{
			L.HasAdmin = true;
			L.AdminX = x;
			string adm = Widget.TranslateString("#STR_INT_GC_ADMIN");
			L.AdminW = INT_GC_Measure.Width(adm, TagSize(size)) + 10;
			x = x + L.AdminW + 5;
		}
		if (IsTalk(kind) && line.Name != "")
		{
			string nick = line.Name + ":";
			if (kind == INT_GC_Line.KIND_PM_OUT)
				nick = "» " + line.Name + ":";
			L.NickText = nick;
			L.NickX = x;
			L.NickW = INT_GC_Measure.Width(nick, size);
			x = x + L.NickW + INT_GC_Measure.SpaceWidth(size);
		}
		float right = width - pad;
		L.Wrap(line.Text, x, right);
		return L;
	}

	static string TagIconOf(INT_GC_Line line)
	{
		int kind = line.Kind;
		if (kind == INT_GC_Line.KIND_MOD)
			return "gc_shield";
		if (kind == INT_GC_Line.KIND_PM_IN || kind == INT_GC_Line.KIND_PM_OUT)
			return "gc_pm";
		if (kind == INT_GC_Line.KIND_NOTICE)
			return "gc_warn";
		if (kind == INT_GC_Line.KIND_LOCAL)
		{
			if (line.Tag == "#STR_INT_GC_TAG_RADIO")
				return "gc_radio";
			if (line.Tag == "#STR_INT_GC_TAG_MEGAPHONE")
				return "gc_megaphone";
			return "gc_local";
		}
		if (kind == INT_GC_Line.KIND_SYSTEM)
		{
			if (line.Tag == "#STR_INT_GC_TAG_ADMIN")
				return "gc_admin";
			return "gc_server";
		}
		return "";
	}

	protected void AddRow(string text, float x)
	{
		INT_GC_Row r = new INT_GC_Row();
		r.Text = text;
		r.X = x;
		Rows.Insert(r);
	}

	// Greedy word wrap with measured words; a word longer than a row is split by characters.
	void Wrap(string text, float startX, float right)
	{
		float space = INT_GC_Measure.SpaceWidth(Size);
		array<string> words = new array<string>();
		text.Split(" ", words);
		string row = "";
		float rowX = startX;
		float rowW = 0;
		foreach (string word : words)
		{
			if (word == "")
				continue;
			string rest = word;
			while (rest != "")
			{
				float ww = INT_GC_Measure.Width(rest, Size);
				float need = ww;
				if (row != "")
					need = ww + space;
				bool fits = rowX + rowW + need <= right;
				// near the edge the sum of word widths may differ from the engine's width of the whole row
				if (fits && row != "" && rowX + rowW + need > right - Size * 2)
				{
					float real = INT_GC_Measure.Width(row + " " + rest, Size);
					if (rowX + real > right)
						fits = false;
					else
						need = real - rowW;
				}
				if (fits)
				{
					if (row != "")
						row = row + " ";
					row = row + rest;
					rowW = rowW + need;
					rest = "";
					continue;
				}
				if (row != "")
				{
					AddRow(row, rowX);
					row = "";
					rowW = 0;
					rowX = Indent;
					continue;
				}
				// an empty row and the word still does not fit: cut it
				float avail = right - rowX;
				int fit = INT_GC_Measure.FitChars(rest, Size, avail);
				string head = rest.SubstringUtf8(0, fit);
				int total = rest.LengthUtf8();
				AddRow(head, rowX);
				rest = rest.SubstringUtf8(fit, total - fit);
				rowX = Indent;
			}
		}
		if (row != "" || Rows.Count() == 0)
			AddRow(row, rowX);
	}
}
