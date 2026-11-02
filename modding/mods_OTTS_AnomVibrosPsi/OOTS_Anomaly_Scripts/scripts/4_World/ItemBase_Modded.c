modded class ItemBase
{
	private SoundOnVehicle m_SEngine;
	protected bool m_IsBOAlertItem = false;
	
	private ref array<string> m_WarningSounds = {"bo_warning_1", "bo_warning_2", "bo_warning_3"};
	private ref array<string> m_ActiveSounds = {"bo_active_1", "bo_active_2", "bo_active_3"};
	
	private ref array<string> m_PsiWarningSounds = {"bo_warning_1", "bo_warning_2", "bo_warning_3"};
	private ref array<string> m_PsiActiveSounds = {"bo_active_1", "bo_active_2", "bo_active_3"};
	
	override void InitItemVariables()
	{
		super.InitItemVariables();

		RegisterNetSyncVariableBool("m_IsBOAlertItem");
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
	}
	
	private void StopSound()
	{
		if(m_SEngine)
		{
			GetGame().ObjectDelete( m_SEngine );
			m_SEngine = NULL;
		}
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if(m_IsBOAlertItem)
		{
			switch(rpc_type)
			{
				case PlayerRPCs.BLOWOUT_PHASE:
					StopSound();
					Param2<int, int> data = new Param2<int, int>(-1, -1);
					if(ctx.Read(data))
					{	
						int phase = data.param1;
						int sound = data.param2;
						PlayBOAlert(phase, sound);
					}
				break;
				case PlayerRPCs.PSISTORM_PHASE:
					StopSound();
					Param2<int, int> psi_data = new Param2<int, int>(-1, -1);
					if(ctx.Read(psi_data))
					{	
						PlayPsiAlert(psi_data.param1, psi_data.param2);
					}
				break;
			}
		}
	}
	
	void PlayBOAlert(int phase, int sound_index)
	{
		string sound;
		switch(phase)
		{
			case BlowOutPhases.COOLDOWN:
				sound = "bo_final";
			break;
			case BlowOutPhases.WARNING:
				sound = m_WarningSounds.Get(sound_index);
			break;
			case BlowOutPhases.ACTIVE:
				sound = m_ActiveSounds.Get(sound_index);
			break;
			case BlowOutPhases.END:
			break;
		}

		m_SEngine = PlaySound(sound, 25);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopSound, 8 * 1000);
	}
	
	protected void PlayPsiAlert(int phase, int sound_index)
	{
		string sound;
		switch(phase)
		{
			case BlowOutPhases.COOLDOWN:
				sound = "bo_final";
			break;
			case BlowOutPhases.WARNING:
				sound = m_PsiWarningSounds.Get(sound_index);
			break;
			case BlowOutPhases.ACTIVE:
				sound = m_PsiActiveSounds.Get(sound_index);
			break;
			case BlowOutPhases.END:
			break;
		}

		m_SEngine = PlaySound(sound, 25);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopSound, 8 * 1000);
	}
}