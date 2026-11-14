// INT Quests — admin protocol (spec.md §6.2). An admin operation carries one JSON payload; the payload travels in
// 600-byte pieces, up to 16 pieces per RPC, in as many RPCs as needed ("bulk"): [op][msgId][part][parts][n][pieces].
// The server answers with the same bulk shape plus a result code. Payload classes below are plain JSON objects:
// every field is written by the sender, so the reader's "created objects get zero fields" rule does not bite.

class INT_QST_A
{
	static const int HELLO = 1;		// -> AHello: problems, board layout, quest files, import folders
	static const int SAVE = 2;		// ASave -> ASaveResult: quests / branches created, changed, deleted
	static const int BOARD_SAVE = 3;	// ABoard -> (code)
	static const int GIVER_SAVE = 4;	// GiverDef -> (code): placed / changed / dressed; respawned
	static const int GIVER_DELETE = 5;	// AKey -> (code)
	static const int EXPORT = 6;		// AExport -> AText (folder name)
	static const int IMPORT_LIST = 7;	// -> AImportList
	static const int IMPORT = 8;		// AImport -> AText (summary)
	static const int RELOAD = 9;		// -> AText (summary); full catalog resend to everybody
	static const int GIVER_HERE = 10;	// AKey -> (code): move a giver to the admin's position and facing
	static const int CONFIG = 11;		// AConfig -> (code): server switches; everybody gets a new HELLO

	static const int MAX_PAYLOAD_BYTES = 600000;
	static const int MAX_PARTS = 80;
}

// One node of the board layout (design px of the board canvas).
class INT_QST_BoardNode
{
	int Id;
	float X;
	float Y;
	int Auto;		// 1 = an automatic place: the server takes it only when the id has no place yet
}

class INT_QST_ABoard
{
	int Version = 1;
	ref array<ref INT_QST_BoardNode> Nodes = new array<ref INT_QST_BoardNode>();
}

// Server switches an admin may change in game (the rest stays in INT_QUESTS_Config.json).
class INT_QST_AConfig
{
	// SIGNS-OFF (owner 2026-10-31)
	// int ShowSigns = 1;
	int ShowLockedAtGiver = 1;
	int CanCancel = 1;
	int QuestSlots = 60;
	int AutoTrackMax = 5;
	int DebugMode;
	int GrenadeKills;
}

class INT_QST_AHello
{
	int Version = 1;
	string Server;					// server name for the board title
	ref INT_QST_AConfig Config = new INT_QST_AConfig();
	ref array<string> Problems = new array<string>();
	ref array<ref INT_QST_BoardNode> Nodes = new array<ref INT_QST_BoardNode>();
	ref array<string> Files = new array<string>();	// "id|Branch/File.json"
	ref array<string> Imports = new array<string>();	// folder names in Imports/
}

// A change made on the admin client. Kind 1 = quest, 2 = branch. New quests carry a NEGATIVE temporary Id; the
// server gives them real ids and rewrites Requires inside the same batch.
class INT_QST_AChange
{
	int Kind;
	int Delete;
	int Id;
	string Key;					// branch key (Kind 2) / branch of the quest (Kind 1)
	ref INT_QST_QuestDef Quest;
	ref INT_QST_BranchDef Branch;
	string BranchTitle;				// Kind 2 (BranchDef.m_Key is not serialized)
	int BaseRec;					// Kind 1: record hash of the quest when the admin took the copy (0 = no check):
							// the server refuses (CONFLICT) when the quest changed since
}

class INT_QST_ASave
{
	int Version = 1;
	ref array<ref INT_QST_AChange> Changes = new array<ref INT_QST_AChange>();
}

class INT_QST_ASaveResult
{
	int Version = 1;
	ref array<int> TempIds = new array<int>();	// negative ids sent by the client
	ref array<int> NewIds = new array<int>();	// the ids the server gave them
	ref array<string> Problems = new array<string>();
	int Saved;
	ref array<int> Failed = new array<int>();	// quest ids the server refused (temp ids for new quests)
	ref array<int> FailCodes = new array<int>();	// INT_QST_Res per Failed entry
}

class INT_QST_AKey
{
	string Key;
}

class INT_QST_AExport
{
	string Name;
	ref array<int> Ids = new array<int>();
}

class INT_QST_AImport
{
	string Folder;
	int Mode;			// 0 = quests with a taken Id get a new Id, 1 = replace quests with the same Id
}

class INT_QST_AImportList
{
	ref array<string> Folders = new array<string>();
	ref array<string> Info = new array<string>();	// "branches N, quests M" per folder
}

class INT_QST_AText
{
	string Text;
}

// Bulk transport helpers (both sides).
class INT_QST_Bulk
{
	// Writes part `part` of `pieces` into rpc (after the caller wrote header + op + extra ints).
	static void WritePart(ScriptRPC rpc, int msgId, int part, int parts, array<string> pieces)
	{
		rpc.Write(msgId);
		rpc.Write(part);
		rpc.Write(parts);
		int first = part * INT_QST_Const.PIECES_PER_PACKET;
		int last = Math.Min(first + INT_QST_Const.PIECES_PER_PACKET, pieces.Count());
		int n = last - first;
		if (n < 0)
			n = 0;
		rpc.Write(n);
		for (int i = first; i < last; i++)
		{
			string p = pieces[i];
			rpc.Write(p);
		}
	}

	static int PartsOf(array<string> pieces)
	{
		int n = pieces.Count();
		if (n == 0)
			return 1;
		return (n + INT_QST_Const.PIECES_PER_PACKET - 1) / INT_QST_Const.PIECES_PER_PACKET;
	}
}

// Receiving side of one bulk message.
class INT_QST_BulkIn
{
	int m_MsgId = -1;
	int m_Op;
	int m_Parts;
	int m_Got;
	ref array<ref array<string>> m_Data = new array<ref array<string>>();

	// Reads one part; true when the message is complete (Text() then gives the payload).
	bool Read(ParamsReadContext ctx, int op, out bool bad)
	{
		bad = false;
		int msgId;
		int part;
		int parts;
		int n;
		if (!ctx.Read(msgId) || !ctx.Read(part) || !ctx.Read(parts) || !ctx.Read(n))
		{
			bad = true;
			return false;
		}
		if (parts <= 0 || parts > INT_QST_A.MAX_PARTS || part < 0 || part >= parts || n < 0 || n > INT_QST_Const.PIECES_PER_PACKET)
		{
			bad = true;
			return false;
		}
		if (msgId != m_MsgId)
		{
			m_MsgId = msgId;
			m_Op = op;
			m_Parts = parts;
			m_Got = 0;
			m_Data.Clear();
			for (int k = 0; k < parts; k++)
				m_Data.Insert(null);
		}
		if (parts != m_Parts || m_Data[part])
			return false;
		array<string> got = new array<string>();
		for (int i = 0; i < n; i++)
		{
			string s;
			if (!ctx.Read(s))
			{
				bad = true;
				return false;
			}
			got.Insert(s);
		}
		m_Data[part] = got;
		m_Got++;
		return m_Got >= m_Parts;
	}

	string Text()
	{
		string t = "";
		foreach (array<string> a : m_Data)
		{
			if (!a)
				continue;
			foreach (string s : a)
				t = t + s;
		}
		m_MsgId = -1;
		m_Data.Clear();
		return t;
	}
}
