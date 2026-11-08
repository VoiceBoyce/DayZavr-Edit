// INT Banking (server) - banknotes in the player's inventory.
//
// Value of an item: units = pieces / requiredQuantity (whole units only, no rounding), value = units x
// itemValue. Pieces = item quantity for stackable notes, 1 for items without quantity.
//
// Deposit (charge): a dry plan first (largest notes first, smallest stacks first so slots are freed),
// then one covering note when the exact sum cannot be collected, then notes made redundant by the
// cover are put back. Overpay = change, paid back in cash (or credited when it does not fit/pay).
//
// Payout (withdraw / change): a dry plan of notes, then delivery in this order:
//   1. top up stacks of the same note the player already carries (needs no new slot);
//   2. new stacks in free cargo (FindFirstFreeLocationForNewEntity + LocationCreateEntity);
//   3. never split: when not everything fits, NOTHING goes into the inventory - with the player's
//      consent the WHOLE payout is put at the feet (owner rule 2026-10-16: all in the inventory or
//      all on the ground).
// The fit test creates the new stacks tentatively in the same frame (never saved) and deletes them,
// so "does it fit" is exact. Greedy notes are tried first; when they do not fit, a plan that fills the
// player's existing stacks first is tried (fewer new slots).

class INT_BankSE_Denom
{
	string ClassName;
	string Lower;
	int Value;			// per unit
	int Unit;			// pieces per unit (requiredQuantity)
	bool Partial;
	int StackMax;		// pieces in one stack, 1 for items without quantity
	bool Exists;

	void INT_BankSE_Denom(string cls, int value, int unit, bool partial)
	{
		ClassName = cls;
		Lower = cls + "";
		Lower.ToLower();
		Value = value;
		Unit = unit;
		if (Unit < 1)
			Unit = 1;
		Partial = partial;
		Exists = GetGame().ConfigIsExisting("CfgVehicles " + cls);
		StackMax = 1;
		int stack = 0;
		string sp = "CfgVehicles " + cls + " varStackMax";
		if (GetGame().ConfigIsExisting(sp))
			stack = GetGame().ConfigGetInt(sp);
		if (stack <= 0)
		{
			string qp = "CfgVehicles " + cls + " varQuantityMax";
			if (GetGame().ConfigIsExisting(qp))
				stack = GetGame().ConfigGetInt(qp);
		}
		if (stack > 1)
			StackMax = stack;
	}
}

// One inventory item that holds money.
class INT_BankSE_CashItem
{
	EntityAI Item;
	INT_BankSE_Denom Denom;
	int Pieces;
	int Units;			// whole units in the item
	int Take;			// units planned to take (charge)

	int StackValue()
	{
		return INT_BankSE_Money.MulSat(Units, Denom.Value);
	}
}

class INT_BankSE_ChargePlan
{
	int Amount;
	int Change;
	int Taken;			// value taken = Amount + Change
	int Total;			// cash the player carries
	ref array<ref INT_BankSE_CashItem> Items;

	void INT_BankSE_ChargePlan()
	{
		Items = new array<ref INT_BankSE_CashItem>();
	}

	string Describe()
	{
		// "Bablo5000Rubley x3, Bablo1000Rubley x2"
		map<string, int> pieces = new map<string, int>();
		array<string> order = new array<string>();
		foreach (INT_BankSE_CashItem ci : Items)
		{
			if (ci.Take <= 0)
				continue;
			int p = ci.Take * ci.Denom.Unit;
			string cls = ci.Denom.ClassName;
			if (!pieces.Contains(cls))
			{
				pieces.Set(cls, 0);
				order.Insert(cls);
			}
			int had = pieces.Get(cls);
			pieces.Set(cls, had + p);
		}
		string res = "";
		foreach (string c : order)
		{
			if (res != "")
				res = res + ", ";
			int pc = pieces.Get(c);
			res = res + c + " x" + pc.ToString();
		}
		if (res == "")
			res = "-";
		return res;
	}
}

// One note type of a payout.
class INT_BankSE_PayLine
{
	INT_BankSE_Denom Denom;
	int Pieces;

	void INT_BankSE_PayLine(INT_BankSE_Denom d, int pieces)
	{
		Denom = d;
		Pieces = pieces;
	}

	int Value()
	{
		int units = Pieces / Denom.Unit;
		return INT_BankSE_Money.MulSat(units, Denom.Value);
	}
}

class INT_BankSE_PayPlan
{
	int Amount;
	ref array<ref INT_BankSE_PayLine> Lines;
	int NewStacks;		// stacks that need a free slot
	int FitValue;		// value that fits into the inventory (after the fit test)

	void INT_BankSE_PayPlan()
	{
		Lines = new array<ref INT_BankSE_PayLine>();
	}

	string Describe()
	{
		string res = "";
		foreach (INT_BankSE_PayLine l : Lines)
		{
			if (l.Pieces <= 0)
				continue;
			if (res != "")
				res = res + ", ";
			res = res + l.Denom.ClassName + " x" + l.Pieces.ToString();
		}
		if (res == "")
			res = "-";
		return res;
	}
}

// What a delivery really did.
class INT_BankSE_PayResult
{
	int ToInventory;	// value placed into the inventory (top-ups + new stacks)
	int ToGround;		// value dropped at the feet
	int Failed;			// value that could not be created at all
	int GroundStacks;
	string GroundPos;
	string Error;
}

class INT_BankSE_Money
{
	protected static ref array<ref INT_BankSE_Denom> s_Denoms;	// descending by value per piece
	static const int MAX_NEW_STACKS = 120;

	static array<ref INT_BankSE_Denom> Denoms()
	{
		if (!s_Denoms)
			s_Denoms = new array<ref INT_BankSE_Denom>();
		return s_Denoms;
	}

	static void Load()
	{
		array<ref INT_BankSE_Denom> list = Denoms();
		list.Clear();
		INT_BankSE_ConfigData cfg = INT_BankSE_Config.Get();
		string desc = "";
		foreach (INT_BankSE_CurrencyCfg c : cfg.currencies)
		{
			string cls = c.itemClassname + "";
			cls.TrimInPlace();
			if (cls == "" || c.itemValue <= 0)
				continue;
			if (Find(cls))
				continue;
			INT_BankSE_Denom d = new INT_BankSE_Denom(cls, c.itemValue, c.requiredQuantity, c.acceptPartial != 0);
			if (!d.Exists)
				INT_BankSE_Log.Warn("валюта " + cls + " не найдена в CfgVehicles (мод с этой валютой не загружен?) - пропущена");
			if (!d.Exists)
				continue;
			list.Insert(d);
			string sv = d.Value.ToString();
			string su = d.Unit.ToString();
			desc = desc + cls + "=" + sv + "/" + su + " ";
		}
		// insertion sort, descending by value of one piece
		int n = list.Count();
		for (int i = 1; i < n; i++)
		{
			int j = i;
			while (j > 0)
			{
				INT_BankSE_Denom a = list.Get(j - 1);
				INT_BankSE_Denom b = list.Get(j);
				if (a.Value * b.Unit >= b.Value * a.Unit)
					break;
				list.SwapItems(j - 1, j);
				j--;
			}
		}
		INT_BankSE_Log.Info("валюта: " + desc);
		if (list.Count() == 0)
			INT_BankSE_Log.Critical("ни одна валюта из конфига не найдена - внесение и снятие наличных невозможны");
	}

	static INT_BankSE_Denom Find(string type)
	{
		string low = type + "";
		low.ToLower();
		array<ref INT_BankSE_Denom> denomList = Denoms();
		foreach (INT_BankSE_Denom d : denomList)
		{
			if (d.Lower == low)
				return d;
		}
		return null;
	}

	static int AddSat(int a, int b)
	{
		if (b > 0 && a > int.MAX - b)
			return int.MAX;
		return a + b;
	}

	static int MulSat(int a, int b)
	{
		if (a <= 0 || b <= 0)
			return 0;
		if (a > int.MAX / b)
			return int.MAX;
		return a * b;
	}

	static int PiecesOf(EntityAI e)
	{
		ItemBase ib = ItemBase.Cast(e);
		if (ib && ib.HasQuantity())
		{
			int p = Math.Round(ib.GetQuantity());
			if (p < 0)
				p = 0;
			return p;
		}
		return 1;
	}

	// Money items in the player's inventory (hands, cargo, clothing), not marked for deletion.
	static void Collect(PlayerBase player, array<ref INT_BankSE_CashItem> out_items)
	{
		out_items.Clear();
		if (!player)
			return;
		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI e : items)
		{
			if (!e || e == player || e.IsSetForDeletion())
				continue;
			if (e.GetHierarchyRootPlayer() != player)
				continue;
			INT_BankSE_Denom d = Find(e.GetType());
			if (!d)
				continue;
			int pcs = PiecesOf(e);
			if (pcs <= 0)
				continue;
			INT_BankSE_CashItem ci = new INT_BankSE_CashItem();
			ci.Item = e;
			ci.Denom = d;
			ci.Pieces = pcs;
			ci.Units = pcs / d.Unit;
			ci.Take = 0;
			out_items.Insert(ci);
		}
	}

	static int CountCash(PlayerBase player)
	{
		array<ref INT_BankSE_CashItem> items = new array<ref INT_BankSE_CashItem>();
		Collect(player, items);
		int total = 0;
		foreach (INT_BankSE_CashItem ci : items)
			total = AddSat(total, ci.StackValue());
		return total;
	}

	// ------------------------------------------------------------------------------------------------
	// Deposit
	// ------------------------------------------------------------------------------------------------
	// false: not enough cash (plan.Total < amount) or the sum cannot be collected.
	static bool PlanCharge(PlayerBase player, int amount, INT_BankSE_ChargePlan plan)
	{
		plan.Amount = amount;
		plan.Change = 0;
		plan.Taken = 0;
		Collect(player, plan.Items);
		plan.Total = 0;
		foreach (INT_BankSE_CashItem c0 : plan.Items)
			plan.Total = AddSat(plan.Total, c0.StackValue());
		if (amount <= 0 || plan.Total < amount)
			return false;

		// largest value per piece first; among equal notes the smaller stacks first (frees slots)
		int n = plan.Items.Count();
		for (int i = 1; i < n; i++)
		{
			int j = i;
			while (j > 0)
			{
				INT_BankSE_CashItem a = plan.Items.Get(j - 1);
				INT_BankSE_CashItem b = plan.Items.Get(j);
				bool swap = false;
				int av = a.Denom.Value * b.Denom.Unit;
				int bv = b.Denom.Value * a.Denom.Unit;
				if (av < bv)
					swap = true;
				else if (av == bv && a.Pieces > b.Pieces)
					swap = true;
				if (!swap)
					break;
				plan.Items.SwapItems(j - 1, j);
				j--;
			}
		}

		int remaining = amount;
		foreach (INT_BankSE_CashItem ci : plan.Items)
		{
			if (remaining <= 0)
				break;
			if (ci.Units <= 0)
				continue;
			if (ci.Denom.Partial)
			{
				int want = remaining / ci.Denom.Value;
				if (want > ci.Units)
					want = ci.Units;
				ci.Take = want;
				remaining = remaining - want * ci.Denom.Value;
			}
			else if (ci.StackValue() <= remaining)
			{
				ci.Take = ci.Units;
				remaining = remaining - ci.StackValue();
			}
		}

		if (remaining > 0)
		{
			// one covering chunk: a single unit of a partial stack, or a whole stack
			INT_BankSE_CashItem best = null;
			int bestValue = 0;
			bool bestWhole = false;
			foreach (INT_BankSE_CashItem cc : plan.Items)
			{
				int left = cc.Units - cc.Take;
				if (left <= 0)
					continue;
				int chunk;
				bool whole = false;
				if (cc.Denom.Partial)
					chunk = cc.Denom.Value;
				else
				{
					if (cc.Take > 0)
						continue;
					chunk = cc.StackValue();
					whole = true;
				}
				if (chunk < remaining)
					continue;
				if (!best || chunk < bestValue)
				{
					best = cc;
					bestValue = chunk;
					bestWhole = whole;
				}
			}
			if (!best)
				return false;
			if (bestWhole)
				best.Take = best.Units;
			else
				best.Take = best.Take + 1;
			plan.Change = bestValue - remaining;
			remaining = 0;

			// put back notes the cover made redundant (smallest first)
			for (int k = n - 1; k >= 0 && plan.Change > 0; k--)
			{
				INT_BankSE_CashItem r = plan.Items.Get(k);
				if (r.Take <= 0)
					continue;
				if (r == best && !r.Denom.Partial)
					continue;
				if (r.Denom.Partial)
				{
					int back = plan.Change / r.Denom.Value;
					if (r == best && back >= r.Take)
						back = r.Take - 1;
					if (back > r.Take)
						back = r.Take;
					if (back > 0)
					{
						r.Take = r.Take - back;
						plan.Change = plan.Change - back * r.Denom.Value;
					}
				}
				else if (r.StackValue() <= plan.Change)
				{
					r.Take = 0;
					plan.Change = plan.Change - r.StackValue();
				}
			}
		}

		plan.Taken = AddSat(amount, plan.Change);
		return true;
	}

	// Removes the planned notes. Returns the value really removed.
	static int ExecuteCharge(INT_BankSE_ChargePlan plan)
	{
		int removed = 0;
		foreach (INT_BankSE_CashItem ci : plan.Items)
		{
			if (ci.Take <= 0 || !ci.Item)
				continue;
			int takePieces = ci.Take * ci.Denom.Unit;
			int value = MulSat(ci.Take, ci.Denom.Value);
			if (takePieces >= ci.Pieces)
			{
				GetGame().ObjectDelete(ci.Item);
				removed = AddSat(removed, value);
				continue;
			}
			ItemBase ib = ItemBase.Cast(ci.Item);
			if (!ib)
				continue;
			int left = ci.Pieces - takePieces;
			ib.SetQuantity(left, false);
			int now = PiecesOf(ib);
			if (now == left)
				removed = AddSat(removed, value);
		}
		return removed;
	}

	// ------------------------------------------------------------------------------------------------
	// Payout
	// ------------------------------------------------------------------------------------------------
	// Greedy notes (fewest pieces). false when the amount cannot be paid with the configured notes.
	static bool PlanPayGreedy(int amount, INT_BankSE_PayPlan plan)
	{
		plan.Amount = amount;
		plan.Lines.Clear();
		int remaining = amount;
		array<ref INT_BankSE_Denom> denomList = Denoms();
		foreach (INT_BankSE_Denom d : denomList)
		{
			if (remaining <= 0)
				break;
			int units = remaining / d.Value;
			if (units <= 0)
				continue;
			plan.Lines.Insert(new INT_BankSE_PayLine(d, units * d.Unit));
			remaining = remaining - units * d.Value;
		}
		return remaining == 0;
	}

	// Fills the stacks the player already carries first (no new slot needed), the rest greedy.
	static bool PlanPayExisting(PlayerBase player, int amount, INT_BankSE_PayPlan plan)
	{
		plan.Amount = amount;
		plan.Lines.Clear();
		int remaining = amount;
		map<string, int> room = new map<string, int>();
		TopUpRoom(player, room);
		array<ref INT_BankSE_Denom> denomList = Denoms();
		foreach (INT_BankSE_Denom d : denomList)
		{
			if (remaining <= 0)
				break;
			int pieces = 0;
			if (room.Contains(d.Lower))
			{
				int canUnits = room.Get(d.Lower) / d.Unit;
				int wantUnits = remaining / d.Value;
				if (canUnits > wantUnits)
					canUnits = wantUnits;
				pieces = canUnits * d.Unit;
				remaining = remaining - canUnits * d.Value;
			}
			if (pieces > 0)
				plan.Lines.Insert(new INT_BankSE_PayLine(d, pieces));
		}
		// the rest greedy (adds to existing lines)
		foreach (INT_BankSE_Denom g : denomList)
		{
			if (remaining <= 0)
				break;
			int units = remaining / g.Value;
			if (units <= 0)
				continue;
			AddLine(plan, g, units * g.Unit);
			remaining = remaining - units * g.Value;
		}
		return remaining == 0;
	}

	protected static void AddLine(INT_BankSE_PayPlan plan, INT_BankSE_Denom d, int pieces)
	{
		foreach (INT_BankSE_PayLine l : plan.Lines)
		{
			if (l.Denom == d)
			{
				l.Pieces = l.Pieces + pieces;
				return;
			}
		}
		plan.Lines.Insert(new INT_BankSE_PayLine(d, pieces));
	}

	// Free pieces in the player's existing stacks, per note class (lower case).
	static void TopUpRoom(PlayerBase player, map<string, int> room)
	{
		room.Clear();
		array<ref INT_BankSE_CashItem> items = new array<ref INT_BankSE_CashItem>();
		Collect(player, items);
		foreach (INT_BankSE_CashItem ci : items)
		{
			ItemBase ib = ItemBase.Cast(ci.Item);
			if (!ib || !ib.HasQuantity() || ib.IsRuined())
				continue;
			int qmax = ib.GetQuantityMax();
			int space = qmax - ci.Pieces;
			if (space <= 0)
				continue;
			int have = 0;
			if (room.Contains(ci.Denom.Lower))
				have = room.Get(ci.Denom.Lower);
			room.Set(ci.Denom.Lower, have + space);
		}
	}

	// Splits the plan into top-ups and new stacks; NewStacks = stacks that need a free slot.
	// Fills the new stacks as (denom, pieces) in creation order. false = more than MAX_NEW_STACKS.
	static bool NewStacksOf(PlayerBase player, INT_BankSE_PayPlan plan, array<INT_BankSE_Denom> stackDenoms, array<int> stackPieces)
	{
		stackDenoms.Clear();
		stackPieces.Clear();
		map<string, int> room = new map<string, int>();
		TopUpRoom(player, room);
		foreach (INT_BankSE_PayLine l : plan.Lines)
		{
			int left = l.Pieces;
			if (room.Contains(l.Denom.Lower))
			{
				int r = room.Get(l.Denom.Lower);
				int use = left;
				if (use > r)
					use = r;
				left = left - use;
				room.Set(l.Denom.Lower, r - use);
			}
			while (left > 0)
			{
				int q = left;
				if (q > l.Denom.StackMax)
					q = l.Denom.StackMax;
				stackDenoms.Insert(l.Denom);
				stackPieces.Insert(q);
				left = left - q;
				if (stackDenoms.Count() > MAX_NEW_STACKS)
				{
					plan.NewStacks = stackDenoms.Count();
					return false;
				}
			}
		}
		plan.NewStacks = stackDenoms.Count();
		return true;
	}

	static int PiecesValue(INT_BankSE_Denom d, int pieces)
	{
		return MulSat(pieces / d.Unit, d.Value);
	}

	// Exact fit test: the value that fits into the inventory (top-ups + new stacks that find a slot).
	// Tentative stacks are created and deleted in the same frame (never saved).
	static int FitValue(PlayerBase player, INT_BankSE_PayPlan plan)
	{
		array<INT_BankSE_Denom> sd = new array<INT_BankSE_Denom>();
		array<int> sp = new array<int>();
		if (!NewStacksOf(player, plan, sd, sp))
		{
			plan.FitValue = 0;
			return 0;
		}
		int newValue = 0;
		int n = sd.Count();
		for (int i = 0; i < n; i++)
		{
			INT_BankSE_Denom di = sd.Get(i);
			int pi = sp.Get(i);
			int vi = PiecesValue(di, pi);
			newValue = AddSat(newValue, vi);
		}
		int topUpValue = plan.Amount - newValue;
		if (topUpValue < 0)
			topUpValue = 0;
		array<EntityAI> tentative = new array<EntityAI>();
		int fitNew = 0;
		for (int k = 0; k < n; k++)
		{
			INT_BankSE_Denom dk = sd.Get(k);
			int pk = sp.Get(k);
			EntityAI e = CreateInCargo(player, dk.ClassName);
			if (!e)
				continue;
			tentative.Insert(e);
			int vk = PiecesValue(dk, pk);
			fitNew = AddSat(fitNew, vk);
		}
		foreach (EntityAI t : tentative)
		{
			if (t)
				GetGame().ObjectDelete(t);
		}
		plan.FitValue = AddSat(topUpValue, fitNew);
		return plan.FitValue;
	}

	static EntityAI CreateInCargo(PlayerBase player, string type)
	{
		if (!player)
			return null;
		InventoryLocation il = new InventoryLocation();
		if (!player.GetInventory().FindFirstFreeLocationForNewEntity(type, FindInventoryLocationType.CARGO, il))
			return null;
		return GameInventory.LocationCreateEntity(il, type, ECE_IN_INVENTORY, RF_DEFAULT);
	}

	// Best plan for a payout: greedy if it fits completely, else the existing-stacks plan if that one
	// fits completely, else the one that fits more. false = amount not payable with the notes.
	static bool PlanPay(PlayerBase player, int amount, INT_BankSE_PayPlan plan)
	{
		if (!PlanPayGreedy(amount, plan))
			return false;
		FitValue(player, plan);
		if (plan.NewStacks > MAX_NEW_STACKS)
			return false;
		if (plan.FitValue >= amount)
			return true;
		INT_BankSE_PayPlan alt = new INT_BankSE_PayPlan();
		if (!PlanPayExisting(player, amount, alt))
			return true;
		FitValue(player, alt);
		if (alt.NewStacks <= MAX_NEW_STACKS && alt.FitValue > plan.FitValue)
		{
			plan.Lines.Clear();
			foreach (INT_BankSE_PayLine l : alt.Lines)
				plan.Lines.Insert(l);
			plan.NewStacks = alt.NewStacks;
			plan.FitValue = alt.FitValue;
		}
		return true;
	}

	// Gives the planned notes. groundOnly = everything at the feet (the player agreed because the
	// inventory has no room for all of it); otherwise inventory only, what fails is reported as Failed.
	static void Deliver(PlayerBase player, INT_BankSE_PayPlan plan, bool groundOnly, INT_BankSE_PayResult res)
	{
		res.ToInventory = 0;
		res.ToGround = 0;
		res.Failed = 0;
		res.GroundStacks = 0;
		res.GroundPos = "";
		res.Error = "";
		if (!player)
		{
			res.Failed = plan.Amount;
			res.Error = "игрок не найден";
			return;
		}
		vector origin = player.GetPosition();
		int dropIndex = 0;
		foreach (INT_BankSE_PayLine l : plan.Lines)
		{
			int left = l.Pieces;
			// 1. top up existing stacks
			if (l.Denom.StackMax > 1 && !groundOnly)
			{
				array<ref INT_BankSE_CashItem> items = new array<ref INT_BankSE_CashItem>();
				Collect(player, items);
				foreach (INT_BankSE_CashItem ci : items)
				{
					if (left <= 0)
						break;
					if (ci.Denom != l.Denom)
						continue;
					ItemBase ib = ItemBase.Cast(ci.Item);
					if (!ib || !ib.HasQuantity() || ib.IsRuined())
						continue;
					int space = ib.GetQuantityMax() - ci.Pieces;
					if (space <= 0)
						continue;
					int add = left;
					if (add > space)
						add = space;
					int want = ci.Pieces + add;
					ib.SetQuantity(want, false);
					int got = PiecesOf(ib) - ci.Pieces;
					if (got <= 0)
						continue;
					res.ToInventory = AddSat(res.ToInventory, PiecesValue(l.Denom, got));
					left = left - got;
				}
			}
			// 2. new stacks in cargo - or, ground mode, every stack at the feet
			while (left > 0)
			{
				int q = left;
				if (q > l.Denom.StackMax)
					q = l.Denom.StackMax;
				EntityAI e = null;
				if (!groundOnly)
					e = CreateInCargo(player, l.Denom.ClassName);
				bool ground = false;
				if (!e && groundOnly)
				{
					vector pos = DropPos(origin, dropIndex);
					dropIndex++;
					Object o = GetGame().CreateObjectEx(l.Denom.ClassName, pos, ECE_PLACE_ON_SURFACE);
					e = EntityAI.Cast(o);
					if (o && !e)
						GetGame().ObjectDelete(o);
					if (e)
					{
						ground = true;
						if (res.GroundPos == "")
							res.GroundPos = INT_BankSE_Log.Pos(pos);
					}
				}
				if (!e)
					break;
				int placed = 1;
				ItemBase nb = ItemBase.Cast(e);
				if (nb && nb.HasQuantity())
				{
					nb.SetQuantity(q, false);
					placed = PiecesOf(nb);
				}
				if (placed > q)
					placed = q;
				if (placed <= 0)
				{
					GetGame().ObjectDelete(e);
					res.Error = "не удалось задать количество " + l.Denom.ClassName;
					break;
				}
				int v = PiecesValue(l.Denom, placed);
				if (ground)
				{
					res.ToGround = AddSat(res.ToGround, v);
					res.GroundStacks++;
				}
				else
				{
					res.ToInventory = AddSat(res.ToInventory, v);
				}
				left = left - placed;
			}
			if (left > 0)
			{
				res.Failed = AddSat(res.Failed, PiecesValue(l.Denom, left));
				if (res.Error == "")
				{
					if (groundOnly)
						res.Error = "не удалось создать " + l.Denom.ClassName + " x" + left.ToString();
					else
						res.Error = "нет места в инвентаре для " + l.Denom.ClassName + " x" + left.ToString();
				}
			}
		}
	}

	// Spread drop positions around the feet (golden-angle spiral), so stacks do not land inside each other.
	static vector DropPos(vector origin, int idx)
	{
		float ang = idx * 2.4;
		int ring = idx / 6;
		float r = 0.35 + 0.15 * ring;
		vector pos = origin;
		pos[0] = origin[0] + Math.Cos(ang) * r;
		pos[2] = origin[2] + Math.Sin(ang) * r;
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		return pos;
	}
}
