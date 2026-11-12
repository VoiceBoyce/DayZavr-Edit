// ShumMutant - lightweight client-side heat haze (air distortion) attached to the mutant.
// Uses vanilla ScriptedLightBase/PointLightBase heat haze. Kept very dim (colors = 0) to avoid visible light,
// while preserving distortion day and night.

class ShumMutantHeatHazeLight extends PointLightBase
{
	void ShumMutantHeatHazeLight()
	{
		ShumMutantTuning.EnsureLoaded(); // загрузка настроек из JSON

		// Keep active day & night
		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetFadeOutTime(0.2);

		// Engine auto-deletes lights with radius/brightness <= 0
		SetRadiusTo(ShumMutantTuning.HEAT_HAZE_LIGHT_RADIUS);
		SetBrightnessTo(ShumMutantTuning.HEAT_HAZE_LIGHT_BRIGHTNESS);
		// Нулевые цвета + низкая яркость часто дают "нулевую" лампу и haze не виден.
		// Поэтому оставляем очень слабый цвет (почти незаметен), зато искажение стабильно работает днем и ночью.
		float c = ShumMutantTuning.HEAT_HAZE_LIGHT_COLOR;
		SetDiffuseColor(c, c, c);
		SetAmbientColor(c, c, c);

		EnableHeatHaze(true);
		SetHeatHazeRadius(ShumMutantTuning.HEAT_HAZE_RADIUS);
		SetHeatHazePower(ShumMutantTuning.HEAT_HAZE_POWER);
	}

	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		// No per-frame logic needed.
	}
}
