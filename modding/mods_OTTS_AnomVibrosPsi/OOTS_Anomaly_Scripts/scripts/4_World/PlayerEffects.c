class SoundOnPlayerInRandTime
{
	private float m_Tick = 0;
	private int m_TickTime;
	private int m_MinTime;
	private int m_MaxTime;
	private string m_SoundSet;
	
	private EffectSound m_Sound; 
	private float m_Volume = 1;
	
	void SoundOnPlayerInRandTime()
	{
		
	}
	
	void Play(string sound_set, int min_rep_time = 0, int max_rep_time = 0)
	{
		m_SoundSet = sound_set;
		m_MinTime = min_rep_time;
		m_MaxTime = max_rep_time;
		
		PlaySound();
		RandomizeTime();
	}
	
	void RandomizeTime()
	{
		m_TickTime = Math.RandomIntInclusive(m_MinTime, m_MaxTime);
	}
	
	void PlaySound()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if(player)
		{
			m_Sound = SEffectManager.CreateSound(m_SoundSet, player.GetPosition(), 1, 1, false);
			m_Sound.SetParent( player );
			m_Sound.SetLocalPosition( vector.Zero );
			m_Sound.SetSoundVolume(m_Volume);
			m_Sound.SetSoundAutodestroy( true );
			m_Sound.SoundPlay();
			
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetVolume, 50, false, m_Volume);
		}
	}
	
	void TickScheduler(float deltatime)
	{
		if(m_TickTime > 0)
		{
			m_Tick += deltatime;
			if(m_Tick >= m_TickTime)
			{
				PlaySound();
				m_Tick = 0;
				RandomizeTime();
			}
		}
	}
	
	void Stop()
	{
		m_TickTime = 0;
		m_Tick = 0;
		
		if(m_Sound)
			m_Sound.Stop();
	}
	
	void SetVolume(float volume)
	{
		m_Volume = volume;
		
		if(m_Sound)
			m_Sound.SetSoundVolume(m_Volume);
	}
}

class PlayerCameraShaker
{
	private float m_Strength;
	private float m_Radius;
	private float m_Smoothness;
	private float m_RadiusDecaySpeed;
	private float m_ShakeLenghtSecs;
	private float m_ShakePeriod = 1;
	private float m_Tick = 0;
	
	void SetShakePeriod(float period)
	{
		m_ShakePeriod = period;
	}
	
	void Shake(float strength, float radius, float smoothness, float radius_decay_speed, float shake_lenght_secs)
	{
		m_Strength = strength;
		m_Radius = radius;
		m_Smoothness = smoothness;
		m_RadiusDecaySpeed = radius_decay_speed;
		m_ShakeLenghtSecs = shake_lenght_secs;
		m_Tick = 0;
	}
	
	void Stop()
	{
		m_ShakeLenghtSecs = 0;
		m_Tick = 0;
	}
	
	void TickScheduler(float deltatime)
	{
		if(m_ShakeLenghtSecs > 0)
		{
			m_Tick += deltatime;
			if(m_Tick >= m_ShakePeriod)
			{
				ShakeCamera();
				m_ShakeLenghtSecs -= m_ShakePeriod;
				if(m_ShakeLenghtSecs < 0)
				{
					m_ShakeLenghtSecs = 0;
				}
				m_Tick = 0;
			}
		}
	}
	
	private void ShakeCamera()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if(player)
		{
			DayZPlayerCameraBase camera = DayZPlayerCameraBase.Cast(player.GetCurrentCamera());
			camera.SpawnCameraShakeProper(m_Strength, m_Radius, m_Smoothness, m_RadiusDecaySpeed);
		}
	}
}