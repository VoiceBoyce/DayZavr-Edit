modded class ModItemRegisterCallbacks 
{
	override void RegisterFireArms( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
        super.RegisterFireArms( pType, pBehavior );    
		
		pType.AddItemInHandsProfileIK("SP_Gauss_Base", "dz/anims/workspaces/player/player_main/weapons/player_main_svd.asi", pBehavior, "dz/anims/anm/player/ik/weapons/svd.anm",  "dz/anims/anm/player/reloads/svd/w_svd_states.anm");
 		
	}  
	
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		
		pType.AddItemInHandsProfileIK("SP_Mag_Gauss", "dz/anims/workspaces/player/player_main/props/player_main_STANAG_mag.asi", pBehavior, "dz/anims/anm/player/ik/attachments/magazines/magazine_cz527.anm");
		pType.AddItemInHandsProfileIK("SP_Mag_Gauss_15rnd", "dz/anims/workspaces/player/player_main/props/player_main_STANAG_mag.asi", pBehavior, "dz/anims/anm/player/ik/attachments/magazines/magazine_cz527.anm");
		
	}
};