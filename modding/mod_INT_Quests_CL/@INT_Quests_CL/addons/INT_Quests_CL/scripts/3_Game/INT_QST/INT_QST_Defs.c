// INT Quests — data classes = the JSON files (spec.md §5). Both sides use them: the server reads the files, the
// client gets the same classes over the wire (JSON text per quest, cut in pieces) and keeps them in its cache.
//
// JSON reader facts (measured in other INT mods on 1.30): the TOP-level object is ours, so its field initializers
// are defaults for missing keys; objects the reader CREATES (array elements) get ZERO fields. Every nested field is
// therefore designed so that 0 / "" means "default" (WorstState "" = any state, ZoneRadius 0 = no zone, Count 0 ->
// normalized to 1). Runtime fields are [NonSerialized()] and filled by Normalize().

class INT_QST_ItemRef
{
	string Item;
	int Count;

	void Normalize()
	{
		Item.TrimInPlace();
		Count = INT_QST_Util.ClampI(Count, 1, INT_QST_Const.MAX_ITEM_COUNT);
	}
}

class INT_QST_TargetDef
{
	string Kind;			// KILL DELIVER SEARCH CRAFT FISH HARVEST EXPLORE ACTION
	string Target;			// killed / delivered / crafted / caught / harvested class, action class name
	float Count;
	string Filter;			// KILL: weapon class (IsKindOf); ACTION: target class (IsKindOf)
	string WorstState;		// DELIVER/SEARCH: worst accepted state ("" any, PRISTINE .. RUINED)
	int MinQuantity;		// DELIVER/SEARCH: minimum quantity percent (0 any)
	string Position;		// EXPLORE: "x y z"
	float Radius;			// EXPLORE: metres (0 -> 5)
	int GroundY;			// EXPLORE: 1 = height from the ground at x/z
	int ShowOnMap;			// EXPLORE: 1 = the point may be shown in the quest window map
	string ZoneCenter;		// KILL: kill only inside this circle ("" = anywhere)
	float ZoneRadius;		// KILL: zone radius in metres (0 = no zone)
	string Text;			// what the player reads ("" = generated from the type and class)

	[NonSerialized()]
	int m_Type;
	[NonSerialized()]
	int m_Need;			// whole units needed
	[NonSerialized()]
	int m_Worst;			// -1 any, 0..4
	[NonSerialized()]
	vector m_Pos;
	[NonSerialized()]
	bool m_HasPos;
	[NonSerialized()]
	vector m_ZonePos;
	[NonSerialized()]
	bool m_HasZone;
	[NonSerialized()]
	string m_ClassLow;
	[NonSerialized()]
	string m_FilterLow;

	void Normalize()
	{
		m_Type = INT_QST_TT.FromString(Kind);
		Target.TrimInPlace();
		Filter.TrimInPlace();
		m_ClassLow = INT_QST_Util.Lower(Target);
		m_FilterLow = INT_QST_Util.Lower(Filter);
		m_Need = (int)Math.Round(Count);
		m_Need = INT_QST_Util.ClampI(m_Need, 1, INT_QST_Const.MAX_TARGET_COUNT);
		m_Worst = INT_QST_State.LevelFromString(WorstState);
		MinQuantity = INT_QST_Util.ClampI(MinQuantity, 0, 100);
		m_HasPos = false;
		if (m_Type == INT_QST_TT.EXPLORE)
		{
			m_Need = 1;
			m_HasPos = INT_QST_Util.ParseVec(Position, m_Pos);
			if (Radius <= 0)
				Radius = INT_QST_Const.DEFAULT_EXPLORE_RADIUS;
		}
		m_HasZone = false;
		if (m_Type == INT_QST_TT.KILL && ZoneRadius > 0)
			m_HasZone = INT_QST_Util.ParseVec(ZoneCenter, m_ZonePos);
	}
}

class INT_QST_RewardDef
{
	string Kind;			// ITEM MONEY
	string Item;			// ITEM: class name
	int Count;			// ITEM: how many items (0 -> 1)
	string Quantity;		// ITEM: "" default, "500", "50%", "max"
	string State;			// ITEM: "" as spawned, PRISTINE .. RUINED
	int Amount;			// MONEY: credited to the INT_Banking account
	string Text;			// optional label

	[NonSerialized()]
	int m_Type;
	[NonSerialized()]
	int m_State;

	void Normalize()
	{
		m_Type = INT_QST_RT.FromString(Kind);
		Item.TrimInPlace();
		Count = INT_QST_Util.ClampI(Count, 1, INT_QST_Const.MAX_ITEM_COUNT);
		m_State = INT_QST_State.LevelFromString(State);
		if (Amount < 0)
			Amount = 0;
	}
}

class INT_QST_QuestDef
{
	int Version = 1;
	int Id;
	int Order;			// NN of the file name inside its branch
	int Enabled = 1;
	string Name;
	// The text in pieces of at most ~900 bytes, joined as they are: the engine JSON reader keeps only 1023 BYTES
	// of one string value (measured 2026-10-22 on 164200: every longer description arrived cut to 1023 bytes).
	ref array<string> Description = new array<string>();
	string GiverStart;		// giver key; "" = taken in the journal (owner decision O13)
	string GiverFinish;		// giver key; "" = completes by itself when the targets are done
	ref array<int> Requires = new array<int>();
	int RequireAny;			// 0 = all of Requires, 1 = any of them
	string RepeatMode = "ONCE";	// ONCE COOLDOWN DAILY WEEKLY
	int Cooldown;			// COOLDOWN: seconds after the turn-in
	int ResetHour = 6;		// DAILY/WEEKLY: server machine time
	int ResetMinute;
	int ResetWeekDay = 1;		// WEEKLY: 1 = Monday .. 7 = Sunday
	int TimeLimit;			// seconds to finish after taking (0 = no limit)
	ref array<ref INT_QST_ItemRef> QuestItems = new array<ref INT_QST_ItemRef>();
	ref array<ref INT_QST_TargetDef> Targets = new array<ref INT_QST_TargetDef>();
	ref array<ref INT_QST_RewardDef> Rewards = new array<ref INT_QST_RewardDef>();
	string Note;			// admin note, never shown to players

	[NonSerialized()]
	string m_Branch;		// folder name (= branch key)
	[NonSerialized()]
	int m_Repeat;
	[NonSerialized()]
	bool m_Instant;			// shop-like: no counted targets, turn in at the same giver right away
	[NonSerialized()]
	string m_Text;			// Description joined
	[NonSerialized()]
	bool m_TextCut;			// a piece reached the reader limit (the admin tab lists it)
	[NonSerialized()]
	string m_File;			// server: file name inside the branch folder
	[NonSerialized()]
	bool m_Trimmed;			// lists were longer than the limits (or held empty entries) and were cut

	void Normalize()
	{
		Name = INT_QST_Util.OneLine(Name, INT_QST_Const.MAX_NAME_BYTES);
		if (!Description)
			Description = new array<string>();
		m_Text = "";
		m_TextCut = false;
		foreach (string piece : Description)
		{
			if (piece.Length() >= INT_QST_Const.JSON_STRING_LIMIT)
				m_TextCut = true;
			m_Text = m_Text + piece;
		}
		m_Text = INT_QST_Util.FitBytes(m_Text, INT_QST_Const.MAX_TEXT_BYTES);
		GiverStart = INT_QST_Util.Lower(GiverStart);
		GiverStart.TrimInPlace();
		GiverFinish = INT_QST_Util.Lower(GiverFinish);
		GiverFinish.TrimInPlace();
		m_Repeat = INT_QST_RM.FromString(RepeatMode);
		Cooldown = INT_QST_Util.ClampI(Cooldown, 0, INT_QST_Const.MAX_COOLDOWN);
		ResetHour = INT_QST_Util.ClampI(ResetHour, 0, 23);
		ResetMinute = INT_QST_Util.ClampI(ResetMinute, 0, 59);
		ResetWeekDay = INT_QST_Util.ClampI(ResetWeekDay, 1, 7);
		TimeLimit = INT_QST_Util.ClampI(TimeLimit, 0, INT_QST_Const.MAX_TIME_LIMIT);
		if (!Requires)
			Requires = new array<int>();
		if (!QuestItems)
			QuestItems = new array<ref INT_QST_ItemRef>();
		if (!Targets)
			Targets = new array<ref INT_QST_TargetDef>();
		if (!Rewards)
			Rewards = new array<ref INT_QST_RewardDef>();
		// no empty entries, no more than the limits (the client refuses a state with more targets than MAX_TARGETS)
		m_Trimmed = false;
		for (int a = QuestItems.Count() - 1; a >= 0; a--)
		{
			if (!QuestItems[a])
			{
				QuestItems.RemoveOrdered(a);
				m_Trimmed = true;
			}
		}
		for (int b = Targets.Count() - 1; b >= 0; b--)
		{
			if (!Targets[b])
			{
				Targets.RemoveOrdered(b);
				m_Trimmed = true;
			}
		}
		for (int c = Rewards.Count() - 1; c >= 0; c--)
		{
			if (!Rewards[c])
			{
				Rewards.RemoveOrdered(c);
				m_Trimmed = true;
			}
		}
		while (QuestItems.Count() > INT_QST_Const.MAX_QUEST_ITEMS)
		{
			QuestItems.RemoveOrdered(QuestItems.Count() - 1);
			m_Trimmed = true;
		}
		while (Targets.Count() > INT_QST_Const.MAX_TARGETS)
		{
			Targets.RemoveOrdered(Targets.Count() - 1);
			m_Trimmed = true;
		}
		while (Rewards.Count() > INT_QST_Const.MAX_REWARDS)
		{
			Rewards.RemoveOrdered(Rewards.Count() - 1);
			m_Trimmed = true;
		}
		while (Requires.Count() > INT_QST_Const.MAX_REQUIRES)
		{
			Requires.RemoveOrdered(Requires.Count() - 1);
			m_Trimmed = true;
		}
		foreach (INT_QST_ItemRef qi : QuestItems)
			qi.Normalize();
		bool counted = false;
		foreach (INT_QST_TargetDef t : Targets)
		{
			t.Normalize();
			if (INT_QST_TT.IsCounted(t.m_Type))
				counted = true;
		}
		foreach (INT_QST_RewardDef r : Rewards)
			r.Normalize();
		// an if chain: `field = (a && b.Call())` corrupts the heap (measured trap)
		bool instant = true;
		if (counted || TimeLimit != 0 || GiverStart == "")
			instant = false;
		if (GiverStart != GiverFinish)
			instant = false;
		int tc = Targets.Count();
		if (tc == 0)
			instant = false;
		m_Instant = instant;
	}

	bool IsRepeatable()
	{
		return m_Repeat != INT_QST_RM.ONCE;
	}
}

class INT_QST_BranchDef
{
	int Version = 1;
	string Title;
	string Color;
	int Sort;

	[NonSerialized()]
	string m_Key;
}

class INT_QST_GiverDef
{
	string Key;
	string Title;
	int Kind;			// 0 = survivor NPC (dressed), 1 = static object (HouseNoDestruct class)
	string Model;			// class name of the NPC / object
	string Position;
	string Orientation;
	ref array<string> Clothes = new array<string>();
	string InHands;
	int Disabled;			// 1 = not spawned (nested object: a missing key reads 0 = enabled)
	string Greeting;		// what the giver says on top of the window ("" = default phrase)
	// SIGNS-OFF (owner 2026-10-31): a "NoSign" key still in Givers.json is skipped by the reader
	// int NoSign;			// 1 = no ! / ? sign above this giver

	[NonSerialized()]
	vector m_Pos;
	[NonSerialized()]
	vector m_Ori;

	void Normalize()
	{
		Key = INT_QST_Util.Lower(Key);
		Key.TrimInPlace();
		Title = INT_QST_Util.OneLine(Title, INT_QST_Const.MAX_NAME_BYTES);
		Greeting = INT_QST_Util.FitBytes(Greeting, INT_QST_Const.MAX_GREETING_BYTES);
		Model.TrimInPlace();
		if (Kind != 1)
			Kind = 0;
		if (!Clothes)
			Clothes = new array<string>();
		INT_QST_Util.ParseVec(Position, m_Pos);
		INT_QST_Util.ParseVec(Orientation, m_Ori);
	}
}

class INT_QST_GiversFile
{
	int Version = 1;
	ref array<ref INT_QST_GiverDef> Givers = new array<ref INT_QST_GiverDef>();
}

class INT_QST_Config
{
	int Version = 1;
	int DebugMode = 0;
	ref array<string> Admins = new array<string>();
	int QuestSlots = 60;
	int CanCancel = 1;
	int ShowLockedAtGiver = 1;
	int AutoTrackMax = 5;
	float TalkDistance = 4.0;
	float ExploreTolerance = 5.0;
	int MoneyRewards = 1;
	// SIGNS-OFF (owner 2026-10-31): a "ShowSigns" key still in the file is skipped by the reader
	// int ShowSigns = 1;		// 0 = no ! / ? signs above any giver (owner 2026-10-22: some servers do not want them)
	// 1 = a thrown grenade's kill counts for the player who pulled its pin (or set it off with a shot). Server 21 had
	// it from RF_KASTETQUESTS_EXTENDED (folded into this mod 2026-10-22); the base old mod counted such kills for nobody.
	int GrenadeKills = 0;

	void Normalize()
	{
		if (!Admins)
			Admins = new array<string>();
		GrenadeKills = INT_QST_Util.ClampI(GrenadeKills, 0, 1);
		QuestSlots = INT_QST_Util.ClampI(QuestSlots, 1, 500);
		AutoTrackMax = INT_QST_Util.ClampI(AutoTrackMax, 0, 10);
		TalkDistance = INT_QST_Util.ClampF(TalkDistance, 1.5, 15.0, 4.0);
		ExploreTolerance = INT_QST_Util.ClampF(ExploreTolerance, 0.0, 50.0, 5.0);
	}
}

// ---- player state (server file Players/<Steam64>.json, sent to the client) ----
class INT_QST_Active
{
	int Id;
	int Start;			// unix UTC when taken
	ref array<float> Progress = new array<float>();	// per target (counted types); 0 for inventory types
	int Sig;			// targets signature when taken: an admin changed the set of targets -> progress starts again

	void Ensure(int targets)
	{
		if (!Progress)
			Progress = new array<float>();
		while (Progress.Count() < targets)
			Progress.Insert(0);
		while (Progress.Count() > targets)
			Progress.Remove(Progress.Count() - 1);
	}
}

class INT_QST_Done
{
	int Id;
	int Last;			// unix UTC of the last turn-in
	int Times;
}

class INT_QST_PlayerFile
{
	int Version = 1;
	ref array<ref INT_QST_Active> Active = new array<ref INT_QST_Active>();
	ref array<ref INT_QST_Done> Done = new array<ref INT_QST_Done>();
	// Server: quests whose quest items were handed out and not all came back (cancel/fail with the items stashed):
	// taking the quest again gives no new items (review 2026-10-22: cancel + retake farmed quest items).
	ref array<int> ItemsOut = new array<int>();

	// Id indexes for the windows (hundreds of done quests are looked up per status). Rebuilt when the array size
	// changed or after Reindex() (the client calls it after every full state); the values are weak, the arrays own.
	[NonSerialized()]
	ref map<int, INT_QST_Active> m_ActIdx;
	[NonSerialized()]
	int m_ActIdxCount = -1;
	[NonSerialized()]
	ref map<int, INT_QST_Done> m_DoneIdx;
	[NonSerialized()]
	int m_DoneIdxCount = -1;

	void Reindex()
	{
		m_ActIdxCount = -1;
		m_DoneIdxCount = -1;
	}

	INT_QST_Active FindActive(int id)
	{
		if (!m_ActIdx || m_ActIdxCount != Active.Count())
		{
			if (!m_ActIdx)
				m_ActIdx = new map<int, INT_QST_Active>();
			m_ActIdx.Clear();
			foreach (INT_QST_Active x : Active)
			{
				if (x && !m_ActIdx.Contains(x.Id))
					m_ActIdx.Set(x.Id, x);
			}
			m_ActIdxCount = Active.Count();
		}
		INT_QST_Active a;
		if (m_ActIdx.Find(id, a) && a && a.Id == id)
			return a;
		return null;
	}

	INT_QST_Done FindDone(int id)
	{
		if (!m_DoneIdx || m_DoneIdxCount != Done.Count())
		{
			if (!m_DoneIdx)
				m_DoneIdx = new map<int, INT_QST_Done>();
			m_DoneIdx.Clear();
			foreach (INT_QST_Done y : Done)
			{
				if (y && !m_DoneIdx.Contains(y.Id))
					m_DoneIdx.Set(y.Id, y);
			}
			m_DoneIdxCount = Done.Count();
		}
		INT_QST_Done d;
		if (m_DoneIdx.Find(id, d) && d && d.Id == id)
			return d;
		return null;
	}

	void RemoveActive(int id)
	{
		for (int i = Active.Count() - 1; i >= 0; i--)
		{
			if (!Active[i] || Active[i].Id == id)
				Active.RemoveOrdered(i);
		}
		Reindex();
	}

	void Normalize()
	{
		if (!Active)
			Active = new array<ref INT_QST_Active>();
		if (!Done)
			Done = new array<ref INT_QST_Done>();
		if (!ItemsOut)
			ItemsOut = new array<int>();
		for (int i = Active.Count() - 1; i >= 0; i--)
		{
			if (!Active[i])
				Active.RemoveOrdered(i);
			else if (!Active[i].Progress)
				Active[i].Progress = new array<float>();
		}
		for (int j = Done.Count() - 1; j >= 0; j--)
		{
			if (!Done[j])
				Done.RemoveOrdered(j);
		}
	}
}
