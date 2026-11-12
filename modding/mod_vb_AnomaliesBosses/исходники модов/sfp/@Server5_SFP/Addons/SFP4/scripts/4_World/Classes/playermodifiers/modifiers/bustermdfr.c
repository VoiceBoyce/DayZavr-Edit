class BusterMdfr : ModifierBase
{
	static const float BASE_LIFETIME_FULL 	= 300;
	static const float ADD_LIFETIME_FULL 	= 65;
	static const float MIN_LIFETIME_ON_DRINK = 25;
	static const float MAX_LIFETIME 		 = 1800;

	// ЧТО КРУТИТЬ: ускорение восстановления стамины (2.0 = в 2 раза быстрее).
	static const float STAMINA_RECOVERY_MULTIPLIER = 3.0;

	ref Param1<float> m_TotalLifetime;

	override void Init()
	{
		m_TrackActivatedTime 	= true;
		m_IsPersistent 			= true;
		m_ID 					= BusterWorldConstants.MDF_BUSTER;

		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;

		DisableActivateCheck();

		m_TotalLifetime = new Param1<float>(0);
		MakeParamObjectPersistent(m_TotalLifetime);
	}

	override bool ActivateCondition(PlayerBase player) { return false; }
	override void OnReconnect(PlayerBase player) { OnActivate(player); }

	protected bool Buster_ShouldRunClientFX()
	{
		if (!GetGame()) return false;
		if (GetGame().IsMultiplayer()) return GetGame().IsClient();
		return true;
	}

	protected void BusterPPE_Start()
	{
		if (!Buster_ShouldRunClientFX()) return;

		PPERequester_BusterChromAbb req = PPERequester_BusterChromAbb.Cast(PPERequesterBank.GetRequester(PPERequester_BusterChromAbb));
		if (req) req.Start(new Param1<float>(BusterConstants.PPE_CHROMABER_POWER));
	}

	protected void BusterPPE_Stop()
	{
		if (!Buster_ShouldRunClientFX()) return;

		PPERequester_BusterChromAbb req = PPERequester_BusterChromAbb.Cast(PPERequesterBank.GetRequester(PPERequester_BusterChromAbb));
		if (req) req.Stop();
	}

	override void OnActivate(PlayerBase player)
	{
		if (!player)
			return;

		// Ускоряем восстановление стамины (через собственный recovery modifier)
		StaminaHandler sh = player.GetStaminaHandler();
		if (sh)
			sh.ActivateRecoveryModifier(BusterWorldConstants.STAMINA_RECOVERY_TYPE);

		// Пересчёт кешированного веса: carry-бонус считается через PlayerBase.GetWeightSpecialized()
		player.SetWeightDirty();

		// ===== ИКОНКА "ТАБЛЕТКА" В СТАТУС-БАРЕ (как у ванильных лекарств) =====
		player.IncreaseHealingsCount();

		// Если хочешь оставить экранный эффект — раскомментируй:
		// BusterPPE_Start();
	}

	override void OnDeactivate(PlayerBase player)
	{
		if (!player)
			return;

		StaminaHandler sh = player.GetStaminaHandler();
		if (sh)
			sh.DeactivateRecoveryModifier(BusterWorldConstants.STAMINA_RECOVERY_TYPE);

		player.SetWeightDirty();

		// ===== УБИРАЕМ ИКОНКУ "ТАБЛЕТКА" =====
		player.DecreaseHealingsCount();

		// Если оставлял экранный эффект — раскомментируй:
		// BusterPPE_Stop();

		// Сбрасываем накопленную длительность
		m_TotalLifetime.param1 = 0;
	}
override bool DeactivateCondition(PlayerBase player)
	{
		if (m_TotalLifetime.param1 <= 0) return true;
		return GetAttachedTime() >= m_TotalLifetime.param1;
	}

	void SetLifetime(float seconds) { m_TotalLifetime.param1 = Math.Clamp(seconds, 0, MAX_LIFETIME); }
	void AddLifetime(float seconds) { SetLifetime(m_TotalLifetime.param1 + seconds); }
	float GetLifetime() { return m_TotalLifetime.param1; }
}
