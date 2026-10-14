class TGK_SetComplete_EOD_Specialist_Pants_Colorbase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class TGK_EOD_Specialist_Pants_Green extends TGK_SetComplete_EOD_Specialist_Pants_Colorbase {};
class TGK_EOD_Specialist_Pants_Black extends TGK_SetComplete_EOD_Specialist_Pants_Colorbase {};