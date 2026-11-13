// Изнанка на домах. Живое сердце на клиенте находит дома рядом и «одевает» их локальными объектами,
// как деревья обвивками:
//   на наружном углу дома, ближнем к сердцу, корни переваливают через ребро на обе стены (VB_RiftWall_Corner*);
//   на стенах, обращённых к сердцу, корни ползут из земли вверх (VB_RiftWall_Low/Tall/Wide*) — только там,
//   где стена тянется на всю ширину корня (лучи по краям и сверху; окна и двери не мешают, если стена есть);
//   перед первыми корнями пульсирует красная подсветка — ночью фасад мерцает в ритме сердца.
// Щупальца на дома автоматически не вешаются: криво встают на сложных крышах, их ставят вручную редактором.
// Раскладка считается от координат дома — у всех игроков одинаковая. По сети ничего не идёт; всё бьётся
// в ритме сердца, днём спит и гаснет вместе с сердцем (корни связаны с ним, как обвивки деревьев).
// Одевание — не в EEInit, а когда игрок подошёл к сердцу (VB_RiftHeart.VB_DecorCheck, VB_RiftBase.c): дома
// выбираются и занимаются сразу, а одеваются по одному за шаг, после обвивок деревьев (баг №37).

modded class VB_RiftHeart
{
	static const bool DEBUG_HOUSES = false;		// true — строка в лог клиента на каждый одетый дом
	static const float HOUSE_RADIUS = 25.0;		// м: дома, чья коробка ближе этого к сердцу
	static const int HOUSE_MAX = 3;				// не больше стольких домов на сердце
	static const float HOUSE_MIN_SIZE = 4.0;	// м: уже/короче — не дом (будки, столбы, заборы)
	static const float HOUSE_MAX_SIZE = 45.0;
	static const float HOUSE_MIN_HEIGHT = 2.5;
	static const float WALL_OUT = 2.0;			// м: лучи к стене пускаем снаружи коробки дома на таком отступе
	static const float WALL_PLANE_TOL = 0.45;	// м: точки стены дальше этого от плоскости — уже не та стена (проём, выступ)
	static const float CORNER_FACE = 2.0;		// м: на сколько угловые корни заходят на каждую стену
	static const float HOUSE_LIGHT_BASE = 0.5;
	static const float HOUSE_LIGHT_BEAT = 2.2;
	static const float HOUSE_LIGHT_RADIUS = 6.0;
	static const ref TStringArray HOUSE_SKIP = {
		"Land_Wall_", "Land_Fence", "Land_Wreck", "Land_wreck", "Land_Misc_", "Land_Power", "Land_Rail",
		"Land_Pipe", "Land_Mil_Fence", "Land_Container", "Land_Tank", "Land_Ship", "Land_Bridge"
	};
	static const ref TStringArray WALL_LOW = {"VB_RiftWall_Low1", "VB_RiftWall_Low2", "VB_RiftWall_Low3", "VB_RiftWall_Wide1", "VB_RiftWall_Wide2"};
	static const ref TStringArray WALL_TALL = {"VB_RiftWall_Tall1", "VB_RiftWall_Tall2", "VB_RiftWall_Wide1", "VB_RiftWall_Wide2", "VB_RiftWall_Low3"};
	static const ref TStringArray WALL_CORNER = {"VB_RiftWall_Corner1", "VB_RiftWall_Corner2", "VB_RiftWall_Corner3"};

	protected static ref array<Object> s_VB_DressedHouses;	// дома, уже одетые каким-то сердцем у этого клиента

	protected ref array<Object> m_VB_Houses;			// дома, занятые этим сердцем (одетые и ждущие очереди)
	protected ref array<Object> m_VB_HouseQueue;		// ещё не одетые, ближние первыми
	protected ref array<vector> m_VB_HouseLightPos;
	protected ref array<float> m_VB_HouseLightRad;
	protected ref array<VB_RiftLight> m_VB_HouseLights;
	protected ref array<vector> m_VB_DecorSpots;		// где уже стоят корни на стенах этого дома
	protected ref array<float> m_VB_DecorHalf;			// и их полуширина
	protected vector m_VB_LastDecorPos;					// последний поставленный корень и куда он смотрит
	protected vector m_VB_LastDecorDir;

	// после деревьев: выбрать и занять дома (дёшево), одевать — по одному в VB_DecorNext
	override protected void VB_DecorPrepare()
	{
		super.VB_DecorPrepare();
		VB_PickHouses();
	}

	override protected bool VB_DecorNext()
	{
		if (super.VB_DecorNext())
			return true;		// сначала все деревья
		if (!m_VB_HouseQueue || m_VB_HouseQueue.Count() == 0)
			return false;

		Object house = m_VB_HouseQueue[0];
		m_VB_HouseQueue.RemoveOrdered(0);
		if (house)
			VB_DressHouse(house);
		return true;
	}

	override protected void VB_Cleanup()
	{
		if (m_VB_HouseQueue)
			m_VB_HouseQueue.Clear();
		VB_HouseTick(false, 0, 0);
		if (m_VB_Houses && s_VB_DressedHouses)
		{
			foreach (Object house : m_VB_Houses)
			{
				int idx = s_VB_DressedHouses.Find(house);
				if (idx >= 0)
					s_VB_DressedHouses.Remove(idx);
			}
			m_VB_Houses.Clear();
		}
		super.VB_Cleanup();		// корни на домах лежат в m_VB_Wraps и удаляются вместе с обвивками
	}

	// подсветка фасадов: включает и пульсирует VB_RiftPulse (ночью, игрок рядом, сердце живо)
	override void VB_HouseTick(bool on, float k, float night)
	{
		if (!m_VB_HouseLightPos || m_VB_HouseLightPos.Count() == 0)
			return;

		if (on && m_VB_HouseLights.Count() < m_VB_HouseLightPos.Count())
		{
			// дома одеваются по одному: подсветку только что одетого добавляем и к уже горящей
			for (int i = m_VB_HouseLights.Count(); i < m_VB_HouseLightPos.Count(); i++)
			{
				VB_RiftLight light = VB_RiftLight.Cast(ScriptedLightBase.CreateLight(VB_RiftLight, m_VB_HouseLightPos[i]));
				if (light)
					light.SetRadiusTo(m_VB_HouseLightRad[i]);
				m_VB_HouseLights.Insert(light);		// и пустой: номера совпадают с m_VB_HouseLightPos
			}
		}
		else if (!on && m_VB_HouseLights.Count() > 0)
		{
			foreach (VB_RiftLight old : m_VB_HouseLights)
			{
				if (old)
					old.Destroy();
			}
			m_VB_HouseLights.Clear();
		}
		if (!on)
			return;

		float brightness = (HOUSE_LIGHT_BASE + HOUSE_LIGHT_BEAT * k) * night;
		foreach (VB_RiftLight lit : m_VB_HouseLights)
		{
			if (lit)
				lit.SetBrightnessTo(brightness);
		}
	}

	// 0..1 от точки и номера: одинаково у всех игроков
	static float VB_Hash(vector p, int salt)
	{
		float v = Math.Sin(p[0] * 12.9898 + p[2] * 78.233 + salt * 37.719) * 43758.5453;
		return v - Math.Floor(v);
	}

	static float VB_HalfWidth(string type)
	{
		float hw = g_Game.ConfigGetFloat("CfgVehicles " + type + " vbHalfWidth");
		if (hw <= 0)
			hw = 1.5;
		return hw;
	}

	protected bool VB_IsHouse(Object obj)
	{
		if (!obj || !obj.IsBuilding() || obj.IsInherited(VB_RiftBase) || obj.IsTree() || obj.IsBush())
			return false;

		string type = obj.GetType();
		foreach (string prefix : HOUSE_SKIP)
		{
			if (type.IndexOf(prefix) == 0)
				return false;
		}

		vector mm[2];
		if (!obj.GetCollisionBox(mm))
			return false;
		float sx = mm[1][0] - mm[0][0];
		float sy = mm[1][1] - mm[0][1];
		float sz = mm[1][2] - mm[0][2];
		if (Math.Min(sx, sz) < HOUSE_MIN_SIZE || Math.Max(sx, sz) > HOUSE_MAX_SIZE || sy < HOUSE_MIN_HEIGHT)
			return false;
		return true;
	}

	// расстояние по земле от сердца до коробки дома (0 — сердце внутри коробки)
	protected float VB_DistToBox(Object b, vector lo, vector hi, vector p)
	{
		vector lp = b.WorldToModel(p);
		float dx = Math.Max(Math.Max(lo[0] - lp[0], 0), lp[0] - hi[0]);
		float dz = Math.Max(Math.Max(lo[2] - lp[2], 0), lp[2] - hi[2]);
		return Math.Sqrt(dx * dx + dz * dz);
	}

	// до HOUSE_MAX ближайших домов: занять (другое сердце их уже не возьмёт) и поставить в очередь на одевание
	protected void VB_PickHouses()
	{
		m_VB_Houses = new array<Object>;
		m_VB_HouseQueue = new array<Object>;
		m_VB_HouseLightPos = new array<vector>;
		m_VB_HouseLightRad = new array<float>;
		m_VB_HouseLights = new array<VB_RiftLight>;
		if (!m_VB_Wraps)
			m_VB_Wraps = new array<Object>;
		if (!s_VB_DressedHouses)
			s_VB_DressedHouses = new array<Object>;

		vector hp = GetPosition();
		array<Object> found = new array<Object>;
		g_Game.GetObjectsAtPosition(hp, HOUSE_RADIUS + HOUSE_MAX_SIZE * 0.5, found, null);

		array<Object> houses = new array<Object>;
		array<float> dists = new array<float>;
		vector mm[2];
		float d;
		foreach (Object obj : found)
		{
			if (!VB_IsHouse(obj) || s_VB_DressedHouses.Find(obj) >= 0)
				continue;
			obj.GetCollisionBox(mm);
			d = VB_DistToBox(obj, mm[0], mm[1], hp);
			if (d > HOUSE_RADIUS)
				continue;
			houses.Insert(obj);
			dists.Insert(d);
		}

		// ближние первыми
		int best;
		for (int n = 0; n < HOUSE_MAX && houses.Count() > 0; n++)
		{
			best = 0;
			for (int i = 1; i < houses.Count(); i++)
			{
				if (dists[i] < dists[best])
					best = i;
			}
			Object house = houses[best];
			houses.Remove(best);
			dists.Remove(best);

			s_VB_DressedHouses.Insert(house);
			m_VB_Houses.Insert(house);
			m_VB_HouseQueue.Insert(house);
		}
	}

	// сторона коробки дома: 0 +X, 1 -X, 2 +Z, 3 -Z (в осях модели дома)
	protected vector VB_SideNormal(int side)
	{
		if (side == 0)
			return "1 0 0";
		if (side == 1)
			return "-1 0 0";
		if (side == 2)
			return "0 0 1";
		return "0 0 -1";
	}

	protected vector VB_WorldNormal(Object b, int side)
	{
		vector wn = b.VectorToParent(VB_SideNormal(side));
		wn[1] = 0;
		wn.Normalize();
		return wn;
	}

	// точка модели дома: на стороне side, отступ offs наружу от коробки, смещение u вдоль стороны от её середины
	protected vector VB_SidePoint(vector lo, vector hi, int side, float offs, float u)
	{
		float midX = (lo[0] + hi[0]) * 0.5;
		float midZ = (lo[2] + hi[2]) * 0.5;
		if (side == 0)
			return Vector(hi[0] + offs, 0, midZ + u);
		if (side == 1)
			return Vector(lo[0] - offs, 0, midZ + u);
		if (side == 2)
			return Vector(midX + u, 0, hi[2] + offs);
		return Vector(midX + u, 0, lo[2] - offs);
	}

	protected float VB_SideHalf(vector lo, vector hi, int side)
	{
		if (side < 2)
			return (hi[2] - lo[2]) * 0.5;
		return (hi[0] - lo[0]) * 0.5;
	}

	// луч снаружи к стене стороны side в точке u на высоте h над землёй: расстояние до этого дома или -1
	protected float VB_WallDist(Object b, vector lo, vector hi, int side, float u, float h, out vector hit)
	{
		vector wn = VB_WorldNormal(b, side);
		vector start = b.ModelToWorld(VB_SidePoint(lo, hi, side, WALL_OUT, u));
		start[1] = g_Game.SurfaceY(start[0], start[2]) + h;
		vector dir;
		int comp;
		set<Object> results = new set<Object>;
		if (!DayZPhysics.RaycastRV(start, start - wn * (WALL_OUT + 3.0), hit, dir, comp, results, null, null, false, false))
			return -1;
		if (results.Find(b) < 0)
			return -1;
		return vector.Distance(start, hit);
	}

	// в точке u на высоте h та же плоскость стены, что и d0
	protected bool VB_WallAt(Object b, vector lo, vector hi, int side, float u, float h, float d0)
	{
		vector hit;
		float d = VB_WallDist(b, lo, hi, side, u, h, hit);
		return d >= 0 && Math.AbsFloat(d - d0) < WALL_PLANE_TOL;
	}

	protected bool VB_FreeFor(vector p, float hw)
	{
		for (int i = 0; i < m_VB_DecorSpots.Count(); i++)
		{
			if (VB_RiftNestUtil.VB_DistXZ(p, m_VB_DecorSpots[i]) < hw + m_VB_DecorHalf[i] + 0.2)
				return false;
		}
		return true;
	}

	protected void VB_DressHouse(Object b)
	{
		m_VB_DecorSpots = new array<vector>;
		m_VB_DecorHalf = new array<float>;

		vector mm[2];
		b.GetCollisionBox(mm);
		vector lo = mm[0];
		vector hi = mm[1];
		vector bp = b.GetPosition();
		vector toHeart = GetPosition() - bp;
		toHeart[1] = 0;
		toHeart.Normalize();

		// стороны по тому, насколько они смотрят на сердце
		array<int> sides = {0, 1, 2, 3};
		array<float> facing = new array<float>;
		for (int s = 0; s < 4; s++)
		{
			facing.Insert(vector.Dot(VB_WorldNormal(b, s), toHeart));
		}
		for (int a = 0; a < 4; a++)
		{
			for (int c = a + 1; c < 4; c++)
			{
				if (facing[c] > facing[a])
				{
					facing.SwapItems(a, c);
					sides.SwapItems(a, c);
				}
			}
		}

		// 1. угол дома к сердцу (ближние к сердцу сторона ±X и сторона ±Z)
		int sideX = -1;
		int sideZ = -1;
		for (int q = 0; q < 4; q++)
		{
			if (sides[q] < 2 && sideX < 0)
				sideX = sides[q];
			if (sides[q] >= 2 && sideZ < 0)
				sideZ = sides[q];
		}
		bool hasGlow = false;
		vector glowPos;
		int corners = 0;
		if (VB_TryCorner(b, lo, hi, sideX, sideZ))
		{
			corners++;
			hasGlow = true;
			glowPos = m_VB_LastDecorPos + m_VB_LastDecorDir * 1.6 + "0 1.5 0";
		}

		// 2. корни по стенам: 2–3 вместе с углом, сначала на стенах к сердцу
		int want = 2;
		if (VB_Hash(bp, 1) < 0.5)
			want = 3;
		want -= corners;
		int walls = 0;
		for (int w = 0; w < 6 && walls < want; w++)
		{
			if (!VB_TryWall(b, lo, hi, sides[w % 3], 10 + w * 7))
				continue;
			walls++;
			if (!hasGlow)
			{
				hasGlow = true;
				glowPos = m_VB_LastDecorPos + m_VB_LastDecorDir * 1.3 + "0 1.5 0";
			}
		}

		// красная подсветка перед первыми корнями: фасад мерцает в ритме сердца (свет внутри дома не виден —
		// у большинства домов Сахала окна глухие)
		if (hasGlow)
		{
			m_VB_HouseLightPos.Insert(glowPos);
			m_VB_HouseLightRad.Insert(HOUSE_LIGHT_RADIUS);
		}

		if (DEBUG_HOUSES)
		{
			string houseType = b.GetType();
			string cornersStr = corners.ToString();
			string wallsStr = walls.ToString();
			Print("[VB_Rift] дом " + houseType + ": угол " + cornersStr + ", корней на стенах " + wallsStr);
		}
	}

	// корни по стене: стена на всю ширину корня (края на 1 м, сверху на 2.4 м — та же плоскость)
	protected bool VB_TryWall(Object b, vector lo, vector hi, int side, int salt)
	{
		float half = VB_SideHalf(lo, hi, side);
		vector bp = b.GetPosition();
		vector wn = VB_WorldNormal(b, side);
		vector hit;
		float d0;
		float u;
		float lim;
		float hw;
		string type;
		int upper;
		for (int a = 0; a < 5; a++)
		{
			// середина: стена на 1 м рядом с коробкой (не дверь, не проём)
			u = (VB_Hash(bp, salt + a) * 2.0 - 1.0) * Math.Max(half - 1.6, 0);
			d0 = VB_WallDist(b, lo, hi, side, u, 1.0, hit);
			if (d0 < 0 || d0 > WALL_OUT + 1.2)
				continue;

			// высокие корни — если стена есть и на 3.6 м
			TStringArray pool = WALL_LOW;
			if (VB_WallAt(b, lo, hi, side, u, 3.6, d0))
				pool = WALL_TALL;
			type = pool[Math.Floor(VB_Hash(bp, salt + 50 + a) * pool.Count() * 0.999)];
			hw = VB_HalfWidth(type);
			if (half - hw - 0.15 < 0)
			{
				type = "VB_RiftWall_Low1";
				hw = VB_HalfWidth(type);
				if (half - hw - 0.15 < 0)
					return false;
			}
			lim = half - hw - 0.15;
			u = Math.Clamp(u, -lim, lim);
			d0 = VB_WallDist(b, lo, hi, side, u, 1.0, hit);
			if (d0 < 0 || d0 > WALL_OUT + 1.2)
				continue;

			// края на 1 м — стена тянется на всю ширину (не висим за углом)
			if (!VB_WallAt(b, lo, hi, side, u - hw * 0.9, 1.0, d0) || !VB_WallAt(b, lo, hi, side, u + hw * 0.9, 1.0, d0))
				continue;
			// сверху на 2.4 м — хотя бы две точки из трёх (окна разрешены)
			upper = 0;
			if (VB_WallAt(b, lo, hi, side, u - hw * 0.8, 2.4, d0))
				upper++;
			if (VB_WallAt(b, lo, hi, side, u, 2.4, d0))
				upper++;
			if (VB_WallAt(b, lo, hi, side, u + hw * 0.8, 2.4, d0))
				upper++;
			if (upper < 2)
				continue;

			vector pos = hit + wn * 0.03;
			pos[1] = g_Game.SurfaceY(pos[0], pos[2]);
			if (!VB_FreeFor(pos, hw))
				continue;

			VB_SpawnHouseDecor(type, pos, wn);
			m_VB_DecorSpots.Insert(pos);
			m_VB_DecorHalf.Insert(hw);
			m_VB_LastDecorPos = pos;
			m_VB_LastDecorDir = wn;
			return true;
		}
		return false;
	}

	// корни на угол между сторонами sideX (±X) и sideZ (±Z): обе стены есть у ребра и на CORNER_FACE от него
	protected bool VB_TryCorner(Object b, vector lo, vector hi, int sideX, int sideZ)
	{
		if (sideX < 0 || sideZ < 0)
			return false;

		// у стороны ±X координата вдоль неё — Z (от середины), у стороны ±Z — X
		float halfAlongZ = VB_SideHalf(lo, hi, sideX);
		float halfAlongX = VB_SideHalf(lo, hi, sideZ);
		if (halfAlongX < CORNER_FACE + 0.3 || halfAlongZ < CORNER_FACE + 0.3)
			return false;

		float signX = 1.0;
		if (sideX == 1)
			signX = -1.0;
		float signZ = 1.0;
		if (sideZ == 3)
			signZ = -1.0;
		float edgeOnX = signZ * halfAlongZ;	// где на стороне ±X находится угол (по её координате вдоль Z)
		float edgeOnZ = signX * halfAlongX;	// где на стороне ±Z находится угол (по её координате вдоль X)

		vector hitX;
		vector hitZ;
		float dX = VB_WallDist(b, lo, hi, sideX, edgeOnX - signZ * 0.5, 1.0, hitX);
		float dZ = VB_WallDist(b, lo, hi, sideZ, edgeOnZ - signX * 0.5, 1.0, hitZ);
		if (dX < 0 || dZ < 0 || dX > WALL_OUT + 0.8 || dZ > WALL_OUT + 0.8)
			return false;		// угол не у коробки (вырез, крыльцо, навес)
		if (!VB_WallAt(b, lo, hi, sideX, edgeOnX - signZ * (CORNER_FACE - 0.2), 1.0, dX))
			return false;
		if (!VB_WallAt(b, lo, hi, sideZ, edgeOnZ - signX * (CORNER_FACE - 0.2), 1.0, dZ))
			return false;
		if (!VB_WallAt(b, lo, hi, sideX, edgeOnX - signZ * 0.5, 2.4, dX) && !VB_WallAt(b, lo, hi, sideZ, edgeOnZ - signX * 0.5, 2.4, dZ))
			return false;

		// ребро — пересечение двух плоскостей стен
		vector nX = VB_WorldNormal(b, sideX);
		vector nZ = VB_WorldNormal(b, sideZ);
		vector p = hitX + nZ * vector.Dot(hitZ - hitX, nZ);
		p = p + (nX + nZ) * 0.03;
		p[1] = g_Game.SurfaceY(p[0], p[2]);

		vector bp = b.GetPosition();
		string type = WALL_CORNER[Math.Floor(VB_Hash(bp, 7) * WALL_CORNER.Count() * 0.999)];
		Object obj = VB_SpawnHouseDecor(type, p, nZ);
		if (!obj)
			return false;

		// модель угла: стена A смотрит вперёд (+Z), стена B — вправо (+X). Если вправо смотрит не вторая стена,
		// разворачиваем лицом к ней — тогда первая окажется справа
		vector mat[4];
		obj.GetTransform(mat);
		if (vector.Dot(mat[0], nX) < 0)
		{
			VB_RiftBase rift = VB_RiftBase.Cast(obj);
			if (rift)
				rift.VB_PlaceDir(p, nX);
			else
				obj.SetDirection(nX);
		}

		m_VB_DecorSpots.Insert(p);
		m_VB_DecorHalf.Insert(CORNER_FACE);
		m_VB_LastDecorPos = p;
		vector diag = nX + nZ;
		diag.Normalize();
		m_VB_LastDecorDir = diag;
		return true;
	}

	protected Object VB_SpawnHouseDecor(string type, vector pos, vector dir)
	{
		Object obj = g_Game.CreateObjectEx(type, pos, ECE_LOCAL);
		if (!obj)
			return null;

		VB_RiftBase rift = VB_RiftBase.Cast(obj);
		if (rift)
		{
			rift.VB_PlaceDir(pos, dir);
			rift.m_VB_LinkHeart = this;
		}
		else
		{
			obj.SetPosition(pos);
			obj.SetDirection(dir);
		}
		m_VB_Wraps.Insert(obj);
		return obj;
	}
}
