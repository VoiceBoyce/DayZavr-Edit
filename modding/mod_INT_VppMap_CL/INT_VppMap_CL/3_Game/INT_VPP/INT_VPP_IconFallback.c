// INT_VPP (2026-11-13, owner): icon fallback for markers whose icon would draw BLANK.
// Since 2026-11-13 the RF bridge is built into the map (CfgMods RF_VanillaPlusPlusMap_CL in config.cpp), so RF event
// markers (airdrop, convoy, black trader, ...) reach every server that runs RF event mods with our map. Their icon path
// comes from the server's RF config (RF\<MOD>\MAPS\vanillaPlusPlusMapConfig.json) and may name a texture that no loaded
// mod has (server 21: sm_proxy\scripts\gui\CustomMapIcons\AieDrop.paa, BlackTrader.paa): the engine then draws nothing.
// Rule: a path that loads stays EXACTLY as it is - our 60 icons, the old VPP icons (INT_IsLegacyPath) and any other
// texture FileExist finds (other mods, vanilla). An empty path or one that does not load gets one of our icons, picked
// by words of the marker's name and of the path (airdrop -> parachute, trader -> trader, convoy -> truck, ...), else
// our default icon. Only what is DRAWN changes: the MarkerInfo keeps its path (its owner may compare it).
// A path with a leading separator ("\dz\...") never loads in a widget (measured 164200): its form without the leading
// separator is used when that one exists.
// 3_Game, no widgets; one FileExist per new path (cached, at most INT_CACHE_MAX paths).
// Every internal call is qualified (INT_VPP_IconFallback.X): bare calls can bind to a class of the same name.

class INT_VPP_IconFallback
{
	static const int INT_CACHE_MAX = 256;
	// path as given -> path to draw ("" = does not load)
	protected static ref map<string, string> s_INT_Loadable;
	protected static ref array<string> s_INT_Words;
	protected static ref array<string> s_INT_WordIds;
	// TEST counters
	protected static int s_INT_FileChecks;
	protected static int s_INT_Fallbacks;

	// the path to draw for a marker icon `path` of a marker named `name`
	static string INT_Resolve(string path, string name)
	{
		string p = path + "";
		p.TrimInPlace();
		if (p == "")
		{
			s_INT_Fallbacks++;
			string byNameOnly = INT_VPP_IconFallback.INT_ByWords("", name);
			return byNameOnly;
		}
		if (INT_VPP_Icons.INT_IsOwnPath(p))
			return path;
		if (INT_VPP_Icons.INT_IsLegacyPath(p))
		{
			string firstL = p.Substring(0, 1);
			if (firstL != "/" && firstL != "\\")
				return path;
		}
		string drawn = INT_VPP_IconFallback.INT_Loadable(p);
		if (drawn != "")
		{
			if (drawn == p)
				return path;
			return drawn;
		}
		s_INT_Fallbacks++;
		string byWords = INT_VPP_IconFallback.INT_ByWords(p, name);
		return byWords;
	}

	// p itself when it loads, p without its leading separators when only that loads, "" when nothing loads
	static string INT_Loadable(string p)
	{
		if (!s_INT_Loadable)
			s_INT_Loadable = new map<string, string>();
		string hit;
		if (s_INT_Loadable.Find(p, hit))
			return hit;
		string res = "";
		string cut = p + "";
		bool lead = false;
		int guard = 0;
		while (cut.Length() > 0 && guard < 8)
		{
			guard++;
			string first = cut.Substring(0, 1);
			if (first != "/" && first != "\\")
				break;
			lead = true;
			int rest = cut.Length() - 1;
			cut = cut.Substring(1, rest);
		}
		if (cut != "")
		{
			s_INT_FileChecks++;
			if (FileExist(cut))
				res = cut;
		}
		if (!lead && res != "")
			res = p;
		if (s_INT_Loadable.Count() >= INT_CACHE_MAX)
			s_INT_Loadable.Clear();
		s_INT_Loadable.Insert(p, res);
		return res;
	}

	// one of our icon paths for the words of a path and a name (never "")
	static string INT_ByWords(string path, string name)
	{
		string hay = INT_VPP_Icons.INT_NormPath(path);
		string nm = name + "";
		nm.ToLower();
		hay = hay + " " + nm;
		string iconId = INT_VPP_IconFallback.INT_IdByWords(hay);
		string res = INT_VPP_Icons.INT_PathOf(iconId);
		if (res == "")
			res = INT_VPP_Icons.INT_DefaultPath();
		return res;
	}

	// icon id for a lower-case haystack (ASCII lowered; Cyrillic words are listed in their usual spellings)
	static string INT_IdByWords(string hay)
	{
		INT_VPP_IconFallback.INT_EnsureWords();
		int n = s_INT_Words.Count();
		for (int i = 0; i < n; i++)
		{
			string w = s_INT_Words.Get(i);
			if (hay.IndexOf(w) >= 0)
				return s_INT_WordIds.Get(i);
		}
		return INT_VPP_Icons.INT_DefaultId();
	}

	protected static void INT_AddWord(string word, string iconId)
	{
		s_INT_Words.Insert(word);
		s_INT_WordIds.Insert(iconId);
	}

	// first match wins: the specific words (convoy, black trader) come before the general ones (heli, trader)
	protected static void INT_EnsureWords()
	{
		if (s_INT_Words)
			return;
		s_INT_Words = new array<string>();
		s_INT_WordIds = new array<string>();
		INT_VPP_IconFallback.INT_AddWord("convoy", "truck");
		INT_VPP_IconFallback.INT_AddWord("конвой", "truck");
		INT_VPP_IconFallback.INT_AddWord("Конвой", "truck");
		INT_VPP_IconFallback.INT_AddWord("КОНВОЙ", "truck");
		INT_VPP_IconFallback.INT_AddWord("blacktrader", "trader");
		INT_VPP_IconFallback.INT_AddWord("black_trader", "trader");
		INT_VPP_IconFallback.INT_AddWord("black trader", "trader");
		INT_VPP_IconFallback.INT_AddWord("trader", "trader");
		INT_VPP_IconFallback.INT_AddWord("торгов", "trader");
		INT_VPP_IconFallback.INT_AddWord("Торгов", "trader");
		INT_VPP_IconFallback.INT_AddWord("ТОРГОВ", "trader");
		INT_VPP_IconFallback.INT_AddWord("airdrop", "parachute");
		INT_VPP_IconFallback.INT_AddWord("aiedrop", "parachute");
		INT_VPP_IconFallback.INT_AddWord("air_drop", "parachute");
		INT_VPP_IconFallback.INT_AddWord("air drop", "parachute");
		INT_VPP_IconFallback.INT_AddWord("parachute", "parachute");
		INT_VPP_IconFallback.INT_AddWord("аирдроп", "parachute");
		INT_VPP_IconFallback.INT_AddWord("Аирдроп", "parachute");
		INT_VPP_IconFallback.INT_AddWord("АИРДРОП", "parachute");
		INT_VPP_IconFallback.INT_AddWord("heli", "heli");
		INT_VPP_IconFallback.INT_AddWord("вертол", "heli");
		INT_VPP_IconFallback.INT_AddWord("Вертол", "heli");
		INT_VPP_IconFallback.INT_AddWord("container", "chest");
		INT_VPP_IconFallback.INT_AddWord("контейнер", "chest");
		INT_VPP_IconFallback.INT_AddWord("Контейнер", "chest");
		INT_VPP_IconFallback.INT_AddWord("radiation", "radiation");
		INT_VPP_IconFallback.INT_AddWord("zombie", "zombie");
	}

	// ---- TEST ----
	static int INT_FileChecks()
	{
		return s_INT_FileChecks;
	}

	static int INT_Fallbacks()
	{
		return s_INT_Fallbacks;
	}

	static void INT_TestClearCache()
	{
		if (s_INT_Loadable)
			s_INT_Loadable.Clear();
	}
}
