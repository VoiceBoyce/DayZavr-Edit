modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_Ghost_Gloves_Colorbase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Ghost_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Ghost_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Ghost_Poncho_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Ghost_Boots_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Ghost_mask_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
    };
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_Ghost_Vest_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/vest_ballistic.anm");
	};                                                          
};
