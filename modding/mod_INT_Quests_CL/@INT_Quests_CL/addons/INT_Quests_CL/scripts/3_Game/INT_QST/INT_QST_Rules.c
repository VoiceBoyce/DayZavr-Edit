// INT Quests — the quest catalog and the availability rules, shared by both sides (spec.md §4.1, §4.2). The client
// uses them for the windows and the markers, the server runs the SAME code to validate requests. Time is computed
// on demand (no timers, owner rule 2): `now` is unix UTC, `offset` the server machine's local-minus-UTC seconds.

class INT_QST_Catalog
{
	ref map<int, ref INT_QST_QuestDef> m_ById = new map<int, ref INT_QST_QuestDef>();
	ref array<ref INT_QST_QuestDef> m_List = new array<ref INT_QST_QuestDef>();	// branch sort, then Order
	ref map<string, ref INT_QST_BranchDef> m_Branches = new map<string, ref INT_QST_BranchDef>();
	ref array<string> m_BranchOrder = new array<string>();	// branch keys by Sort, then key
	int m_Hash;

	void Clear()
	{
		m_ById.Clear();
		m_List.Clear();
		m_Branches.Clear();
		m_BranchOrder.Clear();
		m_Hash = 0;
	}

	INT_QST_QuestDef Get(int id)
	{
		INT_QST_QuestDef q;
		if (m_ById.Find(id, q))
			return q;
		return null;
	}

	INT_QST_BranchDef Branch(string key)
	{
		INT_QST_BranchDef b;
		if (m_Branches.Find(key, b))
			return b;
		return null;
	}

	string BranchTitle(string key)
	{
		INT_QST_BranchDef b = Branch(key);
		if (b && b.Title != "")
			return b.Title;
		return key;
	}

	// Position of a quest inside its branch (1-based) and the branch size.
	void BranchPlace(INT_QST_QuestDef q, out int index, out int total)
	{
		index = 0;
		total = 0;
		foreach (INT_QST_QuestDef x : m_List)
		{
			if (x.m_Branch != q.m_Branch)
				continue;
			total++;
			if (x == q)
				index = total;
		}
	}

	void Add(INT_QST_QuestDef q)
	{
		m_ById.Set(q.Id, q);
		m_List.Insert(q);
	}

	void RemoveQuest(int id)
	{
		m_ById.Remove(id);
		for (int i = m_List.Count() - 1; i >= 0; i--)
		{
			if (m_List[i].Id == id)
				m_List.RemoveOrdered(i);
		}
	}

	// Branches by Sort (ties keep their order), quests by branch, then Order, then Id. Insertion sorts: after a
	// patch the lists are almost sorted, so this is close to linear. Same code on both sides.
	void Resort()
	{
		// branches: every known branch is in the order list
		foreach (string bk, INT_QST_BranchDef bd : m_Branches)
		{
			if (m_BranchOrder.Find(bk) < 0)
				m_BranchOrder.Insert(bk);
		}
		for (int r = m_BranchOrder.Count() - 1; r >= 0; r--)
		{
			if (!m_Branches.Contains(m_BranchOrder[r]))
				m_BranchOrder.RemoveOrdered(r);
		}
		for (int i = 1; i < m_BranchOrder.Count(); i++)
		{
			string cur = m_BranchOrder[i];
			int cs = SortOf(cur);
			int j = i - 1;
			while (j >= 0)
			{
				string prevKey = m_BranchOrder[j];
				if (SortOf(prevKey) <= cs)
					break;
				m_BranchOrder[j + 1] = prevKey;
				j--;
			}
			m_BranchOrder[j + 1] = cur;
		}
		map<string, int> place = new map<string, int>();
		for (int b = 0; b < m_BranchOrder.Count(); b++)
			place.Set(m_BranchOrder[b], b);
		for (int k = 1; k < m_List.Count(); k++)
		{
			INT_QST_QuestDef q = m_List[k];
			// explicit: whether Find touches the out value on a miss is not verified
			int qb = 100000;
			int found;
			if (place.Find(q.m_Branch, found))
				qb = found;
			int m = k - 1;
			while (m >= 0)
			{
				INT_QST_QuestDef prev = m_List[m];
				if (!QuestAfter(prev, qb, q, place))
					break;
				m_List[m + 1] = prev;
				m--;
			}
			m_List[m + 1] = q;
		}
	}

	protected int SortOf(string key)
	{
		INT_QST_BranchDef b = Branch(key);
		if (b)
			return b.Sort;
		return 0;
	}

	// a goes after q (q's branch place = qb)
	protected bool QuestAfter(INT_QST_QuestDef a, int qb, INT_QST_QuestDef q, map<string, int> place)
	{
		int ab = 100000;
		place.Find(a.m_Branch, ab);
		if (ab != qb)
			return ab > qb;
		if (a.Order != q.Order)
			return a.Order > q.Order;
		return a.Id > q.Id;
	}
}

// Status of a quest for one player.
class INT_QST_St
{
	static const int HIDDEN = 0;		// disabled
	static const int AVAILABLE = 1;
	static const int ACTIVE = 2;		// taken, counted targets not done (inventory targets unknown here)
	static const int READY = 3;		// taken, counted targets done (inventory targets still checked at turn-in)
	static const int LOCKED = 4;		// requirements missing
	static const int COOLDOWN = 5;		// done, available again later
	static const int DONE = 6;		// non-repeatable and done
	static const int EXPIRED = 7;		// taken, time limit passed
}

class INT_QST_Rules
{
	// What the targets ARE (kind, class, filter, place, zone per position). A changed count keeps the progress (it is
	// still the same target); anything else resets it (owner 2026-10-22). Built from the NORMALIZED fields (case,
	// spaces, number formatting of a position do not count). Never 0 (0 = an entry from before signatures).
	static int TargetSig(INT_QST_QuestDef q)
	{
		string s = "";
		foreach (INT_QST_TargetDef t : q.Targets)
		{
			string ts = t.m_Type.ToString();
			s = s + ts + "|" + t.m_ClassLow + "|" + t.m_FilterLow;
			if (t.m_HasPos)
				s = s + "|p" + VecKey(t.m_Pos);
			if (t.m_HasZone)
				s = s + "|z" + VecKey(t.m_ZonePos);
			s = s + ";";
		}
		int h = s.Hash();
		if (h == 0)
			h = 1;
		return h;
	}

	// A position rounded to whole metres as text (signatures).
	static string VecKey(vector v)
	{
		int x = (int)Math.Round(v[0]);
		int y = (int)Math.Round(v[1]);
		int z = (int)Math.Round(v[2]);
		string xs = x.ToString();
		string ys = y.ToString();
		string zs = z.ToString();
		return xs + " " + ys + " " + zs;
	}

	// Requirements met? Ids that no longer exist in the catalog are ignored (an admin removed them). A required
	// quest counts while it is done and not yet available again, as in the old mod (it dropped the turn-in record
	// when the cooldown ran out): a non-repeatable one counts forever, a repeatable one only during its wait.
	static bool RequiresMet(INT_QST_QuestDef q, INT_QST_PlayerFile st, INT_QST_Catalog cat, int now, int offset, array<int> missing)
	{
		if (missing)
			missing.Clear();
		if (q.Requires.Count() == 0)
			return true;
		int have = 0;
		int need = 0;
		foreach (int rid : q.Requires)
		{
			INT_QST_QuestDef rq = cat.Get(rid);
			if (!rq)
				continue;
			need++;
			INT_QST_Done d = st.FindDone(rid);
			bool counts = false;
			if (d && d.Times > 0)
			{
				int back = AvailableAt(rq, d, offset);
				if (back < 0 || back > now)
					counts = true;
			}
			if (counts)
				have++;
			else if (missing)
				missing.Insert(rid);
		}
		if (need == 0)
			return true;
		if (q.RequireAny == 1)
			return have > 0;
		return have == need;
	}

	// When the quest can be taken again after its last turn-in: 0 = now, -1 = never (non-repeatable and done).
	static int AvailableAt(INT_QST_QuestDef q, INT_QST_Done d, int offset)
	{
		if (!d || d.Times <= 0)
			return 0;
		switch (q.m_Repeat)
		{
			case INT_QST_RM.COOLDOWN:
				return d.Last + q.Cooldown;
			case INT_QST_RM.DAILY:
				return INT_QST_Util.NextDaily(d.Last, q.ResetHour, q.ResetMinute, offset);
			case INT_QST_RM.WEEKLY:
				return INT_QST_Util.NextWeekly(d.Last, q.ResetWeekDay, q.ResetHour, q.ResetMinute, offset);
		}
		return -1;
	}

	// Seconds left of the time limit (-1 = no limit, or the clock stopped: see IsExpired).
	static int TimeLeft(INT_QST_QuestDef q, INT_QST_Active a, int now)
	{
		if (q.TimeLimit <= 0 || !a || ClockStopped(q, a))
			return -1;
		int left = q.TimeLimit - (now - a.Start);
		if (left < 0)
			left = 0;
		return left;
	}

	// Time limit passed. As in the old mod, a quest whose targets are all counted ones (kill/craft/.../explore) stops
	// the clock once they are reached: it waits for the turn-in without a limit. Quests with "bring/find" targets
	// (checked only at the turn-in) or without targets keep the clock running.
	static bool IsExpired(INT_QST_QuestDef q, INT_QST_Active a, int now)
	{
		if (q.TimeLimit <= 0 || !a)
			return false;
		if (now - a.Start < q.TimeLimit)
			return false;
		return !ClockStopped(q, a);
	}

	static bool ClockStopped(INT_QST_QuestDef q, INT_QST_Active a)
	{
		if (q.Targets.Count() == 0 || HasInventoryTargets(q))
			return false;
		return CountedDone(q, a);
	}

	// All counted targets (kill/craft/fish/harvest/explore/action) reached.
	static bool CountedDone(INT_QST_QuestDef q, INT_QST_Active a)
	{
		if (!a)
			return false;
		int n = q.Targets.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_TargetDef t = q.Targets[i];
			if (!INT_QST_TT.IsCounted(t.m_Type))
				continue;
			if (i >= a.Progress.Count())
				return false;
			if (a.Progress[i] < t.m_Need)
				return false;
		}
		return true;
	}

	static bool HasInventoryTargets(INT_QST_QuestDef q)
	{
		foreach (INT_QST_TargetDef t : q.Targets)
		{
			if (INT_QST_TT.IsInventory(t.m_Type))
				return true;
		}
		return false;
	}

	// Status for one player. waitSec: seconds until COOLDOWN ends / time left while ACTIVE (-1 none).
	static int Status(INT_QST_QuestDef q, INT_QST_PlayerFile st, INT_QST_Catalog cat, int now, int offset, out int waitSec)
	{
		waitSec = -1;
		INT_QST_Active a = st.FindActive(q.Id);
		if (a)
		{
			if (IsExpired(q, a, now))
				return INT_QST_St.EXPIRED;
			waitSec = TimeLeft(q, a, now);
			if (CountedDone(q, a))
				return INT_QST_St.READY;
			return INT_QST_St.ACTIVE;
		}
		if (q.Enabled == 0)
			return INT_QST_St.HIDDEN;
		INT_QST_Done d = st.FindDone(q.Id);
		int at = AvailableAt(q, d, offset);
		if (at < 0)
			return INT_QST_St.DONE;
		if (at > now)
		{
			waitSec = at - now;
			return INT_QST_St.COOLDOWN;
		}
		if (!RequiresMet(q, st, cat, now, offset, null))
			return INT_QST_St.LOCKED;
		return INT_QST_St.AVAILABLE;
	}

	// Free slots for new quests.
	static int FreeSlots(INT_QST_PlayerFile st, int slots)
	{
		return slots - st.Active.Count();
	}

	// Kill counted? `killerWeapon` = class of what killed (weapon in hands etc.), `pos` = where the victim died.
	static bool KillMatches(INT_QST_TargetDef t, string victimType, string killerWeapon, vector pos)
	{
		if (!GetGame().IsKindOf(victimType, t.Target) && INT_QST_Util.Lower(victimType) != t.m_ClassLow)
			return false;
		if (t.Filter != "")
		{
			if (killerWeapon == "")
				return false;
			if (!GetGame().IsKindOf(killerWeapon, t.Filter) && INT_QST_Util.Lower(killerWeapon) != t.m_FilterLow)
				return false;
		}
		if (t.m_HasZone)
		{
			float dx = pos[0] - t.m_ZonePos[0];
			float dz = pos[2] - t.m_ZonePos[2];
			if (dx * dx + dz * dz > t.ZoneRadius * t.ZoneRadius)
				return false;
		}
		return true;
	}

	// Class match for craft/fish/harvest (exact or inheritance, case-insensitive).
	static bool ClassMatches(string have, string need, string needLow)
	{
		if (need == "")
			return false;
		if (GetGame().IsKindOf(have, need))
			return true;
		return INT_QST_Util.Lower(have) == needLow;
	}
}
