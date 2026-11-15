// «Кровь Изнанки» (VB_AR_Gear.c): при выходе с сервера эффект гасится сразу. Очередь CALL_CATEGORY_GAMEPLAY
// между подключениями не очищается, и без этого стук и подсветка доживали до следующего входа (баг №27).

modded class MissionGameplay
{
	override void OnMissionFinish()
	{
		VB_AR_BloodFx.VB_Set(false);
		super.OnMissionFinish();
	}
}
