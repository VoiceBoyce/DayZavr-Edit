// Редактор точек боссов (ТЗ 6): протокол, копия данных у клиента и разбор ответов сервера.
// Клиент шлёт RPC_EDIT на своего игрока (операция + данные), сервер отвечает RPC_REPLY только ему.
// Номера RPC мода: 01 — находка детектора, 02–03 — эффекты способностей, 04 — голос, 05–06 — писк чужих детекторов, редактор — 0x56414210–11.

class VB_AB_EditorNet
{
	static const int RPC_EDIT = 0x56414210;		// клиент -> сервер
	static const int RPC_REPLY = 0x56414211;	// сервер -> клиент

	static const int OP_OPEN = 1;				// войти: сервер проверяет права и шлёт всё
	static const int OP_CLOSE = 2;
	static const int OP_ADD = 3;				// vector позиция, string босс
	static const int OP_DEL = 4;				// int точка
	static const int OP_DISABLE = 5;			// int точка, int 1 — выключить, 0 — включить
	static const int OP_WEIGHT = 6;				// int точка, float вес
	static const int OP_BOSS_ADD = 7;			// int точка, string босс
	static const int OP_BOSS_DEL = 8;			// int точка, string босс
	static const int OP_TELEPORT = 9;			// int точка
	static const int OP_SUMMON = 10;			// int точка, string босс ("" — случайный из списка точки)
	static const int OP_FIELD_OPEN = 11;		// int точка: раскрыть поле без агра (призовёт, если босса нет)
	static const int OP_FIELD_REROLL = 12;		// int точка: свернуть поле и раскрыть заново
	static const int OP_DESPAWN = 13;			// int точка
	static const int OP_RELOAD = 14;			// перечитать конфиги
	static const int OP_SET_ACTIVE = 15;		// int точек за рестарт (settings.json)

	static const int R_DENIED = 1;				// string свой SteamID
	static const int R_BEGIN = 2;				// настройки; дальше R_BOSSES, R_POINT по каждой точке, R_LIVE
	static const int R_BOSSES = 3;				// каталог боссов из bosses.json
	static const int R_POINT = 4;				// одна точка целиком
	static const int R_POINT_GONE = 5;			// int точка удалена
	static const int R_SELECT = 6;				// int точка: выделить (новая)
	static const int R_MSG = 7;					// string ключ строки, string %1, string %2
	static const int R_SETTINGS = 8;			// как в R_BEGIN
	static const int R_LIVE = 9;				// что сейчас на точках этого рестарта

	// что на точке рестарта (R_LIVE)
	static const int LIVE_WAIT = 0;				// ждёт игрока
	static const int LIVE_AWAKE = 1;			// босс на точке, поле свёрнуто
	static const int LIVE_FIELD = 2;			// поле раскрыто
	static const int LIVE_DONE = 3;				// убит или убран — до рестарта
	static const int LIVE_RESPAWN = 4;			// убит, вернётся через RespawnMin
}

// Строки редактора: ключи stringtable (tools/strings/editor.json)
class VB_AB_EdText
{
	static string Tr(string key)
	{
		return Widget.TranslateString("#" + key);
	}

	// Перевод или запасной текст, если ключа нет
	static string TOr(string key, string fallback)
	{
		string s = Widget.TranslateString("#" + key);
		if (s == "" || s.IndexOf("STR_VB_AB") >= 0)
			return fallback;
		return s;
	}

	// «1 аномалия», «3 аномалии», «7 аномалий»
	static string Count(int n, string one, string few, string many)
	{
		int n100 = n % 100;
		int n10 = n % 10;
		string word = Tr(many);
		if (n100 < 11 || n100 > 14)
		{
			if (n10 == 1)
				word = Tr(one);
			else if (n10 >= 2 && n10 <= 4)
				word = Tr(few);
		}
		return n.ToString() + " " + word;
	}

	static string ModelName(string token)
	{
		return TOr("STR_VB_AB_Ed_Model_" + token, token);
	}

	static string PaletteName(string token)
	{
		return TOr("STR_VB_AB_Ed_Pal_" + token, token);
	}
}

// Точка у клиента
class VB_AB_EdPoint
{
	int Id;
	vector Position;
	ref TStringArray Bosses = new TStringArray;
	float Weight;
	int Disabled;
}

// Босс из bosses.json у клиента (каталог)
class VB_AB_EdBoss
{
	string Id;
	string Name;
	string Classname;
	int Tier;
	float Radius;			// поле, м
	float Leash;			// поводок, м
	string Model;			// из Classname VB_AB_<Модель>_<Палитра>
	string Palette;
}

// Что сейчас на точке рестарта
class VB_AB_EdLive
{
	int PointId;
	int Kind;				// VB_AB_EditorNet.LIVE_*
	string BossId;
	float Hp;				// 0..1
	int Anomalies;
	vector Center;			// центр поля; без поля — где босс (или точка)
	int RespawnSec;
}

// Клиент: копия данных сервера для редактора. Меняется только ответами сервера; s_Version растёт с каждым
// изменением точек и каталога, s_LiveVersion — с каждым снимком R_LIVE. Окно и маркеры перерисовываются по ним.
class VB_AB_EditorClient
{
	static ref array<ref VB_AB_EdPoint> s_Points;
	static ref array<ref VB_AB_EdBoss> s_Bosses;
	static ref map<int, ref VB_AB_EdLive> s_Live;
	static int s_Version;
	static int s_CatalogVersion;			// сменился каталог боссов (вход, «Перечитать»)
	static int s_LiveVersion;			// любой новый снимок R_LIVE с изменениями (здоровье, центр поля)
	static int s_LiveListVersion;			// сменилось состояние точек (для списка: не перерисовывать его каждые 2 с)
	protected static string s_LiveSig;
	protected static string s_LiveListSig;
	static int s_ActivePerRestart;
	static float s_WakeRadius;
	static float s_MinDist;
	static int s_MaxAwake;

	// события для редактора (забирает и обнуляет)
	static bool s_Granted;
	static bool s_Denied;
	static string s_DeniedId;
	static int s_SelectPoint;
	static ref TStringArray s_Msgs;

	static void VB_Init()
	{
		if (s_Points)
			return;
		s_Points = new array<ref VB_AB_EdPoint>;
		s_Bosses = new array<ref VB_AB_EdBoss>;
		s_Live = new map<int, ref VB_AB_EdLive>;
		s_Msgs = new TStringArray;
	}

	static VB_AB_EdPoint VB_FindPoint(int id)
	{
		VB_Init();
		foreach (VB_AB_EdPoint p : s_Points)
		{
			if (p.Id == id)
				return p;
		}
		return null;
	}

	static VB_AB_EdBoss VB_FindBoss(string id)
	{
		VB_Init();
		foreach (VB_AB_EdBoss b : s_Bosses)
		{
			if (b.Id == id)
				return b;
		}
		return null;
	}

	static VB_AB_EdLive VB_FindLive(int pointId)
	{
		VB_Init();
		return s_Live.Get(pointId);
	}

	static string VB_BossName(string id)
	{
		VB_AB_EdBoss b = VB_FindBoss(id);
		if (b && b.Name != "")
			return b.Name;
		return id;
	}

	// --- ответы сервера ---------------------------------------------------------------------------

	static void VB_OnReply(ParamsReadContext ctx)
	{
		VB_Init();
		int r;
		if (!ctx.Read(r))
			return;

		int id;
		switch (r)
		{
			case VB_AB_EditorNet.R_DENIED:
				ctx.Read(s_DeniedId);
				s_Denied = true;
				break;

			case VB_AB_EditorNet.R_BEGIN:
				VB_ReadSettings(ctx);
				s_Points.Clear();
				s_Bosses.Clear();
				s_Live.Clear();
				s_Version++;
				s_LiveVersion++;
				s_LiveListVersion++;
				s_LiveSig = "";
				s_LiveListSig = "";
				s_Granted = true;
				break;

			case VB_AB_EditorNet.R_SETTINGS:
				VB_ReadSettings(ctx);
				s_Version++;
				break;

			case VB_AB_EditorNet.R_BOSSES:
				VB_ReadBosses(ctx);
				s_Version++;
				s_CatalogVersion++;
				break;

			case VB_AB_EditorNet.R_POINT:
				VB_ReadPoint(ctx);
				s_Version++;
				break;

			case VB_AB_EditorNet.R_POINT_GONE:
				if (ctx.Read(id))
				{
					VB_AB_EdPoint gone = VB_FindPoint(id);
					if (gone)
						s_Points.RemoveItem(gone);
					s_Live.Remove(id);
				}
				s_Version++;
				break;

			case VB_AB_EditorNet.R_SELECT:
				if (ctx.Read(id))
					s_SelectPoint = id;
				break;

			case VB_AB_EditorNet.R_MSG:
				string key;
				string a1;
				string a2;
				if (ctx.Read(key) && ctx.Read(a1) && ctx.Read(a2))
					s_Msgs.Insert(string.Format(VB_AB_EdText.Tr(key), a1, a2));
				break;

			case VB_AB_EditorNet.R_LIVE:
				VB_ReadLive(ctx);
				break;
		}
	}

	protected static void VB_ReadSettings(ParamsReadContext ctx)
	{
		ctx.Read(s_ActivePerRestart);
		ctx.Read(s_WakeRadius);
		ctx.Read(s_MinDist);
		ctx.Read(s_MaxAwake);
	}

	// каталог: тяжёлые сверху, внутри уровня — по имени
	protected static void VB_ReadBosses(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count))
			return;

		array<ref VB_AB_EdBoss> got = new array<ref VB_AB_EdBoss>;
		TStringArray keys = new TStringArray;
		for (int i = 0; i < count; i++)
		{
			VB_AB_EdBoss b = new VB_AB_EdBoss();
			if (!ctx.Read(b.Id) || !ctx.Read(b.Name) || !ctx.Read(b.Classname) || !ctx.Read(b.Tier) || !ctx.Read(b.Radius) || !ctx.Read(b.Leash))
				return;
			TStringArray parts = new TStringArray;
			b.Classname.Split("_", parts);
			if (parts.Count() >= 3)
				b.Model = parts[2];
			if (parts.Count() >= 4)
				b.Palette = parts[3];
			got.Insert(b);
			int rank = 9 - Math.Clamp(b.Tier, 0, 9);
			keys.Insert(rank.ToString() + b.Name + "|" + i.ToString());
		}

		keys.Sort();
		s_Bosses.Clear();
		foreach (string k : keys)
		{
			int sep = k.LastIndexOf("|");
			int idx = k.Substring(sep + 1, k.Length() - sep - 1).ToInt();
			s_Bosses.Insert(got[idx]);
		}
	}

	protected static void VB_ReadPoint(ParamsReadContext ctx)
	{
		VB_AB_EdPoint p = new VB_AB_EdPoint();
		int count;
		if (!ctx.Read(p.Id) || !ctx.Read(p.Position) || !ctx.Read(p.Weight) || !ctx.Read(p.Disabled) || !ctx.Read(count))
			return;

		string bossId;
		for (int i = 0; i < count; i++)
		{
			if (!ctx.Read(bossId))
				return;
			p.Bosses.Insert(bossId);
		}

		VB_AB_EdPoint old = VB_FindPoint(p.Id);
		if (old)
			s_Points.RemoveItem(old);
		s_Points.Insert(p);
	}

	protected static void VB_ReadLive(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count))
			return;

		s_Live.Clear();
		string sig = "";
		string listSig = "";
		for (int i = 0; i < count; i++)
		{
			VB_AB_EdLive lv = new VB_AB_EdLive();
			if (!ctx.Read(lv.PointId) || !ctx.Read(lv.Kind) || !ctx.Read(lv.BossId) || !ctx.Read(lv.Hp) || !ctx.Read(lv.Anomalies) || !ctx.Read(lv.Center) || !ctx.Read(lv.RespawnSec))
				break;
			s_Live.Set(lv.PointId, lv);
			listSig += lv.PointId.ToString() + ":" + lv.Kind.ToString() + ";";
			sig += lv.PointId.ToString() + ":" + lv.Kind.ToString() + ":" + lv.BossId + ":" + Math.Round(lv.Hp * 100).ToString() + ":" + lv.Anomalies.ToString() + ":" + Math.Round(lv.Center[0]).ToString() + ":" + Math.Round(lv.Center[2]).ToString() + ":" + Math.Ceil(lv.RespawnSec / 60.0).ToString() + ";";
		}

		// снимок приходит раз в 2 с: окно и маркеры перерисовываем, только если что-то изменилось
		if (sig != s_LiveSig)
		{
			s_LiveSig = sig;
			s_LiveVersion++;
		}
		if (listSig != s_LiveListSig)
		{
			s_LiveListSig = listSig;
			s_LiveListVersion++;
		}
	}

	// --- запросы к серверу ------------------------------------------------------------------------

	protected static ScriptRPC VB_Begin(int op)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		return rpc;
	}

	protected static void VB_Send(ScriptRPC rpc)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
			rpc.Send(player, VB_AB_EditorNet.RPC_EDIT, true, null);
	}

	static void VB_SendOp(int op)
	{
		VB_Send(VB_Begin(op));
	}

	static void VB_SendInt(int op, int a)
	{
		ScriptRPC rpc = VB_Begin(op);
		rpc.Write(a);
		VB_Send(rpc);
	}

	static void VB_SendIntInt(int op, int a, int b)
	{
		ScriptRPC rpc = VB_Begin(op);
		rpc.Write(a);
		rpc.Write(b);
		VB_Send(rpc);
	}

	static void VB_SendIntFloat(int op, int a, float b)
	{
		ScriptRPC rpc = VB_Begin(op);
		rpc.Write(a);
		rpc.Write(b);
		VB_Send(rpc);
	}

	static void VB_SendIntStr(int op, int a, string b)
	{
		ScriptRPC rpc = VB_Begin(op);
		rpc.Write(a);
		rpc.Write(b);
		VB_Send(rpc);
	}

	static void VB_SendAdd(vector pos, string bossId)
	{
		ScriptRPC rpc = VB_Begin(VB_AB_EditorNet.OP_ADD);
		rpc.Write(pos);
		rpc.Write(bossId);
		VB_Send(rpc);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		// RPC_EDIT разбирает серверный мод (VB_AnomaliesBosses_Server, VB_AB_EditorServer.c)
		if (rpc_type == VB_AB_EditorNet.RPC_REPLY && !g_Game.IsDedicatedServer())
			VB_AB_EditorClient.VB_OnReply(ctx);
	}
}
