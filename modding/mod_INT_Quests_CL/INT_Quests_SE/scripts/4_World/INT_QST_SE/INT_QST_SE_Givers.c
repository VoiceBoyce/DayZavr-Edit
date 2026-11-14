// INT Quests (server) — placing the quest givers in the world (spec.md §5.3, owner decision O7). Re-created on every
// start, never persisted. Survivor NPCs follow INT_Trader's spawn (live on 1.30): CreateObjectEx with SETUP |
// UPDATEPATHGRAPH | CREATEPHYSICS | NOPERSISTENCY_WORLD, position/orientation after creation, invulnerable, and —
// like INT_Trader — dressed AT ONCE with CreateInInventory (server 1, 2026-10-25: a giver dressed one second later
// refused every item, vanilla ones too, and was gone for the players while the trader NPCs dressed this way were
// fine); a piece that does not go on is tried once more 2 s later, and a refusal logs the NPC's state. No lifetime
// on survivors (the trader sets none). No watchdog / timers (owner). Static objects (HouseNoDestruct classes) spawn like the
// vanilla object spawner (no lifetime, dynamic persistency = none). The client learns each giver's network id.

class INT_QST_SE_Givers
{
	static int s_Spawned;			// self-test
	protected static ref map<string, ref array<string>> s_DressLeft = new map<string, ref array<string>>();

	static void SpawnAll()
	{
		INT_QST_GiverReg.Clear();
		s_Spawned = 0;
		foreach (INT_QST_GiverDef d : INT_QST_SE_Store.s_Givers.Givers)
		{
			if (d.Disabled == 1)
				continue;
			Spawn(d);
		}
		INT_QST_Log.Info("квестовиков поставлено: " + s_Spawned.ToString() + " из " + INT_QST_SE_Store.s_Givers.Givers.Count().ToString());
	}

	static void DespawnAll()
	{
		foreach (INT_QST_GiverSlot s : INT_QST_GiverReg.s_Slots)
		{
			if (s.m_Obj)
				GetGame().ObjectDelete(s.m_Obj);
		}
		INT_QST_GiverReg.Clear();
	}

	// Exactly the old mod's ValidatePos: the file's coordinates, except a point under the ground goes up onto it.
	// Server 1, 2026-10-25: y 45 with the ground at 45.77 — a survivor created 0.77 m inside the terrain fell
	// through the world and was gone; the old mod raised it, so it always stood there.
	static vector GroundPos(vector p)
	{
		vector v = p;
		float ground = GetGame().SurfaceY(v[0], v[2]);
		if (ground > v[1])
			v[1] = ground;
		return v;
	}

	static bool Spawn(INT_QST_GiverDef d)
	{
		Despawn(d.Key);
		if (d.Model == "")
			return false;
		vector pos = GroundPos(d.m_Pos);
		Object o;
		if (d.Kind == 0)
			o = GetGame().CreateObjectEx(d.Model, pos, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOPERSISTENCY_WORLD);
		else
			o = GetGame().CreateObjectEx(d.Model, pos, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD, RF_IGNORE);
		if (!o)
		{
			INT_QST_SE_Store.Problem("квестовик " + d.Key + ": класс " + INT_QST_Log.Clip(d.Model, 60) + " не создаётся");
			return false;
		}
		o.SetPosition(pos);
		o.SetOrientation(d.m_Ori);
		EntityAI e = EntityAI.Cast(o);
		if (e)
		{
			// an object giver made of an item class must not be picked up (it would ride with the player)
			ItemBase gi = ItemBase.Cast(o);
			if (gi)
				gi.SetTakeable(false);
			e.SetAllowDamage(false);
			if (d.Kind != 0)
			{
				e.SetLifetime(316224000);
				e.SetLifetimeMax(316224000);
			}
		}
		INT_QST_GiverSlot s = INT_QST_GiverReg.Put(d.Key);
		s.m_Title = d.Title;
		s.m_Kind = d.Kind;
		s.m_Class = d.Model;
		s.m_Pos = pos;
		s.m_Obj = o;
		int low;
		int high;
		o.GetNetworkID(low, high);
		s.m_NetLow = low;
		s.m_NetHigh = high;
		s_Spawned++;
		float gy = GetGame().SurfaceY(pos[0], pos[2]);
		string sPos = pos.ToString();
		string sGround = gy.ToString();
		string sAsked = d.m_Pos[1].ToString();
		INT_QST_Log.Info("квестовик " + d.Key + " " + d.Model + " в " + sPos + " (в файле высота " + sAsked + ", земля " + sGround + ")");
		if (d.Kind == 0 && (d.Clothes.Count() > 0 || d.InHands != ""))
			DressNow(d, PlayerBase.Cast(o));
		return true;
	}

	// The clothes go on in the creating tick (INT_Trader's way). Pieces that refuse get one more try 2 s later.
	protected static void DressNow(INT_QST_GiverDef d, PlayerBase npc)
	{
		if (!npc)
			return;
		array<string> left = new array<string>();
		foreach (string c : d.Clothes)
		{
			if (c == "")
				continue;
			if (!PutOn(npc, c))
				left.Insert(c);
		}
		if (d.InHands != "")
		{
			EntityAI h = npc.GetHumanInventory().CreateInHands(d.InHands);
			if (!h)
				left.Insert("@hands:" + d.InHands);
		}
		if (left.Count() == 0)
		{
			s_DressLeft.Remove(d.Key);
			return;
		}
		s_DressLeft.Set(d.Key, left);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DressRetry, 2000, false, d.Key);
	}

	protected static bool PutOn(PlayerBase npc, string cls)
	{
		EntityAI att = npc.GetInventory().CreateInInventory(cls);
		if (!att)
			att = npc.GetInventory().CreateAttachment(cls);
		if (!att)
			return false;
		att.SetAllowDamage(false);
		return true;
	}

	static void DressRetry(string key)
	{
		array<string> left;
		if (!s_DressLeft.Find(key, left) || !left)
			return;
		s_DressLeft.Remove(key);
		INT_QST_GiverSlot s = INT_QST_GiverReg.Find(key);
		if (!s || !s.m_Obj)
			return;
		PlayerBase npc = PlayerBase.Cast(s.m_Obj);
		if (!npc)
			return;
		string state = NpcState(npc);
		foreach (string c : left)
		{
			if (c.IndexOf("@hands:") == 0)
			{
				string hc = c.Substring(7, c.Length() - 7);
				EntityAI h = npc.GetHumanInventory().CreateInHands(hc);
				if (!h)
					INT_QST_SE_Store.Problem("квестовик " + key + ": в руки не создаётся " + INT_QST_Log.Clip(hc, 60) + " (" + state + ")");
				continue;
			}
			if (!PutOn(npc, c))
				INT_QST_SE_Store.Problem("квестовик " + key + ": одежда " + INT_QST_Log.Clip(c, 60) + " не надевается (" + state + ")");
		}
	}

	// What the NPC is like when it refuses an item: alive, being deleted, where, how high the ground is.
	protected static string NpcState(PlayerBase npc)
	{
		vector p = npc.GetPosition();
		float ground = GetGame().SurfaceY(p[0], p[2]);
		string alive = "жив";
		if (!npc.IsAlive())
			alive = "МЁРТВ";
		string del = "";
		if (npc.IsSetForDeletion())
			del = ", удаляется";
		string sp = p.ToString();
		string sg = ground.ToString();
		return alive + del + ", позиция " + sp + ", земля " + sg;
	}

	static void Despawn(string key)
	{
		INT_QST_GiverSlot s = INT_QST_GiverReg.Find(key);
		if (!s)
			return;
		if (s.m_Obj)
			GetGame().ObjectDelete(s.m_Obj);
		INT_QST_GiverReg.Remove(key);
	}

	// What the clients get (HELLO / GIVERS).
	static void Views(array<ref INT_QST_GiverView> outList)
	{
		outList.Clear();
		foreach (INT_QST_GiverDef d : INT_QST_SE_Store.s_Givers.Givers)
		{
			INT_QST_GiverView v = new INT_QST_GiverView();
			v.Key = d.Key;
			v.Title = d.Title;
			v.Kind = d.Kind;
			v.Model = d.Model;
			v.Position = d.m_Pos;
			v.Orientation = d.m_Ori;
			v.Clothes.Copy(d.Clothes);
			v.InHands = d.InHands;
			v.Disabled = d.Disabled == 1;
			v.Greeting = d.Greeting;
			// v.NoSign = d.NoSign != 0;		// SIGNS-OFF (owner 2026-10-31)
			INT_QST_GiverSlot s = INT_QST_GiverReg.Find(d.Key);
			if (s)
			{
				v.Position = s.m_Pos;
				v.NetLow = s.m_NetLow;
				v.NetHigh = s.m_NetHigh;
			}
			outList.Insert(v);
		}
	}

	// Distance check for accept / turn-in (the object position when spawned, else the configured one).
	static bool Near(PlayerBase p, string key, float maxDist)
	{
		INT_QST_GiverSlot s = INT_QST_GiverReg.Find(key);
		if (!s || !p)
			return false;
		vector at = s.m_Pos;
		if (s.m_Obj)
			at = s.m_Obj.GetPosition();
		return vector.Distance(p.GetPosition(), at) <= maxDist + 1.0;
	}
}
