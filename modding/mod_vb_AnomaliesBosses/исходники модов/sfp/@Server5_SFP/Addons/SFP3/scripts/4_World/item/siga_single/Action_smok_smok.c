class ActionSmokSmokers : ActionConsumeSingle
{
	void ActionSmokSmokers()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_EAT_TABLET;

		m_Text = "#SFP_KurnutAction";
	}
	void OnStartAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
            horn.BlowHorn();
	}
	
	void OnEndAnimationLoopServer( ActionData action_data )
	{
		SFP_SIGARETA_SMOKE horn = SFP_SIGARETA_SMOKE.Cast(action_data.m_MainItem);
        if(horn)
            horn.StopBlowingHorn();
	}
}
