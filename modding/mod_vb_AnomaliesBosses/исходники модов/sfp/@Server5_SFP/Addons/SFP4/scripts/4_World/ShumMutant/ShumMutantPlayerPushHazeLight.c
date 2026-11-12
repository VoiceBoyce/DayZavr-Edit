// ShumMutant - искажение воздуха НА ИГРОКЕ при толчке/броске (эффект "телекинеза").
// Это клиентский визуальный эффект: сильный heat haze вокруг игрока на короткое время.
// Использует только ванильные ScriptedLightBase/PointLightBase.

class ShumMutantPlayerPushHazeLight extends PointLightBase
{
	void ShumMutantPlayerPushHazeLight()
	{
		ShumMutantTuning.EnsureLoaded();

		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetFadeOutTime(0.15);

		// Важно: если радиус/яркость 0, движок может удалить light и haze не будет.
		SetRadiusTo(ShumMutantTuning.PUSH_HAZE_LIGHT_RADIUS);
		SetBrightnessTo(ShumMutantTuning.PUSH_HAZE_LIGHT_BRIGHTNESS);

		float c = ShumMutantTuning.PUSH_HAZE_LIGHT_COLOR;
		SetDiffuseColor(c, c, c);
		SetAmbientColor(c, c, c);

		EnableHeatHaze(true);
		SetHeatHazeRadius(ShumMutantTuning.PUSH_HAZE_RADIUS);
		SetHeatHazePower(ShumMutantTuning.PUSH_HAZE_POWER);
	}

	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		// эффекты обновляются движком, логика не нужна
	}
}
