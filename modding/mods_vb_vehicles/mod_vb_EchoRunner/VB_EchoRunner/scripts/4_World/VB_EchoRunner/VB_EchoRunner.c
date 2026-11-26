// Echo Runner (сервер 16). Основа — Interceptor ALEX (скрипт Borizz.k для ALEX.P, 2021; переделка разрешена
// владельцем пака). Своё: урона от столкновений машина не получает, неон Изнанки и экран горят при работающем
// моторе, свои слоты и детали (VB_ER_*).

class VB_EchoRunner extends CarScript
{
	static const string VB_NEON_ON = "vb_echorunner\\data\\parts\\er_neon.rvmat";
	static const string VB_NEON_OFF = "vb_echorunner\\data\\parts\\er_neon_off.rvmat";
	static const string VB_NEON_IDLE = "vb_echorunner\\data\\parts\\er_neon_idle.rvmat";	// деталь не на машине: еле тлеет (превью было тёмным, 11-18)
	// цвет энергии полёта: Эхо — красный, холодный синтез — бирюзовый (трубопроводы и модули перекрашиваются по баку)
	static const string VB_NEON_TEX = "vb_echorunner\\data\\parts\\er_neon_co.paa";
	static const string VB_NEON_TEX_FUSION = "vb_echorunner\\data\\parts\\er_neon_fusion_co.paa";
	// горящий неон (er_neon.rvmat, свечение 22) — чистый цвет без примеси: днём примесь зелёного в er_neon_co
	// пережигала красный в оранжевый (скриншоты пользователя 11-20); делает tools/er_neon_variants.py
	static const string VB_NEON_TEX_HOT = "vb_echorunner\\data\\parts\\er_neon_hot_co.paa";
	static const string VB_NEON_TEX_FUSION_HOT = "vb_echorunner\\data\\parts\\er_neon_fusion_hot_co.paa";
	// энергии нет совсем — светящиеся части пустые, серые, как металл трубок (просьба пользователя 11-20)
	static const string VB_NEON_TEX_EMPTY = "vb_echorunner\\data\\parts\\er_neon_empty_co.paa";
	static const string VB_NEON_EMPTY = "vb_echorunner\\data\\parts\\er_neon_empty.rvmat";
	static const int VB_GLOW_OFF = 0;	// цвет энергии без свечения
	static const int VB_GLOW_IDLE = 1;	// еле тлеет (энергия есть, мотор заглушён)
	static const int VB_GLOW_ON = 2;	// горит
	static const string VB_SCREEN_ON = "vb_echorunner\\data\\parts\\er_screen.rvmat";
	static const string VB_SCREEN_OFF = "vb_echorunner\\data\\parts\\er_screen_off.rvmat";

	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	protected ref EffVehicleSmoke m_exhaustFxR;
	protected int m_exhaustPtcFxR;
	protected vector m_exhaustPtcPosR;
	protected vector m_exhaustPtcDirR;
	bool m_IsSecondExhaustPresent = false;

	protected int m_VB_NeonKey = -1;
	protected bool m_VB_NeonDirty = true;

	// ------------------------------------------------ полёт (этап 8): VB_EchoRunner_Flight.c, эффекты — VB_EchoRunner_Fx.c
	bool m_VB_FlyReady;				// сервер: 4 модуля + 3 трубопровода + энергия в баках (синхр.)
	bool m_VB_ParkReady;			// сервер: стоянка на подушке — 4 модуля + трубопроводы + аккумулятор Эхо/синтеза
									// с зарядом (синхр.; решение 11-20: держит заглушенную машину только такой аккумулятор)
	bool m_VB_Fusion;				// стоит бак холодного синтеза: полёт без расхода (по навескам, везде)
	int m_VB_EnergyKind = -1;		// клиент: какая энергия есть на машине — VB_EchoKind (цвет неона, света, частиц)
	bool m_VB_Flying;				// сервер: в воздухе (синхр., для эффектов)
	float m_VB_Energy;				// энергия баков 0..1 (синхр.)
	float m_VB_SyncFwd;				// нажатия водителя для остальных игроков (синхр., шаг 0.5)
	float m_VB_SyncTurn;
	float m_VB_SyncUp;
	bool m_VB_ModulesFitted;		// хоть один модуль на месте колеса (считается везде по навескам)
	float m_VB_InFwd;				// ввод водителя: W - S
	float m_VB_InTurn;				// D - A
	float m_VB_InUp;				// Shift - Ctrl
	float m_VB_FwdSign = 1;			// нос машины по оси z модели (у ALEX: -1)
	float m_VB_LeftSign = 1;		// левый борт по оси x модели
	float m_VB_BottomLS;			// низ колёс/модулей по y модели
	float m_VB_ModSide[4];			// модуль на борту +x модели (прокси без поворота) = 1, иначе -1
	bool m_VB_ModFront[4];
	ref array<string> m_VB_WheelSlots;
	ref VB_EchoRunnerFlight m_VB_Fly;
	ref VB_EchoRunnerFx m_VB_Fx;
	protected int m_VB_MoveTime = -100000;
	protected float m_VB_FxTime;

	void VB_EchoRunner()
	{
		SetEventMask(EntityEvent.SIMULATE);			// у CarScript только POSTSIMULATE/POSTFRAME (скилл dayz-aviation)
		RegisterNetSyncVariableBool("m_VB_FlyReady");
		RegisterNetSyncVariableBool("m_VB_ParkReady");
		RegisterNetSyncVariableBool("m_VB_Flying");
		RegisterNetSyncVariableFloat("m_VB_Energy", 0, 1, 2);
		RegisterNetSyncVariableFloat("m_VB_SyncFwd", -1, 1, 1);
		RegisterNetSyncVariableFloat("m_VB_SyncTurn", -1, 1, 1);
		RegisterNetSyncVariableFloat("m_VB_SyncUp", -1, 1, 1);
		m_VB_WheelSlots = {"VB_ER_Wheel_1_1", "VB_ER_Wheel_1_2", "VB_ER_Wheel_2_1", "VB_ER_Wheel_2_2"};
		VB_InitGeometry();
		m_VB_Fly = new VB_EchoRunnerFlight(this);
		if (!GetGame().IsDedicatedServer())
			m_VB_Fx = new VB_EchoRunnerFx(this);

		m_dmgContactCoef = 0.0010;
		m_EngineStartOK 		= "VB_ER_Engine_start_SoundSet";
		m_EngineStartBattery 	= "VB_ER_Engine_failed_start_battery_SoundSet";
		m_EngineStartPlug 		= "VB_ER_Engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel 		= "VB_ER_Engine_failed_start_fuel_SoundSet";
		m_EngineStopFuel 		= "VB_ER_Engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound 		= "Truck_01_door_open_SoundSet";
		m_CarDoorCloseSound 	= "Truck_01_door_close_SoundSet";

		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Offroad_02_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, "Offroad_02_Horn_SoundSet");
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new VB_EchoRunnerLightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new VB_EchoRunnerLightProfileRear()));

		SetEnginePos("0 0.7 1.2");
		if (MemoryPointExists("ptcExhaust1_end"))
		{
			m_exhaustPtcPosR = GetMemoryPointPos("ptcExhaust1_end");
			if (MemoryPointExists("ptcExhaust1_start"))
			{
				vector exhaustStartR = GetMemoryPointPos("ptcExhaust1_start");
				if (m_exhaustPtcPosR && exhaustStartR)
				{
					m_IsSecondExhaustPresent = true;
					vector tempOriR = vector.Direction(exhaustStartR, m_exhaustPtcPosR);
					m_exhaustPtcDirR[0] = -tempOriR[2];
					m_exhaustPtcDirR[1] = tempOriR[1];
					m_exhaustPtcDirR[2] = tempOriR[0];
					m_exhaustPtcDirR = m_exhaustPtcDirR.Normalized().VectorToAngles();
				}
			}
		}
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate 	= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= 30;
			m_UTSSettings.m_RangeFull		= 0.5;
			m_UTSSettings.m_RangeMax		= 2;
			m_UTSSettings.m_TemperatureCap	= 25;
			m_UTSLEngine					= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}
		VB_UpdateModulesFitted();
		if (!GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_UpdateNeon, 400, true);
			VB_UpdateNeon();
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_UpdateNeon);
		if (m_VB_Fx)
			m_VB_Fx.Stop();
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			if (m_IsSecondExhaustPresent && SEffectManager.IsEffectExist(m_exhaustPtcFxR))
				SEffectManager.Stop(m_exhaustPtcFxR);
		}
	}

	// ------------------------------------------------ урона от столкновений нет (машина эксклюзивная, 2026-11-17)
	// Как ванильный CarScript.CheckContactCache: звук удара и урон экипажу (по настройкам сервера) остаются,
	// урон самой машине (ProcessDirectDamage) не наносится. Мод сервера, глушащий OnContact, этому не мешает.
	override void CheckContactCache()
	{
		int count = m_ContactCache.Count();
		if (count == 0)
			return;

		for (int i = 0; i < count; ++i)
		{
			array<ref VehicleContactData> data = m_ContactCache.GetElement(i);
			if (!data || data.Count() == 0)
				continue;

			float dmg = Math.AbsFloat(data[0].m_Impulse * m_dmgContactCoef);
			float crewDmgBase = Math.AbsFloat((data[0].m_Impulse / dBodyGetMass(this)) * 1000 * m_dmgContactCoef);
			if (dmg < GameConstants.CARS_CONTACT_DMG_MIN)
				continue;

			if (dmg < GameConstants.CARS_CONTACT_DMG_THRESHOLD)
			{
				SynchCrashLightSound(true);
			}
			else
			{
				DamageCrew(crewDmgBase);
				SynchCrashHeavySound(true);
			}
		}

		UpdateHeadlightState();
		UpdateLights();
		m_ContactCache.Clear();
	}

	// ------------------------------------------------ неон Изнанки и экран (клиент)
	// Кузов, двери, экран — горят при работающем моторе (кузов всегда красный).
	// Модули, трубопроводы, баки (просьба пользователя 11-20): энергии нет совсем — пустые серые, как металл; энергия есть —
	// цвет энергии (Эхо — красный, синтез — бирюзовый): мотор заглушён — еле тлеют, мотор заведён — горят.
	// Модули горят, когда машина готова к полёту (m_VB_FlyReady); баки — каждый по своему заряду
	// (пара синтеза горит всегда, одиночный бак синтеза только тлеет — без пары не работает).
	// Модули красит любая энергия машины (и аккумулятор — на нём они держат подушку), трубопроводы — только энергия
	// баков: без баков трубки серые, сразу видно, что машина висит, но не полетит (решение пользователя 11-20).
	void VB_UpdateNeon()
	{
		bool engine = EngineIsOn();
		int kind = VB_EnergyKind();
		int pipeKind = VB_TankKind();
		m_VB_EnergyKind = kind;
		int key = 0;
		if (engine)
			key += 1;
		if (m_VB_FlyReady)
			key += 2;
		if (VB_TankHasEnergy("VB_ER_Tank_1"))
			key += 4;
		if (VB_TankHasEnergy("VB_ER_Tank_2"))
			key += 8;
		key += 16 * (kind + 1);
		if (key == m_VB_NeonKey && !m_VB_NeonDirty)
			return;
		m_VB_NeonKey = key;
		m_VB_NeonDirty = false;
		bool tanksCharged = (key & 12) != 0;

		if (engine)
			VB_SetSelTexture(this, "neon", VB_NEON_TEX_HOT);
		else
			VB_SetSelTexture(this, "neon", VB_NEON_TEX);
		VB_SetSelMaterial(this, "neon", VB_NeonMat(engine));
		if (engine)
			VB_SetSelMaterial(this, "screen", VB_SCREEN_ON);
		else
			VB_SetSelMaterial(this, "screen", VB_SCREEN_OFF);
		for (int i = 0; i < GetInventory().AttachmentCount(); i++)
		{
			EntityAI att = GetInventory().GetAttachmentFromIndex(i);
			if (!att)
				continue;
			if (att.IsInherited(VB_EchoRunner_GravModule))
			{
				int modGlow = VB_GLOW_IDLE;
				if (engine && m_VB_FlyReady)
					modGlow = VB_GLOW_ON;
				VB_PaintNeon(att, kind, modGlow);
			}
			else if (att.IsKindOf("VB_EchoRunner_Tank"))
			{
				int tankKind = VB_EchoKind.NONE;
				if (VB_Echo.HasCharge(att))
					tankKind = VB_Echo.Kind(att);
				// горит, только когда питает машину: Эхо — при заведённом моторе, пара синтеза — всегда;
				// одиночный бак синтеза только тлеет — без пары он не работает
				int tankGlow = VB_GLOW_IDLE;
				if (VB_TankPowers(att) && (engine || VB_Echo.IsInfinite(att)))
					tankGlow = VB_GLOW_ON;
				VB_PaintNeon(att, tankKind, tankGlow);
			}
			else if (att.IsInherited(VB_EchoRunner_RiftPart))
			{
				int pipeGlow = VB_GLOW_IDLE;
				if (engine && tanksCharged)
					pipeGlow = VB_GLOW_ON;
				VB_PaintNeon(att, pipeKind, pipeGlow);
			}
		}
	}

	// энергия баков (цвет трубопроводов): пара синтеза > бак Эхо с зарядом; иначе VB_EchoKind.NONE
	int VB_TankKind()
	{
		if (VB_FusionPair())
			return VB_EchoKind.FUSION;
		if (VB_TankHasEnergy("VB_ER_Tank_1") || VB_TankHasEnergy("VB_ER_Tank_2"))
			return VB_EchoKind.ECHO;
		return VB_EchoKind.NONE;
	}

	// какая энергия есть на машине (цвет модулей, света, частиц): энергия баков > аккумулятор Эхо/синтеза с зарядом;
	// ничего нет — VB_EchoKind.NONE (детали серые). Годится на клиенте (заряд — по синхронизированному количеству).
	int VB_EnergyKind()
	{
		int tk = VB_TankKind();
		if (tk != VB_EchoKind.NONE)
			return tk;
		EntityAI b = GetInventory().FindAttachment(CarBattery.SLOT_ID);
		if (b && VB_Echo.IsCarrier(b) && VB_Echo.HasCharge(b))
			return VB_Echo.Kind(b);
		return VB_EchoKind.NONE;
	}

	// неон детали полёта: нет энергии — серый пустой; есть — цвет энергии с нужным свечением
	static void VB_PaintNeon(EntityAI part, int kind, int glow)
	{
		int idx = part.GetHiddenSelectionIndex("neon");
		if (idx < 0)
			return;
		if (kind == VB_EchoKind.NONE)
		{
			part.SetObjectTexture(idx, VB_NEON_TEX_EMPTY);
			part.SetObjectMaterial(idx, VB_NEON_EMPTY);
			return;
		}
		bool hot = (glow == VB_GLOW_ON);
		if (kind == VB_EchoKind.FUSION)
		{
			if (hot)
				part.SetObjectTexture(idx, VB_NEON_TEX_FUSION_HOT);
			else
				part.SetObjectTexture(idx, VB_NEON_TEX_FUSION);
		}
		else
		{
			if (hot)
				part.SetObjectTexture(idx, VB_NEON_TEX_HOT);
			else
				part.SetObjectTexture(idx, VB_NEON_TEX);
		}
		if (glow == VB_GLOW_ON)
			part.SetObjectMaterial(idx, VB_NEON_ON);
		else if (glow == VB_GLOW_IDLE)
			part.SetObjectMaterial(idx, VB_NEON_IDLE);
		else
			part.SetObjectMaterial(idx, VB_NEON_OFF);
	}

	// деталь не на машине: бак — цвет своей энергии (пустой — серый), модуль и трубопровод — пустые серые
	static void VB_PaintDetached(EntityAI item)
	{
		if (!item)
			return;
		int kind = VB_EchoKind.NONE;
		if (item.IsKindOf("VB_EchoRunner_Tank") && VB_Echo.HasCharge(item))
			kind = VB_Echo.Kind(item);
		VB_PaintNeon(item, kind, VB_GLOW_IDLE);
#ifdef DIAG_DEVELOPER
		ItemBase dbgItem = ItemBase.Cast(item);
		if (dbgItem)
			Print("[VB_ER] снятая деталь " + item.GetType() + " кол-во " + dbgItem.GetQuantity() + "/" + dbgItem.GetQuantityMax() + " вид " + kind + " неон " + item.GetHiddenSelectionIndex("neon"));
#endif
	}

	static void VB_SetSelTexture(EntityAI ent, string selection, string texture)
	{
		int idx = ent.GetHiddenSelectionIndex(selection);
		if (idx >= 0)
			ent.SetObjectTexture(idx, texture);
	}

	static string VB_NeonMat(bool on)
	{
		if (on)
			return VB_NEON_ON;
		return VB_NEON_OFF;
	}

	// бак в слоте питает машину (одиночный бак синтеза — нет, см. VB_TankPowers)
	protected bool VB_TankHasEnergy(string slot)
	{
		return VB_TankPowers(FindAttachmentBySlotName(slot));
	}

	static void VB_SetSelMaterial(EntityAI ent, string selection, string material)
	{
		int idx = ent.GetHiddenSelectionIndex(selection);
		if (idx >= 0)
			ent.SetObjectMaterial(idx, material);
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		m_VB_NeonDirty = true;
		VB_UpdateModulesFitted();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		m_VB_NeonDirty = true;
		VB_UpdateModulesFitted();
		if (item && !GetGame().IsDedicatedServer())
		{
			VB_PaintDetached(item);		// снятая деталь: бак — тлеет цветом своей энергии, модуль/трубопровод — серые
			VB_EchoRunner_GravModule detachedMod = VB_EchoRunner_GravModule.Cast(item);
			if (detachedMod)
				detachedMod.VB_PaintSoon();
			VB_EchoRunner_RiftPart detachedPart = VB_EchoRunner_RiftPart.Cast(item);
			if (detachedPart)
				detachedPart.VB_PaintSoon();
			if (item.IsInherited(VB_EchoRunner_GravModule))
				VB_EchoRunnerFx.ResetModule(item);
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		m_VB_NeonDirty = true;
	}

	// ------------------------------------------------ полёт (этап 8)
	// оси машины по памяти модели: перед — ось колеса 1_1, левый борт — колёса 1_x; низ модулей = ось - радиус 0.42
	protected void VB_InitGeometry()
	{
		vector fl = GetMemoryPointPos("wheel_1_1_axis");
		vector rl = GetMemoryPointPos("wheel_1_2_axis");
		m_VB_FwdSign = 1;
		if (fl[2] < rl[2])
			m_VB_FwdSign = -1;
		m_VB_LeftSign = 1;
		if (fl[0] < 0)
			m_VB_LeftSign = -1;
		float ysum = 0;
		for (int i = 0; i < 4; i++)
		{
			string suf = m_VB_WheelSlots[i].Substring(12, 3);		// "VB_ER_Wheel_1_1" -> "1_1"
			vector p = GetMemoryPointPos("wheel_" + suf + "_axis");
			ysum += p[1];
			m_VB_ModSide[i] = 1;
			if (p[0] < 0)
				m_VB_ModSide[i] = -1;
			m_VB_ModFront[i] = suf.Substring(2, 1) == "1";
		}
		m_VB_BottomLS = ysum / 4.0 - 0.42;
	}

	EntityAI VB_ModuleAt(int i)
	{
		EntityAI e = FindAttachmentBySlotName(m_VB_WheelSlots[i]);
		if (e && e.IsInherited(VB_EchoRunner_GravModule))
			return e;
		return null;
	}

	protected void VB_UpdateModulesFitted()
	{
		bool any = false;
		for (int i = 0; i < 4; i++)
		{
			if (VB_ModuleAt(i))
				any = true;
		}
		m_VB_ModulesFitted = any;
		m_VB_Fusion = VB_FusionPair();
	}

	int VB_ModulesWorking()
	{
		int n = 0;
		for (int i = 0; i < 4; i++)
		{
			EntityAI m = VB_ModuleAt(i);
			if (m && !m.IsRuined())
				n++;
		}
		return n;
	}

	bool VB_PipesFitted()
	{
		EntityAI l = FindAttachmentBySlotName("VB_ER_Pipes_L");
		EntityAI r = FindAttachmentBySlotName("VB_ER_Pipes_R");
		EntityAI e = FindAttachmentBySlotName("VB_ER_Pipes_Engine");
		return l && r && e && !l.IsRuined() && !r.IsRuined() && !e.IsRuined();
	}

	// энергия баков, Э (носители ядра VB_EchoCore): считаются только баки, которые питают машину (VB_TankPowers)
	float VB_TankEnergy()
	{
		float e = 0;
		EntityAI t1 = FindAttachmentBySlotName("VB_ER_Tank_1");
		EntityAI t2 = FindAttachmentBySlotName("VB_ER_Tank_2");
		if (VB_TankPowers(t1))
			e += VB_Echo.Get(t1);
		if (VB_TankPowers(t2))
			e += VB_Echo.Get(t2);
		return e;
	}

	// стоит пара целых баков холодного синтеза. Синтез работает только парой: один бак машину не питает
	// (решение пользователя 11-20 — в донате баки идут парой, одна пара = одна машина)
	bool VB_FusionPair()
	{
		EntityAI t1 = FindAttachmentBySlotName("VB_ER_Tank_1");
		EntityAI t2 = FindAttachmentBySlotName("VB_ER_Tank_2");
		return t1 && t2 && !t1.IsRuined() && !t2.IsRuined() && VB_Echo.IsInfinite(t1) && VB_Echo.IsInfinite(t2);
	}

	// бак питает машину: Эхо — пока есть заряд (и один бак летает, с половиной запаса); синтез — только в паре
	bool VB_TankPowers(EntityAI tank)
	{
		if (!tank || !VB_Echo.HasCharge(tank))
			return false;
		if (VB_Echo.IsInfinite(tank))
			return VB_FusionPair();
		return true;
	}

	// баки Эхо и синтеза не смешивать (решение пользователя 11-20): во второй слот встаёт только бак того же вида.
	// Загрузку из сохранения (CanLoadAsAttachment) не трогаем — старая машина со смесью загрузится как есть.
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		if (attachment && attachment.IsKindOf("VB_EchoRunner_Tank"))
		{
			string otherSlot = "VB_ER_Tank_1";
			if (slotId == InventorySlots.GetSlotIdFromString("VB_ER_Tank_1"))
				otherSlot = "VB_ER_Tank_2";
			EntityAI other = FindAttachmentBySlotName(otherSlot);
			if (other && other != attachment && VB_Echo.Kind(other) != VB_Echo.Kind(attachment))
				return false;
		}
		return true;
	}

	// аккумулятор, который держит заглушенную машину на подушке: носитель ядра (Эхо или синтез) с зарядом; иначе null.
	// Обычный автомобильный аккумулятор подушку не держит (решение пользователя 11-20).
	EntityAI VB_ParkBattery()
	{
		EntityAI b = GetInventory().FindAttachment(CarBattery.SLOT_ID);
		if (b && VB_Echo.IsCarrier(b) && VB_Echo.Get(b) > 0)
			return b;
		return null;
	}

	// идёт управляемый полёт: всё на месте, мотор заведён, водитель в сознании
	bool VB_FlightActive()
	{
		if (!m_VB_ModulesFitted || !m_VB_FlyReady || !EngineIsOn())
			return false;
		Human driver = CrewDriver();
		return driver && driver.IsControllingVehicle();
	}

	// клиент водителя: свои клавиши полёта (SyncedValue заодно отправляет их серверу — как у RFFS)
	protected void VB_ReadLocalInputs()
	{
		PlayerBase pl = PlayerBase.Cast(CrewDriver());
		if (!pl || pl != GetGame().GetPlayer())
			return;
		VB_ReadInputsOf(pl);
	}

	protected void VB_ReadInputsOf(PlayerBase pl)
	{
		m_VB_InFwd = 0;
		m_VB_InTurn = 0;
		m_VB_InUp = 0;
		if (!pl)
			return;
		UAInterface ui = pl.GetInputInterface();
		if (!ui)
			return;
		m_VB_InFwd = ui.SyncedValue("UAVBERForward") - ui.SyncedValue("UAVBERBack");
		m_VB_InTurn = ui.SyncedValue("UAVBERRight") - ui.SyncedValue("UAVBERLeft");
		m_VB_InUp = ui.SyncedValue("UAVBERUp") - ui.SyncedValue("UAVBERDown");
#ifdef DIAG_DEVELOPER
		// стенд dayz-mcp: цифровой блок клиента (VB_ERDebug, 5_Mission) — только DayZDiag
		if (!GetGame().IsDedicatedServer() && pl == GetGame().GetPlayer())
		{
			m_VB_InFwd = Math.Clamp(m_VB_InFwd + VB_ERDebug.s_Fwd, -1, 1);
			m_VB_InTurn = Math.Clamp(m_VB_InTurn + VB_ERDebug.s_Turn, -1, 1);
			m_VB_InUp = Math.Clamp(m_VB_InUp + VB_ERDebug.s_Up, -1, 1);
		}
#endif
	}

	override void EOnSimulate(IEntity other, float dt)
	{
		super.EOnSimulate(other, dt);
		if (!m_VB_ModulesFitted || !IsServerOrOwner())
			return;
		// сервер: ввод приходит в ходах машины; если ходов давно нет — читаем синхронизированный ввод водителя
		if (GetGame().IsServer() && GetGame().GetTime() - m_VB_MoveTime > 300)
			VB_ReadInputsOf(PlayerBase.Cast(CrewDriver()));
		m_VB_Fly.Step(dt);
	}

	override void OnUpdate(float dt)
	{
		super.OnUpdate(dt);
		if (GetGame().IsServer() && m_VB_Fly)
		{
			m_VB_Fly.ServerTick(dt);
			VB_SyncPilotInput();
		}
	}

	// сервер раздаёт нажатия водителя остальным игрокам: по ним наклоны модулей, луч, шлейф и гул (отзыв 11-18:
	// наклоны — от клавиш); шаг 0.5, отправка только при изменении
	protected void VB_SyncPilotInput()
	{
		float f = 0;
		float t = 0;
		float u = 0;
		if (m_VB_ModulesFitted && VB_FlightActive())
		{
			f = Math.Round(m_VB_InFwd * 2.0) * 0.5;
			t = Math.Round(m_VB_InTurn * 2.0) * 0.5;
			u = Math.Round(m_VB_InUp * 2.0) * 0.5;
		}
		if (f != m_VB_SyncFwd || t != m_VB_SyncTurn || u != m_VB_SyncUp)
		{
			m_VB_SyncFwd = f;
			m_VB_SyncTurn = t;
			m_VB_SyncUp = u;
			SetSynchDirty();
		}
	}

	// На модулях колёсного хода нет (решение пользователя 11-18): коробка всегда на нейтрали, переключить нельзя —
	// с передачей газ проворачивал колёса, а модули сидят в их слотах и кувыркались; тормоза зажаты и без водителя,
	// руль прямо. На колёсах (модулей нет) машина обычная, ручная коробка.
	// Мотор в полёте: на нейтрали газ в пол давал перегазовку (отзыв 11-18) — держим обороты по силе тяги:
	// холостые без клавиш, VB_ERFlight.RPM_FLY при полной тяге.
	override void OnInput(float dt)
	{
		super.OnInput(dt);
		if (!m_VB_ModulesFitted)
			return;
		int neutral = GetNeutralGear();
		if (GetGear() != neutral)
			ShiftTo(neutral);
		SetBrake(1.0);
		SetHandbrake(1.0);
		SetBrakesActivateWithoutDriver(true);
		SetSteering(0);
		float thrust = 0;
		if (VB_FlightActive())
			thrust = VB_Thrust();
		float throttle = 0;
		if (thrust > 0.02)
		{
			float idle = EngineGetRPMIdle();
			float target = idle + (VB_ERFlight.RPM_FLY - idle) * thrust;
			throttle = Math.Clamp((target - EngineGetRPM()) / 1500.0 + 0.1 * thrust, 0, 1);
		}
		SetThrottle(throttle);
	}

	// ввод водителя для эффектов: у самого водителя — свои клавиши, у остальных — с сервера (m_VB_Sync*)
	void VB_PilotInput(out float fwd, out float turn, out float up)
	{
		Human drv = CrewDriver();
		if (drv && drv == GetGame().GetPlayer())
		{
			fwd = m_VB_InFwd;
			turn = m_VB_InTurn;
			up = m_VB_InUp;
			return;
		}
		fwd = m_VB_SyncFwd;
		turn = m_VB_SyncTurn;
		up = m_VB_SyncUp;
	}

	// сила тяги по вводу 0..1 (W/S, набор высоты, поворот)
	static float VB_ThrustOf(float fwd, float turn, float up)
	{
		float t = Math.AbsFloat(fwd);
		t = Math.Max(t, Math.Max(up, 0) * 0.75);
		t = Math.Max(t, Math.AbsFloat(turn) * 0.35);
		return Math.Clamp(t, 0, 1);
	}

	// сила тяги по вводу водителя 0..1 (W/S, набор высоты, поворот)
	float VB_Thrust()
	{
		return VB_ThrustOf(m_VB_InFwd, m_VB_InTurn, m_VB_InUp);
	}

#ifdef DIAG_DEVELOPER
	// стенд (Баги №60): раз в 2 с — где колесо в физике (WheelGetPositionLS) и где нарисовано (кость колеса), фаза damper
	protected float m_VB_SuspDiagT;
	protected void VB_SuspDiag(float dt)
	{
		m_VB_SuspDiagT += dt;
		if (m_VB_SuspDiagT < 0.5)
			return;
		m_VB_SuspDiagT = 0;
		// колёса движка: 0 левое переднее, 1 правое переднее, 2 левое заднее, 3 правое заднее (Car.c); касание — в осях модели
		string s = "[VB_ER] подвеска v" + GetSpeedometerAbsolute();
		TStringArray wtag = {"1_1", "2_1", "1_2", "2_2"};
		for (int w = 0; w < WheelCount() && w < 4; w++)
		{
			string cy = "-";
			if (WheelHasContact(w))
			{
				vector cp = WorldToModel(WheelGetContactPosition(w));
				cy = cp[1].ToString();
			}
			s = s + " | k" + wtag[w] + " " + cy;
		}
		TStringArray src = {"wheelfrontleft", "wheelbackleft", "wheelfrontright", "wheelbackright"};
		TStringArray tag = {"1_1", "1_2", "2_1", "2_2"};
		TIntArray levels = {GetViewGeometryLevel(), GetFireGeometryLevel(), GetGeometryLevel(), 0};
		TIntArray pv = new TIntArray();
		for (int i = 0; i < 4; i++)
		{
			int pivot = -1;
			foreach (int lvl : levels)
			{
				if (lvl < 0 || pivot >= 0)
					continue;
				pv.Clear();
				GetBonePivotsForAnimationSource(lvl, src[i], pv);
				if (pv.Count() > 0)
					pivot = pv[0];
			}
			vector c = GetMemoryPointPos("wheel_" + tag[i] + "_axis");
			string vis = "-";
			if (pivot >= 0)
			{
				vector m[4];
				GetBoneTransformMS(pivot, m);
				vector v = m[0] * c[0] + m[1] * c[1] + m[2] * c[2] + m[3];
				vis = v[1].ToString();
			}
			s = s + " | " + tag[i] + " p " + GetAnimationPhase("damper_" + tag[i]) + " рис " + vis;
		}
		Print(s);
	}
#endif

	override void EOnPostFrame(IEntity other, int extra)
	{
		super.EOnPostFrame(other, extra);
		if (!m_VB_Fx)
			return;
		float now = GetGame().GetTickTime();
		float dt = Math.Clamp(now - m_VB_FxTime, 0, 0.1);
		m_VB_FxTime = now;
#ifdef DIAG_DEVELOPER
		VB_SuspDiag(dt);
#endif
		if (!m_VB_ModulesFitted && m_VB_Fx.GetSpin() <= 0)
			return;
		if (m_VB_ModulesFitted)
			VB_ReadLocalInputs();
		m_VB_Fx.Update(dt);
	}

#ifdef FEATURE_NETWORK_RECONCILIATION
	protected override event typename GetMoveType()
	{
		return VB_EchoRunnerMove;
	}

	// клиент водителя: ввод полёта в ход (сервер повторит ход с тем же вводом)
	protected override event void ObtainMove(PawnMove pMove)
	{
		super.ObtainMove(pMove);
		VB_EchoRunnerMove move = VB_EchoRunnerMove.Cast(pMove);
		if (!move)
			return;
		if (m_VB_ModulesFitted)
			VB_ReadLocalInputs();
		move.m_VB_Fwd = m_VB_InFwd;
		move.m_VB_Turn = m_VB_InTurn;
		move.m_VB_Up = m_VB_InUp;
	}

	protected override event void ConsumeMove(PawnMove pMove)
	{
		super.ConsumeMove(pMove);
		VB_EchoRunnerMove move = VB_EchoRunnerMove.Cast(pMove);
		if (!move)
			return;
		m_VB_InFwd = move.m_VB_Fwd;
		m_VB_InTurn = move.m_VB_Turn;
		m_VB_InUp = move.m_VB_Up;
		m_VB_MoveTime = GetGame().GetTime();
	}

	protected override event bool ReplayMove(PawnMove pMove)
	{
		if (!super.ReplayMove(pMove))
			return false;
		VB_EchoRunnerMove move = VB_EchoRunnerMove.Cast(pMove);
		if (move)
		{
			m_VB_InFwd = move.m_VB_Fwd;
			m_VB_InTurn = move.m_VB_Turn;
			m_VB_InUp = move.m_VB_Up;
		}
		return true;
	}
#endif

	// ------------------------------------------------ из Interceptor ALEX (второй выхлоп, тепло мотора)
	override void OnEngineStart()
	{
		super.OnEngineStart();
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			m_UTSource.SetDefferedActive(true, 20.0);
		m_VB_NeonDirty = true;
	}

	override void OnEngineStop()
	{
		super.OnEngineStop();
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			m_UTSource.SetDefferedActive(false, 10.0);
		m_VB_NeonDirty = true;
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		super.EOnPostSimulate(other, timeSlice);

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource && m_UTSource.IsActive())
				m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
		}

		if ((GetGame().IsClient() || !GetGame().IsMultiplayer()) && m_IsSecondExhaustPresent)
		{
			if (EngineIsOn())
			{
				if (!SEffectManager.IsEffectExist(m_exhaustPtcFxR))
				{
					m_exhaustFxR = new EffExhaustSmoke();
					m_exhaustPtcFxR = SEffectManager.PlayOnObject(m_exhaustFxR, this, m_exhaustPtcPosR, m_exhaustPtcDirR);
				}
				if (m_exhaustFxR)
					m_exhaustFxR.SetParticleStateLight();
			}
			else if (SEffectManager.IsEffectExist(m_exhaustPtcFxR))
			{
				SEffectManager.Stop(m_exhaustPtcFxR);
			}
		}
	}

	override float GetActionDistanceCoolant() { return 6.0; }
	override float GetActionDistanceFuel() { return 4.0; }
	override float GetActionDistanceOil() { return 4.0; }
	override float GetActionDistanceBrakes() { return 4.0; }
	override float GetTransportCameraDistance() { return 5.5; }
	override vector GetTransportCameraOffset() { return "0 1.6 0"; }
	override int GetAnimInstance() { return VehicleAnimInstances.SEDAN; }

	override int GetSeatAnimationType(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		}
		return 0;
	}

	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;

		// в воздухе модули, баки и трубопроводы не снять
		if (m_VB_Flying && (attachment.IsInherited(VB_EchoRunner_GravModule) || attachment.IsInherited(VB_EchoRunner_RiftPart)))
			return false;

		if (EngineIsOn() || GetCarDoorsState("VB_ER_Hood") == CarDoorState.DOORS_CLOSED)
		{
			// аккумулятор — по наследнику CarBattery: аккумуляторы Эхо/синтеза (VB_EchoBattery) тоже не вынуть
			string attType = attachment.GetType();
			if (attType == "CarRadiator" || attType == "SparkPlug" || attachment.IsKindOf("CarBattery"))
				return false;
		}
		return true;
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if (!super.CanDisplayAttachmentCategory(category_name))
			return false;

		category_name.ToLower();
		if (category_name.Contains("engine"))
		{
			if (GetCarDoorsState("VB_ER_Hood") == CarDoorState.DOORS_CLOSED)
				return false;
		}
		return true;
	}

	override bool CanDisplayCargo()
	{
		if (!super.CanDisplayCargo())
			return false;
		if (GetCarDoorsState("VB_ER_Door_Driver") == CarDoorState.DOORS_CLOSED)
			return false;
		return true;
	}

	override int GetCarDoorsState(string slotType)
	{
		CarDoor carDoor;
		Class.CastTo(carDoor, FindAttachmentBySlotName(slotType));
		if (!carDoor)
			return CarDoorState.DOORS_MISSING;

		switch (slotType)
		{
		case "VB_ER_Door_Driver":
			return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
		case "VB_ER_Door_CoDriver":
			return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
		case "VB_ER_Hood":
			return TranslateAnimationPhaseToCarDoorState("DoorsHood");
		}
		return CarDoorState.DOORS_MISSING;
	}

	override bool CrewCanGetThrough(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return GetCarDoorsState("VB_ER_Door_Driver") != CarDoorState.DOORS_CLOSED;
		case 1:
			return GetCarDoorsState("VB_ER_Door_CoDriver") != CarDoorState.DOORS_CLOSED;
		}
		return false;
	}

	override string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return "doors_driver";
		case 1:
			return "doors_codriver";
		}
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}

	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return "VB_ER_Door_Driver";
		case 1:
			return "VB_ER_Door_CoDriver";
		}
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}

	override string GetAnimSourceFromSelection(string selection)
	{
		switch (selection)
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		case "doors_hood":
			return "DoorsHood";
		}
		return "";
	}

	override bool IsVitalGlowPlug() { return false; }
	override bool IsVitalSparkPlug() { return false; }

	// аккумулятор — контракт 1.30
	override bool NeedElectricitySourceDevice() { return true; }
	override EntityAI GetElectricitySourceDevice() { return GetInventory().FindAttachment(CarBattery.SLOT_ID); }
	override string GetElectricitySourceDeviceType() { return "CarBattery"; }

	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat) { return false; }

	override bool CanReachDoorsFromSeat(string pDoorsSelection, int pCurrentSeat)
	{
		switch (pCurrentSeat)
		{
		case 0:
			return pDoorsSelection == "DoorsDriver";
		case 1:
			return pDoorsSelection == "DoorsCoDriver";
		}
		return false;
	}

	override void OnDebugSpawn()
	{
		EntityAI entity;
		if (Class.CastTo(entity, this))
		{
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Door_Driver");
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Door_CoDriver");
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Hood");
			for (int w = 0; w < 4; w++)
				entity.GetInventory().CreateInInventory("VB_EchoRunner_Wheel");
			for (int t = 0; t < 2; t++)
			{
				ItemBase tank = ItemBase.Cast(entity.GetInventory().CreateInInventory("VB_EchoRunner_Tank"));
				if (tank && GetGame().IsServer())
					tank.SetQuantity(tank.GetQuantityMax());		// отладочная машина — с полными баками (стенд зовёт и на клиенте)
			}
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Pipes_L");
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Pipes_R");
			entity.GetInventory().CreateInInventory("VB_EchoRunner_Pipes_Engine");
			// аккумулятор Эхо (общие детали @VB_EchoVehicles) — держит заглушенную машину на подушке; без мода деталей — обычный
			if (!entity.GetInventory().CreateInInventory("VB_EchoBattery"))
				entity.GetInventory().CreateInInventory("CarBattery");
			entity.GetInventory().CreateInInventory("SparkPlug");
			entity.GetInventory().CreateInInventory("CarRadiator");
			entity.GetInventory().CreateInInventory("HeadlightH7");
			entity.GetInventory().CreateInInventory("HeadlightH7");
		}
		Fill(CarFluid.FUEL, 50);
		Fill(CarFluid.COOLANT, 6.0);
		Fill(CarFluid.OIL, 4.0);
	}
}
