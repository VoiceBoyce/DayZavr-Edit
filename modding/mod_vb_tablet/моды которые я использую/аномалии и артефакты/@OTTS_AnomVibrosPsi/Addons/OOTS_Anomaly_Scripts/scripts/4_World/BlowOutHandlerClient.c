class BlowOutHandlerClient
{
	private PlayerBase 					m_Player;
	private int 						m_CurPhase;
	private int 						m_PhaseDuration;
	
	private float 						m_Tick;
	private float 						m_TickTime;
	
	private ref EffectSound 			m_BORumbleSoundLoop, m_EarthCollapseLoop;
	private ref SoundOnPlayerInRandTime m_ThunderHitSound
	
	private ref PlayerCameraShaker      m_CameraShaker;
	
	private ref TStringArray 			m_MonolithPraySoundSets = {"BlowOutMonolithPray1_SoundSet", "BlowOutMonolithPray2_SoundSet"};
	
	private bool 						m_IsHidden;
	private float 						m_SoundVolume = 1;
	
	void BlowOutHandlerClient(PlayerBase player)
	{
		m_Player = player;
		
		m_ThunderHitSound = new SoundOnPlayerInRandTime();
		m_CameraShaker = new PlayerCameraShaker();
		
		m_IsHidden = m_Player.IsBlowOutHidden();
	}

	void ~BlowOutHandlerClient()
	{
		if(m_EarthCollapseLoop)
		{
			m_EarthCollapseLoop.Stop();
			m_EarthCollapseLoop = null;
		}
		
		if(m_BORumbleSoundLoop)
		{
			m_BORumbleSoundLoop.Stop();
			m_BORumbleSoundLoop = null;
		}
		
		if(m_ThunderHitSound)
		{
			m_ThunderHitSound.Stop();
			m_ThunderHitSound = null;
		}
		
		if(m_CameraShaker)
		{
			m_CameraShaker.Stop();
			m_CameraShaker = null;
		}
	}
	
	void OnUpdate(float deltaTime)
	{	
		m_Player.GetCamEffects().OnUpdate(deltaTime);
		
		if(m_CurPhase == BlowOutPhases.WARNING)
		{
			m_Tick += deltaTime;
			if(m_Tick >= m_TickTime)
			{
				m_Player.ClearScreen();
				m_Tick = 0;
				m_TickTime = 0;
			}
		}
		else if(m_CurPhase == BlowOutPhases.ACTIVE)
		{
			m_Tick += deltaTime;
			m_Player.GetCamEffects().SetInfluence(PPERequester_OOTS_Anomaly_Client.BLOWOUT_INFLUENCE);
			if(m_Tick >= m_TickTime)
			{
				RumbleHit();
				m_Tick = 0;
			}
			
			m_CameraShaker.TickScheduler(deltaTime);
			m_ThunderHitSound.TickScheduler(deltaTime);
		}
	}
	
	void SetPhaseDuration(int duration)
	{
		m_PhaseDuration = duration;
	}
	
	void OnPhaseSwitched(int phase)
	{
		switch(phase)
		{
			case BlowOutPhases.COOLDOWN:
				if(m_CurPhase == BlowOutPhases.END)
				{
					if(m_Player.IsMonolith())
						PlaySound(m_MonolithPraySoundSets.GetRandomElement(), 1);
				
					GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( m_Player.ClearScreen, 100, false, false);
				
					m_Player.OnBlowOutHandlerDone();
				}
			break;
			case BlowOutPhases.WARNING:
				if(m_Player.IsMonolith())
					PlaySound(m_MonolithPraySoundSets.GetRandomElement());
				
				m_Player.GetCamEffects().BlowOutPhaseImpulse(false);
				m_BORumbleSoundLoop = PlaySound("WarningRumble_SoundSet", m_SoundVolume, true);
				m_TickTime = Math.RandomIntInclusive(8, 14);
				m_Tick = 0;
			
			break;
			case BlowOutPhases.ACTIVE:
				if(m_Player.IsMonolith())
					PlaySound(m_MonolithPraySoundSets.GetRandomElement());
			
				RumbleHit();
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ActivePhaseStart, (Math.RandomIntInclusive(2, 5) * 1000), false);
			break;
			case BlowOutPhases.END:
				if(m_EarthCollapseLoop)
				{
					m_EarthCollapseLoop.Stop();
					m_EarthCollapseLoop = null;
				}
				
				if(m_BORumbleSoundLoop)
				{
					m_BORumbleSoundLoop.Stop();
					m_BORumbleSoundLoop = null;
				}
				
				if(m_ThunderHitSound)
				{
					m_ThunderHitSound.Stop();
					m_ThunderHitSound = null;
				}
				
				if(m_CameraShaker)
				{
					m_CameraShaker.Stop();
					m_CameraShaker = null;
				}
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( m_Player.ClearScreen, 100, false, true);
			break;
		}
		
		m_CurPhase = phase;
	}
	
	void OnHiddenStateChanged(bool state)
	{
		m_IsHidden = state;
		
		if(m_IsHidden)
			m_SoundVolume = 0.33;
		else
			m_SoundVolume = 1;
		
		if(m_EarthCollapseLoop)
		{
			m_EarthCollapseLoop.SetSoundVolume(m_SoundVolume);
		}
		
		if(m_BORumbleSoundLoop)
		{
			m_BORumbleSoundLoop.SetSoundVolume(m_SoundVolume);
		}
		
		if(m_ThunderHitSound)
		{
			m_ThunderHitSound.SetVolume(m_SoundVolume);
		}
	}
	
	private void RumbleHit()
	{
		if(m_IsHidden)
		{
			m_CameraShaker.Shake(0.2, 0.25, 0.23, 0.25, 1);
		}
		else
		{
			m_CameraShaker.Shake(0.8, 1, 0.7, 1.0, 4);
		}
		
		TStringArray rumbles = {"RumbleHit_1_SoundSet", "RumbleHit_2_SoundSet"};
		PlaySound(rumbles.GetRandomElement(), m_SoundVolume);
		
		m_TickTime = Math.RandomIntInclusive(5, 10);
	}
	
	private EffectSound PlaySound(string sound_set, float volume = 1, bool loop = false)
	{
		EffectSound effect_sound = SEffectManager.CreateSound(sound_set, m_Player.GetPosition(), 0, 0, loop);
		effect_sound.SetParent( m_Player );
		effect_sound.SetLocalPosition( vector.Zero );
		effect_sound.SetSoundVolume(volume);
		effect_sound.SetSoundAutodestroy( true );
		effect_sound.SoundPlay();
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ApplySoundVolume, 50, false, effect_sound, volume);
		
		return effect_sound;
	}
	
	private void ApplySoundVolume(EffectSound sound, float volume)
	{
		if(sound)
			sound.SetSoundVolume(volume);
	}
	
	private void ActivePhaseStart()
	{
		m_Player.GetCamEffects().BlowOutPhaseImpulse(true);
		 
		if(m_IsHidden)
		{
			m_CameraShaker.Shake(0.75, 2, 0.25, 0.5, 2);
		}	
		else
		{
			m_CameraShaker.Shake(1.5, 4, 0.5, 1.0, 4);
		}
		
		//PlaySound("BlowOutStart_SoundSet", m_SoundVolume);
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_Player.ClearScreen, (Math.RandomIntInclusive(6, 10) * 1000), false, false);
		//sounds
		m_ThunderHitSound.Play("ThunderHit_SoundSet", 30, 50);
		
		m_EarthCollapseLoop = PlaySound( "EarthCollapse_SoundSet", m_SoundVolume, true);
	}
}