/**
 * SFP_DatcikAlarmLight
 * Небольшая тёплая "лампочка" (ванильная), не слишком яркая.
 * Само моргание делается на объекте датчика через SetEnabled() (таймером).
 */
class SFP_DatcikAlarmLight extends PointLightBase
{
	void SFP_DatcikAlarmLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(6.0);
		SetBrightnessTo(0.65);
		SetCastShadow(false);
		SetFlareVisible(false);

		// тёплый белый
		SetAmbientColor(0.95, 0.90, 0.80);
		SetDiffuseColor(0.95, 0.90, 0.80);

		FadeIn(0.1);
		SetFadeOutTime(0.1);
	}
}
