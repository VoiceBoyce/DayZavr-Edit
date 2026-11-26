// Как держать детали Echo Runner в руках (тяжёлые, двумя руками) — анимации ванильных колёс и дверей.

modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterHeavy(pType, pBehavior);

		string asi = "dz/anims/workspaces/player/player_main/player_main_heavy.asi";
		string wheel = "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_wheel.anm";
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel_Ruined", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel_Destroyed", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Tank", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_TankFusionPair", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_GravModule", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_GravModule_Ruined", asi, pBehavior, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Door_Driver", asi, pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_driver.anm");
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Door_CoDriver", asi, pBehavior, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_codriver.anm");
		string hood = "dz/anims/anm/player/ik/vehicles/sedan/sedan_hood.anm";
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Hood", asi, pBehavior, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_L", asi, pBehavior, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_R", asi, pBehavior, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_Engine", asi, pBehavior, hood);
	}
}
