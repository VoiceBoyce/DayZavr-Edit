modded class PPERequesterRegistrations
{
	override protected void RegisterAdditionalRequesters()
	{
		super.RegisterAdditionalRequesters();
		// Виньетка взгляда удалена по дизайну (заменено на ослепление как у светошумовой)
		PPERequesterBank.RegisterRequester(PPERequester_TenevnikAuraTint);
		PPERequesterBank.RegisterRequester(PPERequester_TenevnikBlindFlash);
	}
}
