class IBMindStateSoundEventBase extends InfectedBanditSoundEventBase
{
	override bool CanPlay()
	{
		if( !super.CanPlay() )
		{
			return false;
		}
		return true;
	}
}

class IBCalmIdleSoundEvent extends IBMindStateSoundEventBase
{
	void IBCalmIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CALM_IDLE;
		m_SoundSetNameRoot = "CalmIdle";
	}
}

class IBCalmMoveSoundEvent extends IBMindStateSoundEventBase
{
	void IBCalmMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CALM_MOVE;
		m_SoundSetNameRoot = "CalmMove";
	}
}

class IBDisturbedIdleSoundEvent extends IBMindStateSoundEventBase
{
	void IBDisturbedIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_DISTURBED_IDLE;
		m_SoundSetNameRoot = "DisturbedIdle";
	}
}

class IBChaseMoveSoundEvent extends IBMindStateSoundEventBase
{
	void IBChaseMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CHASE_MOVE;
		m_SoundSetNameRoot = "ChaseMove";
	}
}

class IBAlertedIdleSoundEvent extends IBMindStateSoundEventBase
{
	void IBAlertedIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_ALERTED_IDLE;
		m_SoundSetNameRoot = "AlertedIdle";
	}
}

class IBAlertedMoveSoundEvent extends IBMindStateSoundEventBase
{
	void IBAlertedMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_ALERTED_MOVE;
		m_SoundSetNameRoot = "AlertedMove";
	}
}