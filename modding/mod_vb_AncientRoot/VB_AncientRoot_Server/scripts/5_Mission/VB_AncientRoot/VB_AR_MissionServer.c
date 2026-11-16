// Запуск серверной части «Древнего корня» вместе с миссией: метка сессии для окаменения сердцевины
// (VB_AR_ItemsServer), рощи из $profile:VB_Mods\VB_AncientRoot\ (VB_AR_Nests: файлы, выбор, установка, редактор),
// расположение леса (VB_AR_FavorServer: favor.json, игроку — при заходе, запись при остановке).
modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_AR_ItemsServer.VB_Session();	// метка этой сессии: сердцевина из прошлой окаменеет
		VB_AR_Nests.VB_StartServer();
		VB_AR_FavorServer.VB_Start();
		VB_AR_Siege.VB_Start();			// «Защита рощи»: жребий нашествия пауков (VB_AR_WorldSiege.c)
		VB_AR_Memory.VB_Start();		// «Лес помнит»: шёпот и хватка корней у Врага и Проклятого (VB_AR_MemoryServer.c)
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		VB_AR_FavorServer.VB_OnConnect(player);
	}

	override void OnMissionFinish()
	{
		VB_AR_FavorServer.VB_Save();
		super.OnMissionFinish();
	}
}
