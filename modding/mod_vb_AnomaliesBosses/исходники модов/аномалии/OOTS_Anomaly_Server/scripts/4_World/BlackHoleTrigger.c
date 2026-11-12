class BlackHoleTrigger: Trigger
{
	private Anomaly_BlackHole m_Parent;
		
	void SetParent(Anomaly_BlackHole anomaly)
	{
		m_Parent = anomaly;
	}
	
	override bool CanAddObjectAsInsider(Object object)
	{
		return object.IsEntityAI();
	}
	
	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		m_Parent.OnSucked(insider.GetObject());
	}
}