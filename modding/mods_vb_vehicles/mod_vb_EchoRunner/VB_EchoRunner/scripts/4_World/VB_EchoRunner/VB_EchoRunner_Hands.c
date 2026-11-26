// Как держать детали Echo Runner в руках (тяжёлые, двумя руками) — анимации ванильных колёс и дверей.
// Не через RegisterHeavy (№ 90, 2026-11-26): на 16 эту цепочку переопределяют больше сотни модов машин, она длиннее
// предела стека Enforce (Stack overflow в логе клиента), и моды в её конце не вызываются вовсе. RegisterCustom
// короткая; «тяжёлое» поведение создаём как ваниль (DayZPlayerCfgBase.c: heavyItemBehaviour.SetHeavyItems()).
// Свои предметы регистрируем до super: даже если чужая цепочка ниже сорвётся, наши уже на месте.

modded class ModItemRegisterCallbacks
{
	override void RegisterCustom(DayZPlayerType pType)
	{
		DayzPlayerItemBehaviorCfg heavy = new DayzPlayerItemBehaviorCfg;
		heavy.SetHeavyItems();
		string asi = "dz/anims/workspaces/player/player_main/player_main_heavy.asi";
		string wheel = "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_wheel.anm";
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel_Ruined", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Wheel_Destroyed", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Tank", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_TankFusionPair", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_GravModule", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_GravModule_Ruined", asi, heavy, wheel);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Door_Driver", asi, heavy, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_driver.anm");
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Door_CoDriver", asi, heavy, "dz/anims/anm/player/ik/vehicles/hatchback_02/Hatchback_02_Door_codriver.anm");
		string hood = "dz/anims/anm/player/ik/vehicles/sedan/sedan_hood.anm";
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Hood", asi, heavy, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_L", asi, heavy, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_R", asi, heavy, hood);
		pType.AddItemInHandsProfileIK("VB_EchoRunner_Pipes_Engine", asi, heavy, hood);

		super.RegisterCustom(pType);
	}
}
