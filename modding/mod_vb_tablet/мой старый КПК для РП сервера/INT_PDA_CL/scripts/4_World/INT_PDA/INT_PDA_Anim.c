// INT_PDA — how the PDA is held (owner 2026-10-23: «как у компаса» — in front of the player, horizontal, screen towards
// him). Measured on the stand with the three vanilla grips (TestKit INT_PDA_T_Grip*): the compass grips (Compass,
// OrienteeringCompass) lay this model on its side — the BestPDA model is built for the GPS receiver's grip. The GPS
// receiver's grip holds it landscape with the screen to the player: one hand in front of the chest, raised (RMB) in
// both hands. No animation files of our own.

modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("INT_PDA", "dz/anims/workspaces/player/player_main/props/player_main_1h_GPSReciever.asi", pBehavior, "dz/anims/anm/player/ik/gear/GPSReciever.anm");
	}
}
