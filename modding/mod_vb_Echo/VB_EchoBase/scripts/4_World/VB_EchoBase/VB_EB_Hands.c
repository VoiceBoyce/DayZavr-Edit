// Хват наборов VB_EB_Kit_* в руках (решение пользователя 11-20: «как ванильный ящик»). Без регистрации предмет брал
// профиль Inventory_Base и висел перед игроком огромным кейсом. Модель набора (tools/eb_models.py kit) — в габаритах
// и осях ванильного WoodenCrate (wooden_case.p3d: x ±0.21, вверх 0..0.26, длина ±0.40, начало у дна), поэтому берём
// его тяжёлый хват двумя руками (P:\scripts\4_World\Entities\ManBase\DayZPlayer\DayZPlayerCfgBase.c, WoodenCrate).
// Регистрация по базовому классу действует на все наборы (как хват ножа у наследников, VB_AR_Hands.c).
modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterHeavy(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_EB_KitBase", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
	}
};
