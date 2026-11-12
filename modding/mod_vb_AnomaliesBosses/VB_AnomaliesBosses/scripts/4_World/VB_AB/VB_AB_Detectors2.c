// Детекторы Око, Медведь, Сварог и Велес (ТЗ 2.2). Модели и материалы OOTS, логика своя, по образцу OOTS
// (Detectors.c, Detectors_Server.c), но на нашей схеме находок VB_AB_Finds.
// Пути текстур и материалов экранов/ламп скрипт берёт из config.cpp (cfg/detectors.hpp, генератор tools/ab_detectors.py).
//
// Что синхронизируется:
//   все     m_VB_Range 0..3      — сила сигнала ближайшей показанной находки (писк, как у Отклика);
//   Медведь m_VB_Dir 0..12       — сектор направления по 15°: 0 — нет, 6 — прямо, 1 — слева, 11 — справа;
//   Сварог,
//   Велес   m_VB_Slot1..4 0..740 — метки на экране: 0 — пусто, иначе 1 + шаг_расстояния * 37 + шаг_угла.

// Доступ к списку находок для детекторов с несколькими целями. Наследник видит защищённый список VB_AB_Finds.
class VB_AB_FindsEx extends VB_AB_Finds
{
	// Запись ещё в списке: не стала предметом и не убрана вместе с хозяином.
	static bool Has(VB_AB_Find f)
	{
		if (!f || !s_Finds)
			return false;
		return s_Finds.Find(f) != -1;
	}

	// Все разрешённые детектору записи ближе radius, от ближней к дальней.
	static void InRadius(vector pos, float radius, string detector, int tier, array<VB_AB_Find> result)
	{
		result.Clear();
		if (!s_Finds)
			return;
		array<float> dists = new array<float>;
		int k;
		float d;
		foreach (VB_AB_Find f : s_Finds)
		{
			if (!f.AllowedFor(detector, tier))
				continue;
			d = vector.Distance(pos, f.m_Pos);
			if (d >= radius)
				continue;
			k = 0;
			while (k < dists.Count() && dists[k] <= d)
				k++;
			dists.InsertAt(d, k);
			result.InsertAt(f, k);
		}
	}
};

// ==================================================================== Око
// Как Отклик: только расстояние и только впереди (±75°). Своя индикация: экран светится, пока детектор
// включён; лампа мигает тем чаще и ярче, чем ближе находка (1,2 с / 0,65 с / 0,22 с).
class VB_AB_Detector_Oko extends VB_AB_DetectorBase
{
	static const int BLINK_ON_MS = 100;		// сколько горит лампа за одну вспышку

	protected VB_AB_Light_DetectorLamp m_VB_Lamp;
	protected int m_VB_BlinkMs;				// клиент: период мигания, 0 — не мигает

	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}

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
				m_VB_Lamp.AttachOnMemoryPoint(this, "light", "");
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

// ==================================================================== Медведь
// Расстояние и направление по секторам (как OOTS: только впереди, ±75°, шаг 15°). Экран показывает силу
// сигнала (пусто / слабый / средний / сильный), поворот экрана (анимация ekran, 0..180°) — сектор.
// В OOTS фаза считалась целочисленным делением и всегда была 0, стрелка не двигалась; здесь — по сектору.
class VB_AB_Detector_Medved extends VB_AB_DetectorBase
{
	static const int SECTOR_DEG = 15;
	static const int SECTORS = 12;			// фаза поворота = сектор / 12

	protected int m_VB_Dir;					// 0 — нет сигнала, 1..11 — сектор, 6 — прямо
	protected int m_VB_DirSeen = -1;
	protected ref TStringArray m_VB_RangeTex;	// клиент: vbABScreenRangeTex из config.cpp

	void VB_AB_Detector_Medved()
	{
		RegisterNetSyncVariableInt("m_VB_Dir", 0, SECTORS);
	}

	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}

	// ---------------------------------------------------------------- сервер
	override protected void VB_OnScanned(PlayerBase p, VB_AB_Find shown)
	{
		int dir = 0;
		if (p && shown)
		{
			float b = VB_Bearing(p, shown.m_Pos);
			dir = Math.Round((b + 90) / SECTOR_DEG);
			if (dir < 1)
				dir = 1;
			if (dir > SECTORS - 1)
				dir = SECTORS - 1;
		}
		if (dir != m_VB_Dir)
		{
			m_VB_Dir = dir;
			SetSynchDirty();
		}
	}

	override protected void VB_ServerReset()
	{
		super.VB_ServerReset();
		if (m_VB_Dir != 0)
		{
			m_VB_Dir = 0;
			SetSynchDirty();
		}
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

// ==================================================================== Сварог и Велес
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
	protected ref array<ref VB_AB_Find> m_VB_SlotFinds;	// сервер: находка в каждом слоте
	protected ref array<int> m_VB_SlotSeen;				// клиент: что уже показано

	void VB_AB_DetectorMulti()
	{
		RegisterNetSyncVariableInt("m_VB_Slot1", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot2", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot3", 0, DIST_N * ANG_N);
		RegisterNetSyncVariableInt("m_VB_Slot4", 0, DIST_N * ANG_N);
	}

	// ---------------------------------------------------------------- сервер
	override protected void VB_ServerScan()
	{
		VB_InitSlots();
		PlayerBase p = PlayerBase.Cast(GetHierarchyRootPlayer());
		bool active = p && p.IsAlive() && p.GetItemInHands() == this;
		VB_AB_DetectorCfg cfg = VB_AB_Cfg.Get().GetDetector(GetType());
		int i;
		int free;
		int packed;
		float d;
		float bearing;
		float nearest = -1;
		bool dirty = false;
		VB_AB_Find f;
		vector at;

		// 1. отпустить слоты: находки нет, она за радиусом или не впереди
		for (i = 0; i < SLOTS; i++)
		{
			f = m_VB_SlotFinds[i];
			if (!f)
				continue;
			if (!active || !VB_AB_FindsEx.Has(f) || !f.AllowedFor(GetType(), cfg.Tier) || !VB_SlotSees(p, f.m_Pos, cfg.Radius))
				m_VB_SlotFinds[i] = null;
		}

		// 2. занять свободные слоты ближайшими новыми находками
		if (active && VB_FreeSlot() != -1)
		{
			array<VB_AB_Find> cand = new array<VB_AB_Find>;
			VB_AB_FindsEx.InRadius(p.GetPosition(), cfg.Radius, GetType(), cfg.Tier, cand);
			foreach (VB_AB_Find c : cand)
			{
				free = VB_FreeSlot();
				if (free == -1)
					break;
				if (m_VB_SlotFinds.Find(c) != -1)
					continue;
				if (!VB_SlotSees(p, c.m_Pos, cfg.Radius))
					continue;
				m_VB_SlotFinds[free] = c;
			}
		}

		// 3. значения слотов; ближе SpawnDistance — находка становится предметом
		for (i = 0; i < SLOTS; i++)
		{
			packed = 0;
			f = m_VB_SlotFinds[i];
			if (f)
			{
				d = vector.Distance(p.GetPosition(), f.m_Pos);
				if (d < cfg.SpawnDistance)
				{
					at = f.m_Pos;
					VB_AB_Finds.Materialize(f, p, GetType());
					m_VB_SlotFinds[i] = null;
					GetGame().RPCSingleParam(this, RPC_FOUND, new Param1<vector>(at), true);
				}
				else
				{
					bearing = VB_Bearing(p, f.m_Pos);
					packed = VB_Pack(d, bearing, cfg);
					if (nearest < 0 || d < nearest)
						nearest = d;
				}
			}
			if (VB_SetSlot(i, packed))
				dirty = true;
		}

		int range = R_NONE;
		if (nearest >= 0)
			range = VB_RangeOf(nearest, cfg.Radius);
		if (range != m_VB_Range)
		{
			m_VB_Range = range;
			dirty = true;
		}
		if (dirty)
			SetSynchDirty();
	}

	override protected void VB_ServerReset()
	{
		super.VB_ServerReset();
		VB_InitSlots();
		bool dirty = false;
		for (int i = 0; i < SLOTS; i++)
		{
			m_VB_SlotFinds[i] = null;
			if (VB_SetSlot(i, 0))
				dirty = true;
		}
		if (dirty)
			SetSynchDirty();
	}

	protected void VB_InitSlots()
	{
		if (m_VB_SlotFinds)
			return;
		m_VB_SlotFinds = new array<ref VB_AB_Find>;
		for (int i = 0; i < SLOTS; i++)
			m_VB_SlotFinds.Insert(null);
	}

	protected int VB_FreeSlot()
	{
		for (int i = 0; i < SLOTS; i++)
		{
			if (!m_VB_SlotFinds[i])
				return i;
		}
		return -1;
	}

	// Находка в радиусе и в полукруге впереди игрока.
	protected bool VB_SlotSees(PlayerBase p, vector target, float radius)
	{
		if (vector.Distance(p.GetPosition(), target) >= radius)
			return false;
		float b = VB_Bearing(p, target);
		return Math.AbsFloat(b) <= SECTOR_DEG;
	}

	protected int VB_Pack(float dist, float bearing, VB_AB_DetectorCfg cfg)
	{
		float t = 1;
		if (cfg.Radius > cfg.SpawnDistance)
			t = Math.Clamp(Math.InverseLerp(cfg.Radius, cfg.SpawnDistance, dist), 0, 1);
		int dq = Math.Round(t * (DIST_N - 1));
		float ang = Math.Clamp(bearing, -SECTOR_DEG, SECTOR_DEG);
		int aq = Math.Round((ang + SECTOR_DEG) / 5);
		if (aq > ANG_N - 1)
			aq = ANG_N - 1;
		return 1 + dq * ANG_N + aq;
	}

	protected bool VB_SetSlot(int i, int v)
	{
		switch (i)
		{
			case 0:
				if (m_VB_Slot1 == v)
					return false;
				m_VB_Slot1 = v;
				return true;
			case 1:
				if (m_VB_Slot2 == v)
					return false;
				m_VB_Slot2 = v;
				return true;
			case 2:
				if (m_VB_Slot3 == v)
					return false;
				m_VB_Slot3 = v;
				return true;
			case 3:
				if (m_VB_Slot4 == v)
					return false;
				m_VB_Slot4 = v;
				return true;
		}
		return false;
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

// Сварог: метки зелёные.
class VB_AB_Detector_Svarog extends VB_AB_DetectorMulti
{
};

// Велес: то же, другая модель, метки оранжевые.
class VB_AB_Detector_Veles extends VB_AB_DetectorMulti
{
};
