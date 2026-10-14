modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_BackPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_AbdominalProtection_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");	
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_DumpPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_Alpha_IFAK_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_Alpha_MedPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_MagPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Balaclava_base","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_ArmBand_base","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Headphones_base","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Belt_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Helmet_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/ballistic_helmet.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Gloves_Colorbase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_BackPanel_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Boots_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
    };
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_FSB_ALPHA_Vest_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/vest_ballistic.anm");
	};                                                          
};
