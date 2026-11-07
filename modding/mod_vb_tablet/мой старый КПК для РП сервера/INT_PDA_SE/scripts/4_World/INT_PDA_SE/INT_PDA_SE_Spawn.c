// INT_PDA (server) — spawn of a FRESH character (owner brief, admin items 3, 12, 13): a player without a faction gets
// Spawn/Default.json, a faction member gets Spawn/<faction id>.json — its point and exactly its clothes and items,
// empty = he comes back naked (owner 2026-10-30: the start gear of Default is never put on a faction member; only an
// empty point list falls back to the Default points). Called only from MissionServer.OnClientNewEvent (first join or
// after death — never on a reconnect). The faction is found by FileExist(GP/<SteamID>.json): no list, no scan.

class INT_PDA_SE_Spawn
{
	static int s_Spawned;			// counters for Debug and the self-test
	static int s_Missing;			// class names that could not be created
	protected static ref INT_PDA_SpawnSet s_NoSet;		// a faction without a set file: naked, the Default points

	// The set for this player (null = vanilla spawn, e.g. the Default set is not for him by m_OwnerSteamID). The
	// account is peeked, not cached: the spawn comes before the player's HELLO.
	static INT_PDA_SpawnSet SetFor(string steam, out INT_PDA_SpawnSet fallback)
	{
		fallback = INT_PDA_SE_Store.SpawnSet(INT_PDA_SE_Store.SPAWN_DEFAULT);
		if (fallback && !fallback.IsFor(steam))
			fallback = null;
		if (INT_PDA_Util.IsSteamId(steam) && INT_PDA_SE_Store.IsFactionMemberFile(steam))
		{
			INT_PDA_Account a = INT_PDA_SE_Store.PeekAccount(steam);
			if (a && a.InFaction() && INT_PDA_SE_Store.Faction(a.FactionId))
			{
				INT_PDA_SpawnSet fs = INT_PDA_SE_Store.SpawnSet(a.FactionId);
				if (!fs)
					return NoSet();
				if (fs.IsFor(steam))
					return fs;
			}
		}
		return fallback;
	}

	// SetFor gave the set of his faction (the Default set is the fallback object itself).
	static bool IsFaction(INT_PDA_SpawnSet spawnSet, INT_PDA_SpawnSet fallback)
	{
		return spawnSet && spawnSet != fallback;
	}

	static INT_PDA_SpawnSet NoSet()
	{
		if (!s_NoSet)
		{
			s_NoSet = INT_PDA_SpawnSet.ForRead();
			s_NoSet.Clean();
		}
		return s_NoSet;
	}

	static vector PickPos(INT_PDA_SpawnSet spawnSet, INT_PDA_SpawnSet fallback, vector pos)
	{
		array<string> list = spawnSet.m_SpawnPosList;
		if (list.Count() == 0 && fallback)
			list = fallback.m_SpawnPosList;
		if (list.Count() == 0)
			return pos;
		string s = list.GetRandomElement();
		s.Replace(",", " ");
		vector p = s.ToVector();
		if (p[0] == 0 && p[2] == 0)
		{
			INT_PDA_Log.Warn("спавн: точка не читается: " + INT_PDA_Log.Clip(s, 40));
			return pos;
		}
		float ground = GetGame().SurfaceY(p[0], p[2]);
		if (p[1] < ground)
			p[1] = ground;
		return p;
	}

	// Clothes and items of the set replace the vanilla starting gear. Without a faction an empty set keeps the vanilla
	// gear; a faction member gets exactly his faction's set: the vanilla clothes go first, so an empty set = naked.
	static void Dress(PlayerBase player, INT_PDA_SpawnSet spawnSet, INT_PDA_SpawnSet fallback)
	{
		if (!player || !spawnSet)
			return;
		INT_PDA_SpawnSet clothes = spawnSet;
		if (IsFaction(spawnSet, fallback))
		{
			StripHands(player);
			StripClothes(player);
			if (!clothes.HasGear())
			{
				s_Spawned++;
				return;
			}
		}
		else
		{
			if (!clothes.HasGear())
				return;
			StripHands(player);
		}
		Wear(player, clothes.m_HeadItem, "Headgear");
		Wear(player, clothes.m_TopItem, "Body");
		Wear(player, clothes.m_PantsItem, "Legs");
		Wear(player, clothes.m_BootsItem, "Feet");
		Wear(player, clothes.m_RukzakItem, "Back");
		Wear(player, clothes.m_PerchatkiItem, "Gloves");
		INT_PDA pda;
		EntityAI made;
		foreach (string type : clothes.m_DopList)
		{
			if (type == "")
				continue;
			made = null;
			if (pda && IsPdaPart(type))
				made = pda.GetInventory().CreateAttachment(type);
			if (!made)
				made = player.GetInventory().CreateInInventory(type);
			if (!made)
			{
				s_Missing++;
				Missing(type);
				continue;
			}
			if (!pda)
				pda = INT_PDA.Cast(made);
		}
		s_Spawned++;
	}

	static bool IsNaked(INT_PDA_SpawnSet s)
	{
		return s.m_HeadItem == "" && s.m_TopItem == "" && s.m_PantsItem == "" && s.m_BootsItem == "" && s.m_RukzakItem == "" && s.m_PerchatkiItem == "";
	}

	protected static bool IsPdaPart(string type)
	{
		return type == "Battery9V" || type.IndexOf("INT_PDA_Module_") == 0;
	}

	// One slot of the set: the vanilla item of that slot goes away only when the set's class really exists, so a
	// typo in Spawn/*.json never leaves a fresh character without trousers.
	protected static void Wear(PlayerBase player, string type, string slot)
	{
		if (type == "")
			return;
		if (!GetGame().ConfigIsExisting("CfgVehicles " + type))
		{
			s_Missing++;
			Missing(type);
			return;
		}
		// a class of another slot (boots in m_PantsItem) would leave the slot empty: the vanilla item stays then
		if (!FitsSlot(type, slot))
		{
			s_Missing++;
			Missing(type);
			return;
		}
		EntityAI old = player.FindAttachmentBySlotName(slot);
		if (old)
			GetGame().ObjectDelete(old);
		if (!player.GetInventory().CreateAttachment(type))
		{
			s_Missing++;
			Missing(type);
		}
	}

	// inventorySlot of the class (a string or an array, inherited) names this slot.
	protected static bool FitsSlot(string type, string slot)
	{
		string path = "CfgVehicles " + type + " inventorySlot";
		array<string> slots = new array<string>();
		GetGame().ConfigGetTextArray(path, slots);
		if (slots.Count() == 0)
		{
			string one;
			GetGame().ConfigGetText(path, one);
			if (one != "")
				slots.Insert(one);
		}
		string want = slot;
		want.ToLower();
		foreach (string sl : slots)
		{
			string have = sl;
			have.ToLower();
			if (have == want)
				return true;
		}
		return false;
	}

	// A wrong class name in a spawn set is a server-owner mistake: warn once per class (not per spawn).
	protected static ref set<string> s_Warned = new set<string>();

	protected static void Missing(string type)
	{
		if (s_Warned.Find(type) >= 0)
			return;
		s_Warned.Insert(type);
		INT_PDA_Log.Warn("спавн: класс не найден или не помещается: " + type + " (проверьте Spawn/*.json)");
	}

	// The vanilla item in hands goes away (the set gives its own gear).
	protected static void StripHands(PlayerBase player)
	{
		EntityAI hands = player.GetHumanInventory().GetEntityInHands();
		if (hands)
			GetGame().ObjectDelete(hands);
	}

	// Every piece of clothing the vanilla creation put on (its cargo goes with it): a faction member wears only his set.
	protected static void StripClothes(PlayerBase player)
	{
		array<EntityAI> worn = new array<EntityAI>();
		int n = player.GetInventory().AttachmentCount();
		for (int i = 0; i < n; i++)
		{
			EntityAI att = player.GetInventory().GetAttachmentFromIndex(i);
			if (att && att.IsClothing())
				worn.Insert(att);
		}
		foreach (EntityAI e : worn)
			GetGame().ObjectDelete(e);
	}
}
