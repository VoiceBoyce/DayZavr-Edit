// Сервопривод экзоскелета Эхо (VB_EchoClothes, этап 4, 11-23; ТЗ_VB_EchoClothes.md раздел 7).
// Самостоятельный модуль на поясе: работает сам, когда надет и в гнёздах VB_Echo_GearCell1/2 есть энергия Эхо
// (энергоячейки ядра; ячейка синтеза — бесконечно). С любой одеждой — выбор игрока. Под энергией:
//   - снимает с нагрузки бойца assistKg (с полным комплектом экзо — assistKgFullExo): запас выносливости считается,
//     как будто груза меньше (ванильный StaminaHandler.RecalculateStaminaCap берёт вес игрока);
//   - расход выносливости × staminaUse;
//   - сам весит 0, содержимое — contentsWeight;
//   - полный экзоскелет: падение × fallHeightFullExo.
// Расход энергии (сервер, раз в tickSeconds): drainBase + drainPerKg × снятые кг (не больше нагрузки), при беге ×sprintMul.
// Числа — config.cpp, class CfgVB_EchoClothes Servo. Энергия ячеек — VB_Echo ядра (DrainE, GetE).
// Без энергии (надет, ячеек нет или пустые) — как у OOTS: только шагом (noPowerMaxSpeed), без прыжка и залезания.
// Звук (как у OOTS, его ogg): на каждый шаг — рама, а когда работает — мотор (шагом мягкий, бегом смешанный, спринтом
// тяжёлый); PlayerBase.OnStepEvent, клиенты.

class VB_EC_Servo
{
	static float s_AssistKg = 50;
	static float s_AssistKgFull = 85;
	static float s_StaminaUse = 0.8;
	static float s_Contents = 0.4;
	static float s_FallFull = 0.3;
	static float s_DrainBase = 2;
	static float s_DrainPerKg = 0.12;
	static float s_SprintMul = 1.5;
	static float s_TickSeconds = 10;
	static int s_NoPowerMaxSpeed = DayZPlayerConstants.MOVEMENTIDX_WALK;
	protected static bool s_Loaded;

	static void Load()
	{
		if (s_Loaded)
			return;
		s_Loaded = true;
		string p = "CfgVB_EchoClothes Servo ";
		s_AssistKg = VB_Read(p + "assistKg", s_AssistKg);
		s_AssistKgFull = VB_Read(p + "assistKgFullExo", s_AssistKgFull);
		s_StaminaUse = VB_Read(p + "staminaUse", s_StaminaUse);
		s_Contents = VB_Read(p + "contentsWeight", s_Contents);
		s_FallFull = VB_Read(p + "fallHeightFullExo", s_FallFull);
		s_DrainBase = VB_Read(p + "drainBase", s_DrainBase);
		s_DrainPerKg = VB_Read(p + "drainPerKg", s_DrainPerKg);
		s_SprintMul = VB_Read(p + "sprintMul", s_SprintMul);
		s_TickSeconds = VB_Read(p + "tickSeconds", s_TickSeconds);
		if (s_TickSeconds < 1)
			s_TickSeconds = 10;
		if (GetGame().ConfigIsExisting(p + "noPowerMaxSpeed"))
			s_NoPowerMaxSpeed = GetGame().ConfigGetInt(p + "noPowerMaxSpeed");
		if (s_NoPowerMaxSpeed < DayZPlayerConstants.MOVEMENTIDX_WALK)
			s_NoPowerMaxSpeed = DayZPlayerConstants.MOVEMENTIDX_WALK;
	}

	protected static float VB_Read(string path, float def)
	{
		if (!GetGame().ConfigIsExisting(path))
			return def;
		return GetGame().ConfigGetFloat(path);
	}

	// включённый сервопривод на поясе игрока (или null)
	static VB_EC_Exo_Servo_Base Active(PlayerBase player)
	{
		if (!player)
			return null;
		VB_EC_Exo_Servo_Base servo = VB_EC_Exo_Servo_Base.Cast(player.FindAttachmentBySlotName("Hips"));
		if (servo && servo.VB_IsActive())
			return servo;
		return null;
	}

	// сколько кг снимает сейчас (0 — не работает)
	static float AssistKg(PlayerBase player)
	{
		if (!Active(player))
			return 0;
		Load();
		if (player.VB_EC_ExoSetActive())
			return s_AssistKgFull;
		return s_AssistKg;
	}
};

class VB_EC_Exo_Servo_Base extends Clothing
{
	// свет (выделение vbglow, этап 3б): 0 — нет энергии (серый), 1 — энергия Эхо (красный), 2 — синтез (бирюзовый)
	static const string GLOW_TEX_OFF = "VB_EchoClothes\\exo_set\\Backpacks\\ExoPrivod\\data\\vb_ec_neon_empty_co.paa";
	static const string GLOW_TEX_ECHO = "VB_EchoClothes\\exo_set\\Backpacks\\ExoPrivod\\data\\vb_ec_neon_co.paa";
	static const string GLOW_TEX_FUSION = "VB_EchoClothes\\exo_set\\Backpacks\\ExoPrivod\\data\\vb_ec_neon_fusion_co.paa";
	static const string GLOW_MAT_OFF = "VB_EchoClothes\\exo_set\\Backpacks\\ExoPrivod\\data\\vb_ec_neon_off.rvmat";
	static const string GLOW_MAT_ON = "VB_EchoClothes\\exo_set\\Backpacks\\ExoPrivod\\data\\vb_ec_neon.rvmat";

	protected bool m_VB_Active;			// синхр.: работает (надет на пояс, есть энергия, цел)
	protected bool m_VB_ActiveSeen;		// клиент: последнее известное значение (пересчёт веса при смене)
	protected int m_VB_Glow;			// синхр.: цвет света (см. выше)
	protected int m_VB_GlowShown = -1;	// клиент: что сейчас нарисовано

	void VB_EC_Exo_Servo_Base()
	{
		RegisterNetSyncVariableBool("m_VB_Active");
		RegisterNetSyncVariableInt("m_VB_Glow", 0, 2);
	}

	int VB_GlowKind()
	{
		return m_VB_Glow;
	}

	// клиент: перекрасить свет по m_VB_Glow (и маску экзо на том же бойце — линзы горят от сервопривода)
	void VB_ApplyGlow(bool force = false)
	{
		if (GetGame().IsDedicatedServer())
			return;
		if (!force && m_VB_GlowShown == m_VB_Glow)
			return;
		m_VB_GlowShown = m_VB_Glow;
		int idx = GetHiddenSelectionIndex("vbglow");
		if (idx >= 0)
		{
			string tex = GLOW_TEX_OFF;
			string mat = GLOW_MAT_OFF;
			if (m_VB_Glow == 1)
			{
				tex = GLOW_TEX_ECHO;
				mat = GLOW_MAT_ON;
			}
			else if (m_VB_Glow == 2)
			{
				tex = GLOW_TEX_FUSION;
				mat = GLOW_MAT_ON;
			}
			SetObjectTexture(idx, tex);
			SetObjectMaterial(idx, mat);
		}
		PlayerBase p = VB_Wearer();
		if (p)
			p.VB_EC_ApplyHeadGlow(m_VB_Glow);
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		VB_ApplyGlow(true);				// уровни повреждения меняют материалы — вернуть свет
	}

	bool VB_IsActive()
	{
		return m_VB_Active;
	}

	// игрок, у которого сервопривод на поясе (или null)
	PlayerBase VB_Wearer()
	{
		PlayerBase p = PlayerBase.Cast(GetHierarchyParent());
		if (p && p.FindAttachmentBySlotName("Hips") == this)
			return p;
		return null;
	}

	protected void VB_Cells(array<EntityAI> cells)
	{
		cells.Clear();
		EntityAI pair = FindAttachmentBySlotName("VB_Echo_GearCellPair");		// сдвоенная ячейка синтеза
		if (pair)
			cells.Insert(pair);
		EntityAI c1 = FindAttachmentBySlotName("VB_Echo_GearCell1");
		if (c1)
			cells.Insert(c1);
		EntityAI c2 = FindAttachmentBySlotName("VB_Echo_GearCell2");
		if (c2)
			cells.Insert(c2);
	}

	// ---------------------------------------------------------------- гнёзда (2026-11-26)
	// Сдвоенная ячейка синтеза — одним предметом вместо обеих ячеек: стоит она — ячейки Эхо не вставить и их пустые
	// гнёзда не показываются, стоят ячейки Эхо — наоборот (решение пользователя: «чтобы игрок не путался»)
	protected static int s_VB_SlotCell1 = -2;
	protected static int s_VB_SlotCell2;
	protected static int s_VB_SlotPair;

	protected static void VB_InitSlots()
	{
		if (s_VB_SlotCell1 != -2)
			return;
		s_VB_SlotCell1 = InventorySlots.GetSlotIdFromString("VB_Echo_GearCell1");
		s_VB_SlotCell2 = InventorySlots.GetSlotIdFromString("VB_Echo_GearCell2");
		s_VB_SlotPair = InventorySlots.GetSlotIdFromString("VB_Echo_GearCellPair");
	}

	// гнездо slotId сейчас закрыто другим видом ячеек
	protected bool VB_SlotBlocked(int slotId)
	{
		VB_InitSlots();
		if (slotId == s_VB_SlotPair)
			return GetInventory().FindAttachment(s_VB_SlotCell1) != null || GetInventory().FindAttachment(s_VB_SlotCell2) != null;
		if (slotId == s_VB_SlotCell1 || slotId == s_VB_SlotCell2)
			return GetInventory().FindAttachment(s_VB_SlotPair) != null;
		return false;
	}

	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		return !VB_SlotBlocked(slotId);
	}

	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;
		return !VB_SlotBlocked(slot_id);
	}

	bool VB_HasEnergy()
	{
		array<EntityAI> cells = new array<EntityAI>;
		VB_Cells(cells);
		foreach (EntityAI c : cells)
		{
			if (c.IsRuined())
				continue;
			if (VB_Echo.IsInfinite(c))
				return true;
			if (VB_Echo.GetE(c) > 0)
				return true;
		}
		return false;
	}

	// ---------------------------------------------------------------- состояние (сервер)
	// «работает» (надет на пояс, есть энергия, цел), свет, цикл расхода — серверный мод VB_Echo_Server. Без него
	// m_VB_Active не включается: сервопривод мёртвый, в нём только шагом
	protected void VB_UpdateActive()
	{
	}

	protected void VB_ServerAttached()
	{
	}

	protected void VB_ServerDetached(EntityAI parent)
	{
	}

	protected void VB_ServerDelete()
	{
	}

	protected void VB_OnActiveChanged()
	{
		SetWeightDirty();						// включённый весит 0, карманы — 40 %
		PlayerBase p = VB_Wearer();
		if (p)
			p.VB_EC_ForceStaminaRecheck();		// запас выносливости — с новой помощью
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_VB_ActiveSeen != m_VB_Active)
		{
			m_VB_ActiveSeen = m_VB_Active;
			SetWeightDirty();
		}
		VB_ApplyGlow();
	}

	override void EEInit()
	{
		super.EEInit();
		VB_ApplyGlow(true);				// клиент: первый вид (серый, пока сервер не прислал состояние)
	}

	// ---------------------------------------------------------------- цикл расхода (сервер, пока на поясе)
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		if (!GetGame().IsDedicatedServer())
		{
			VB_ApplyGlow(true);				// клиент: шлем на бойце — тем же светом
			return;
		}
		VB_ServerAttached();
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		if (!GetGame().IsDedicatedServer())
		{
			PlayerBase wearer = PlayerBase.Cast(parent);
			if (wearer)
				wearer.VB_EC_ApplyHeadGlow(0);	// клиент: сняли сервопривод — стёкла шлема гаснут
			return;
		}
		VB_ServerDetached(parent);
	}

	// вставили или вынули ячейку
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		VB_UpdateActive();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		VB_UpdateActive();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (GetGame().IsDedicatedServer())
			VB_ServerDelete();
	}

	// включённый: сам весит 0, содержимое (ячейки, карманы) — contentsWeight
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float weight = super.GetWeightSpecialized(forceRecalc);
		if (!m_VB_Active)
			return weight;
		VB_EC_Servo.Load();
		float contents = GetInventoryAndCargoWeight(forceRecalc);
		return contents * VB_EC_Servo.s_Contents;
	}
};

// Свет снаряжения экзо по состоянию сервопривода (клиент, вид): 0 — нет энергии (offTex/offMat), 1 — Эхо, 2 — синтез
class VB_EC_Glow
{
	static void Apply(EntityAI e, string selection, int kind, string offTex, string offMat)
	{
		int idx = e.GetHiddenSelectionIndex(selection);
		if (idx < 0)
			return;
		string tex = offTex;
		string mat = offMat;
		if (kind == 1)
		{
			tex = VB_EC_Exo_Servo_Base.GLOW_TEX_ECHO;
			mat = VB_EC_Exo_Servo_Base.GLOW_MAT_ON;
		}
		else if (kind == 2)
		{
			tex = VB_EC_Exo_Servo_Base.GLOW_TEX_FUSION;
			mat = VB_EC_Exo_Servo_Base.GLOW_MAT_ON;
		}
		e.SetObjectTexture(idx, tex);
		e.SetObjectMaterial(idx, mat);
	}
};

// Маска экзо (этап 3б): под закрытым шлемом экзо пластина черепа торчит сквозь его лицевую пластину — прятать
// (выделение vbplate — simpleHiddenSelections[0]), балаклава остаётся. Линзы в глазницах убраны (решение пользователя
// 11-23). Вызывается на всех машинах (вид).
class VB_EC_Exo_Mask_Base extends Clothing
{
	void VB_SetPlateHidden(bool hidden)
	{
		SetSimpleHiddenSelectionState(0, !hidden);
	}

	// на бойце со шлемом экзо — пластину спрятать
	protected void VB_SyncPlate()
	{
		PlayerBase p = PlayerBase.Cast(GetHierarchyParent());
		VB_SetPlateHidden(p && p.VB_EC_HasExoHelmet());
	}

	override void EEInit()
	{
		super.EEInit();
		VB_SyncPlate();
	}

	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		VB_SyncPlate();
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		VB_SetPlateHidden(false);
	}
};

modded class PlayerBase
{
	protected static int s_VB_EC_HipsSlot = -1;
	protected static int s_VB_EC_MaskSlot = -1;
	protected static int s_VB_EC_HeadSlot = -1;

	// свет сервопривода на поясе (0 — нет сервопривода или нет энергии)
	int VB_EC_ServoGlowKind()
	{
		VB_EC_Exo_Servo_Base s = VB_EC_WornServo();
		if (s)
			return s.VB_GlowKind();
		return 0;
	}

	bool VB_EC_HasExoHelmet()
	{
		if (s_VB_EC_HeadSlot < 0)
			s_VB_EC_HeadSlot = InventorySlots.GetSlotIdFromString("Headgear");
		return VB_EC_Exo_Helmet_Base.Cast(GetInventory().FindAttachment(s_VB_EC_HeadSlot)) != null;
	}

	// шлем экзо надели/сняли — пластину маски спрятать/показать (helmetOn — явно: при снятии шлем ещё может числиться)
	void VB_EC_UpdateMaskPlate(bool helmetOn)
	{
		if (s_VB_EC_MaskSlot < 0)
			s_VB_EC_MaskSlot = InventorySlots.GetSlotIdFromString("Mask");
		VB_EC_Exo_Mask_Base mask = VB_EC_Exo_Mask_Base.Cast(GetInventory().FindAttachment(s_VB_EC_MaskSlot));
		if (mask)
			mask.VB_SetPlateHidden(helmetOn);
	}

	// клиент: стёкла шлема экзо — светом сервопривода
	void VB_EC_ApplyHeadGlow(int kind)
	{
		if (s_VB_EC_MaskSlot < 0)
		{
			s_VB_EC_MaskSlot = InventorySlots.GetSlotIdFromString("Mask");
			s_VB_EC_HeadSlot = InventorySlots.GetSlotIdFromString("Headgear");
		}
		VB_EC_Exo_Helmet_Base helmet = VB_EC_Exo_Helmet_Base.Cast(GetInventory().FindAttachment(s_VB_EC_HeadSlot));
		if (helmet)
			helmet.VB_SetGlow(kind);
	}
	protected bool m_VB_EC_SpeedLimited;		// мы сами поставили лимит скорости (снять, когда питание вернулось)

	void VB_EC_ForceStaminaRecheck()
	{
		SetWeightDirty();
		StaminaHandler sh = GetStaminaHandler();
		if (sh)
			sh.VB_EC_ForceLoadRecheck();
	}

	// сервопривод на поясе (или null)
	VB_EC_Exo_Servo_Base VB_EC_WornServo()
	{
		if (s_VB_EC_HipsSlot < 0)
			s_VB_EC_HipsSlot = InventorySlots.GetSlotIdFromString("Hips");
		return VB_EC_Exo_Servo_Base.Cast(GetInventory().FindAttachment(s_VB_EC_HipsSlot));
	}

	// надет, но без энергии (ячеек нет, пустые или сервопривод разрушен) — тяжёлая мёртвая рама, как у OOTS
	bool VB_EC_ServoDead()
	{
		VB_EC_Exo_Servo_Base s = VB_EC_WornServo();
		return s && !s.VB_IsActive();
	}

	// ---------------------------------------------------------------- без энергии: только шагом (сервер и свой клиент)
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		VB_EC_ApplyServoSpeedLimit();
	}

	// Ваниль сама ставит и снимает лимиты (сдача в плен, эмоции — StopAllMovement/RestartAllMovement). Поэтому лимит
	// только опускаем до шага, если он выше, — более строгий ванильный не трогаем; снимаем — только свой.
	protected void VB_EC_ApplyServoSpeedLimit()
	{
		HumanInputController hic = GetInputController();
		if (!hic)
			return;
		if (VB_EC_ServoDead())
		{
			VB_EC_Servo.Load();
			int maxSpeed = VB_EC_Servo.s_NoPowerMaxSpeed;
			if (hic.GetErectSpeedLimit() > maxSpeed)
				hic.SetErectSpeedLimit(true, maxSpeed);
			if (hic.GetCrouchSpeedLimit() > maxSpeed)
				hic.SetCrouchSpeedLimit(true, maxSpeed);
			m_VB_EC_SpeedLimited = true;
			return;
		}
		if (!m_VB_EC_SpeedLimited)
			return;
		m_VB_EC_SpeedLimited = false;
		if (hic.GetErectSpeedLimit() == VB_EC_Servo.s_NoPowerMaxSpeed)
			hic.SetErectSpeedLimit(false);
		if (hic.GetCrouchSpeedLimit() == VB_EC_Servo.s_NoPowerMaxSpeed)
			hic.SetCrouchSpeedLimit(false);
	}

	override bool CanJump()
	{
		if (VB_EC_ServoDead())
			return false;
		return super.CanJump();
	}

	override bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		if (VB_EC_ServoDead())
			return false;
		return super.CanClimb(climbType, climbRes);
	}

	// ---------------------------------------------------------------- звук на шаг (клиенты, для всех игроков рядом)
	override void OnStepEvent(string pEventType, string pUserString, int pUserInt)
	{
		super.OnStepEvent(pEventType, pUserString, pUserInt);
		if (pUserInt >= 100 || g_Game.IsDedicatedServer())		// < 100 — шаги ног (как ванильные звуки шагов)
			return;
		VB_EC_Exo_Servo_Base s = VB_EC_WornServo();
		if (!s)
			return;
		VB_EC_PlaySoundSet("VB_EC_Servo_Step_SoundSet");			// рама — всегда
		if (!s.VB_IsActive())
			return;
		HumanMovementState hms = new HumanMovementState();
		GetMovementState(hms);
		string motor = "VB_EC_Servo_MotorLight_SoundSet";
		if (hms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
			motor = "VB_EC_Servo_MotorHeavy_SoundSet";
		else if (hms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_RUN)
			motor = "VB_EC_Servo_MotorMixed_SoundSet";
		VB_EC_PlaySoundSet(motor);
	}

	protected void VB_EC_PlaySoundSet(string soundSet)
	{
		SoundParams sp = new SoundParams(soundSet);
		if (!sp.IsValid())
			return;
		SoundObjectBuilder sb = new SoundObjectBuilder(sp);
		SoundObject so = sb.BuildSoundObject();
		if (!so)
			return;
		AttenuateSoundIfNecessary(so);
		PlaySound(so, sb);
	}
};

// помощь с грузом: запас выносливости считается от нагрузки минус снятые сервоприводом кг
modded class StaminaHandler
{
	void VB_EC_ForceLoadRecheck()
	{
		m_ForceLoadRecheck = true;
	}

	override void RecalculateStaminaCap()
	{
		float assistKg = VB_EC_Servo.AssistKg(m_Player);
		if (assistKg <= 0)
		{
			super.RecalculateStaminaCap();
			return;
		}
		float saved = m_PlayerLoad;
		m_PlayerLoad = Math.Max(0, m_PlayerLoad - assistKg * 1000);
		super.RecalculateStaminaCap();
		m_PlayerLoad = saved;
	}
};
