// Волна зомби Изнанки (сервер). Когда игрок начинает вырезать сердце, вокруг него в 18–45 м
// по одному вылезают 8–15 зомби VB_RiftZmb* — один раз на сердце до рестарта.
// Шум у сердца (CfgNoises VB_RiftCallNoise) тянет их к месту резки.
// Через 10 минут волна «рассасывается»: зомби, рядом с которыми нет игроков, сервер убирает.

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
	protected static const ref TStringArray TYPES = {
		"VB_RiftZmbM_VillagerOld", "VB_RiftZmbF_VillagerOld", "VB_RiftZmbM_Prisoner", "VB_RiftZmbM_Jacket",
		"VB_RiftZmbM_Hiker", "VB_RiftZmbM_Worker", "VB_RiftZmbM_Sailor",
		"VB_RiftZmbF_Patient", "VB_RiftZmbF_Citizen", "VB_RiftZmbF_Doctor"
	};

	protected static ref array<ref VB_RiftHorde> s_VB_All;
	protected static ref NoiseParams s_VB_Noise;

	protected vector m_VB_Center;
	protected int m_VB_Left;
	protected int m_VB_BornMs;
	protected bool m_VB_Done;
	protected ref array<EntityAI> m_VB_Zombies;

	// сервер: новая волна вокруг center
	static void VB_Spawn(vector center)
	{
		if (!s_VB_All)
			s_VB_All = new array<ref VB_RiftHorde>;

		for (int i = s_VB_All.Count() - 1; i >= 0; i--)
		{
			if (s_VB_All[i].m_VB_Done)
				s_VB_All.Remove(i);
		}

		VB_RiftHorde horde = new VB_RiftHorde(center);
		s_VB_All.Insert(horde);
		Print("[VB_Rift] волна зомби: " + horde.m_VB_Left.ToString() + " у " + center.ToString());
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

	void VB_RiftHorde(vector center)
	{
		m_VB_Center = center;
		m_VB_Zombies = new array<EntityAI>;
		m_VB_Left = Math.RandomIntInclusive(COUNT_MIN, COUNT_MAX);
		m_VB_BornMs = g_Game.GetTime();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SpawnNext, STEP_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Check, CHECK_MS, true);
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
	}

	// случайная точка кольца на суше
	protected bool VB_FindSpot(out vector pos)
	{
		float a;
		float d;
		float x;
		float z;
		for (int i = 0; i < 8; i++)
		{
			a = Math.RandomFloat(0, Math.PI2);
			d = Math.RandomFloat(DIST_MIN, DIST_MAX);
			x = m_VB_Center[0] + Math.Cos(a) * d;
			z = m_VB_Center[2] + Math.Sin(a) * d;
			if (g_Game.SurfaceIsSea(x, z) || g_Game.SurfaceIsPond(x, z))
				continue;

			pos = Vector(x, g_Game.SurfaceY(x, z), z);
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
