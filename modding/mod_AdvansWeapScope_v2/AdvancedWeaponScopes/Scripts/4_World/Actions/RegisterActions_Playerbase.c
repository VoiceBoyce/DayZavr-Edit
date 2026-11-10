modded class PlayerBase extends ManBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		
		AddAction(ActionChangeOpticMount_RMR_NONE_to_L, InputActionMap);		
		AddAction(ActionChangeOpticMount_RMR_L_to_H, InputActionMap);	
		AddAction(ActionChangeOpticMount_RMR_H_to_NONE, InputActionMap);	
		
		AddAction(ActionDismountPilad_B13, InputActionMap);
		AddAction(ActionDismountDHF5_B13, InputActionMap);
		AddAction(ActionDismountPVS4_B13, InputActionMap);	
		AddAction(ActionDismountOKP7_B13, InputActionMap);	
		AddAction(ActionDismountSpecterDR_B13, InputActionMap);	
		
		AddAction(ActionDismountPilad_MosinMount, InputActionMap);
		AddAction(ActionDismountDHF5_MosinMount, InputActionMap);
		AddAction(ActionDismountPVS4_MosinMount, InputActionMap);	
		AddAction(ActionDismountOKP7_MosinMount, InputActionMap);		
		AddAction(ActionDismountSpecterDR_MosinMount, InputActionMap);	

		AddAction(ActionDismountRMRLM_B13, InputActionMap);
		AddAction(ActionDismountRMRTM_B13, InputActionMap);
		AddAction(ActionDismountACOG_B13, InputActionMap);	
		AddAction(ActionDismountACOGRMR_B13, InputActionMap);	

		AddAction(ActionDismountRMRLM_MosinMount, InputActionMap);
		AddAction(ActionDismountRMRTM_MosinMount, InputActionMap);
		AddAction(ActionDismountACOG_MosinMount, InputActionMap);	
		AddAction(ActionDismountACOGRMR_MosinMount, InputActionMap);		
		
		AddAction(ActionDismountRMR_ACOG, InputActionMap);		

		AddAction(ActionDismountMRS_MosinMount, InputActionMap);	
		AddAction(ActionDismountMRS_B13, InputActionMap);		

		AddAction(ActionDismountNFATACR_B13, InputActionMap);	
		AddAction(ActionDismountNFATACR_MosinMount, InputActionMap);		

		AddAction(ActionDismountNFATACR_1_8_B13, InputActionMap);	
		AddAction(ActionDismountNFATACR_1_8_MosinMount, InputActionMap);	

		AddAction(ActionDismountLeupoldMk4_B13, InputActionMap);	
		AddAction(ActionDismountLeupoldMk4_MosinMount, InputActionMap);		

		AddAction(ActionDismountXPS34_B13, InputActionMap);	
		AddAction(ActionDismountXPS34_HHS_B13, InputActionMap);	
		AddAction(ActionDismountG33_B13, InputActionMap);	
		AddAction(ActionDismountXPS34_MosinMount, InputActionMap);
		AddAction(ActionDismountXPS34_HHS_MosinMount, InputActionMap);	
		AddAction(ActionDismountG33_MosinMount, InputActionMap);
		
		AddAction(ActionDismountXPS34HHS, InputActionMap);	
	}
};

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		
		actions.Insert(ActionChangeOpticMount_RMR_NONE_to_L);	
		actions.Insert(ActionChangeOpticMount_RMR_L_to_H);	
		actions.Insert(ActionChangeOpticMount_RMR_H_to_NONE);	
		
		actions.Insert(ActionDismountPilad_B13);
		actions.Insert(ActionDismountDHF5_B13);		
		actions.Insert(ActionDismountPVS4_B13);			
		actions.Insert(ActionDismountOKP7_B13);		
		actions.Insert(ActionDismountSpecterDR_B13);	
		
		actions.Insert(ActionDismountPilad_MosinMount);
		actions.Insert(ActionDismountDHF5_MosinMount);		
		actions.Insert(ActionDismountPVS4_MosinMount);			
		actions.Insert(ActionDismountOKP7_MosinMount);		
		actions.Insert(ActionDismountSpecterDR_MosinMount);	
		
		actions.Insert(ActionDismountRMRLM_B13);
		actions.Insert(ActionDismountRMRTM_B13);
		actions.Insert(ActionDismountACOG_B13);	
		actions.Insert(ActionDismountACOGRMR_B13);	

		actions.Insert(ActionDismountRMRLM_MosinMount);
		actions.Insert(ActionDismountRMRTM_MosinMount);
		actions.Insert(ActionDismountACOG_MosinMount);	
		actions.Insert(ActionDismountACOGRMR_MosinMount);	

		actions.Insert(ActionDismountRMR_ACOG);		
		
		actions.Insert(ActionDismountMRS_B13);	
		actions.Insert(ActionDismountMRS_MosinMount);	

		actions.Insert(ActionDismountNFATACR_B13);	
		actions.Insert(ActionDismountNFATACR_MosinMount);			
		
		actions.Insert(ActionDismountNFATACR_1_8_B13);	
		actions.Insert(ActionDismountNFATACR_1_8_MosinMount);			

		actions.Insert(ActionDismountLeupoldMk4_B13);	
		actions.Insert(ActionDismountLeupoldMk4_MosinMount);	
		
		actions.Insert(ActionDismountXPS34_B13);	
		actions.Insert(ActionDismountXPS34_MosinMount);	
		actions.Insert(ActionDismountXPS34_HHS_B13);	
		actions.Insert(ActionDismountG33_B13);	
		actions.Insert(ActionDismountXPS34_HHS_MosinMount);	
		actions.Insert(ActionDismountG33_MosinMount);	
		
		actions.Insert(ActionDismountXPS34HHS);	
	}
};