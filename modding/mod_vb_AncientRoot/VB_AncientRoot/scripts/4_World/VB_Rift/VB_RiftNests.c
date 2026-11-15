// Гнёзда Изнанки, клиентская часть (аддон VB_Rift: ставится и на сервер, и игрокам).
// Гнездо — одна точка: сердце, его корни и щупальца. Все гнёзда лежат на сервере в nests.json, на каждом рестарте
// сервер выбирает часть и ставит. Выбор, файлы ($profile:VB_Mods\VB_Rift\: settings.json, nests.json, state.json,
// nests_backup.json) и правка — только в серверном моде VB_Rift_Server (VB_RiftNestsServer.c), игрокам он не отдаётся.
//
// Здесь: классы данных гнезда (их же сервер пишет в nests.json), протокол редактора VB_RiftNestNet,
// клиентская копия гнёзд для редактора VB_RiftNestClient и приём ответов сервера (RPC_REPLY).

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
	int Enabled = 1;		// в nests.json пишется всегда; без ключа JSON даст 0 (E57) — сервер это проверяет при загрузке
	ref array<ref VB_RiftNestObj> Objects;

	void VB_RiftNest()
	{
		Objects = new array<ref VB_RiftNestObj>;
	}

	// первое сердце гнезда (тип проверяется по кэшу VB_RiftNestUtil, без IsKindOf)
	VB_RiftNestObj VB_FindHeart()
	{
		if (!Objects)
			return null;
		foreach (VB_RiftNestObj o : Objects)
		{
			if (o && VB_RiftNestUtil.VB_IsHeartType(o.Type))
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

		if (!Objects)
			return vector.Zero;
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
		if (!Objects)
			return -1;
		for (int i = 0; i < Objects.Count(); i++)
		{
			if (Objects[i].Id == objId)
				return i;
		}
		return -1;
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

// Заранее посчитанное по гнезду из клиентской копии: есть ли сердце, где центр, как далеко разбросаны объекты.
// Редактор каждый кадр ищет ближайшее гнездо и объект под прицелом — без этого кэша он каждый кадр гонял бы
// поиск сердца по всем объектам всех гнёзд (аудит 2026-11-13, №38).
class VB_RiftNestView
{
	ref VB_RiftNest Nest;
	bool HasHeart;
	vector Heart;			// позиция сердца (если HasHeart)
	vector Center;			// сердце, а без него — середина объектов
	float Reach;			// м: самый дальний объект от центра (по земле)
}

// Клиент: копия гнёзд с сервера для редактора. Меняется только ответами сервера; s_VB_Version растёт
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

	protected static ref array<ref VB_RiftNestView> s_VB_Views;
	protected static int s_VB_ViewsVersion = -1;

	static void VB_Init()
	{
		if (s_VB_Nests)
			return;

		s_VB_Nests = new array<ref VB_RiftNest>;
		s_VB_LiveObjs = new set<int>;
		s_VB_LiveNests = new TIntArray;
		s_VB_Msgs = new TStringArray;
		s_VB_Views = new array<ref VB_RiftNestView>;
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

	// гнёзда с заранее посчитанными центрами; пересчитываются только при новой версии копии
	static array<ref VB_RiftNestView> VB_Views()
	{
		VB_Init();
		if (s_VB_ViewsVersion == s_VB_Version)
			return s_VB_Views;

		s_VB_ViewsVersion = s_VB_Version;
		s_VB_Views.Clear();
		VB_RiftNestObj heart;
		float d;
		foreach (VB_RiftNest nest : s_VB_Nests)
		{
			VB_RiftNestView v = new VB_RiftNestView();
			v.Nest = nest;
			heart = nest.VB_FindHeart();
			v.HasHeart = heart != null;
			if (heart)
				v.Heart = heart.Position;
			v.Center = nest.VB_Center();
			v.Reach = 0;
			foreach (VB_RiftNestObj o : nest.Objects)
			{
				d = VB_RiftNestUtil.VB_DistXZ(o.Position, v.Center);
				if (d > v.Reach)
					v.Reach = d;
			}
			s_VB_Views.Insert(v);
		}
		return s_VB_Views;
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

// Клиент принимает ответы редактора. Запросы RPC_EDIT принимает серверный мод VB_Rift_Server (свой modded PlayerBase).
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == VB_RiftNestNet.RPC_REPLY && !g_Game.IsDedicatedServer())
			VB_RiftNestClient.VB_OnReply(ctx);
	}
}
