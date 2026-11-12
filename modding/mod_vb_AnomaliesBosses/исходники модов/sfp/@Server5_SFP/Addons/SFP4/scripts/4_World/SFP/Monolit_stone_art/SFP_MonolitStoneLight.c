// ============================================================================
// МОНОЛИТ — свет (PointLight)
//
// Сделано по ванильной схеме (BonfireLight / большой огонь):
//  - CreateLight(0,0,0) + AttachOnMemoryPoint делается в объекте
//  - здесь — только параметры света
//
// Требование: цвет небесно‑синий
// ============================================================================

class SFP_MonolitStoneLight extends PointLightBase
{
	// гарантируем, что tuning применён до чтения конфига
	static bool s_Tuned;

	void SFP_MonolitStoneLight()
	{
		if (!s_Tuned)
		{
			SFP_MonolitStone_Tuning.Apply();
			s_Tuned = true;
		}

		SetVisibleDuringDaylight(true);

		// базовые значения (после Attach в объекте дополнительно делаем Fade... как в ваниле)
		SetRadiusTo(SFP_MonolitStone_Config.LIGHT_RADIUS);
		SetBrightnessTo(SFP_MonolitStone_Config.LIGHT_BRIGHTNESS);

		// Как у костра/большого огня: без теней.
		// Если включить тени и точка monolit_light окажется внутри геометрии — свет может полностью «задушиться».
		SetCastShadow(true);

		SetFadeOutTime(SFP_MonolitStone_Config.LIGHT_FADE_OUT_TIME);

		// Цвет: небесно‑синий. В ваниле допускаются значения > 1.0 (см. FireplaceLight).
		SetDiffuseColor(SFP_MonolitStone_Config.LIGHT_R, SFP_MonolitStone_Config.LIGHT_G, SFP_MonolitStone_Config.LIGHT_B);
		SetAmbientColor(SFP_MonolitStone_Config.LIGHT_R, SFP_MonolitStone_Config.LIGHT_G, SFP_MonolitStone_Config.LIGHT_B);

		SetFlareVisible(false);

		// Лёгкое мерцание — как у BonfireLight
		SetFlickerAmplitude(0.55);
		SetFlickerSpeed(0.75);

		// (не критично при SetCastShadow(false), но оставляем как у огня)
		SetDancingShadowsMovementSpeed(0.1);
		SetDancingShadowsAmplitude(0.03);

		// Heat-haze (оставляем твои значения)
		if (SFP_MonolitStone_Config.HEAT_HAZE_ENABLED > 0.5)
		{
			EnableHeatHaze(true);
			SetHeatHazeRadius(SFP_MonolitStone_Config.HEAT_HAZE_RADIUS);
			SetHeatHazePower(SFP_MonolitStone_Config.HEAT_HAZE_POWER);
		}
	}

	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
	}
}
