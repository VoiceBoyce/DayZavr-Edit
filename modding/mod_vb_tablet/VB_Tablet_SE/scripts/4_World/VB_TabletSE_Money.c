// Деньги это предметы. Списание и выдача целиком или никак: сначала создаётся всё, что надо выдать,
// и только потом забираются купюры. Если сдача не помещается, ничего не меняется. Образец: INT_BankSE_Money.
class VB_TabletCash
{
	EntityAI Item;
	int Value;
	int Pieces;
	int Take;
}

class VB_TabletMoney
{
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

	protected static ref array<ref VB_TabletDenom> s_BankDenoms;

	// Купюры. При INT_Banking те же, что принимает банк: одна валюта на сервере, сдача и выдача теми же купюрами.
	// Валюты банка «N штук за номинал» (requiredQuantity > 1) планшет не берёт, их принимает только банк.
	// Без банка или с пустым списком банка: Money из конфига планшета.
	static array<ref VB_TabletDenom> Denoms()
	{
#ifdef INT_BANKING_SE
		array<ref INT_BankSE_Denom> bank = INT_BankSE_Money.Denoms();
		if (bank && bank.Count() > 0)
		{
			if (!s_BankDenoms)
				s_BankDenoms = new array<ref VB_TabletDenom>();
			s_BankDenoms.Clear();
			foreach (INT_BankSE_Denom b : bank)
			{
				if (b.Unit == 1 && b.Value > 0)
					s_BankDenoms.Insert(new VB_TabletDenom(b.ClassName, b.Value));
			}
			if (s_BankDenoms.Count() > 0)
				return s_BankDenoms;
		}
#endif
		return VB_TabletConfig.Get().Money;
	}

	protected static int ValueOf(string type, array<ref VB_TabletDenom> denoms)
	{
		string low = type + "";
		low.ToLower();
		foreach (VB_TabletDenom d : denoms)
		{
			string cls = d.Item + "";
			cls.ToLower();
			if (cls == low)
				return d.Value;
		}
		return 0;
	}

	protected static void Collect(PlayerBase player, array<ref VB_TabletCash> out_items)
	{
		out_items.Clear();
		array<ref VB_TabletDenom> denoms = Denoms();
		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI e : items)
		{
			if (!e || e == player || e.IsSetForDeletion())
				continue;
			if (e.GetHierarchyRootPlayer() != player)
				continue;
			int value = ValueOf(e.GetType(), denoms);
			if (value <= 0)
				continue;
			int pcs = PiecesOf(e);
			if (pcs <= 0)
				continue;
			VB_TabletCash c = new VB_TabletCash();
			c.Item = e;
			c.Value = value;
			c.Pieces = pcs;
			// Крупные вперёд: так уходит меньше предметов.
			int at = out_items.Count();
			for (int i = 0; i < out_items.Count(); i++)
			{
				if (value > out_items.Get(i).Value)
				{
					at = i;
					break;
				}
			}
			out_items.InsertAt(c, at);
		}
	}

	static int Count(PlayerBase player)
	{
		if (!player)
			return 0;
		array<ref VB_TabletCash> items = new array<ref VB_TabletCash>();
		Collect(player, items);
		int sum = 0;
		foreach (VB_TabletCash c : items)
			sum += c.Value * c.Pieces;
		return sum;
	}

	// Выдача в инвентарь. false: не поместилось, созданное удалено, ничего не изменилось.
	static bool Give(PlayerBase player, int amount)
	{
		if (amount <= 0)
			return true;
		array<ref VB_TabletDenom> denoms = new array<ref VB_TabletDenom>();
		array<ref VB_TabletDenom> all = Denoms();
		foreach (VB_TabletDenom d : all)
		{
			int at = denoms.Count();
			for (int i = 0; i < denoms.Count(); i++)
			{
				if (d.Value > denoms.Get(i).Value)
				{
					at = i;
					break;
				}
			}
			denoms.InsertAt(d, at);
		}
		array<EntityAI> made = new array<EntityAI>();
		int left = amount;
		foreach (VB_TabletDenom n : denoms)
		{
			while (left >= n.Value)
			{
				EntityAI e = player.GetInventory().CreateInInventory(n.Item);
				if (!e)
				{
					foreach (EntityAI back : made)
						GetGame().ObjectDelete(back);
					return false;
				}
				made.Insert(e);
				ItemBase ib = ItemBase.Cast(e);
				int stack = 1;
				if (ib && ib.HasQuantity())
				{
					stack = Math.Min(left / n.Value, ib.GetQuantityMax());
					ib.SetQuantity(stack);
				}
				left -= stack * n.Value;
			}
		}
		if (left > 0)
		{
			foreach (EntityAI rest : made)
				GetGame().ObjectDelete(rest);
			return false;
		}
		return true;
	}

	// Списание со сдачей. Результат VB_TabletResult.
	static int Charge(PlayerBase player, int amount)
	{
		if (amount <= 0)
			return VB_TabletResult.OK;
		array<ref VB_TabletCash> items = new array<ref VB_TabletCash>();
		Collect(player, items);
		int total = 0;
		foreach (VB_TabletCash c : items)
			total += c.Value * c.Pieces;
		if (total < amount)
			return VB_TabletResult.NO_MONEY;

		int need = amount;
		foreach (VB_TabletCash a : items)
		{
			int take = Math.Min(a.Pieces, need / a.Value);
			a.Take = take;
			need -= take * a.Value;
		}
		int change = 0;
		if (need > 0)
		{
			// Точно не собрать: берём наименьшую купюру, которой хватает, остальное сдача.
			for (int i = items.Count() - 1; i >= 0; i--)
			{
				VB_TabletCash b = items.Get(i);
				if (b.Take < b.Pieces && b.Value >= need)
				{
					b.Take = b.Take + 1;
					change = b.Value - need;
					need = 0;
					break;
				}
			}
		}
		if (need > 0)
		{
			// Мелких хватает только все вместе: забираем всё и сдаём разницу.
			foreach (VB_TabletCash f : items)
				f.Take = f.Pieces;
			change = total - amount;
		}
		if (!Give(player, change))
			return VB_TabletResult.NO_ROOM;
		foreach (VB_TabletCash t : items)
		{
			if (t.Take <= 0 || !t.Item)
				continue;
			ItemBase ib = ItemBase.Cast(t.Item);
			if (t.Take >= t.Pieces || !ib || !ib.HasQuantity())
				GetGame().ObjectDelete(t.Item);
			else
				ib.SetQuantity(t.Pieces - t.Take);
		}
		return VB_TabletResult.OK;
	}
}
