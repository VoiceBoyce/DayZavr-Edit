class TGK_Ghost_Gloves_Colorbase extends Gloves_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TGK_Ghost_Gloves_Black extends TGK_Ghost_Gloves_Colorbase {};