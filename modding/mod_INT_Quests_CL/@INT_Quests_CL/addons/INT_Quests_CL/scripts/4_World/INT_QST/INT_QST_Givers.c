// INT Quests — registry of the quest givers on THIS side (spec.md §4.3, §5.3). The server fills it when it spawns
// the givers; the client fills it from HELLO/GIVERS and finds each object by its network id when it streams in.
// No sync variable on PlayerBase/House is needed (research 2026-10-22: GetNetworkID is shared for the session).
// Lookups walk a short list (a server has a handful of givers), so the per-frame cost of the action condition is
// a few pointer compares.

class INT_QST_GiverSlot
{
	string m_Key;
	string m_Title;
	int m_Kind;			// 0 NPC, 1 object
	string m_Class;
	vector m_Pos;			// where it stands (marker anchor fallback)
	int m_NetLow;
	int m_NetHigh;
	Object m_Obj;			// weak: the engine owns the object
	// SIGNS-OFF (owner 2026-10-31)
	// bool m_NoSign;			// admin switched the sign off for this giver
	float m_NextResolve;		// client: next attempt to find the object by network id
}

class INT_QST_GiverReg
{
	static ref array<ref INT_QST_GiverSlot> s_Slots = new array<ref INT_QST_GiverSlot>();
	static int s_Revision;		// bumped on every change (markers rebuild)

	static void Clear()
	{
		s_Slots.Clear();
		s_Revision++;
	}

	static INT_QST_GiverSlot Find(string key)
	{
		foreach (INT_QST_GiverSlot s : s_Slots)
		{
			if (s.m_Key == key)
				return s;
		}
		return null;
	}

	static INT_QST_GiverSlot Put(string key)
	{
		INT_QST_GiverSlot s = Find(key);
		if (!s)
		{
			s = new INT_QST_GiverSlot();
			s.m_Key = key;
			s_Slots.Insert(s);
		}
		s_Revision++;
		return s;
	}

	static void Remove(string key)
	{
		for (int i = s_Slots.Count() - 1; i >= 0; i--)
		{
			if (s_Slots[i].m_Key == key)
				s_Slots.RemoveOrdered(i);
		}
		s_Revision++;
	}

	// Key of the giver this object is ("" = not a giver). Works on both sides.
	static string KeyOf(Object o)
	{
		if (!o)
			return "";
		foreach (INT_QST_GiverSlot s : s_Slots)
		{
			if (s.m_Obj && s.m_Obj == o)
				return s.m_Key;
		}
		return "";
	}

	static bool IsGiver(Object o)
	{
		return KeyOf(o) != "";
	}

	// Client: try to find streamed-in giver objects near the player (cheap, rate-limited by the caller).
	static void ResolveNear(vector playerPos, float now)
	{
		foreach (INT_QST_GiverSlot s : s_Slots)
		{
			if (s.m_Obj)
				continue;
			if (s.m_NetLow == 0 && s.m_NetHigh == 0)
				continue;
			if (now < s.m_NextResolve)
				continue;
			if (vector.DistanceSq(playerPos, s.m_Pos) > 1000000)	// 1 km: not streamed in anyway
				continue;
			s.m_NextResolve = now + 1.0;
			Object o = GetGame().GetObjectByNetworkId(s.m_NetLow, s.m_NetHigh);
			if (o)
			{
				s.m_Obj = o;
				s_Revision++;
			}
		}
	}
}

// Calls from 4_World (the action) into the client windows that live in 5_Mission. The mission replaces s_Ui.
class INT_QST_UiBridge
{
	static ref INT_QST_UiBridge s_Ui;

	void OpenGiver(string key)
	{
	}

	void OpenJournal()
	{
	}

	static INT_QST_UiBridge Get()
	{
		if (!s_Ui)
			s_Ui = new INT_QST_UiBridge();
		return s_Ui;
	}
}
