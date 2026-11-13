// Аномалии мода (ТЗ 2.1). Все числа и ресурсы — из config.cpp (cfg/anomalies.hpp, генерирует
// tools/ab_anomalies.py), поэтому один класс на все 22 аномалии.
//
// Здесь только то, что видит игрок: частицы, звук и свет простоя и срабатывания. Срабатывание сервер передаёт
// счётчиком m_VB_Fire (синхронизируется), вторую фазу Воронки — m_VB_Fire2. Урон, перезарядка, особые действия
// и материалы — в серверном моде VB_AnomaliesBosses_Server (VB_AB_AnomalyServer.c).

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
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ClientIdle);
		VB_StopVisuals();
		super.EEDelete(parent);
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
