// Хват предметов «Древнего корня» в руках (2026-11-18). Без регистрации предмет берёт профиль Inventory_Base —
// ванильный ikpose яблока, и крупные предметы проходили сквозь пальцы. Здесь каждому — ikpose похожего ванильного
// предмета (P:\scripts\4_World\Entities\ManBase\DayZPlayer\DayZPlayerCfgBase.c), а модели в генераторе
// (tools/gen_ar_veins.py, HAND_XF) сдвинуты/повёрнуты под начало координат и оси ванильной модели-образца.
// Хваты выбраны на стенде из нескольких вариантов (яблоко, апельсин, камень двумя руками).
// Мелкие предметы (клык, семя, пыльца, лыко, ампула, оберег, ловушка) и вытянутые (отросток, вещун) с хватом яблока
// смотрятся нормально — не регистрируем. Клинок наследует хват ножа FangeKnife.
modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);

		// бутылки — как бутылка воды: горизонтально, за горлышко (WaterBottle.p3d стоит, середина в начале координат)
		pType.AddItemInHandsProfileIK("VB_AR_Reconcile", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/water_bottle.anm");
		pType.AddItemInHandsProfileIK("VB_AR_PollenBrew", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/water_bottle.anm");
		// Светоч — как газовая лампа: перед грудью, за петлю сверху
		pType.AddItemInHandsProfileIK("VB_AR_Lantern", "dz/anims/workspaces/player/player_main/props/player_main_1h_torch.asi", pBehavior, "dz/anims/anm/player/ik/gear/GasLight.anm");
		// Книга древних — как ванильная книга (Book_Base): за верхний край
		pType.AddItemInHandsProfileIK("VB_AR_Book", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
		// одна модель сердцевины (vb_ar_heart_chunk.p3d, начало у основания) — как апельсин; VB_AR_AmberInclusion наследует янтарь
		pType.AddItemInHandsProfileIK("VB_AR_HeartChunk", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/orange.anm");
		pType.AddItemInHandsProfileIK("VB_AR_HeartDead", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/orange.anm");
		pType.AddItemInHandsProfileIK("VB_AR_Amber", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/orange.anm");
	}

	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);

		// Сердце Хранителя (1,5 кг, 3×3) — двумя руками перед грудью, как ванильный камень Stone (начало у основания).
		// Даром рощи не считается (VB_AR_Offer.c), двуручный хват действиям не мешает.
		pType.AddItemInHandsProfileIK("VB_AR_KeeperHeart", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/stone.anm");
	}
}
