//electra
class Anomaly_Electra_Idle_Light extends PointLightBase
{		
	void Anomaly_Electra_Idle_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 6 );
		SetBrightnessTo( 0.3 );
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
}

class Anomaly_Electra_Active_Light extends PointLightBase
{
	void Anomaly_Electra_Active_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(20);
		SetBrightnessTo(100);
		SetFlareVisible(false);
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
}

//studen
class Anomaly_Studen_Idle_Light extends PointLightBase
{		
	void Anomaly_Studen_Idle_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 6 );
		SetBrightnessTo( 0.3 );
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.0, 0.8, 0.1);
		SetAmbientColor(0.0, 0.8, 0.1);
		SetFlareVisible(false);
	}
}

class Anomaly_Studen_Active_Light extends PointLightBase
{
	void Anomaly_Studen_Active_Light()
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
}

//zharka
class Anomaly_Zharka_Active_Light extends PointLightBase
{
	void Anomaly_Zharka_Active_Light()
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
}

//BlackHole
class Anomaly_BlackHole_Idle_Light extends PointLightBase
{		
	void Anomaly_BlackHole_Idle_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 6 );
		SetBrightnessTo( 0.3 );
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
        SetHeatHazePower(0.007);
	}
}

class Anomaly_BlackHole_Active_Light extends PointLightBase
{
	void Anomaly_BlackHole_Active_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(50);
		SetBrightnessTo(40);
		SetFlareVisible(false);
		SetAmbientColor(0.9, 0.5, 0.3);
		SetDiffuseColor(0.9, 0.5, 0.3);
		SetFadeOutTime(0.0);
		SetLifetime(5);
		SetDisableShadowsWithinRadius(-1);
		SetFlareVisible(true);					/* - Установить видимую вспышку */
		SetFlickerAmplitude(1.0);				/* - Установить амплитуду мерцания */
		SetFlickerSpeed(10.0);					/* - Установить скорость мерцания */
		SetDancingShadowsMovementSpeed(10.0);	/* - Установить скорость движения танцующих теней */
		SetDancingShadowsAmplitude(1.0);		/* - Установить амплитуду танцующих теней */
		EnableHeatHaze(true);
        SetHeatHazeRadius(5.0);
        SetHeatHazePower(0.013);
	}
}

 class Anomaly_Holera_Idle_Light extends PointLightBase
{		
	void Anomaly_Holera_Idle_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 6 );
		SetBrightnessTo( 0.3 );
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.0, 0.8, 0.1);
		SetAmbientColor(0.0, 0.8, 0.1);
		SetFlareVisible(false);
	}
} 

class Anomaly_Holera_Active_Light extends PointLightBase
{
	void Anomaly_Holera_Active_Light()
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
}