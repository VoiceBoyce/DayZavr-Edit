// VB_Echo_Server (2026-11-26): серверная часть устройств базы, перенесена из клиентского VB_EchoBase/VB_EB_Devices.c
// без изменений логики. Клиент оставил крючки VB_ServerInit / VB_ServerDelete / VB_SendInfo — здесь они наполнены.
// Выработка, топливо печи, крыша над ветряком и панелью, ответ курсору и состав сети — только сервер.

modded class VB_EB_Device
{
	protected VB_EB_Rating m_VB_RatingCached;	// сервер: показатели класса (держит кэш VB_EBS_Rating — тут без ref)

	VB_EB_Rating VB_Rating()
	{
		if (!m_VB_RatingCached)
			m_VB_RatingCached = VB_EBS_Rating.Of(this, VB_EB_Network.GetSettings());
		return m_VB_RatingCached;
	}

	// ветряк и панель: крыша над устройством — луч раз в COVER_EVERY_MS (устройство не двигается; раньше луч на каждый
	// тик и на каждый ответ курсору)
	protected bool m_VB_Covered;
	protected int m_VB_CoverAt = -1000000;
	static const int COVER_EVERY_MS = 60000;

	bool VB_IsCovered(VB_EB_Settings s)
	{
		int now = GetGame().GetTime();
		if (now - m_VB_CoverAt >= COVER_EVERY_MS)
		{
			m_VB_CoverAt = now;
			m_VB_Covered = VB_EB_Network.IsCovered(this, s);
		}
		return m_VB_Covered;
	}

	// сервер: выработка тока источником сейчас, kE/tick (ветер, солнце; у печи — есть ли горящая порция)
	float VB_SourceOutput(VB_EB_Settings s, float dtTicks)
	{
		return 0;
	}

	// сервер, источники: выработать ток в свой буфер за dtTicks, вернуть, сколько легло (полный буфер — лишнее
	// пропадает). Ветряк и панель — по погоде, печь переопределяет (жжёт, только пока есть место)
	float VB_Generate(VB_EB_Settings s, float dtTicks)
	{
		float rate = VB_SourceOutput(s, 0);
		return VB_CurAdd(rate * dtTicks);
	}

	// сервер: своё для курсора (ветер, солнце, топливо); общее заполняет VB_EB_Network.FillInfo
	void VB_FillInfo(VB_EB_Settings s, VB_EB_NetInfo info)
	{
	}

	// сервер: одному игроку — не чаще раза в SERVER_INFO_MS по всем устройствам (свой клиент спрашивает раз в секунду,
	// чаще — только подделанный: не даём засыпать сервер запросами)
	static const int SERVER_INFO_MS = 300;
	protected static ref map<string, int> s_VB_InfoSentAt;

	override protected void VB_SendInfo(PlayerIdentity sender)
	{
		if (!sender || !VB_IsPlaced())
			return;
		if (!s_VB_InfoSentAt)
			s_VB_InfoSentAt = new map<string, int>;
		string pid = sender.GetId();
		int now = GetGame().GetTime();
		int last;
		if (s_VB_InfoSentAt.Find(pid, last) && now - last < SERVER_INFO_MS)
			return;
		s_VB_InfoSentAt.Set(pid, now);
		Man man = sender.GetPlayer();
		if (!man || vector.Distance(man.GetPosition(), GetPosition()) > INFO_MAX_DIST)
			return;
		VB_EB_NetInfo info = new VB_EB_NetInfo;
		VB_EB_Network.FillInfo(this, info);
		ScriptRPC rpc = new ScriptRPC();
		info.Write(rpc);
		rpc.Send(this, VB_EB_RPC.INFO, true, sender);
	}

	override void VB_ServerInit()
	{
		VB_EB_Network.Register(this);
	}

	override void VB_ServerDelete()
	{
		VB_EB_Network.Unregister(this);
	}
};

// ------------------------------------------------------------------ источники тока
modded class VB_EB_WindTurbine
{
	// сервер, каждый тик для всех поставленных ветряков
	void VB_UpdateSpeed(VB_EB_Settings s)
	{
		float outMax = VB_Rating().outMax;
		float frac = 0;
		if (outMax > 0)
			frac = VB_SourceOutput(s, 0) / outMax;
		int sp = Math.Round(Math.Clamp(frac, 0, 1) * 20);
		if (sp == m_VB_Speed)
			return;
		m_VB_Speed = sp;
		SetSynchDirty();
	}

	override float VB_SourceOutput(VB_EB_Settings s, float dtTicks)
	{
		if (VB_IsCovered(s))
			return 0;
		Weather w = GetGame().GetWeather();
		float maxSpeed = w.GetWindMaximumSpeed();
		if (maxSpeed <= 0)
			maxSpeed = 10;
		return VB_Rating().outMax * Math.Clamp(w.GetWindSpeed() / maxSpeed, 0, 1);
	}

	override void VB_FillInfo(VB_EB_Settings s, VB_EB_NetInfo info)
	{
		info.m_Covered = VB_IsCovered(s);
		info.m_Wind = GetGame().GetWeather().GetWindSpeed();
		info.m_Out = VB_SourceOutput(s, 0);
	}
};

modded class VB_EB_SolarPanel
{
	// высота солнца 0..1 (ночь — 0)
	static float VB_SunFactor()
	{
		World world = GetGame().GetWorld();
		if (world.GetSunOrMoon() < 0.5)
			return 0;
		vector dir = world.GetSunOrMoonDirection();		// направление света: от солнца вниз
		return Math.Clamp(-dir[1], 0, 1);
	}

	override float VB_SourceOutput(VB_EB_Settings s, float dtTicks)
	{
		if (VB_IsCovered(s))
			return 0;
		float overcast = GetGame().GetWeather().GetOvercast().GetActual();
		return VB_Rating().outMax * VB_SunFactor() * (1 - s.solarOvercastLoss * overcast);
	}

	override void VB_FillInfo(VB_EB_Settings s, VB_EB_NetInfo info)
	{
		info.m_Covered = VB_IsCovered(s);
		info.m_Sun = VB_SunFactor();
		info.m_Cloud = GetGame().GetWeather().GetOvercast().GetActual();
		info.m_Out = VB_SourceOutput(s, 0);
	}
};

modded class VB_EB_Furnace
{
	protected string m_VB_BurnType;		// сервер: что горит (для курсора; после рестарта пусто — порция догорит молча)

	protected void VB_SetFuelLeft(float v)
	{
		SetQuantity(Math.Max(0, v), false);
	}

	// горит ли сейчас (для курсора): есть горящая порция — выработка на полную
	override float VB_SourceOutput(VB_EB_Settings s, float dtTicks)
	{
		if (VB_FuelLeft() > 0)
			return VB_Rating().outMax;
		return 0;
	}

	// жжёт со скоростью своего выхода в свой буфер тока и только пока в нём есть место — топливо зря не горит
	// (ТЗ_VB_EchoBase_буферы.md; в сети печь или нет — неважно, буфер свой)
	override float VB_Generate(VB_EB_Settings s, float dtTicks)
	{
		float want = Math.Min(VB_Rating().outMax * dtTicks, VB_CurSpace());
		float got = 0;
		float left = VB_FuelLeft();
		int portions = 0;
		while (got < want && portions < 16)		// предел — на случай огромного выхода из настроек
		{
			portions++;
			if (left <= 0)
			{
				left = VB_TakeFuel(s);
				if (left <= 0)
					break;
			}
			float take = Math.Min(left, want - got);
			left -= take;
			got += take;
		}
		if (left != VB_FuelLeft())
			VB_SetFuelLeft(left);
		return VB_CurAdd(got);
	}

	// первая подходящая штука из отсека: из стопки — одна, иначе предмет целиком. Вернёт kE порции (0 — нечего жечь)
	protected float VB_TakeFuel(VB_EB_Settings s)
	{
		CargoBase cargo = GetInventory().GetCargo();
		if (!cargo)
			return 0;
		for (int i = 0; i < cargo.GetItemCount(); i++)
		{
			EntityAI e = cargo.GetItem(i);
			if (!e || e.IsSetForDeletion())		// сожжённое в этом же тике ещё лежит в отсеке до удаления
				continue;
			VB_EB_FuelEntry f = s.FindFuel(e.GetType());
			if (!f)
				continue;
			m_VB_BurnType = f.type;
			ItemBase ib = ItemBase.Cast(e);
			if (ib && ib.GetQuantity() > 1)
				ib.AddQuantity(-1);
			else
				e.Delete();
			return f.energy;
		}
		return 0;
	}

	override void VB_FillInfo(VB_EB_Settings s, VB_EB_NetInfo info)
	{
		float left = VB_FuelLeft();
		info.m_FuelLeft = left;
		info.m_BurnType = m_VB_BurnType;
		// свой запас печи (решение 11-20): kE в горящей порции + топливо в отсеке (стопка — по штукам, как жжёт VB_TakeFuel)
		info.m_FuelStock = left;
		CargoBase cargo = GetInventory().GetCargo();
		if (!cargo)
			return;
		for (int i = 0; i < cargo.GetItemCount(); i++)
		{
			EntityAI e = cargo.GetItem(i);
			if (!e || e.IsSetForDeletion())
				continue;
			VB_EB_FuelEntry f = s.FindFuel(e.GetType());
			if (!f)
				continue;
			info.m_HasFuel = true;
			float pieces = 1;
			ItemBase ib = ItemBase.Cast(e);
			if (ib && ib.GetQuantity() > 1)
				pieces = ib.GetQuantity();
			info.m_FuelStock = info.m_FuelStock + f.energy * pieces;
		}
	}
};

// ------------------------------------------------------------------ ядро сети
modded class VB_EB_Converter
{
	// сервер: состав сети и итоги последнего тика (заполняет VB_EB_Network)
	ref array<VB_EB_Device> m_VB_Nodes = new array<VB_EB_Device>;		// сам преобразователь и ретрансляторы
	ref array<VB_EB_Device> m_VB_Relays = new array<VB_EB_Device>;
	ref array<VB_EB_Device> m_VB_Sources = new array<VB_EB_Device>;
	ref array<VB_EB_Device> m_VB_Storages = new array<VB_EB_Device>;
	ref array<VB_EB_Device> m_VB_Stations = new array<VB_EB_Device>;
	ref array<VB_EB_Device> m_VB_Pads = new array<VB_EB_Device>;
	float m_VB_StatProd;		// ток источников, kE/tick
	float m_VB_StatConv;		// сделано Эхо, kE/tick
	float m_VB_StatUse;			// взято из запаса станциями и платформами, kE/tick
	bool m_VB_StatNight;
	int m_VB_StatSkipped;		// устройств в радиусе сверх лимита

	void VB_ClearNet()
	{
		m_VB_Nodes.Clear();
		m_VB_Relays.Clear();
		m_VB_Sources.Clear();
		m_VB_Storages.Clear();
		m_VB_Stations.Clear();
		m_VB_Pads.Clear();
		m_VB_StatSkipped = 0;
	}
};
