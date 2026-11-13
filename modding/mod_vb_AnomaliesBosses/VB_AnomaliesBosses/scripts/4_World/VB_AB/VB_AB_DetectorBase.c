// Детекторы мода (ТЗ 2.2). Перенесены из OOTS: модель, звуки, лампа; логика своя.
// Пока детектор включён и в руках, сервер на каждом такте менеджера энергии ищет ближайшую разрешённую ему
// находку и выставляет синхронизированное расстояние m_VB_Range (серверный мод VB_AnomaliesBosses_Server,
// VB_AB_DetectorServer.c). Клиент по m_VB_Range пищит и светит.
// Здесь база и Отклик; Око, Медведь, Сварог и Велес — в VB_AB_Detectors2.c, чужие детекторы — VB_AB_GenericDetectors.c.

class VB_AB_DetectorBase extends ItemBase
{
	static const int R_NONE = 0;
	static const int R_FAR = 1;
	static const int R_MID = 2;
	static const int R_CLOSE = 3;
	static const int RPC_FOUND = 0x56414201;	// «VAB» 01: звук появления находки у клиентов
	// Направление на экранах (Медведь, Сварог, Велес): фаза анимации 0..1 = от -90° (слева) до +90° (справа)
	// от взгляда игрока. Если в игре стрелка или метка показывает зеркально — поставить true.
	static const bool DIR_MIRROR = false;

	protected int m_VB_Range;
	protected int m_VB_RangeSeen = -1;
	protected SoundOnVehicle m_VB_Snd;
	protected bool m_VB_Powered;		// клиент: детектор работает (между OnWorkStart и OnWorkStop)
	static const int IDLE_TICK_MS = 3000;
	static const float LOW_BATTERY = 0.15;
	protected int m_VB_TickN;

	void VB_AB_DetectorBase()
	{
		RegisterNetSyncVariableInt("m_VB_Range", 0, 3);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_IdleTick);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beep);
		}
		VB_StopSound();
		super.EEDelete(parent);
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}

	override void OnWorkStart()
	{
		super.OnWorkStart();
		SetAnimationPhase("top", 1.0);
		if (!GetGame().IsDedicatedServer())
		{
			m_VB_Powered = true;
			VB_OnPower(true);
			m_VB_TickN = 0;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_IdleTick);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_IdleTick, IDLE_TICK_MS, true);
		}
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();
		SetAnimationPhase("top", 0.0);
		if (!GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_IdleTick);
			VB_ClientUpdate(R_NONE);
			m_VB_Powered = false;
			VB_OnPower(false);
		}
	}

	// Видно, что детектор работает (ТЗ 8.16): пока сигнала нет, раз в IDLE_TICK_MS тихий щелчок; батарейка
	// садится (меньше LOW_BATTERY) — раз в два щелчка двойной писк погромче.
	protected void VB_IdleTick()
	{
		if (!m_VB_Powered)
			return;
		m_VB_TickN++;
		if (VB_BatteryLow() && m_VB_TickN % 2 == 0)
		{
			VB_Beep(0.6);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beep, 200, false, 0.6);
			return;
		}
		if (m_VB_RangeSeen == R_NONE)
			VB_Beep(0.12);
	}

	protected void VB_Beep(float vol)
	{
		EffectSound s = SEffectManager.PlaySoundOnObject("VB_AB_DetectorBeep_SoundSet", this);
		if (s)
		{
			s.SetSoundVolume(vol);
			s.SetAutodestroy(true);
		}
	}

	protected bool VB_BatteryLow()
	{
		if (!HasEnergyManager())
			return false;
		EntityAI src = GetCompEM().GetEnergySource();
		if (!src || !src.HasEnergyManager())
			return false;
		return src.GetCompEM().GetEnergy0To1() < LOW_BATTERY;
	}

	// ---------------------------------------------------------------- клиент
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_VB_Range != m_VB_RangeSeen)
			VB_ClientUpdate(m_VB_Range);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != RPC_FOUND || GetGame().IsDedicatedServer())
			return;
		VB_AB_FoundFx.OnRpc(ctx);		// звук, всплеск частиц и вспышка стихии (VB_AB_Materials.c)
	}

	protected void VB_StopSound()
	{
		if (m_VB_Snd && GetGame())
			GetGame().ObjectDelete(m_VB_Snd);
		m_VB_Snd = null;
	}

	protected void VB_ClientUpdate(int range)
	{
		m_VB_RangeSeen = range;
		VB_StopSound();
		switch (range)
		{
			case R_FAR:
				m_VB_Snd = PlaySoundLoop("vb_ab_detector_far", 10);
				break;
			case R_MID:
				m_VB_Snd = PlaySoundLoop("vb_ab_detector_mid", 10);
				break;
			case R_CLOSE:
				m_VB_Snd = PlaySoundLoop("vb_ab_detector_close", 10);
				break;
		}
		VB_UpdateVisuals(range);
	}

	protected void VB_UpdateVisuals(int range);

	// Клиент: детектор включился или выключился (экраны, индикаторы). У Отклика ничего не делает.
	protected void VB_OnPower(bool on)
	{
	}

	// Клиент: положить на секцию текстуру и материал состояния из config.cpp (cfg/detectors.hpp):
	// vbAB<part>Sel — индекс секции в hiddenSelections, vbAB<part><state>Tex / Mat — пути. Пустое не меняется.
	protected void VB_ApplyCfg(string part, string state)
	{
		string key = "vbAB" + part;
		if (IsRuined() || !ConfigIsExisting(key + "Sel"))
			return;
		int sel = ConfigGetInt(key + "Sel");
		// в локальные: два вызова одной функции в аргументах затирают результат первого
		string tex = VB_CfgText(key + state + "Tex");
		string mat = VB_CfgText(key + state + "Mat");
		VB_ApplySel(sel, tex, mat);
	}

	protected void VB_ApplySel(int sel, string tex, string mat)
	{
		if (sel < 0)
			return;
		if (tex != "")
			SetObjectTexture(sel, tex);
		if (mat != "")
			SetObjectMaterial(sel, mat);
	}

	protected string VB_CfgText(string entry)
	{
		if (!ConfigIsExisting(entry))
			return "";
		return ConfigGetString(entry);
	}
};

// «Отклик»: только расстояние и только впереди. Лампа на крышке горит тем ярче, чем ближе находка.
class VB_AB_Detector_Otklik extends VB_AB_DetectorBase
{
	static const int LAMP = 1;		// индекс секции Lampa в hiddenSelections
	static const string LAMP_OFF_TEX = "VB_AB\\t\\dox\\lampa_defaultl_empty_ca__________________________________________.paa";
	static const string LAMP_ON_TEX = "VB_AB\\t\\dox\\lampa_on_ca.paa";
	static const string LAMP_OFF_MAT = "VB_AB\\t\\dox\\oklik_lampa__________________________________________.rvmat";
	static const string LAMP_ON_MAT = "VB_AB\\t\\dox\\oklik_lampa_on.rvmat";

	protected VB_AB_Light_DetectorLamp m_VB_Light;

	override void EEDelete(EntityAI parent)
	{
		if (m_VB_Light)
			m_VB_Light.Destroy();
		super.EEDelete(parent);
	}

	override protected void VB_UpdateVisuals(int range)
	{
		if (range == R_NONE)
		{
			SetObjectTexture(LAMP, LAMP_OFF_TEX);
			SetObjectMaterial(LAMP, LAMP_OFF_MAT);
			if (m_VB_Light)
				m_VB_Light.Destroy();
			m_VB_Light = null;
			return;
		}
		SetObjectTexture(LAMP, LAMP_ON_TEX);
		SetObjectMaterial(LAMP, LAMP_ON_MAT);
		if (!m_VB_Light)
		{
			m_VB_Light = VB_AB_Light_DetectorLamp.Cast(ScriptedLightBase.CreateLight(VB_AB_Light_DetectorLamp, "0 0 0", 0.08));
			if (m_VB_Light)
				m_VB_Light.AttachOnMemoryPoint(this, "light", "");
		}
		if (m_VB_Light)
		{
			if (range == R_FAR)
				m_VB_Light.VB_SetIntensity(0.25);
			else if (range == R_MID)
				m_VB_Light.VB_SetIntensity(0.5);
			else
				m_VB_Light.VB_SetIntensity(1.0);
		}
	}
};
