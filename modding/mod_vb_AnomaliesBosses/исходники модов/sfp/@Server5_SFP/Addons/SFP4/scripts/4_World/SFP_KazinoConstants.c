// SFP Kazino - constants, icons and helpers

static const int SFP_KAZINO_RPC_PLAY = 95123001;

// Icon IDs (должны совпадать с логикой текстур и слотов)
static const int SFP_KAZINO_ICON_AXE    = 0;
static const int SFP_KAZINO_ICON_BULLET = 1;
static const int SFP_KAZINO_ICON_CAN    = 2;
static const int SFP_KAZINO_ICON_RAD    = 3;
static const int SFP_KAZINO_ICON_RIFLE  = 4;
static const int SFP_KAZINO_ICON_VODKA  = 5;

static const int SFP_KAZINO_ICON_COUNT  = 6;

// HiddenSelections indices (порядок ДОЛЖЕН совпадать с config.cpp hiddenSelections[])
static const int SFP_KAZINO_SEL_WINDOW_1        = 0; // window_1
static const int SFP_KAZINO_SEL_WINDOW_2        = 1; // window_2
static const int SFP_KAZINO_SEL_WINDOW_3        = 2; // window_3

static const int SFP_KAZINO_SEL_RIFLE_NUMBER1   = 3;
static const int SFP_KAZINO_SEL_RIFLE_NUMBER2   = 4;
static const int SFP_KAZINO_SEL_VODKA_NUMBER1   = 5;
static const int SFP_KAZINO_SEL_VODKA_NUMBER2   = 6;
static const int SFP_KAZINO_SEL_AXE_NUMBER1     = 7;
static const int SFP_KAZINO_SEL_AXE_NUMBER2     = 8;
static const int SFP_KAZINO_SEL_BULLET_NUMBER1  = 9;
static const int SFP_KAZINO_SEL_BULLET_NUMBER2  = 10;
static const int SFP_KAZINO_SEL_CAN_NUMBER1     = 11;
static const int SFP_KAZINO_SEL_CAN_NUMBER2     = 12;
static const int SFP_KAZINO_SEL_RAD_NUMBER1     = 13;
static const int SFP_KAZINO_SEL_RAD_NUMBER2     = 14;

static const int SFP_KAZINO_SELECTION_COUNT     = 15;

// Время анимации (мс)
static const int SFP_KAZINO_SPIN_INTERVAL_MS = 80;
static const int SFP_KAZINO_STOP1_MS = 1600;
static const int SFP_KAZINO_STOP2_MS = 2400;
static const int SFP_KAZINO_STOP3_MS = 3200;

class SFP_KazinoHelpers
{
	// Возвращает "человеческое" (локализованное) имя предмета по classname.
	// Стараемся быть универсальными: предмет может лежать в CfgVehicles/CfgWeapons/CfgMagazines.
	static string GetTypeDisplayName(string type)
	{
		if (type == "")
			return "";

		string name;

		// Большинство инвентарных предметов в DayZ лежат в CfgVehicles.
		name = GetGame().ConfigGetTextOut("CfgVehicles " + type + " displayName");
		if (name != "")
			return name;

		// Оружие/модули часто находятся в CfgWeapons.
		name = GetGame().ConfigGetTextOut("CfgWeapons " + type + " displayName");
		if (name != "")
			return name;

		// Магазины/патроны могут быть в CfgMagazines.
		name = GetGame().ConfigGetTextOut("CfgMagazines " + type + " displayName");
		if (name != "")
			return name;

		// Fallback: если не нашли displayName.
		return type;
	}

	// ===== Icon textures =====
	static string GetIconTexture(int iconId)
	{
		switch (iconId)
		{
			case SFP_KAZINO_ICON_AXE:    return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_axe_co.paa";
			case SFP_KAZINO_ICON_BULLET: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_bullet_co.paa";
			case SFP_KAZINO_ICON_CAN:    return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_can_co.paa";
			case SFP_KAZINO_ICON_RAD:    return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_rad_co.paa";
			case SFP_KAZINO_ICON_RIFLE:  return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_rifle_co.paa";
			case SFP_KAZINO_ICON_VODKA:  return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_vodka_co.paa";
		}

		return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_icon_can_co.paa";
	}

	// ===== Number textures =====
	static string GetDigitTexture(int digit)
	{
		switch (digit)
		{
			case 0: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_0_co.paa";
			case 1: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_1_co.paa";
			case 2: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_2_co.paa";
			case 3: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_3_co.paa";
			case 4: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_4_co.paa";
			case 5: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_5_co.paa";
			case 6: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_6_co.paa";
			case 7: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_7_co.paa";
			case 8: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_8_co.paa";
			case 9: return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_9_co.paa";
		}

		return "SFP3\\1_item\\Kazino\\data\\icons\\kazino_nomber_0_co.paa";
	}

	// ===== Icon name mapping for JSON =====
	static int IconNameToId(string iconName)
	{
		if (iconName == "")
			return -1;

		string n = iconName;
		n.ToLower();

		if (n == "axe")    return SFP_KAZINO_ICON_AXE;
		if (n == "bullet") return SFP_KAZINO_ICON_BULLET;
		if (n == "can")    return SFP_KAZINO_ICON_CAN;
		if (n == "rad")    return SFP_KAZINO_ICON_RAD;
		if (n == "rifle")  return SFP_KAZINO_ICON_RIFLE;
		if (n == "vodka")  return SFP_KAZINO_ICON_VODKA;

		return -1;
	}

	static bool IsTripleWin(int a, int b, int c)
	{
		return (a == b) && (b == c);
	}

	static ItemBase FindAcceptItem(PlayerBase player, string acceptClass)
	{
		if (!player)
			return null;

		EntityAI inHands = player.GetItemInHands();
		if (inHands && inHands.IsKindOf(acceptClass))
			return ItemBase.Cast(inHands);

		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		foreach (EntityAI e : items)
		{
			if (e && e.IsKindOf(acceptClass))
				return ItemBase.Cast(e);
		}

		return null;
	}

	static bool ConsumeOneFrom(ItemBase item)
	{
		if (!item)
			return false;

		// Если предмет не стакается (нет quantity) — просто удаляем один экземпляр.
		// Это делает AcceptItemClass в JSON более универсальным.
		if (!item.HasQuantity())
		{
			item.Delete();
			return true;
		}

		float q = item.GetQuantity();
		if (q <= 0)
			return false;

		if (q > 1)
		{
			item.AddQuantity(-1);
			return true;
		}

		// q == 1
		item.Delete();
		return true;
	}

	static void GiveQuantityToPlayer(PlayerBase player, string payoutClass, int amount)
	{
		if (!player || payoutClass == "" || amount <= 0)
			return;

		int remaining = amount;

		// В DayZ патроны (ammo piles) и магазины лежат в CfgMagazines и используют ammoCount,
		// а не стандартное quantity. Поэтому для таких предметов нужно работать через Magazine.ServerSetAmmoCount().
		bool isCfgMagazine = GetGame().ConfigIsExisting("CfgMagazines " + payoutClass);

		// 1) пытаемся "долить" в существующие стаки/пачки в инвентаре
		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		foreach (EntityAI e : items)
		{
			if (remaining <= 0)
				break;

			if (!e || !e.IsKindOf(payoutClass))
				continue;

			// Magazines / ammo piles (ammoCount)
			if (isCfgMagazine)
			{
				Magazine mag = Magazine.Cast(e);
				if (!mag)
					continue;

				int maxAmmo = mag.GetAmmoMax();
				int curAmmo = mag.GetAmmoCount();
				int freeAmmo = maxAmmo - curAmmo;

				if (freeAmmo <= 0)
					continue;

				int addAmmo = Math.Min(remaining, freeAmmo);
				mag.ServerSetAmmoCount(curAmmo + addAmmo);
				remaining -= addAmmo;
				continue;
			}

			// Standard quantity stacks
			ItemBase stack = ItemBase.Cast(e);
			if (!stack || !stack.HasQuantity())
				continue;

			float maxq = stack.GetQuantityMax();
			float curq = stack.GetQuantity();
			float free = maxq - curq;

			if (free <= 0)
				continue;

			int addQ = Math.Min(remaining, (int)free);
			stack.AddQuantity(addQ);
			remaining -= addQ;
		}

		// 2) создаём новые предметы, если нужно
		while (remaining > 0)
		{
			EntityAI created = player.GetInventory().CreateInInventory(payoutClass);
			if (!created)
				return;

			// Magazines / ammo piles (ammoCount)
			if (isCfgMagazine)
			{
				Magazine magNew = Magazine.Cast(created);
				if (!magNew)
				{
					// На всякий случай (если класс оказался не Magazine), считаем как "1 штука"
					remaining -= 1;
					continue;
				}

				int maxAmmo2 = magNew.GetAmmoMax();
				int addAmmo2 = Math.Min(remaining, maxAmmo2);
				magNew.ServerSetAmmoCount(addAmmo2);
				remaining -= addAmmo2;
				continue;
			}

			// Standard quantity stacks
			ItemBase newStack = ItemBase.Cast(created);
			if (newStack && newStack.HasQuantity())
			{
				int maxStack = (int)newStack.GetQuantityMax();
				int add2 = Math.Min(remaining, maxStack);
				newStack.SetQuantity(add2);
				remaining -= add2;
			}
			else
			{
				// Не стакается — выдаём поштучно
				remaining -= 1;
			}
		}
	}

};
