// Как держать общие комплектующие в руках — анимации ванильного автомобильного аккумулятора.

modded class ModItemRegisterCallbacks
{
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);

		string asi = "dz/anims/workspaces/player/player_main/player_main_2h.asi";
		string battery = "dz/anims/anm/player/ik/two_handed/BatteryCar.anm";
		pType.AddItemInHandsProfileIK("VB_EchoBattery", asi, pBehavior, battery);
		pType.AddItemInHandsProfileIK("VB_EchoBatteryFusion", asi, pBehavior, battery);
	}
};
