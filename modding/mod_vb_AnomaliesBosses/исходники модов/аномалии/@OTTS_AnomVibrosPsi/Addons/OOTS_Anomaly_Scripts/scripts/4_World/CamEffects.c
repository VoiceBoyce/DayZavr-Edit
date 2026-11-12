class CamEffects
{
    Material        chromAber;
    Material        matColors;
    Material        rotBlur;
    Material        radBlur;
    float           CARGB[4];

	bool 			m_PsiInpulse;
	float 			m_PsiImpulseTick;
	float 			m_PsiImpulseDuration;
	
	private ref PPERequester_OOTS_Anomaly_Client m_PostFX;
	
    void CamEffects()
    {
		m_PostFX = PPERequester_OOTS_Anomaly_Client.Cast(PPERequesterBank.GetRequester(PPERequester_OOTS_Anomaly_Client));
		m_PostFX.Init();
    }
	
	void SetInfluence(int type, bool effect_stop = false)
	{
		if(!m_PostFX.IsRequesterRunning())
			m_PostFX.Start();
			
		m_PostFX.SetInfluence(type);

		if(effect_stop)
			m_PostFX.Stop();
	}
	
	void OnUpdate(float delta)
	{
		if(m_PostFX)
			m_PostFX.OnInfluenceUpdate(delta);
		
		if(m_PsiInpulse)
		{
			m_PsiImpulseTick += delta;
			
			if(m_PsiImpulseTick >= m_PsiImpulseDuration)
			{
				m_PsiImpulseTick = 0;
				m_PsiInpulse = false;
				
				m_PostFX.PsiStormImpulse(0, true);
				return;
			}
			
			float time, value;
			float half_duration = m_PsiImpulseDuration/2;
			
			
			
			if(m_PsiImpulseTick < half_duration)
			{
				time = Easing.EaseOutCubic(m_PsiImpulseTick/half_duration);
				//value = Math.Lerp(0, 1, time);
			}
			else if(m_PsiImpulseTick >= half_duration)
			{
				time = 1 - Easing.EaseOutCubic((m_PsiImpulseTick - half_duration)/half_duration);
				//value = Math.Lerp(1, 0, time);
			}
			
			m_PostFX.PsiStormImpulse(time);
		}
	}

	void BlowOutPhaseImpulse(bool is_active_phase)
	{
		m_PostFX.BlowOutPhaseImpulse(is_active_phase);
	}
	
	void PsiStormImpulse(float duration)
	{
		if(duration <= 0)
			return;
		
		m_PsiImpulseTick = 0;
		m_PsiInpulse = true;
		m_PsiImpulseDuration = duration;
	}
	
	PPERequester_OOTS_Anomaly_Client GetRequester()
	{
		return m_PostFX;
	}
	
}