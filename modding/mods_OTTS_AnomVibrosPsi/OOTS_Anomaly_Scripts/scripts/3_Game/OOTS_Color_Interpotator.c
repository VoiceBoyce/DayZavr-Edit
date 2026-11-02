class OOTS_Color_Interpotator
{
	private float m_Tick;
	private float m_Duration;
	private float m_IsInProcess;
	
	private ref TFloatArray m_CurColor;
	private ref TFloatArray m_PrevColor;
	private ref TFloatArray m_TargetColor;
	
	void OOTS_Color_Interpotator()
	{
		m_CurColor = {1, 1, 1, 1};
		m_PrevColor = new TFloatArray;
		m_TargetColor = {1, 1, 1, 1};
	}
	
	void OnUpdate(float delta, out TFloatArray result)
	{
		if(m_IsInProcess)
		{
			m_Tick += delta;
			for(int i = 0; i < 4; i++)
			{
				m_CurColor[i] = Math.Clamp(Math.Lerp(m_PrevColor[i], m_TargetColor[i], m_Tick/m_Duration), 0, 1);
			}
			
			result.Copy(m_CurColor);
			
			if(m_Tick >= m_Duration)
			{
				m_IsInProcess = false;
			}
		}
	}
	
	void OnLerpUpdate(float lerp_val, out TFloatArray result)
	{
		for(int i = 0; i < 4; i++)
		{
			m_CurColor[i] = Math.Clamp(Math.Lerp(m_PrevColor[i], m_TargetColor[i], lerp_val), 0, 1);
		}
		
		result.Copy(m_CurColor);
	}
	
	bool IsInProcess()
	{
		return m_IsInProcess;
	}
	
	void SetCurrent(TFloatArray color)
	{
		m_CurColor.Copy(color);
	}
	
	void SetPrev(TFloatArray color)
	{
		m_PrevColor.Copy(color);
	}
	
	void SetTarget(TFloatArray color)
	{
		m_PrevColor.Copy(m_CurColor);
		m_TargetColor.Copy(color);
		
		Start();
	}
	
	void SetDuration(float duration)
	{
		m_Duration = duration;
	}
	
	private void Start()
	{
		if(m_Duration == 0)
			return;
		
		m_IsInProcess = true;
		m_Tick = 0;
	}
	
	void Stop()
	{
		m_IsInProcess = true;
		m_Tick = 0;
	}
	
	void Clear()
	{
		m_CurColor = {1, 1, 1, 1};
		m_PrevColor = new TFloatArray;
		m_TargetColor = {1, 1, 1, 1};
	}
}