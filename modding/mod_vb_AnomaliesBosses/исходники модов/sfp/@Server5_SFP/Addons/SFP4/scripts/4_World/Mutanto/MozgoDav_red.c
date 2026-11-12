class SFP_MozgoTras_red extends ZombieMaleBase
{
	// ---------------- Параметры (JSON, синхронизация на клиентов) ----------------
	protected float m_SFP_AuraCalmRadius;
	protected float m_SFP_AuraAggroRadius;
	protected float m_SFP_DamagePerSecond;
	protected float m_SFP_StaminaDT;
	protected float m_SFP_VomitChance;
	protected float m_SFP_DropItemChance;
	protected float m_SFP_SuicideHPThreshold;
	protected float m_SFP_SuicideChance;
	protected float m_SFP_TinnitusTimeMin;
	protected float m_SFP_TinnitusTimeMax;

	// Calm visuals
	protected float m_SFP_CalmColorR;
	protected float m_SFP_CalmColorG;
	protected float m_SFP_CalmColorB;
	protected float m_SFP_CalmBlur;
	protected float m_SFP_CalmBloodSaturation;
	protected float m_SFP_CalmShockVignette;
	protected float m_SFP_CalmTunnelVignette;
	protected float m_SFP_CalmChromAbb;
	protected float m_SFP_CalmCameraShakeInterval;
	protected float m_SFP_CalmCameraShakeStrength;

	// Aggro visuals
	protected float m_SFP_AggroColorR;
	protected float m_SFP_AggroColorG;
	protected float m_SFP_AggroColorB;
	protected float m_SFP_AggroBlurMin;
	protected float m_SFP_AggroBlurMax;
	protected float m_SFP_AggroSaturationMin;
	protected float m_SFP_AggroSaturationMax;
	protected float m_SFP_AggroShockVignetteCoef;
	protected float m_SFP_AggroTunnelVignetteCoef;
	protected float m_SFP_AggroChromAbbCoef;
	protected float m_SFP_AggroLensDistortCoef;
	protected float m_SFP_AggroCameraShakeInterval;
	protected float m_SFP_AggroCameraShakeStrength;
	protected float m_SFP_AggroLensParam3;
	protected float m_SFP_AggroLensParam4;

	// Tinnitus
	protected float m_SFP_TinnitusEffectChance;
	protected float m_SFP_FlashbangProgressMult;
	protected float m_SFP_FlashbangVisualMax;
	protected float m_SFP_FlashbangSoundMax;

	// Sound (synced as indices)
	protected int   m_SFP_CalmSoundSetId;
	protected int   m_SFP_AggroSoundSetId;
	protected float m_SFP_LoopSoundVolume;
	protected float m_SFP_LoopSoundPitch;

	// ---------------- Внутренние поля ----------------
	protected float m_PsiServerTimer;
	protected float m_PsiClientTimer;
	protected float m_NextTinnitusTime;

	protected bool  m_CalmEffectActive;
	protected bool  m_AggroEffectActive;

	protected ref EffectSound m_PsiLoopSound;

	void SFP_MozgoTras_red()
	{
		// Net sync registration
		RegisterNetSyncVariableFloat("m_SFP_AuraCalmRadius");
		RegisterNetSyncVariableFloat("m_SFP_AuraAggroRadius");
		RegisterNetSyncVariableFloat("m_SFP_DamagePerSecond");
		RegisterNetSyncVariableFloat("m_SFP_StaminaDT");
		RegisterNetSyncVariableFloat("m_SFP_VomitChance");
		RegisterNetSyncVariableFloat("m_SFP_DropItemChance");
		RegisterNetSyncVariableFloat("m_SFP_SuicideHPThreshold");
		RegisterNetSyncVariableFloat("m_SFP_SuicideChance");
		RegisterNetSyncVariableFloat("m_SFP_TinnitusTimeMin");
		RegisterNetSyncVariableFloat("m_SFP_TinnitusTimeMax");

		RegisterNetSyncVariableFloat("m_SFP_CalmColorR");
		RegisterNetSyncVariableFloat("m_SFP_CalmColorG");
		RegisterNetSyncVariableFloat("m_SFP_CalmColorB");
		RegisterNetSyncVariableFloat("m_SFP_CalmBlur");
		RegisterNetSyncVariableFloat("m_SFP_CalmBloodSaturation");
		RegisterNetSyncVariableFloat("m_SFP_CalmShockVignette");
		RegisterNetSyncVariableFloat("m_SFP_CalmTunnelVignette");
		RegisterNetSyncVariableFloat("m_SFP_CalmChromAbb");
		RegisterNetSyncVariableFloat("m_SFP_CalmCameraShakeInterval");
		RegisterNetSyncVariableFloat("m_SFP_CalmCameraShakeStrength");

		RegisterNetSyncVariableFloat("m_SFP_AggroColorR");
		RegisterNetSyncVariableFloat("m_SFP_AggroColorG");
		RegisterNetSyncVariableFloat("m_SFP_AggroColorB");
		RegisterNetSyncVariableFloat("m_SFP_AggroBlurMin");
		RegisterNetSyncVariableFloat("m_SFP_AggroBlurMax");
		RegisterNetSyncVariableFloat("m_SFP_AggroSaturationMin");
		RegisterNetSyncVariableFloat("m_SFP_AggroSaturationMax");
		RegisterNetSyncVariableFloat("m_SFP_AggroShockVignetteCoef");
		RegisterNetSyncVariableFloat("m_SFP_AggroTunnelVignetteCoef");
		RegisterNetSyncVariableFloat("m_SFP_AggroChromAbbCoef");
		RegisterNetSyncVariableFloat("m_SFP_AggroLensDistortCoef");
		RegisterNetSyncVariableFloat("m_SFP_AggroCameraShakeInterval");
		RegisterNetSyncVariableFloat("m_SFP_AggroCameraShakeStrength");
		RegisterNetSyncVariableFloat("m_SFP_AggroLensParam3");
		RegisterNetSyncVariableFloat("m_SFP_AggroLensParam4");

		RegisterNetSyncVariableFloat("m_SFP_TinnitusEffectChance");
		RegisterNetSyncVariableFloat("m_SFP_FlashbangProgressMult");
		RegisterNetSyncVariableFloat("m_SFP_FlashbangVisualMax");
		RegisterNetSyncVariableFloat("m_SFP_FlashbangSoundMax");

		RegisterNetSyncVariableInt("m_SFP_CalmSoundSetId", 0, 100);
		RegisterNetSyncVariableInt("m_SFP_AggroSoundSetId", 0, 100);
		RegisterNetSyncVariableFloat("m_SFP_LoopSoundVolume");
		RegisterNetSyncVariableFloat("m_SFP_LoopSoundPitch");

		// Default values (old hard-coded for red)
		SFP_ApplyDefaults();

		m_PsiServerTimer   = 0;
		m_PsiClientTimer   = 0;
		m_NextTinnitusTime = 0;

		m_CalmEffectActive  = false;
		m_AggroEffectActive = false;
	}

	protected void SFP_ApplyDefaults()
	{
		m_SFP_AuraCalmRadius  = 90.0;
		m_SFP_AuraAggroRadius = 70.0;
		m_SFP_DamagePerSecond = 3.0;
		m_SFP_StaminaDT       = 0.3;
		m_SFP_VomitChance     = 0.15;
		m_SFP_DropItemChance  = 0.25;
		m_SFP_SuicideHPThreshold = 50.0;
		m_SFP_SuicideChance      = 0.70;
		m_SFP_TinnitusTimeMin = 8.0;
		m_SFP_TinnitusTimeMax = 15.0;

		m_SFP_CalmColorR = 1.0;
		m_SFP_CalmColorG = 1.0;
		m_SFP_CalmColorB = 1.0;
		m_SFP_CalmBlur = 0.0;
		m_SFP_CalmBloodSaturation = 0.9;
		m_SFP_CalmShockVignette   = 0.15;
		m_SFP_CalmTunnelVignette  = 0.0;
		m_SFP_CalmChromAbb        = 0.0;
		m_SFP_CalmCameraShakeInterval = 1.0;
		m_SFP_CalmCameraShakeStrength = 1.2;

		m_SFP_AggroColorR = 0.60;
		m_SFP_AggroColorG = 0.80;
		m_SFP_AggroColorB = 1.00;
		m_SFP_AggroBlurMin = 0.15;
		m_SFP_AggroBlurMax = 0.85;
		m_SFP_AggroSaturationMin = 0.05;
		m_SFP_AggroSaturationMax = 1.0;
		m_SFP_AggroShockVignetteCoef  = 0.6;
		m_SFP_AggroTunnelVignetteCoef = 0.8;
		m_SFP_AggroChromAbbCoef       = 0.4;
		m_SFP_AggroLensDistortCoef    = 0.4;
		m_SFP_AggroCameraShakeInterval = 0.6;
		m_SFP_AggroCameraShakeStrength = 4.5;
			// Defaults must not depend on JSON (cfg is not available here)
			m_SFP_AggroLensParam3 = 0.0;
			m_SFP_AggroLensParam4 = 0.0;

		m_SFP_TinnitusEffectChance = 0.5;
		m_SFP_FlashbangProgressMult = 0.5;
		m_SFP_FlashbangVisualMax    = 0.15;
		m_SFP_FlashbangSoundMax     = 0.5;

		m_SFP_CalmSoundSetId = SFP_MozgoTrasSoundSetRegistry.GetId("SFP_MozgoTras_PsiAura_Soundset");
		m_SFP_AggroSoundSetId = SFP_MozgoTrasSoundSetRegistry.GetId("SFP_MozgoTras_PsiAuraGuaa_Soundset");
		m_SFP_LoopSoundVolume = 0.1;
		m_SFP_LoopSoundPitch  = 0.2;
	}

	override void EEInit()
	{
		super.EEInit();

		if (!GetGame() || !GetGame().IsServer())
			return;

		ref SFP_MozgoTrasConfig cfg = SFP_MozgoTrasConfigManager.Get(GetType());
		SFP_ApplyConfig(cfg);
		SetSynchDirty();

		if (cfg)
		{
			if (cfg.MutantStartHealth >= 0.0)
			{
				SetHealth("", "Health", cfg.MutantStartHealth);
			}
			else
			{
				float coef = Math.Clamp(cfg.MutantStartHealth01, 0.0, 1.0);
				SetHealth01("", "Health", coef);
			}
		}
	}

	protected void SFP_ApplyConfig(SFP_MozgoTrasConfig cfg)
	{
		if (!cfg)
			return;

		m_SFP_AuraCalmRadius  = cfg.AuraCalmRadius;
		m_SFP_AuraAggroRadius = cfg.AuraAggroRadius;
		m_SFP_DamagePerSecond = cfg.DamagePerSecond;
		m_SFP_StaminaDT       = cfg.StaminaDT;
		m_SFP_VomitChance     = cfg.VomitChance;
		m_SFP_DropItemChance  = cfg.DropItemChance;
		m_SFP_SuicideHPThreshold = cfg.SuicideHPThreshold;
		m_SFP_SuicideChance      = cfg.SuicideChance;
		m_SFP_TinnitusTimeMin = cfg.TinnitusTimeMin;
		m_SFP_TinnitusTimeMax = cfg.TinnitusTimeMax;

		m_SFP_CalmColorR = cfg.CalmColorR;
		m_SFP_CalmColorG = cfg.CalmColorG;
		m_SFP_CalmColorB = cfg.CalmColorB;
		m_SFP_CalmBlur = cfg.CalmBlur;
		m_SFP_CalmBloodSaturation = cfg.CalmBloodSaturation;
		m_SFP_CalmShockVignette   = cfg.CalmShockVignette;
		m_SFP_CalmTunnelVignette  = cfg.CalmTunnelVignette;
		m_SFP_CalmChromAbb        = cfg.CalmChromAbb;
		m_SFP_CalmCameraShakeInterval = cfg.CalmCameraShakeInterval;
		m_SFP_CalmCameraShakeStrength = cfg.CalmCameraShakeStrength;

		m_SFP_AggroColorR = cfg.AggroColorR;
		m_SFP_AggroColorG = cfg.AggroColorG;
		m_SFP_AggroColorB = cfg.AggroColorB;
		m_SFP_AggroBlurMin = cfg.AggroBlurMin;
		m_SFP_AggroBlurMax = cfg.AggroBlurMax;
		m_SFP_AggroSaturationMin = cfg.AggroSaturationMin;
		m_SFP_AggroSaturationMax = cfg.AggroSaturationMax;
		m_SFP_AggroShockVignetteCoef  = cfg.AggroShockVignetteCoef;
		m_SFP_AggroTunnelVignetteCoef = cfg.AggroTunnelVignetteCoef;
		m_SFP_AggroChromAbbCoef       = cfg.AggroChromAbbCoef;
		m_SFP_AggroLensDistortCoef    = cfg.AggroLensDistortCoef;
		m_SFP_AggroCameraShakeInterval = cfg.AggroCameraShakeInterval;
		m_SFP_AggroCameraShakeStrength = cfg.AggroCameraShakeStrength;
			m_SFP_AggroLensParam3 = cfg.AggroLensParam3;
			m_SFP_AggroLensParam4 = cfg.AggroLensParam4;

		m_SFP_TinnitusEffectChance = cfg.TinnitusEffectChance;
		m_SFP_FlashbangProgressMult = cfg.FlashbangProgressMult;
		m_SFP_FlashbangVisualMax    = cfg.FlashbangVisualMax;
		m_SFP_FlashbangSoundMax     = cfg.FlashbangSoundMax;

		m_SFP_CalmSoundSetId  = SFP_MozgoTrasSoundSetRegistry.GetId(cfg.CalmSoundSet);
		m_SFP_AggroSoundSetId = SFP_MozgoTrasSoundSetRegistry.GetId(cfg.AggroSoundSet);
		m_SFP_LoopSoundVolume = cfg.LoopSoundVolume;
		m_SFP_LoopSoundPitch  = cfg.LoopSoundPitch;
	}

	bool IsPsiAggressive()
	{
		int mind = GetMindStateSynced();
		if (mind == DayZInfectedConstants.MINDSTATE_CHASE || mind == DayZInfectedConstants.MINDSTATE_FIGHT)
			return true;
		return false;
	}

	override void EEDelete(EntityAI parent)
	{
		StopPsiLoopSound();
		ResetClientVisuals();
		super.EEDelete(parent);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		StopPsiLoopSound();
		ResetClientVisuals();
		m_CalmEffectActive  = false;
		m_AggroEffectActive = false;
	}

	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);
		if (!GetGame())
			return;
		if (GetGame().IsServer())
		{
			ProcessPsiAuraServer(timeSlice);
		}
		if (!GetGame().IsDedicatedServer())
		{
			ProcessPsiAuraClient(timeSlice);
		}
	}

	protected void ProcessPsiAuraServer(float dt)
	{
		m_PsiServerTimer += dt;
		if (m_PsiServerTimer < 1.0)
			return;
		m_PsiServerTimer = 0;

		bool aggressive = IsPsiAggressive();
		float radius = m_SFP_AuraCalmRadius;
		if (aggressive)
			radius = m_SFP_AuraAggroRadius;

		float radiusSq  = radius * radius;

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player;
			if (!Class.CastTo(player, players[i]))
				continue;
			if (!player || !player.IsAlive())
				continue;
			float distSq = vector.DistanceSq(player.GetPosition(), GetPosition());
			if (distSq > radiusSq)
				continue;

			if (aggressive)
				ApplyPsiAggressiveServer(player);
			else
				ApplyPsiCalmServer(player);
		}
	}

	protected void ApplyPsiCalmServer(PlayerBase player)
	{
	}

	protected void ApplyPsiAggressiveServer(PlayerBase player)
	{
		player.DecreaseHealth("", "Health", m_SFP_DamagePerSecond);
		StaminaHandler stam = player.GetStaminaHandler();
		if (stam)
			// DepleteStamina() is deprecated in vanilla; use DepleteStaminaEx() to avoid warnings
			stam.DepleteStaminaEx(EStaminaModifiers.OVERALL_DRAIN, m_SFP_StaminaDT);
		if (Math.RandomFloat01() <= m_SFP_VomitChance)
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
		if (Math.RandomFloat01() <= m_SFP_DropItemChance)
		{
			ItemBase itemInHands;
			if (Class.CastTo(itemInHands, player.GetItemInHands()))
				player.ServerDropEntity(itemInHands);
		}
	}

	protected void TryForceSuicide(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return;
		if (GetGame().IsDedicatedServer())
			return;
		if (player.IsUnconscious() || player.IsRestrained())
			return;
		ItemBase weapon;
		if (!Class.CastTo(weapon, player.GetItemInHands()))
			return;
		if (!weapon.ConfigIsExisting("suicideAnim"))
			return;
		EmoteManager em = player.GetEmoteManager();
		if (!em)
			return;
		if (em.IsEmotePlaying())
			return;
		if (!em.CanPlayEmote(EmoteConstants.ID_EMOTE_SUICIDE))
			return;
		em.CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SUICIDE, true);
		EmoteLauncher launcher = em.GetEmoteLauncher();
		if (launcher)
			launcher.SetForced(EmoteLauncher.FORCE_DIFFERENT);
	}

	protected void ProcessPsiAuraClient(float dt)
	{
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer || !localPlayer.IsAlive())
			return;
		if (!IsAlive())
		{
			if (m_CalmEffectActive || m_AggroEffectActive)
			{
				StopPsiLoopSound();
				ResetClientVisuals();
				m_CalmEffectActive  = false;
				m_AggroEffectActive = false;
			}
			return;
		}

		float distSqCalm  = m_SFP_AuraCalmRadius  * m_SFP_AuraCalmRadius;
		float distSqAggro = m_SFP_AuraAggroRadius * m_SFP_AuraAggroRadius;
		float distSq      = vector.DistanceSq(localPlayer.GetPosition(), GetPosition());
		bool aggressive = IsPsiAggressive();
		m_PsiClientTimer += dt;

		if ((!aggressive && distSq > distSqCalm) || (aggressive && distSq > distSqAggro))
		{
			if (m_CalmEffectActive || m_AggroEffectActive)
			{
				StopPsiLoopSound();
				ResetClientVisuals();
				m_CalmEffectActive  = false;
				m_AggroEffectActive = false;
			}
			return;
		}

		if (!aggressive && distSq <= distSqCalm)
		{
			if (!m_CalmEffectActive)
			{
				m_CalmEffectActive  = true;
				m_AggroEffectActive = false;
				StartPsiLoopSound(SFP_MozgoTrasSoundSetRegistry.GetName(m_SFP_CalmSoundSetId));
				PPEffects.SetColorizationNV(m_SFP_CalmColorR, m_SFP_CalmColorG, m_SFP_CalmColorB);
			}
			PPEffects.SetBlur(m_SFP_CalmBlur);
			PPEffects.SetBloodSaturation(m_SFP_CalmBloodSaturation);
			PPEffects.SetShockVignette(m_SFP_CalmShockVignette);
			PPEffects.SetTunnelVignette(m_SFP_CalmTunnelVignette);
			PPEffects.SetChromAbb(m_SFP_CalmChromAbb);
			PPEffects.ResetLensEffect();
			if (m_PsiClientTimer >= m_SFP_CalmCameraShakeInterval)
			{
				m_PsiClientTimer = 0;
				if (localPlayer.GetCurrentCamera())
					localPlayer.GetCurrentCamera().SpawnCameraShake(m_SFP_CalmCameraShakeStrength);
			}
		}
		else if (aggressive && distSq <= distSqAggro)
		{
			if (!m_AggroEffectActive)
			{
				m_AggroEffectActive = true;
				m_CalmEffectActive  = false;
				StartPsiLoopSound(SFP_MozgoTrasSoundSetRegistry.GetName(m_SFP_AggroSoundSetId));
				PPEffects.SetColorizationNV(m_SFP_AggroColorR, m_SFP_AggroColorG, m_SFP_AggroColorB);
				ResetTinnitusTimer();
			}

			float dist = Math.Sqrt(distSq);
			float t = dist / m_SFP_AuraAggroRadius;
			t = Math.Clamp(t, 0.0, 1.0);
			float closeness = 1.0 - t;
			float blurValue = m_SFP_AggroBlurMin + (m_SFP_AggroBlurMax - m_SFP_AggroBlurMin) * closeness;
			PPEffects.SetBlur(blurValue);
			float saturation = m_SFP_AggroSaturationMin + (m_SFP_AggroSaturationMax - m_SFP_AggroSaturationMin) * t;
			PPEffects.SetBloodSaturation(saturation);
			float shockVig  = m_SFP_AggroShockVignetteCoef * closeness;
			float tunnelVig = m_SFP_AggroTunnelVignetteCoef * closeness;
			PPEffects.SetShockVignette(shockVig);
			PPEffects.SetTunnelVignette(tunnelVig);
			float chromAbb = m_SFP_AggroChromAbbCoef * closeness;
			PPEffects.SetChromAbb(chromAbb);
			float lensDistort = m_SFP_AggroLensDistortCoef * closeness;
			PPEffects.SetLensEffect(lensDistort, chromAbb, m_SFP_AggroLensParam3, m_SFP_AggroLensParam4);

			if (m_PsiClientTimer >= m_SFP_AggroCameraShakeInterval)
			{
				m_PsiClientTimer = 0;
				if (localPlayer.GetCurrentCamera())
					localPlayer.GetCurrentCamera().SpawnCameraShake(m_SFP_AggroCameraShakeStrength);

				float hp01 = localPlayer.GetHealth01("", "Health");
				float hpThreshold01 = m_SFP_SuicideHPThreshold / 100.0;
				if (hp01 < hpThreshold01 && Math.RandomFloat01() <= m_SFP_SuicideChance)
					TryForceSuicide(localPlayer);
			}

			UpdateTinnitus(localPlayer);
		}
	}

	protected void ResetTinnitusTimer()
	{
		float now = GetGame().GetTime() / 1000.0;
		m_NextTinnitusTime = now + Math.RandomFloat(m_SFP_TinnitusTimeMin, m_SFP_TinnitusTimeMax);
	}

	protected void UpdateTinnitus(PlayerBase player)
	{
		if (m_NextTinnitusTime <= 0)
			return;
		float now = GetGame().GetTime() / 1000.0;
		if (now < m_NextTinnitusTime)
			return;

		if (Math.RandomFloat01() <= m_SFP_TinnitusEffectChance)
		{
			player.SpawnFlashbangEffect(player, true);
			FlashbangEffect fb = player.GetFlashbangEffect();
			if (fb)
				fb.SetupFlashbangValues(m_SFP_FlashbangProgressMult, m_SFP_FlashbangVisualMax, m_SFP_FlashbangSoundMax);
		}
		ResetTinnitusTimer();
	}

	protected void StartPsiLoopSound(string soundSet)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (soundSet == "")
			return;
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		string path = "CfgSoundSets " + soundSet;
		if (!GetGame().ConfigIsExisting(path))
			return;
		if (m_PsiLoopSound)
			localPlayer.StopSoundSet(m_PsiLoopSound);
		localPlayer.PlaySoundSetLoop(m_PsiLoopSound, soundSet, m_SFP_LoopSoundVolume, m_SFP_LoopSoundPitch);
	}

	protected void StopPsiLoopSound()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!localPlayer)
			return;
		if (m_PsiLoopSound)
		{
			localPlayer.StopSoundSet(m_PsiLoopSound);
			m_PsiLoopSound = null;
		}
	}

	protected void ResetClientVisuals()
	{
		PPEffects.ResetColorize();
		PPEffects.SetBlur(0.0);
		PPEffects.ResetVignettes();
		PPEffects.SetChromAbb(0.0);
		PPEffects.ResetLensEffect();
		PPEffects.SetBloodSaturation(1.0);
	}

	override bool CanBeSkinned()
	{
		return true;
	}
}
