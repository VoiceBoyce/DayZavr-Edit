class AIBStaticPlace
{
    string name;
	int npcid;
	string npcpos;
	string npcclass;
	ref TStringArray npcloadout;
	string npcitem;
	int npcpose;
	string triggerpos;
	string triggermins;
	string triggermaxs;
	float triggerradius;
	int triggercooldown;
	ref TStringArray sounds;
	ref TStringArray emotes;


    void AIBStaticPlace(string nam, int nid, string npo, string npcls, TStringArray npclo, string npcit, int npcpo, string trxz, string trmin, string trmax, float trrad, int trcl, TStringArray snd, TStringArray emo) 
	{
        name = nam;
		npcid = nid;
		npcpos = npo;
		npcclass = npcls;
		npcloadout = npclo;
		npcitem = npcit;
		npcpose = npcpo;
		triggerpos = trxz;
		triggermins = trmin;
		triggermaxs = trmax;
		triggerradius = trrad;
		triggercooldown = trcl;
		sounds = snd;
		emotes = emo;
		//typ jmeno pozice triggeru
		// delay kdy znova spustit 
		// seznam spawn pointu seznam bubaku a u kazdeho pointu pocet bubaku 
		// jestli zahrat i zvuk 
		
    }
}


class StaticAIBConfig
{

	bool NPCDebug;
	int version;
	ref array< ref AIBStaticPlace > NPCLocations;
	ref array< ref AIBPredefinedWeapons > PredefinedWeapons;
	
	void StaticAIBConfig()
	{
		NPCLocations = new array< ref AIBStaticPlace >;
		PredefinedWeapons = new array< ref AIBPredefinedWeapons >;
	}

}

class StaticAIBanditsConfig
{

    protected const static string m_ConfigRoot = "$profile:\\AI_Bandits\\";

    static void LoadConfig(string configName, out StaticAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            AIBLogger.Log("'" + configName + "' does not exist, creating default config");
            CreateDefaultConfig(configData);
            SaveConfig(configName, configData);
            return;
        }

        JsonFileLoader<StaticAIBConfig>.JsonLoadFile(configPath, configData);
        AIBLogger.Log("'" + configName + "' found, loading existing config");

    }

    protected static void SaveConfig(string configName, StaticAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(m_ConfigRoot))
        {
            AIBLogger.Log("'" + m_ConfigRoot + "' does not exist, creating directory");
            MakeDirectory(m_ConfigRoot);
        }

        JsonFileLoader<StaticAIBConfig>.JsonSaveFile(configPath, configData);
    }
	
	static void UpgradeConfig(string configName, out StaticAIBConfig configData)
    {
        string configPath = m_ConfigRoot + configName;

        if (!FileExist(configPath))
        {
            AIBLogger.Log("'"  + configName + "' does not exist, creating default config");
            CreateDefaultConfig(configData);
            SaveConfig(configName, configData);
            return;
        }
		JsonFileLoader<StaticAIBConfig>.JsonLoadFile(configPath, configData);
		
		//upgrades...

        JsonFileLoader<StaticAIBConfig>.JsonSaveFile(configPath, configData);
    }

    protected static void CreateDefaultConfig(out StaticAIBConfig configData)
    {
        configData = new StaticAIBConfig();
		configData.NPCDebug = false;
		configData.version = 1;
		configData.NPCLocations.Insert( new AIBStaticPlace("npc1", 0, "", "", {""}, "", 1, "4546 71 1814", "-2 -1 -2", "2 1 2", 0, 30, {"1", "2"}, {"1","44"} ));
		configData.NPCLocations.Insert( new AIBStaticPlace("npc2", 0,"", "", {""}, "", 2, "4864 123 6671", "-2 -1 -2", "2 1 2", 0, 30, {"4", "12", "25"}, {"1","44"} ));
		configData.NPCLocations.Insert( new AIBStaticPlace("npc3", 0, "", "", {""}, "", 3, "2453 39 6209", "2 -1 -2", "2 1 2", 0, 30,  {"4", "12", "25"}, {"1","44"} ));
		configData.PredefinedWeapons.Insert( new AIBPredefinedWeapons("Predefined_AKM", "AKM", {"Mag_AKM_30Rnd", "AK_WoodHndgrd", "AK_WoodBttstck"} ));
		configData.PredefinedWeapons.Insert( new AIBPredefinedWeapons("Predefined_Stick", "SharpWoodenStick", {"PigSteakMeat"} ));
	}
}
