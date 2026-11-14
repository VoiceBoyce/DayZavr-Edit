// INT Quests (server) — config, givers and the quest catalog from files (spec.md §4.1, §5). Everything is read ONCE
// at start (and again only when an admin presses «Перечитать квесты»). The catalog is also prepared for the wire
// once: each branch and quest is a JSON text cut in pieces; clients pull the pieces only when their cached hash
// differs (owner rules 1-3: no work per player beyond that).

class INT_QST_SE_Store
{
	static ref INT_QST_Config s_Config;
	static ref INT_QST_GiversFile s_Givers;
	static ref INT_QST_Catalog s_Catalog;
	static ref map<string, int> s_BranchRec = new map<string, int>();	// record hash per branch / quest (patches)
	static ref map<int, int> s_QuestRec = new map<int, int>();
	static int s_MaxDescBytes;			// longest description read (JSON reader string limit check)
	static string s_MaxDescFile;
	static ref array<string> s_Problems = new array<string>();	// what the admin tab shows ("файл …: …")

	// wire pieces (flattened: piece i belongs to record s_PieceRec[i])
	static ref array<int> s_PieceRec = new array<int>();
	static ref array<int> s_PieceKind = new array<int>();		// 1 branch, 2 quest
	static ref array<int> s_PieceIdx = new array<int>();
	static ref array<int> s_PieceCnt = new array<int>();
	static ref array<string> s_PieceText = new array<string>();
	static int s_RecordCount;

	static bool IsAdmin(string uid)
	{
		if (!s_Config)
			return false;
		return s_Config.Admins.Find(uid) > -1;
	}

	static int PacketCount()
	{
		int n = s_PieceText.Count();
		return (n + INT_QST_Const.PIECES_PER_PACKET - 1) / INT_QST_Const.PIECES_PER_PACKET;
	}

	static void Problem(string text)
	{
		if (s_Problems.Count() < 500)
			s_Problems.Insert(text);
		INT_QST_Log.Warn(text);
	}

	// ---- everything ------------------------------------------------------------------------------------------
	static void LoadAll()
	{
		s_Problems.Clear();
		INT_QST_SE_Paths.MakeAll();
		LoadConfig();
		LoadGivers();
		LoadQuests();
		BuildWire();
	}

	// ---- main config (auto-created with defaults, owner rule 17) ---------------------------------------------
	static void LoadConfig()
	{
		INT_QST_Config c = new INT_QST_Config();
		string path = INT_QST_SE_Paths.ConfigFile();
		if (!FileExist(path))
		{
			INT_QST_SE_Json<INT_QST_Config>.Save(path, c);
			INT_QST_Log.Info("создан конфиг по умолчанию: " + path);
			INT_QST_SE_FileLog.Line("Создан конфиг по умолчанию");
		}
		else
		{
			string err;
			if (!INT_QST_SE_Json<INT_QST_Config>.Load(path, c, err))
			{
				string kept = INT_QST_SE_Files.KeepBroken(path);
				Problem("конфиг не читается (" + err + "), взяты значения по умолчанию, копия: " + kept);
				c = new INT_QST_Config();
			}
		}
		c.Normalize();
		s_Config = c;
		INT_QST_Log.s_Debug = c.DebugMode == 1;
		array<string> clean = new array<string>();
		foreach (string a : c.Admins)
		{
			string id = a + "";
			id.TrimInPlace();
			if (INT_QST_Util.IsSteam64(id))
				clean.Insert(id);
			else if (id != "")
				Problem("в Admins не Steam64: " + INT_QST_Log.Clip(id, 40));
		}
		c.Admins = clean;
	}

	// ---- givers ------------------------------------------------------------------------------------------------
	static void LoadGivers()
	{
		INT_QST_GiversFile g = new INT_QST_GiversFile();
		string path = INT_QST_SE_Paths.GiversFile();
		if (!FileExist(path))
		{
			INT_QST_SE_Json<INT_QST_GiversFile>.Save(path, g);
		}
		else
		{
			string err;
			if (!INT_QST_SE_Json<INT_QST_GiversFile>.Load(path, g, err))
			{
				string kept = INT_QST_SE_Files.KeepBroken(path);
				Problem("Givers.json не читается (" + err + "), квестовики не поставлены, копия: " + kept);
				g = new INT_QST_GiversFile();
			}
		}
		if (!g.Givers)
			g.Givers = new array<ref INT_QST_GiverDef>();
		map<string, bool> seen = new map<string, bool>();
		for (int i = g.Givers.Count() - 1; i >= 0; i--)
		{
			INT_QST_GiverDef d = g.Givers[i];
			if (!d)
			{
				g.Givers.RemoveOrdered(i);
				continue;
			}
			d.Normalize();
			if (d.Key == "" || d.Model == "")
			{
				Problem("квестовик без Key или Model пропущен");
				g.Givers.RemoveOrdered(i);
				continue;
			}
		}
		for (int j = 0; j < g.Givers.Count(); j++)
		{
			if (seen.Contains(g.Givers[j].Key))
			{
				Problem("повтор ключа квестовика " + g.Givers[j].Key + " — второй пропущен");
				g.Givers.RemoveOrdered(j);
				j--;
				continue;
			}
			seen.Set(g.Givers[j].Key, true);
		}
		s_Givers = g;
	}

	static bool SaveGivers()
	{
		return INT_QST_SE_Json<INT_QST_GiversFile>.Save(INT_QST_SE_Paths.GiversFile(), s_Givers);
	}

	static INT_QST_GiverDef FindGiver(string key)
	{
		foreach (INT_QST_GiverDef d : s_Givers.Givers)
		{
			if (d.Key == key)
				return d;
		}
		return null;
	}

	// ---- quests ------------------------------------------------------------------------------------------------
	static void LoadQuests()
	{
		s_Catalog = new INT_QST_Catalog();
		s_MaxDescBytes = 0;
		s_MaxDescFile = "";
		array<string> branches = new array<string>();
		ListDirs(INT_QST_SE_Paths.QuestsDir(), branches);
		if (branches.Count() == 0)
		{
			CreateExample();
			ListDirs(INT_QST_SE_Paths.QuestsDir(), branches);
		}
		array<ref INT_QST_BranchDef> metas = new array<ref INT_QST_BranchDef>();
		foreach (string b : branches)
		{
			if (b.IndexOf("_") == 0)
				continue;		// service folders (_deleted)
			if (!INT_QST_SE_Paths.IsSafeName(b))
			{
				Problem("папка ветки «" + INT_QST_Log.Clip(b, 40) + "»: только латиница, цифры и _ — пропущена");
				continue;
			}
			INT_QST_BranchDef meta = new INT_QST_BranchDef();
			string mpath = INT_QST_SE_Paths.BranchDir(b) + "/_branch.json";
			array<string> anyFiles = new array<string>();
			ListFiles(INT_QST_SE_Paths.BranchDir(b), "*.json", anyFiles);
			if (anyFiles.Count() == 0)
				continue;		// an emptied (deleted) branch folder
			if (FileExist(mpath))
			{
				string merr;
				if (!INT_QST_SE_Json<INT_QST_BranchDef>.Load(mpath, meta, merr))
				{
					Problem("ветка " + b + ": _branch.json не читается (" + merr + ")");
					meta = new INT_QST_BranchDef();
				}
			}
			meta.m_Key = b;
			if (meta.Title == "")
				meta.Title = b;
			meta.Title = INT_QST_Util.OneLine(meta.Title, INT_QST_Const.MAX_NAME_BYTES);
			metas.Insert(meta);
		}
		SortBranches(metas);
		foreach (INT_QST_BranchDef m : metas)
		{
			s_Catalog.m_Branches.Set(m.m_Key, m);
			s_Catalog.m_BranchOrder.Insert(m.m_Key);
			LoadBranch(m.m_Key);
		}
		INT_QST_Log.Info("квестов загружено: " + s_Catalog.m_List.Count().ToString() + ", веток: " + metas.Count().ToString() + ", проблем: " + s_Problems.Count().ToString());
		INT_QST_Log.Info("самое длинное описание: " + s_MaxDescBytes.ToString() + " байт (" + s_MaxDescFile + ")");
	}

	static void LoadBranch(string b)
	{
		string dir = INT_QST_SE_Paths.BranchDir(b);
		array<string> files = new array<string>();
		ListFiles(dir, "*.json", files);
		array<ref INT_QST_QuestDef> list = new array<ref INT_QST_QuestDef>();
		foreach (string f : files)
		{
			if (f.IndexOf("_branch") == 0)
				continue;
			string path = dir + "/" + f;
			string txt;
			if (!INT_QST_SE_Files.ReadText(path, txt))
			{
				Problem(b + "/" + f + ": не открывается");
				continue;
			}
			INT_QST_QuestDef q = new INT_QST_QuestDef();
			string err;
			if (!INT_QST_SE_Json<INT_QST_QuestDef>.Parse(txt, q, err))
			{
				Problem(b + "/" + f + ": ошибка JSON " + INT_QST_Log.Clip(err, 80));
				continue;
			}
			q.m_Branch = b;
			q.m_File = f;
			q.Normalize();
			int dl = q.m_Text.Length();
			if (dl > s_MaxDescBytes)
			{
				s_MaxDescBytes = dl;
				s_MaxDescFile = b + "/" + f;
			}
			if (q.m_TextCut)
				Problem(b + "/" + f + ": кусок описания длиннее 1000 байт обрезан движком — разбейте его на части");
			if (q.m_Trimmed)
				Problem(b + "/" + f + ": пустые записи или больше целей/наград/предметов/требований, чем можно — лишнее отброшено");
			if (q.Id <= 0)
			{
				Problem(b + "/" + f + ": нет Id");
				continue;
			}
			if (s_Catalog.Get(q.Id))
			{
				Problem(b + "/" + f + ": Id " + q.Id.ToString() + " уже занят — квест пропущен");
				continue;
			}
			list.Insert(q);
			s_Catalog.m_ById.Set(q.Id, q);
		}
		SortQuests(list);
		foreach (INT_QST_QuestDef x : list)
			s_Catalog.m_List.Insert(x);
	}

	static void ListDirs(string root, array<string> outDirs)
	{
		outDirs.Clear();
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile(root + "/*", name, attr, FindFileFlags.ALL);
		bool more = name != "";
		while (more)
		{
			if ((attr & FileAttr.DIRECTORY) != 0 && name != "." && name != "..")
				outDirs.Insert(name);
			more = FindNextFile(h, name, attr);
		}
		CloseFindFile(h);
		outDirs.Sort();
	}

	static void ListFiles(string dir, string pattern, array<string> outFiles)
	{
		outFiles.Clear();
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile(dir + "/" + pattern, name, attr, FindFileFlags.ALL);
		bool more = name != "";
		while (more)
		{
			if ((attr & FileAttr.DIRECTORY) == 0)
				outFiles.Insert(name);
			more = FindNextFile(h, name, attr);
		}
		CloseFindFile(h);
		outFiles.Sort();
	}

	// Insertion sorts (a few hundred entries, once at start).
	static void SortBranches(array<ref INT_QST_BranchDef> list)
	{
		for (int i = 1; i < list.Count(); i++)
		{
			INT_QST_BranchDef cur = list[i];
			int j = i - 1;
			while (j >= 0 && BranchAfter(list[j], cur))
			{
				list[j + 1] = list[j];
				j--;
			}
			list[j + 1] = cur;
		}
	}

	static bool BranchAfter(INT_QST_BranchDef a, INT_QST_BranchDef b)
	{
		if (a.Sort != b.Sort)
			return a.Sort > b.Sort;
		return a.m_Key > b.m_Key;
	}

	static void SortQuests(array<ref INT_QST_QuestDef> list)
	{
		for (int i = 1; i < list.Count(); i++)
		{
			INT_QST_QuestDef cur = list[i];
			int j = i - 1;
			while (j >= 0 && (list[j].Order > cur.Order || (list[j].Order == cur.Order && list[j].Id > cur.Id)))
			{
				list[j + 1] = list[j];
				j--;
			}
			list[j + 1] = cur;
		}
	}

	// First start with an empty Quests folder: one switched-off example branch for the admins (owner rule 17).
	static void CreateExample()
	{
		string b = "Example";
		INT_QST_SE_Paths.Make(INT_QST_SE_Paths.BranchDir(b));
		INT_QST_BranchDef meta = new INT_QST_BranchDef();
		meta.Title = "Пример ветки";
		meta.Sort = 10;
		INT_QST_SE_Json<INT_QST_BranchDef>.Save(INT_QST_SE_Paths.BranchDir(b) + "/_branch.json", meta);
		INT_QST_QuestDef q = new INT_QST_QuestDef();
		q.Id = 1;
		q.Order = 1;
		q.Enabled = 0;
		q.Name = "Пример: зачистка";
		q.Description.Insert("Пример задания. Включите его в админке или создайте свои квесты.");
		INT_QST_TargetDef t = new INT_QST_TargetDef();
		t.Kind = "KILL";
		t.Target = "ZombieBase";
		t.Count = 5;
		t.Text = "Убить заражённых";
		q.Targets.Insert(t);
		INT_QST_RewardDef r = new INT_QST_RewardDef();
		r.Kind = "ITEM";
		r.Item = "BandageDressing";
		r.Count = 2;
		q.Rewards.Insert(r);
		INT_QST_SE_Json<INT_QST_QuestDef>.Save(INT_QST_SE_Paths.BranchDir(b) + "/Example_01.json", q);
		INT_QST_Log.Info("создана пример-ветка Quests/Example (квест выключен)");
		INT_QST_SE_FileLog.Line("Создана пример-ветка Quests/Example (квест выключен)");
	}

	// ---- wire catalog ------------------------------------------------------------------------------------------
	static void BuildWire()
	{
		s_PieceRec.Clear();
		s_PieceKind.Clear();
		s_PieceIdx.Clear();
		s_PieceCnt.Clear();
		s_PieceText.Clear();
		s_RecordCount = 0;
		s_BranchRec.Clear();
		s_QuestRec.Clear();
		int hash = INT_QST_Hash.START;
		array<string> pieces = new array<string>();
		string txt;
		foreach (string key : s_Catalog.m_BranchOrder)
		{
			INT_QST_BranchDef bd = s_Catalog.Branch(key);
			if (!bd)
				continue;
			if (!BranchText(bd, txt))
				continue;
			int bh = INT_QST_Hash.Record(txt);
			s_BranchRec.Set(key, bh);
			hash = hash + bh;
			AddRecord(1, txt, pieces);
		}
		foreach (INT_QST_QuestDef q : s_Catalog.m_List)
		{
			if (!QuestText(q, txt))
			{
				Problem("квест " + q.Id.ToString() + " не собирается в JSON");
				continue;
			}
			int qh = INT_QST_Hash.Record(txt);
			s_QuestRec.Set(q.Id, qh);
			hash = hash + qh;
			AddRecord(2, txt, pieces);
		}
		s_Catalog.m_Hash = hash;
		INT_QST_Log.Dbg("каталог для сети: записей " + s_RecordCount.ToString() + ", кусков " + s_PieceText.Count().ToString() + ", пакетов " + PacketCount().ToString() + ", хэш " + hash.ToString());
	}

	// The wire record of a branch / quest (the same text for full downloads and patches).
	static bool BranchText(INT_QST_BranchDef bd, out string txt)
	{
		INT_QST_WireBranch wb = new INT_QST_WireBranch();
		wb.Key = bd.m_Key;
		wb.Title = bd.Title;
		wb.Color = bd.Color;
		wb.Sort = bd.Sort;
		return INT_QST_SE_Json<INT_QST_WireBranch>.ToText(wb, false, txt);
	}

	static bool QuestText(INT_QST_QuestDef q, out string txt)
	{
		INT_QST_WireQuest wq = new INT_QST_WireQuest();
		wq.Branch = q.m_Branch;
		wq.Q = q;
		return INT_QST_SE_Json<INT_QST_WireQuest>.ToText(wq, false, txt);
	}

	protected static void AddRecord(int kind, string txt, array<string> pieces)
	{
		INT_QST_Util.SplitBytes(txt, INT_QST_Const.PIECE_BYTES, pieces);
		int n = pieces.Count();
		for (int i = 0; i < n; i++)
		{
			s_PieceRec.Insert(s_RecordCount);
			s_PieceKind.Insert(kind);
			s_PieceIdx.Insert(i);
			s_PieceCnt.Insert(n);
			s_PieceText.Insert(pieces[i]);
		}
		s_RecordCount++;
	}

	// ---- players -----------------------------------------------------------------------------------------------
	static INT_QST_PlayerFile LoadPlayer(string uid)
	{
		INT_QST_PlayerFile st = new INT_QST_PlayerFile();
		string path = INT_QST_SE_Paths.PlayerFile(uid);
		string txt;
		if (INT_QST_SE_Files.ReadFast(path, txt))
		{
			string err;
			if (!INT_QST_SE_Json<INT_QST_PlayerFile>.Parse(txt, st, err))
			{
				// a broken main file: the .tmp of an interrupted save may still hold the last good state
				st = new INT_QST_PlayerFile();
				string tmpTxt;
				string terr;
				bool fromTmp = false;
				if (INT_QST_SE_Files.ReadText(path + ".tmp", tmpTxt))
					fromTmp = INT_QST_SE_Json<INT_QST_PlayerFile>.Parse(tmpTxt, st, terr);
				string kept = INT_QST_SE_Files.KeepBroken(path);
				if (fromTmp)
				{
					INT_QST_Log.Err("файл игрока " + uid + " не читается (" + err + "), взят .tmp, копия: " + kept);
					INT_QST_SE_FileLog.Line("ОШИБКА: файл игрока " + uid + " повреждён, восстановлен из .tmp, копия " + kept);
				}
				else
				{
					st = new INT_QST_PlayerFile();
					INT_QST_Log.Err("файл игрока " + uid + " не читается (" + err + "), начат заново, копия: " + kept);
					INT_QST_SE_FileLog.Line("ОШИБКА: файл игрока " + uid + " повреждён, начат заново, копия " + kept);
				}
			}
		}
		st.Normalize();
		return st;
	}

	static bool SavePlayer(string uid, INT_QST_PlayerFile st)
	{
		string txt;
		if (!INT_QST_SE_Json<INT_QST_PlayerFile>.ToText(st, false, txt))
			return false;
		bool ok = INT_QST_SE_Files.SaveFast(INT_QST_SE_Paths.PlayerFile(uid), txt);
		if (!ok)
			INT_QST_Log.Err("не записан файл игрока " + uid);
		return ok;
	}
}
