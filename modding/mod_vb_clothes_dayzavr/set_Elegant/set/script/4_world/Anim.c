modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
        super.RegisterOneHanded( pType, pBehavior ); 

		pType.AddItemInHandsProfileIK("Vp_Set15_Mask_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/balaclava_mask.anm");
		pType.AddItemInHandsProfileIK("Vp_SET15_Top_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/clothing/Skirt.anm");
		pType.AddItemInHandsProfileIK("Vp_SET15_Pants_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/camping/fabric.anm");
		pType.AddItemInHandsProfileIK("Vp_SET15_Gloves_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/TacticalGloves.anm");
		pType.AddItemInHandsProfileIK("VP_SET15_Belt_ColorBase","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/clothing/belts/mil_belt_g.anm");
	};
};