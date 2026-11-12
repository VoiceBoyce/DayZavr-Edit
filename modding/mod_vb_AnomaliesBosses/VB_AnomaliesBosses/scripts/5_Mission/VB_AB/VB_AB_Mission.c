// Запуск сервера боссов вместе с миссией.
modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_AB_Server.Init();
		VB_AB_GenericDetectors.Start();		// писк чужих детекторов (Signal Generic)
	}
};
