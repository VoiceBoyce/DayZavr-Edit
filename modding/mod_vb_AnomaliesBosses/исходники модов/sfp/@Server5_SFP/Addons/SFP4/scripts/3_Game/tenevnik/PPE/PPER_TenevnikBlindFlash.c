// Ослепление от взгляда на агрессивного Теневника.
// По визуалу похоже на эффект светошумовой гранаты (white overlay + blur + exposure),
// но без звукового оглушения (только PPE).
// Управляется плагином PluginSFP_TenevnikClient.
//
// Совместимость:
//  - используем свои высокие слои
//  - не трогаем ванильный PPERequester_FlashbangEffects
//  - после Stop() гарантированно чистим наши параметры (без "белого напыления")

class PPERequester_TenevnikBlindFlash extends PPERequester_GameplayBase
{
	protected float m_Intensity; // 0..1
	protected float m_DayTimeToggle;
	protected bool  m_ClearNextUpdate;

	// Высокие слои, чтобы не конфликтовать с ванилью/другими модами
	static const int L_SFP_TENEVNIK_FLASH_BLUR     = 54040;
	static const int L_SFP_TENEVNIK_FLASH_OVERLAY  = 54041;
	static const int L_SFP_TENEVNIK_FLASH_COLOR    = 54042;
	static const int L_SFP_TENEVNIK_FLASH_EXPOSURE = 54043;

	static const float EXPOSURE_MAX = 50.0;

	void SetIntensity(float intensity01)
	{
		m_Intensity = Math.Clamp(intensity01, 0.0, 1.0);
	}

	// Принудительно убрать эффект сразу (страховка от залипаний)
	void ForceClearNow()
	{
		m_Intensity = 0.0;
		m_ClearNextUpdate = true;
		SetRequesterUpdating(true);
	}

	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);
		m_ClearNextUpdate = false;
		SetRequesterUpdating(true);

		// Наивный селектор день/ночь как в ванильном FlashbangEffect
		m_DayTimeToggle = 5.0;
		if (g_Game && (g_Game.GetDayTime() >= 22.0 || g_Game.GetDayTime() < 7.0))
			m_DayTimeToggle = 10.0;
	}

	override protected void OnStop(Param par = null)
	{
		// Важно: НЕ выключаем updating сразу.
		// Иначе движок может не получить "нулевые" значения, и остаётся слабое белое напыление.
		m_Intensity = 0.0;
		m_ClearNextUpdate = true;

		super.OnStop(par);
		SetRequesterUpdating(true);
	}

	override protected void OnUpdate(float delta)
	{
		super.OnUpdate(delta);

		// При Stop() m_IsRunning уже false, но мы должны один раз отдать нулевые значения и дефолты
		if (!m_IsRunning && !m_ClearNextUpdate)
			return;

		float f = Math.Clamp(m_Intensity, 0.0, 1.0);

		if (m_ClearNextUpdate)
			f = 0.0;

		// overlay color (white) — альфа = f
		array<float> overlay_color = {1.0, 1.0, 1.0, f};

		// exposure (как у ванили)
		float exposure = 0.0;
		if (f > 0.0)
		{
			if (m_DayTimeToggle <= 5.0)
				exposure = Math.Clamp(f * m_DayTimeToggle, 0.0, EXPOSURE_MAX);
			else
			{
				exposure = Easing.EaseInQuart(f);
				exposure = Math.Clamp(EXPOSURE_MAX * exposure, 0.0, EXPOSURE_MAX);
			}
		}

		// blur
		SetTargetValueFloat(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY, true, f, L_SFP_TENEVNIK_FLASH_BLUR, PPOperators.ADD_RELATIVE);

		// overlay
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_OVERLAYFACTOR, true, f, L_SFP_TENEVNIK_FLASH_OVERLAY, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_OVERLAYCOLOR, overlay_color, L_SFP_TENEVNIK_FLASH_COLOR, PPOperators.SET);

		// exposure
		SetTargetValueFloat(PPEExceptions.EXPOSURE, PPEExposureNative.PARAM_INTENSITY, false, exposure, L_SFP_TENEVNIK_FLASH_EXPOSURE, PPOperators.ADD);

		if (m_ClearNextUpdate)
		{
			// Сбросить все параметры, которыми requester трогал PPE, обратно в дефолт.
			SetDefaultValuesAll();

			m_ClearNextUpdate = false;
			SetRequesterUpdating(false);
		}
	}
}
