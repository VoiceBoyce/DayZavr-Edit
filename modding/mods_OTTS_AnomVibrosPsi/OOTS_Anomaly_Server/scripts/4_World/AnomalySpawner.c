class AnomalySpawner
{
	static const private string CFG_PATH = "$profile:\\AnomalySpawner\\";
	static const private string CFG_FILENAME = "Config_AnomalySpawner.json";	
	
	static private ref map<string, ref DetectorConfig> m_DetectorConfig = new map<string, ref DetectorConfig>;
	static private ref array<ref ArtefactForDetector> m_Artefacts = new array<ref ArtefactForDetector>;
	
	static private ref AnomalySpawnerMainConfig m_Config;
	
	static private ref array<Anomaly_DetectorBase> m_RegistredDetectors;
	static private bool m_IsReaded = false;
	
	void Init()
	{
		
	}
	
	static DetectorConfig GetDetectorConfig(string detector)
	{
		return m_DetectorConfig.Get(detector);
	}
	
	static void RegisterDetectorToConfigure(Anomaly_DetectorBase detector)
	{
		if(m_IsReaded)
		{
			DetectorConfig cfg = GetDetectorConfig(detector.GetType());
			if(cfg)
			{
				detector.SetIsDetector(cfg);
			}
			
			return;
		}
		
		if(!m_RegistredDetectors)
			m_RegistredDetectors = new array<Anomaly_DetectorBase>;
		
		m_RegistredDetectors.Insert(detector);
	}
	
	static void AddArtefact(ArtefactForDetector artefact)
	{
		m_Artefacts.Insert(artefact);
	}
	
	static void RemoveArtefact(ArtefactForDetector artefact)
	{
		m_Artefacts.RemoveItemUnOrdered(artefact);
	}
	
	static map<ref ArtefactForDetector, float> GetArtefactsForDetectorInRadius(vector position, float radius)
	{
		map<ref ArtefactForDetector, float> artefacts = new map<ref ArtefactForDetector, float>;
		
		if(m_Artefacts.Count() > 0)
		{
			//TIntArray indexes_to_remove = new TIntArray;
			foreach(int i, auto art: m_Artefacts)
			{
				if(art)
				{
					float distance = vector.Distance(position, art.GetPos());
					if(distance <= radius)
					{
						artefacts.Set(art, distance);
					}
				}
				/*else
				{
					indexes_to_remove.Insert(i);
				}*/
			}
			
			/*foreach(int index: indexes_to_remove)
			{
				m_Artefacts.Remove(index);
			}*/
		}
		
		return artefacts;
	}
	
	static void ReadConfig()
	{
		if( !FileExist(CFG_PATH) )
		{
			MakeDirectory(CFG_PATH);
		}
		
		if( !FileExist(CFG_PATH + CFG_FILENAME) )
		{
			Print("[AnonalySpawner]: Creating default config...");
			//JsonFileLoader<AnomalySpawnerMainConfig>.JsonSaveFile((CFG_PATH + CFG_FILENAME), CreateDefaultConfig());
			
			CopyFile("OOTS_Anomaly_Server\\data\\Config_AnomalySpawner.json", CFG_PATH + CFG_FILENAME);
			
			MakeDirectory(CFG_PATH + "anomalies");
			MakeDirectory(CFG_PATH + "teleports");
			
			CopyFile("OOTS_Anomaly_Server\\data\\anomalies\\defaults.json", CFG_PATH + "anomalies\\defaults.json");
			CopyFile("OOTS_Anomaly_Server\\data\\anomalies\\defaults2.json", CFG_PATH + "anomalies\\defaults2.json");
			
			CopyFile("OOTS_Anomaly_Server\\data\\teleports\\defaults.json", CFG_PATH + "teleports\\defaults.json");
		}
		else
		{
			//read and parse congif
			//AnomalySpawnerMainConfig config;
			string error;
			if(!JsonFileLoader<AnomalySpawnerMainConfig>.LoadFile(CFG_PATH + CFG_FILENAME, m_Config, error))
			{
				PrintFormat("[AnonalySpawner]: Reading config error: %1", error);
				return;	
			}
			if(m_Config)
			{
				Print("[AnonalySpawner]: Reading config...");
				ConfigureDetectors();
				ConfigureAnomalies();
				m_IsReaded = true;
			}
			else
			{
				Print("[AnonalySpawner]: Occured some errors while reading config. Please, check your config is correct");
			}
		}
	}
	
	static private void ConfigureDetectors()
	{
		if(m_Config.DetectorsConfig.Count() > 0)
		{
			foreach(DetectorJSONConfig det: m_Config.DetectorsConfig)
			{
				DetectorConfig detector = new DetectorConfig(det);
				m_DetectorConfig.Set(det.ClassName, detector);
			}
		}
		
		if(!m_RegistredDetectors)
			return;
		
		foreach(Anomaly_DetectorBase detector_item: m_RegistredDetectors)
		{
			DetectorConfig cfg = GetDetectorConfig(detector_item.GetType());
			if(cfg)
			{
				detector_item.SetIsDetector(cfg);
			}
		}
		
		m_RegistredDetectors.Clear();
		delete m_RegistredDetectors;
	}
	
	static private void ConfigureAnomalies()
	{
		if(m_Config.AnomalyList.Count() > 0)
		{
			foreach(AnomalyListConfig anomaly: m_Config.AnomalyList)
			{
				anomaly.Init(false);
			}
		}
		
		if(m_Config.Teleports.Count() > 0)
		{
			//turn off Teleport manager
			GetAnomalyTeleportManager().SetAnomalySpawnerStatus(true);
		
			foreach(AnomalyListConfig teleport: m_Config.Teleports)
			{
				teleport.Init(true);
			}
			
			//turn on Teleport manager
			GetAnomalyTeleportManager().SetAnomalySpawnerStatus(false);
		}
	}
	
	//rebuild anomalies
	static void ClearAndReconfigure()
	{
		PrintFormat("[AnomalySpawner]: Reconfiguration....");
		
		if(m_Artefacts.Count() > 0)
		{
			foreach(ArtefactForDetector artefact: m_Artefacts)
			{
				if(artefact)
				{
					delete artefact;
				}
			}
		}
		
		m_DetectorConfig.Clear();
		m_Artefacts.Clear();
		
		delete m_Config;
		m_Config = null;
		
		ReadConfig();
	}
}