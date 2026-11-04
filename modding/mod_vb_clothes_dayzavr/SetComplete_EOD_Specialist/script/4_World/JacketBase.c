class TGK_SetComplete_EOD_Specialist_Top_Colorbase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class TGK_EOD_Specialist_Top_Green extends TGK_SetComplete_EOD_Specialist_Top_Colorbase {};
class TGK_EOD_Specialist_Top_Black extends TGK_SetComplete_EOD_Specialist_Top_Colorbase {};