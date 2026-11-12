// Детекторы мода (ТЗ 2.2). Перенесены из OOTS: модель, звуки, лампа; логика своя.
// Пока детектор включён и в руках, сервер на каждом такте менеджера энергии (OnWork, updateInterval в config.cpp)
// ищет ближайшую разрешённую ему находку (VB_AB_Finds) и выставляет синхронизированное расстояние m_VB_Range.
// Ближе SpawnDistance — находка становится предметом. Клиент по m_VB_Range пищит и светит.
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

	void VB_AB_DetectorBase()
	{
		RegisterNetSyncVariableInt("m_VB_Range", 0, 3);
	}

	override void EEDelete(EntityAI parent)
	{
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
		}
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();
		SetAnimationPhase("top", 0.0);
		if (GetGame().IsServer())
			VB_ServerReset();
		if (!GetGame().IsDedicatedServer())
		{
			VB_ClientUpdate(R_NONE);
			m_VB_Powered = false;
			VB_OnPower(false);
		}
	}

	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		if (GetGame().IsServer())
			VB_ServerScan();
	}

	// ---------------------------------------------------------------- сервер
	// Ближайшая разрешённая находка в секторе: расстояние в m_VB_Range, ближе SpawnDistance — предмет.
	// Сварог и Велес ведут до 4 находок сразу и переопределяют весь метод (VB_AB_Detectors2.c).
	protected void VB_ServerScan()
	{
		int range = R_NONE;
		VB_AB_Find shown;
		PlayerBase p = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (p && p.IsAlive() && p.GetItemInHands() == this)
		{
			VB_AB_DetectorCfg cfg = VB_AB_Cfg.Get().GetDetector(GetType());
			float dist;
			VB_AB_Find f = VB_AB_Finds.Nearest(p.GetPosition(), cfg.Radius, GetType(), cfg.Tier, dist);
			if (f && VB_InSector(p, f.m_Pos))
			{
				if (dist < cfg.SpawnDistance)
				{
					vector at = f.m_Pos;
					VB_AB_Finds.Materialize(f, p, GetType());
					GetGame().RPCSingleParam(this, RPC_FOUND, new Param1<vector>(at), true);
				}
				else
				{
					range = VB_RangeOf(dist, cfg.Radius);
					shown = f;
				}
			}
		}
		if (range != m_VB_Range)
		{
			m_VB_Range = range;
			SetSynchDirty();
		}
		VB_OnScanned(p, shown);
	}

	// Для наследников: находка, о которой детектор сейчас сигналит (null — сигнала нет). Медведь берёт направление.
	protected void VB_OnScanned(PlayerBase p, VB_AB_Find shown)
	{
	}

	// Детектор выключился: сбросить синхронизированный сигнал.
	protected void VB_ServerReset()
	{
		if (m_VB_Range != R_NONE)
		{
			m_VB_Range = R_NONE;
			SetSynchDirty();
		}
	}

	// Сила сигнала по расстоянию: ближе 40 % радиуса — близко, ближе 60 % — средне, дальше — далеко.
	static int VB_RangeOf(float dist, float radius)
	{
		if (dist < radius * 0.4)
			return R_CLOSE;
		if (dist < radius * 0.6)
			return R_MID;
		return R_FAR;
	}

	// Пеленг на точку от взгляда игрока, градусы -180..180: плюс — справа (по часовой стрелке, если смотреть сверху).
	static float VB_Bearing(PlayerBase p, vector target)
	{
		vector to = target - p.GetPosition();
		to[1] = 0;
		if (to.LengthSq() < 0.0001)
			return 0;
		vector look = p.GetDirection();
		look[1] = 0;
		// результаты VectorToAngles — в локальные: два вызова в одном выражении затирают друг друга
		vector toAng = to.VectorToAngles();
		vector lookAng = look.VectorToAngles();
		float b = Math.NormalizeAngle(toAng[0] - lookAng[0]);
		if (b > 180)
			b = b - 360;
		return b;
	}

	// Видит ли детектор находку в этой стороне. Базовый — во все стороны.
	protected bool VB_InSector(PlayerBase p, vector target)
	{
		return true;
	}

	// Впереди игрока, в конусе ±75°.
	protected bool VB_InFront(PlayerBase p, vector target)
	{
		vector dir = p.GetDirection();
		dir[1] = 0;
		vector to = target - p.GetPosition();
		to[1] = 0;
		if (to.Length() < 0.5)
			return true;
		dir.Normalize();
		to.Normalize();
		return vector.Dot(dir, to) > 0.26;
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
		Param1<vector> pos = new Param1<vector>(vector.Zero);
		if (!ctx.Read(pos))
			return;
		EffectSound s = SEffectManager.PlaySound("VB_AB_Found_SoundSet", pos.param1);
		if (s)
			s.SetAutodestroy(true);
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

	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}

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
