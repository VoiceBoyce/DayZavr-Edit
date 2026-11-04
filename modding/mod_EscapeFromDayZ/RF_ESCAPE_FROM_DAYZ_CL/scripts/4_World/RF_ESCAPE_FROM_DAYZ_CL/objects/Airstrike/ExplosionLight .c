class RF_ESCAPE_FROM_DAYZ_ExplosionLight : PointLightBase {
	protected float m_DefaultBrightness = 10;
	protected float m_DefaultRadius = 100;
	
	void RF_ESCAPE_FROM_DAYZ_ExplosionLight() {
        Print("init RF_ESCAPE_FROM_DAYZ_ExplosionLight");
		SetVisibleDuringDaylight(false);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 1.0, 0.3);
		SetDiffuseColor(1.0, 1.0, 0.3);
		SetLifetime(0.15);
		SetDisableShadowsWithinRadius(-1);
		SetCastShadow( false );
	}

    void ~RF_ESCAPE_FROM_DAYZ_ExplosionLight() {
        Print("~RF_ESCAPE_FROM_DAYZ_ExplosionLight");
    }
}