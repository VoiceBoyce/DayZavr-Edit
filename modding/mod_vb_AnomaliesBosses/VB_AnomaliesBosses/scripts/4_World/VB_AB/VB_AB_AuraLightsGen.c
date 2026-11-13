// СГЕНЕРИРОВАНО tools/ab_aura.py — руками не править
// Свет аур боссов (VB_AB_Aura.c). Только ночью (днём точечный свет не нужен), без теней.
// Цвет — строгими каналами (E45). Радиус и яркость — для масштаба 1, VB_SetScale умножает радиус.

class VB_AB_Light_AuraBase extends PointLightBase
{
	protected float m_VB_BaseRadius = 8;
	protected float m_VB_BaseBright = 1;

	void VB_AB_Light_AuraBase()
	{
		SetVisibleDuringDaylight(false);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetFadeOutTime(2.5);
	}

	protected void VB_InitAura(float r, float g, float b, float radius, float bright)
	{
		m_VB_BaseRadius = radius;
		m_VB_BaseBright = bright;
		SetDiffuseColor(r, g, b);
		SetAmbientColor(r, g, b);
		SetRadiusTo(radius);
		SetBrightnessTo(bright);
	}

	// масштаб босса (Энт 1,6): шире круг света, яркость та же
	void VB_SetScale(float scale)
	{
		SetRadiusTo(m_VB_BaseRadius * Math.Max(1, scale));
	}

	// вспышка при распаде тела: ярче, гаснет за 2.5 с и удаляется сама
	void VB_Flash(float scale)
	{
		SetRadiusTo(m_VB_BaseRadius * Math.Max(1, scale));
		SetBrightnessTo(m_VB_BaseBright * 2);
		FadeOut(2.5);
	}
};

class VB_AB_Light_AuraFire extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraFire()
	{
		VB_InitAura(1, 0.32, 0, 8, 1.3);
		SetFlickerAmplitude(0.35);
		SetFlickerSpeed(3);
		EnableHeatHaze(true);
		SetHeatHazeRadius(1.2);
		SetHeatHazePower(0.006);
	}
};

class VB_AB_Light_AuraStorm extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraStorm()
	{
		VB_InitAura(0, 0.45, 1, 8, 1.3);
		SetFlickerAmplitude(0.6);
		SetFlickerSpeed(9);
	}
};

class VB_AB_Light_AuraIce extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraIce()
	{
		VB_InitAura(0.4, 0.75, 1, 8, 1);
	}
};

class VB_AB_Light_AuraGravity extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraGravity()
	{
		VB_InitAura(0.55, 0, 1, 8, 1.2);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(1.2);
	}
};

class VB_AB_Light_AuraAcid extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraAcid()
	{
		VB_InitAura(0.25, 1, 0, 8, 1);
		SetFlickerAmplitude(0.2);
		SetFlickerSpeed(2);
	}
};

class VB_AB_Light_AuraPsi extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraPsi()
	{
		VB_InitAura(0.8, 0.75, 1, 8, 0.9);
		SetFlickerAmplitude(0.25);
		SetFlickerSpeed(0.8);
	}
};

class VB_AB_Light_AuraRad extends VB_AB_Light_AuraBase
{
	void VB_AB_Light_AuraRad()
	{
		VB_InitAura(0.65, 1, 0, 8, 1);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(2.5);
	}
};
