class TGK_SetComplete_EOD_Specialist_Helmet_Colorbase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TGK_EOD_Specialist_Helmet_Green extends TGK_SetComplete_EOD_Specialist_Helmet_Colorbase {};
class TGK_EOD_Specialist_Helmet_Black extends TGK_SetComplete_EOD_Specialist_Helmet_Colorbase {};