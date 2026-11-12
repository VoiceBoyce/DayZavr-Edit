/**
 * SFP_Datcik_alarm
 *
 * Датчик-звонок (House):
 * - На СЕРВЕРЕ/ХОСТЕ сканирует радиус 40м.
 * - Срабатывает только на цели, которые ДВИЖУТСЯ (или недавно двигались).
 *   - Игрок/заражённый/животное/транспорт при появлении в зоне считаются "двигавшимися" (чтобы вход в зону сразу включал сигнал).
 *   - Если цель перестала двигаться, через некоторое время (таймаут) датчик отключится.
 *   - Поворот корпуса (изменение ориентации) тоже считается движением.
 * - Пытается не реагировать "за стеной" (простая проверка видимости Raycast).
 *
 * На КЛИЕНТАХ:
 * - Воспроизводит зацикленный SoundSet "SFP_zvonok_alarm1_Soundset" из memory point "sound_point"
 * - Включает свет в memory point "light_point" и мигает им (таймером, надёжно).
 *
 * Важно:
 * - В модели должны быть memory points: sound_point, light_point
 * - SoundSet "SFP_zvonok_alarm1_Soundset" должен существовать в config.cpp
 */

class SFP_DatcikMotionInfo
{
	vector m_LastPos;
	vector m_LastOri;
	float  m_LastMoveTimeS;
	float  m_LastSeenTimeS;

	void SFP_DatcikMotionInfo(vector pos, vector ori, float nowS, float moveTimeS)
	{
		m_LastPos       = pos;
		m_LastOri       = ori;
		m_LastSeenTimeS = nowS;
		m_LastMoveTimeS = moveTimeS;
	}
}

class SFP_Datcik_alarm extends House
{
	// --- tuning
	protected const float SFP_ALARM_RADIUS_METERS = 10.0;
	protected const float SFP_SCAN_INTERVAL_SEC  = 0.5;

	// считаем цель "активной", если она двигалась не так давно
	protected const float SFP_STILL_TIMEOUT_SEC  = 4.0;

	// пороги движения
	protected const float SFP_MIN_SPEED_MPS      = 0.05; // скорость (м/с)
	protected const float SFP_MIN_POS_DELTA_M    = 0.02; // сдвиг позиции между сканами (м)
	protected const float SFP_MIN_YAW_DELTA_DEG  = 2.0;  // поворот (градусы)

	// очистка памяти о целях
	protected const float SFP_FORGET_TARGET_SEC  = 10.0;

	// --- networked state
	protected bool m_SFP_AlarmState;

	// --- server-side scanning
	protected ref Timer m_SFP_ScanTimer;
	protected ref map<Object, ref SFP_DatcikMotionInfo> m_SFP_MotionMap;

	#ifndef SERVER
	// --- client-side effects
	protected EffectSound     m_SFP_AlarmSound;
	protected PointLightBase  m_SFP_AlarmLight;

	protected ref Timer m_SFP_BlinkTimer;
	protected bool      m_SFP_BlinkOn;
	#endif

	void SFP_Datcik_alarm()
	{
		RegisterNetSyncVariableBool("m_SFP_AlarmState");

		if (!m_SFP_MotionMap)
			m_SFP_MotionMap = new map<Object, ref SFP_DatcikMotionInfo>;
	}

	override void EEInit()
	{
		super.EEInit();

		// Start scanning only where "IsServer" is true (server in MP, host in SP).
		if (GetGame() && GetGame().IsServer())
		{
			if (!m_SFP_ScanTimer)
				m_SFP_ScanTimer = new Timer(CALL_CATEGORY_SYSTEM);

			m_SFP_ScanTimer.Run(SFP_SCAN_INTERVAL_SEC, this, "SFP_ScanForTargets", null, true);
		}

		#ifndef SERVER
		// In case of late init / SP, apply current state
		SFP_ApplyClientEffects(m_SFP_AlarmState);
		#endif
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (m_SFP_ScanTimer && m_SFP_ScanTimer.IsRunning())
			m_SFP_ScanTimer.Stop();

		#ifndef SERVER
		SFP_StopClientEffects();
		#endif
	}

	// --------------------------
	// Server scanning
	// --------------------------
	void SFP_ScanForTargets()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		float nowS = GetGame().GetTime() * 0.001;

		bool should_be_on = SFP_HasVisibleMovingTarget(nowS);

		// чистим старые записи (не влияет на should_be_on, просто чтобы не копилось)
		SFP_CleanupMotionMap(nowS);

		if (should_be_on != m_SFP_AlarmState)
		{
			m_SFP_AlarmState = should_be_on;
			SetSynchDirty();
		}
	}

	protected bool SFP_HasVisibleMovingTarget(float nowS)
	{
		array<Object> objects = new array<Object>();
		GetGame().GetObjectsAtPosition(GetPosition(), SFP_ALARM_RADIUS_METERS, objects, null);

		foreach (Object obj : objects)
		{
			if (!obj || obj == this)
				continue;

			// не реагируем на здания/статик
			if (obj.IsInherited(House) || obj.IsInherited(Building))
				continue;

			EntityAI ent = EntityAI.Cast(obj);
			if (!ent)
				continue;

			// обновим "движение" для всех целей в радиусе (даже если пока за стеной)
			SFP_UpdateMotion(ent, nowS);

			// движется/двигалась недавно?
			if (!SFP_IsRecentlyMoving(ent, nowS))
				continue;

			// простая проверка "не за стеной"
			if (SFP_IsLineOfSightClear(obj))
				return true;
		}

		return false;
	}

	protected void SFP_UpdateMotion(EntityAI ent, float nowS)
	{
		if (!m_SFP_MotionMap)
			m_SFP_MotionMap = new map<Object, ref SFP_DatcikMotionInfo>;

		SFP_DatcikMotionInfo info = m_SFP_MotionMap.Get(ent);

		vector pos = ent.GetPosition();
		vector ori = ent.GetOrientation();

		vector vel = GetVelocity(ent);
		float speed = vel.Length();

		if (!info)
		{
			// При первом обнаружении:
			// - для "живых" целей и транспорта считаем, что они "активны" (вход в зону сразу включает датчик)
			// - для предметов/контейнеров по умолчанию НЕ считаем активными, если они уже лежат и не двигаются
			float initMove = nowS - (SFP_STILL_TIMEOUT_SEC + 1.0);
			if (SFP_PresenceShouldTrigger(ent) || speed > SFP_MIN_SPEED_MPS)
				initMove = nowS;

			info = new SFP_DatcikMotionInfo(pos, ori, nowS, initMove);
			m_SFP_MotionMap.Insert(ent, info);
			return;
		}

		// delta pos
		float pos_delta = (pos - info.m_LastPos).Length();

		// delta yaw
		float yaw_delta = SFP_GetYawDeltaDeg(ori[0], info.m_LastOri[0]);

		bool moved = (speed > SFP_MIN_SPEED_MPS) || (pos_delta > SFP_MIN_POS_DELTA_M) || (yaw_delta > SFP_MIN_YAW_DELTA_DEG);

		if (moved)
			info.m_LastMoveTimeS = nowS;

		info.m_LastSeenTimeS = nowS;
		info.m_LastPos = pos;
		info.m_LastOri = ori;
	}

	protected bool SFP_IsRecentlyMoving(EntityAI ent, float nowS)
	{
		if (!m_SFP_MotionMap)
			return false;

		SFP_DatcikMotionInfo info = m_SFP_MotionMap.Get(ent);
		if (!info)
			return false;

		return (nowS - info.m_LastMoveTimeS) <= SFP_STILL_TIMEOUT_SEC;
	}

	protected void SFP_CleanupMotionMap(float nowS)
	{
		if (!m_SFP_MotionMap)
			return;

		for (int i = m_SFP_MotionMap.Count() - 1; i >= 0; i--)
		{
			Object key = m_SFP_MotionMap.GetKey(i);
			SFP_DatcikMotionInfo info = m_SFP_MotionMap.GetElement(i);

			if (!key || !info)
			{
				m_SFP_MotionMap.RemoveElement(i);
				continue;
			}

			// если давно не видели в радиусе — забываем
			if ((nowS - info.m_LastSeenTimeS) > SFP_FORGET_TARGET_SEC)
			{
				m_SFP_MotionMap.RemoveElement(i);
				continue;
			}
		}
	}

	protected bool SFP_PresenceShouldTrigger(EntityAI ent)
	{
		// "живые" и транспорт — вход в зону должен сразу включать сигнал,
		// даже если на первом скане цель уже стоит.
		if (ent.IsInherited(Man))
			return true;

		if (ent.IsInherited(ZombieBase))
			return true;

		if (ent.IsInherited(DayZAnimal))
			return true;

		if (ent.IsInherited(Transport))
			return true;

		return false;
	}

	protected float SFP_GetYawDeltaDeg(float a, float b)
	{
		a = Math.NormalizeAngle(a);
		b = Math.NormalizeAngle(b);

		float d = Math.AbsFloat(a - b);
		if (d > 180.0)
			d = 360.0 - d;

		return d;
	}

	// --------------------------
	// Line of sight
	// --------------------------
	protected bool SFP_IsLineOfSightClear(Object obj)
	{
		// start from sound point (если его нет - с позиции объекта чуть выше)
		vector from = SFP_GetWorldPosFromMemoryPoint("sound_point");
		if (from == vector.Zero)
		{
			from = GetPosition();
			from[1] = from[1] + 1.0;
		}

		// aim to object position (слегка выше, чтобы не цеплять землю)
		vector to = obj.GetPosition();
		to[1] = to[1] + 1.0;

		RaycastRVParams params = new RaycastRVParams(from, to, this);
		params.flags  = CollisionFlags.NEARESTCONTACT;
		params.type   = ObjIntersectView;
		params.sorted = true;

		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>;
		bool hit = DayZPhysics.RaycastRVProxy(params, results);

		if (!hit || results.Count() == 0)
			return true;

		RaycastRVResult res = results[0];
		if (!res)
			return true;

		if (res.obj == obj || res.parent == obj)
			return true;

		return false;
	}

	protected vector SFP_GetWorldPosFromMemoryPoint(string mem_point)
	{
		if (MemoryPointExists(mem_point))
		{
			vector mp = GetMemoryPointPos(mem_point);
			return ModelToWorld(mp);
		}
		return vector.Zero;
	}

	// --------------------------
	// Client-side effects
	// --------------------------
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		#ifndef SERVER
		SFP_ApplyClientEffects(m_SFP_AlarmState);
		#endif
	}

	#ifndef SERVER
	protected void SFP_ApplyClientEffects(bool state)
	{
		if (state)
			SFP_StartClientEffects();
		else
			SFP_StopClientEffects();
	}

	protected void SFP_StartClientEffects()
	{
		// Sound
		if (!m_SFP_AlarmSound || !m_SFP_AlarmSound.IsPlaying())
		{
			if (MemoryPointExists("sound_point"))
				PlaySoundSetAtMemoryPointLoopedSafe(m_SFP_AlarmSound, "SFP_zvonok_alarm1_Soundset", "sound_point", 0.2, 0.2);
			else
				PlaySoundSetLoop(m_SFP_AlarmSound, "SFP_zvonok_alarm1_Soundset", 0.2, 0.2);
		}

		// Light
		if (!m_SFP_AlarmLight)
		{
			m_SFP_AlarmLight = PointLightBase.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(SFP_DatcikAlarmLight, this, "light_point"));
		}

		// Start blinking timer (надёжно, независимо от SetBlinkingSpeed)
		SFP_StartBlinking();
	}

	protected void SFP_StartBlinking()
	{
		if (!m_SFP_AlarmLight)
			return;

		// включаем сразу
		m_SFP_BlinkOn = true;
		m_SFP_AlarmLight.SetEnabled(true);

		if (!m_SFP_BlinkTimer)
			m_SFP_BlinkTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (!m_SFP_BlinkTimer.IsRunning())
			m_SFP_BlinkTimer.Run(0.25, this, "SFP_BlinkTick", null, true);
	}

	void SFP_BlinkTick()
	{
		if (!m_SFP_AlarmLight)
			return;

		m_SFP_BlinkOn = !m_SFP_BlinkOn;
		m_SFP_AlarmLight.SetEnabled(m_SFP_BlinkOn);
	}

	protected void SFP_StopClientEffects()
	{
		StopSoundSet(m_SFP_AlarmSound);

		if (m_SFP_BlinkTimer && m_SFP_BlinkTimer.IsRunning())
			m_SFP_BlinkTimer.Stop();

		if (m_SFP_AlarmLight)
		{
			m_SFP_AlarmLight.Destroy();
			m_SFP_AlarmLight = null;
		}
	}
	#endif
}
