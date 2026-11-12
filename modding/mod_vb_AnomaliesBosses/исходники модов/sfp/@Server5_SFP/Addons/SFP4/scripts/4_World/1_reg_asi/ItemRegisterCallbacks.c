
modded class ModItemRegisterCallbacks
{
	 override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded( pType, pBehavior );
		
		pType.AddItemInHandsProfileIK("SFP_Pohlebka_st",	"SFP3/2_anm/workspaces/Pohlevka_1h_up.asi",	pBehavior, "dz/anims/anm/player/ik/gear/compass.anm"/*"SFP3/2_anm/ik/pohlebka_ik2.anm"*/);
		pType.AddItemInHandsProfileIK("SFP_ksiva_base",	"SFP3/2_anm/workspaces/ksiva_provide.asi",	pBehavior, "SFP3/2_anm/ik/ksiva_ik.anm"/*"SFP3/2_anm/ik/pohlebka_ik2.anm"*/);
		pType.AddItemInHandsProfileIK("SFP_item_Chetki_01",	"SFP3/2_anm/workspaces/chetki_provide.asi",	pBehavior, "SFP3/2_anm/ik/cketki_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Knife_Kerambit",	"SFP3/2_anm/workspaces/karamba_krutit.asi",	pBehavior, "dz/anims/anm/player/ik/gear/combatknife.anm");
		pType.AddItemInHandsProfileIK("SFP_Knife_Skeleton",	"SFP3/2_anm/workspaces/skeletonKNF_krutit.asi",	pBehavior, "dz/anims/anm/player/ik/gear/combatknife.anm");
		pType.AddItemInHandsProfileIK("SFP_Items_osmotr_base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/bait_worm.anm");
		pType.AddItemInHandsProfileIK("SFP_Flash_Base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/bait_worm.anm");
		pType.AddItemInHandsProfileIK("SFP_DikoDog_Uho", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_himera_kogot", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_himera_uho", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_izlome_hand", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_boar_leg", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_BearZMB_Kogot", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_slepo_hvost", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Plot_eye", "SFP3/2_anm/workspaces/osmotr_item_eye.asi", pBehavior, "dz/anims/anm/player/ik/gear/apple.anm");
		pType.AddItemInHandsProfileIK("SFP_tenevnik_eye", "SFP3/2_anm/workspaces/osmotr_item_eye.asi", pBehavior, "dz/anims/anm/player/ik/gear/apple.anm");
		pType.AddItemInHandsProfileIK("SFP_Shum_jaw", "SFP3/2_anm/workspaces/osmotr_item_eye.asi", pBehavior, "dz/anims/anm/player/ik/gear/apple.anm");
		pType.AddItemInHandsProfileIK("SFP_tushkano_head", "SFP3/2_anm/workspaces/osmotr_item_eye.asi", pBehavior, "dz/anims/anm/player/ik/gear/apple.anm");
		pType.AddItemInHandsProfileIK("SFP_Piyavka_Yazik", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Psedog_hvost", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_CatBaun_lapa", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Trupojor_Kogot", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_Diplomat", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/attachments/magazines/magazine_ump45.anm");
		pType.AddItemInHandsProfileIK("SFP_Krovosos_jaw", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_Access_Card_Base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_radio_cassette_Base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_MiniPDA", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_textolit", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_box_condensers", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_box_transistors", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_tools_3", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_tools_2", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_tools_1", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_ZombieCivilian_hand", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_ratdog_Otrezano_Lapa", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_kanifol", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_DetectorRuined_1", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_DetectorRuined_2", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_mednaya_provoloka", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/gear/apple.anm");
		pType.AddItemInHandsProfileIK("SFP_Battery9V_pack_base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_2cigarettes_pack_base", "SFP3/2_anm/workspaces/osmotr_item.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		pType.AddItemInHandsProfileIK("SFP_SingleCigarette", "SFP3/2_anm/workspaces/FIDOv_SigaSmokers.asi", pBehavior, "SFP2/anm/siga_smokes/remastered/smoker_siga_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_item_mobile_Simen", "SFP3/2_anm/workspaces/Telephone_nabor.asi", pBehavior, "SFP3/2_anm/ik/telefon_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Juice_Base", "SFP3/2_anm/workspaces/juice_drink.asi", pBehavior, "SFP3/2_anm/ik/juice_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Juice_box_base", "SFP3/2_anm/workspaces/juice_craft_1FIDOv.asi", pBehavior, "SFP3/2_anm/ik/juice_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota01_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota01_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_Milk_Open", "SFP3/2_anm/workspaces/Sgusenka.asi", pBehavior, "dz/anims/anm/player/ik/gear/food_can_opened.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota02_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota02_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota03_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota03_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota04_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota04_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota05_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota05_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota06_closed", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_Can_shprota06_open", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/sardines.anm");
		pType.AddItemInHandsProfileIK("SFP_ST_BarigaBag",	"dz/anims/workspaces/player/player_main/player_main_1h.asi",	pBehavior, "dz/anims/anm/player/ik/gear/combatknife.anm");
		pType.AddItemInHandsProfileIK("SFP_st_bolt01", "SFP3/2_anm/workspaces/st_bolt_haxyu.asi", pBehavior, "SFP3/2_anm/ik/bolt_ik_03_clear.anm");
		pType.AddItemInHandsProfileIK("SFP_BandageDressing_soviet", "SFP3/2_anm/workspaces/Bandage_run_fidov.asi", pBehavior, "dz/anims/anm/player/ik/gear/bandage_dressing.anm");
		pType.AddItemInHandsProfileIK("SFP_SigaPack_Single", "SFP3/2_anm/workspaces/FIDOv_SigaSmokers.asi", pBehavior, "SFP2/anm/siga_smokes/remastered/smoker_siga_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_SigaPack_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/ammunition/308Win_20RoundBox.anm");
		//pType.AddItemInHandsProfileIK("SFP_Item_scanner_anom", "dz/anims/workspaces/player/player_main/props/player_main_2h_jerrycan.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/jerrycan.anm");
	
	}
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);
		DayzPlayerItemBehaviorCfg 	fireArmsItemBehaviour = new DayzPlayerItemBehaviorCfg;
		
		fireArmsItemBehaviour.SetFirearms();
		
		pType.AddItemInHandsProfileIK("SFP_BootleWaterPack6", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		pType.AddItemInHandsProfileIK("SFP_Karton_box", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		
		pType.AddItemInHandsProfileIK("SFP_CanPack6", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		pType.AddItemInHandsProfileIK("SFP_PipsiPack12", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		
		pType.AddItemInHandsProfileIK("SFP_VodkaPack12", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		pType.AddItemInHandsProfileIK("SFP_VodkaSFPPack12", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		
		pType.AddItemInHandsProfileIK("SFP_CanTunaPack15", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/BatteryCar.anm");
		pType.AddItemInHandsProfileIK("SFP_BootleWaterPack12", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/vehicles/BatteryTruck.anm");
		//pType.AddItemInHandsProfileIK("SFP_Striker_shotgunBase", "SFP3/2_anm/workspaces/striker.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/Vikhr.anm", "dz/anims/anm/player/reloads/SKS/w_SKS_states.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_scanner_anom", "dz/anims/workspaces/player/player_main/props/player_main_2h_jerrycan.asi", pBehavior,"dz/anims/anm/player/ik/two_handed/jerrycan.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_scanner_mutant", "dz/anims/workspaces/player/player_main/props/player_main_2h_jerrycan.asi", pBehavior,"dz/anims/anm/player/ik/two_handed/jerrycan.anm");
		pType.AddItemInHandsProfileIK("SFP_Item_scanner_spawnMutant", "dz/anims/workspaces/player/player_main/props/player_main_2h_jerrycan.asi", pBehavior,"dz/anims/anm/player/ik/two_handed/jerrycan.anm");
	
		
	}
	
	
};
































/*

                                        ▄▄▄██████████████▄▄▄▄
                                    ▄███████████████████████████▄▄
                                ▄ ▓██████████████████████████████████▄▄
                               ▒▄███████████████████████████████████████▄
                              ▒▄███████████████████████████████████████████▄
                            ▄█████████████████████████████████████████████▄▀▌
                          ▄█████████████▀▀▀▀▀██▀██▀▀▀▀▀██▀▀▀▀▀████████████████
                       ▒ ███████████████ ▐█████ █▌▐███ █▌▐███ ██████████████████
                     ▀ ▄████████████████ ▀▀▀███ █▌▐███ █▌▐███ █████████████████▌█▒
                     ▐▓█████████████████ ▐█████ █▌▐███ █▌▐███ ██████████████████▌█
					 ▐▓█████████████████ ▐█████ █▌▐███ █▌▐███ ██████████████████▌█
                    ▐ ▐█████████████████ ▐█████ █▌▐███ █▌▐███ █████████████████▌█▄▌▄▌
                      ██████████████████▄██████▄██▄▄▄▄███▄▄▄▄███████████████████▐▓█
                   ▌ ▐████████████████████████████████████▀██████████████████████▓   █
                  ▐████████████████████████▌ ████████████  ███████████████████████▌▄▄▄
                 ▀██████████████████████████  ██████████▌ ████████████████████████▌▐▄▀
                 ▒ █████████████████████████▌ ▐█████████ ▐████████████████████████▒ ▄
                   ▐▌████████████████████████  ████████  █████████████████████████ ▐▌
                   ▓▌█████████████████████████ ▐██████▌ ▐███████████████████████▀▐▐▐▌
                    ██████████████████████████▌ ██████  ████████████████████████▓▒ ▐
                    ███████████████████████████  ████▌ █████████████████████████▌
                     █░▐███████████████████████▌ ▐███  █████████████████████████▌
                     ▀█▄ ███████████████████████  ██  ███████████████████████▓█▀
                      ▀▓█████████████████████████ ▐█ ▐██████████████████████▄█▌
                       ▀▌████████████████████████    ██████████████████████▓▒▀
                        ▓▀▀▀██████████████████████▄▄█████████████████████▒
                         ▀▌  ████████████████████████████████████████▀▀██▀
                             ▓▀▀██████████████████████████████████▀▒▄▓▀▀
                                  ▀██████████████████████████████▓▀
                                      ▀▀███████████████████▀▀▀▀
                                          ▀▀▀▀▀▀ ▀█▀▀

 

 */