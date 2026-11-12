// ShumMutant - PlayerBase extensions
// - Receives RPCs for tinnitus (no blinding), death FX, and throw camera shake.
// - Server-side tracking to apply landing damage after being launched (fallback if vanilla fall damage doesn't trigger).
// Uses only vanilla DayZ APIs.

modded class PlayerBase
{
	// -----------------------------
	// Server-side throw tracking
	// -----------------------------
	protected bool  m_SM_ThrowActive;
	protected bool  m_SM_ThrowWasFalling;
	protected bool  m_SM_ThrowWasAirborne;
	protected float m_SM_ThrowMaxY;
	protected float m_SM_ThrowStartTimeS;
	protected float m_SM_ThrowIntensity;
	protected EntityAI m_SM_ThrowSource;

	// -----------------------------
	// Client-side continuous camera shake during throw
	// -----------------------------
	#ifndef SERVER
	protected ref Timer m_SM_ThrowShakeTimer;
	protected float m_SM_ThrowShakeStrength;
	protected ref Timer m_SM_TinnitusTimer;
	protected EffectSound m_SM_TinnitusSound;
	protected ref Timer m_SM_CamShakeTimer;
	protected ref ShumMutantCameraShake m_SM_CamShake;

	// Искажение воздуха на игроке при толчке (телекинез)
	protected ref Timer m_SM_PushHazeTimer;
	protected ScriptedLightBase m_SM_PushHazeLight;
	#endif

	void PlayerBase()
	{
		ShumMutantTuning.EnsureLoaded(); // загрузка настроек из $profile:\SFP\ShumMutantConfig.json

		m_SM_ThrowActive = false;
		m_SM_ThrowWasFalling = false;
		m_SM_ThrowWasAirborne = false;
		m_SM_ThrowMaxY = 0.0;
		m_SM_ThrowStartTimeS = 0.0;
		m_SM_ThrowIntensity = 0.0;
		m_SM_ThrowSource = null;

		#ifndef SERVER
		m_SM_ThrowShakeTimer = null;
		m_SM_ThrowShakeStrength = 0.0;
		m_SM_TinnitusTimer = null;
		m_SM_TinnitusSound = null;
		m_SM_CamShakeTimer = null;
		m_SM_CamShake = null;
		m_SM_PushHazeTimer = null;
		m_SM_PushHazeLight = null;
		#endif
	}

	// Called by the mutant on server whenever it throws this player.
	void ShumMutant_OnThrown(EntityAI source, float intensity)
	{
		if (!GetGame().IsServer())
			return;

		if (!IsAlive())
			return;

		if (GetCommand_Vehicle())
			return;

		// Apply immediate damage on each throw (player + a few inventory items)
		ShumMutant_ApplyThrowStartDamage(source, intensity);

		// Каждый бросок сбрасывает стамину в 0
		if (ShumMutantTuning.THROW_ZERO_STAMINA && GetStaminaHandler())
			GetStaminaHandler().SetStamina(0);

		// Start client shake immediately
		ShumMutant_SendThrowShakeRPC(true, intensity);

		// Arm landing damage fallback tracker
		vector pos = PhysicsGetPositionWS();
		m_SM_ThrowActive = true;
		m_SM_ThrowWasFalling = false;
		m_SM_ThrowWasAirborne = false;
		m_SM_ThrowMaxY = pos[1];
		m_SM_ThrowStartTimeS = GetGame().GetTime() * 0.001;
		m_SM_ThrowIntensity = Math.Clamp(intensity, 0.0, 1.0);
		m_SM_ThrowSource = source;
	}

	protected void ShumMutant_ApplyThrowStartDamage(EntityAI source, float intensity)
	{
		EntityAI src = source;
		if (!src)
			src = this;
		// Scale a bit with intensity
		float t = Math.Clamp(intensity, 0.0, 1.0);
		float healthCoef = ShumMutantTuning.THROW_START_HEALTH_COEF * Math.Lerp(0.65, 1.0, t);
		float shockCoef  = ShumMutantTuning.THROW_START_SHOCK_COEF  * Math.Lerp(0.65, 1.0, t);

		// Apply via vanilla fall damage ammo types (stable and present in vanilla configs)
		ProcessDirectDamage(DamageType.CUSTOM, src, "", DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_HEALTH, vector.Zero, Math.Clamp(healthCoef, 0.0, 1.0));
		ProcessDirectDamage(DamageType.CUSTOM, src, "", DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_SHOCK,  vector.Zero, Math.Clamp(shockCoef,  0.0, 1.0));

		// Damage a few random inventory items (incl. containers; their EEHitBy can cascade to cargo)
		ShumMutant_DamageRandomInventoryItems(src, ShumMutantTuning.THROW_ITEM_DAMAGE_COUNT,
			ShumMutantTuning.THROW_ITEM_DAMAGE_COEF_MIN * Math.Lerp(0.7, 1.0, t),
			ShumMutantTuning.THROW_ITEM_DAMAGE_COEF_MAX * Math.Lerp(0.7, 1.0, t));
	}

	protected void ShumMutant_DamageRandomInventoryItems(EntityAI source, int count, float coefMin, float coefMax)
	{
		// Сильная порча инвентаря. Работает напрямую по "Health" предметов, чтобы эффект был заметен.
		if (count <= 0)
			return;

		coefMin = Math.Clamp(coefMin, 0.0, 1.0);
		coefMax = Math.Clamp(coefMax, 0.0, 1.0);
		if (coefMax <= 0)
			return;
		if (coefMin > coefMax)
		{
			float tmp = coefMin;
			coefMin = coefMax;
			coefMax = tmp;
		}

		// Собираем ТОЛЬКО ItemBase из всего инвентаря (включая контейнеры и карго)
		array<EntityAI> entities = new array<EntityAI>();
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, entities);

		array<ItemBase> items = new array<ItemBase>();
		for (int k = 0; k < entities.Count(); k++)
		{
			ItemBase it = ItemBase.Cast(entities.Get(k));
			if (!it)
				continue;
			if (it.IsRuined() || it.IsExplosive())
				continue;
			items.Insert(it);
		}

		int n = items.Count();
		if (n == 0)
			return;

		count = Math.Min(count, n);

		// Перемешиваем индексы, чтобы выбрать без повторов
		array<int> idxs = new array<int>();
		idxs.Reserve(n);
		for (int i = 0; i < n; i++)
			idxs.Insert(i);

		for (int j = 0; j < n; j++)
		{
			int a = Math.RandomInt(0, n);
			int b = Math.RandomInt(0, n);
			int t = idxs[a];
			idxs[a] = idxs[b];
			idxs[b] = t;
		}

		for (int m = 0; m < count; m++)
		{
			ItemBase item = items.Get(idxs[m]);
			if (!item)
				continue;

			float maxH = item.GetMaxHealth("", "Health");
			float curH = item.GetHealth("", "Health");

			// Некоторые предметы используют другие зоны, пробуем глобальные
			if (maxH <= 0)
				maxH = item.GetMaxHealth("", "");
			if (curH < 0)
				curH = item.GetHealth("", "");

			if (maxH <= 0 || curH < 0)
			{
				// Абсолютный фолбек: ванильный direct damage
				vector posMS = item.WorldToModel(item.GetOrigin());
				item.ProcessDirectDamage(DamageType.CUSTOM, source, "", DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS, posMS, 1.0);
				continue;
			}

			float coef = Math.RandomFloatInclusive(coefMin, coefMax);
			float chunk = maxH * coef;

			// AddHealth работает в ваниле на большинстве предметов (одежда/оружие/контейнеры)
			item.AddHealth("", "Health", -chunk);
		}
	}

	protected void ShumMutant_SendThrowShakeRPC(bool start, float intensity)
	{
		PlayerIdentity id = GetIdentity();
		if (!id)
			return;

		ref Param2<bool, float> p = new Param2<bool, float>(start, Math.Clamp(intensity, 0.0, 1.0));
		GetGame().RPCSingleParam(this, ShumMutantRPC.RPC_THROW_SHAKE, p, true, id);
	}

	protected void ShumMutant_ApplyLandingDamageFallback(float heightM, EntityAI source, float intensity)
	{
		if (heightM < ShumMutantTuning.THROW_LAND_MIN_HEIGHT_M)
			return;

		EntityAI src = source;
		if (!src)
			src = this;

		float t = Math.Clamp(intensity, 0.0, 1.0);
		// Mimic vanilla fall damage curves
		float healthCoef = Math.InverseLerp(ShumMutantTuning.THROW_LAND_HEALTH_HEIGHT_LOW, ShumMutantTuning.THROW_LAND_HEALTH_HEIGHT_HIGH, heightM);
		float shockCoef  = Math.InverseLerp(ShumMutantTuning.THROW_LAND_SHOCK_HEIGHT_LOW,  ShumMutantTuning.THROW_LAND_SHOCK_HEIGHT_HIGH,  heightM);
		healthCoef = Math.Clamp(healthCoef, 0.0, 1.0) * Math.Lerp(0.75, 1.0, t);
		shockCoef  = Math.Clamp(shockCoef,  0.0, 1.0) * Math.Lerp(0.75, 1.0, t);

		ProcessDirectDamage(DamageType.CUSTOM, src, "", DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_HEALTH, vector.Zero, healthCoef);
		ProcessDirectDamage(DamageType.CUSTOM, src, "", DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_SHOCK,  vector.Zero, shockCoef);

			// Extra gear damage on landing (заметно портит инвентарь)
			ShumMutant_DamageRandomInventoryItems(src, 6, Math.Clamp(0.25 + 0.35 * healthCoef, 0.0, 1.0), Math.Clamp(0.25 + 0.35 * healthCoef + 0.25, 0.0, 1.0));
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);

		if (!GetGame().IsServer())
			return;

		if (!m_SM_ThrowActive)
			return;

		if (!IsAlive() || GetCommand_Vehicle())
		{
			m_SM_ThrowActive = false;
			m_SM_ThrowWasFalling = false;
			return;
		}

		vector pos = PhysicsGetPositionWS();
		if (pos[1] > m_SM_ThrowMaxY)
			m_SM_ThrowMaxY = pos[1];

		float nowS = GetGame().GetTime() * 0.001;
		if ((nowS - m_SM_ThrowStartTimeS) > ShumMutantTuning.THROW_TRACK_TIMEOUT_S)
		{
			// Safety timeout
			m_SM_ThrowActive = false;
			m_SM_ThrowWasFalling = false;
			ShumMutant_SendThrowShakeRPC(false, 0.0);
			return;
		}

		// Detect airborne/landing (physics + downward ray)
		float distToSurface;
		bool airborneByRay = ShumMutant_IsAirborneByRay(pos, distToSurface);
		bool fallingNow = PhysicsIsFalling(false) || IsFalling() || airborneByRay;
		if (fallingNow)
		{
			m_SM_ThrowWasFalling = true;
			m_SM_ThrowWasAirborne = true;
			return;
		}

		// We were airborne and now we are not: treat as landing
		if (m_SM_ThrowWasAirborne)
		{
			float height = m_SM_ThrowMaxY - pos[1];
			ShumMutant_ApplyLandingDamageFallback(height, m_SM_ThrowSource, m_SM_ThrowIntensity);
			ShumMutant_SendThrowShakeRPC(false, 0.0);
			m_SM_ThrowActive = false;
			m_SM_ThrowWasFalling = false;
			m_SM_ThrowWasAirborne = false;
			m_SM_ThrowSource = null;
		}
	}

	protected bool ShumMutant_IsAirborneByRay(vector pos, out float distToSurface)
	{
		distToSurface = 0.0;
		vector from = pos;
		from[1] = from[1] + 0.15;
		vector to = from;
		to[1] = to[1] - 2.2;

		RaycastRVParams params = new RaycastRVParams(from, to, this, 0.15);
		params.type = ObjIntersectGeom;
		params.flags = CollisionFlags.NEARESTCONTACT;
		params.sorted = false;
		params.groundOnly = false;

		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>();
		bool hit = DayZPhysics.RaycastRVProxy(params, results);
		if (hit && results && results.Count() > 0)
		{
			distToSurface = vector.Distance(from, results.Get(0).pos);
			// Consider "in air" when clearance below the feet is noticeable
			return distToSurface > 0.65;
		}

		// No hit below us -> definitely airborne
		distToSurface = 999.0;
		return true;
	}

	// ----------------------------------------------------
	// RPC Receiver (client-side effects)
	// ----------------------------------------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		#ifndef SERVER
		// (2) Tinnitus-only hit (no blinding)
		if (rpc_type == ShumMutantRPC.RPC_TINNITUS_ONLY)
		{
			ref Param2<float, float> p = new Param2<float, float>(0.0, 0.0);
			if (!ctx.Read(p))
				return;

			ShumMutant_StartTinnitus(p.param1, p.param2);
			return;
		}

		// (4) Death burst feedback: sound + strong camera shake
		if (rpc_type == ShumMutantRPC.RPC_DEATH_FX)
		{
			ref Param2<float, float> p2 = new Param2<float, float>(0.0, 0.0);
			if (!ctx.Read(p2))
				return;

			vector pos = GetPosition();
			float cam_shake = p2.param1;
			float volume = p2.param2;

			if (ShumMutantTuning.DEATH_SOUNDSET != string.Empty)
			{
				EffectSound snd = SEffectManager.PlaySound(ShumMutantTuning.DEATH_SOUNDSET, pos);
				if (snd)
				{
					snd.SetSoundMaxVolume(Math.Clamp(volume, 0.1, 1.0));
					snd.SetAutodestroy(true);
				}
			}


			ShumMutant_SpawnCameraShake(cam_shake, ShumMutantTuning.DEATH_CAMERA_SHAKE_RADIUS, ShumMutantTuning.DEATH_CAMERA_SHAKE_SMOOTH, ShumMutantTuning.DEATH_CAMERA_SHAKE_DECAY);

			return;
		}

		// Throw camera shake: start/stop strong "earthquake" while in the air
		if (rpc_type == ShumMutantRPC.RPC_THROW_SHAKE)
		{
			ref Param2<bool, float> p3 = new Param2<bool, float>(false, 0.0);
			if (!ctx.Read(p3))
				return;

			bool start = p3.param1;
			float intensity = Math.Clamp(p3.param2, 0.0, 1.0);

			if (start)
			{
				m_SM_ThrowShakeStrength = Math.Lerp(ShumMutantTuning.THROW_CAMERA_SHAKE_STRENGTH * 0.75, ShumMutantTuning.THROW_CAMERA_SHAKE_STRENGTH, intensity);
				if (!m_SM_ThrowShakeTimer)
					m_SM_ThrowShakeTimer = new Timer(CALL_CATEGORY_SYSTEM);
				m_SM_ThrowShakeTimer.Run(ShumMutantTuning.THROW_CAMERA_SHAKE_INTERVAL_S, this, "ShumMutant_ThrowShakeTick", null, true);
				ShumMutant_ThrowShakeTick();

				// Доп. эффект "телекинеза": большое искажение воздуха на игроке на пару секунд
				ShumMutant_StartPushHaze(intensity);

				// Звук телекинезного удара (любой бросок/толчок)
				ShumMutant_PlayTeleHitSound();
			}
			else
			{
				if (m_SM_ThrowShakeTimer && m_SM_ThrowShakeTimer.IsRunning())
					m_SM_ThrowShakeTimer.Stop();
				ShumMutant_StopCameraShake();
			}

			return;
		}
		#endif
	}
	#ifndef SERVER



	// ---------------------------------------------------------
	// Camera shake (без DayZPlayer::SpawnCameraShake)
	// ---------------------------------------------------------
	protected static const float SM_CAMSHAKE_TICK_S = 0.02;

	void ShumMutant_SpawnCameraShake(float strength_factor, float radius, float smoothness, float radius_decay_speed)
	{
		// В некоторых сборках/скрипт-инъекции DayZPlayer::SpawnCameraShake может быть недоступен для PlayerBase.
		// Поэтому используем ванильную математику из scripts/4_World/classes/CameraShake.c и выставляем значения через AimingModel.
		strength_factor = Math.Clamp(strength_factor, 0.0, 5.0);
		radius = Math.Max(radius, 0.0);
		smoothness = Math.Max(smoothness, 0.1);
		radius_decay_speed = Math.Max(radius_decay_speed, 0.01);

		if (strength_factor <= 0 || radius <= 0)
			return;

		m_SM_CamShake = new ShumMutantCameraShake(strength_factor, radius, smoothness, radius_decay_speed);

		if (!m_SM_CamShakeTimer)
			m_SM_CamShakeTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (m_SM_CamShakeTimer.IsRunning())
			m_SM_CamShakeTimer.Stop();

		m_SM_CamShakeTimer.Run(SM_CAMSHAKE_TICK_S, this, "ShumMutant_CamShakeTick", null, true);
		ShumMutant_CamShakeTick();
	}

	void ShumMutant_CamShakeTick()
	{
		if (!m_SM_CamShake)
		{
			ShumMutant_StopCameraShake();
			return;
		}

		float lr, ud;
		bool active = m_SM_CamShake.Update(SM_CAMSHAKE_TICK_S, lr, ud);

		DayZPlayerImplement dpi = DayZPlayerImplement.Cast(this);
		if (dpi)
			dpi.GetAimingModel().SetCamShakeValues(lr, ud);

		if (!active)
			ShumMutant_StopCameraShake();
	}

	void ShumMutant_StopCameraShake()
	{
		if (m_SM_CamShakeTimer && m_SM_CamShakeTimer.IsRunning())
			m_SM_CamShakeTimer.Stop();

		m_SM_CamShake = null;

		DayZPlayerImplement dpi = DayZPlayerImplement.Cast(this);
		if (dpi)
			dpi.GetAimingModel().SetCamShakeValues(0, 0);
	}
	#endif


	#ifndef SERVER
	void ShumMutant_PlayTeleHitSound()
	{
		// Воспроизводится НА ИГРОКЕ при любом телекинезном ударе (бросок).
		// Крутится в JSON: THROW_HIT_SOUNDSET.
		if (ShumMutantTuning.THROW_HIT_SOUNDSET == string.Empty)
			return;

		vector headPos;
		MiscGameplayFunctions.GetHeadBonePos(this, headPos);
		EffectSound hit = SEffectManager.CreateSound(ShumMutantTuning.THROW_HIT_SOUNDSET, headPos);
		if (hit)
		{
			hit.SetParent(this);
			hit.SetAttachedLocalPos(WorldToModel(headPos));
			hit.SetSoundWaveKind(WaveKind.WAVEEFFECTEX);
			hit.SetAutodestroy(true);
			hit.SoundPlay();
		}
	}

	void ShumMutant_ThrowShakeTick()
	{
		// Тряска камеры. Используем ванильный DayZPlayer::SpawnCameraShake(), чтобы работало стабильно.
		ShumMutant_SpawnCameraShake(m_SM_ThrowShakeStrength, ShumMutantTuning.THROW_CAMERA_SHAKE_RADIUS, ShumMutantTuning.THROW_CAMERA_SHAKE_SMOOTH, ShumMutantTuning.THROW_CAMERA_SHAKE_DECAY);
	}


	void ShumMutant_StartPushHaze(float intensity)
	{
		// При получении толчка/броска вешаем на игрока сильное искажение воздуха на пару секунд.
		// Крутить параметры можно в $profile:\SFP\ShumMutantConfig.json (PUSH_HAZE_*).
		if (!ShumMutantTuning.PUSH_HAZE_ENABLED)
			return;

		float dur = ShumMutantTuning.PUSH_HAZE_DURATION_S;
		if (dur <= 0)
			return;

		if (!m_SM_PushHazeLight)
		{
			m_SM_PushHazeLight = ScriptedLightBase.CreateLight(ShumMutantPlayerPushHazeLight, vector.Zero);
			if (m_SM_PushHazeLight)
				m_SM_PushHazeLight.AttachOnObject(this, ShumMutantTuning.PUSH_HAZE_LOCAL_POS);
		}

		// Подстраиваем силу по интенсивности толчка (ближе к мутанту = сильнее)
		PointLightBase pl = PointLightBase.Cast(m_SM_PushHazeLight);
		if (pl)
		{
			float t = Math.Clamp(intensity, 0.0, 1.0);
			pl.SetHeatHazeRadius(ShumMutantTuning.PUSH_HAZE_RADIUS * Math.Lerp(0.90, 1.25, t));
			pl.SetHeatHazePower(ShumMutantTuning.PUSH_HAZE_POWER  * Math.Lerp(0.90, 1.30, t));
		}

		if (!m_SM_PushHazeTimer)
			m_SM_PushHazeTimer = new Timer(CALL_CATEGORY_SYSTEM);
		else if (m_SM_PushHazeTimer.IsRunning())
			m_SM_PushHazeTimer.Stop();

		m_SM_PushHazeTimer.Run(dur, this, "ShumMutant_StopPushHaze", null, false);
	}

	void ShumMutant_StopPushHaze()
	{
		if (m_SM_PushHazeLight)
		{
			m_SM_PushHazeLight.Destroy();
			m_SM_PushHazeLight = null;
		}
	}

	protected void ShumMutant_StartTinnitus(float progressMult, float soundMax)
	{
		// Максимально близко к ванильному FlashbangEffect.PlaySound(), но без ослепления.
		// Даёт: звон + приглушение (attenuation).
		progressMult = Math.Clamp(progressMult, 0.25, 1.5);
		soundMax = Math.Clamp(soundMax, 0.1, 1.0);

		// Stop previous
		if (m_SM_TinnitusSound)
		{
			m_SM_TinnitusSound.SoundStop();
			SEffectManager.DestroyEffect(m_SM_TinnitusSound);
			m_SM_TinnitusSound = null;
		}

		if (!m_SM_TinnitusTimer)
			m_SM_TinnitusTimer = new Timer(CALL_CATEGORY_SYSTEM);
		else if (m_SM_TinnitusTimer.IsRunning())
			m_SM_TinnitusTimer.Stop();

		// Аттенюация как от флешки (глушит окружение)
		if (ShumMutantTuning.TINNITUS_ATTENUATION != string.Empty)
			SetMasterAttenuation(ShumMutantTuning.TINNITUS_ATTENUATION);

		vector headPos;
		MiscGameplayFunctions.GetHeadBonePos(this, headPos);

		// Создаём звук как в ваниле: CreateSound + parent + attach
		m_SM_TinnitusSound = SEffectManager.CreateSound(ShumMutantTuning.TINNITUS_SOUNDSET, headPos);
		if (m_SM_TinnitusSound && !m_SM_TinnitusSound.IsPlaying())
		{
			m_SM_TinnitusSound.SetParent(this);
			m_SM_TinnitusSound.SetAttachedLocalPos(WorldToModel(headPos));
			m_SM_TinnitusSound.SetSoundWaveKind(WaveKind.WAVEEFFECTEX);

			float fadeIn = 4 * Math.Clamp(progressMult, 0.5, 1.0);
			float fadeOut = 2 * Math.Clamp(progressMult, 0.5, 1.0);
			m_SM_TinnitusSound.SetSoundFadeIn(fadeIn);
			m_SM_TinnitusSound.SetSoundFadeOut(fadeOut);

			m_SM_TinnitusSound.SetSoundMaxVolume(soundMax);
			m_SM_TinnitusSound.SetSoundLoop(true);
			m_SM_TinnitusSound.SoundPlay();
			m_SM_TinnitusSound.SetAutodestroy(true);
		}

		float duration = ShumMutantTuning.TINNITUS_BASE_DURATION_S * progressMult;
		m_SM_TinnitusTimer.Run(duration, this, "ShumMutant_StopTinnitus", null, false);
	}

	void ShumMutant_StopTinnitus()
	{
		if (m_SM_TinnitusSound)
		{
			m_SM_TinnitusSound.SoundStop();
			m_SM_TinnitusSound = null;
		}
		SetMasterAttenuation(string.Empty); // вернуть звук в норму
	}
	#endif
}


#ifndef SERVER
// Ванильная логика, адаптированная из scripts/4_World/classes/CameraShake.c
class ShumMutantCameraShake
{
	float m_Radius;
	float m_RadiusDecaySpeed;
	float m_RandomAngle;
	float m_Smoothness;
	float m_StrengthFactor;

	void ShumMutantCameraShake(float strength_factor, float radius, float smoothness, float radius_decay_speed)
	{
		m_Radius = radius;
		m_RadiusDecaySpeed = radius_decay_speed;
		m_Smoothness = smoothness;
		m_StrengthFactor = strength_factor;
		m_RandomAngle = 0.0;
	}

	bool Update(float delta_time, out float x_axis, out float y_axis)
	{
		m_Radius -= delta_time * m_RadiusDecaySpeed; // уменьшаем амплитуду

		if (m_RandomAngle >= 0)
			m_RandomAngle = -m_Radius + Math.RandomFloat(-m_Radius / m_Smoothness, m_Radius / m_Smoothness);
		else
			m_RandomAngle = m_Radius + Math.RandomFloat(-m_Radius / m_Smoothness, m_Radius / m_Smoothness);

		x_axis = m_RandomAngle * m_StrengthFactor;
		y_axis = m_RandomAngle * m_StrengthFactor;

		return (m_Radius >= 0.01);
	}
}
#endif