modded class ModItemRegisterCallbacks
{
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_RussianSolider_V3_Vest_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/vest_ballistic.anm");
	};                                                          
};
