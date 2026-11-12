modded class Animal_CanisLupus
{
	protected bool m_SFP_MutantSummoned;
	protected PlayerBase m_SFP_MutantTarget;
	protected ref Timer m_SFP_MutantPrimeTimer;
	protected ref NoiseParams m_SFP_MutantNoiseParams;
	
	void SFP_MW_MarkMutantSummoned(bool state = true)
	{
		m_SFP_MutantSummoned = state;
		if (!state)
		{
			m_SFP_MutantTarget = null;
			if (m_SFP_MutantPrimeTimer && m_SFP_MutantPrimeTimer.IsRunning())
				m_SFP_MutantPrimeTimer.Stop();
			SFP_MW_ClearPrimeAggro();
		}
	}
	
	bool SFP_MW_IsMutantSummoned()
	{
		return m_SFP_MutantSummoned;
	}
	
	void SFP_MW_SetMutantTarget(PlayerBase player)
	{
		m_SFP_MutantTarget = player;
	}
	
	void SFP_MW_RestoreStandardHealth()
	{
		// Без DamageSystem.ResetAllZones(this): она без проверки пишет Blood/Shock в каждую зону
		// и может уронить сервер, если в WolfTypes указан зверь без этих типов здоровья.
		SFP_MW_ResetHealthTypeIfExists("", "Health", true);
		SFP_MW_ResetHealthTypeIfExists("", "Blood");
		SFP_MW_ResetHealthTypeIfExists("", "Shock");

		TStringArray zones = new TStringArray;
		GetDamageZones(zones);
		foreach (string zone : zones)
		{
			SFP_MW_ResetHealthTypeIfExists(zone, "Health", true);

			TStringArray addTypes = new TStringArray;
			if (GetAdditionalHealthTypes(zone, addTypes))
			{
				foreach (string healthType : addTypes)
				{
					SFP_MW_ResetHealthTypeIfExists(zone, healthType, true);
				}
			}
		}
	}

	protected void SFP_MW_ResetHealthTypeIfExists(string zone, string healthType, bool assumeExists = false)
	{
		if (!assumeExists)
		{
			TStringArray addTypes = new TStringArray;
			if (!GetAdditionalHealthTypes(zone, addTypes))
				return;
			if (addTypes.Find(healthType) < 0)
				return;
		}

		float maxValue = GetMaxHealth(zone, healthType);
		if (maxValue > 0.0)
			SetHealth(zone, healthType, maxValue);
	}
	
	void SFP_MW_RefreshAggro(PlayerBase preferredTarget = null)
	{
		if (preferredTarget && preferredTarget.IsAlive())
			m_SFP_MutantTarget = preferredTarget;
		
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;
		
		PlayerBase target = m_SFP_MutantTarget;
		if (!target || !target.IsAlive() || target.IsUnconscious() || target.GetCommand_Vehicle())
			return;
		
		if (GetAIAgent())
			GetAIAgent().SetKeepInIdle(false);
		
		DayZAnimalInputController animalController = GetInputController();
		if (animalController)
		{
			animalController.OverrideAlertLevel(true, true, 4, 1.0);
			animalController.OverrideBehaviourSlot(true, DayZAnimalBehaviourSlot.SPECIFIC_THREAT);
			animalController.OverrideBehaviourAction(true, DayZAnimalBehaviourAction.APPROACHING_INPUT);
		}
		
		if (GetGame().GetNoiseSystem())
		{
			if (!m_SFP_MutantNoiseParams)
			{
				m_SFP_MutantNoiseParams = new NoiseParams();
				m_SFP_MutantNoiseParams.Load("WolfRoarNoise");
			}
			if (m_SFP_MutantNoiseParams)
				GetGame().GetNoiseSystem().AddNoiseTarget(target.GetPosition(), 4.0, m_SFP_MutantNoiseParams, 1.35);
		}
		
		if (!m_SFP_MutantPrimeTimer)
			m_SFP_MutantPrimeTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		m_SFP_MutantPrimeTimer.Stop();
		m_SFP_MutantPrimeTimer.Run(1.75, this, "SFP_MW_ClearPrimeAggro");
	}
	
	void SFP_MW_ClearPrimeAggro()
	{
		DayZAnimalInputController animalController = GetInputController();
		if (!animalController)
			return;
		
		animalController.OverrideAlertLevel(false, false, 0, 0.0);
		animalController.OverrideBehaviourSlot(false, DayZAnimalBehaviourSlot.NONE);
		animalController.OverrideBehaviourAction(false, DayZAnimalBehaviourAction.NONE_INPUT);
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (!GetGame().IsServer() || !m_SFP_MutantSummoned)
			return;
		
		SFP_MW_ConfigData cfg = SFP_MW_ConfigManager.Get().GetData();
		if (cfg && cfg.DeadWolfDespawnDelay > 0)
			SetLifetime(cfg.DeadWolfDespawnDelay);
	}
};
