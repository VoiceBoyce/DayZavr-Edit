// Виртуальные находки (ТЗ 2.2): до находки предмет — запись на сервере. Детектор ищет ближайшую разрешённую ему
// запись; когда игрок подходит ближе SpawnDistance, сервер создаёт предмет и убирает запись.
// Запись принадлежит хозяину (аномалии или полю) и исчезает вместе с ним.

class VB_AB_Find
{
	string m_Cls;
	vector m_Pos;
	int m_MinTier;
	ref array<string> m_Detectors;
	int m_QMin;
	int m_QMax;
	Object m_Owner;

	bool AllowedFor(string detector, int tier)
	{
		if (m_Detectors && m_Detectors.Count() > 0)
			return m_Detectors.Find(detector) != -1;
		return tier >= m_MinTier;
	}
};

class VB_AB_Finds
{
	protected static ref array<ref VB_AB_Find> s_Finds;

	static int Count()
	{
		if (!s_Finds)
			return 0;
		return s_Finds.Count();
	}

	static void Add(VB_AB_Find f)
	{
		if (!s_Finds)
			s_Finds = new array<ref VB_AB_Find>;
		s_Finds.Insert(f);
	}

	static int RemoveByOwner(Object owner)
	{
		if (!s_Finds)
			return 0;
		int n = 0;
		for (int i = s_Finds.Count() - 1; i >= 0; i--)
		{
			if (s_Finds[i].m_Owner == owner)
			{
				s_Finds.Remove(i);
				n++;
			}
		}
		return n;
	}

	// Ближайшая разрешённая детектору запись в радиусе.
	static VB_AB_Find Nearest(vector pos, float radius, string detector, int tier, out float dist)
	{
		dist = radius;
		if (!s_Finds)
			return null;
		VB_AB_Find best;
		foreach (VB_AB_Find f : s_Finds)
		{
			if (!f.AllowedFor(detector, tier))
				continue;
			float d = vector.Distance(pos, f.m_Pos);
			if (d < dist)
			{
				dist = d;
				best = f;
			}
		}
		return best;
	}

	// Создать предмет на месте записи и убрать запись.
	static EntityAI Materialize(VB_AB_Find f, PlayerBase player, string detector)
	{
		if (!f)
			return null;
		if (s_Finds)
			s_Finds.RemoveItem(f);
		EntityAI e = SpawnItem(f.m_Cls, f.m_Pos, f.m_QMin, f.m_QMax);
		string who = "?";
		if (player && player.GetIdentity())
			who = player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ")";
		VB_AB_Log.Info("находка: " + f.m_Cls + " — " + who + ", детектор " + detector + ", " + f.m_Pos.ToString());
		return e;
	}

	static EntityAI SpawnItem(string cls, vector pos, int qMin, int qMax)
	{
		EntityAI e = EntityAI.Cast(GetGame().CreateObjectEx(cls, pos, ECE_PLACE_ON_SURFACE));
		ItemBase ib;
		if (Class.CastTo(ib, e) && qMax > 0 && ib.HasQuantity())
		{
			int q = Math.RandomIntInclusive(Math.Max(1, qMin), qMax);
			ib.SetQuantity(Math.Min(q, ib.GetQuantityMax()));
		}
		return e;
	}

	// Засеять таблицу у точки center на расстоянии rMin..rMax. Visible — предмет сразу лежит на виду.
	static int SeedTable(VB_AB_LootTable t, vector center, float rMin, float rMax, Object owner)
	{
		if (!t || !t.Items || t.Items.Count() == 0)
			return 0;
		int n = 0;
		for (int r = 0; r < t.Rolls; r++)
		{
			VB_AB_LootItem it = Roll(t);
			if (!it)
				continue;
			vector p = RandomGroundPos(center, rMin, rMax);
			if (it.Visible != 0)
			{
				SpawnItem(it.Classname, p, it.QuantityMin, it.QuantityMax);
				n++;
				continue;
			}
			VB_AB_Find f = new VB_AB_Find;
			f.m_Cls = it.Classname;
			f.m_Pos = p;
			f.m_MinTier = it.MinTier;
			f.m_Detectors = it.Detectors;
			f.m_QMin = it.QuantityMin;
			f.m_QMax = it.QuantityMax;
			f.m_Owner = owner;
			Add(f);
			n++;
		}
		return n;
	}

	// Рулетка: Chance — вес строки, Nothing — вес «ничего».
	static VB_AB_LootItem Roll(VB_AB_LootTable t)
	{
		float total = Math.Max(0, t.Nothing);
		foreach (VB_AB_LootItem it : t.Items)
			total += Math.Max(0, it.Chance);
		if (total <= 0)
			return null;
		float x = Math.RandomFloat(0, total);
		if (x < t.Nothing)
			return null;
		x -= Math.Max(0, t.Nothing);
		foreach (VB_AB_LootItem it2 : t.Items)
		{
			x -= Math.Max(0, it2.Chance);
			if (x < 0)
				return it2;
		}
		return null;
	}

	static vector RandomGroundPos(vector center, float rMin, float rMax)
	{
		float a = Math.RandomFloat(0, Math.PI2);
		float d = Math.RandomFloat(rMin, rMax);
		vector p = center;
		p[0] = p[0] + Math.Cos(a) * d;
		p[2] = p[2] + Math.Sin(a) * d;
		p[1] = GetGame().SurfaceY(p[0], p[2]);
		return p;
	}
};
