modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
        super.RegisterOneHanded( pType, pBehavior ); 

		pType.AddItemInHandsProfileIK("VP_SET20_Shoes_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
		pType.AddItemInHandsProfileIK("Vp_SET20_Mask_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/balaclava_mask.anm");
		pType.AddItemInHandsProfileIK("Vp_SET20_Pants_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/camping/fabric.anm");
		pType.AddItemInHandsProfileIK("Vp_SET20_Top_ColorBase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/camping/fabric.anm");
	};

	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("Vp_SET20_Vest_ColorBase","dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/vest_police.anm");
    };
};