modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
        super.RegisterOneHanded( pType, pBehavior ); 

		pType.AddItemInHandsProfileIK("Vp_SET16_Helmet_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/gorka_helmet.anm");
		pType.AddItemInHandsProfileIK("Vp_SET16_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
		pType.AddItemInHandsProfileIK("Vp_SET16_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/camping/fabric.anm");
		pType.AddItemInHandsProfileIK("Vp_SET16_Gloves_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/TacticalGloves.anm");
	};


	override void RegisterTwoHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
        super.RegisterTwoHanded( pType, pBehavior ); 

		pType.AddItemInHandsProfileIK("Vp_SET16_Vest_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/vest_police.anm");
    };
};