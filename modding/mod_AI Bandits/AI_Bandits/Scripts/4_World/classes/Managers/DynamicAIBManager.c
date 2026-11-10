class DynamicAIBManager
{
	static autoptr array<autoptr BanditAIGroup> BanditGroups = new array<autoptr BanditAIGroup>();
	protected static ref DynamicAIBManager Instance;


	// MARK: - RF MODIFICATION -----------------------------------------------------

    private int minimumItemsToSpawn = 1;
    private int maximumItemsToSpawn = 3;

    private void rf_setupBandintLootListInside(BanditAI_Base banditAi, int locationId) {
		DynamicAIBConfig config = GetDayZGame().GetDynamicAIBConfig();
        TStringArray temporaryLootList = {};
        temporaryLootList.Copy(config.GroupLocations.Get(locationId).npcproperties.loot);


        TStringArray lootToSpawn = {};
        int minItems = minimumItemsToSpawn;
        int maxItems = maximumItemsToSpawn;
        if (temporaryLootList.Count() < maximumItemsToSpawn) { maxItems = temporaryLootList.Count(); }

        // RandomIntInclusive: from minItems to maxItems INCLUDING maxItems (1..3). Empty loot list -> 0 items.
        int randomAmount = 0;
        if (maxItems >= minItems) { randomAmount = Math.RandomIntInclusive(minItems, maxItems); }

        for (int i = 0; i < randomAmount; i++) {
            auto randomLootItem = temporaryLootList.GetRandomElement();
            if (!randomLootItem) {
                Print("[RF_Modding_BanditAi]-[Error] Объекта нет! Надо исправлять. Пытаемся отспавнить больше элементов чем в массиве");
                continue;
            }
            lootToSpawn.Insert(randomLootItem);
            temporaryLootList.RemoveItem(randomLootItem);
        }

        banditAi.SetupLoot(lootToSpawn);
    }

    // MARK: - END RF MODIFICATION -----------------------------------------------------



	static DynamicAIBManager GetInstance()
	{
		if (!Instance)
        {
            Instance = new DynamicAIBManager();
        }
		return Instance;
	}

	static void ClearInstance()
	{
		Instance = null;
	}

	static bool IsInitialized()
	{
		return Instance != null;
	}

	//for SpawnerBubaku
	static void SpawnGroup(string name, string position)
	{
		DynamicAIBConfig config = GetDayZGame().GetDynamicAIBConfig();

		TStringArray loc,lootlist;
		int i,j,k,rndnum;
		string headgear,masks,vests,backpacks,bodywear,pants,belts,shoes,gloves,armband,weapon,loot;

		BanditSniperTrigger snipertrigger;
		vector mins, maxs;
		string posrot, pos, rot;

		for ( i=0; i < config.GroupLocations.Count(); i++)
		{
			if (config.GroupLocations.Get(i).name == name)
			{

				//sniperi deteckce hrace uvnitr polygonu (trojuehlinku)

				AIBLogger.Log("Spawned NPC Group " + config.GroupLocations.Get(i).name + " at " + position);
				BanditAIGroup group = new BanditAIGroup();
				group.SetName(config.GroupLocations.Get(i).name);
				group.SetFaction(config.GroupLocations.Get(i).faction);
				//group.SetWaypoints(config.GroupLocations.Get(i).waypoints);

				group.SetFormation(new BanditAIFormationVee());

				for (j=0; j < config.GroupLocations.Get(i).npcclasses.Count(); j++)
				{
					vector offset = group.GetFormation().LocalFormPos(j);
					vector grplocation = position.ToVector();
					vector posi = grplocation + offset;

					InfectedBanditBase bandit = InfectedBanditBase.Cast(GetGame().CreateObject(config.GroupLocations.Get(i).npcclasses.Get(j), posi , false, true));
					//Print("NUMBER " + j + " " + config.GroupLocations.Get(i).npcclasses.Get(j));
					if (bandit)
					{
						//Print("NUMBER INSIDE" + j);
						bandit.SetPose(0);
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
							#ifdef DAYZ_DOG
							if (config.GroupLocations.Get(i).dog>0 && config.GroupLocations.Get(i).faction == "Bandits")
							{
								//dat random psa
								int dognum = config.GroupLocations.Get(i).dog;
								Dayz_Doggo dog = Dayz_Doggo.Cast(GetGame().CreateObject("Doggo_Follow"+dognum, posi , false, true));
								if (masks)
								{
									dog.GetInventory().CreateAttachment("dog_gasmask");
								}
								dog.SetNPCOwner(bandit);
							}
							#endif
						}
						else
						{
							group.AddMember(bandit);
						}
					}
				}

				BanditGroups.Insert(group);

			}
		}
	}

	void CleanupVectorZero()
	{
		array<Object> objects_in_vicinity;
		array<CargoBase> proxyCargos;
		int j;
		Object object_in_radius;
		//clean debug area 0 0 0
		objects_in_vicinity = new array<Object>;
		proxyCargos = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(vector.Zero, 1.5, objects_in_vicinity, proxyCargos);
		for ( j = 0; j < objects_in_vicinity.Count(); j++ )
		{
			GetGame().ObjectDelete(objects_in_vicinity.Get(j));
		}
	}

	void DynamicAIBManager()
    {

		DynamicAIBConfig config = GetDayZGame().GetDynamicAIBConfig();
		if (config.cleanzerovector)
		{
			CleanupVectorZero();
		}
		TStringArray loc,lootlist;
		int i,j,k,rndnum, rndnum1;
		string headgear,masks,vests,backpacks,bodywear,pants,belts,shoes,gloves,armband,weapon,loot;

		BanditSniperTrigger snipertrigger;
		vector mins, maxs;
		string posrot, pos, rot;

		for ( i=0; i < config.GroupLocations.Count(); i++)
		{
			if (config.GroupLocations.Get(i).npcclasses && config.GroupLocations.Get(i).waypoints)
			{

				//sniperi deteckce hrace uvnitr polygonu (trojuehlinku)


				BanditAIGroup group = new BanditAIGroup();
				group.SetName(config.GroupLocations.Get(i).name);
				group.SetFaction(config.GroupLocations.Get(i).faction);
				group.SetWaypoints(config.GroupLocations.Get(i).waypoints);
				int wpcount = -1+config.GroupLocations.Get(i).waypoints.Count();
				rndnum1 = Math.RandomIntInclusive(0,wpcount);
				AIBLogger.Log("Spawned NPC Group " + config.GroupLocations.Get(i).name + " at " + config.GroupLocations.Get(i).waypoints.Get(rndnum1));
				group.SetWaypointIndex(rndnum1);
				group.SetFormation(new BanditAIFormationVee());

				for (j=0; j < config.GroupLocations.Get(i).npcclasses.Count(); j++)
				{
					vector offset = group.GetFormation().LocalFormPos(j);
					vector grplocation = config.GroupLocations.Get(i).waypoints.Get(rndnum1).ToVector();
					vector posi = grplocation + offset;

					InfectedBanditBase bandit = InfectedBanditBase.Cast(GetGame().CreateObject(config.GroupLocations.Get(i).npcclasses.Get(j), posi , false, true));
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
							/* lootlist = new TStringArray;
							rndnum = Math.RandomInt(0, 3);
							for (k = 0; k < rndnum; k++)
							{
								loot = config.GroupLocations.Get(i).npcproperties.loot.GetRandomElement();
								lootlist.Insert(loot);
								banditai.SetupLoot(lootlist);

							} */

							this.rf_setupBandintLootListInside(banditai, i);
						}



						//bandit.SetStance(IBStance.CROUCH);
						bandit.SetAccuracy(Math.Clamp(config.GroupLocations.Get(i).accuracy,0,100));
						bandit.SetGrenadeChance(Math.Clamp(config.GroupLocations.Get(i).grenadechance,0,100));
						bandit.SetGroup(group);
						bandit.SetGroupID(j);
						if (j==0)
						{
							group.SetLeader(bandit);
							#ifdef DAYZ_DOG
							if (config.GroupLocations.Get(i).dog>0 && config.GroupLocations.Get(i).faction == "Bandits")
							{
								//dat random psa
								int dognum = config.GroupLocations.Get(i).dog;
								Dayz_Doggo dog = Dayz_Doggo.Cast(GetGame().CreateObject("Doggo_Follow"+dognum, posi , false, true));
								if (masks)
								{
									dog.GetInventory().CreateAttachment("dog_gasmask");
								}
								dog.SetNPCOwner(bandit);
							}
							#endif
						}
						else
						{
							group.AddMember(bandit);
						}
					}
				}

				BanditGroups.Insert(group);

			}
		}

		//Snipers
		// sniper pomoci trigger oblasti + test occluded
		// trigger vypocitat na zaklade dvou souradnic - box
		// udelat klidne vice triggeru na jednoho snipera - uvidime

		for ( i=0; i < config.SniperLocations.Count(); i++)
		{
			if (config.SniperLocations.Get(i).npcclass && config.SniperLocations.Get(i).positions)
			{
				vector sniperpos = config.SniperLocations.Get(i).positions.GetRandomElement().ToVector();
				InfectedBanditBase sniper = InfectedBanditBase.Cast(GetGame().CreateObject(config.SniperLocations.Get(i).npcclass, sniperpos , false, true));
				//Print("NUMBER " + j + " " + config.GroupLocations.Get(i).npcclasses.Get(j));
				if (sniper)
				{
					sniper.SetPose(0);
					//Print("NUMBER INSIDE" + j);
					//loadout
					headgear = config.SniperLocations.Get(i).npcproperties.headgear.GetRandomElement();
					masks = config.SniperLocations.Get(i).npcproperties.masks.GetRandomElement();
					vests = config.SniperLocations.Get(i).npcproperties.vests.GetRandomElement();
					backpacks = config.SniperLocations.Get(i).npcproperties.backpacks.GetRandomElement();
					bodywear = config.SniperLocations.Get(i).npcproperties.bodywear.GetRandomElement();
					belts = config.SniperLocations.Get(i).npcproperties.belts.GetRandomElement();
					pants = config.SniperLocations.Get(i).npcproperties.pants.GetRandomElement();
					shoes = config.SniperLocations.Get(i).npcproperties.shoes.GetRandomElement();
					gloves = config.SniperLocations.Get(i).npcproperties.gloves.GetRandomElement();
					armband = config.SniperLocations.Get(i).npcproperties.armband.GetRandomElement();
					weapon = config.SniperLocations.Get(i).weaponpool.GetRandomElement();

					if (weapon.Contains("Predefined_"))
					{
						for (k = 0; k < config.PredefinedWeapons.Count(); k++)
						{
							if (config.PredefinedWeapons.Get(k).name == weapon)
							{
								string wpclass1 = config.PredefinedWeapons.Get(k).weapon;
								TStringArray wpattach1 = config.PredefinedWeapons.Get(k).attachments;
								sniper.SetupWeapon(wpclass1, wpattach1);
							}
						}
					}
					else
					{
						sniper.SetupWeapon(weapon, null);
					}

					BanditAI_Base sniperai = BanditAI_Base.Cast(sniper);
					TStringArray clothing1 = new TStringArray;
					if (sniperai)
					{
						clothing1 = {headgear, masks, vests, backpacks, bodywear, belts, pants, shoes, gloves, armband};
						sniperai.InitialSetup(clothing1);
						lootlist = new TStringArray;
						rndnum = Math.RandomInt(0, 3);
						for (k = 0; k < rndnum; k++)
						{
							loot = config.SniperLocations.Get(i).npcproperties.loot.GetRandomElement();
							lootlist.Insert(loot);
							sniperai.SetupLoot(lootlist);

						}
					}
					sniper.SetAccuracy( Math.Clamp(config.SniperLocations.Get(i).accuracy,0,100));
					sniper.SetStance(IBStance.CROUCH);

					posrot = config.SniperLocations.Get(i).triggerpos;
					loc = new TStringArray;
					posrot.Split( "|", loc );
					pos = loc.Get(0);
					rot = loc.Get(1);
					if (!rot)
					{
						pos = config.SniperLocations.Get(i).triggerpos;
						rot = "0 0 0";
					}
					AIBLogger.Log("Created sniper trigger " + config.SniperLocations.Get(i).name + " at " + config.SniperLocations.Get(i).triggerpos);
					//
					snipertrigger = BanditSniperTrigger.Cast(GetGame().CreateObject("BanditSniperTrigger", pos.ToVector()));
					if (config.showtriggers)
					{
						EntityAI snipertrigvis = EntityAI.Cast(GetGame().CreateObject("AIBox100x80x20", pos.ToVector(), false, false, false));
						snipertrigvis.SetOrientation(rot.ToVector());
					}

					//EntityAI snipertrigvis = EntityAI.Cast(GetGame().CreateObject("AIBox1x1x1", pos.ToVector(), false, false, false));

					snipertrigger.SetOrientation(rot.ToVector());
					snipertrigger.AssignNPC(sniper);


					//mins = config.SniperLocations.Get(i).triggermins.ToVector();
					//maxs = config.SniperLocations.Get(i).triggermaxs.ToVector();
					mins = "-50 -10 -40".ToVector();
					maxs = "50 10 40".ToVector();
					snipertrigger.SetExtents(mins, maxs);

					sniper.AttachTrigger(snipertrigger);
					sniper.ConfigureSniper(sniperpos, config.sniperDefensiveAggroSeconds);

					//konfig triggeru predelat
					// jen 100x20x80 automaticky vydeli dvema a udela maxs mins a stred bude vzy uprostred
					// pak pujde udelat krasne scale p3d 1x1x1

				}
			}
		}

	}
}
