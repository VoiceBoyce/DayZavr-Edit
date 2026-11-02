// Скриптовый класс ящика 07 из версии 1.29. Без него сервер RF не может открыть и наполнить ящик
// (fillCrate работает с RF_CONTAINERS_CL_EventCrate_BaseEntity).
// Звуки берём от ящика 03: их id (51807/51808) регистрирует сам RF_CONTAINERS_CL_S.
class RF_CONTAINERS_CRATE_BASE_07 : RF_CONTAINERS_CL_EventCrate_BaseEntity
{
	override void RF_CONTAINERS_CL_SoundOpenPlay()
	{
		GetItemSoundHandler().PlayItemSoundClient(51807);
	}

	override void RF_CONTAINERS_CL_SoundClosePlay()
	{
		GetItemSoundHandler().PlayItemSoundClient(51808);
	}
};
