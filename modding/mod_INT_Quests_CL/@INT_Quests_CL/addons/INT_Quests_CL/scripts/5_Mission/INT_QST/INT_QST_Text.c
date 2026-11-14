// INT Quests (client) — what the windows, the tracker and the banners SAY about a quest: target lines, reward
// names, status stamps, repeat/limit texts, giver distance, word wrap. Quest texts and giver names are shown as the
// admin wrote them (owner: no translation of quest content); only the frame words come from the stringtable.

class INT_QST_Text
{
	protected static ref map<string, string> s_Names = new map<string, string>();	// class -> display name cache

	// Display name of a class, cached (config lookups are not free and lists rebind while scrolling).
	static string Name(string type)
	{
		if (type == "")
			return "";
		string low = INT_QST_Util.Lower(type);
		string n;
		if (s_Names.Find(low, n))
			return n;
		n = INT_QST_Util.DisplayName(type);
		if (n.IndexOf("#") == 0 || n.IndexOf("$") == 0)
			n = Widget.TranslateString(n);
		s_Names.Set(low, n);
		return n;
	}

	static string ActionName(string type)
	{
		string low = "act:" + INT_QST_Util.Lower(type);
		string n;
		if (s_Names.Find(low, n))
			return n;
		n = type;
		typename tn = type.ToType();
		if (tn && tn.IsInherited(ActionBase))
		{
			ActionBase ab = ActionManagerBase.GetAction(tn);
			if (ab)
			{
				string t = ab.GetText();
				if (t != "")
					n = INT_QST_Look.Tr(t);
			}
		}
		s_Names.Set(low, n);
		return n;
	}

	// --- targets ----------------------------------------------------------------------------------------------------
	static string TargetIcon(int type)
	{
		switch (type)
		{
			case INT_QST_TT.KILL:
				return "skull";
			case INT_QST_TT.DELIVER:
				return "box";
			case INT_QST_TT.SEARCH:
				return "search";
			case INT_QST_TT.CRAFT:
				return "hammer";
			case INT_QST_TT.FISH:
				return "fish";
			case INT_QST_TT.HARVEST:
				return "wheat";
			case INT_QST_TT.EXPLORE:
				return "map_pin";
			case INT_QST_TT.ACTION:
				return "action";
		}
		return "info";
	}

	// The target line ("Убить: Волк из Mosin 9130") — the admin's own text wins.
	static string Target(INT_QST_TargetDef t)
	{
		if (t.Text != "")
			return t.Text;
		string what = Name(t.Target);
		switch (t.m_Type)
		{
			case INT_QST_TT.KILL:
				if (t.Filter != "")
				{
					string wpn = Name(t.Filter);
					return INT_QST_Look.Fmt("#STR_INT_QST_T_KILL_WITH", what, wpn);
				}
				return INT_QST_Look.Fmt("#STR_INT_QST_T_KILL", what);
			case INT_QST_TT.DELIVER:
				return INT_QST_Look.Fmt("#STR_INT_QST_T_DELIVER", what);
			case INT_QST_TT.SEARCH:
				return INT_QST_Look.Fmt("#STR_INT_QST_T_SEARCH", what);
			case INT_QST_TT.CRAFT:
				return INT_QST_Look.Fmt("#STR_INT_QST_T_CRAFT", what);
			case INT_QST_TT.FISH:
				return INT_QST_Look.Fmt("#STR_INT_QST_T_FISH", what);
			case INT_QST_TT.HARVEST:
				return INT_QST_Look.Fmt("#STR_INT_QST_T_HARVEST", what);
			case INT_QST_TT.EXPLORE:
				return INT_QST_Look.Tr("#STR_INT_QST_T_EXPLORE");
			case INT_QST_TT.ACTION:
				string act = ActionName(t.Target);
				if (t.Filter != "")
				{
					string obj = Name(t.Filter);
					return INT_QST_Look.Fmt("#STR_INT_QST_T_ACTION_ON", act, obj);
				}
				return INT_QST_Look.Fmt("#STR_INT_QST_T_ACTION", act);
		}
		return what;
	}

	// Second line under a target card: conditions (state, quantity, zone, taken or only checked).
	static string TargetCond(INT_QST_TargetDef t)
	{
		string r = "";
		if (t.m_Type == INT_QST_TT.DELIVER)
			r = INT_QST_Look.Tr("#STR_INT_QST_C_TAKEN");
		else if (t.m_Type == INT_QST_TT.SEARCH)
			r = INT_QST_Look.Tr("#STR_INT_QST_C_CHECKED");
		if (t.m_Type == INT_QST_TT.DELIVER || t.m_Type == INT_QST_TT.SEARCH)
		{
			if (t.m_Worst >= 0)
				r = Join(r, INT_QST_Look.Fmt("#STR_INT_QST_C_STATE", StateName(t.m_Worst)));
			if (t.MinQuantity > 0)
				r = Join(r, INT_QST_Look.Fmt("#STR_INT_QST_C_QTY", t.MinQuantity.ToString()));
		}
		if (t.m_Type == INT_QST_TT.KILL && t.m_HasZone)
		{
			string zr = INT_QST_Look.Meters(t.ZoneRadius);
			r = Join(r, INT_QST_Look.Fmt("#STR_INT_QST_C_ZONE", zr));
		}
		if (t.m_Type == INT_QST_TT.EXPLORE)
		{
			string rr = INT_QST_Look.Meters(t.Radius);
			r = Join(r, INT_QST_Look.Fmt("#STR_INT_QST_C_RADIUS", rr));
		}
		return r;
	}

	static string StateName(int level)
	{
		switch (level)
		{
			case 0:
				return INT_QST_Look.Tr("#STR_INT_QST_S_PRISTINE");
			case 1:
				return INT_QST_Look.Tr("#STR_INT_QST_S_WORN");
			case 2:
				return INT_QST_Look.Tr("#STR_INT_QST_S_DAMAGED");
			case 3:
				return INT_QST_Look.Tr("#STR_INT_QST_S_BADLY");
		}
		return INT_QST_Look.Tr("#STR_INT_QST_S_RUINED");
	}

	protected static string Join(string a, string b)
	{
		if (a == "")
			return b;
		if (b == "")
			return a;
		return a + " · " + b;
	}

	// --- rewards ------------------------------------------------------------------------------------------------------
	static string RewardName(INT_QST_RewardDef r)
	{
		if (r.m_Type == INT_QST_RT.MONEY)
			return INT_QST_Look.Money(r.Amount);
		if (r.Text != "")
			return r.Text;
		return Name(r.Item);
	}

	static string RewardIcon(INT_QST_RewardDef r)
	{
		if (r.m_Type == INT_QST_RT.MONEY)
			return "bank";
		return "gift";
	}

	// --- status -------------------------------------------------------------------------------------------------------
	static string StatusKey(int st)
	{
		switch (st)
		{
			case INT_QST_St.AVAILABLE:
				return "#STR_INT_QST_ST_NEW";
			case INT_QST_St.ACTIVE:
				return "#STR_INT_QST_ST_ACTIVE";
			case INT_QST_St.READY:
				return "#STR_INT_QST_ST_READY";
			case INT_QST_St.LOCKED:
				return "#STR_INT_QST_ST_LOCKED";
			case INT_QST_St.COOLDOWN:
				return "#STR_INT_QST_ST_COOLDOWN";
			case INT_QST_St.DONE:
				return "#STR_INT_QST_ST_DONE";
			case INT_QST_St.EXPIRED:
				return "#STR_INT_QST_ST_EXPIRED";
		}
		return "#STR_INT_QST_ST_HIDDEN";
	}

	// Blue stamp = in progress / new, red = ready / expired / done; grey = closed.
	static int StatusColor(int st)
	{
		switch (st)
		{
			case INT_QST_St.READY:
			case INT_QST_St.EXPIRED:
			case INT_QST_St.DONE:
				return INT_QST_Look.Red();
			case INT_QST_St.LOCKED:
			case INT_QST_St.COOLDOWN:
			case INT_QST_St.HIDDEN:
				return INT_QST_Look.Ink3();
		}
		return INT_QST_Look.Blue();
	}

	// --- repeat / limit -----------------------------------------------------------------------------------------------
	static string Repeat(INT_QST_QuestDef q)
	{
		string hm = Clock(q.ResetHour, q.ResetMinute);
		switch (q.m_Repeat)
		{
			case INT_QST_RM.COOLDOWN:
				string cd = INT_QST_Look.Dur(q.Cooldown);
				return INT_QST_Look.Fmt("#STR_INT_QST_R_COOLDOWN", cd);
			case INT_QST_RM.DAILY:
				return INT_QST_Look.Fmt("#STR_INT_QST_R_DAILY", hm);
			case INT_QST_RM.WEEKLY:
				string day = INT_QST_Look.Tr("#STR_INT_QST_WD_" + q.ResetWeekDay.ToString());
				return INT_QST_Look.Fmt("#STR_INT_QST_R_WEEKLY", day, hm);
		}
		return INT_QST_Look.Tr("#STR_INT_QST_R_ONCE");
	}

	static string Limit(INT_QST_QuestDef q)
	{
		if (q.TimeLimit <= 0)
			return INT_QST_Look.Tr("#STR_INT_QST_L_NONE");
		return INT_QST_Look.Dur(q.TimeLimit);
	}

	static string Clock(int h, int m)
	{
		string hs = h.ToString();
		if (h < 10)
			hs = "0" + hs;
		string ms = m.ToString();
		if (m < 10)
			ms = "0" + ms;
		return hs + ":" + ms;
	}

	// --- givers -------------------------------------------------------------------------------------------------------
	static string GiverName(string key)
	{
		if (key == "")
			return "";
		return INT_QST_Client.GiverTitle(key);
	}

	// Distance from the player to a giver ("" when unknown).
	static string GiverDistance(string key)
	{
		INT_QST_GiverView g = INT_QST_Client.Giver(key);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!g || !p)
			return "";
		vector gp = g.Position;
		if (gp == vector.Zero)
			return "";
		vector pp = p.GetPosition();
		float d = vector.Distance(pp, gp);
		return INT_QST_Look.Meters(d);
	}

	// Who gives / takes it, for the sheet: the journal itself when the quest has no giver (owner decision O13).
	static string StartText(INT_QST_QuestDef q)
	{
		if (q.GiverStart == "")
			return INT_QST_Look.Tr("#STR_INT_QST_G_JOURNAL");
		return GiverName(q.GiverStart);
	}

	static string FinishText(INT_QST_QuestDef q)
	{
		if (q.GiverFinish == "")
			return INT_QST_Look.Tr("#STR_INT_QST_G_AUTO");
		return GiverName(q.GiverFinish);
	}

	// --- word wrap ----------------------------------------------------------------------------------------------------
	// Splits text into lines that fit `width` design px at glyph `size` (Metron estimate, INT_UIF_Draw.TextWidth).
	// "\n" in the text starts a new line; a word longer than a line is cut.
	static void Wrap(string text, float width, float size, array<string> lines)
	{
		lines.Clear();
		float perChar = size * 0.53;
		int maxChars = (int)(width / perChar);
		if (maxChars < 8)
			maxChars = 8;
		string src = text;
		array<string> paras = new array<string>();
		src.Split("\n", paras);
		foreach (string para : paras)
		{
			string pt = para.Trim();
			array<string> words = new array<string>();
			pt.Split(" ", words);
			string line = "";
			int lineLen = 0;
			foreach (string word : words)
			{
				string w = word;
				if (w == "")
					continue;
				int wl = w.LengthUtf8();
				while (wl > maxChars)
				{
					if (lineLen > 0)
					{
						lines.Insert(line);
						line = "";
						lineLen = 0;
					}
					lines.Insert(w.SubstringUtf8(0, maxChars));
					w = w.SubstringUtf8(maxChars, wl - maxChars);
					wl = w.LengthUtf8();
				}
				if (lineLen == 0)
				{
					line = w;
					lineLen = wl;
				}
				else if (lineLen + 1 + wl <= maxChars)
				{
					line = line + " " + w;
					lineLen = lineLen + 1 + wl;
				}
				else
				{
					lines.Insert(line);
					line = w;
					lineLen = wl;
				}
			}
			lines.Insert(line);
		}
		// no empty tail
		while (lines.Count() > 1 && lines[lines.Count() - 1] == "")
			lines.Remove(lines.Count() - 1);
	}
}
