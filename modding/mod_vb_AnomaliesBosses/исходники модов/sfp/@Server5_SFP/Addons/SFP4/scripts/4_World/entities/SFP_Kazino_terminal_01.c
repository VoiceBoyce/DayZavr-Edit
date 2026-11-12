class SFP_Kazino_terminal_01 : Container_Base
{
	protected bool m_IsBusy;

	// Кэш индексов hiddenSelections.
	// Важно: не полагаемся на "жёсткие" индексы, т.к. базовые классы/наследование могут добавлять свои hiddenSelections.
	protected int m_SelWindow1 = -1;
	protected int m_SelWindow2 = -1;
	protected int m_SelWindow3 = -1;

	protected int m_SelRifleNumber1  = -1;
	protected int m_SelRifleNumber2  = -1;
	protected int m_SelVodkaNumber1  = -1;
	protected int m_SelVodkaNumber2  = -1;
	protected int m_SelAxeNumber1    = -1;
	protected int m_SelAxeNumber2    = -1;
	protected int m_SelBulletNumber1 = -1;
	protected int m_SelBulletNumber2 = -1;
	protected int m_SelCanNumber1    = -1;
	protected int m_SelCanNumber2    = -1;
	protected int m_SelRadNumber1    = -1;
	protected int m_SelRadNumber2    = -1;

	// Финальные иконки (для синхронизации и JIP)
	protected int m_FinalIcon0 = SFP_KAZINO_ICON_CAN;
	protected int m_FinalIcon1 = SFP_KAZINO_ICON_CAN;
	protected int m_FinalIcon2 = SFP_KAZINO_ICON_CAN;

	// Выплаты (для вывода цифр на модели; синхронизируются с сервера)
	protected int m_PayoutRifle  = 30;
	protected int m_PayoutVodka  = 25;
	protected int m_PayoutAxe    = 20;
	protected int m_PayoutBullet = 15;
	protected int m_PayoutCan    = 10;
	protected int m_PayoutRad    = 5;

	// Client-side spin
	// Используем один мастер-таймер на клиенте. Это исключает ситуации,
	// когда 2-е/3-е окно визуально обновляется реже из‑за конкуренции таймеров.
	protected ref Timer m_SpinMasterTimer;
	protected bool m_SpinActive0;
	protected bool m_SpinActive1;
	protected bool m_SpinActive2;

	protected int m_ClientFinal0;
	protected int m_ClientFinal1;
	protected int m_ClientFinal2;
	protected int m_ClientPayout;

	void SFP_Kazino_terminal_01()
	{
		RegisterNetSyncVariableBool("m_IsBusy");
		RegisterNetSyncVariableInt("m_FinalIcon0", 0, SFP_KAZINO_ICON_COUNT - 1);
		RegisterNetSyncVariableInt("m_FinalIcon1", 0, SFP_KAZINO_ICON_COUNT - 1);
		RegisterNetSyncVariableInt("m_FinalIcon2", 0, SFP_KAZINO_ICON_COUNT - 1);

		RegisterNetSyncVariableInt("m_PayoutRifle", 0, 99);
		RegisterNetSyncVariableInt("m_PayoutVodka", 0, 99);
		RegisterNetSyncVariableInt("m_PayoutAxe", 0, 99);
		RegisterNetSyncVariableInt("m_PayoutBullet", 0, 99);
		RegisterNetSyncVariableInt("m_PayoutCan", 0, 99);
		RegisterNetSyncVariableInt("m_PayoutRad", 0, 99);
	}

	protected void CacheHiddenSelectionIndices()
	{
		m_SelWindow1 = GetHiddenSelectionIndex("window_1");
		m_SelWindow2 = GetHiddenSelectionIndex("window_2");
		m_SelWindow3 = GetHiddenSelectionIndex("window_3");

		m_SelRifleNumber1  = GetHiddenSelectionIndex("rifle_number1");
		m_SelRifleNumber2  = GetHiddenSelectionIndex("rifle_number2");
		m_SelVodkaNumber1  = GetHiddenSelectionIndex("vodka_number1");
		m_SelVodkaNumber2  = GetHiddenSelectionIndex("vodka_number2");
		m_SelAxeNumber1    = GetHiddenSelectionIndex("axe_number1");
		m_SelAxeNumber2    = GetHiddenSelectionIndex("axe_number2");
		m_SelBulletNumber1 = GetHiddenSelectionIndex("bullet_number1");
		m_SelBulletNumber2 = GetHiddenSelectionIndex("bullet_number2");
		m_SelCanNumber1    = GetHiddenSelectionIndex("can_number1");
		m_SelCanNumber2    = GetHiddenSelectionIndex("can_number2");
		m_SelRadNumber1    = GetHiddenSelectionIndex("rad_number1");
		m_SelRadNumber2    = GetHiddenSelectionIndex("rad_number2");

		// fallback на старую схему индексов (если по какой-то причине не находится по имени)
		if (m_SelWindow1 < 0) m_SelWindow1 = SFP_KAZINO_SEL_WINDOW_1;
		if (m_SelWindow2 < 0) m_SelWindow2 = SFP_KAZINO_SEL_WINDOW_2;
		if (m_SelWindow3 < 0) m_SelWindow3 = SFP_KAZINO_SEL_WINDOW_3;

		if (m_SelRifleNumber1 < 0)  m_SelRifleNumber1  = SFP_KAZINO_SEL_RIFLE_NUMBER1;
		if (m_SelRifleNumber2 < 0)  m_SelRifleNumber2  = SFP_KAZINO_SEL_RIFLE_NUMBER2;
		if (m_SelVodkaNumber1 < 0)  m_SelVodkaNumber1  = SFP_KAZINO_SEL_VODKA_NUMBER1;
		if (m_SelVodkaNumber2 < 0)  m_SelVodkaNumber2  = SFP_KAZINO_SEL_VODKA_NUMBER2;
		if (m_SelAxeNumber1 < 0)    m_SelAxeNumber1    = SFP_KAZINO_SEL_AXE_NUMBER1;
		if (m_SelAxeNumber2 < 0)    m_SelAxeNumber2    = SFP_KAZINO_SEL_AXE_NUMBER2;
		if (m_SelBulletNumber1 < 0) m_SelBulletNumber1 = SFP_KAZINO_SEL_BULLET_NUMBER1;
		if (m_SelBulletNumber2 < 0) m_SelBulletNumber2 = SFP_KAZINO_SEL_BULLET_NUMBER2;
		if (m_SelCanNumber1 < 0)    m_SelCanNumber1    = SFP_KAZINO_SEL_CAN_NUMBER1;
		if (m_SelCanNumber2 < 0)    m_SelCanNumber2    = SFP_KAZINO_SEL_CAN_NUMBER2;
		if (m_SelRadNumber1 < 0)    m_SelRadNumber1    = SFP_KAZINO_SEL_RAD_NUMBER1;
		if (m_SelRadNumber2 < 0)    m_SelRadNumber2    = SFP_KAZINO_SEL_RAD_NUMBER2;
	}

	protected void SetObjectTextureSafe(int selIdx, string texturePath)
	{
		if (selIdx < 0)
			return;
		SetObjectTexture(selIdx, texturePath);
	}

	// Запрет брать в руки/инвентарь
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		return false;
	}

	override bool CanLoadItemIntoCargo(EntityAI item)
	{
		return false;
	}

	override bool IsInventoryVisible()
	{
		return false;
	}

	override bool CanDisplayCargo()
	{
		return false;
	}

	bool IsBusy()
	{
		return m_IsBusy;
	}

	override bool IsTakeable()
	{
		return false;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_KazinoPressButton);
	}

	override void EEInit()
	{
		super.EEInit();

		CacheHiddenSelectionIndices();

		// Сервер применяет JSON к net vars (чтобы цифры у клиентов совпадали с серверными выплатами)
		if (GetGame().IsServer())
		{
			ApplyConfigServer();
		}

		if (!GetGame().IsDedicatedServer())
		{
			ApplyPayoutBoardTextures();
			ApplyFinalTextures();
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (!GetGame().IsDedicatedServer())
		{
			// цифры обновляются всегда (на случай JIP)
			ApplyPayoutBoardTextures();

			// не трогаем экраны во время анимации, чтобы не мешать прокрутке
			if (!m_IsBusy)
			{
				ApplyFinalTextures();
			}
		}
	}

	protected void ApplyFinalTextures()
	{
		SetObjectTextureSafe(m_SelWindow1, SFP_KazinoHelpers.GetIconTexture(m_FinalIcon0));
		SetObjectTextureSafe(m_SelWindow2, SFP_KazinoHelpers.GetIconTexture(m_FinalIcon1));
		SetObjectTextureSafe(m_SelWindow3, SFP_KazinoHelpers.GetIconTexture(m_FinalIcon2));
	}

	// ===== Плата с выплатами (цифры) =====

	protected void ApplyPayoutBoardTextures()
	{
		SetTwoDigitNumber(m_SelRifleNumber1,  m_SelRifleNumber2,  m_PayoutRifle);
		SetTwoDigitNumber(m_SelVodkaNumber1,  m_SelVodkaNumber2,  m_PayoutVodka);
		SetTwoDigitNumber(m_SelAxeNumber1,    m_SelAxeNumber2,    m_PayoutAxe);
		SetTwoDigitNumber(m_SelBulletNumber1, m_SelBulletNumber2, m_PayoutBullet);
		SetTwoDigitNumber(m_SelCanNumber1,    m_SelCanNumber2,    m_PayoutCan);
		SetTwoDigitNumber(m_SelRadNumber1,    m_SelRadNumber2,    m_PayoutRad);
	}

	protected void SetTwoDigitNumber(int sel1, int sel2, int value)
	{
		int v = Math.Clamp(value, 0, 99);
		int tens = v / 10;
		int ones = v - (tens * 10);

		SetObjectTextureSafe(sel1, SFP_KazinoHelpers.GetDigitTexture(tens));
		SetObjectTextureSafe(sel2, SFP_KazinoHelpers.GetDigitTexture(ones));
	}

	// Серверная загрузка/применение выплат из JSON
	protected void ApplyConfigServer()
	{
		if (!GetGame().IsServer())
			return;

		// дефолты
		int rifle  = 30;
		int vodka  = 25;
		int axe    = 20;
		int bullet = 15;
		int can    = 10;
		int rad    = 5;

		SFP_KazinoConfig cfg = SFP_KazinoConfigManager.Get();
		if (cfg)
		{
			// 1) Если админ использует Wins — берём выплаты оттуда
			if (cfg.Wins && cfg.Wins.Count() > 0)
			{
				foreach (SFP_KazinoWinEntry e : cfg.Wins)
				{
					if (!e)
						continue;

					int id = SFP_KazinoHelpers.IconNameToId(e.Icon);
					if (id < 0 || id >= SFP_KAZINO_ICON_COUNT)
						continue;

					int p = Math.Clamp(e.Payout, 0, 99);

					if (id == SFP_KAZINO_ICON_RIFLE)       rifle  = p;
					else if (id == SFP_KAZINO_ICON_VODKA)  vodka  = p;
					else if (id == SFP_KAZINO_ICON_AXE)    axe    = p;
					else if (id == SFP_KAZINO_ICON_BULLET) bullet = p;
					else if (id == SFP_KAZINO_ICON_CAN)    can    = p;
					else if (id == SFP_KAZINO_ICON_RAD)    rad    = p;
				}
			}
			// 2) Если Wins пустой, но заданы Combos — попробуем вывести выплаты для платы
			else if (cfg.Combos && cfg.Combos.Count() > 0)
			{
				foreach (SFP_KazinoComboEntry ce : cfg.Combos)
				{
					if (!ce || !ce.Icons || ce.Icons.Count() < 3)
						continue;

					int i0 = SFP_KazinoHelpers.IconNameToId(ce.Icons[0]);
					int i1 = SFP_KazinoHelpers.IconNameToId(ce.Icons[1]);
					int i2 = SFP_KazinoHelpers.IconNameToId(ce.Icons[2]);
					if (i0 < 0 || i1 < 0 || i2 < 0)
						continue;

					// Плата отображает только 3 одинаковых (jackpot по иконке)
					if (i0 != i1 || i1 != i2)
						continue;

					int p2 = Math.Clamp(ce.Payout, 0, 99);
					if (i0 == SFP_KAZINO_ICON_RIFLE)       rifle  = p2;
					else if (i0 == SFP_KAZINO_ICON_VODKA)  vodka  = p2;
					else if (i0 == SFP_KAZINO_ICON_AXE)    axe    = p2;
					else if (i0 == SFP_KAZINO_ICON_BULLET) bullet = p2;
					else if (i0 == SFP_KAZINO_ICON_CAN)    can    = p2;
					else if (i0 == SFP_KAZINO_ICON_RAD)    rad    = p2;
				}
			}
		}

		m_PayoutRifle  = rifle;
		m_PayoutVodka  = vodka;
		m_PayoutAxe    = axe;
		m_PayoutBullet = bullet;
		m_PayoutCan    = can;
		m_PayoutRad    = rad;

		SetSynchDirty();
	}

	// ===== Server gameplay =====

	void TryPlayServer(PlayerBase player)
	{
		if (!GetGame().IsServer())
			return;

		if (m_IsBusy)
			return;

		SFP_KazinoConfig cfg = SFP_KazinoConfigManager.Get();
		string acceptClass = "";
		if (cfg)
			acceptClass = cfg.AcceptItemClass;
		if (acceptClass == "")
			acceptClass = "SFP_Kazino_fishka100";

		ItemBase acceptItem = SFP_KazinoHelpers.FindAcceptItem(player, acceptClass);
		if (!acceptItem)
		{
			string acceptName = SFP_KazinoHelpers.GetTypeDisplayName(acceptClass);
			// Сообщаем игроку, почему ничего не происходит.
			NotificationSystem.SendNotificationToPlayerExtended(player, 3.0, "Казино", "Нужен предмет: " + acceptName);
			return;
		}

		if (!SFP_KazinoHelpers.ConsumeOneFrom(acceptItem))
		{
			string acceptName2 = SFP_KazinoHelpers.GetTypeDisplayName(acceptClass);
			NotificationSystem.SendNotificationToPlayerExtended(player, 3.0, "Казино", "Не удалось списать предмет: " + acceptName2);
			return;
		}

		int a, b, c, payout;
		RollOutcome(cfg, a, b, c, payout);

		m_FinalIcon0 = a;
		m_FinalIcon1 = b;
		m_FinalIcon2 = c;

		m_IsBusy = true;
		SetSynchDirty();

		// Запускаем анимацию на клиентах
		Param1<string> p = new Param1<string>(a.ToString() + "|" + b.ToString() + "|" + c.ToString() + "|" + payout.ToString());
		GetGame().RPCSingleParam(this, SFP_KAZINO_RPC_PLAY, p, true);

		// Выплата после остановки 3-го окна
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FinishPlayServer, SFP_KAZINO_STOP3_MS + 250, false, player, payout);
	}

	protected void FinishPlayServer(PlayerBase player, int payout)
	{
		if (!GetGame().IsServer())
			return;

		SFP_KazinoConfig cfg = SFP_KazinoConfigManager.Get();

		if (payout > 0)
		{
			SFP_KazinoHelpers.GiveQuantityToPlayer(player, cfg.PayoutItemClass, payout);
		}

		m_IsBusy = false;
		SetSynchDirty();
	}

	// Выбор исхода:
	// - если выпал выигрыш: 3 одинаковые иконки + выплата по JSON
	// - иначе: случайные иконки, но не 3 одинаковые
	protected float ChanceToProb(float chanceValue, bool percentMode)
	{
		if (percentMode)
			return chanceValue / 100.0;
		return chanceValue;
	}

	protected void Sort3(inout int x, inout int y, inout int z)
	{
		int t;
		if (x > y) { t = x; x = y; y = t; }
		if (y > z) { t = y; y = z; z = t; }
		if (x > y) { t = x; x = y; y = t; }
	}

	protected bool MatchAnyOrder3(int a, int b, int c, int x, int y, int z)
	{
		int aa = a, bb = b, cc = c;
		int xx = x, yy = y, zz = z;
		Sort3(aa, bb, cc);
		Sort3(xx, yy, zz);
		return (aa == xx) && (bb == yy) && (cc == zz);
	}

	protected void Shuffle3(inout int a, inout int b, inout int c)
	{
		int x = a;
		int y = b;
		int z = c;
		int r = Math.RandomInt(0, 6);
		switch (r)
		{
			case 0: a = x; b = y; c = z; break;
			case 1: a = x; b = z; c = y; break;
			case 2: a = y; b = x; c = z; break;
			case 3: a = y; b = z; c = x; break;
			case 4: a = z; b = x; c = y; break;
			default: a = z; b = y; c = x; break;
		}
	}

	protected bool IsWinningVisual(int a, int b, int c, SFP_KazinoConfig cfg)
	{
		if (cfg && cfg.Combos && cfg.Combos.Count() > 0)
		{
			foreach (SFP_KazinoComboEntry ce : cfg.Combos)
			{
				if (!ce || !ce.Icons || ce.Icons.Count() < 3)
					continue;

				int i0 = SFP_KazinoHelpers.IconNameToId(ce.Icons[0]);
				int i1 = SFP_KazinoHelpers.IconNameToId(ce.Icons[1]);
				int i2 = SFP_KazinoHelpers.IconNameToId(ce.Icons[2]);
				if (i0 < 0 || i1 < 0 || i2 < 0)
					continue;

				if (ce.AnyOrder)
				{
					if (MatchAnyOrder3(a, b, c, i0, i1, i2))
						return true;
				}
				else
				{
					if (a == i0 && b == i1 && c == i2)
						return true;
				}
			}
			return false;
		}

		return SFP_KazinoHelpers.IsTripleWin(a, b, c);
	}

	protected void RollLoseNoWinCombos(SFP_KazinoConfig cfg, out int a, out int b, out int c)
	{
		// Enforce Script не поддерживает do..while, поэтому делаем обычный while.
		int tries = 0;
		while (true)
		{
			a = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
			b = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
			c = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
			tries++;

			if (!IsWinningVisual(a, b, c, cfg))
				break;
			if (tries >= 25)
				break;
		}

		// жёсткий разрыв, если вдруг не удалось
		if (IsWinningVisual(a, b, c, cfg))
		{
			for (int k = 0; k < SFP_KAZINO_ICON_COUNT; k++)
			{
				c = (c + 1) % SFP_KAZINO_ICON_COUNT;
				if (!IsWinningVisual(a, b, c, cfg))
					break;
			}
		}
	}

	// Выбор исхода:
	// - если выпал выигрыш: выплата по JSON
	// - иначе: случайные иконки, но так, чтобы визуально не повторять выигрышную комбинацию
	protected void RollOutcome(SFP_KazinoConfig cfg, out int a, out int b, out int c, out int payout)
	{
		payout = 0;

		bool percentMode = false;
		float multiplier = 1.0;
		bool useGlobalWinChance = false;
		float globalWinProb = 0.0; // 0..1
		if (cfg)
		{
			multiplier = Math.Max(cfg.WinChanceMultiplier, 0.0);
			string cm = cfg.ChanceMode;
			cm.ToLower();
			if (cm == "percent")
				percentMode = true;

			// Если задано OverallWinChancePercent (>0), то это и есть общий шанс "выиграть что угодно" за спин.
			if (cfg.OverallWinChancePercent > 0.0)
			{
				useGlobalWinChance = true;
				globalWinProb = Math.Clamp(cfg.OverallWinChancePercent / 100.0, 0.0, 1.0);
			}
		}

		// ===== v2: если Combos задана — используем её (для более продвинутых комбинаций) =====
		if (cfg && cfg.Combos && cfg.Combos.Count() > 0)
		{
			ref array<int> wa = new array<int>;
			ref array<int> wb = new array<int>;
			ref array<int> wc = new array<int>;
			ref array<int> wp = new array<int>;
			ref array<float> wch = new array<float>;
			ref array<bool> any = new array<bool>;

			foreach (SFP_KazinoComboEntry ce : cfg.Combos)
			{
				if (!ce || !ce.Icons || ce.Icons.Count() < 3)
					continue;

				int i0 = SFP_KazinoHelpers.IconNameToId(ce.Icons[0]);
				int i1 = SFP_KazinoHelpers.IconNameToId(ce.Icons[1]);
				int i2 = SFP_KazinoHelpers.IconNameToId(ce.Icons[2]);
				if (i0 < 0 || i1 < 0 || i2 < 0)
					continue;

				float prob = ChanceToProb(ce.Chance, percentMode) * multiplier;
				if (useGlobalWinChance)
				{
					// в режиме глобального шанса используем Chance как "вес" (без Clamp до 1.0)
					prob = Math.Max(prob, 0.0);
				}
				else
				{
					prob = Math.Clamp(prob, 0.0, 1.0);
				}
				if (prob <= 0.0)
					continue;

				wa.Insert(i0);
				wb.Insert(i1);
				wc.Insert(i2);
				wp.Insert(Math.Clamp(ce.Payout, 0, 99));
				wch.Insert(prob);
				any.Insert(ce.AnyOrder);
			}

			float totalChance = 0.0;
			for (int i0b = 0; i0b < wch.Count(); i0b++)
				totalChance += wch[i0b];

			float winProb;
			if (useGlobalWinChance)
				winProb = globalWinProb;
			else
				winProb = Math.Clamp(totalChance, 0.0, 1.0);
			if (totalChance <= 0.0)
				winProb = 0.0;
			float r0 = Math.RandomFloat01();
			if (winProb > 0.0 && r0 < winProb && wch.Count() > 0)
			{
				float rr0 = r0 / winProb;
				float cum0 = 0.0;
				int chosen0 = -1;

				float denom0 = totalChance;
				if (denom0 <= 0.0)
					denom0 = 1.0;

				for (int j0 = 0; j0 < wch.Count(); j0++)
				{
					float pc = wch[j0];
					if (pc <= 0.0)
						continue;

					cum0 += (pc / denom0);
					if (rr0 < cum0)
					{
						chosen0 = j0;
						break;
					}
				}

				if (chosen0 == -1)
					chosen0 = 0;

				a = wa[chosen0];
				b = wb[chosen0];
				c = wc[chosen0];
				if (any[chosen0])
					Shuffle3(a, b, c);
				payout = wp[chosen0];
				return;
			}

			// Проигрыш (важно: не дать выпадать выигрышным комбинациям визуально)
			RollLoseNoWinCombos(cfg, a, b, c);
			payout = 0;
			return;
		}

		// ===== Старый режим: Wins (3 одинаковые) =====

		// собираем шансы/выплаты по иконкам (0..5)
		ref array<float> chances = new array<float>;
		chances.Resize(SFP_KAZINO_ICON_COUNT);

		ref array<int> payouts = new array<int>;
		payouts.Resize(SFP_KAZINO_ICON_COUNT);

		// дефолтные выплаты берём из net vars (они уже применены с сервера из JSON)
		payouts[SFP_KAZINO_ICON_RIFLE]  = m_PayoutRifle;
		payouts[SFP_KAZINO_ICON_VODKA]  = m_PayoutVodka;
		payouts[SFP_KAZINO_ICON_AXE]    = m_PayoutAxe;
		payouts[SFP_KAZINO_ICON_BULLET] = m_PayoutBullet;
		payouts[SFP_KAZINO_ICON_CAN]    = m_PayoutCan;
		payouts[SFP_KAZINO_ICON_RAD]    = m_PayoutRad;

		if (cfg && cfg.Wins)
		{
			foreach (SFP_KazinoWinEntry e2 : cfg.Wins)
			{
				if (!e2)
					continue;

				int id2 = SFP_KazinoHelpers.IconNameToId(e2.Icon);
				if (id2 < 0 || id2 >= SFP_KAZINO_ICON_COUNT)
					continue;

				float prob2 = ChanceToProb(e2.Chance, percentMode) * multiplier;
				chances[id2] = Math.Clamp(prob2, 0.0, 1.0);
				payouts[id2] = Math.Clamp(e2.Payout, 0, 99);
			}
		}

		float totalChance2 = 0.0;
		for (int i2b = 0; i2b < SFP_KAZINO_ICON_COUNT; i2b++)
			totalChance2 += chances[i2b];

		float winProb2;
		if (useGlobalWinChance)
			winProb2 = globalWinProb;
		else
			winProb2 = Math.Clamp(totalChance2, 0.0, 1.0);
		if (totalChance2 <= 0.0)
			winProb2 = 0.0;
		float r2 = Math.RandomFloat01();
		if (winProb2 > 0.0 && r2 < winProb2)
		{
			float rr2 = r2 / winProb2;
			float cum2 = 0.0;
			int chosen2 = -1;

			float denom2 = totalChance2;
			if (denom2 <= 0.0)
				denom2 = 1.0;

			for (int j2b = 0; j2b < SFP_KAZINO_ICON_COUNT; j2b++)
			{
				if (chances[j2b] <= 0.0)
					continue;

				cum2 += (chances[j2b] / denom2);
				if (rr2 < cum2)
				{
					chosen2 = j2b;
					break;
				}
			}

			if (chosen2 == -1)
			{
				for (int k2b = 0; k2b < SFP_KAZINO_ICON_COUNT; k2b++)
				{
					if (chances[k2b] > 0.0)
					{
						chosen2 = k2b;
						break;
					}
				}
			}

			if (chosen2 != -1)
			{
				a = chosen2;
				b = chosen2;
				c = chosen2;
				payout = payouts[chosen2];
				return;
			}
		}

		// Проигрыш: случайные иконки, но не три одинаковые
		a = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
		b = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
		c = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);

		if (SFP_KazinoHelpers.IsTripleWin(a, b, c))
			c = (c + 1) % SFP_KAZINO_ICON_COUNT;

		payout = 0;
	}

	// ===== Client visuals & audio =====

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == SFP_KAZINO_RPC_PLAY)
		{
			Param1<string> p1;
			if (!ctx.Read(p1))
				return;

			if (GetGame().IsDedicatedServer())
				return;

			TStringArray parts = new TStringArray;
			p1.param1.Split("|", parts);
			if (parts.Count() < 4)
				return;

			int final0 = parts.Get(0).ToInt();
			int final1 = parts.Get(1).ToInt();
			int final2 = parts.Get(2).ToInt();
			int payout  = parts.Get(3).ToInt();
			StartSpinClient(final0, final1, final2, payout);
		}
	}

	protected void StartSpinClient(int final0, int final1, int final2, int payout)
	{
		StopAllClientSpin();

		m_ClientFinal0 = final0;
		m_ClientFinal1 = final1;
		m_ClientFinal2 = final2;
		m_ClientPayout = payout;


		// Старт прокрутки: 1-е окно сразу, 2-е/3-е с задержкой.
		m_SpinActive0 = true;
		m_SpinActive1 = false;
		m_SpinActive2 = false;

		if (!m_SpinMasterTimer)
			m_SpinMasterTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		// Один повторяющийся тик обновляет все активные окна.
		m_SpinMasterTimer.Run(SFP_KAZINO_SPIN_INTERVAL_MS / 1000.0, this, "SpinTickMaster", NULL, true);

		PlaySoundSet("SFP_kazik_start_window01_Soundset");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StartSpinWindowDelayed, 150, false, 1);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StartSpinWindowDelayed, 300, false, 2);

		// остановка по очереди
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopSpinWindow, SFP_KAZINO_STOP1_MS, false, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopSpinWindow, SFP_KAZINO_STOP2_MS, false, 1);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopSpinWindow, SFP_KAZINO_STOP3_MS, false, 2);
	}

	protected void StartSpinWindowDelayed(int idx)
	{
		if (idx == 1)
		{
			PlaySoundSet("SFP_kazik_start_window02_Soundset");
			m_SpinActive1 = true;
		}
		else if (idx == 2)
		{
			PlaySoundSet("SFP_kazik_start_window03_Soundset");
			m_SpinActive2 = true;
		}
	}

	protected void SpinTickMaster()
	{
		if (m_SpinActive0)
			SpinOneWindow(m_SelWindow1);
		if (m_SpinActive1)
			SpinOneWindow(m_SelWindow2);
		if (m_SpinActive2)
			SpinOneWindow(m_SelWindow3);
	}

	protected void SpinOneWindow(int selIdx)
	{
		int icon = Math.RandomInt(0, SFP_KAZINO_ICON_COUNT);
		SetObjectTextureSafe(selIdx, SFP_KazinoHelpers.GetIconTexture(icon));
	}

	protected void StopSpinWindow(int idx)
	{
		if (idx == 0) m_SpinActive0 = false;
		else if (idx == 1) m_SpinActive1 = false;
		else m_SpinActive2 = false;

		int finalIcon = GetClientFinalByIndex(idx);
		int selIdx2 = m_SelWindow1;
		if (idx == 1) selIdx2 = m_SelWindow2;
		else if (idx == 2) selIdx2 = m_SelWindow3;
		SetObjectTextureSafe(selIdx2, SFP_KazinoHelpers.GetIconTexture(finalIcon));

		PlaySoundSet("SFP_kazik_stop_window_Soundset");

		if (idx == 2)
		{
			// Последнее окно остановилось — останавливаем мастер-таймер
			if (m_SpinMasterTimer)
				m_SpinMasterTimer.Stop();

			// Последнее окно остановилось — если выигрыш, проиграть звук победы
			if (m_ClientPayout > 0)
				PlaySoundSet("SFP_kazik_winner_Soundset");
		}
	}

	protected int GetClientFinalByIndex(int idx)
	{
		if (idx == 0) return m_ClientFinal0;
		if (idx == 1) return m_ClientFinal1;
		return m_ClientFinal2;
	}

	protected void StopAllClientSpin()
	{
		m_SpinActive0 = false;
		m_SpinActive1 = false;
		m_SpinActive2 = false;

		if (m_SpinMasterTimer)
			m_SpinMasterTimer.Stop();

		// Снимаем отложенные вызовы, чтобы прежний запуск не влиял на текущий.
		ScriptCallQueue cq = GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		if (cq)
		{
			cq.RemoveByName(this, "StartSpinWindowDelayed");
			cq.RemoveByName(this, "StopSpinWindow");
		}
	}

	protected void PlaySoundSet(string soundSet)
	{
		EffectSound sound = SEffectManager.PlaySound(soundSet, GetPosition());
		if (sound)
			sound.SetAutodestroy(true);
	}
};
