modded class PluginRecipesManagerBase
{
	override void RegisterRecipies()
	{
		super.RegisterRecipies();

		//Закрепить прицел на B-13		
		RegisterRecipe(new MountPiladOnB13);
		RegisterRecipe(new MountDHF5OnB13);
		RegisterRecipe(new MountPVS4OnB13);	
		RegisterRecipe(new MountOKP7OnB13);	
		RegisterRecipe(new MountRMRLMOnB13);
		RegisterRecipe(new MountRMRTMOnB13);
		RegisterRecipe(new MountACOGOnB13);	
		RegisterRecipe(new MountACOGRMROnB13);			
		RegisterRecipe(new MountMRSOnB13);	
		RegisterRecipe(new MountNFATACROnB13);	
		RegisterRecipe(new MountNFATACR_1_8OnB13);		
		RegisterRecipe(new MountSpecterDROnB13);				
		RegisterRecipe(new MountLeupoldMk4OnB13);				
		RegisterRecipe(new MountG33OnXPS34_B13);				
		RegisterRecipe(new MountXPS34HHSOnB13);				
		RegisterRecipe(new MountXPS34OnB13);				
		RegisterRecipe(new MountG33OnB13);				
		RegisterRecipe(new MountXPS34OnG33B13);				
		
		//Закрепить прицел на MosinMount		
		RegisterRecipe(new MountPiladOnMosinMount);
		RegisterRecipe(new MountDHF5OnMosinMount);
		RegisterRecipe(new MountPVS4OnMosinMount);	
		RegisterRecipe(new MountOKP7OnMosinMount);	
		RegisterRecipe(new MountRMRLMOnMosinMount);
		RegisterRecipe(new MountRMRTMOnMosinMount);
		RegisterRecipe(new MountACOGOnMosinMount);	
		RegisterRecipe(new MountACOGRMROnMosinMount);			
		RegisterRecipe(new MountMRSOnMosinMount);	
		RegisterRecipe(new MountNFATACROnMosinMount);		
		RegisterRecipe(new MountNFATACR_1_8OnMosinMount);			
		RegisterRecipe(new MountSpecterDROnMosinMount);		
		RegisterRecipe(new MountLeupoldMk4OnMosinMount);		
		RegisterRecipe(new MountG33OnXPS34_MM);		
		RegisterRecipe(new MountG33OnMM);		
		RegisterRecipe(new MountXPS34OnMM);		
		RegisterRecipe(new MountXPS34HHSOnMM);		
		RegisterRecipe(new MountXPS34OnG33MM);		
		
		//Закрепить RMR на ACOG		
		RegisterRecipe(new MountRMROnACOG);				
		RegisterRecipe(new MountG33OnXPS34);				
		
	}	
}