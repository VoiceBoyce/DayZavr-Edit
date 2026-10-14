modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_S2_WartaPants_Colorbase",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
        pType.AddItemInHandsProfileIK("TGK_S2_WartaJacket_Base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
        pType.AddItemInHandsProfileIK("TGK_S2_WartaGloves_Colorbase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, 					"dz/anims/anm/player/ik/gear/TacticalGloves.anm");
    };
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_S2_WartaVest_ColorBase", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior,					"dz/anims/anm/player/ik/two_handed/vest_ballistic.anm");
        pType.AddItemInHandsProfileIK("TGK_S2_WartaVest_Korshunov_ColorBase", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior,					"dz/anims/anm/player/ik/two_handed/vest_ballistic.anm");
	};                                                          
};
