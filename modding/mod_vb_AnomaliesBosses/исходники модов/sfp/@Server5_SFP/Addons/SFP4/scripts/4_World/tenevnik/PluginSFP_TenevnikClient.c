// Клиентский менеджер эффектов Теневника (без 5_Mission и без модификации PlayerBase.OnRPC).
// Делает:
// 1) Пси-звук в "наушниках" (луп) всем игрокам, кто находится в радиусе 60м от агрессивного Теневника.
// 2) Ослепление (как у светошумовой гранаты, ТОЛЬКО визуал), если игрок смотрит на агрессивного Теневника
//    непрерывно дольше 3 секунд (работает в прицеле и без), и только до 60м.
// 3) Сильная дрожь камеры, если игрок в радиусе ауры агрессивного Теневника (сила зависит от расстояния + фазы).
// 4) Слабый жёлтый оттенок (чем ближе к агрессивному Теневнику, тем сильнее).

class PluginSFP_TenevnikClient extends PluginBase
{
	// -----------------------------
	// Константы (не в json)
	// -----------------------------
	static const float PSI_RADIUS_M            = 60.0;

	static const float LOOK_SEARCH_RADIUS_M    = 120.0;
	static const float LOOK_MAX_DIST_M         = 60.0;  // дальше 60м ослепление не работает
	static const float LOOK_TRIGGER_TIME_S     = 3.0;   // сколько надо смотреть непрерывно
	static const float LOOK_DECAY_RATE         = 2.2;   // как быстро "сбрасывается" таймер, если отвёл взгляд

	static const float LOOK_ANGLE_DEG_HIP      = 10.0;
	static const float LOOK_ANGLE_DEG_ADS      = 7.0;
	static const float LOOK_ANGLE_JITTER_DEG   = 2.0;   // небольшой разброс, чтобы не требовать идеально центр

	static const float BLIND_HOLD_S            = 1.6;   // держим белый экран
	static const float BLIND_FADE_S            = 7.0;   // и дальше долго уходит
	static const float BLIND_COOLDOWN_S        = 10.0;  // защита от спама

	static const float TINT_RAMP_UP            = 1.6;
	static const float TINT_RAMP_DOWN          = 2.4;
	static const float TINT_GAMMA              = 1.7;

	static const float SHAKE_MAX               = 0.14;
	static const float SHAKE_LERP_SPEED        = 10.0;

	static const float UPDATE_STEP_S           = 0.05;  // 20 раз/сек

	// -----------------------------
	// Состояние
	// -----------------------------
	protected float m_Accu;
	protected ref array<Object>    m_Objects;
	protected ref array<CargoBase> m_Cargos;

	protected EffectSound m_PsiSnd;

	protected PPERequester_TenevnikBlindFlash m_BlindReq;
	protected float m_LookHoldTime;
	protected float m_BlindTime;
	protected float m_BlindCooldown;
	protected bool  m_BlindActive;

	protected PPERequester_TenevnikAuraTint m_TintReq;
	protected float m_TintIntensity;

	protected float m_ShakeIntensity;

	// Вызов из сущности Теневника: принудительно убрать эффекты прямо сейчас (например при смерти)
	void SFP_ForceClearAllNow()
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		SFP_StopAll(pb, true);
	}

	override void OnUpdate(float delta_time)
	{
		if (GetGame().IsDedicatedServer())
			return;

		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb || !pb.IsAlive() || !pb.IsControlledPlayer())
		{
			SFP_StopAll(pb, false);
			return;
		}

		m_Accu += delta_time;
		if (m_Accu < UPDATE_STEP_S)
			return;

		float dt = m_Accu;
		m_Accu = 0.0;

		SFP_Update(pb, dt);
	}

	protected void SFP_StopAll(PlayerBase pb, bool forceClear)
	{
		// звук
		if (pb)
			SFP_StopPsi();

		// ослепление
		SFP_BlindApply(0.0, true);
		m_LookHoldTime = 0.0;
		m_BlindTime = 0.0;
		m_BlindCooldown = 0.0;
		m_BlindActive = false;

		// жёлтый тинт
		SFP_TintApply(0.0, true);

		// дрожь
		m_ShakeIntensity = 0.0;
		if (pb)
			SFP_SetCamShake(pb, 0.0, 0.0);
	}

	protected void SFP_Update(PlayerBase pb, float dt)
	{
		if (!m_Objects) m_Objects = new array<Object>;
		if (!m_Cargos)  m_Cargos  = new array<CargoBase>;

		m_Objects.Clear();
		m_Cargos.Clear();
		GetGame().GetObjectsAtPosition(pb.GetPosition(), LOOK_SEARCH_RADIUS_M, m_Objects, m_Cargos);

		vector camPos = GetGame().GetCurrentCameraPosition();
		vector camDir = GetGame().GetCurrentCameraDirection();
		camDir.Normalize();

		// 1) Пси-звук (в радиусе 60м от агрессивного)
		float psiR2 = PSI_RADIUS_M * PSI_RADIUS_M;
		bool psiActive = false;

		// 2) Взгляд: выбираем "лучшего" Теневника в центре
		SFP_Tenevnik bestT;
		float bestDist = 1e12;

		float angleDeg = LOOK_ANGLE_DEG_HIP;
		if (pb.IsInOptics() || pb.IsInIronsights())
			angleDeg = LOOK_ANGLE_DEG_ADS;
		angleDeg = angleDeg + Math.RandomFloat(-LOOK_ANGLE_JITTER_DEG, LOOK_ANGLE_JITTER_DEG);
		angleDeg = Math.Clamp(angleDeg, 3.0, 20.0);
		float dotThreshold = Math.Cos(angleDeg * Math.DEG2RAD);
		float bestDot = dotThreshold;

		// 3) Дрожь камеры в зоне ауры
		float wantShake = 0.0;

		// 4) Жёлтый тинт
		float wantTint = 0.0;

		for (int i = 0; i < m_Objects.Count(); i++)
		{
			SFP_Tenevnik t = SFP_Tenevnik.Cast(m_Objects[i]);
			if (!t)
				continue;

			if (!t.IsAlive())
				continue;

			float dsqToPlayer = vector.DistanceSq(t.GetPosition(), pb.GetPosition());

			// Пси-звук + дрожь + жёлтый оттенок — только от агрессивного
			if (t.SFP_IsAggressive())
			{
				if (dsqToPlayer <= psiR2)
					psiActive = true;

				// дрожь камеры: если игрок внутри радиуса ауры
				float rAura = t.SFP_GetAuraRadius();
				if (rAura < 0.1)
					rAura = PSI_RADIUS_M;

				if (dsqToPlayer <= (rAura * rAura))
				{
					float d = Math.Sqrt(dsqToPlayer);
					float base01 = 1.0 - (d / rAura);
					base01 = Math.Clamp(base01, 0.0, 1.0);

					float phaseMul = t.SFP_GetAuraPhaseMultiplier();
					float s = base01 * phaseMul;
					if (s > wantShake)
						wantShake = s;

					float tt = Math.Pow(base01, TINT_GAMMA);
					if (tt > wantTint)
						wantTint = tt;
				}
			}

			// Взгляд: только если Теневник агрессивный и ближе 60м
			if (!t.SFP_IsAggressive())
				continue;

			vector tpos = t.GetPosition() + "0 1.3 0";
			vector to = tpos - camPos;
			float dist = to.Length();
			if (dist <= 0.1 || dist > LOOK_MAX_DIST_M)
				continue;

			to.Normalize();
			float dot = vector.Dot(camDir, to);
			if (dot > bestDot)
			{
				if (SFP_HasLineOfSight(camPos, tpos, pb, t))
				{
					bestDot = dot;
					bestT = t;
					bestDist = dist;
				}
			}
		}

		// Пси-звук
		if (psiActive)
			SFP_StartPsi(pb);
		else
			SFP_StopPsi();

		// Ослепление: копим время, пока игрок смотрит на Теневника
		if (m_BlindCooldown > 0.0)
			m_BlindCooldown = Math.Max(0.0, m_BlindCooldown - dt);

		bool isLooking = (bestT != null);
		if (!m_BlindActive)
		{
			if (isLooking && m_BlindCooldown <= 0.0)
				m_LookHoldTime = m_LookHoldTime + dt;
			else
				m_LookHoldTime = Math.Max(0.0, m_LookHoldTime - dt * LOOK_DECAY_RATE);

			if (isLooking && m_BlindCooldown <= 0.0 && m_LookHoldTime >= LOOK_TRIGGER_TIME_S)
			{
				// Если игрок уже ослеплён ванильной светошумовой — не накладываем второй эффект.
				PPERequester_FlashbangEffects fb = PPERequester_FlashbangEffects.Cast(PPERequesterBank.GetRequester(PPERequester_FlashbangEffects));
				if (!fb || !fb.IsRequesterRunning())
					SFP_BlindStart();

				m_LookHoldTime = 0.0;
			}
		}
		else
		{
			m_LookHoldTime = 0.0;
		}

		SFP_BlindUpdate(dt);

		// Жёлтый тинт (плавно)
		m_TintIntensity = Math.Clamp(Math.Lerp(m_TintIntensity, wantTint, Math.Clamp(dt * TINT_RAMP_UP, 0.0, 1.0)), 0.0, 1.0);
		if (wantTint < 0.001)
			m_TintIntensity = Math.Clamp(m_TintIntensity - dt * TINT_RAMP_DOWN, 0.0, 1.0);

		SFP_TintApply(m_TintIntensity, false);

		// Дрожь камеры
		SFP_AuraShakeApply(pb, wantShake, dt);
	}

	// ---------------------------------------
	// Пси-звук
	// ---------------------------------------
	protected void SFP_StartPsi(PlayerBase pb)
	{
		if (m_PsiSnd)
			return;

		string ss = "SFP_MozgoTras_PsiAura_Soundset";
		if (SFP_Tenevnik.FX_SoundsetsPsiAura && SFP_Tenevnik.FX_SoundsetsPsiAura.Count() > 0)
			ss = SFP_Tenevnik.FX_SoundsetsPsiAura[0];

		pb.PlaySoundSetLoop(m_PsiSnd, ss, 0.2, 0.6);
	}

	protected void SFP_StopPsi()
	{
		if (!m_PsiSnd)
			return;

		m_PsiSnd.SoundStop();
		m_PsiSnd = null;
	}

	// ---------------------------------------
	// Ослепление (PPERequester)
	// ---------------------------------------
	protected PPERequester_TenevnikBlindFlash SFP_GetBlindReq()
	{
		if (!m_BlindReq)
			m_BlindReq = PPERequester_TenevnikBlindFlash.Cast(PPERequesterBank.GetRequester(PPERequester_TenevnikBlindFlash));
		return m_BlindReq;
	}

	protected void SFP_BlindStart()
	{
		m_BlindActive = true;
		m_BlindTime = 0.0;
		m_BlindCooldown = BLIND_COOLDOWN_S;
		SFP_BlindApply(1.0, false);
	}

	protected void SFP_BlindUpdate(float dt)
	{
		if (!m_BlindActive)
			return;

		m_BlindTime = m_BlindTime + dt;

		float intensity = 0.0;
		if (m_BlindTime <= BLIND_HOLD_S)
		{
			intensity = 1.0;
		}
		else if (m_BlindTime <= (BLIND_HOLD_S + BLIND_FADE_S))
		{
			float t = (m_BlindTime - BLIND_HOLD_S) / BLIND_FADE_S;
			t = Math.Clamp(t, 0.0, 1.0);
			float k = Easing.EaseInQuart(t);
			intensity = 1.0 - k;
		}
		else
		{
			m_BlindActive = false;
			intensity = 0.0;
		}

		SFP_BlindApply(intensity, false);

		if (!m_BlindActive)
			SFP_BlindApply(0.0, true);
	}

	protected void SFP_BlindApply(float intensity01, bool forceClear)
	{
		PPERequester_TenevnikBlindFlash req = SFP_GetBlindReq();
		if (!req)
			return;

		if (forceClear)
		{
			req.ForceClearNow();
			if (req.IsRequesterRunning())
				req.Stop();
			return;
		}

		float f = Math.Clamp(intensity01, 0.0, 1.0);
		req.SetIntensity(f);

		if (f > 0.001)
		{
			if (!req.IsRequesterRunning())
				req.Start();
		}
		else
		{
			if (req.IsRequesterRunning())
				req.Stop();
		}
	}

	// ---------------------------------------
	// Жёлтый тинт (PPERequester)
	// ---------------------------------------
	protected PPERequester_TenevnikAuraTint SFP_GetTintReq()
	{
		if (!m_TintReq)
			m_TintReq = PPERequester_TenevnikAuraTint.Cast(PPERequesterBank.GetRequester(PPERequester_TenevnikAuraTint));
		return m_TintReq;
	}

	protected void SFP_TintApply(float intensity01, bool forceClear)
	{
		PPERequester_TenevnikAuraTint req = SFP_GetTintReq();
		if (!req)
			return;

		if (forceClear)
		{
			m_TintIntensity = 0.0;
			req.SetIntensity(0.0);
			if (req.IsRequesterRunning())
				req.Stop();
			return;
		}

		float f = Math.Clamp(intensity01, 0.0, 1.0);
		req.SetIntensity(f);

		if (f > 0.001)
		{
			if (!req.IsRequesterRunning())
				req.Start();
		}
		else
		{
			if (req.IsRequesterRunning())
				req.Stop();
		}
	}

	// ---------------------------------------
	// Дрожь камеры
	// ---------------------------------------
	protected void SFP_AuraShakeApply(PlayerBase pb, float want01, float dt)
	{
		m_ShakeIntensity = Math.Lerp(m_ShakeIntensity, want01, Math.Clamp(dt * SHAKE_LERP_SPEED, 0.0, 1.0));
		if (m_ShakeIntensity < 0.002)
		{
			SFP_SetCamShake(pb, 0.0, 0.0);
			return;
		}

		float amp = SHAKE_MAX * m_ShakeIntensity;
		float sx = Math.RandomFloat(-amp, amp);
		float sy = Math.RandomFloat(-amp, amp);
		SFP_SetCamShake(pb, sx, sy);
	}

	protected void SFP_SetCamShake(PlayerBase pb, float x, float y)
	{
		DayZPlayerImplementAiming aim = pb.GetAimingModel();
		if (!aim)
			return;
		aim.SetCamShakeValues(x, y);
	}

	// ---------------------------------------
	// Линия видимости
	// ---------------------------------------
	protected bool SFP_HasLineOfSight(vector from, vector to, PlayerBase pb, Object targetObj)
	{
		RaycastRVParams ray = new RaycastRVParams(from, to, pb);
		ray.flags = CollisionFlags.ALLOBJECTS;
		ray.type = ObjIntersectView;
		ray.sorted = false;

		array<ref RaycastRVResult> hits = new array<ref RaycastRVResult>;
		if (!DayZPhysics.RaycastRVProxy(ray, hits) || hits.Count() == 0)
			return true;

		float bestDistSq = 1e12;
		Object bestObj;
		Object bestParent;

		for (int i = 0; i < hits.Count(); i++)
		{
			RaycastRVResult r = hits[i];
			if (!r || !r.obj)
				continue;

			if (r.obj == pb)
				continue;
			if (r.parent && r.parent.IsMan())
				continue;

			float dsq = vector.DistanceSq(r.pos, from);
			if (dsq < bestDistSq)
			{
				bestDistSq = dsq;
				bestObj = r.obj;
				bestParent = r.parent;
			}
		}

		if (!bestObj)
			return true;

		if (bestObj == targetObj || bestParent == targetObj)
			return true;

		return false;
	}
}
