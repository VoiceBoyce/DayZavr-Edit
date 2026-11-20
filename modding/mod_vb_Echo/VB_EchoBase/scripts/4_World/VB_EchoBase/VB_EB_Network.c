// Сеть базы (только сервер): один таймер на все сети, у устройств нет своего OnUpdate (ТЗ_VB_EchoBase.md, раздел 4).
// Единицы: kE, kE/tick (тик = 10 с). Показатели устройств — VB_EB_Rating (конфиг + настройки).
// Буферы и порядок тика — ТЗ_VB_EchoBase_буферы.md (11-20).
// Состав: сеть = преобразователь + ретрансляторы волной (каждый в радиусе уже подключённого узла) + всё остальное
// в радиусе любого узла (у каждого узла свой range), с лимитами по видам (лишние — в счёт «сверх лимита»).
// Порядок тика в сети (RunNet):
//   1) источники вырабатывают ток в свои буферы (EnergyManager; печь жжёт, только пока в её буфере есть место);
//   2) преобразователь собирает ток в свой буфер: каждый источник — до своего outMax за тик, места мало — поровну;
//   3) с кристаллом: из буфера тока min(ток, inMax) × ночной множитель → Эхо в запас (буфер преобразователя, потом
//      накопители); кристалл не даёт больше своего остатка, износ — по сделанному (синтез не изнашивается);
//   4) ячейки в гнёздах разрядки станций и накопителей отдают энергию в запас;
//   5) станции и платформы набирают свои буферы Эхо из запаса (поровну по запросам; весь доступный запас идёт туда,
//      где есть место — решение пользователя 11-20) и заряжают вложенное уже из них.
// Запас = буфер Эхо преобразователя + накопители (носители ядра, kE = количество предмета); у накопителя свой предел
// входа и выхода за тик (io). Вне сети (RunAlone) источник копит ток в свой буфер, станция и платформа доводят
// зарядку из своего.

class VB_EB_Network
{
	protected static ref VB_EB_Network s_Inst;

	protected ref array<VB_EB_Device> m_Devices = new array<VB_EB_Device>;
	protected ref VB_EB_Settings m_S;
	protected ref array<Object> m_Objs = new array<Object>;
	protected ref array<EntityAI> m_Items = new array<EntityAI>;	// вложения станции на этот проход
	protected ref array<float> m_Amounts = new array<float>;		// запросы на этот проход (параллельно спискам)
	protected ref array<VB_EB_Device> m_Consumers = new array<VB_EB_Device>;	// станции и платформы сети
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
				RunAlone(rest, dtTicks);
			if (refreshLife && rest.m_VB_Net && rest.VB_IsPlaced())
				rest.SetLifetime(rest.GetLifetimeMax());
		}
	}

	// вне сети (ТЗ_VB_EchoBase_буферы.md): источник копит ток в свой буфер, станция и платформа доводят зарядку из своего
	// буфера; остальное — серое
	protected void RunAlone(VB_EB_Device d, float dtTicks)
	{
		if (!d.VB_IsPlaced())
		{
			d.VB_SetState(VB_EBState.NONE);
			return;
		}
		if (d.VB_IsSource())
		{
			float gen = d.VB_Generate(m_S, dtTicks);
			d.m_VB_LastOut = gen / dtTicks;
			SetSourceState(d, d.m_VB_LastOut);
			return;
		}
		int kind = d.VB_Kind();
		if (kind == VB_EBKind.STATION)
		{
			float gs = ChargeStationItems(d, dtTicks);
			d.m_VB_LastOut = gs / dtTicks;
			SetConsumerState(null, d, gs);
			return;
		}
		if (kind == VB_EBKind.PAD)
		{
			float gp = ChargePadFromBuffer(d, dtTicks);
			d.m_VB_LastOut = gp / dtTicks;
			SetConsumerState(null, d, gp);
			return;
		}
		d.VB_SetState(VB_EBState.NONE);
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
		// ретрансляторы сверх лимита — тоже в счёт «не подключено» (раньше молча пропадали)
		if (conv.m_VB_Relays.Count() >= m_S.maxRelays)
		{
			foreach (VB_EB_Device relX : m_Devices)
			{
				if (relX && !relX.m_VB_Net && relX.VB_Kind() == VB_EBKind.RELAY && relX.VB_IsPlaced() && InReach(conv, relX))
					conv.m_VB_StatSkipped++;
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

	// ------------------------------------------------------------------ работа сети (ТЗ_VB_EchoBase_буферы.md, раздел 4)
	// 1) источники копят ток в своих буферах; 2) преобразователь собирает ток к себе: каждый источник отдаёт не больше
	// своего выхода за тик, места мало — поровну; 3) из своего буфера тока делает Эхо (до inMax, ночью ×) в свой буфер
	// Эхо, излишек — в накопители; 4) ячейки в гнёздах разрядки — в запас сети; 5) станции и платформы набирают свои
	// буферы Эхо из запаса (поровну) и заряжают уже из них
	protected void RunNet(VB_EB_Converter conv, float dtTicks, bool night)
	{
		// пределы накопителей на этот тик
		foreach (VB_EB_Device stIo : conv.m_VB_Storages)
		{
			float ioTick = stIo.VB_Rating().io * dtTicks;
			stIo.m_VB_IoInLeft = ioTick;
			stIo.m_VB_IoOutLeft = ioTick;
		}

		// 1. выработка — в буферы источников
		float prod = 0;
		foreach (VB_EB_Device src : conv.m_VB_Sources)
		{
			float gen = src.VB_Generate(m_S, dtTicks);
			src.m_VB_LastOut = gen / dtTicks;
			prod += gen;
		}

		// 2. сбор тока в буфер преобразователя
		m_Amounts.Clear();
		float offerSum = 0;
		foreach (VB_EB_Device srcO : conv.m_VB_Sources)
		{
			float offer = Math.Min(srcO.VB_CurGet(), srcO.VB_Rating().outMax * dtTicks);
			m_Amounts.Insert(offer);
			offerSum += offer;
		}
		float collected = 0;
		float room = conv.VB_CurSpace();
		if (offerSum > 0.00001 && room > 0.00001)
		{
			float kCur = Math.Min(1, room / offerSum);
			for (int si = 0; si < conv.m_VB_Sources.Count(); si++)
			{
				VB_EB_Device srcT = conv.m_VB_Sources[si];
				float took = srcT.VB_CurTake(m_Amounts[si] * kCur);
				collected += conv.VB_CurAdd(took);
			}
		}
		foreach (VB_EB_Device srcS : conv.m_VB_Sources)
			SetSourceState(srcS, srcS.m_VB_LastOut);

		// 3. преобразование: из буфера тока в Эхо (буфер преобразователя, потом накопители)
		bool ready = conv.VB_CrystalReady();
		bool fusion = conv.VB_HasFusionCrystal();
		bool nightNow = night || (fusion && m_S.fusionCrystalNightAllDay == 1);
		float mul = 1;
		if (nightNow)
			mul = m_S.nightMultiplier;
		float made = 0;
		if (ready)
		{
			float canCur = Math.Min(conv.VB_CurGet(), conv.VB_Rating().inMax * dtTicks);
			float wantEcho = Math.Min(canCur * mul, PoolSpace(conv));
			ItemBase crystal = null;
			if (!fusion)
				crystal = ItemBase.Cast(conv.VB_Crystal());
			// кристалл не делает больше своего остатка (раньше последний огрызок давал целый тик)
			if (crystal && m_S.crystalWearPerKE > 0)
				wantEcho = Math.Min(wantEcho, crystal.GetQuantity() / m_S.crystalWearPerKE);
			if (wantEcho > 0.00001)
			{
				made = PoolAdd(conv, wantEcho);
				conv.VB_CurTake(made / mul);
				if (made > 0 && crystal)
				{
					// без звука вложения на каждый тик (VB_Echo.s_Quiet); до нуля — рассыпается (varQuantityDestroyOnMin)
					VB_Echo.s_Quiet = true;
					crystal.AddQuantity(-made * m_S.crystalWearPerKE);
					VB_Echo.s_Quiet = false;
				}
			}
		}
		// сразу: по нему решается, тлеют ли станции и платформы в этом же тике (раньше неон отставал на тик)
		conv.m_VB_StatConv = made / dtTicks;

		// 4. ячейки в гнёздах разрядки — в запас сети (станция: на гнездо drainCell; накопитель: всего его вход io)
		foreach (VB_EB_Device stD : conv.m_VB_Stations)
		{
			float perCell = stD.VB_Rating().drainCell * dtTicks;
			float ds = DrainCells(conv, stD, perCell, perCell * 2);
			stD.m_VB_LastDrain = ds / dtTicks;
		}
		foreach (VB_EB_Device stoD : conv.m_VB_Storages)
		{
			float ioD = stoD.VB_Rating().io * dtTicks;
			float dsto = DrainCells(conv, stoD, ioD, ioD);
			stoD.m_VB_LastDrain = dsto / dtTicks;
		}

		// 5. станции и платформы: свои буферы из запаса, зарядка из них
		m_Consumers.Clear();
		foreach (VB_EB_Device cSt : conv.m_VB_Stations)
			m_Consumers.Insert(cSt);
		foreach (VB_EB_Device cPad : conv.m_VB_Pads)
			m_Consumers.Insert(cPad);
		float used = FillBuffers(conv, dtTicks);
		foreach (VB_EB_Device st : conv.m_VB_Stations)
		{
			float gs = ChargeStationItems(st, dtTicks);
			st.m_VB_LastOut = gs / dtTicks;
			SetConsumerState(conv, st, gs + st.m_VB_LastDrain);
		}
		foreach (VB_EB_Device pad : conv.m_VB_Pads)
		{
			float gp = ChargePadFromBuffer(pad, dtTicks);
			pad.m_VB_LastOut = gp / dtTicks;
			SetConsumerState(conv, pad, gp);
		}

		// итоги тика
		conv.m_VB_StatProd = prod / dtTicks;
		conv.m_VB_StatUse = used / dtTicks;
		conv.m_VB_StatNight = nightNow;
		conv.m_VB_LastIn = collected / dtTicks;
		conv.m_VB_LastOut = made / dtTicks;

		// вид
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
		string dProd = Fmt(conv.m_VB_StatProd);
		string dCol = Fmt(conv.m_VB_LastIn);
		string dConv = Fmt(conv.m_VB_StatConv);
		string dUse = Fmt(conv.m_VB_StatUse);
		float curNow = conv.VB_CurGet();
		string dCur = Fmt(curNow);
		float poolNow = PoolGet(conv);
		string dPool = Fmt(poolNow);
		int nSrc = conv.m_VB_Sources.Count();
		int nSt = conv.m_VB_Stations.Count();
		int nPad = conv.m_VB_Pads.Count();
		int nStor = conv.m_VB_Storages.Count();
		int nRel = conv.m_VB_Relays.Count();
		string dHead = "[VB_EB] сеть: ист " + nSrc + " накоп " + nStor + " стан " + nSt + " плат " + nPad + " ретр " + nRel;
		Print(dHead + " | выработка " + dProd + " собрано " + dCol + " ток " + dCur + " крист " + ready + " эхо " + dConv + " взято " + dUse + " запас " + dPool + " kE ночь " + nightNow);
#endif
	}

	// источник: вырабатывает — горит; ток в буфере есть — тлеет; иначе серый
	protected void SetSourceState(VB_EB_Device src, float rate)
	{
		if (rate > 0)
			src.VB_SetState(VB_EBState.WORK);
		else if (src.VB_CurGet() > 0)
			src.VB_SetState(VB_EBState.IDLE);
		else
			src.VB_SetState(VB_EBState.NONE);
	}

	// станция и платформа: заряжает — горит; в своём буфере или в сети есть Эхо — тлеет; иначе серый (conv — null вне сети)
	protected void SetConsumerState(VB_EB_Converter conv, VB_EB_Device d, float given)
	{
		if (given > 0)
			d.VB_SetState(VB_EBState.WORK);
		else if (VB_Echo.Get(d) > 0 || (conv && (conv.m_VB_StatConv > 0 || PoolGet(conv) > 0)))
			d.VB_SetState(VB_EBState.IDLE);
		else
			d.VB_SetState(VB_EBState.NONE);
	}

	// ------------------------------------------------------------------ буферы станций и платформ
	// набрать свои буферы Эхо из запаса сети: каждой — до inMax за тик и до места в буфере. Энергия идёт туда, где
	// есть место (решение пользователя 11-20); доступного мало — поровну по запросам. Доступно = буфер
	// преобразователя + из каждого накопителя не больше его остатка выхода (io) — раньше доля считалась от всего
	// запаса, накопители упирались в io, и первые станции в списке забирали больше остальных. Вернёт взятое из запаса
	protected float FillBuffers(VB_EB_Converter conv, float dtTicks)
	{
		m_Amounts.Clear();
		float wantSum = 0;
		foreach (VB_EB_Device c : m_Consumers)
		{
			float inTick = c.VB_Rating().inMax * dtTicks;
			float space = VB_Echo.SpaceE(c);
			float w = Math.Min(inTick, space);
			m_Amounts.Insert(w);
			wantSum += w;
		}
		if (wantSum <= 0.00001)
			return 0;
		float k = Math.Min(1, PoolAvail(conv) / wantSum);
		float used = 0;
		for (int i = 0; i < m_Consumers.Count(); i++)
		{
			VB_EB_Device cd = m_Consumers[i];
			float got = PoolDraw(conv, m_Amounts[i] * k);
			if (got <= 0)
				continue;
			float put = VB_Echo.AddE(cd, got);
			if (got - put > 0.00001)
				PoolAdd(conv, got - put);
			cd.m_VB_LastIn = put / dtTicks;
			used += put;
		}
		return used;
	}

	// станция: из своего буфера — вложенному в гнёздах зарядки, без потерь: бак и аккумулятор до chargeItem, ячейка
	// до chargeCell за тик (ТЗ 3.3). В буфере мало — поровну по запросам. Вернёт отданное
	protected float ChargeStationItems(VB_EB_Device st, float dtTicks)
	{
		VB_EB_Rating r = st.VB_Rating();
		GameInventory inv = st.GetInventory();
		m_Items.Clear();
		m_Amounts.Clear();
		float wantSum = 0;
		for (int a = 0; a < inv.GetAttachmentSlotsCount(); a++)
		{
			int slotId = inv.GetAttachmentSlotId(a);
			if (!VB_EB_Device.VB_IsChargeSlot(slotId))
				continue;
			EntityAI it = inv.FindAttachment(slotId);
			if (!it)
				continue;
			float rate = r.chargeItem;
			if (VB_EB_Device.VB_IsChargeCellSlot(slotId))
				rate = r.chargeCell;
			float w = Math.Min(rate * dtTicks, VB_Echo.SpaceE(it));
			if (w <= 0.00001)
				continue;
			m_Items.Insert(it);
			m_Amounts.Insert(w);
			wantSum += w;
		}
		if (wantSum <= 0.00001)
			return 0;
		float k = Math.Min(1, VB_Echo.GetE(st) / wantSum);
		float given = 0;
		for (int i = 0; i < m_Items.Count(); i++)
		{
			float taken = VB_Echo.DrainE(st, m_Amounts[i] * k);
			float put = VB_Echo.AddE(m_Items[i], taken);
			if (taken - put > 0.00001)
				VB_Echo.AddE(st, taken - put);
			given += put;
		}
		return given;
	}

	// станция и накопитель: ячейки в гнёздах разрядки отдают энергию в запас сети (буфер преобразователя, накопители с
	// их пределом входа): каждая до perItem, все вместе до total kE за тик. Что запас не принял — обратно в ячейку.
	// Синтез не переливается (CanTransfer)
	protected float DrainCells(VB_EB_Converter conv, VB_EB_Device dev, float perItem, float total)
	{
		if (!dev || total <= 0)
			return 0;
		m_Items.Clear();
		dev.VB_SlotItems(true, m_Items);
		float taken = 0;
		foreach (EntityAI it : m_Items)
		{
			float left = total - taken;
			if (left <= 0.00001)
				break;
			if (!VB_Echo.CanTransfer(it))
				continue;
			float space = PoolSpace(conv);
			if (space <= 0.00001)
				break;
			float haveE = VB_Echo.GetE(it);
			// Math.Min дважды в аргументах одного выражения затирает результат (E71) — по шагам
			float want = Math.Min(left, perItem);
			want = Math.Min(want, space);
			want = Math.Min(want, haveE);
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

	// платформа: из своего буфера — вложениям машин и живых игроков в прямоугольнике над ней (заряжается только
	// установленное — баки и аккумулятор машины, надетое игроком, не рюкзаки; решение 11-20). Отдаёт до outMax за
	// тик, из буфера уходит в inMax/outMax раз больше (Mk I: 256 → 128, потери 50 %). Вернёт отданное
	protected float ChargePadFromBuffer(VB_EB_Device pad, float dtTicks)
	{
		VB_EB_Rating r = pad.VB_Rating();
		if (r.outMax <= 0 || r.inMax <= 0 || VB_Echo.Get(pad) <= 0)
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
				given += GiveFromBuffer(pad, inv.GetAttachmentFromIndex(ai), budget - given, eff);
			}
		}
		return given;
	}

	// отдать носителю до budget kE из буфера Эхо устройства; eff — доля, что доходит (из буфера уходит give / eff).
	// Не носитель, бесконечный, разрушенный, полный — ничего (VB_Echo.SpaceE = 0). Вернёт отданное
	protected float GiveFromBuffer(VB_EB_Device dev, EntityAI it, float budget, float eff)
	{
		if (!it || budget <= 0)
			return 0;
		float give = Math.Min(VB_Echo.SpaceE(it), budget);
		float have = VB_Echo.GetE(dev);
		if (give / eff > have)
			give = have * eff;
		if (give <= 0.00001)
			return 0;
		float put = VB_Echo.AddE(it, give);
		VB_Echo.DrainE(dev, put / eff);
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

	// сколько можно взять в этом тике (с учётом пределов выхода накопителей)
	protected float PoolAvail(VB_EB_Converter conv)
	{
		float e = VB_Echo.Get(conv);
		foreach (VB_EB_Device st : conv.m_VB_Storages)
		{
			float have = VB_Echo.Get(st);
			e += Math.Min(have, st.m_VB_IoOutLeft);
		}
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
