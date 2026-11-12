class PsiStormHandlerClient
{
	private PlayerBase 								m_Player;
	private int 									m_PhaseDuration;
	private float 									m_PhaseTick;
	
	private int 									m_CurPhase;
	
	private float 									m_EffectTick;
	private int 									m_EffectTickTime;
	
	private EffectSound 							m_SandSound, m_StormSound, m_PsiInfluenceSound;
	private EffectSound 							m_HeadVoiceSound;
	private float 									m_CurHeadVoiceVolume;
	private float 									m_MaxHeadVoiceDuration;
	private float 									m_MaxHeadVoiceTick;
	
	private float 									m_SoundRelVol = 1;
	
	private bool 									m_IsHidden;
	
	private bool 									m_IsUnderPsiInfluence;
	private ref OOTS_Color_Interpotator 			m_PsiTranslate;
	
	void PsiStormHandlerClient(PlayerBase player)
	{
		m_Player = player;
		
		GetNextTickTime();
	}
	
	void ~PsiStormHandlerClient()
	{
		if(m_SandSound)
		{
			m_SandSound.Stop();
			m_SandSound = null;
		}
		
		if(m_StormSound)
		{
			m_StormSound.Stop();
			m_StormSound = null;
		}
		
		if(m_PsiInfluenceSound)
		{
			m_PsiInfluenceSound.Stop();
			m_PsiInfluenceSound = null;
		}
		
		if(m_HeadVoiceSound)
		{
			m_HeadVoiceSound.Stop();
			m_HeadVoiceSound = null;
		}
	}
	
	void SetPsiInfluenceState(bool state)
	{
		m_IsUnderPsiInfluence = state;
	}
	
	void OnUpdate(float delta_time)
	{
		CamEffects effect;
		
		m_PhaseTick += delta_time / 60;
		
		if(m_CurPhase == BlowOutPhases.WARNING)
		{
			float easing = Easing.EaseInOutCubic(m_PhaseTick/m_PhaseDuration);
			
			if(m_SandSound && m_StormSound && m_PhaseDuration > 0)
			{
				m_SandSound.SetSoundVolume(easing * m_SoundRelVol);
				m_StormSound.SetSoundVolume(easing * m_SoundRelVol);
			}
			
			effect = m_Player.GetCamEffects();
			if(effect)
			{
				effect.GetRequester().OnInfluenceEasingUpdate(easing, !m_IsUnderPsiInfluence);
			}
		}
		else if(m_CurPhase == BlowOutPhases.ACTIVE)
		{
			effect = m_Player.GetCamEffects();
			if(effect)
			{
				effect.OnUpdate(delta_time);
			}
			
			m_EffectTick += delta_time;
			if(m_EffectTick >= m_EffectTickTime)
			{
				ApplyEffect();
				m_EffectTick = 0;
			}
			
			if(m_HeadVoiceSound)
			{
				m_MaxHeadVoiceTick += delta_time;
				if(m_MaxHeadVoiceTick >= m_MaxHeadVoiceDuration)
				{
					m_HeadVoiceSound.Stop();
					m_HeadVoiceSound = null;
					m_MaxHeadVoiceTick = 0;
				}
			}
		}
		else if(m_CurPhase == BlowOutPhases.END)
		{
			if(m_SandSound && m_StormSound && m_PhaseDuration > 0)
			{
				float sound_vol = (1 - (m_PhaseTick/m_PhaseDuration)) * m_SoundRelVol;
				m_SandSound.SetSoundVolume(sound_vol);
				m_StormSound.SetSoundVolume(sound_vol);
			}
		}
		else if(m_CurPhase == BlowOutPhases.COOLDOWN)
		{
			
		}
	}
	
	void SetPhaseDuration(int duration)
	{
		m_PhaseDuration = duration;
	}
	
	int GetCurPhase()
	{
		return m_CurPhase;
	}
	
	void OnPhaseSwitched()
	{
		m_CurPhase = m_Player.GetPsiStormPhase();
		m_PhaseTick = 0;
		
		CamEffects effect;
		
		if(m_CurPhase == BlowOutPhases.WARNING)
		{
			effect = m_Player.GetCamEffects();
			if(effect)
			{
				effect.GetRequester().CreatePsiStormTranslater();
			}
			
			if(!m_SandSound && !m_StormSound)
			{
				m_Player.PlaySoundSet(m_SandSound, "PsiStorm_1_SoundSet", 0, 0, true);
				m_SandSound.SetSoundVolume(0);
				
				m_Player.PlaySoundSet(m_StormSound, "WarningRumble_SoundSet", 0, 0, true);
				m_StormSound.SetSoundVolume(0);
			}
			
			//ApplyInfluence();
		}
		else if(m_CurPhase == BlowOutPhases.ACTIVE)
		{
			if(!m_SandSound && !m_StormSound)
			{
				m_Player.PlaySoundSet(m_SandSound, "PsiStorm_1_SoundSet", 0, 0, true);
				m_Player.PlaySoundSet(m_StormSound, "WarningRumble_SoundSet", 0, 0, true);
			}
			
			if(m_SandSound && m_StormSound)
			{
				m_SandSound.SetSoundVolume(m_SoundRelVol);
				m_StormSound.SetSoundVolume(m_SoundRelVol);
			}
			
			
			if(!m_PsiInfluenceSound)
			{
				m_Player.PlaySoundSet(m_PsiInfluenceSound, "PsiInfluence_SoundSet", 1, 1, true);
				m_PsiInfluenceSound.SetSoundVolume(m_SoundRelVol);
			}
			
			effect = m_Player.GetCamEffects();
			if(effect)
			{
				effect.GetRequester().RemovePsiStormTranslater();
			}
			
			ApplyInfluence();
		}
		else if(m_CurPhase == BlowOutPhases.END)
		{
			effect = m_Player.GetCamEffects();
			if(effect)
			{
				effect.SetInfluence(PPERequester_OOTS_Anomaly_Client.NONE_INFLUENCE, false);
			}
		}
		else if(m_CurPhase == BlowOutPhases.COOLDOWN)
		{
			if(m_SandSound && m_StormSound)
			{
				m_SandSound.Stop();
				m_SandSound = null;
				
				m_StormSound.Stop();
				m_StormSound = null;
			}
			
			if(m_PsiInfluenceSound)
			{
				m_PsiInfluenceSound.Stop();
				m_PsiInfluenceSound = null;
			}
			
			if(m_HeadVoiceSound)
			{
				m_HeadVoiceSound.SetSoundFadeOut(3);
				m_HeadVoiceSound.Stop();
			}
			
			m_Player.OnPsiHandlerDone();
		}
	}
	
	//==== 
	
	void OnHiddenStateChanged(bool state)
	{
		m_IsHidden = state;
		
		int influence;
		if(state)
		{
			m_SoundRelVol = 0.20;
		}
		else
		{
			m_SoundRelVol = 1;
		}
		
		if(m_CurPhase == BlowOutPhases.ACTIVE)
		{
			if(m_SandSound && m_StormSound)
			{
				m_SandSound.SetSoundVolume(m_SoundRelVol);
				m_StormSound.SetSoundVolume(m_SoundRelVol);
			}
			
			if(m_PsiInfluenceSound)
			{
				m_PsiInfluenceSound.SetSoundVolume(m_SoundRelVol);
			}
			
			if(m_HeadVoiceSound)
			{
				m_HeadVoiceSound.SetSoundVolume(m_SoundRelVol);
			}
		}
		
		ApplyInfluence();
	}
	
	private void GetNextTickTime()
	{
		m_EffectTickTime = Math.RandomIntInclusive(8, 22);
	}
	
	private void ApplyEffect()
	{
		GetNextTickTime();
		
		float chance = 0.75;
		if(m_IsHidden)
			chance = 0.12;
		
		float rnd = Math.RandomFloatInclusive(0, 1);
		if(rnd <= chance)
		{	
			if(m_Player.GetEmoteManager().CanPlayEmote(EmoteConstants.ID_EMOTE_SUICIDE))
			{
				m_Player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SUICIDE);
			}
			else
			{
				TIntArray emotes = {EmoteConstants.ID_EMOTE_THROAT, EmoteConstants.ID_EMOTE_TAUNT};
				int emo = emotes.GetRandomElement();
				if(m_Player.GetEmoteManager().CanPlayEmote(emo))
				{
					m_Player.GetEmoteManager().CreateEmoteCBFromMenu(emo);
				}
			}
		}
		
		CamEffects effect = m_Player.GetCamEffects();
		if(effect)
		{
			effect.PsiStormImpulse(Math.RandomIntInclusive(10, 12));
		}
		
		HeadVoiceHandle();
	}
	
	private void ApplyInfluence()
	{
		if(m_CurPhase != BlowOutPhases.ACTIVE)
			return;
		
		int influence = PPERequester_OOTS_Anomaly_Client.PSI_INFLUENCE;
		if(m_IsHidden)
			influence = PPERequester_OOTS_Anomaly_Client.HIDDEN_PSI_INFLUENCE;
				
		CamEffects effect = m_Player.GetCamEffects();
		if(effect)
		{
			effect.SetInfluence(influence);
		}
	}
	
	private void HeadVoiceHandle()
	{
		if(!m_HeadVoiceSound)
		{
			TStringArray soundsets = {"PsiStorm_HeadVoice_1_SoundSet", "PsiStorm_HeadVoice_2_SoundSet"};
			
			m_MaxHeadVoiceDuration = Math.RandomIntInclusive(12, 25);
			
			float fades = (m_MaxHeadVoiceDuration - 5)/2;
			
			m_Player.PlaySoundSet(m_HeadVoiceSound, soundsets.GetRandomElement(), fades, fades);
		}
	}
}