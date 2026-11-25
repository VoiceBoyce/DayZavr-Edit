// Хват крупных материалов двумя руками (стенд 11-25): кузовная панель (70 см) и силовая рама (60 см) с профилем
// Inventory_Base (яблоко, одна рука) висели вдоль ноги до голени — ноги проходили бы сквозь них при ходьбе.
// Хват листового металла MetalPlate (metal_sheet.anm) не подошёл: он под лист 1,3 м — панель висела за плечом,
// поза кривая. Берём хват ванильного ящика WoodenCrate (P:\scripts\4_World\Entities\ManBase\DayZPlayer\
// DayZPlayerCfgBase.c:854): предмет поперёк груди на руках снизу, как наборы базы Эхо (VB_EB_Hands.c, одобрено).
// Модели повёрнуты длинной стороной по Z, как ящик (tools/em_models.py, CRATE_HOLD).
modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterHeavy(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_EM_BodyPanel", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
		pType.AddItemInHandsProfileIK("VB_EM_PowerFrame", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
	}
};
