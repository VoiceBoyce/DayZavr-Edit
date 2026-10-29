// DayZavr_Mapping — точки запуска маппинга на сервере и на клиенте.

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		DZM_Mapping.Apply();
	}
}

modded class MissionGameplay
{
	override void OnMissionStart()
	{
		super.OnMissionStart();
		DZM_Mapping.Apply();
	}
}
