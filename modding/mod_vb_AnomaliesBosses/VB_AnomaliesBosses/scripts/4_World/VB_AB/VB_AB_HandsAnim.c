// Как предметы мода держатся в руке (анимация хвата и поза кисти). Без регистрации предмет получает ванильную
// позу Inventory_Base («яблоко»), и детектор висел у бедра, а материал лежал в ладони криво (пользователь, 2026-11-13).
// Регистрация по базовому классу config.cpp действует на всех наследников (как Colt1911_Base в DayZPlayerCfgBase.c).
//   детекторы — как фонарик: так же держат свои детекторы OOTS и мод артефактов (модели детекторов — копии OOTS);
//   материалы аномалий — как компас: так мод артефактов держит артефакты (DD_Artifact_*).

modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_AB_DetectorBase", "dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("VB_AB_Mat_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_compass.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
	}
};
