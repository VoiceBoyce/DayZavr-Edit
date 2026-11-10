modded class MissionServer
{
    void MissionServer()
    {
		StaticAIBConfig sconfig;
        StaticAIBanditsConfig.UpgradeConfig("StaticAIB.json", sconfig);
		StaticAIBanditsConfig.LoadConfig("StaticAIB.json", sconfig);
        GetDayZGame().SetStaticAIBConfig(sconfig);
		
		DynamicAIBConfig dconfig;
        DynamicAIBanditsConfig.UpgradeConfig("DynamicAIB.json", dconfig);
		DynamicAIBanditsConfig.LoadConfig("DynamicAIB.json", dconfig);
        GetDayZGame().SetDynamicAIBConfig(dconfig);
		
		StaticAIBManager.ClearInstance();
		StaticAIBManager.GetInstance();
				
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( RunAIBLater, 30000 , false);	
		
    }
	
	void RunAIBLater()
	{
		StaticAIBManagerLate.ClearInstance();
		StaticAIBManagerLate.GetInstance();
		
		DynamicAIBManager.ClearInstance();
		DynamicAIBManager.GetInstance();
		
	}
    
};
