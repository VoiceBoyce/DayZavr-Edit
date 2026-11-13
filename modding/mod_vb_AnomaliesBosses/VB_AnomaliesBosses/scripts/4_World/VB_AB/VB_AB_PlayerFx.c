// Действие способностей боссов на игрока (ТЗ 4.3), клиентская часть: то, что видно и слышно.
// Урон и состояния считает серверный мод (VB_AnomaliesBosses_Server, VB_AB_PlayerFxServer.c).
// API сверено по P:\scripts (DayZ 1.30), ссылки на файлы — в комментариях.

// Клиентская часть: таймеры размытия и снега — методы экземпляра (для CallLater)
class VB_AB_PlayerFxClient
{
	protected static ref VB_AB_PlayerFxClient s_Instance;
	protected float m_BlurUntil;
	protected float m_BlurStart;
	protected float m_BlurPower;

	static VB_AB_PlayerFxClient Get()
	{
		if (!s_Instance)
			s_Instance = new VB_AB_PlayerFxClient();
		return s_Instance;
	}

	void Apply(PlayerBase me, int kind, float sec)
	{
		DayZPlayerCamera cam = me.GetCurrentCamera();
		switch (kind)
		{
			case VB_AB_Fx.P_BLIND:
				// флешбанг (FlashGrenade.OnExplosionEffects): ослепление и звон; длительность ~8 с x коэффициент
				me.OnPlayerReceiveFlashbangHitStart(true);
				if (me.GetFlashbangEffect())
					me.GetFlashbangEffect().SetupFlashbangValues(Math.Clamp(sec / 8.0, 0.3, 1.5), 1.0, 0.6);
				break;
			case VB_AB_Fx.P_SCREAM:
				// только звон в ушах, без вспышки
				me.OnPlayerReceiveFlashbangHitStart(false);
				if (me.GetFlashbangEffect())
					me.GetFlashbangEffect().SetupFlashbangValues(Math.Clamp(sec / 8.0, 0.3, 1.5), 0, 0.8);
				if (cam)
					cam.SpawnCameraShake(1.5);
				break;
			case VB_AB_Fx.P_PSI:
				if (cam)
					cam.SpawnCameraShake(1.0);
				StartBlur(sec, 0.6);
				VB_AB_Fx.Play("", "SpookyArea_Whispering", me.GetPosition());
				break;
			case VB_AB_Fx.P_COLD:
				me.VB_AB_NoSprintFor(sec);		// замедление и у самого игрока, иначе клиент предскажет бег
				PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Start(new Param3<float, float, float>(0.3, 0.7, 0.2));
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopShockPpe, 1500, false);
				break;
			case VB_AB_Fx.P_BLIZZARD:
				StartBlur(sec, 0.25);
				SnowAround(me, sec);
				break;
			case VB_AB_Fx.P_SHAKE:
				if (cam)
					cam.SpawnCameraShake(2.5);
				break;
		}
	}

	void StopShockPpe()
	{
		PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Stop();
	}

	// Размытие волной (как FeverBlurState: SetFeverIntensity каждый кадр)
	void StartBlur(float sec, float power)
	{
		float now = GetGame().GetTickTime();
		m_BlurStart = now;
		m_BlurUntil = now + sec;
		m_BlurPower = power;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BlurTick);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(BlurTick, 40, true);
	}

	void BlurTick()
	{
		PPERequester_FeverEffects req = PPERequester_FeverEffects.Cast(PPERequesterBank.GetRequester(PPERequester_FeverEffects));
		float now = GetGame().GetTickTime();
		if (!req || now >= m_BlurUntil)
		{
			if (req)
				req.SetFeverIntensity(0);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BlurTick);
			return;
		}
		float t = (now - m_BlurStart) / Math.Max(0.1, m_BlurUntil - m_BlurStart);
		req.SetFeverIntensity(Math.Sin(t * Math.PI) * m_BlurPower * (0.75 + 0.25 * Math.Sin(now * 6)));
	}

	// Метель: снег вокруг игрока, пока она идёт (частица Снега, раз в 4 с)
	void SnowAround(PlayerBase me, float sec)
	{
		if (!me || sec <= 0)
			return;
		VB_AB_Fx.Play("vb_ab_snow_idle", "", me.GetPosition());
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SnowAround, 4000, false, me, sec - 4);
	}
};

// Замедление: множителя скорости в ванили нет, поэтому запрет бега на время (ставит сервер и сам игрок)
modded class PlayerBase
{
	protected float m_VB_AB_NoSprintUntil;

	void VB_AB_NoSprintFor(float sec)
	{
		m_VB_AB_NoSprintUntil = GetGame().GetTickTime() + sec;
	}

	override bool CanSprint()
	{
		if (m_VB_AB_NoSprintUntil > 0 && GetGame().GetTickTime() < m_VB_AB_NoSprintUntil)
			return false;
		return super.CanSprint();
	}
};
