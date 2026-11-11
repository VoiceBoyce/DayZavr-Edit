// Общий «пульс» всех объектов Изнанки на клиенте: один таймер на все объекты, а не у каждого свой.
// Днём Изнанка спит: сухие серо-бурые текстуры, без свечения, света и звука. В сумерках тлеет, ночью горит.
// Красный свет горит только у ближайших сердец (не больше MAX_LIGHTS одновременно).
// Корни рядом с сердцем бьются вместе с ним с задержкой по расстоянию: удар бежит от сердца наружу.

class VB_RiftLight extends PointLightBase
{
	void VB_RiftLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(VB_RiftPulse.LIGHT_RADIUS);
		SetBrightnessTo(VB_RiftPulse.LIGHT_BASE);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.12, 0.2);
		SetDiffuseColor(1.0, 0.12, 0.2);
	}
}

class VB_RiftPulse
{
	static const float BEAT_PERIOD = 6.0;	// секунд на цикл: «тук-тук» и долгая пауза
	static const float BEAT_FIRST = 0.15;	// первый удар в цикле, с
	static const float BEAT_SECOND = 0.6;	// второй, слабее, с
	static const int TICK_MS = 40;
	static const float ACTIVE_DIST = 250.0;	// дальше от игрока объект не пульсирует
	static const float LIGHT_DIST = 150.0;	// свет только у сердец ближе этого
	static const int MAX_LIGHTS = 3;		// не больше стольких красных источников света сразу
	static const float LINK_DIST = 30.0;	// корни ближе этого к сердцу бьются вместе с ним
	static const float WAVE_SPEED = 8.0;	// м/с: так быстро удар бежит от сердца по корням
	static const float LIGHT_RADIUS = 9.0;
	static const float LIGHT_BASE = 0.6;
	static const float LIGHT_BEAT = 2.6;
	static const int MAX_SPORES = 4;		// споры (частицы) не больше чем у стольких сердец сразу
	static const float SPORE_DIST = 80.0;	// и только ближе этого

	// ночь: 0 = день (спит), 1 = ночь; между — сумерки
	static const float NIGHT_DAY_BELOW = 0.25;	// меньше — дневные текстуры
	static const float NIGHT_FULL_ABOVE = 0.6;	// меньше — только тлеет (не выше уровня 1)
	static const float NIGHT_LIGHT_ABOVE = 0.4;	// свет и звук только темнее этого

	static const string SOUND_SET = "VB_RiftHeartbeat_SoundSet";
	static const float SOUND_DIST = 50.0;	// дальше сердцебиение не запускаем
	static const float SOUND_LEAD = 0.03;	// в файле первый удар через 0.03 с от начала

	protected static ref VB_RiftPulse s_VB_Instance;

	protected ref array<VB_RiftBase> m_VB_Objects;
	protected int m_VB_Ticks;
	protected int m_VB_LastTickMs = -100000;

	void VB_RiftPulse()
	{
		m_VB_Objects = new array<VB_RiftBase>;
	}

	static void VB_Register(VB_RiftBase obj)
	{
		if (!s_VB_Instance)
			s_VB_Instance = new VB_RiftPulse();

		// таймер (пере)запускаем, если он не тикал больше секунды: первый объект или новая миссия после переподключения
		int nowMs = g_Game.GetTime();
		if (nowMs - s_VB_Instance.m_VB_LastTickMs > 1000 || nowMs < s_VB_Instance.m_VB_LastTickMs)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_VB_Instance.VB_Tick);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_VB_Instance.VB_Tick, TICK_MS, true);
			s_VB_Instance.m_VB_LastTickMs = nowMs;
		}

		if (s_VB_Instance.m_VB_Objects.Find(obj) < 0)
			s_VB_Instance.m_VB_Objects.Insert(obj);

		s_VB_Instance.m_VB_Ticks = 0;	// на ближайшем тике пересчитать связи с сердцами и свет
	}

	static void VB_Unregister(VB_RiftBase obj)
	{
		if (!s_VB_Instance)
			return;

		s_VB_Instance.m_VB_Objects.RemoveItem(obj);
		if (s_VB_Instance.m_VB_Objects.Count() == 0)
		{
			if (g_Game)
				g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_VB_Instance.VB_Tick);
			s_VB_Instance = null;
		}
	}

	// один удар: быстрый подъём, медленный спад, 0..1
	static float VB_Beat(float t, float at)
	{
		float x;
		if (t < at)
			x = (at - t) / 0.08;
		else
			x = (t - at) / 0.4;

		if (x >= 1.0)
			return 0.0;

		float y = 1.0 - x * x;
		return y * y;
	}

	// 0 = день, 1 = ночь; по высоте солнца, переход примерно от +7° до -1°
	static float VB_NightFactor()
	{
		World wrld = g_Game.GetWorld();
		if (!wrld)
			return 1.0;

		if (wrld.GetSunOrMoon() < 0.5)
			return 1.0;	// светит луна

		vector sunDir = wrld.GetSunOrMoonDirection();
		float elev = -sunDir[1];
		return 1.0 - Math.Clamp((elev + 0.02) / 0.14, 0.0, 1.0);
	}

	void VB_Tick()
	{
		m_VB_LastTickMs = g_Game.GetTime();
		Man player = g_Game.GetPlayer();
		if (!player)
			return;

		for (int i = m_VB_Objects.Count() - 1; i >= 0; i--)
		{
			if (!m_VB_Objects[i])
				m_VB_Objects.Remove(i);
		}

		vector ppos = player.GetPosition();
		float night = VB_NightFactor();
		if (m_VB_Ticks % 50 == 0)
			VB_Relink();
		if (m_VB_Ticks % 10 == 0)
			VB_UpdateLights(ppos, night);
		m_VB_Ticks++;

		float now = g_Game.GetTime() * 0.001;
		float activeSq = ACTIVE_DIST * ACTIVE_DIST;
		float soundSq = SOUND_DIST * SOUND_DIST;
		float soundAt = BEAT_FIRST - SOUND_LEAD;
		float dSq;
		float t;
		float k;
		int level;
		foreach (VB_RiftBase obj : m_VB_Objects)
		{
			if (night < NIGHT_DAY_BELOW)
			{
				obj.VB_SetLevel(VB_RiftBase.LEVEL_DAY);
				obj.m_VB_LastT = -1;
				continue;
			}

			dSq = vector.DistanceSq(ppos, obj.GetPosition());
			if (dSq > activeSq)
			{
				obj.VB_SetLevel(VB_RiftBase.LEVEL_IDLE);
				obj.m_VB_LastT = -1;
				continue;
			}

			t = Math.ModFloat(now + obj.m_VB_Phase, BEAT_PERIOD);
			k = Math.Clamp(VB_Beat(t, BEAT_FIRST) + 0.65 * VB_Beat(t, BEAT_SECOND), 0.0, 1.0);
			level = Math.Round(k * 3.0);
			if (night < NIGHT_FULL_ABOVE && level > 1)
				level = 1;	// сумерки: только тлеет

			obj.VB_SetLevel(level);
			obj.VB_SetLightPulse(k, night);

			// сердцебиение: запускаем звук на переходе через момент первого удара
			if (obj.VB_IsHeart() && night >= NIGHT_LIGHT_ABOVE && dSq < soundSq && obj.m_VB_LastT >= 0)
			{
				if (t >= soundAt && (obj.m_VB_LastT < soundAt || obj.m_VB_LastT > t))
					obj.VB_PlayBeat();
			}
			obj.m_VB_LastT = t;
		}
	}

	// корни берут ритм ближайшего сердца в LINK_DIST, со сдвигом на время пробега волны
	protected void VB_Relink()
	{
		float linkSq = LINK_DIST * LINK_DIST;
		VB_RiftBase best;
		float bestSq;
		float dSq;
		foreach (VB_RiftBase root : m_VB_Objects)
		{
			if (root.VB_IsHeart())
				continue;

			best = null;
			bestSq = linkSq;
			foreach (VB_RiftBase heart : m_VB_Objects)
			{
				if (!heart.VB_IsHeart())
					continue;

				dSq = vector.DistanceSq(root.GetPosition(), heart.GetPosition());
				if (dSq < bestSq)
				{
					bestSq = dSq;
					best = heart;
				}
			}

			if (best)
				root.m_VB_Phase = Math.ModFloat(best.m_VB_Phase - Math.Sqrt(bestSq) / WAVE_SPEED + 10.0 * BEAT_PERIOD, BEAT_PERIOD);
		}
	}

	// до maxCount ближайших к игроку сердец не дальше maxDist
	protected void VB_PickHearts(vector ppos, int maxCount, float maxDist, array<VB_RiftBase> chosen)
	{
		float maxSq = maxDist * maxDist;
		VB_RiftBase pick;
		float pickSq;
		float cSq;
		for (int n = 0; n < maxCount; n++)
		{
			pick = null;
			pickSq = maxSq;
			foreach (VB_RiftBase cand : m_VB_Objects)
			{
				if (!cand.VB_IsHeart() || chosen.Find(cand) >= 0)
					continue;

				cSq = vector.DistanceSq(ppos, cand.GetPosition());
				if (cSq < pickSq)
				{
					pickSq = cSq;
					pick = cand;
				}
			}

			if (!pick)
				break;

			chosen.Insert(pick);
		}
	}

	// свет и споры получают ближайшие сердца, остальные гаснут; днём ни света, ни спор
	protected void VB_UpdateLights(vector ppos, float night)
	{
		array<VB_RiftBase> lit = new array<VB_RiftBase>;
		array<VB_RiftBase> spored = new array<VB_RiftBase>;
		if (night >= NIGHT_LIGHT_ABOVE)
		{
			VB_PickHearts(ppos, MAX_LIGHTS, LIGHT_DIST, lit);
			VB_PickHearts(ppos, MAX_SPORES, SPORE_DIST, spored);
		}

		foreach (VB_RiftBase item : m_VB_Objects)
		{
			item.VB_SetLight(lit.Find(item) >= 0);
			item.VB_SetSpores(spored.Find(item) >= 0);
		}
	}
}
