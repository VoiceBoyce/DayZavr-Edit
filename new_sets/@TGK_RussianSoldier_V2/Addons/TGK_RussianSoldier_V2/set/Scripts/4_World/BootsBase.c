class TGK_RussianSoldier_v2_Boots_ColorBase extends Shoes_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TGK_RussianSoldier_v2_Boots_Black extends TGK_RussianSoldier_v2_Boots_ColorBase {};