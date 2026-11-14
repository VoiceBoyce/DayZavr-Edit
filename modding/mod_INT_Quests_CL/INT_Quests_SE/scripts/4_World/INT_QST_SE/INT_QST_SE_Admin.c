// INT Quests (server) — admin operations (spec.md §6.2). Only Steam64 ids from the config reach this file (the core
// checks). Payloads arrive as bulk JSON (INT_QST_Bulk); answers go back the same way. Quest edits are written to
// their files at once and sent to every connected client as catalog PATCHES (one record each, the catalog hash is
// a sum of record hashes, so each client checks the patch fits its copy and re-downloads only on a mismatch).
// Reload and import change many records: they rebuild the catalog and send the usual "catalog changed".

// Ids.json: the highest quest id ever given on this server.
class INT_QST_SE_Ids
{
	int LastId;
}

class INT_QST_SE_Admin
{
	protected static ref map<string, ref INT_QST_BulkIn> s_In = new map<string, ref INT_QST_BulkIn>();
	protected static int s_MsgId = 1;
	protected static ref INT_QST_ABoard s_Board;
	protected static ref INT_QST_SE_Ids s_Ids;
	static int s_Ops;			// self-test

	// ---- transport ---------------------------------------------------------------------------------------------
	static void Handle(INT_QST_SE_Session s, PlayerBase p, ParamsReadContext ctx)
	{
		int op;
		if (!ctx.Read(op))
			return;
		INT_QST_BulkIn bin;
		if (!s_In.Find(s.m_Uid, bin))
		{
			bin = new INT_QST_BulkIn();
			s_In.Set(s.m_Uid, bin);
		}
		bool bad;
		if (!bin.Read(ctx, op, bad))
		{
			if (bad)
				INT_QST_Log.Warn("админ-пакет не прочитан от " + INT_QST_Log.Clip(s.m_Name, 32));
			return;
		}
		string text = bin.Text();
		if (text.Length() > INT_QST_A.MAX_PAYLOAD_BYTES)
		{
			Reply(s, op, INT_QST_Res.BAD_DATA, "");
			return;
		}
		s_Ops++;
		Dispatch(s, p, op, text);
	}

	static void Forget(string uid)
	{
		s_In.Remove(uid);
	}

	static void Reply(INT_QST_SE_Session s, int op, int code, string text)
	{
		if (!s || !s.m_Identity)
			return;
		array<string> pieces = new array<string>();
		INT_QST_Util.SplitLong(text, INT_QST_Const.PIECE_BYTES, pieces);
		int parts = INT_QST_Bulk.PartsOf(pieces);
		int msgId = s_MsgId;
		s_MsgId++;
		for (int part = 0; part < parts; part++)
		{
			ScriptRPC rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, INT_QST_S2C.ADMIN);
			rpc.Write(op);
			rpc.Write(code);
			INT_QST_Bulk.WritePart(rpc, msgId, part, parts, pieces);
			INT_QST_SE_Core.SendTo(rpc, s.m_Identity, "ADMIN");
		}
	}

	protected static string Who(INT_QST_SE_Session s)
	{
		return "админ " + INT_QST_Log.Clip(s.m_Name, 32) + " (" + s.m_Uid + ")";
	}

	protected static void Dispatch(INT_QST_SE_Session s, PlayerBase p, int op, string text)
	{
		switch (op)
		{
			case INT_QST_A.HELLO:
				Hello(s);
				return;
			case INT_QST_A.CONFIG:
				SaveConfig(s, text);
				return;
			case INT_QST_A.SAVE:
				Save(s, text);
				return;
			case INT_QST_A.BOARD_SAVE:
				SaveBoard(s, text);
				return;
			case INT_QST_A.GIVER_SAVE:
				SaveGiver(s, text);
				return;
			case INT_QST_A.GIVER_DELETE:
				DeleteGiver(s, text);
				return;
			case INT_QST_A.GIVER_HERE:
				GiverHere(s, p, text);
				return;
			case INT_QST_A.EXPORT:
				Export(s, text);
				return;
			case INT_QST_A.IMPORT_LIST:
				ImportList(s);
				return;
			case INT_QST_A.IMPORT:
				Import(s, text);
				return;
			case INT_QST_A.RELOAD:
				Reload(s);
				return;
		}
		Reply(s, op, INT_QST_Res.BAD_DATA, "");
	}

	// ---- hello -------------------------------------------------------------------------------------------------
	protected static void Hello(INT_QST_SE_Session s)
	{
		INT_QST_AHello h = new INT_QST_AHello();
		foreach (string pr : INT_QST_SE_Store.s_Problems)
			h.Problems.Insert(pr);
		INT_QST_ABoard board = Board();
		foreach (INT_QST_BoardNode bn : board.Nodes)
			h.Nodes.Insert(bn);
		foreach (INT_QST_QuestDef q : INT_QST_SE_Store.s_Catalog.m_List)
			h.Files.Insert(q.Id.ToString() + "|" + q.m_Branch + "/" + q.m_File);
		array<string> dirs = new array<string>();
		INT_QST_SE_Store.ListDirs(INT_QST_SE_Paths.ImportsDir(), dirs);
		foreach (string d : dirs)
			h.Imports.Insert(d);
		INT_QST_Config c = INT_QST_SE_Store.s_Config;
		// h.Config.ShowSigns = c.ShowSigns;		// SIGNS-OFF (owner 2026-10-31)
		h.Config.ShowLockedAtGiver = c.ShowLockedAtGiver;
		h.Config.CanCancel = c.CanCancel;
		h.Config.QuestSlots = c.QuestSlots;
		h.Config.AutoTrackMax = c.AutoTrackMax;
		h.Config.DebugMode = c.DebugMode;
		h.Config.GrenadeKills = c.GrenadeKills;
		string txt;
		INT_QST_SE_Json<INT_QST_AHello>.ToText(h, false, txt);
		Reply(s, INT_QST_A.HELLO, INT_QST_Res.OK, txt);
	}

	// ---- server switches -----------------------------------------------------------------------------------------
	protected static void SaveConfig(INT_QST_SE_Session s, string text)
	{
		INT_QST_AConfig a = new INT_QST_AConfig();
		string err;
		if (!INT_QST_SE_Json<INT_QST_AConfig>.Parse(text, a, err))
		{
			Reply(s, INT_QST_A.CONFIG, INT_QST_Res.BAD_DATA, "");
			return;
		}
		INT_QST_Config c = INT_QST_SE_Store.s_Config;
		ApplySwitches(c, a);
		INT_QST_Log.s_Debug = c.DebugMode == 1;
		// only the switches of this window change in the file: what the owner edited on disk while the server runs
		// (admins, distances, ...) stays (review 2026-10-22)
		string path = INT_QST_SE_Paths.ConfigFile();
		INT_QST_Config disk = new INT_QST_Config();
		string derr;
		bool haveDisk = FileExist(path) && INT_QST_SE_Json<INT_QST_Config>.Load(path, disk, derr);
		INT_QST_Config toWrite = c;
		if (haveDisk)
		{
			ApplySwitches(disk, a);
			toWrite = disk;
		}
		if (!INT_QST_SE_Json<INT_QST_Config>.Save(path, toWrite))
		{
			Reply(s, INT_QST_A.CONFIG, INT_QST_Res.STORAGE, "");
			return;
		}
		string sGr = c.GrenadeKills.ToString();
		// SIGNS-OFF (owner 2026-10-31): the line had «знаки N» first
		// INT_QST_SE_FileLog.Line(Who(s) + " изменил настройки: знаки " + c.ShowSigns.ToString() + ", закрытые у квестовика " + c.ShowLockedAtGiver.ToString() + ", отказ " + c.CanCancel.ToString() + ", слоты " + c.QuestSlots.ToString() + ", гранаты " + sGr);
		INT_QST_SE_FileLog.Line(Who(s) + " изменил настройки: закрытые у квестовика " + c.ShowLockedAtGiver.ToString() + ", отказ " + c.CanCancel.ToString() + ", слоты " + c.QuestSlots.ToString() + ", гранаты " + sGr);
		// everybody gets the new switches (HELLO carries them)
		foreach (string uid, INT_QST_SE_Session o : INT_QST_SE_Players.s_ByUid)
		{
			if (o.m_Identity && o.m_Hello)
				INT_QST_SE_Core.SendHello(o);
		}
		Reply(s, INT_QST_A.CONFIG, INT_QST_Res.OK, "");
	}

	protected static void ApplySwitches(INT_QST_Config c, INT_QST_AConfig a)
	{
		// c.ShowSigns = INT_QST_Util.ClampI(a.ShowSigns, 0, 1);		// SIGNS-OFF (owner 2026-10-31)
		c.ShowLockedAtGiver = INT_QST_Util.ClampI(a.ShowLockedAtGiver, 0, 1);
		c.CanCancel = INT_QST_Util.ClampI(a.CanCancel, 0, 1);
		c.QuestSlots = a.QuestSlots;
		c.AutoTrackMax = a.AutoTrackMax;
		c.DebugMode = INT_QST_Util.ClampI(a.DebugMode, 0, 1);
		c.GrenadeKills = INT_QST_Util.ClampI(a.GrenadeKills, 0, 1);
		c.Normalize();
	}

	// ---- quests and branches -------------------------------------------------------------------------------------
	// A branch key the loader keeps: a safe folder name that does not start with "_" (service folders like _deleted
	// are skipped at load, so such a branch would vanish after a restart).
	protected static bool BranchKeyOk(string key)
	{
		if (!INT_QST_SE_Paths.IsSafeName(key))
			return false;
		return key.IndexOf("_") != 0;
	}

	// The existing branch with the same name in another letter case (Windows folders ignore case: "main" next to a
	// live "Main" would overwrite its _branch.json).
	protected static string SameBranch(string key)
	{
		string low = INT_QST_Util.Lower(key);
		foreach (string k : INT_QST_SE_Store.s_Catalog.m_BranchOrder)
		{
			string kl = INT_QST_Util.Lower(k);
			if (kl == low)
				return k;
		}
		return key;
	}

	// A refused quest change: the client gets its id + the reason (and keeps the admin's copy).
	protected static void Refuse(INT_QST_ASaveResult res, int id, int code, string problem)
	{
		res.Failed.Insert(id);
		res.FailCodes.Insert(code);
		if (problem != "")
			res.Problems.Insert(problem);
	}

	// true when `from` reaches `target` through the Requires of the catalog (explicit stack + seen: the recursive
	// walk was exponential on diamond-shaped requirements)
	protected static bool Reaches(int from, int target)
	{
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		array<int> stack = new array<int>();
		map<int, bool> seen = new map<int, bool>();
		stack.Insert(from);
		int guard = 0;
		while (stack.Count() > 0 && guard < 100000)
		{
			guard++;
			int last = stack.Count() - 1;
			int cur = stack[last];
			stack.RemoveOrdered(last);
			if (cur == target)
				return true;
			if (seen.Contains(cur))
				continue;
			seen.Set(cur, true);
			INT_QST_QuestDef cq = cat.Get(cur);
			if (!cq)
				continue;
			foreach (int nx : cq.Requires)
				stack.Insert(nx);
		}
		return false;
	}

	protected static void Save(INT_QST_SE_Session s, string text)
	{
		INT_QST_ASave a = new INT_QST_ASave();
		string err;
		if (!INT_QST_SE_Json<INT_QST_ASave>.Parse(text, a, err) || !a.Changes)
		{
			Reply(s, INT_QST_A.SAVE, INT_QST_Res.BAD_DATA, "");
			return;
		}
		INT_QST_ASaveResult res = new INT_QST_ASaveResult();
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		// new quests first get their real ids (Requires inside the batch may point at them)
		int nextId = FirstFreeId();
		map<int, int> idMap = new map<int, int>();
		foreach (INT_QST_AChange c0 : a.Changes)
		{
			if (c0 && c0.Kind == 1 && c0.Delete == 0 && c0.Id < 0 && !idMap.Contains(c0.Id))
			{
				idMap.Set(c0.Id, nextId);
				res.TempIds.Insert(c0.Id);
				res.NewIds.Insert(nextId);
				nextId++;
			}
		}
		if (res.NewIds.Count() > 0)
			UseId(nextId - 1);
		// branches, then quests
		foreach (INT_QST_AChange cb : a.Changes)
		{
			if (cb && cb.Kind == 2)
				SaveBranch(s, cb, res);
		}
		foreach (INT_QST_AChange cq : a.Changes)
		{
			if (cq && cq.Kind == 1)
				SaveQuest(s, cq, idMap, res);
		}
		cat.Resort();
		int incremental = cat.m_Hash;
		INT_QST_SE_Store.BuildWire();
		if (cat.m_Hash != incremental)
		{
			INT_QST_Log.Warn("хэш каталога после правок разошёлся: полная пересылка");
			INT_QST_SE_Core.BroadcastCatalogChanged();
		}
		INT_QST_SE_Players.ReindexAll();
		string txt;
		INT_QST_SE_Json<INT_QST_ASaveResult>.ToText(res, false, txt);
		Reply(s, INT_QST_A.SAVE, INT_QST_Res.OK, txt);
	}

	// Ids are never given twice: Ids.json keeps the highest id ever used (a deleted quest keeps its id, so a new quest
	// does not inherit the progress / «done» marks players still have for the old one).
	static string IdsFile()
	{
		return INT_QST_SE_Paths.Root() + "/Ids.json";
	}

	protected static INT_QST_SE_Ids Ids()
	{
		if (s_Ids)
			return s_Ids;
		s_Ids = new INT_QST_SE_Ids();
		string path = IdsFile();
		if (FileExist(path))
		{
			string err;
			if (!INT_QST_SE_Json<INT_QST_SE_Ids>.Load(path, s_Ids, err))
				s_Ids = new INT_QST_SE_Ids();
		}
		return s_Ids;
	}

	protected static int FirstFreeId()
	{
		INT_QST_SE_Ids ids = Ids();
		int m = MaxId();
		if (ids.LastId > m)
			m = ids.LastId;
		return m + 1;
	}

	// remember an id as used (after a create, an import or a delete)
	protected static void UseId(int id)
	{
		INT_QST_SE_Ids ids = Ids();
		if (id <= ids.LastId)
			return;
		ids.LastId = id;
		INT_QST_SE_Json<INT_QST_SE_Ids>.Save(IdsFile(), ids);
	}

	protected static int MaxId()
	{
		int m = 0;
		foreach (INT_QST_QuestDef q : INT_QST_SE_Store.s_Catalog.m_List)
		{
			if (q.Id > m)
				m = q.Id;
		}
		return m;
	}

	protected static void SaveBranch(INT_QST_SE_Session s, INT_QST_AChange c, INT_QST_ASaveResult res)
	{
		string key = SameBranch(c.Key);
		if (!BranchKeyOk(key))
		{
			res.Problems.Insert("ветка " + INT_QST_Log.Clip(key, 40) + ": имя папки — латиница, цифры, _ (не с _ в начале)");
			return;
		}
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		string dir = INT_QST_SE_Paths.BranchDir(key);
		string meta = dir + "/_branch.json";
		int oldRec = 0;
		INT_QST_SE_Store.s_BranchRec.Find(key, oldRec);
		if (c.Delete == 1)
		{
			foreach (INT_QST_QuestDef q : cat.m_List)
			{
				if (q.m_Branch == key)
				{
					res.Problems.Insert("ветка " + key + " не пуста — не удалена");
					return;
				}
			}
			if (FileExist(meta))
				DeleteFile(meta);
			cat.m_Branches.Remove(key);
			cat.m_BranchOrder.RemoveItem(key);
			Patch(1, 1, key, "", oldRec, 0);
			INT_QST_SE_FileLog.Line(Who(s) + " удалил ветку " + key);
			return;
		}
		INT_QST_BranchDef bd = cat.Branch(key);
		if (!bd)
		{
			bd = new INT_QST_BranchDef();
			bd.m_Key = key;
			cat.m_Branches.Set(key, bd);
			cat.m_BranchOrder.Insert(key);
		}
		if (c.Branch)
		{
			bd.Color = c.Branch.Color;
			bd.Sort = c.Branch.Sort;
		}
		string title = INT_QST_Util.OneLine(c.BranchTitle, INT_QST_Const.MAX_NAME_BYTES);
		if (title == "")
			title = key;
		bd.Title = title;
		INT_QST_SE_Paths.Make(dir);
		if (!INT_QST_SE_Json<INT_QST_BranchDef>.Save(meta, bd))
		{
			res.Problems.Insert("ветка " + key + ": _branch.json не записан");
			return;
		}
		string txt;
		INT_QST_SE_Store.BranchText(bd, txt);
		int bRec = INT_QST_Hash.Record(txt);
		Patch(1, 0, key, txt, oldRec, bRec);
		res.Saved++;
		INT_QST_SE_FileLog.Line(Who(s) + " сохранил ветку " + key + " «" + title + "»");
	}

	protected static void SaveQuest(INT_QST_SE_Session s, INT_QST_AChange c, map<int, int> idMap, INT_QST_ASaveResult res)
	{
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		int id = c.Id;
		if (id < 0)
		{
			int mapped;
			if (!idMap.Find(c.Id, mapped))
				return;
			id = mapped;
		}
		INT_QST_QuestDef old = cat.Get(id);
		int oldRec = 0;
		int haveRec;
		if (INT_QST_SE_Store.s_QuestRec.Find(id, haveRec))
			oldRec = haveRec;
		if (c.Delete == 1)
		{
			if (!old)
				return;
			string oldPath = INT_QST_SE_Paths.BranchDir(old.m_Branch) + "/" + old.m_File;
			Trash(oldPath, old.m_Branch);
			UseId(id);
			string oldName = old.Name;
			cat.RemoveQuest(id);
			string delKey = id.ToString();
			Patch(2, 1, delKey, "", oldRec, 0);
			res.Saved++;
			INT_QST_SE_FileLog.Line(Who(s) + " удалил квест [" + id.ToString() + "] " + oldName);
			return;
		}
		string idText = id.ToString();
		// an existing quest edited from a stale copy: refuse instead of undoing the other admin's change, and never
		// bring back a quest that was deleted meanwhile (it would skip Ids.json)
		if (c.Id > 0 && !old)
		{
			Refuse(res, c.Id, INT_QST_Res.NOT_FOUND, "квест " + idText + " уже удалён — не восстановлен");
			return;
		}
		if (c.BaseRec != 0 && old && oldRec != c.BaseRec)
		{
			Refuse(res, c.Id, INT_QST_Res.CONFLICT, "квест " + idText + " изменил другой админ — правка не записана");
			return;
		}
		INT_QST_QuestDef q = c.Quest;
		string branch = SameBranch(c.Key);
		if (!q || !BranchKeyOk(branch))
		{
			Refuse(res, c.Id, INT_QST_Res.BAD_DATA, "квест " + idText + ": нет данных или плохое имя ветки");
			return;
		}
		q.Id = id;
		// Requires: temporary ids -> real ones, unknown ids dropped
		array<int> req = new array<int>();
		foreach (int r : q.Requires)
		{
			int rr = r;
			if (r < 0)
			{
				int realId;
				if (!idMap.Find(r, realId))
					continue;
				rr = realId;
			}
			if (rr == id)
				continue;
			// a requirement that already needs this quest would make a loop (nobody could ever take either)
			if (Reaches(rr, id))
			{
				string rrText = rr.ToString();
				res.Problems.Insert("квест " + idText + ": требование " + rrText + " убрано — получился бы круг");
				continue;
			}
			if (req.Find(rr) < 0)
				req.Insert(rr);
		}
		q.Requires = req;
		q.m_Branch = branch;
		q.Normalize();
		if (q.Name == "")
		{
			Refuse(res, c.Id, INT_QST_Res.BAD_DATA, "квест " + idText + ": нет названия — не сохранён");
			return;
		}
		// the branch must exist (folder + meta)
		if (!cat.Branch(branch))
		{
			INT_QST_AChange nb = new INT_QST_AChange();
			nb.Kind = 2;
			nb.Key = branch;
			nb.BranchTitle = branch;
			SaveBranch(s, nb, res);
		}
		// file: keep the old one in the same branch, else a new <Branch>_<NN>.json
		string file = "";
		if (old && old.m_Branch == branch && old.m_File != "")
			file = old.m_File;
		if (file == "")
			file = NewFileName(branch, q.Order);
		string path = INT_QST_SE_Paths.BranchDir(branch) + "/" + file;
		if (!INT_QST_SE_Json<INT_QST_QuestDef>.Save(path, q))
		{
			Refuse(res, c.Id, INT_QST_Res.STORAGE, "квест " + idText + ": файл не записан");
			return;
		}
		if (old && (old.m_Branch != branch || old.m_File != file))
			Trash(INT_QST_SE_Paths.BranchDir(old.m_Branch) + "/" + old.m_File, old.m_Branch);
		q.m_File = file;
		cat.RemoveQuest(id);
		cat.Add(q);
		string txt;
		INT_QST_SE_Store.QuestText(q, txt);
		string idKey = id.ToString();
		int newRec = INT_QST_Hash.Record(txt);
		Patch(2, 0, idKey, txt, oldRec, newRec);
		res.Saved++;
		string what = "изменил";
		if (!old)
			what = "создал";
		INT_QST_SE_FileLog.Line(Who(s) + " " + what + " квест [" + id.ToString() + "] " + q.Name + " (" + branch + "/" + file + ")");
	}

	// "<Branch>_<NN>.json" with the first free number from `order`.
	protected static string NewFileName(string branch, int order)
	{
		int n = order;
		if (n < 1)
			n = 1;
		string dir = INT_QST_SE_Paths.BranchDir(branch);
		for (int guard = 0; guard < 2000; guard++)
		{
			string num = n.ToString();
			if (n < 10)
				num = "0" + num;
			string f = branch + "_" + num + ".json";
			if (!FileExist(dir + "/" + f))
				return f;
			n++;
		}
		return branch + "_" + INT_QST_Util.FileStamp() + ".json";
	}

	// Deleted / moved quest files go to Quests/_deleted (never lost by an admin click).
	protected static void Trash(string path, string branch)
	{
		if (!FileExist(path))
			return;
		string dir = INT_QST_SE_Paths.QuestsDir() + "/_deleted";
		INT_QST_SE_Paths.Make(dir);
		array<string> parts = new array<string>();
		path.Split("/", parts);
		string name = parts[parts.Count() - 1];
		string target = dir + "/" + INT_QST_Util.FileStamp() + "_" + branch + "_" + name;
		CopyFile(path, target);
		DeleteFile(path);
	}

	// Moves the catalog hash by one record and tells every client.
	protected static void Patch(int kind, int del, string key, string text, int oldRec, int newRec)
	{
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		int oldTotal = cat.m_Hash;
		int newTotal = oldTotal - oldRec + newRec;
		cat.m_Hash = newTotal;
		if (kind == 1)
		{
			if (del == 1)
				INT_QST_SE_Store.s_BranchRec.Remove(key);
			else
				INT_QST_SE_Store.s_BranchRec.Set(key, newRec);
		}
		else
		{
			int id = key.ToInt();
			if (del == 1)
				INT_QST_SE_Store.s_QuestRec.Remove(id);
			else
				INT_QST_SE_Store.s_QuestRec.Set(id, newRec);
		}
		INT_QST_SE_Core.BroadcastPatch(oldTotal, newTotal, kind, del, key, text);
	}

	// ---- board layout --------------------------------------------------------------------------------------------
	static string BoardFile()
	{
		return INT_QST_SE_Paths.Root() + "/Board.json";
	}

	static INT_QST_ABoard Board()
	{
		if (s_Board)
			return s_Board;
		s_Board = new INT_QST_ABoard();
		string path = BoardFile();
		if (FileExist(path))
		{
			string err;
			if (!INT_QST_SE_Json<INT_QST_ABoard>.Load(path, s_Board, err))
				s_Board = new INT_QST_ABoard();
		}
		if (!s_Board.Nodes)
			s_Board.Nodes = new array<ref INT_QST_BoardNode>();
		return s_Board;
	}

	// The board is one for every admin (owner): a save brings only the cards that admin moved (or that the server
	// had no place for yet) and they are merged by id — two admins at once do not undo each other's cards.
	protected static void SaveBoard(INT_QST_SE_Session s, string text)
	{
		INT_QST_ABoard b = new INT_QST_ABoard();
		string err;
		if (!INT_QST_SE_Json<INT_QST_ABoard>.Parse(text, b, err) || !b.Nodes)
		{
			Reply(s, INT_QST_A.BOARD_SAVE, INT_QST_Res.BAD_DATA, "");
			return;
		}
		INT_QST_ABoard cur = Board();
		map<int, int> at = new map<int, int>();
		for (int i = 0; i < cur.Nodes.Count(); i++)
		{
			INT_QST_BoardNode cn = cur.Nodes[i];
			at.Set(cn.Id, i);
		}
		foreach (INT_QST_BoardNode bn : b.Nodes)
		{
			if (!bn || bn.Id <= 0)
				continue;
			bn.X = Math.Clamp(bn.X, -1000000, 1000000);
			bn.Y = Math.Clamp(bn.Y, -1000000, 1000000);
			int idx;
			if (at.Find(bn.Id, idx))
			{
				// an automatic place never overwrites a place the server has (another admin's hand placement)
				if (bn.Auto == 1)
					continue;
				INT_QST_BoardNode old = cur.Nodes[idx];
				old.X = bn.X;
				old.Y = bn.Y;
				continue;
			}
			bn.Auto = 0;
			cur.Nodes.Insert(bn);
			at.Set(bn.Id, cur.Nodes.Count() - 1);
		}
		// places of quests that are gone are dropped
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		if (cat)
		{
			for (int r = cur.Nodes.Count() - 1; r >= 0; r--)
			{
				INT_QST_BoardNode rn = cur.Nodes[r];
				if (!cat.Get(rn.Id))
					cur.Nodes.RemoveOrdered(r);
			}
		}
		if (!INT_QST_SE_Json<INT_QST_ABoard>.Save(BoardFile(), cur))
		{
			Reply(s, INT_QST_A.BOARD_SAVE, INT_QST_Res.STORAGE, "");
			return;
		}
		Reply(s, INT_QST_A.BOARD_SAVE, INT_QST_Res.OK, "");
	}

	// ---- givers --------------------------------------------------------------------------------------------------
	protected static void SaveGiver(INT_QST_SE_Session s, string text)
	{
		INT_QST_GiverDef d = new INT_QST_GiverDef();
		string err;
		if (!INT_QST_SE_Json<INT_QST_GiverDef>.Parse(text, d, err))
		{
			Reply(s, INT_QST_A.GIVER_SAVE, INT_QST_Res.BAD_DATA, "");
			return;
		}
		d.Normalize();
		if (d.Key == "" || d.Model == "" || !INT_QST_SE_Paths.IsSafeName(d.Key))
		{
			Reply(s, INT_QST_A.GIVER_SAVE, INT_QST_Res.BAD_DATA, "");
			return;
		}
		array<ref INT_QST_GiverDef> list = INT_QST_SE_Store.s_Givers.Givers;
		bool replaced = false;
		int at = -1;
		INT_QST_GiverDef prev = null;
		for (int i = 0; i < list.Count(); i++)
		{
			if (list[i].Key == d.Key)
			{
				prev = list[i];
				at = i;
				// an existing giver keeps his place: only «Поставить здесь» (GIVER_HERE) moves him (the window sends
				// the place rounded to 0.01, every save would rewrite it)
				d.Position = prev.Position;
				d.Orientation = prev.Orientation;
				d.Normalize();
				list.Set(i, d);
				replaced = true;
				break;
			}
		}
		if (!replaced)
			list.Insert(d);
		if (!INT_QST_SE_Store.SaveGivers())
		{
			// the list in memory goes back to what the file still has
			if (replaced)
				list.Set(at, prev);
			else
				list.RemoveItem(d);
			Reply(s, INT_QST_A.GIVER_SAVE, INT_QST_Res.STORAGE, "");
			return;
		}
		if (d.Disabled == 1)
			INT_QST_SE_Givers.Despawn(d.Key);
		else
			INT_QST_SE_Givers.Spawn(d);
		INT_QST_SE_Core.BroadcastGivers();
		string what = "изменил";
		if (!replaced)
			what = "поставил";
		INT_QST_SE_FileLog.Line(Who(s) + " " + what + " квестовика " + d.Key + " «" + d.Title + "» " + d.Model + " в " + d.m_Pos.ToString());
		Reply(s, INT_QST_A.GIVER_SAVE, INT_QST_Res.OK, d.Key);
	}

	protected static void DeleteGiver(INT_QST_SE_Session s, string text)
	{
		INT_QST_AKey k = new INT_QST_AKey();
		string err;
		if (!INT_QST_SE_Json<INT_QST_AKey>.Parse(text, k, err))
		{
			Reply(s, INT_QST_A.GIVER_DELETE, INT_QST_Res.BAD_DATA, "");
			return;
		}
		array<ref INT_QST_GiverDef> list = INT_QST_SE_Store.s_Givers.Givers;
		array<ref INT_QST_GiverDef> removed = new array<ref INT_QST_GiverDef>();
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			if (list[i].Key == k.Key)
			{
				removed.Insert(list[i]);
				list.RemoveOrdered(i);
			}
		}
		if (!INT_QST_SE_Store.SaveGivers())
		{
			foreach (INT_QST_GiverDef back : removed)
				list.Insert(back);
			Reply(s, INT_QST_A.GIVER_DELETE, INT_QST_Res.STORAGE, "");
			return;
		}
		INT_QST_SE_Givers.Despawn(k.Key);
		INT_QST_SE_Core.BroadcastGivers();
		INT_QST_SE_FileLog.Line(Who(s) + " убрал квестовика " + k.Key);
		Reply(s, INT_QST_A.GIVER_DELETE, INT_QST_Res.OK, k.Key);
	}

	// The giver stands where the admin stands, facing the admin's way.
	protected static void GiverHere(INT_QST_SE_Session s, PlayerBase p, string text)
	{
		INT_QST_AKey k = new INT_QST_AKey();
		string err;
		INT_QST_GiverDef d = null;
		if (INT_QST_SE_Json<INT_QST_AKey>.Parse(text, k, err))
			d = INT_QST_SE_Store.FindGiver(k.Key);
		if (!d || !p)
		{
			Reply(s, INT_QST_A.GIVER_HERE, INT_QST_Res.NOT_FOUND, "");
			return;
		}
		vector pos = p.GetPosition();
		vector ori = p.GetOrientation();
		string oldPos = d.Position;
		string oldOri = d.Orientation;
		d.Position = INT_QST_Util.VecText(pos);
		d.Orientation = INT_QST_Util.VecText(ori);
		d.Normalize();
		if (!INT_QST_SE_Store.SaveGivers())
		{
			d.Position = oldPos;
			d.Orientation = oldOri;
			d.Normalize();
			Reply(s, INT_QST_A.GIVER_HERE, INT_QST_Res.STORAGE, "");
			return;
		}
		INT_QST_SE_Givers.Spawn(d);
		INT_QST_SE_Core.BroadcastGivers();
		INT_QST_SE_FileLog.Line(Who(s) + " переставил квестовика " + d.Key + " в " + d.Position);
		Reply(s, INT_QST_A.GIVER_HERE, INT_QST_Res.OK, d.Key);
	}

	// ---- export / import -----------------------------------------------------------------------------------------
	protected static void Export(INT_QST_SE_Session s, string text)
	{
		INT_QST_AExport a = new INT_QST_AExport();
		string err;
		if (!INT_QST_SE_Json<INT_QST_AExport>.Parse(text, a, err) || !a.Ids || a.Ids.Count() == 0)
		{
			Reply(s, INT_QST_A.EXPORT, INT_QST_Res.BAD_DATA, "");
			return;
		}
		// letters, digits and _ only: the import accepts exactly such folder names
		string name = INT_QST_Util.FileStamp();
		name = INT_QST_Util.ReplaceByte(name, "-", "");
		if (a.Name != "" && INT_QST_SE_Paths.IsSafeName(a.Name))
			name = name + "_" + a.Name;
		string root = INT_QST_SE_Paths.ExportsDir() + "/" + name;
		INT_QST_SE_Paths.Make(root);
		map<string, bool> branches = new map<string, bool>();
		int n = 0;
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		foreach (int id : a.Ids)
		{
			INT_QST_QuestDef q = cat.Get(id);
			if (!q)
				continue;
			string dir = root + "/" + q.m_Branch;
			if (!branches.Contains(q.m_Branch))
			{
				branches.Set(q.m_Branch, true);
				INT_QST_SE_Paths.Make(dir);
				INT_QST_BranchDef bd = cat.Branch(q.m_Branch);
				if (bd)
					INT_QST_SE_Json<INT_QST_BranchDef>.Save(dir + "/_branch.json", bd);
			}
			string file = q.m_File;
			if (file == "")
				file = q.m_Branch + "_" + q.Order.ToString() + ".json";
			if (INT_QST_SE_Json<INT_QST_QuestDef>.Save(dir + "/" + file, q))
				n++;
		}
		INT_QST_SE_FileLog.Line(Who(s) + " выгрузил квестов: " + n.ToString() + " в Exports/" + name);
		INT_QST_AText t = new INT_QST_AText();
		t.Text = "Exports/" + name + " · " + n.ToString();
		string txt;
		INT_QST_SE_Json<INT_QST_AText>.ToText(t, false, txt);
		Reply(s, INT_QST_A.EXPORT, INT_QST_Res.OK, txt);
	}

	protected static void ImportList(INT_QST_SE_Session s)
	{
		INT_QST_AImportList l = new INT_QST_AImportList();
		array<string> dirs = new array<string>();
		INT_QST_SE_Store.ListDirs(INT_QST_SE_Paths.ImportsDir(), dirs);
		foreach (string d : dirs)
		{
			array<string> br = new array<string>();
			INT_QST_SE_Store.ListDirs(INT_QST_SE_Paths.ImportsDir() + "/" + d, br);
			int quests = 0;
			foreach (string b : br)
			{
				array<string> files = new array<string>();
				INT_QST_SE_Store.ListFiles(INT_QST_SE_Paths.ImportsDir() + "/" + d + "/" + b, "*.json", files);
				foreach (string f : files)
				{
					if (f.IndexOf("_branch") != 0)
						quests++;
				}
			}
			l.Folders.Insert(d);
			l.Info.Insert(br.Count().ToString() + "|" + quests.ToString());
		}
		string txt;
		INT_QST_SE_Json<INT_QST_AImportList>.ToText(l, false, txt);
		Reply(s, INT_QST_A.IMPORT_LIST, INT_QST_Res.OK, txt);
	}

	// Copies Imports/<Folder>/<Branch>/* into Quests/<Branch>/. Mode 0: a quest whose Id is taken gets a new Id
	// (Requires inside the import follow it); Mode 1: it replaces the quest with that Id.
	protected static void Import(INT_QST_SE_Session s, string text)
	{
		INT_QST_AImport a = new INT_QST_AImport();
		string err;
		if (!INT_QST_SE_Json<INT_QST_AImport>.Parse(text, a, err) || !INT_QST_SE_Paths.IsSafeName(a.Folder))
		{
			Reply(s, INT_QST_A.IMPORT, INT_QST_Res.BAD_DATA, "");
			return;
		}
		string src = INT_QST_SE_Paths.ImportsDir() + "/" + a.Folder;
		array<string> branches = new array<string>();
		INT_QST_SE_Store.ListDirs(src, branches);
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		map<int, int> remap = new map<int, int>();
		array<ref INT_QST_QuestDef> incoming = new array<ref INT_QST_QuestDef>();
		array<string> inBranch = new array<string>();
		array<string> inFile = new array<string>();
		foreach (string b0 : branches)
		{
			// service folders (_deleted, ...) are never imported
			if (!BranchKeyOk(b0))
				continue;
			string b = SameBranch(b0);
			string bdir = src + "/" + b0;
			INT_QST_SE_Paths.Make(INT_QST_SE_Paths.BranchDir(b));
			if (FileExist(bdir + "/_branch.json") && !FileExist(INT_QST_SE_Paths.BranchDir(b) + "/_branch.json"))
				CopyFile(bdir + "/_branch.json", INT_QST_SE_Paths.BranchDir(b) + "/_branch.json");
			array<string> files = new array<string>();
			INT_QST_SE_Store.ListFiles(bdir, "*.json", files);
			foreach (string f : files)
			{
				if (f.IndexOf("_branch") == 0)
					continue;
				string raw;
				if (!INT_QST_SE_Files.ReadText(bdir + "/" + f, raw))
					continue;
				INT_QST_QuestDef q = new INT_QST_QuestDef();
				if (!INT_QST_SE_Json<INT_QST_QuestDef>.Parse(raw, q, err) || q.Id <= 0)
					continue;
				incoming.Insert(q);
				inBranch.Insert(b);
				inFile.Insert(f);
			}
		}
		// second pass: who gets a new id. Mode 0: an id taken in the catalog, repeated inside this import, or ever used
		// on this server (a deleted quest's id would inherit old player progress). Mode 1 replaces by id, but a repeat
		// inside the import still gets a new one (two quests never share an id).
		int lastUsed = Ids().LastId;
		int nextId = FirstFreeId();
		foreach (INT_QST_QuestDef mq : incoming)
		{
			if (mq.Id >= nextId)
				nextId = mq.Id + 1;
		}
		map<int, bool> taken = new map<int, bool>();
		foreach (INT_QST_QuestDef iq0 : incoming)
		{
			int oldId = iq0.Id;
			bool change = taken.Contains(oldId);
			bool inCatalog = cat.Get(oldId) != null;
			if (a.Mode == 0 && (inCatalog || oldId <= lastUsed))
				change = true;
			if (change)
			{
				if (!remap.Contains(oldId) && !taken.Contains(oldId))
					remap.Set(oldId, nextId);
				iq0.Id = nextId;
				nextId++;
			}
			taken.Set(iq0.Id, true);
		}
		int topId = 0;
		foreach (INT_QST_QuestDef tq : incoming)
		{
			if (tq.Id > topId)
				topId = tq.Id;
		}
		UseId(topId);
		int written = 0;
		for (int i = 0; i < incoming.Count(); i++)
		{
			INT_QST_QuestDef iq = incoming[i];
			array<int> req = new array<int>();
			foreach (int r : iq.Requires)
			{
				int rr = r;
				int mappedR;
				if (remap.Find(r, mappedR))
					rr = mappedR;
				req.Insert(rr);
			}
			iq.Requires = req;
			string ib = inBranch[i];
			string fileName = inFile[i];
			INT_QST_QuestDef same = cat.Get(iq.Id);
			if (same && a.Mode == 1)
				Trash(INT_QST_SE_Paths.BranchDir(same.m_Branch) + "/" + same.m_File, same.m_Branch);
			if (FileExist(INT_QST_SE_Paths.BranchDir(ib) + "/" + fileName))
				fileName = NewFileName(ib, iq.Order);
			if (INT_QST_SE_Json<INT_QST_QuestDef>.Save(INT_QST_SE_Paths.BranchDir(ib) + "/" + fileName, iq))
				written++;
		}
		ReloadCatalog();
		string summary = "Imports/" + a.Folder + ": " + written.ToString() + " / " + incoming.Count().ToString() + ", новых Id " + remap.Count().ToString();
		INT_QST_SE_FileLog.Line(Who(s) + " загрузил " + summary);
		INT_QST_AText t = new INT_QST_AText();
		t.Text = summary;
		string txt;
		INT_QST_SE_Json<INT_QST_AText>.ToText(t, false, txt);
		Reply(s, INT_QST_A.IMPORT, INT_QST_Res.OK, txt);
	}

	// ---- reload ------------------------------------------------------------------------------------------------
	protected static void ReloadCatalog()
	{
		INT_QST_SE_Store.s_Problems.Clear();
		INT_QST_SE_Store.LoadQuests();
		INT_QST_SE_Store.BuildWire();
		INT_QST_SE_Players.ReindexAll();
		INT_QST_SE_Core.BroadcastCatalogChanged();
	}

	// «Перечитать квесты»: the config too (admins added to the file while the server runs, 2026-10-26 — before
	// this they were seen only after a restart), quests, givers; every online player gets a new HELLO (admin flag).
	protected static void Reload(INT_QST_SE_Session s)
	{
		INT_QST_SE_Givers.DespawnAll();
		INT_QST_SE_Store.s_Problems.Clear();
		INT_QST_SE_Store.LoadConfig();
		INT_QST_SE_Store.LoadGivers();
		INT_QST_SE_Store.LoadQuests();
		INT_QST_SE_Store.BuildWire();
		INT_QST_SE_Givers.SpawnAll();
		INT_QST_SE_Players.ReindexAll();
		INT_QST_SE_Core.BroadcastCatalogChanged();
		INT_QST_SE_Core.BroadcastGivers();
		foreach (string ouid, INT_QST_SE_Session o : INT_QST_SE_Players.s_ByUid)
		{
			if (o.m_Identity && o.m_Hello)
				INT_QST_SE_Core.SendHello(o);
		}
		s_Board = null;
		INT_QST_Catalog cat = INT_QST_SE_Store.s_Catalog;
		string summary = "квестов " + cat.m_List.Count().ToString() + ", веток " + cat.m_BranchOrder.Count().ToString() + ", квестовиков " + INT_QST_GiverReg.s_Slots.Count().ToString() + ", проблем " + INT_QST_SE_Store.s_Problems.Count().ToString();
		INT_QST_SE_FileLog.Line(Who(s) + " перечитал квесты: " + summary);
		INT_QST_AText t = new INT_QST_AText();
		t.Text = summary;
		string txt;
		INT_QST_SE_Json<INT_QST_AText>.ToText(t, false, txt);
		Reply(s, INT_QST_A.RELOAD, INT_QST_Res.OK, txt);
	}
}
