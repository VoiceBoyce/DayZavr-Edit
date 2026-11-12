class PPERequester_OOTS_Anomaly_Client extends PPERequester_GameplayBase
{
	static const int NONE_INFLUENCE = 0;
	static const int BLOWOUT_INFLUENCE = 1;
	static const int PSI_INFLUENCE = 2;
	static const int PROTECTED_PSI_INFLUENCE = 3;
	static const int HIDDEN_PSI_INFLUENCE = 4;
	
	static const ref TFloatArray NONE_INFLUENCE_COLOR = {1, 1, 1, 0.0};
	static const ref TFloatArray BLOWOUT_INFLUENCE_COLOR = {1, 0, 0, 0.0};
	static const ref TFloatArray PSI_INFLUENCE_COLOR = {0.67, 0.53, 0.01, 0.0};
	static const ref TFloatArray PROTECTED_PSI_INFLUENCE_COLOR = {0.2, 0.21, 0.39, 0.0};
	static const ref TFloatArray HIDDEN_PSI_INFLUENCE_COLOR = {0.976, 0.784, 0.015, 0.0};
	
	private int m_CurrentInfluenceType = 0;
	
	private ref OOTS_Color_Interpotator m_PsiTranslate;
	private ref OOTS_Color_Interpotator m_PsiStormTranslate;
	
	void Init()
	{
		m_PsiTranslate = new OOTS_Color_Interpotator;
		m_PsiTranslate.SetDuration(3);
		m_PsiTranslate.SetPrev({1,1,1,0});
	}
	
	//blowout and psi influence
	void SetInfluence(int type, float duration = 5.0)
	{
		if(m_CurrentInfluenceType == type)
			return;
		
		m_PsiTranslate.SetDuration(duration);
		
		switch(type)
		{
			case NONE_INFLUENCE:
				SetTargetValueFloatDefault(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERX);
				SetTargetValueFloatDefault(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERY);
				SetTargetValueFloatDefault(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY);
				m_PsiTranslate.SetTarget(NONE_INFLUENCE_COLOR);
			break;
			case BLOWOUT_INFLUENCE:
				m_PsiTranslate.SetDuration(10);
				m_PsiTranslate.SetTarget(BLOWOUT_INFLUENCE_COLOR);
			break;
			case PSI_INFLUENCE:
				m_PsiTranslate.SetTarget(PSI_INFLUENCE_COLOR);
			break;
			case PROTECTED_PSI_INFLUENCE:
				m_PsiTranslate.SetDuration(2);
				m_PsiTranslate.SetTarget(PROTECTED_PSI_INFLUENCE_COLOR);
			break;
			case HIDDEN_PSI_INFLUENCE:
				m_PsiTranslate.SetDuration(2);
				m_PsiTranslate.SetTarget(HIDDEN_PSI_INFLUENCE_COLOR);
			break;
		}
		
		m_CurrentInfluenceType = type;
	}
	
	void BlowOutPhaseImpulse(bool active_phase = false)
	{
		m_PsiTranslate.Stop();
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {1, 0.7, 0.7, 0.0}, PPEGlow.L_23_NVG, PPOperators.SET);
		if(active_phase)
			SetTargetValueFloat(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY , false, 0.8, PPEGaussFilter.L_0_FLASHBANG, PPOperators.SET);
	}
		
	void OnInfluenceUpdate(float delta)
	{
		if(m_PsiTranslate.IsInProcess())
		{
			TFloatArray color = new TFloatArray;
			m_PsiTranslate.OnUpdate(delta, color);
			SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, color, PPEGlow.L_23_NVG, PPOperators.MULTIPLICATIVE);
		}
	}
	
	void CreatePsiStormTranslater()
	{
		m_PsiStormTranslate = new OOTS_Color_Interpotator;
		m_PsiStormTranslate.SetPrev({1,1,1,0});
		m_PsiStormTranslate.SetTarget(PSI_INFLUENCE_COLOR);
	}
	
	void RemovePsiStormTranslater()
	{
		if(m_PsiStormTranslate)
			m_PsiStormTranslate = null;
	}
	
	void OnInfluenceEasingUpdate(float lerp, bool apply)
	{
		TFloatArray color = new TFloatArray;
		m_PsiStormTranslate.OnLerpUpdate(lerp, color);
		if(apply)
			SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, color, PPEGlow.L_23_NVG, PPOperators.MULTIPLICATIVE);
	}
	
	void PsiStormImpulse(float value, bool reset = false)
	{
		if(reset)
		{
			SetTargetValueFloatDefault(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY);
			SetTargetValueFloatDefault(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERX);
			SetTargetValueFloatDefault(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERY);
			return;
		}
		SetTargetValueFloat(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY , false, Math.Lerp(0, 0.3, value), PPEGaussFilter.L_0_FLASHBANG, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERX, false, Math.Lerp(0, 0.05, value), PPEChromAber.L_0_INTRO, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERY, false, Math.Lerp(0, 0.05, value), PPEChromAber.L_0_INTRO, PPOperators.SET);
	}
	
	OOTS_Color_Interpotator GetPsiTranslate()
	{
		return m_PsiTranslate;
	}
}