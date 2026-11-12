modded class StaminaHandler
{
	override void Init()
	{
		super.Init();

		// Регистрируем собственные recovery modifiers (не трогаем ванильные ключи типа DROWNING).
		RegisterRecoveryModifier(BusterWorldConstants.STAMINA_RECOVERY_TYPE, BusterMdfr.STAMINA_RECOVERY_MULTIPLIER);
		RegisterRecoveryModifier(SFP_NonStopConstants.STAMINA_RECOVERY_TYPE, SFP_NonStopConstants.STAMINA_RECOVERY_MULTIPLIER);
	}

	protected void RegisterRecoveryModifier(EStaminaMultiplierTypes type, float mult)
	{
		if (m_RegisteredRecoveryModifiers.Contains(type))
			m_RegisteredRecoveryModifiers.Set(type, mult);
		else
			m_RegisteredRecoveryModifiers.Insert(type, mult);
	}
}
