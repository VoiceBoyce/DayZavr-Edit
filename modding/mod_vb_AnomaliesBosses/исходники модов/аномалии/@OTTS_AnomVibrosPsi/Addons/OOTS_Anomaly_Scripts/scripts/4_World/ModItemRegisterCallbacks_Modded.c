modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
    {
		super.RegisterOneHanded( pType, pBehavior );        
		pType.AddItemInHandsProfileIK("OOTS_item_PsyBox", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, 			"DZ/anims/anm/player/ik/two_handed/batterycar.anm");
		pType.AddItemInHandsProfileIK("OOTS_item_PsyTower", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, 			"DZ/anims/anm/player/ik/two_handed/batterycar.anm");
		pType.AddItemInHandsProfileIK("OOTS_item_PsyTower_osnova", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, 			"DZ/anims/anm/player/ik/two_handed/batterycar.anm");
		pType.AddItemInHandsProfileIK("OOTS_item_PsyTower_battery", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, 			"DZ/anims/anm/player/ik/two_handed/batterycar.anm");
		pType.AddItemInHandsProfileIK("OOTS_item_PsyDecoder", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, 			"dz/anims/anm/player/ik/gear/sewing_kit.anm");
		pType.AddItemInHandsProfileIK("OOTS_Item_Bolt", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, 			"DZ/anims/anm/player/ik/gear/9v_battery.anm");
		pType.AddItemInHandsProfileIK("Anomaly_Detector_Otklik","dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("Anomaly_Detector_Oko","dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("Anomaly_Detector_Medved","dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("Anomaly_Detector_Svarog","dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("Anomaly_Detector_Veles","dz/anims/workspaces/player/player_main/props/player_main_1h_flashlight.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
		pType.AddItemInHandsProfileIK("OOTS_item_PsyHelmet",  "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/gorka_helmet.anm");
	}
}