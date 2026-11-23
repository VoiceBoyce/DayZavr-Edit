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
	protected bool m_VB_Active;			// синхр.: работает (надет на пояс, есть энергия, цел)
	protected bool m_VB_ActiveSeen;		// клиент: последнее известное значение (пересчёт веса при смене)

	void VB_EC_Exo_Servo_Base()
	{
		RegisterNetSyncVariableBool("m_VB_Active");
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
		EntityAI c1 = FindAttachmentBySlotName("VB_Echo_GearCell1");
		if (c1)
			cells.Insert(c1);
		EntityAI c2 = FindAttachmentBySlotName("VB_Echo_GearCell2");
		if (c2)
			cells.Insert(c2);
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

	// сервер: списать kE — сначала из первой ячейки с зарядом, остаток из второй; синтез — бесплатно
	protected float VB_Drain(float kE)
	{
		array<EntityAI> cells = new array<EntityAI>;
		VB_Cells(cells);
		float left = kE;
		foreach (EntityAI c : cells)
		{
			if (left <= 0.0001)
				break;
			if (c.IsRuined())
				continue;
			if (VB_Echo.IsInfinite(c))
				return kE;
			float took = VB_Echo.DrainE(c, left);
			left -= took;
		}
		return kE - left;
	}

	// ---------------------------------------------------------------- состояние (сервер)
	protected void VB_UpdateActive()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		bool a = VB_Wearer() != null && !IsRuined() && VB_HasEnergy();
		if (a == m_VB_Active)
			return;
		m_VB_Active = a;
		SetSynchDirty();
		VB_OnActiveChanged();
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
	}

	// ---------------------------------------------------------------- цикл расхода (сервер, пока на поясе)
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		if (!GetGame().IsDedicatedServer())
			return;
		VB_EC_Servo.Load();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, VB_EC_Servo.s_TickSeconds * 1000, true);
		VB_UpdateActive();
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		if (!GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
		PlayerBase was = PlayerBase.Cast(parent);
		if (m_VB_Active)
		{
			m_VB_Active = false;
			SetSynchDirty();
			SetWeightDirty();
			if (was)
				was.VB_EC_ForceStaminaRecheck();
		}
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
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
	}

	void VB_Tick()
	{
		PlayerBase p = VB_Wearer();
		if (!p)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
			VB_UpdateActive();
			return;
		}
		VB_UpdateActive();
		if (!m_VB_Active)
			return;
		float assist = VB_EC_Servo.AssistKg(p);
		float loadKg = p.GetWeightEx() / 1000;
		float usedKg = Math.Min(loadKg, assist);
		float use = VB_EC_Servo.s_DrainBase + VB_EC_Servo.s_DrainPerKg * usedKg;
		HumanMovementState hms = new HumanMovementState();
		p.GetMovementState(hms);
		bool sprint = hms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT;
		if (sprint)
			use = use * VB_EC_Servo.s_SprintMul;
		use = use * VB_EC_Servo.s_TickSeconds / 10;		// kE/tick, тик = 10 с
		float drained = VB_Drain(use);
		VB_UpdateActive();								// кончилась энергия — выключается сам
#ifdef DIAG_DEVELOPER
		Print("[VB_EC] сервопривод: нагрузка " + loadKg + " кг, помощь " + usedKg + " кг, бег " + sprint + ", списано " + drained + " kE, работает " + m_VB_Active);
#endif
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

modded class PlayerBase
{
	void VB_EC_ForceStaminaRecheck()
	{
		SetWeightDirty();
		StaminaHandler sh = GetStaminaHandler();
		if (sh)
			sh.VB_EC_ForceLoadRecheck();
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
