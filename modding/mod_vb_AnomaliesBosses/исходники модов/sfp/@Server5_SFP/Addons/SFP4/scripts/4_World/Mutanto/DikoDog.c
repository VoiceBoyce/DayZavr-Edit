class SFP_DikoDog extends AnimalBase
{
	// ====== ТЕЛЕПОРТ ЛОГИКА (СЕРВЕР) ======
	protected float m_TeleportNearTimer;
	protected float m_TeleportBehindTimer;
	
	// новые рандомные интервалы
	protected float m_NextTeleportNear;     // телепорт "от игрока" (вокруг него)
	protected float m_NextTeleportBehind;   // телепорт за спину
	// конфиг (сервер): интервалы телепорта (сек)
	protected float m_SFP_TeleportNearIntervalMin;
	protected float m_SFP_TeleportNearIntervalMax;
	protected float m_SFP_TeleportBehindIntervalMin;
	protected float m_SFP_TeleportBehindIntervalMax;

	
	protected const float TELEPORT_NEAR_INTERVAL_MIN    = 4.0;   // от 4
	protected const float TELEPORT_NEAR_INTERVAL_MAX    = 10.0;  // до 10 секунд
	
	protected const float TELEPORT_BEHIND_INTERVAL_MIN  = 2.0;   // от 2
	protected const float TELEPORT_BEHIND_INTERVAL_MAX  = 6.0;   // до 6 секунд
	
	// радиус, в котором считаем, что мутант агрессивен относительно игрока
	protected const float TELEPORT_MAX_TARGET_DISTANCE = 60.0;
	
	// дистанции телепорта вокруг игрока (НЕ за спину, а просто “где-то рядом в пределах 50м”)
	protected const float TELEPORT_AROUND_MIN_DIST   = 2.0;   // чтобы не в ноги
	protected const float TELEPORT_AROUND_MAX_DIST   = 50.0;  // максимум 50 метров от игрока
	
	// дистанции для телепорта за спину
	protected const float TELEPORT_BEHIND_MIN_DIST = 2.0;
	protected const float TELEPORT_BEHIND_MAX_DIST = 5.0;
	
	// ====== КЛИЕНТСКАЯ ДЕТЕКЦИЯ ТЕЛЕПОРТА (VFX/SFX) ======
	protected vector m_LastPos;
	protected bool   m_HasLastPos;
	
	protected const float TELEPORT_DETECTION_MIN_MOVE = 1.5; // метры
	
	protected float m_TeleportVisualCooldown;
	protected const float TELEPORT_VISUAL_COOLDOWN = 0.25; // секунды
	
	// звук телепорта
	protected EffectSound m_TeleportSound;
	
	// радиус лёгкой “полу-флешки” при телепорте
	protected const float TELEPORT_BLIND_RADIUS = 12.0; // метры
	
	// радиус сильной флешки при смерти
	protected const float DEATH_FLASH_RADIUS = 20.0; // метры
	
	void SFP_DikoDog()
	{
		int mask = GetEventMask();
		mask |= EntityEvent.FRAME;
		SetEventMask(mask);
		
		m_TeleportNearTimer    = 0.0;
		m_TeleportBehindTimer  = 0.0;
		m_HasLastPos           = false;
		m_TeleportVisualCooldown = 0.0;
		
		// дефолтные интервалы (до загрузки json)
		m_SFP_TeleportNearIntervalMin   = TELEPORT_NEAR_INTERVAL_MIN;
		m_SFP_TeleportNearIntervalMax   = TELEPORT_NEAR_INTERVAL_MAX;
		m_SFP_TeleportBehindIntervalMin = TELEPORT_BEHIND_INTERVAL_MIN;
		m_SFP_TeleportBehindIntervalMax = TELEPORT_BEHIND_INTERVAL_MAX;

		// сразу задаём первые случайные интервалы
		m_NextTeleportNear   = Math.RandomFloatInclusive(m_SFP_TeleportNearIntervalMin,   m_SFP_TeleportNearIntervalMax);
		m_NextTeleportBehind = Math.RandomFloatInclusive(m_SFP_TeleportBehindIntervalMin, m_SFP_TeleportBehindIntervalMax);
}
	

	override void EEInit()
	{
		super.EEInit();

		if (!GetGame() || !GetGame().IsServer())
			return;

		ref SFP_TeleDogConfig cfg = SFP_TeleDogConfigManager.Get();
		if (!cfg)
			return;

		// интервалы телепорта (сек)
		m_SFP_TeleportNearIntervalMin   = Math.Max(0.1, cfg.TeleportNearIntervalMin);
		m_SFP_TeleportNearIntervalMax   = Math.Max(m_SFP_TeleportNearIntervalMin, cfg.TeleportNearIntervalMax);
		m_SFP_TeleportBehindIntervalMin = Math.Max(0.1, cfg.TeleportBehindIntervalMin);
		m_SFP_TeleportBehindIntervalMax = Math.Max(m_SFP_TeleportBehindIntervalMin, cfg.TeleportBehindIntervalMax);

		// сброс таймеров + первые интервалы
		m_TeleportNearTimer   = 0.0;
		m_TeleportBehindTimer = 0.0;
		m_NextTeleportNear   = Math.RandomFloatInclusive(m_SFP_TeleportNearIntervalMin,   m_SFP_TeleportNearIntervalMax);
		m_NextTeleportBehind = Math.RandomFloatInclusive(m_SFP_TeleportBehindIntervalMin, m_SFP_TeleportBehindIntervalMax);

		// здоровье (стартовое)
		if (cfg.StartHealth >= 0.0)
		{
			SetHealth("", "Health", cfg.StartHealth);
		}
		else
		{
			float coef = Math.Clamp(cfg.StartHealth01, 0.0, 1.0);
			SetHealth01("", "Health", coef);
		}
	}

	override bool CanBeSkinned()
	{
		return true;
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);
		
		if (!IsAlive())
			return;
		
		// ----- КЛИЕНТ: детекция телепорта, партикл, звук, лёгкая слепота -----
		if (GetGame() && !GetGame().IsDedicatedServer())
		{
			UpdateTeleportVisuals(timeSlice);
		}
		
		// ----- СЕРВЕР: поиск цели и телепорты -----
		if (GetGame() && GetGame().IsServer())
		{
			UpdateTeleportLogicServer(timeSlice);
		}
	}
	
	// ---------- СЕРВЕР: логика с РАНДОМНЫМИ интервалами ----------
	protected void UpdateTeleportLogicServer(float timeSlice)
	{
		PlayerBase target = GetClosestTargetPlayer(TELEPORT_MAX_TARGET_DISTANCE);
		if (!target)
		{
			// никто рядом — сбрасываем таймеры и заново рандомим интервалы
			m_TeleportNearTimer    = 0.0;
			m_TeleportBehindTimer  = 0.0;
			m_NextTeleportNear   = Math.RandomFloatInclusive(m_SFP_TeleportNearIntervalMin,   m_SFP_TeleportNearIntervalMax);
			m_NextTeleportBehind = Math.RandomFloatInclusive(m_SFP_TeleportBehindIntervalMin, m_SFP_TeleportBehindIntervalMax);
			return;
		}
		
		m_TeleportNearTimer   += timeSlice;
		m_TeleportBehindTimer += timeSlice;
		
		// телепорт за спину с интервалом 2–6 сек
		if (m_TeleportBehindTimer >= m_NextTeleportBehind)
		{
			m_TeleportBehindTimer = 0.0;
			m_NextTeleportBehind = Math.RandomFloatInclusive(m_SFP_TeleportBehindIntervalMin, m_SFP_TeleportBehindIntervalMax);
			TeleportBehindPlayer(target);
		}
		
		// телепорт "от игрока" (где-то в радиусе до 50м) с интервалом 4–10 сек
		if (m_TeleportNearTimer >= m_NextTeleportNear)
		{
			m_TeleportNearTimer = 0.0;
			m_NextTeleportNear = Math.RandomFloatInclusive(m_SFP_TeleportNearIntervalMin, m_SFP_TeleportNearIntervalMax);
			TeleportAroundPlayer(target); // обновлённая логика бывшего TeleportNearPlayer
		}
	}
	
	protected PlayerBase GetClosestTargetPlayer(float maxDistance)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		PlayerBase best;
		float maxDistSq = maxDistance * maxDistance;
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player)
				continue;
			
			if (!player.IsAlive() || player.IsUnconscious())
				continue;
			
			float distSq = vector.DistanceSq(player.GetPosition(), GetPosition());
			if (distSq <= maxDistSq)
			{
				maxDistSq = distSq;
				best      = player;
			}
		}
		
		return best;
	}
	
	// НОВАЯ логика "телепорта от игрока": случайная точка в радиусе до 50 м от игрока
	protected void TeleportAroundPlayer(PlayerBase target)
	{
		if (!target)
			return;
		
		vector targetPos = target.GetPosition();
		vector newPos;
		bool found = false;

		// 1.30 port: never teleport the AI into water or into geometry (several tries, otherwise skip)
		for (int attempt = 0; attempt < 6 && !found; attempt++)
		{
			float radius = Math.RandomFloatInclusive(TELEPORT_AROUND_MIN_DIST, TELEPORT_AROUND_MAX_DIST);
			float angle  = Math.RandomFloatInclusive(0.0, Math.PI2);

			vector offset;
			offset[0] = radius * Math.Cos(angle);
			offset[1] = 0.0;
			offset[2] = radius * Math.Sin(angle);

			newPos = targetPos + offset;
			newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);
			found = SFP_IsTeleportPosSafe(newPos, target);
		}

		if (!found)
			return;

		SetPosition(newPos);
		
		// можно смотреть в сторону игрока, чтобы он выглядел "осознанным"
		vector dirToPlayer = targetPos - newPos;
		dirToPlayer.Normalize();
		SetDirection(dirToPlayer);
	}
	
	// телепорт строго за спину игрока на небольшое расстояние
	protected void TeleportBehindPlayer(PlayerBase target)
	{
		if (!target)
			return;
		
		vector targetPos = target.GetPosition();
		vector targetDir = target.GetDirection();
		
		targetDir[1] = 0;
		targetDir.Normalize();

		vector newPos;
		bool found = false;

		// 1.30 port: never teleport the AI into water or into geometry (several tries, otherwise skip)
		for (int attempt = 0; attempt < 4 && !found; attempt++)
		{
			float dist = Math.RandomFloatInclusive(TELEPORT_BEHIND_MIN_DIST, TELEPORT_BEHIND_MAX_DIST);

			vector offset = -targetDir * dist;
			newPos = targetPos + offset;
			newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);
			found = SFP_IsTeleportPosSafe(newPos, target);
		}

		if (!found)
			return;

		SetPosition(newPos);

		vector dirToPlayer = targetPos - newPos;
		dirToPlayer.Normalize();
		SetDirection(dirToPlayer);
	}

	protected bool SFP_IsTeleportPosSafe(vector pos, PlayerBase target)
	{
		array<Object> exclude = new array<Object>;
		exclude.Insert(this);
		if (target)
			exclude.Insert(target);
		return SFP_Safety.IsFreeGroundPos(pos, Vector(0.8, 0.9, 1.4), exclude);
	}

	// ---------- КЛИЕНТ: детекция телепорта, партикл, звук, лёгкая слепота ----------
	protected void UpdateTeleportVisuals(float timeSlice)
	{
		m_TeleportVisualCooldown -= timeSlice;
		if (m_TeleportVisualCooldown < 0.0)
			m_TeleportVisualCooldown = 0.0;
		
		vector currentPos = GetPosition();
		
		if (m_HasLastPos)
		{
			float moveSq = vector.DistanceSq(m_LastPos, currentPos);
			float thresholdSq = TELEPORT_DETECTION_MIN_MOVE * TELEPORT_DETECTION_MIN_MOVE;
			
			if (moveSq >= thresholdSq && m_TeleportVisualCooldown <= 0.0)
			{
				OnTeleportVisualEvent();
				m_TeleportVisualCooldown = TELEPORT_VISUAL_COOLDOWN;
			}
		}
		
		m_LastPos   = currentPos;
		m_HasLastPos = true;
	}
	
	protected void OnTeleportVisualEvent()
	{
		SpawnTeleportParticle();
		PlayTeleportSound();
		TryApplySmallBlindToLocalPlayer();
	}
	
	protected void SpawnTeleportParticle()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		if (ParticleList.IsValidId(ParticleList.SFP_PSEDOG_CLONED))
		{
			ParticleManager.GetInstance().PlayInWorld(ParticleList.SFP_PSEDOG_CLONED, GetPosition());
		}
	}
	
	protected void PlayTeleportSound()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		PlaySoundSet(m_TeleportSound, "SFP_PseDog_DeadClone_Soundset", 0, 0);
	}
	
	protected void TryApplySmallBlindToLocalPlayer()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		if (Math.RandomInt(0, 2) != 0) // 50/50
			return;
		
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		
		float distSq = vector.DistanceSq(localPlayer.GetPosition(), GetPosition());
		if (distSq > TELEPORT_BLIND_RADIUS * TELEPORT_BLIND_RADIUS)
			return;
		
		localPlayer.SpawnFlashbangEffect(localPlayer, true);
		
		FlashbangEffect fb = localPlayer.GetFlashbangEffect();
		if (fb)
		{
			fb.SetupFlashbangValues(0.6, 0.5, 0.4); // мягкая флешка
		}
	}
	
	// ---------- Смерть мутанта: сильная флешка в радиусе 20м ----------
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (GetGame() && !GetGame().IsDedicatedServer())
		{
			SpawnDeathFlashbangEffectForLocalPlayer();
		}
	}
	
	protected void SpawnDeathFlashbangEffectForLocalPlayer()
	{
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		
		float distSq = vector.DistanceSq(localPlayer.GetPosition(), GetPosition());
		if (distSq > DEATH_FLASH_RADIUS * DEATH_FLASH_RADIUS)
			return;
		
		localPlayer.SpawnFlashbangEffect(localPlayer, true);
	}
}

/*class SFP_DikoDog extends AnimalBase
{
	// ====== ТЕЛЕПОРТ ЛОГИКА (СЕРВЕР) ======
	protected float m_TeleportNearTimer;
	protected float m_TeleportBehindTimer;
	
	protected const float TELEPORT_NEAR_INTERVAL    = 10.0;   // раз в 10 сек рядом
	protected const float TELEPORT_BEHIND_INTERVAL  = 5.0;    // раз в 5 сек за спиной
	protected const float TELEPORT_MAX_TARGET_DISTANCE = 60.0;
	
	protected const float TELEPORT_NEAR_MIN_DIST   = 2.0;
	protected const float TELEPORT_NEAR_MAX_DIST   = 6.0;
	protected const float TELEPORT_BEHIND_MIN_DIST = 2.0;
	protected const float TELEPORT_BEHIND_MAX_DIST = 5.0;
	
	// ====== КЛИЕНТСКАЯ ДЕТЕКЦИЯ ТЕЛЕПОРТА (VFX/SFX) ======
	protected vector m_LastPos;
	protected bool   m_HasLastPos;
	
	// если за один кадр мутант переместился дальше этого порога - считаем, что был телепорт
	protected const float TELEPORT_DETECTION_MIN_MOVE = 1.5; // метры
	
	// чтоб не спамило несколько раз подряд из-за сетевого апдейта
	protected float m_TeleportVisualCooldown;
	protected const float TELEPORT_VISUAL_COOLDOWN = 0.25; // секунды
	
	// звук телепорта
	protected EffectSound m_TeleportSound;
	
	// радиус для лёгкой “полу-флешки” при телепорте
	protected const float TELEPORT_BLIND_RADIUS = 12.0; // метры
	
	// радиус для сильной флешки при смерти
	protected const float DEATH_FLASH_RADIUS = 20.0; // метры
	
	void SFP_DikoDog()
	{
		int mask = GetEventMask();
		mask |= EntityEvent.FRAME;
		SetEventMask(mask);
		
		m_TeleportNearTimer    = 0.0;
		m_TeleportBehindTimer  = 0.0;
		m_HasLastPos           = false;
		m_TeleportVisualCooldown = 0.0;
	}
	
	override bool CanBeSkinned()
	{
		return true;
	}
	
	// каждый кадр
	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);
		
		if (!IsAlive())
			return;
		
		// ----- КЛИЕНТСКАЯ ЧАСТЬ: детекция телепорта, партикл, звук, лёгкая слепота -----
		if (GetGame() && !GetGame().IsDedicatedServer())
		{
			UpdateTeleportVisuals(timeSlice);
		}
		
		// ----- СЕРВЕРНАЯ ЧАСТЬ: собственно телепорт ИИ -----
		if (GetGame() && GetGame().IsServer())
		{
			UpdateTeleportLogicServer(timeSlice);
		}
	}
	
	// ---------- СЕРВЕР: логика поиска цели и телепортов ----------
	protected void UpdateTeleportLogicServer(float timeSlice)
	{
		PlayerBase target = GetClosestTargetPlayer(TELEPORT_MAX_TARGET_DISTANCE);
		if (!target)
		{
			m_TeleportNearTimer   = 0.0;
			m_TeleportBehindTimer = 0.0;
			return;
		}
		
		m_TeleportNearTimer   += timeSlice;
		m_TeleportBehindTimer += timeSlice;
		
		if (m_TeleportBehindTimer >= TELEPORT_BEHIND_INTERVAL)
		{
			m_TeleportBehindTimer = 0.0;
			TeleportBehindPlayer(target);
		}
		
		if (m_TeleportNearTimer >= TELEPORT_NEAR_INTERVAL)
		{
			m_TeleportNearTimer = 0.0;
			TeleportNearPlayer(target);
		}
	}
	
	protected PlayerBase GetClosestTargetPlayer(float maxDistance)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		PlayerBase best;
		float maxDistSq = maxDistance * maxDistance;
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player)
				continue;
			
			if (!player.IsAlive() || player.IsUnconscious())
				continue;
			
			float distSq = vector.DistanceSq(player.GetPosition(), GetPosition());
			if (distSq <= maxDistSq)
			{
				maxDistSq = distSq;
				best      = player;
			}
		}
		
		return best;
	}
	
	protected void TeleportNearPlayer(PlayerBase target)
	{
		if (!target)
			return;
		
		vector targetPos = target.GetPosition();
		
		float radius = Math.RandomFloatInclusive(TELEPORT_NEAR_MIN_DIST, TELEPORT_NEAR_MAX_DIST);
		float angle  = Math.RandomFloatInclusive(0.0, Math.PI2);
		
		vector offset;
		offset[0] = radius * Math.Cos(angle);
		offset[1] = 0.0;
		offset[2] = radius * Math.Sin(angle);
		
		vector newPos = targetPos + offset;
		newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);
		
		SetPosition(newPos);
		
		vector dirToPlayer = targetPos - newPos;
		dirToPlayer.Normalize();
		SetDirection(dirToPlayer);
		
		// VFX/SFX не делаем тут, они считаются на клиентах в UpdateTeleportVisuals()
	}
	
	protected void TeleportBehindPlayer(PlayerBase target)
	{
		if (!target)
			return;
		
		vector targetPos = target.GetPosition();
		vector targetDir = target.GetDirection();
		
		targetDir[1] = 0;
		targetDir.Normalize();
		
		float dist = Math.RandomFloatInclusive(TELEPORT_BEHIND_MIN_DIST, TELEPORT_BEHIND_MAX_DIST);
		
		vector offset = -targetDir * dist;
		vector newPos = targetPos + offset;
		newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);
		
		SetPosition(newPos);
		
		vector dirToPlayer = targetPos - newPos;
		dirToPlayer.Normalize();
		SetDirection(dirToPlayer);
		
		// VFX/SFX не делаем тут, они считаются на клиентах в UpdateTeleportVisuals()
	}
	
	// ---------- КЛИЕНТ: отслеживаем резкий скачок позиции и считаем его телепортом ----------
	protected void UpdateTeleportVisuals(float timeSlice)
	{
		m_TeleportVisualCooldown -= timeSlice;
		if (m_TeleportVisualCooldown < 0.0)
			m_TeleportVisualCooldown = 0.0;
		
		vector currentPos = GetPosition();
		
		if (m_HasLastPos)
		{
			float moveSq = vector.DistanceSq(m_LastPos, currentPos);
			float thresholdSq = TELEPORT_DETECTION_MIN_MOVE * TELEPORT_DETECTION_MIN_MOVE;
			
			if (moveSq >= thresholdSq && m_TeleportVisualCooldown <= 0.0)
			{
				// считаем, что был телепорт
				OnTeleportVisualEvent();
				m_TeleportVisualCooldown = TELEPORT_VISUAL_COOLDOWN;
			}
		}
		
		m_LastPos   = currentPos;
		m_HasLastPos = true;
	}
	
	protected void OnTeleportVisualEvent()
	{
		SpawnTeleportParticle();
		PlayTeleportSound();
		TryApplySmallBlindToLocalPlayer();
	}
	
	// партикл при телепорте
	protected void SpawnTeleportParticle()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		// тут предполагается, что в ParticleList есть SFP_PSEDOG_CLONED
		if (ParticleList.IsValidId(ParticleList.SFP_PSEDOG_CLONED))
		{
			ParticleManager.GetInstance().PlayInWorld(ParticleList.SFP_PSEDOG_CLONED, GetPosition());
		}
	}
	
	// звук при телепорте
	protected void PlayTeleportSound()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		// одноразовый звук, auto-destroy
		PlaySoundSet(m_TeleportSound, "SFP_PseDog_DeadClone_Soundset", 0, 0);
	}
	
	// 50/50 шанс слегка ослепить локального игрока
	protected void TryApplySmallBlindToLocalPlayer()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		
		// 0 или 1; только при 0 срабатывает (50/50)
		if (Math.RandomInt(0, 2) != 0)
			return;
		
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		
		float distSq = vector.DistanceSq(localPlayer.GetPosition(), GetPosition());
		if (distSq > TELEPORT_BLIND_RADIUS * TELEPORT_BLIND_RADIUS)
			return;
		
		// создаём эффект флешки, но потом режем его интенсивность
		localPlayer.SpawnFlashbangEffect(localPlayer, true);
		
		FlashbangEffect fb = localPlayer.GetFlashbangEffect();
		if (fb)
		{
			// progress_mult, visual_max, sound_max
			// значения меньше 1.0 делают эффект короче/слабее
			fb.SetupFlashbangValues(0.6, 0.5, 0.4); // мягкая полу-флешка
		}
	}
	
	// ---------- Смерть мутанта: флешка, как от гранаты, в радиусе 20м ----------
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		// VFX/SFX только на клиентах (в т.ч. SP)
		if (GetGame() && !GetGame().IsDedicatedServer())
		{
			SpawnDeathFlashbangEffectForLocalPlayer();
		}
	}
	
	protected void SpawnDeathFlashbangEffectForLocalPlayer()
	{
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		
		float distSq = vector.DistanceSq(localPlayer.GetPosition(), GetPosition());
		if (distSq > DEATH_FLASH_RADIUS * DEATH_FLASH_RADIUS)
			return;
		
		// здесь полноценная флешка как от гранаты
		localPlayer.SpawnFlashbangEffect(localPlayer, true);
	}
}
