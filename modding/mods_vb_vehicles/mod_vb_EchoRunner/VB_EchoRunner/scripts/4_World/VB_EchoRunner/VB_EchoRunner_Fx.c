// Echo Runner, этап 8: эффекты антиграв-модулей на клиентах. Три состояния (отзыв пользователя 11-17):
//  * стоянка на подушке (мотор заглушён, энергия есть): модули медленно вращаются, прозрачное марево под ними,
//    очень тихий гул, слабый свет;
//  * мотор заведён, без газа: марево сильнее, гул, вращение быстрее; луча и шлейфа нет;
//  * газ (тяга): луч из каждого модуля и шлейф в движении, рёв тяги, мотор ревёт по оборотам, задние модули
//    наклоняются сильнее — видно, что машину толкают.
// Сила газа: у водителя — по его клавишам, у остальных — по оборотам мотора (обороты видят все).
// Марево — настоящее искажение воздуха: тепловое марево у света (EnableHeatHaze, как у костра и мутанта SFP).
// Вращение небольшое: у кольца 12 одинаковых секций, при быстром вращении стробоскоп «останавливает» его.

class VB_ER_GravLight extends PointLightBase
{
	void VB_ER_GravLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(3.0);
		SetBrightnessTo(0.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.1, 0.14);
		SetDiffuseColor(1.0, 0.1, 0.14);
		EnableHeatHaze(true);
		SetHeatHazeRadius(0.5);
		SetHeatHazePower(0.04);
	}
}

class VB_EchoRunnerFx : Managed
{
	// скорости, об/с: резонатор (крестовина — вдвое быстрее навстречу) и корпус
	static const float ROT_PARK = 0.12;		// стоянка на подушке
	static const float ROT_WEAK = 0.15;		// мотор заведён, лететь не на чем
	static const float ROT_IDLE = 0.45;		// мотор заведён, без газа
	static const float ROT_GAS = 0.65;		// сверху при полном газе
	static const float SPIN_PARK = 0.05;
	static const float SPIN_IDLE = 0.12;
	static const float SPIN_GAS = 0.16;
	static const float ACC = 0.6;			// об/с²: раскрутка и выбег
	static const float TILT_GAIN = 4.0;		// 1/с: сглаживание наклона
	static const float TILT_SX = 1.0;		// знак tilt_x: + — верх модуля к +z модуля
	static const float TILT_SZ = -1.0;		// знак tilt_z: + — верх модуля к -x модуля
	static const float AIR_AGL = 0.3;		// м: модуль выше земли — машина в воздухе

	// излучатели vb_er_grav.ptc
	static const int EM_CORE = 0;
	static const int EM_TRAIL = 1;
	static const int EM_RINGS = 2;
	static const int EM_HAZE = 3;
	static const int EM_ARCS = 4;

	protected VB_EchoRunner m_Car;
	protected float m_Rot;			// об/с резонатора
	protected float m_Spin;			// об/с корпуса
	protected float m_Phase;
	protected float m_SpinPhase;
	protected float m_Gas;			// сила газа 0..1 (сглаженная)
	protected float m_Time;
	protected bool m_HavePrev;
	protected vector m_PrevPos;
	protected float m_PrevYaw;
	protected vector m_Vel;			// сглаженная скорость (по смещению: у чужих машин GetVelocity бывает нулём)
	protected float m_YawRate;
	protected float m_AirCheck;
	protected bool m_Air;
	protected float m_TiltX[4];
	protected float m_TiltZ[4];
	protected VB_ER_GravLight m_Lights[4];
	protected EntityAI m_LightOn[4];
	protected ParticleSource m_Ptc[4];
	protected EntityAI m_PtcOn[4];
	protected EffectSound m_HumIdle;
	protected EffectSound m_HumFly;
	protected EffectSound m_HumThrust;
	protected float m_DiagT;

	void VB_EchoRunnerFx(VB_EchoRunner car)
	{
		m_Car = car;
	}

	float GetSpin()
	{
		return m_Rot + m_Spin;
	}

	void Update(float dt)
	{
		if (dt <= 0)
			return;
		VB_EchoRunner car = m_Car;
		m_Time += dt;

		// --- движение машины (по кадрам)
		vector pos = car.GetPosition();
		vector mat[4];
		car.GetTransform(mat);
		vector fwd = mat[2] * car.m_VB_FwdSign;
		float yaw = Math.Atan2(fwd[0], fwd[2]);
		if (m_HavePrev)
		{
			vector v = (pos - m_PrevPos) * (1.0 / dt);
			if (v.Length() > 120)
				v = m_Vel;							// телепорт/рывок сети — пропустить
			float dy = yaw - m_PrevYaw;
			if (dy > Math.PI)
				dy -= Math.PI2;
			if (dy < -Math.PI)
				dy += Math.PI2;
			float k = Math.Min(1.0, dt * 6.0);
			m_Vel = m_Vel + (v - m_Vel) * k;
			m_YawRate = m_YawRate + (dy / dt - m_YawRate) * k;
		}
		m_HavePrev = true;
		m_PrevPos = pos;
		m_PrevYaw = yaw;

		// --- в воздухе ли (свой луч раз в 0.1 с; флаг сервера — запасной)
		m_AirCheck -= dt;
		if (m_AirCheck <= 0)
		{
			m_AirCheck = 0.1;
			float agl = pos[1] + car.m_VB_BottomLS - VB_ERFlight.GroundY(car, pos);
			m_Air = agl > AIR_AGL;
		}

		bool fitted = car.m_VB_ModulesFitted;
		bool engine = car.EngineIsOn();
		bool ready = fitted && car.m_VB_FlyReady;
		bool driven = ready && engine;					// мотор заведён — летит или висит на 1.2 м
		bool park = ready && !engine;					// стоянка на подушке
		float hv = Vector(m_Vel[0], 0, m_Vel[2]).Length();
		float vF = vector.Dot(Vector(m_Vel[0], 0, m_Vel[2]), Vector(fwd[0], 0, fwd[2]).Normalized());

		// --- сила газа: водитель — по клавишам, остальные — по оборотам мотора
		float gasT = 0;
		if (driven)
		{
			float idle = car.EngineGetRPMIdle();
			float red = car.EngineGetRPMRedline();
			if (red > idle + 100)
				gasT = Math.Clamp((car.EngineGetRPM() - idle * 1.15) / (red - idle * 1.15), 0, 1);
			Human driver = car.CrewDriver();
			if (driver && driver == GetGame().GetPlayer())
				gasT = Math.Max(gasT, car.VB_Thrust());
		}
		m_Gas = VB_ERFlight.Approach(m_Gas, gasT, 3.0 * dt);

		// --- скорости вращения
		float rotT = 0;
		float spinT = 0;
		if (park)
		{
			rotT = ROT_PARK;
			spinT = SPIN_PARK;
		}
		else if (driven)
		{
			rotT = ROT_IDLE + ROT_GAS * m_Gas;
			spinT = SPIN_IDLE + SPIN_GAS * m_Gas;
		}
		else if (fitted && engine)
		{
			rotT = ROT_WEAK;
		}
		m_Rot = VB_ERFlight.Approach(m_Rot, rotT, ACC * dt);
		m_Spin = VB_ERFlight.Approach(m_Spin, spinT, ACC * 0.3 * dt);
		m_Phase = m_Phase + m_Rot * dt;
		m_Phase = m_Phase - Math.Floor(m_Phase);
		m_SpinPhase = m_SpinPhase + m_Spin * dt;
		m_SpinPhase = m_SpinPhase - Math.Floor(m_SpinPhase);

		// --- модули: роторы, наклон, свет и марево, частицы
		vector vl = car.VectorToLocal(m_Vel);
		float leanZcar = Math.Clamp(vl[2] / 22.0, -1, 1) * 0.45;		// к движению вдоль z модели
		float pushDir = car.m_VB_FwdSign;								// газ вперёд — верх задних модулей к носу
		if (vF < -1.0)
			pushDir = -pushDir;
		float turnK = Math.Clamp(m_YawRate / 1.2, -1, 1);
		float rightX = -car.m_VB_LeftSign;						// «вправо» в осях машины
		for (int i = 0; i < 4; i++)
		{
			EntityAI mod = car.VB_ModuleAt(i);
			if (!mod)
			{
				KillLight(i);
				KillPtc(i);
				m_TiltX[i] = 0;
				m_TiltZ[i] = 0;
				continue;
			}
			// наклон в осях машины: по ходу — верх модуля вперёд; задние под газом сильнее (толкают машину);
			// в повороте передние к центру поворота, задние наружу. Фаза ±1 = ±25°.
			bool isFront = car.m_VB_ModFront[i];
			float front = -1.0;
			float lzc = leanZcar;
			if (isFront)
			{
				front = 1.0;
			}
			else
			{
				lzc = leanZcar * 1.4 + m_Gas * 0.55 * pushDir;
			}
			float leanXcar = Math.Clamp(vl[0] / 10.0, -1, 1) * 0.35 + front * turnK * 0.3 * rightX;
			// модуль справа повёрнут на 180° вокруг вертикали (прокси ALEX): его x и z — наоборот
			float side = car.m_VB_ModSide[i];
			float lz = lzc * side;
			float lx = leanXcar * side;
			float kk = Math.Min(1.0, TILT_GAIN * dt);
			m_TiltX[i] = m_TiltX[i] + (Math.Clamp(TILT_SX * lz, -1, 1) - m_TiltX[i]) * kk;
			m_TiltZ[i] = m_TiltZ[i] + (Math.Clamp(TILT_SZ * lx, -1, 1) - m_TiltZ[i]) * kk;
			mod.SetAnimationPhaseNow("rotor", m_Phase);
			mod.SetAnimationPhaseNow("spin", m_SpinPhase);
			// фаза user-источника — только 0..1 (отрицательные движок режет в 0): 0.5 — ровно, 0/1 — ±25°
			mod.SetAnimationPhaseNow("tilt_x", (m_TiltX[i] + 1.0) * 0.5);
			mod.SetAnimationPhaseNow("tilt_z", (m_TiltZ[i] + 1.0) * 0.5);
			UpdateLight(i, mod, park, driven);
			UpdatePtc(i, mod, driven && m_Air, hv);
		}

		UpdateHum(park, driven);
#ifdef DIAG_DEVELOPER
		// стенд: коробка, обороты и углы колёс (проверка, что модули не проворачиваются)
		m_DiagT += dt;
		if (m_DiagT > 2.0 && fitted)
		{
			m_DiagT = 0;
			Print("[VB_ER] gear " + car.GetGear() + " mode " + car.GearboxGetMode() + " rpm " + car.EngineGetRPM() + " wfl " + car.GetAnimationPhase("wheelfrontleft") + " wbl " + car.GetAnimationPhase("wheelbackleft") + " wfr " + car.GetAnimationPhase("wheelfrontright") + " wbr " + car.GetAnimationPhase("wheelbackright") + " tilt " + m_TiltX[1] + " " + m_TiltX[3]);
		}
#endif
	}

	// красный свет под модулем + марево (подушка): на стоянке слабее
	protected void UpdateLight(int i, EntityAI mod, bool park, bool driven)
	{
		if (!park && !driven)
		{
			KillLight(i);
			return;
		}
		VB_ER_GravLight l = m_Lights[i];
		if (l && m_LightOn[i] != mod)
		{
			KillLight(i);
			l = null;
		}
		if (!l)
		{
			l = VB_ER_GravLight.Cast(ScriptedLightBase.CreateLight(VB_ER_GravLight, mod.GetPosition(), 0.6));
			if (!l)
				return;
			l.AttachOnObject(mod, mod.GetMemoryPointPos("light"));
			m_Lights[i] = l;
			m_LightOn[i] = mod;
		}
		float flicker = 0.88 + 0.08 * Math.Sin(m_Time * 23.0 + i * 1.7) + 0.04 * Math.Sin(m_Time * 61.0 + i * 0.6);
		float bright = 0.25;
		float radius = 2.0;
		float haze = 0.035;
		if (driven)
		{
			bright = 0.9 + 1.4 * m_Gas;
			radius = 3.0 + 2.0 * m_Gas;
			haze = 0.06 + 0.04 * m_Gas;
		}
		l.SetBrightnessTo(bright * flicker);
		l.SetRadiusTo(radius);
		l.SetHeatHazePower(haze);
	}

	protected void KillLight(int i)
	{
		if (m_Lights[i])
			m_Lights[i].Destroy();
		m_Lights[i] = null;
		m_LightOn[i] = null;
	}

	// частицы модуля: мотор заведён и машина в воздухе; луч и шлейф — только под газом
	protected void UpdatePtc(int i, EntityAI mod, bool on, float hv)
	{
		if (!on)
		{
			KillPtc(i);
			return;
		}
		ParticleSource ps = m_Ptc[i];
		if (ps && m_PtcOn[i] != mod)
		{
			KillPtc(i);
			ps = null;
		}
		if (!ps)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (!pm)
				return;
			// низ излучателя; поворот — по осям модуля: с force_world_rotation смещение OX считалось по осям мира,
			// и у правых модулей (прокси повёрнут на 180°) луч уходил на 0.38 м наружу (замечено пользователем 11-17)
			vector lp = mod.GetMemoryPointPos("light") + "0 0.2 0";
			ps = pm.PlayOnObject(ParticleList.VB_ER_GRAV, mod, lp, "0 0 0", false);
			if (!ps)
				return;
			m_Ptc[i] = ps;
			m_PtcOn[i] = mod;
		}
		float gas = m_Gas;
		if (gas < 0.05)
			gas = 0;
		float trailK = Math.Clamp(hv / 6.0, 0.3, 1.0);
		ps.SetParameter(EM_CORE, EmitorParam.BIRTH_RATE, 100 * gas);
		ps.SetParameter(EM_TRAIL, EmitorParam.BIRTH_RATE, 90 * gas * trailK);
		ps.SetParameter(EM_RINGS, EmitorParam.BIRTH_RATE, 0);
		ps.SetParameter(EM_HAZE, EmitorParam.BIRTH_RATE, 0);
		ps.SetParameter(EM_ARCS, EmitorParam.BIRTH_RATE, 1.5 + 4 * gas);
	}

	protected void KillPtc(int i)
	{
		if (m_Ptc[i])
			m_Ptc[i].StopParticle();
		m_Ptc[i] = null;
		m_PtcOn[i] = null;
	}

	// гул: холостой (на стоянке очень тихо), полётный (мотор заведён), тяга (газ)
	protected void UpdateHum(bool park, bool driven)
	{
		float idleV = 0;
		float flyV = 0;
		float thrustV = 0;
		if (park)
		{
			idleV = 0.22 * Math.Clamp(m_Rot / ROT_PARK, 0, 1);
		}
		else if (driven)
		{
			idleV = 0.55;
			flyV = Math.Clamp(m_Rot / ROT_IDLE, 0, 1) * (0.55 + 0.25 * m_Gas);
			thrustV = m_Gas;
		}
		else if (m_Rot > 0.02)
		{
			idleV = Math.Clamp(m_Rot / ROT_IDLE, 0, 1) * 0.5;
		}
		m_HumIdle = SetLoop(m_HumIdle, "VB_ER_GravIdle_SoundSet", idleV, 1.0);
		m_HumFly = SetLoop(m_HumFly, "VB_ER_GravFly_SoundSet", flyV, 0.6);
		m_HumThrust = SetLoop(m_HumThrust, "VB_ER_GravThrust_SoundSet", thrustV, 0.3);
	}

	protected EffectSound SetLoop(EffectSound snd, string soundSet, float volume, float fadeIn)
	{
		if (volume < 0.01)
		{
			StopSound(snd);
			return null;
		}
		if (!snd)
			snd = SEffectManager.PlaySoundOnObject(soundSet, m_Car, fadeIn, 1.0, true);
		if (snd)
			snd.SetSoundVolume(volume);
		return snd;
	}

	protected void StopSound(EffectSound snd)
	{
		if (!snd)
			return;
		snd.SetAutodestroy(true);
		snd.SoundStop();
	}

	// модуль снят: вернуть его в покой
	static void ResetModule(EntityAI mod)
	{
		if (!mod)
			return;
		mod.SetAnimationPhaseNow("tilt_x", 0.5);
		mod.SetAnimationPhaseNow("tilt_z", 0.5);
	}

	void Stop()
	{
		for (int i = 0; i < 4; i++)
		{
			KillLight(i);
			KillPtc(i);
		}
		StopSound(m_HumIdle);
		m_HumIdle = null;
		StopSound(m_HumFly);
		m_HumFly = null;
		StopSound(m_HumThrust);
		m_HumThrust = null;
		m_Rot = 0;
		m_Spin = 0;
	}
};
