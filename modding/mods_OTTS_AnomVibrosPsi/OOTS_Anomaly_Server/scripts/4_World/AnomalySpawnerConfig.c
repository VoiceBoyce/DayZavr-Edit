//Config classes
class AnomalySpawnerMainConfig
{
	bool DebugLogs_enabled;
	ref array<ref DetectorJSONConfig> 		DetectorsConfig;
	ref array<ref AnomalyListConfig> 		AnomalyList;
	ref array<ref AnomalyListConfig> 		Teleports;
	
	void ~AnomalySpawnerMainConfig()
	{
		//Print("~AnomalySpawnerMainConfig()");
		
		foreach(DetectorJSONConfig detector: DetectorsConfig)
		{
			delete detector;
		}
		
		foreach(AnomalyListConfig anomalies: AnomalyList)
		{
			delete anomalies;
		}
		
		foreach(AnomalyListConfig teleports: Teleports)
		{
			delete teleports;
		}
		
		delete DetectorsConfig;
		delete AnomalyList;
		delete Teleports;
	}
}

class AnomalyListConfig
{
	private string 								m_Title;
	private ref AnomalyHandlerTriggerCfg 		m_Handler;
	private ref TStringArray 					m_Files;
	
	[NonSerialized()]
	protected const string BASE_PATH = "$profile:\\AnomalySpawner\\";
	[NonSerialized()]
	protected ref array<ref AnomalyTypeConfig> 		m_Anomalies;
	[NonSerialized()]
	protected ref array<ref AnomalyTeleportConfig> 	m_Teleports;
	[NonSerialized()]
	protected bool 									m_IsTeleports;
	
	
	void ~AnomalyListConfig()
	{
		//Print("~AnomalyListConfig()");
		
		if(m_Anomalies)
		{
			foreach(AnomalyTypeConfig anomalies: m_Anomalies)
			{
				delete anomalies;
			}
		}
		
		if(m_Teleports)
		{
			foreach(AnomalyTeleportConfig teleports: m_Teleports)
			{
				delete teleports;
			}
		}
		
		delete m_Anomalies;
		delete m_Teleports;
		delete m_Handler;
	}
	
	void Init(bool teleports = false)
	{
		PrintFormat("[AnomalySpawner]: Parsing list %1", m_Title);
		
		m_IsTeleports = teleports;
		
		if(m_Handler.IsShapeDataExits())
		{
			m_Handler.SetParent(this);
			m_Handler.Init();
		}
		
		if(m_IsTeleports)
			m_Teleports = new array<ref AnomalyTeleportConfig>;
		else
			m_Anomalies = new array<ref AnomalyTypeConfig>;
		
		foreach(string file: m_Files)
		{
			if(FileExist(BASE_PATH + file))
			{
				if(m_IsTeleports)
					ReadTeleports(file);
				else
					ReadAnomalies(file);
			}
			else
			{
				PrintFormat("[AnomalySpawner]: There is no file on path %1", BASE_PATH + file);
			}
		}
	}
	
	private bool ReadAnomalies(string file)
	{
		string error;
		array<ref AnomalyTypeConfig> data;
		if(!JsonFileLoader<array<ref AnomalyTypeConfig>>.LoadFile(BASE_PATH + file, data, error))
		{
			PrintFormat("[AnomalySpawner]: Loading file error: %1", error);
			return false;
		}
		
		foreach(AnomalyTypeConfig config: data)
		{
			config.SetParent(this);
			config.Spawn();
			m_Anomalies.Insert(config);
		}
		
		return true;
	}
	
	private bool ReadTeleports(string file)
	{
		string error;
		array<ref AnomalyTeleportConfig> data;
		if(!JsonFileLoader<array<ref AnomalyTeleportConfig>>.LoadFile(BASE_PATH + file, data, error))
		{
			PrintFormat("[AnomalySpawner]: Loading file error: %1", error);
			return false;
		}
		
		foreach(AnomalyTeleportConfig config: data)
		{
			config.SetParent(this);
			config.Spawn();
			m_Teleports.Insert(config);
		}
		
		return true;
	}
	
	string GetTitle()
	{
		return m_Title;
	}
	
	bool IsHandlerExists()
	{
		return m_Handler.IsExists();
	}
	
	bool IsOnlyTriggers()
	{
		return m_Handler.IsOnlyTriggers();
	}
	
	void OnEnter()
	{
		if(m_Anomalies)
		{
			foreach(AnomalyTypeConfig anomalies: m_Anomalies)
			{
				anomalies.OnEnter();
			}
		}
		
		if(m_Teleports)
		{
			foreach(AnomalyTeleportConfig teleports: m_Teleports)
			{
				teleports.OnEnter();
			}
		}
	}
	
	void OnLeave()
	{
		if(m_Anomalies)
		{
			foreach(AnomalyTypeConfig anomalies: m_Anomalies)
			{
				anomalies.OnLeave();
			}
		}
		
		if(m_Teleports)
		{
			foreach(AnomalyTeleportConfig teleports: m_Teleports)
			{
				teleports.OnLeave();
			}
		}
	}
}

class AnomalyBaseConfig
{
	//ref AnomalyHandlerTriggerCfg			m_Handler;
	
	[NonSerialized()]
	protected AnomalyListConfig 			m_Parent;
	[NonSerialized()]
	protected ref array<OOTS_AnomalyBase> 	m_Anomalies;
	[NonSerialized()]
	protected ref array<vector> 			m_AnomaliesPoses;
	
	
	void ~AnomalyBaseConfig()
	{
		//Print("~AnomalyBaseConfig()");
		
		if(m_AnomaliesPoses)
		{
			m_AnomaliesPoses.Clear();
			m_AnomaliesPoses = null;
		}
		
		if(m_Anomalies)
		{
			foreach(OOTS_AnomalyBase anomaly: m_Anomalies)
			{
				if(anomaly)
					anomaly.Delete();
			}
			
			m_Anomalies.Clear();
			m_Anomalies = null;
		}
		
		delete m_Anomalies;
		delete m_AnomaliesPoses;
		//delete m_Handler;
	}
	
	void Init()
	{
		/*if(m_Handler.IsShapeDataExits())
		{
			m_Handler.SetParent(this);
			m_Handler.Init();
		}*/
	}
	
	void SetParent(AnomalyListConfig config)
	{
		m_Parent = config;
	}
	
	string GetTitle()
	{
		return "";
	}
	
	protected bool WillBeSpawned(float chance)
	{
		if(Math.RandomFloat01() <= chance)
		{
			return true;
		}
		
		return false;
	}
	
	void Spawn()
	{
	
	}
	
	void OnEnter()
	{
	
	}
	
	void OnLeave()
	{
	
	}
	
	void Clear()
	{
		
	}
}

class AnomalyTypeConfig: AnomalyBaseConfig
{
	string 												Unique_Group_Title;
	string 												Classname_Anomaly_Object;
	int 												Chance_To_Spawn;
	
	ref array<vector> 									Positions_Spawn_Anomaly;
	ref AnomalySpawnByRadius 							m_SpawnByRadius;
	
	bool 												Enable_Spawn_ArtefactsWithoutDetectors;
	bool 												Enable_Spawn_ArtefactsWithDetectors;
	
	float 												Distance_To_Group_Art_Trigger;
	float 												Additional_Distance_For_Art_Triggger;
	float 												Additional_Height_For_Art_Trigger;
	
	ref ArtefactsWithoutDetectorsConfig 				ArtefactsWithoutDetectors;
	ref ArtefactsWithDetectorsConfig 					ArtefactsWithDetectors;
	
	[NonSerialized()]
	private ref array<ref AnomaliesArtefactTriggerData> m_ArtefactTriggersDatas;
	[NonSerialized()]
	private ref ArtefactTriggerConfig 					m_ArtefactTriggersConfig;
	
	void ~AnomalyTypeConfig()
	{
		//Print("~AnomalyTypeConfig()");
		
		if(m_ArtefactTriggersConfig)
		{
			delete m_ArtefactTriggersConfig;
			m_ArtefactTriggersConfig = null;
		}
		
		if(m_ArtefactTriggersDatas)
		{
			foreach(AnomaliesArtefactTriggerData data: m_ArtefactTriggersDatas)
			{
				delete data;
			}
			
			delete m_ArtefactTriggersDatas;
			m_ArtefactTriggersDatas = null;
		}
		
		delete ArtefactsWithoutDetectors;
		delete ArtefactsWithDetectors;
		
		delete m_SpawnByRadius;
		delete Positions_Spawn_Anomaly;
	}
	
	override void Spawn()
	{
		m_Anomalies = new array<OOTS_AnomalyBase>;
		
		/*if(m_Handler.IsExists() && !m_Handler.IsOnlyTriggers())
		{
			m_AnomaliesPoses = new array<vector>;
		}*/
		
		if(m_Parent.IsHandlerExists() && !m_Parent.IsOnlyTriggers())
		{
			m_AnomaliesPoses = new array<vector>;
		}
		
		OOTS_AnomalyBase anomaly;
		array<vector> anomaly_type_spanwed_poses = new array<vector>;
		if(Positions_Spawn_Anomaly.Count() > 0)
		{
			PrintFormat("[AnomalySpawner]: Handling \"%1\" anomaly group", Unique_Group_Title);
			foreach(vector pos: Positions_Spawn_Anomaly)
			{
				if(WillBeSpawned(Chance_To_Spawn/100))
				{
					if(m_AnomaliesPoses)
					{
						m_AnomaliesPoses.Insert(pos);
						PrintFormat("[AnomalySpawner]: Anomaly \"%1\" is controlled by handler at %2", Classname_Anomaly_Object, pos.ToString());
					}
					else
					{
						PrintFormat("[AnomalySpawner]: Create anomaly \"%1\" at %2", Classname_Anomaly_Object, pos.ToString());
						anomaly = OOTS_AnomalyBase.Cast( GetGame().CreateObject(Classname_Anomaly_Object, pos) );
						if(anomaly)
						{
							if(m_Parent.IsHandlerExists())
							{
								anomaly.DestroyAreaDamage();
							}
							
							m_Anomalies.Insert(anomaly);
						}
					}
					
					if(Enable_Spawn_ArtefactsWithoutDetectors || Enable_Spawn_ArtefactsWithDetectors)
					{
						anomaly_type_spanwed_poses.Insert(pos);
					}
				}
				else
				{
					//print
					PrintFormat("[AnomalySpawner]: Skip anomaly \"%1\" at %2", Classname_Anomaly_Object, pos.ToString());
				}
			}
		}
		
		//spawn by radius
		if(m_SpawnByRadius.CanSpawn())
		{
			m_SpawnByRadius.Init();
			
			int count_to_spawn = m_SpawnByRadius.GetCount();
			PrintFormat("[AnomalySpawner]: Handling \"%1\" anomaly group by radius. Selected count is %2", Unique_Group_Title, count_to_spawn);
			vector position;
			//int iFlags = ECE_SETUP|ECE_PLACE_ON_SURFACE;
			for(int i = 0; i < count_to_spawn; i++)
			{
				if(m_SpawnByRadius.UseChance() && !WillBeSpawned(Chance_To_Spawn/100))
				{
					PrintFormat("[AnomalySpawner]: Skip next spawn by radius position for \"%1\"", Classname_Anomaly_Object);
					continue;
				}
				
				position = m_SpawnByRadius.GetNextPosition();
				position[1] = GetGame().SurfaceY(position[0], position[2]);
				
				if(m_AnomaliesPoses)
				{
					m_AnomaliesPoses.Insert(position);
					PrintFormat("[AnomalySpawner]: Anomaly \"%1\" is controlled by handler at %2", Classname_Anomaly_Object, position.ToString());
				}
				else
				{
					anomaly = OOTS_AnomalyBase.Cast( GetGame().CreateObject(Classname_Anomaly_Object, position) );
					if(anomaly)
					{
						PrintFormat("[AnomalySpawner]: Create anomaly \"%1\" at %2", Classname_Anomaly_Object, position.ToString());
						//anomaly.PlaceOnSurface();
						
						if(m_Parent.IsHandlerExists())
						{
							anomaly.DestroyAreaDamage();
						}
						
						m_Anomalies.Insert(anomaly);
					}
				}
				
				if(Enable_Spawn_ArtefactsWithoutDetectors || Enable_Spawn_ArtefactsWithDetectors)
				{
					anomaly_type_spanwed_poses.Insert(position);
				}
				
			}
		}
		
		//artefacts triggers and config creation
		if(anomaly_type_spanwed_poses.Count() > 0)
		{
			m_ArtefactTriggersConfig = CreateArtefactConfig();
			CreateArtefactTriggers(anomaly_type_spanwed_poses);
		}
		
	}
	
	private ArtefactTriggerConfig CreateArtefactConfig()
	{
		//create config
		NativeArtefacts config_no_detector = new NativeArtefacts();
		config_no_detector.EnableToSpawn = Enable_Spawn_ArtefactsWithoutDetectors;
		config_no_detector.MaxCount = ArtefactsWithoutDetectors.MaxInOnePoint;
		map<string, float> artefacts = new map<string, float>;
		foreach(ArtefactSettingsConfig artefact_config: ArtefactsWithoutDetectors.ArtefactsList)
		{
			artefacts.Set(artefact_config.Classname, artefact_config.Chance);
		}
		config_no_detector.Artefacts = artefacts;
		DetectorArtefacts config_detector = TransformConfigWithDetector(ArtefactsWithDetectors, Enable_Spawn_ArtefactsWithDetectors);
		
		
		ArtefactTriggerConfig config = new ArtefactTriggerConfig(config_no_detector, config_detector);
		return config;
	}
	
	private DetectorArtefacts TransformConfigWithDetector(ArtefactsWithDetectorsConfig config, bool enable_to_spawn = false)
	{
		DetectorArtefacts config_detector = new DetectorArtefacts();
		
		config_detector.EnableToSpawn = enable_to_spawn;
		config_detector.MaxCount = config.MaxInOnePoint;
		map<string, ref ConfigArtefacts> detectorList = new map<string, ref ConfigArtefacts>;
		
		foreach(ArtefactsDetectorListConfig detector: config.ArtefactsList)
		{
			ref ConfigArtefacts artefacts_class = new ConfigArtefacts();
			//artefacts_class.Artefacts = new map<string, int>;
			map<string, float> artefacts = new map<string, float>;
			foreach(ArtefactSettingsConfig artefact: detector.Detect_Artefacts_Settings)
			{
				artefacts.Set(artefact.Classname, artefact.Chance);
			}
			
			artefacts_class.Artefacts = artefacts;
			
			detectorList.Set(detector.Classname_Detector, artefacts_class);
		}
		
		config_detector.DetectorList = detectorList;
		
		return config_detector;
	}
	
	private void CreateArtefactTriggers(array<vector> poses)
	{
		if(Distance_To_Group_Art_Trigger == 0)
			Distance_To_Group_Art_Trigger = 12;
		
		//sort by coordinates
		poses.Sort();

		//devide by groups
		int group_index = 0;
		map<int, ref array<vector>> groups = new map<int, ref array<vector>>;
		array<vector> group = new array<vector>;
		for(int i = 0; i < poses.Count(); i++)
		{
			if(i == 0)
			{
				group.Insert(poses[i]);
				groups.Set(group_index, group);
			}
			
			if(i > 0)
			{
				if(vector.Distance(poses[i - 1], poses[i]) > Distance_To_Group_Art_Trigger)
				{
					group_index++;
					group = new array<vector>;
				}
				
				group.Insert(poses[i]);
				groups.Set(group_index, group);
			}
		}
		
		//find center of each group and create trigger with its extents
		if(groups.Count() > 0)
		{
			m_ArtefactTriggersDatas = new array<ref AnomaliesArtefactTriggerData>;
			PrintFormat("[AnomalySpawner]: %1 are devided to %2 groups anomalies", Classname_Anomaly_Object, groups.Count());
			for(int j = 0; j < groups.Count(); j++)
			{
				CreateAndConfigureArtefactTrigger(groups.Get(j));
			}
		}
	}
	
	private void CreateAndConfigureArtefactTrigger(array<vector> group)
	{
		vector position = vector.Zero;
		float radius;	
		float height = 1;	
		float distance;	
		
		float minY, maxY;
		vector pos;
		if(group.Count() > 1)
		{
			for(int k = 0; k < group.Count(); k++;)
			{
				pos = group.Get(k);
				
				if(minY == 0 || pos[1] < minY)
					minY = pos[1];
				
				if(maxY == 0 || pos[1] > maxY)
					maxY = pos[1];
				
				if(k > 0)
				{
					distance = vector.Distance(group.Get(k - 1), pos);
					if(radius == 0 || distance > radius)
						radius = distance;			
				}
				
				position[0] = position[0] + pos[0];
				position[2] = position[2] + pos[2];
			}
			
			position[0] = position[0]/group.Count();
			position[2] = position[2]/group.Count();
			
			position[1] = minY;
			
			height = maxY - minY;
		}
		else
		{
			position = group.Get(0);
			radius = 4;
			height = 2;
		}
		
		float add_radius = Additional_Distance_For_Art_Triggger;
		float add_height = Additional_Height_For_Art_Trigger;
		
		if(add_height == 0)
			add_height = 1.7;
		
		if(add_radius == 0)
			add_height = 0.5;
		
		//TODO: get anomaly height
		
		if(height < 2)
			height = 2;
		
		height += add_height;
		
		//TODO: get anomaly radius
		radius += add_radius;
		
		
		AnomaliesArtefactTriggerData art_trigger_data = new AnomaliesArtefactTriggerData(position, radius, height);
		
		string log;
		if(!m_Parent.IsHandlerExists())
		{
			art_trigger_data.Spawn(m_ArtefactTriggersConfig);
		}
		else
		{
			log = "This trigger is controlled by handler";	
		}
			
		m_ArtefactTriggersDatas.Insert(art_trigger_data);
		
		PrintFormat("[AnomalySpawner]: Created an artefact trigger with radius as %1 and height as %2 at %3 for %4 anomalies (%5). %6", radius, height, position.ToString(), group.Count(), Classname_Anomaly_Object, log);
	}
	
	//Calls by only by Handler when there are some players
	override void OnEnter()
	{
		//PrintFormat("[AnomalyTypeConfig][OnEnter]: IsOnlyTriggers is %1", m_Parent.IsOnlyTriggers());
		
		if(m_Parent.IsOnlyTriggers())
		{
			foreach(OOTS_AnomalyBase anomaly: m_Anomalies)
			{
				anomaly.CreateAreaDamage();
			}
		}
		else
		{
			//Print("m_AnomaliesPoses count is " + m_AnomaliesPoses.Count());
			
			OOTS_AnomalyBase new_anomaly;
			foreach(vector position: m_AnomaliesPoses)
			{
				new_anomaly = OOTS_AnomalyBase.Cast( GetGame().CreateObject(Classname_Anomaly_Object, position) );
				m_Anomalies.Insert(new_anomaly);
			}
		}
		
		if(m_ArtefactTriggersDatas)
		{
			foreach(AnomaliesArtefactTriggerData data: m_ArtefactTriggersDatas)
			{
				data.Spawn(m_ArtefactTriggersConfig);
			}
		}
	}
	
	//Calls by only by Handler when there are not players
	override void OnLeave()
	{
		//PrintFormat("[AnomalyTypeConfig][OnLeave]: IsOnlyTriggers is %1", m_Parent.IsOnlyTriggers());
		
		foreach(OOTS_AnomalyBase anomaly: m_Anomalies)
		{
			if(m_Parent.IsOnlyTriggers())
			{
				anomaly.DestroyAreaDamage();
			}
			else
				anomaly.Delete();
		}
		
		if(!m_Parent.IsOnlyTriggers())
		{
			m_Anomalies.Clear();
		}
		
		if(m_ArtefactTriggersDatas)
		{
			foreach(AnomaliesArtefactTriggerData data: m_ArtefactTriggersDatas)
			{
				data.Despawn();
			}
		}
	}
	
	override void Clear()
	{
		
	}
	
	override string GetTitle()
	{
		return string.Format("\"%1\"", Unique_Group_Title);
	}
}

class AnomaliesArtefactTriggerData
{
	private float 						m_Radius;
	private float 						m_Height;
	private vector 						m_Position;
	private ArtefactTrigger 			m_Trigger;
	
	private bool 						m_CanBeSpawned = true;
	
	void AnomaliesArtefactTriggerData(vector pos, float radius, float height)
	{
		m_Position = pos;
		m_Radius = radius;
		m_Height = height;
	}
	
	void ~AnomaliesArtefactTriggerData()
	{
		//Print("~AnomaliesArtefactTriggerData()");
		
		if(m_Trigger)
		{
			m_Trigger.Delete();
		}
	}
	
	void Spawn(ArtefactTriggerConfig config)
	{
		if(!m_CanBeSpawned)
			return;
		
		m_Trigger = ArtefactTrigger.Cast( GetGame().CreateObject("ArtefactTrigger", m_Position) );
		if(m_Trigger)
		{
			m_Trigger.SetParent(this);
			m_Trigger.Configure(config);
			m_Trigger.SetCollisionCylinder(m_Radius, m_Height);
		}
	}
	
	void Despawn()
	{
		if(m_Trigger)
		{
			m_Trigger.Delete();
		}
	}
	
	void OnDelete()
	{
		m_CanBeSpawned = false;
	}
}

class AnomalyHandlerTriggerCfg
{
	string 	shapedata;
	vector 	position;
	vector 	orientation;
	float 	enter_smothtime;
	float 	leave_smothtime;
	bool 	only_triggers;
	
	[NonSerialized()]
	private AnomalyListConfig m_Parent;
	[NonSerialized()]
	private AnomalyHandlerTrigger m_Trigger;
	[NonSerialized()]
	private ref array<Object> m_Insiders;
	[NonSerialized()]
	private ref Timer m_Timer;
	[NonSerialized()]
	private bool m_IsExists;
	
	void ~AnomalyHandlerTriggerCfg()
	{
		//Print("~AnomalyHandlerTriggerCfg()");
		
		if(m_Trigger)
			m_Trigger.Delete();
		
		if(m_Timer)
		{
			m_Timer.Stop();
			m_Timer = null;
		}
		
		if(m_Insiders)
		{
			m_Insiders.Clear();
			m_Insiders = null;
		}
	}
	
	bool IsShapeDataExits()
	{
		if(shapedata.Length() > 0)
		{
			m_IsExists = true;
		}
		
		return IsExists();
	}
	
	bool IsExists()
	{
		return m_IsExists;
	}
	
	void SetParent(AnomalyListConfig parent)
	{
		m_Parent = parent;
	}
	
	void Init()
	{
		if(shapedata.Length() > 0)
		{
			m_IsExists = true;
		}
		
		m_Trigger = AnomalyHandlerTrigger.Cast(GetGame().CreateObjectEx("AnomalyHandlerTrigger", position, ECE_NONE));
		if(!m_Trigger)
		{
			return;
		}
		
		TStringArray shapedata_arr = new TStringArray;
		shapedata.Split("|", shapedata_arr);
		
		string shape_log;
		int shape = shapedata_arr.Get(0).ToInt();
		switch(shape)
		{
			case TriggerShape.BOX:
				shape_log = string.Format("BOX shape, mins is %1, maxs is %2", shapedata_arr.Get(1), shapedata_arr.Get(2));
				m_Trigger.SetExtents(shapedata_arr.Get(1).BeautifiedToVector(), shapedata_arr.Get(2).BeautifiedToVector());
			break;
			case TriggerShape.SPHERE:
				shape_log = string.Format("SPHERE shape, radius is %1, maxs is %2", shapedata_arr.Get(1));
				m_Trigger.SetCollisionSphere(shapedata_arr.Get(1).ToFloat());
			break;
			case TriggerShape.CYLINDER:
				shape_log = string.Format("CYLINDER shape, radius is %1, height is %2", shapedata_arr.Get(1), shapedata_arr.Get(2));
				m_Trigger.SetCollisionCylinder(shapedata_arr.Get(1).ToFloat(), shapedata_arr.Get(2).ToFloat());
			break;
			default:
				m_Trigger.Delete();
				return;
		}
		
		
		m_Trigger.SetOrientation(orientation);
		m_Trigger.SetParent(this);
		
		PrintFormat("[AnomalySpawner]: Create handler trigger for %1 with %2", m_Parent.GetTitle(), shape_log);
		
		m_Insiders = new array<Object>;
	}
	
	
	
	void OnEnterServerEvent(Object object)
	{
		m_Insiders.Insert(object);
		
		//Print("OnEnterServerEvent");
		
		if(m_Insiders.Count() == 1)
		{
			if(enter_smothtime > 0)
			{
				m_Timer = new Timer();
				m_Timer.Run(enter_smothtime, m_Parent, "OnEnter");
			}
			else
				m_Parent.OnEnter();
		}
	}
	
	void OnLeaveServerEvent(Object object)
	{
		//Print("OnLeaveServerEvent");
		
		m_Insiders.RemoveItem(object);
		
		if(m_Insiders.Count() == 0)
		{
			if(leave_smothtime > 0)
			{
				m_Timer = new Timer();
				m_Timer.Run(leave_smothtime, m_Parent, "OnLeave");
			}
			else
				m_Parent.OnLeave();
		}	
	}
	
	bool IsOnlyTriggers()
	{
		return only_triggers;
	}
}

class AnomalySpawnByRadius
{
	vector 			position;
	bool 			use_chance;
	ref TIntArray 	count_minmax;
	ref TIntArray 	min_radius_minmax;
	ref TIntArray 	max_radius_minmax;
	ref TIntArray 	angle_minmax;
	
	void ~AnomalySpawnByRadius()
	{
		//Print("~AnomalySpawnByRadius()");
	}
	
	void Init()
	{
		if(angle_minmax.Count() == 0)
			angle_minmax = {0, 360};
	}
	
	bool CanSpawn()
	{
		if(max_radius_minmax.Count() == 2 && max_radius_minmax.Get(1) > 0 && count_minmax.Count() == 2)
			return true;
		
		return false;
	}
	
	int GetCount()
	{
		return Math.RandomIntInclusive(count_minmax.Get(0), count_minmax.Get(1));
	}
	
	bool UseChance()
	{
		return use_chance;
	}
		
	vector GetNextPosition()
	{
		int min_radius = GetParam(min_radius_minmax);
		int max_radius = GetParam(max_radius_minmax);
		int angle = GetParam(angle_minmax);
		vector direction = vector.RotateAroundZeroDeg(position, vector.Up, angle).Normalized();
		float distance = Math.RandomFloatInclusive(min_radius, max_radius);
		vector new_position = position + (direction * distance);
		
		return new_position;
	}
	
	private int GetParam(TIntArray data)
	{
		if(data.Count() == 2)
		{
			return Math.RandomIntInclusive(data.Get(0), data.Get(1));
		}
		
		return 0;
	}
}

class ArtefactsWithoutDetectorsConfig
{
	int 									MaxInOnePoint;
	ref array<ref ArtefactSettingsConfig> 	ArtefactsList;
}

class ArtefactsWithDetectorsConfig
{
	int 										MaxInOnePoint;
	ref array<ref ArtefactsDetectorListConfig> 	ArtefactsList;
}

class ArtefactsDetectorListConfig
{
	string 									Classname_Detector;
	ref array<ref ArtefactSettingsConfig> 	Detect_Artefacts_Settings;
}

class ArtefactSettingsConfig
{
	string 	Classname;
	float 	Chance;
}

class DetectorJSONConfig
{
	string 		ClassName;
	float 		PollingTime;
	float 		Radius;
	bool 		ShowDistance;
	string 		ShowDistanceMessage;
	bool 		ShowArtsCount;
	string 		ShowArtsCountMessage;
}

class AnomalyTeleportConfig: AnomalyBaseConfig
{
	string				Title;
	int 				Chance;
	ref array<vector> 	Entries;
	ref array<vector> 	Outputs;
	
	void ~AnomalyTeleportConfig()
	{
		//Print("~AnomalyTeleportConfig()");
		
		delete Entries;
		delete Outputs;
	}
	
	override void Spawn()
	{
		m_Anomalies = new array<OOTS_AnomalyBase>;
		
		if(m_Parent.IsHandlerExists() && !m_Parent.IsOnlyTriggers())
		{
			m_AnomaliesPoses = new array<vector>;
		}
		
		Anomaly_Teleport_Entry entry_teleport;
		Anomaly_Teleport_Output output_teleport;
		
		array<Anomaly_Teleport_Output> outputs_tp_array = new array<Anomaly_Teleport_Output>;
		foreach(vector entry_pos: Entries)
		{
			if(WillBeSpawned(Chance/100))
			{
				if(m_AnomaliesPoses)
				{
					m_AnomaliesPoses.Insert(entry_pos);
					PrintFormat("[AnomalySpawner]: Anomaly \"Anomaly_Teleport_Entry\" is controlled by handler at %1", entry_pos.ToString());
				}
				else
				{
					entry_teleport = Anomaly_Teleport_Entry.Cast( GetGame().CreateObject("Anomaly_Teleport_Entry", entry_pos) );
					if(entry_teleport)
					{
						m_Anomalies.Insert(entry_teleport);
						PrintFormat("[AnomalySpawner]: Create anomaly \"Anomaly_Teleport_Entry\" at %1", entry_pos.ToString());
						
						if(outputs_tp_array.Count() == 0) //creating outputs one time
						{
							foreach(vector output_pos: Outputs)
							{
								output_teleport = Anomaly_Teleport_Output.Cast( GetGame().CreateObject("Anomaly_Teleport_Output", output_pos) );
								outputs_tp_array.Insert(output_teleport);
								entry_teleport.AddOutput(output_teleport);
							}
						}
						else if(outputs_tp_array.Count() > 0) //adding outputs to entry
						{
							foreach(Anomaly_Teleport_Output output_teleport_item: outputs_tp_array)
							{
								entry_teleport.AddOutput(output_teleport_item);
							}
						}
						
						if(m_Parent.IsHandlerExists())
						{
							entry_teleport.DestroyAreaDamage();
						}
					}
				}
			}
			else
			{
				PrintFormat("[AnomalySpawner]: Skip anomaly \"Anomaly_Teleport_Entry\" at %1", entry_pos.ToString());
			}
		}
	}
	
	//Calls by only by Handler when there are some players
	override void OnEnter()
	{
		if(m_Parent.IsOnlyTriggers())
		{
			foreach(OOTS_AnomalyBase anomaly: m_Anomalies)
			{
				anomaly.CreateAreaDamage();
			}
		}
		else
		{
			//turn off Teleport manager
			GetAnomalyTeleportManager().SetAnomalySpawnerStatus(true);
			
			Anomaly_Teleport_Entry entry_teleport;
			Anomaly_Teleport_Output output_teleport;
			array<Anomaly_Teleport_Output> outputs_tp_array = new array<Anomaly_Teleport_Output>;
			
			foreach(vector position: m_AnomaliesPoses)
			{
				entry_teleport = Anomaly_Teleport_Entry.Cast( GetGame().CreateObject("Anomaly_Teleport_Entry", position) );
				if(entry_teleport)
				{
					m_Anomalies.Insert(entry_teleport);
					
					if(outputs_tp_array.Count() == 0) //creating outputs one time
					{
						foreach(vector output_pos: Outputs)
						{
							output_teleport = Anomaly_Teleport_Output.Cast( GetGame().CreateObject("Anomaly_Teleport_Output", output_pos) );
							outputs_tp_array.Insert(output_teleport);
							entry_teleport.AddOutput(output_teleport);
						}
					}
					else if(outputs_tp_array.Count() > 0) //adding outputs to entry
					{
						foreach(Anomaly_Teleport_Output output_teleport_item: outputs_tp_array)
						{
							entry_teleport.AddOutput(output_teleport_item);
						}
					}
				}
			}
			
			//turn on Teleport manager
			GetAnomalyTeleportManager().SetAnomalySpawnerStatus(false);
		}
	}
	
	//Calls by only by Handler when there are not players
	override void OnLeave()
	{
		foreach(OOTS_AnomalyBase anomaly: m_Anomalies)
		{
			if(m_Parent.IsOnlyTriggers())
			{
				anomaly.DestroyAreaDamage();
			}
			else
				anomaly.Delete();
		}
		
		if(!m_Parent.IsOnlyTriggers())
		{
			m_Anomalies.Clear();
		}
	}
	
	override void Clear()
	{
		if(m_Anomalies.Count() > 0)
		{
			foreach(EntityAI amonaly: m_Anomalies)
			{
				if(amonaly)
				{
					amonaly.Delete();
				}
			}
		}
		
		/*if(m_Handler)
		{
			m_Handler = null;
		}*/
	}
	
	override string GetTitle()
	{
		return string.Format("\"%1\"", Title);
	}
}