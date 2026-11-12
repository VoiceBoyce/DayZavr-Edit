class SFP_KrovososClientController
{
	protected static ref SFP_KrovososClientController s_Instance;
	
	protected float m_ScanAccumulator;
	protected float m_TargetHeartVolume;
	protected float m_CurrentHeartVolume;
	protected float m_AggroCooldown;
	protected EffectSound m_HeartLoop;
	
	protected float m_AggroEffectTime;
	protected float m_AggroEffectDuration;
	protected bool m_AggroEffectActive;
	
	protected ref array<SFP_ANM_Krovosos_inviz_Base> m_TrackedMutants;
	protected ref array<int> m_TrackedAggroPulses;
	
	static SFP_KrovososClientController GetInstance()
	{
		if (!s_Instance)
			s_Instance = new SFP_KrovososClientController();
		return s_Instance;
	}
	
	void SFP_KrovososClientController()
	{
		m_TrackedMutants = new array<SFP_ANM_Krovosos_inviz_Base>();
		m_TrackedAggroPulses = new array<int>();
	}
	
	void Update(PlayerBase player, float deltaTime)
	{
		if (GetGame().IsDedicatedServer())
			return;
		
		if (!player || !player.IsPlayerSelected() || !player.IsAlive())
		{
			StopAll();
			return;
		}
		
		if (m_AggroCooldown > 0)
			m_AggroCooldown = Math.Max(0.0, m_AggroCooldown - deltaTime);
		
		m_ScanAccumulator += deltaTime;
		if (m_ScanAccumulator >= 0.10)
		{
			m_ScanAccumulator = 0.0;
			ScanNearbyMutants(player);
		}
		
		UpdateHeartSound(player, deltaTime);
		UpdateAggroEffect(player, deltaTime);
	}
	
	protected void ScanNearbyMutants(PlayerBase player)
	{
		array<Object> objects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		array<SFP_ANM_Krovosos_inviz_Base> seenMutants = new array<SFP_ANM_Krovosos_inviz_Base>();
		
		GetGame().GetObjectsAtPosition3D(player.GetPosition(), 25.0, objects, proxyCargos);
		
		float nearestDistance = 9999.0;
		m_TargetHeartVolume = 0.0;
		
		foreach (Object obj : objects)
		{
			SFP_ANM_Krovosos_inviz_Base mutant;
			if (!Class.CastTo(mutant, obj))
				continue;
			
			if (!mutant.IsAlive())
				continue;
			
			seenMutants.Insert(mutant);
			
			float distance = vector.Distance(player.GetPosition(), mutant.GetPosition());
			if (distance < nearestDistance)
				nearestDistance = distance;
			
			int idx = m_TrackedMutants.Find(mutant);
			int pulse = mutant.SFP_GetAggroPulse();
			if (idx == -1)
			{
				m_TrackedMutants.Insert(mutant);
				m_TrackedAggroPulses.Insert(pulse);
			}
			else if (m_TrackedAggroPulses[idx] != pulse)
			{
				m_TrackedAggroPulses[idx] = pulse;
				if (distance <= 20.0)
					TriggerAggroEffect(player);
			}
		}
		
		for (int i = m_TrackedMutants.Count() - 1; i >= 0; i--)
		{
			if (seenMutants.Find(m_TrackedMutants[i]) == -1)
			{
				m_TrackedMutants.Remove(i);
				m_TrackedAggroPulses.Remove(i);
			}
		}
		
		if (nearestDistance <= 10.0)
		{
			float t = 1.0 - Math.Clamp(nearestDistance / 10.0, 0.0, 1.0);
			m_TargetHeartVolume = 0.35 + (t * 0.65);
		}
	}
	
	protected void TriggerAggroEffect(PlayerBase player)
	{
		if (m_AggroCooldown > 0.0)
			return;
		
		m_AggroEffectTime = 0.0;
		m_AggroEffectDuration = 4.25;
		m_AggroEffectActive = true;
		
		if (player.GetCurrentCamera())
			player.GetCurrentCamera().SpawnCameraShake(1.10, 1.65, 10.0, 2.2);
		
		m_AggroCooldown = 3.5;
	}
	
	protected void UpdateAggroEffect(PlayerBase player, float deltaTime)
	{
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (!req)
			return;
		
		if (!m_AggroEffectActive)
			return;
		
		m_AggroEffectTime += deltaTime;
		float progress = Math.Clamp(m_AggroEffectTime / m_AggroEffectDuration, 0.0, 1.0);
		
		float intensity;
		if (progress <= 0.18)
			intensity = Math.InverseLerp(0.0, 0.18, progress);
		else
			intensity = 1.0 - Math.InverseLerp(0.18, 1.0, progress);
		
		intensity = Math.Clamp(intensity, 0.0, 1.0);
		
		float blur = 0.95 * intensity;
		float vignette = 0.40 * intensity;
		float overlay = 0.16 * intensity;
		req.Start(new Param3<float,float,float>(blur, vignette, overlay));
		
		if (player.GetCurrentCamera() && progress <= 0.38)
			player.GetCurrentCamera().SpawnCameraShake(0.10 * intensity, 1.00, 8.0, 2.0);
		
		if (progress >= 1.0)
		{
			m_AggroEffectActive = false;
			req.Stop();
		}
	}
	
	protected void UpdateHeartSound(PlayerBase player, float deltaTime)
	{
		float blendSpeed = 3.0;
		m_CurrentHeartVolume = Math.Lerp(m_CurrentHeartVolume, m_TargetHeartVolume, Math.Clamp(deltaTime * blendSpeed, 0.0, 1.0));
		
		if (m_TargetHeartVolume > 0.01)
		{
			if (!m_HeartLoop)
			{
				player.PlaySoundSetLoop(m_HeartLoop, "SFP_Krovosos_playerHeart_Soundset", 0.0, 1.0);
				if (m_HeartLoop)
				{
					m_HeartLoop.SetAutodestroy(false);
					m_HeartLoop.SetSoundVolume(0.0);
				}
			}
			
			if (m_HeartLoop)
				m_HeartLoop.SetSoundVolume(Math.Clamp(m_CurrentHeartVolume, 0.0, 1.0));
		}
		else if (m_HeartLoop)
		{
			m_HeartLoop.SetSoundVolume(Math.Clamp(m_CurrentHeartVolume, 0.0, 1.0));
			if (m_CurrentHeartVolume <= 0.01)
			{
				m_HeartLoop.SoundStop();
				m_HeartLoop = null;
			}
		}
	}
	
	void StopAll()
	{
		m_TargetHeartVolume = 0.0;
		m_CurrentHeartVolume = 0.0;
		m_AggroCooldown = 0.0;
		m_ScanAccumulator = 0.0;
		m_AggroEffectTime = 0.0;
		m_AggroEffectDuration = 0.0;
		m_AggroEffectActive = false;
		m_TrackedMutants.Clear();
		m_TrackedAggroPulses.Clear();
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Stop();
		
		if (m_HeartLoop)
		{
			m_HeartLoop.SoundStop();
			m_HeartLoop = null;
		}
	}
};

modded class PlayerBase
{
	protected int m_SFP_LastKrovososBloodDamageMs;
	
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);
		
		if (!GetGame().IsDedicatedServer())
			SFP_KrovososClientController.GetInstance().Update(this, deltaTime);
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (!GetGame().IsServer() || !IsAlive())
			return;
		
		SFP_ANM_Krovosos_inviz_Base mutant;
		if (!Class.CastTo(mutant, source))
			return;
		
		int nowMs = GetGame().GetTime();
		if ((nowMs - m_SFP_LastKrovososBloodDamageMs) < 250)
			return;
		m_SFP_LastKrovososBloodDamageMs = nowMs;
		
		float bloodDamage = SFP_KrovososConfig.Get().BloodDamagePerAttack;
		if (bloodDamage <= 0.0)
			return;
		
		AddHealth("", "Blood", -bloodDamage);
	}
};
