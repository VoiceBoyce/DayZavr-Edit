// SFP Vodka effects — кастомный PPE requester (постпроцесс)
//
// Даёт:
//  - лёгкое размытие по Гауссу (GaussFilter)
//  - «двоение»/шлейф (Ghost)
//  - хроматическую аберрацию (ChromAber)
//
// Где «крутить»:
//  - размытие: переменная blur ниже (формула + потолок 0.28)
//  - двоение: baseX/baseY + коэффициенты синусов offX/offY + вес шума w
//  - хром.аберрация: переменная ca (0.003 + 0.014*intensity)


class PPERequester_DrunkVodka extends PPERequester_GameplayBase
{
	// intensity: 0..3 (1 ≈ одна полная бутылка)
	// time: секунды с момента старта эффекта (клиент)
	static const int LAYER_VODKA = 950; // важно держать <1000 (1000 — overlay disconnect в ваниле)

	protected ref array<float> m_ChannelWeights = { 0, 0, 0, 0 };
	protected ref array<float> m_ColorMod = { 1, 1, 1, 1 };

	void SetVodkaEffects(float intensity, float time)
	{
		// Масштабируем эффект до ~3 бутылок
		intensity = Math.Clamp(intensity, 0.0, 3.0);

		if (!m_IsRunning)
			Start();

		float n = Math.Clamp(intensity / 3.0, 0.0, 1.0);

		// --- Размытие (Gauss) ---
		// Сделано «слабым, но заметным».
		// PPOperators.SET используем, чтобы размытие не «съедали» другие PP requesters.
		// Порог/сила: меняй числа 0.06, 0.20 и потолок 0.28
		//float blur = Math.Clamp(0.06 + (0.20 * Math.Pow(n, 1.25)), 0.0, 0.28);
		float blur = Math.Clamp(0.06 + (0.20 * Math.Pow(n, 1.25)), 0.0, 0.40);
		SetTargetValueFloat(PostProcessEffectType.GaussFilter, PPEGaussFilter.PARAM_INTENSITY, true, blur, LAYER_VODKA, PPOperators.SET);

		// --- «Двоение»/шлейф (Ghost) ---
		// Базовые параметры шума — безопасно дергать каждый тик
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_NOISE_SCALE_X, false, 0.05, LAYER_VODKA, PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_NOISE_SCALE_Y, false, 0.05, LAYER_VODKA, PPOperators.LOWEST);

		// Сдвиги «призрака» (чем выше intensity — тем сильнее двоение)
		float baseX = 12.0 + (35.0 * intensity);
		float baseY = 7.0 + (20.0 * intensity);

		float offX = (Math.Sin(time * 0.75) * baseX) + (Math.Sin(time * 1.65) * baseX * 0.15);
		float offY = (Math.Cos(time * 0.60) * baseY) + (Math.Sin(time * 1.20) * baseY * 0.12);

		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_GHOST_OFFSET_X, false, offX, LAYER_VODKA, PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_GHOST_OFFSET_Y, false, offY, LAYER_VODKA, PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_GHOST_SCALE_X, false, 1.0, LAYER_VODKA, PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_GHOST_SCALE_Y, false, 1.0, LAYER_VODKA, PPOperators.LOWEST);

		// Вес «зерна/шума» — добавляет ощущение «двоения»
		float w = Math.Clamp(0.10 + (0.55 * n), 0.0, 1.0);
		m_ChannelWeights[0] = w;
		m_ChannelWeights[1] = w * 0.85;
		m_ChannelWeights[2] = w * 0.70;
		m_ChannelWeights[3] = 0.0;

		SetTargetValueColor(PostProcessEffectType.Ghost, PPEGhost.PARAM_NOISE_CHANNEL_WEIGHT, m_ChannelWeights, LAYER_VODKA, PPOperators.SET);

		float nOffX = (Math.Sin(time * 0.29) * 0.5 + 0.5) * (0.2 + 0.6 * n);
		float nOffY = (Math.Cos(time * 0.23) * 0.5 + 0.5) * (0.2 + 0.6 * n);

		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_NOISE_OFFSET_X, false, nOffX, LAYER_VODKA, PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Ghost, PPEGhost.PARAM_NOISE_OFFSET_Y, false, nOffY, LAYER_VODKA, PPOperators.ADD);

		// A-канал — кривая (0=линейная, 1=экспонента). Чем выше — тем «ядовитей» шлейф.
		float curve = Math.Clamp(0.45 + (0.25 * n), 0.0, 1.0);
		m_ColorMod[0] = 1.0;
		m_ColorMod[1] = 1.0;
		m_ColorMod[2] = 1.0;
		m_ColorMod[3] = curve;

		SetTargetValueColor(PostProcessEffectType.Ghost, PPEGhost.PARAM_GHOST_COLOR_MOD, m_ColorMod, LAYER_VODKA, PPOperators.SET);

		// --- Хром. аберрация (ChromAber) ---
		// «цветовые края» по контурам. Можно увеличить множитель 0.014 если хочешь сильнее.
		float ca = Math.Clamp(0.003 + (0.014 * intensity), 0.0, 0.05);
		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERX, false, ca, LAYER_VODKA, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERY, false, ca * 0.85, LAYER_VODKA, PPOperators.SET);
	}
}
