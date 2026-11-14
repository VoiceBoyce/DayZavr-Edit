// SIGNS-OFF (owner 2026-10-31): the ! / ? signs above the quest givers are switched off - the whole file is commented
// out, not deleted (the owner may turn them back on). To bring them back: uncomment every block marked SIGNS-OFF
// (grep the mod), bump INT_QST_Const.PROTO, and ship the sign model/textures again - their sources are kept in
// asset_src/marker/INT_Quests_Data (the separate INT_Quests_Data PBO is gone: the client is one PBO now, so the
// model has to be rebuilt with INT_Quests_CL paths, asset_src/marker/README.md).

//// INT Quests (client) — signs above the quest givers (owner decisions O8, O12). The sign is a LOCAL world object
//// (INT_Quests_Data: enamel diamond, glowing material) created with ECE_LOCAL, so walls hide it like any object; it
//// slowly spins and bobs. Its state is this player's own and is computed here:
////   yellow ?  a taken quest can be turned in to this giver now
////   yellow !  this giver has a quest the player can take
////   silver ?  a taken quest goes back to this giver (not done yet)
////   silver !  this giver has quests that are closed for now (when the server shows closed quests)
//// States are recomputed on client changes and every 2 s (inventory targets), not every frame; the frame only
//// moves the few nearby signs. The name plate above givers was removed by the owner (2026-10-22).
//
//class INT_QST_MarkerSlot : Managed
//{
//	string m_Key;
//	Object m_Sign;			// local object, deleted by us
//	int m_State = -1;
//	float m_Phase;
//	float m_Height;			// sign height above the giver's origin
//}
//
//class INT_QST_Markers
//{
//	static const int ST_NONE = 0;
//	static const int ST_AVAIL = 1;
//	static const int ST_TURNIN = 2;
//	static const int ST_PROGRESS = 3;
//	static const int ST_LOCKED = 4;
//
//	static const float SIGN_DISTANCE = 150;
//	static const float SIGN_SCALE = 0.33;		// the model is a 0.60 m diamond; owner 2026-10-22: three times smaller
//	static const float BOB = 0.02;
//	static const float SPIN_DEG = 70;		// degrees per second
//	static const string DATA = "INT_Quests_Data\\data\\int_qst_marker_";
//
//	protected static ref INT_QST_Markers s_Instance;
//	static int s_StateBuilds;			// self-test counters
//	static int s_SignsAlive;
//
//	protected ref map<string, ref INT_QST_MarkerSlot> m_Slots = new map<string, ref INT_QST_MarkerSlot>();
//	protected ref map<string, ref array<int>> m_StartBy = new map<string, ref array<int>>();
//	protected ref map<string, ref array<int>> m_FinishBy = new map<string, ref array<int>>();
//	protected int m_IndexHash = -1;
//	protected int m_IndexCount = -1;
//	protected bool m_Dirty = true;
//	protected bool m_Hooked;
//	protected float m_StateAcc;
//
//	static INT_QST_Markers Get()
//	{
//		if (!s_Instance)
//			s_Instance = new INT_QST_Markers();
//		return s_Instance;
//	}
//
//	static INT_QST_Markers Peek()
//	{
//		return s_Instance;
//	}
//
//	static void Shutdown()
//	{
//		if (s_Instance)
//			s_Instance.Destroy();
//		s_Instance = null;
//	}
//
//	protected void Destroy()
//	{
//		if (m_Hooked && INT_QST_Client.s_OnChanged)
//			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
//		m_Hooked = false;
//		foreach (string k, INT_QST_MarkerSlot s : m_Slots)
//			DropSign(s);
//		m_Slots.Clear();
//	}
//
//	void OnDataChanged()
//	{
//		m_Dirty = true;
//	}
//
//	// giver key -> quest ids it gives / takes (rebuilt when the catalog changes)
//	protected void Reindex()
//	{
//		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
//		if (cat.m_Hash == m_IndexHash && cat.m_List.Count() == m_IndexCount)
//			return;
//		m_IndexHash = cat.m_Hash;
//		m_IndexCount = cat.m_List.Count();
//		m_StartBy.Clear();
//		m_FinishBy.Clear();
//		foreach (INT_QST_QuestDef q : cat.m_List)
//		{
//			if (q.GiverStart != "")
//				Ids(m_StartBy, q.GiverStart).Insert(q.Id);
//			if (q.GiverFinish != "")
//				Ids(m_FinishBy, q.GiverFinish).Insert(q.Id);
//		}
//	}
//
//	protected array<int> Ids(map<string, ref array<int>> m, string key)
//	{
//		array<int> a;
//		if (!m.Find(key, a))
//		{
//			a = new array<int>();
//			m.Set(key, a);
//		}
//		return a;
//	}
//
//	// The sign this player sees above a giver.
//	int StateOf(string key)
//	{
//		Reindex();
//		bool showLocked = INT_QST_Client.s_Hello && INT_QST_Client.s_Hello.ShowLocked;
//		int best = ST_NONE;
//		int wait;
//		array<int> fin;
//		if (m_FinishBy.Find(key, fin))
//		{
//			foreach (int fid : fin)
//			{
//				INT_QST_QuestDef fq = INT_QST_Client.s_Catalog.Get(fid);
//				if (!fq || !INT_QST_Client.s_State.FindActive(fid))
//					continue;
//				int fst = INT_QST_Client.StatusOf(fq, wait);
//				if (fst == INT_QST_St.READY)
//					return ST_TURNIN;
//				if (fst == INT_QST_St.ACTIVE)
//					best = ST_PROGRESS;
//			}
//		}
//		array<int> st;
//		if (m_StartBy.Find(key, st))
//		{
//			bool locked = false;
//			foreach (int sid : st)
//			{
//				INT_QST_QuestDef sq = INT_QST_Client.s_Catalog.Get(sid);
//				if (!sq)
//					continue;
//				int sst = INT_QST_Client.StatusOf(sq, wait);
//				if (sst == INT_QST_St.AVAILABLE)
//					return ST_AVAIL;
//				if (sst == INT_QST_St.LOCKED || sst == INT_QST_St.COOLDOWN)
//					locked = true;
//			}
//			if (best == ST_NONE && locked && showLocked)
//				best = ST_LOCKED;
//		}
//		return best;
//	}
//
//	// Server switch (config ShowSigns); per giver: GiverDef.NoSign.
//	static bool SignsOn()
//	{
//		return !INT_QST_Client.s_Hello || INT_QST_Client.s_Hello.ShowSigns;
//	}
//
//	// --- frame ----------------------------------------------------------------------------------------------
//	void Tick(float dt)
//	{
//		if (!m_Hooked)
//		{
//			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
//			m_Hooked = true;
//		}
//		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
//		if (!p || !INT_QST_Client.s_CatalogReady)
//			return;
//		m_StateAcc = m_StateAcc + dt;
//		// a change is applied at most every 0.25 s (kill storms send a progress packet per kill)
//		if ((m_Dirty && m_StateAcc >= 0.25) || m_StateAcc >= 2.0)
//		{
//			m_Dirty = false;
//			m_StateAcc = 0;
//			UpdateStates(p);
//		}
//		AnimateSigns(dt);
//	}
//
//	protected void UpdateStates(PlayerBase p)
//	{
//		s_StateBuilds++;
//		vector pp = p.GetPosition();
//		array<string> seen = new array<string>();
//		foreach (INT_QST_GiverSlot g : INT_QST_GiverReg.s_Slots)
//		{
//			if (!g.m_Obj)
//				continue;
//			vector gp = g.m_Obj.GetPosition();
//			if (vector.DistanceSq(pp, gp) > SIGN_DISTANCE * SIGN_DISTANCE)
//				continue;
//			seen.Insert(g.m_Key);
//			INT_QST_MarkerSlot s;
//			if (!m_Slots.Find(g.m_Key, s))
//			{
//				s = new INT_QST_MarkerSlot();
//				s.m_Key = g.m_Key;
//				s.m_Phase = Math.RandomFloat(0, 6.28);
//				m_Slots.Set(g.m_Key, s);
//			}
//			s.m_Height = SignHeight(g);
//			int state = ST_NONE;
//			if (SignsOn() && !g.m_NoSign)
//				state = StateOf(g.m_Key);
//			if (state != s.m_State || (state != ST_NONE && !s.m_Sign))
//				SetState(s, state, gp);
//		}
//		// givers out of range / gone
//		array<string> drop = new array<string>();
//		foreach (string key, INT_QST_MarkerSlot ms : m_Slots)
//		{
//			if (seen.Find(key) < 0)
//				drop.Insert(key);
//		}
//		foreach (string dk : drop)
//		{
//			INT_QST_MarkerSlot ds = m_Slots.Get(dk);
//			DropSign(ds);
//			m_Slots.Remove(dk);
//		}
//	}
//
//	// Above the head of an NPC (bone "Head"), above the bounding box of an object.
//	protected float SignHeight(INT_QST_GiverSlot g)
//	{
//		Object o = g.m_Obj;
//		if (g.m_Kind == 0)
//		{
//			Human h = Human.Cast(o);
//			if (h)
//			{
//				int bone = h.GetBoneIndexByName("Head");
//				if (bone >= 0)
//				{
//					vector hp = h.GetBonePositionWS(bone);
//					vector op = h.GetPosition();
//					float hh = hp[1] - op[1] + 0.36;
//					// a remote NPC may not be animated yet: keep the head height sane
//					if (hh == hh && hh > 1.4 && hh < 3.2)
//						return hh;
//				}
//			}
//			return 2.1;
//		}
//		vector mm[2];
//		if (o.GetCollisionBox(mm))
//			return mm[1][1] + 0.25;
//		return 2.0;
//	}
//
//	protected void SetState(INT_QST_MarkerSlot s, int state, vector gp)
//	{
//		s.m_State = state;
//		if (state == ST_NONE)
//		{
//			DropSign(s);
//			return;
//		}
//		if (!s.m_Sign)
//		{
//			vector at = gp + Vector(0, s.m_Height, 0);
//			// created into a local first, the holder is Managed: an ECE_LOCAL object put straight into a field of a
//			// plain class read null (measured 2026-10-22)
//			Object made = GetGame().CreateObjectEx("INT_QST_Marker", at, ECE_LOCAL);
//			if (!made)
//			{
//				INT_QST_Log.Dbg("знак не создан (INT_QST_Marker) для " + s.m_Key);
//				return;
//			}
//			s.m_Sign = made;
//			made.SetScale(SIGN_SCALE);
//			s_SignsAlive++;
//		}
//		string name = "avail";
//		if (state == ST_TURNIN)
//			name = "turnin";
//		else if (state == ST_PROGRESS)
//			name = "progress";
//		else if (state == ST_LOCKED)
//			name = "locked";
//		string tex = DATA + name + "_co.paa";
//		string mat = DATA + name + ".rvmat";
//		EntityAI e = EntityAI.Cast(s.m_Sign);
//		if (!e)
//			return;
//		e.SetObjectTexture(0, tex);
//		e.SetObjectMaterial(0, mat);
//	}
//
//	protected void DropSign(INT_QST_MarkerSlot s)
//	{
//		if (!s || !s.m_Sign)
//			return;
//		GetGame().ObjectDelete(s.m_Sign);
//		s.m_Sign = null;
//		s_SignsAlive--;
//	}
//
//	protected void AnimateSigns(float dt)
//	{
//		foreach (string key, INT_QST_MarkerSlot s : m_Slots)
//		{
//			if (!s.m_Sign)
//				continue;
//			INT_QST_GiverSlot g = INT_QST_GiverReg.Find(key);
//			if (!g || !g.m_Obj)
//				continue;
//			s.m_Phase = s.m_Phase + dt;
//			vector gp = g.m_Obj.GetPosition();
//			float bob = Math.Sin(s.m_Phase * 1.7) * BOB;
//			vector at = gp + Vector(0, s.m_Height + bob, 0);
//			s.m_Sign.SetPosition(at);
//			float yaw = s.m_Phase * SPIN_DEG;
//			int turns = (int)(yaw / 360.0);
//			yaw = yaw - turns * 360.0;
//			s.m_Sign.SetOrientation(Vector(yaw, 0, 0));
//			s.m_Sign.SetScale(SIGN_SCALE);		// re-applied after the rotation every frame (cheap, a few signs)
//		}
//	}
//
//	// --- self-test ------------------------------------------------------------------------------------------
//	int SignState(string key)
//	{
//		INT_QST_MarkerSlot s;
//		if (m_Slots.Find(key, s))
//			return s.m_State;
//		return -1;
//	}
//
//	bool HasSign(string key)
//	{
//		INT_QST_MarkerSlot s;
//		return m_Slots.Find(key, s) && s.m_Sign != null;
//	}
//}
