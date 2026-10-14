modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_02_Pouches_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_02_Gloves_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_02_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_02_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("Stalker_2_02_Monolith_Boots_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi",pBehavior,"dz/anims/anm/player/ik/gear/first_aid_kit.anm");
    };
    override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("Stalker_2_Monolith_02_Backpack_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/tortilla_g.anm");
	};       
};
