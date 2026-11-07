// INT_PDA (server) — what is stored in the files (spec.md §6). Every class has ForRead() (lists pre-created: the
// engine JSON reader gives zero fields to objects it creates) and Clean() (values clamped after a read; a hand edit
// of a file can never crash the server). Zero / empty is always the normal case of a missing field.

// Players/<SteamID>.json or GP/<SteamID>.json
class INT_PDA_Account
{
	int Version = 1;
	string SteamId;
	string Callsign;
	string Password;				// plain: the admins see it in the admin tab (owner brief, admin item 4)
	string Discord;
	int Avatar;
	string Registered;				// "YYYY-MM-DD HH:MM:SS" server time
	string LastSeen;
	string GameName;				// DayZ nick at the last login (admin tab)
	string FactionId;				// "" = no faction (then the file lives in Players/)
	int Role;						// INT_PDA_Const.ROLE_*
	string Rank;
	string Joined;					// joined the faction, "YYYY-MM-DD"
	ref array<string> OldCallsigns;
	string Character;				// survivor class of his new characters («Персонаж» tab), "" = the game decides

	static INT_PDA_Account ForRead()
	{
		INT_PDA_Account a = new INT_PDA_Account();
		a.OldCallsigns = new array<string>();
		return a;
	}

	void Clean()
	{
		if (!OldCallsigns)
			OldCallsigns = new array<string>();
		while (OldCallsigns.Count() > 10)
			OldCallsigns.RemoveOrdered(0);
		Avatar = INT_PDA_Util.ClampI(Avatar, 0, INT_PDA_Const.AVATARS - 1);
		Role = INT_PDA_Util.ClampI(Role, INT_PDA_Const.ROLE_NONE, INT_PDA_Const.ROLE_LEADER);
		Discord = INT_PDA_Util.CleanLine(Discord, INT_PDA_Const.DISCORD_MAX, INT_PDA_Const.MAX_NAME_BYTES);
		Rank = INT_PDA_Util.CleanLine(Rank, 24, INT_PDA_Const.MAX_NAME_BYTES);
		if (FactionId == "")
		{
			Role = INT_PDA_Const.ROLE_NONE;
			Rank = "";
		}
		else if (Role == INT_PDA_Const.ROLE_NONE)
		{
			Role = INT_PDA_Const.ROLE_MEMBER;
		}
	}

	bool InFaction()
	{
		return FactionId != "";
	}
}

// Names/<callsign key>.json — the index that keeps callsigns unique and finds a player by callsign without a scan.
class INT_PDA_NameRef
{
	string SteamId;
	string Callsign;
	int ReservedUntil;			// 0 = his current callsign; > 0 = an old one kept for him until then (UTC s, NowSec)
}

// Factions/<id>.json
class INT_PDA_FactionData
{
	int Version = 1;
	string Id;						// file name, Latin letters/digits/underscore
	string Name;
	int Emblem;						// 1..30 (design/emblems)
	string Motto;
	string Base;					// free text: where the base is
	string Created;
	ref array<string> Ranks;
	ref array<ref INT_PDA_Member> Members;		// leader / deputies are members with their Role
	ref array<ref INT_PDA_Mark> Marks;
	int NextMarkId;

	static INT_PDA_FactionData ForRead()
	{
		INT_PDA_FactionData f = new INT_PDA_FactionData();
		f.Ranks = new array<string>();
		f.Members = new array<ref INT_PDA_Member>();
		f.Marks = new array<ref INT_PDA_Mark>();
		return f;
	}

	void Clean()
	{
		if (!Ranks)
			Ranks = new array<string>();
		if (!Members)
			Members = new array<ref INT_PDA_Member>();
		if (!Marks)
			Marks = new array<ref INT_PDA_Mark>();
		Name = INT_PDA_Util.CleanLine(Name, 32, INT_PDA_Const.MAX_NAME_BYTES);
		Motto = INT_PDA_Util.CleanLine(Motto, 120, 400);
		Base = INT_PDA_Util.CleanLine(Base, 60, 200);
		Emblem = INT_PDA_Util.ClampI(Emblem, 1, INT_PDA_Const.EMBLEMS);
		int i;
		for (i = Members.Count() - 1; i >= 0; i--)
		{
			INT_PDA_Member m = Members[i];
			if (!m || !INT_PDA_Util.IsSteamId(m.SteamId))
			{
				Members.Remove(i);
				continue;
			}
			m.Role = INT_PDA_Util.ClampI(m.Role, INT_PDA_Const.ROLE_MEMBER, INT_PDA_Const.ROLE_LEADER);
		}
		for (i = Marks.Count() - 1; i >= 0; i--)
		{
			if (!Marks[i])
				Marks.Remove(i);
		}
	}

	INT_PDA_Member FindMember(string steam)
	{
		foreach (INT_PDA_Member m : Members)
		{
			if (m.SteamId == steam)
				return m;
		}
		return null;
	}

	int MemberIndex(string steam)
	{
		for (int i = 0; i < Members.Count(); i++)
		{
			if (Members[i].SteamId == steam)
				return i;
		}
		return -1;
	}
}

// Spawn/Default.json and Spawn/<faction id>.json — the owner's format from the brief (field names kept as given, so
// files of the old setup can be reused). m_SpawnPosList: "x, y, z" strings, one is picked at random.
// m_OwnerSteamID: "EVERYONE" (or empty) = the set is for everybody, otherwise only for these SteamIDs (comma list).
class INT_PDA_SpawnSet
{
	string m_HeadItem;
	string m_TopItem;
	string m_PantsItem;
	string m_BootsItem;
	string m_RukzakItem;
	string m_PerchatkiItem;
	ref array<string> m_SpawnPosList;
	ref array<string> m_DopList;
	string m_OwnerSteamID;

	static INT_PDA_SpawnSet ForRead()
	{
		INT_PDA_SpawnSet s = new INT_PDA_SpawnSet();
		s.m_SpawnPosList = new array<string>();
		s.m_DopList = new array<string>();
		return s;
	}

	void Clean()
	{
		if (!m_SpawnPosList)
			m_SpawnPosList = new array<string>();
		if (!m_DopList)
			m_DopList = new array<string>();
		if (m_OwnerSteamID == "")
			m_OwnerSteamID = "EVERYONE";
	}

	bool IsFor(string steam)
	{
		string owner = m_OwnerSteamID + "";
		owner.ToUpper();
		if (owner == "" || owner == "EVERYONE")
			return true;
		return m_OwnerSteamID.IndexOf(steam) >= 0;
	}

	// Clothes or items to give (points and owners aside).
	bool HasGear()
	{
		if (m_HeadItem != "" || m_TopItem != "" || m_PantsItem != "" || m_BootsItem != "" || m_RukzakItem != "" || m_PerchatkiItem != "")
			return true;
		return m_DopList.Count() > 0;
	}

	// The same clothes and the same items in the same order.
	bool SameGear(INT_PDA_SpawnSet o)
	{
		if (!o)
			return false;
		if (m_HeadItem != o.m_HeadItem || m_TopItem != o.m_TopItem || m_PantsItem != o.m_PantsItem)
			return false;
		if (m_BootsItem != o.m_BootsItem || m_RukzakItem != o.m_RukzakItem || m_PerchatkiItem != o.m_PerchatkiItem)
			return false;
		if (m_DopList.Count() != o.m_DopList.Count())
			return false;
		for (int i = 0; i < m_DopList.Count(); i++)
		{
			if (m_DopList[i] != o.m_DopList[i])
				return false;
		}
		return true;
	}

	void ClearGear()
	{
		m_HeadItem = "";
		m_TopItem = "";
		m_PantsItem = "";
		m_BootsItem = "";
		m_RukzakItem = "";
		m_PerchatkiItem = "";
		m_DopList.Clear();
	}

	// The owner's example from the brief, with our PDA classes.
	static INT_PDA_SpawnSet DefaultSet()
	{
		INT_PDA_SpawnSet s = ForRead();
		s.m_HeadItem = "aod_headgear_neytral_baseballcap";
		s.m_TopItem = "aod_body_loner2_m65";
		s.m_PantsItem = "aod_pants_loner_jeans";
		s.m_BootsItem = "TTSKOBoots";
		s.m_RukzakItem = "AssaultBag_Green";
		s.m_PerchatkiItem = "WoolGlovesFingerless_Green";
		// no points: vanilla spawn points until the owner writes his own ("x, y, z" each; the brief's example point is in
		// the sea on Chernarus)
		s.m_DopList.Insert("SFP_Food_DoshirakRed_pack");
		s.m_DopList.Insert("SFP_Food_Kolbasa");
		s.m_DopList.Insert("PurificationTablets");
		s.m_DopList.Insert("Canteen");
		s.m_DopList.Insert("HuntingKnife");
		s.m_DopList.Insert("SFP_Makarom_Apoc");
		s.m_DopList.Insert("Mag_IJ70_8Rnd");
		s.m_DopList.Insert("Mag_IJ70_8Rnd");
		s.m_DopList.Insert("AmmoBox_380_35rnd");
		s.m_DopList.Insert("AmmoBox_380_35rnd");
		s.m_DopList.Insert("AMS_Bandage");
		s.m_DopList.Insert("AMS_Bandage");
		s.m_DopList.Insert("INT_PDA");
		s.m_DopList.Insert("INT_PDA_Module_GPS");
		s.m_DopList.Insert("INT_PDA_Module_MAP");
		s.m_DopList.Insert("INT_PDA_Module_Chat");
		s.m_DopList.Insert("Battery9V");
		s.m_DopList.Insert("Battery9V");
		s.m_DopList.Insert("Flashlight");
		s.m_OwnerSteamID = "EVERYONE";
		return s;
	}
}

// Relations.json — set by the admin only (owner 2026-10-22), with the RP reason.
class INT_PDA_RelationEntry
{
	string A;
	string B;
	int Rel;						// INT_PDA_Const.REL_*
	string Reason;
}

class INT_PDA_RelationsFile
{
	int Version = 1;
	ref array<ref INT_PDA_RelationEntry> Items;

	static INT_PDA_RelationsFile ForRead()
	{
		INT_PDA_RelationsFile f = new INT_PDA_RelationsFile();
		f.Items = new array<ref INT_PDA_RelationEntry>();
		return f;
	}

	void Clean()
	{
		if (!Items)
			Items = new array<ref INT_PDA_RelationEntry>();
		for (int i = Items.Count() - 1; i >= 0; i--)
		{
			INT_PDA_RelationEntry e = Items[i];
			if (!e || e.A == "" || e.B == "" || e.A == e.B)
			{
				Items.Remove(i);
				continue;
			}
			e.Rel = INT_PDA_Util.ClampI(e.Rel, INT_PDA_Const.REL_NEUTRAL, INT_PDA_Const.REL_ENEMY);
			e.Reason = INT_PDA_Util.CleanLine(e.Reason, 120, 400);
		}
	}

	int Get(string a, string b)
	{
		foreach (INT_PDA_RelationEntry e : Items)
		{
			if ((e.A == a && e.B == b) || (e.A == b && e.B == a))
				return e.Rel;
		}
		return INT_PDA_Const.REL_NEUTRAL;
	}

	INT_PDA_RelationEntry Find(string a, string b)
	{
		foreach (INT_PDA_RelationEntry e : Items)
		{
			if ((e.A == a && e.B == b) || (e.A == b && e.B == a))
				return e;
		}
		return null;
	}
}

// Chat/Public.json, Chat/Faction_<id>.json, Mail/<SteamID>.json
class INT_PDA_ChatFile
{
	int Version = 1;
	ref array<ref INT_PDA_ChatLine> Lines;

	static INT_PDA_ChatFile ForRead()
	{
		INT_PDA_ChatFile f = new INT_PDA_ChatFile();
		f.Lines = new array<ref INT_PDA_ChatLine>();
		return f;
	}

	void Clean(int max)
	{
		if (!Lines)
			Lines = new array<ref INT_PDA_ChatLine>();
		for (int i = Lines.Count() - 1; i >= 0; i--)
		{
			if (!Lines[i])
				Lines.Remove(i);
		}
		while (Lines.Count() > max)
			Lines.RemoveOrdered(0);
	}
}

// News.json
class INT_PDA_NewsFile
{
	int Version = 1;
	int NextId = 1;
	ref array<ref INT_PDA_News> Items;

	static INT_PDA_NewsFile ForRead()
	{
		INT_PDA_NewsFile f = new INT_PDA_NewsFile();
		f.Items = new array<ref INT_PDA_News>();
		return f;
	}

	void Clean(int max)
	{
		if (!Items)
			Items = new array<ref INT_PDA_News>();
		for (int i = Items.Count() - 1; i >= 0; i--)
		{
			if (!Items[i])
				Items.Remove(i);
		}
		while (Items.Count() > max)
			Items.RemoveOrdered(0);
		if (NextId < 1)
			NextId = 1;
	}
}
