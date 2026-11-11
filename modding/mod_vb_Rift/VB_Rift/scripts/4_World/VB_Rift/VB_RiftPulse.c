// Общий «пульс» всех объектов Изнанки на клиенте: один таймер на все объекты, а не у каждого свой.
// Днём Изнанка спит: сухие серо-бурые текстуры, без свечения, света и звука. В сумерках тлеет, ночью горит.
// Красный свет горит только у ближайших сердец (не больше MAX_LIGHTS одновременно).
// Корни рядом с сердцем бьются вместе с ним с задержкой по расстоянию: удар бежит от сердца наружу.
// Пока сердце режут (VB_IsDisturbed), всё в STIR_DIST от него просыпается даже днём и бьётся в STIR_RATE раз чаще:
// у каждого объекта свои часы m_VB_Clock с плавно меняющейся скоростью, корни идут по часам своего сердца.
// Звуки: гул у сердец со спорами (VB_RiftBase.VB_SetHum), редкие скрипы корней и шёпот за спиной (VB_PlayAmbient).

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

	// звуки Изнанки: гул у ближайших сердец (вместе со спорами), скрипы корней и шёпот вокруг игрока
	static const string HUM_SET = "VB_RiftHum_SoundSet";
	static const string CREAK_SET = "VB_RiftCreak_SoundSet";
	static const string WHISPER_SET = "VB_RiftWhisper_SoundSet";
	static const float AMBIENT_DIST = 45.0;		// скрипы и шёпот, только если живое сердце ближе этого
	static const float WHISPER_DIST = 25.0;		// шёпот — только совсем рядом с сердцем
	static const float CREAK_DIST = 35.0;		// скрип идёт от корня не дальше этого от игрока
	static const int AMBIENT_MIN_MS = 6000;		// пауза между звуками, мс
	static const int AMBIENT_MAX_MS = 15000;
	static const int AMBIENT_STIR_MIN_MS = 2000;	// пока сердце режут — чаще
	static const int AMBIENT_STIR_MAX_MS = 5000;

	// сердце режут: в этом радиусе всё бьётся чаще и не спит даже днём
	static const float STIR_DIST = 50.0;
	static const float STIR_RATE = 2.5;		// во столько раз чаще (цикл 6 с -> 2.4 с)
	static const float RATE_EASE = 0.8;		// 1/с: как быстро ритм разгоняется и успокаивается

	protected static ref VB_RiftPulse s_VB_Instance;

	protected ref array<VB_RiftBase> m_VB_Objects;
	protected ref array<VB_RiftBase> m_VB_Stirred;	// потревоженные сердца на этом тике
	protected int m_VB_Ticks;
	protected int m_VB_LastTickMs = -100000;
	protected float m_VB_LastNow = -1;
	protected int m_VB_NextAmbientMs;

	void VB_RiftPulse()
	{
		m_VB_Objects = new array<VB_RiftBase>;
		m_VB_Stirred = new array<VB_RiftBase>;
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
		float dt = now - m_VB_LastNow;
		if (m_VB_LastNow < 0 || dt < 0 || dt > 1.0)
			dt = TICK_MS * 0.001;
		m_VB_LastNow = now;
		float ease = Math.Clamp(dt * RATE_EASE, 0.0, 1.0);

		m_VB_Stirred.Clear();
		foreach (VB_RiftBase cand : m_VB_Objects)
		{
			if (cand.VB_IsHeart() && cand.VB_IsDisturbed())
				m_VB_Stirred.Insert(cand);
		}

		if (g_Game.GetTime() >= m_VB_NextAmbientMs)
			VB_PlayAmbient(ppos, night);

		float activeSq = ACTIVE_DIST * ACTIVE_DIST;
		float soundSq = SOUND_DIST * SOUND_DIST;
		float soundAt = BEAT_FIRST - SOUND_LEAD;
		float dSq;
		float t;
		float k;
		float clock;
		float objNight;
		bool stirred;
		int level;
		foreach (VB_RiftBase obj : m_VB_Objects)
		{
			// часы идут всегда, даже у спящих: иначе после пробуждения корни разойдутся с сердцем
			stirred = VB_IsStirred(obj);
			if (stirred)
				obj.m_VB_Rate += (STIR_RATE - obj.m_VB_Rate) * ease;
			else
				obj.m_VB_Rate += (1.0 - obj.m_VB_Rate) * ease;
			obj.m_VB_Clock += dt * obj.m_VB_Rate;

			objNight = night;
			if (stirred)
				objNight = 1.0;	// потревоженная Изнанка не спит и днём

			if (objNight < NIGHT_DAY_BELOW || obj.VB_IsDormant())	// день или сердце вырезано: спит
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

			// корни бьются по часам своего сердца (со сдвигом на пробег волны в m_VB_Phase)
			clock = obj.m_VB_Clock;
			if (!obj.VB_IsHeart() && obj.m_VB_LinkHeart)
				clock = obj.m_VB_LinkHeart.m_VB_Clock;
			t = Math.ModFloat(clock + obj.m_VB_Phase, BEAT_PERIOD);
			k = Math.Clamp(VB_Beat(t, BEAT_FIRST) + 0.65 * VB_Beat(t, BEAT_SECOND), 0.0, 1.0);
			level = Math.Round(k * 3.0);
			if (objNight < NIGHT_FULL_ABOVE && level > 1)
				level = 1;	// сумерки: только тлеет

			obj.VB_SetLevel(level);
			obj.VB_SetLightPulse(k, objNight);

			// сердцебиение: запускаем звук на переходе через момент первого удара
			if (obj.VB_IsHeart() && objNight >= NIGHT_LIGHT_ABOVE && dSq < soundSq && obj.m_VB_LastT >= 0)
			{
				if (t >= soundAt && (obj.m_VB_LastT < soundAt || obj.m_VB_LastT > t))
					obj.VB_PlayBeat();
			}
			obj.m_VB_LastT = t;
		}
	}

	// Скрип корня или шёпот за спиной, если рядом живое сердце и ночь (или его режут).
	// Раз в AMBIENT_MIN..MAX мс; пока сердце режут — чаще.
	protected void VB_PlayAmbient(vector ppos, float night)
	{
		float ambSq = AMBIENT_DIST * AMBIENT_DIST;
		VB_RiftBase heart;
		float heartSq = ambSq;
		float dSq;
		bool stirred = false;
		foreach (VB_RiftBase cand : m_VB_Objects)
		{
			if (!cand.VB_IsHeart() || cand.VB_IsDormant())
				continue;
			dSq = vector.DistanceSq(ppos, cand.GetPosition());
			if (dSq < heartSq)
			{
				heartSq = dSq;
				heart = cand;
			}
		}
		if (heart)
			stirred = heart.VB_IsDisturbed();

		int wait = Math.RandomIntInclusive(AMBIENT_MIN_MS, AMBIENT_MAX_MS);
		if (stirred)
			wait = Math.RandomIntInclusive(AMBIENT_STIR_MIN_MS, AMBIENT_STIR_MAX_MS);
		m_VB_NextAmbientMs = g_Game.GetTime() + wait;

		if (!heart || (!stirred && night < NIGHT_LIGHT_ABOVE))
			return;

		vector pos;
		string soundSet;
		if (heartSq < WHISPER_DIST * WHISPER_DIST && Math.RandomFloat01() < 0.3)
		{
			// шёпот: за спиной, в 2.5–5 м
			vector back = g_Game.GetCurrentCameraDirection() * -1.0;
			back[1] = 0;
			back.Normalize();
			float sideK = Math.RandomFloat(-1.5, 1.5);
			float backK = Math.RandomFloat(2.5, 5.0);	// не два вызова в одном выражении: Enforce затирает результат
			vector side = Vector(back[2], 0, -back[0]) * sideK;
			pos = ppos + back * backK + side + Vector(0, 1.6, 0);
			soundSet = WHISPER_SET;
		}
		else
		{
			// скрип: от случайного живого корня (или сердца) рядом с игроком
			array<VB_RiftBase> near = new array<VB_RiftBase>;
			float creakSq = CREAK_DIST * CREAK_DIST;
			foreach (VB_RiftBase obj : m_VB_Objects)
			{
				if (!obj.VB_IsDormant() && vector.DistanceSq(ppos, obj.GetPosition()) < creakSq)
					near.Insert(obj);
			}
			if (near.Count() == 0)
				return;
			float offX = Math.RandomFloat(-2, 2);
			float offZ = Math.RandomFloat(-2, 2);
			pos = near.GetRandomElement().GetPosition() + Vector(offX, 0.3, offZ);
			soundSet = CREAK_SET;
		}

		EffectSound snd = SEffectManager.PlaySound(soundSet, pos);
		if (snd)
			snd.SetAutodestroy(true);
	}

	// рядом (STIR_DIST) потревоженное сердце
	protected bool VB_IsStirred(VB_RiftBase obj)
	{
		if (m_VB_Stirred.Count() == 0)
			return false;

		float stirSq = STIR_DIST * STIR_DIST;
		vector pos = obj.GetPosition();
		foreach (VB_RiftBase heart : m_VB_Stirred)
		{
			if (vector.DistanceSq(pos, heart.GetPosition()) < stirSq)
				return true;
		}
		return false;
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
			{
				root.m_VB_Phase = Math.ModFloat(best.m_VB_Phase - Math.Sqrt(bestSq) / WAVE_SPEED + 10.0 * BEAT_PERIOD, BEAT_PERIOD);
				root.m_VB_LinkHeart = best;
			}
		}
	}

	// до maxCount ближайших к игроку сердец не дальше maxDist; stirredOnly — только потревоженные (днём)
	protected void VB_PickHearts(vector ppos, int maxCount, float maxDist, array<VB_RiftBase> chosen, bool stirredOnly)
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
				if (!cand.VB_IsHeart() || cand.VB_IsDormant() || chosen.Find(cand) >= 0)
					continue;
				if (stirredOnly && !cand.VB_IsDisturbed())
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

	// свет и споры получают ближайшие сердца, остальные гаснут; днём — только потревоженные
	protected void VB_UpdateLights(vector ppos, float night)
	{
		array<VB_RiftBase> lit = new array<VB_RiftBase>;
		array<VB_RiftBase> spored = new array<VB_RiftBase>;
		bool dayOnlyStirred = night < NIGHT_LIGHT_ABOVE;
		VB_PickHearts(ppos, MAX_LIGHTS, LIGHT_DIST, lit, dayOnlyStirred);
		VB_PickHearts(ppos, MAX_SPORES, SPORE_DIST, spored, dayOnlyStirred);

		foreach (VB_RiftBase item : m_VB_Objects)
		{
			item.VB_SetLight(lit.Find(item) >= 0);
			item.VB_SetSpores(spored.Find(item) >= 0);
		}
	}
}
