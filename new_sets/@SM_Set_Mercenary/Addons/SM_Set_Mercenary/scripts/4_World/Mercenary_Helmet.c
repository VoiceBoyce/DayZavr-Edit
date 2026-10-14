class SM_Mercenary_Helmet_ColorBase extends Mich2001Helmet
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};
class SM_Mercenary_Helmet_Black_and_White extends SM_Mercenary_Helmet_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};
class SM_Mercenary_Helmet_Red_and_Yellow extends SM_Mercenary_Helmet_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTORANGE;
	}
};
