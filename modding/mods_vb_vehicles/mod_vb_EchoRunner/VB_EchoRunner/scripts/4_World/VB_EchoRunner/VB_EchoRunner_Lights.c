// Свет Echo Runner — профили Interceptor ALEX.

class VB_EchoRunnerLightProfileFront : VehicleLightProfileFront
{
	void VB_EchoRunnerLightProfileFront()
	{
		m_SegregatedBrightness = 20;
		m_SegregatedRadius = 70;
		m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.8);

		m_AggregatedBrightness = 20;
		m_AggregatedRadius = 70;
		m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.8);

		m_FadeInTime = 0.3;
		m_FadeOutTime = 0.25;
	}
}

class VB_EchoRunnerLightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void VB_EchoRunnerLightProfileRear()
	{
		m_SegregatedBrakeBrightness = 1;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 270;
		m_SegregatedBrakeColorRGB = Vector(1, 0.6, 0.05);

		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 13;
		m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 1.0, 0.8);

		m_AggregatedBrightness = 2.5;
		m_AggregatedRadius = 15;
		m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.6);

		m_FadeInTime = 0.1;
		m_FadeOutTime = 0.1;
		m_DaylightVisible = false;
		m_CastShadow = false;
		m_FlaresVisible = false;
	}
}
