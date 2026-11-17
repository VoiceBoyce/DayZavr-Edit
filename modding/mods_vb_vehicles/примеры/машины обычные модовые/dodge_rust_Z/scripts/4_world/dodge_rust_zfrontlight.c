class dodge_rust_ZLightProfileFront extends VehicleLightProfileFront
{
  void dodge_rust_ZLightProfileFront()
  {
    m_SegregatedBrightness = 1;
    m_SegregatedRadius = 45;
    m_SegregatedAngle = 145;
    m_SegregatedColorRGB = Vector(20.0, 57.0, 127.0);

    m_AggregatedBrightness = 1;
    m_AggregatedRadius = 45;
    m_AggregatedAngle = 145;
    m_AggregatedColorRGB = Vector(20.0, 57.0, 127.0);

    m_FadeInTime = 0.3;
    m_FadeOutTime = 0.25;
  }
}
