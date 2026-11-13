// Свет аномалий и детекторов. Перенесено из OOTS (AnomalyLights.c, Detectors.c) без изменений параметров.

class VB_AB_Light_ElectraIdle extends PointLightBase
{
	void VB_AB_Light_ElectraIdle()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6);
		SetBrightnessTo(0.3);
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.2, 0.5, 1.0);
		SetAmbientColor(0.2, 0.5, 1.0);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.7);
		SetFlickerSpeed(4.0);
		SetDancingShadowsMovementSpeed(0.4);
		SetDancingShadowsAmplitude(0.5);
		EnableHeatHaze(true);
		SetHeatHazeRadius(4.0);
		SetHeatHazePower(0.008);
	}
};

class VB_AB_Light_ElectraActive extends PointLightBase
{
	void VB_AB_Light_ElectraActive()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(20);
		SetBrightnessTo(100);
		SetAmbientColor(0.2, 0.2, 1.0);
		SetDiffuseColor(0.2, 0.2, 1.0);
		SetFadeOutTime(0.0);
		SetLifetime(1.5);
		SetDisableShadowsWithinRadius(-1);
		SetFlareVisible(true);
		SetFlickerAmplitude(1.0);
		SetFlickerSpeed(10.0);
		SetDancingShadowsMovementSpeed(10.0);
		SetDancingShadowsAmplitude(1.0);
		EnableHeatHaze(true);
		SetHeatHazeRadius(5.0);
		SetHeatHazePower(0.01);
	}
};

class VB_AB_Light_StudenIdle extends PointLightBase
{
	void VB_AB_Light_StudenIdle()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6);
		SetBrightnessTo(0.3);
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.0, 0.8, 0.1);
		SetAmbientColor(0.0, 0.8, 0.1);
		SetFlareVisible(false);
	}
};

class VB_AB_Light_StudenActive extends PointLightBase
{
	void VB_AB_Light_StudenActive()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(10);
		SetBrightnessTo(15);
		SetFlareVisible(false);
		SetAmbientColor(0.2, 1.0, 0.2);
		SetDiffuseColor(0.2, 1.0, 0.2);
		SetFadeOutTime(0.5);
		SetLifetime(1.0);
		SetDisableShadowsWithinRadius(-1);
		EnableHeatHaze(true);
		SetHeatHazeRadius(5.0);
		SetHeatHazePower(0.01);
	}
};

class VB_AB_Light_ZharkaActive extends PointLightBase
{
	void VB_AB_Light_ZharkaActive()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(15);
		SetBrightnessTo(30);
		SetFlareVisible(false);
		SetAmbientColor(0.9, 0.5, 0.3);
		SetDiffuseColor(0.9, 0.5, 0.3);
		SetFadeOutTime(0.15);
		SetLifetime(10.0);
		SetDisableShadowsWithinRadius(-1);
		SetDancingShadowsMovementSpeed(0.4);
		SetDancingShadowsAmplitude(0.5);
		SetFlickerAmplitude(0.5);
		SetFlickerSpeed(4.0);
		EnableHeatHaze(true);
		SetHeatHazeRadius(6.0);
		SetHeatHazePower(0.015);
	}
};

class VB_AB_Light_HoleraIdle extends PointLightBase
{
	void VB_AB_Light_HoleraIdle()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6);
		SetBrightnessTo(0.3);
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.0, 0.8, 0.1);
		SetAmbientColor(0.0, 0.8, 0.1);
		SetFlareVisible(false);
	}
};

class VB_AB_Light_HoleraActive extends PointLightBase
{
	void VB_AB_Light_HoleraActive()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(10);
		SetBrightnessTo(15);
		SetFlareVisible(false);
		SetAmbientColor(0.2, 1.0, 0.2);
		SetDiffuseColor(0.2, 1.0, 0.2);
		SetFadeOutTime(0.5);
		SetLifetime(1.0);
		SetDisableShadowsWithinRadius(-1);
		EnableHeatHaze(true);
		SetHeatHazeRadius(5.0);
		SetHeatHazePower(0.01);
	}
};

// Лампа детектора (Лоза, Огонёк)
class VB_AB_Light_DetectorLamp extends PointLightBase
{
	void VB_AB_Light_DetectorLamp()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1);
		SetSpotLightAngle(360);
		SetCastShadow(true);
		EnableSpecular(true);
		SetBrightnessTo(0.5);
		SetFadeOutTime(0.15);
		VB_SetColor(LOZA);
	}

	// цвет лампы под ретекстур (ТЗ 2.2): Лоза — янтарь, Огонёк — зелёный
	static const vector LOZA = "1.0 0.55 0.05";
	static const vector OGONEK = "0.15 1.0 0.25";

	void VB_SetColor(vector c)
	{
		SetAmbientColor(c[0], c[1], c[2]);
		SetDiffuseColor(c[0], c[1], c[2]);
	}

	void VB_SetIntensity(float percent)
	{
		SetBrightnessTo(4 * percent);
	}
};
