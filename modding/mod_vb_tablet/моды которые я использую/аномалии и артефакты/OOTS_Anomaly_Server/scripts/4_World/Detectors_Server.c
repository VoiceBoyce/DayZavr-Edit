modded class Anomaly_DetectorBase
{
	protected float DISTANCE_SPAWN = 2;
	protected float DISTANCE_CLOSE = 3.5;
	protected float DISTANCE_MID = 6;
	protected float DISTANCE_FAR = 10;
	
	protected ref DetectorConfig m_Config;

	override void EEInit()
	{
		super.EEInit();
		
		/* DetectorConfig config = AnomalySpawner.GetDetectorConfig(GetType());
		if(config)
		{
			SetIsDetector(config);
		} */
		
		AnomalySpawner.RegisterDetectorToConfigure(this);
	}
	
	void SetIsDetector(DetectorConfig config)
	{
		m_Config = config;
		DISTANCE_FAR = m_Config.GetRadius();
		DISTANCE_MID = DISTANCE_FAR * 0.6;
		DISTANCE_CLOSE = DISTANCE_FAR * 0.4;
		DISTANCE_SPAWN = DISTANCE_FAR * 0.2;
	}
	
	bool IsDetector()
	{
		return true;
	}
	
	protected bool GetClosestArt(out ArtefactForDetector art, out float distance)
	{
		float radius = DISTANCE_FAR;
		if(!m_Config)
		{
			PrintFormat("!!!ERROR: Detector %1 is not configured", GetType());
		}
		
		map<ref ArtefactForDetector, float> artefacts = AnomalySpawner.GetArtefactsForDetectorInRadius(GetPosition(), radius);
		if(artefacts.Count() == 0)
		{
			return false;
		}
		
		distance = 1000;
		int index = -1;
		for(int i = 0; i < artefacts.Count(); i++)
		{
			if(distance > artefacts.GetElement(i))
			{
				distance = artefacts.GetElement(i);
				index = i;
			}
		}
		
		if(index == -1)
			return false;
		
		art = artefacts.GetKey(index);
		return true;
	}
	
	protected int GetArtRange(vector art_pos)
	{
		float distance = vector.Distance(GetPosition(), art_pos);
		
		return GetArtRangeByDistance(distance);
	}
	
	protected int GetArtRangeByDistance(float distance)
	{
		int range;
		if(distance >= DISTANCE_FAR)
		{ 
			range = DetectorDistancesSignals.DISTANCE_NONE;
		}
		else if(distance >= DISTANCE_MID && distance < DISTANCE_FAR)
		{
			range = DetectorDistancesSignals.DISTANCE_FAR;
		}
		else if(distance >= DISTANCE_CLOSE && distance < DISTANCE_MID)
		{
			range = DetectorDistancesSignals.DISTANCE_MIDDLE;
		}
		else if(distance >= DISTANCE_SPAWN && distance < DISTANCE_CLOSE)
		{
			range = DetectorDistancesSignals.DISTANCE_CLOSE;
		}
		else if(distance < DISTANCE_SPAWN)
		{
			range = DetectorDistancesSignals.DISTANCE_SPAWN;
		}
		
		return range;
	}
	
	protected float GetArtAngleCorrection()
	{
		return 0;
	}
	
	protected float GetArtAngle(vector art_pos)
	{
		art_pos[1] = 0;
		
		vector det_pos = GetPosition();
		det_pos[1] = 0;
		
		float det_yaw = GetOrientation()[0] - 180;
		if(det_yaw < 0)
			det_yaw = 360 + det_yaw;

		vector det_to_art_dir = vector.Direction(art_pos, det_pos).Normalized();
		//det_to_art_dir[1] = 0;
		
		float yaw = det_to_art_dir.VectorToAngles()[0];

		float angle_diff = yaw - det_yaw;

		angle_diff += GetArtAngleCorrection();
		if(angle_diff < 0)
			angle_diff = 360 + angle_diff;
		
		return angle_diff;
	}
	
	override protected void CheckArt()
	{
		
	}
	
	protected void SpawnArtefact(ArtefactForDetector art)
	{
		string type = art.GetType();
		vector pos = art.GetPos();
		
		art.SetSpawned();
		AnomalySpawner.RemoveArtefact(art);
		
		bool is_art = true;
        int iFlags = ECE_SETUP|ECE_KEEPHEIGHT|ECE_PLACE_ON_SURFACE;
        if(GetGame().IsKindOf( type, "DZ_LightAI" ) || GetGame().IsKindOf(type, "SurvivorBase"))
        {
            iFlags = ECE_INITAI|ECE_CREATEPHYSICS|iFlags;
            is_art = false;
        }
        if(is_art)
        {
            ItemBase artefact = ItemBase.Cast( GetGame().CreateObjectEx(type, pos, iFlags) );
            if(artefact)
            {
                //artefact.SendDetectorSoundRPC(DetectorDistancesSignals.ARTEFACT_SPAWN);
            }
        }
        else 
            GetGame().CreateObjectEx(type, pos, iFlags);
		
		
		GetGame().RPCSingleParam(this, ART_SPAWN_SOUND_RPC, new Param1<vector>(pos), true);
	}
		
	//====DEBUG=====
	#ifdef DIAG
	protected ref array<Apple> m_DebugApples = new array<Apple>;
	
	protected void FindDebugApples()
	{
		if(!m_Config)
		{
			DetectorJSONConfig json = new DetectorJSONConfig;
			json.Radius = 10;
			SetIsDetector(new DetectorConfig(json));
		}
		
		array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), 12, objects, null);
		Apple apple;
		array<Apple> arr = new array<Apple>;
		foreach(Object obj: objects)
		{
			apple = Apple.Cast(obj);
			if(apple && m_DebugApples.Find(apple) == INDEX_NOT_FOUND)
			{
				m_DebugApples.Insert(apple);
				AnomalySpawner.AddArtefact(new ArtefactForDetector("Plum", apple.GetPosition()));
			}
		}
	}
	#endif;
	//==============
}

modded class Anomaly_Detector_Otklik
{
	override protected float GetArtAngleCorrection()
	{
		return 30;
	}
	
	override protected void CheckArt()
	{
		vector art_pos;
		ArtefactForDetector art;
		float distance;
		
		//====DEBUG=====
		#ifdef DIAG
		FindDebugApples();
		#endif;
		//==============
		
		if(!GetClosestArt(art, distance))
		{
			if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			{
				m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
				SetSynchDirty();
			}
			return;
		}
		
		art_pos = art.GetPos();
		
		m_RangeState = GetArtRangeByDistance(distance);
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_NONE)
		{
			SetSynchDirty();
			return;
		}
		
		float angle = GetArtAngle(art_pos);
		
		if(angle < 15 || angle > 165)
		{
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
			SetSynchDirty();
			return;
		}
		
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_SPAWN && art)
		{
			SpawnArtefact(art);
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
		}
		
		SetSynchDirty();
	}
}

modded class Anomaly_Detector_Oko
{
	override protected float GetArtAngleCorrection()
	{
		return 30;
	}
	
	override protected void CheckArt()
	{
		vector art_pos;
		ArtefactForDetector art;
		float distance;
		
		//====DEBUG=====
		#ifdef DIAG
		FindDebugApples();
		#endif;
		//==============
		
		if(!GetClosestArt(art, distance))
		{
			if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			{
				m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
				SetSynchDirty();
			}
			return;
		}
		
		art_pos = art.GetPos();
		
		m_RangeState = GetArtRangeByDistance(distance);
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_NONE)
		{
			SetSynchDirty();
			return;
		}
		
		float angle = GetArtAngle(art_pos);
		
		if(angle < 15 || angle > 165)
		{
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
			SetSynchDirty();
			return;
		}
		
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_SPAWN && art)
		{
			SpawnArtefact(art);
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
		}
		
		SetSynchDirty();
	}
}


modded class Anomaly_Detector_Medved
{

	private const int STEP = 15;
	
	override protected float GetArtAngleCorrection()
	{
		return 35;
	}
	
	override protected void CheckArt()
	{
		vector art_pos;
		ArtefactForDetector art;
		float distance;
		
		//====DEBUG=====
		#ifdef DIAG
		FindDebugApples();
		#endif;
		//==============
		
		if(!GetClosestArt(art, distance))
		{
			if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			{
				m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
				SetSynchDirty();
			}
			return;
		}
			

		art_pos = art.GetPos();
		
		m_RangeState = GetArtRangeByDistance(distance);
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_NONE)
		{
			SetSynchDirty();
			return;
		}
		
		float angle = GetArtAngle(art_pos);
		
		if(angle < 15 || angle > 165)
		{
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
			SetSynchDirty();
			return;
		}
		
		int cur_step = angle / STEP;
		m_AnimPhase = cur_step * STEP;
		
		if(m_RangeState == DetectorDistancesSignals.DISTANCE_SPAWN && art)
		{
			SpawnArtefact(art);
			m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
		}
		
		SetSynchDirty();
	}
}

modded class Anomaly_Detector_Svarog
{
	private const int MIN_ANGLE = -90;
	private const int MAX_ANGLE = 90;
	
	private const int SLOT_MAX_COUNT = 4;
	private ref array<ref ArtefactForDetector> m_ArtSlots = new array<ref ArtefactForDetector>;
	
	override protected float GetArtAngleCorrection()
	{
		return 25;
	}
	
	private void FillArtSlots()
	{
		TIntArray indexes = new TIntArray;
		
		foreach(int a, ArtefactForDetector art: m_ArtSlots)
		{
			if(art.IsSpawned())
			{
				indexes.Insert(a);
			}
		}
		foreach(int b: indexes)
		{
			m_ArtSlots.Remove(b);
		}
		
		if(m_ArtSlots.Count() == SLOT_MAX_COUNT)
			return;
		
		
		if(!m_Config)
		{
		  PrintFormat("!!!ERROR: Detector %1 is not configured", GetType());
		  return;
		}
		
		map<ref ArtefactForDetector, float> artefacts = AnomalySpawner.GetArtefactsForDetectorInRadius(GetPosition(), m_Config.GetRadius());
		if(artefacts.Count() == 0)
			return;
		
		int rest_slot_count = SLOT_MAX_COUNT - m_ArtSlots.Count();
		for(int i = 0; i < artefacts.Count(); i++)
		{
			if(rest_slot_count == 0)
				return;
			
			float angle = GetArtAngle(artefacts.GetKey(i).GetPos());
			if(angle < 0 || angle > 180)
				continue;
			
			if(m_ArtSlots.Find(artefacts.GetKey(i)) == INDEX_NOT_FOUND)
			{
				m_ArtSlots.Insert(artefacts.GetKey(i));
				rest_slot_count--;
			}
		}
	}
	
	
	override protected void CheckArt()
	{
		//=====DEBUG=====
		#ifdef DIAG
		FindDebugApples();
		#endif;
		//===============
		
		FillArtSlots();

		if(m_ArtSlots.Count() == 0)
		{
			if(m_Art1Dist != -1 || m_Art2Dist != -1 || m_Art3Dist != -1 || m_Art4Dist != -1)
			{
				for(int j = 0; j < SLOT_MAX_COUNT; j++)
				{
					SetValues(j, -1, 0.5);
				}
				
				SetSynchDirty();
			}
		
			return;
		}
		
		bool need_to_sync = false;
		for(int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			need_to_sync |= CalcArtData(i);
		}

		if(need_to_sync)
			SetSynchDirty();
	}
	
	private bool CalcArtData(int art_slot)
	{
		ArtefactForDetector art = m_ArtSlots.Get(art_slot);
		if(!art)
		{
			m_ArtSlots.Remove(art_slot);
			SetValues(art_slot, -1, 0.5);
			return true;
		}
		
		vector art_pos = art.GetPos();
		
		float rel_distance = -1;
		float abs_distance = vector.Distance(GetPosition(), art_pos);

		if(abs_distance > DISTANCE_FAR)
		{
			SetValues(art_slot, -1, 0.5);
			m_ArtSlots.Remove(art_slot);
			return true;
		}
		
		rel_distance = Math.InverseLerp(DISTANCE_FAR, DISTANCE_SPAWN, abs_distance);
		
		float angle = GetArtAngle(art_pos);
		if(angle < 0 || angle > 180)
		{
			SetValues(art_slot, -1, 0.5);
			m_ArtSlots.Remove(art_slot);
			return true;
		}
		
		if(abs_distance < DISTANCE_SPAWN)
		{
			m_ArtSlots.Remove(art_slot);
			SpawnArtefact(art);
			SetValues(art_slot, -1, 0.5);
			return true;
		}
		
		angle = Math.InverseLerp(MIN_ANGLE, MAX_ANGLE, (angle - 90));
		
		SetValues(art_slot, rel_distance, angle);
		
		return true;
	}
	
	private void SetValues(int art_slot, float distance, float angle)
	{
		switch(art_slot)
		{
			case 0:
				m_Art1Dist = distance;
				m_Art1Angle = angle;
			break;
			case 1:
				m_Art2Dist = distance;
				m_Art2Angle = angle;
			break;
			case 2:
				m_Art3Dist = distance;
				m_Art3Angle = angle;
			break;
			case 3:
				m_Art4Dist = distance;
				m_Art4Angle = angle;
			break;
		}
	}
	override void OnWorkStop()
    {
        super.OnWorkStop();
		//m_ArtSlots.Clear();
	}
	override void EEItemLocationChanged( notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc )
    {
        super.EEItemLocationChanged(oldLoc, newLoc);
		if (newLoc.GetType() == InventoryLocationType.HANDS)
			//m_ArtSlots.Clear();
    }
}


modded class Anomaly_Detector_Veles
{
	private const int MIN_ANGLE = -90;
	private const int MAX_ANGLE = 90;
	
	private const int SLOT_MAX_COUNT = 4;
	private ref array<ref ArtefactForDetector> m_ArtSlots = new array<ref ArtefactForDetector>;
	
	override protected float GetArtAngleCorrection()
	{
		return 25;
	}
	
	private void FillArtSlots()
	{
		TIntArray indexes = new TIntArray;
		
		foreach(int a, ArtefactForDetector art: m_ArtSlots)
		{
			if(art.IsSpawned())
			{
				indexes.Insert(a);
			}
		}
		foreach(int b: indexes)
		{
			m_ArtSlots.Remove(b);
		}
		
		if(m_ArtSlots.Count() == SLOT_MAX_COUNT)
			return;
		
		if(!m_Config)
		{
		  PrintFormat("!!!ERROR: Detector %1 is not configured", GetType());
		  return;
		}
		
		
		map<ref ArtefactForDetector, float> artefacts = AnomalySpawner.GetArtefactsForDetectorInRadius(GetPosition(), m_Config.GetRadius());
		if(artefacts.Count() == 0)
			return;
		
		int rest_slot_count = SLOT_MAX_COUNT - m_ArtSlots.Count();
		for(int i = 0; i < artefacts.Count(); i++)
		{
			if(rest_slot_count == 0)
				return;
			
			float angle = GetArtAngle(artefacts.GetKey(i).GetPos());
			if(angle < 0 || angle > 180)
				continue;
			
			if(m_ArtSlots.Find(artefacts.GetKey(i)) == INDEX_NOT_FOUND)
			{
				m_ArtSlots.Insert(artefacts.GetKey(i));
				rest_slot_count--;
			}
		}
	}
	
	
	override protected void CheckArt()
	{
		//=====DEBUG=====
		#ifdef DIAG
		FindDebugApples();
		#endif;
		//===============
		
		FillArtSlots();

		if(m_ArtSlots.Count() == 0)
		{
			if(m_Art1Dist != -1 || m_Art2Dist != -1 || m_Art3Dist != -1 || m_Art4Dist != -1)
			{
				for(int j = 0; j < SLOT_MAX_COUNT; j++)
				{
					SetValues(j, -1, 0.5);
				}
				
				SetSynchDirty();
			}
		
			return;
		}
		
		bool need_to_sync = false;
		for(int i = 0; i < SLOT_MAX_COUNT; i++)
		{
			need_to_sync |= CalcArtData(i);
		}

		if(need_to_sync)
			SetSynchDirty();
	}
	
	private bool CalcArtData(int art_slot)
	{
		ArtefactForDetector art = m_ArtSlots.Get(art_slot);
		if(!art)
		{
			m_ArtSlots.Remove(art_slot);
			SetValues(art_slot, -1, 0.5);
			return true;
		}
		
		vector art_pos = art.GetPos();
		
		float rel_distance = -1;
		float abs_distance = vector.Distance(GetPosition(), art_pos);

		if(abs_distance > DISTANCE_FAR)
		{
			SetValues(art_slot, -1, 0.5);
			m_ArtSlots.Remove(art_slot);
			return true;
		}
		
		rel_distance = Math.InverseLerp(DISTANCE_FAR, DISTANCE_SPAWN, abs_distance);
		
		float angle = GetArtAngle(art_pos);
		if(angle < 0 || angle > 180)
		{
			SetValues(art_slot, -1, 0.5);
			m_ArtSlots.Remove(art_slot);
			return true;
		}
		
		if(abs_distance < DISTANCE_SPAWN)
		{
			m_ArtSlots.Remove(art_slot);
			SpawnArtefact(art);
			SetValues(art_slot, -1, 0.5);
			return true;
		}
		
		angle = Math.InverseLerp(MIN_ANGLE, MAX_ANGLE, (angle - 90));
		
		SetValues(art_slot, rel_distance, angle);
		
		return true;
	}
	
	private void SetValues(int art_slot, float distance, float angle)
	{
		switch(art_slot)
		{
			case 0:
				m_Art1Dist = distance;
				m_Art1Angle = angle;
			break;
			case 1:
				m_Art2Dist = distance;
				m_Art2Angle = angle;
			break;
			case 2:
				m_Art3Dist = distance;
				m_Art3Angle = angle;
			break;
			case 3:
				m_Art4Dist = distance;
				m_Art4Angle = angle;
			break;
		}
	}
	override void OnWorkStop()
    {
        super.OnWorkStop();
		//m_ArtSlots.Clear();
	}
	override void EEItemLocationChanged( notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc )
    {
        super.EEItemLocationChanged(oldLoc, newLoc);
		if (newLoc.GetType() == InventoryLocationType.HANDS)
			//m_ArtSlots.Clear();
    }
	
}