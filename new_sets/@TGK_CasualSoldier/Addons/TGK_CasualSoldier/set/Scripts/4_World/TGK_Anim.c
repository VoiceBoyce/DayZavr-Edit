modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_Casual_Belt_ColorBase",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "DZ/anims/anm/player/ik/clothing/belts/mil_belt_g.anm");
        pType.AddItemInHandsProfileIK("TGK_Casual_MedPouch_base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/gear/first_aid_kit.anm");
        pType.AddItemInHandsProfileIK("TGK_Casual_Respirator_base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/gear/aviators_glasses.anm");
        pType.AddItemInHandsProfileIK("TGK_Casual_Hat_base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/gear/gorka_helmet.anm");
        pType.AddItemInHandsProfileIK("TGK_Casual_Pants_Base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
        pType.AddItemInHandsProfileIK("TGK_Casual_Top_Base",      "dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
    };
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterTwoHanded(pType, pBehavior);
        pType.AddItemInHandsProfileIK("TGK_Casual_Backpack_base", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior,					"dz/anims/anm/player/ik/two_handed/mountain_g.anm");
	};                                                          
};
