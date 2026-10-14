modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_LegPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_GranadePouch_Colorbase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_MagPouch_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Balaclava_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_01_Gloves_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Boots_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Helmet_Colorbase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/ballistic_helmet.anm");
    };
    override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_Backpack_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/tortilla_g.anm");
	};       
};
