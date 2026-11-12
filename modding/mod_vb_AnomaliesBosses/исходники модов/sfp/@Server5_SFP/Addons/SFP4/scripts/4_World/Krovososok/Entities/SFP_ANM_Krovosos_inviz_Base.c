class SFP_ANM_Krovosos_inviz_Base extends ZombieMaleBase
{
	protected static const string SFP_TRANSPARENT_TEXTURE = "#(argb,8,8,3)color(1,1,1,0,CO)";
	
	protected int m_SFP_ImpulsePulse;
	protected EffectSound m_SFP_ImpulseSound;
	protected int m_SFP_LastImpulsePulse;
	protected bool m_SFP_ClientImpulseSyncInit;
	
	protected bool m_SFP_IsInvisible;
	protected int m_SFP_AggroPulse;
	
	protected ref Timer m_SFP_ReInvisTimer;
	protected int m_SFP_LastHitTimeMS;
	protected int m_SFP_HitCountWindow;
	protected float m_SFP_DamageWindow;
	protected int m_SFP_NextElevatedPulseAllowedMs;
	protected ref Timer m_SFP_ElevatedPulseCheckTimer;
	
	protected bool m_SFP_ClientAppliedInvisible;
	protected ref array<string> m_SFP_OriginalTextures;
	protected ref array<SFP_KrovososHeatLight> m_SFP_HeatHazeLights;
	protected EffectSound m_SFP_TransitionSound;
	
	void SFP_ANM_Krovosos_inviz_Base()
	{
		RegisterNetSyncVariableBool("m_SFP_IsInvisible");
		RegisterNetSyncVariableInt("m_SFP_AggroPulse", 0, 255);
		RegisterNetSyncVariableInt("m_SFP_ImpulsePulse", 0, 255);
		
		m_SFP_OriginalTextures = new array<string>();
		m_SFP_HeatHazeLights = new array<SFP_KrovososHeatLight>();
		TStringArray textures = GetHiddenSelectionsTextures();
		for (int i = 0; i < textures.Count(); i++)
			m_SFP_OriginalTextures.Insert(textures[i]);
	}
	
	void ~SFP_ANM_Krovosos_inviz_Base()
	{
		SFP_StopAllClientEffects();
		if (m_SFP_ReInvisTimer)
			m_SFP_ReInvisTimer.Stop();
		if (m_SFP_ElevatedPulseCheckTimer)
			m_SFP_ElevatedPulseCheckTimer.Stop();
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);
		
		if (GetGame().IsServer())
		{
			SFP_ApplyConfiguredSpawnHealth();
			SFP_ScheduleReInvisibility(1.0);
			SFP_StartElevatedPulseCheck();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (!GetGame().IsDedicatedServer())
		{
			if (m_SFP_ClientAppliedInvisible != m_SFP_IsInvisible)
			{
				m_SFP_ClientAppliedInvisible = m_SFP_IsInvisible;
				if (m_SFP_IsInvisible)
					SFP_OnInvisibleStartClient();
				else
					SFP_OnInvisibleEndClient(IsAlive());
			}

			if (!m_SFP_ClientImpulseSyncInit)
			{
				m_SFP_ClientImpulseSyncInit = true;
				m_SFP_LastImpulsePulse = m_SFP_ImpulsePulse;
			}
			else if (m_SFP_LastImpulsePulse != m_SFP_ImpulsePulse)
			{
				m_SFP_LastImpulsePulse = m_SFP_ImpulsePulse;
				SFP_PlayImpulseSoundClient();
			}
		}
	}
	
	override bool HandleMindStateChange(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		int oldMindState = m_LastMindState;
		bool ret = super.HandleMindStateChange(pCurrentCommandID, pInputController, pDt);
		
		if (GetGame().IsServer())
		{
			bool oldAggro = (oldMindState == DayZInfectedConstants.MINDSTATE_CHASE || oldMindState == DayZInfectedConstants.MINDSTATE_FIGHT);
			bool newAggro = (m_MindState == DayZInfectedConstants.MINDSTATE_CHASE || m_MindState == DayZInfectedConstants.MINDSTATE_FIGHT);
			if (newAggro && !oldAggro)
			{
				m_SFP_AggroPulse++;
				if (m_SFP_AggroPulse > 255)
					m_SFP_AggroPulse = 1;
				SetSynchDirty();
			}
		}
		
		return ret;
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (!GetGame().IsServer() || !IsAlive())
			return;
		
		SFP_ProcessInvisibleBreak(damageResult, dmgZone);
	}
	
	override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		return super.FightLogic(pCurrentCommandID, pInputController, pDt);
	}
	
	override void EEKilled(Object killer)
	{
		if (GetGame().IsServer())
		{
			if (m_SFP_ReInvisTimer)
				m_SFP_ReInvisTimer.Stop();
			if (m_SFP_ElevatedPulseCheckTimer)
				m_SFP_ElevatedPulseCheckTimer.Stop();
			m_SFP_IsInvisible = false;
			SetSynchDirty();
		}
		
		SFP_StopAllClientEffects();
		SFP_RestoreVisibleTextures();
		m_SFP_ClientAppliedInvisible = false;
		
		super.EEKilled(killer);
	}
	
	override bool CanBeSkinned()
	{
		return true;
	}
	
	bool SFP_IsInvisibleState()
	{
		return m_SFP_IsInvisible;
	}
	
	int SFP_GetAggroPulse()
	{
		return m_SFP_AggroPulse;
	}
	
	protected void SFP_ApplyConfiguredSpawnHealth()
	{
		SFP_KrovososConfigData cfg = SFP_KrovososConfig.Get();
		float maxHealth = GetMaxHealth("", "Health");
		float healthToSet = Math.Clamp(cfg.SpawnHealth, 1.0, maxHealth);
		SetHealth("", "Health", healthToSet);
	}
	
	protected void SFP_ProcessInvisibleBreak(TotalDamageResult damageResult, string dmgZone)
	{
		SFP_KrovososConfigData cfg = SFP_KrovososConfig.Get();
		int nowMs = GetGame().GetTime();
		int windowMs = Math.Round(cfg.ContinuousHitWindowSeconds * 1000.0);
		
		if ((nowMs - m_SFP_LastHitTimeMS) > windowMs)
		{
			m_SFP_HitCountWindow = 0;
			m_SFP_DamageWindow = 0.0;
		}
		
		m_SFP_LastHitTimeMS = nowMs;
		m_SFP_HitCountWindow++;
		m_SFP_DamageWindow += SFP_GetHealthDamageFromResult(damageResult, dmgZone);
		
		if (m_SFP_IsInvisible)
		{
			if (m_SFP_HitCountWindow >= cfg.HitsToBreakInvisibility || m_SFP_DamageWindow >= cfg.DamageToBreakInvisibility)
			{
				SFP_SetInvisibleServer(false);
				SFP_ScheduleReInvisibility(Math.RandomFloat(cfg.InvisibleReturnMinSeconds, cfg.InvisibleReturnMaxSeconds));
				m_SFP_HitCountWindow = 0;
				m_SFP_DamageWindow = 0.0;
			}
		}
		else
		{
			SFP_ScheduleReInvisibility(Math.RandomFloat(cfg.InvisibleReturnMinSeconds, cfg.InvisibleReturnMaxSeconds));
		}
	}
	
	protected float SFP_GetHealthDamageFromResult(TotalDamageResult damageResult, string dmgZone)
	{
		float damage = 0.0;
		if (dmgZone != string.Empty)
			damage = damageResult.GetDamage(dmgZone, "Health");
		
		if (damage <= 0.0)
			damage = damageResult.GetDamage("", "Health");
		
		return damage;
	}
	
	
	protected void SFP_StartElevatedPulseCheck()
	{
		if (!GetGame().IsServer())
			return;
		
		if (!m_SFP_ElevatedPulseCheckTimer)
			m_SFP_ElevatedPulseCheckTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		
		m_SFP_ElevatedPulseCheckTimer.Stop();
		float checkInterval = Math.Clamp(SFP_KrovososConfig.Get().ElevatedPulseCheckIntervalSeconds, 0.10, 2.0);
		m_SFP_ElevatedPulseCheckTimer.Run(checkInterval, this, "SFP_ProcessElevatedTargetPulse", null, true);
	}
	
	protected void SFP_ProcessElevatedTargetPulse()
	{
		if (!GetGame().IsServer() || !IsAlive())
			return;
		
		DayZInfectedInputController pInputController = GetInputController();
		if (!pInputController)
			return;
		
		int mindState = pInputController.GetMindState();
		if (mindState != DayZInfectedConstants.MINDSTATE_CHASE && mindState != DayZInfectedConstants.MINDSTATE_FIGHT)
			return;
		
		EntityAI target = pInputController.GetTargetEntity();
		if (!target)
			return;
		
		PlayerBase playerTarget = PlayerBase.Cast(target);
		if (!playerTarget || !playerTarget.IsAlive() || playerTarget.GetCommand_Vehicle())
			return;
		
		SFP_KrovososConfigData cfg = SFP_KrovososConfig.Get();
		vector targetPos = playerTarget.GetPosition();
		vector myPos = GetPosition();
		float horizontalDist = vector.Distance(Vector(targetPos[0], 0, targetPos[2]), Vector(myPos[0], 0, myPos[2]));
		float heightDelta = targetPos[1] - myPos[1];
		
		if (horizontalDist > cfg.ElevatedPulseTriggerDistance)
			return;
		
		if (heightDelta < cfg.ElevatedPulseMinHeightDelta)
			return;
		
		bool unreachable = !CanAttackToPosition(targetPos);
		if (!unreachable && heightDelta < (cfg.ElevatedPulseMinHeightDelta + 0.35))
			return;
		
		int nowMs = GetGame().GetTime();
		if (nowMs < m_SFP_NextElevatedPulseAllowedMs)
			return;
		
		SFP_TriggerElevatedPulseServer(playerTarget);
		float cooldownSeconds = cfg.ElevatedPulseReuseCooldownSeconds;
		if (cooldownSeconds <= 0.0)
			cooldownSeconds = Math.RandomFloat(cfg.ElevatedPulseCooldownMinSeconds, cfg.ElevatedPulseCooldownMaxSeconds);

		m_SFP_NextElevatedPulseAllowedMs = nowMs + Math.Round(Math.Max(0.1, cooldownSeconds) * 1000.0);
	}
	
	protected void SFP_TriggerElevatedPulseServer(PlayerBase centerTarget)
	{
		if (!centerTarget)
			return;
		
		SFP_TriggerAggroPulseServer();
		m_SFP_ImpulsePulse++;
		if (m_SFP_ImpulsePulse > 255)
			m_SFP_ImpulsePulse = 1;

		SetSynchDirty();
		
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		SFP_KrovososConfigData cfg = SFP_KrovososConfig.Get();
		vector centerPos = centerTarget.GetPosition();
		
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.IsAlive() || pb.GetCommand_Vehicle())
				continue;
			
			if (vector.Distance(pb.GetPosition(), centerPos) > cfg.ElevatedPulseAffectRadius)
				continue;
			
			SFP_ApplyElevatedImpulseToPlayer(pb, cfg);
		}
	}
	
	protected void SFP_ApplyElevatedImpulseToPlayer(PlayerBase player, SFP_KrovososConfigData cfg)
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
		
		float forceMul = Math.Max(cfg.ElevatedPulseForceMultiplier, 0.05);
		float horizontalForce = Math.Max(cfg.ElevatedPulseHorizontalVelocity, 8.0) * forceMul;
		float verticalForce = Math.Max(cfg.ElevatedPulseVerticalVelocity, 4.0) * forceMul;
		vector launchVel = randomised * horizontalForce;
		launchVel[1] = verticalForce;
		
		SetVelocity(player, launchVel);
		
		vector impulse = randomised * (horizontalForce * 38.0);
		impulse[1] = verticalForce * 55.0;
		dBodyApplyImpulse(player, impulse);
	}
	protected void SFP_PlayImpulseSoundClient()
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		PlaySoundSet(m_SFP_ImpulseSound, "SFP_Krovosos_impulse_Soundset", 0.0, 0.0);
	}
	protected void SFP_TriggerAggroPulseServer()
	{
		m_SFP_AggroPulse++;
		if (m_SFP_AggroPulse > 255)
			m_SFP_AggroPulse = 1;
		
		DayZInfectedCommandMove moveCommand = GetCommand_Move();
		if (moveCommand && !moveCommand.IsTurning())
			moveCommand.SetIdleState(2);
		
		SetSynchDirty();
	}
	
	protected void SFP_ScheduleReInvisibility(float delaySeconds)
	{
		if (!GetGame().IsServer())
			return;
		
		if (!m_SFP_ReInvisTimer)
			m_SFP_ReInvisTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		
		m_SFP_ReInvisTimer.Stop();
		m_SFP_ReInvisTimer.Run(Math.Max(0.05, delaySeconds), this, "SFP_EnterInvisible");
	}
	
	void SFP_EnterInvisible()
	{
		if (!GetGame().IsServer() || !IsAlive())
			return;
		
		SFP_SetInvisibleServer(true);
	}
	
	protected void SFP_SetInvisibleServer(bool state)
	{
		if (m_SFP_IsInvisible == state)
			return;
		
		m_SFP_IsInvisible = state;
		SetSynchDirty();
	}
	
	protected void SFP_OnInvisibleStartClient()
	{
		SFP_ApplyInvisibleTextures();
		SFP_StartHeatHaze();
		SFP_PlayTransitionSound("SFP_Krovosos_inviz_start_Soundset");
		ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_INVZ_START, this, "0 1.0 0", vector.Zero, true);
	}
	
	protected void SFP_OnInvisibleEndClient(bool playExitSound)
	{
		SFP_RestoreVisibleTextures();
		SFP_StopHeatHaze();
		if (playExitSound)
			SFP_PlayTransitionSound("SFP_Krovosos_inviz_exit_Soundset");
	}
	
	protected void SFP_PlayTransitionSound(string soundSet)
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		PlaySoundSet(m_SFP_TransitionSound, soundSet, 0.0, 0.0);
	}
	
	protected void SFP_StartHeatHaze()
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		if (m_SFP_HeatHazeLights && m_SFP_HeatHazeLights.Count() > 0)
			return;
		
		SFP_CreateHeatHazeAtOffset("0 0.80 0");
		SFP_CreateHeatHazeAtOffset("0 1.04 0");
		SFP_CreateHeatHazeAtOffset("0 1.28 0");
		SFP_CreateHeatHazeAtOffset("0.11 1.04 0");
		SFP_CreateHeatHazeAtOffset("-0.11 1.04 0");
	}
	
	protected void SFP_CreateHeatHazeAtOffset(vector offset)
	{
		SFP_KrovososHeatLight haze = SFP_KrovososHeatLight.Cast(ScriptedLightBase.CreateLight(SFP_KrovososHeatLight, Vector(0,0,0)));
		if (!haze)
			return;
		
		haze.AttachOnObject(this, offset);
		m_SFP_HeatHazeLights.Insert(haze);
	}
	
	protected void SFP_StopHeatHaze()
	{
		if (!m_SFP_HeatHazeLights)
			return;
		
		for (int i = m_SFP_HeatHazeLights.Count() - 1; i >= 0; i--)
		{
			if (m_SFP_HeatHazeLights[i])
				m_SFP_HeatHazeLights[i].Destroy();
		}
		
		m_SFP_HeatHazeLights.Clear();
	}
	
	protected void SFP_ApplyInvisibleTextures()
	{
		int count = m_SFP_OriginalTextures.Count();
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
				SetObjectTexture(i, SFP_TRANSPARENT_TEXTURE);
		}
		else
		{
			int camo = GetHiddenSelectionIndex("camo");
			if (camo > -1)
				SetObjectTexture(camo, SFP_TRANSPARENT_TEXTURE);
		}
	}
	
	protected void SFP_RestoreVisibleTextures()
	{
		int count = m_SFP_OriginalTextures.Count();
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
				SetObjectTexture(i, m_SFP_OriginalTextures[i]);
		}
	}
	
	protected void SFP_StopAllClientEffects()
	{
		SFP_StopHeatHaze();
		
		if (m_SFP_TransitionSound && m_SFP_TransitionSound.IsPlaying())
			m_SFP_TransitionSound.SoundStop();
		if (m_SFP_ImpulseSound && m_SFP_ImpulseSound.IsPlaying())
			m_SFP_ImpulseSound.SoundStop();
	}
};
// Наследники мутанта
class SFP_ANM_Krovosos_inviz extends SFP_ANM_Krovosos_inviz_Base {};
class SFP_ANM_Krovosos_Boloto_inviz extends SFP_ANM_Krovosos_inviz_Base {};
class SFP_ANM_Krovosos_Dark_inviz extends SFP_ANM_Krovosos_inviz_Base {};

class SFP_Piyavka_invz_base extends SFP_ANM_Krovosos_inviz_Base {};
class SFP_Piyavka_invz extends SFP_ANM_Krovosos_inviz_Base {};

class SFP_Shipastiy_invz_Base extends SFP_ANM_Krovosos_inviz_Base {};
class SFP_Shipastiy_invz extends SFP_ANM_Krovosos_inviz_Base {};