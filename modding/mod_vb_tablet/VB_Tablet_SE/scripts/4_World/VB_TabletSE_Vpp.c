// Серверные метки карты VPP (VPPMapConfig.json) в серверном слое карты планшета.
// Карта INT_VppMap держит файл в $profile:INT_Mods/INT_VppMap/ и один раз переносит туда старый $profile:VPPMapConfig.json;
// старое место читается, только если нового файла нет (сервер без INT_VppMap или перенос не удался).
// Файл VPP только читается: из планшета эти метки не правятся и не удаляются, номера от VB_MAP_VPP_ID.
// Номер это номер метки в VPP (INT_SrvId) минус 1: он не меняется, когда администратор карты правит метки из игры,
// поэтому скрытие и 3D у игрока переживают и рестарт, и правки. В файле без номеров номер раздаётся по порядку, как это
// делает сама карта, и совпадает с прежней позицией в файле.
// Поля названы как в файле VPP: JsonFileLoader сопоставляет по именам.
class VB_VppMarker
{
	string M_MARKER_NAME;
	string M_ICON_PATH;
	vector M_COLOR;
	vector M_POSITION;
	bool M_ISACTIVE;
	bool M_IS_3D_ACTIVE;
	int INT_SrvId;
}

class VB_VppConfig
{
	ref array<ref VB_VppMarker> M_STATIC_MARKER_ARRAY;
}

class VB_TabletVpp
{
	static const string FILE = "$profile:INT_Mods/INT_VppMap/VPPMapConfig.json";
	static const string FILE_OLD = "$profile:VPPMapConfig.json";
	static const int MAX = 300;
	static const int RELOAD_MS = 300000;

	protected static ref array<ref VB_MapObject> s_Marks;
	protected static int s_At;

	// Файл перечитывается не чаще раза в пять минут: администратор карты мог поменять метки без рестарта.
	static array<ref VB_MapObject> Marks()
	{
		int now = GetGame().GetTime();
		if (s_Marks && now - s_At < RELOAD_MS)
			return s_Marks;
		s_At = now;
		s_Marks = Load();
		return s_Marks;
	}

	static bool Has(int objId)
	{
		foreach (VB_MapObject o : Marks())
		{
			if (o.ObjId == objId)
				return true;
		}
		return false;
	}

	protected static string PathOf()
	{
		if (FileExist(FILE))
			return FILE;
		return FILE_OLD;
	}

	// Номера как у карты: свои > 0 и неповторяющиеся остаются, остальные получают max + 1, max + 2 по порядку файла.
	protected static void AssignIds(array<ref VB_VppMarker> list)
	{
		map<int, bool> used = new map<int, bool>();
		int maxId = 0;
		foreach (VB_VppMarker a : list)
		{
			if (!a)
				continue;
			if (a.INT_SrvId <= 0 || used.Contains(a.INT_SrvId))
			{
				a.INT_SrvId = 0;
				continue;
			}
			used.Set(a.INT_SrvId, true);
			if (a.INT_SrvId > maxId)
				maxId = a.INT_SrvId;
		}
		foreach (VB_VppMarker b : list)
		{
			if (!b || b.INT_SrvId > 0)
				continue;
			maxId++;
			b.INT_SrvId = maxId;
		}
	}

	protected static array<ref VB_MapObject> Load()
	{
		array<ref VB_MapObject> list = new array<ref VB_MapObject>();
		string path = PathOf();
		if (!FileExist(path))
			return list;
		VB_VppConfig cfg = new VB_VppConfig();
		string error;
		if (!JsonFileLoader<VB_VppConfig>.LoadFile(path, cfg, error) || !cfg.M_STATIC_MARKER_ARRAY)
		{
			Print(VB_TABLET_TAG + " не прочитан " + path + ": " + error);
			return list;
		}
		AssignIds(cfg.M_STATIC_MARKER_ARRAY);
		foreach (VB_VppMarker m : cfg.M_STATIC_MARKER_ARRAY)
		{
			if (list.Count() >= MAX)
				break;
			if (!m || !m.M_ISACTIVE)
				continue;
			VB_MapObject o = new VB_MapObject();
			o.ObjId = VB_MAP_VPP_ID + m.INT_SrvId - 1;
			o.Scope = VB_MapScope.SERVER;
			o.Kind = VB_MapKind.MARK;
			o.IconIdx = VB_MapIcons.VppMark(m.M_ICON_PATH);
			o.Title = TitleOf(m.M_MARKER_NAME, VB_MapIcons.VppGroup(m.M_ICON_PATH));
			o.Color = VB_MapIcons.VppColor(m.M_COLOR);
			o.Author = "";
			vector p = m.M_POSITION;
			o.AddPoint(p[0], p[2]);
			list.Insert(o);
		}
		Print(VB_TABLET_TAG + " метки VPP на карте планшета: " + list.Count().ToString() + " (" + path + ")");
		return list;
	}

	// Без названия метка подписывается видом: «Медпункт», «Вышка».
	protected static string TitleOf(string name, string group)
	{
		string t = name + "";
		t.TrimInPlace();
		if (t == "")
			return "#STR_VB_TAB_VPP_" + group;
		if (t.LengthUtf8() > VB_MAP_TITLE_MAX)
			t = t.SubstringUtf8(0, VB_MAP_TITLE_MAX);
		return t;
	}
}
