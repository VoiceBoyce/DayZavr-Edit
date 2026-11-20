// Сеть базы (только сервер): один таймер на все сети, у устройств нет своего OnUpdate (ТЗ_VB_EchoBase.md, раздел 4).
// Единицы: kE, kE/tick (тик = 10 с). Показатели устройств — VB_EB_Rating (конфиг + настройки).
// Тик: сеть = преобразователь + ретрансляторы волной (каждый в радиусе уже подключённого узла) + всё остальное
// в радиусе любого узла (у каждого узла свой range), с лимитами по видам. Порядок в сети:
//   1) выдача — станции (предметы в отсеке, до inMax) и платформы (вложения машин и игроков в зоне; берут inMax,
//      отдают outMax — разница теряется) берут из запаса;
//   2) ток источников (печь жжёт топливо, только если запас может принять энергию);
//   3) преобразователь с кристаллом: min(ток, inMax) × ночной множитель → в запас (буфер, потом накопители);
//      износ кристалла по принятому (кристалл синтеза не изнашивается).
// Запас = буфер преобразователя + накопители (носители ядра, kE = количество предмета); у накопителя свой
// предел входа и выхода за тик (io).

class VB_EB_Network
{
	protected static ref VB_EB_Network s_Inst;

	protected ref array<VB_EB_Device> m_Devices = new array<VB_EB_Device>;
	protected ref VB_EB_Settings m_S;
	protected ref array<Object> m_Objs = new array<Object>;
	protected ref array<EntityAI> m_Items = new array<EntityAI>;	// вложения станции на этот проход
	protected bool m_Started;
	protected int m_TickCount;
	static const int LIFE_EVERY_TICKS = 360;		// при тике 10 с — раз в час

	static void Register(VB_EB_Device d)
	{
		if (!d)
			return;
		if (!s_Inst)
			s_Inst = new VB_EB_Network();
		s_Inst.Start();
		if (s_Inst.m_Devices.Find(d) < 0)
			s_Inst.m_Devices.Insert(d);
	}

	static void Unregister(VB_EB_Device d)
	{
		if (s_Inst && d)
			s_Inst.m_Devices.RemoveItem(d);
	}

	static VB_EB_Settings GetSettings()
	{
		if (!s_Inst)
			s_Inst = new VB_EB_Network();
		if (!s_Inst.m_S)
			s_Inst.m_S = VB_EB_Settings.Load();
		return s_Inst.m_S;
	}

	protected void Start()
	{
		if (m_Started)
			return;
		m_Started = true;
		if (!m_S)
			m_S = VB_EB_Settings.Load();
		int ms = m_S.tickSeconds * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, ms, true);
	}

	// ------------------------------------------------------------------ тик
	void Tick()
	{
		if (!m_S)
			return;
		float dtTicks = m_S.tickSeconds / VB_EB_Settings.TICK;
		bool night = GetGame().GetWorld().IsNight();

		for (int i = m_Devices.Count() - 1; i >= 0; i--)
		{
			VB_EB_Device d = m_Devices[i];
			if (!d)
			{
				m_Devices.Remove(i);
				continue;
			}
			d.m_VB_Net = null;
			d.m_VB_LastOut = 0;
			d.m_VB_LastIn = 0;
			d.m_VB_LastDrain = 0;
		}

		foreach (VB_EB_Device dev : m_Devices)
		{
			VB_EB_Converter conv = VB_EB_Converter.Cast(dev);
			if (!conv)
				continue;
			if (!conv.VB_IsPlaced())
			{
				conv.VB_ClearNet();
				conv.VB_SetState(VB_EBState.NONE);
				continue;
			}
			BuildNet(conv);
			RunNet(conv, dtTicks, night);
		}

		// ротор ветряка крутит ветер — и вне сети; рабочей базе раз в час продлевается срок жизни (не исчезает)
		m_TickCount++;
		bool refreshLife = (m_TickCount % LIFE_EVERY_TICKS) == 0;
		foreach (VB_EB_Device rest : m_Devices)
		{
			if (!rest)
				continue;
			VB_EB_WindTurbine wt = VB_EB_WindTurbine.Cast(rest);
			if (wt && wt.VB_IsPlaced())
				wt.VB_UpdateSpeed(m_S);
			if (!rest.m_VB_Net && !VB_EB_Converter.Cast(rest))
				rest.VB_SetState(VB_EBState.NONE);
			if (refreshLife && rest.m_VB_Net && rest.VB_IsPlaced())
				rest.SetLifetime(rest.GetLifetimeMax());
		}
	}

	// ------------------------------------------------------------------ состав сети
	protected void BuildNet(VB_EB_Converter conv)
	{
		conv.VB_ClearNet();
		conv.m_VB_Net = conv;
		conv.m_VB_Nodes.Insert(conv);

		// ретрансляторы волной: каждый новый расширяет досягаемость своим радиусом
		bool grew = true;
		while (grew && conv.m_VB_Relays.Count() < m_S.maxRelays)
		{
			grew = false;
			foreach (VB_EB_Device rel : m_Devices)
			{
				if (conv.m_VB_Relays.Count() >= m_S.maxRelays)
					break;
				if (!rel || rel.m_VB_Net || rel.VB_Kind() != VB_EBKind.RELAY || !rel.VB_IsPlaced())
					continue;
				if (!InReach(conv, rel))
					continue;
				rel.m_VB_Net = conv;
				conv.m_VB_Relays.Insert(rel);
				conv.m_VB_Nodes.Insert(rel);
				grew = true;
			}
		}

		foreach (VB_EB_Device d : m_Devices)
		{
			if (!d || d.m_VB_Net || !d.VB_IsPlaced())
				continue;
			int kind = d.VB_Kind();
			array<VB_EB_Device> list = null;
			int limit = 0;
			if (kind == VB_EBKind.WIND || kind == VB_EBKind.SOLAR || kind == VB_EBKind.FURNACE)
			{
				list = conv.m_VB_Sources;
				limit = m_S.maxSources;
			}
			else if (kind == VB_EBKind.STORAGE)
			{
				list = conv.m_VB_Storages;
				limit = m_S.maxStorages;
			}
			else if (kind == VB_EBKind.STATION)
			{
				list = conv.m_VB_Stations;
				limit = m_S.maxStations;
			}
			else if (kind == VB_EBKind.PAD)
			{
				list = conv.m_VB_Pads;
				limit = m_S.maxPads;
			}
			if (!list)
				continue;
			if (!InReach(conv, d))
				continue;
			if (list.Count() >= limit)
			{
				conv.m_VB_StatSkipped++;
				continue;
			}
			d.m_VB_Net = conv;
			list.Insert(d);
		}
	}

	// в радиусе хотя бы одного узла сети (у преобразователя и каждого ретранслятора свой радиус)
	protected bool InReach(VB_EB_Converter conv, VB_EB_Device d)
	{
		vector p = d.GetPosition();
		foreach (VB_EB_Device n : conv.m_VB_Nodes)
		{
			if (!n)
				continue;
			float r = n.VB_Rating().range;
			if (vector.DistanceSq(n.GetPosition(), p) <= r * r)
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------------ работа сети
	protected void RunNet(VB_EB_Converter conv, float dtTicks, bool night)
	{
		// пределы накопителей на этот тик
		foreach (VB_EB_Device stIo : conv.m_VB_Storages)
		{
			float ioTick = stIo.VB_Rating().io * dtTicks;
			stIo.m_VB_IoInLeft = ioTick;
			stIo.m_VB_IoOutLeft = ioTick;
		}

		// 1. выдача
		float used = 0;
		// станция v2: заряжает вложенное в гнёздах зарядки (inMax), ячейки в гнёздах разрядки отдают в запас (outMax)
		foreach (VB_EB_Device st : conv.m_VB_Stations)
		{
			VB_EB_Rating stR = st.VB_Rating();
			float gs = ChargeStation(conv, st, stR.inMax * dtTicks);
			float ds = DrainCells(conv, st, stR.outMax * dtTicks);
			st.m_VB_LastIn = gs / dtTicks;		// заряжено, kE/tick
			st.m_VB_LastOut = ds / dtTicks;		// отдано в сеть из ячеек, kE/tick
			st.m_VB_LastDrain = ds / dtTicks;
			used += gs;
			SetConsumerState(conv, st, gs + ds);
		}
		// накопитель: ячейки в его гнёздах разрядки отдают в запас (идея пользователя 11-20), до io за тик
		foreach (VB_EB_Device stoD : conv.m_VB_Storages)
		{
			float dsto = DrainCells(conv, stoD, stoD.VB_Rating().io * dtTicks);
			stoD.m_VB_LastDrain = dsto / dtTicks;
		}
		foreach (VB_EB_Device pad : conv.m_VB_Pads)
		{
			float padDrawn = 0;
			float gp = ChargePad(conv, pad, dtTicks, padDrawn);
			pad.m_VB_LastOut = gp / dtTicks;
			pad.m_VB_LastIn = padDrawn / dtTicks;
			used += padDrawn;
			SetConsumerState(conv, pad, gp);
		}

		// 2. ток источников
		bool ready = conv.VB_CrystalReady();
		bool accept = ready && PoolSpace(conv) > 0.001;
		float prod = 0;
		foreach (VB_EB_Device src : conv.m_VB_Sources)
		{
			float o = 0;
			if (src.VB_Kind() != VB_EBKind.FURNACE || accept)
				o = src.VB_SourceOutput(m_S, dtTicks);
			src.m_VB_LastOut = o;
			prod += o;
			if (o > 0)
				src.VB_SetState(VB_EBState.WORK);
			else
				src.VB_SetState(VB_EBState.NONE);
		}

		// 3. преобразование в Эхо
		bool fusion = conv.VB_HasFusionCrystal();
		bool nightNow = night || (fusion && m_S.fusionCrystalNightAllDay == 1);
		float mul = 1;
		if (nightNow)
			mul = m_S.nightMultiplier;
		float made = 0;
		if (accept)
		{
			float inMax = conv.VB_Rating().inMax;
			made = PoolAdd(conv, Math.Min(prod, inMax) * mul * dtTicks);
			if (made > 0 && !fusion)
			{
				ItemBase crystal = ItemBase.Cast(conv.VB_Crystal());
				if (crystal)
					crystal.AddQuantity(-made * m_S.crystalWearPerKE);		// до нуля — рассыпается (varQuantityDestroyOnMin)
			}
		}
		conv.m_VB_StatProd = prod;
		conv.m_VB_StatConv = made / dtTicks;
		conv.m_VB_StatUse = used / dtTicks;
		conv.m_VB_StatNight = nightNow;
		conv.m_VB_LastOut = made / dtTicks;
		conv.m_VB_LastIn = Math.Min(prod, conv.VB_Rating().inMax);

		// 4. вид
		if (!ready)
			conv.VB_SetState(VB_EBState.NONE);
		else if (made > 0)
			conv.VB_SetState(VB_EBState.WORK);
		else
			conv.VB_SetState(VB_EBState.IDLE);
		int relayState = VB_EBState.IDLE;
		if (ready)
			relayState = VB_EBState.WORK;
		foreach (VB_EB_Device rel : conv.m_VB_Relays)
			rel.VB_SetState(relayState);
		foreach (VB_EB_Device stor : conv.m_VB_Storages)
		{
			float ioMax = stor.VB_Rating().io * dtTicks;
			stor.m_VB_LastIn = (ioMax - stor.m_VB_IoInLeft) / dtTicks;
			stor.m_VB_LastOut = (ioMax - stor.m_VB_IoOutLeft) / dtTicks;
			if (VB_Echo.Get(stor) > 0)
				stor.VB_SetState(VB_EBState.WORK);
			else
				stor.VB_SetState(VB_EBState.IDLE);
		}

#ifdef DIAG_DEVELOPER
		// значения — в локальные: два вызова одной функции в аргументах затирают друг друга (E71)
		string dProd = Fmt(prod);
		string dConv = Fmt(conv.m_VB_StatConv);
		string dUse = Fmt(conv.m_VB_StatUse);
		float poolNow = PoolGet(conv);
		string dPool = Fmt(poolNow);
		int nSrc = conv.m_VB_Sources.Count();
		int nSt = conv.m_VB_Stations.Count();
		int nPad = conv.m_VB_Pads.Count();
		int nStor = conv.m_VB_Storages.Count();
		int nRel = conv.m_VB_Relays.Count();
		string dHead = "[VB_EB] сеть: ист " + nSrc + " накоп " + nStor + " стан " + nSt + " плат " + nPad + " ретр " + nRel;
		Print(dHead + " | ток " + dProd + " крист " + ready + " эхо " + dConv + " взято " + dUse + " запас " + dPool + " kE ночь " + nightNow);
#endif
	}

	// заряжает — горит; сеть делает Эхо (прошлый тик) или в запасе что-то есть — тлеет; иначе серый.
	// По одному запасу нельзя: станция в этом же тике может выбрать всё, и платформа казалась бы без энергии
	protected void SetConsumerState(VB_EB_Converter conv, VB_EB_Device d, float given)
	{
		if (given > 0)
			d.VB_SetState(VB_EBState.WORK);
		else if (conv.m_VB_StatConv > 0 || PoolGet(conv) > 0)
			d.VB_SetState(VB_EBState.IDLE);
		else
			d.VB_SetState(VB_EBState.NONE);
	}

	// ------------------------------------------------------------------ зарядка
	// станция: вложенное в гнёздах зарядки (баки, аккумулятор, ячейки), без потерь, всего до budget kE за тик
	protected float ChargeStation(VB_EB_Converter conv, VB_EB_Device st, float budget)
	{
		VB_EB_ChargeStation cs = VB_EB_ChargeStation.Cast(st);
		if (!cs)
			return 0;
		m_Items.Clear();
		cs.VB_SlotItems(false, m_Items);
		int n = m_Items.Count();
		if (n == 0)
			return 0;
		// поровну (стенд 11-20: первая по очереди ячейка забирала всю малую выработку, бак не заряжался):
		// сначала каждому равная доля того, что есть в запасе, потом остаток по очереди
		float avail = Math.Min(budget, PoolGet(conv));
		float share = avail / n;
		float given = 0;
		foreach (EntityAI it : m_Items)
		{
			float drawnSt = 0;
			given += ChargeItem(conv, it, Math.Min(share, budget - given), 1, drawnSt);
		}
		foreach (EntityAI rest : m_Items)
		{
			if (budget - given <= 0.00001)
				break;
			float drawnRest = 0;
			given += ChargeItem(conv, rest, budget - given, 1, drawnRest);
		}
		return given;
	}

	// станция и накопитель: ячейки в гнёздах разрядки отдают энергию в запас сети (буфер, накопители с их пределом входа),
	// всего до budget kE за тик. Что запас не принял — обратно в ячейку. Синтез не переливается (CanTransfer)
	protected float DrainCells(VB_EB_Converter conv, VB_EB_Device dev, float budget)
	{
		if (!dev || budget <= 0)
			return 0;
		m_Items.Clear();
		dev.VB_SlotItems(true, m_Items);
		float taken = 0;
		foreach (EntityAI it : m_Items)
		{
			float left = budget - taken;
			if (left <= 0.00001)
				break;
			if (!VB_Echo.CanTransfer(it))
				continue;
			float space = PoolSpace(conv);
			if (space <= 0.00001)
				break;
			float haveE = VB_Echo.GetE(it);
			float want = Math.Min(left, Math.Min(space, haveE));
			if (want <= 0.00001)
				continue;
			float got = VB_Echo.DrainE(it, want);
			float added = PoolAdd(conv, got);
			if (got - added > 0.00001)
				VB_Echo.AddE(it, got - added);
			taken += added;
		}
		return taken;
	}

	// платформа: машины и живые игроки в прямоугольнике над ней; заряжается только установленное — вложения
	// (баки и аккумулятор машины, надетое игроком), не содержимое рюкзаков (решение 11-20).
	// Отдаёт до outMax за тик, из сети берёт в inMax/outMax раз больше (Mk I: 8 → 4, потери 50 %, решение 11-20).
	// Вернёт, сколько отдано; drawn — сколько взято из запаса
	protected float ChargePad(VB_EB_Converter conv, VB_EB_Device pad, float dtTicks, out float drawn)
	{
		drawn = 0;
		VB_EB_Rating r = pad.VB_Rating();
		if (r.outMax <= 0 || r.inMax <= 0)
			return 0;
		float eff = Math.Clamp(r.outMax / r.inMax, 0.01, 1);
		float budget = r.outMax * dtTicks;
		m_Objs.Clear();
		GetGame().GetObjectsAtPosition(pad.GetPosition(), m_S.padRadius, m_Objs, null);
		float given = 0;
		foreach (Object o : m_Objs)
		{
			if (budget - given <= 0.00001)
				break;
			EntityAI ent = EntityAI.Cast(o);
			if (!ent || ent == pad)
				continue;
			bool isCar = Transport.Cast(ent) != null;
			bool isPlayer = PlayerBase.Cast(ent) != null;
			if (!isCar && !isPlayer)
				continue;
			if (isPlayer && !ent.IsAlive())
				continue;
			vector lp = pad.WorldToModel(ent.GetPosition());
			if (Math.AbsFloat(lp[0]) > m_S.padHalfWidth || Math.AbsFloat(lp[2]) > m_S.padHalfLength)
				continue;
			if (lp[1] < -1.5 || lp[1] > m_S.padHeight)
				continue;
			GameInventory inv = ent.GetInventory();
			if (!inv)
				continue;
			for (int ai = 0; ai < inv.AttachmentCount(); ai++)
			{
				if (budget - given <= 0.00001)
					break;
				float drawnPad = 0;
				given += ChargeItem(conv, inv.GetAttachmentFromIndex(ai), budget - given, eff, drawnPad);
				drawn += drawnPad;
			}
		}
		return given;
	}

	// отдать носителю до budget kE (столько он получит); eff — доля, что доходит (станция 1, платформа out/in):
	// из запаса берётся budget / eff. Не носитель, бесконечный, разрушенный, полный — ничего (VB_Echo.SpaceE = 0).
	// Вернёт отданное, drawn — взятое из запаса
	protected float ChargeItem(VB_EB_Converter conv, EntityAI it, float budget, float eff, out float drawn)
	{
		drawn = 0;
		if (!it || budget <= 0)
			return 0;
		float space = VB_Echo.SpaceE(it);
		if (space <= 0)
			return 0;
		float give = Math.Min(space, budget);
		float got = PoolDraw(conv, give / eff);
		if (got <= 0)
			return 0;
		float put = VB_Echo.AddE(it, got * eff);
		float back = got - put / eff;
		if (back > 0.00001)
			PoolAdd(conv, back);
		drawn = got - back;
		return put;
	}

	// ------------------------------------------------------------------ запас: буфер преобразователя + накопители
	protected float PoolGet(VB_EB_Converter conv)
	{
		float e = VB_Echo.Get(conv);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
			e += VB_Echo.Get(st);
		return e;
	}

	// сколько ещё влезет в этом тике (с учётом пределов входа накопителей)
	protected float PoolSpace(VB_EB_Converter conv)
	{
		float e = VB_Echo.Capacity(conv) - VB_Echo.Get(conv);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
			e += Math.Min(VB_Echo.Capacity(st) - VB_Echo.Get(st), st.m_VB_IoInLeft);
		return e;
	}

	protected float PoolAdd(VB_EB_Converter conv, float e)
	{
		if (e <= 0)
			return 0;
		float rest = e - VB_Echo.Add(conv, e);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
		{
			if (rest <= 0.00001)
				break;
			float putSt = VB_Echo.Add(st, Math.Min(rest, st.m_VB_IoInLeft));
			st.m_VB_IoInLeft -= putSt;
			rest -= putSt;
		}
		return e - rest;
	}

	protected float PoolDraw(VB_EB_Converter conv, float e)
	{
		if (e <= 0)
			return 0;
		float rest = e - VB_Echo.Drain(conv, e);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
		{
			if (rest <= 0.00001)
				break;
			float tookSt = VB_Echo.Drain(st, Math.Min(rest, st.m_VB_IoOutLeft));
			st.m_VB_IoOutLeft -= tookSt;
			rest -= tookSt;
		}
		return e - rest;
	}

	// ------------------------------------------------------------------ общее
	// ветряк и панель: луч вверх задел крышу, навес, крону — не работают
	static bool IsCovered(Object o, VB_EB_Settings s)
	{
		vector from = o.GetPosition() + Vector(0, 0.3, 0);
		vector to = from + Vector(0, s.roofCheckHeight, 0);
		vector cp;
		vector cd;
		int comp;
		return DayZPhysics.RaycastRV(from, to, cp, cd, comp, null, null, o, false, false, ObjIntersectGeom);
	}

	static string Fmt(float v)
	{
		return (Math.Round(v * 10) / 10).ToString();
	}

	// цифры для курсора действий (сервер, по запросу клиента — VB_EB_Info.c): итоги последнего тика сети
	static void FillInfo(VB_EB_Device d, VB_EB_NetInfo info)
	{
		VB_EB_Settings s = GetSettings();
		VB_EB_Rating r = d.VB_Rating();
		info.m_OutMax = r.outMax;
		info.m_InMax = r.inMax;
		info.m_Io = r.io;
		info.m_Range = r.range;
		info.m_In = d.m_VB_LastIn;
		info.m_Out = d.m_VB_LastOut;
		info.m_Drain = d.m_VB_LastDrain;
		d.VB_FillInfo(s, info);
		VB_EB_Converter conv = d.m_VB_Net;
		if (!conv)
			return;
		info.m_InNet = true;
		info.m_Prod = conv.m_VB_StatProd;
		info.m_Conv = conv.m_VB_StatConv;
		info.m_Use = conv.m_VB_StatUse;
		info.m_Night = conv.m_VB_StatNight;
		info.m_NightMul = s.nightMultiplier;
		info.m_Pool = VB_Echo.Get(conv);
		info.m_PoolMax = VB_Echo.Capacity(conv);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
		{
			info.m_Pool = info.m_Pool + VB_Echo.Get(st);
			info.m_PoolMax = info.m_PoolMax + VB_Echo.Capacity(st);
		}
		if (conv.VB_HasFusionCrystal())
			info.m_Crystal = 2;
		else if (conv.VB_CrystalReady())
			info.m_Crystal = 1;
		info.m_Sources = conv.m_VB_Sources.Count();
		info.m_Storages = conv.m_VB_Storages.Count();
		info.m_Stations = conv.m_VB_Stations.Count();
		info.m_Pads = conv.m_VB_Pads.Count();
		info.m_Relays = conv.m_VB_Relays.Count();
		info.m_Skipped = conv.m_VB_StatSkipped;
	}
};
