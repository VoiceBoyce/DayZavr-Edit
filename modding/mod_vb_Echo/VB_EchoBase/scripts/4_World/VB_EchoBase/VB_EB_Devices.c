// Устройства сети базы. Своих полей в сохранении нет (урок DupeCheck): заряд буфера и накопителей, износ
// кристалла — количество предмета (ваниль); сеть пересчитывается заново каждый тик (VB_EB_Network).
// Единицы: kE и kE/tick (тик = 10 с). Вход/выход — VB_EB_Rating (конфиг class VB_EchoRating + настройки).

class VB_EBKind
{
	static const int NONE = 0;
	static const int WIND = 1;
	static const int SOLAR = 2;
	static const int FURNACE = 3;
	static const int CONVERTER = 4;
	static const int STORAGE = 5;
	static const int STATION = 6;
	static const int PAD = 7;
	static const int RELAY = 8;
};

// состояние для вида (неон своих моделей): синхронизируется на клиент
class VB_EBState
{
	static const int NONE = 0;		// нет энергии / не в сети — серый
	static const int IDLE = 1;		// в сети, энергия есть, сейчас не работает — тлеет
	static const int WORK = 2;		// работает — горит
};

class VB_EB_Device extends ItemBase
{
	protected int m_VB_State;
	VB_EB_Converter m_VB_Net;		// сервер: преобразователь, к сети которого устройство подключено в последнем тике
	float m_VB_LastOut;				// сервер: выработка/выдача в последнем тике, kE/tick (для «Состояние сети»)
	float m_VB_LastIn;				// сервер: сколько взято из сети в последнем тике, kE/tick
	float m_VB_LastDrain;			// сервер: сколько отдали в сеть ячейки в гнёздах разрядки, kE/tick
	float m_VB_IoInLeft;			// сервер, накопитель: сколько ещё можно принять в этом тике, kE
	float m_VB_IoOutLeft;			// сервер, накопитель: сколько ещё можно отдать в этом тике, kE

	void VB_EB_Device()
	{
		RegisterNetSyncVariableInt("m_VB_State", 0, 2);
	}

	int VB_Kind()
	{
		return VB_EBKind.NONE;
	}

	protected VB_EB_Rating m_VB_RatingCached;	// сервер: показатели класса (держит кэш VB_EB_Rating — тут без ref)

	VB_EB_Rating VB_Rating()
	{
		if (!m_VB_RatingCached)
			m_VB_RatingCached = VB_EB_Rating.Of(this, VB_EB_Network.GetSettings());
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

	// стоит на земле (не в руках, не в инвентаре), цело и не голограмма установки — участвует в сети.
	// Голограмма набора на сервере — настоящий объект класса устройства (Hologram.c), его в сеть не брать
	bool VB_IsPlaced()
	{
		return !GetHierarchyParent() && !IsRuined() && !IsHologram();
	}

	// источник тока: неон — цвет уровня (решение 11-20), у остальных — красный Эхо + метка уровня
	bool VB_IsSource()
	{
		int k = VB_Kind();
		return k == VB_EBKind.WIND || k == VB_EBKind.SOLAR || k == VB_EBKind.FURNACE;
	}

	// ---------------------------------------------------------------- неон (клиент)
	static const string TEX_EMPTY = "vb_echobase\\data\\er_neon_empty_co.paa";
	static const string TEX_ECHO = "vb_echobase\\data\\er_neon_co.paa";
	static const string TEX_ECHO_HOT = "vb_echobase\\data\\er_neon_hot_co.paa";	// горящий — чистый красный (E116)
	static const string TEX_FUSION = "vb_echobase\\data\\er_neon_fusion_co.paa";			// холодный синтез — бирюзовый
	static const string TEX_FUSION_HOT = "vb_echobase\\data\\er_neon_fusion_hot_co.paa";
	static const string MAT_EMPTY = "vb_echobase\\data\\er_neon_empty.rvmat";
	static const string MAT_IDLE = "vb_echobase\\data\\er_neon_idle.rvmat";
	static const string MAT_ON = "vb_echobase\\data\\er_neon.rvmat";

	static string VB_TierTex(int tier)
	{
		int t = Math.Clamp(tier, 1, 4);
		return "vb_echobase\\data\\eb_neon_t" + t + "_co.paa";
	}

	// «neon»: нет энергии — серый; есть — красный (источник — цвет уровня), тлеет или горит; «tier» — метка уровня
	void VB_Paint()
	{
		if (IsHologram())
			return;				// голограмму установки красит ваниль (можно / нельзя поставить)
		int tier = VB_EB_Rating.Of(this, null).tier;		// клиент: только конфиг (настройки сервера тут нет)
		string tierTex = VB_TierTex(tier);
		bool on = m_VB_State != VB_EBState.NONE;
		bool hot = m_VB_State == VB_EBState.WORK;
		int idx = GetHiddenSelectionIndex("neon");
		if (idx >= 0)
		{
			if (!on)
			{
				SetObjectTexture(idx, TEX_EMPTY);
				SetObjectMaterial(idx, MAT_EMPTY);
			}
			else
			{
				string tex = TEX_ECHO;
				if (VB_IsSource())
					tex = tierTex;
				else if (hot)
					tex = TEX_ECHO_HOT;
				SetObjectTexture(idx, tex);
				if (hot)
					SetObjectMaterial(idx, MAT_ON);
				else
					SetObjectMaterial(idx, MAT_IDLE);
			}
		}
		int tidx = GetHiddenSelectionIndex("tier");
		if (tidx >= 0)
		{
			if (on)
			{
				SetObjectTexture(tidx, tierTex);
				SetObjectMaterial(tidx, MAT_IDLE);
			}
			else
			{
				SetObjectTexture(tidx, TEX_EMPTY);
				SetObjectMaterial(tidx, MAT_EMPTY);
			}
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_Paint();
	}

	// клиент, каждый кадр (VB_EB_RotorSpinner): анимация — ротор ветряка, кристалл преобразователя
	void VB_Spin(float dt)
	{
	}

	int VB_GetState()
	{
		return m_VB_State;
	}

	// сервер
	void VB_SetState(int s)
	{
		if (s == m_VB_State)
			return;
		m_VB_State = s;
		SetSynchDirty();
	}

	// сервер: выработка тока источником сейчас, kE/tick (ветер, солнце; у печи — есть ли горящая порция)
	float VB_SourceOutput(VB_EB_Settings s, float dtTicks)
	{
		return 0;
	}

	// ---------------------------------------------------------------- буферы (ТЗ_VB_EchoBase_буферы.md, 11-20)
	// Ток — энергия ванильного EnergyManager (источники и преобразователь), Эхо — количество предмета (VB_Echo:
	// преобразователь, накопитель, станция, платформа). Ваниль сохраняет и шлёт клиенту оба; своих полей нет.
	bool VB_HasCurBuf()
	{
		return GetCompEM() != null;
	}

	// всё через локальные: GetCompEM() второй раз внутри аргументов того же выражения затирает первый результат —
	// «NULL pointer» в VB_CurAdd на стенде 11-20 (E71)
	float VB_CurGet()
	{
		ComponentEnergyManager em = GetCompEM();
		if (!em)
			return 0;
		return em.GetEnergy();
	}

	float VB_CurMax()
	{
		ComponentEnergyManager em = GetCompEM();
		if (!em)
			return 0;
		return em.GetEnergyMax();
	}

	float VB_CurSpace()
	{
		ComponentEnergyManager em = GetCompEM();
		if (!em)
			return 0;
		float cur = em.GetEnergy();
		float max = em.GetEnergyMax();
		return Math.Max(0, max - cur);
	}

	// сервер: положить до e kE тока, вернуть, сколько влезло (SetEnergy сам клиенту не шлёт — SetSynchDirty)
	float VB_CurAdd(float e)
	{
		ComponentEnergyManager em = GetCompEM();
		if (e <= 0 || !em)
			return 0;
		float cur = em.GetEnergy();
		float max = em.GetEnergyMax();
		float put = Math.Min(e, max - cur);
		if (put <= 0)
			return 0;
		em.SetEnergy(cur + put);
		SetSynchDirty();
		return put;
	}

	// сервер: взять до e kE тока, вернуть, сколько взято
	float VB_CurTake(float e)
	{
		ComponentEnergyManager em = GetCompEM();
		if (e <= 0 || !em)
			return 0;
		float cur = em.GetEnergy();
		float take = Math.Min(e, cur);
		if (take <= 0)
			return 0;
		em.SetEnergy(cur - take);
		SetSynchDirty();
		return take;
	}

	// сервер, источники: выработать ток в свой буфер за dtTicks, вернуть, сколько легло (полный буфер — лишнее
	// пропадает). Ветряк и панель — по погоде, печь переопределяет (жжёт, только пока есть место)
	float VB_Generate(VB_EB_Settings s, float dtTicks)
	{
		float rate = VB_SourceOutput(s, 0);
		return VB_CurAdd(rate * dtTicks);
	}

	// курсор: строка буфера тока (жёлтая, пустой — серый)
	protected void VB_CursorCur(VB_EB_Lines lines, string label)
	{
		if (!VB_HasCurBuf())
			return;
		float cur = VB_CurGet();
		float curMax = VB_CurMax();
		string tCur = VB_EB_Lines.N(cur);
		string tMax = VB_EB_Lines.N(curMax);
		int c = VB_EB_Colors.CURRENT;
		if (cur <= 0)
			c = VB_EB_Colors.DIM;
		lines.Add(label + " " + tCur + "/" + tMax + " kE", c);
	}

	// курсор: строка буфера Эхо (красная, пустой — серый)
	protected void VB_CursorEcho(VB_EB_Lines lines, string label)
	{
		float e = VB_Echo.GetE(this);
		float cap = VB_Echo.Capacity(this);
		float per = VB_Echo.PerE(this);
		float eMax = cap / per;
		string tE = VB_EB_Lines.N(e);
		string tMax = VB_EB_Lines.N(eMax);
		int c = VB_EB_Colors.ECHO;
		if (e <= 0)
			c = VB_EB_Colors.DIM;
		lines.Add(label + " " + tE + "/" + tMax + " kE", c);
	}

	// ---------------------------------------------------------------- курсор действий (VB_EB_Info.c)
	protected ref VB_EB_NetInfo m_VB_Info;		// клиент: последний ответ сервера
	protected int m_VB_InfoAsked = -100000;		// клиент: когда спрашивали, мс
	static const int INFO_EVERY_MS = 1000;
	static const float INFO_MAX_DIST = 10;		// м: сервер отвечает только игроку рядом
	static const int CONTENT_ROWS = 6;			// строк о вложенном, дальше — «…и ещё N»

	// клиент, пока игрок смотрит на устройство: не чаще раза в секунду (сеть считается раз в тик)
	void VB_RequestInfo()
	{
		int now = GetGame().GetTime();
		if (now - m_VB_InfoAsked < INFO_EVERY_MS)
			return;
		m_VB_InfoAsked = now;
		GetGame().RPCSingleParam(this, VB_EB_RPC.INFO_REQUEST, null, true);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == VB_EB_RPC.INFO_REQUEST)
		{
			if (GetGame().IsDedicatedServer())
				VB_SendInfo(sender);
		}
		else if (rpc_type == VB_EB_RPC.INFO)
		{
			if (GetGame().IsDedicatedServer())
				return;
			VB_EB_NetInfo info = new VB_EB_NetInfo;
			if (info.Read(ctx))
				m_VB_Info = info;
		}
	}

	// сервер: одному игроку — не чаще раза в SERVER_INFO_MS по всем устройствам (свой клиент спрашивает раз в секунду,
	// чаще — только подделанный: не даём засыпать сервер запросами)
	static const int SERVER_INFO_MS = 300;
	protected static ref map<string, int> s_VB_InfoSentAt;

	protected void VB_SendInfo(PlayerIdentity sender)
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

	// сервер: своё для курсора (ветер, солнце, топливо); общее заполняет VB_EB_Network.FillInfo
	void VB_FillInfo(VB_EB_Settings s, VB_EB_NetInfo info)
	{
	}

	// клиент: строки под названием — своё, сеть, вложенное
	void VB_CursorLines(VB_EB_Lines lines)
	{
		lines.Clear();
		VB_CursorOwn(m_VB_Info, lines);
		VB_CursorNet(m_VB_Info, lines);
		VB_CursorContents(lines);
	}

	// своё устройство; i — ответ сервера или null (ещё не пришёл)
	protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
	}

	// числа — в локальные (E71: несколько вызовов одной функции в одном выражении затирают друг друга)
	protected void VB_CursorNet(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		if (!i)
			return;
		if (!i.m_InNet)
		{
			lines.Add("Не в сети: нет преобразователя рядом", VB_EB_Colors.WARN);
			return;
		}
		string tProd = VB_EB_Lines.N(i.m_Prod);
		if (i.m_Crystal == 0)
		{
			lines.Add("Сеть: ток " + tProd + " kE/tick, нет кристалла", VB_EB_Colors.WARN);
		}
		else
		{
			string tConv = VB_EB_Lines.N(i.m_Conv);
			string night = "";
			if (i.m_Night)
				night = " (ночь)";
			int cNet = VB_EB_Colors.ECHO;
			if (i.m_Conv <= 0)
				cNet = VB_EB_Colors.DIM;
			lines.Add("Сеть: ток " + tProd + " → Эхо " + tConv + " kE/tick" + night, cNet);
		}
		// общий запас сети (буфер + накопители) — только у преобразователя (решение пользователя 11-20: у каждого
		// механизма свой запас — VB_CursorOwn, общий — в преобразователе)
	}

	// ---------------------------------------------------------------- гнёзда зарядки и разрядки (станция, накопитель)
	// Слоты — в ядре (config.cpp VB_EchoCore): Charge* — устройство заряжает вложенное, Drain* — ячейка отдаёт
	// энергию в запас сети (станция v2 и накопитель, решения пользователя 11-20)
	// номера слотов — один раз (поиск по имени — строковый, а спрашивают каждый тик и каждый кадр курсора)
	protected static bool s_VB_SlotsReady;
	protected static int s_VB_SlotDrain1;
	protected static int s_VB_SlotDrain2;
	protected static int s_VB_SlotCell1;
	protected static int s_VB_SlotCell2;
	protected static int s_VB_SlotTank1;
	protected static int s_VB_SlotTank2;
	protected static int s_VB_SlotBattery;

	protected static void VB_InitSlots()
	{
		if (s_VB_SlotsReady)
			return;
		s_VB_SlotsReady = true;
		s_VB_SlotDrain1 = InventorySlots.GetSlotIdFromString("VB_Echo_DrainCell1");
		s_VB_SlotDrain2 = InventorySlots.GetSlotIdFromString("VB_Echo_DrainCell2");
		s_VB_SlotCell1 = InventorySlots.GetSlotIdFromString("VB_Echo_ChargeCell1");
		s_VB_SlotCell2 = InventorySlots.GetSlotIdFromString("VB_Echo_ChargeCell2");
		s_VB_SlotTank1 = InventorySlots.GetSlotIdFromString("VB_Echo_ChargeTank1");
		s_VB_SlotTank2 = InventorySlots.GetSlotIdFromString("VB_Echo_ChargeTank2");
		s_VB_SlotBattery = InventorySlots.GetSlotIdFromString("VB_Echo_ChargeBattery");
	}

	static bool VB_IsDrainSlot(int slotId)
	{
		VB_InitSlots();
		return slotId == s_VB_SlotDrain1 || slotId == s_VB_SlotDrain2;
	}

	static bool VB_IsChargeCellSlot(int slotId)
	{
		VB_InitSlots();
		return slotId == s_VB_SlotCell1 || slotId == s_VB_SlotCell2;
	}

	static bool VB_IsChargeSlot(int slotId)
	{
		if (VB_IsChargeCellSlot(slotId))
			return true;
		return slotId == s_VB_SlotTank1 || slotId == s_VB_SlotTank2 || slotId == s_VB_SlotBattery;
	}

	// вложенное в гнёздах разрядки (drain = true) или зарядки (false), в порядке гнёзд
	void VB_SlotItems(bool drain, array<EntityAI> items)
	{
		GameInventory inv = GetInventory();
		for (int i = 0; i < inv.GetAttachmentSlotsCount(); i++)
		{
			int slotId = inv.GetAttachmentSlotId(i);
			bool isDrain = VB_IsDrainSlot(slotId);
			if (isDrain != drain || (!isDrain && !VB_IsChargeSlot(slotId)))
				continue;
			EntityAI e = inv.FindAttachment(slotId);
			if (e)
				items.Insert(e);
		}
	}

	// в гнёзда зарядки и разрядки — только переливаемые носители Эхо (синтез — нет). Гнездо разрядки — только с зарядом,
	// гнездо зарядки ячейки — только неполная: полная ячейка идёт лишь в разрядку, пустая — лишь в зарядку, начатая —
	// в первое свободное (движок берёт первое гнездо, куда можно). Свои слоты (кристалл преобразователя) — как в ванили
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		bool drain = VB_IsDrainSlot(slotId);
		if (!drain && !VB_IsChargeSlot(slotId))
			return true;
		if (!VB_Echo.CanTransfer(attachment))
			return false;
		float frac = VB_Echo.Fraction(attachment);
		if (drain)
			return frac > 0;
		if (VB_IsChargeCellSlot(slotId))
			return frac < 1;
		return true;
	}

	// вложения (кристалл; в гнёздах: ↓ — заряжается, ↑ — отдаёт в сеть; ▲▼ в шрифте курсора нет) и отсек (топливо)
	// — клиент знает их сам
	protected void VB_CursorContents(VB_EB_Lines lines)
	{
		GameInventory inv = GetInventory();
		if (!inv)
			return;
		int total = 0;
		for (int a = 0; a < inv.GetAttachmentSlotsCount(); a++)
		{
			int slotId = inv.GetAttachmentSlotId(a);
			EntityAI att = inv.FindAttachment(slotId);
			if (!att)
				continue;
			total++;
			if (total > CONTENT_ROWS)
				continue;
			string mark = "• ";
			if (VB_IsChargeSlot(slotId))
				mark = "↓ ";
			else if (VB_IsDrainSlot(slotId))
				mark = "↑ ";
			VB_ItemLine(att, lines, mark);
		}
		CargoBase cargo = inv.GetCargo();
		if (cargo)
		{
			for (int c = 0; c < cargo.GetItemCount(); c++)
			{
				total++;
				if (total <= CONTENT_ROWS)
					VB_ItemLine(cargo.GetItem(c), lines);
			}
		}
		if (total > CONTENT_ROWS)
		{
			int more = total - CONTENT_ROWS;
			lines.Add("…и ещё " + more, VB_EB_Colors.DIM);
		}
	}

	static void VB_ItemLine(EntityAI e, VB_EB_Lines lines, string mark = "• ")
	{
		if (!e)
			return;
		string name = e.GetDisplayName();
		int color;
		if (VB_Echo.IsCarrier(e))
		{
			color = VB_EB_Colors.ECHO;
			if (VB_Echo.IsFusion(e))
				color = VB_EB_Colors.FUSION;
			if (VB_Echo.IsInfinite(e))
			{
				lines.Add(mark + name + " — ∞", color);
				return;
			}
			// в kE: у аккумулятора единица заряда — энергия (2000 = 512 kE), а не kE
			float now = VB_Echo.GetE(e);
			float cap = VB_Echo.Capacity(e) / VB_Echo.PerE(e);
			if (now <= 0)
				color = VB_EB_Colors.DIM;
			string tNow = VB_EB_Lines.N(now);
			string tCap = VB_EB_Lines.N(cap);
			lines.Add(mark + name + " — " + tNow + "/" + tCap + " kE", color);
			return;
		}
		if (e.IsKindOf("VB_EB_CrystalFusion"))
		{
			lines.Add(mark + name + " — ∞", VB_EB_Colors.FUSION);
			return;
		}
		ItemBase ib = ItemBase.Cast(e);
		if (ib && e.IsKindOf("VB_EB_Crystal"))
		{
			float q = ib.GetQuantity();
			float qMax = ib.GetQuantityMax();
			color = VB_EB_Colors.ECHO;
			if (q <= 0)
				color = VB_EB_Colors.DIM;
			string tQ = VB_EB_Lines.N(q);
			string tQMax = VB_EB_Lines.N(qMax);
			lines.Add(mark + name + " — " + tQ + "/" + tQMax + " kE", color);
			return;
		}
		if (ib && ib.HasQuantity() && ib.GetQuantityMax() > 1 && ib.ConfigGetBool("canBeSplit"))
		{
			int count = Math.Round(ib.GetQuantity());
			lines.Add(mark + name + " ×" + count, VB_EB_Colors.TEXT);
			return;
		}
		lines.Add(mark + name, VB_EB_Colors.TEXT);
	}

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsDedicatedServer())
		{
			VB_EB_Network.Register(this);
			return;
		}
		// клиент: сразу и ещё раз чуть позже — движок может наложить текстуры конфига поверх (как у Echo Runner)
		VB_Paint();
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_Paint, 300, false);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (GetGame().IsDedicatedServer())
			VB_EB_Network.Unregister(this);
		else
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_Paint);
	}

	// ставится только из набора; в руки и рюкзак не берётся, обратно — «Разобрать» в набор
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	// курсор действий (название и строки сети) и у того, что нельзя взять в руки — как у забора (Fence.c);
	// без этого ваниль прячет курсор, пока у устройства нет доступного действия
	override bool IsActionTargetVisible()
	{
		return true;
	}

	// класс набора этого устройства (конфиг vbKit)
	string VB_KitType()
	{
		return ConfigGetString("vbKit");
	}

	// разобрать можно только пустое: без кристалла и без вещей в отсеке (топливо, баки)
	bool VB_IsEmpty()
	{
		if (GetInventory().AttachmentCount() > 0)
			return false;
		CargoBase cargo = GetInventory().GetCargo();
		return !cargo || cargo.GetItemCount() == 0;
	}

	override void SetActions()
	{
		super.SetActions();
		RemoveAction(ActionTakeItem);
		RemoveAction(ActionTakeItemToHands);
		AddAction(ActionVB_EBDisassemble);
	}
};

// ------------------------------------------------------------------ источники тока
class VB_EB_WindTurbine extends VB_EB_Device
{
	protected int m_VB_Speed;			// 0..20, синхр.: скорость ротора по ветру (крутится и вне сети — это ветер)
	protected float m_VB_RotorPhase;	// клиент: фаза анимации rotor (0..1 = оборот)

	void VB_EB_WindTurbine()
	{
		RegisterNetSyncVariableInt("m_VB_Speed", 0, 20);
	}

	override int VB_Kind()
	{
		return VB_EBKind.WIND;
	}

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

	// клиент: ротор крутит общий покадровый обработчик (VB_EB_RotorSpinner). EOnFrame у поставленного предмета
	// не вызывается даже с SetEventMask(FRAME) — на стенде 11-20 ротор стоял при ветре и жёлтом неоне
	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			VB_EB_RotorSpinner.Add(this);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			VB_EB_RotorSpinner.Remove(this);
	}

	// клиент, каждый кадр: до ~0,7 оборота в секунду при сильном ветре
	override void VB_Spin(float dt)
	{
		if (m_VB_Speed <= 0 || IsHologram() || GetHierarchyParent())
			return;
		m_VB_RotorPhase += dt * (0.08 + 0.6 * m_VB_Speed / 20.0);
		if (m_VB_RotorPhase >= 1)
			m_VB_RotorPhase -= Math.Floor(m_VB_RotorPhase);
		SetAnimationPhaseNow("rotor", m_VB_RotorPhase);
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

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		if (i && i.m_Covered)
		{
			lines.Add("Крыша над ветряком — не крутится", VB_EB_Colors.WARN);
		}
		else if (i)
		{
			string tWind = VB_EB_Lines.N(i.m_Wind);
			string tOut = VB_EB_Lines.N(i.m_Out);
			string tMax = VB_EB_Lines.N(i.m_OutMax);
			int c = VB_EB_Colors.CURRENT;
			if (i.m_Out <= 0)
				c = VB_EB_Colors.DIM;
			lines.Add("Ветер " + tWind + " м/с · ток " + tOut + " из " + tMax + " kE/tick", c);
		}
		VB_CursorCur(lines, "Буфер тока");
	}
};

class VB_EB_SolarPanel extends VB_EB_Device
{
	override int VB_Kind()
	{
		return VB_EBKind.SOLAR;
	}

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

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		VB_CursorSolar(i, lines);
		VB_CursorCur(lines, "Буфер тока");
	}

	protected void VB_CursorSolar(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		if (!i)
			return;
		if (i.m_Covered)
		{
			lines.Add("Крыша над панелью — нет тока", VB_EB_Colors.WARN);
			return;
		}
		if (i.m_Sun <= 0)
		{
			lines.Add("Ночь — панель не даёт ток", VB_EB_Colors.DIM);
			return;
		}
		float sunF = i.m_Sun * 100;
		float cloudF = i.m_Cloud * 100;
		int sun = Math.Round(sunF);
		int cloud = Math.Round(cloudF);
		lines.Add("Солнце " + sun + "% · облака " + cloud + "%", VB_EB_Colors.TEXT);
		string tOut = VB_EB_Lines.N(i.m_Out);
		string tMax = VB_EB_Lines.N(i.m_OutMax);
		int c = VB_EB_Colors.CURRENT;
		if (i.m_Out <= 0)
			c = VB_EB_Colors.DIM;
		lines.Add("Ток " + tOut + " из " + tMax + " kE/tick", c);
	}
};

class VB_EB_Furnace extends VB_EB_Device
{
	protected string m_VB_BurnType;		// сервер: что горит (для курсора; после рестарта пусто — порция догорит молча)

	override int VB_Kind()
	{
		return VB_EBKind.FURNACE;
	}

	// kE, что осталось в горящей порции, — количество предмета: ваниль сохраняет его, порция переживает рестарт
	// (раньше было поле в памяти — после рестарта недогоревшее пропадало)
	float VB_FuelLeft()
	{
		return GetQuantity();
	}

	protected void VB_SetFuelLeft(float v)
	{
		SetQuantity(Math.Max(0, v), false);
	}

	// в отсек — только топливо из списка конфига vbFuel (точный класс, как ищет настройки VB_TakeFuel). Загрузку из
	// сохранения не трогаем (CanLoadItemIntoCargo): что уже лежит — не пропадёт
	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;
		return item && VB_IsFuelType(item.GetType());
	}

	override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
	{
		if (!super.CanSwapItemInCargo(child_entity, new_entity))
			return false;
		return new_entity && VB_IsFuelType(new_entity.GetType());
	}

	protected static ref TStringArray s_VB_FuelTypes;

	static bool VB_IsFuelType(string type)
	{
		if (!s_VB_FuelTypes)
		{
			s_VB_FuelTypes = new TStringArray;
			GetGame().ConfigGetTextArray("CfgVehicles VB_EB_Furnace vbFuel", s_VB_FuelTypes);
		}
		return s_VB_FuelTypes.Find(type) >= 0;
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

	// m_Out — сколько печь выработала в последнем тике: жжёт, только пока в её буфере тока есть место
	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		if (i)
		{
			if (i.m_Out > 0)
			{
				if (i.m_BurnType != "")
				{
					string burn = VB_EB_Lines.ClassName(i.m_BurnType);
					lines.Add("Горит: " + burn, VB_EB_Colors.CURRENT);
				}
				string tOut = VB_EB_Lines.N(i.m_Out);
				lines.Add("Ток " + tOut + " kE/tick", VB_EB_Colors.CURRENT);
			}
			else if (i.m_FuelLeft <= 0 && !i.m_HasFuel)
			{
				lines.Add("Не горит: положи топливо в отсек", VB_EB_Colors.WARN);
			}
			else if (VB_CurSpace() <= 0)
			{
				lines.Add("Не горит: буфер тока полон", VB_EB_Colors.DIM);
			}
		}
		VB_CursorCur(lines, "Буфер тока");
		if (i && i.m_FuelStock > 0)
		{
			string tStock = VB_EB_Lines.N(i.m_FuelStock);
			lines.Add("Запас топлива " + tStock + " kE", VB_EB_Colors.TEXT);
		}
	}
};

// ------------------------------------------------------------------ ядро сети
class VB_EB_Converter extends VB_EB_Device
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

	override int VB_Kind()
	{
		return VB_EBKind.CONVERTER;
	}

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

	EntityAI VB_Crystal()
	{
		return FindAttachmentBySlotName("VB_EB_Crystal");
	}

	// ---------------- кристалл в колбе (клиент, решение пользователя 11-20): виден только вставленный, его цвет
	// (Эхо красный, синтез бирюзовый, разрушенный серый), чистый цвет, когда преобразователь делает Эхо; крутится
	// вокруг своей оси — быстрее в работе. Кость crystal, анимации crystal_hide / crystal_spin (model.cfg)
	protected bool m_VB_CrystalShown;
	protected float m_VB_CrystalPhase;

	override void VB_Paint()
	{
		super.VB_Paint();
		VB_PaintCrystal();
	}

	protected void VB_PaintCrystal()
	{
		EntityAI c = VB_Crystal();
		m_VB_CrystalShown = c && !IsHologram();
		if (!m_VB_CrystalShown)
		{
			SetAnimationPhaseNow("crystal_hide", 1);
			return;
		}
		SetAnimationPhaseNow("crystal_hide", 0);
		int idx = GetHiddenSelectionIndex("crystal_glow");
		if (idx < 0)
			return;
		// материал всегда «тлеющий»: у горящего (er_neon, свечение 22) грани сливаются в ровное пятно и вращения не
		// видно (стенд 11-20); работу показывает чистый цвет (hot) и скорость вращения
		bool hot = m_VB_State == VB_EBState.WORK;
		string tex;
		string mat = MAT_IDLE;
		if (c.IsRuined())
		{
			tex = TEX_EMPTY;
			mat = MAT_EMPTY;
		}
		else if (c.IsKindOf("VB_EB_CrystalFusion"))
		{
			tex = TEX_FUSION;
			if (hot)
				tex = TEX_FUSION_HOT;
		}
		else
		{
			tex = TEX_ECHO;
			if (hot)
				tex = TEX_ECHO_HOT;
		}
		SetObjectTexture(idx, tex);
		SetObjectMaterial(idx, mat);
	}

	// вставили или вынули кристалл (и при подгрузке объекта): перекрасить чуть позже — в момент снятия вложение
	// ещё числится в слоте
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		VB_PaintCrystalSoon();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		VB_PaintCrystalSoon();
	}

	protected void VB_PaintCrystalSoon()
	{
		if (GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_Paint);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_Paint, 100, false);
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			VB_EB_RotorSpinner.Add(this);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			VB_EB_RotorSpinner.Remove(this);
	}

	// об/с: делает Эхо — 0,3; в сети, но ждёт — 0,12; иначе едва поворачивается
	override void VB_Spin(float dt)
	{
		if (!m_VB_CrystalShown || IsHologram() || GetHierarchyParent())
			return;
		float speed = 0.05;
		if (m_VB_State == VB_EBState.WORK)
			speed = 0.3;
		else if (m_VB_State == VB_EBState.IDLE)
			speed = 0.12;
		m_VB_CrystalPhase += dt * speed;
		if (m_VB_CrystalPhase >= 1)
			m_VB_CrystalPhase -= Math.Floor(m_VB_CrystalPhase);
		SetAnimationPhaseNow("crystal_spin", m_VB_CrystalPhase);
	}

	bool VB_HasFusionCrystal()
	{
		EntityAI c = VB_Crystal();
		return c && !c.IsRuined() && c.IsKindOf("VB_EB_CrystalFusion");
	}

	// кристалл вставлен и не выработан
	bool VB_CrystalReady()
	{
		EntityAI c = VB_Crystal();
		if (!c || c.IsRuined())
			return false;
		if (c.IsKindOf("VB_EB_CrystalFusion"))
			return true;
		ItemBase ib = ItemBase.Cast(c);
		return ib && ib.GetQuantity() > 0;
	}

	// свои буферы (ток — EnergyManager, Эхо — количество) клиент знает сам; состав сети — из ответа сервера
	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		VB_CursorCur(lines, "Ток");
		VB_CursorEcho(lines, "Эхо");
		if (!i)
			return;
		// общий запас сети — только здесь (решение пользователя 11-20)
		string tPool = VB_EB_Lines.N(i.m_Pool);
		string tPoolMax = VB_EB_Lines.N(i.m_PoolMax);
		string tUse = VB_EB_Lines.N(i.m_Use);
		int cPool = VB_EB_Colors.ECHO;
		if (i.m_Pool <= 0)
			cPool = VB_EB_Colors.DIM;
		lines.Add("Запас сети " + tPool + "/" + tPoolMax + " kE", cPool);
		lines.Add("Расход станций и платформ " + tUse + " kE/tick", VB_EB_Colors.TEXT);
		lines.Add("Источников " + i.m_Sources + " · накопителей " + i.m_Storages, VB_EB_Colors.TEXT);
		lines.Add("Станций " + i.m_Stations + " · платформ " + i.m_Pads + " · ретр. " + i.m_Relays, VB_EB_Colors.TEXT);
		string tRange = VB_EB_Lines.N(i.m_Range);
		lines.Add("Радиус узла " + tRange + " м", VB_EB_Colors.DIM);
		if (i.m_Skipped > 0)
			lines.Add("Сверх лимита не подключено: " + i.m_Skipped, VB_EB_Colors.WARN);
	}
};

// ------------------------------------------------------------------ хранение и выдача
class VB_EB_Storage extends VB_EB_Device
{
	static const int LEVELS = 12;		// секций столба в колбах: выделения lvl0..lvl11 (eb_models.py STORAGE_LEVELS)
	static const int LAMPS = 8;			// ламп шкалы спереди: lamp0..lamp7

	override int VB_Kind()
	{
		return VB_EBKind.STORAGE;
	}

	// уровень заряда виден (решение пользователя 11-20): колбы заполняются вместе, как сообщающиеся сосуды,
	// и лампы шкалы — заполненное красное, пустое серое. Заряд = количество предмета, клиент знает его сам и
	// перекрашивает при каждой синхронизации (OnVariablesSynchronized базового класса)
	override void VB_Paint()
	{
		super.VB_Paint();
		if (IsHologram())
			return;
		float frac = VB_Echo.Fraction(this);
		VB_PaintRow("lvl", LEVELS, frac);
		VB_PaintRow("lamp", LAMPS, frac);
	}

	// count выделений prefix0..prefix<count-1>: горят первые ceil(frac × count) (любой заряд — хотя бы одно)
	protected void VB_PaintRow(string prefix, int count, float frac)
	{
		float litF = Math.Ceil(frac * count - 0.0001);
		int lit = litF;
		for (int k = 0; k < count; k++)
		{
			int idx = GetHiddenSelectionIndex(prefix + k);
			if (idx < 0)
				continue;
			if (k < lit)
			{
				SetObjectTexture(idx, TEX_ECHO);
				SetObjectMaterial(idx, MAT_IDLE);
			}
			else
			{
				SetObjectTexture(idx, TEX_EMPTY);
				SetObjectMaterial(idx, MAT_EMPTY);
			}
		}
	}

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		float now = VB_Echo.Get(this);
		float cap = VB_Echo.Capacity(this);
		string tNow = VB_EB_Lines.N(now);
		string tCap = VB_EB_Lines.N(cap);
		int c = VB_EB_Colors.ECHO;
		if (now <= 0)
			c = VB_EB_Colors.DIM;
		lines.Add("Заряд " + tNow + "/" + tCap + " kE", c);
		if (!i || !i.m_InNet)
			return;
		string tIn = VB_EB_Lines.N(i.m_In);
		string tOut = VB_EB_Lines.N(i.m_Out);
		string tIo = VB_EB_Lines.N(i.m_Io);
		lines.Add("Принял " + tIn + " · отдал " + tOut + " kE/tick (до " + tIo + ")", VB_EB_Colors.TEXT);
		if (i.m_Drain > 0)
		{
			string tDrain = VB_EB_Lines.N(i.m_Drain);
			lines.Add("Из ячеек в сеть " + tDrain + " kE/tick", VB_EB_Colors.ECHO);
		}
	}
};

class VB_EB_ChargeStation extends VB_EB_Device
{
	override int VB_Kind()
	{
		return VB_EBKind.STATION;
	}

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		VB_CursorEcho(lines, "Буфер Эхо");
		if (!i)
			return;
		bool busy = false;
		if (i.m_Out > 0)
		{
			string tGive = VB_EB_Lines.N(i.m_Out);
			lines.Add("Заряжает " + tGive + " kE/tick", VB_EB_Colors.ECHO);
			busy = true;
		}
		if (i.m_Drain > 0)
		{
			string tDrain = VB_EB_Lines.N(i.m_Drain);
			lines.Add("Отдаёт в сеть " + tDrain + " kE/tick из ячеек", VB_EB_Colors.ECHO);
			busy = true;
		}
		if (busy)
			return;
		if (GetInventory().AttachmentCount() == 0)
			lines.Add("Повесь бак, аккумулятор или ячейку", VB_EB_Colors.DIM);
		else if (VB_Echo.Get(this) <= 0 && (!i.m_InNet || i.m_Pool <= 0))
			lines.Add("Ждёт: нет энергии в буфере и в сети", VB_EB_Colors.DIM);
		else
			lines.Add("Всё заряжено", VB_EB_Colors.TEXT);
	}
};

class VB_EB_ChargePad extends VB_EB_Device
{
	protected Object m_VB_Clutter;		// сервер: вырезает траву под плитой (плита 0,16 м — трава её прячет), как у грядки

	override int VB_Kind()
	{
		return VB_EBKind.PAD;
	}

	override void EEInit()
	{
		super.EEInit();
		// позже: голограмма установки на сервере — тоже платформа (флаг ставится после создания), а набор
		// переставляет устройство уже после CreateObjectEx
		if (GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_MakeClutter, 1000, false);
	}

	void VB_MakeClutter()
	{
		if (m_VB_Clutter || IsHologram() || GetHierarchyParent())
			return;
		m_VB_Clutter = GetGame().CreateObjectEx("ClutterCutter6x6", GetPosition(), ECE_PLACE_ON_SURFACE);
		if (m_VB_Clutter)
			m_VB_Clutter.SetOrientation(GetOrientation());
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_MakeClutter);
		if (m_VB_Clutter)
		{
			GetGame().ObjectDelete(m_VB_Clutter);
			m_VB_Clutter = null;
		}
	}

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		VB_CursorEcho(lines, "Буфер Эхо");
		if (!i)
			return;
		if (i.m_Out > 0)
		{
			string tOut = VB_EB_Lines.N(i.m_Out);
			lines.Add("Отдаёт " + tOut + " kE/tick (потери 50 %)", VB_EB_Colors.ECHO);
			return;
		}
		string tInMax = VB_EB_Lines.N(i.m_InMax);
		string tOutMax = VB_EB_Lines.N(i.m_OutMax);
		lines.Add("Встань или заедь: " + tInMax + " → " + tOutMax + " kE/tick", VB_EB_Colors.DIM);
	}
};

class VB_EB_Relay extends VB_EB_Device
{
	override int VB_Kind()
	{
		return VB_EBKind.RELAY;
	}

	override protected void VB_CursorOwn(VB_EB_NetInfo i, VB_EB_Lines lines)
	{
		if (!i)
			return;
		string tRange = VB_EB_Lines.N(i.m_Range);
		lines.Add("Радиус узла " + tRange + " м", VB_EB_Colors.DIM);
	}
};

// ------------------------------------------------------------------ расходник преобразователя
class VB_EB_Crystal extends ItemBase {};

// ------------------------------------------------------------------ клиент: вращение (ротор ветряка, кристалл преобразователя)
// Один обработчик на все устройства в покадровой очереди игры (как SoftSkillsManager.OnUpdate ванили; EOnFrame у
// поставленного предмета не вызывается, E120); дальние (дальше SPIN_DIST от камеры) не крутятся — их не видно
class VB_EB_RotorSpinner
{
	static const float SPIN_DIST = 300;
	protected static ref VB_EB_RotorSpinner s_Inst;
	protected ref array<VB_EB_Device> m_List = new array<VB_EB_Device>;
	protected float m_Last = -1;

	static void Add(VB_EB_Device t)
	{
		if (!t)
			return;
		if (!s_Inst)
		{
			s_Inst = new VB_EB_RotorSpinner();
			GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(s_Inst.Update);
		}
		if (s_Inst.m_List.Find(t) < 0)
			s_Inst.m_List.Insert(t);
	}

	static void Remove(VB_EB_Device t)
	{
		if (s_Inst)
			s_Inst.m_List.RemoveItem(t);
	}

	void Update()
	{
		float now = GetGame().GetTickTime();
		float dt = now - m_Last;
		bool first = m_Last < 0;
		m_Last = now;
		if (first || dt <= 0 || dt > 0.5)		// первый кадр, пауза, загрузка — без рывка
			return;
		vector cam = GetGame().GetCurrentCameraPosition();
		float maxSq = SPIN_DIST * SPIN_DIST;
		for (int i = m_List.Count() - 1; i >= 0; i--)
		{
			VB_EB_Device t = m_List[i];
			if (!t)
			{
				m_List.Remove(i);
				continue;
			}
			if (vector.DistanceSq(cam, t.GetPosition()) > maxSq)
				continue;
			t.VB_Spin(dt);
		}
	}
};
class VB_EB_CrystalFusion extends VB_EB_Crystal {};

// ------------------------------------------------------------------ набор (кит)
// Одна модель кейса на все устройства (как кит станка DRL, решение 11-20). Как ванильный набор забора (FenceKit):
// IsBasebuildingKit — ваниль не создаёт объект голограммы сама (Hologram.PlaceEntity), не роняет набор при установке и
// удаляет его по завершении (ActionDeployObject.OnEndServer); устройство ставит OnPlacementComplete на сервере.
// Голограмма — само устройство: projectionTypename в конфиге набора. Установка ~10 с (решение 11-20).
class VB_EB_KitBase extends ItemBase
{
	override bool IsBasebuildingKit()
	{
		return true;
	}

	override bool IsDeployable()
	{
		return true;
	}

	override float GetDeployTime()
	{
		return 5;		// решение пользователя 11-20 (было 10)
	}

	string VB_DeviceType()
	{
		return ConfigGetString("projectionTypename");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		if (!GetGame().IsDedicatedServer())
			return;
		string type = VB_DeviceType();
		if (type == "")
			return;
		Object obj = GetGame().CreateObjectEx(type, GetPosition(), ECE_PLACE_ON_SURFACE);
		if (obj)
		{
			obj.SetPosition(position);
			obj.SetOrientation(orientation);
			// срок жизни базы (45 дней; в сети продлевается раз в час — VB_EB_Network); на сервере — ещё и types.xml
			EntityAI dev = EntityAI.Cast(obj);
			if (dev)
			{
				dev.SetLifetimeMax(3888000);
				dev.SetLifetime(3888000);
			}
		}
		HideAllSelections();		// набор удалит действие установки по завершении (как FenceKit)
	}
};
