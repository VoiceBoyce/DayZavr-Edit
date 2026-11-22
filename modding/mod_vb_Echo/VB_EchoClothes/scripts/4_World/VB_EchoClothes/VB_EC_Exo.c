// Экзоскелет Эхо (VB_EchoClothes, этап 1, 11-23). Остальным частям свой скрипт пока не нужен: их поведение даёт
// ванильный родитель из конфига (жилет — PlateCarrierVest, перчатки — TacticalGloves, ботинки — MilitaryBoots).

// Шлем (был IMP_Helmet_RoninHeavy_ColorBase): как MICH 2001 — ночной прицел и фонарь на шлеме (их включают
// ванильные действия по этому классу); тёмное забрало — затемнение экрана, как у спортивных очков
class VB_EC_Exo_Helmet_Base extends Mich2001Helmet
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};

// хват шлема в руках — как у шлема «Горка» (так было в IMP)
modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_EC_Exo_Helmet_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/gorka_helmet.anm");
	}
};
