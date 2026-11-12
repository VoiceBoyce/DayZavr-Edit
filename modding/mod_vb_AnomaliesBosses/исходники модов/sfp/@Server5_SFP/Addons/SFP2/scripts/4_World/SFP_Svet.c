
class SFP_LightLamp_Light extends SpotLightBase//поинт это свет от точки типо как факел во круг, а СПОТ это как у фонаря идет луч
{
	
	/*void FH_Combine_Light()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 10 );
		SetBrightnessTo( 0.02 );
		SetCastShadow(false);
		SetDiffuseColor(0, 206, 209);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}*/
	/*void SFP_Bankomat_Ekran_svet()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(2);
		SetSpotLightAngle(140);
		SetCastShadow(true);
		FadeIn(0.06);
		SetFadeOutTime(0.1);
		SetBrightnessTo( 0.01 );
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(248, 248, 255);
		SetDiffuseColor(248, 248, 255);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
		SetVisibleDuringDaylight( false ); Установить видимость при дневном свете (false);
		SetRadiusTo( 1 ); Установить радиус на (1);
		SetBrightnessTo( 3.0 ); Установить яркость на (3.0);
		SetCastShadow( true );Установить отбрасываемую тень (true);
		SetFadeOutTime( 0.2 ); Установите время затухания (0,2);
		SetDiffuseColor( 0.2, 0.45, 0.7 );Установите диффузный цвет (0,2, 0,45, 0,7);
		SetAmbientColor( 0.2, 0.45, 0.7 );Установить окружающий цвет (0,2, 0,45, 0,7);
		SetFlareVisible( false ); Установить видимость вспышки (false);
		SetFlickerAmplitude( 0.35 ); Установите амплитуду мерцания (0,35);
		SetFlickerSpeed( 0.55 ); Установите скорость мерцания (0,55);
		SetDancingShadowsMovementSpeed( 0.05 ); Установите скорость движения танцующих теней (0,05);
		SetDancingShadowsAmplitude( 0.03 ); Установить амплитуду танцующих теней (0,03);
		EnableHeatHaze( true); //Включить тепловую дымку (true);
		SetHeatHazeRadius( 0.08 ); //Установить радиус теплового тумана (0,08);
		SetHeatHazePower( 0.015 ); //Установить мощность теплового тумана (0,015);
		*/
	void SFP_LightLamp_Light()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(16);//Установить радиус на
		SetSpotLightAngle(80); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.06 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(238, 221 , 130);
		SetDiffuseColor(238, 221 , 130);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
class SFP_MonolitStone_Light extends PointLightBase //свет камня моноит
{
	void SFP_MonolitStone_Light()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 30 );
		SetBrightnessTo( 0.01 );
		FadeIn(1.06);
		SetCastShadow(false);
		SetDiffuseColor(128, 159, 255);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
		EnableHeatHaze( true);
		SetHeatHazeRadius( 7.5 );
		SetHeatHazePower( 0.015 );
	}
};
class SFP_AntiRad_Kvarc_Light extends PointLightBase //свет кварца
{
	void SFP_AntiRad_Kvarc_Light()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 4 );
		SetBrightnessTo( 0.03 );
		FadeIn(1.06);
		SetCastShadow(false);
		SetDiffuseColor(158, 0, 253);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_LAMPATKA_Light extends PointLightBase //свет от точка для звезды
{
	void SFP_LAMPATKA_Light()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 8 );
		SetBrightnessTo( 0.01 );
		FadeIn(1.06);
		SetCastShadow(false);
		SetDiffuseColor(244, 164 , 96);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
		EnableHeatHaze( true); //Включить тепловую дымку (true);
		SetHeatHazeRadius( 0.09 ); //Установить радиус теплового тумана (0,08);
		SetHeatHazePower( 0.015 ); //Установить мощность теплового тумана (0,015);
	}
};
class SFP_LightLamp_point extends PointLightBase //свет от точка для звезды
{
	void SFP_LightLamp_point()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 0.07 );
		SetBrightnessTo( 0.01 );
		SetCastShadow(false);
		SetDiffuseColor(238, 221 , 130);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_LightLampLenin_point extends PointLightBase //свет от лампы простой
{
	void SFP_LightLampLenin_point()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 10 );
		SetBrightnessTo( 0.02 );
		SetCastShadow(true);
		SetFlareVisible( true );
		SetFlickerAmplitude( 0.35 );
		SetFlickerSpeed( 0.25 );
		FadeIn(0.06);
		SetDiffuseColor(238, 221 , 130);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_LightLampOnly_point extends PointLightBase //свет от лампы простой
{
	void SFP_LightLampOnly_point()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 10 );
		SetBrightnessTo( 0.02 );
		SetCastShadow(true);
		SetDiffuseColor(238, 221 , 130);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_MolotovTrapkaLight extends PointLightBase //свет от тряпки молотова
{
	void SFP_MolotovTrapkaLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 0.5 );
		SetBrightnessTo( 0.02 );
		FadeIn(1.06); //Исчезать
		SetFadeOutTime(1.0);//Установите время затухания
		SetCastShadow(true);
		SetDiffuseColor(255, 140, 0);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_Anomaly_ElectroLight extends PointLightBase //свет от электропартикла
{
	void SFP_Anomaly_ElectroLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 10 );
		SetBrightnessTo( 0.02 );
		SetCastShadow(true);
		SetDiffuseColor(72, 118 , 255);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};

class SFP_svetoforRED_Light extends SpotLightBase//свет красного светофора
{
	void SFP_svetoforRED_Light()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(6);//Установить радиус на
		SetSpotLightAngle(60); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.04 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(255, 48, 48);
		SetDiffuseColor(255, 0, 0);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
class SFP_svetoforRED_LightZvezda extends PointLightBase //свет от точка для звезды
{
	void SFP_svetoforRED_LightZvezda()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 0.07 );
		SetBrightnessTo( 0.05 );
		SetCastShadow(false);
		SetDiffuseColor(255, 48, 48);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
class SFP_svetoforGREEN_Light extends SpotLightBase//свет зеленый светофора
{
	void SFP_svetoforGREEN_Light()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(6);//Установить радиус на
		SetSpotLightAngle(60); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.04 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(127, 255, 0);
		SetDiffuseColor(127, 255, 0);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
class SFP_svetoforGREEN_LightZvezda extends PointLightBase //свет от точка для звезды
{
	void SFP_svetoforGREEN_LightZvezda()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 0.07 );
		SetBrightnessTo( 0.06 );
		SetCastShadow(false);
		SetDiffuseColor(127, 255, 0);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
//--------------------------------------------------------------Свет От телека 
class SFP_OldTV_Light extends SpotLightBase//свет от телека
{
	void SFP_OldTV_Light()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(6);//Установить радиус на
		SetSpotLightAngle(100); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.02 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(240, 248, 255);
		SetDiffuseColor(240, 248, 255);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
//--------------------------------------------------------------Свет От гололамп
class SFP_GologenLampLight extends SpotLightBase//
{
	void SFP_GologenLampLight()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(9);//Установить радиус на
		SetSpotLightAngle(190); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.02 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(238, 221, 130);
		SetDiffuseColor(238, 221, 130);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
class SFP_GologenLampLightZvezda extends PointLightBase //свет от точка для звезды
{
	void SFP_GologenLampLightZvezda()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 3 );
		SetBrightnessTo( 0.01 );
		SetCastShadow(false);
		SetDiffuseColor(238, 221, 130);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};
//--------------------------------------------------------------Свет От гололамп Холодный
class SFP_GologenLampIceLight extends SpotLightBase//
{
	void SFP_GologenLampIceLight()
	{
		SetVisibleDuringDaylight(false); //Установить видимость при дневном свете
		SetRadiusTo(9);//Установить радиус на
		SetSpotLightAngle(190); //Установить угол точечного освещения
		SetCastShadow(true); //Увидеть отбрасываемую тень
		FadeIn(0.06); //Исчезать
		SetFadeOutTime(0.1);//Установите время затухания
		SetBrightnessTo( 0.02 );//Установить яркость на
		//SetBrightnessTo(m_DefaultBrightness);
		SetAmbientColor(224, 255, 255);
		SetDiffuseColor(224, 255, 255);
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
};
class SFP_GologenLampIceLightZvezda extends PointLightBase //свет от точка для звезды
{
	void SFP_GologenLampIceLightZvezda()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 3 );
		SetBrightnessTo( 0.01 );
		SetCastShadow(false);
		SetDiffuseColor(224, 255, 255);
		SetDancingShadowsMovementSpeed(0.005);
		SetDancingShadowsAmplitude(0.003);
	}
};