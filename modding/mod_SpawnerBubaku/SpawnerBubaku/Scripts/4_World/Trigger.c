class BubakTrigger extends Trigger
{
    protected string m_TriggerName;
	protected string m_TriggerNotification;
	protected int m_TriggerNotificationTime;
	protected ref TStringArray m_SpawnLocations;
	protected ref TStringArray m_Bubaci;
	protected ref TStringArray m_BubaciInventory;
	protected float m_SpawnRadius;
	protected int m_TriggerDelay;
	protected int m_BubakNum;
	protected int m_LastTriggerTime;
	protected int m_OnlyFillUpToBubaknum;
	protected bool m_RandomDmg;
	protected string m_WorkingHours;
	protected ref TStringArray m_TriggerDependency;
	protected bool m_ShouldSpawnAI;

	// Управляемый режим для маппинга, который стоит над рельефом.
	// Вдали от игроков зомби проваливаются сквозь такой пол на землю, поэтому держим их только рядом с игроками.
	protected const int MANAGED_SPAWN_PER_TICK = 6;
	protected const int MANAGED_POINT_TRIES = 15;
	protected const float MANAGED_MIN_PLAYER_DIST = 12.0;	// Не появляться перед носом у игрока
	protected const float MANAGED_KEEP_FACTOR = 1.15;		// Удалять чуть дальше, чем спавнить, чтобы зомби не мигали

	protected float m_ManagedRadius;
	protected float m_ManagedMinY;
	protected float m_ManagedMaxY;
	protected float m_ManagedZone;
	protected int m_ManagedBudget;	// Сколько ещё можно создать до следующего отката триггера
	protected ref TVectorArray m_ManagedPoints;
	protected ref array<EntityAI> m_ManagedBubaci;

	void AdditionalAction()
	{
		// Override this, if you need custom actions serverside
	}

	void SetTriggerName(string name)
	{
		m_TriggerName = name;
		m_ShouldSpawnAI = false;
	}

	string GetTriggerName()
	{
		return m_TriggerName;
	}

	bool ShouldSpawnAI()
	{
		#ifdef AI_BANDITS
		DynamicAIBConfig config = GetDayZGame().GetDynamicAIBConfig();
		if (config)
		{
			for (int i=0; i < config.GroupLocations.Count(); i++)
			{
				if (config.GroupLocations.Get(i).name == GetTriggerName())
				{
					m_ShouldSpawnAI = true;
				}
			}
		}
		#endif
		return m_ShouldSpawnAI;
	}

	void SetTriggerDependency(TStringArray dependency)
	{
		m_TriggerDependency = dependency;
	}

	void SetTriggerNotification(string notification)
	{
		m_TriggerNotification = notification;
	}

	string GetTriggerNotification()
	{
		return m_TriggerNotification;
	}

	void SetTriggerNotificationTime(int notificationtime)
	{
		m_TriggerNotificationTime = notificationtime;
	}

	int GetTriggerNotificationTime()
	{
		return m_TriggerNotificationTime;
	}

	void SetSpawnLocations(TStringArray spawnloc)
	{
		m_SpawnLocations = spawnloc;
	}

	TStringArray GetSpawnLocations()
	{
		return m_SpawnLocations;
	}
	void SetSpawnRadius(float radius)
	{
		m_SpawnRadius = radius;
	}

	float GetSpawnRadius()
	{
		return m_SpawnRadius;
	}
	/*
	void SetSpawnChance(float chance)
	{
		m_SpawnChance = chance;
	}
	*/

	void SetBubaci(TStringArray bubaci)
	{
		m_Bubaci = bubaci;
	}

	TStringArray GetBubaci()
	{
		return m_Bubaci;
	}

	void SetBubaciInventory(TStringArray inventory)
	{
		m_BubaciInventory = inventory;
	}

	TStringArray GetBubaciInventory()
	{
		return m_BubaciInventory;
	}


	void SetTriggerDelay(int delay)
	{
		m_TriggerDelay = delay;
	}

	void SetLastTriggerTime(int time)
	{
		m_LastTriggerTime = time;
	}

	void SetBubakNum(int bubaknum)
	{
		m_BubakNum = bubaknum;
	}

	void SetOnlyFillUpToBubaknum(int onlyFillUpToBubaknum)
	{
		m_OnlyFillUpToBubaknum = onlyFillUpToBubaknum;
	}

	void SetRandomDamage(bool dmg)
	{
		m_RandomDmg = dmg;
	}
	void SetWorkingHours(string hrs)
	{
		m_WorkingHours = hrs;
	}

	bool CanTriggerAction(int time)
	{
		if (time > m_LastTriggerTime+m_TriggerDelay)
		{
			m_LastTriggerTime = time;
			return true;
		}

		return false;
	}
	bool IsTriggerValid()
	{
		//add configurable time of validity
		if (m_LastTriggerTime > 0 && (GetGame().GetTime()/1000) < m_LastTriggerTime+300)
		{
			return true;
		}
		return false;
	}

	bool IsDependencyMet()
	{
		if (m_TriggerDependency.Count()>0)
		{
			for (int i=0; i < m_TriggerDependency.Count(); i++)
			{

				if (m_TriggerDependency.Get(i).Length()>0)
				{
					SPBLogger.GetInstance().Log( "Trigger " + m_TriggerName + " have dependency on trigger " + m_TriggerDependency.Get(i) + " classname " + GetTrigger(m_TriggerDependency.Get(i)) );
					if ( GetTrigger(m_TriggerDependency.Get(i)) && !GetTrigger(m_TriggerDependency.Get(i)).IsTriggerValid())
					{
						return false;
					}
				}


			}

		}
		return true;
	}
	BubakTrigger GetTrigger(string name)
	{
		BubakTrigger trigger;
		SpawnerBubaku plugin = SpawnerBubaku.Cast(GetPluginManager().GetPluginByType(SpawnerBubaku));
		array<BubakTrigger> triggers = plugin.GetTriggers();
		for (int i=0; i< triggers.Count(); i++)
		{
			if (triggers.Get(i).GetTriggerName() == name)
			{
				return triggers.Get(i);
			}
		}
		return null;
	}

	override bool CanAddObjectAsInsider(Object object)
    {
        PlayerBase player = PlayerBase.Cast(object);
        return player != null;
    }

	void SetManaged(float radius, float minY, float maxY, float zone)
	{
		m_ManagedRadius = radius;
		m_ManagedMinY = minY;
		m_ManagedMaxY = maxY;
		m_ManagedZone = zone;

		if (!IsManaged())
			return;

		m_ManagedBubaci = new array<EntityAI>;
		m_ManagedPoints = new TVectorArray;
		for (int i = 0; i < m_SpawnLocations.Count(); i++)
		{
			TStringArray parts = new TStringArray;
			m_SpawnLocations.Get(i).Split("|", parts);
			m_ManagedPoints.Insert(parts.Get(0).ToVector());
		}
	}

	bool IsManaged()
	{
		return m_ManagedRadius > 0;
	}

	protected void FindPlayersInside(out array<vector> inside, out Man firstPlayer)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		vector center = GetPosition();
		center[1] = 0;

		for (int i = 0; i < players.Count(); i++)
		{
			Man man = players.Get(i);
			if (!man || !man.IsAlive())
				continue;

			vector pos = man.GetPosition();
			if (pos[1] < m_ManagedMinY || pos[1] > m_ManagedMaxY)
				continue;

			vector flat = pos;
			flat[1] = 0;
			if (vector.DistanceSq(flat, center) > m_ManagedZone * m_ManagedZone)
				continue;

			inside.Insert(pos);
			if (!firstPlayer)
				firstPlayer = man;
		}
	}

	protected float NearestPlayerDistance(vector position, array<vector> inside)
	{
		float best = -1;
		for (int i = 0; i < inside.Count(); i++)
		{
			float dist = vector.Distance(position, inside.Get(i));
			if (best < 0 || dist < best)
				best = dist;
		}
		return best;
	}

	protected void ManagedCleanup(array<vector> inside)
	{
		float keepDistance = m_ManagedRadius * MANAGED_KEEP_FACTOR;

		for (int i = m_ManagedBubaci.Count() - 1; i >= 0; i--)
		{
			EntityAI bubak = m_ManagedBubaci.Get(i);

			// Убитый не возвращается в запас: он появится только после отката триггера
			if (!bubak || !bubak.IsAlive())
			{
				m_ManagedBubaci.Remove(i);
				continue;
			}

			vector position = bubak.GetPosition();
			float nearest = NearestPlayerDistance(position, inside);
			if (position[1] < m_ManagedMinY || nearest < 0 || nearest > keepDistance)
			{
				GetGame().ObjectDelete(bubak);
				m_ManagedBubaci.Remove(i);
				m_ManagedBudget++;
			}
		}
	}

	protected void ManagedSpawn(array<vector> inside)
	{
		int spawned = 0;

		for (int tryIndex = 0; tryIndex < MANAGED_SPAWN_PER_TICK * MANAGED_POINT_TRIES; tryIndex++)
		{
			if (spawned >= MANAGED_SPAWN_PER_TICK || m_ManagedBudget <= 0 || m_ManagedBubaci.Count() >= m_BubakNum)
				return;

			vector point = m_ManagedPoints.GetRandomElement();
			float nearest = NearestPlayerDistance(point, inside);
			if (nearest < MANAGED_MIN_PLAYER_DIST || nearest > m_ManagedRadius)
				continue;

			string bubak = GetBubaci().GetRandomElement();
			if (bubak.Contains("|"))
			{
				TStringArray bubchance = new TStringArray;
				bubak.Split("|", bubchance);
				bubak = bubchance.Get(0);
				if (bubchance.Get(1).ToFloat() < Math.RandomFloatInclusive(0, 1.0))
					continue;
			}

			EntityAI created = EntityAI.Cast(SpawnerBubaku_CreateObject(bubak, point, false, true, true));
			if (!created)
				continue;

			m_ManagedBubaci.Insert(created);
			m_ManagedBudget--;
			spawned++;
		}
	}

	void ManagedTick()
	{
		array<vector> inside = new array<vector>;
		Man firstPlayer;
		FindPlayersInside(inside, firstPlayer);

		ManagedCleanup(inside);

		if (inside.Count() == 0 || !IsWorkingTime() || !IsDependencyMet())
			return;

		if (CanTriggerAction(GetGame().GetTime() / 1000))
		{
			m_ManagedBudget = Math.Max(m_BubakNum - m_ManagedBubaci.Count(), 0);
			AdditionalAction();
			if (GetTriggerNotification() != "")
				NotificationSystem.SendNotificationToPlayerExtended(firstPlayer, GetTriggerNotificationTime(), GetTriggerName(), GetTriggerNotification());
			SPBLogger.GetInstance().Log("Managed trigger " + GetTriggerName() + " budget " + m_ManagedBudget, SPBLogger.LOGLEVEL_DEBUG);
		}

		ManagedSpawn(inside);
	}

	override void OnEnter(Object obj)
    {
		// В управляемом режиме всё делает ManagedTick
		if (IsManaged())
			return;

		SPBLogger.GetInstance().Log( "SpawnerBubaku ENTER" );

		if (obj.IsMan() && GetGame().IsServer() && IsWorkingTime() && IsDependencyMet())
		{
			// porovnat s ulozenym casem tiku, kdyz je vetsi nez cooldown ulozit novy a povolit akci
			//GetGame().CreateObject("Seachest", obj.GetPosition() );
			//GetGame().CreateObject("ZmbM_ClerkFat_White", obj.GetPosition(), false, true, true );
			SPBLogger.GetInstance().Log( "Triggered " + GetTriggerName() + " time " + GetGame().GetTime()/1000, SPBLogger.LOGLEVEL_DEBUG);
			if (CanTriggerAction(GetGame().GetTime()/1000))
			{
				if (PlayerBase.Cast(obj) && PlayerBase.Cast(obj).GetIdentity())
				{
					if (PlayerBase.Cast(obj).GetIdentity().GetName())
					{
						SPBLogger.GetInstance().Log("Can trigger action " + GetGame().GetTime()/1000 + " last " + m_LastTriggerTime + " trigger delay " + m_TriggerDelay);
						SetLastTriggerTime(GetGame().GetTime()/1000);
						#ifdef AI_BANDITS
						if (ShouldSpawnAI())
						{
							SpawniAI( GetTriggerName(),  GetSpawnLocations().GetRandomElement());
						}
						else
						{
							SpawniBubaky();
						}
						#else
						SpawniBubaky();
						#endif
						AdditionalAction();
						if (GetTriggerNotification() != "")
						{
							NotificationSystem.SendNotificationToPlayerExtended(Man.Cast(obj), GetTriggerNotificationTime(), GetTriggerName(), GetTriggerNotification());
						}
						SPBLogger.GetInstance().Log( PlayerBase.Cast(obj).GetIdentity().GetName() + " triggered " + GetTriggerName(), SPBLogger.LOGLEVEL_DEBUG);
					}
				}
			}
			else
			{
				SPBLogger.GetInstance().Log("Cannot trigger action it was called at " +  m_LastTriggerTime );
			}
		}

    }

    override void OnLeave(Object obj)
    {
		SPBLogger.GetInstance().Log( "SpawnerBubaku EXIT" );
    }

	/*
    override void EOnInit(IEntity other, int extra)
    {
        const vector mins = "-1 -0.2 -1";
        const vector maxs = "1 1 1";
        SetExtents(mins, maxs);
    }
	*/

	int SpawnerBubaku_GetActiveObjectsNum()
	{
		SpawnerBubaku plugin = SpawnerBubaku.Cast(GetPluginManager().GetPluginByType(SpawnerBubaku));
		TIntArray spawned_instances = plugin.GetSpawnedInstances(GetID());
		if(spawned_instances) return spawned_instances.Count();
		return 0;
	}

	Object SpawnerBubaku_CreateObject(string type, vector pos, bool create_local = false, bool init_ai = false, bool create_physics = true)
	{
		auto newObject = GetGame().CreateObject(type, pos, create_local, init_ai, create_physics);
		if (newObject)
		{
			if (!EntityAI.Cast(newObject).IsDayZCreature() && m_RandomDmg)
			{
				float rndhealth = Math.RandomFloat(newObject.GetMaxHealth("", "")*0.1, newObject.GetMaxHealth("", ""));
				newObject.SetHealth("", "", rndhealth);
			}
			if (EntityAI.Cast(newObject).IsDayZCreature() && GetBubaciInventory() && GetBubaciInventory().Count() > 0 && Math.RandomIntInclusive(0, 100) > 50)
			{
				EntityAI.Cast(newObject).GetInventory().CreateInInventory(GetBubaciInventory().GetRandomElement());
			}
			SPBLogger.GetInstance().Log("Created bubak: " + newObject.GetID());
			SPBLogger.GetInstance().Log("pos: " + pos.ToString());
		}

		if(m_OnlyFillUpToBubaknum && newObject && !IsManaged())
		{
			ZombieBase zombie = ZombieBase.Cast(newObject);
			if(zombie)
			{
				zombie.SetTriggerId(GetID());
				SpawnerBubaku plugin = SpawnerBubaku.Cast(GetPluginManager().GetPluginByType(SpawnerBubaku));
				plugin.AddSpawnedInstance(GetID(), zombie.GetID());
			}
		}

		return newObject;
	}

	#ifdef AI_BANDITS
	void SpawniAI(string name, string position)
	{
		DynamicAIBManager.GetInstance().SpawnGroup(name, position);
	}
	#endif

	void SpawniBubaky()
	{
		vector randvec, randompos, spawnpos;
		string randvecstr;
		float yko, spawnchance, rndflt;
		int rndnum,i;
		int j = 0;
		bool rotated;
		string posrot, pos,ori, bubak;
		TStringArray loc, bubchance;

		SPBLogger.GetInstance().Log("m_OnlyFillUpToBubaknum: " + m_OnlyFillUpToBubaknum);
		SPBLogger.GetInstance().Log("m_BubakNum: " + m_BubakNum);
		SPBLogger.GetInstance().Log("m_SpawnLocations.Count(): " + m_SpawnLocations.Count());
		SPBLogger.GetInstance().Log("SpawnerBubaku_GetActiveObjectsNum(): " + SpawnerBubaku_GetActiveObjectsNum());

		int create_bubaks = m_BubakNum - SpawnerBubaku_GetActiveObjectsNum();

		SPBLogger.GetInstance().Log("Need to create: " + create_bubaks);

		if (create_bubaks < m_SpawnLocations.Count())
		{
			//ramdom select spawn pos
			TStringArray positions = new TStringArray();
			positions.Copy(m_SpawnLocations);
			for ( i=0; i < create_bubaks; i++)
			{
				rndnum = Math.RandomIntInclusive(0, positions.Count() - 1);
				SPBLogger.GetInstance().Log("random position index: " + rndnum);

				rotated = false;
				pos = positions.Get(rndnum);
				ori = "0 0 0";
				if (positions.Get(rndnum).Contains("|"))
				{
					posrot = positions.Get(rndnum);
					loc = new TStringArray;
					posrot.Split( "|", loc );
					pos = loc.Get(0);
					ori = loc.Get(1);
					rotated = true;
				}
				if (pos.Contains(","))
				{
					SPBLogger.GetInstance().Log("Trigger spawnerpos contains commas in coordinates, thats wrong! Only one space between numbers! Fix yours config according example!!!", SPBLogger.LOGLEVEL_CRITICAL);
				}
				spawnpos = pos.ToVector();
				if (m_SpawnRadius >0)
				{
					spawnpos = SetRandomPos(spawnpos, m_SpawnRadius);
				}

				bubak = GetBubaci().GetRandomElement();
				spawnchance = 1.0;
				if (bubak.Contains("|"))
				{
					bubchance = new TStringArray;
					bubak.Split("|", bubchance);
					bubak = bubchance.Get(0);
					spawnchance = bubchance.Get(1).ToFloat();
				}
				rndflt = Math.RandomFloatInclusive(0, 1.0);
				if (spawnchance >= rndflt)
				{
					auto object1 = SpawnerBubaku_CreateObject(bubak, spawnpos ,false,true, true);
					if (rotated)
					{
						object1.SetOrientation(ori.ToVector());
					}
					positions.RemoveOrdered(rndnum);
				}

			}

		}
		else
		{
			for ( i=0; i < create_bubaks; i++)
			{
				/*
				randompos = m_SpawnLocations.GetRandomElement().ToVector();
				randvecstr = Math.RandomFloatInclusive(-0.5, 0.5).ToString()+ " 0 " + Math.RandomFloatInclusive(-0.5, 0.5).ToString();
				randvec = randompos+randvecstr.ToVector();
				GetGame().CreateObject(GetBubaci().GetRandomElement(),randvec,false,true, true);
				*/
				rotated = false;
				pos = m_SpawnLocations.Get(j);
				SPBLogger.GetInstance().Log("position index: " + j);

				ori = "0 0 0";
				if (m_SpawnLocations.Get(j).Contains("|"))
				{
					posrot = m_SpawnLocations.Get(j);
					loc = new TStringArray;
					posrot.Split( "|", loc );
					pos = loc.Get(0);
					ori = loc.Get(1);
					rotated = true;
				}
				spawnpos = pos.ToVector();
				if (m_SpawnRadius >0)
				{
					spawnpos = SetRandomPos(spawnpos, m_SpawnRadius);
				}

				bubak = GetBubaci().GetRandomElement();
				spawnchance = 1.0;
				if (bubak.Contains("|"))
				{
					bubchance = new TStringArray;
					bubak.Split("|", bubchance);
					bubak = bubchance.Get(0);
					spawnchance = bubchance.Get(1).ToFloat();
				}
				rndflt = Math.RandomFloatInclusive(0, 1.0);
				if (spawnchance >= rndflt)
				{
					auto object2 = SpawnerBubaku_CreateObject(bubak, spawnpos ,false,true, true);
					if (rotated)
					{
						object2.SetOrientation(ori.ToVector());
					}
					j++;
					if (j == m_SpawnLocations.Count())
					{
						j = 0;
					}
				}

			}
		}
	}
	vector SetRandomPos(vector position, float radius)
	{
		vector randompos;
		float t, u, v, w, x, y;

		u =  Math.RandomFloatInclusive(0.0, 1.0);
		v =  Math.RandomFloatInclusive(0.0, 1.0);

		w = radius * Math.Sqrt(u);
		t = 2 * Math.PI * v;
		x = w * Math.Cos(t);
		y = w * Math.Sin(t);


		randompos[0] = position[0]+x;
		randompos[1] = position[1];
		randompos[2] = position[2]+y;
		//check surface
		if (GetGame().SurfaceY(randompos[0], randompos[2]) > position[1])
		{
			randompos[1] = GetGame().SurfaceY(randompos[0], randompos[2]) + 0.3;
		}

		return randompos;

	}

	bool IsWorkingTime()
	{
		int year, month, day, hour, minute;
		int wtbegin = 0;
		int wtend = 24;
		if (m_WorkingHours.Contains("-"))
		{
			TStringArray loc = new TStringArray;
			m_WorkingHours.Split( "-", loc );
			wtbegin = loc.Get(0).ToInt();
			wtend = loc.Get(1).ToInt();
		}

		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		if (wtbegin > wtend)
		{
			if ( hour >= wtbegin || hour <= wtend )
			{
				return true;
			}
			return false;
		}
		else if (wtbegin < wtend)
		{
			if ( hour >= wtbegin && hour <= wtend )
			{
				return true;
			}
			return false;
		}
		else {
			return false;
		}

	}
};
