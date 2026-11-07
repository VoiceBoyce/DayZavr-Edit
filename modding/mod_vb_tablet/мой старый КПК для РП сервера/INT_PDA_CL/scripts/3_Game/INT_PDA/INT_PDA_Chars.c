// INT_PDA — the «Персонаж» tab (owner 2026-10-26): the player picks the survivor his new characters are made of and
// may be reborn as it at once. Shared by both sides: the vanilla survivor list (the owner's list from his spawn-set
// config = all 31 vanilla classes), names and gender of a class, the tab's data packet.

class INT_PDA_Chars
{
	static const int MAX_TYPES = 64;
	static const int WEAR_SLOTS = 6;			// head, body, legs, feet, back, gloves (the spawn set's clothes)
	static string s_ForceType;					// server: the class of the character being made now (INT_PDA_SE mission)

	// Every vanilla survivor (P:\DZ\characters\data\config.cpp, scope 2), men first — the order of the owner's list.
	static void Vanilla(array<string> outList)
	{
		outList.Clear();
		array<string> men = {"SurvivorM_Mirek", "SurvivorM_Boris", "SurvivorM_Cyril", "SurvivorM_Denis", "SurvivorM_Elias", "SurvivorM_Francis", "SurvivorM_Guo", "SurvivorM_Hassan", "SurvivorM_Indar", "SurvivorM_Jose", "SurvivorM_Kaito", "SurvivorM_Lewis", "SurvivorM_Manua", "SurvivorM_Niki", "SurvivorM_Oliver", "SurvivorM_Peter", "SurvivorM_Quinn", "SurvivorM_Rolf", "SurvivorM_Seth", "SurvivorM_Taiki"};
		array<string> women = {"SurvivorF_Eva", "SurvivorF_Frida", "SurvivorF_Gabi", "SurvivorF_Helga", "SurvivorF_Irena", "SurvivorF_Judy", "SurvivorF_Keiko", "SurvivorF_Linda", "SurvivorF_Maria", "SurvivorF_Naomi", "SurvivorF_Baty"};
		outList.InsertAll(men);
		outList.InsertAll(women);
	}

	static bool IsVanilla(string type)
	{
		array<string> all = new array<string>();
		Vanilla(all);
		return all.Find(type) >= 0;
	}

	// A class the engine can make a player of: exists and is a survivor.
	static bool IsSurvivor(string type)
	{
		if (type == "" || !GetGame())
			return false;
		if (!GetGame().ConfigIsExisting("CfgVehicles " + type))
			return false;
		return GetGame().IsKindOf(type, "SurvivorBase");
	}

	static bool IsFemale(string type)
	{
		if (type == "" || !GetGame())
			return false;
		return GetGame().IsKindOf(type, "SurvivorFemale_Base");
	}

	// "SurvivorM_Mirek" -> "Mirek"
	static string Suffix(string type)
	{
		int cut = type.LastIndexOf("_");
		if (cut < 0 || cut >= type.Length() - 1)
			return type;
		return type.Substring(cut + 1, type.Length() - cut - 1);
	}

	// Name to show: a stringtable key for the vanilla survivors (#STR_INT_PDA_CHR_MIREK), the class suffix otherwise.
	static string NameKey(string type)
	{
		string name = Suffix(type);
		if (!IsVanilla(type))
			return name;
		name.ToUpper();
		return "#STR_INT_PDA_CHR_" + name;
	}
}

// OP_CHAR_DATA: what the tab shows. Wear = the clothes the player's spawn set gives (for the preview), "" = none.
class INT_PDA_CharData
{
	bool Enabled;
	ref array<string> Types = new array<string>();
	string Chosen;					// "" = not chosen (vanilla character of the main menu)
	int WaitSec;					// seconds until «Переродиться» is allowed again (0 = now)
	int CooldownMin;				// the server's CharacterRespawnMinutes (text of the button hint)
	ref array<string> Wear = new array<string>();

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(Enabled);
		int n = Math.Min(Types.Count(), INT_PDA_Chars.MAX_TYPES);
		ctx.Write(n);
		int i;
		for (i = 0; i < n; i++)
			INT_PDA_Proto.WriteStr(ctx, Types[i]);
		INT_PDA_Proto.WriteStr(ctx, Chosen);
		ctx.Write(WaitSec);
		ctx.Write(CooldownMin);
		n = Math.Min(Wear.Count(), INT_PDA_Chars.WEAR_SLOTS);
		ctx.Write(n);
		for (i = 0; i < n; i++)
			INT_PDA_Proto.WriteStr(ctx, Wear[i]);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Enabled))
			return false;
		int n;
		int i;
		string s;
		if (!INT_PDA_Proto.ReadCount(ctx, n, INT_PDA_Chars.MAX_TYPES))
			return false;
		Types.Clear();
		for (i = 0; i < n; i++)
		{
			if (!ctx.Read(s))
				return false;
			Types.Insert(s);
		}
		if (!ctx.Read(Chosen) || !ctx.Read(WaitSec) || !ctx.Read(CooldownMin))
			return false;
		if (!INT_PDA_Proto.ReadCount(ctx, n, INT_PDA_Chars.WEAR_SLOTS))
			return false;
		Wear.Clear();
		for (i = 0; i < n; i++)
		{
			if (!ctx.Read(s))
				return false;
			Wear.Insert(s);
		}
		return true;
	}
}
