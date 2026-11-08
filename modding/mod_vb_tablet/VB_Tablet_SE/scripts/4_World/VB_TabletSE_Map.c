// Карта на сервере. Личное: $profile:VB_Mods/VB_Tablet/players/<uid>_map.json, клановое лежит в файле клана,
// серверные метки: map_server.json. Клиент присылает только желание: права, лимиты и текст проверяются здесь.
class VB_MapPlayerFile
{
	int NextId = 1;
	string PrefClan;	// клан, к которому относятся клановые ключи настроек
	ref array<ref VB_MapObject> Objects = new array<ref VB_MapObject>();
	ref array<int> Hidden = new array<int>();
	ref array<int> D3 = new array<int>();
	ref array<int> HiddenCats = new array<int>();

	void Fix()
	{
		if (!Objects)
			Objects = new array<ref VB_MapObject>();
		if (!Hidden)
			Hidden = new array<int>();
		if (!D3)
			D3 = new array<int>();
		if (!HiddenCats)
			HiddenCats = new array<int>();
	}
}

class VB_MapServerFile
{
	int NextId = 1;
	ref array<ref VB_MapObject> Objects = new array<ref VB_MapObject>();
}

class VB_TabletMap
{
	static const string FILE_SERVER = "$profile:VB_Mods/VB_Tablet/map_server.json";
	static const int HIDDEN_MAX = 300;
	static const int GEN_CAT = 1;

	protected static ref map<string, ref VB_MapPlayerFile> s_Players;
	protected static ref VB_MapServerFile s_Server;
	protected static bool s_ServerBroken;						// map_server.json не читается: не перезаписываем
	protected static ref array<string> s_DirtyPlayers = new array<string>();	// личные файлы ждут записи (Flush)

	static void Handle(PlayerBase player, PlayerIdentity id, int op, ParamsReadContext ctx)
	{
		int a;
		int b;
		int c1;
		switch (op)
		{
			case VB_TabletOp.MAP_GET:
				SendAll(player, id);
				break;
			case VB_TabletOp.MAP_SAVE:
				SaveObject(player, id, ctx);
				break;
			case VB_TabletOp.MAP_DELETE:
				if (ctx.Read(a) && ctx.Read(b))
					DeleteObject(player, id, a, b);
				break;
			case VB_TabletOp.MAP_CAT_SAVE:
				SaveCat(player, id, ctx);
				break;
			case VB_TabletOp.MAP_CAT_DELETE:
				if (ctx.Read(a))
					DeleteCat(player, id, a);
				break;
			case VB_TabletOp.MAP_PREF:
				if (ctx.Read(a) && ctx.Read(b) && ctx.Read(c1))
					SetPref(player, id, a, b, c1);
				break;
			case VB_TabletOp.MAP_MATES:
				SendMates(player, id);
				break;
		}
	}

	// Игрок вышел: несохранённое пишется, файл выгружается из памяти.
	static void Release(string uid)
	{
		if (s_DirtyPlayers.Find(uid) >= 0)
			WritePlayer(uid);
		if (s_Players)
			s_Players.Remove(uid);
	}

	// Из таймера миссии и при остановке: личные файлы карты пишутся пачкой, а не на каждый клик настроек.
	static void Flush()
	{
		array<string> list = new array<string>();
		list.Copy(s_DirtyPlayers);
		foreach (string uid : list)
			WritePlayer(uid);
	}

	// --- хранение ---------------------------------------------------------------------------
	protected static string PathOf(string uid)
	{
		return VB_TabletPlayers.DIR_PLAYERS + "/" + uid + "_map.json";
	}

	protected static VB_MapPlayerFile FileOf(string uid)
	{
		if (!s_Players)
			s_Players = new map<string, ref VB_MapPlayerFile>();
		VB_MapPlayerFile f;
		if (s_Players.Find(uid, f))
			return f;
		f = null;
		string path = PathOf(uid);
		string error;
		for (int n = 0; n < 3; n++)
		{
			VB_MapPlayerFile read = new VB_MapPlayerFile();
			if (VB_SafeJson<VB_MapPlayerFile>.LoadFrom(VB_TabletFiles.Attempt(path, n), read, error))
			{
				VB_TabletFiles.Recovered(path, n);
				f = read;
				break;
			}
		}
		if (!f)
			f = new VB_MapPlayerFile();
		f.Fix();
		s_Players.Set(uid, f);
		return f;
	}

	protected static void SavePlayer(string uid)
	{
		if (s_DirtyPlayers.Find(uid) < 0)
			s_DirtyPlayers.Insert(uid);
	}

	protected static void WritePlayer(string uid)
	{
		s_DirtyPlayers.RemoveItem(uid);
		if (!VB_TabletCheck.IsSteam64(uid))
			return;
		VB_TabletFiles.Dirs();
		MakeDirectory(VB_TabletPlayers.DIR_PLAYERS);
		string error;
		if (!VB_SafeJson<VB_MapPlayerFile>.Save(PathOf(uid), FileOf(uid), error))
			Print(VB_TABLET_TAG + " не записана карта игрока " + uid + ": " + error);
	}

	protected static VB_MapServerFile ServerFile()
	{
		if (s_Server)
			return s_Server;
		s_Server = null;
		string error;
		bool any = false;
		for (int n = 0; n < 3; n++)
		{
			string file = VB_TabletFiles.Attempt(FILE_SERVER, n);
			if (!FileExist(file))
				continue;
			any = true;
			VB_MapServerFile read = new VB_MapServerFile();
			if (JsonFileLoader<VB_MapServerFile>.LoadFile(file, read, error))
			{
				VB_TabletFiles.Recovered(FILE_SERVER, n);
				s_Server = read;
				break;
			}
			Print(VB_TABLET_TAG + " ошибка в " + file + ": " + error);
		}
		// Файл есть, но ни одна копия не читается: серверные метки не затираем пустым списком.
		if (!s_Server)
		{
			s_ServerBroken = any;
			s_Server = new VB_MapServerFile();
		}
		if (!s_Server.Objects)
			s_Server.Objects = new array<ref VB_MapObject>();
		return s_Server;
	}

	protected static void SaveServer()
	{
		if (s_ServerBroken)
		{
			Print(VB_TABLET_TAG + " map_server.json повреждён, изменения серверных меток не записаны. Почините файл и перезапустите сервер.");
			return;
		}
		string error;
		VB_TabletFiles.Dirs();
		if (!VB_SafeJson<VB_MapServerFile>.Save(FILE_SERVER, ServerFile(), error))
			Print(VB_TABLET_TAG + " не записан map_server.json: " + error);
	}

	// Категории клана: общая есть всегда.
	protected static void EnsureCats(VB_Clan c)
	{
		if (!c.MapObjects)
			c.MapObjects = new array<ref VB_MapObject>();
		if (!c.MapCats)
			c.MapCats = new array<ref VB_MapCat>();
		if (c.MapCats.Count() > 0)
			return;
		VB_MapCat gen = new VB_MapCat();
		gen.CatId = GEN_CAT;
		gen.Title = "#STR_VB_TAB_D_CAT_GEN";
		gen.Color = c.Color;
		gen.IconIdx = 0;
		c.MapCats.Insert(gen);
		c.MapNextCat = Math.Max(c.MapNextCat, GEN_CAT + 1);
	}

	protected static array<ref VB_MapObject> ListOf(string uid, int scope)
	{
		if (scope == VB_MapScope.ME)
			return FileOf(uid).Objects;
		if (scope == VB_MapScope.SERVER)
			return ServerFile().Objects;
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (!c)
			return null;
		EnsureCats(c);
		return c.MapObjects;
	}

	protected static bool CanEdit(string uid, int scope)
	{
		if (scope == VB_MapScope.ME)
			return true;
		if (scope == VB_MapScope.SERVER)
			return VB_TabletConfig.Get().IsAdmin(uid);
		if (scope != VB_MapScope.CLAN)
			return false;
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		return c && c.MemberCan(uid, VB_ClanPerm.MAP);
	}

	protected static int NextIdOf(string uid, int scope)
	{
		int id;
		if (scope == VB_MapScope.ME)
		{
			VB_MapPlayerFile f = FileOf(uid);
			id = f.NextId;
			f.NextId = id + 1;
			return id;
		}
		if (scope == VB_MapScope.SERVER)
		{
			VB_MapServerFile s = ServerFile();
			id = s.NextId;
			s.NextId = id + 1;
			return id;
		}
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		id = Math.Max(1, c.MapNext);
		c.MapNext = id + 1;
		return id;
	}

	protected static void Persist(string uid, int scope)
	{
		if (scope == VB_MapScope.ME)
		{
			SavePlayer(uid);
		}
		else if (scope == VB_MapScope.SERVER)
		{
			SaveServer();
		}
		else
		{
			VB_Clan c = VB_TabletClans.OfPlayer(uid);
			if (c)
				VB_TabletClans.Save(c);
		}
	}

	// --- отправка ---------------------------------------------------------------------------
	protected static void SendAll(PlayerBase player, PlayerIdentity id)
	{
		SendConf(player, id);
		SendScope(player, id, VB_MapScope.ME);
		SendScope(player, id, VB_MapScope.CLAN);
		SendScope(player, id, VB_MapScope.SERVER);
	}

	// Настройки клановых объектов живут только в своём клане: номера в новом клане начинаются заново.
	protected static void DropForeignPrefs(string uid, VB_MapPlayerFile f)
	{
		string clanId = "";
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (c)
			clanId = c.Id;
		if (f.PrefClan == clanId)
			return;
		f.PrefClan = clanId;
		f.HiddenCats.Clear();
		DropScopeKeys(f.Hidden, VB_MapScope.CLAN);
		DropScopeKeys(f.D3, VB_MapScope.CLAN);
		SavePlayer(uid);
	}

	protected static void DropScopeKeys(array<int> keys, int scope)
	{
		for (int i = keys.Count() - 1; i >= 0; i--)
		{
			if (keys.Get(i) / VB_MAP_KEY_STEP == scope)
				keys.Remove(i);
		}
	}

	protected static void WriteInts(ScriptRPC rpc, array<int> list)
	{
		int n = list.Count();
		rpc.Write(n);
		for (int i = 0; i < n; i++)
		{
			int v = list.Get(i);
			rpc.Write(v);
		}
	}

	protected static void SendConf(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_MapPlayerFile f = FileOf(uid);
		DropForeignPrefs(uid, f);
		int op = VB_TabletOp.MAP_CONF;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		VB_TabletConfig.Get().MapLimits.Write(rpc);
		WriteInts(rpc, f.Hidden);
		WriteInts(rpc, f.D3);
		WriteInts(rpc, f.HiddenCats);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void SendScope(PlayerBase player, PlayerIdentity id, int scope)
	{
		string uid = id.GetPlainId();
		array<ref VB_MapObject> list = ListOf(uid, scope);
		VB_Clan c = null;
		if (scope == VB_MapScope.CLAN)
			c = VB_TabletClans.OfPlayer(uid);
		int op = VB_TabletOp.MAP_SCOPE;
		int cats = 0;
		if (c)
			cats = c.MapCats.Count();
		int n = 0;
		if (list)
			n = list.Count();
		// Серверный слой: ещё метки карты VPP, только для чтения.
		array<ref VB_MapObject> vpp = null;
		if (scope == VB_MapScope.SERVER)
			vpp = VB_TabletVpp.Marks();
		if (vpp)
			n = n + vpp.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(scope);
		rpc.Write(cats);
		if (c)
		{
			foreach (VB_MapCat cat : c.MapCats)
				cat.Write(rpc);
		}
		rpc.Write(n);
		if (list)
		{
			foreach (VB_MapObject o : list)
				o.Write(rpc);
		}
		if (vpp)
		{
			foreach (VB_MapObject v : vpp)
				v.Write(rpc);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	// Изменение видят все, кому оно положено.
	protected static void Push(PlayerBase player, PlayerIdentity id, int scope)
	{
		if (scope == VB_MapScope.ME)
		{
			SendScope(player, id, scope);
			return;
		}
		if (scope == VB_MapScope.CLAN)
		{
			VB_Clan c = VB_TabletClans.OfPlayer(id.GetPlainId());
			if (!c)
				return;
			foreach (VB_ClanMember m : c.Members)
			{
				PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
				if (p)
					SendScope(p, p.GetIdentity(), scope);
			}
			return;
		}
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase other = PlayerBase.Cast(man);
			if (other && other.GetIdentity())
				SendScope(other, other.GetIdentity(), scope);
		}
	}

	protected static void SendMates(PlayerBase player, PlayerIdentity id)
	{
		string uid = id.GetPlainId();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		array<ref VB_MapMate> list = new array<ref VB_MapMate>();
		if (c)
		{
			foreach (VB_ClanMember m : c.Members)
			{
				if (m.Uid == uid)
					continue;
				PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
				if (!p || !p.IsAlive())
					continue;
				vector at = p.GetPosition();
				VB_MapMate row = new VB_MapMate();
				row.Who = m.PlayerName;
				row.X = at[0];
				row.Z = at[2];
				list.Insert(row);
			}
		}
		int op = VB_TabletOp.MAP_MATES;
		int n = list.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (VB_MapMate item : list)
			item.Write(rpc);
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	// --- объекты ----------------------------------------------------------------------------
	protected static int CheckObject(VB_MapObject o)
	{
		int len = o.Title.LengthUtf8();
		if (len < 1 || len > VB_MAP_TITLE_MAX)
			return VB_TabletResult.BAD_INPUT;
		if (VB_TabletClans.HasBanned(o.Title))
			return VB_TabletResult.BANNED_WORD;
		if (o.Kind < VB_MapKind.MARK || o.Kind > VB_MapKind.ROUTE)
			return VB_TabletResult.BAD_INPUT;
		if (o.Scope < VB_MapScope.ME || o.Scope > VB_MapScope.SERVER)
			return VB_TabletResult.BAD_INPUT;
		if (o.Scope == VB_MapScope.SERVER && o.Kind != VB_MapKind.MARK)
			return VB_TabletResult.BAD_INPUT;
		int total = o.Pts.Count();
		if (total % 2 != 0)
			return VB_TabletResult.BAD_INPUT;
		int n = total / 2;
		int need = 1;
		int most = 1;
		if (o.Kind == VB_MapKind.ZONE)
		{
			need = 3;
			most = VB_MAP_POINTS_MAX;
		}
		else if (o.Kind == VB_MapKind.ROUTE)
		{
			need = 2;
			most = VB_MAP_POINTS_MAX;
		}
		if (n < need || n > most)
			return VB_TabletResult.BAD_INPUT;
		for (int i = 0; i < total; i++)
		{
			float v = o.Pts.Get(i);
			// Сравнение ложно и для нечисла.
			if (!(v >= 0 && v <= VB_MAP_WORLD_MAX))
				return VB_TabletResult.BAD_INPUT;
			o.Pts.Set(i, Math.Round(v));
		}
		if (o.Kind == VB_MapKind.CIRCLE)
		{
			if (!(o.Radius >= VB_MAP_RADIUS_MIN && o.Radius <= VB_MAP_RADIUS_MAX))
				return VB_TabletResult.BAD_INPUT;
			o.Radius = Math.Round(o.Radius);
		}
		else
		{
			o.Radius = 0;
		}
		if (o.IconIdx < 0 || o.IconIdx >= VB_MapIcons.Marks().Count())
			o.IconIdx = 0;
		o.Color = o.Color | 0xFF000000;
		return VB_TabletResult.OK;
	}

	protected static VB_MapObject FindIn(array<ref VB_MapObject> list, int objId)
	{
		foreach (VB_MapObject o : list)
		{
			if (o.ObjId == objId)
				return o;
		}
		return null;
	}

	protected static int CountIn(array<ref VB_MapObject> list, int kind)
	{
		int n = 0;
		foreach (VB_MapObject o : list)
		{
			if (VB_MapState.SameLimit(o.Kind, kind))
				n++;
		}
		return n;
	}

	protected static void SaveObject(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		int oldScope;
		VB_MapObject o = new VB_MapObject();
		if (!ctx.Read(oldScope) || !o.Read(ctx))
			return;
		string uid = id.GetPlainId();
		int res = CheckObject(o);
		if (res != VB_TabletResult.OK)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		if (!CanEdit(uid, o.Scope) || !CanEdit(uid, oldScope))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		array<ref VB_MapObject> list = ListOf(uid, o.Scope);
		if (!list)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_Clan c = null;
		if (o.Scope == VB_MapScope.CLAN)
		{
			c = VB_TabletClans.OfPlayer(uid);
			o.CatId = FixCat(c, o.CatId);
		}
		else
		{
			o.CatId = 0;
		}
		VB_MapObject was = null;
		array<ref VB_MapObject> oldList = null;
		if (o.ObjId > 0)
		{
			oldList = ListOf(uid, oldScope);
			if (oldList)
				was = FindIn(oldList, o.ObjId);
			if (!was)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
				return;
			}
		}
		if (was && oldScope == o.Scope)
		{
			// Вид сменился (метка стала зоной): у нового вида свой лимит.
			if (!VB_MapState.SameLimit(was.Kind, o.Kind))
			{
				int limKind = VB_TabletConfig.Get().MapLimits.Of(o.Scope, o.Kind);
				if (CountIn(list, o.Kind) >= limKind)
				{
					VB_TabletServer.SendResult(player, id, VB_TabletResult.MAP_LIMIT);
					return;
				}
			}
			o.Author = was.Author;
			list.Set(list.Find(was), o);
			Persist(uid, o.Scope);
			Push(player, id, o.Scope);
			return;
		}
		int lim = VB_TabletConfig.Get().MapLimits.Of(o.Scope, o.Kind);
		if (CountIn(list, o.Kind) >= lim)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.MAP_LIMIT);
			return;
		}
		o.Author = id.GetName();
		if (o.Scope == VB_MapScope.SERVER)
			o.Author = "";
		else if (was)
			o.Author = was.Author;
		o.ObjId = NextIdOf(uid, o.Scope);
		list.Insert(o);
		if (c)
			VB_TabletClans.Log(c, VB_ClanEvent.MAP, id.GetName(), o.Title);
		Persist(uid, o.Scope);
		Push(player, id, o.Scope);
		if (was)
		{
			oldList.RemoveOrdered(oldList.Find(was));
			Persist(uid, oldScope);
			Push(player, id, oldScope);
		}
	}

	protected static int FixCat(VB_Clan c, int catId)
	{
		foreach (VB_MapCat cat : c.MapCats)
		{
			if (cat.CatId == catId)
				return catId;
		}
		return GEN_CAT;
	}

	protected static void DeleteObject(PlayerBase player, PlayerIdentity id, int scope, int objId)
	{
		string uid = id.GetPlainId();
		if (!CanEdit(uid, scope))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		array<ref VB_MapObject> list = ListOf(uid, scope);
		VB_MapObject o = null;
		if (list)
			o = FindIn(list, objId);
		// Объекта уже нет (удалил другой соклановец): клиент получает свежий список вместо тишины.
		if (!o)
		{
			if (list)
				Push(player, id, scope);
			return;
		}
		list.RemoveOrdered(list.Find(o));
		VB_MapPlayerFile f = FileOf(uid);
		int key = scope * VB_MAP_KEY_STEP + objId;
		f.Hidden.RemoveItem(key);
		f.D3.RemoveItem(key);
		Persist(uid, scope);
		if (scope != VB_MapScope.ME)
			SavePlayer(uid);
		Push(player, id, scope);
	}

	// --- категории --------------------------------------------------------------------------
	protected static void SaveCat(PlayerBase player, PlayerIdentity id, ParamsReadContext ctx)
	{
		VB_MapCat cat = new VB_MapCat();
		if (!cat.Read(ctx))
			return;
		string uid = id.GetPlainId();
		if (!CanEdit(uid, VB_MapScope.CLAN))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		int len = cat.Title.LengthUtf8();
		if (len < 1 || len > VB_MAP_CAT_TITLE_MAX || cat.IconIdx < 0 || cat.IconIdx >= VB_MapIcons.Cats().Count())
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		if (VB_TabletClans.HasBanned(cat.Title))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BANNED_WORD);
			return;
		}
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		EnsureCats(c);
		cat.Color = cat.Color | 0xFF000000;
		VB_MapCat was = null;
		foreach (VB_MapCat item : c.MapCats)
		{
			if (item.CatId == cat.CatId)
				was = item;
		}
		if (cat.CatId > 0 && !was)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		if (was)
		{
			was.Title = cat.Title;
			was.Color = cat.Color;
			was.IconIdx = cat.IconIdx;
		}
		else
		{
			if (c.MapCats.Count() >= VB_TabletConfig.Get().MapLimits.ClanCats)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.MAP_LIMIT);
				return;
			}
			cat.CatId = Math.Max(GEN_CAT + 1, c.MapNextCat);
			c.MapNextCat = cat.CatId + 1;
			c.MapCats.Insert(cat);
		}
		VB_TabletClans.Save(c);
		Push(player, id, VB_MapScope.CLAN);
	}

	protected static void DeleteCat(PlayerBase player, PlayerIdentity id, int catId)
	{
		string uid = id.GetPlainId();
		if (!CanEdit(uid, VB_MapScope.CLAN) || catId == GEN_CAT)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		EnsureCats(c);
		for (int i = c.MapCats.Count() - 1; i >= 0; i--)
		{
			if (c.MapCats.Get(i).CatId == catId)
				c.MapCats.RemoveOrdered(i);
		}
		foreach (VB_MapObject o : c.MapObjects)
		{
			if (o.CatId == catId)
				o.CatId = GEN_CAT;
		}
		VB_TabletClans.Save(c);
		Push(player, id, VB_MapScope.CLAN);
	}

	// --- настройки показа: у каждого свои ----------------------------------------------------
	protected static bool MarkExists(string uid, int key)
	{
		int scope = key / VB_MAP_KEY_STEP;
		int objId = key % VB_MAP_KEY_STEP;
		if (scope == VB_MapScope.SERVER && objId >= VB_MAP_VPP_ID)
			return VB_TabletVpp.Has(objId);
		array<ref VB_MapObject> list = ListOf(uid, scope);
		if (!list)
			return false;
		VB_MapObject o = FindIn(list, objId);
		return o && o.Kind == VB_MapKind.MARK;
	}

	protected static void SetPref(PlayerBase player, PlayerIdentity id, int what, int key, int mode)
	{
		string uid = id.GetPlainId();
		VB_MapPlayerFile f = FileOf(uid);
		if (what == VB_MapPref.CATEGORY)
		{
			f.HiddenCats.RemoveItem(key);
			VB_Clan c = VB_TabletClans.OfPlayer(uid);
			bool known = false;
			if (c && c.MapCats)
			{
				foreach (VB_MapCat cat : c.MapCats)
				{
					if (cat.CatId == key)
						known = true;
				}
			}
			if (mode == VB_MapMode.HIDDEN && known)
				f.HiddenCats.Insert(key);
			while (f.HiddenCats.Count() > VB_TabletConfig.Get().MapLimits.ClanCats + 1)
				f.HiddenCats.RemoveOrdered(0);
			SavePlayer(uid);
			return;
		}
		if (what != VB_MapPref.OBJECT || key < 0 || key / VB_MAP_KEY_STEP > VB_MapScope.SERVER)
			return;
		f.Hidden.RemoveItem(key);
		f.D3.RemoveItem(key);
		if (mode == VB_MapMode.HIDDEN)
		{
			f.Hidden.Insert(key);
			while (f.Hidden.Count() > HIDDEN_MAX)
				f.Hidden.RemoveOrdered(0);
		}
		else if (mode == VB_MapMode.WORLD)
		{
			VB_MapLimits lim = VB_TabletConfig.Get().MapLimits;
			for (int i = f.D3.Count() - 1; i >= 0; i--)
			{
				if (!MarkExists(uid, f.D3.Get(i)))
					f.D3.Remove(i);
			}
			if (!lim.Allow3D || !MarkExists(uid, key))
			{
				SendConf(player, id);
				return;
			}
			if (f.D3.Count() >= lim.D3Max)
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.MAP_D3);
				SendConf(player, id);
				return;
			}
			f.D3.Insert(key);
		}
		SavePlayer(uid);
	}
}
