//This is for ability to use HHS with turned on NVG.

modded class DayZPlayerCameraOptics
{
	override void SetCameraPP(bool state,DayZPlayerCamera launchedFrom)
	{
		if (m_opticsUsed && (m_opticsUsed.IsInherited( AD_XPS34_HHS ) || m_opticsUsed.IsInherited( AD_XPS34_HHS_B13 ) || m_opticsUsed.IsInherited( AD_XPS34_HHS_MosinMount )))
		{
			array<float> lens_values = new array<float>;
			array<float> mask_values = new array<float>;
			float gauss = 0.0;
			temp_array = {false, 0.0, 0.0, 0.0, 0.0, 1.0};
			
			// optics mask
			if (m_opticsUsed.GetOpticsPPMask() && m_opticsUsed.GetOpticsPPMask().Count() == 4)
			{
				mask_values = m_opticsUsed.GetOpticsPPMask();
			}
			
			//optics lens
			if (m_opticsUsed.GetOpticsPPLens() && m_opticsUsed.GetOpticsPPLens().Count() == 4)
			{
				lens_values = m_opticsUsed.GetOpticsPPLens();
			}
			
			//optics blur
			if (m_opticsUsed.GetOpticsPPBlur() != 0)
			{
				gauss = m_opticsUsed.GetOpticsPPBlur();
			}
				
			m_RequesterADS.SetValuesOptics(mask_values,lens_values,gauss);
			
			if (IsCameraNV())
			{
				SetNVPostprocess(GetCameraNVType());
			}
			else
			{
				SetNVPostprocess(NVTypes.NONE);
			}
		}
		else super.SetCameraPP(state,launchedFrom);
	}
};