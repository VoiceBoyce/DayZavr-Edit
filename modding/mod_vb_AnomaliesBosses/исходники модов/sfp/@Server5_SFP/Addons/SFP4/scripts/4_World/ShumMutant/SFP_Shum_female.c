// ShumMutant - main infected class
// Class name MUST match your config classname: SFP_Shum_female
// Inherits from ZombieMaleBase as requested.
//
// Implemented features using vanilla DayZ script APIs:
// 1) While aggressive (CHASE/FIGHT), periodically throws all players within 50m using physics impulse (no teleport).
// 2) While aggressive and focused on a player, triggers tinnitus-only flashbang effect (no blinding).
// 3) If a focused target is unreachable for melee (e.g. climbed on a car/plate/shed), forces a wind burst.
// 4) On death: flashbang explosion soundset + strong camera shake on nearby clients, and knockback.

class SFP_Shum_female extends ZombieMaleBase
{
    // Cooldown/state trackers (server-side)
    protected float m_WindCooldownAcc;
    protected float m_TinnitusCooldownAcc;
    protected float m_UnreachableAcc;
    protected bool  m_WasAggressive;
	// Отдельный таймер для телекинеза предметов (бросаем предметы В ЦЕЛЬ)
	protected float m_ItemThrowCooldownAcc;

	// Temporary invulnerability (server-driven, synced)
	protected bool  m_SM_Invulnerable;
	protected float m_SM_InvulnTimeLeft;
	protected float m_SM_InvulnCooldownLeft;

	#ifndef SERVER
	protected ScriptedLightBase m_HeatHazeLight;
	protected ScriptedLightBase m_InvulnLight;
	protected bool m_SM_InvulnLocal;
	// Постоянный партикл на мутанте (например, мусор/пыль "вверх")
	protected ParticleSource m_SM_IdleParticle;
	// Звуки защиты (воспроизводятся НА КЛИЕНТЕ, т.к. PlaySoundSet* не работает на dedicated server)
	protected EffectSound m_SM_ArmorSoundLoop;        // SFP_ShumF_TeleArmor_Soundset
	protected EffectSound m_SM_ArmorReloadSoundLoop;  // SFP_ShumF_TeleArmorReload_Soundset
	#endif
	override bool CanBeSkinned()
	{
		return true;
	}

    void SFP_Shum_female()
	{
		ShumMutantTuning.EnsureLoaded(); // загрузка настроек из $profile:\SFP\ShumMutantConfig.json

        // Start with abilities "ready" so the first aggro can fire immediately.
        m_WindCooldownAcc     = ShumMutantTuning.WIND_COOLDOWN_S;
        m_TinnitusCooldownAcc = ShumMutantTuning.TINNITUS_COOLDOWN_S;
        m_UnreachableAcc      = 0.0;
        m_WasAggressive       = false;
		m_ItemThrowCooldownAcc = 0.0;

		m_SM_Invulnerable = false;
		m_SM_InvulnTimeLeft = 0.0;
		m_SM_InvulnCooldownLeft = 0.0;
		RegisterNetSyncVariableBool("m_SM_Invulnerable");

		#ifndef SERVER
		m_HeatHazeLight = null;
		m_InvulnLight = null;
		m_SM_InvulnLocal = false;
		m_SM_IdleParticle = null;
		m_SM_ArmorSoundLoop = null;
		m_SM_ArmorReloadSoundLoop = null;
		#endif
    }

	// Client-side visual: attach a tiny heat-haze light to the mutant (distortion visible day and night)
	override void EEInit()
	{
		super.EEInit();

		// Server: initialize invulnerability cycle
		if (GetGame() && GetGame().IsServer())
		{
			if (m_SM_InvulnCooldownLeft <= 0.0)
				m_SM_InvulnCooldownLeft = Math.RandomFloatInclusive(ShumMutantTuning.INVULN_COOLDOWN_MIN_S, ShumMutantTuning.INVULN_COOLDOWN_MAX_S);
			m_SM_InvulnTimeLeft = 0.0;
			m_SM_Invulnerable = false;
			SetAllowDamage(true);
			SetSynchDirty();
		}

		#ifndef SERVER
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;


			// На трупе луп-звуки не должны висеть
			if (!IsAlive())
			{
				if (m_SM_ArmorSoundLoop) { StopSoundSet(m_SM_ArmorSoundLoop); m_SM_ArmorSoundLoop = null; }
				if (m_SM_ArmorReloadSoundLoop) { StopSoundSet(m_SM_ArmorReloadSoundLoop); m_SM_ArmorReloadSoundLoop = null; }
				return;
			}
		if (!m_HeatHazeLight)
		{
			m_HeatHazeLight = ScriptedLightBase.CreateLight(ShumMutantHeatHazeLight, vector.Zero);
			if (m_HeatHazeLight)
				m_HeatHazeLight.AttachOnObject(this, ShumMutantTuning.HEAT_HAZE_LOCAL_POS);
		}

		// Постоянный партикл на мутанте (если зарегистрирован в ParticleList)
		SM_EnsureIdleParticle();
		UpdateInvulnVisual();
		#endif
	}

	void ~SFP_Shum_female()
	{
		#ifndef SERVER
		SM_CleanupAllFX();
		#endif
	}

	#ifndef SERVER
	// ---------------------------------------------------------
	// FX helpers (client)
	// ---------------------------------------------------------
	protected void SM_EnsureIdleParticle()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		// На трупе ничего не держим
		if (!IsAlive())
		{
			if (m_SM_IdleParticle)
			{
				m_SM_IdleParticle.Stop();
				m_SM_IdleParticle = null;
			}
			return;
		}

		if (!ShumMutantTuning.MUTANT_IDLE_PARTICLE_ENABLED)
		{
			if (m_SM_IdleParticle)
			{
				m_SM_IdleParticle.Stop();
				m_SM_IdleParticle = null;
			}
			return;
		}

		if (m_SM_IdleParticle)
			return;

		ParticleManager pm = ParticleManager.GetInstance();
		if (!pm)
			return;

		// Вариант A: частица уже зарегистрирована в другом PBO (SFP3),
		// поэтому используем ID из ParticleList напрямую, как в ванильных скриптах.
		// Пример: ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_FIRE01_GRILL, this, Vector(...));
		m_SM_IdleParticle = pm.PlayOnObject(ParticleList.SFP_TELE_UP_TRASH_PRT, this, ShumMutantTuning.MUTANT_IDLE_PARTICLE_LOCAL_POS);
	}

	protected void SM_CleanupAllFX()
	{
		// Партикл
		if (m_SM_IdleParticle)
		{
			m_SM_IdleParticle.Stop();
			m_SM_IdleParticle = null;
		}

		// Свет/искажение
		if (m_HeatHazeLight)
		{
			m_HeatHazeLight.Destroy();
			m_HeatHazeLight = null;
		}
		if (m_InvulnLight)
		{
			m_InvulnLight.Destroy();
			m_InvulnLight = null;
		}

		// Луп-звуки
		if (m_SM_ArmorSoundLoop)
		{
			StopSoundSet(m_SM_ArmorSoundLoop);
			m_SM_ArmorSoundLoop = null;
		}
		if (m_SM_ArmorReloadSoundLoop)
		{
			StopSoundSet(m_SM_ArmorReloadSoundLoop);
			m_SM_ArmorReloadSoundLoop = null;
		}
	}
	#endif

	// Sync handler (client): react to invulnerability state changes
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		#ifndef SERVER
		UpdateInvulnVisual();
		#endif
	}

    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);

		#ifndef SERVER
		// Клиент: если мутант умер, очищаем все визуальные/звуковые эффекты сразу (чтобы не оставались на трупе)
		if (GetGame() && !GetGame().IsDedicatedServer())
		{
			if (!IsAlive())
			{
				SM_CleanupAllFX();
				return;
			}
		}
		#endif


        // Server-authoritative logic (impulses + RPC triggers)
        if (!GetGame().IsServer())
            return;

        if (!IsAlive())
            return;

		// Keep invulnerability cycle running regardless of aggression
		UpdateInvulnerability(pDt);

        DayZInfectedInputController ic = GetInputController();
        if (!ic)
            return;

        int mindState = ic.GetMindState();
        bool aggressive = IsMindStateAggressive(mindState);

        if (aggressive)
        {
            // Tick cooldowns
            m_WindCooldownAcc += pDt;
            m_TinnitusCooldownAcc += pDt;
			if (ShumMutantTuning.ITEM_THROW_ENABLED)
				m_ItemThrowCooldownAcc += pDt;

            EntityAI target = ic.GetTargetEntity();
            PlayerBase targetPlayer = PlayerBase.Cast(target);

            float targetDist = 999999.0;
            bool unreachable = false;

            if (target)
            {
                targetDist = vector.Distance(GetPosition(), target.GetPosition());

                // Anti-height exploit: if target is close but cannot be attacked in melee
                if (targetDist <= ShumMutantTuning.WIND_RADIUS_M)
                {
                    unreachable = !CanAttackToPosition(target.GetPosition());
                }
            }

            // First moment of aggression: immediate burst
            if (!m_WasAggressive)
            {
				DoWindBurst();
                m_WindCooldownAcc = 0.0;
                m_UnreachableAcc = 0.0;
				// Телекинез предметов: при первом агро можно кидать сразу (отдельный таймер)
				// NOTE: avoid ternary operator for older EnforceScript parsers
				if (ShumMutantTuning.ITEM_THROW_ENABLED)
				{
					m_ItemThrowCooldownAcc = ShumMutantTuning.ITEM_THROW_COOLDOWN_S;
				}
				else
				{
					m_ItemThrowCooldownAcc = 0.0;
				}
                m_WasAggressive = true;
            }

            // Periodic wind burst (1)
            if (m_WindCooldownAcc >= ShumMutantTuning.WIND_COOLDOWN_S)
            {
				DoWindBurst();
                m_WindCooldownAcc = 0.0;
                m_UnreachableAcc = 0.0;
            }

			// Отдельный таймер: бросание предметов В ФОКУСИРОВАННОГО игрока
			// (не привязано к "волне" игроков)
			if (targetPlayer && ShumMutantTuning.ITEM_THROW_ENABLED && ShumMutantTuning.ITEM_THROW_COOLDOWN_S > 0.05)
			{
				float trgDist = targetDist;
				if (trgDist <= ShumMutantTuning.ITEM_THROW_TARGET_RANGE_M)
				{
					if (m_ItemThrowCooldownAcc >= ShumMutantTuning.ITEM_THROW_COOLDOWN_S)
					{
						ThrowVicinityItemsAtTarget(targetPlayer);
						m_ItemThrowCooldownAcc = 0.0;
					}
				}
			}

            // Forced wind burst if target is unreachable (3)
            if (target && unreachable)
            {
                m_UnreachableAcc += pDt;
                if (m_UnreachableAcc >= ShumMutantTuning.UNREACHABLE_GRACE_S)
                {
					DoWindBurst();
                    m_WindCooldownAcc = 0.0;
                    m_UnreachableAcc = 0.0;
                }
            }
            else
            {
                m_UnreachableAcc = 0.0;
            }

            // Focused tinnitus-only hit (2)
			if (m_TinnitusCooldownAcc >= ShumMutantTuning.TINNITUS_COOLDOWN_S)
			{
				PlayerBase tinnitusTarget = null;
				float tinnitusDist = 999999.0;

				// Primary: current focus target
				if (targetPlayer && targetDist <= ShumMutantTuning.TINNITUS_RANGE_M)
				{
					tinnitusTarget = targetPlayer;
					tinnitusDist = targetDist;
				}

				// Fallback: nearest player in range (helps when infected targeting is unstable)
				if (!tinnitusTarget && ShumMutantTuning.TINNITUS_FALLBACK_NEAREST)
				{
					array<Man> pl = new array<Man>;
					GetGame().GetPlayers(pl);
					vector myP = GetPosition();
					for (int k = 0; k < pl.Count(); k++)
					{
						PlayerBase pb2 = PlayerBase.Cast(pl[k]);
						if (!pb2 || !pb2.IsAlive() || pb2.GetCommand_Vehicle())
							continue;
						float d = vector.Distance(myP, pb2.GetPosition());
						if (d <= ShumMutantTuning.TINNITUS_RANGE_M && d < tinnitusDist)
						{
							tinnitusTarget = pb2;
							tinnitusDist = d;
						}
					}
				}

				if (tinnitusTarget)
				{
					float tt = 1.0 - Math.Clamp(tinnitusDist / ShumMutantTuning.TINNITUS_RANGE_M, 0.0, 1.0);
					float progressMult = Math.Lerp(ShumMutantTuning.TINNITUS_PROGRESS_MIN, ShumMutantTuning.TINNITUS_PROGRESS_MAX, tt);
					SendTinnitusOnly(tinnitusTarget, progressMult, ShumMutantTuning.TINNITUS_SOUND_MAX);
					m_TinnitusCooldownAcc = 0.0;
				}
			}
        }
        else
        {
            // Not aggressive: reset state, but keep cooldowns from going negative
            m_WasAggressive = false;
            m_UnreachableAcc = 0.0;

            m_WindCooldownAcc = Math.Min(m_WindCooldownAcc + pDt, ShumMutantTuning.WIND_COOLDOWN_S);
            m_TinnitusCooldownAcc = Math.Min(m_TinnitusCooldownAcc + pDt, ShumMutantTuning.TINNITUS_COOLDOWN_S);
			if (ShumMutantTuning.ITEM_THROW_ENABLED)
				m_ItemThrowCooldownAcc = Math.Min(m_ItemThrowCooldownAcc + pDt, ShumMutantTuning.ITEM_THROW_COOLDOWN_S);
			else
				m_ItemThrowCooldownAcc = 0.0;
        }
    }

    protected bool IsMindStateAggressive(int mindState)
    {
        return (mindState == DayZInfectedConstants.MINDSTATE_CHASE || mindState == DayZInfectedConstants.MINDSTATE_FIGHT);
    }

    // (1) + (3) - throw all players in radius using impulse (no teleport)
	protected void DoWindBurst()
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        vector myPos = GetPosition();

        for (int i = 0; i < players.Count(); i++)
        {
            Man man = players[i];
            if (!man)
                continue;

            if (!man.IsAlive())
                continue;

            PlayerBase pb = PlayerBase.Cast(man);
            if (!pb)
                continue;

            // Skip players in vehicles (impulse is unreliable / can cause issues)
            if (pb.GetCommand_Vehicle())
                continue;

            float dist = vector.Distance(myPos, man.GetPosition());
            if (dist > ShumMutantTuning.WIND_RADIUS_M)
                continue;

			// Physics only on a player standing/walking on his own (not swimming, on a ladder, unconscious...)
			if (!SFP_Safety.CanPushPlayer(pb))
				continue;

			// Intensity scales with proximity
			float intensity = 1.0 - Math.Clamp(dist / ShumMutantTuning.WIND_RADIUS_M, 0.0, 1.0);

			ApplyThrowImpulse(pb, ShumMutantTuning.WIND_FORCE_HORIZONTAL, ShumMutantTuning.WIND_FORCE_UP, ShumMutantTuning.WIND_FORCE_RANDOM_YAW);
			pb.ShumMutant_OnThrown(this, intensity);
        }
    }

	// Бросает предметы в радиусе мутанта в сторону фокусированного игрока.
	// Работает только на сервере.
	protected void ThrowVicinityItemsAtTarget(PlayerBase focusTarget)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!ShumMutantTuning.ITEM_THROW_ENABLED)
			return;
		if (!focusTarget || !focusTarget.IsAlive() || focusTarget.GetCommand_Vehicle())
			return;

		float r = ShumMutantTuning.ITEM_THROW_RADIUS_M;
		if (r <= 0)
			return;

			array<Object> objs = new array<Object>;
			array<CargoBase> proxyCargo = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r, objs, proxyCargo);

		int maxItems = Math.Max(0, ShumMutantTuning.ITEM_THROW_MAX_ITEMS);
		if (maxItems == 0)
			return;

		vector targetPos = focusTarget.GetPosition();
		targetPos[1] = targetPos[1] + 1.0; // примерно в туловище
		int thrown = 0;

		// Обрабатываем и обычные объекты, и proxy_cargo (на случай предметов в прокси-карго)
		// Ищем предметы в радиусе и целенаправленно швыряем их в фокус-цель
		for (int i = 0; i < objs.Count(); i++)
		{
			if (thrown >= maxItems)
				break;

			Object obj = objs.Get(i);
			if (!obj)
				continue;

			if (obj == this)
				continue;

			ItemBase item = ItemBase.Cast(obj);
			if (!item)
				continue;

			// не трогаем предметы, которые в руках/инвентаре у кого-то
			if (item.GetHierarchyRootPlayer() != null)
				continue;

			// исключаем вложенные предметы (чтобы не пытаться кидать содержимое карго)
			if (item.GetHierarchyParent() != null)
				continue;

			// фильтр по массе: dBodyGetMass иногда возвращает 0 у "спящих" предметов — делаем fallback по весу из конфига
			float mass = dBodyGetMass(item);
			if (mass <= 0.0)
				mass = (float)item.GetWeight() / 1000.0; // g -> kg

			if (mass < ShumMutantTuning.ITEM_THROW_MIN_MASS_KG || mass > ShumMutantTuning.ITEM_THROW_MAX_MASS_KG)
				continue;

				vector itemPos = item.GetPosition();
				vector dir = targetPos - itemPos;
				float len = dir.Normalize(); // Normalize() возвращает длину до нормализации
				if (len < 0.1)
					continue;

			// небольшой разброс, чтобы не было идеально "лазерно"
				float spread = ShumMutantTuning.ITEM_THROW_RANDOM_SPREAD_DEG;
				if (spread > 0.0)
				{
					// В Enforce нет VectorRotate/Quaternion API на vector, поэтому работаем через углы
					float yawDelta   = Math.RandomFloatInclusive(-spread, spread);
					float pitchDelta = Math.RandomFloatInclusive(-spread, spread);
					vector ang = dir.VectorToAngles();
					ang[0] = ang[0] + yawDelta;   // yaw
					ang[1] = ang[1] + pitchDelta; // pitch
					dir = ang.AnglesToVector();
					dir.Normalize();
				}

			float fH = ShumMutantTuning.ITEM_THROW_FORCE_HORIZONTAL;
			float fU = ShumMutantTuning.ITEM_THROW_FORCE_UP;
			float massScale = 1.0;
			if (ShumMutantTuning.ITEM_THROW_SCALE_BY_MASS)
			{
				// тяжелее предмет — чуть меньше разгоняем, легче — чуть больше (в пределах)
				float tMass = Math.Clamp((mass - ShumMutantTuning.ITEM_THROW_MIN_MASS_KG) / Math.Max(0.01, (ShumMutantTuning.ITEM_THROW_MAX_MASS_KG - ShumMutantTuning.ITEM_THROW_MIN_MASS_KG)), 0.0, 1.0);
				massScale = Math.Lerp(ShumMutantTuning.ITEM_THROW_MASS_SCALE_MAX, ShumMutantTuning.ITEM_THROW_MASS_SCALE_MIN, tMass);
			}

			vector impulse = (dir * (fH * massScale));
			impulse[1] = impulse[1] + (fU * massScale);

				// Для ground loot на сервере надёжнее ThrowPhysically (ванильный метод InventoryItem)
				item.ThrowPhysically(null, impulse, true);
			thrown++;
		}
	}

    protected void ApplyThrowImpulse(notnull IEntity victim, float forceHorizontal, float forceUp, float randomYawDeg)
    {
        // Base direction (away from mutant). Random yaw ensures "куда попало" while still feeling like a push.
        // NOTE: ApplyImpulse operates on IEntity, which does not expose GetPosition().
        // Use GetOrigin() (native on IEntity) to compute directions in world space.
        vector baseDir = victim.GetOrigin() - GetOrigin();
        baseDir[1] = 0;

        if (baseDir.LengthSq() < 0.0001)
        {
            baseDir = Vector(0, 0, 1);
        }

        baseDir.Normalize();

        float baseYaw = Math.Atan2(baseDir[0], baseDir[2]); // radians
        float yawOffset = Math.RandomFloat(-randomYawDeg, randomYawDeg) * Math.DEG2RAD;
        float yaw = baseYaw + yawOffset;

        vector dir = Vector(Math.Sin(yaw), 0, Math.Cos(yaw));
        dir.Normalize();

        vector impulse = dir * forceHorizontal;
        impulse[1] = forceUp + Math.RandomFloat(-forceUp * 0.15, forceUp * 0.15);

        dBodyApplyImpulse(victim, impulse);
    }

    protected void SendTinnitusOnly(PlayerBase player, float progressMult, float soundMax)
    {
        if (!player || !player.GetIdentity())
            return;

        ref Param2<float, float> p = new Param2<float, float>(progressMult, soundMax);
        GetGame().RPCSingleParam(player, ShumMutantRPC.RPC_TINNITUS_ONLY, p, true, player.GetIdentity());
    }

    // (4) Death burst
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);


		#ifndef SERVER
		// Клиент: при смерти мутанта очищаем все эффекты, чтобы на трупе не оставались луп-звуки/искажение
		if (GetGame() && !GetGame().IsDedicatedServer())
			SM_CleanupAllFX();
		#endif

        if (!GetGame().IsServer())
            return;

        DoDeathBurst();
    }

    protected void DoDeathBurst()
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        vector myPos = GetPosition();

        for (int i = 0; i < players.Count(); i++)
        {
            Man man = players[i];
            if (!man)
                continue;

            if (!man.IsAlive())
                continue;

            PlayerBase pb = PlayerBase.Cast(man);
            if (!pb || !pb.GetIdentity())
                continue;

            if (pb.GetCommand_Vehicle())
                continue;

            float dist = vector.Distance(myPos, pb.GetPosition());
            if (dist > ShumMutantTuning.DEATH_RADIUS_M)
                continue;

            // Knockback (server-authoritative impulse), only on a player standing/walking on his own
            if (SFP_Safety.CanPushPlayer(pb))
                ApplyThrowImpulse(pb, ShumMutantTuning.DEATH_FORCE_HORIZONTAL, ShumMutantTuning.DEATH_FORCE_UP, 180.0);

			// Client feedback (sound + camera shake), scaled by distance
            float t = 1.0 - Math.Clamp(dist / ShumMutantTuning.DEATH_RADIUS_M, 0.0, 1.0);
            float volume = Math.Lerp(0.35, 1.0, t);
			float shake = Math.Lerp(ShumMutantTuning.DEATH_CAMERA_SHAKE_STRENGTH * 0.5, ShumMutantTuning.DEATH_CAMERA_SHAKE_STRENGTH, t);

			ref Param2<float, float> fx = new Param2<float, float>(shake, volume);
            GetGame().RPCSingleParam(pb, ShumMutantRPC.RPC_DEATH_FX, fx, true, pb.GetIdentity());
        }
    }

	// --------------------
	// Invulnerability
	protected void UpdateInvulnerability(float pDt)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		// If already dead, do nothing
		if (!IsAlive())
			return;

		if (m_SM_Invulnerable)
		{
			m_SM_InvulnTimeLeft -= pDt;
			if (m_SM_InvulnTimeLeft <= 0.0)
				SetInvulnerable(false);
		}
		else
		{
			m_SM_InvulnCooldownLeft -= pDt;
			if (m_SM_InvulnCooldownLeft <= 0.0)
				SetInvulnerable(true);
		}
	}

	protected void SetInvulnerable(bool state)
	{
		if (m_SM_Invulnerable == state)
			return;

		m_SM_Invulnerable = state;
		if (m_SM_Invulnerable)
		{
			SetAllowDamage(false);
			m_SM_InvulnTimeLeft = Math.RandomFloatInclusive(ShumMutantTuning.INVULN_DURATION_MIN_S, ShumMutantTuning.INVULN_DURATION_MAX_S);
			m_SM_InvulnCooldownLeft = 0.0;
		}
		else
		{
			SetAllowDamage(true);
			m_SM_InvulnCooldownLeft = Math.RandomFloatInclusive(ShumMutantTuning.INVULN_COOLDOWN_MIN_S, ShumMutantTuning.INVULN_COOLDOWN_MAX_S);
			m_SM_InvulnTimeLeft = 0.0;
		}

		SetSynchDirty();
	}

	#ifndef SERVER
	protected void UpdateInvulnVisual()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;


			// Если объект уже труп - эффекты должны быть сняты
			if (!IsAlive())
			{
				SM_CleanupAllFX();
				return;
			}
		// Визуал обновляем только при смене состояния, но звук обновляем ВСЕГДА.
		bool changed = (m_SM_Invulnerable != m_SM_InvulnLocal);
		if (changed)
			m_SM_InvulnLocal = m_SM_Invulnerable;

		// Always keep base light at base values (can be boosted while invulnerable)
		PointLightBase baseLight = PointLightBase.Cast(m_HeatHazeLight);

		if (m_SM_Invulnerable)
		{
			if (!m_InvulnLight)
			{
				m_InvulnLight = ScriptedLightBase.CreateLight(ShumMutantInvulnLight, vector.Zero);
				if (m_InvulnLight)
					m_InvulnLight.AttachOnObject(this, ShumMutantTuning.INVULN_LIGHT_LOCAL_POS);
			}

			if (baseLight)
			{
				baseLight.EnableHeatHaze(true);
				baseLight.SetHeatHazeRadius(ShumMutantTuning.INVULN_HEAT_HAZE_RADIUS);
				baseLight.SetHeatHazePower(ShumMutantTuning.INVULN_HEAT_HAZE_POWER);
			}
		}
		else
		{
			if (m_InvulnLight)
			{
				m_InvulnLight.Destroy();
				m_InvulnLight = null;
			}

			if (baseLight)
			{
				baseLight.EnableHeatHaze(true);
				baseLight.SetHeatHazeRadius(ShumMutantTuning.HEAT_HAZE_RADIUS);
				baseLight.SetHeatHazePower(ShumMutantTuning.HEAT_HAZE_POWER);
			}
		}

		// Аудио: броня/перезарядка
		UpdateInvulnAudio();
	}

	protected void UpdateInvulnAudio()
	{
		// Воспроизведение звуковых сетов защиты.
		// ВАЖНО: PlaySoundSet* работает только на клиенте (на dedicated server вернёт false).
		// Звуки задаются в JSON:
		// 	INVULN_ARMOR_SOUNDSET = "SFP_ShumF_TeleArmor_Soundset"
		// 	INVULN_RELOAD_SOUNDSET = "SFP_ShumF_TeleArmorReload_Soundset"
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		if (m_SM_Invulnerable)
		{
			// Идёт броня
			if (m_SM_ArmorReloadSoundLoop)
			{
				StopSoundSet(m_SM_ArmorReloadSoundLoop);
				m_SM_ArmorReloadSoundLoop = null;
			}
			if (ShumMutantTuning.INVULN_ARMOR_SOUNDSET != string.Empty)
				PlaySoundSetLoop(m_SM_ArmorSoundLoop, ShumMutantTuning.INVULN_ARMOR_SOUNDSET, 0, 0);
			else if (m_SM_ArmorSoundLoop)
			{
				StopSoundSet(m_SM_ArmorSoundLoop);
				m_SM_ArmorSoundLoop = null;
			}
		}
		else
		{
			// Идёт перезарядка
			if (m_SM_ArmorSoundLoop)
			{
				StopSoundSet(m_SM_ArmorSoundLoop);
				m_SM_ArmorSoundLoop = null;
			}
			if (ShumMutantTuning.INVULN_RELOAD_SOUNDSET != string.Empty)
				PlaySoundSetLoop(m_SM_ArmorReloadSoundLoop, ShumMutantTuning.INVULN_RELOAD_SOUNDSET, 0, 0);
			else if (m_SM_ArmorReloadSoundLoop)
			{
				StopSoundSet(m_SM_ArmorReloadSoundLoop);
				m_SM_ArmorReloadSoundLoop = null;
			}
		}
	}
	#endif
}