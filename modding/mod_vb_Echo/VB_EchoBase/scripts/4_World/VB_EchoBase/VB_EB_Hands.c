// Хват наборов VB_EB_*_Kit в руках (решение пользователя 11-20: «как ванильный ящик»). Без регистрации предмет брал
// профиль Inventory_Base и висел перед игроком огромным кейсом. Модель набора (tools/eb_models.py kit) — в габаритах
// и осях ванильного WoodenCrate (wooden_case.p3d: x ±0.21, вверх 0..0.26, длина ±0.40, начало у дна), поэтому берём
// его тяжёлый хват двумя руками (P:\scripts\4_World\Entities\ManBase\DayZPlayer\DayZPlayerCfgBase.c, WoodenCrate).
// Регистрация по базовому классу действует на все наборы (как хват ножа у наследников, VB_AR_Hands.c).
// Не через RegisterHeavy (№ 90, 2026-11-26): на 16 эта цепочка у модов машин длиннее предела стека Enforce
// (Stack overflow), её конец не вызывается. RegisterCustom короткая; «тяжёлое» поведение — как у ванили
// (heavyItemBehaviour.SetHeavyItems()); свои предметы — до super.
modded class ModItemRegisterCallbacks
{
	override void RegisterCustom(DayZPlayerType pType)
	{
		DayzPlayerItemBehaviorCfg heavy = new DayzPlayerItemBehaviorCfg;
		heavy.SetHeavyItems();
		pType.AddItemInHandsProfileIK("VB_EB_KitBase", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", heavy, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
		super.RegisterCustom(pType);
	}
};
