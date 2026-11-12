// SFP Vodka effects — регистрация нашего PPE requester'а
//
// Это нужно, чтобы PPERequesterBank знал про PPERequester_DrunkVodka
// и его можно было получать через PPERequesterBank.GetRequester(...)

modded class PPERequesterRegistrations
{
	override protected void RegisterAdditionalRequesters()
	{
		super.RegisterAdditionalRequesters();
		PPERequesterBank.RegisterRequester(PPERequester_DrunkVodka);
	}
}
