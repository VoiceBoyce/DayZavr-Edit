class dodge_rust_ZLightProfileRear extends VehicleLightProfileBrakeAndReverse
{
  void dodge_rust_ZLightProfileRear()
  {
    m_SegregatedBrakeBrightness = 1.9;
    m_SegregatedBrakeRadius = 6;
    m_SegregatedBrakeAngle = 160;
    m_SegregatedBrakeColorRGB = Vector(0.95, 0.16, 0.05);

    m_SegregatedBrightness = 0.02;
    m_SegregatedRadius = 13;
    m_SegregatedAngle = 140;
    m_SegregatedColorRGB = Vector(1.0, 1.0, 0.8);

    m_AggregatedBrightness = 1.9;
    m_AggregatedRadius = 10;
    m_AggregatedAngle = 160;
    m_AggregatedColorRGB = Vector(0.9, 0.4, 0.5);

    m_FadeInTime = 0.1;
    m_FadeOutTime = 0.1;
  }
}
