modded class ASVAL 
{
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && (AD_PVS4.Cast(optic) || AD_NFATACR.Cast(optic) || AD_NFATACR_1_8.Cast(optic) || AD_Pilad.Cast(optic) || AD_LeupoldMk4.Cast(optic)))
			return true;
		return super.CanEnterIronsights();
	}
};