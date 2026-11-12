class SFP_MutaWolf_Base extends ZmbM_HermitSkinny_Base
{
	protected static const int SFP_MW_SOUND_SUMMON = 1;
	protected static const int SFP_MW_SOUND_AGGRO = 2;
	
	protected ref Timer m_SFP_SummonTimer;
	protected ref array<Animal_CanisLupus> m_SFP_SummonedWolves;
	protected ref NoiseParams m_SFP_WolfNoiseParams;
	protected int m_SFP_NextSummonMs;
	protected int m_SFP_NextHighgroundMs;
	protected ref Timer m_SFP_HighgroundCheckTimer;
	protected bool m_SFP_SummonPending;
	protected int m_SFP_PendingSummonCount;
	protected PlayerBase m_SFP_PendingSummonTarget;
	protected int m_SFP_SoundEventCounter;
	protected int m_SFP_SoundEventType;
	protected int m_SFP_LastSoundEventCounter;
	protected int m_SFP_AggroStartMs;
	
	void SFP_MutaWolf_Base()
	{
		RegisterNetSyncVariableInt("m_SFP_SoundEventCounter", 0, int.MAX);
		RegisterNetSyncVariableInt("m_SFP_SoundEventType", 0, 2);
		
		m_SFP_SummonedWolves = new array<Animal_CanisLupus>;
		m_SFP_WolfNoiseParams = new NoiseParams();
		m_SFP_WolfNoiseParams.Load("WolfRoarNoise");
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SFP_MW_ConfigManager.Get();
			m_SFP_SummonTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
			m_SFP_HighgroundCheckTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		}
	}
	
	void ~SFP_MutaWolf_Base()
	{
		if (m_SFP_SummonTimer && m_SFP_SummonTimer.IsRunning())
			m_SFP_SummonTimer.Stop();
		if (m_SFP_HighgroundCheckTimer && m_SFP_HighgroundCheckTimer.IsRunning())
			m_SFP_HighgroundCheckTimer.Stop();
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
			if (cfg && cfg.MutantHealth > 0)
				SetHealth("", "Health", Math.Clamp(cfg.MutantHealth, 0.0, GetMaxHealth("", "Health")));
		}
	}
	
	override bool CanBeSkinned()
	{
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (!GetGame().IsDedicatedServer() && m_SFP_SoundEventCounter != m_SFP_LastSoundEventCounter)
		{
			m_SFP_LastSoundEventCounter = m_SFP_SoundEventCounter;
			SFP_MW_PlaySyncedSound();
		}
	}
	
	override bool HandleMindStateChange(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		int oldMindState = m_LastMindState;
		bool ret = super.HandleMindStateChange(pCurrentCommandID, pInputController, pDt);
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			bool oldAggro = (oldMindState == DayZInfectedConstants.MINDSTATE_CHASE || oldMindState == DayZInfectedConstants.MINDSTATE_FIGHT);
			bool newAggro = (m_MindState == DayZInfectedConstants.MINDSTATE_CHASE || m_MindState == DayZInfectedConstants.MINDSTATE_FIGHT);
			
			if (newAggro && !oldAggro)
			{
				m_SFP_AggroStartMs = GetGame().GetTime();
				SFP_MW_StartHighgroundCheck();
			}
			else if (!newAggro)
			{
				m_SFP_AggroStartMs = 0;
				m_SFP_SummonPending = false;
				m_SFP_PendingSummonCount = 0;
				m_SFP_PendingSummonTarget = null;
				if (m_SFP_SummonTimer && m_SFP_SummonTimer.IsRunning())
					m_SFP_SummonTimer.Stop();
				if (m_SFP_HighgroundCheckTimer && m_SFP_HighgroundCheckTimer.IsRunning())
					m_SFP_HighgroundCheckTimer.Stop();
			}
		}
		
		return ret;
	}
	
	protected void SFP_MW_BroadcastSoundEvent(int soundType)
	{
		m_SFP_SoundEventType = soundType;
		m_SFP_SoundEventCounter++;
		SetSynchDirty();
		
		if (!GetGame().IsMultiplayer())
			SFP_MW_PlaySyncedSound();
	}
	
	protected void SFP_MW_PlaySyncedSound()
	{
		EffectSound snd;
		if (m_SFP_SoundEventType == SFP_MW_SOUND_SUMMON)
			PlaySoundSet(snd, "SFP_MutantWolf_summonerWolf_Soundset", 0, 0);
		else if (m_SFP_SoundEventType == SFP_MW_SOUND_AGGRO)
			PlaySoundSet(snd, "SFP_MutantWolf_AGRE_Soundset", 0, 0);
	}
	
	override bool ModCommandHandlerInside(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			SFP_MW_ServerTick();
		
		return super.ModCommandHandlerInside(pDt, pCurrentCommandID, pCurrentCommandFinished);
	}
	
	override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE)
		{
			int mindState = pInputController.GetMindState();
			if (mindState == DayZInfectedConstants.MINDSTATE_CHASE)
				return ChaseAttackLogic(pCurrentCommandID, pInputController, pDt);
			else if (mindState == DayZInfectedConstants.MINDSTATE_FIGHT)
				return FightAttackLogic(pCurrentCommandID, pInputController, pDt);
		}
		else if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
		{
			DayZInfectedCommandAttack attackCommand = GetCommand_Attack();
			if (attackCommand && attackCommand.WasHit() && m_ActualTarget != null)
			{
				if (m_ActualTarget.GetMeleeTargetType() == EMeleeTargetType.NONALIGNABLE)
					return false;
				
				bool playerInBlockStance = false;
				vector targetPos = m_ActualTarget.GetPosition();
				vector hitPosWS = targetPos;
				vector zombiePos = GetPosition();
				bool dealtRealHit = false;
				bool shouldApplyExtra = false;
				string damageAmmo = m_ActualAttackType.m_AmmoType;
				
				PlayerBase playerTarget = PlayerBase.Cast(m_ActualTarget);
				if (playerTarget)
					playerInBlockStance = playerTarget.GetMeleeFightLogic() && playerTarget.GetMeleeFightLogic().IsInBlock();
				
				if (vector.DistanceSq(targetPos, zombiePos) <= m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance)
				{
					if (playerInBlockStance && (Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(targetPos, MiscGameplayFunctions.GetHeadingVector(playerTarget), zombiePos))) <= GameConstants.AI_MAX_BLOCKABLE_ANGLE)
					{
						if (m_ActualAttackType.m_IsHeavy == 1)
						{
							hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition());
							damageAmmo = "MeleeZombie";
							DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), damageAmmo, hitPosWS);
							dealtRealHit = true;
							shouldApplyExtra = true;
						}
						else
						{
							hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition());
							DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), "Dummy_Light", hitPosWS);
						}
					}
					else
					{
						hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition());
						DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), damageAmmo, hitPosWS);
						dealtRealHit = true;
						shouldApplyExtra = true;
					}
				}
				
				if (dealtRealHit && shouldApplyExtra)
					SFP_MW_ApplyHitExtras(m_ActualTarget, hitPosWS, damageAmmo);
			}
			
			return true;
		}
		
		return false;
	}
	
	protected void SFP_MW_ServerTick()
	{
		if (!IsAlive())
			return;
		
		SFP_MW_CleanupSummonedWolves();
		SFP_MW_UpdateMobility();
		SFP_MW_TryStartSummon();
	}
	
	protected void SFP_MW_UpdateMobility()
	{
		// Возвращаем стандартное поведение ИИ без кастомной мобильности.
		DayZInfectedInputController ic = GetInputController();
		if (!ic)
			return;
		ic.OverrideMovementSpeed(false, 0);
		ic.OverrideTurnSpeed(false, 0);
		ic.OverrideHeading(false, 0);
		ic.OverrideLookAt(false, Vector(0, 0, 0));
	}

	protected void SFP_MW_StartHighgroundCheck()
	{
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;

		if (!m_SFP_HighgroundCheckTimer)
			m_SFP_HighgroundCheckTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		m_SFP_HighgroundCheckTimer.Stop();
		m_SFP_HighgroundCheckTimer.Run(0.20, this, "SFP_MW_ProcessHighgroundPulse", null, true);
	}

	void SFP_MW_ProcessHighgroundPulse()
	{
		if (!GetGame().IsServer() || !IsAlive())
			return;

		DayZInfectedInputController pInputController = GetInputController();
		if (!pInputController)
			return;

		int mindState = pInputController.GetMindState();
		if (mindState != DayZInfectedConstants.MINDSTATE_CHASE && mindState != DayZInfectedConstants.MINDSTATE_FIGHT)
			return;

		PlayerBase playerTarget = PlayerBase.Cast(pInputController.GetTargetEntity());
		if (!playerTarget || !playerTarget.IsAlive() || playerTarget.GetCommand_Vehicle())
			return;

		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (!cfg)
			return;

		vector targetPos = playerTarget.GetPosition();
		vector myPos = GetPosition();
		float horizontalDist = vector.Distance(Vector(targetPos[0], 0, targetPos[2]), Vector(myPos[0], 0, myPos[2]));
		float heightDelta = targetPos[1] - myPos[1];

		if (horizontalDist > cfg.HighgroundTriggerDistance)
			return;

		if (heightDelta < cfg.HighgroundMinVerticalDiff)
			return;

		bool unreachable = !CanAttackToPosition(targetPos);
		if (!unreachable && heightDelta < (cfg.HighgroundMinVerticalDiff + 0.35))
			return;

		int nowMs = GetGame().GetTime();
		if (nowMs < m_SFP_NextHighgroundMs)
			return;

		SFP_MW_BroadcastSoundEvent(SFP_MW_SOUND_AGGRO);
		DayZInfectedCommandMove moveCommand = GetCommand_Move();
		if (moveCommand && !moveCommand.IsTurning())
			moveCommand.SetIdleState(2);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		vector centerPos = playerTarget.GetPosition();
		bool applied = false;

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.IsAlive() || pb.GetCommand_Vehicle())
				continue;

			if (vector.Distance(pb.GetPosition(), centerPos) > cfg.HighgroundCheckRadius)
				continue;

			SFP_MW_SendHighgroundEffectToPlayer(pb, cfg);
			SFP_MW_ApplyHighgroundImpulseToPlayer(pb, cfg);
			applied = true;
		}

		if (applied)
			m_SFP_NextHighgroundMs = nowMs + Math.Round(cfg.ThrowCooldown * 1000.0);
	}

	protected bool SFP_MW_IsAggressive()
	{
		DayZInfectedInputController ic = GetInputController();
		if (!ic)
			return false;
		int mindState = ic.GetMindState();
		return (mindState == DayZInfectedConstants.MINDSTATE_CHASE || mindState == DayZInfectedConstants.MINDSTATE_FIGHT);
	}
	
	protected void SFP_MW_TryStartSummon()
	{
		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (!cfg || m_SFP_SummonPending)
			return;
		
		int now = GetGame().GetTime();
		if (now < m_SFP_NextSummonMs)
			return;
		if (!SFP_MW_IsAggressive())
			return;
		if (m_SFP_AggroStartMs <= 0)
			return;
		if ((now - m_SFP_AggroStartMs) < Math.Round(Math.Max(cfg.SummonAggroDelay, 0.0) * 1000.0))
			return;
		
		PlayerBase focus = SFP_MW_GetAggroTargetPlayer(Math.Max(cfg.FlashRadius, cfg.HighgroundCheckRadius));
		if (!focus)
			return;
		
		int aliveCount = SFP_MW_GetAliveSummonedWolfCount();
		int missingCount = cfg.WolvesPerSummon - aliveCount;
		if (missingCount <= 0)
			return;
		
		m_SFP_PendingSummonCount = missingCount;
		m_SFP_PendingSummonTarget = focus;
		m_SFP_SummonPending = true;
		m_SFP_NextSummonMs = now + Math.Round(cfg.SummonCooldown * 1000.0);
		
		SFP_MW_Log("Старт призыва. Живых волков=" + aliveCount.ToString() + ", нужно добрать=" + missingCount.ToString());
		
		if (m_SFP_SummonTimer && m_SFP_SummonTimer.IsRunning())
			m_SFP_SummonTimer.Stop();
		if (m_SFP_SummonTimer)
			m_SFP_SummonTimer.Run(cfg.SummonDelay, this, "SFP_MW_ExecuteSummon", null, false);
	}
	
	void SFP_MW_ExecuteSummon()
	{
		if (!IsAlive())
		{
			m_SFP_SummonPending = false;
			return;
		}
		if (!SFP_MW_IsAggressive())
		{
			m_SFP_SummonPending = false;
			return;
		}
		
		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (!cfg)
		{
			m_SFP_SummonPending = false;
			return;
		}
		
		array<PlayerBase> affectedPlayers = SFP_MW_GetPlayersInRadius(GetPosition(), cfg.FlashRadius);
		foreach (PlayerBase player : affectedPlayers)
			SFP_MW_SendSummonEffectToPlayer(player, cfg);
		
		// Даём клиенту короткое окно, чтобы ослепление стартовало раньше звуксета и спавна.
		if (m_SFP_SummonTimer)
			m_SFP_SummonTimer.Run(0.25, this, "SFP_MW_FinalizeSummon", null, false);
		else
			SFP_MW_FinalizeSummon();
	}
	
	void SFP_MW_FinalizeSummon()
	{
		m_SFP_SummonPending = false;
		if (!IsAlive())
			return;
		if (!SFP_MW_IsAggressive())
			return;
		
		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (!cfg)
			return;
		
		SFP_MW_BroadcastSoundEvent(SFP_MW_SOUND_SUMMON);
		
		PlayerBase focus = SFP_MW_GetAggroTargetPlayer(Math.Max(cfg.FlashRadius, cfg.HighgroundCheckRadius));
		if (!focus)
			focus = m_SFP_PendingSummonTarget;
		m_SFP_PendingSummonTarget = null;
		
		if (!focus || !focus.IsAlive())
		{
			m_SFP_PendingSummonCount = 0;
			return;
		}
		
		int aliveCount = SFP_MW_GetAliveSummonedWolfCount();
		int missingCount = cfg.WolvesPerSummon - aliveCount;
		if (m_SFP_PendingSummonCount > 0 && m_SFP_PendingSummonCount < missingCount)
			missingCount = m_SFP_PendingSummonCount;
		m_SFP_PendingSummonCount = 0;
		
		if (missingCount <= 0)
			return;
		
		for (int i = 0; i < missingCount; i++)
			SFP_MW_SpawnWolfNearPlayer(focus, cfg);
		
		if (GetGame().GetNoiseSystem() && m_SFP_WolfNoiseParams)
			GetGame().GetNoiseSystem().AddNoiseTarget(focus.GetPosition(), 10.0, m_SFP_WolfNoiseParams, 1.0);
	}
	
	protected void SFP_MW_SendSummonEffectToPlayer(PlayerBase player, SFP_MW_ConfigData cfg)
	{
		if (!player || !player.IsAlive() || !player.GetIdentity())
			return;
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(cfg.FlashDurationMultiplier);
		rpc.Write(cfg.FlashIntensity);
		rpc.Write(cfg.FlashSoundIntensity);
		rpc.Write(cfg.ShockBlur);
		rpc.Write(cfg.ShockVignette);
		rpc.Write(cfg.ShockOverlay);
		rpc.Write(cfg.ShakeStrength);
		rpc.Write(cfg.ShakeDuration);
		rpc.Send(player, SFP_MW_RPC_PLAY_SUMMON_EFFECT, true, player.GetIdentity());
		
		if (Math.RandomFloat01() <= Math.Clamp(cfg.DropChance, 0.0, 1.0))
		{
			EntityAI itemInHands = player.GetItemInHands();
			if (itemInHands && player.CanDropEntity(itemInHands) && !player.IsRestrained() && !player.IsSurrendered())
				player.ServerDropEntity(itemInHands);
		}
	}
	
	protected void SFP_MW_SendHighgroundEffectToPlayer(PlayerBase player, SFP_MW_ConfigData cfg)
	{
		if (!player || !player.IsAlive() || !player.GetIdentity())
			return;
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(cfg.HighgroundBlur);
		rpc.Write(cfg.HighgroundVignette);
		rpc.Write(cfg.HighgroundOverlay);
		rpc.Write(cfg.HighgroundShakeStrength);
		rpc.Write(cfg.HighgroundShakeDuration);
		rpc.Send(player, SFP_MW_RPC_PLAY_HIGHGROUND_EFFECT, true, player.GetIdentity());
	}
	
	protected void SFP_MW_SpawnWolfNearPlayer(PlayerBase focus, SFP_MW_ConfigData cfg)
	{
		if (!focus || !cfg || !cfg.WolfTypes || cfg.WolfTypes.Count() == 0)
			return;
		
		vector spawnPos;
		if (!SFP_MW_FindSpawnPositionBehindPlayer(focus, cfg, spawnPos))
		{
			SFP_MW_Log("Не удалось найти безопасную точку спавна волка для игрока " + focus.ToString());
			return;
		}
		
		string wolfType = cfg.WolfTypes.Get(Math.RandomInt(0, cfg.WolfTypes.Count()));
		EntityAI wolf = EntityAI.Cast(GetGame().CreateObjectEx(wolfType, spawnPos, ECE_PLACE_ON_SURFACE | ECE_INITAI));
		if (!wolf)
		{
			SFP_MW_Log("Не удалось создать волка типа: " + wolfType, true);
			return;
		}
		
		wolf.SetPosition(spawnPos);
		wolf.PlaceOnSurface();
		
		vector toPlayer = focus.GetPosition() - spawnPos;
		float yawToPlayer = Math.Atan2(toPlayer[0], toPlayer[2]) * Math.RAD2DEG;
		wolf.SetOrientation(Vector(yawToPlayer, 0, 0));
		
		Animal_CanisLupus wolfAI = Animal_CanisLupus.Cast(wolf);
		if (wolfAI)
		{
			wolfAI.SFP_MW_MarkMutantSummoned(true);
			wolfAI.SFP_MW_RestoreStandardHealth();
			wolfAI.SFP_MW_SetMutantTarget(focus);
			wolfAI.SFP_MW_RefreshAggro(focus);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_MW_FinalizeSpawnedWolf, 100, false, wolfAI, focus);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_MW_RefreshSpawnedWolfAggro, 900, false, wolfAI, focus);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_MW_RefreshSpawnedWolfAggro, 2200, false, wolfAI, focus);
			m_SFP_SummonedWolves.Insert(wolfAI);
		}
	}
	
	void SFP_MW_FinalizeSpawnedWolf(Animal_CanisLupus wolfAI, PlayerBase focus)
	{
		if (!wolfAI || !wolfAI.IsAlive())
			return;
		wolfAI.SFP_MW_MarkMutantSummoned(true);
		wolfAI.SFP_MW_RestoreStandardHealth();
		wolfAI.SFP_MW_SetMutantTarget(focus);
		wolfAI.SFP_MW_RefreshAggro(focus);
	}

	void SFP_MW_RefreshSpawnedWolfAggro(Animal_CanisLupus wolfAI, PlayerBase focus)
	{
		if (!wolfAI || !wolfAI.IsAlive())
			return;
		wolfAI.SFP_MW_SetMutantTarget(focus);
		wolfAI.SFP_MW_RefreshAggro(focus);
	}
	
	protected bool SFP_MW_FindSpawnPositionBehindPlayer(PlayerBase focus, SFP_MW_ConfigData cfg, out vector result)
	{
		vector playerPos = focus.GetPosition();
		float yawBase = focus.GetOrientation()[0];
		int attempts = cfg.SpawnSearchAttempts;
		if (attempts < 8)
			attempts = 8;
		if (attempts > 80)
			attempts = 80;
		
		vector fallback = playerPos - (focus.GetDirection() * cfg.SpawnMinDistance);
		fallback[1] = GetGame().SurfaceY(fallback[0], fallback[2]);
		
		for (int i = 0; i < attempts; i++)
		{
			float angle = yawBase + Math.RandomFloat(cfg.SpawnBackAngleMin, cfg.SpawnBackAngleMax);
			float dist = Math.RandomFloat(cfg.SpawnMinDistance, cfg.SpawnMaxDistance);
			vector dir = SFP_MW_DirectionFromYaw(angle);
			vector candidate = playerPos + (dir * dist);
			candidate[1] = GetGame().SurfaceY(candidate[0], candidate[2]);
			
			if (GetGame().SurfaceIsSea(candidate[0], candidate[2]))
				continue;
			if (vector.DistanceSq(candidate, playerPos) < cfg.SpawnMinDistance * cfg.SpawnMinDistance)
				continue;
			if (vector.DistanceSq(candidate, GetPosition()) < 4.0)
				continue;
			if (SFP_MW_IsSpawnVisibleToNearbyPlayers(candidate, cfg))
				continue;
			if (!SFP_MW_IsSafeWolfSpawnPosition(candidate, focus))
				continue;
			
			result = candidate;
			return true;
		}
		
		if (SFP_MW_IsSafeWolfSpawnPosition(fallback, focus))
		{
			result = fallback;
			return true;
		}
		
		return false;
	}
	
	protected bool SFP_MW_IsSafeWolfSpawnPosition(vector candidate, PlayerBase focus)
	{
		array<Object> excludeObjects = new array<Object>;
		excludeObjects.Insert(this);
		if (focus)
			excludeObjects.Insert(focus);
		
		array<Object> collidedObjects = new array<Object>;
		vector center = candidate;
		center[1] = center[1] + 0.95;
		vector orientation = Vector(0, 0, 0);
		vector extents = Vector(1.40, 2.10, 1.40);
		if (GetGame().IsBoxCollidingGeometry(center, orientation, extents, ObjIntersectView, ObjIntersectGeom, excludeObjects, collidedObjects))
			return false;
		
		vector from = center + Vector(0, 1.10, 0);
		vector to = center + Vector(0, 1.95, 0);
		vector contactPos;
		vector contactDir;
		int contactComp;
		if (DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComp, null, null, this, false, false, ObjIntersectGeom))
			return false;
		
		return true;
	}
	
	protected bool SFP_MW_IsSpawnVisibleToNearbyPlayers(vector candidate, SFP_MW_ConfigData cfg)
	{
		array<PlayerBase> players = SFP_MW_GetPlayersInRadius(candidate, cfg.FlashRadius);
		foreach (PlayerBase pb : players)
		{
			if (!pb || !pb.IsAlive())
				continue;
			if (!MiscGameplayFunctions.IsPlayerOrientedTowardPos(pb, candidate, 85))
				continue;
			if (SFP_MW_HasLineFromPlayerHeadToPos(pb, candidate))
				return true;
		}
		
		return false;
	}
	
	protected bool SFP_MW_HasLineFromPlayerHeadToPos(PlayerBase player, vector pos)
	{
		vector from;
		MiscGameplayFunctions.GetHeadBonePos(player, from);
		vector to = pos;
		to[1] = to[1] + 0.6;
		vector contactPos;
		vector contactDir;
		int contactComp;
		return !DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComp, null, null, player, false, false, ObjIntersectIFire);
	}
	
	protected vector SFP_MW_DirectionFromYaw(float yaw)
	{
		float yawRad = yaw * Math.DEG2RAD;
		return Vector(Math.Sin(yawRad), 0, Math.Cos(yawRad));
	}
	
	protected void SFP_MW_TryHighgroundReaction()
	{
		// Совместимость со старой структурой класса. Реальная логика вынесена в SFP_MW_ProcessHighgroundPulse().
	}

	protected void SFP_MW_ApplyHighgroundImpulseToPlayer(PlayerBase player, SFP_MW_ConfigData cfg)
	{
		// Physics only on a player standing/walking on his own (not swimming, on a ladder, in or on a vehicle, unconscious...)
		if (!SFP_Safety.CanPushPlayer(player))
			return;
		
		vector away = player.GetPosition() - GetPosition();
		away[1] = 0;
		if (away.LengthSq() <= 0.001)
		{
			float fallbackAngle = Math.RandomFloat(0.0, Math.PI2);
			away = Vector(Math.Cos(fallbackAngle), 0, Math.Sin(fallbackAngle));
		}
		else
		{
			away = away.Normalized();
		}
		
		float randomAngle = Math.RandomFloat(-1.35, 1.35);
		float sinA = Math.Sin(randomAngle);
		float cosA = Math.Cos(randomAngle);
		vector randomised = Vector((away[0] * cosA) - (away[2] * sinA), 0, (away[0] * sinA) + (away[2] * cosA));
		if (randomised.LengthSq() <= 0.001)
			randomised = away;
		randomised = randomised.Normalized();
		
		// Серверный импульс по рабочей схеме из донорской реализации.
		float forceMul = Math.Clamp(cfg.ThrowForceMultiplier, 0.10, 3.0);
		float horizontalForce = Math.Clamp(cfg.ThrowHorizontalVelocity, 1.0, 30.0) * forceMul;
		float verticalForce = Math.Clamp(cfg.ThrowVerticalVelocity, 0.0, 15.0) * forceMul;
		vector launchVel = randomised * horizontalForce;
		launchVel[1] = verticalForce;
		
		SetVelocity(player, launchVel);
		
		vector impulse = randomised * (horizontalForce * 20.0);
		impulse[1] = verticalForce * 26.0;
		dBodyApplyImpulse(player, impulse);
	}

	
	protected void SFP_MW_ApplyHitExtras(Object targetObject, vector hitPosWS, string damageAmmo)
	{
		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (!cfg)
			return;
		
		PlayerBase player = PlayerBase.Cast(targetObject);
		if (player)
		{
			player.AddHealth("", "Blood", -cfg.BloodDamagePerHit);
			if (Math.RandomFloat01() <= Math.Clamp(cfg.BleedChance, 0.0, 1.0))
				SFP_MW_ForceBleeding(player);
		}
		
		if (cfg.InventoryDamageMultiplier > 1.0)
		{
			EntityAI targetEntity = EntityAI.Cast(targetObject);
			if (targetEntity)
			{
				int extraHits = Math.Floor(cfg.InventoryDamageMultiplier - 1.0);
				float fractional = (cfg.InventoryDamageMultiplier - 1.0) - extraHits;
				for (int i = 0; i < extraHits; i++)
					DamageSystem.CloseCombatDamageName(this, targetObject, targetEntity.GetHitComponentForAI(), damageAmmo, hitPosWS, ProcessDirectDamageFlags.NO_GLOBAL_TRANSFER);
				if (fractional > 0 && Math.RandomFloat01() <= fractional)
					DamageSystem.CloseCombatDamageName(this, targetObject, targetEntity.GetHitComponentForAI(), damageAmmo, hitPosWS, ProcessDirectDamageFlags.NO_GLOBAL_TRANSFER);
			}
		}
	}
	
	protected void SFP_MW_ForceBleeding(PlayerBase player)
	{
		BleedingSourcesManagerServer bleeding = player.GetBleedingManagerServer();
		if (!bleeding)
			return;
		
		array<string> allSources = new array<string>;
		int i;
		for (i = 0; i < PlayerBase.m_BleedingSourcesUp.Count(); i++)
			allSources.Insert(PlayerBase.m_BleedingSourcesUp.Get(i));
		for (i = 0; i < PlayerBase.m_BleedingSourcesLow.Count(); i++)
			allSources.Insert(PlayerBase.m_BleedingSourcesLow.Get(i));
		allSources.Insert("Head");
		
		int startIndex = Math.RandomInt(0, allSources.Count());
		for (i = 0; i < allSources.Count(); i++)
		{
			string selection = allSources.Get((startIndex + i) % allSources.Count());
			if (bleeding.AttemptAddBleedingSourceBySelection(selection))
				return;
		}
	}
	
	protected PlayerBase SFP_MW_GetAggroTargetPlayer(float radius)
	{
		DayZInfectedInputController ic = GetInputController();
		if (!ic)
			return null;
		
		int mindState = ic.GetMindState();
		if (mindState != DayZInfectedConstants.MINDSTATE_CHASE && mindState != DayZInfectedConstants.MINDSTATE_FIGHT)
			return null;
		
		PlayerBase targetPlayer = PlayerBase.Cast(ic.GetTargetEntity());
		if (targetPlayer && targetPlayer.IsAlive() && vector.DistanceSq(targetPlayer.GetPosition(), GetPosition()) <= radius * radius)
			return targetPlayer;
		
		array<PlayerBase> players = SFP_MW_GetPlayersInRadius(GetPosition(), radius);
		PlayerBase nearest;
		float nearestDist = float.MAX;
		foreach (PlayerBase pb : players)
		{
			float dist = vector.DistanceSq(pb.GetPosition(), GetPosition());
			if (dist < nearestDist)
			{
				nearestDist = dist;
				nearest = pb;
			}
		}
		
		return nearest;
	}
	
	protected void SFP_MW_CleanupSummonedWolves()
	{
		if (!m_SFP_SummonedWolves)
			return;
		
		for (int i = m_SFP_SummonedWolves.Count() - 1; i >= 0; i--)
		{
			Animal_CanisLupus wolf = m_SFP_SummonedWolves.Get(i);
			if (!wolf || !wolf.IsAlive() || !wolf.SFP_MW_IsMutantSummoned())
				m_SFP_SummonedWolves.Remove(i);
		}
	}
	
	protected int SFP_MW_GetAliveSummonedWolfCount()
	{
		SFP_MW_CleanupSummonedWolves();
		return m_SFP_SummonedWolves.Count();
	}
	
	protected array<PlayerBase> SFP_MW_GetPlayersInRadius(vector center, float radius)
	{
		array<PlayerBase> result = new array<PlayerBase>;
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		float radiusSq = radius * radius;
		
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.IsAlive())
				continue;
			if (vector.DistanceSq(pb.GetPosition(), center) <= radiusSq)
				result.Insert(pb);
		}
		return result;
	}
};
