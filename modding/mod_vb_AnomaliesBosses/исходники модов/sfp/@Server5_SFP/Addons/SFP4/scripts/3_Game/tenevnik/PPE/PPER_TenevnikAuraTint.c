// Слабый жёлтый оттенок от ауры Теневника.
// Реализовано через PPEGlow -> COLORIZATIONCOLOR (уменьшаем синий канал).
// Управляется с клиента плагином PluginSFP_TenevnikClient (интенсивность 0..1).
//
// Совместимость:
//  - используем MULTIPLICATIVE (нейтральное значение {1,1,1,0})
//  - НЕ вызываем SetTargetValue*Default() вручную
//  - отдельный высокий слой

class PPERequester_TenevnikAuraTint extends PPERequester_GameplayBase
{
	protected float m_Intensity; // 0..1

	static const int L_SFP_TENEVNIK_TINT = 54030;

	// максимальное "урезание" синего (чем больше, тем сильнее желтизна)
	static const float BLUE_REDUCE_MAX = 0.88;

	void SetIntensity(float intensity01)
	{
		m_Intensity = Math.Clamp(intensity01, 0.0, 1.0);
	}

	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);
		SetRequesterUpdating(true);
	}

	override protected void OnStop(Param par = null)
	{
		m_Intensity = 0.0;
		super.OnStop(par);
		SetRequesterUpdating(false);
	}

	override protected void OnUpdate(float delta)
	{
		super.OnUpdate(delta);

		if (!m_IsRunning)
			return;

		float f = Math.Clamp(m_Intensity, 0.0, 1.0);
		if (f <= 0.001)
		{
			// нейтрально, не влияет
			SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {1.0, 1.0, 1.0, 0.0}, L_SFP_TENEVNIK_TINT, PPOperators.MULTIPLICATIVE);
			return;
		}

		float b = BLUE_REDUCE_MAX * f;
		float bb = Math.Clamp(1.0 - b, 0.08, 1.0);

		float rg = 1.0 + (0.18 * f);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {rg, rg, bb, 0.0}, L_SFP_TENEVNIK_TINT, PPOperators.MULTIPLICATIVE);
	}
}
