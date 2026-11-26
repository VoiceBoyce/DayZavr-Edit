// VB_Echo_Server (2026-11-26): пропуск Echo Runner в полёт и на подушку и расход энергии, перенесены из клиентского
// VB_EchoRunner (VB_EchoRunner_Flight.c ServerTick, VB_EchoRunner.c VB_DrainEnergy) без изменений логики. Физика полёта
// осталась у клиента — она нужна водителю; включается флагами, которые ставит только этот код.

class VB_ERS
{
	// стоянка на подушке держится только аккумулятором Эхо/синтеза (решение 11-20) и тратит его заряд, не баки:
	// 2000 ед. полного аккумулятора Эхо ~8 ч висения; мотор подзаряжает его, как ванильный (0,15 ед./с); синтез не убывает
	static const float PARK_BATTERY_DRAIN = 0.07;
	// энергия в kE (решение пользователя 11-20: единица kE, тик = 10 с; шкала ×32 — ТЗ_VB_EchoBase_буферы.md): бак
	// 20 000 kE; полёт вперёд 64 kE/tick (бак ~52 мин), висение 32 kE/tick (~104 мин), набор высоты ×1.5
	static const float DRAIN_HOVER = 3.2;		// kE/с = 32 kE/tick
	static const float DRAIN_MOVE = 3.2;		// kE/с сверху при полном ходе (вместе 64 kE/tick: 20 000 / 6.4 = 3125 с)
	static const float DRAIN_CLIMB = 1.5;		// множитель при наборе высоты
	static const float CHECK_PERIOD = 0.5;		// с: проверка условий и списание на сервере
};

modded class VB_EchoRunner
{
	// сервер: списать энергию поровну с баков, которые питают машину. С парой синтеза расход не списывается.
	void VB_DrainEnergy(float amount)
	{
		if (VB_FusionPair())
			return;
		for (int pass = 0; pass < 2 && amount > 0.00001; pass++)
		{
			array<EntityAI> tanks = new array<EntityAI>;
			EntityAI t1 = FindAttachmentBySlotName("VB_ER_Tank_1");
			EntityAI t2 = FindAttachmentBySlotName("VB_ER_Tank_2");
			if (VB_TankPowers(t1) && VB_Echo.Get(t1) > 0)
				tanks.Insert(t1);
			if (VB_TankPowers(t2) && VB_Echo.Get(t2) > 0)
				tanks.Insert(t2);
			if (tanks.Count() == 0)
				return;
			float share = amount / tanks.Count();
			amount = 0;
			foreach (EntityAI t : tanks)
				amount += share - VB_Echo.Drain(t, share);
		}
	}
};

modded class VB_EchoRunnerFlight
{
	protected float m_CheckTimer;

	// ------------------------------------------------ сервер: условия полёта и расход энергии
	override void ServerTick(float dt)
	{
		m_CheckTimer += dt;
		if (m_CheckTimer < VB_ERS.CHECK_PERIOD)
			return;
		float step = m_CheckTimer;
		m_CheckTimer = 0;

		VB_EchoRunner car = m_Car;
		float energy = car.VB_TankEnergy();
		bool rig = car.VB_ModulesWorking() == 4 && car.VB_PipesFitted();
		bool ready = rig && energy > 0;							// полёт — от баков
		EntityAI battery = car.VB_ParkBattery();
		bool parkReady = rig && battery != null;				// подушка у заглушенной — от аккумулятора Эхо/синтеза
		if (ready || parkReady)
			dBodyActive(car, ActiveState.ACTIVE);		// стоящее тело засыпает — без шага физики подушки нет

		bool flying = false;
		if (ready && car.VB_FlightActive())
		{
			vector pos = car.GetPosition();
			float agl = pos[1] + car.m_VB_BottomLS - VB_ERFlight.GroundY(car, pos);
			flying = agl > 0.4;
			// расход: висение, ход вперёд, набор высоты
			float rate = VB_ERS.DRAIN_HOVER + VB_ERS.DRAIN_MOVE * Math.AbsFloat(car.m_VB_InFwd);
			if (car.m_VB_InUp > 0.05)
				rate *= VB_ERS.DRAIN_CLIMB;
			car.VB_DrainEnergy(rate * step);
			energy = car.VB_TankEnergy();
			if (energy <= 0)
				ready = false;
		}
		else if (parkReady)
		{
			VB_Echo.Drain(battery, VB_ERS.PARK_BATTERY_DRAIN * step);		// стоянка на подушке — заряд аккумулятора
			parkReady = VB_Echo.Get(battery) > 0;
		}

		float e01 = Math.Clamp(energy / 200.0, 0, 1);
		if (ready != car.m_VB_FlyReady || parkReady != car.m_VB_ParkReady || flying != car.m_VB_Flying || Math.AbsFloat(e01 - car.m_VB_Energy) > 0.01)
		{
			car.m_VB_FlyReady = ready;
			car.m_VB_ParkReady = parkReady;
			car.m_VB_Flying = flying;
			car.m_VB_Energy = e01;
			car.SetSynchDirty();
		}
	}
};
