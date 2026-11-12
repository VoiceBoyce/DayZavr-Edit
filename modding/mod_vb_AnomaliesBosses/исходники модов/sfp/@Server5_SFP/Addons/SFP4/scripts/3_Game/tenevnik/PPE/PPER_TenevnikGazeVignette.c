// "Выжигание глаз" при взгляде на агрессивного Теневника вблизи.
// Важно для совместимости:
//  - НЕ используем SetTargetValue*Default() вручную
//  - НЕ используем OVERRIDE/SET на нуле
//  - Работает на своих слоях в высоком диапазоне
// Управляется плагином PluginSFP_TenevnikClient (интенсивность 0..1).

class PPERequester_TenevnikGazeVignette extends PPERequester_GameplayBase
{
	protected float m_Intensity; // 0..1

	// Высокие слои, чтобы не конфликтовать с ванилью/другими модами
	static const int L_SFP_TENEVNIK_VIGNETTE     = 54025;
	static const int L_SFP_TENEVNIK_VIGNETTE_COL = 54026;

	// Бёрлап использует 2.0; нам хватает мягче
	static const float VIGNETTE_MAX = 1.6;
	// Интенсивность белого по краям (0..1)
	static const float WHITE_MAX = 0.35;

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
		super.OnStop(par); // вернёт параметры, которые трогал requester, к дефолту (только для этого requester)
		SetRequesterUpdating(false);
	}

	override protected void OnUpdate(float delta)
	{
		super.OnUpdate(delta);

		if (!m_IsRunning)
			return;

		float f = Math.Clamp(m_Intensity, 0.0, 1.0);

		// 1) сила виньетки
		float v = VIGNETTE_MAX * f;
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, v, L_SFP_TENEVNIK_VIGNETTE, PPOperators.HIGHEST);

		// 2) белый цвет по краям (добавляем, чтобы не перетирать чужие эффекты)
		float w = WHITE_MAX * f;
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {w, w, w, 0.0}, L_SFP_TENEVNIK_VIGNETTE_COL, PPOperators.ADD);
	}
}
