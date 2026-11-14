// INT Quests — wire format shared by both sides (spec.md §6). Every packet starts with the operation code and the
// protocol version; a packet of another version is dropped by the reader. Every string field stays at or below
// INT_QST_Const.PIECE_BYTES (RPC strings of ~1000+ bytes break on receive, measured on 1.30).

// What HELLO carries besides the state (config subset the client needs).
class INT_QST_Hello
{
	bool IsAdmin;
	bool DebugOn;
	int QuestSlots;
	bool CanCancel;
	bool ShowLocked;
	int AutoTrackMax;
	float TalkDistance;
	bool Money;			// money rewards available (INT_Banking loaded and enabled)
	int ServerUnix;
	int LocalOffset;		// server machine local time minus UTC (daily/weekly resets)
	int CatalogHash;
	int CatalogCount;
	// SIGNS-OFF (owner 2026-10-31): field, write and read together (INT_QST_Const.PROTO was bumped for it)
	// bool ShowSigns;

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(IsAdmin);
		ctx.Write(DebugOn);
		ctx.Write(QuestSlots);
		ctx.Write(CanCancel);
		ctx.Write(ShowLocked);
		ctx.Write(AutoTrackMax);
		ctx.Write(TalkDistance);
		ctx.Write(Money);
		ctx.Write(ServerUnix);
		ctx.Write(LocalOffset);
		ctx.Write(CatalogHash);
		ctx.Write(CatalogCount);
		// SIGNS-OFF
		// ctx.Write(ShowSigns);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(IsAdmin) || !ctx.Read(DebugOn) || !ctx.Read(QuestSlots) || !ctx.Read(CanCancel))
			return false;
		if (!ctx.Read(ShowLocked) || !ctx.Read(AutoTrackMax) || !ctx.Read(TalkDistance) || !ctx.Read(Money))
			return false;
		if (!ctx.Read(ServerUnix) || !ctx.Read(LocalOffset) || !ctx.Read(CatalogHash) || !ctx.Read(CatalogCount))
			return false;
		// SIGNS-OFF
		// if (!ctx.Read(ShowSigns))
		// 	return false;
		return true;
	}
}

// One quest giver as the client sees it (the object is found by its network id when it streams in).
class INT_QST_GiverView
{
	string Key;
	string Title;
	int Kind;
	string Model;
	vector Position;
	vector Orientation;
	int NetLow;
	int NetHigh;
	ref array<string> Clothes = new array<string>();
	string InHands;
	bool Disabled;
	string Greeting;
	// SIGNS-OFF (owner 2026-10-31): field, write and read together (INT_QST_Const.PROTO was bumped for it)
	// bool NoSign;

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(Key);
		ctx.Write(Title);
		ctx.Write(Kind);
		ctx.Write(Model);
		ctx.Write(Position);
		ctx.Write(Orientation);
		ctx.Write(NetLow);
		ctx.Write(NetHigh);
		INT_QST_Wire.WriteStrings(ctx, Clothes, INT_QST_Const.MAX_CLOTHES);
		ctx.Write(InHands);
		ctx.Write(Disabled);
		ctx.Write(Greeting);
		// SIGNS-OFF
		// ctx.Write(NoSign);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Key) || !ctx.Read(Title) || !ctx.Read(Kind) || !ctx.Read(Model))
			return false;
		if (!ctx.Read(Position) || !ctx.Read(Orientation) || !ctx.Read(NetLow) || !ctx.Read(NetHigh))
			return false;
		if (!INT_QST_Wire.ReadStrings(ctx, Clothes, INT_QST_Const.MAX_CLOTHES))
			return false;
		if (!ctx.Read(InHands) || !ctx.Read(Disabled) || !ctx.Read(Greeting))
			return false;
		// SIGNS-OFF
		// if (!ctx.Read(NoSign))
		// 	return false;
		return true;
	}
}

class INT_QST_Wire
{
	static void WriteHeader(ParamsWriteContext ctx, int op)
	{
		ctx.Write(op);
		int ver = INT_QST_Const.PROTO;
		ctx.Write(ver);
	}

	static bool ReadHeader(ParamsReadContext ctx, out int op)
	{
		int ver;
		if (!ctx.Read(op))
			return false;
		if (!ctx.Read(ver))
			return false;
		return ver == INT_QST_Const.PROTO;
	}

	static void WriteStrings(ParamsWriteContext ctx, array<string> list, int maxCount)
	{
		int n = 0;
		if (list)
			n = Math.Min(list.Count(), maxCount);
		ctx.Write(n);
		for (int i = 0; i < n; i++)
		{
			string s = INT_QST_Util.FitBytes(list[i], INT_QST_Const.PIECE_BYTES);
			ctx.Write(s);
		}
	}

	static bool ReadStrings(ParamsReadContext ctx, array<string> list, int maxCount)
	{
		list.Clear();
		int n;
		if (!ctx.Read(n) || n < 0 || n > maxCount)
			return false;
		string s;
		for (int i = 0; i < n; i++)
		{
			if (!ctx.Read(s))
				return false;
			list.Insert(s);
		}
		return true;
	}

	// Player state (active + done) as compact binary.
	static void WriteState(ParamsWriteContext ctx, INT_QST_PlayerFile st)
	{
		int na = st.Active.Count();
		ctx.Write(na);
		foreach (INT_QST_Active a : st.Active)
			WriteActive(ctx, a);
		int nd = st.Done.Count();
		ctx.Write(nd);
		foreach (INT_QST_Done d : st.Done)
		{
			ctx.Write(d.Id);
			ctx.Write(d.Last);
			ctx.Write(d.Times);
		}
	}

	// Reads into new arrays and swaps them in only when the whole state arrived: a short read used to leave the old
	// arrays cleared while the id indexes still pointed at the freed entries (review 2026-10-22).
	static bool ReadState(ParamsReadContext ctx, INT_QST_PlayerFile st)
	{
		array<ref INT_QST_Active> act = new array<ref INT_QST_Active>();
		array<ref INT_QST_Done> done = new array<ref INT_QST_Done>();
		int na;
		if (!ctx.Read(na) || na < 0 || na > 1000)
			return false;
		for (int i = 0; i < na; i++)
		{
			INT_QST_Active a = new INT_QST_Active();
			if (!ReadActive(ctx, a))
				return false;
			act.Insert(a);
		}
		int nd;
		if (!ctx.Read(nd) || nd < 0 || nd > 100000)
			return false;
		for (int j = 0; j < nd; j++)
		{
			INT_QST_Done d = new INT_QST_Done();
			if (!ctx.Read(d.Id) || !ctx.Read(d.Last) || !ctx.Read(d.Times))
				return false;
			done.Insert(d);
		}
		st.Active = act;
		st.Done = done;
		st.Reindex();
		return true;
	}

	static void WriteActive(ParamsWriteContext ctx, INT_QST_Active a)
	{
		ctx.Write(a.Id);
		ctx.Write(a.Start);
		int np = a.Progress.Count();
		ctx.Write(np);
		for (int i = 0; i < np; i++)
		{
			float v = a.Progress[i];
			ctx.Write(v);
		}
	}

	static bool ReadActive(ParamsReadContext ctx, INT_QST_Active a)
	{
		int np;
		if (!ctx.Read(a.Id) || !ctx.Read(a.Start) || !ctx.Read(np))
			return false;
		if (np < 0 || np > INT_QST_Const.MAX_TARGETS)
			return false;
		a.Progress.Clear();
		float v;
		for (int i = 0; i < np; i++)
		{
			if (!ctx.Read(v))
				return false;
			a.Progress.Insert(v);
		}
		return true;
	}
}

// One record of the wire catalog (branch meta or quest) and the client's cache file of the whole catalog.
class INT_QST_WireBranch
{
	string Key;
	string Title;
	string Color;
	int Sort;
}

class INT_QST_WireQuest
{
	string Branch;
	ref INT_QST_QuestDef Q;
}

class INT_QST_CatalogCache
{
	int Version = 2;
	int Hash;
	ref array<ref INT_QST_WireBranch> Branches = new array<ref INT_QST_WireBranch>();
	ref array<ref INT_QST_WireQuest> Quests = new array<ref INT_QST_WireQuest>();
	ref array<int> BranchHashes = new array<int>();	// record hash of each entry (patches update one record)
	ref array<int> QuestHashes = new array<int>();

	bool HasHashes()
	{
		return BranchHashes && QuestHashes && BranchHashes.Count() == Branches.Count() && QuestHashes.Count() == Quests.Count();
	}

	int FindQuest(int id)
	{
		int n = Quests.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_WireQuest wq = Quests[i];
			if (wq && wq.Q && wq.Q.Id == id)
				return i;
		}
		return -1;
	}

	int FindBranch(string key)
	{
		int n = Branches.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_WireBranch wb = Branches[i];
			if (wb && wb.Key == key)
				return i;
		}
		return -1;
	}
}

// Catalog hash: order-dependent mix of the per-quest JSON texts (same on both sides for the same texts).
// Catalog hash = START + the sum of the record hashes (text.Hash() of each record's JSON). The sum does not depend
// on the order, so one changed record moves the total by (new - old) and both sides can check a patch.
class INT_QST_Hash
{
	static const int START = 17;

	static int Record(string text)
	{
		return text.Hash();
	}

	static int Mix(int h, string text)
	{
		int x = text.Hash();
		return h * 31 + x;
	}
}
