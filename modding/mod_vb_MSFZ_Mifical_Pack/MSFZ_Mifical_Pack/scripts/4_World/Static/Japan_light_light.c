class Japan_light_light extends PointLightBase //свет от точка для звезды
{
	void Japan_light_light()
	{
		SetVisibleDuringDaylight(true); //Включить свечение днем (true);
		SetRadiusTo( 0.6 ); //радиус свечения;
		SetBrightnessTo( 0.03 );
		FadeIn(1.06); 
		SetCastShadow(false); //тени;
		SetDiffuseColor(255, 215, 0); //цвет свечения RGB;
		SetDancingShadowsMovementSpeed(0.005); //танцующей тени скорость;
		SetDancingShadowsAmplitude(0.003); //танцующей тени амплитуда;
		EnableHeatHaze( true); //Включить тепловую дымку (true);
		SetHeatHazeRadius( 0.2 ); //Установить радиус теплового тумана (0,08);
		SetHeatHazePower( 0.06 ); //Установить мощность теплового тумана (0,015);
	}
};






