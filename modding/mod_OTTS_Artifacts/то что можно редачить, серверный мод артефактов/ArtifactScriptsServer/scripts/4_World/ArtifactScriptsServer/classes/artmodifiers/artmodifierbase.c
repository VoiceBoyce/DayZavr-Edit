class ArtModifierBase
{
	protected float m_Value;
	protected ref map<DD_ArtifactBase, float> m_Relatives;
	
	protected PlayerBase m_Player;
	protected ref ArtModifiersManager m_Manager;
	
	protected float m_Time = 3;
	protected float m_Timer = 0;
	
	
	void ArtModifierBase()
	{
		m_Value = 0;
	}
	
	void Init(PlayerBase player, ArtModifiersManager manager)
	{
		m_Player = player;
		m_Manager = manager;
		
		m_Relatives = new map<DD_ArtifactBase, float>;
	}
	
	void AddAbsolute(float val)
	{
		m_Value += val;
	}
	
	void AddRelative(DD_ArtifactBase art, float val)
	{
		m_Relatives.Set(art, val);
	}
	
	void RemoveRelative(DD_ArtifactBase art)
	{
		m_Relatives.Remove(art);
	}
	
	void SetTime(float time)
	{
		m_Time = time;
	}
	
	protected float GetRelativeValue()
	{
		float value = 1;
		for(int i = 0; i < m_Relatives.Count(); i++)
		{
			value *= m_Relatives.GetElement(i);
		}
		
		return value;
	}
	
	void OnTick(float delta)
	{
		if(m_Value == 0 || !m_Player.IsAlive())
			return;
		
		m_Timer += delta;
		if(m_Timer >= m_Time)
		{
			Process();
			
			m_Timer = 0;
		}
	}
	
	void Deactivate()
	{
		m_Value = 0;
		m_Relatives.Clear();
	}
	
	protected void Process()
	{
	
	}
}