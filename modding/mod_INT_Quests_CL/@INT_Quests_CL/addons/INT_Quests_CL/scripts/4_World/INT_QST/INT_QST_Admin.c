// INT Quests (client) — admin requests and answers (spec.md §6.2). The admin windows call the typed helpers below;
// answers arrive as bulk JSON and are handed out through s_OnReply(int op, int code, string text). The server checks
// the admin list itself — a non-admin gets NOT_ADMIN, so nothing here is a security boundary.

class INT_QST_Admin
{
	protected static ref INT_QST_BulkIn s_In = new INT_QST_BulkIn();
	protected static int s_MsgId = 1;
	static ref INT_QST_AHello s_Hello;		// last admin hello (problems, board layout, files, config)
	static int s_HelloSeq;				// +1 per admin hello received: a window remembers the value when it
							// asks, a larger value later = the hello of THIS request arrived
	static ref ScriptInvoker s_OnReply = new ScriptInvoker();	// (int op, int code, string text)
	static int s_Replies;				// self-test

	// New session: nothing of the last server stays (Client.Init).
	static void Reset()
	{
		s_Hello = null;
		s_HelloSeq = 0;
		s_In = new INT_QST_BulkIn();
	}

	// ---- out -------------------------------------------------------------------------------------------------------
	static void Send(int op, string text)
	{
		array<string> pieces = new array<string>();
		INT_QST_Util.SplitLong(text, INT_QST_Const.PIECE_BYTES, pieces);
		int parts = INT_QST_Bulk.PartsOf(pieces);
		int msgId = s_MsgId;
		s_MsgId++;
		for (int part = 0; part < parts; part++)
		{
			ScriptRPC rpc = new ScriptRPC();
			INT_QST_Wire.WriteHeader(rpc, INT_QST_C2S.ADMIN);
			rpc.Write(op);
			INT_QST_Bulk.WritePart(rpc, msgId, part, parts, pieces);
			rpc.Send(null, INT_QST_Const.RPC_C2S, true, null);
		}
	}

	static void RequestHello()
	{
		Send(INT_QST_A.HELLO, "");
	}

	static void SaveConfig(INT_QST_AConfig c)
	{
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(c, false, txt);
		Send(INT_QST_A.CONFIG, txt);
	}

	static void Save(INT_QST_ASave a)
	{
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(a, false, txt);
		Send(INT_QST_A.SAVE, txt);
	}

	static void SaveBoard(INT_QST_ABoard b)
	{
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(b, false, txt);
		Send(INT_QST_A.BOARD_SAVE, txt);
	}

	static void SaveGiver(INT_QST_GiverDef d)
	{
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(d, false, txt);
		Send(INT_QST_A.GIVER_SAVE, txt);
	}

	static void DeleteGiver(string key)
	{
		SendKey(INT_QST_A.GIVER_DELETE, key);
	}

	static void GiverHere(string key)
	{
		SendKey(INT_QST_A.GIVER_HERE, key);
	}

	protected static void SendKey(int op, string key)
	{
		INT_QST_AKey k = new INT_QST_AKey();
		k.Key = key;
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(k, false, txt);
		Send(op, txt);
	}

	static void Export(array<int> ids, string name)
	{
		INT_QST_AExport a = new INT_QST_AExport();
		a.Name = name;
		foreach (int id : ids)
			a.Ids.Insert(id);
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(a, false, txt);
		Send(INT_QST_A.EXPORT, txt);
	}

	static void ImportList()
	{
		Send(INT_QST_A.IMPORT_LIST, "");
	}

	static void Import(string folder, int mode)
	{
		INT_QST_AImport a = new INT_QST_AImport();
		a.Folder = folder;
		a.Mode = mode;
		string txt;
		JsonSerializer js = new JsonSerializer();
		js.WriteToString(a, false, txt);
		Send(INT_QST_A.IMPORT, txt);
	}

	static void Reload()
	{
		Send(INT_QST_A.RELOAD, "");
	}

	// ---- in --------------------------------------------------------------------------------------------------------
	static void OnPacket(ParamsReadContext ctx)
	{
		int op;
		int code;
		if (!ctx.Read(op) || !ctx.Read(code))
			return;
		bool bad;
		if (!s_In.Read(ctx, op, bad))
			return;
		string text = s_In.Text();
		s_Replies++;
		if (op == INT_QST_A.HELLO && code == INT_QST_Res.OK)
		{
			INT_QST_AHello h = new INT_QST_AHello();
			JsonSerializer js = new JsonSerializer();
			string err;
			if (js.ReadFromString(h, text, err))
			{
				s_Hello = h;
				s_HelloSeq++;
			}
		}
		s_OnReply.Invoke(op, code, text);
	}

	// Text field of an AText answer ("" when it is not one).
	static string TextOf(string json)
	{
		INT_QST_AText t = new INT_QST_AText();
		JsonSerializer js = new JsonSerializer();
		string err;
		if (js.ReadFromString(t, json, err))
			return t.Text;
		return "";
	}
}
