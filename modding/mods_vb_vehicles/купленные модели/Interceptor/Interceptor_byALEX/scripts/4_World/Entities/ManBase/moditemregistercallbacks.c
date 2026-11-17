modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
		super.RegisterHeavy(pType, pBehavior);
	
		
		pType.AddItemInHandsProfileIK("Interceptor_Wheel", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior,			"dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_wheel.anm");
		
		pType.AddItemInHandsProfileIK("Interceptor_Wheel_destroyed", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior,				"dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_wheel.anm");
		
		pType.AddItemInHandsProfileIK("Interceptor_Doors_Driver", 	"dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior,	"dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_driver.anm");
		
    	pType.AddItemInHandsProfileIK("Interceptor_Doors_coDriver", 				"dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior,	"dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_codriver.anm");
		
		pType.AddItemInHandsProfileIK("Interceptor_Doors_hood", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior,				"dz/anims/anm/player/ik/vehicles/sedan/sedan_hood.anm");		
		
    }	
}