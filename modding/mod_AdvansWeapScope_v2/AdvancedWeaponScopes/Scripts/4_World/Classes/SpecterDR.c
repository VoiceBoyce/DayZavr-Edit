class AD_SpecterDR_Base : ItemOptics 
{
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
class AD_SpecterDR_B13 : AD_SpecterDR_Base {};
class AD_SpecterDR_MosinMount : AD_SpecterDR_Base {};