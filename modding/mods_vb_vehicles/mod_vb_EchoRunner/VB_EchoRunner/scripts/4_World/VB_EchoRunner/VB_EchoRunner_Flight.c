// Echo Runner, этап 8: полёт на антиграв-модулях (как Deluxo из GTA) и энергия Изнанки в баках.
// ТЗ: ТЗ_VB_EchoRunner_полёт.md. Условия полёта: 4 модуля вместо колёс + 3 трубопровода + энергия в баках + мотор.
//
// Как устроено (по образцу RFFS — вертолёты сервера 16 — и разбора в скилле dayz-aviation):
//  * машину с водителем физически ведёт его клиент (NetworkMoveStrategy.PHYSICS), сервер повторяет те же ходы;
//    поэтому шаг полёта считается и на клиенте водителя, и на сервере (IsServerOrOwner) — одинаково, из одного ввода;
//  * ввод (W/S, A/D, Shift/Ctrl — свои действия в data/vb_er_inputs.xml) едет к серверу внутри хода машины
//    (VB_EchoRunnerMove); если ходов нет — сервер читает синхронизированный ввод водителя, как RFFS;
//  * управление кинематическое: целевая скорость через SetVelocity (+ g*dt: тело само вычитает тяжесть после нас),
//    ориентация — угловой скоростью к целевому положению (поворот между ориентациями — как в RFFS DayZavr);
//  * сервер раз в 0.5 с проверяет условия (m_VB_FlyReady, синхронизируется) и списывает энергию с баков
//    (энергия бака = его количество 0..100).

#ifdef FEATURE_NETWORK_RECONCILIATION
class VB_EchoRunnerMove : CarScriptMove
{
	float m_VB_Fwd;		// W - S
	float m_VB_Turn;	// D - A
	float m_VB_Up;		// Shift - Ctrl

	protected override event void Write(PawnMoveWriter ctx, PawnMove prev)
	{
		super.Write(ctx, prev);
		ctx.Write(m_VB_Fwd);
		ctx.Write(m_VB_Turn);
		ctx.Write(m_VB_Up);
	}

	protected override event void Read(PawnMoveReader ctx, PawnMove prev)
	{
		super.Read(ctx, prev);
		ctx.Read(m_VB_Fwd);
		ctx.Read(m_VB_Turn);
		ctx.Read(m_VB_Up);
	}

	protected override event int EstimateMaximumSize()
	{
		return super.EstimateMaximumSize() + 12;
	}
};
#endif

// отладка на стенде dayz-mcp (DayZDiag): стенд не умеет жать игровые действия, поэтому цифровой блок клиента
// дублирует клавиши полёта (ловит 5_Mission/VB_EchoRunner_Debug.c). В обычной игре (без DIAG_DEVELOPER) не работает.
class VB_ERDebug
{
	static float s_Fwd;
	static float s_Turn;
	static float s_Up;

	static void Key(int key, float v)
	{
#ifdef DIAG_DEVELOPER
		switch (key)
		{
		case KeyCode.KC_NUMPAD8:
			s_Fwd = v;
			break;
		case KeyCode.KC_NUMPAD2:
			s_Fwd = -v;
			break;
		case KeyCode.KC_NUMPAD6:
			s_Turn = v;
			break;
		case KeyCode.KC_NUMPAD4:
			s_Turn = -v;
			break;
		case KeyCode.KC_NUMPAD9:
			s_Up = v;
			break;
		case KeyCode.KC_NUMPAD3:
			s_Up = -v;
			break;
		}
#endif
	}
};

// настройки полёта (ТЗ, раздел 3–4; цифры по умолчанию — предложенные, ждут проверки пользователем)
class VB_ERFlight
{
	static const float HOVER = 1.2;			// м: высота парения низа модулей над землёй/водой
	static const float PARK_HOVER = 0.45;	// м: стоянка на подушке (мотор заглушён / нет водителя, энергия есть)
	static const float PARK_DAMP = 6.0;		// м/с²: на стоянке машину не сносит
	static const float PARK_DRAIN = 0.004;	// ед./с: расход на стоянке (2 полных бака ~14 ч)
	static const float RPM_FLY = 4000;		// об/мин мотора при полной тяге (на нейтрали; выше — перегазовка, отзыв 11-18)
	static const float FOLLOW_AGL = 8.0;	// м: ниже — держим высоту над рельефом, выше — абсолютную
	static const float VMAX = 33.0;			// м/с вперёд (~120 км/ч)
	static const float VBACK = 10.0;		// м/с назад
	static const float ACCEL = 7.0;			// м/с² разгон
	static const float BRAKE = 10.0;		// м/с² торможение/реверс клавишей
	static const float GLIDE = 3.0;			// м/с² замедление, когда клавиши отпущены
	static const float SIDE_DAMP = 6.0;		// м/с² гашение бокового сноса
	static const float CLIMB = 7.0;			// м/с набор высоты (Shift)
	static const float DESC = 6.0;			// м/с снижение (Ctrl)
	static const float VACC = 10.0;			// м/с² вертикальный разгон
	static const float FALL = 3.0;			// м/с: без тяги машина опускается не быстрее
	static const float YAW_RATE = 1.3;		// рад/с поворот (A/D)
	static const float YAW_ACC = 3.5;		// рад/с²: предел разгона поворота (было 4 — рывок на старте, отзыв 11-18)
	static const float YAW_TAU = 0.25;		// с: плавный выход на скорость поворота и плавная остановка
	static const float PITCH_UP = 14.0;		// град: нос вверх/вниз от Shift/Ctrl
	static const float PITCH_ACC = 4.0;		// град: нос вниз при разгоне вперёд
	static const float BANK = 16.0;			// град: крен в повороте
	static const float ATT_GAIN = 5.0;		// 1/с: как быстро корпус выходит на целевой наклон
	static const float G = 9.81;

	// знаки осей движка (сверить на стенде: Shift — нос вверх, D — вправо с креном вправо)
	static const float PITCH_SGN = 1.0;
	static const float ROLL_SGN = -1.0;

	// энергия: 2 полных бака (200) ~ 20 минут полёта вперёд, висение — вдвое экономнее, набор высоты — дороже
	static const float DRAIN_HOVER = 0.0834;	// ед./с
	static const float DRAIN_MOVE = 0.0833;		// ед./с сверху при полном ходе
	static const float DRAIN_CLIMB = 1.5;		// множитель при наборе высоты
	static const float CHECK_PERIOD = 0.5;		// с: проверка условий и списание на сервере

	static const string MODULE = "VB_EchoRunner_GravModule";
	static const string TANK = "VB_EchoRunner_Tank";

	static float Approach(float cur, float target, float step)
	{
		if (cur < target)
			return Math.Min(cur + step, target);
		return Math.Max(cur - step, target);
	}

	static float AngleDiff(float a, float b)
	{
		float d = a - b;
		while (d > 180)
			d -= 360;
		while (d < -180)
			d += 360;
		return d;
	}

	// поворот (ось * угол, рад) от одной ориентации (рыскание, тангаж, крен, град) к другой — как INT_RFFS_RotationBetween
	static vector RotationBetween(vector from_orient, vector to_orient)
	{
		vector a[3];
		vector b[3];
		Math3D.YawPitchRollMatrix(from_orient, a);
		Math3D.YawPitchRollMatrix(to_orient, b);

		vector r0 = a[0] * b[0];
		vector r1 = a[1] * b[1];
		vector r2 = a[2] * b[2];
		vector rot = (r0 + r1 + r2) * 0.5;
		float trace = vector.Dot(a[0], b[0]) + vector.Dot(a[1], b[1]) + vector.Dot(a[2], b[2]);
		float cosA = Math.Clamp((trace - 1.0) * 0.5, -1.0, 1.0);
		float sinA = rot.Length();
		if (sinA < 0.000001)
		{
			if (cosA > 0)
				return vector.Zero;
			return a[2] * Math.PI;
		}
		float ang = Math.Atan2(sinA, cosA);
		return rot * (ang / sinA);
	}

	// Скорость рыскания корпуса сейчас, рад/с (+ вправо, как yawRate в Attitude). Берём из физики тела, а не из своей
	// переменной: угловую скорость движок восстанавливает при сетевой сверке и повторе ходов, свою переменную — нет,
	// и сервер с клиентом водителя расходились именно в поворотах (рывки, отзыв 11-18; скилл dayz-aviation LL-194).
	static float s_YawSign;
	static float YawRateOf(IEntity body)
	{
		if (s_YawSign == 0)
		{
			// знак оси y мира для поворота «вправо» — той же формулой, что строит угловую скорость в Attitude
			vector probe = RotationBetween("0 0 0", Vector(Math.RAD2DEG, 0, 0));
			s_YawSign = 1;
			if (probe[1] < 0)
				s_YawSign = -1;
		}
		vector w = dBodyGetAngularVelocity(body);
		return w[1] * s_YawSign;
	}

	// высота опоры под точкой: земля, крыши/дороги/заборы (луч физики, своя машина не мешает) и вода
	static float GroundY(Object self, vector p)
	{
		float y = g_Game.SurfaceY(p[0], p[2]);
		vector from = p + "0 0.5 0";
		vector to = p - "0 60 0";
		PhxInteractionLayers mask = PhxInteractionLayers.BUILDING;
		mask |= PhxInteractionLayers.ROADWAY;
		mask |= PhxInteractionLayers.FENCE;
		mask |= PhxInteractionLayers.TERRAIN;
		Object hitObj;
		vector hitPos, hitNormal;
		float hitFrac;
		if (DayZPhysics.RayCastBullet(from, to, mask, self, hitObj, hitPos, hitNormal, hitFrac))
		{
			if (hitPos[1] > y)
				y = hitPos[1];
		}
		float depth = g_Game.GetWaterDepth(Vector(p[0], y, p[2]));
		if (depth > 0)
			y += depth;
		return y;
	}
};

// шаг полёта, проверка условий и расход энергии — отдельный объект машины (VB_EchoRunner.m_VB_Fly)
class VB_EchoRunnerFlight : Managed
{
	protected VB_EchoRunner m_Car;

	// удержание высоты
	protected bool m_HoldSet;
	protected bool m_HoldAbs;		// true — абсолютная высота (высоко), false — над рельефом
	protected float m_HoldY;
	protected float m_HoldAgl;

	// сервер
	protected float m_CheckTimer;

	void VB_EchoRunnerFlight(VB_EchoRunner car)
	{
		m_Car = car;
	}

	// ------------------------------------------------ шаг физики (сервер и клиент водителя)
	void Step(float dt)
	{
		if (dt <= 0 || dt > 0.2)
			return;

		VB_EchoRunner car = m_Car;
		vector pos = car.GetPosition();
		vector mat[4];
		car.GetTransform(mat);
		vector fwd = mat[2] * car.m_VB_FwdSign;
		vector hf = Vector(fwd[0], 0, fwd[2]);
		if (hf.Length() < 0.05)
		{
			vector upv = mat[1];
			hf = Vector(-upv[0], 0, -upv[2]);		// нос отвесно: курс по «спине»
		}
		hf.Normalize();
		vector hr = Vector(hf[2], 0, -hf[0]);		// вправо по курсу

		float ground = VB_ERFlight.GroundY(car, pos);
		float agl = pos[1] + car.m_VB_BottomLS - ground;
		vector vel = GetVelocity(car);

		bool active = car.VB_FlightActive();
		if (!active && car.m_VB_ModulesFitted && car.m_VB_FlyReady)
		{
			// стоянка на подушке (просьба пользователя 11-17): мотор заглушён или водителя нет, но энергия есть —
			// машина держится низко над землёй, ровно, не дрейфует
			m_HoldSet = false;
			float parkY = ground + VB_ERFlight.PARK_HOVER - car.m_VB_BottomLS;
			vector pv = vel;
			float pvy = Math.Clamp((parkY - pos[1]) * 2.5, -VB_ERFlight.FALL, 3.0);
			pv[1] = VB_ERFlight.Approach(vel[1], pvy, VB_ERFlight.VACC * dt) + VB_ERFlight.G * dt;
			float damp = VB_ERFlight.PARK_DAMP * dt;
			pv[0] = VB_ERFlight.Approach(vel[0], 0, damp);
			pv[2] = VB_ERFlight.Approach(vel[2], 0, damp);
			SetVelocity(car, pv);
			Attitude(0, 0, 0, dt);
			dBodyActive(car, ActiveState.ACTIVE);
			return;
		}
		if (!active)
		{
			m_HoldSet = false;
			if (agl > 0.35 && car.m_VB_ModulesFitted)
			{
				// энергия кончилась / мотор заглох / водитель вышел: плавно опускаемся и выравниваемся
				vel[0] = vel[0] * (1.0 - 0.5 * dt);
				vel[2] = vel[2] * (1.0 - 0.5 * dt);
				if (vel[1] < -VB_ERFlight.FALL)
					vel[1] = -VB_ERFlight.FALL + VB_ERFlight.G * dt;
				SetVelocity(car, vel);
				Attitude(0, 0, 0, dt);
				dBodyActive(car, ActiveState.ACTIVE);
			}
			return;
		}

		float inF = car.m_VB_InFwd;
		float inT = car.m_VB_InTurn;
		float inU = car.m_VB_InUp;

		// --- высота: у земли парим на HOVER и повторяем рельеф; Shift/Ctrl меняют высоту, отпустил — висим
		float minY = ground + VB_ERFlight.HOVER - car.m_VB_BottomLS;
		if (!m_HoldSet)
		{
			m_HoldSet = true;
			m_HoldAbs = false;
			m_HoldAgl = VB_ERFlight.HOVER;
			m_HoldY = minY;
		}
		if (Math.AbsFloat(inU) > 0.05)
		{
			m_HoldAbs = agl > VB_ERFlight.FOLLOW_AGL;
			m_HoldAgl = Math.Max(agl, VB_ERFlight.HOVER);
			m_HoldY = pos[1];
		}
		float tgtY = ground + m_HoldAgl - car.m_VB_BottomLS;
		if (m_HoldAbs)
			tgtY = Math.Max(m_HoldY, minY);

		float vyT;
		if (inU > 0.05)
			vyT = inU * VB_ERFlight.CLIMB;
		else if (inU < -0.05)
			vyT = inU * VB_ERFlight.DESC;
		else
			vyT = Math.Clamp((tgtY - pos[1]) * 2.0, -VB_ERFlight.DESC, VB_ERFlight.CLIMB);
		if (pos[1] < minY)
			vyT = Math.Max(vyT, Math.Min((minY - pos[1]) * 3.0, VB_ERFlight.CLIMB));

		// --- скорость по курсу (W/S), боковой снос гасится
		vector vh = Vector(vel[0], 0, vel[2]);
		float vF = vector.Dot(vh, hf);
		float vR = vector.Dot(vh, hr);
		float tF = 0;
		if (inF > 0.05)
			tF = inF * VB_ERFlight.VMAX;
		else if (inF < -0.05)
			tF = inF * VB_ERFlight.VBACK;
		float acc = VB_ERFlight.GLIDE;
		if (tF != 0)
		{
			acc = VB_ERFlight.BRAKE;
			if (tF * vF >= 0 && Math.AbsFloat(tF) > Math.AbsFloat(vF))
				acc = VB_ERFlight.ACCEL;
		}
		vF = VB_ERFlight.Approach(vF, tF, acc * dt);
		vR = VB_ERFlight.Approach(vR, 0, VB_ERFlight.SIDE_DAMP * dt);
		float vy = VB_ERFlight.Approach(vel[1], vyT, VB_ERFlight.VACC * dt);

		vector nv = hf * vF + hr * vR;
		nv[1] = vy + VB_ERFlight.G * dt;		// тело само вычтет g*dt после нас (скилл dayz-aviation, LFHeli)
		SetVelocity(car, nv);

		// --- поворот, нос, крен. Плавно (отзыв 11-18 «повороты дёрганые»): скорость поворота выходит на цель
		// по экспоненте (YAW_TAU) с пределом разгона, текущая — из тела (YawRateOf); крен следует за самим поворотом,
		// а не за клавишей — нажал A/D, корпус сначала начинает поворачивать и вместе с этим ложится в крен
		float speedK = Math.Clamp(Math.AbsFloat(vF) / VB_ERFlight.VMAX, 0, 1);
		float yrMax = VB_ERFlight.YAW_RATE * (1.0 - 0.3 * speedK);
		float yrT = inT * yrMax;
		float yr = VB_ERFlight.YawRateOf(car);
		float dYr = (yrT - yr) * Math.Min(1.0, dt / VB_ERFlight.YAW_TAU);
		float maxDYr = VB_ERFlight.YAW_ACC * dt;
		yr = yr + Math.Clamp(dYr, -maxDYr, maxDYr);
		float noseUp = inU * VB_ERFlight.PITCH_UP - inF * VB_ERFlight.PITCH_ACC;
		float turnK = Math.Clamp(yr / yrMax, -1, 1);
		float bank = turnK * VB_ERFlight.BANK * Math.Clamp(Math.AbsFloat(vF) / 12.0, 0.25, 1.0);
		Attitude(noseUp, bank, yr, dt);
		dBodyActive(car, ActiveState.ACTIVE);
	}

	// корпус к целевому наклону: noseUp и bankRight в градусах (по носу машины), yawRate рад/с (+ вправо)
	protected void Attitude(float noseUp, float bankRight, float yawRate, float dt)
	{
		VB_EchoRunner car = m_Car;
		vector ori = car.GetOrientation();
		// GetOrientation считает от оси +z модели; нос Echo Runner — m_VB_FwdSign
		float pT = noseUp * car.m_VB_FwdSign * VB_ERFlight.PITCH_SGN;
		float rT = bankRight * car.m_VB_FwdSign * VB_ERFlight.ROLL_SGN;
		float k = Math.Min(1.0, VB_ERFlight.ATT_GAIN * dt);
		vector tgt;
		tgt[0] = ori[0] + yawRate * Math.RAD2DEG * dt;
		tgt[1] = ori[1] + VB_ERFlight.AngleDiff(pT, ori[1]) * k;
		tgt[2] = ori[2] + VB_ERFlight.AngleDiff(rT, ori[2]) * k;
		vector rot = VB_ERFlight.RotationBetween(ori, tgt);
		float rate = rot.Length() / dt;
		vector w = rot * (1.0 / dt);
		if (!(rate < 6.0))
		{
			if (rate > 0 && rate < 1000000.0)
				w = w * (6.0 / rate);
			else
				w = vector.Zero;
		}
		dBodySetAngularVelocity(car, w);
	}

	// ------------------------------------------------ сервер: условия полёта и расход энергии
	void ServerTick(float dt)
	{
		m_CheckTimer += dt;
		if (m_CheckTimer < VB_ERFlight.CHECK_PERIOD)
			return;
		float step = m_CheckTimer;
		m_CheckTimer = 0;

		VB_EchoRunner car = m_Car;
		float energy = car.VB_TankEnergy();
		bool ready = car.VB_ModulesWorking() == 4 && car.VB_PipesFitted() && energy > 0;
		if (ready)
			dBodyActive(car, ActiveState.ACTIVE);		// стоящее тело засыпает — без шага физики подушки нет

		bool flying = false;
		if (ready && car.VB_FlightActive())
		{
			vector pos = car.GetPosition();
			float agl = pos[1] + car.m_VB_BottomLS - VB_ERFlight.GroundY(car, pos);
			flying = agl > 0.4;
			// расход: висение, ход вперёд, набор высоты
			float rate = VB_ERFlight.DRAIN_HOVER + VB_ERFlight.DRAIN_MOVE * Math.AbsFloat(car.m_VB_InFwd);
			if (car.m_VB_InUp > 0.05)
				rate *= VB_ERFlight.DRAIN_CLIMB;
			car.VB_DrainEnergy(rate * step);
			energy = car.VB_TankEnergy();
			if (energy <= 0)
				ready = false;
		}
		else if (ready)
		{
			car.VB_DrainEnergy(VB_ERFlight.PARK_DRAIN * step);		// стоянка на подушке
			energy = car.VB_TankEnergy();
			if (energy <= 0)
				ready = false;
		}

		float e01 = Math.Clamp(energy / 200.0, 0, 1);
		if (ready != car.m_VB_FlyReady || flying != car.m_VB_Flying || Math.AbsFloat(e01 - car.m_VB_Energy) > 0.01)
		{
			car.m_VB_FlyReady = ready;
			car.m_VB_Flying = flying;
			car.m_VB_Energy = e01;
			car.SetSynchDirty();
		}
	}
};
