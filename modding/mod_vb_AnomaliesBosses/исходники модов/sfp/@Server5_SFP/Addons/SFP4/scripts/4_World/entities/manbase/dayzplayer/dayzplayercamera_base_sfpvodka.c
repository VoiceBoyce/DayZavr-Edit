// SFP Vodka effects — «GTA4» камера для 1/3 лица (когда НЕ ADS)
//
// Ванильная особенность DayZ:
//  - AimingModel.SetCamShakeValues(...) в основном используется камерой прицела (ADS/ironsights).
//  - обычные камеры 1/3 лица качаются через DayZPlayerCameraBase.ProcessCameraShake(...).
//
// Поэтому, чтобы «шатало» и в 1/3 лице без прицеливания, мы добавляем здесь:
//  - плавное «пьяное» качание («лодочка»)
//  - редкие микросрывы/спотыкания на высокой степени опьянения
//  - дополнительно — наклон горизонта (ROLL), чтобы было более похоже на GTA4.

modded class DayZPlayerCameraBase
{
	protected float m_SFP_VodkaIntensity = 0.0;
	protected float m_SFP_VodkaTime = 0.0;
	protected bool  m_SFP_VodkaADS = false;
	
	// Доп. «GTA4» наклон горизонта (roll)
	protected float m_SFP_VodkaRoll = 0.0;
	protected float m_SFP_VodkaLRLast = 0.0;

	// «GTA4» микросрывы (как будто персонаж слегка спотыкается)
	protected float m_SFP_VodkaJoltCooldown = 0.0;
	protected float m_SFP_VodkaJoltRemain = 0.0;
	protected float m_SFP_VodkaJoltLen = 0.0;
	protected float m_SFP_VodkaJoltAmp = 0.0;
	protected float m_SFP_VodkaJoltFreq = 0.0;
	protected float m_SFP_VodkaJoltPhase = 0.0;

	// Передаём сюда данные из PlayerBase (каждый тик, только на клиенте локального игрока)
	void SFP_SetVodkaSway(float intensity, float time, bool ads)
	{
		m_SFP_VodkaIntensity = Math.Max(0.0, intensity);
		m_SFP_VodkaTime = Math.Max(0.0, time);
		m_SFP_VodkaADS = ads;
	}

	// Полный сброс (когда эффект закончился)
	void SFP_ClearVodkaSway()
	{
		m_SFP_VodkaIntensity = 0.0;
		m_SFP_VodkaTime = 0.0;
		m_SFP_VodkaADS = false;
		m_SFP_VodkaRoll = 0.0;
		m_SFP_VodkaLRLast = 0.0;
		m_SFP_VodkaJoltCooldown = 0.0;
		m_SFP_VodkaJoltRemain = 0.0;
		m_SFP_VodkaJoltLen = 0.0;
		m_SFP_VodkaJoltAmp = 0.0;
		m_SFP_VodkaJoltFreq = 0.0;
		m_SFP_VodkaJoltPhase = 0.0;
	}

	// Добавляем «пьяный наклон» (roll) поверх ванильного.
	// Работает и в 1p и в 3p, потому что камеры собирают трансформацию через GetCurrentOrientation().
	override float GetCurrentRoll()
	{
		float baseRoll = m_CurrentCameraRoll;
		
		// В ADS наклон не трогаем (ADS качается через AimingModel)
		if (m_SFP_VodkaADS)
			return baseRoll;
		
		if (m_SFP_VodkaIntensity <= 0.01)
			return baseRoll;

		float n = Math.Clamp(m_SFP_VodkaIntensity / SFP_VodkaConstants.INTENSITY_MAX, 0.0, 1.0);
		float t = m_SFP_VodkaTime;

		// Скорость и амплитуда наклона
		float speed = Math.Lerp(0.42, 1.25, n);
		float amp = Math.Lerp(1.10, 15.00, n);

		// «лодочка» + медленный дрейф
		float roll = (Math.Sin(t * 0.44 * speed) * 0.95 + Math.Sin(t * 0.14 * speed + 1.35) * 0.60 + Math.Sin(t * 0.08 * speed - 0.70) * 0.30) * amp;
		roll += Math.Sin(t * 0.030 * speed) * (amp * 0.55);
		
		// Небольшая «привязка» к боковому качанию (так ощущается более по-gta4)
		roll += m_SFP_VodkaLRLast * Math.Lerp(0.06, 0.38, n);

		// Во время микросрыва добавляем краткий наклон (как «спотыкание»)
		if (m_SFP_VodkaJoltRemain > 0.0 && m_SFP_VodkaJoltLen > 0.0)
		{
			float k = Math.Clamp(m_SFP_VodkaJoltRemain / m_SFP_VodkaJoltLen, 0.0, 1.0);
			float env = k * k; // быстро затухает
			float jr = Math.Sin(m_SFP_VodkaJoltPhase * (m_SFP_VodkaJoltFreq * 0.88) + 0.90) * m_SFP_VodkaJoltAmp * env;
			roll += jr * Math.Lerp(0.85, 2.40, n);
		}

		return baseRoll + roll;
	}

	override void ProcessCameraShake(float delta, out float leftRight, out float upDown)
	{
		super.ProcessCameraShake(delta, leftRight, upDown);

		// Применяем только для 1/3 лица (НЕ ADS). В ADS используется шейк из PlayerBase через AimingModel.
		if (m_SFP_VodkaADS)
			return;

		if (m_SFP_VodkaIntensity <= 0.01)
			return;

		float n = Math.Clamp(m_SFP_VodkaIntensity / SFP_VodkaConstants.INTENSITY_MAX, 0.0, 1.0);

		// Плавное сильное качание («пьяная лодка»)
		float t = m_SFP_VodkaTime;
		float speed = Math.Lerp(0.45, 1.35, n);//скорость
		float amp = Math.Lerp(0.60, 9.80, n);//сила

		float lr = (Math.Sin(t * 0.62 * speed) * 0.90 + Math.Sin(t * 0.17 * speed) * 0.55) * amp;
		float ud = (Math.Cos(t * 0.48 * speed) * 0.80 + Math.Sin(t * 0.21 * speed) * 0.45) * amp * 0.85;

		leftRight += lr;
		upDown += ud;

		// Сохраним боковое значение — оно используется в GetCurrentRoll() для «привязки» наклона
		m_SFP_VodkaLRLast = lr;

		// Просто кеш (можно использовать для отладки/тюнинга)
		m_SFP_VodkaRoll = 0.0;

		// Редкие «спотыкания» (микросрывы) когда сильно пьян
		if (n > 0.55)
		{
			m_SFP_VodkaJoltCooldown -= delta;
			if (m_SFP_VodkaJoltCooldown <= 0.0 && m_SFP_VodkaJoltRemain <= 0.0)
			{
				m_SFP_VodkaJoltLen = 0.18 + (0.12 * n);
				m_SFP_VodkaJoltRemain = m_SFP_VodkaJoltLen;
				m_SFP_VodkaJoltAmp = Math.Lerp(0.35, 2.20, n);
				m_SFP_VodkaJoltFreq = Math.Lerp(10.0, 18.0, n);
				m_SFP_VodkaJoltPhase = 0.0;
				// Чем сильнее пьян — тем чаще срывы
				m_SFP_VodkaJoltCooldown = Math.RandomFloat(1.6, 3.8) / Math.Lerp(0.9, 1.8, n);
			}

			if (m_SFP_VodkaJoltRemain > 0.0)
			{
				m_SFP_VodkaJoltRemain = Math.Max(0.0, m_SFP_VodkaJoltRemain - delta);
				m_SFP_VodkaJoltPhase += delta;

				float k = 0.0;
				if (m_SFP_VodkaJoltLen > 0.0)
					k = Math.Clamp(m_SFP_VodkaJoltRemain / m_SFP_VodkaJoltLen, 0.0, 1.0);
				
				// Быстро затухает, чтобы не было «жёсткой тряски» долго
				float env = k * k;
				float j = Math.Sin(m_SFP_VodkaJoltPhase * m_SFP_VodkaJoltFreq) * m_SFP_VodkaJoltAmp * env;

				leftRight += j;
				upDown += j * 0.65;
				m_SFP_VodkaLRLast += j;
			}
		}
		else
		{
			// Если не сильно пьян — полностью убираем микросрывы
			m_SFP_VodkaJoltCooldown = 0.0;
			m_SFP_VodkaJoltRemain = 0.0;
		}
	}
}
