class SFP_MW_ClientSummonEffect
{
	protected PlayerBase m_Player;
	protected ref Timer m_Timer;
	protected float m_Time;
	protected float m_Duration;
	protected float m_ShockBlur;
	protected float m_ShockVignette;
	protected float m_ShockOverlay;
	protected float m_ShakeStrength;
	protected float m_FlashDurationMultiplier;
	protected float m_FlashIntensity;
	protected float m_FlashSoundIntensity;
	
	void SFP_MW_ClientSummonEffect(PlayerBase player)
	{
		m_Player = player;
		m_Timer = new Timer(CALL_CATEGORY_GAMEPLAY);
	}
	
	void ~SFP_MW_ClientSummonEffect()
	{
		Stop();
	}
	
	void Start(float flashDurationMultiplier, float flashIntensity, float flashSoundIntensity, float shockBlur, float shockVignette, float shockOverlay, float shakeStrength, float duration)
	{
		Stop();
		m_Time = 0.0;
		m_Duration = Math.Max(duration, 0.25);
		m_FlashDurationMultiplier = Math.Clamp(flashDurationMultiplier, 0.20, 2.50);
		m_FlashIntensity = Math.Clamp(flashIntensity, 0.05, 1.00);
		m_FlashSoundIntensity = Math.Clamp(flashSoundIntensity, 0.05, 1.00);
		m_ShockBlur = Math.Clamp(shockBlur, 0.0, 1.0);
		m_ShockVignette = Math.Clamp(shockVignette, 0.0, 1.0);
		m_ShockOverlay = Math.Clamp(shockOverlay, 0.0, 1.0);
		m_ShakeStrength = Math.Max(shakeStrength, 0.05);
		
		if (m_Player)
		{
			m_Player.OnPlayerReceiveFlashbangHitStart(true);
			if (m_Player.GetFlashbangEffect())
				m_Player.GetFlashbangEffect().SetupFlashbangValues(m_FlashDurationMultiplier, m_FlashIntensity, m_FlashSoundIntensity);
			
			DayZPlayerCamera camera = m_Player.GetCurrentCamera();
			if (camera)
				camera.SpawnCameraShake(0.55 * m_ShakeStrength, 1.65, 10.0, 2.0);
		}
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Start(new Param3<float,float,float>(m_ShockBlur, m_ShockVignette, m_ShockOverlay));
		
		if (m_Timer)
			m_Timer.Run(0.04, this, "Tick", null, true);
		
		Tick();
	}
	
	void Stop()
	{
		if (m_Timer && m_Timer.IsRunning())
			m_Timer.Stop();
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Stop();
	}
	
	protected void Tick()
	{
		if (!m_Player || !m_Player.IsAlive())
		{
			Stop();
			return;
		}
		
		m_Time += 0.04;
		float progress = Math.Clamp(m_Time / m_Duration, 0.0, 1.0);
		float intensity;
		if (progress <= 0.20)
			intensity = Math.InverseLerp(0.0, 0.20, progress);
		else
			intensity = 1.0 - Math.InverseLerp(0.20, 1.0, progress);
		intensity = Math.Clamp(intensity, 0.0, 1.0);
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Start(new Param3<float,float,float>(m_ShockBlur * intensity, m_ShockVignette * intensity, m_ShockOverlay * intensity));
		
		DayZPlayerCamera camera = m_Player.GetCurrentCamera();
		if (camera && progress <= 0.72)
			camera.SpawnCameraShake((0.22 + (0.34 * intensity)) * m_ShakeStrength, 1.25, 9.0, 2.0);
		
		if (progress >= 1.0)
			Stop();
	}
};

class SFP_MW_ClientHighgroundEffect
{
	protected PlayerBase m_Player;
	protected ref Timer m_Timer;
	protected float m_Time;
	protected float m_Duration;
	protected float m_Blur;
	protected float m_Vignette;
	protected float m_Overlay;
	protected float m_ShakeStrength;
	
	void SFP_MW_ClientHighgroundEffect(PlayerBase player)
	{
		m_Player = player;
		m_Timer = new Timer(CALL_CATEGORY_GAMEPLAY);
	}
	
	void ~SFP_MW_ClientHighgroundEffect()
	{
		Stop();
	}
	
	void Start(float blur, float vignette, float overlay, float shakeStrength, float duration)
	{
		Stop();
		m_Time = 0.0;
		m_Duration = Math.Max(duration, 0.25);
		m_Blur = Math.Clamp(blur, 0.0, 1.0);
		m_Vignette = Math.Clamp(vignette, 0.0, 1.0);
		m_Overlay = Math.Clamp(overlay, 0.0, 1.0);
		m_ShakeStrength = Math.Max(shakeStrength, 0.05);
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Start(new Param3<float,float,float>(m_Blur, m_Vignette, m_Overlay));
		
		if (m_Player && m_Player.GetCurrentCamera())
			m_Player.GetCurrentCamera().SpawnCameraShake(0.34 * m_ShakeStrength, 1.15, 8.0, 2.0);
		
		if (m_Timer)
			m_Timer.Run(0.05, this, "Tick", null, true);
		
		Tick();
	}
	
	void Stop()
	{
		if (m_Timer && m_Timer.IsRunning())
			m_Timer.Stop();
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Stop();
	}
	
	protected void Tick()
	{
		if (!m_Player || !m_Player.IsAlive())
		{
			Stop();
			return;
		}
		
		m_Time += 0.05;
		float progress = Math.Clamp(m_Time / m_Duration, 0.0, 1.0);
		float intensity;
		if (progress <= 0.25)
			intensity = Math.InverseLerp(0.0, 0.25, progress);
		else
			intensity = 1.0 - Math.InverseLerp(0.25, 1.0, progress);
		intensity = Math.Clamp(intensity, 0.0, 1.0);
		
		PPERequester_ShockHitReaction req = PPERequester_ShockHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_ShockHitReaction));
		if (req)
			req.Start(new Param3<float,float,float>(m_Blur * intensity, m_Vignette * intensity, m_Overlay * intensity));
		
		DayZPlayerCamera camera = m_Player.GetCurrentCamera();
		if (camera && progress <= 0.55)
			camera.SpawnCameraShake((0.18 + (0.14 * intensity)) * m_ShakeStrength, 1.05, 8.0, 2.0);
		
		if (progress >= 1.0)
			Stop();
	}
};

modded class PlayerBase
{
	protected ref SFP_MW_ClientSummonEffect m_SFP_MW_ClientSummonEffect;
	protected ref SFP_MW_ClientHighgroundEffect m_SFP_MW_ClientHighgroundEffect;
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		if (rpc_type == SFP_MW_RPC_PLAY_SUMMON_EFFECT)
		{
			float flashDurationMultiplier;
			float flashIntensity;
			float flashSoundIntensity;
			float shockBlur;
			float shockVignette;
			float shockOverlay;
			float shakeStrength;
			float shakeDuration;
			
			if (!ctx.Read(flashDurationMultiplier)) return;
			if (!ctx.Read(flashIntensity)) return;
			if (!ctx.Read(flashSoundIntensity)) return;
			if (!ctx.Read(shockBlur)) return;
			if (!ctx.Read(shockVignette)) return;
			if (!ctx.Read(shockOverlay)) return;
			if (!ctx.Read(shakeStrength)) return;
			if (!ctx.Read(shakeDuration)) return;
			
			SFP_MW_StartSummonEffect(flashDurationMultiplier, flashIntensity, flashSoundIntensity, shockBlur, shockVignette, shockOverlay, shakeStrength, shakeDuration);
			return;
		}
		
		if (rpc_type == SFP_MW_RPC_PLAY_HIGHGROUND_EFFECT)
		{
			float hgBlur;
			float hgVignette;
			float hgOverlay;
			float hgShakeStrength;
			float hgDuration;
			
			if (!ctx.Read(hgBlur)) return;
			if (!ctx.Read(hgVignette)) return;
			if (!ctx.Read(hgOverlay)) return;
			if (!ctx.Read(hgShakeStrength)) return;
			if (!ctx.Read(hgDuration)) return;
			
			SFP_MW_StartHighgroundEffect(hgBlur, hgVignette, hgOverlay, hgShakeStrength, hgDuration);
		}
	}
	
	void SFP_MW_StartSummonEffect(float flashDurationMultiplier, float flashIntensity, float flashSoundIntensity, float shockBlur, float shockVignette, float shockOverlay, float shakeStrength, float shakeDuration)
	{
		if (!m_SFP_MW_ClientSummonEffect)
			m_SFP_MW_ClientSummonEffect = new SFP_MW_ClientSummonEffect(this);
		
		m_SFP_MW_ClientSummonEffect.Start(flashDurationMultiplier, flashIntensity, flashSoundIntensity, shockBlur, shockVignette, shockOverlay, shakeStrength, shakeDuration);
	}
	
	void SFP_MW_StartHighgroundEffect(float blur, float vignette, float overlay, float shakeStrength, float duration)
	{
		if (!m_SFP_MW_ClientHighgroundEffect)
			m_SFP_MW_ClientHighgroundEffect = new SFP_MW_ClientHighgroundEffect(this);
		
		m_SFP_MW_ClientHighgroundEffect.Start(blur, vignette, overlay, shakeStrength, duration);
	}
};
