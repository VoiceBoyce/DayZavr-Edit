class AIBDynamicGroup
{
    string name;
	string faction;
	ref TStringArray waypoints;
	ref TStringArray npcclasses;
	float accuracy;
	float grenadechance;
	//float geardamage;
	int dog;
	ref TStringArray weaponpool;
	ref DynNPCProperties npcproperties;
	
    void AIBDynamicGroup(string nam, string fac, TStringArray wayp, TStringArray npc, float accu, float gren, /*float geard,*/ int dogg, TStringArray weap)
	{
        name = nam;
		faction = fac;
		waypoints = wayp;
		npcclasses = npc;
		accuracy = accu;
		grenadechance = gren;
		//geardamage = geard;
		dog = dogg;
		weaponpool = weap;
		TStringArray headgear = {};
		TStringArray masks = {};
		TStringArray vests = {};
		TStringArray backpacks = {"HuntingBag"};
		TStringArray bodywear = {"HuntingJacket_Brown"};
		TStringArray belts = {};
		TStringArray pants = {"HunterPants_Brown"};
		TStringArray shoes = {"CombatBoots_Black"};
		TStringArray gloves = {};
		TStringArray armband = {};
		TStringArray loot = {"Rag","Apple"};
		npcproperties = new DynNPCProperties( headgear, masks, vests, backpacks, bodywear, belts, pants, shoes, gloves, armband, loot);
		
    }
}

class AIBSniper
{
    string name;
	ref TStringArray positions;
	string npcclass;
	float accuracy;
	bool fixedpos;
	string triggerpos;
	ref TStringArray weaponpool;
	ref DynNPCProperties npcproperties;
	
    void AIBSniper(string nam, TStringArray posit, string npc, float accu, bool fixe, string tpos, TStringArray weap)
	{
        name = nam;
		positions = posit;
		npcclass = npc;
		accuracy = accu;
		fixedpos = fixe;
		triggerpos = tpos;
		weaponpool = weap;
		TStringArray headgear = {};
		TStringArray masks = {};
		TStringArray vests = {};
		TStringArray backpacks = {"HuntingBag"};
		TStringArray bodywear = {"HuntingJacket_Brown"};
		TStringArray belts = {};
		TStringArray pants = {"HunterPants_Brown"};
		TStringArray shoes = {"CombatBoots_Black"};
		TStringArray gloves = {};
		TStringArray armband = {};
		TStringArray loot = {"Rag","Apple"};
		npcproperties = new DynNPCProperties( headgear, masks, vests, backpacks, bodywear, belts, pants, shoes, gloves, armband, loot);
    }
}


class DynamicAIBConfig
{

	int version;
	bool showtriggers;
	bool cleanzerovector;
	string crashsitegroup;
	string patrolobjectgroup;
	bool cardamage;
	int sniperDefensiveAggroSeconds;
	ref array< ref AIBDynamicGroup > GroupLocations;
	ref array< ref AIBSniper > SniperLocations;
	ref array< ref AIBPredefinedWeapons > PredefinedWeapons;
	void DynamicAIBConfig()
	{
		GroupLocations = new array< ref AIBDynamicGroup >;
		SniperLocations = new array< ref AIBSniper >;
		PredefinedWeapons = new array< ref AIBPredefinedWeapons >;
	}

}

class DynNPCProperties
{
	ref TStringArray headgear;
	ref TStringArray masks;
	ref TStringArray vests;
	ref TStringArray backpacks;
	ref TStringArray bodywear;
	ref TStringArray belts;
	ref TStringArray pants;
	ref TStringArray shoes;
	ref TStringArray gloves;
	ref TStringArray armband;
	ref TStringArray loot;
	
	void DynNPCProperties( TStringArray he, TStringArray ma, TStringArray ve, TStringArray ba, TStringArray bo, TStringArray bel, TStringArray pa, TStringArray sho, TStringArray glo, TStringArray arm, TStringArray loo )
	{
		headgear = he;
		masks = ma;
		vests = ve;
		backpacks = ba;
		bodywear = bo;
		belts = bel;
		pants = pa;
		shoes = sho;
		gloves = glo;
		armband = arm;
		loot = loo;
	}
}

class AIBPredefinedWeapons
{
	string name;
	string weapon;
	ref TStringArray attachments;
	
	void AIBPredefinedWeapons(string nam, string weap, TStringArray att)
	{
		name = nam;
		weapon = weap;
		attachments = att;
	}
}

class DynamicAIBanditsConfig
{

    protected const static string m_ConfigRoot = "$profile:\\AI_Bandits\\";

    static void LoadConfig(string configName, out DynamicAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            AIBLogger.Log("'" + configName + "' does not exist, creating default config");
            CreateDefaultConfig(configData);
            SaveConfig(configName, configData);
            return;
        }

        JsonFileLoader<DynamicAIBConfig>.JsonLoadFile(configPath, configData);
        AIBLogger.Log("'" + configName + "' found, loading existing config");

    }

    protected static void SaveConfig(string configName, DynamicAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(m_ConfigRoot))
        {
            AIBLogger.Log("'" + m_ConfigRoot + "' does not exist, creating directory");
            MakeDirectory(m_ConfigRoot);
        }

        JsonFileLoader<DynamicAIBConfig>.JsonSaveFile(configPath, configData);
    }
	
	static void UpgradeConfig(string configName, out DynamicAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            AIBLogger.Log("'"  + configName + "' does not exist, creating default config");
            CreateDefaultConfig(configData);
            SaveConfig(configName, configData);
            return;
        }
		JsonFileLoader<DynamicAIBConfig>.JsonLoadFile(configPath, configData);
		
		//upgrades...
		for ( int i=0; i < configData.GroupLocations.Count(); i++)
		{
			if (configData.GroupLocations.Get(i).faction == "")
			{
				configData.GroupLocations.Get(i).faction = "Bandits";
			}

		}
		if (configData.version == 1)
		{
			configData.version = 2;
			configData.crashsitegroup = "Random";
		}

		if (configData.version < 3)
		{
			configData.version = 3;
			configData.sniperDefensiveAggroSeconds = 30;
			// MARK: - RF MODIFICATION: old configs (before v3) always cleaned 0 0 0, keep that
			configData.cleanzerovector = true;
			// MARK: - END RF MODIFICATION
		}
		if (configData.version < 4)
		{
			configData.version = 4;
			configData.patrolobjectgroup = "Random";
		}
		else if (configData.sniperDefensiveAggroSeconds < 1)
		{
			configData.sniperDefensiveAggroSeconds = 30;
		}
		

        JsonFileLoader<DynamicAIBConfig>.JsonSaveFile(configPath, configData);
    }

    protected static void CreateDefaultConfig(out DynamicAIBConfig configData)
    {
        configData = new DynamicAIBConfig();
		configData.version = 3;
		configData.showtriggers = false;
		configData.cleanzerovector = false;
		configData.crashsitegroup = "Random";
		configData.patrolobjectgroup = "Random";
		configData.cardamage = true;
		configData.sniperDefensiveAggroSeconds = 30;
		//DynNPCProperties npcprop = new DynNPCProperties( {"BanditAI_Keiko", "BanditAI_Linda", "BanditAI_Rolf", "BanditAI_Denis", "BanditAI_Mirek"}, {}, {}, {}, {"HuntingBag"}, {"HuntingJacket_Brown"}, {"HunterPants_Brown"}, {"CombatBoots_Black"}, "");
		configData.GroupLocations.Insert( new AIBDynamicGroup("Group1", "Bandits", {"5237.821777 9.568775 2157.804199", "4887.835449 9.560701 2578.265625", "4917.622559 9.539299 2599.980469"}, {"BanditAI_Keiko", "BanditAI_Linda", "BanditAI_Rolf", "BanditAI_Denis", "BanditAI_Adam"} , 75, 0, 1, {"Predefined_AKM"} ));
		configData.SniperLocations.Insert( new AIBSniper("Sniper1", {"5237.821777 9.568775 2157.804199", "4887.835449 9.560701 2578.265625", "4917.622559 9.539299 2599.980469"}, "BanditAI_Keiko" , 75, 0, "5237.821777 9.568775 2157.804199", {"Predefined_AKM"} ));
		configData.PredefinedWeapons.Insert( new AIBPredefinedWeapons("Predefined_AKM", "AKM", {"Mag_AKM_30Rnd", "AK_WoodHndgrd", "AK_WoodBttstck"} ));

	}
}
