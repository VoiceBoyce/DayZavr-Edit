// Аномалии мода (ТЗ 2.1). Логика своя, по образцу OOTS; все числа и ресурсы — из config.cpp
// (cfg/anomalies.hpp, генерирует tools/ab_anomalies.py), поэтому один класс на все 22 аномалии.
//
// Цикл: простой -> кто-то вошёл в зону -> срабатывание (урон всем внутри, особое действие, вспышка у клиентов)
// -> activeTime -> перезарядка rechargeTime -> снова простой.
// Сервер опрашивает все аномалии одним таймером (VB_AB_AnomalyManager), у аномалии нет своего триггера.
// Клиент видит срабатывание по счётчику m_VB_Fire (синхронизируется), вторую фазу Воронки — по m_VB_Fire2.

class VB_AB_AnomalyDef
{
	string PtcIdle;
	string PtcActive;
	string PtcActive2;
	string SndIdle;
	float SndIdleRange;
	bool SndIdleLoop;
	string SndActive;
	float SndActiveRange;
	bool SndActiveLoop;
	string LightIdle;
	string LightActive;
	float Radius;
	float Height;
	float Radius2;
	float Height2;
	float ActiveTime;
	float RechargeTime;
	float ChargeTime;
	string Ammo;
	ref array<string> Zones = new array<string>;
	float DmgMin;
	float DmgMax;
	string EffectKind;
	float EffectRadius;

	protected static ref map<string, ref VB_AB_AnomalyDef> s_Defs;

	static VB_AB_AnomalyDef Get(string type)
	{
		if (!s_Defs)
			s_Defs = new map<string, ref VB_AB_AnomalyDef>;
		VB_AB_AnomalyDef d = s_Defs.Get(type);
		if (d)
			return d;
		d = new VB_AB_AnomalyDef;
		string p = "CfgVehicles " + type + " ";
		d.PtcIdle = GetGame().ConfigGetTextOut(p + "vbABPtcIdle");
		d.PtcActive = GetGame().ConfigGetTextOut(p + "vbABPtcActive");
		d.PtcActive2 = GetGame().ConfigGetTextOut(p + "vbABPtcActive2");
		d.SndIdle = GetGame().ConfigGetTextOut(p + "vbABSndIdle");
		d.SndIdleRange = GetGame().ConfigGetFloat(p + "vbABSndIdleRange");
		d.SndIdleLoop = GetGame().ConfigGetInt(p + "vbABSndIdleLoop") == 1;
		d.SndActive = GetGame().ConfigGetTextOut(p + "vbABSndActive");
		d.SndActiveRange = GetGame().ConfigGetFloat(p + "vbABSndActiveRange");
		d.SndActiveLoop = GetGame().ConfigGetInt(p + "vbABSndActiveLoop") == 1;
		d.LightIdle = GetGame().ConfigGetTextOut(p + "vbABLightIdle");
		d.LightActive = GetGame().ConfigGetTextOut(p + "vbABLightActive");
		d.Radius = GetGame().ConfigGetFloat(p + "vbABRadius");
		d.Height = GetGame().ConfigGetFloat(p + "vbABHeight");
		d.Radius2 = GetGame().ConfigGetFloat(p + "vbABRadius2");
		d.Height2 = GetGame().ConfigGetFloat(p + "vbABHeight2");
		d.ActiveTime = GetGame().ConfigGetFloat(p + "vbABActiveTime");
		d.RechargeTime = GetGame().ConfigGetFloat(p + "vbABRechargeTime");
		d.ChargeTime = GetGame().ConfigGetFloat(p + "vbABChargeTime");
		d.Ammo = GetGame().ConfigGetTextOut(p + "vbABAmmo");
		GetGame().ConfigGetTextArray(p + "vbABHitZones", d.Zones);
		if (d.Zones.Count() == 0)
			d.Zones.Insert("Torso");
		d.DmgMin = GetGame().ConfigGetFloat(p + "vbABDmgMin");
		d.DmgMax = GetGame().ConfigGetFloat(p + "vbABDmgMax");
		if (d.DmgMax <= 0)
		{
			d.DmgMin = 1;
			d.DmgMax = 1;
		}
		d.EffectKind = GetGame().ConfigGetTextOut(p + "vbABEffect");
		d.EffectRadius = GetGame().ConfigGetFloat(p + "vbABEffectRadius");
		if (d.Radius <= 0)
			d.Radius = 2;
		if (d.Height <= 0)
			d.Height = 2;
		s_Defs.Set(type, d);
		return d;
	}
};

class VB_AB_AnomalyBase extends House
{
	static const int ST_IDLE = 0;
	static const int ST_ACTIVE = 1;
	static const int ST_RECHARGE = 2;
	static const float HEAT_CAP = 45.0;			// потолок температуры у Жарки/Пара (у костра 25)

	protected ref VB_AB_AnomalyDef m_VB_Def;
	protected int m_VB_Fire;					// синхр.: номер срабатывания
	protected int m_VB_Fire2;					// синхр.: номер второй фазы (Воронка)

	// клиент
	protected int m_VB_FireSeen;
	protected int m_VB_Fire2Seen;
	protected Particle m_VB_Ptc;
	protected Particle m_VB_Ptc2;
	protected SoundOnVehicle m_VB_Snd;
	protected ScriptedLightBase m_VB_Light;

	// сервер
	protected int m_VB_State;
	protected float m_VB_DmgMult = 1;
	protected float m_VB_PushRadius;
	protected Object m_VB_Owner;				// босс, чьё это поле (его аномалии не бьют)
	protected ref UniversalTemperatureSource m_VB_Heat;
	protected ref UniversalTemperatureSourceSettings m_VB_HeatSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_VB_HeatLambda;

	void VB_AB_AnomalyBase()
	{
		RegisterNetSyncVariableInt("m_VB_Fire");
		RegisterNetSyncVariableInt("m_VB_Fire2");
	}

	void ~VB_AB_AnomalyBase()
	{
		VB_StopVisuals();
	}

	override void EEInit()
	{
		super.EEInit();
		m_VB_Def = VB_AB_AnomalyDef.Get(GetType());

		if (!GetGame().IsDedicatedServer())
		{
			m_VB_FireSeen = m_VB_Fire;
			m_VB_Fire2Seen = m_VB_Fire2;
			VB_ClientIdle();
		}

		if (GetGame().IsServer())
		{
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
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ClientIdle);
			if (GetGame().IsServer())
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
		}
		VB_StopVisuals();
		super.EEDelete(parent);
	}

	void VB_SetOwner(Object owner)
	{
		m_VB_Owner = owner;
	}

	Object VB_GetOwner()
	{
		return m_VB_Owner;
	}

	float VB_GetRadius()
	{
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
	void VB_SeedLoot()
	{
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

	// ---------------------------------------------------------------- клиент
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_VB_Fire != m_VB_FireSeen)
		{
			m_VB_FireSeen = m_VB_Fire;
			VB_ClientActive();
		}
		if (m_VB_Fire2 != m_VB_Fire2Seen)
		{
			m_VB_Fire2Seen = m_VB_Fire2;
			VB_ClientActive2();
		}
	}

	protected void VB_StopVisuals()
	{
		if (m_VB_Ptc)
			m_VB_Ptc.Stop();
		m_VB_Ptc = null;
		if (m_VB_Ptc2)
			m_VB_Ptc2.Stop();
		m_VB_Ptc2 = null;
		if (m_VB_Light)
			m_VB_Light.FadeOut();
		m_VB_Light = null;
		if (m_VB_Snd && GetGame())
			GetGame().ObjectDelete(m_VB_Snd);
		m_VB_Snd = null;
	}

	protected void VB_PlayState(string ptc, string snd, float sndRange, bool sndLoop, string light, float fadeIn)
	{
		if (ptc != "")
		{
			int id = ParticleList.GetParticleIDByName(ptc);
			if (id > 0)
				m_VB_Ptc = Particle.PlayOnObject(id, this);
		}
		if (snd != "")
		{
			if (sndLoop)
				m_VB_Snd = PlaySoundLoop(snd, sndRange);
			else
				m_VB_Snd = PlaySound(snd, sndRange);
		}
		if (light != "")
		{
			typename t = light.ToType();
			if (t)
			{
				m_VB_Light = ScriptedLightBase.CreateLight(t, "0 0 0", fadeIn);
				if (m_VB_Light)
					m_VB_Light.AttachOnObject(this, "0 1 0", "0 0 0");
			}
		}
	}

	void VB_ClientIdle()
	{
		VB_StopVisuals();
		if (m_VB_Def)
			VB_PlayState(m_VB_Def.PtcIdle, m_VB_Def.SndIdle, m_VB_Def.SndIdleRange, m_VB_Def.SndIdleLoop, m_VB_Def.LightIdle, 0.5);
	}

	void VB_ClientActive()
	{
		if (!m_VB_Def)
			return;
		VB_StopVisuals();
		VB_PlayState(m_VB_Def.PtcActive, m_VB_Def.SndActive, m_VB_Def.SndActiveRange, m_VB_Def.SndActiveLoop, m_VB_Def.LightActive, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ClientIdle);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_ClientIdle, (m_VB_Def.ActiveTime + m_VB_Def.RechargeTime) * 1000, false);
	}

	void VB_ClientActive2()
	{
		if (!m_VB_Def || m_VB_Def.PtcActive2 == "")
			return;
		int id = ParticleList.GetParticleIDByName(m_VB_Def.PtcActive2);
		if (id > 0)
		{
			if (m_VB_Ptc)
				m_VB_Ptc.Stop();
			m_VB_Ptc = Particle.PlayOnObject(id, this);
		}
	}
};

// Опрос всех аномалий сервера одним таймером.
class VB_AB_AnomalyManager
{
	static const int TICK_MS = 250;
	static const float NEAR_PLAYERS_M = 80;	// зомби проверяем, только если рядом есть игрок
	protected static ref VB_AB_AnomalyManager s_Instance;
	protected ref array<VB_AB_AnomalyBase> m_List = new array<VB_AB_AnomalyBase>;

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
				a.VB_ServerTick(players, true);
		}
	}
};
