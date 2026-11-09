class DetectorConfig
{
	private string m_Type = "Detector1";
	private float m_PollingTime = 3;
	private float m_Radius = 3;
	private bool m_ShowDistance = false;
	private string m_ShowDistanceMessage = "The closest artefact in %1 distance";
	private bool m_ShowArtsCount = false;
	private string m_ShowArtsCountMessage = "Artefact(s) count is %1";
	
	void DetectorConfig(DetectorJSONConfig config)
	{
		m_Type = config.ClassName;
		m_PollingTime = config.PollingTime;
		m_Radius = config.Radius;
		m_ShowDistance = config.ShowDistance;
		m_ShowDistanceMessage = config.ShowDistanceMessage;
		m_ShowArtsCount = config.ShowArtsCount;
		m_ShowArtsCountMessage = config.ShowArtsCountMessage;
	}
	
	float GetPollingTime()
	{
		return m_PollingTime;
	}
	
	float GetRadius()
	{
		return m_Radius;
	}
	
	bool NeedShowDistance()
	{
		return m_ShowDistance;
	}
	
	string GetDistanceMessage(float distance)
	{
		return string.Format(m_ShowDistanceMessage, distance);
	}
	
	bool NeedShowCount()
	{
		return m_ShowArtsCount;
	}
	
	string GetCountMessage(int count)
	{
		return string.Format(m_ShowArtsCountMessage, count);
	}
}

class ArtefactTriggerConfig
{
	private ref NativeArtefacts m_Native;
	private ref DetectorArtefacts m_Detector;
	
	void ArtefactTriggerConfig(NativeArtefacts native_artefacts, DetectorArtefacts detector_artefacts)
	{
		m_Native = native_artefacts;
		m_Detector = detector_artefacts;
	}
	
	bool CanBeSpanwedNativeArtefacts()
	{
		return m_Native.EnableToSpawn;
	}
	
	bool CanBeSpanwedDetectorArtefacts()
	{
		return m_Detector.EnableToSpawn;
	}
	
	int GetDetectorSpawnCount()
	{
		return m_Detector.MaxCount;
	}
	
	string GetArtefactByDetector(string detector)
	{
		ConfigArtefacts artefacts = m_Detector.DetectorList.Get(detector);
		if(artefacts)
		{
			return artefacts.GetArtefactChanced();
		}
		
		return "";
	}
	
	array<string> GetDetectorsByArtefact(string artefact)
	{
		return m_Detector.GetDetectorsByArtefact(artefact);
	}
	
	bool IsDetector(string type)
	{
		if(m_Detector.DetectorList.Get(type))
		{
			return true;
		}
		
		return false;
	}
	
	int GetNativeSpawnCount()
	{
		return m_Native.MaxCount;
	}
	
	string GetNativeArtefact()
	{
		return m_Native.GetArtefactChanced();
	}
}

class RouletteSectorData
{
	int index;
	float min;
	float max;
	
	string ToText()
	{
		return string.Format("(%1, %2, %3)", index, min, max);
	}
}

class ConfigArtefacts
{
	ref map<string, float> Artefacts;
	
	string GetArtefactChanced()
	{	
		int sector_count = (2 * Artefacts.Count()) + 1;
		
		float p_sum = 0;
		for(int i = 0; i < Artefacts.Count(); i++)
		{
			p_sum += Artefacts.GetElement(i);
		}
		
		float cur_p_pos = 0;
		float cur_p_sigma = 0;
		int k = 0;
		int art_index = 0;
		array<ref RouletteSectorData> roulette = new array<ref RouletteSectorData>;
		RouletteSectorData sector_data;
		if(p_sum > 100)
		{
			for(i = 0; i < Artefacts.Count(); i++)
			{
				sector_data = new RouletteSectorData();
				
				cur_p_sigma = (Artefacts.GetElement(i)/p_sum) * 100;
				
				sector_data.index = i;
				sector_data.min = cur_p_pos;
				sector_data.max = cur_p_pos + cur_p_sigma;
				
				cur_p_pos += cur_p_sigma;
				
				roulette.Insert(sector_data);
			}
		}
		else
		{
			float p_empty = (100 - p_sum)/(Artefacts.Count() + 1);

			for(i = 0; i < sector_count; i++)
			{
				sector_data = new RouletteSectorData();
				
				if(i % 2 == 1)
				{
					cur_p_sigma = Artefacts.GetElement(k);
					art_index = k;
					k++;
				}
				else
				{
					art_index = -1;
					cur_p_sigma = p_empty;
				}
				
				sector_data.index = art_index;
				sector_data.min = cur_p_pos;
				sector_data.max = cur_p_pos + cur_p_sigma;
				
				cur_p_pos += cur_p_sigma;
				
				roulette.Insert(sector_data);
			}
		}
		
		
		
		float rnd = Math.RandomFloatInclusive(0, 100);
		art_index = -1;
		foreach(int j, RouletteSectorData data: roulette)
		{
			if(rnd >= data.min && rnd < data.max)
			{
				art_index = data.index;
				break;
			}
		}
		
		if(art_index > -1)
			return Artefacts.GetKey(art_index);
	
		return "";
	}
}

class DetectorArtefacts
{
	bool EnableToSpawn = false;
	int MaxCount = 2;
	ref map<string, ref ConfigArtefacts> DetectorList;
	
	array<string> GetDetectorsByArtefact(string artefact)
	{
		array<string> detectors = new array<string>;
		for(int i = 0; i < DetectorList.Count(); i++)
		{
			ConfigArtefacts arts = DetectorList.GetElement(i);
			for(int j = 0; arts.Artefacts.Count(); j++)
			{
				string art = arts.Artefacts.GetKey(j);
				if(art == artefact)
				{
					detectors.Insert(DetectorList.GetKey(i));
				}
			}
		}
		
		return detectors;
	}
}

class NativeArtefacts: ConfigArtefacts
{
	bool EnableToSpawn = false;
	int MaxCount = 2;
}

class ArtefactForDetector
{
	private string m_Type;
	private vector m_Pos;
	private bool m_IsSpawned;
	
	
	void ArtefactForDetector(string type, vector pos)
	{
		m_Type = type;
		m_Pos = pos;
	}
	
	string GetType()
	{
		return m_Type;
	}
	
	vector GetPos()
	{
		return m_Pos;
	}
	
	bool IsSpawned()
	{
		return m_IsSpawned;
	}
	
	void SetSpawned()
	{
		m_IsSpawned = true;
	}
	
}

//The main task is spawn and active artefacts by player data
class ArtefactTrigger: ManTrigger
{
	private AnomaliesArtefactTriggerData m_Parent;
	private ArtefactTriggerConfig m_Config;
	private bool m_EnableToSpawnNative = false;
	private bool m_EnableToSpawnDetector = false;
	
	private int m_DetectorRestCount = 0;

	private float m_MinSpawnDistance = 3;
	private float m_MaxSpawnDistance = 10;
	
	//-----------------trigger config-----------------------
	void ArtefactTrigger()
	{
		
	}
	
	override void SetExtents(vector mins, vector maxs)
	{
		super.SetExtents(mins, maxs);
		
		m_MaxSpawnDistance = GetRadius(mins, maxs);
	}
	
	void SetParent(AnomaliesArtefactTriggerData parent)
	{
		m_Parent = parent;
	}
	
	void Configure(ArtefactTriggerConfig config)
	{
		m_Config = config;
		m_EnableToSpawnNative = m_Config.CanBeSpanwedNativeArtefacts();
		m_EnableToSpawnDetector = m_Config.CanBeSpanwedDetectorArtefacts();
	}

	//-----------------trigger actions-----------------------
	override void OnEnterServerEvent(TriggerInsider insider)
	{
		PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			CheckNativeArtefacts();
			CheckDetectorArtefacts(player);
			if(!m_EnableToSpawnDetector && !m_EnableToSpawnNative)
			{
				m_Parent.OnDelete();
				Delete();
			}
		}
	}
	
	private void CheckDetectorArtefacts(PlayerBase player)
	{
		if(m_EnableToSpawnDetector)
		{
			string detector = CheckDetectorType(player);
			if(detector.Length() > 0)
			{
				for(int i = 0; i < m_Config.GetDetectorSpawnCount(); i++)
				{
					string artefact = m_Config.GetArtefactByDetector(detector);
					if(artefact.Length() > 0)
					{
						AnomalySpawner.AddArtefact(new ArtefactForDetector(artefact, GetArtefactPos()));
					}
				}
				
				m_EnableToSpawnDetector = false;
			}
		}
	}
	
	//check a player has detector and it is on and has battery
	private string CheckDetectorType(PlayerBase player)
	{
		if(player)
		{
			Anomaly_DetectorBase handItem = Anomaly_DetectorBase.Cast( player.GetHumanInventory().GetEntityInHands() );
			if(handItem && handItem.IsDetector())
			{	
				return handItem.GetType();
			}
		}
		
		return "";
	}
	
	
	private void CheckNativeArtefacts()
	{
		if(m_EnableToSpawnNative)
		{
			for(int i = 0; i < m_Config.GetNativeSpawnCount(); i++)
			{
				string artefact = m_Config.GetNativeArtefact();
				if(artefact.Length() > 0)
				{
					SpawnArtefact(artefact, GetArtefactPos());
				}
			}
			m_EnableToSpawnNative = false;
		}
	}
	
	private vector GetArtefactPos()
	{
		vector pos = GetPosition();
		float random_x = Math.RandomFloatInclusive(m_MinSpawnDistance, m_MaxSpawnDistance);
		float random_z = Math.RandomFloatInclusive(m_MinSpawnDistance, m_MaxSpawnDistance);
		if(Math.RandomIntInclusive(0,1))
		{
			pos[0] = pos[0] + random_x;
		} 
		else
		{
			pos[0] = pos[0] - random_x;
		}
		if(Math.RandomIntInclusive(0,1))
		{
			pos[2] = pos[2] + random_z;
		} 
		else
		{
			pos[2] = pos[2] - random_z;
		}
			
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		
		return pos;
	}
	
	private void SpawnArtefact(string type, vector pos)
	{
		GetGame().CreateObject(type, pos);
	}
}