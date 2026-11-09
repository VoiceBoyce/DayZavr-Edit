modded class MissionServer
{

	override void OnInit()
	{
		super.OnInit();
		
		
	}
	
	override void OnMissionLoaded()
	{
		super.OnMissionLoaded();
		
		AnomalyDmgCfg.ReadConfig();
		AnomalySpawner.ReadConfig();
		
		GetBlowOutManager().MissionLoaded();
	}

}
