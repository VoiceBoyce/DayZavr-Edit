modded class PlayerBase
{
	ref ArtModifiersManager m_ArtManager;
	
	override void Init()
	{
		super.Init();
		
		m_ArtManager = new ArtModifiersManager(this);
	}
	
	ArtModifiersManager GetArtManager()
	{
		return m_ArtManager;
	}
	
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);
		
		if(m_ArtManager)
			m_ArtManager.OnScheduledTick(deltaTime);
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if(GetArtManager())
			GetArtManager().DeactivateAll();
	}
}