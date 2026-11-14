// INT Quests — counting quest items in a player's inventory, the same code on both sides (spec.md §4.2, §6): the
// client shows "принеси/найди" progress from its own inventory, the server re-counts at turn-in before it takes
// anything (two-phase). Units exactly like the old mod: stackables (QUANTITY_COUNT: coins, ammo piles AND magazines =
// rounds inside) count their quantity, everything else counts 1 per item. The old quests rely on it: a magazine
// target's Count is a number of ROUNDS («7 пистолетов и 7 обойм, обоймы заряди» = Mag_IJ70_8Rnd × 84). Class match is exact (case-insensitive), as in the old mod. Items already handed to the engine for deletion do not
// count (the delete of a carried item is a juncture: the item lives on for a moment and could be handed in twice).

class INT_QST_Inv
{
	// Units of `item` that count for target `t` (0 = does not match).
	static int Units(ItemBase item, INT_QST_TargetDef t)
	{
		if (!item || item.IsSetForDeletion())
			return 0;
		if (INT_QST_Util.Lower(item.GetType()) != t.m_ClassLow)
			return 0;
		if (t.m_Worst >= 0 && item.GetHealthLevel() > t.m_Worst)
			return 0;
		int qtype = QuantityConversions.HasItemQuantity(item);
		if (t.MinQuantity > 0 && qtype != QUANTITY_HIDDEN)
		{
			float cur = QuantityConversions.GetItemQuantity(item);
			float max = QuantityConversions.GetItemQuantityMax(item);
			if (max > 0 && cur * 100.0 < t.MinQuantity * max - 0.001)
				return 0;
		}
		if (qtype == QUANTITY_COUNT)
		{
			float q = QuantityConversions.GetItemQuantity(item);
			int whole = (int)Math.Floor(q + 0.001);
			return whole;
		}
		return 1;
	}

	// Cargo or attachments: handing the item over would delete what it holds.
	// Things lying INSIDE the item (its cargo, or the cargo of what is attached to it: vest pouches). Attachments
	// themselves are handed over with the item, as in the old mod (a gun with its magazine and scope).
	static bool HoldsSomething(ItemBase item)
	{
		if (!item)
			return false;
		GameInventory inv = item.GetInventory();
		if (!inv)
			return false;
		if (CargoCount(inv) > 0)
			return true;
		int n = inv.AttachmentCount();
		for (int i = 0; i < n; i++)
		{
			EntityAI att = inv.GetAttachmentFromIndex(i);
			if (!att)
				continue;
			GameInventory ai = att.GetInventory();
			if (ai && CargoCount(ai) > 0)
				return true;
		}
		return false;
	}

	protected static int CargoCount(GameInventory inv)
	{
		CargoBase cargo = inv.GetCargo();
		if (!cargo)
			return 0;
		return cargo.GetItemCount();
	}

	// Every item the player carries; items that hold something come last (they are handed over only if needed).
	static void Items(PlayerBase p, array<ItemBase> outItems)
	{
		outItems.Clear();
		if (!p || !p.GetInventory())
			return;
		array<EntityAI> all = new array<EntityAI>();
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, all);
		array<ItemBase> holders = new array<ItemBase>();
		foreach (EntityAI e : all)
		{
			if (!e || e == p)
				continue;
			ItemBase ib = ItemBase.Cast(e);
			if (!ib)
				continue;
			if (HoldsSomething(ib))
				holders.Insert(ib);
			else
				outItems.Insert(ib);
		}
		foreach (ItemBase h : holders)
			outItems.Insert(h);
	}

	// Units available for one target (no reservation across targets).
	static int Count(PlayerBase p, INT_QST_TargetDef t)
	{
		array<ItemBase> items = new array<ItemBase>();
		Items(p, items);
		int sum = 0;
		foreach (ItemBase ib : items)
			sum = sum + Units(ib, t);
		return sum;
	}

	// Every inventory target of the quest covered at once (items are reserved target by target, so two targets
	// of the same class do not count the same stack twice). cleanDeliver: DELIVER targets may use only items that
	// hold nothing (the server refuses the turn-in with ITEM_NOT_EMPTY when only full items would do).
	static bool AllCovered(PlayerBase p, INT_QST_QuestDef q, bool cleanDeliver = false)
	{
		array<ItemBase> items = new array<ItemBase>();
		Items(p, items);
		map<ItemBase, int> used = new map<ItemBase, int>();
		foreach (INT_QST_TargetDef t : q.Targets)
		{
			if (!INT_QST_TT.IsInventory(t.m_Type))
				continue;
			bool clean = cleanDeliver && t.m_Type == INT_QST_TT.DELIVER;
			int need = t.m_Need;
			foreach (ItemBase ib : items)
			{
				if (need <= 0)
					break;
				if (clean && HoldsSomething(ib))
					continue;
				int u = Units(ib, t);
				if (u <= 0)
					continue;
				int already = 0;
				used.Find(ib, already);
				int free = u - already;
				if (free <= 0)
					continue;
				int take = Math.Min(free, need);
				used.Set(ib, already + take);
				need = need - take;
			}
			if (need > 0)
				return false;
		}
		return true;
	}
}
