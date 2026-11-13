// Серверная часть детекторов (ТЗ 2.2): поиск находок, сектора, превращение находки в предмет.
// Клиентская часть (экраны, лампы, писк) — VB_AnomaliesBosses, VB_AB_DetectorBase.c и VB_AB_Detectors2.c.
//
// Что синхронизируется:
//   все     m_VB_Range 0..3      — сила сигнала ближайшей показанной находки;
//   Медведь m_VB_Dir 0..12       — сектор направления по 15°: 0 — нет, 6 — прямо, 1 — слева, 11 — справа;
//   Сварог,
//   Велес   m_VB_Slot1..4 0..740 — метки на экране: 0 — пусто, иначе 1 + шаг_расстояния * 37 + шаг_угла.

modded class VB_AB_DetectorBase
{
	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		if (GetGame().IsServer())
			VB_ServerScan();
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();
		if (GetGame().IsServer())
			VB_ServerReset();
	}


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
					EntityAI found = VB_AB_Finds.Materialize(f, p, GetType());
					GetGame().RPCSingleParam(this, RPC_FOUND, VB_AB_Finds.FoundParam(found, at), true);
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
};

// «Отклик»: только впереди
modded class VB_AB_Detector_Otklik
{
	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}
};


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

// Око: только впереди
modded class VB_AB_Detector_Oko
{

	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}
};

// Медведь: направление по секторам (как OOTS: только впереди, ±75°, шаг 15°)
modded class VB_AB_Detector_Medved
{

	override protected bool VB_InSector(PlayerBase p, vector target)
	{
		return VB_InFront(p, target);
	}

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
};

// Сварог и Велес: до 4 находок сразу. Находка, попавшая в слот, держит его, пока не станет предметом, не уйдёт
// за радиус или из полукруга впереди (±90°); свободные слоты занимают ближайшие.
modded class VB_AB_DetectorMulti
{
	protected ref array<ref VB_AB_Find> m_VB_SlotFinds;	// находка в каждом слоте


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
					EntityAI got = VB_AB_Finds.Materialize(f, p, GetType());
					m_VB_SlotFinds[i] = null;
					GetGame().RPCSingleParam(this, RPC_FOUND, VB_AB_Finds.FoundParam(got, at), true);
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
};
