class InfectedBanditSoundEventHandler extends SoundEventHandler
{
	const int SOUND_EVENTS_MAX = EInfectedSoundEventID.ENUM_COUNT;
	static ref InfectedBanditSoundEventBase m_AvailableStates[SOUND_EVENTS_MAX];
	ref InfectedBanditSoundEventBase m_CurrentState;
	InfectedBanditBase m_Infected;
	
	void InfectedBanditSoundEventHandler(InfectedBanditBase pInfected)
	{
		m_Infected = pInfected;
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CALM_IDLE] = new IBCalmIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CALM_MOVE] = new IBCalmMoveSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_DISTURBED_IDLE] = new IBDisturbedIdleSoundEvent();
		//m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CHASE_IDLE] = new IBChaseIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CHASE_MOVE] = new IBChaseMoveSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_ALERTED_IDLE] = new IBAlertedIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_ALERTED_MOVE] = new IBAlertedMoveSoundEvent();
	}
	
	override static int GetSoundEventType(int id)
	{
		return m_AvailableStates[id].GetSoundEventType();
	}
	
	override int GetCurrentStateEventID()
	{
		if (m_CurrentState)
		{
			return m_CurrentState.GetSoundEventID();
		}
		return -1;
	}
	
	override int GetCurrentStateEventType()
	{
		if (m_CurrentState)
		{
			return m_CurrentState.GetSoundEventType();
		}
		return -1;
	}
	
	void Stop()
	{
		if ( m_CurrentState )
		{
			m_CurrentState.Stop();
		}
	}
	
	void SoftStop()
	{
		if ( m_CurrentState )
		{
			m_CurrentState.SoftStop();
		}
	}
	
	bool IsPlaying()
	{
		if ( m_CurrentState && m_CurrentState.IsSoundCallbackExist() )
		{
			return true;
		}

		return false;
	}
	
	override bool PlayRequest(int id, bool sent_from_server = false)
	{
		InfectedBanditSoundEventBase requested_state = m_AvailableStates[id];

		if ( !requested_state.CanPlay() )
		{
			return false;
		}
		
		if (m_CurrentState)
		{
		 	if (!m_CurrentState.IsSoundCallbackExist())
			{
				//Print("Missing callback - cleanup and continue");
				delete m_CurrentState;
			}
			
			//! same sound event - skip
			if (GetCurrentStateEventID() == id)
			{
				//Print("Same ID - skipping");
				return false;
			}
		}
		
		if (m_CurrentState)
		{
			m_CurrentState.SoftStop();
			m_CurrentState = InfectedBanditSoundEventBase.Cast(requested_state.ClassName().ToType().Spawn());
			m_CurrentState.Init(m_Infected);
			m_CurrentState.Play();
			return true;
		}
		else
		{
			m_CurrentState = InfectedBanditSoundEventBase.Cast(requested_state.ClassName().ToType().Spawn());
			m_CurrentState.Init(m_Infected);
			//m_CurrentState.Play();
			return true;
		}

		return false;//should never get here
	}
}