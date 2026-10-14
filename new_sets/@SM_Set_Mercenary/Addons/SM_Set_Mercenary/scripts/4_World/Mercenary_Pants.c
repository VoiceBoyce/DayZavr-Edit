class SM_Mercenary_Pants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

class SM_Mercenary_Pants_Black extends SM_Mercenary_Pants_ColorBase {};
class SM_Mercenary_Pants_White extends SM_Mercenary_Pants_ColorBase {};
class SM_Mercenary_Pants_Red_and_Yellow extends SM_Mercenary_Pants_ColorBase {};