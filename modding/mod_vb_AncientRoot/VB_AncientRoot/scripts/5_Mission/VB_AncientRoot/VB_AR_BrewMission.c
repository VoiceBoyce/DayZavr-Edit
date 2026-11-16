// «Настой пыльцы» (VB_AR_Gear.c): при выходе с сервера подсветка гаснет сразу. Очередь CALL_CATEGORY_GAMEPLAY
// между подключениями не очищается, и без этого лампы доживали бы до следующего входа (как было с баг №27).

modded class MissionGameplay
{
	override void OnMissionFinish()
	{
		VB_AR_BrewFx.VB_Set(false);
		super.OnMissionFinish();
	}
}
