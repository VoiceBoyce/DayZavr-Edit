class SFP_KrovososHeatLight extends PointLightBase
{
	void SFP_KrovososHeatLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(2.35);
		SetBrightnessTo(0.95);
		SetCastShadow(false);
		SetFadeOutTime(0.05);
		SetDiffuseColor(0.045, 0.014, 0.000);
		SetAmbientColor(0.026, 0.008, 0.000);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.0);
		SetFlickerSpeed(0.0);
		EnableHeatHaze(true);
		//SetHeatHazeRadius(0.68);
		SetHeatHazeRadius(0.48);
		SetHeatHazePower(0.145);
	}
};
