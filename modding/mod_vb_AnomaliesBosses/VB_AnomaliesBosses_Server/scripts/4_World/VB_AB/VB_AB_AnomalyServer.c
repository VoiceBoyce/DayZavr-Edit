// Серверная часть аномалий (ТЗ 2.1): по образцу OOTS. Клиентская часть (описание из config.cpp и визуал) —
// VB_AnomaliesBosses, VB_AB_AnomalyBase.c.
//
// Цикл: простой -> кто-то вошёл в зону -> срабатывание (урон всем внутри, особое действие, вспышка у клиентов)
// -> activeTime -> перезарядка rechargeTime -> снова простой.
// Сервер опрашивает все аномалии одним таймером (VB_AB_AnomalyManager), у аномалии нет своего триггера.

modded class VB_AB_AnomalyBase
{
	static const int ST_IDLE = 0;
	static const int ST_ACTIVE = 1;
	static const int ST_RECHARGE = 2;
	static const float HEAT_CAP = 45.0;			// потолок температуры у Жарки/Пара (у костра 25)

	protected int m_VB_State;
	protected float m_VB_DmgMult = 1;
	protected float m_VB_PushRadius;
	protected Object m_VB_Owner;				// босс, чьё это поле (его аномалии не бьют)
	protected bool m_VB_Fading;					// поле хозяина гаснет
	protected bool m_VB_NoLoot;					// без материала: временная («под ногами») или поле точки уже давало лут
	protected ref UniversalTemperatureSource m_VB_Heat;
	protected ref UniversalTemperatureSourceSettings m_VB_HeatSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_VB_HeatLambda;

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsServer())
			return;
		VB_AB_AnomalyCfg cfg = VB_AB_Cfg.Get().GetAnomaly(GetType());
		if (cfg)
		{
			m_VB_DmgMult = cfg.DamageMult;
			m_VB_PushRadius = cfg.PushRadius;
		}
		if (m_VB_PushRadius <= 0)
			m_VB_PushRadius = m_VB_Def.EffectRadius;
		if (m_VB_Def.EffectKind == "heat")
			VB_CreateHeat();
		VB_AB_AnomalyManager.Register(this);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SeedLoot, 500, false);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame() && GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_SeedLoot);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndActive);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRecharge);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Phase2);
			VB_AB_AnomalyManager.Unregister(this);
			VB_AB_Finds.RemoveByOwner(this);
			if (m_VB_Heat)
				m_VB_Heat.SetActive(false);
		}
		super.EEDelete(parent);
	}


	void VB_SetOwner(Object owner)
	{
		m_VB_Owner = owner;
	}

	// Ставить сразу после CreateObjectEx: засев идёт через 500 мс (VB_SeedLoot)
	void VB_SetNoLoot(bool on)
	{
		m_VB_NoLoot = on;
	}

	// Поле хозяина гаснет (VB_AB_Field.Collapse): аномалия скоро исчезнет вместе с находками (детекторы, ТЗ 8.19).
	void VB_SetFading(bool on)
	{
		m_VB_Fading = on;
	}

	bool VB_IsFading()
	{
		return m_VB_Fading;
	}

	Object VB_GetOwner()
	{
		return m_VB_Owner;
	}

	float VB_GetRadius()
	{
		if (!m_VB_Def)
			return 0;		// ещё не прошла EEInit
		return m_VB_Def.Radius;
	}

	bool VB_IsInside(vector pos, float r, float h)
	{
		vector c = GetPosition();
		float dy = pos[1] - c[1];
		if (dy < -0.5 || dy > h)
			return false;
		return vector.DistanceSq(Vector(pos[0], 0, pos[2]), Vector(c[0], 0, c[2])) <= r * r;
	}

	// ---------------------------------------------------------------- сервер
	// Материал аномалии (ТЗ 2.6): записи у самой аномалии, 0,5–3 м от центра; исчезают вместе с ней.
	// Без материала: временная аномалия, поле точки уже давало лут в этом рестарте, исчерпан MaxMaterialsPerRestart.
	void VB_SeedLoot()
	{
		if (m_VB_NoLoot || !VB_AB_Server.MaterialsLeft())
			return;
		VB_AB_AnomalyCfg cfg = VB_AB_Cfg.Get().GetAnomaly(GetType());
		if (cfg && cfg.Loot)
			VB_AB_Finds.SeedTable(cfg.Loot, GetPosition(), 0.5, 3.0, this);
	}

	// Вызывает VB_AB_AnomalyManager каждые 0,25 с.
	void VB_ServerTick(array<Man> players, bool playersNear)
	{
		if (m_VB_State != ST_IDLE)
			return;
		if (VB_HasVictim(m_VB_Def.Radius, m_VB_Def.Height, players, playersNear))
			VB_Activate();
	}

	// Кто может зацепить аномалию: живые игроки, зомби и звери (не боссы).
	protected bool VB_CanHit(EntityAI e)
	{
		if (!e || !e.IsAlive())
			return false;
		if (e.IsInherited(VB_AB_BossBase))
			return false;
		return e.IsMan() || e.IsInherited(DayZCreatureAI);
	}

	protected bool VB_HasVictim(float r, float h, array<Man> players, bool playersNear)
	{
		foreach (Man m : players)
		{
			if (m && m.IsAlive() && VB_IsInside(m.GetPosition(), r, h))
				return true;
		}
		if (!playersNear)
			return false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r + 0.5, objs, null);
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (e && !e.IsMan() && VB_CanHit(e) && VB_IsInside(e.GetPosition(), r, h))
				return true;
		}
		return false;
	}

	void VB_Activate()
	{
		m_VB_State = ST_ACTIVE;
		m_VB_Fire++;
		SetSynchDirty();
		if (VB_AB_Log.s_Debug)
			VB_AB_Log.Dbg("аномалия " + GetType() + " сработала у " + GetPosition().ToString());
		if (m_VB_Def.EffectKind == "twophase")
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Phase2, Math.Max(0.1, m_VB_Def.ChargeTime) * 1000, false);
		else
			VB_DamageInside(m_VB_Def.Radius, m_VB_Def.Height);
		VB_ApplySpecial();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndActive, Math.Max(0.05, m_VB_Def.ActiveTime) * 1000, false);
	}

	void VB_Phase2()
	{
		m_VB_Fire2++;
		SetSynchDirty();
		float r = m_VB_Def.Radius2;
		float h = m_VB_Def.Height2;
		if (r <= 0)
			r = m_VB_Def.Radius;
		if (h <= 0)
			h = m_VB_Def.Height;
		if (VB_AB_Log.s_Debug)
			VB_AB_Log.Dbg("аномалия " + GetType() + ": вторая фаза, радиус " + r);
		VB_DamageInside(r, h);
	}

	void VB_EndActive()
	{
		m_VB_State = ST_RECHARGE;
		if (m_VB_Heat)
			m_VB_Heat.SetActive(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndRecharge, Math.Max(0.05, m_VB_Def.RechargeTime) * 1000, false);
	}

	void VB_EndRecharge()
	{
		m_VB_State = ST_IDLE;
	}

	void VB_DamageInside(float r, float h)
	{
		if (m_VB_Def.Ammo == "")
			return;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r + 0.5, objs, null);
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (!VB_CanHit(e) || !VB_IsInside(e.GetPosition(), r, h))
				continue;
			float coef = Math.RandomFloatInclusive(m_VB_Def.DmgMin, m_VB_Def.DmgMax) * m_VB_DmgMult;
			string zone = "";
			if (e.IsMan())
				zone = m_VB_Def.Zones.GetRandomElement();
			e.ProcessDirectDamage(DT_CUSTOM, this, zone, m_VB_Def.Ammo, "0 0 0", coef);
			if (VB_AB_Log.s_Debug)
				VB_AB_Log.Dbg("аномалия " + GetType() + " задела " + e.GetType() + " (" + m_VB_Def.Ammo + " ×" + coef + ")");
		}
	}

	// Особые действия: wet — промокание, emp — разряд батареек, heat — жар, push — удар-толчок (Атом).
	protected void VB_ApplySpecial()
	{
		string eff = m_VB_Def.EffectKind;
		if (eff == "" || eff == "twophase")
			return;
		if (eff == "heat")
		{
			if (m_VB_Heat)
				m_VB_Heat.SetActive(true);
			return;
		}
		float r = m_VB_Def.EffectRadius;
		if (eff == "push")
			r = m_VB_PushRadius;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r, objs, null);
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (!e)
				continue;
			if (eff == "wet")
			{
				if (e.IsMan())
					VB_Wet(e);
			}
			else if (eff == "emp")
			{
				if (e.IsMan())
					VB_Emp(e);
			}
			else if (eff == "push")
			{
				if (VB_CanHit(e))
					e.ProcessDirectDamage(DT_CUSTOM, this, "", "MeleeCraftedHeavy_3", "0 0 0", 0.5);
			}
		}
	}

	protected void VB_Wet(EntityAI man)
	{
		for (int i = 0; i < man.GetInventory().AttachmentCount(); i++)
		{
			ItemBase att = ItemBase.Cast(man.GetInventory().GetAttachmentFromIndex(i));
			if (att)
				att.SetWetMax();
		}
	}

	protected void VB_Emp(EntityAI man)
	{
		array<EntityAI> items = new array<EntityAI>;
		man.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		PlayerBase pb = PlayerBase.Cast(man);
		if (pb && pb.GetItemInHands())
			items.Insert(pb.GetItemInHands());
		foreach (EntityAI it : items)
		{
			if (it && it != man && it.HasEnergyManager())
			{
				if (it.GetCompEM().IsWorking())
					it.GetCompEM().SwitchOff();
				it.GetCompEM().SetEnergy(0);
			}
		}
	}

	protected void VB_CreateHeat()
	{
		m_VB_HeatSettings = new UniversalTemperatureSourceSettings();
		m_VB_HeatSettings.m_UpdateInterval = 1;
		m_VB_HeatSettings.m_Updateable = true;
		m_VB_HeatSettings.m_TemperatureItemCap = GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_VB_HeatSettings.m_TemperatureCap = HEAT_CAP;
		m_VB_HeatSettings.m_RangeFull = 2.0;
		m_VB_HeatSettings.m_RangeMax = Math.Max(3.0, m_VB_Def.EffectRadius);
		m_VB_HeatSettings.m_ItemDryModifier = 3.0;
		m_VB_HeatLambda = new UniversalTemperatureSourceLambdaConstant();
		m_VB_Heat = new UniversalTemperatureSource(this, m_VB_HeatSettings, m_VB_HeatLambda);
		m_VB_Heat.SetActive(false);
	}
};

// Опрос всех аномалий сервера одним таймером.
class VB_AB_AnomalyManager
{
	static const int TICK_MS = 250;
	static const float NEAR_PLAYERS_M = 80;	// зомби проверяем, только если рядом есть игрок
	protected static ref VB_AB_AnomalyManager s_Instance;
	protected ref array<VB_AB_AnomalyBase> m_List = new array<VB_AB_AnomalyBase>;
	protected int m_TickNo;

	static void Register(VB_AB_AnomalyBase a)
	{
		if (!s_Instance)
		{
			s_Instance = new VB_AB_AnomalyManager;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Instance.Tick, TICK_MS, true);
		}
		if (s_Instance.m_List.Find(a) == -1)
			s_Instance.m_List.Insert(a);
	}

	static void Unregister(VB_AB_AnomalyBase a)
	{
		if (s_Instance)
			s_Instance.m_List.RemoveItem(a);
	}

	static int Count()
	{
		if (!s_Instance)
			return 0;
		return s_Instance.m_List.Count();
	}

	void Tick()
	{
		if (m_List.Count() == 0)
			return;
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		float near2 = NEAR_PLAYERS_M * NEAR_PLAYERS_M;
		// игроков проверяем каждый такт (0,25 с), зомби и зверей поиском объектов — раз в секунду: это была
		// самая тяжёлая постоянная нагрузка мода (аудит 2026-11-13)
		m_TickNo++;
		bool creatures = (m_TickNo % 4) == 0;
		for (int i = m_List.Count() - 1; i >= 0; i--)
		{
			VB_AB_AnomalyBase a = m_List[i];
			if (!a)
			{
				m_List.Remove(i);
				continue;
			}
			bool near = false;
			vector ap = a.GetPosition();
			foreach (Man m : players)
			{
				if (m && vector.DistanceSq(m.GetPosition(), ap) < near2)
				{
					near = true;
					break;
				}
			}
			if (near)
				a.VB_ServerTick(players, creatures);
		}
	}
};
