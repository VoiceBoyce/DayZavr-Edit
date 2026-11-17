class InterceptorLightProfileFront : VehicleLightProfileFront
{
	void InterceptorLightProfileFront()
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
