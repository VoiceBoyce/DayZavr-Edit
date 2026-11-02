class AnomalyHandlerTrigger: ManTrigger
{
	private AnomalyHandlerTriggerCfg m_Parent;
	
	void SetParent(AnomalyHandlerTriggerCfg cfg)
	{
		m_Parent = cfg;
	}
	
	override bool CanAddObjectAsInsider(Object object)
	{
		PlayerBase player;
		if(Class.CastTo(player, object))
			return true;
		
		return false;
	}
	
	override void OnEnterServerEvent(TriggerInsider insider)
	{
		//Print("[AnonalySpawner]: Player enter AmonalyZone...");
		if(m_Parent)
			m_Parent.OnEnterServerEvent(insider.GetObject());
	}
	
	override void OnLeaveServerEvent(TriggerInsider insider)
	{
		//Print("[AnonalySpawner]: Player leave AmonalyZone...");
		if(m_Parent)
			m_Parent.OnLeaveServerEvent(insider.GetObject());
	}
}