// SFP NonStop energy drink - stamina regen buff modifier
// Shows vanilla "pills" badge while active.
class SFP_NonStopStaminaRegenMdfr : ModifierBase
{
	protected float m_EndTimeSeconds;
	ref HumanMovementState m_MovementState;

	override void Init()
	{
		m_TrackActivatedTime   = true;
		m_IsPersistent         = false;
		m_ID                   = SFP_NonStopConstants.MDF_NONSTOP_STAMINA_REGEN;
		m_TickIntervalInactive = DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive   = 1;

		DisableActivateCheck(); // activated by request (drink)

		m_EndTimeSeconds = 0;
		m_MovementState = new HumanMovementState();
	}

	// Called from the drink item each time some quantity is consumed
	void AddDuration(float seconds)
	{
		if (seconds <= 0)
			return;

		if (!IsActive())
		{
			m_EndTimeSeconds = seconds;
			ActivateRequest(EActivationType.TRIGGER_EVENT_ON_ACTIVATION);

			// Apply instantly (otherwise we'd wait for ModifiersManager tick)
			Activate();
		}
		else
		{
			m_EndTimeSeconds += seconds;
		}
	}

	override void OnActivate(PlayerBase player)
	{
		// Show standard "pills" badge while the buff is active (client-side UI)
		if (player && player.GetNotifiersManager())
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);

		// Gameplay effects should be server-authoritative
		if (!GetGame().IsServer() || !player)
			return;

		StaminaHandler sh = player.GetStaminaHandler();
		if (sh)
			sh.ActivateRecoveryModifier(SFP_NonStopConstants.STAMINA_RECOVERY_TYPE);
	}

	override void OnDeactivate(PlayerBase player)
	{
		// Hide standard "pills" badge when buff ends
		if (player && player.GetNotifiersManager())
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);

		if (!GetGame().IsServer() || !player)
		{
			m_EndTimeSeconds = 0;
			return;
		}

		StaminaHandler sh = player.GetStaminaHandler();
		if (sh)
			sh.DeactivateRecoveryModifier(SFP_NonStopConstants.STAMINA_RECOVERY_TYPE);

		m_EndTimeSeconds = 0;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return GetAttachedTime() >= m_EndTimeSeconds;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		// Keep the badge active (some builds may clear notifiers unless refreshed)
		if (player && player.GetNotifiersManager())
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);

		if (!GetGame().IsServer() || !player)
			return;

		// Extra hunger drain while buff is active:
		// We mirror HungerMdfr logic and apply only the EXTRA part ( +20% by default )
		player.GetMovementState(m_MovementState);

		float energy = player.GetStatEnergy().Get();
		float metabolic_speed = MiscGameplayFunctions.GetEnergyMetabolicSpeed(m_MovementState.m_iMovement);

		float modifier = energy / PlayerConstants.SL_ENERGY_MAX + PlayerConstants.CONSUMPTION_MULTIPLIER_BASE;
		metabolic_speed *= modifier;

		float extra = metabolic_speed * (SFP_NonStopConstants.ENERGY_DRAIN_MULTIPLIER - 1.0) * deltaT;
		player.GetStatEnergy().Add(-extra);
	}
}
