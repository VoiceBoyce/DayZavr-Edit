class PatrolObjectBase extends House
{
	protected const int CRASH_PATROL_WAYPOINT_COUNT = 4;
	protected const float CRASH_PATROL_MIN_RADIUS = 15.0;
	protected const float CRASH_PATROL_MAX_RADIUS = 30.0;

	protected ref array<InfectedBanditBase> m_PatrolObjectBandits = new array<InfectedBanditBase>();

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnBanditsWhenDynamicAIBReady, 30000, false);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SpawnBanditsWhenDynamicAIBReady);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VerifyPatrolObjectBandits);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(LogCrashBanditPositions);
		}

		if (GetGame() && GetGame().IsServer())
		{
			AIBLogger.Log("Removing PatrolObject bandit patrol at " + GetPosition() + ", releasing " + m_PatrolObjectBandits.Count() + " bandits");

			foreach (InfectedBanditBase bandit : m_PatrolObjectBandits)
			{
				if (bandit && bandit.IsAlive())
				{
					bandit.ClearWaypoints();
				}
			}

			m_PatrolObjectBandits.Clear();
		}

		super.EEDelete(parent);
	}

	protected TStringArray CreatePatrolWaypoints()
	{
		TStringArray waypoints = new TStringArray();
		float startAngle = Math.RandomFloatInclusive(0.0, 360.0);
		float angleStep = 360.0 / CRASH_PATROL_WAYPOINT_COUNT;
		float patrolDirection = 1.0;
		if (Math.RandomIntInclusive(0, 1) == 0)
		{
			patrolDirection = -1.0;
		}

		for (int waypointIndex = 0; waypointIndex < CRASH_PATROL_WAYPOINT_COUNT; waypointIndex++)
		{
			float angle = (startAngle + angleStep * waypointIndex * patrolDirection + Math.RandomFloatInclusive(-20.0, 20.0)) * Math.DEG2RAD;
			float radius = Math.RandomFloatInclusive(CRASH_PATROL_MIN_RADIUS, CRASH_PATROL_MAX_RADIUS);
			vector localOffset = Vector(Math.Sin(angle) * radius, 0, Math.Cos(angle) * radius);
			vector waypoint = ModelToWorld(localOffset);
			waypoint[1] = GetGame().SurfaceY(waypoint[0], waypoint[2]);
			waypoints.Insert(string.Format("%1 %2 %3", waypoint[0], waypoint[1], waypoint[2]));
		}

		return waypoints;
	}

	protected void SpawnBanditsWhenDynamicAIBReady()
	{
		if (!GetGame() || !GetGame().IsServer())
		{
			return;
		}

		if (!DynamicAIBManager.IsInitialized())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnBanditsWhenDynamicAIBReady, 1000, false);
			return;
		}

		SpawnBandits();
	}

	void SpawnBandits()
	{
		DynamicAIBConfig config = GetDayZGame().GetDynamicAIBConfig();
		string headgear,masks,vests,backpacks,bodywear,pants,belts,shoes,gloves,armband,weapon,loot;
		TStringArray lootlist;
		int i,j,k,l,rndnum;
		i = -1;
		if (!config || config.GroupLocations.Count() == 0 || config.patrolobjectgroup == "None" || config.patrolobjectgroup == "")
		{
			return;
		}
		else if (config.patrolobjectgroup == "Random")
		{
			i = Math.RandomIntInclusive(0,-1+config.GroupLocations.Count());
		}
		else
		{
			for (l=0; l < config.GroupLocations.Count(); l++)
			{
				if (config.GroupLocations.Get(l).name == config.patrolobjectgroup)
				{
					i = l;
				}
			}
		} 
		
		if (i>-1 && config.GroupLocations.Get(i).npcclasses)
		{
			AIBLogger.Log("Spawning PatrolObject NPC Group " + config.GroupLocations.Get(i).name + " at " + GetPosition());
			BanditAIGroup group = new BanditAIGroup();
			group.SetName(config.GroupLocations.Get(i).name);
			group.SetFaction("Bandits");
			group.SetFormation(new BanditAIFormationVee());
			
			for (j=0; j < config.GroupLocations.Get(i).npcclasses.Count(); j++)
			{
				TStringArray patrolWaypoints = CreatePatrolWaypoints();
				vector posi = patrolWaypoints.Get(0).ToVector();
				
				InfectedBanditBase bandit = InfectedBanditBase.Cast(GetGame().CreateObject(config.GroupLocations.Get(i).npcclasses.Get(j), posi, false, true));
				//Print("NUMBER " + j + " " + config.GroupLocations.Get(i).npcclasses.Get(j));
				if (bandit)
				{
					bandit.SetPose(0);
					//Print("NUMBER INSIDE" + j);
					//loadout
					headgear = config.GroupLocations.Get(i).npcproperties.headgear.GetRandomElement();
					masks = config.GroupLocations.Get(i).npcproperties.masks.GetRandomElement();
					vests = config.GroupLocations.Get(i).npcproperties.vests.GetRandomElement();
					backpacks = config.GroupLocations.Get(i).npcproperties.backpacks.GetRandomElement();
					bodywear = config.GroupLocations.Get(i).npcproperties.bodywear.GetRandomElement();
					belts = config.GroupLocations.Get(i).npcproperties.belts.GetRandomElement();
					pants = config.GroupLocations.Get(i).npcproperties.pants.GetRandomElement();
					shoes = config.GroupLocations.Get(i).npcproperties.shoes.GetRandomElement();
					gloves = config.GroupLocations.Get(i).npcproperties.gloves.GetRandomElement();
					armband = config.GroupLocations.Get(i).npcproperties.armband.GetRandomElement();
					weapon = config.GroupLocations.Get(i).weaponpool.GetRandomElement();
					
					if (weapon.Contains("Predefined_"))
					{
						for ( k = 0; k < config.PredefinedWeapons.Count(); k++)
						{
							if (config.PredefinedWeapons.Get(k).name == weapon)
							{
								string wpclass = config.PredefinedWeapons.Get(k).weapon;
								TStringArray wpattach = config.PredefinedWeapons.Get(k).attachments;
								bandit.SetupWeapon(wpclass, wpattach);
							}
						}
					}
					else
					{
						bandit.SetupWeapon(weapon, null);
					}
					
					BanditAI_Base banditai = BanditAI_Base.Cast(bandit);
					TStringArray clothing = new TStringArray;
					if (banditai)
					{
						clothing = {headgear, masks, vests, backpacks, bodywear, belts, pants, shoes, gloves, armband};
						banditai.InitialSetup(clothing);
						lootlist = new TStringArray;
						rndnum = Math.RandomInt(0, 3);
						for (k = 0; k < rndnum; k++)
						{
							loot = config.GroupLocations.Get(i).npcproperties.loot.GetRandomElement();
							lootlist.Insert(loot);
							banditai.SetupLoot(lootlist);

						}
					}
					//bandit.SetStance(IBStance.CROUCH);
					bandit.SetAccuracy(Math.Clamp(config.GroupLocations.Get(i).accuracy,0,100));
					bandit.SetGrenadeChance(Math.Clamp(config.GroupLocations.Get(i).grenadechance,0,100));
					bandit.SetGroup(group);
					bandit.SetGroupID(j);
					if (j==0)
					{
						group.SetLeader(bandit);
					}
					else
					{
						group.AddMember(bandit);
					}

					bandit.SetWaypoints(patrolWaypoints);
					bandit.ConfigureCrashSiteGuard(GetPosition(), patrolWaypoints);
					m_PatrolObjectBandits.Insert(bandit);
				}
			}

			if (m_PatrolObjectBandits.Count() > 0)
			{
				DynamicAIBManager.BanditGroups.Insert(group);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VerifyPatrolObjectBandits, 5000, false);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LogCrashBanditPositions, 5000, false, 5);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LogCrashBanditPositions, 20000, false, 20);
			}
		}
	}

	protected void VerifyPatrolObjectBandits()
	{
		if (!GetGame() || !GetGame().IsServer())
		{
			return;
		}

		int aliveBandits;
		foreach (InfectedBanditBase bandit : m_PatrolObjectBandits)
		{
			if (bandit && bandit.IsAlive())
			{
				aliveBandits++;
			}
		}

		AIBLogger.Log("PatrolObject bandit patrol active at " + GetPosition() + ": " + aliveBandits + "/" + m_PatrolObjectBandits.Count() + " bandits alive");
	}

	protected void LogCrashBanditPositions(int secondsAfterSpawn)
	{
		if (!GetGame() || !GetGame().IsServer())
		{
			return;
		}

		string positions;
		foreach (InfectedBanditBase bandit : m_PatrolObjectBandits)
		{
			if (bandit && bandit.IsAlive())
			{
				vector position = bandit.GetPosition();
				positions += bandit.GetType() + " at " + position + " (" + Math.Round(vector.Distance(position, GetPosition())) + "m); ";
			}
			else
			{
				positions += "removed; ";
			}
		}

		AIBLogger.Log("PatrolObject bandit positions after " + secondsAfterSpawn + "s at " + GetPosition() + ": " + positions);
	}

};

class AIB_PatrolObject : PatrolObjectBase {};