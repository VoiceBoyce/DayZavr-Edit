// INT Quests (server) — online player sessions (spec.md §4.1, §5.5). A session is opened when the player connects
// (one file read) and closed on disconnect. Each session keeps a count of its unfinished counted targets per type,
// and the server keeps the sum: an event (kill, craft, ...) of a type nobody needs right now is dropped after one
// int compare, and the kill hook in 3_Game is disarmed completely while nobody has a kill target.

class INT_QST_SE_Session
{
	string m_Uid;
	string m_HashId;			// identity.GetId(): what vanilla hands to PlayerDisconnected
	string m_Name;
	PlayerIdentity m_Identity;		// weak
	ref INT_QST_PlayerFile m_St;
	ref array<int> m_Count = new array<int>();	// unfinished counted targets per INT_QST_TT
	bool m_Hello;				// the client asked for its state (it is ready for RPCs)
	ref array<int> m_ResetNotes = new array<int>();	// quests whose progress was reset before the client was ready
	float m_LastTurnIn;			// tick time of the last turn-in (INT_QST_SE_Core.TURNIN_GAP)
	bool m_Unsaved;				// the last save failed: written again on the next change and at logout

	void INT_QST_SE_Session()
	{
		for (int i = 0; i < INT_QST_TT.COUNT; i++)
			m_Count.Insert(0);
	}
}

class INT_QST_SE_Players
{
	static ref map<string, ref INT_QST_SE_Session> s_ByUid = new map<string, ref INT_QST_SE_Session>();
	static ref array<int> s_Global = new array<int>();

	static void Init()
	{
		s_Global.Clear();
		for (int i = 0; i < INT_QST_TT.COUNT; i++)
			s_Global.Insert(0);
		INT_QST_SE_Hub.s_Armed = false;
	}

	static INT_QST_SE_Session Get(string uid)
	{
		INT_QST_SE_Session s;
		if (s_ByUid.Find(uid, s))
			return s;
		return null;
	}

	static INT_QST_SE_Session FindByHash(string hash)
	{
		if (hash == "")
			return null;
		foreach (string uid, INT_QST_SE_Session s : s_ByUid)
		{
			if (s.m_HashId == hash)
				return s;
		}
		return null;
	}

	static INT_QST_SE_Session Of(Man m)
	{
		if (!m)
			return null;
		PlayerIdentity id = m.GetIdentity();
		if (!id)
			return null;
		return Get(id.GetPlainId());
	}

	static bool Needs(int type)
	{
		if (type < 0 || type >= s_Global.Count())
			return false;
		return s_Global[type] > 0;
	}

	static INT_QST_SE_Session Open(PlayerIdentity identity)
	{
		if (!identity)
			return null;
		string uid = identity.GetPlainId();
		INT_QST_SE_Session s = Get(uid);
		if (!s)
		{
			s = new INT_QST_SE_Session();
			s.m_Uid = uid;
			s.m_St = INT_QST_SE_Store.LoadPlayer(uid);
			s_ByUid.Set(uid, s);
		}
		s.m_Identity = identity;
		s.m_HashId = identity.GetId();
		s.m_Name = identity.GetName();
		Reindex(s);
		INT_QST_Log.Dbg("сессия открыта " + uid + " активных " + s.m_St.Active.Count().ToString() + " выполнено " + s.m_St.Done.Count().ToString());
		return s;
	}

	static void Close(string uid)
	{
		INT_QST_SE_Session s = Get(uid);
		if (!s)
			return;
		if (s.m_Unsaved)
			Save(s);
		for (int i = 0; i < INT_QST_TT.COUNT; i++)
			s_Global[i] = s_Global[i] - s.m_Count[i];
		s_ByUid.Remove(uid);
		Rearm();
	}

	// Recount the unfinished counted targets of one session (after accept/turn-in/cancel/progress/reload/admin edit).
	// A taken quest whose set of targets an admin changed starts its progress again (owner 2026-10-22): the progress
	// is kept per target position, so a moved or replaced target would count for the wrong one.
	static void Reindex(INT_QST_SE_Session s)
	{
		int i;
		for (i = 0; i < INT_QST_TT.COUNT; i++)
		{
			s_Global[i] = s_Global[i] - s.m_Count[i];
			s.m_Count[i] = 0;
		}
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		bool dirty = false;
		array<int> reset = new array<int>();
		foreach (INT_QST_Active a : s.m_St.Active)
		{
			INT_QST_QuestDef q = cat.Get(a.Id);
			if (!q)
				continue;
			int sig = INT_QST_Rules.TargetSig(q);
			if (a.Sig == 0)
			{
				a.Sig = sig;			// taken before signatures existed: nothing to compare with
				dirty = true;
			}
			else if (a.Sig != sig)
			{
				if (a.Progress)
					a.Progress.Clear();
				a.Sig = sig;
				dirty = true;
				reset.Insert(a.Id);
			}
			a.Ensure(q.Targets.Count());
			int n = q.Targets.Count();
			for (int t = 0; t < n; t++)
			{
				INT_QST_TargetDef td = q.Targets[t];
				if (!INT_QST_TT.IsCounted(td.m_Type))
					continue;
				if (a.Progress[t] >= td.m_Need)
					continue;
				s.m_Count[td.m_Type] = s.m_Count[td.m_Type] + 1;
			}
		}
		for (i = 0; i < INT_QST_TT.COUNT; i++)
			s_Global[i] = s_Global[i] + s.m_Count[i];
		Rearm();
		if (!dirty)
			return;
		Save(s);
		if (reset.Count() == 0)
			return;
		foreach (int rid : reset)
		{
			INT_QST_QuestDef rq = cat.Get(rid);
			string rname = "";
			if (rq)
				rname = rq.Name;
			INT_QST_SE_FileLog.Line("сброшен прогресс: " + s.m_Name + " (" + s.m_Uid + ") [" + rid.ToString() + "] " + rname + " — админ изменил цели");
			if (!s.m_Hello)
				s.m_ResetNotes.Insert(rid);
		}
		if (!s.m_Hello)
			return;
		INT_QST_SE_Core.SendState(s);
		foreach (int nid : reset)
			INT_QST_SE_Core.SendNote(s, INT_QST_Note.RESET, nid, 0, "");
	}

	// Banners for progress reset while the client was not ready yet (login after an admin edit).
	static void FlushResetNotes(INT_QST_SE_Session s)
	{
		foreach (int id : s.m_ResetNotes)
			INT_QST_SE_Core.SendNote(s, INT_QST_Note.RESET, id, 0, "");
		s.m_ResetNotes.Clear();
	}

	static void ReindexAll()
	{
		foreach (string uid, INT_QST_SE_Session s : s_ByUid)
			Reindex(s);
	}

	static void Rearm()
	{
		INT_QST_SE_Hub.s_Armed = s_Global[INT_QST_TT.KILL] > 0;
	}

	static bool Save(INT_QST_SE_Session s)
	{
		bool ok = INT_QST_SE_Store.SavePlayer(s.m_Uid, s.m_St);
		if (!ok && !s.m_Unsaved)
			INT_QST_SE_FileLog.Line("ОШИБКА: файл игрока " + s.m_Uid + " не записан — повтор при следующем изменении и при выходе");
		s.m_Unsaved = !ok;
		return ok;
	}

	// Online player object of a session (null when the character is not in the world).
	static PlayerBase PlayerOf(INT_QST_SE_Session s)
	{
		if (!s || !s.m_Identity)
			return null;
		return PlayerBase.Cast(s.m_Identity.GetPlayer());
	}
}
