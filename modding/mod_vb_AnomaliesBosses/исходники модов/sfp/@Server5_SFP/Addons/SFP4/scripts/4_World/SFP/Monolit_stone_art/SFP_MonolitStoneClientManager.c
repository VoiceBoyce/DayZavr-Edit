// ============================================================================
// МОНОЛИТ — клиентский менеджер дистанции
//
// Задача:
//  - хранит список всех SFP_Artefact_Monolit, которые существуют на клиенте
//  - каждый тик находит ближайшую «силу воздействия» и применяет:
//      1) PPE (зерно / ЧБ / искажение)
//      2) звук (SoundSet) ЛОКАЛЬНО на игрока ("в наушники"), а не от объекта
//
// Где крутить параметры:
//   scripts\4_World\SFP\Monolit_stone_art\SFP_MonolitStone_Tuning.c
// ============================================================================

#ifndef SERVER

class SFP_MonolitStoneClientManager
{
	protected static ref SFP_MonolitStoneClientManager s_Instance;

	protected ref array<SFP_Artefact_Monolit> m_Stones;
	protected EffectSound m_SoundLoop;
	protected bool m_Tuned;

	static SFP_MonolitStoneClientManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new SFP_MonolitStoneClientManager();
		return s_Instance;
	}

	void SFP_MonolitStoneClientManager()
	{
		m_Stones = new array<SFP_Artefact_Monolit>;
		m_Tuned = false;
		// Стартуем цикл обновления (первый запуск с запасом, дальше интервал выставится из tuning)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Update, 100, true);
	}

	void RegisterStone(SFP_Artefact_Monolit stone)
	{
		if (!stone)
			return;
		if (m_Stones.Find(stone) == -1)
			m_Stones.Insert(stone);
	}

	void UnregisterStone(SFP_Artefact_Monolit stone)
	{
		m_Stones.RemoveItem(stone);
	}

	protected void EnsureTuning()
	{
		if (m_Tuned)
			return;
		SFP_MonolitStone_Tuning.Apply();

		// Применяем JSON-настройки (звук + радиус экранных эффектов) ТОЛЬКО для основного камня.
		// Type2 не регистрируется в этом менеджере, поэтому на него это не влияет.
		SFP_MonolitStoneSFPSettings st = SFP_MonolitStoneSFPSettings.Get();
		if (st)
		{
			SFP_MonolitStone_Config.SOUND_RADIUS  = st.m_SoundRadius_M;
			SFP_MonolitStone_Config.EFFECT_RADIUS = st.m_ScreenEffectsRadius_M;
		}

		m_Tuned = true;

		// Перезапускаем таймер с интервалом из конфига (UPDATE_INTERVAL_MS)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Update);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Update, SFP_MonolitStone_Config.UPDATE_INTERVAL_MS, true);
	}

	protected void Update()
	{
		EnsureTuning();

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		float bestGrain = 0.0;
		float bestSat = 0.0;
		float bestDistort = 0.0;
		bool inSound = false;

		for (int i = m_Stones.Count() - 1; i >= 0; i--)
		{
			SFP_Artefact_Monolit stone = m_Stones[i];
			if (!stone)
			{
				m_Stones.Remove(i);
				continue;
			}

			float dist = vector.Distance(player.GetPosition(), stone.GetEffectOriginWS());

			if (dist <= SFP_MonolitStone_Config.SOUND_RADIUS)
				inSound = true;

			if (dist > SFP_MonolitStone_Config.EFFECT_RADIUS)
				continue;

			float g = SFP_MonolitStone_Config.StrengthFromDistance(dist, SFP_MonolitStone_Config.GRAIN_START_DIST, SFP_MonolitStone_Config.GRAIN_FULL_DIST, SFP_MonolitStone_Config.GRAIN_CURVE);
			float s = SFP_MonolitStone_Config.StrengthFromDistance(dist, SFP_MonolitStone_Config.SATURATION_START_DIST, SFP_MonolitStone_Config.SATURATION_FULL_DIST, SFP_MonolitStone_Config.SATURATION_CURVE);
			float d = SFP_MonolitStone_Config.StrengthFromDistance(dist, SFP_MonolitStone_Config.DISTORT_START_DIST, SFP_MonolitStone_Config.DISTORT_FULL_DIST, SFP_MonolitStone_Config.DISTORT_CURVE);

			bestGrain = Math.Max(bestGrain, g);
			bestSat = Math.Max(bestSat, s);
			bestDistort = Math.Max(bestDistort, d);
		}

		HandleSound(player, inSound);
		HandlePPE(bestGrain, bestSat, bestDistort);
	}

	protected void HandleSound(PlayerBase player, bool shouldPlay)
	{
		if (shouldPlay)
		{
			if (!m_SoundLoop)
			{
				// Играем звук ЛОКАЛЬНО на игрока (эффект «в наушники»)
				player.PlaySoundSetLoop(m_SoundLoop, SFP_MonolitStone_Config.SOUNDSET_MONOLIT, 0, 0);
			}
		}
		else
		{
			if (m_SoundLoop)
			{
				player.StopSoundSet(m_SoundLoop);
				m_SoundLoop = null;
			}
		}
	}

	protected void HandlePPE(float grain, float sat, float distort)
	{
		// Получаем наш requester из банка (он зарегистрирован в 3_Game)
		PPERequester_SFPMonolitStone req = PPERequester_SFPMonolitStone.Cast(PPERequesterBank.GetRequester(PPERequester_SFPMonolitStone));
		if (!req)
			return;

		float any = Math.Max(grain, Math.Max(sat, distort));
		if (any > 0.001)
		{
			if (!req.IsRequesterRunning())
				req.Start();
			req.SetIntensity(grain, sat, distort);
		}
		else
		{
			if (req.IsRequesterRunning())
				req.Stop();
		}
	}
}

#endif
