// INT Quests (server) — counting game events for active quests (spec.md §4.1). Each hook first asks
// INT_QST_SE_Players.Needs(type): when no online player has an unfinished target of that type, the event costs one
// int compare. Otherwise only the acting player's own active quests are walked (at most QuestSlots of them).

class INT_QST_SE_Progress
{
	static void OnKill(Man killer, EntityAI victim, string weapon)
	{
		if (!killer || !victim)
			return;
		INT_QST_SE_Session s = INT_QST_SE_Players.Of(killer);
		if (!s || s.m_Count[INT_QST_TT.KILL] <= 0)
			return;
		string vType = victim.GetType();
		vector vPos = victim.GetPosition();
		Count(s, INT_QST_TT.KILL, vType, weapon, vPos, null);
	}

	static void OnSimple(PlayerBase p, int type, string cls)
	{
		if (!p || !INT_QST_SE_Players.Needs(type))
			return;
		INT_QST_SE_Session s = INT_QST_SE_Players.Of(p);
		if (!s || s.m_Count[type] <= 0)
			return;
		Count(s, type, cls, "", p.GetPosition(), null);
	}

	// Garden harvest: plant class (the old mod's target) and the crop it gives.
	static void OnHarvest(PlayerBase p, string plantType, string crop)
	{
		if (!p || !INT_QST_SE_Players.Needs(INT_QST_TT.HARVEST))
			return;
		INT_QST_SE_Session s = INT_QST_SE_Players.Of(p);
		if (!s || s.m_Count[INT_QST_TT.HARVEST] <= 0)
			return;
		vector pPos = p.GetPosition();
		Count(s, INT_QST_TT.HARVEST, plantType, crop, pPos, null);
	}

	static void OnAction(PlayerBase p, ActionBase action, string targetType)
	{
		if (!p || !action || !INT_QST_SE_Players.Needs(INT_QST_TT.ACTION))
			return;
		INT_QST_SE_Session s = INT_QST_SE_Players.Of(p);
		if (!s || s.m_Count[INT_QST_TT.ACTION] <= 0)
			return;
		string aName = action.ClassName();
		vector pPos = p.GetPosition();
		Count(s, INT_QST_TT.ACTION, aName, targetType, pPos, action);
	}

	protected static bool Matches(int type, INT_QST_TargetDef t, string cls, string extra, vector pos, ActionBase action)
	{
		switch (type)
		{
			case INT_QST_TT.KILL:
				return INT_QST_Rules.KillMatches(t, cls, extra, pos);
			case INT_QST_TT.CRAFT:
			case INT_QST_TT.FISH:
				return INT_QST_Rules.ClassMatches(cls, t.Target, t.m_ClassLow);
			case INT_QST_TT.HARVEST:
				// the old mod's target was the plant (Plant_Tomato); the crop (Tomato) counts as well
				if (INT_QST_Rules.ClassMatches(cls, t.Target, t.m_ClassLow))
					return true;
				return extra != "" && INT_QST_Rules.ClassMatches(extra, t.Target, t.m_ClassLow);
			case INT_QST_TT.ACTION:
				return ActionMatches(t, cls, extra, action);
		}
		return false;
	}

	// Action class: the exact name only (case-insensitive), as in the old mod — ActionMineTree does not count
	// ActionMineTreeBark. Filter = target class (IsKindOf), "" = any.
	protected static bool ActionMatches(INT_QST_TargetDef t, string actionName, string targetType, ActionBase action)
	{
		if (INT_QST_Util.Lower(actionName) != t.m_ClassLow)
			return false;
		if (t.Filter == "")
			return true;
		if (targetType == "")
			return false;
		return GetGame().IsKindOf(targetType, t.Filter) || INT_QST_Util.Lower(targetType) == t.m_FilterLow;
	}

	protected static void Count(INT_QST_SE_Session s, int type, string cls, string extra, vector pos, ActionBase action)
	{
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		int now = INT_QST_Util.UnixNow();
		array<int> changedQuest = new array<int>();
		array<int> changedTarget = new array<int>();
		array<float> changedValue = new array<float>();
		array<int> doneTargets = new array<int>();
		foreach (INT_QST_Active a : s.m_St.Active)
		{
			INT_QST_QuestDef q = cat.Get(a.Id);
			if (!q || INT_QST_Rules.IsExpired(q, a, now))
				continue;
			a.Ensure(q.Targets.Count());
			int n = q.Targets.Count();
			for (int i = 0; i < n; i++)
			{
				INT_QST_TargetDef t = q.Targets[i];
				if (t.m_Type != type)
					continue;
				if (a.Progress[i] >= t.m_Need)
					continue;
				if (!Matches(type, t, cls, extra, pos, action))
					continue;
				float v = a.Progress[i] + 1;
				a.Progress[i] = v;
				changedQuest.Insert(a.Id);
				changedTarget.Insert(i);
				changedValue.Insert(v);
				if (v >= t.m_Need)
					doneTargets.Insert(changedQuest.Count() - 1);
			}
		}
		if (changedQuest.Count() == 0)
			return;
		INT_QST_SE_Players.Save(s);
		INT_QST_SE_Players.Reindex(s);
		for (int c = 0; c < changedQuest.Count(); c++)
			INT_QST_SE_Core.SendProgress(s, changedQuest[c], changedTarget[c], changedValue[c]);
		foreach (int di : doneTargets)
			INT_QST_SE_Core.SendNote(s, INT_QST_Note.TARGET_DONE, changedQuest[di], changedTarget[di], "");
		// quests that just became complete (each once)
		array<int> seen = new array<int>();
		PlayerBase p = INT_QST_SE_Players.PlayerOf(s);
		foreach (int qid : changedQuest)
		{
			if (seen.Find(qid) >= 0)
				continue;
			seen.Insert(qid);
			INT_QST_Active act = s.m_St.FindActive(qid);
			INT_QST_QuestDef qd = cat.Get(qid);
			if (act && qd)
				INT_QST_SE_Core.AfterProgress(s, p, qd, act);
		}
	}
}
