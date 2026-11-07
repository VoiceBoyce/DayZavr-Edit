// 3D метки: значок, название и расстояние поверх мира. Рисуются без открытия планшета, клавиша K скрывает их.
// Режим 3D у каждой метки включается глазком на карте, настройка своя у каждого игрока.
class VB_TabletMark3D extends Managed
{
	Widget Root;
	TextWidget DistText;
	vector Spot;
	float WX;
	float WZ;
	int Shown = -1;
}

class VB_TabletMarks3D
{
	static const float ASK_GAP_SEC = 5.0;
	static const int ASK_TRIES = 12;
	static const float SPOT_HEIGHT = 1.5;
	static const float EDGE = 60;

	protected static ref VB_TabletMarks3D s_Instance;

	protected Widget m_Layer;
	protected ref array<ref VB_TabletMark3D> m_Items = new array<ref VB_TabletMark3D>();
	protected int m_Version = -1;
	protected float m_AskIn;
	protected int m_Tries;

	static VB_TabletMarks3D Get()
	{
		if (!s_Instance)
			s_Instance = new VB_TabletMarks3D();
		return s_Instance;
	}

	static void Reset()
	{
		if (s_Instance)
			s_Instance.Drop();
		s_Instance = null;
	}

	protected void Drop()
	{
		m_Items.Clear();
		if (m_Layer)
			m_Layer.Unlink();
		m_Layer = null;
	}

	void Update(float dt)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_MapState state = client.Map;
		// Метки нужны и тем, кто планшет ещё не открывал.
		if (!state.Loaded)
		{
			m_AskIn = m_AskIn - dt;
			if (m_AskIn <= 0 && m_Tries < ASK_TRIES)
			{
				m_AskIn = ASK_GAP_SEC;
				m_Tries++;
				client.Request(VB_TabletOp.MAP_GET);
			}
			return;
		}
		if (m_Version != state.Version)
		{
			m_Version = state.Version;
			Rebuild(state);
		}
		bool show = !client.Hide3D && state.Limits.Allow3D && m_Items.Count() > 0;
		if (show && (!player.IsAlive() || player.IsUnconscious()))
			show = false;
		if (show && GetGame().GetUIManager().GetMenu())
			show = false;
		if (!m_Layer)
			return;
		if (m_Layer.IsVisible() != show)
			m_Layer.Show(show);
		if (show)
			Paint(player);
	}

	protected void Rebuild(VB_MapState state)
	{
		if (!m_Layer)
		{
			m_Layer = INT_UIF_Draw.Frame(null, 0, 0, 1, 1, "VBT_Marks3D");
			if (!m_Layer)
				return;
			m_Layer.SetSort(1);
		}
		m_Layer.SetPos(0, 0);
		m_Layer.SetSize(INT_UIF_Scale.ScreenW(), INT_UIF_Scale.ScreenH());
		INT_UIF_Widgets.ClearChildren(m_Layer);
		m_Items.Clear();
		foreach (VB_MapObject o : state.Objects)
		{
			if (o.ObjId > 0 && state.ModeOf(o) == VB_MapMode.WORLD && state.IsVisible(o))
				Add(o);
		}
	}

	protected void Add(VB_MapObject o)
	{
		VB_TabletMark3D item = new VB_TabletMark3D();
		item.Root = INT_UIF_Draw.Frame(m_Layer, 0, 0, 1, 1);
		item.WX = o.PX(0);
		item.WZ = o.PZ(0);
		item.Spot = Vector(item.WX, GetGame().SurfaceY(item.WX, item.WZ) + SPOT_HEIGHT, item.WZ);
		VBT_MapUi.Dot(item.Root, 30, 0xCC0B0E0F);
		VBT_MapUi.Dot(item.Root, 26, o.Color);
		VBT_MapUi.Glyph(item.Root, 18, VB_MapIcons.MarkName(o.IconIdx), 0xFF0E0D0C);
		string title = VBT.T(o.Title);
		float w = Math.Max(INT_UIF_Draw.TextWidth(title, 13), 56) + 16;
		INT_UIF_Surface.Box(item.Root, -w * 0.5, 18, w, 40, 4, 0x9E0B0E0F);
		INT_UIF_Draw.Text(item.Root, -w * 0.5, 19, w, 20, title, 13, VB_C_TEXT, INT_UIF_Align.CENTER);
		item.DistText = INT_UIF_Draw.Text(item.Root, -w * 0.5, 37, w, 18, "", 12, VB_C_DIM, INT_UIF_Align.CENTER);
		m_Items.Insert(item);
	}

	protected void Paint(PlayerBase player)
	{
		vector me = player.GetPosition();
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		foreach (VB_TabletMark3D item : m_Items)
		{
			vector s = GetGame().GetScreenPos(item.Spot);
			bool seen = s[2] > 0 && s[0] > -EDGE && s[0] < sw + EDGE && s[1] > -EDGE && s[1] < sh + EDGE;
			item.Root.Show(seen);
			if (!seen)
				continue;
			item.Root.SetPos(Math.Round(s[0]), Math.Round(s[1]));
			float dx = item.WX - me[0];
			float dz = item.WZ - me[2];
			int d = Math.Round(Math.Sqrt(dx * dx + dz * dz));
			if (d != item.Shown)
			{
				item.Shown = d;
				item.DistText.SetText(VBT_MapUi.Dist(d));
			}
		}
	}
}
