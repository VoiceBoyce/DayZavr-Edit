// Плавный экранный эффект для бустера: ChromAber (разъезд цветов).
// ВАЖНО: сделано максимально “старо-совместимо” (без ternary ?:, без Param2<>),
// чтобы Enforce Script не ругался на ':' и прочие ошибки парсинга.

class PPERequester_BusterChromAbb extends PPERequester_GameplayBase
{
	static const int L_BUSTER = 200;

	// ===== ЧТО КРУТИТЬ =====
	static const float FADE_IN_TIME  = 0.60; // секунды появления
	static const float FADE_OUT_TIME = 0.80; // секунды исчезновения
	static const float TICK          = 0.05; // шаг таймера (меньше = плавнее)

	protected float m_TargetPower;
	protected float m_CurrentPower;

	protected float m_FadeFrom;
	protected float m_FadeTo;
	protected float m_FadeTime;
	protected float m_FadeElapsed;

	protected bool  m_StopAfterFade;

	protected ref Timer m_FadeTimer;

	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);

		// Определяем целевую силу эффекта
		m_TargetPower = BusterConstants.PPE_CHROMABER_POWER;

		Param1<float> p1 = Param1<float>.Cast(par);
		if (p1)
			m_TargetPower = p1.param1;

		m_TargetPower = Math.Clamp(m_TargetPower, 0.0, 0.05);

		m_StopAfterFade = false;

		// Стартуем с нуля и плавно доходим до целевого значения
		m_CurrentPower = 0.0;
		Buster_ApplyPower(m_CurrentPower);
		Buster_BeginFade(0.0, m_TargetPower, FADE_IN_TIME);
	}

	override protected void OnStop(Param par = null)
	{
		// НЕ останавливаем сразу — сначала плавно гасим и только потом super.OnStop()
		m_StopAfterFade = true;
		Buster_BeginFade(m_CurrentPower, 0.0, FADE_OUT_TIME);
	}

	protected void Buster_BeginFade(float from, float to, float timeSec)
	{
		m_FadeFrom = Math.Clamp(from, 0.0, 0.05);
		m_FadeTo   = Math.Clamp(to,   0.0, 0.05);

		m_FadeTime = timeSec;
		if (m_FadeTime < 0.0)
			m_FadeTime = 0.0;

		m_FadeElapsed = 0.0;

		if (!m_FadeTimer)
			m_FadeTimer = new Timer(CALL_CATEGORY_SYSTEM);
		else
			m_FadeTimer.Stop();

		// Если время 0 — применяем мгновенно
		if (m_FadeTime <= 0.0)
		{
			m_CurrentPower = m_FadeTo;
			Buster_ApplyPower(m_CurrentPower);

			if (m_StopAfterFade && m_CurrentPower <= 0.0)
			{
				m_StopAfterFade = false;
				super.OnStop();
			}
			return;
		}

		// Запускаем тики
		m_FadeTimer.Run(TICK, this, "Buster_FadeTick", NULL, true);
	}

	void Buster_FadeTick()
	{
		m_FadeElapsed = m_FadeElapsed + TICK;

		float t = m_FadeElapsed / m_FadeTime;
		if (t >= 1.0)
			t = 1.0;

		m_CurrentPower = m_FadeFrom + (m_FadeTo - m_FadeFrom) * t;
		m_CurrentPower = Math.Clamp(m_CurrentPower, 0.0, 0.05);

		Buster_ApplyPower(m_CurrentPower);

		if (t >= 1.0)
		{
			if (m_FadeTimer)
				m_FadeTimer.Stop();

			// Если это был fade-out — реально останавливаем requester
			if (m_StopAfterFade && m_CurrentPower <= 0.0)
			{
				m_StopAfterFade = false;
				super.OnStop();
			}
		}
	}

	protected void Buster_ApplyPower(float power)
	{
		power = Math.Clamp(power, 0.0, 0.05);

		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERX, false, power, L_BUSTER, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.ChromAber, PPEChromAber.PARAM_POWERY, false, power, L_BUSTER, PPOperators.SET);
	}
}
