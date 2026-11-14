// INT Quests (client) — state, catalog cache and requests (spec.md §4.2, §6). RPCs arrive through
// DayZGame.Event_OnRPC (no modded DayZGame / PlayerBase.OnRPC). The catalog is kept in this client's profile per
// server; it is pulled again only when the server's hash differs, one packet at a time (the client paces the
// download, the server never loops). Windows and the HUD (5_Mission) listen to s_OnChanged / s_OnNote.

class INT_QST_ClientPrefsServer
{
	string Key;
	ref array<int> Tracked = new array<int>();
	ref array<int> Untracked = new array<int>();	// quests the player switched off (never auto-tracked again)
	ref array<int> MapOff = new array<int>();	// taken quests whose points the player hid on the map (VPP / RF Map)
}

class INT_QST_ClientPrefs
{
	int Version = 1;
	ref array<ref INT_QST_ClientPrefsServer> Servers = new array<ref INT_QST_ClientPrefsServer>();
}

class INT_QST_Client
{
	static const string DIR = "$profile:INT_Mods/INT_QUESTS";

	static ref INT_QST_Hello s_Hello;
	static ref INT_QST_Catalog s_Catalog = new INT_QST_Catalog();
	protected static ref INT_QST_CatalogCache s_Cache;	// the records behind s_Catalog (patches edit it)
	protected static float s_CacheSaveAt;			// >0: write the cache file then (patches come in bursts)
	static ref INT_QST_PlayerFile s_State = new INT_QST_PlayerFile();
	static ref array<ref INT_QST_GiverView> s_Givers = new array<ref INT_QST_GiverView>();
	static ref INT_QST_ClientPrefs s_Prefs;
	static ref INT_QST_ClientPrefsServer s_Server;
	static string s_ServerKey;

	static bool s_HelloDone;
	static bool s_CatalogReady;
	static int s_ClockSkew;			// server unix - local unix at the last HELLO/STATE
	static int s_Revision;			// bumped on every change (UI refresh)
	static int s_ShapeRevision;		// bumped on every change except a progress counter (PROGRESS): the
						// windows may update numbers only while it stays the same

	// events: s_OnChanged() ; s_OnNote(int kind, int questId, int a, string text) ; s_OnResult(int op, int code, int questId)
	static ref ScriptInvoker s_OnChanged = new ScriptInvoker();
	static ref ScriptInvoker s_OnNote = new ScriptInvoker();
	static ref ScriptInvoker s_OnResult = new ScriptInvoker();

	protected static bool s_Hooked;
	protected static float s_NextHello;
	protected static float s_TickAcc;
	protected static float s_ExploreAcc;
	protected static float s_ResolveAcc;

	// catalog download
	protected static int s_PullHash;
	static const int PULL_WINDOW = 4;
	protected static bool s_Pulling;
	protected static int s_PullNext;		// next packet index to request
	protected static int s_PullCount;		// 0 = unknown yet
	protected static int s_PullReceived;
	protected static ref array<bool> s_PullGot = new array<bool>();
	protected static float s_PullDeadline;
	protected static int s_PullRetries;		// 1..PULL_FAST_TRIES every 8 s, then every 25 s until it works
	static const int PULL_FAST_TRIES = 5;
	protected static ref map<int, ref array<string>> s_Pieces = new map<int, ref array<string>>();
	protected static ref map<int, int> s_PieceKinds = new map<int, int>();
	protected static ref map<int, int> s_PieceCounts = new map<int, int>();
	protected static int s_RecordCount;

	// exploration reports already sent ("id:ti" -> time)
	protected static ref map<string, float> s_ExploreSent = new map<string, float>();

	static void Init()
	{
		if (!s_Hooked)
		{
			DayZGame.Event_OnRPC.Insert(OnRPC);
			s_Hooked = true;
		}
		// a new session (another server, a reconnect): nothing of the last one stays
		s_Hello = null;
		s_HelloDone = false;
		s_CatalogReady = false;
		s_NextHello = 0;
		s_Catalog.Clear();
		s_Cache = null;
		s_CacheSaveAt = 0;
		s_State = new INT_QST_PlayerFile();
		s_Givers.Clear();
		s_Server = null;
		s_ServerKey = "";
		s_ClockSkew = 0;
		s_ExploreSent.Clear();
		ResetPull();
		INT_QST_GiverReg.Clear();
		INT_QST_Admin.Reset();
		LoadPrefs();
	}

	static void Shutdown()
	{
		if (s_Hooked && DayZGame.Event_OnRPC)
			DayZGame.Event_OnRPC.Remove(OnRPC);
		s_Hooked = false;
		s_HelloDone = false;
		// patches of the last seconds are written now (else the next login downloads the catalog again)
		if (s_CacheSaveAt > 0 && s_Cache && s_ServerKey != "")
			SaveCache(s_Cache);
		s_CacheSaveAt = 0;
		ResetPull();
		INT_QST_GiverReg.Clear();
	}

	protected static void ResetPull()
	{
		s_Pulling = false;
		s_PullHash = 0;
		s_PullNext = 0;
		s_PullCount = 0;
		s_PullReceived = 0;
		s_PullRetries = 0;
		s_PullDeadline = 0;
		s_PullGot.Clear();
		s_Pieces.Clear();
		s_PieceKinds.Clear();
		s_PieceCounts.Clear();
		s_RecordCount = 0;
	}

	static bool IsAdmin()
	{
		return s_Hello && s_Hello.IsAdmin;
	}

	// Server time estimate (unix UTC).
	static int Now()
	{
		return INT_QST_Util.UnixNow() + s_ClockSkew;
	}

	static int Offset()
	{
		if (s_Hello)
			return s_Hello.LocalOffset;
		return 0;
	}

	static void Changed()
	{
		s_Revision++;
		s_ShapeRevision++;
		s_OnChanged.Invoke();
	}

	// Only a progress counter moved (kills, crafts ...): s_ShapeRevision stays.
	protected static void ProgressChanged()
	{
		s_Revision++;
		s_OnChanged.Invoke();
	}

	// Record hash of quest `id` in the catalog cache: the server keeps the same value per quest, so an admin edit
	// sends it as BaseRec and the server refuses (CONFLICT) a save over a version changed by somebody else meanwhile.
	// 0 = not known (no check).
	static int QuestRec(int id)
	{
		if (!s_Cache || !s_Cache.HasHashes())
			return 0;
		int qi = s_Cache.FindQuest(id);
		if (qi < 0)
			return 0;
		return s_Cache.QuestHashes[qi];
	}

	// ---- per frame (from MissionGameplay.OnUpdate) ---------------------------------------------------------------
	static void Tick(float dt)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		float now = GetGame().GetTickTime();
		if (!s_HelloDone && now >= s_NextHello)
		{
			s_NextHello = now + 5.0;
			Send(INT_QST_C2S.HELLO, null);
		}
		if (s_Pulling && now > s_PullDeadline)
		{
			// a lost answer: a few fast retries, then a slow one every 25 s for as long as it takes (giving up left
			// the player without quests for the whole session); every 4th slow retry also asks HELLO again, whose
			// hash restarts the download when the catalog changed meanwhile
			s_PullRetries++;
			RetryPull();
			if (s_PullRetries > PULL_FAST_TRIES)
			{
				s_PullDeadline = now + 25.0;
				int slow = s_PullRetries - PULL_FAST_TRIES;
				if (slow % 4 == 0)
					Send(INT_QST_C2S.HELLO, null);
			}
		}
		s_ResolveAcc += dt;
		if (s_ResolveAcc >= 1.0)
		{
			s_ResolveAcc = 0;
			INT_QST_GiverReg.ResolveNear(p.GetPosition(), now);
		}
		if (s_CacheSaveAt > 0 && now >= s_CacheSaveAt && s_Cache)
		{
			s_CacheSaveAt = 0;
			SaveCache(s_Cache);
		}
		s_ExploreAcc += dt;
		if (s_ExploreAcc >= 0.5)
		{
			s_ExploreAcc = 0;
			CheckExplore(p, now);
		}
	}

	// Own exploration targets: report once when inside the radius; the server validates the position.
	protected static void CheckExplore(PlayerBase p, float now)
	{
		if (!s_CatalogReady || !p.IsAlive())
			return;
		vector pp = p.GetPosition();
		foreach (INT_QST_Active a : s_State.Active)
		{
			INT_QST_QuestDef q = s_Catalog.Get(a.Id);
			if (!q)
				continue;
			int n = q.Targets.Count();
			for (int i = 0; i < n; i++)
			{
				INT_QST_TargetDef t = q.Targets[i];
				if (t.m_Type != INT_QST_TT.EXPLORE || !t.m_HasPos)
					continue;
				if (i < a.Progress.Count() && a.Progress[i] >= 1)
					continue;
				float d;
				if (t.GroundY == 1)
				{
					float dx = pp[0] - t.m_Pos[0];
					float dz = pp[2] - t.m_Pos[2];
					d = Math.Sqrt(dx * dx + dz * dz);
				}
				else
				{
					d = vector.Distance(pp, t.m_Pos);
				}
				if (d > t.Radius)
					continue;
				string k = a.Id.ToString() + ":" + i.ToString();
				float last;
				if (s_ExploreSent.Find(k, last) && now - last < 10.0)
					continue;
				s_ExploreSent.Set(k, now);
				Explored(a.Id, i);
			}
		}
	}

	// ---- requests ------------------------------------------------------------------------------------------------
	protected static void Send(int op, ScriptRPC prepared)
	{
		ScriptRPC rpc = prepared;
		if (!rpc)
		{
			rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, op);
		}
		rpc.Send(null, INT_QST_Const.RPC_C2S, true, null);
	}

	static void Accept(int id, string giverKey)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.ACCEPT);
		rpc.Write(id);
		rpc.Write(giverKey);
		Send(INT_QST_C2S.ACCEPT, rpc);
	}

	static void TurnIn(int id, string giverKey)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.TURNIN);
		rpc.Write(id);
		rpc.Write(giverKey);
		Send(INT_QST_C2S.TURNIN, rpc);
	}

	static void Cancel(int id)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.CANCEL);
		rpc.Write(id);
		Send(INT_QST_C2S.CANCEL, rpc);
	}

	static void Dismiss(int id)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.DISMISS);
		rpc.Write(id);
		Send(INT_QST_C2S.DISMISS, rpc);
	}

	static void Explored(int id, int ti)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.EXPLORED);
		rpc.Write(id);
		rpc.Write(ti);
		Send(INT_QST_C2S.EXPLORED, rpc);
	}

	// Admin packets are built by the admin windows (op + payload) and sent here.
	static ScriptRPC AdminPacket(int adminOp)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.ADMIN);
		rpc.Write(adminOp);
		return rpc;
	}

	static void SendAdmin(ScriptRPC rpc)
	{
		Send(INT_QST_C2S.ADMIN, rpc);
	}

	protected static void RequestPacket(int k)
	{
		ScriptRPC rpc = new ScriptRPC();
		INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.CATALOG_PULL);
		rpc.Write(s_PullHash);
		rpc.Write(k);
		s_PullDeadline = GetGame().GetTickTime() + 8.0;
		Send(INT_QST_C2S.CATALOG_PULL, rpc);
	}

	protected static void StartPull(int hash)
	{
		s_PullHash = hash;
		s_PullNext = 1;
		s_PullCount = 0;		// unknown until the first packet arrives
		s_PullReceived = 0;
		s_PullGot.Clear();
		s_Pulling = true;
		s_PullRetries = 0;
		s_Pieces.Clear();
		s_PieceKinds.Clear();
		s_PieceCounts.Clear();
		s_RecordCount = 0;
		INT_QST_Log.Dbg("каталог: загрузка с сервера, хэш " + hash.ToString());
		RequestPacket(0);
	}

	// ---- network in ----------------------------------------------------------------------------------------------
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type != INT_QST_Const.RPC_S2C)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		int op;
		if (!INT_QST_Wire.ReadHeader(ctx, op))
			return;
		switch (op)
		{
			case INT_QST_S2C.HELLO:
				OnHello(ctx);
				return;
			case INT_QST_S2C.CATALOG:
				OnCatalogPacket(ctx);
				return;
			case INT_QST_S2C.CATALOG_CHANGED:
				int h;
				if (ctx.Read(h))
					OnCatalogHash(h);
				return;
			case INT_QST_S2C.CATALOG_PATCH:
				OnPatch(ctx);
				return;
			case INT_QST_S2C.STATE:
				int su;
				if (!ctx.Read(su))
					return;
				// read into a new file: a packet that breaks half way must not leave a half state behind
				INT_QST_PlayerFile st = new INT_QST_PlayerFile();
				if (INT_QST_Wire.ReadState(ctx, st))
				{
					s_ClockSkew = su - INT_QST_Util.UnixNow();
					s_State = st;
					FixTracking();
					Changed();
				}
				return;
			case INT_QST_S2C.PROGRESS:
				OnProgress(ctx);
				return;
			case INT_QST_S2C.RESULT:
				int forOp;
				int code;
				int rid;
				if (ctx.Read(forOp) && ctx.Read(code) && ctx.Read(rid))
					s_OnResult.Invoke(forOp, code, rid);
				return;
			case INT_QST_S2C.NOTE:
				int kind;
				int nid;
				int na;
				string text;
				if (ctx.Read(kind) && ctx.Read(nid) && ctx.Read(na) && ctx.Read(text))
				{
					if (kind == INT_QST_Note.ACCEPTED)
						AutoTrack(nid);
					s_OnNote.Invoke(kind, nid, na, text);
				}
				return;
			case INT_QST_S2C.GIVERS:
				if (ReadGivers(ctx))
					Changed();
				return;
			case INT_QST_S2C.ADMIN:
				INT_QST_Admin.OnPacket(ctx);
				return;
		}
	}

	protected static void OnHello(ParamsReadContext ctx)
	{
		// everything is read first and applied only when the whole packet was good
		INT_QST_Hello h = new INT_QST_Hello();
		if (!h.Read(ctx))
			return;
		array<ref INT_QST_GiverView> givers = new array<ref INT_QST_GiverView>();
		if (!ReadGiverList(ctx, givers))
			return;
		INT_QST_PlayerFile st = new INT_QST_PlayerFile();
		if (!INT_QST_Wire.ReadState(ctx, st))
			return;
		ApplyGivers(givers);
		s_State = st;
		s_Hello = h;
		s_HelloDone = true;
		s_ClockSkew = h.ServerUnix - INT_QST_Util.UnixNow();
		INT_QST_Log.s_Debug = h.DebugOn;
		ServerPrefs();
		OnCatalogHash(h.CatalogHash);
		FixTracking();
		Changed();
	}

	protected static bool ReadGivers(ParamsReadContext ctx)
	{
		array<ref INT_QST_GiverView> list = new array<ref INT_QST_GiverView>();
		if (!ReadGiverList(ctx, list))
			return false;
		ApplyGivers(list);
		return true;
	}

	protected static bool ReadGiverList(ParamsReadContext ctx, array<ref INT_QST_GiverView> list)
	{
		list.Clear();
		int n;
		if (!ctx.Read(n) || n < 0 || n > INT_QST_Const.MAX_GIVERS)
			return false;
		for (int i = 0; i < n; i++)
		{
			INT_QST_GiverView v = new INT_QST_GiverView();
			if (!v.Read(ctx))
				return false;
			list.Insert(v);
		}
		return true;
	}

	protected static void ApplyGivers(array<ref INT_QST_GiverView> list)
	{
		s_Givers = list;
		// keep the objects already found for givers whose network id did not change (no sign flicker)
		map<string, Object> known = new map<string, Object>();
		string lowText;
		string highText;
		foreach (INT_QST_GiverSlot old : INT_QST_GiverReg.s_Slots)
		{
			if (!old.m_Obj)
				continue;
			lowText = old.m_NetLow.ToString();
			highText = old.m_NetHigh.ToString();
			known.Set(old.m_Key + "|" + lowText + "|" + highText, old.m_Obj);
		}
		INT_QST_GiverReg.Clear();
		foreach (INT_QST_GiverView g : s_Givers)
		{
			if (g.Disabled)
				continue;
			INT_QST_GiverSlot s = INT_QST_GiverReg.Put(g.Key);
			Object was;
			lowText = g.NetLow.ToString();
			highText = g.NetHigh.ToString();
			string knownKey = g.Key + "|" + lowText + "|" + highText;
			if (known.Find(knownKey, was))
				s.m_Obj = was;
			s.m_Title = g.Title;
			s.m_Kind = g.Kind;
			s.m_Class = g.Model;
			s.m_Pos = g.Position;
			s.m_NetLow = g.NetLow;
			s.m_NetHigh = g.NetHigh;
			// SIGNS-OFF (owner 2026-10-31)
			// s.m_NoSign = g.NoSign;
		}
	}

	static INT_QST_GiverView Giver(string key)
	{
		foreach (INT_QST_GiverView g : s_Givers)
		{
			if (g.Key == key)
				return g;
		}
		return null;
	}

	static string GiverTitle(string key)
	{
		INT_QST_GiverView g = Giver(key);
		if (g && g.Title != "")
			return g.Title;
		return key;
	}

	protected static void OnProgress(ParamsReadContext ctx)
	{
		int id;
		int ti;
		float v;
		if (!ctx.Read(id) || !ctx.Read(ti) || !ctx.Read(v))
			return;
		INT_QST_Active a = s_State.FindActive(id);
		if (!a)
			return;
		while (a.Progress.Count() <= ti && a.Progress.Count() < INT_QST_Const.MAX_TARGETS)
			a.Progress.Insert(0);
		if (ti >= 0 && ti < a.Progress.Count())
			a.Progress[ti] = v;
		ProgressChanged();
	}

	// The server's hash: use the cache file when it matches, else download.
	protected static void OnCatalogHash(int hash)
	{
		if (!s_HelloDone)
			return;		// the cache file is per server (key from HELLO); HELLO brings the hash anyway
		if (s_CatalogReady && s_Catalog.m_Hash == hash)
			return;
		if (s_Pulling && s_PullHash == hash)
			return;		// this download is running already (a repeated HELLO must not restart it)
		if (LoadCache(hash))
		{
			ResetPull();	// a download of an older hash may still run: it is over
			s_CatalogReady = true;
			FixTracking();
			Changed();
			return;
		}
		s_CatalogReady = false;
		StartPull(hash);
	}

	protected static void OnCatalogPacket(ParamsReadContext ctx)
	{
		int hash;
		int k;
		int packets;
		int records;
		int n;
		if (!ctx.Read(hash) || !ctx.Read(k) || !ctx.Read(packets) || !ctx.Read(records) || !ctx.Read(n))
			return;
		if (hash != s_PullHash || packets <= 0)
			return;
		if (s_PullCount == 0)
		{
			s_PullCount = packets;
			s_PullGot.Clear();
			for (int g = 0; g < packets; g++)
				s_PullGot.Insert(false);
		}
		if (k < 0 || k >= s_PullCount || s_PullGot[k])
			return;
		s_RecordCount = records;
		int rec;
		int kind;
		int pidx;
		int pcnt;
		string text;
		for (int i = 0; i < n; i++)
		{
			if (!ctx.Read(rec) || !ctx.Read(kind) || !ctx.Read(pidx) || !ctx.Read(pcnt) || !ctx.Read(text))
				return;
			array<string> parts;
			if (!s_Pieces.Find(rec, parts))
			{
				parts = new array<string>();
				for (int j = 0; j < pcnt; j++)
					parts.Insert("");
				s_Pieces.Set(rec, parts);
				s_PieceKinds.Set(rec, kind);
				s_PieceCounts.Set(rec, pcnt);
			}
			if (pidx >= 0 && pidx < parts.Count())
				parts[pidx] = text;
		}
		s_PullGot[k] = true;
		s_PullReceived++;
		s_PullRetries = 0;
		s_PullDeadline = GetGame().GetTickTime() + 8.0;
		if (s_PullReceived >= s_PullCount)
		{
			Assemble();
			return;
		}
		// keep PULL_WINDOW requests in flight (the server only answers; the client paces the download)
		while (s_PullNext < s_PullCount && s_PullNext < s_PullReceived + PULL_WINDOW)
		{
			RequestPacket(s_PullNext);
			s_PullNext++;
		}
	}

	// A lost answer: ask again for the first packet that did not arrive.
	protected static void RetryPull()
	{
		if (s_PullCount == 0)
		{
			RequestPacket(0);
			return;
		}
		for (int i = 0; i < s_PullCount && i < s_PullNext; i++)
		{
			if (!s_PullGot[i])
			{
				RequestPacket(i);
				return;
			}
		}
	}

	protected static void Assemble()
	{
		INT_QST_CatalogCache cache = new INT_QST_CatalogCache();
		cache.Hash = s_PullHash;
		JsonSerializer js = new JsonSerializer();
		string err;
		for (int r = 0; r < s_RecordCount; r++)
		{
			array<string> parts;
			if (!s_Pieces.Find(r, parts))
				continue;
			string txt = "";
			foreach (string piece : parts)
				txt = txt + piece;
			int kind = s_PieceKinds.Get(r);
			int rh = INT_QST_Hash.Record(txt);
			if (kind == 1)
			{
				INT_QST_WireBranch wb = new INT_QST_WireBranch();
				if (js.ReadFromString(wb, txt, err))
				{
					cache.Branches.Insert(wb);
					cache.BranchHashes.Insert(rh);
				}
			}
			else
			{
				INT_QST_WireQuest wq = new INT_QST_WireQuest();
				if (js.ReadFromString(wq, txt, err) && wq.Q)
				{
					cache.Quests.Insert(wq);
					cache.QuestHashes.Insert(rh);
				}
				else
				{
					string recText = r.ToString();
					string errText = INT_QST_Log.Clip(err, 80);
					INT_QST_Log.Warn("каталог: запись " + recText + " не читается " + errText);
				}
			}
		}
		ResetPull();
		ApplyCache(cache);
		SaveCache(cache);
		s_CatalogReady = true;
		INT_QST_Log.Dbg("каталог загружен: квестов " + s_Catalog.m_List.Count().ToString());
		FixTracking();
		Changed();
	}

	protected static void ApplyCache(INT_QST_CatalogCache cache)
	{
		s_Cache = cache;
		s_Catalog.Clear();
		foreach (INT_QST_WireBranch wb : cache.Branches)
		{
			INT_QST_BranchDef b = new INT_QST_BranchDef();
			b.m_Key = wb.Key;
			b.Title = wb.Title;
			b.Color = wb.Color;
			b.Sort = wb.Sort;
			s_Catalog.m_Branches.Set(wb.Key, b);
			s_Catalog.m_BranchOrder.Insert(wb.Key);
		}
		foreach (INT_QST_WireQuest wq : cache.Quests)
		{
			if (!wq.Q)
				continue;
			wq.Q.m_Branch = wq.Branch;
			wq.Q.Normalize();
			s_Catalog.Add(wq.Q);
		}
		s_Catalog.m_Hash = cache.Hash;
		// patched records sit at the end of the cache file: the order comes from Sort / Order, not from the file
		s_Catalog.Resort();
	}

	// One changed record (admin edit). Applied only when it fits this copy exactly (old total matches and the new
	// total comes out right); anything else falls back to the normal check / download by the new hash.
	protected static void OnPatch(ParamsReadContext ctx)
	{
		int oldTotal;
		int newTotal;
		int kind;
		int del;
		string key;
		int n;
		if (!ctx.Read(oldTotal) || !ctx.Read(newTotal) || !ctx.Read(kind) || !ctx.Read(del) || !ctx.Read(key) || !ctx.Read(n))
			return;
		if (n < 0 || n > 64)
			return;
		string txt = "";
		for (int i = 0; i < n; i++)
		{
			string piece;
			if (!ctx.Read(piece))
				return;
			txt = txt + piece;
		}
		if (!s_CatalogReady || s_Pulling)
			return;		// a running download ends with the server's current hash anyway
		if (!s_Cache || !s_Cache.HasHashes() || s_Catalog.m_Hash != oldTotal)
		{
			OnCatalogHash(newTotal);
			return;
		}
		if (!ApplyPatch(kind, del, key, txt, oldTotal, newTotal))
		{
			INT_QST_Log.Dbg("каталог: заплатка не подошла, загрузка заново");
			StartPull(newTotal);
			return;
		}
		INT_QST_Log.Dbg("каталог: заплатка " + key + ", хэш " + newTotal.ToString());
		s_CacheSaveAt = GetGame().GetTickTime() + 3.0;
		FixTracking();
		Changed();
	}

	protected static bool ApplyPatch(int kind, int del, string key, string txt, int oldTotal, int newTotal)
	{
		JsonSerializer js = new JsonSerializer();
		string err;
		int oldRec = 0;
		int newRec = 0;
		if (kind == 1)
		{
			int bi = s_Cache.FindBranch(key);
			if (bi >= 0)
				oldRec = s_Cache.BranchHashes[bi];
			INT_QST_WireBranch wb = null;
			if (del == 0)
			{
				wb = new INT_QST_WireBranch();
				if (!js.ReadFromString(wb, txt, err))
					return false;
				newRec = INT_QST_Hash.Record(txt);
			}
			if (oldTotal - oldRec + newRec != newTotal)
				return false;
			if (bi >= 0)
			{
				s_Cache.Branches.RemoveOrdered(bi);
				s_Cache.BranchHashes.RemoveOrdered(bi);
			}
			s_Catalog.m_Branches.Remove(key);
			if (wb)
			{
				s_Cache.Branches.Insert(wb);
				s_Cache.BranchHashes.Insert(newRec);
				INT_QST_BranchDef b = new INT_QST_BranchDef();
				b.m_Key = wb.Key;
				b.Title = wb.Title;
				b.Color = wb.Color;
				b.Sort = wb.Sort;
				s_Catalog.m_Branches.Set(wb.Key, b);
			}
		}
		else
		{
			int id = key.ToInt();
			int qi = s_Cache.FindQuest(id);
			if (qi >= 0)
				oldRec = s_Cache.QuestHashes[qi];
			INT_QST_WireQuest wq = null;
			if (del == 0)
			{
				wq = new INT_QST_WireQuest();
				if (!js.ReadFromString(wq, txt, err) || !wq.Q)
					return false;
				newRec = INT_QST_Hash.Record(txt);
			}
			if (oldTotal - oldRec + newRec != newTotal)
				return false;
			if (qi >= 0)
			{
				s_Cache.Quests.RemoveOrdered(qi);
				s_Cache.QuestHashes.RemoveOrdered(qi);
			}
			s_Catalog.RemoveQuest(id);
			if (wq)
			{
				s_Cache.Quests.Insert(wq);
				s_Cache.QuestHashes.Insert(newRec);
				wq.Q.m_Branch = wq.Branch;
				wq.Q.Normalize();
				s_Catalog.Add(wq.Q);
			}
		}
		s_Cache.Hash = newTotal;
		s_Catalog.m_Hash = newTotal;
		s_Catalog.Resort();
		return true;
	}

	// ---- client files ----------------------------------------------------------------------------------------------
	protected static void MakeDir()
	{
		if (!FileExist("$profile:INT_Mods"))
			MakeDirectory("$profile:INT_Mods");
		if (!FileExist(DIR))
			MakeDirectory(DIR);
	}

	protected static string CachePath()
	{
		return DIR + "/Cache_" + s_ServerKey + ".json";
	}

	protected static bool LoadCache(int hash)
	{
		string path = CachePath();
		if (!FileExist(path))
			return false;
		INT_QST_CatalogCache cache = new INT_QST_CatalogCache();
		string err;
		if (!JsonFileLoader<INT_QST_CatalogCache>.LoadFile(path, cache, err))
			return false;
		if (cache.Hash != hash)
			return false;
		ApplyCache(cache);
		INT_QST_Log.Dbg("каталог из кэша: квестов " + s_Catalog.m_List.Count().ToString());
		return true;
	}

	protected static void SaveCache(INT_QST_CatalogCache cache)
	{
		MakeDir();
		string err;
		JsonFileLoader<INT_QST_CatalogCache>.SaveFile(CachePath(), cache, err);
	}

	protected static void LoadPrefs()
	{
		s_Prefs = new INT_QST_ClientPrefs();
		string path = DIR + "/INT_QUESTS_Client.json";
		if (FileExist(path))
		{
			string err;
			if (!JsonFileLoader<INT_QST_ClientPrefs>.LoadFile(path, s_Prefs, err))
				s_Prefs = new INT_QST_ClientPrefs();
		}
		if (!s_Prefs.Servers)
			s_Prefs.Servers = new array<ref INT_QST_ClientPrefsServer>();
	}

	static void SavePrefs()
	{
		MakeDir();
		string err;
		JsonFileLoader<INT_QST_ClientPrefs>.SaveFile(DIR + "/INT_QUESTS_Client.json", s_Prefs, err);
	}

	// Per-server part of the prefs (tracking) and the cache key "<address>_<port>".
	protected static void ServerPrefs()
	{
		string addr;
		int port;
		GetGame().GetHostAddress(addr, port);
		string key = INT_QST_Util.ReplaceByte(addr, ".", "_") + "_" + port.ToString();
		key = INT_QST_Util.ReplaceByte(key, ":", "_");
		if (key == "_0")
			key = "local";
		s_ServerKey = key;
		s_Server = null;
		foreach (INT_QST_ClientPrefsServer ps : s_Prefs.Servers)
		{
			if (ps && ps.Key == key)
				s_Server = ps;
		}
		if (!s_Server)
		{
			s_Server = new INT_QST_ClientPrefsServer();
			s_Server.Key = key;
			s_Prefs.Servers.Insert(s_Server);
		}
		if (!s_Server.Tracked)
			s_Server.Tracked = new array<int>();
		if (!s_Server.Untracked)
			s_Server.Untracked = new array<int>();
		if (!s_Server.MapOff)
			s_Server.MapOff = new array<int>();
	}

	// ---- the quest's points on the map (VPP / RF Map layer, 5_Mission INT_QST_MapLayer); shown unless switched off
	static bool IsOnMap(int id)
	{
		return !s_Server || s_Server.MapOff.Find(id) < 0;
	}

	static void SetOnMap(int id, bool on)
	{
		if (!s_Server)
			return;
		s_Server.MapOff.RemoveItem(id);
		if (!on)
			s_Server.MapOff.Insert(id);
		SavePrefs();
		Changed();
	}

	// ---- tracking (owner decision O19: new quests auto-tracked while fewer than AutoTrackMax are shown) ------------
	static bool IsTracked(int id)
	{
		return s_Server && s_Server.Tracked.Find(id) >= 0;
	}

	// False when the tracker is full (TRACK_MAX) — the window tells the player.
	static bool SetTracked(int id, bool on)
	{
		if (!s_Server)
			return false;
		if (on && !IsTracked(id) && s_Server.Tracked.Count() >= INT_QST_Const.TRACK_MAX)
			return false;
		s_Server.Tracked.RemoveItem(id);
		s_Server.Untracked.RemoveItem(id);
		if (on)
			s_Server.Tracked.Insert(id);
		else
			s_Server.Untracked.Insert(id);
		SavePrefs();
		Changed();
		return true;
	}

	protected static void AutoTrack(int id)
	{
		if (!s_Server || !s_Hello)
			return;
		INT_QST_QuestDef q = s_Catalog.Get(id);
		if (q && q.m_Instant)
			return;
		if (s_Server.Untracked.Find(id) >= 0 || s_Server.Tracked.Find(id) >= 0)
			return;
		if (s_Server.Tracked.Count() >= s_Hello.AutoTrackMax || s_Server.Tracked.Count() >= INT_QST_Const.TRACK_MAX)
			return;
		s_Server.Tracked.Insert(id);
		SavePrefs();
	}

	// Tracked ids that are no longer active are dropped.
	protected static void FixTracking()
	{
		if (!s_Server)
			return;
		bool dirty = false;
		for (int i = s_Server.Tracked.Count() - 1; i >= 0; i--)
		{
			if (!s_State.FindActive(s_Server.Tracked[i]))
			{
				s_Server.Tracked.RemoveOrdered(i);	// the order is the tracker order
				dirty = true;
			}
		}
		// a «hide on the map» choice lives while the quest is taken (taken again = shown again)
		for (int m = s_Server.MapOff.Count() - 1; m >= 0; m--)
		{
			if (!s_State.FindActive(s_Server.MapOff[m]))
			{
				s_Server.MapOff.Remove(m);
				dirty = true;
			}
		}
		if (dirty)
			SavePrefs();
	}

	// ---- helpers for the windows ----------------------------------------------------------------------------------
	// Status for the windows, the tracker and the markers. READY here means "can be turned in right now": a quest
	// whose counted targets are done but whose items are not in the inventory is still ACTIVE for the player.
	static int StatusOf(INT_QST_QuestDef q, out int waitSec)
	{
		int now = Now();
		int offset = Offset();
		int st = INT_QST_Rules.Status(q, s_State, s_Catalog, now, offset, waitSec);
		if (st == INT_QST_St.READY && INT_QST_Rules.HasInventoryTargets(q))
		{
			PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
			if (!INT_QST_Inv.AllCovered(p, q))
				st = INT_QST_St.ACTIVE;
		}
		return st;
	}

	// Target progress for display: counted types from the server state, inventory types from this inventory.
	static int TargetValue(INT_QST_QuestDef q, int ti)
	{
		INT_QST_TargetDef t = q.Targets[ti];
		if (INT_QST_TT.IsInventory(t.m_Type))
		{
			PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
			int c = INT_QST_Inv.Count(p, t);
			return Math.Min(c, t.m_Need);
		}
		INT_QST_Active a = s_State.FindActive(q.Id);
		if (!a || ti >= a.Progress.Count())
			return 0;
		int v = (int)a.Progress[ti];
		return Math.Min(v, t.m_Need);
	}

	// Ready to turn in right now (counted targets + inventory).
	static bool CanTurnIn(INT_QST_QuestDef q)
	{
		INT_QST_Active a = s_State.FindActive(q.Id);
		if (!a || !INT_QST_Rules.CountedDone(q, a))
			return false;
		if (INT_QST_Rules.IsExpired(q, a, Now()))
			return false;
		if (!INT_QST_Rules.HasInventoryTargets(q))
			return true;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		return INT_QST_Inv.AllCovered(p, q);
	}
}
