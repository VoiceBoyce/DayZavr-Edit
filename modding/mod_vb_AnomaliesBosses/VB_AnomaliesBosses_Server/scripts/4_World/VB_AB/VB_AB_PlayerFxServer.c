// Действие способностей боссов на игрока (ТЗ 4.3), серверная часть: урон и состояния.
// Клиентская часть (экран, звук, снег) — VB_AnomaliesBosses, VB_AB_PlayerFx.c.
// API сверено по P:\scripts (DayZ 1.30), ссылки на файлы — в комментариях.

// Временный источник тепла или холода на боссе (UniversalTemperatureSource; один на объект — у боссов либо жар, либо холод).
// Environment игрока подхватывает его в радиусе (Environment.GatherTemperatureSources), отрицательная температура
// охлаждает (Environment.c: «m_UTSAverageTemperature can be negative»; ванильный холодный источник — SpookyPlayerStalker).
class VB_AB_TempSource
{
	protected ref UniversalTemperatureSourceSettings m_Settings;
	protected ref UniversalTemperatureSourceLambdaConstant m_Lambda;
	protected ref UniversalTemperatureSource m_Uts;

	void Pulse(EntityAI owner, float cap, float range, float sec)
	{
		if (!owner)
			return;
		if (!m_Uts)
		{
			m_Settings = new UniversalTemperatureSourceSettings();
			m_Settings.m_Updateable = true;
			m_Settings.m_UpdateInterval = 1;
			m_Settings.m_ManualUpdate = false;
			m_Settings.m_IsWorldOverriden = false;
			m_Lambda = new UniversalTemperatureSourceLambdaConstant();
			m_Uts = new UniversalTemperatureSource(owner, m_Settings, m_Lambda);
		}
		m_Settings.m_TemperatureCap = cap;
		m_Settings.m_TemperatureItemCap = cap;
		m_Settings.m_RangeFull = range * 0.5;
		m_Settings.m_RangeMax = range;
		m_Uts.SetActive(true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Off);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Off, sec * 1000, false);
	}

	void Off()
	{
		if (m_Uts)
			m_Uts.SetActive(false);
	}

	void ~VB_AB_TempSource()
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Off);
	}
};

class VB_AB_PlayerFx
{
	// ---------------------------------------------------------------- сервер
	// Импульс: гасит фонари, ПНВ, рации (ComponentEnergyManager.SwitchOff — как ActionTurnOffWhileInHands).
	// ПНВ поднимаем ванильно (NVGoggles.RotateGoggles), иначе при включении они опустятся сами.
	static void SwitchOffDevices(PlayerBase p)
	{
		array<EntityAI> items = new array<EntityAI>;
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		if (p.GetItemInHands())
			items.Insert(p.GetItemInHands());
		foreach (EntityAI it : items)
		{
			if (!it || it == p || !it.HasEnergyManager() || !it.GetCompEM().IsWorking())
				continue;
			NVGoggles nvg = NVGoggles.Cast(it);
			if (nvg)
				nvg.RotateGoggles(true);
			else
				it.GetCompEM().SwitchOff();
		}
	}

	static void NoiseAt(vector pos, string noise, float time)
	{
		NoiseParams np = new NoiseParams();
		np.Load(noise);
		g_Game.GetNoiseSystem().AddNoiseTarget(pos, time, np);
	}

	// Жар: быстрый перегрев через тепловой сдвиг 1.30 (ThermalBiasHandler, стадии теплового удара от 0,4)
	static void Overheat(PlayerBase p, float add)
	{
		if (p.GetThermalBiasHandler())
			p.GetThermalBiasHandler().Add(add);
	}

	// Стамина в ноль (как флешбанг в ванили: PlayerBase.EEHitBy, OVERALL_DRAIN = вся стамина)
	static void ZeroStamina(PlayerBase p)
	{
		if (p.GetStaminaHandler())
			p.GetStaminaHandler().DepleteStaminaEx(EStaminaModifiers.OVERALL_DRAIN);
	}

	// Замедление: множителя скорости в ванили нет; запрещаем бег (CanSprint ниже) на sec секунд.
	// Ставится и на сервере, и у самого игрока (RPC P_COLD), иначе клиент предскажет бег и его будет дёргать.
	static void Slow(PlayerBase p, float sec)
	{
		p.VB_AB_NoSprintFor(sec);
	}

	// Стаггер без урона: ванильный патрон Dummy_Heavy (hitAnimation = 1, урона нет), направление — от источника
	static void KnockDown(EntityAI src, PlayerBase p)
	{
		if (p && p.IsAlive())
			p.ProcessDirectDamage(DamageType.CUSTOM, src, "", "Dummy_Heavy", "0 0 0");
	}

	// Порча надетого (как TrapBase.DamageClothing): frac — доля максимального здоровья вещи
	static void DamageClothes(PlayerBase p, float frac)
	{
		array<string> slots = {"HeadGear", "Mask", "BODY", "VEST", "GLOVES", "LEGS", "FEET", "BACK"};
		foreach (string slot : slots)
		{
			EntityAI it = p.GetItemOnSlot(slot);
			if (it && !it.IsRuined())
				it.AddHealth("", "", -it.GetMaxHealth("", "") * frac);
		}
	}

	// Отравление кислотой: химический агент (CHEMICAL_POISON: от 100 — рвота, от 400 — тяжело)
	static void Poison(PlayerBase p)
	{
		p.InsertAgent(eAgents.CHEMICAL_POISON, 60);
	}

	// Рвота сразу (как Poisoning.c)
	static void Vomit(PlayerBase p)
	{
		if (!p.GetSymptomManager())
			return;
		SymptomBase s = p.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
		if (s)
		{
			CachedObjectsParams.PARAM1_FLOAT.param1 = 30.0;
			s.SetParam(CachedObjectsParams.PARAM1_FLOAT);
			s.SetDuration(5);
		}
	}

	// Выбить предмет из рук (ванильный шаблон сдачи, EmoteManager)
	static void DropHands(PlayerBase p)
	{
		ItemBase item = p.GetItemInHands();
		if (!item || !p.CanDropEntity(item))
			return;
		InventoryLocation hands = new InventoryLocation();
		hands.SetHands(p, null);
		if (p.GetInventory().HasInventoryReservation(null, hands))
			p.GetInventory().ClearInventoryReservationEx(null, hands);
		p.ServerDropEntity(item);
	}

};

// Сила удара босса и Ярость
modded class PlayerBase
{
	static const float VB_AB_RAGE_BONUS = 0.5;		// удары разъярённого босса +50 %
	float m_VB_AB_EmpNext;							// разряд EMP-аномалии не раньше этого времени (только сервер)

	// Удар босса: ванильный удар зомби × MeleeMul из bosses.json, в Ярости ещё +50 %. Шок не усиливаем —
	// иначе пара ударов Энта валит без сознания. У способностей свой урон (Damage), их не трогаем.
	// Добавка через AddHealth, а не повторный удар, чтобы не вызвать EEHitBy снова.
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!GetGame().IsServer() || !damageResult || !IsAlive())
			return;
		VB_AB_BossBase boss = VB_AB_BossBase.Cast(source);
		if (!boss || ammo == "VB_AB_AbilityDmg")
			return;
		float mul = boss.VB_GetMeleeMul();
		if (boss.VB_IsEnraged())
			mul = mul * (1 + VB_AB_RAGE_BONUS);
		float extra = mul - 1;
		if (extra <= 0)
			return;
		float dmgHealth = damageResult.GetDamage("", "Health");
		float dmgBlood = damageResult.GetDamage("", "Blood");
		if (dmgHealth <= 0 && dmgBlood <= 0)
			return;		// стаггер Удара оземь (Dummy_Heavy) и прочие удары без урона
		if (dmgHealth > 0)
			AddHealth("", "Health", -dmgHealth * extra);
		if (dmgBlood > 0)
			AddHealth("", "Blood", -dmgBlood * extra);
		VB_AB_Log.Dbg("удар " + boss.GetType() + " по " + GetCachedName() + ": " + ammo + " здоровье −" + (dmgHealth * mul).ToString() + ", кровь −" + (dmgBlood * mul).ToString() + " (×" + mul.ToString() + ")");
	}
};
