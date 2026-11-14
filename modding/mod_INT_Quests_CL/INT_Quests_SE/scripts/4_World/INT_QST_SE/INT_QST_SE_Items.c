// INT Quests (server) — taking delivered items, giving rewards and quest items (spec.md §6). Turn-in is two-phase:
// INT_QST_Inv.AllCovered() checks everything first, Take() runs only after every other check passed. Partial
// stacks are reduced, whole items are deleted with DeleteSafe (a juncture for items a living player carries).
// Rewards go into the inventory; what does not fit is put at the player's feet (on a building floor too).

class INT_QST_SE_Items
{
	// Health fraction of a state level (GameConstants.DAMAGE_*_VALUE).
	static float StateHealth(int level)
	{
		switch (level)
		{
			case 0:
				return GameConstants.DAMAGE_PRISTINE_VALUE;
			case 1:
				return GameConstants.DAMAGE_WORN_VALUE;
			case 2:
				return GameConstants.DAMAGE_DAMAGED_VALUE;
			case 3:
				return GameConstants.DAMAGE_BADLY_DAMAGED_VALUE;
		}
		return GameConstants.DAMAGE_RUINED_VALUE;
	}

	// Remove the units of every DELIVER target (SEARCH targets are only checked). Call after AllCovered().
	static void TakeDelivered(PlayerBase p, INT_QST_QuestDef q)
	{
		array<ItemBase> items = new array<ItemBase>();
		INT_QST_Inv.Items(p, items);
		map<ItemBase, int> take = new map<ItemBase, int>();
		foreach (INT_QST_TargetDef t : q.Targets)
		{
			if (t.m_Type != INT_QST_TT.DELIVER)
				continue;
			int need = t.m_Need;
			foreach (ItemBase ib : items)
			{
				if (need <= 0)
					break;
				// never an item that holds something (the turn-in was refused before if only such items would do)
				if (INT_QST_Inv.HoldsSomething(ib))
					continue;
				int u = INT_QST_Inv.Units(ib, t);
				if (u <= 0)
					continue;
				int already = 0;
				take.Find(ib, already);
				int free = u - already;
				if (free <= 0)
					continue;
				int k = Math.Min(free, need);
				take.Set(ib, already + k);
				need = need - k;
			}
		}
		foreach (ItemBase it, int units : take)
			RemoveUnits(it, units);
	}

	static void RemoveUnits(ItemBase item, int units)
	{
		if (!item || units <= 0)
			return;
		int qtype = QuantityConversions.HasItemQuantity(item);
		if (qtype != QUANTITY_COUNT)
		{
			item.DeleteSafe();
			return;
		}
		Magazine mag = Magazine.Cast(item);
		if (mag)
		{
			int ammo = mag.GetAmmoCount();
			if (units >= ammo)
				mag.DeleteSafe();
			else
				mag.ServerSetAmmoCount(ammo - units);
			return;
		}
		float cur = item.GetQuantity();
		if (units >= cur)
			item.DeleteSafe();
		else
			item.SetQuantity(cur - units);
	}

	// Quest items handed out at accept; taken back (as many as are still carried) at turn-in / cancel / fail.
	static void GiveQuestItems(PlayerBase p, INT_QST_QuestDef q, out bool ground)
	{
		ground = false;
		foreach (INT_QST_ItemRef r : q.QuestItems)
		{
			for (int i = 0; i < r.Count; i++)
			{
				bool g;
				Spawn(p, r.Item, "", -1, g);
				if (g)
					ground = true;
			}
		}
	}

	// Returns true when every quest item came back (nothing stashed, nothing lost).
	static bool TakeQuestItems(PlayerBase p, INT_QST_QuestDef q)
	{
		if (q.QuestItems.Count() == 0)
			return true;
		if (!p)
			return false;
		array<ItemBase> items = new array<ItemBase>();
		INT_QST_Inv.Items(p, items);
		bool all = true;
		foreach (INT_QST_ItemRef r : q.QuestItems)
		{
			string low = INT_QST_Util.Lower(r.Item);
			int left = r.Count;
			foreach (ItemBase ib : items)
			{
				if (left <= 0)
					break;
				if (!ib || ib.IsSetForDeletion())
					continue;
				if (INT_QST_Util.Lower(ib.GetType()) != low)
					continue;
				ib.DeleteSafe();
				left--;
			}
			if (left > 0)
				all = false;
		}
		return all;
	}

	// Item rewards. Returns how many entities could not be created at all (bad class names).
	static int GiveItems(PlayerBase p, INT_QST_QuestDef q, out bool ground)
	{
		ground = false;
		int failed = 0;
		foreach (INT_QST_RewardDef r : q.Rewards)
		{
			if (r.m_Type != INT_QST_RT.ITEM)
				continue;
			for (int i = 0; i < r.Count; i++)
			{
				bool g;
				if (!Spawn(p, r.Item, r.Quantity, r.m_State, g))
					failed++;
				if (g)
					ground = true;
			}
		}
		return failed;
	}

	// One entity into the inventory, else on the ground next to the player.
	static bool Spawn(PlayerBase p, string cls, string quantity, int state, out bool ground)
	{
		ground = false;
		if (!p || cls == "")
			return false;
		EntityAI e = p.GetInventory().CreateInInventory(cls);
		if (!e)
		{
			e = AtFeet(p, cls);
			if (e)
				ground = true;
		}
		if (!e)
		{
			INT_QST_Log.Warn("награда не создана (нет такого класса?): " + INT_QST_Log.Clip(cls, 60));
			return false;
		}
		ItemBase ib = ItemBase.Cast(e);
		if (ib && quantity != "")
			ApplyQuantity(ib, quantity);
		if (state >= 0 && state <= 4)
			e.SetHealth01("", "", StateHealth(state));
		// DebugMode: what the reward really got (fill of kits/bottles/tape, rounds of magazines)
		if (ib && INT_QST_Log.s_Debug)
		{
			float qv = ib.GetQuantity();
			float qm = ib.GetQuantityMax();
			Magazine mg = Magazine.Cast(ib);
			if (mg)
			{
				qv = mg.GetAmmoCount();
				qm = mg.GetAmmoMax();
			}
			string qvT = qv.ToString();
			string qmT = qm.ToString();
			string line = "награда " + cls + " [" + quantity + "]: " + qvT + " из " + qmT;
			if (ib.IsLiquidContainer())
			{
				int lt = ib.GetLiquidType();
				string ltT = lt.ToString();
				line = line + ", жидкость " + ltT;
			}
			INT_QST_Log.Dbg(line);
		}
		return true;
	}

	// On the ground at the player's feet, at the player's height. Not SpawnEntityOnGroundPos: its ECE_PLACE_ON_SURFACE
	// traces down from the feet, the trace starts inside the floor of a building / an object hill, misses it and puts
	// the item on the terrain under the object (2026-10-30). 5 cm up like vanilla PlayerBase's own height fix.
	static EntityAI AtFeet(PlayerBase p, string cls)
	{
		vector pos = p.GetPosition();
		pos[1] = pos[1] + 0.05;
		return EntityAI.Cast(GetGame().CreateObjectEx(cls, pos, ECE_CREATEPHYSICS | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN));
	}

	// "max", "50%", "500" (old mod formats).
	static void ApplyQuantity(ItemBase ib, string raw)
	{
		string s = INT_QST_Util.Lower(raw);
		s.TrimInPlace();
		bool pct = s.IndexOf("%") >= 0;
		s.Replace("%", "");
		s.Replace(" ", "");
		Magazine mag = Magazine.Cast(ib);
		if (s == "max")
		{
			if (mag)
				mag.ServerSetAmmoMax();
			else
				ib.SetQuantityMax();
			return;
		}
		float v = s.ToFloat();
		if (v <= 0)
			return;
		if (mag)
		{
			int ammo;
			if (pct)
				ammo = (int)Math.Round(Math.Clamp(v / 100.0, 0, 1) * mag.GetAmmoMax());
			else
				ammo = (int)v;
			mag.ServerSetAmmoCount(ammo);
			return;
		}
		if (pct)
			ib.SetQuantityNormalized(Math.Clamp(v / 100.0, 0, 1));
		else
			ib.SetQuantity(v);
	}
}
