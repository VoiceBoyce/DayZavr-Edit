// Карта планшета: данные, общие для клиента и сервера. Сервер хранит объекты в JSON, клиент получает полями по ScriptRPC.
// Координаты мировые: x, z. Настройка показа (скрыт, 2D, 3D) своя у каждого игрока и хранится отдельно от объекта.

const int VB_MAP_POINTS_MAX = 16;
const int VB_MAP_TITLE_MAX = 32;
const int VB_MAP_CAT_TITLE_MAX = 24;
const float VB_MAP_WORLD_MAX = 40000;
const float VB_MAP_RADIUS_MIN = 20;
const float VB_MAP_RADIUS_MAX = 20000;
const int VB_MAP_KEY_STEP = 10000000;	// ключ объекта: область * шаг + номер

enum VB_MapKind
{
	MARK = 0,
	ZONE = 1,
	CIRCLE = 2,
	ROUTE = 3
}

enum VB_MapScope
{
	ME = 0,
	CLAN = 1,
	SERVER = 2
}

// Режим показа по кругу: скрыт, 2D, 3D.
enum VB_MapMode
{
	HIDDEN = 0,
	FLAT = 1,
	WORLD = 2
}

// Что меняет MAP_PREF.
enum VB_MapPref
{
	OBJECT = 0,
	CATEGORY = 1
}

// Значки: на сервере и в сети только индекс.
class VB_MapIcons
{
	protected static ref array<string> s_Marks;
	protected static ref array<string> s_Cats;

	static array<string> Marks()
	{
		if (!s_Marks)
		{
			array<string> marks = {"pin", "flag", "base", "stash", "danger", "rally", "sos", "crosshair", "zombie", "radiation", "anomaly", "car", "heart", "fish", "axe", "hammer", "key", "lock", "star", "mate", "gps", "clock", "sym_paw", "sym_tree", "sym_mountain", "sym_drop", "sym_flame", "sym_sun", "sym_skull", "sym_castle", "sym_tower", "sym_target"};
			s_Marks = marks;
		}
		return s_Marks;
	}

	static array<string> Cats()
	{
		if (!s_Cats)
		{
			array<string> cats = {"flag", "base", "danger", "stash", "rally", "zombie", "car", "hammer", "route", "crosshair", "heart", "fish"};
			s_Cats = cats;
		}
		return s_Cats;
	}

	static string MarkName(int i)
	{
		array<string> all = Marks();
		if (i < 0 || i >= all.Count())
			return all.Get(0);
		return all.Get(i);
	}

	static string CatName(int i)
	{
		array<string> all = Cats();
		if (i < 0 || i >= all.Count())
			return all.Get(0);
		return all.Get(i);
	}
}

// Лимиты карты. Значения из конфига сервера, клиент получает их в MAP_CONF.
class VB_MapLimits
{
	int MeZones = 10;		// зоны считаются вместе с кругами
	int MeMarks = 30;
	int MeRoutes = 5;
	int ClanZones = 20;
	int ClanMarks = 50;
	int ClanRoutes = 10;
	int ClanCats = 12;
	int ServerMarks = 100;
	int D3Max = 15;			// сколько 3D меток включено у игрока одновременно
	bool Allow3D = true;

	int Of(int scope, int kind)
	{
		if (scope == VB_MapScope.SERVER)
		{
			if (kind == VB_MapKind.MARK)
				return ServerMarks;
			return 0;
		}
		if (scope == VB_MapScope.CLAN)
		{
			if (kind == VB_MapKind.MARK)
				return ClanMarks;
			if (kind == VB_MapKind.ROUTE)
				return ClanRoutes;
			return ClanZones;
		}
		if (kind == VB_MapKind.MARK)
			return MeMarks;
		if (kind == VB_MapKind.ROUTE)
			return MeRoutes;
		return MeZones;
	}

	void Write(ScriptRPC rpc)
	{
		rpc.Write(MeZones);
		rpc.Write(MeMarks);
		rpc.Write(MeRoutes);
		rpc.Write(ClanZones);
		rpc.Write(ClanMarks);
		rpc.Write(ClanRoutes);
		rpc.Write(ClanCats);
		rpc.Write(ServerMarks);
		rpc.Write(D3Max);
		rpc.Write(Allow3D);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(MeZones) || !ctx.Read(MeMarks) || !ctx.Read(MeRoutes) || !ctx.Read(ClanZones) || !ctx.Read(ClanMarks))
			return false;
		return ctx.Read(ClanRoutes) && ctx.Read(ClanCats) && ctx.Read(ServerMarks) && ctx.Read(D3Max) && ctx.Read(Allow3D);
	}
}

// Метка, зона, круг или маршрут. Точки лежат подряд: x0, z0, x1, z1...
class VB_MapObject extends Managed
{
	int ObjId;			// 0: ещё не сохранён на сервере
	int Scope;
	int Kind;
	int IconIdx;
	string Title;
	int Color;
	int CatId;
	float Radius;
	string Author;		// у серверных меток пусто, клиент пишет «Администрация»
	ref array<float> Pts = new array<float>();

	[NonSerialized()]
	bool Sent;			// новый объект уже отправлен, ответ придёт новым списком

	int ObjKey()
	{
		return Scope * VB_MAP_KEY_STEP + ObjId;
	}

	int PointCount()
	{
		return Pts.Count() / 2;
	}

	float PX(int i)
	{
		return Pts.Get(i * 2);
	}

	float PZ(int i)
	{
		return Pts.Get(i * 2 + 1);
	}

	void AddPoint(float x, float z)
	{
		Pts.Insert(x);
		Pts.Insert(z);
	}

	void SetPoint(int i, float x, float z)
	{
		Pts.Set(i * 2, x);
		Pts.Set(i * 2 + 1, z);
	}

	// Точка привязки подписи и показа на карте.
	void AnchorOf(out float x, out float z)
	{
		int n = PointCount();
		if (n <= 0)
			return;
		if (Kind == VB_MapKind.ZONE)
		{
			float sx = 0;
			float sz = 0;
			for (int i = 0; i < n; i++)
			{
				sx = sx + PX(i);
				sz = sz + PZ(i);
			}
			x = sx / n;
			z = sz / n;
			return;
		}
		int at = 0;
		if (Kind == VB_MapKind.ROUTE)
			at = n / 2;
		x = PX(at);
		z = PZ(at);
	}

	// Площадь многоугольника, квадратные метры.
	float AreaOf()
	{
		int n = PointCount();
		float s = 0;
		for (int i = 0; i < n; i++)
		{
			int k = (i + 1) % n;
			s = s + Pts.Get(i * 2) * Pts.Get(k * 2 + 1) - Pts.Get(k * 2) * Pts.Get(i * 2 + 1);
		}
		return Math.AbsFloat(s) * 0.5;
	}

	// Длина ломаной, метры.
	float LengthOf()
	{
		float s = 0;
		for (int i = 1; i < PointCount(); i++)
		{
			float dx = Pts.Get(i * 2) - Pts.Get(i * 2 - 2);
			float dz = Pts.Get(i * 2 + 1) - Pts.Get(i * 2 - 1);
			s = s + Math.Sqrt(dx * dx + dz * dz);
		}
		return s;
	}

	void Write(ScriptRPC rpc)
	{
		rpc.Write(ObjId);
		rpc.Write(Scope);
		rpc.Write(Kind);
		rpc.Write(IconIdx);
		rpc.Write(Title);
		rpc.Write(Color);
		rpc.Write(CatId);
		rpc.Write(Radius);
		rpc.Write(Author);
		int n = Pts.Count();
		rpc.Write(n);
		for (int i = 0; i < n; i++)
		{
			float v = Pts.Get(i);
			rpc.Write(v);
		}
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(ObjId) || !ctx.Read(Scope) || !ctx.Read(Kind) || !ctx.Read(IconIdx) || !ctx.Read(Title))
			return false;
		if (!ctx.Read(Color) || !ctx.Read(CatId) || !ctx.Read(Radius) || !ctx.Read(Author))
			return false;
		int n;
		if (!ctx.Read(n) || n < 0 || n > VB_MAP_POINTS_MAX * 2)
			return false;
		Pts.Clear();
		for (int i = 0; i < n; i++)
		{
			float v;
			if (!ctx.Read(v))
				return false;
			Pts.Insert(v);
		}
		return true;
	}
}

// Категория клановых объектов. Первая, общая, не удаляется.
class VB_MapCat extends Managed
{
	int CatId;
	string Title;
	int Color;
	int IconIdx;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(CatId);
		rpc.Write(Title);
		rpc.Write(Color);
		rpc.Write(IconIdx);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(CatId) && ctx.Read(Title) && ctx.Read(Color) && ctx.Read(IconIdx);
	}
}

// Соклановец в сети: сервер присылает только по запросу открытой карты.
class VB_MapMate extends Managed
{
	string Who;
	float X;
	float Z;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Who);
		rpc.Write(X);
		rpc.Write(Z);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Who) && ctx.Read(X) && ctx.Read(Z);
	}
}

// Что клиент знает о карте. Окно планшета и 3D метки следят за Version.
class VB_MapState
{
	bool Loaded;
	int Version;
	int MatesVersion;
	ref VB_MapLimits Limits = new VB_MapLimits();
	ref array<ref VB_MapObject> Objects = new array<ref VB_MapObject>();
	ref array<ref VB_MapCat> Cats = new array<ref VB_MapCat>();
	ref array<ref VB_MapMate> Mates = new array<ref VB_MapMate>();
	ref array<int> HiddenKeys = new array<int>();
	ref array<int> D3Keys = new array<int>();
	ref array<int> HiddenCats = new array<int>();
	// Слои живут до конца сессии, как и настройки вида карты.
	bool LayerMine = true;
	bool LayerClan = true;
	bool LayerMates = true;
	bool LayerServer = true;
	protected string m_ClanId;

	static bool IsMapOp(int op)
	{
		return op >= 60 && op <= 79;
	}

	void OnServer(int op, ParamsReadContext ctx)
	{
		switch (op)
		{
			case VB_TabletOp.MAP_CONF:
				ReadConf(ctx);
				break;
			case VB_TabletOp.MAP_SCOPE:
				ReadScope(ctx);
				break;
			case VB_TabletOp.MAP_MATES:
				ReadMates(ctx);
				break;
		}
	}

	// Клан игрока сменился: клановые объекты чужие. true: нужен новый запрос.
	bool ClanChanged(string clanId)
	{
		if (clanId == m_ClanId)
			return false;
		m_ClanId = clanId;
		for (int i = Objects.Count() - 1; i >= 0; i--)
		{
			if (Objects.Get(i).Scope == VB_MapScope.CLAN)
				Objects.RemoveOrdered(i);
		}
		Cats.Clear();
		Mates.Clear();
		Version++;
		MatesVersion++;
		return true;
	}

	protected bool ReadInts(ParamsReadContext ctx, array<int> into)
	{
		int n;
		if (!ctx.Read(n))
			return false;
		into.Clear();
		for (int i = 0; i < n; i++)
		{
			int v;
			if (!ctx.Read(v))
				return false;
			into.Insert(v);
		}
		return true;
	}

	protected void ReadConf(ParamsReadContext ctx)
	{
		if (!Limits.Read(ctx) || !ReadInts(ctx, HiddenKeys) || !ReadInts(ctx, D3Keys) || !ReadInts(ctx, HiddenCats))
			return;
		Loaded = true;
		Version++;
	}

	protected void ReadScope(ParamsReadContext ctx)
	{
		int scope;
		int cats;
		int n;
		int i;
		if (!ctx.Read(scope) || !ctx.Read(cats))
			return;
		array<ref VB_MapCat> newCats = new array<ref VB_MapCat>();
		for (i = 0; i < cats; i++)
		{
			VB_MapCat c = new VB_MapCat();
			if (!c.Read(ctx))
				return;
			newCats.Insert(c);
		}
		if (!ctx.Read(n))
			return;
		array<ref VB_MapObject> list = new array<ref VB_MapObject>();
		for (i = 0; i < n; i++)
		{
			VB_MapObject o = new VB_MapObject();
			if (!o.Read(ctx))
				return;
			list.Insert(o);
		}
		// Новый объект, который игрок ещё правит, остаётся: сервер о нём не знает.
		for (i = Objects.Count() - 1; i >= 0; i--)
		{
			VB_MapObject old = Objects.Get(i);
			if (old.Scope == scope && (old.ObjId > 0 || old.Sent))
				Objects.RemoveOrdered(i);
		}
		foreach (VB_MapObject item : list)
			Objects.Insert(item);
		if (scope == VB_MapScope.CLAN)
			Cats = newCats;
		Version++;
	}

	protected void ReadMates(ParamsReadContext ctx)
	{
		int n;
		if (!ctx.Read(n))
			return;
		array<ref VB_MapMate> list = new array<ref VB_MapMate>();
		for (int i = 0; i < n; i++)
		{
			VB_MapMate m = new VB_MapMate();
			if (!m.Read(ctx))
				return;
			list.Insert(m);
		}
		Mates = list;
		MatesVersion++;
	}

	VB_MapObject Find(int key)
	{
		foreach (VB_MapObject o : Objects)
		{
			if (o.ObjId > 0 && o.ObjKey() == key)
				return o;
		}
		return null;
	}

	VB_MapCat FindCat(int id)
	{
		foreach (VB_MapCat c : Cats)
		{
			if (c.CatId == id)
				return c;
		}
		if (Cats.Count() > 0)
			return Cats.Get(0);
		return null;
	}

	int ModeOf(VB_MapObject o)
	{
		int key = o.ObjKey();
		if (HiddenKeys.Find(key) >= 0)
			return VB_MapMode.HIDDEN;
		if (o.Kind == VB_MapKind.MARK && D3Keys.Find(key) >= 0)
			return VB_MapMode.WORLD;
		return VB_MapMode.FLAT;
	}

	void SetMode(VB_MapObject o, int mode)
	{
		int key = o.ObjKey();
		HiddenKeys.RemoveItem(key);
		D3Keys.RemoveItem(key);
		if (mode == VB_MapMode.HIDDEN)
			HiddenKeys.Insert(key);
		else if (mode == VB_MapMode.WORLD)
			D3Keys.Insert(key);
		Version++;
	}

	bool CatHidden(int id)
	{
		return HiddenCats.Find(id) >= 0;
	}

	bool InLayer(VB_MapObject o)
	{
		if (o.Scope == VB_MapScope.SERVER)
			return LayerServer;
		if (o.Scope == VB_MapScope.ME)
			return LayerMine;
		if (!LayerClan)
			return false;
		VB_MapCat c = FindCat(o.CatId);
		return !c || !CatHidden(c.CatId);
	}

	bool IsVisible(VB_MapObject o)
	{
		return InLayer(o) && ModeOf(o) != VB_MapMode.HIDDEN;
	}

	// Сколько 3D меток включено: удалённые объекты не считаются.
	int CountD3()
	{
		int n = 0;
		foreach (VB_MapObject o : Objects)
		{
			if (o.ObjId > 0 && ModeOf(o) == VB_MapMode.WORLD)
				n++;
		}
		return n;
	}

	// Сколько объектов занято в счётчике лимита.
	int Used(int scope, int kind)
	{
		int n = 0;
		foreach (VB_MapObject o : Objects)
		{
			if (o.Scope != scope)
				continue;
			if (SameLimit(o.Kind, kind))
				n++;
		}
		return n;
	}

	static bool SameLimit(int a, int b)
	{
		if (a == b)
			return true;
		bool za = a == VB_MapKind.ZONE || a == VB_MapKind.CIRCLE;
		bool zb = b == VB_MapKind.ZONE || b == VB_MapKind.CIRCLE;
		return za && zb;
	}
}
