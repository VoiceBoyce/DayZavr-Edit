// VB_Echo_Server (2026-11-26): серверная часть сервопривода экзо, перенесена из клиентского
// VB_EchoClothes/VB_EC_Servo.c без изменений логики: решение «работает» (надет, цел, есть энергия), свет, цикл расхода
// энергии из ячеек. Эффект (выносливость, вес, шаг без энергии) остаётся у клиента — по флагу m_VB_Active.

modded class VB_EC_Exo_Servo_Base
{
	// сервер: чем светится — ячейка, из которой берётся энергия (первая с зарядом; синтез — бесконечная)
	protected int VB_CalcGlow()
	{
		if (!m_VB_Active)
			return 0;
		array<EntityAI> cells = new array<EntityAI>;
		VB_Cells(cells);
		foreach (EntityAI c : cells)
		{
			if (c.IsRuined())
				continue;
			if (VB_Echo.IsInfinite(c))
				return 2;
			if (VB_Echo.GetE(c) > 0)
				return 1;
		}
		return 1;
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

	override protected void VB_UpdateActive()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		bool a = VB_Wearer() != null && !IsRuined() && VB_HasEnergy();
		bool changed = a != m_VB_Active;
		m_VB_Active = a;
		int g = VB_CalcGlow();
		if (!changed && g == m_VB_Glow)
			return;
		m_VB_Glow = g;
		SetSynchDirty();
		if (changed)
			VB_OnActiveChanged();
	}

	// сервер, надели на пояс: цикл расхода
	override protected void VB_ServerAttached()
	{
		VB_EC_Servo.Load();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, VB_EC_Servo.s_TickSeconds * 1000, true);
		VB_UpdateActive();
	}

	// сервер, сняли: цикл стоп, выключить
	override protected void VB_ServerDetached(EntityAI parent)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
		PlayerBase was = PlayerBase.Cast(parent);
		if (m_VB_Active || m_VB_Glow != 0)
		{
			m_VB_Active = false;
			m_VB_Glow = 0;
			SetSynchDirty();
			SetWeightDirty();
			if (was)
				was.VB_EC_ForceStaminaRecheck();
		}
	}

	override protected void VB_ServerDelete()
	{
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
};
