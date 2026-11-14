class ArtModifiersManager
{
	PlayerBase m_Player;
	
	private ref map<string, ref ArtModifierBase> m_Modifiers;
	
	void ArtModifiersManager(PlayerBase player)
	{
		m_Modifiers = new map<string, ref ArtModifierBase>;
		m_Player = player;
		
		
		Init();
	}
	
	private void Init()
	{
		AddModifier("Health", new ArtModifierHealth);
		AddModifier("Shock", new ArtModifierShock);
		AddModifier("Blood", new ArtModifierBlood);
		AddModifier("Bleeds", new ArtModifierBleeds);
		AddModifier("Energy", new ArtModifierFood);
		AddModifier("Water", new ArtModifierWater);
		
		AddModifier("Stamina", new ArtModifierStamina);
		
		AddModifier("Radiation", new ArtModifierRadiation);
	}
	
	ArtModifierBase GetModifier(string type)
	{
		return m_Modifiers.Get(type);
	}
	
	void OnScheduledTick(float delta)
	{
		for(int i = 0; i < m_Modifiers.Count(); i++)
		{
			m_Modifiers.GetElement(i).OnTick(delta);
		}
	}
	
	private void AddModifier(string type, ArtModifierBase modifier)
	{
		modifier.Init(m_Player, this);
		
		m_Modifiers.Set(type, modifier);
	}
	
	void DeactivateAll()
	{
		for(int i = 0; i < m_Modifiers.Count(); i++)
		{
			m_Modifiers.GetElement(i).Deactivate();
		}
	}
}