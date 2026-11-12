// ShumMutant - client-side invulnerability visuals.
// Weak blue light + very strong heat haze distortion.
// Uses only vanilla ScriptedLightBase/PointLightBase APIs.

class ShumMutantInvulnLight extends PointLightBase
{
	void ShumMutantInvulnLight()
	{
		ShumMutantTuning.EnsureLoaded(); // загрузка настроек из JSON

		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetFadeOutTime(0.2);

		SetRadiusTo(ShumMutantTuning.INVULN_LIGHT_RADIUS);
		SetBrightnessTo(ShumMutantTuning.INVULN_LIGHT_BRIGHTNESS);

		// Слабое синее освещение (RGB задаётся в ShumMutantConfig.json)
		vector c = ShumMutantTuning.INVULN_LIGHT_COLOR_RGB;
		SetDiffuseColor(c[0], c[1], c[2]);
		SetAmbientColor(c[0], c[1], c[2]);

		EnableHeatHaze(true);
		SetHeatHazeRadius(ShumMutantTuning.INVULN_HEAT_HAZE_RADIUS);
		SetHeatHazePower(ShumMutantTuning.INVULN_HEAT_HAZE_POWER);
	}

	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		// No per-frame logic.
	}
}
