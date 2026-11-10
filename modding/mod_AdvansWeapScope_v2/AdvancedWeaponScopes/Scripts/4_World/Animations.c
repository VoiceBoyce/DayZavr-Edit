modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
    	super.RegisterOneHanded(pType, pBehavior);
		
		pType.AddItemInHandsProfileIK("AD_Pilad", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_longrange.anm");			
		pType.AddItemInHandsProfileIK("AD_Pilad_RIS", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_longrange.anm");			
		pType.AddItemInHandsProfileIK("AD_Pilad_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");			
		pType.AddItemInHandsProfileIK("AD_DHF5", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_puscope.anm");	
		pType.AddItemInHandsProfileIK("AD_DHF5_RIS", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_puscope.anm");	
		pType.AddItemInHandsProfileIK("AD_DHF5_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");				
		pType.AddItemInHandsProfileIK("AD_LeupoldMk4_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");				
		pType.AddItemInHandsProfileIK("AD_PVS4", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_pso1.anm");
		pType.AddItemInHandsProfileIK("AD_PVS4_Hunt", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_pso1.anm");
		pType.AddItemInHandsProfileIK("AD_PVS4_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_1PN51.anm");			
		pType.AddItemInHandsProfileIK("AD_OKP7_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_OKP7_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_Pilad_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_LeupoldMk4_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_DHF5_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_PVS4_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_1PN51.anm");
		pType.AddItemInHandsProfileIK("AD_OKP7_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_ACOG_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_acog.anm");
		pType.AddItemInHandsProfileIK("AD_ACOG_RMR_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_acog.anm");
		pType.AddItemInHandsProfileIK("AD_RMR_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_fnp45_mrd.anm");	
		pType.AddItemInHandsProfileIK("AD_RMR_LM_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_G33", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34_HHS", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_RMR_TM_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");
		pType.AddItemInHandsProfileIK("AD_RMR_LM_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_RMR_TM_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_ACOG_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");	
		pType.AddItemInHandsProfileIK("AD_ACOG_RMR_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");	
		pType.AddItemInHandsProfileIK("AD_RMR_LM_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_RMR_TM_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34_HHS_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_G33_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_ACOG_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_1PN51.anm");
		pType.AddItemInHandsProfileIK("AD_ACOG_RMR_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_MRS", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_reflex.anm");	
		pType.AddItemInHandsProfileIK("AD_MRS_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_MRS_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_XPS34_HHS_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_G33_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kobra.anm");
		pType.AddItemInHandsProfileIK("AD_NFATACR", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_puscope.anm");			
		pType.AddItemInHandsProfileIK("AD_NFATACR_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_NFATACR_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_NFATACR_1_8", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_longrange.anm");			
		pType.AddItemInHandsProfileIK("AD_NFATACR_1_8_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");			
		pType.AddItemInHandsProfileIK("AD_NFATACR_1_8_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_SpecterDR_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_acog.anm");
		pType.AddItemInHandsProfileIK("AD_SpecterDR_B13", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");	
		pType.AddItemInHandsProfileIK("AD_SpecterDR_MosinMount", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope_kobra.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_1PN51.anm");
		pType.AddItemInHandsProfileIK("AD_NFATACR_B13_RMR", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_kashtan.anm");
		pType.AddItemInHandsProfileIK("AD_LeupoldMk4", "dz/anims/workspaces/player/player_main/props/player_main_1h_scope.asi", pBehavior, "dz/anims/anm/player/ik/attachments/optic/optic_puscope.anm");	
    }	
};