// Тепловое искажение (эффект жара) через PointLightBase.
// Важно: heat-haze у DayZ завязан на LightSource, и при "нулевом" свете может не рисоваться.
// Поэтому держим очень маленький радиус подсветки (чтобы почти не было видно света),
// но яркость/цвет оставляем ненулевыми — так искажение стабильно видно.

class SFP_TenevnikAuraLight extends PointLightBase
{
	// Множители для параметров из json
	static const float HAZE_RADIUS_MUL = 7.5;
	static const float HAZE_POWER_MUL  = 3.0;

	void SFP_TenevnikAuraLight()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetFadeOutTime(0.25);

		// Очень слабая подсветка (малый радиус -> почти не заметно, но heat-haze живёт)
		SetDiffuseColor(0.05, 0.05, 0.05);
		SetAmbientColor(0.05, 0.05, 0.05);

		EnableHeatHaze(true);
		SetHeatHazeRadius(0.25);
		SetHeatHazePower(0.020);

		SetRadiusTo(2.0);
		SetBrightnessTo(0.35);
	}

	void ApplyActive(float hazeRadius, float hazePower)
	{
		hazeRadius *= (HAZE_RADIUS_MUL * 1.5); // ACTIVE чуть больше
		hazePower  *= (HAZE_POWER_MUL  * 1.5);

		hazeRadius = Math.Clamp(hazeRadius, 0.22, 0.95);
		hazePower  = Math.Clamp(hazePower,  0.015, 0.150);

		EnableHeatHaze(true);

		// Свет минимальный: радиус маленький, яркость умеренная (для стабильного heat-haze)
		SetRadiusTo(2.5);
		SetBrightnessTo(0.55);

		SetHeatHazeRadius(hazeRadius);
		SetHeatHazePower(hazePower);
	}

	void ApplyReload(float hazeRadius, float hazePower)
	{
		hazeRadius *= HAZE_RADIUS_MUL;
		hazePower  *= HAZE_POWER_MUL;

		hazeRadius = Math.Clamp(hazeRadius, 0.18, 0.80);
		hazePower  = Math.Clamp(hazePower,  0.012, 0.110);

		EnableHeatHaze(true);

		SetRadiusTo(2.2);
		SetBrightnessTo(0.45);

		SetHeatHazeRadius(hazeRadius);
		SetHeatHazePower(hazePower);
	}

	void ApplyOff()
	{
		EnableHeatHaze(false);
		SetBrightnessTo(0.0);
	}
}
