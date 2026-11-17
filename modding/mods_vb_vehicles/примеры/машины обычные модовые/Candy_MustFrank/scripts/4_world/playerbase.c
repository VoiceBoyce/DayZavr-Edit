modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
    {
		super.RegisterHeavy( pType, pBehavior );
		
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Wheel", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Wheel.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Driver", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_CoDriver", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Cargo1", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Cargo2", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Trunk", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Trunk0", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
		pType.AddItemInHandsProfileIK("Candy_MustFrank_Doors_Hood", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_PassengerL.anm");
    }
};