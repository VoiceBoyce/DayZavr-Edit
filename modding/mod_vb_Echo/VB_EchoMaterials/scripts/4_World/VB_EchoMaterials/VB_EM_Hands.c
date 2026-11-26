// Хват крупных материалов двумя руками (стенд 11-25): кузовная панель (70 см) и силовая рама (60 см) с профилем
// Inventory_Base (яблоко, одна рука) висели вдоль ноги до голени — ноги проходили бы сквозь них при ходьбе.
// Хват листового металла MetalPlate (metal_sheet.anm) не подошёл: он под лист 1,3 м — панель висела за плечом,
// поза кривая. Берём хват ванильного ящика WoodenCrate (P:\scripts\4_World\Entities\ManBase\DayZPlayer\
// DayZPlayerCfgBase.c:854): предмет поперёк груди на руках снизу, как наборы базы Эхо (VB_EB_Hands.c, одобрено).
// Модели повёрнуты длинной стороной по Z, как ящик (tools/em_models.py, CRATE_HOLD).
// Не через RegisterHeavy (№ 90, 2026-11-26): на 16 эта цепочка у модов машин длиннее предела стека Enforce
// (Stack overflow), её конец не вызывается — наш хват туда не попадал. RegisterCustom короткая; «тяжёлое»
// поведение — как у ванили (heavyItemBehaviour.SetHeavyItems()); свои предметы — до super.
modded class ModItemRegisterCallbacks
{
	override void RegisterCustom(DayZPlayerType pType)
	{
		DayzPlayerItemBehaviorCfg heavy = new DayzPlayerItemBehaviorCfg;
		heavy.SetHeavyItems();
		string asi = "dz/anims/workspaces/player/player_main/player_main_heavy.asi";
		string crate = "dz/anims/anm/player/ik/heavy/wooden_crate.anm";
		pType.AddItemInHandsProfileIK("VB_EM_BodyPanel", asi, heavy, crate);
		pType.AddItemInHandsProfileIK("VB_EM_PowerFrame", asi, heavy, crate);
		super.RegisterCustom(pType);
	}
};
