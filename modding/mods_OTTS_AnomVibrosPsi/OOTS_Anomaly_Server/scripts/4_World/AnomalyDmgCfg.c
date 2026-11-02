class AnomalyDmgCfg 
{
	static private ref map <string, ref AnomalyDmgCfgJson> m_Anomaly;
	
	static void ReadConfig()
	{
		string path="$profile:\\AnomalySpawner\\AnomalyDmgCfg.json";
		string error;
		if (!JsonFileLoader <map <string, ref AnomalyDmgCfgJson>>.LoadFile(path, m_Anomaly, error))
		{
			PrintFormat("[AnomalyDmgCfg]:Error Json load - %1", error);
			return;
		}
	}
	static AnomalyDmgCfgJson GetAnomaly(string type)
	{
		if(!m_Anomaly)
			return null;
		
		AnomalyDmgCfgJson cfg;
		if(m_Anomaly.Find(type, cfg))
		{
			return cfg;
		}
		
		return null;
		
	}
}


class AnomalyDmgCfgJson
{
	private ref TFloatArray m_DmgRatioMinMax;
	private string 			m_AmmoType;
	
	float GetDmgCoef()
	{
		return Math.RandomFloatInclusive(m_DmgRatioMinMax.Get(0), m_DmgRatioMinMax.Get(1));
	}
}