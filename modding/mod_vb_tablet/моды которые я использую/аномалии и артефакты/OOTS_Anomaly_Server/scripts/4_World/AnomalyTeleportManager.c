class AnomalyTeleportManager: PluginBase
{
	private bool m_AnomalySpawnerStatus = false; 
	private ref map<Anomaly_Teleport_Entry, Anomaly_Teleport_Output> m_TeleportPair = new map<Anomaly_Teleport_Entry, Anomaly_Teleport_Output>;
	private Anomaly_Teleport_Entry m_Entry;
	
	void SetAnomalySpawnerStatus(bool status)
	{
		m_AnomalySpawnerStatus = status;
	}
	
	void RegisterEntry(Anomaly_Teleport_Entry entry)
	{
		if(m_AnomalySpawnerStatus)
			return;
		
		if(m_Entry)
			GetGame().ObjectDelete(m_Entry);
		
		m_Entry = entry;
		
	}
	
	void RegisterOutput(Anomaly_Teleport_Output teleport)
	{
		if(m_AnomalySpawnerStatus)
			return;
		
		if(m_Entry)
		{
			m_Entry.AddOutput(teleport);
		}
		else
		{
			GetGame().ObjectDelete(teleport);
		}
	}
	
	void RegisterDot(Anomaly_Teleport_Dot teleport)
	{
		if(m_AnomalySpawnerStatus)
			return;
		
		if(m_Entry && !m_Entry.HasOutput())
			GetGame().ObjectDelete(m_Entry);
		
		m_Entry = null;
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( RemoveDot, 3000, false, teleport);
	}
	
	private void RemoveDot(Anomaly_Teleport_Dot teleport)
	{
		GetGame().ObjectDelete(teleport);
	}
}

AnomalyTeleportManager GetAnomalyTeleportManager()
{
	return AnomalyTeleportManager.Cast(GetPluginManager().GetPluginByType(AnomalyTeleportManager));
}