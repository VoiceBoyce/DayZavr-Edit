// INT Banking (server) - mission hooks: start-up (INT_BankSE_Boot, 4_World), self-test tick, shutdown.

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		INT_BankSE_Boot.Init();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_BankSE_SelfTest.Tick(timeslice);
	}

	override void OnMissionFinish()
	{
		INT_BankSE_Log.Info("сервер останавливается - лог закрыт");
		INT_BankSE_Server.SetReady(false);
		super.OnMissionFinish();
	}
}
