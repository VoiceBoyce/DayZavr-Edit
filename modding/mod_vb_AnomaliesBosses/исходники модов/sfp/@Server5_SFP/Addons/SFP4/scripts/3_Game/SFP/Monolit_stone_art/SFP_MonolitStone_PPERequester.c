// ============================================================================
// МОНОЛИТ — PPE requester (модуль 3_Game)
//
// Этот класс ДЕЛАЕТ картинку на экране:
//   1) FilmGrain — «зерно» (очень сильное у центра)
//   2) Glow.Saturation — уводит цветность в ЧБ (у центра почти/полностью ЧБ)
//   3) WetDistort — искажение типа «жар/шимер»
//
// Где крутить силу/радиусы:
//   scripts\4_World\SFP\Monolit_stone_art\SFP_MonolitStone_Tuning.c
//
// Важно:
//   FilmGrain в DayZ нормально «разгоняется» только если поднят NVLIGHTPARAMS.NOISE_MULT.
//   Поэтому здесь используются сразу 2 компонента зерна: FilmGrain + NOISE_MULT.
// ============================================================================

class PPERequester_SFPMonolitStone extends PPERequester_GameplayBase
{
	// ID слоёв (просто числа, главное чтобы не пересекались с твоими другими модами)
	static const int L_SFP_MONOLIT_BASE = 5000;
	static const int L_SFP_MONOLIT_GRAIN = 5001;
	static const int L_SFP_MONOLIT_SAT = 5002;
	static const int L_SFP_MONOLIT_DISTORT = 5003;

	protected float m_LastStrength;

	// Вызывается клиентским менеджером, пока эффект активен
	void SetIntensity(float grainStrength, float satStrength, float distortStrength)
	{
		grainStrength = Math.Clamp(grainStrength, 0.0, 1.0);
		satStrength = Math.Clamp(satStrength, 0.0, 1.0);
		distortStrength = Math.Clamp(distortStrength, 0.0, 1.0);

		// --------------------
		// ЗЕРНО (FilmGrain) — КРУТИТСЯ В TUNING (NOISE_MULT_MAX / GRAIN_* )
		// --------------------
		float noiseMult = SFP_MonolitStone_Config.NOISE_MULT_MAX * grainStrength;
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS, PPELightIntensityParamsNative.PARAM_NOISE_MULT, false, noiseMult, L_SFP_MONOLIT_BASE, PPOperators.HIGHEST);

		SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_INTENSITY, false,
			SFP_MonolitStone_Config.GRAIN_INTENSITY_MAX * grainStrength,
			L_SFP_MONOLIT_GRAIN, PPOperators.HIGHEST);

		SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_SHARPNESS, false,
			SFP_MonolitStone_Config.Lerp(2.35, SFP_MonolitStone_Config.GRAIN_SHARPNESS_MAX, grainStrength),
			L_SFP_MONOLIT_GRAIN, PPOperators.HIGHEST);

		SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_GRAINSIZE, false,
			SFP_MonolitStone_Config.Lerp(2.75, SFP_MonolitStone_Config.GRAIN_SIZE_MIN, grainStrength),
			L_SFP_MONOLIT_GRAIN, PPOperators.LOWEST);

		// Зерно делаем монохромным (а ЧБ/цветность регулируем через Saturation ниже)
		SetTargetValueBool(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_MONOCHROMATIC, true, L_SFP_MONOLIT_GRAIN, PPOperators.SET);

		// --------------------
		// Ч/Б (потеря насыщенности)
		// --------------------
		float satVal = SFP_MonolitStone_Config.Lerp(1.0, SFP_MonolitStone_Config.SATURATION_MIN, satStrength);
		// Используем Glow.Saturation — так сделано во многих ванильных эффектах DayZ
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_SATURATION, false, satVal, L_SFP_MONOLIT_SAT, PPOperators.LOWEST);

		// --------------------
		// ИСКАЖЕНИЕ «жар/шимер» (WetDistort)
		// --------------------
		float effTop = SFP_MonolitStone_Config.DISTORT_EFFPOWER_MAX * distortStrength;
		float effBottom = SFP_MonolitStone_Config.DISTORT_EFFPOWER_MAX * distortStrength;
		float blurPower = SFP_MonolitStone_Config.DISTORT_BLURPOWER_MAX * distortStrength;
		float waveAmp = SFP_MonolitStone_Config.DISTORT_WAVEAMP_MAX * distortStrength;

		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_EFFPOWERTOP, false, effTop, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_EFFPOWERBOTTOM, false, effBottom, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_BLURPOWER, false, blurPower, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_LOCALBLURPOWER, false, blurPower, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);

		// Усиливаем амплитуды волн для «шумящего жара» (максимум у центра)
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_WAVEAMPX1, false, 0.005 + waveAmp, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_WAVEAMPX2, false, 0.004 + waveAmp, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_WAVEAMPY1, false, 0.009 + waveAmp, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.WetDistort, PPEWetDistort.PARAM_WAVEAMPY2, false, 0.007 + waveAmp, L_SFP_MONOLIT_DISTORT, PPOperators.HIGHEST);

		m_LastStrength = Math.Max(grainStrength, satStrength);
	}

	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);
		m_LastStrength = 0.0;
		// Тут ничего дополнительно не нужно: как только мы начнём ставить значения — WetDistort сам «оживёт»
	}

	override void OnStop(Param par = null)
	{
		m_LastStrength = 0.0;
		super.OnStop(par); // сброс к дефолту ванили
	}
}
