// Детекторы Огонёк, Пеленг, Сито и Невод (ТЗ 2.2). Модели и материалы OOTS, логика своя, по образцу OOTS.
// Пути текстур и материалов экранов/ламп скрипт берёт из config.cpp (cfg/detectors.hpp, генератор tools/ab_detectors.py).
// Здесь только экраны, лампы и метки; что и как ищется — в серверном моде (VB_AB_DetectorServer.c).
//
// Что синхронизируется:
//   все     m_VB_Range 0..3      — сила сигнала ближайшей показанной находки (писк, как у Лозы);
//   Пеленг  m_VB_Dir 0..12       — сектор направления по 15°: 0 — нет, 6 — прямо, 1 — слева, 11 — справа;
//   Сито,
//   Невод   m_VB_Slot1..4 0..740 — метки на экране: 0 — пусто, иначе 1 + шаг_расстояния * 37 + шаг_угла.

// ==================================================================== Огонёк
// Как Лоза: только расстояние и только впереди (±75°). Своя индикация: экран светится, пока детектор
// включён; лампа мигает тем чаще и ярче, чем ближе находка (1,2 с / 0,65 с / 0,22 с).
class VB_AB_Detector_Ogonek extends VB_AB_DetectorBase
{
	static const int BLINK_ON_MS = 100;		// сколько горит лампа за одну вспышку

	protected VB_AB_Light_DetectorLamp m_VB_Lamp;
	protected int m_VB_BlinkMs;				// клиент: период мигания, 0 — не мигает


	override void EEDelete(EntityAI parent)
	{
		VB_StopBlink(false);
		super.EEDelete(parent);
	}

	override protected void VB_OnPower(bool on)
	{
		if (on)
			VB_ApplyCfg("Screen", "On");
		else
			VB_ApplyCfg("Screen", "Off");
	}

	override protected void VB_UpdateVisuals(int range)
	{
		int ms = 0;
		if (range == R_FAR)
			ms = 1200;
		else if (range == R_MID)
			ms = 650;
		else if (range == R_CLOSE)
			ms = 220;
		if (ms == m_VB_BlinkMs)
			return;
		VB_StopBlink(true);
		if (ms <= 0)
			return;
		m_VB_BlinkMs = ms;
		VB_BlinkOn();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BlinkOn, ms, true);
	}

	protected void VB_BlinkOn()
	{
		VB_ApplyCfg("Lamp", "On");
		if (!m_VB_Lamp)
		{
			m_VB_Lamp = VB_AB_Light_DetectorLamp.Cast(ScriptedLightBase.CreateLight(VB_AB_Light_DetectorLamp, "0 0 0", 0.05));
			if (m_VB_Lamp)
			{
				m_VB_Lamp.VB_SetColor(VB_AB_Light_DetectorLamp.OGONEK);
				m_VB_Lamp.AttachOnMemoryPoint(this, "light", "");
			}
		}
		if (m_VB_Lamp)
		{
			float k = 0.25;
			if (m_VB_RangeSeen == R_MID)
				k = 0.5;
			else if (m_VB_RangeSeen == R_CLOSE)
				k = 1.0;
			m_VB_Lamp.VB_SetIntensity(k);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BlinkOff, BLINK_ON_MS, false);
	}

	protected void VB_BlinkOff()
	{
		VB_ApplyCfg("Lamp", "Off");
		if (m_VB_Lamp)
			m_VB_Lamp.VB_SetIntensity(0);
	}

	// visual = false — объект удаляется, секции не трогаем
	protected void VB_StopBlink(bool visual)
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BlinkOn);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BlinkOff);
		}
		m_VB_BlinkMs = 0;
		if (m_VB_Lamp)
			m_VB_Lamp.Destroy();
		m_VB_Lamp = null;
		if (visual)
			VB_ApplyCfg("Lamp", "Off");
	}
};

// ==================================================================== Пеленг
// Расстояние и направление по секторам (как OOTS: только впереди, ±75°, шаг 15°). Экран показывает силу
// сигнала (пусто / слабый / средний / сильный), поворот экрана (анимация ekran, 0..180°) — сектор.
// В OOTS фаза считалась целочисленным делением и всегда была 0, стрелка не двигалась; здесь — по сектору.
class VB_AB_Detector_Peleng extends VB_AB_DetectorBase
{
	static const int SECTOR_DEG = 15;
	static const int SECTORS = 12;			// фаза поворота = сектор / 12

	protected int m_VB_Dir;					// 0 — нет сигнала, 1..11 — сектор, 6 — прямо
	protected int m_VB_DirSeen = -1;
	protected ref TStringArray m_VB_RangeTex;	// клиент: vbABScreenRangeTex из config.cpp

	void VB_AB_Detector_Peleng()
	{
		RegisterNetSyncVariableInt("m_VB_Dir", 0, SECTORS);
	}


	// ---------------------------------------------------------------- клиент
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_VB_Dir != m_VB_DirSeen)
			VB_ClientDir();
	}

	protected void VB_ClientDir()
	{
		m_VB_DirSeen = m_VB_Dir;
		if (m_VB_Dir <= 0)
			return;			// на пустом экране сектора нет, поворот не важен
		float phase = m_VB_Dir;
		phase = phase / SECTORS;
		if (DIR_MIRROR)
			phase = 1 - phase;
		SetAnimationPhase("ekran", phase);
	}

	override protected void VB_OnPower(bool on)
	{
		VB_Screen();
	}

	override protected void VB_UpdateVisuals(int range)
	{
		VB_Screen();
	}

	// Экран и дисплей: выключен — EkranOff; включён — картинка силы сигнала.
	protected void VB_Screen()
	{
		if (IsRuined())
			return;
		string tex;
		string mat;
		if (m_VB_Powered)
		{
			tex = VB_RangeTex(m_VB_RangeSeen);
			mat = VB_CfgText("vbABScreenOnMat");
		}
		else
		{
			tex = VB_CfgText("vbABScreenOffTex");
			mat = VB_CfgText("vbABScreenOffMat");
		}
		VB_ApplySel(ConfigGetInt("vbABScreenSel"), tex, mat);
		if (ConfigIsExisting("vbABDisplaySel"))
			VB_ApplySel(ConfigGetInt("vbABDisplaySel"), tex, "");
	}

	protected string VB_RangeTex(int range)
	{
		if (!m_VB_RangeTex)
		{
			m_VB_RangeTex = new TStringArray;
			ConfigGetTextArray("vbABScreenRangeTex", m_VB_RangeTex);
		}
		if (range < 0)
			range = 0;
		if (range >= m_VB_RangeTex.Count())
			return "";
		return m_VB_RangeTex[range];
	}
};

// ==================================================================== Сито и Невод
// До 4 находок сразу: метки art1..art4 на экране. Как в OOTS: находка, попавшая в слот, держит его, пока не
// станет предметом, не уйдёт за радиус или из полукруга впереди (±90°); свободные слоты занимают ближайшие.
// Метка: art*_translate — расстояние (0 — край радиуса, 1 — SpawnDistance), art*_rotate — угол (0 — -90°,
// 0,5 — прямо, 1 — +90°), art*_hide — скрыта. Писк — по ближайшей метке (в OOTS у них звука не было).
class VB_AB_DetectorMulti extends VB_AB_DetectorBase
{
	static const int SLOTS = 4;
	static const int DIST_N = 20;			// шагов расстояния
	static const int ANG_N = 37;			// шагов угла по 5° (-90..+90)
	static const float SECTOR_DEG = 90;	// полукруг впереди

	protected int m_VB_Slot1;
	protected int m_VB_Slot2;
	protected int m_VB_Slot3;
	protected int m_VB_Slot4;
	protected ref array<int> m_VB_SlotSeen;				// клиент: что уже показано

	void VB_AB_DetectorMulti()
	{
		RegisterNetSyncVariableInt("m_VB_Slot1", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot2", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot3", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot4", 0, DIST_N * ANG_N);
	}

	protected int VB_GetSlot(int i)
	{
		switch (i)
		{
			case 0:
				return m_VB_Slot1;
			case 1:
				return m_VB_Slot2;
			case 2:
				return m_VB_Slot3;
			case 3:
				return m_VB_Slot4;
		}
		return 0;
	}

	// ---------------------------------------------------------------- клиент
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_ClientSlots(false);
	}

	override protected void VB_OnPower(bool on)
	{
		if (on)
		{
			VB_ApplyCfg("Screen", "On");
			VB_ApplyCfg("Emiss", "On");
		}
		else
		{
			VB_ApplyCfg("Screen", "Off");
			VB_ApplyCfg("Emiss", "Off");
		}
		VB_ClientSlots(true);
	}

	protected void VB_ClientSlots(bool force)
	{
		int i;
		int v;
		bool wasHidden;
		if (!m_VB_SlotSeen)
		{
			m_VB_SlotSeen = new array<int>;
			for (i = 0; i < SLOTS; i++)
				m_VB_SlotSeen.Insert(-1);
		}
		for (i = 0; i < SLOTS; i++)
		{
			v = VB_GetSlot(i);
			if (!m_VB_Powered)
				v = 0;
			if (!force && v == m_VB_SlotSeen[i])
				continue;
			wasHidden = m_VB_SlotSeen[i] <= 0;
			m_VB_SlotSeen[i] = v;
			VB_ShowSlot(i + 1, v, wasHidden);
		}
	}

	// Метка artN: v = 0 — скрыть. Появившуюся метку ставим на место сразу, без проезда от старого места.
	protected void VB_ShowSlot(int n, int v, bool instant)
	{
		string art = "art" + n.ToString();
		if (v <= 0)
		{
			SetAnimationPhase(art + "_hide", 1);
			return;
		}
		int x = v - 1;
		int dq = x / ANG_N;
		int aq = x - dq * ANG_N;
		float tr = dq;
		tr = tr / (DIST_N - 1);
		float rot = aq;
		rot = rot / (ANG_N - 1);
		if (DIR_MIRROR)
			rot = 1 - rot;
		if (instant)
		{
			SetAnimationPhaseNow(art + "_translate", tr);
			SetAnimationPhaseNow(art + "_rotate", rot);
		}
		else
		{
			SetAnimationPhase(art + "_translate", tr);
			SetAnimationPhase(art + "_rotate", rot);
		}
		SetAnimationPhase(art + "_hide", 0);
	}
};

// Сито: метки голубые (цвет в модели, ab_detectors.py MODEL_PATCHES).
class VB_AB_Detector_Sito extends VB_AB_DetectorMulti
{
};

// Невод: то же, другая модель, метки белые.
class VB_AB_Detector_Nevod extends VB_AB_DetectorMulti
{
};
