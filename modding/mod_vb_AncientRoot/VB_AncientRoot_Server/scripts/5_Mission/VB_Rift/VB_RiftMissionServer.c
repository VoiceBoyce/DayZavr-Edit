// Запуск серверной части Изнанки вместе с миссией: метка сессии для протухания «Сердца Изнанки»
// (VB_RiftItemsServer) и гнёзда из $profile:VB_Mods\VB_Rift\ (VB_RiftNests: файлы, выбор, установка, редактор).
modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_RiftItemsServer.VB_Session();	// метка этой сессии: «Сердце Изнанки» из прошлой протухнет
		VB_RiftNests.VB_StartServer();
	}
}
