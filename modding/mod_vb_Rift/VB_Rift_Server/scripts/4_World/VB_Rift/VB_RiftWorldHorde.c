// Волна зомби Изнанки (только сервер, VB_Rift_Server). Когда игрок начинает вырезать сердце, вокруг него
// в 18–45 м по одному вылезают 8–15 зомби VB_RiftZmb*. Сколько волн на сердце — решает сердце
// (VB_RiftHeart.VB_DisturbServer, VB_RiftWorldServer.c): до HORDE_MAX, новая — когда прежняя кончилась (баг №29).
// Шум у сердца (CfgNoises VB_RiftCallNoise) тянет их к месту резки.
// Через 10 минут волна «рассасывается»: зомби, рядом с которыми нет игроков, сервер убирает.
// Потолок (баг №30): живых зомби всех волн на сервере не больше CAP_SERVER и в CAP_NEAR_DIST от центра новой
// волны не больше CAP_NEAR — лишних не спавним (волна урезается или не поднимается, строка в лог).
// Точка появления: суша, над ней ничего твёрдого (крыша, скала) и рядом навмеш (баг №39).
// Лут с зомби — разделкой (VB_RiftWorldZombie.c). Волны с loot = false (фонарь-сердце, топливная капсула)
// помечают своих зомби «без лута»: их разделка ничего не даёт.

class VB_RiftHorde
{
	static const int COUNT_MIN = 8;
	static const int COUNT_MAX = 15;
	static const float DIST_MIN = 18.0;		// ближе не появляются: не на глазах у игрока
	static const float DIST_MAX = 45.0;
	static const int STEP_MS = 350;			// между появлениями, мс
	static const int CHECK_MS = 60000;		// проверка «рассасывания», мс
	static const int KEEP_MS = 600000;		// столько волна живёт при любых условиях, мс
	static const float CLEAN_DIST = 120.0;	// потом убираем зомби, рядом с которыми нет игроков
	static const string NOISE = "VB_RiftCallNoise";
	static const float NOISE_TIME = 30.0;	// с, сколько держится шум у сердца
	static const int CAP_SERVER = 60;		// живых (и ещё не вылезших) зомби всех волн на сервере
	static const int CAP_NEAR = 25;			// и в CAP_NEAR_DIST от центра новой волны
	static const float CAP_NEAR_DIST = 100.0;
	static const int SPOT_TRIES = 12;		// попыток найти точку на одного зомби
	static const int SPOT_NAV_TRIES = 8;	// первые столько — только с навмешем рядом, остальные без него
	static const float NAV_RADIUS = 1.5;	// м: навмеш ищем так близко от точки
	static const float NAV_HEIGHT = 1.0;	// м: найденный навмеш не выше/ниже земли больше этого (не пол дома)
	protected static const ref TStringArray TYPES = {
		"VB_RiftZmbM_VillagerOld", "VB_RiftZmbF_VillagerOld", "VB_RiftZmbM_Prisoner", "VB_RiftZmbM_Jacket",
		"VB_RiftZmbM_Hiker", "VB_RiftZmbM_Worker", "VB_RiftZmbM_Sailor",
		"VB_RiftZmbF_Patient", "VB_RiftZmbF_Citizen", "VB_RiftZmbF_Doctor"
	};

	protected static ref array<ref VB_RiftHorde> s_VB_All;
	protected static ref NoiseParams s_VB_Noise;
	protected static ref array<EntityAI> s_VB_NoLoot;	// зомби «без лута» (зомби — Managed: удалённый становится null)
	protected static ref PGFilter s_VB_NavFilter;
	protected static bool s_VB_NavWarned;				// строка про «навмеш не нашёлся» — один раз за рестарт

	protected vector m_VB_Center;
	protected float m_VB_DistMin = DIST_MIN;
	protected float m_VB_DistMax = DIST_MAX;
	protected int m_VB_Left;
	protected int m_VB_BornMs;
	protected bool m_VB_Done;
	protected bool m_VB_Loot = true;
	protected ref array<EntityAI> m_VB_Zombies;

	// сервер: новая волна вокруг center; count = 0 — случайно COUNT_MIN..COUNT_MAX.
	// Малые «волны» (1–3 зомби) зовут фонарь-сердце и топливная капсула (loot = false: разделка без лута);
	// убираются так же через 10 минут без игроков.
	static void VB_Spawn(vector center, int count = 0, float distMin = 0, float distMax = 0, bool loot = true)
	{
		VB_SpawnWave(center, count, distMin, distMax, loot);
	}

	// то же, но вернуть волну (сердцу — следить, кончилась ли); null — потолок, волна не поднята
	static VB_RiftHorde VB_SpawnWave(vector center, int count = 0, float distMin = 0, float distMax = 0, bool loot = true)
	{
		if (!s_VB_All)
			s_VB_All = new array<ref VB_RiftHorde>;

		for (int i = s_VB_All.Count() - 1; i >= 0; i--)
		{
			if (!s_VB_All[i] || s_VB_All[i].m_VB_Done)
				s_VB_All.Remove(i);
		}

		int want = count;
		if (want <= 0)
			want = Math.RandomIntInclusive(COUNT_MIN, COUNT_MAX);

		// потолок: сколько ещё можно
		int near;
		int total = VB_CountLive(center, near);
		int room = CAP_SERVER - total;
		if (CAP_NEAR - near < room)
			room = CAP_NEAR - near;

		string at = center.ToString();
		string totalStr = total.ToString();
		string nearStr = near.ToString();
		if (room <= 0)
		{
			Print("[VB_Rift] волна зомби у " + at + " не поднята: потолок (всего " + totalStr + ", в 100 м " + nearStr + ")");
			return null;
		}
		if (want > room)
		{
			string wantStr = want.ToString();
			string roomStr = room.ToString();
			Print("[VB_Rift] волна зомби у " + at + " урезана с " + wantStr + " до " + roomStr + ": потолок (всего " + totalStr + ", в 100 м " + nearStr + ")");
			want = room;
		}

		VB_RiftHorde horde = new VB_RiftHorde(center, want, distMin, distMax, loot);
		s_VB_All.Insert(horde);

		string countStr = want.ToString();
		string lootNote = "";
		if (!loot)
			lootNote = " (без лута)";
		Print("[VB_Rift] волна зомби: " + countStr + " у " + at + lootNote);
		return horde;
	}

	// сервер: шум у сердца, зомби идут проверить
	static void VB_CallNoise(vector pos)
	{
		if (!s_VB_Noise)
		{
			s_VB_Noise = new NoiseParams();
			s_VB_Noise.Load(NOISE);
		}
		g_Game.GetNoiseSystem().AddNoiseTarget(pos, NOISE_TIME, s_VB_Noise);
	}

	// разделка (VB_RiftWorldZombie.c): у этого зомби есть лут? Нет — если его подняла волна с loot = false
	static bool VB_HasLoot(EntityAI zmb)
	{
		return !s_VB_NoLoot || s_VB_NoLoot.Find(zmb) < 0;
	}

	// живые и ещё не вылезшие зомби всех волн; near — из них в CAP_NEAR_DIST от center
	// (не вылезших считаем у центра их волны)
	protected static int VB_CountLive(vector center, out int near)
	{
		near = 0;
		int total = 0;
		float nearSq = CAP_NEAR_DIST * CAP_NEAR_DIST;
		foreach (VB_RiftHorde h : s_VB_All)
		{
			if (!h || h.m_VB_Done)
				continue;

			if (h.m_VB_Left > 0)
			{
				total += h.m_VB_Left;
				if (vector.DistanceSq(h.m_VB_Center, center) < nearSq)
					near += h.m_VB_Left;
			}
			foreach (EntityAI z : h.m_VB_Zombies)
			{
				if (!z || !z.IsAlive())
					continue;
				total++;
				if (vector.DistanceSq(z.GetPosition(), center) < nearSq)
					near++;
			}
		}
		return total;
	}

	protected static void VB_MarkNoLoot(EntityAI zmb)
	{
		if (!s_VB_NoLoot)
			s_VB_NoLoot = new array<EntityAI>;

		for (int i = s_VB_NoLoot.Count() - 1; i >= 0; i--)
		{
			if (!s_VB_NoLoot[i])
				s_VB_NoLoot.Remove(i);		// тело уже убрала очистка
		}
		s_VB_NoLoot.Insert(zmb);
	}

	// фильтр навмеша как у ИИ Expansion: ходьба, двери, внутри домов; без отключённых полигонов и воды
	protected static PGFilter VB_NavFilter()
	{
		if (!s_VB_NavFilter)
		{
			s_VB_NavFilter = new PGFilter();
			int inFlags = PGPolyFlags.WALK | PGPolyFlags.DOOR | PGPolyFlags.INSIDE;
			int exFlags = PGPolyFlags.DISABLED | PGPolyFlags.SWIM | PGPolyFlags.SWIM_SEA;
			s_VB_NavFilter.SetFlags(inFlags, exFlags, PGPolyFlags.NONE);
		}
		return s_VB_NavFilter;
	}

	// над точкой земли что-то твёрдое (крыша, скала, стена, машина): луч сверху упирается выше земли
	protected static bool VB_Covered(vector p)
	{
		vector from = Vector(p[0], p[1] + 25.0, p[2]);
		vector to = Vector(p[0], p[1] - 0.5, p[2]);
		vector hit;
		vector hitDir;
		int comp;
		if (!DayZPhysics.RaycastRV(from, to, hit, hitDir, comp, null, null, null, false, false, ObjIntersectGeom))
			return false;
		return hit[1] > p[1] + 0.5;
	}

	void VB_RiftHorde(vector center, int count, float distMin, float distMax, bool loot)
	{
		m_VB_Center = center;
		m_VB_Zombies = new array<EntityAI>;
		m_VB_Left = count;
		m_VB_Loot = loot;
		if (distMin > 0)
			m_VB_DistMin = distMin;
		if (distMax > m_VB_DistMin)
			m_VB_DistMax = distMax;
		m_VB_BornMs = g_Game.GetTime();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SpawnNext, STEP_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Check, CHECK_MS, true);
	}

	// волна кончилась: все вылезли, и живых не осталось (убиты или убраны)
	bool VB_IsOver()
	{
		if (m_VB_Done)
			return true;
		if (m_VB_Left > 0)
			return false;

		foreach (EntityAI z : m_VB_Zombies)
		{
			if (z && z.IsAlive())
				return false;
		}
		return true;
	}

	protected void VB_SpawnNext()
	{
		if (m_VB_Left <= 0)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_SpawnNext);
			return;
		}
		m_VB_Left--;

		vector pos;
		if (!VB_FindSpot(pos))
			return;

		EntityAI zmb = EntityAI.Cast(g_Game.CreateObjectEx(TYPES.GetRandomElement(), pos, ECE_PLACE_ON_SURFACE | ECE_INITAI));
		if (!zmb)
			return;

		// лицом к сердцу
		float yaw = Math.Atan2(m_VB_Center[0] - pos[0], m_VB_Center[2] - pos[2]) * Math.RAD2DEG;
		zmb.SetOrientation(Vector(yaw, 0, 0));
		m_VB_Zombies.Insert(zmb);
		if (!m_VB_Loot)
			VB_MarkNoLoot(zmb);
	}

	// Случайная точка кольца: суша, над ней ничего твёрдого (не на крышу, не в скалу), рядом навмеш на уровне земли.
	// Если навмеш не подтвердил SPOT_NAV_TRIES точек подряд (нет навмеша, не тот фильтр) — дальше без него,
	// с одной строкой в лог за рестарт: по ней на стенде видно, работает ли проверка навмешем.
	protected bool VB_FindSpot(out vector pos)
	{
		float a;
		float d;
		float x;
		float z;
		float y;
		vector p;
		vector nav;
		int navFails = 0;
		AIWorld aiWorld;
		if (g_Game.GetWorld())
			aiWorld = g_Game.GetWorld().GetAIWorld();

		for (int i = 0; i < SPOT_TRIES; i++)
		{
			a = Math.RandomFloat(0, Math.PI2);
			d = Math.RandomFloat(m_VB_DistMin, m_VB_DistMax);
			x = m_VB_Center[0] + Math.Cos(a) * d;
			z = m_VB_Center[2] + Math.Sin(a) * d;
			if (g_Game.SurfaceIsSea(x, z) || g_Game.SurfaceIsPond(x, z))
				continue;

			y = g_Game.SurfaceY(x, z);
			p = Vector(x, y, z);
			if (VB_Covered(p))
				continue;

			if (i < SPOT_NAV_TRIES && aiWorld)
			{
				if (!aiWorld.SampleNavmeshPosition(p, NAV_RADIUS, VB_NavFilter(), nav) || Math.AbsFloat(nav[1] - y) > NAV_HEIGHT)
				{
					navFails++;
					continue;
				}
			}
			else if (navFails > 0 && !s_VB_NavWarned)
			{
				s_VB_NavWarned = true;
				string at = m_VB_Center.ToString();
				Print("[VB_Rift] волна у " + at + ": навмеш не подтвердил точки, зомби ставлю без него (строка раз за рестарт)");
			}

			pos = p;
			return true;
		}
		return false;
	}

	protected void VB_Check()
	{
		for (int i = m_VB_Zombies.Count() - 1; i >= 0; i--)
		{
			if (!m_VB_Zombies[i] || !m_VB_Zombies[i].IsAlive())
				m_VB_Zombies.Remove(i);		// трупы убирает ванильная очистка
		}

		if (g_Game.GetTime() - m_VB_BornMs >= KEEP_MS)
		{
			array<Man> players = new array<Man>;
			g_Game.GetPlayers(players);
			float cleanSq = CLEAN_DIST * CLEAN_DIST;
			bool near;
			for (int k = m_VB_Zombies.Count() - 1; k >= 0; k--)
			{
				near = false;
				foreach (Man someone : players)
				{
					if (someone && vector.DistanceSq(someone.GetPosition(), m_VB_Zombies[k].GetPosition()) < cleanSq)
					{
						near = true;
						break;
					}
				}
				if (!near)
				{
					g_Game.ObjectDelete(m_VB_Zombies[k]);
					m_VB_Zombies.Remove(k);
				}
			}
		}

		if (m_VB_Left <= 0 && m_VB_Zombies.Count() == 0)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Check);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_SpawnNext);
			m_VB_Done = true;	// из списка уберёт следующий VB_Spawn
		}
	}
}
