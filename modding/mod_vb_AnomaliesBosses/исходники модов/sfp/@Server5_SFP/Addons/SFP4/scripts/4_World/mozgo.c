/*class SFP_MozgoTras_Base extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}

	// Всё, что ниже, работает только на клиентах.
	// На dedicated-сервере код под #ifndef SERVER не компилится, так что сервак не будет ругаться.
	#ifndef SERVER

	// --- НАСТРОЙКИ ЗОНЫ ---

	// Радиус действия "ауры" (в метрах)
	const float SFP_DISORIENT_RADIUS = 100.0;

	// Минимальный и максимальный интервал между "волнами" (в миллисекундах)
	const int SFP_DISORIENT_MIN_INTERVAL_MS = 6000;  // 6 сек
	const int SFP_DISORIENT_MAX_INTERVAL_MS = 11000; // 11 сек

	// Длительность самой зоны (в миллисекундах)
	const int SFP_DISORIENT_MIN_DURATION_MS = 6000;   // 6 сек
	const int SFP_DISORIENT_MAX_DURATION_MS = 10000;  // 10 сек

	// Частота тика эффекта (как часто дёргать камеру), 100 мс ≈ 10 раз/сек
	const int SFP_DISORIENT_TICK_MS = 100;

	// --- СОСТОЯНИЕ ЗОНЫ ---

	protected bool        m_SFPDisorientZoneActive;
	protected EffectSound m_SFPDisorientSound;          // звук "мозгового удара"
	protected int         m_SFPDisorientDamageCounter;  // счётчик тиков для урона

	// играет ли сейчас звук ауры "в ушах" у игрока
	protected bool        m_SFPDisorientSoundPlaying;

	// --- ПОЛЯ ДЛЯ ЭФФЕКТОВ ЭКРАНА ---

	// Размытие (ванильный "fever" requester)
	protected PPERequester_FeverEffects m_SFPFeverRequester;
	// Синеватый оттенок (ванильный "spooky area" requester)
	protected PPERequesterBase          m_SFPSpookyRequester;
	// Флаг, что PP-эффекты от этой ауры сейчас включены
	protected bool                      m_SFPPostProcessActive;
	protected bool                      m_SFPFeverStarted;

	// --- ЖИЗНЕННЫЙ ЦИКЛ ЗОМБИ ---

	void SFP_MozgoTras_Base()
	{
		// При создании зомби планируем первую "зону"
		SFP_ScheduleNextDisorientZone();
	}

	void ~SFP_MozgoTras_Base()
	{
		SFP_CancelDisorientZone();
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		SFP_CancelDisorientZone();
	}

	// --- СЛУЖЕБНЫЕ ФУНКЦИИ ---

	// Ленивая инициализация ванильных PPE-requester'ов
	protected void SFP_InitPostProcess()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		if (!m_SFPFeverRequester)
		{
			Class.CastTo(m_SFPFeverRequester, PPERequesterBank.GetRequester(PPERequester_FeverEffects));
		}

		if (!m_SFPSpookyRequester)
		{
			// Ванильный "жуткий"/синеватый фильтр (см. PPERSpooky.c)
			Class.CastTo(m_SFPSpookyRequester, PPERequesterBank.GetRequester(PPERequester_SpookyAreaTint));
		}
	}

	// Полное отключение размытия/оттенка, используемых этой аурой
	protected void SFP_ResetPostProcess()
	{
		if (!GetGame())
			return;

		// Сбрасываем размытие (fever blur)
		if (m_SFPFeverRequester)
		{
			// убираем интенсивность
			m_SFPFeverRequester.SetFeverIntensity(0.0);

			// и именно останавливаем requester
			if (m_SFPFeverStarted)
			{
				m_SFPFeverRequester.Stop();
				m_SFPFeverStarted = false;
			}
		}

		// Выключаем "жуткий" фильтр с плавным фейдом
		if (m_SFPSpookyRequester)
		{
			m_SFPSpookyRequester.Stop(new Param1<bool>(true)); // true = fade out
		}

		m_SFPPostProcessActive = false;
	}

	// Отключаем все таймеры и эффекты, когда зомби умирает/удаляется
	protected void SFP_CancelDisorientZone()
	{
		if (!GetGame())
			return;

		m_SFPDisorientZoneActive    = false;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false;

		// Снимаем все возможные CallLater'ы этой механики
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_StartDisorientZone);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_StopDisorientZone);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_DisorientTick);

		// Останавливаем звук, если играет
		StopSoundSet(m_SFPDisorientSound);

		// Убираем размытие/оттенок
		SFP_ResetPostProcess();
	}

	// Планируем следующую зону через случайный интервал 6–11 секунд
	protected void SFP_ScheduleNextDisorientZone()
	{
		if (!GetGame())
			return;

		int delayMs = Math.RandomIntInclusive(SFP_DISORIENT_MIN_INTERVAL_MS, SFP_DISORIENT_MAX_INTERVAL_MS);

		// Планируем включение зоны
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_StartDisorientZone, delayMs, false);
	}

	// Включаем зону (ауру)
	protected void SFP_StartDisorientZone()
	{
		if (!GetGame())
			return;

		// Если зомби уже уничтожен — ничего не делаем
		if (IsDamageDestroyed())
			return;

		m_SFPDisorientZoneActive    = true;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false; // звук включим на игроке, когда он войдёт в радиус

		// Раньше звук ауры крутился на мутанте:
		// PlaySoundSetLoop(m_SFPDisorientSound, "SFP_MozgoTras_PsiAura_Soundset", 0, 0);
		// Теперь его запускаем на локальном игроке в SFP_DisorientTick().

		// Начинаем периодически накатывать эффект на игрока
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_DisorientTick, SFP_DISORIENT_TICK_MS, true);

		// Случайная длительность зоны 6–10 сек
		int durationMs = Math.RandomIntInclusive(SFP_DISORIENT_MIN_DURATION_MS, SFP_DISORIENT_MAX_DURATION_MS);

		// Планируем выключение зоны
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_StopDisorientZone, durationMs, false);
	}

	// Выключаем зону и планируем следующую
	protected void SFP_StopDisorientZone()
	{
		if (!GetGame())
			return;

		m_SFPDisorientZoneActive    = false;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false;

		// Останавливаем периодическое дёргание камеры/урон
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_DisorientTick);

		// Глушим звук
		StopSoundSet(m_SFPDisorientSound);

		// Убираем размытие и фильтр
		SFP_ResetPostProcess();

		// Если зомби ещё жив — планируем следующую "волну"
		if (!IsDamageDestroyed())
		{
			SFP_ScheduleNextDisorientZone();
		}
	}

	// Тик эффекта: проверяем дистанцию от игрока до мутанта, шатаем камеру,
	// накладываем PP-эффекты, наносим урон и с шансом выбиваем предмет из рук
	// и вызываем рвоту.
	protected void SFP_DisorientTick()
	{
		if (!m_SFPDisorientZoneActive)
			return;

		if (!GetGame())
			return;

		// Локальный игрок на этом клиенте
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.IsAlive())
			return;

		// Проверяем, в радиусе ли он
		vector playerPos = player.GetPosition();
		vector zombiePos = GetPosition();
		float  distSq    = vector.DistanceSq(playerPos, zombiePos);
		float  radiusSq  = SFP_DISORIENT_RADIUS * SFP_DISORIENT_RADIUS;

		if (distSq > radiusSq)
		{
			// Вышли из зоны — убираем эффекты, но зону не выключаем (она может "крутиться" вхолостую)
			if (m_SFPPostProcessActive)
			{
				SFP_ResetPostProcess();
			}

			// И глушим "ушной" звук, если он крутился
			if (m_SFPDisorientSoundPlaying)
			{
				StopSoundSet(m_SFPDisorientSound);
				m_SFPDisorientSoundPlaying = false;
			}

			return;
		}

		// Находимся ВНУТРИ радиуса — инициализируем ванильные requester'ы
		SFP_InitPostProcess();

		// --- ЗВУК "В УШАХ" ---
		// Стартуем луп на самом игроке, чтобы звук не шёл от мутанта в мире,
		// а сразу прилетал в наушники (одинаково, пока игрок в зоне).
		if (!m_SFPDisorientSoundPlaying)
		{
			player.PlaySoundSetLoop(m_SFPDisorientSound, "SFP_MozgoTras_PsiAura_Soundset", 0.0, 0.0);
			m_SFPDisorientSoundPlaying = true;
		}

		// --- ТРЯСКА КАМЕРЫ ---
		float strength = Math.RandomFloatInclusive(1.0, 2.8);
		player.GetCurrentCamera().SpawnCameraShake(strength);

		// --- РАЗМЫТИЕ ЭКРАНА (Fever blur) ---

		if (m_SFPFeverRequester)
		{
			// Первый раз в зоне — запускаем requester
			if (!m_SFPFeverStarted)
			{
				m_SFPFeverRequester.Start();
				m_SFPFeverStarted = true;
			}

			// Чем ближе к мутанту — тем сильнее размытие/засвет
			float dist   = Math.Sqrt(distSq);
			float factor = 1.0 - (dist / SFP_DISORIENT_RADIUS); // 0..1
			if (factor < 0.0)
				factor = 0.0;
			if (factor > 1.0)
				factor = 1.0;

			// Лёгкий "флеш": не белый в ноль, а умеренный пересвет
			float blurStrength = Math.Lerp(0.1, 0.3, factor);

			m_SFPFeverRequester.SetFeverIntensity(blurStrength);
		}

		// --- СИНЕВАТЫЙ ОТТЕНОК (SpookyAreaTint) ---

		if (m_SFPSpookyRequester && !m_SFPPostProcessActive)
		{
			m_SFPSpookyRequester.Start(); // fade-in внутри самого requester'а
		}

		m_SFPPostProcessActive = true;

		// --- УРОН ПО ЗДОРОВЬЮ + ШАНС ВЫРОНИТЬ ПРЕДМЕТ + ШАНС РВОТЫ ---

		// При SFP_DISORIENT_TICK_MS = 100 мс 10 тиков ≈ 1 секунда.
		m_SFPDisorientDamageCounter++;

		int ticksPerSecond = 1000 / SFP_DISORIENT_TICK_MS; // здесь будет 10 при 100 мс
		if (m_SFPDisorientDamageCounter >= ticksPerSecond)
		{
			// Минус 2 HP раз в секунду
			player.AddHealth("", "", -2);
			m_SFPDisorientDamageCounter = 0;

			// --- ШАНС ВЫРОНИТЬ ПРЕДМЕТ ИЗ РУК ---
			// Здесь пока 50% (0 или 1). Можешь поменять на своё.
			int dropRoll = Math.RandomIntInclusive(0, 4); // 0 или 1
			if (dropRoll == 0) // ~50%
			{
				ItemBase itemInHands = ItemBase.Cast(player.GetItemInHands());
				if (itemInHands)
				{
					// Ванильный способ выбросить предмет из рук
					player.DropItem(itemInHands);
				}
			}

			// --- ШАНС ВЫЗВАТЬ РВОТУ (25%) ---
			// Аналогично PoisoningMdfr: просто вероятность по RandomFloat01().
			SymptomManager symMgr = player.GetSymptomManager();
			if (symMgr && Math.RandomFloat01() < 0.10) // 10% раз в секунду, пока в зоне
			{
				SymptomBase symptom = symMgr.QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
				if (symptom)
				{
					CachedObjectsParams.PARAM1_FLOAT.param1 = 65.0;
					symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
					symptom.SetDuration(Math.RandomFloatInclusive(4.0, 8.0)); // длительность, как у болезней
				}
			}
		}
	}

	#endif // SERVER
}


/*class SFP_MozgoTras_Base extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}

	// Всё, что ниже, работает только на клиентах.
	// На dedicated-сервере код под #ifndef SERVER не компилится, так что сервак не будет ругаться.
	#ifndef SERVER

	// --- НАСТРОЙКИ ЗОНЫ ---

	// Радиус действия "ауры" (в метрах)
	const float SFP_DISORIENT_RADIUS = 100.0;

	// Минимальный и максимальный интервал между "волнами" (в миллисекундах)
	const int SFP_DISORIENT_MIN_INTERVAL_MS = 6000;  // 6 сек
	const int SFP_DISORIENT_MAX_INTERVAL_MS = 11000; // 11 сек

	// Длительность самой зоны (в миллисекундах)
	const int SFP_DISORIENT_MIN_DURATION_MS = 6000;   // 6 сек
	const int SFP_DISORIENT_MAX_DURATION_MS = 10000;  // 10 сек

	// Частота тика эффекта (как часто дёргать камеру), 100 мс ≈ 10 раз/сек
	const int SFP_DISORIENT_TICK_MS = 100;

	// --- СОСТОЯНИЕ ЗОНЫ ---

	protected bool        m_SFPDisorientZoneActive;
	protected EffectSound m_SFPDisorientSound;          // звук "мозгового удара"
	protected int         m_SFPDisorientDamageCounter;  // счётчик тиков для урона

	// играет ли сейчас звук ауры "в ушах" у игрока
	protected bool        m_SFPDisorientSoundPlaying;

	// --- ПОЛЯ ДЛЯ ЭФФЕКТОВ ЭКРАНА ---

	// Размытие (ванильный "fever" requester)
	protected PPERequester_FeverEffects m_SFPFeverRequester;
	// Синеватый оттенок (ванильный "spooky area" requester)
	protected PPERequesterBase          m_SFPSpookyRequester;
	// Флаг, что PP-эффекты от этой ауры сейчас включены
	protected bool                      m_SFPPostProcessActive;
	protected bool                      m_SFPFeverStarted;

	// --- ЖИЗНЕННЫЙ ЦИКЛ ЗОМБИ ---

	void SFP_MozgoTras_Base()
	{
		// При создании зомби планируем первую "зону"
		SFP_ScheduleNextDisorientZone();
	}

	void ~SFP_MozgoTras_Base()
	{
		SFP_CancelDisorientZone();
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		SFP_CancelDisorientZone();
	}

	// --- СЛУЖЕБНЫЕ ФУНКЦИИ ---

	// Ленивая инициализация ванильных PPE-requester'ов
	protected void SFP_InitPostProcess()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		if (!m_SFPFeverRequester)
		{
			Class.CastTo(m_SFPFeverRequester, PPERequesterBank.GetRequester(PPERequester_FeverEffects));
		}

		if (!m_SFPSpookyRequester)
		{
			// Ванильный "жуткий"/синеватый фильтр (см. PPERSpooky.c)
			Class.CastTo(m_SFPSpookyRequester, PPERequesterBank.GetRequester(PPERequester_SpookyAreaTint));
		}
	}

	// Полное отключение размытия/оттенка, используемых этой аурой
	protected void SFP_ResetPostProcess()
	{
		if (!GetGame())
			return;

		// Сбрасываем размытие (fever blur)
		if (m_SFPFeverRequester)
		{
			// убираем интенсивность
			m_SFPFeverRequester.SetFeverIntensity(0.0);

			// и именно останавливаем requester
			if (m_SFPFeverStarted)
			{
				m_SFPFeverRequester.Stop();
				m_SFPFeverStarted = false;
			}
		}

		// Выключаем "жуткий" фильтр с плавным фейдом
		if (m_SFPSpookyRequester)
		{
			m_SFPSpookyRequester.Stop(new Param1<bool>(true)); // true = fade out
		}

		m_SFPPostProcessActive = false;
	}

	// Отключаем все таймеры и эффекты, когда зомби умирает/удаляется
	protected void SFP_CancelDisorientZone()
	{
		if (!GetGame())
			return;

		m_SFPDisorientZoneActive    = false;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false;

		// Снимаем все возможные CallLater'ы этой механики
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_StartDisorientZone);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_StopDisorientZone);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_DisorientTick);

		// Останавливаем звук, если играет
		StopSoundSet(m_SFPDisorientSound);

		// Убираем размытие/оттенок
		SFP_ResetPostProcess();
	}

	// Планируем следующую зону через случайный интервал 6–11 секунд
	protected void SFP_ScheduleNextDisorientZone()
	{
		if (!GetGame())
			return;

		int delayMs = Math.RandomIntInclusive(SFP_DISORIENT_MIN_INTERVAL_MS, SFP_DISORIENT_MAX_INTERVAL_MS);

		// Планируем включение зоны
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_StartDisorientZone, delayMs, false);
	}

	// Включаем зону (ауру)
	protected void SFP_StartDisorientZone()
	{
		if (!GetGame())
			return;

		// Если зомби уже уничтожен — ничего не делаем
		if (IsDamageDestroyed())
			return;

		m_SFPDisorientZoneActive    = true;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false; // звук включим на игроке, когда он войдёт в радиус

		// Раньше звук ауры крутился на мутанте:
		// PlaySoundSetLoop(m_SFPDisorientSound, "SFP_MozgoTras_PsiAura_Soundset", 0, 0);
		// Теперь его запускаем на локальном игроке в SFP_DisorientTick().

		// Начинаем периодически накатывать эффект на игрока
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_DisorientTick, SFP_DISORIENT_TICK_MS, true);

		// Случайная длительность зоны 6–10 сек
		int durationMs = Math.RandomIntInclusive(SFP_DISORIENT_MIN_DURATION_MS, SFP_DISORIENT_MAX_DURATION_MS);

		// Планируем выключение зоны
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_StopDisorientZone, durationMs, false);
	}

	// Выключаем зону и планируем следующую
	protected void SFP_StopDisorientZone()
	{
		if (!GetGame())
			return;

		m_SFPDisorientZoneActive    = false;
		m_SFPDisorientDamageCounter = 0;
		m_SFPDisorientSoundPlaying  = false;

		// Останавливаем периодическое дёргание камеры/урон
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.SFP_DisorientTick);

		// Глушим звук
		StopSoundSet(m_SFPDisorientSound);

		// Убираем размытие и фильтр
		SFP_ResetPostProcess();

		// Если зомби ещё жив — планируем следующую "волну"
		if (!IsDamageDestroyed())
		{
			SFP_ScheduleNextDisorientZone();
		}
	}

	// Тик эффекта: проверяем дистанцию от игрока до мутанта, шатаем камеру,
	// накладываем PP-эффекты, наносим урон и с шансом 50% выбиваем предмет из рук
	protected void SFP_DisorientTick()
	{
		if (!m_SFPDisorientZoneActive)
			return;

		if (!GetGame())
			return;

		// Локальный игрок на этом клиенте
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.IsAlive())
			return;

		// Проверяем, в радиусе ли он
		vector playerPos = player.GetPosition();
		vector zombiePos = GetPosition();
		float  distSq    = vector.DistanceSq(playerPos, zombiePos);
		float  radiusSq  = SFP_DISORIENT_RADIUS * SFP_DISORIENT_RADIUS;

		if (distSq > radiusSq)
		{
			// Вышли из зоны — убираем эффекты, но зону не выключаем (она может "крутиться" вхолостую)
			if (m_SFPPostProcessActive)
			{
				SFP_ResetPostProcess();
			}

			// И глушим "ушной" звук, если он крутился
			if (m_SFPDisorientSoundPlaying)
			{
				StopSoundSet(m_SFPDisorientSound);
				m_SFPDisorientSoundPlaying = false;
			}

			return;
		}

		// Находимся ВНУТРИ радиуса — инициализируем ванильные requester'ы
		SFP_InitPostProcess();

		// --- ЗВУК "В УШАХ" ---
		// Стартуем луп на самом игроке, чтобы звук не шёл от мутанта в мире,
		// а сразу прилетал в наушники (одинаково, пока игрок в зоне).
		if (!m_SFPDisorientSoundPlaying)
		{
			player.PlaySoundSetLoop(m_SFPDisorientSound, "SFP_MozgoTras_PsiAura_Soundset", 0.0, 0.0);
			m_SFPDisorientSoundPlaying = true;
		}

		// --- ТРЯСКА КАМЕРЫ ---
		float strength = Math.RandomFloatInclusive(1.0, 2.8);
		player.GetCurrentCamera().SpawnCameraShake(strength);

		// --- РАЗМЫТИЕ ЭКРАНА (Fever blur) ---

		if (m_SFPFeverRequester)
		{
			// Первый раз в зоне — запускаем requester
			if (!m_SFPFeverStarted)
			{
				m_SFPFeverRequester.Start();
				m_SFPFeverStarted = true;
			}

			// Чем ближе к мутанту — тем сильнее размытие/засвет
			float dist   = Math.Sqrt(distSq);
			float factor = 1.0 - (dist / SFP_DISORIENT_RADIUS); // 0..1
			if (factor < 0.0)
				factor = 0.0;
			if (factor > 1.0)
				factor = 1.0;

			// Лёгкий "флеш": не белый в ноль, а умеренный пересвет
			float blurStrength = Math.Lerp(0.35, 0.6, factor);

			m_SFPFeverRequester.SetFeverIntensity(blurStrength);
		}

		// --- СИНЕВАТЫЙ ОТТЕНОК (SpookyAreaTint) ---

		if (m_SFPSpookyRequester && !m_SFPPostProcessActive)
		{
			m_SFPSpookyRequester.Start(); // fade-in внутри самого requester'а
		}

		m_SFPPostProcessActive = true;

		// --- УРОН ПО ЗДОРОВЬЮ + ШАНС ВЫРОНИТЬ ПРЕДМЕТ ---

		// При SFP_DISORIENT_TICK_MS = 100 мс 10 тиков ≈ 1 секунда.
		m_SFPDisorientDamageCounter++;

		int ticksPerSecond = 1000 / SFP_DISORIENT_TICK_MS; // здесь будет 10 при 100 мс
		if (m_SFPDisorientDamageCounter >= ticksPerSecond)
		{
			// Минус 2 HP раз в секунду
			player.AddHealth("", "", -2);
			m_SFPDisorientDamageCounter = 0;

			// 50/50 шанс выбить из рук оружие/предмет
			int dropRoll = Math.RandomIntInclusive(0, 2); // 0 или 1
			if (dropRoll == 0) // ~50%
			{
				ItemBase itemInHands = ItemBase.Cast(player.GetItemInHands());
				if (itemInHands)
				{
					// Используем ванильный PlayerBase.DropItem, как в оригинальных скриптах
					player.DropItem(itemInHands);
				}
			}
		}
	}

	#endif // SERVER
}


