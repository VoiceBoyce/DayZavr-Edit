// Гнёзда Изнанки. Гнездо — одна точка: сердце, его корни и щупальца. Все гнёзда лежат в nests.json,
// на каждом рестарте сервер выбирает часть и ставит, поэтому игроки не знают, где Изнанка проросла сегодня,
// и не фармят одни и те же места. Гнездо без сердца не встаёт никогда (редактор помечает его «без сердца»).
//
// Файлы сервера, $profile:VB_Mods\VB_Rift\ :
//   settings.json — сколько гнёзд ставить, доля корней, расстояние, прорастание, админы редактора
//   nests.json    — все гнёзда (пишет внутриигровой редактор, можно править руками при выключенном сервере)
//   state.json    — какие гнёзда стояли на прошлом рестарте (чтобы не повторять подряд)
//   nests_backup.json — копия nests.json до первой правки редактором за этот запуск сервера
//
// Сервер ставит объекты обычными сетевыми (сердце должно быть сетевым, иначе его не вырезать),
// без сохранения в мире: после рестарта их нет, сервер выбирает новый набор.

class VB_RiftNestObj
{
	int Id;
	string Type;
	vector Position;
	vector Orientation;		// поворот; наклон 0 — объект сам ложится по склону
}

class VB_RiftNest
{
	int Id;
	string Name;
	int Enabled = 1;
	ref array<ref VB_RiftNestObj> Objects;

	void VB_RiftNest()
	{
		Objects = new array<ref VB_RiftNestObj>;
	}

	VB_RiftNestObj VB_FindHeart()
	{
		foreach (VB_RiftNestObj o : Objects)
		{
			if (VB_RiftNests.VB_IsHeartType(o.Type))
				return o;
		}
		return null;
	}

	bool VB_HasHeart()
	{
		return VB_FindHeart() != null;
	}

	// центр: сердце, а без него — середина всех объектов
	vector VB_Center()
	{
		VB_RiftNestObj heart = VB_FindHeart();
		if (heart)
			return heart.Position;

		int n = Objects.Count();
		if (n == 0)
			return vector.Zero;

		vector sum = vector.Zero;
		foreach (VB_RiftNestObj o : Objects)
		{
			sum = sum + o.Position;
		}
		return sum * (1.0 / n);
	}

	int VB_FindIndex(int objId)
	{
		for (int i = 0; i < Objects.Count(); i++)
		{
			if (Objects[i].Id == objId)
				return i;
		}
		return -1;
	}
}

class VB_RiftNestFile
{
	int Version = 1;
	int NextId = 1;			// id объектов и гнёзд, общий счётчик
	int NextNumber = 1;		// номер в имени нового гнезда («Гнездо 7»)
	ref array<ref VB_RiftNest> Nests;
	ref array<ref VB_RiftNestObj> Decor;	// постоянный декор: щупальца вдали от сердец, стоят на каждом рестарте

	void VB_RiftNestFile()
	{
		Nests = new array<ref VB_RiftNest>;
		Decor = new array<ref VB_RiftNestObj>;
	}
}

class VB_RiftNestSettings
{
	ref TStringArray Admins;		// SteamID64 тех, кому можно открыть редактор (Ctrl+F8)
	int ActiveNests = 5;			// гнёзд с сердцем за рестарт
	float MinNestDistance = 300;	// м: выбранные гнёзда не ближе этого друг к другу
	int RootsPercentMin = 70;		// сколько процентов корней и щупалец гнезда встаёт (случайно от Min до Max)
	int RootsPercentMax = 100;
	int NoRepeat = 1;				// 1 — не ставить подряд те же гнёзда, что на прошлом рестарте (если есть из чего выбрать)
	int RegrowMinutes = 0;			// через столько минут после вырезания сердца прорастает новое гнездо (0 — выключено)

	void VB_RiftNestSettings()
	{
		Admins = new TStringArray;
	}
}

class VB_RiftNestState
{
	ref TIntArray LastActive;

	void VB_RiftNestState()
	{
		LastActive = new TIntArray;
	}
}

// Протокол редактора: клиент шлёт RPC_EDIT (операция + данные), сервер отвечает RPC_REPLY.
class VB_RiftNestNet
{
	static const int RPC_EDIT = 1447186010;		// клиент -> сервер, на объект своего игрока
	static const int RPC_REPLY = 1447186011;	// сервер -> клиент

	static const int OP_OPEN = 1;				// войти в редактор: сервер проверяет права и шлёт все гнёзда
	static const int OP_CLOSE = 2;
	static const int OP_ADD = 3;				// int гнездо, bool только в него, int n, n x (string тип, vector pos, vector ori)
	static const int OP_DEL = 4;				// int объект
	static const int OP_UNDO = 5;				// int гнездо: убрать последний поставленный объект
	static const int OP_NEST_DEL = 6;			// int гнездо
	static const int OP_NEST_TOGGLE = 7;		// int гнездо: включить/выключить в выборе
	static const int OP_REROLL = 8;				// снять все гнёзда и выбрать заново, как на рестарте
	static const int OP_TELEPORT = 9;			// int гнездо
	static const int OP_SET_ACTIVE = 10;		// int сколько гнёзд ставить за рестарт (пишется в settings.json)

	static const int R_DENIED = 1;				// string свой SteamID
	static const int R_BEGIN = 2;				// настройки + число гнёзд, дальше R_NEST по каждому
	static const int R_NEST = 3;				// одно гнездо целиком
	static const int R_NEST_GONE = 4;			// int гнездо удалено
	static const int R_SELECT = 5;				// int гнездо, в которое легли новые объекты
	static const int R_MSG = 6;					// string сообщение админу
	static const int R_LIVE = 7;				// int гнездо, bool живое (сменилось без правки гнезда)
	static const int R_SETTINGS = 8;			// настройки сменились (как в R_BEGIN, без числа гнёзд)
	static const int R_DECOR = 9;				// постоянный декор целиком: int n, n x (id, тип, pos, ori, bool стоит)

	static const int DECOR_ID = -1;				// «гнездо» постоянного декора в редакторе (Undo, телепорт, выбор)
	static const string DECOR_NAME = "Постоянный декор";

	static const float NEST_RADIUS = 60.0;		// м: корни ближе этого к гнезду идут в ближайшее, дальше — новое гнездо
}

class VB_RiftNests
{
	static const string DIR_MODS = "$profile:VB_Mods";
	static const string DIR = "$profile:VB_Mods\\VB_Rift";
	static const string FILE_SETTINGS = "$profile:VB_Mods\\VB_Rift\\settings.json";
	static const string FILE_NESTS = "$profile:VB_Mods\\VB_Rift\\nests.json";
	static const string FILE_BACKUP = "$profile:VB_Mods\\VB_Rift\\nests_backup.json";
	static const string FILE_STATE = "$profile:VB_Mods\\VB_Rift\\state.json";
	static const string DEFAULT_ADMIN = "76561198903710848";

	protected static ref VB_RiftNests s_VB_Server;

	ref VB_RiftNestSettings m_VB_Settings;
	ref VB_RiftNestFile m_VB_File;
	ref VB_RiftNestState m_VB_State;
	ref map<int, Object> m_VB_Live;			// id объекта -> поставленный объект
	ref TIntArray m_VB_LiveNests;			// id гнёзд, которые стоят сейчас
	ref TStringArray m_VB_Editors;			// SteamID админов с открытым редактором
	protected bool m_VB_BackedUp;
	protected int m_VB_Regrowing;			// сколько прорастаний ждёт таймера

	static bool VB_IsHeartType(string type)
	{
		return g_Game.IsKindOf(type, "VB_RiftHeart");
	}

	static bool VB_IsHangType(string type)
	{
		return g_Game.IsKindOf(type, "VB_RiftHangBase");
	}

	static VB_RiftNests VB_Get()
	{
		return s_VB_Server;
	}

	// сервер, из MissionServer: прочитать файлы и поставить гнёзда
	static void VB_StartServer()
	{
		if (s_VB_Server)
			return;

		s_VB_Server = new VB_RiftNests();
		s_VB_Server.VB_Load();
		s_VB_Server.VB_Activate();
	}

	static void VB_OnHeartKilled(VB_RiftHeart heart)
	{
		if (!s_VB_Server || s_VB_Server.m_VB_Settings.RegrowMinutes <= 0)
			return;

		s_VB_Server.m_VB_Regrowing++;
		int ms = s_VB_Server.m_VB_Settings.RegrowMinutes * 60000;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_VB_Server.VB_Regrow, ms, false);
		Print("[VB_Rift] гнёзда: новое прорастёт через " + s_VB_Server.m_VB_Settings.RegrowMinutes.ToString() + " мин");
	}

	void VB_RiftNests()
	{
		m_VB_Live = new map<int, Object>;
		m_VB_LiveNests = new TIntArray;
		m_VB_Editors = new TStringArray;
	}

	// --- файлы ------------------------------------------------------------------------------------

	protected void VB_Load()
	{
		if (!FileExist(DIR_MODS))
			MakeDirectory(DIR_MODS);
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		string err;
		m_VB_Settings = new VB_RiftNestSettings();
		if (FileExist(FILE_SETTINGS))
		{
			if (!JsonFileLoader<VB_RiftNestSettings>.LoadFile(FILE_SETTINGS, m_VB_Settings, err))
			{
				Print("[VB_Rift] гнёзда: ошибка в settings.json, беру настройки по умолчанию: " + err);
				m_VB_Settings = new VB_RiftNestSettings();
			}
		}
		else
		{
			m_VB_Settings.Admins.Insert(DEFAULT_ADMIN);
			JsonFileLoader<VB_RiftNestSettings>.SaveFile(FILE_SETTINGS, m_VB_Settings, err);
			Print("[VB_Rift] гнёзда: создан " + FILE_SETTINGS);
		}
		if (!m_VB_Settings.Admins)
			m_VB_Settings.Admins = new TStringArray;

		m_VB_File = new VB_RiftNestFile();
		if (FileExist(FILE_NESTS))
		{
			if (!JsonFileLoader<VB_RiftNestFile>.LoadFile(FILE_NESTS, m_VB_File, err))
			{
				// файл не трогаем: редактор его не перезапишет, пока не починят
				Print("[VB_Rift] гнёзда: ОШИБКА в nests.json, гнёзда не поставлены: " + err);
				m_VB_File = null;
				return;
			}
		}
		else
		{
			JsonFileLoader<VB_RiftNestFile>.SaveFile(FILE_NESTS, m_VB_File, err);
			Print("[VB_Rift] гнёзда: создан пустой " + FILE_NESTS);
		}
		VB_FixFile();

		m_VB_State = new VB_RiftNestState();
		if (FileExist(FILE_STATE))
			JsonFileLoader<VB_RiftNestState>.LoadFile(FILE_STATE, m_VB_State, err);
		if (!m_VB_State || !m_VB_State.LastActive)
			m_VB_State = new VB_RiftNestState();
	}

	// «Перебросить гнёзда»: перечитать настройки и гнёзда с диска — файлы можно поправить руками без рестарта.
	// Если nests.json сломан, остаются гнёзда из памяти.
	protected void VB_ReloadFromDisk()
	{
		VB_RiftNestFile oldFile = m_VB_File;
		VB_Load();
		if (!m_VB_File)
		{
			m_VB_File = oldFile;
			Print("[VB_Rift] гнёзда: nests.json не прочитался, остались гнёзда из памяти");
		}
	}

	protected void VB_ReloadSettings()
	{
		if (!FileExist(FILE_SETTINGS))
			return;

		string err;
		VB_RiftNestSettings fresh = new VB_RiftNestSettings();
		if (!JsonFileLoader<VB_RiftNestSettings>.LoadFile(FILE_SETTINGS, fresh, err) || !fresh)
			return;
		if (!fresh.Admins)
			fresh.Admins = new TStringArray;
		m_VB_Settings = fresh;
	}

	// после ручной правки файла: пустые списки, повторные и нулевые id, счётчики
	protected void VB_FixFile()
	{
		if (!m_VB_File.Nests)
			m_VB_File.Nests = new array<ref VB_RiftNest>;
		if (!m_VB_File.Decor)
			m_VB_File.Decor = new array<ref VB_RiftNestObj>;

		int maxId = 0;
		foreach (VB_RiftNestObj dm : m_VB_File.Decor)
		{
			maxId = Math.Max(maxId, dm.Id);
		}
		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (!nest.Objects)
				nest.Objects = new array<ref VB_RiftNestObj>;
			maxId = Math.Max(maxId, nest.Id);
			foreach (VB_RiftNestObj o : nest.Objects)
			{
				maxId = Math.Max(maxId, o.Id);
			}
		}
		if (m_VB_File.NextId <= maxId)
			m_VB_File.NextId = maxId + 1;

		set<int> seen = new set<int>;
		foreach (VB_RiftNest n : m_VB_File.Nests)
		{
			if (n.Id <= 0 || seen.Find(n.Id) >= 0)
				n.Id = VB_TakeId();
			seen.Insert(n.Id);
			foreach (VB_RiftNestObj obj : n.Objects)
			{
				if (obj.Id <= 0 || seen.Find(obj.Id) >= 0)
					obj.Id = VB_TakeId();
				seen.Insert(obj.Id);
			}
			if (n.Name == "")
				n.Name = VB_TakeName();
		}
		foreach (VB_RiftNestObj dobj : m_VB_File.Decor)
		{
			if (dobj.Id <= 0 || seen.Find(dobj.Id) >= 0)
				dobj.Id = VB_TakeId();
			seen.Insert(dobj.Id);
		}

		if (VB_Regroup())
		{
			Print("[VB_Rift] гнёзда: перегруппированы при загрузке (одно сердце на гнездо, корни к ближайшему сердцу)");
			VB_SaveNests();
		}
	}

	// Порядок в гнёздах: в гнезде одно сердце (лишние уходят в свои новые гнёзда), каждый корень и щупальце —
	// в гнезде ближайшего сердца не дальше NEST_RADIUS; опустевшие гнёзда удаляются. true — что-то поменялось.
	protected bool VB_Regroup()
	{
		bool changed = false;
		array<VB_RiftNest> nests = new array<VB_RiftNest>;
		foreach (VB_RiftNest n : m_VB_File.Nests)
		{
			nests.Insert(n);
		}

		// 1. одно сердце на гнездо
		bool hasHeart;
		foreach (VB_RiftNest nest : nests)
		{
			array<ref VB_RiftNestObj> extra = new array<ref VB_RiftNestObj>;
			hasHeart = false;
			foreach (VB_RiftNestObj o : nest.Objects)
			{
				if (!VB_IsHeartType(o.Type))
					continue;
				if (hasHeart)
					extra.Insert(o);
				hasHeart = true;
			}
			foreach (VB_RiftNestObj h : extra)
			{
				VB_RiftNest split = VB_NewNest();
				split.Objects.Insert(h);
				nest.Objects.RemoveItem(h);
				changed = true;
			}
		}

		// 2. корни и щупальца — к ближайшему сердцу; щупальца без сердца рядом — в постоянный декор
		nests.Clear();
		foreach (VB_RiftNest n2 : m_VB_File.Nests)
		{
			nests.Insert(n2);
		}
		VB_RiftNest best;
		VB_RiftNestObj obj;
		foreach (VB_RiftNest from : nests)
		{
			for (int i = from.Objects.Count() - 1; i >= 0; i--)
			{
				obj = from.Objects[i];
				if (VB_IsHeartType(obj.Type))
					continue;

				best = VB_NearestHeartNest(obj.Position);
				if (best && best != from)
				{
					best.Objects.Insert(obj);
					from.Objects.Remove(i);
					changed = true;
				}
				else if (!best && VB_IsHangType(obj.Type))
				{
					m_VB_File.Decor.Insert(obj);
					from.Objects.Remove(i);
					changed = true;
				}
			}
		}

		// 2б. щупальце декора, рядом с которым появилось сердце, уходит в его гнездо
		for (int di = m_VB_File.Decor.Count() - 1; di >= 0; di--)
		{
			obj = m_VB_File.Decor[di];
			best = VB_NearestHeartNest(obj.Position);
			if (best)
			{
				best.Objects.Insert(obj);
				m_VB_File.Decor.Remove(di);
				changed = true;
			}
		}

		// 3. пустые гнёзда убрать
		for (int k = m_VB_File.Nests.Count() - 1; k >= 0; k--)
		{
			if (m_VB_File.Nests[k].Objects.Count() == 0)
			{
				m_VB_LiveNests.RemoveItem(m_VB_File.Nests[k].Id);
				m_VB_File.Nests.Remove(k);
				changed = true;
			}
		}
		return changed;
	}

	// новый id объекта или гнезда (без x++ в выражении: в Enforce на это не полагаемся)
	protected int VB_TakeId()
	{
		int id = m_VB_File.NextId;
		m_VB_File.NextId = id + 1;
		return id;
	}

	protected string VB_TakeName()
	{
		int num = m_VB_File.NextNumber;
		m_VB_File.NextNumber = num + 1;
		return "Гнездо " + num.ToString();
	}

	protected void VB_SaveNests()
	{
		if (!m_VB_File)
			return;

		string err;
		if (!m_VB_BackedUp)
		{
			m_VB_BackedUp = true;
			if (FileExist(FILE_NESTS))
				CopyFile(FILE_NESTS, FILE_BACKUP);
		}
		if (!JsonFileLoader<VB_RiftNestFile>.SaveFile(FILE_NESTS, m_VB_File, err))
			Print("[VB_Rift] гнёзда: не удалось записать nests.json: " + err);
	}

	protected void VB_SaveSettings()
	{
		string err;
		if (!JsonFileLoader<VB_RiftNestSettings>.SaveFile(FILE_SETTINGS, m_VB_Settings, err))
			Print("[VB_Rift] гнёзда: не удалось записать settings.json: " + err);
	}

	protected void VB_SaveState()
	{
		string err;
		JsonFileLoader<VB_RiftNestState>.SaveFile(FILE_STATE, m_VB_State, err);
	}

	// --- выбор и установка ------------------------------------------------------------------------

	static float VB_DistXZ(vector a, vector b)
	{
		float dx = a[0] - b[0];
		float dz = a[2] - b[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}

	// снять всё, что стоит, и выбрать гнёзда заново, как на рестарте
	void VB_Activate()
	{
		VB_DespawnNests();
		if (!m_VB_File)
			return;

		array<VB_RiftNest> hearts = new array<VB_RiftNest>;
		int noHeart = 0;
		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (nest.Enabled == 0 || nest.Objects.Count() == 0)
				continue;
			if (nest.VB_HasHeart())
				hearts.Insert(nest);
			else
				noHeart++;
		}

		TIntArray avoid = new TIntArray;
		if (m_VB_Settings.NoRepeat == 1)
			avoid.Copy(m_VB_State.LastActive);

		array<VB_RiftNest> chosen = new array<VB_RiftNest>;
		array<vector> centers = new array<vector>;
		VB_Pick(hearts, m_VB_Settings.ActiveNests, avoid, centers, chosen);

		string names = "";
		m_VB_State.LastActive.Clear();
		foreach (VB_RiftNest pick : chosen)
		{
			VB_SpawnNest(pick);
			m_VB_State.LastActive.Insert(pick.Id);
			if (names != "")
				names += ", ";
			names += pick.Name;
		}
		VB_SaveState();

		VB_SpawnDecor();
		Print(string.Format("[VB_Rift] гнёзда: поставлено %1 из %2 (%3 объектов вместе с декором, декора %4): %5",
			chosen.Count(), hearts.Count(), m_VB_Live.Count(), m_VB_File.Decor.Count(), names));
		if (noHeart > 0)
			Print("[VB_Rift] гнёзда: без сердца, не ставятся: " + noHeart.ToString());
	}

	// count гнёзд из pool: в случайном порядке, не ближе MinNestDistance к уже выбранным;
	// сначала без гнёзд прошлого рестарта, если их не хватает — и с ними
	protected void VB_Pick(array<VB_RiftNest> pool, int count, TIntArray avoid, array<vector> centers, array<VB_RiftNest> chosen)
	{
		array<VB_RiftNest> cand = new array<VB_RiftNest>;
		cand.Copy(pool);
		cand.ShuffleArray();

		int got = 0;
		vector c;
		for (int pass = 0; pass < 2 && got < count; pass++)
		{
			foreach (VB_RiftNest nest : cand)
			{
				if (got >= count)
					break;
				if (chosen.Find(nest) >= 0)
					continue;
				if (pass == 0 && avoid.Find(nest.Id) >= 0)
					continue;

				c = nest.VB_Center();
				if (!VB_FarEnough(c, centers))
					continue;

				chosen.Insert(nest);
				centers.Insert(c);
				got++;
			}
		}
	}

	protected bool VB_FarEnough(vector c, array<vector> centers)
	{
		foreach (vector other : centers)
		{
			if (VB_DistXZ(c, other) < m_VB_Settings.MinNestDistance)
				return false;
		}
		return true;
	}

	// сердце встаёт всегда, корни и щупальца — случайная доля RootsKeepMin..Max
	protected void VB_SpawnNest(VB_RiftNest nest)
	{
		if (m_VB_LiveNests.Find(nest.Id) >= 0)
			return;

		float lo = Math.Clamp(m_VB_Settings.RootsPercentMin * 0.01, 0.0, 1.0);
		float hi = Math.Clamp(m_VB_Settings.RootsPercentMax * 0.01, lo, 1.0);
		float keep = Math.RandomFloat(lo, hi);

		array<VB_RiftNestObj> rest = new array<VB_RiftNestObj>;
		foreach (VB_RiftNestObj o : nest.Objects)
		{
			if (VB_IsHeartType(o.Type) || VB_IsHangType(o.Type))
				VB_SpawnObj(o);		// сердце и щупальца (их ставят поштучно) — всегда
			else
				rest.Insert(o);
		}

		rest.ShuffleArray();
		int n = Math.Round(rest.Count() * keep);
		if (rest.Count() > 0 && n < 1)
			n = 1;
		for (int i = 0; i < n; i++)
		{
			VB_SpawnObj(rest[i]);
		}

		m_VB_LiveNests.Insert(nest.Id);
	}

	protected Object VB_SpawnObj(VB_RiftNestObj o)
	{
		if (m_VB_Live.Contains(o.Id))
			return m_VB_Live.Get(o.Id);

		int flags = ECE_SETUP | ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD;
		Object obj = g_Game.CreateObjectEx(o.Type, o.Position, flags);
		if (!obj)
		{
			Print("[VB_Rift] гнёзда: не создался " + o.Type + " в " + o.Position.ToString());
			return null;
		}

		VB_RiftBase rift = VB_RiftBase.Cast(obj);
		if (rift)
		{
			rift.VB_PlaceAt(o.Position, o.Orientation);
		}
		else
		{
			obj.SetPosition(o.Position);
			obj.SetOrientation(o.Orientation);
		}
		obj.Update();
		m_VB_Live.Set(o.Id, obj);
		return obj;
	}

	protected void VB_DespawnObj(int objId)
	{
		Object obj;
		if (!m_VB_Live.Find(objId, obj))
			return;

		if (obj)
			g_Game.ObjectDelete(obj);
		m_VB_Live.Remove(objId);
	}

	protected void VB_DespawnNest(VB_RiftNest nest)
	{
		foreach (VB_RiftNestObj o : nest.Objects)
		{
			VB_DespawnObj(o.Id);
		}
		m_VB_LiveNests.RemoveItem(nest.Id);
	}

	// снять всё, кроме постоянного декора (он стоит весь рестарт, «Перебросить» его не трогает)
	protected void VB_DespawnNests()
	{
		set<int> keep = new set<int>;
		if (m_VB_File)
		{
			foreach (VB_RiftNestObj d : m_VB_File.Decor)
			{
				keep.Insert(d.Id);
			}
		}
		TIntArray drop = new TIntArray;
		foreach (int id, Object obj : m_VB_Live)
		{
			if (keep.Find(id) < 0)
				drop.Insert(id);
		}
		foreach (int dropId : drop)
		{
			VB_DespawnObj(dropId);
		}
		m_VB_LiveNests.Clear();
	}

	protected void VB_SpawnDecor()
	{
		if (!m_VB_File)
			return;
		foreach (VB_RiftNestObj d : m_VB_File.Decor)
		{
			VB_SpawnObj(d);
		}
	}

	protected int VB_DecorIndex(int objId)
	{
		for (int i = 0; i < m_VB_File.Decor.Count(); i++)
		{
			if (m_VB_File.Decor[i].Id == objId)
				return i;
		}
		return -1;
	}

	protected void VB_DespawnAll()
	{
		foreach (int id, Object obj : m_VB_Live)
		{
			if (obj)
				g_Game.ObjectDelete(obj);
		}
		m_VB_Live.Clear();
		m_VB_LiveNests.Clear();
	}

	// через RegrowMinutes после вырезания: одно новое гнездо с сердцем там, где сейчас ничего нет
	void VB_Regrow()
	{
		m_VB_Regrowing--;
		if (!m_VB_File)
			return;

		array<vector> centers = new array<vector>;
		array<VB_RiftNest> pool = new array<VB_RiftNest>;
		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (m_VB_LiveNests.Find(nest.Id) >= 0)
				centers.Insert(nest.VB_Center());
			else if (nest.Enabled == 1 && nest.VB_HasHeart())
				pool.Insert(nest);
		}

		array<VB_RiftNest> chosen = new array<VB_RiftNest>;
		VB_Pick(pool, 1, m_VB_State.LastActive, centers, chosen);
		if (chosen.Count() == 0)
		{
			Print("[VB_Rift] гнёзда: прорастать некуда (нет свободных гнёзд дальше " + m_VB_Settings.MinNestDistance.ToString() + " м)");
			return;
		}

		VB_RiftNest grown = chosen[0];
		VB_SpawnNest(grown);
		Print("[VB_Rift] гнёзда: проросло " + grown.Name + " в " + grown.VB_Center().ToString());
		VB_SendNestToEditors(grown);
	}

	// --- поиск ------------------------------------------------------------------------------------

	VB_RiftNest VB_FindNest(int nestId)
	{
		if (!m_VB_File)
			return null;

		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (nest.Id == nestId)
				return nest;
		}
		return null;
	}

	VB_RiftNest VB_FindNestOfObj(int objId)
	{
		if (!m_VB_File)
			return null;

		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (nest.VB_FindIndex(objId) >= 0)
				return nest;
		}
		return null;
	}

	// ближайшее гнездо (по сердцу, а без него — по середине объектов) не дальше maxDist; heartlessOnly — только без сердца
	protected VB_RiftNest VB_NearestNest(vector pos, float maxDist, bool heartlessOnly = false)
	{
		VB_RiftNest best;
		float bestD = maxDist;
		float d;
		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			if (nest.Objects.Count() == 0)
				continue;
			if (heartlessOnly && nest.VB_HasHeart())
				continue;
			d = VB_DistXZ(nest.VB_Center(), pos);
			if (d < bestD)
			{
				bestD = d;
				best = nest;
			}
		}
		return best;
	}

	// гнездо с сердцем, чьё сердце ближе всех к pos и не дальше NEST_RADIUS
	protected VB_RiftNest VB_NearestHeartNest(vector pos)
	{
		VB_RiftNest best;
		float bestD = VB_RiftNestNet.NEST_RADIUS;
		float d;
		VB_RiftNestObj heart;
		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			heart = nest.VB_FindHeart();
			if (!heart)
				continue;
			d = VB_DistXZ(heart.Position, pos);
			if (d < bestD)
			{
				bestD = d;
				best = nest;
			}
		}
		return best;
	}

	protected VB_RiftNest VB_NewNest()
	{
		VB_RiftNest nest = new VB_RiftNest();
		nest.Id = VB_TakeId();
		nest.Name = VB_TakeName();
		m_VB_File.Nests.Insert(nest);
		return nest;
	}

	// --- редактор (сервер) ------------------------------------------------------------------------

	bool VB_IsAdmin(string plainId)
	{
		return m_VB_Settings && m_VB_Settings.Admins.Find(plainId) >= 0;
	}

	static void VB_OnEditRpc(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (s_VB_Server && player && sender)
			s_VB_Server.VB_HandleEdit(player, sender, ctx);
	}

	protected void VB_HandleEdit(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		int op;
		if (!ctx.Read(op))
			return;

		string uid = sender.GetPlainId();
		if (!VB_IsAdmin(uid) && op == VB_RiftNestNet.OP_OPEN)
			VB_ReloadSettings();	// админа можно дописать в settings.json без рестарта
		if (!VB_IsAdmin(uid))
		{
			Print("[VB_Rift] редактор гнёзд: нет прав у " + sender.GetName() + " (" + uid + ")");
			if (op == VB_RiftNestNet.OP_OPEN)
			{
				ScriptRPC deny = new ScriptRPC();
				deny.Write(VB_RiftNestNet.R_DENIED);
				deny.Write(uid);
				deny.Send(player, VB_RiftNestNet.RPC_REPLY, true, sender);
			}
			return;
		}

		if (!m_VB_File)
		{
			VB_SendMsg(player, "nests.json не прочитался, правка выключена: смотри лог сервера");
			return;
		}

		int nestId;
		int objId;
		VB_RiftNest nest;
		switch (op)
		{
			case VB_RiftNestNet.OP_OPEN:
				if (m_VB_Editors.Find(uid) < 0)
					m_VB_Editors.Insert(uid);
				Print("[VB_Rift] редактор гнёзд открыл " + sender.GetName() + " (" + uid + ")");
				VB_SendAll(player);
				break;

			case VB_RiftNestNet.OP_CLOSE:
				m_VB_Editors.RemoveItem(uid);
				break;

			case VB_RiftNestNet.OP_ADD:
				VB_EditAdd(player, ctx);
				break;

			case VB_RiftNestNet.OP_DEL:
				if (ctx.Read(objId))
					VB_EditDelete(objId);
				break;

			case VB_RiftNestNet.OP_UNDO:
				if (!ctx.Read(nestId))
					break;
				if (nestId == VB_RiftNestNet.DECOR_ID)
				{
					int lastDecor = 0;
					foreach (VB_RiftNestObj dl : m_VB_File.Decor)
					{
						lastDecor = Math.Max(lastDecor, dl.Id);
					}
					if (lastDecor > 0)
						VB_EditDelete(lastDecor);
					break;
				}
				nest = VB_FindNest(nestId);
				if (nest && nest.Objects.Count() > 0)
				{
					int lastId = 0;
					foreach (VB_RiftNestObj o : nest.Objects)
					{
						lastId = Math.Max(lastId, o.Id);
					}
					VB_EditDelete(lastId);
				}
				break;

			case VB_RiftNestNet.OP_NEST_DEL:
				if (!ctx.Read(nestId))
					break;
				nest = VB_FindNest(nestId);
				if (nest)
				{
					VB_DespawnNest(nest);
					Print("[VB_Rift] гнёзда: удалено " + nest.Name + " (" + sender.GetName() + ")");
					m_VB_File.Nests.RemoveItem(nest);
					VB_SaveNests();
					VB_SendGoneToEditors(nestId);
				}
				break;

			case VB_RiftNestNet.OP_NEST_TOGGLE:
				if (!ctx.Read(nestId))
					break;
				nest = VB_FindNest(nestId);
				if (nest)
				{
					nest.Enabled = 1 - nest.Enabled;
					VB_SaveNests();
					VB_SendNestToEditors(nest);
				}
				break;

			case VB_RiftNestNet.OP_REROLL:
				Print("[VB_Rift] гнёзда: переброс по команде " + sender.GetName());
				VB_ReloadFromDisk();
				VB_Activate();
				foreach (string editor : m_VB_Editors)
				{
					PlayerBase ep = VB_FindPlayer(editor);
					if (ep)
						VB_SendAll(ep);
				}
				VB_SendMsg(player, "Гнёзда выбраны заново: " + m_VB_LiveNests.Count().ToString());
				break;

			case VB_RiftNestNet.OP_TELEPORT:
				if (!ctx.Read(nestId))
					break;
				if (nestId == VB_RiftNestNet.DECOR_ID)
				{
					// к ближайшему щупальцу декора
					VB_RiftNestObj nearDecor;
					float nearD = 1000000000.0;
					float dd;
					foreach (VB_RiftNestObj dt : m_VB_File.Decor)
					{
						dd = VB_DistXZ(dt.Position, player.GetPosition());
						if (dd < nearD)
						{
							nearD = dd;
							nearDecor = dt;
						}
					}
					if (nearDecor)
					{
						float ax = nearDecor.Position[0] + 3.0;
						float az = nearDecor.Position[2] + 3.0;
						player.SetPosition(Vector(ax, g_Game.SurfaceY(ax, az), az));
					}
					break;
				}
				nest = VB_FindNest(nestId);
				if (nest && nest.Objects.Count() > 0)
				{
					vector c = nest.VB_Center();
					float tx = c[0] + 6.0;
					float tz = c[2] + 6.0;
					player.SetPosition(Vector(tx, g_Game.SurfaceY(tx, tz), tz));
				}
				break;

			case VB_RiftNestNet.OP_SET_ACTIVE:
				int want;
				if (!ctx.Read(want))
					break;
				m_VB_Settings.ActiveNests = Math.Clamp(want, 0, 500);
				VB_SaveSettings();
				Print("[VB_Rift] гнёзда: за рестарт теперь " + m_VB_Settings.ActiveNests.ToString() + " (" + sender.GetName() + ")");
				VB_SendSettingsToEditors();
				break;
		}
	}

	// Куда ложится объект: force — в указанное гнездо («Обсадить корнями»). Иначе сердце начинает новое гнездо
	// (или становится сердцем гнезда без сердца рядом), а корни и щупальца идут в ближайшее гнездо в NEST_RADIUS;
	// дальше — в новое гнездо.
	protected void VB_EditAdd(PlayerBase player, ParamsReadContext ctx)
	{
		int nestId;
		bool force;
		int count;
		if (!ctx.Read(nestId) || !ctx.Read(force) || !ctx.Read(count) || count <= 0 || count > 64)
			return;

		array<ref VB_RiftNestObj> added = new array<ref VB_RiftNestObj>;
		string type;
		vector pos;
		vector ori;
		for (int i = 0; i < count; i++)
		{
			if (!ctx.Read(type) || !ctx.Read(pos) || !ctx.Read(ori))
				return;
			if (!g_Game.IsKindOf(type, "VB_RiftBase"))
				continue;

			VB_RiftNestObj o = new VB_RiftNestObj();
			o.Type = type;
			o.Position = pos;
			o.Orientation = ori;
			added.Insert(o);
		}
		if (added.Count() == 0)
			return;

		vector first = added[0].Position;
		VB_RiftNest nest = null;
		if (force)
			nest = VB_FindNest(nestId);
		if (!nest && VB_IsHangType(added[0].Type))
		{
			// щупальце: в гнездо ближайшего сердца, а без сердца рядом — в постоянный декор
			nest = VB_NearestHeartNest(first);
			if (!nest)
			{
				VB_EditAddDecor(player, added);
				return;
			}
		}
		if (!nest)
			nest = VB_NearestNest(first, VB_RiftNestNet.NEST_RADIUS, VB_IsHeartType(added[0].Type));
		if (!nest)
		{
			nest = VB_NewNest();
			Print("[VB_Rift] гнёзда: новое " + nest.Name + " в " + first.ToString());
		}

		bool live = m_VB_LiveNests.Find(nest.Id) >= 0;
		bool heartAdded = false;
		foreach (VB_RiftNestObj add : added)
		{
			add.Id = VB_TakeId();
			nest.Objects.Insert(add);
			if (live)
				VB_SpawnObj(add);
			if (VB_IsHeartType(add.Type))
				heartAdded = true;
		}

		// новое сердце забирает корни, которые теперь ближе к нему
		if (heartAdded && !force && VB_Regroup())
		{
			VB_SpawnDecor();
			VB_SendAllToEditors();
		}
		else
			VB_SendNestToEditors(nest);
		VB_SaveNests();

		ScriptRPC sel = new ScriptRPC();
		sel.Write(VB_RiftNestNet.R_SELECT);
		sel.Write(nest.Id);
		sel.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_EditAddDecor(PlayerBase player, array<ref VB_RiftNestObj> added)
	{
		foreach (VB_RiftNestObj add : added)
		{
			add.Id = VB_TakeId();
			m_VB_File.Decor.Insert(add);
			VB_SpawnObj(add);
		}
		VB_SaveNests();
		VB_SendDecorToEditors();

		ScriptRPC sel = new ScriptRPC();
		sel.Write(VB_RiftNestNet.R_SELECT);
		sel.Write(VB_RiftNestNet.DECOR_ID);
		sel.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_EditDelete(int objId)
	{
		int decorIdx = VB_DecorIndex(objId);
		if (decorIdx >= 0)
		{
			VB_DespawnObj(objId);
			m_VB_File.Decor.Remove(decorIdx);
			VB_SaveNests();
			VB_SendDecorToEditors();
			return;
		}

		VB_RiftNest nest = VB_FindNestOfObj(objId);
		if (!nest)
			return;

		int idx = nest.VB_FindIndex(objId);
		bool wasHeart = VB_IsHeartType(nest.Objects[idx].Type);
		VB_DespawnObj(objId);
		nest.Objects.Remove(idx);

		// убрали сердце: его корни переходят к ближайшему другому сердцу, если оно рядом
		if (wasHeart && VB_Regroup())
		{
			VB_SpawnDecor();
			VB_SaveNests();
			VB_SendAllToEditors();
			return;
		}

		if (nest.Objects.Count() == 0)
		{
			// пустое гнездо не нужно: следующий объект рядом создаст новое
			m_VB_LiveNests.RemoveItem(nest.Id);
			int goneId = nest.Id;
			m_VB_File.Nests.RemoveItem(nest);
			VB_SaveNests();
			VB_SendGoneToEditors(goneId);
			return;
		}
		VB_SaveNests();
		VB_SendNestToEditors(nest);
	}

	// --- отправка ---------------------------------------------------------------------------------

	protected PlayerBase VB_FindPlayer(string plainId)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		foreach (Man someone : players)
		{
			if (someone && someone.GetIdentity() && someone.GetIdentity().GetPlainId() == plainId)
				return PlayerBase.Cast(someone);
		}
		return null;
	}

	protected void VB_SendMsg(PlayerBase player, string text)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_RiftNestNet.R_MSG);
		rpc.Write(text);
		rpc.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());
	}

	// порядок полей читает VB_RiftNestClient.VB_ReadSettings
	protected void VB_WriteSettings(ScriptRPC rpc)
	{
		rpc.Write(m_VB_Settings.ActiveNests);
		rpc.Write(m_VB_Settings.MinNestDistance);
		rpc.Write(m_VB_Settings.RootsPercentMin);
		rpc.Write(m_VB_Settings.RootsPercentMax);
		rpc.Write(m_VB_Settings.RegrowMinutes);
	}

	protected void VB_SendSettingsToEditors()
	{
		foreach (string editor : m_VB_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (!ep)
				continue;

			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_RiftNestNet.R_SETTINGS);
			VB_WriteSettings(rpc);
			rpc.Send(ep, VB_RiftNestNet.RPC_REPLY, true, ep.GetIdentity());
		}
	}

	protected void VB_SendAll(PlayerBase player)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_RiftNestNet.R_BEGIN);
		VB_WriteSettings(rpc);
		rpc.Write(m_VB_File.Nests.Count());
		rpc.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());

		foreach (VB_RiftNest nest : m_VB_File.Nests)
		{
			VB_SendNest(player, nest);
		}
		VB_SendDecor(player);
	}

	protected void VB_SendDecor(PlayerBase player)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_RiftNestNet.R_DECOR);
		rpc.Write(m_VB_File.Decor.Count());
		bool objLive;
		foreach (VB_RiftNestObj o : m_VB_File.Decor)
		{
			rpc.Write(o.Id);
			rpc.Write(o.Type);
			rpc.Write(o.Position);
			rpc.Write(o.Orientation);
			objLive = m_VB_Live.Contains(o.Id);
			rpc.Write(objLive);
		}
		rpc.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_SendDecorToEditors()
	{
		foreach (string editor : m_VB_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (ep)
				VB_SendDecor(ep);
		}
	}

	protected void VB_SendNest(PlayerBase player, VB_RiftNest nest)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_RiftNestNet.R_NEST);
		rpc.Write(nest.Id);
		rpc.Write(nest.Name);
		rpc.Write(nest.Enabled);
		bool nestLive = m_VB_LiveNests.Find(nest.Id) >= 0;
		rpc.Write(nestLive);
		rpc.Write(nest.Objects.Count());
		bool objLive;
		foreach (VB_RiftNestObj o : nest.Objects)
		{
			rpc.Write(o.Id);
			rpc.Write(o.Type);
			rpc.Write(o.Position);
			rpc.Write(o.Orientation);
			objLive = m_VB_Live.Contains(o.Id);
			rpc.Write(objLive);
		}
		rpc.Send(player, VB_RiftNestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_SendNestToEditors(VB_RiftNest nest)
	{
		foreach (string editor : m_VB_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (ep)
				VB_SendNest(ep, nest);
		}
	}

	protected void VB_SendAllToEditors()
	{
		foreach (string editor : m_VB_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (ep)
				VB_SendAll(ep);
		}
	}

	protected void VB_SendGoneToEditors(int nestId)
	{
		foreach (string editor : m_VB_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (!ep)
				continue;

			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_RiftNestNet.R_NEST_GONE);
			rpc.Write(nestId);
			rpc.Send(ep, VB_RiftNestNet.RPC_REPLY, true, ep.GetIdentity());
		}
	}
}

// Клиент: копия гнёзд с сервера для редактора. Меняется только ответами сервера; m_VB_Version растёт
// с каждым изменением, редактор и окно перерисовываются, когда видят новую версию.
class VB_RiftNestClient
{
	static ref array<ref VB_RiftNest> s_VB_Nests;
	static ref set<int> s_VB_LiveObjs;			// id объектов, которые сервер поставил по-настоящему
	static ref TIntArray s_VB_LiveNests;
	static int s_VB_Version;
	static int s_VB_ActiveNests;
	static float s_VB_MinDist;
	static int s_VB_KeepMin;					// проценты корней
	static int s_VB_KeepMax;
	static int s_VB_Regrow;

	// события для редактора (забирает и обнуляет)
	static bool s_VB_Granted;					// пришёл R_BEGIN: права есть
	static bool s_VB_Denied;
	static string s_VB_DeniedId;
	static int s_VB_SelectNest;					// сервер положил объекты в это гнездо
	static ref TStringArray s_VB_Msgs;

	static void VB_Init()
	{
		if (s_VB_Nests)
			return;

		s_VB_Nests = new array<ref VB_RiftNest>;
		s_VB_LiveObjs = new set<int>;
		s_VB_LiveNests = new TIntArray;
		s_VB_Msgs = new TStringArray;
	}

	static void VB_Clear()
	{
		VB_Init();
		s_VB_Nests.Clear();
		s_VB_LiveObjs.Clear();
		s_VB_LiveNests.Clear();
		s_VB_Version++;
	}

	static VB_RiftNest VB_Find(int nestId)
	{
		VB_Init();
		foreach (VB_RiftNest nest : s_VB_Nests)
		{
			if (nest.Id == nestId)
				return nest;
		}
		return null;
	}

	static bool VB_IsLiveNest(int nestId)
	{
		VB_Init();
		return s_VB_LiveNests.Find(nestId) >= 0;
	}

	static void VB_OnReply(ParamsReadContext ctx)
	{
		VB_Init();
		int r;
		if (!ctx.Read(r))
			return;

		int nestId;
		switch (r)
		{
			case VB_RiftNestNet.R_DENIED:
				ctx.Read(s_VB_DeniedId);
				s_VB_Denied = true;
				break;

			case VB_RiftNestNet.R_BEGIN:
				int count;
				VB_ReadSettings(ctx);
				ctx.Read(count);
				VB_Clear();
				s_VB_Granted = true;
				break;

			case VB_RiftNestNet.R_SETTINGS:
				VB_ReadSettings(ctx);
				s_VB_Version++;
				break;

			case VB_RiftNestNet.R_DECOR:
				VB_ReadDecor(ctx);
				s_VB_Version++;
				break;

			case VB_RiftNestNet.R_NEST:
				VB_ReadNest(ctx);
				s_VB_Version++;
				break;

			case VB_RiftNestNet.R_NEST_GONE:
				if (ctx.Read(nestId))
					VB_Remove(nestId);
				s_VB_Version++;
				break;

			case VB_RiftNestNet.R_SELECT:
				if (ctx.Read(nestId))
					s_VB_SelectNest = nestId;
				break;

			case VB_RiftNestNet.R_MSG:
				string msg;
				if (ctx.Read(msg))
					s_VB_Msgs.Insert(msg);
				break;
		}
	}

	// декор у клиента — псевдо-гнездо с id DECOR_ID в общем списке (так его видят прицел, удаление и предпросмотр)
	protected static void VB_ReadDecor(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count))
			return;

		VB_Remove(VB_RiftNestNet.DECOR_ID);
		VB_RiftNest decor = new VB_RiftNest();
		decor.Id = VB_RiftNestNet.DECOR_ID;
		decor.Name = VB_RiftNestNet.DECOR_NAME;
		bool objLive;
		for (int i = 0; i < count; i++)
		{
			VB_RiftNestObj o = new VB_RiftNestObj();
			if (!ctx.Read(o.Id) || !ctx.Read(o.Type) || !ctx.Read(o.Position) || !ctx.Read(o.Orientation) || !ctx.Read(objLive))
				return;
			decor.Objects.Insert(o);
			if (objLive)
				s_VB_LiveObjs.Insert(o.Id);
		}
		if (count > 0)
			s_VB_Nests.Insert(decor);
	}

	protected static void VB_ReadSettings(ParamsReadContext ctx)
	{
		ctx.Read(s_VB_ActiveNests);
		ctx.Read(s_VB_MinDist);
		ctx.Read(s_VB_KeepMin);
		ctx.Read(s_VB_KeepMax);
		ctx.Read(s_VB_Regrow);
	}

	protected static void VB_Remove(int nestId)
	{
		VB_RiftNest old = VB_Find(nestId);
		if (!old)
			return;

		foreach (VB_RiftNestObj o : old.Objects)
		{
			s_VB_LiveObjs.RemoveItem(o.Id);
		}
		s_VB_LiveNests.RemoveItem(nestId);
		s_VB_Nests.RemoveItem(old);
	}

	protected static void VB_ReadNest(ParamsReadContext ctx)
	{
		VB_RiftNest nest = new VB_RiftNest();
		bool live;
		int count;
		if (!ctx.Read(nest.Id) || !ctx.Read(nest.Name) || !ctx.Read(nest.Enabled) || !ctx.Read(live) || !ctx.Read(count))
			return;

		VB_Remove(nest.Id);
		bool objLive;
		for (int i = 0; i < count; i++)
		{
			VB_RiftNestObj o = new VB_RiftNestObj();
			if (!ctx.Read(o.Id) || !ctx.Read(o.Type) || !ctx.Read(o.Position) || !ctx.Read(o.Orientation) || !ctx.Read(objLive))
				return;
			nest.Objects.Insert(o);
			if (objLive)
				s_VB_LiveObjs.Insert(o.Id);
		}
		s_VB_Nests.Insert(nest);
		if (live)
			s_VB_LiveNests.Insert(nest.Id);
	}

	// --- запросы к серверу ------------------------------------------------------------------------

	static void VB_SendOp(int op, int arg = 0)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(arg);
		rpc.Send(player, VB_RiftNestNet.RPC_EDIT, true, null);
	}

	// force — положить именно в nestId (иначе сервер сам выберет гнездо по месту)
	static void VB_SendAdd(int nestId, TStringArray types, array<vector> poses, array<vector> oris, bool force = false)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player || types.Count() == 0)
			return;

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_RiftNestNet.OP_ADD);
		rpc.Write(nestId);
		rpc.Write(force);
		rpc.Write(types.Count());
		for (int i = 0; i < types.Count(); i++)
		{
			rpc.Write(types[i]);
			rpc.Write(poses[i]);
			rpc.Write(oris[i]);
		}
		rpc.Send(player, VB_RiftNestNet.RPC_EDIT, true, null);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == VB_RiftNestNet.RPC_EDIT && g_Game.IsServer())
			VB_RiftNests.VB_OnEditRpc(this, sender, ctx);
		else if (rpc_type == VB_RiftNestNet.RPC_REPLY && !g_Game.IsDedicatedServer())
			VB_RiftNestClient.VB_OnReply(ctx);
	}
}
