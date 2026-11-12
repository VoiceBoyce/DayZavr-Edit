modded class ItemBase
{
	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);
		
		if(HasEnergyManager())
		{
			if(GetBlowOutManager().IsSoundItem(this))
			{
				m_IsBOAlertItem = true;
				SetSynchDirty();
			}
		}
		
	}

	void SetBOSound(int phase, int sound_index, bool psi_storm)
	{
		if(m_IsBOAlertItem && GetCompEM().IsWorking())
		{
			Param2<int, int> data = new Param2<int, int>(phase, sound_index);
			int rpc_type = PlayerRPCs.BLOWOUT_PHASE;
			if(psi_storm)
				rpc_type = PlayerRPCs.PSISTORM_PHASE;
			
			RPCSingleParam(rpc_type, data, true, null);
		}
	}
}