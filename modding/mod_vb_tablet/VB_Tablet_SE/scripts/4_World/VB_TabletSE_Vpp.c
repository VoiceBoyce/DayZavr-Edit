// Серверные метки карты VanillaPlusPlusMap ($profile:VPPMapConfig.json) в серверном слое карты планшета.
// Файл VPP только читается: из планшета эти метки не правятся и не удаляются, номера от VB_MAP_VPP_ID.
// Номер это позиция в файле VPP, поэтому скрытие и 3D у игрока переживают рестарт.
// Поля названы как в файле VPP: JsonFileLoader сопоставляет по именам.
class VB_VppMarker
{
	string M_MARKER_NAME;
	string M_ICON_PATH;
	vector M_COLOR;
	vector M_POSITION;
	bool M_ISACTIVE;
	bool M_IS_3D_ACTIVE;
}

class VB_VppConfig
{
	ref array<ref VB_VppMarker> M_STATIC_MARKER_ARRAY;
}

class VB_TabletVpp
{
	static const string FILE = "$profile:VPPMapConfig.json";
	static const int MAX = 300;
	static const int RELOAD_MS = 300000;

	protected static ref array<ref VB_MapObject> s_Marks;
	protected static int s_At;

	// Файл перечитывается не чаще раза в пять минут: администратор VPP мог поменять метки без рестарта.
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

	protected static array<ref VB_MapObject> Load()
	{
		array<ref VB_MapObject> list = new array<ref VB_MapObject>();
		if (!FileExist(FILE))
			return list;
		VB_VppConfig cfg = new VB_VppConfig();
		string error;
		if (!JsonFileLoader<VB_VppConfig>.LoadFile(FILE, cfg, error) || !cfg.M_STATIC_MARKER_ARRAY)
		{
			Print(VB_TABLET_TAG + " не прочитан VPPMapConfig.json: " + error);
			return list;
		}
		int n = cfg.M_STATIC_MARKER_ARRAY.Count();
		for (int i = 0; i < n && list.Count() < MAX; i++)
		{
			VB_VppMarker m = cfg.M_STATIC_MARKER_ARRAY.Get(i);
			if (!m || !m.M_ISACTIVE)
				continue;
			string group = VB_MapIcons.VppGroup(m.M_ICON_PATH);
			VB_MapObject o = new VB_MapObject();
			o.ObjId = VB_MAP_VPP_ID + i;
			o.Scope = VB_MapScope.SERVER;
			o.Kind = VB_MapKind.MARK;
			o.IconIdx = VB_MapIcons.VppIcon(group);
			o.Title = TitleOf(m.M_MARKER_NAME, group);
			o.Color = VB_MapIcons.VppColor(m.M_COLOR);
			o.Author = "";
			vector p = m.M_POSITION;
			o.AddPoint(p[0], p[2]);
			list.Insert(o);
		}
		Print(VB_TABLET_TAG + " метки VPP на карте планшета: " + list.Count().ToString());
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
