/*modded class PlayerBase extends ManBase
{

	override void Init()
		{
			if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
		DayzPlayerItemBehaviorCfg toolsOneHanded = new DayzPlayerItemBehaviorCfg;
		toolsOneHanded.SetToolsOneHanded();

		DayzPlayerItemBehaviorCfg heavyItemBehaviour = new DayzPlayerItemBehaviorCfg;
		heavyItemBehaviour.SetHeavyItems();

		DayzPlayerItemBehaviorCfg twoHanded = new DayzPlayerItemBehaviorCfg;
		twoHanded.SetTwoHanded();

		GetDayZPlayerType().AddItemInHandsProfileIK("SFP_SIGARETA_SMOKE", "dz/anims/workspaces/player/player_main/props/player_main_1h_morphine.asi", toolsOneHanded, "dz/anims/anm/player/ik/vehicles/sparkplug.anm");
		//GetDayZPlayerType().AddItemInHandsProfileIK("SFP_SvistokMYZ", "dz/anims/workspaces/player/player_main/props/player_main_1h_morphine.asi", toolsOneHanded, "dz/anims/anm/player/ik/vehicles/sparkplug.anm");
		GetDayZPlayerType().AddItemInHandsProfileIK("SFP_SvistokMYZ", "SFP2/anm/workspaces/SFPplayer_main.asi", toolsOneHanded, "dz/anims/anm/player/ik/vehicles/sparkplug.anm");
	}
		super.Init();
	};
};
*/

modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("SFP_LapTop_work", "SFP2/anm/workspaces/player_main_1h_noutWORK.asi", pBehavior, "dz/anims/anm/player/ik/gear/thermometer.anm");
		pType.AddItemInHandsProfileIK("SFP_TualBumaga", "SFP2/anm/workspaces/player_main_PlayerPoo.asi", pBehavior, "dz/anims/anm/player/ik/gear/rice.anm");
		pType.AddItemInHandsProfileIK("SFP_ActionPopisat", "SFP2/anm/workspaces/player_main_Popisat.asi", pBehavior, "dz/anims/anm/player/ik/gear/thermometer.anm");
		pType.AddItemInHandsProfileIK("SFP_Collection_base", "SFP2/anm/workspaces/Player_Inspect_Jurnals.asi", pBehavior, "dz/anims/anm/player/ik/gear/paper.anm");
		pType.AddItemInHandsProfileIK("SFP_SIGARETA_SMOKE", "SFP2/anm/workspaces/SmokesSigas.asi", pBehavior, "SFP2/anm/siga_smokes/remastered/smoker_siga_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_GitaraMYZ", "SFP2/anm/workspaces/play_myz_gitar.asi", pBehavior, "SFP2/anm/Play_Myz/gitara/PlayMyz_Gitara_ik2.anm");
		pType.AddItemInHandsProfileIK("SFP_GitaraMYZ_NoMusic", "SFP2/anm/workspaces/play_myz_gitar.asi", pBehavior, "SFP2/anm/Play_Myz/gitara/PlayMyz_Gitara_ik2.anm");
		pType.AddItemInHandsProfileIK("SFP_PDA_NoFunc", "SFP2/anm/workspaces/play_pda_work.asi", pBehavior, "SFP2/anm/pda/PDA_Work_IK.anm");
		pType.AddItemInHandsProfileIK("SFP_Game_VolkKura", "SFP2/anm/Play_Myz/PlayGame/PlayGame_wrs.asi", pBehavior, "SFP2/anm/Play_Myz/PlayGame/PlayGame_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Game_VolkKura_Oh_Bleaa", "SFP2/anm/Play_Myz/PlayGame/PlayGame_wrs_ohBleat.asi", pBehavior, "SFP2/anm/Play_Myz/PlayGame/PlayGame_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_BalalaikaMYZ", "SFP2/anm/workspaces/play_myz_gitar.asi", pBehavior, "SFP2/anm/Play_Myz/gitara/PlayMyz_Gitara_ik2.anm");
		pType.AddItemInHandsProfileIK("SFP_MusicPlayerKidsMYZ", "SFP2/anm/workspaces/play_myz_wolkman.asi", pBehavior, "SFP2/anm/wolkman_player/old_wolkman_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_BayanMYZ", "SFP2/anm/workspaces/play_myz_bayan.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		pType.AddItemInHandsProfileIK("SFP_ingalator_Base", "SFP2/anm/Medical_ingalator/FIDOv_asmaSpray.asi", pBehavior, "SFP2/anm/Medical_ingalator/ingalator_IK.anm");
		pType.AddItemInHandsProfileIK("SFP_ai2medkit_Base", "SFP2/anm/ai2_medkit/FIDOv_AI2_medkit.asi", pBehavior, "SFP2/anm/ai2_medkit/ai2_used_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_HardDisk_WD", "SFP2/anm/ai2_medkit/FIDOv_AI2_medkit.asi", pBehavior, "SFP2/anm/RPmoments/hard_disk_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Express_Syringe_Base", "SFP2/anm/exp_spric/FIDOv_Exp_spric.asi", pBehavior, "SFP2/anm/exp_spric/spric_express_ik2.anm");
		pType.AddItemInHandsProfileIK("SFP_StakanGranen", "SFP2/anm/GranenStakan/granen_vodka_1h.asi", pBehavior, "SFP2/anm/GranenStakan/granen_vodka_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_StakanRumka", "SFP2/anm/GranenStakan/granen_vodka_1h.asi", pBehavior, "SFP2/anm/GranenStakan/rumka_standart_ik.anm");
		pType.AddItemInHandsProfileIK("SFP_Vodka_ColorBase", "SFP2/anm/Vodka_Roza/vodka_roza_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/water_bottle.anm");
		pType.AddItemInHandsProfileIK("SFP_Molotov", "SFP2/anm/Vodka_Roza/vodka_roza_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/water_bottle.anm");
		pType.AddItemInHandsProfileIK("SFP_Collection_folder", "SFP2/anm/workspaces/Player_Inspect_Jurnals.asi", pBehavior, "dz/anims/anm/player/ik/gear/paper.anm");
		pType.AddItemInHandsProfileIK("SFP_Collection_folder2", "SFP2/anm/workspaces/Player_Inspect_Jurnals.asi", pBehavior, "dz/anims/anm/player/ik/gear/paper.anm");
		//pType.AddItemInHandsProfileIK("SFP_PDA", "SFP2/anm/workspaces/play_myz_gitar.asi", pBehavior, "SFP2/anm/pda/PDA_IK2.anm");
		
	}
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);
		
		pType.AddItemInHandsProfileIK("SFP_Big_Kanistra_Base", "dz/anims/workspaces/player/player_main/props/player_main_2h_jerrycan.asi", pBehavior,"dz/anims/anm/player/ik/two_handed/jerrycan.anm");
	
		DayzPlayerItemBehaviorCfg 	fireArmsItemBehaviour = new DayzPlayerItemBehaviorCfg;
		
		fireArmsItemBehaviour.SetFirearms();
		
		pType.AddItemInHandsProfileIK("SFP_ShotgunMP153ModPlastic", "SFP2/anm/workspaces/player_main_mp153.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/akm.anm", "dz/anims/anm/player/reloads/SKS/w_SKS_states.anm");
		pType.AddItemInHandsProfileIK("SFP_SPAS12_Shotgun", "SFP2/anm/workspaces/player_main_mp153.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/akm.anm", "dz/anims/anm/player/reloads/SKS/w_SKS_states.anm");
		pType.AddItemInHandsProfileIK("SFP_BenelliM4", "SFP2/anm/workspaces/player_main_beneM4.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/akm.anm", "dz/anims/anm/player/reloads/SKS/w_SKS_states.anm");
		//pType.AddItemInHandsProfileIK("SFP_MurkaShotgun_Base", "SFP2/anm/workspaces/player_main_Murka.asi", fireArmsItemBehaviour, "dz/anims/anm/player/ik/weapons/SKS.anm", "dz/anims/anm/player/reloads/SKS/w_SKS_states.anm");
		
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