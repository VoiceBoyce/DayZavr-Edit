// «Кровь Изнанки» (VB_RiftGear.c): при выходе с сервера эффект гасится сразу. Очередь CALL_CATEGORY_GAMEPLAY
// между подключениями не очищается, и без этого стук и подсветка доживали до следующего входа (баг №27).

modded class MissionGameplay
{
	override void OnMissionFinish()
	{
		VB_RiftBloodFx.VB_Set(false);
		super.OnMissionFinish();
	}
}
