// Объекты карты планшета на большой карте VanillaPlusPlusMap (клавиша M): метки, зоны, круги и маршруты,
// банкомат клана и соклановцы в сети. Показывается то же, что и в планшете: слои и скрытие объектов игрока.
// Слой живёт внутри MapWidget меню VPP, как слой друзей INT_PartyMode: типов VPP в коде нет, меню узнаётся по имени
// класса, карта по имени виджета. Каждому виджету слоя сортировка 300, иначе карта рисуется поверх; виджеты не ловят
// мышь, поэтому двойной клик VPP по карте работает как раньше. Метки VPP, которые планшет показывает у себя, здесь
// пропускаются: на этой карте они уже есть.
class VBT_VppPin
{
	Widget Root;
	Widget Tag;
	float WX;
	float WZ;
	float Size;
	float TagW;
	bool Mate;		// соклановец: эти метки двигаются и обновляются отдельно от объектов
	string Label;
}

class VB_TabletVppLayer
{
	static const int SORT = 300;
	static const string MAP_NAME = "Map_Widget";
	static const string LAYOUT_CANVAS = "VB_Tablet/gui/layouts/vb_tablet_canvas.layout";
	static const float MATES_GAP = 3;
	static const float DATA_GAP = 10;
	static const int INK_DARK = 0xFF0E0D0C;
	static const int RING = 0xCC0B0E0F;
	static const int TAG_BG = 0x9E0B0E0F;

	protected static ref VB_TabletVppLayer s_Inst;

	protected UIScriptedMenu m_MenuSeen;
	protected MapWidget m_Map;
	protected Widget m_Host;
	protected CanvasWidget m_Canvas;
	protected ref array<ref VBT_VppPin> m_Pins = new array<ref VBT_VppPin>();
	protected int m_SeenVer = -1;
	protected int m_SeenMates = -1;
	protected float m_MateAsk;
	protected float m_DataAsk;
	protected float m_LastScale = -1;
	protected vector m_LastPos;
	protected float m_MX = -1;
	protected float m_MY = -1;
	protected float m_MW = -1;
	protected float m_MH = -1;
	protected float m_F = 1;
	protected ref array<float> m_X = new array<float>();
	protected ref array<float> m_Y = new array<float>();

	static VB_TabletVppLayer Get()
	{
		if (!s_Inst)
			s_Inst = new VB_TabletVppLayer();
		return s_Inst;
	}

	static void Reset()
	{
		if (s_Inst)
			s_Inst.Unbind();
		s_Inst = null;
	}

	void Tick(float dt)
	{
		MapWidget found = FindMap();
		if (found != m_Map || (found && !m_Host))
		{
			Unbind();
			if (found)
				Bind(found);
		}
		if (!m_Map || !m_Host)
		{
			if (m_Map || m_Host)
				Unbind();
			return;
		}
		// VPP прячет карту под окном правки своей метки.
		if (!m_Map.IsVisibleHierarchy())
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_MapState st = client.Map;
		if (!st.Loaded)
		{
			m_DataAsk = m_DataAsk - dt;
			if (m_DataAsk <= 0)
			{
				m_DataAsk = DATA_GAP;
				client.Request(VB_TabletOp.MAP_GET);
			}
		}
		if (client.Clan && st.LayerMates)
		{
			m_MateAsk = m_MateAsk - dt;
			if (m_MateAsk <= 0)
			{
				m_MateAsk = MATES_GAP;
				client.Request(VB_TabletOp.MAP_MATES);
			}
		}
		// Объекты пересобираются только при смене данных карты. Соклановцы приходят каждые 3 с:
		// у них двигаются метки, а пересоздаются, только если сменился состав.
		bool content = st.Version != m_SeenVer;
		bool mates = st.MatesVersion != m_SeenMates;
		if (content)
		{
			m_SeenVer = st.Version;
			BuildPins(client, st);
		}
		if (content || mates)
		{
			m_SeenMates = st.MatesVersion;
			BuildMatePins(client, st);
		}
		bool view = ViewChanged();
		if (content || mates || view)
			Place(st);
	}

	// Верхнее меню это карта VPP: её MapWidget. Поиск по дереву только при смене меню.
	protected MapWidget FindMap()
	{
		UIManager um = GetGame().GetUIManager();
		UIScriptedMenu top = null;
		if (um)
			top = um.GetMenu();
		if (!top || top.ClassName() != VB_TabletBridge.VPP_MENU)
		{
			m_MenuSeen = null;
			return null;
		}
		if (top == m_MenuSeen && m_Map)
			return m_Map;
		m_MenuSeen = top;
		Widget root = top.GetLayoutRoot();
		if (!root)
			return null;
		return MapWidget.Cast(root.FindAnyWidget(MAP_NAME));
	}

	protected void Bind(MapWidget target)
	{
		m_Host = INT_UIF_Draw.Clip(target, 0, 0, 10, 10, "VBT_VppHost");
		if (!m_Host)
			return;
		m_Map = target;
		m_Canvas = CanvasWidget.Cast(GetGame().GetWorkspace().CreateWidgets(LAYOUT_CANVAS, m_Host));
		m_Pins.Clear();
		m_SeenVer = -1;
		m_SeenMates = -1;
		m_MateAsk = 0;
		m_DataAsk = 0;
		m_LastScale = -1;
		m_MX = -1;
		m_MY = -1;
		m_MW = -1;
		m_MH = -1;
		SortAll(m_Host);
	}

	protected void Unbind()
	{
		// Карта VPP при открытии показывает все свои 3D метки: если игрок прятал их клавишей K, прячем снова.
		if (m_Map && VB_TabletClient.Get().Hide3D)
			VB_TabletBridge.SyncVpp3D();
		if (m_Host)
			m_Host.Unlink();
		m_Host = null;
		m_Map = null;
		m_Canvas = null;
		m_Pins.Clear();
	}

	// SetSort ставит виджет в конец соседей: дети собираются заранее и сортируются в порядке создания.
	static void SortAll(Widget w)
	{
		if (!w)
			return;
		w.SetSort(SORT);
		array<Widget> kids = new array<Widget>();
		Widget c = w.GetChildren();
		while (c)
		{
			kids.Insert(c);
			c = c.GetSibling();
		}
		foreach (Widget k : kids)
			SortAll(k);
	}

	// Карта VPP немного «дрожит» каждый кадр, поэтому сравнение с допуском.
	protected bool ViewChanged()
	{
		float scale = m_Map.GetScale();
		vector center = m_Map.GetMapPos();
		float x;
		float y;
		float w;
		float h;
		m_Map.GetScreenPos(x, y);
		m_Map.GetScreenSize(w, h);
		bool same = Math.AbsFloat(scale - m_LastScale) < 0.00001;
		if (same && vector.DistanceSq(center, m_LastPos) > 0.0001)
			same = false;
		if (same && (Math.AbsFloat(x - m_MX) > 0.5 || Math.AbsFloat(y - m_MY) > 0.5))
			same = false;
		if (same && (Math.AbsFloat(w - m_MW) > 0.5 || Math.AbsFloat(h - m_MH) > 0.5))
			same = false;
		if (same)
			return false;
		bool resized = Math.AbsFloat(w - m_MW) > 0.5 || Math.AbsFloat(h - m_MH) > 0.5;
		m_LastScale = scale;
		m_LastPos = center;
		m_MX = x;
		m_MY = y;
		m_MW = w;
		m_MH = h;
		if (resized)
		{
			m_Host.SetPos(0, 0);
			m_Host.SetSize(w, h);
			if (m_Canvas)
			{
				m_Canvas.SetPos(0, 0);
				m_Canvas.SetSize(w, h);
			}
		}
		return true;
	}

	// --- метки -------------------------------------------------------------------------------
	protected void BuildPins(VB_TabletClient client, VB_MapState st)
	{
		m_F = INT_UIF_Scale.Factor();
		for (int i = m_Pins.Count() - 1; i >= 0; i--)
		{
			VBT_VppPin old = m_Pins.Get(i);
			if (old.Mate)
				continue;
			if (old.Root)
				old.Root.Unlink();
			m_Pins.Remove(i);
		}
		foreach (VB_MapObject o : st.Objects)
		{
			if (o.IsVpp() || !st.IsVisible(o))
				continue;
			float ax;
			float az;
			o.AnchorOf(ax, az);
			string title = VBT.T(o.Title);
			if (o.Kind == VB_MapKind.MARK)
				AddPin(ax, az, o.Color, VB_MapIcons.MarkName(o.IconIdx), title, 22);
			else
				AddPin(ax, az, 0, "", title, 0);
		}
	}

	// Соклановцы: тот же состав в том же порядке значит только сдвиг меток, без пересоздания виджетов.
	protected void BuildMatePins(VB_TabletClient client, VB_MapState st)
	{
		m_F = INT_UIF_Scale.Factor();
		array<ref VB_MapMate> want = new array<ref VB_MapMate>();
		if (client.Clan && st.LayerMates)
		{
			foreach (VB_MapMate m : st.Mates)
			{
				if (!PartyDrawsMate(m.Who))
					want.Insert(m);
			}
		}
		array<ref VBT_VppPin> have = new array<ref VBT_VppPin>();
		foreach (VBT_VppPin p : m_Pins)
		{
			if (p.Mate)
				have.Insert(p);
		}
		bool same = have.Count() == want.Count();
		for (int k = 0; same && k < want.Count(); k++)
		{
			if (have.Get(k).Label != want.Get(k).Who)
				same = false;
		}
		if (same)
		{
			for (int j = 0; j < want.Count(); j++)
			{
				have.Get(j).WX = want.Get(j).X;
				have.Get(j).WZ = want.Get(j).Z;
			}
			return;
		}
		for (int i = m_Pins.Count() - 1; i >= 0; i--)
		{
			VBT_VppPin old = m_Pins.Get(i);
			if (!old.Mate)
				continue;
			if (old.Root)
				old.Root.Unlink();
			m_Pins.Remove(i);
		}
		foreach (VB_MapMate add : want)
		{
			VBT_VppPin pin = AddPin(add.X, add.Z, VB_C_OK, "", add.Who, 12);
			pin.Mate = true;
		}
	}

	// Пати сама рисует на этой карте друзей, которых игрок включил в показ: второй отметки не нужно.
	protected bool PartyDrawsMate(string who)
	{
#ifdef INT_PARTYMODE_CL
		if (!INT_PTY_Client.s_Friends)
			return false;
		foreach (INT_PTY_Friend fr : INT_PTY_Client.s_Friends)
		{
			if (fr && fr.Online && fr.Nick == who && INT_PTY_Client.IsShown(fr.Id))
				return true;
		}
#endif
		return false;
	}

	protected VBT_VppPin AddPin(float wx, float wz, int fill, string icon, string label, float size)
	{
		VBT_VppPin pin = new VBT_VppPin();
		pin.WX = wx;
		pin.WZ = wz;
		pin.Size = size;
		pin.Label = label;
		pin.Root = INT_UIF_Draw.Frame(m_Host, 0, 0, 1, 1);
		if (fill != 0)
		{
			VBT_MapUi.Dot(pin.Root, size + 4, RING);
			VBT_MapUi.Dot(pin.Root, size, fill);
			if (icon != "")
				VBT_MapUi.Glyph(pin.Root, 14, icon, INK_DARK);
		}
		if (label != "")
		{
			pin.TagW = INT_UIF_Draw.TextWidth(label, 12) + 12;
			pin.Tag = INT_UIF_Draw.Frame(pin.Root, 0, 0, pin.TagW, 20);
			INT_UIF_Surface.Box(pin.Tag, 0, 0, pin.TagW, 20, 4, TAG_BG);
			INT_UIF_Draw.Text(pin.Tag, 6, 0, pin.TagW - 6, 20, label, 12, VB_C_TEXT);
		}
		SortAll(pin.Root);
		m_Pins.Insert(pin);
		return pin;
	}

	// Подпись справа от метки, у правого края карты слева. Подпись зоны стоит по центру точки.
	protected void Place(VB_MapState st)
	{
		float f = m_F;
		float pad = 30 * f;
		foreach (VBT_VppPin pin : m_Pins)
		{
			vector sp = m_Map.MapToScreen(Vector(pin.WX, 0, pin.WZ));
			float lx = sp[0] - m_MX;
			float ly = sp[1] - m_MY;
			bool on = lx > -pad && lx < m_MW + pad && ly > -pad && ly < m_MH + pad;
			pin.Root.Show(on);
			if (!on)
				continue;
			pin.Root.SetPos(Math.Round(lx), Math.Round(ly));
			if (!pin.Tag)
				continue;
			float dx = -pin.TagW * 0.5;
			if (pin.Size > 0)
			{
				dx = pin.Size * 0.5 + 4;
				if (lx + (dx + pin.TagW) * f > m_MW)
					dx = -pin.Size * 0.5 - 4 - pin.TagW;
			}
			pin.Tag.SetPos(dx * f, -10 * f);
		}
		PaintShapes(st);
	}

	// --- зоны, круги, маршруты -----------------------------------------------------------------
	protected void PaintShapes(VB_MapState st)
	{
		if (!m_Canvas)
			return;
		m_Canvas.Clear();
		vector p0 = m_Map.MapToScreen(Vector(0, 0, 0));
		vector p1 = m_Map.MapToScreen(Vector(1000, 0, 0));
		float k = (p1[0] - p0[0]) / 1000;
		if (k <= 0)
			return;
		foreach (VB_MapObject o : st.Objects)
		{
			if (o.Kind == VB_MapKind.MARK || o.IsVpp() || !st.IsVisible(o))
				continue;
			if (o.Kind == VB_MapKind.CIRCLE)
			{
				Circle(o.PX(0), o.PZ(0), o.Radius * k);
				Lines(false, 2, o.Color);
			}
			else
			{
				Points(o);
				if (o.Kind == VB_MapKind.ZONE)
				{
					Lines(true, 2, o.Color);
				}
				else
				{
					Lines(false, 5, 0x8C0E0D0C);
					Lines(false, 3, o.Color);
				}
			}
		}
	}

	protected void Points(VB_MapObject o)
	{
		m_X.Clear();
		m_Y.Clear();
		for (int i = 0; i < o.PointCount(); i++)
		{
			vector sp = m_Map.MapToScreen(Vector(o.PX(i), 0, o.PZ(i)));
			m_X.Insert(sp[0] - m_MX);
			m_Y.Insert(sp[1] - m_MY);
		}
	}

	protected void Circle(float wx, float wz, float r)
	{
		m_X.Clear();
		m_Y.Clear();
		vector sp = m_Map.MapToScreen(Vector(wx, 0, wz));
		float cx = sp[0] - m_MX;
		float cy = sp[1] - m_MY;
		int n = Math.Clamp(Math.Round(r / 6), 24, 96);
		for (int i = 0; i <= n; i++)
		{
			float a = Math.PI2 * i / n;
			m_X.Insert(cx + Math.Cos(a) * r);
			m_Y.Insert(cy + Math.Sin(a) * r);
		}
	}

	protected void Lines(bool closed, float w, int color)
	{
		int n = m_X.Count();
		for (int i = 1; i < n; i++)
			Seg(m_X.Get(i - 1), m_Y.Get(i - 1), m_X.Get(i), m_Y.Get(i), w, color);
		if (closed && n > 2)
			Seg(m_X.Get(n - 1), m_Y.Get(n - 1), m_X.Get(0), m_Y.Get(0), w, color);
	}

	protected bool ClipT(float p, float q, inout float t0, inout float t1)
	{
		if (p == 0)
			return q >= 0;
		float r = q / p;
		if (p < 0)
		{
			if (r > t1)
				return false;
			if (r > t0)
				t0 = r;
		}
		else
		{
			if (r < t0)
				return false;
			if (r < t1)
				t1 = r;
		}
		return true;
	}

	// Отрезок обрезается по карте: при сильном приближении точки уходят далеко за экран.
	protected void Seg(float x1, float y1, float x2, float y2, float w, int color)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		float t0 = 0;
		float t1 = 1;
		if (!ClipT(-dx, x1, t0, t1) || !ClipT(dx, m_MW - x1, t0, t1) || !ClipT(-dy, y1, t0, t1) || !ClipT(dy, m_MH - y1, t0, t1))
			return;
		if (t1 <= t0)
			return;
		m_Canvas.DrawLine(x1 + t0 * dx, y1 + t0 * dy, x1 + t1 * dx, y1 + t1 * dy, w * m_F, color);
	}
}
