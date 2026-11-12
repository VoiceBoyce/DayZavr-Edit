// SFP Vodka effects — вмешательство в стамину
//
// Что делает:
//  - замедляет восстановление стамины (RecoveryMultiplier)
//  - режет максимум стамины (cap)
//  - ускоряет расход стамины (DepletionMultiplier)
//
// ВАЖНО:
//  - мы НЕ расширяем никакие enum ("modded enum" в DayZ обычно не поддерживается и ломает компиляцию)
//  - мы просто умножаем ванильные значения на свои множители

modded class StaminaHandler
{
	// 1.0 = норма, <1.0 = реген медленнее
	protected float m_SFP_VodkaRecoveryMultiplier = 1.0;

	// 1.0 = норма, <1.0 = меньше максимальная стамина (cap)
	protected float m_SFP_VodkaCapMultiplier = 1.0;

	// 1.0 = норма, >1.0 = стамина тратится быстрее
	protected float m_SFP_VodkaDepletionMultiplier = 1.0;

	// Вызывается из PlayerBase когда меняется уровень «пьянства»
	void SFP_SetVodkaRecoveryMultiplier(float multiplier)
	{
		m_SFP_VodkaRecoveryMultiplier = Math.Clamp(multiplier, 0.0, 1.0);
		// Форсим пересчёт, чтобы ванильные модификаторы + наш множитель применились сразу
		RecalculateRecoveryMultiplier();
	}

	// Вызывается из PlayerBase когда меняется уровень «пьянства»
	void SFP_SetVodkaCapMultiplier(float multiplier)
	{
		m_SFP_VodkaCapMultiplier = Math.Clamp(multiplier, 0.05, 1.0);
	}

	// Вызывается из PlayerBase когда меняется уровень «пьянства»
	void SFP_SetVodkaDepletionMultiplier(float multiplier)
	{
		m_SFP_VodkaDepletionMultiplier = Math.Max(1.0, multiplier);
		RecalculateDepletionMultiplier();
	}

	// Ваниль при пересчёте вызывает SetRecoveryMultiplier(val).
	// Мы перехватываем и домножаем ванильное значение на «алко-множитель».
	override void SetRecoveryMultiplier(float val)
	{
		super.SetRecoveryMultiplier(val * m_SFP_VodkaRecoveryMultiplier);
	}

	// Ваниль при пересчёте вызывает SetDepletionMultiplier(val).
	override void SetDepletionMultiplier(float val)
	{
		super.SetDepletionMultiplier(val * m_SFP_VodkaDepletionMultiplier);
	}

	// После того как ваниль рассчитала cap (от нагрузки/перков и т.п.), мы режем его дополнительно.
	override void Update(float deltaT, int pCurrentCommandID)
	{
		super.Update(deltaT, pCurrentCommandID);

		// Делаем это только на сервере, чтобы не было странных рассинхронов
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (m_SFP_VodkaCapMultiplier < 0.999)
		{
			float baseCap = m_StaminaCap;
			m_StaminaCap = Math.Max(CfgGameplayHandler.GetStaminaMinCap(), baseCap * m_SFP_VodkaCapMultiplier);
			// Подрезаем текущую стамину под новый максимум
			m_Stamina = Math.Min(m_Stamina, m_StaminaCap);
			m_StaminaSynced = Math.Min(m_StaminaSynced, m_StaminaCap);
		}
	}
}
