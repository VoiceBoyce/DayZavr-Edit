// ============================================================================
// МОНОЛИТ — регистрация PPE requester'а (модуль 3_Game)
//
// Ничего крутить тут не нужно. Этот файл просто регистрирует класс:
//   PPERequester_SFPMonolitStone
// чтобы его можно было получать через PPERequesterBank.
// ============================================================================

modded class PPERequesterRegistrations extends Managed
{
	override protected void RegisterAdditionalRequesters()
	{
		super.RegisterAdditionalRequesters();
		PPERequesterBank.RegisterRequester(PPERequester_SFPMonolitStone);
	}
}
