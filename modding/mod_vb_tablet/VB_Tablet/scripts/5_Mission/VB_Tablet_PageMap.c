// Карта планшета: MapWidget на всю рабочую область, поверх него линии на CanvasWidget, метки, панели-стекло.
// Мышь над картой ловит прозрачная кнопка m_Catch: сдвиг и масштаб считает скрипт, поэтому карта не спорит
// с перетаскиванием точек. Всё хранится в мировых координатах, экран пересчитывается при сдвиге или масштабе.
class VBT_MapUi
{
	// Квадрат с центром точно в нулевой точке родителя: размер в пикселях экрана всегда чётный.
	static void Center(Widget w, float size)
	{
		int half = Math.Round(size * INT_UIF_Scale.Factor() * 0.5);
		int full = half * 2;
		w.SetSize(full, full);
		w.SetPos(-half, -half);
	}

	static ImageWidget Dot(Widget root, float size, int color)
	{
		ImageWidget img = INT_UIF_Draw.Image(root, 0, 0, size, size, "iuf_circle64", color);
		Center(img, size);
		return img;
	}

	static ImageWidget Glyph(Widget root, float size, string name, int color)
	{
		ImageWidget img = VBT.Ico(root, 0, 0, size, name, color);
		Center(img, size);
		return img;
	}

	// Метка из списка: цветной круг и тёмный значок строго по его центру. cx, cy это центр.
	static Widget Badge(Widget p, float cx, float cy, float size, int fill, string icon, float iconSize)
	{
		Widget f = INT_UIF_Draw.Frame(p, cx, cy, 1, 1);
		Dot(f, size, fill);
		if (icon != "")
			Glyph(f, iconSize, icon, 0xFF0E0D0C);
		return f;
	}

	static string Fix1(float v)
	{
		int t = Math.Round(v * 10);
		int whole = t / 10;
		int frac = t % 10;
		return whole.ToString() + "," + frac.ToString();
	}

	static string Fix2(float v)
	{
		int t = Math.Round(v * 100);
		int whole = t / 100;
		int frac = t % 100;
		string tail = frac.ToString();
		if (frac < 10)
			tail = "0" + tail;
		return whole.ToString() + "," + tail;
	}

	static string Dist(float meters)
	{
		int d = Math.Round(meters);
		if (d >= 1000)
			return Fix1(d / 1000.0) + " " + VBT.T("#STR_VB_TAB_KM");
		return d.ToString() + " " + VBT.T("#STR_VB_TAB_D_M");
	}

	static string Coords(float x, float z)
	{
		int ix = Math.Round(x);
		int iz = Math.Round(z);
		return "X " + ix.ToString() + "   Y " + iz.ToString();
	}

	static string KindName(int kind)
	{
		return "#STR_VB_TAB_D_KIND_" + kind.ToString();
	}

	static string KindIcon(int kind)
	{
		if (kind == VB_MapKind.ZONE)
			return "polygon";
		if (kind == VB_MapKind.CIRCLE)
			return "circleo";
		if (kind == VB_MapKind.ROUTE)
			return "route";
		return "pin";
	}
}

// Обычный обработчик меню плюс колесо: у меню его нет.
class VBT_MapHandler extends INT_UIF_MenuHandler
{
	protected VB_TabletPageMap m_Page;

	void SetPage(VB_TabletPageMap page)
	{
		m_Page = page;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (m_Page)
			return m_Page.OnWheel(w, wheel);
		return false;
	}
}

// Что делает кнопка.
enum VBT_MapAct
{
	NONE = 0,
	TOOL,
	DRAWER,
	LABELS,
	ZOOM_IN,
	ZOOM_OUT,
	CENTER_ME,
	DEST,
	UNDO,
	CANCEL,
	FINISH,
	CARD_CLOSE,
	CARD_EDIT,
	CARD_CENTER,
	CARD_DELETE,
	PICK_ICON,
	PICK_COLOR,
	PICK_SCOPE,
	VIS,
	SIDE_TAB,
	SIDE_LAYER,
	SIDE_CLOSE,
	CAT_NEW,
	ROW,
	CAT_ROW,
	CAT_EYE,
	CAT_EDIT,
	CHIP,
	MATE_ROW,
	EDIT_COLOR,
	EDIT_ICON,
	EDIT_SAVE,
	EDIT_CANCEL,
	EDIT_DELETE
}

// Кнопка панели: подсветка при наведении и подсказка. Клик разбирает страница по Act и Arg.
class VBT_MapBtn extends Managed
{
	Widget Hit;
	int Act;
	int Arg;
	int Sect;
	ref INT_UIF_Surface Bg;
	ref VBT_Button Btn;
	ImageWidget Img;
	TextWidget Txt;
	int Fill;
	int FillHot;
	int Ink;
	int InkHot;
	string Tip;
	float TipX;
	float TipY;
	Widget TipParent;

	void Hover(bool enter)
	{
		if (Btn)
			Btn.Hover(enter);
		if (Bg && Fill != FillHot)
		{
			if (enter)
				Bg.SetFill(FillHot);
			else
				Bg.SetFill(Fill);
		}
		int col = Ink;
		if (enter)
			col = InkHot;
		if (Img && Ink != InkHot)
			Img.SetColor(col);
		if (Txt && Ink != InkHot)
			Txt.SetColor(col);
	}
}

// Стекло поверх карты: при наведении плотнее. В игре это альфа заливки.
class VBT_MapPlate extends Managed
{
	float X;
	float Y;
	float W;
	float H;
	int Alpha;
	bool Hot;
	Widget Root;
	ref INT_UIF_Surface Bg;
}

// Метка или подпись на карте.
class VBT_MapPin extends Managed
{
	ref VB_MapObject Obj;	// у банкомата и соклановцев нет
	float WX;
	float WZ;
	float X;
	float Y;
	float Size;
	float TagW;
	Widget Root;
	Widget Tag;
	string FriendId;	// друг из INT_PartyMode: место обновляется без перерисовки
}

// Строка списка объектов.
class VBT_MapRow extends Managed
{
	ref VB_MapCat Cat;
	ref VB_MapObject Obj;
	int Count;
	float Ind;
}

class VB_TabletPageMap extends VB_TabletPage
{
	static const float VW = 1228;
	static const float VH = 776;
	static const int GLASS_HOT = 230;
	static const int WHITE_08 = 0x14FFFFFF;
	static const int WHITE_09 = 0x17FFFFFF;
	static const int WHITE_10 = 0x1AFFFFFF;
	static const int WHITE_12 = 0x1FFFFFFF;
	static const int WHITE_14 = 0x24FFFFFF;
	static const int WHITE_16 = 0x29FFFFFF;
	static const int INK_DARK = 0xFF0E0D0C;
	static const int HATCH_ALPHA = 217;

	// Разделы, которые перерисовываются отдельно в следующем кадре.
	static const int NEED_TOOLS = 1;
	static const int NEED_SIDE = 2;
	static const int NEED_MATES = 4;
	static const int NEED_DRAFT = 8;
	static const int NEED_CARD = 16;
	static const int NEED_PINS = 32;
	static const int NEED_SHAPES = 64;
	static const int NEED_EDIT = 128;

	static const int SECT_TOOLS = 1;
	static const int SECT_ZOOM = 2;
	static const int SECT_SIDE = 3;
	static const int SECT_MATES = 4;
	static const int SECT_DRAFT = 5;
	static const int SECT_CARD = 6;
	static const int SECT_EDIT = 7;
	static const int SECT_EDIT_PAL = 8;

	static const int TOOL_HAND = 0;
	static const int TOOL_MARK = 1;
	static const int TOOL_ZONE = 2;
	static const int TOOL_CIRCLE = 3;
	static const int TOOL_ROUTE = 4;

	static const int SIDE_NONE = -1;
	static const int SIDE_CLAN = 0;
	static const int SIDE_ME = 1;
	static const int SIDE_SERVER = 2;

	// Вид и настройки панели живут между заходами на экран.
	protected static bool s_HasView;
	protected static float s_ViewX;
	protected static float s_ViewZ;
	protected static float s_ViewS = 0.34;
	protected static bool s_Drawer;
	protected static bool s_Labels = true;
	protected static int s_Side = -1;
	protected static int s_Top;
	protected static int s_Dest = 0;
	protected static int s_CurCat;
	protected static ref array<int> s_OpenCats;

	protected VB_TabletClient m_Client;
	protected VB_MapState m_State;
	protected ref VBT_MapHandler m_Handler;
	protected MapWidget m_Map;
	protected CanvasWidget m_Canvas;
	protected Widget m_Base;
	protected Widget m_Over;	// слой поверх карты: на экране планшета, внутри рабочей области карта его перекрывает
	protected Widget m_Pins;
	protected Widget m_MeLayer;
	protected Widget m_HandleLayer;
	protected Widget m_Catch;
	protected Widget m_Ui;
	protected Widget m_Tip;
	protected ImageWidget m_ArrowOut;
	protected ImageWidget m_ArrowIn;
	protected ref array<ref VBT_MapBtn> m_Btns = new array<ref VBT_MapBtn>();
	protected ref array<ref VBT_MapPlate> m_Plates = new array<ref VBT_MapPlate>();
	protected ref array<ref VBT_MapPin> m_Items = new array<ref VBT_MapPin>();
	protected ref array<ref VBT_MapRow> m_Rows = new array<ref VBT_MapRow>();
	protected ref array<Widget> m_HandlePool = new array<Widget>();
	protected ref array<ImageWidget> m_HandleOut = new array<ImageWidget>();
	protected ref array<ImageWidget> m_HandleIn = new array<ImageWidget>();
	protected ref VBT_MapPlate m_RailPlate;
	protected ref VBT_MapPlate m_SidePlate;
	protected ref VBT_MapPlate m_DraftPlate;
	protected ref VBT_MapPlate m_CardPlate;
	protected ref VBT_MapPlate m_ChipPlate;
	protected ref VBT_MapPlate m_PopPlate;
	protected ref VBT_Switch m_MatesSwitch;
	protected ref VBT_Input m_NameField;
	protected ref VBT_Input m_NameFieldOld;
	protected ref VBT_Dropdown m_CatDrop;
	protected ref array<ref VB_MapCat> m_CatDropCats = new array<ref VB_MapCat>();
	protected TextWidget m_CoordsText;
	protected TextWidget m_ScaleText;
	protected ImageWidget m_ScaleBar;
	protected ref array<ImageWidget> m_TapeLines = new array<ImageWidget>();
	protected ref array<TextWidget> m_TapeTexts = new array<TextWidget>();
	protected int m_TapeHeading = -1000;
	protected string m_CoordsShown;

	// Проекция мир -> холст: пиксели макета, линейная.
	protected bool m_HasProj;
	protected float m_A0;
	protected float m_B0;
	protected float m_Kx;
	protected float m_Kz;
	protected float m_F = 1;
	protected int m_ViewFrames = 3;

	// Мышь.
	protected float m_MouseA;
	protected float m_MouseB;
	protected bool m_LeftWas;
	protected bool m_RightWas;
	protected bool m_Pressed;
	protected bool m_Pan;
	protected bool m_Moved;
	protected float m_PressA;
	protected float m_PressB;
	protected float m_PanWX;
	protected float m_PanWZ;
	protected int m_DragPoint = -1;
	protected bool m_DragNew;
	protected int m_ZoomFrames;
	protected float m_ZoomA;
	protected float m_ZoomB;
	protected float m_ZoomWX;
	protected float m_ZoomWZ;

	// Рисование.
	protected int m_Tool = TOOL_HAND;
	protected int m_DraftKind = -1;
	protected ref array<float> m_DraftPts = new array<float>();
	protected float m_DraftR;
	protected bool m_DraftHasCur;
	protected float m_DraftCurX;
	protected float m_DraftCurZ;

	// Выбранный объект.
	protected ref VB_MapObject m_Sel;
	protected int m_SelOldScope;
	protected bool m_IsNew;
	protected bool m_SelDirty;
	protected bool m_CardEdit;
	protected string m_DefaultTitle;
	protected bool m_FocusName;
	protected bool m_MatesOpen;
	protected int m_ChipCount = -1;
	protected bool m_SavePending;

	// Окно категории.
	protected ref VBT_Modal m_EditModal;
	protected ref VBT_Input m_EditField;
	protected ref VBT_Input m_EditFieldOld;
	protected Widget m_EditPal;
	protected int m_EditId;
	protected int m_EditColor;
	protected int m_EditIcon;

	protected int m_Need;
	protected int m_SeenVersion = -1;
	protected int m_SeenMates = -1;
	protected bool m_ShapesDirty = true;
	protected float m_MateAsk;
	protected float m_LabelsIn;
	protected bool m_LabelsDue;
	protected VBT_MapPin m_HotPin;
	protected string m_FriendSig;
	protected float m_FriendT;

	// Для расчёта штриховки: не создавать массивы на каждую зону.
	protected ref array<float> m_CutX = new array<float>();
	protected ref array<float> m_CutY = new array<float>();
	protected ref array<float> m_LineX = new array<float>();
	protected ref array<float> m_LineY = new array<float>();

	// --- сборка экрана -----------------------------------------------------------------------
	override void Build()
	{
		m_Client = VB_TabletClient.Get();
		m_State = m_Client.Map;
		m_F = INT_UIF_Scale.Factor();
		if (!s_OpenCats)
			s_OpenCats = new array<int>();
		m_Handler = new VBT_MapHandler();
		m_Handler.SetMenu(m_Menu);
		m_Handler.SetPage(this);

		// Страница стоит с отступом 28: карте нужна вся рабочая область.
		INT_UIF_Draw.Place(m_Root, 0, 0, VW, VH);
		m_Base = INT_UIF_Draw.Clip(m_Root, 0, 0, VW, VH, "VBT_MapBase");
		Widget holder = GetGame().GetWorkspace().CreateWidgets("VB_Tablet/gui/layouts/vb_tablet_map.layout", m_Base);
		if (holder)
		{
			INT_UIF_Draw.Place(holder, 0, 0, VW, VH);
			m_Map = MapWidget.Cast(holder.FindAnyWidget("VBT_Map"));
			m_Canvas = CanvasWidget.Cast(holder.FindAnyWidget("VBT_MapCanvas"));
		}
		if (!m_Map || !m_Canvas)
		{
			INT_UIF_Draw.Text(m_Root, 0, 340, 1172, 32, "#STR_VB_TAB_NO_DATA", 17, VB_C_DIM, INT_UIF_Align.CENTER);
			return;
		}
		INT_UIF_Draw.Place(m_Map, 0, 0, VW, VH);
		INT_UIF_Draw.Place(m_Canvas, 0, 0, VW, VH);
		// Всё поверх карты должно быть её дочерними элементами, иначе карта это перекрывает.
		m_Over = INT_UIF_Draw.Clip(m_Menu.GetScreen(), 240, 72, VW, VH, "VBT_MapOver");
		m_Over.SetSort(150);
		m_Over.AddChild(m_Canvas);
		m_Pins = INT_UIF_Draw.Frame(m_Over, 0, 0, VW, VH);
		m_HandleLayer = INT_UIF_Draw.Frame(m_Over, 0, 0, VW, VH);
		m_MeLayer = INT_UIF_Draw.Frame(m_Over, 0, 0, 1, 1);
		m_ArrowOut = VBT_MapUi.Glyph(m_MeLayer, 30, "arrow", INK_DARK);
		m_ArrowIn = VBT_MapUi.Glyph(m_MeLayer, 20, "arrow", VB_C_ACCENT);
		m_Catch = INT_UIF_Draw.Hit(m_Over, 0, 0, VW, VH, m_Handler);
		m_Catch.SetName("VBT_MapCatch");
		m_Ui = INT_UIF_Draw.Frame(m_Over, 0, 0, VW, VH);

		ApplyStartView();
		m_Client.Request(VB_TabletOp.MAP_GET);
		if (!m_Client.Clan)
			s_Dest = VB_MapScope.ME;
		if (!m_Client.Clan)
		{
			if (s_Side != SIDE_SERVER)
				s_Side = SIDE_ME;
		}
		else if (s_Side == SIDE_NONE)
		{
			s_Side = SIDE_CLAN;
		}
		BuildZoom();
		BuildCompass();
		m_Need = NEED_TOOLS | NEED_SIDE | NEED_MATES | NEED_DRAFT | NEED_PINS | NEED_SHAPES;
		m_SeenVersion = m_State.Version;
		m_SeenMates = m_State.MatesVersion;
		m_MateAsk = 0;
	}

	// Следующее открытие карты покажет эту точку.
	static void FocusOn(float x, float z)
	{
		s_ViewX = x;
		s_ViewZ = z;
		s_HasView = true;
	}

	protected void ApplyStartView()
	{
		if (!s_HasView)
		{
			vector me = GetGame().GetPlayer().GetPosition();
			s_ViewX = me[0] + 150;
			s_ViewZ = me[2] - 100;
			s_HasView = true;
		}
		m_Map.SetScale(s_ViewS);
		m_Map.SetMapPos(Vector(s_ViewX, 0, s_ViewZ));
	}

	override void Leave()
	{
		FlushSelection();
		if (GetFocus())
			SetFocus(null);
		ClearTip();
		// Окно категории лежит на экране планшета, а не на странице: без этого оно осталось бы сиротой.
		CloseCatEdit();
		if (m_Over)
		{
			m_Over.Show(false);
			m_Menu.Trash(m_Over);
			m_Over = null;
		}
	}

	// Esc сначала отменяет рисование и закрывает карточку, как в макете. Планшет закрывается следующим нажатием.
	bool HoldEscape()
	{
		return m_EditModal != null || m_DraftKind >= 0 || m_Tool != TOOL_HAND || m_Sel != null;
	}

	protected void Escape()
	{
		if (m_EditModal)
		{
			CloseCatEdit();
			return;
		}
		if (m_DraftKind >= 0 || m_Tool != TOOL_HAND)
		{
			StopDraft();
			return;
		}
		CloseSel();
	}

	// --- такт --------------------------------------------------------------------------------
	override void Tick(float dt)
	{
		if (!m_Map)
			return;
		m_NameFieldOld = null;
		m_EditFieldOld = null;
		if (m_ViewFrames > 0)
		{
			m_ViewFrames--;
			m_Map.SetScale(s_ViewS);
			m_Map.SetMapPos(Vector(s_ViewX, 0, s_ViewZ));
		}
		WatchData();
		Flush();
		bool moved = ReadView();
		if (!m_HasProj)
			return;
		ReadMouse();
		if (HoldEscape())
		{
			UAInput back = GetUApi().GetInputByID(UAUIBack);
			if (back && back.LocalPress())
				Escape();
		}
		PollButtons();
		if (m_Pan)
			PanStep();
		if (m_ZoomFrames > 0)
			ZoomStep();
		if (moved)
			PlaceAll();
		if (moved && m_ViewFrames == 0)
			KeepView();
		if (m_ShapesDirty)
			PaintShapes();
		PaintMe();
		TickFriends(dt);
		HoverPlates();
		HoverPin();
		ShowCoords();
		m_LabelsIn = m_LabelsIn - dt;
		if (m_LabelsDue && m_LabelsIn <= 0)
			Declutter();
		m_MateAsk = m_MateAsk - dt;
		if (m_MateAsk <= 0)
		{
			m_MateAsk = 3;
			if (m_Client.Clan)
				m_Client.Request(VB_TabletOp.MAP_MATES);
		}
	}

	// Новые данные: права и клан могли смениться. Текст в открытой форме не трогаем.
	override bool OnData()
	{
		int need = NEED_TOOLS | NEED_SIDE | NEED_MATES | NEED_DRAFT | NEED_PINS;
		if (!IsEditing())
			need = need | NEED_CARD;
		m_Need = m_Need | need;
		return true;
	}

	protected bool IsEditing()
	{
		return m_Sel && (m_CardEdit || m_IsNew);
	}

	override void OnResult(int code)
	{
		if (!m_SavePending)
			return;
		m_SavePending = false;
		if (!VBK.ResultIsGood(code))
			m_Client.Request(VB_TabletOp.MAP_GET);
	}

	protected void WatchData()
	{
		if (m_SeenVersion != m_State.Version)
		{
			// Сервер прислал область: сохранение принято (успех RESULT не шлёт), ждать ответа больше нечего.
			m_SavePending = false;
			m_SeenVersion = m_State.Version;
			Relink();
			m_Need = m_Need | NEED_PINS | NEED_SIDE | NEED_DRAFT | NEED_SHAPES;
			if (!IsEditing())
				m_Need = m_Need | NEED_CARD;
		}
		if (m_SeenMates != m_State.MatesVersion)
		{
			m_SeenMates = m_State.MatesVersion;
			MatesMoved();
		}
	}

	// Сервер прислал новый список: выбранный объект это теперь другой экземпляр.
	protected void Relink()
	{
		if (!m_Sel)
			return;
		if (m_Sel.ObjId == 0)
		{
			if (m_State.Objects.Find(m_Sel) < 0)
				ForgetSel();
			return;
		}
		VB_MapObject fresh = m_State.Find(m_SelOldScope * VB_MAP_KEY_STEP + m_Sel.ObjId);
		if (!fresh)
		{
			// Правка ещё не отправлена, а объект мог быть в другой области: ищем по текущей.
			if (m_State.Objects.Find(m_Sel) < 0)
				ForgetSel();
			return;
		}
		if (fresh == m_Sel)
			return;
		if (m_SelDirty)
		{
			int i = m_State.Objects.Find(fresh);
			m_State.Objects.Set(i, m_Sel);
			return;
		}
		m_Sel = fresh;
	}

	protected void ForgetSel()
	{
		m_Sel = null;
		m_IsNew = false;
		m_CardEdit = false;
		m_SelDirty = false;
		m_Need = m_Need | NEED_CARD | NEED_SHAPES;
	}

	// Панели перестраиваются здесь: кликнутый виджет к этому моменту уже не нужен.
	protected void Flush()
	{
		if (m_Need == 0)
			return;
		int need = m_Need;
		m_Need = 0;
		ClearTip();
		if (need & NEED_TOOLS)
			PaintTools();
		if (need & NEED_SIDE)
			PaintSide();
		if (need & NEED_MATES)
			PaintMates();
		if (need & NEED_DRAFT)
			PaintDraft();
		if (need & NEED_CARD)
			PaintCard();
		if (need & NEED_EDIT)
			PaintEditPalette();
		if (need & NEED_PINS)
		{
			PaintPins();
			m_LabelsDue = true;
			m_LabelsIn = 0;
		}
		if (need & NEED_SHAPES)
			m_ShapesDirty = true;
	}

	// --- вид ---------------------------------------------------------------------------------
	// Две опорные точки дают масштаб и сдвиг. true: картинка сдвинулась.
	protected bool ReadView()
	{
		vector p0 = m_Map.MapToScreen(Vector(0, 0, 0));
		vector p1 = m_Map.MapToScreen(Vector(1000, 0, 1000));
		float ox;
		float oy;
		m_Map.GetScreenPos(ox, oy);
		float a0 = (p0[0] - ox) / m_F;
		float b0 = (p0[1] - oy) / m_F;
		float kx = (p1[0] - p0[0]) / (1000 * m_F);
		float kz = (p1[1] - p0[1]) / (1000 * m_F);
		if (kx <= 0)
			return false;
		bool moved = !m_HasProj || Math.AbsFloat(a0 - m_A0) > 0.05 || Math.AbsFloat(b0 - m_B0) > 0.05 || Math.AbsFloat(kx - m_Kx) > 0.000001;
		m_A0 = a0;
		m_B0 = b0;
		m_Kx = kx;
		m_Kz = kz;
		m_HasProj = true;
		return moved;
	}

	protected void KeepView()
	{
		vector at = m_Map.GetMapPos();
		s_ViewX = at[0];
		s_ViewZ = at[2];
		s_ViewS = m_Map.GetScale();
	}

	protected float LocA(float x)
	{
		return m_A0 + x * m_Kx;
	}

	protected float LocB(float z)
	{
		return m_B0 + z * m_Kz;
	}

	protected float WorldX(float a)
	{
		return (a - m_A0) / m_Kx;
	}

	protected float WorldZ(float b)
	{
		return (b - m_B0) / m_Kz;
	}

	protected void ShiftMap(float dx, float dz)
	{
		vector at = m_Map.GetMapPos();
		m_Map.SetMapPos(Vector(at[0] + dx, 0, at[2] + dz));
	}

	protected void ZoomBy(float k, float a, float b)
	{
		m_ZoomA = a;
		m_ZoomB = b;
		m_ZoomWX = WorldX(a);
		m_ZoomWZ = WorldZ(b);
		m_ZoomFrames = 3;
		m_Map.SetScale(Math.Clamp(m_Map.GetScale() * k, 0.03, 1));
	}

	// Точка под курсором остаётся на месте. Шаг повторяется: проекция обновляется не сразу.
	protected void ZoomStep()
	{
		m_ZoomFrames--;
		float dx = m_ZoomWX - WorldX(m_ZoomA);
		float dz = m_ZoomWZ - WorldZ(m_ZoomB);
		if (Math.AbsFloat(dx) > 0.5 || Math.AbsFloat(dz) > 0.5)
			ShiftMap(dx, dz);
	}

	protected void PanStep()
	{
		if (!m_Moved && Math.AbsFloat(m_MouseA - m_PressA) + Math.AbsFloat(m_MouseB - m_PressB) > 3)
			m_Moved = true;
		if (!m_Moved)
			return;
		float dx = m_PanWX - WorldX(m_MouseA);
		float dz = m_PanWZ - WorldZ(m_MouseB);
		if (Math.AbsFloat(dx) > 0.05 || Math.AbsFloat(dz) > 0.05)
			ShiftMap(dx, dz);
	}

	protected void CenterOn(float x, float z)
	{
		m_Map.SetMapPos(Vector(x, 0, z));
	}

	// --- мышь --------------------------------------------------------------------------------
	protected void ReadMouse()
	{
		int mx;
		int my;
		GetMousePos(mx, my);
		float ox;
		float oy;
		m_Map.GetScreenPos(ox, oy);
		m_MouseA = (mx - ox) / m_F;
		m_MouseB = (my - oy) / m_F;
	}

	// Нажатия читаются опросом: отпускание за пределами карты событие не даёт.
	protected void PollButtons()
	{
		bool left = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		bool right = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) != 0;
		bool over = GetWidgetUnderCursor() == m_Catch;
		if (left && !m_LeftWas && over)
			Press();
		if (right && !m_RightWas && over && m_DraftKind >= 0)
			Undo();
		m_LeftWas = left;
		m_RightWas = right;
		if (m_Pressed && left)
			Drag();
		if (m_Pressed && !left)
			Release();
		if (!m_Pressed && m_DraftKind >= 0 && m_DraftKind != VB_MapKind.CIRCLE && over)
		{
			m_DraftHasCur = true;
			m_DraftCurX = WorldX(m_MouseA);
			m_DraftCurZ = WorldZ(m_MouseB);
			m_ShapesDirty = true;
		}
	}

	protected void Press()
	{
		if (GetFocus())
			SetFocus(null);
		m_Pressed = true;
		m_Moved = false;
		m_PressA = m_MouseA;
		m_PressB = m_MouseB;
		int h = HandleAt(m_MouseA, m_MouseB);
		if (h >= 0)
		{
			m_DragPoint = h;
			return;
		}
		if (m_Tool == TOOL_CIRCLE && m_DraftKind < 0)
		{
			m_DraftKind = VB_MapKind.CIRCLE;
			m_DraftPts.Clear();
			m_DraftPts.Insert(WorldX(m_MouseA));
			m_DraftPts.Insert(WorldZ(m_MouseB));
			m_DraftR = 0;
			m_DragNew = true;
			return;
		}
		m_Pan = true;
		m_PanWX = WorldX(m_MouseA);
		m_PanWZ = WorldZ(m_MouseB);
	}

	protected void Drag()
	{
		// В пределах мира и радиуса, как проверяет сервер: иначе объект пропал бы с общей ошибкой.
		float wx = Math.Clamp(WorldX(m_MouseA), 0, VB_MAP_WORLD_MAX);
		float wz = Math.Clamp(WorldZ(m_MouseB), 0, VB_MAP_WORLD_MAX);
		if (m_DragNew)
		{
			m_DraftR = Math.Clamp(Hypot(wx - m_DraftPts.Get(0), wz - m_DraftPts.Get(1)), 0, VB_MAP_RADIUS_MAX);
			m_ShapesDirty = true;
			return;
		}
		if (m_DragPoint < 0 || !m_Sel)
			return;
		if (m_Sel.Kind == VB_MapKind.CIRCLE)
			m_Sel.Radius = Math.Clamp(Hypot(wx - m_Sel.PX(0), wz - m_Sel.PZ(0)), VB_MAP_RADIUS_MIN, VB_MAP_RADIUS_MAX);
		else
			m_Sel.SetPoint(m_DragPoint, wx, wz);
		m_SelDirty = true;
		m_ShapesDirty = true;
		PlaceHandles();
		PlacePinsOnly();
	}

	protected void Release()
	{
		m_Pressed = false;
		if (m_DragNew)
		{
			m_DragNew = false;
			if (m_DraftR < VB_MAP_RADIUS_MIN)
				m_DraftR = 150;
			Finish();
			return;
		}
		if (m_DragPoint >= 0)
		{
			m_DragPoint = -1;
			// Готовый объект сохраняется сразу, новый вместе с формой.
			if (m_Sel && !IsEditing() && m_SelDirty)
				SendSel();
			m_Need = m_Need | NEED_CARD;
			return;
		}
		bool wasPan = m_Pan;
		m_Pan = false;
		if (wasPan && !m_Moved)
			ClickAt(m_MouseA, m_MouseB);
	}

	protected float Hypot(float a, float b)
	{
		return Math.Sqrt(a * a + b * b);
	}

	bool OnWheel(Widget w, int wheel)
	{
		if (w == m_Catch)
		{
			float k = 0.8;
			if (wheel < 0)
				k = 1.25;
			ZoomBy(k, m_MouseA, m_MouseB);
			return true;
		}
		if (m_SidePlate && Inside(m_SidePlate, m_MouseA, m_MouseB) && m_MouseB >= m_SidePlate.Y + 44)
		{
			if (wheel < 0)
				s_Top = s_Top + 1;
			else
				s_Top = Math.Max(0, s_Top - 1);
			m_Need = m_Need | NEED_SIDE;
			return true;
		}
		return false;
	}

	protected void ClickAt(float a, float b)
	{
		float wx = WorldX(a);
		float wz = WorldZ(b);
		if (m_Tool == TOOL_HAND)
		{
			VB_MapObject o = HitObject(a, b);
			if (o)
				SelectObj(o);
			else
				CloseSel();
			return;
		}
		if (m_Tool == TOOL_MARK)
		{
			m_DraftKind = VB_MapKind.MARK;
			m_DraftPts.Clear();
			m_DraftPts.Insert(wx);
			m_DraftPts.Insert(wz);
			Finish();
			return;
		}
		if (m_Tool == TOOL_ZONE || m_Tool == TOOL_ROUTE)
		{
			if (m_DraftKind < 0)
			{
				m_DraftKind = VB_MapKind.ROUTE;
				if (m_Tool == TOOL_ZONE)
					m_DraftKind = VB_MapKind.ZONE;
				m_DraftPts.Clear();
			}
			int n = m_DraftPts.Count() / 2;
			if (m_Tool == TOOL_ZONE && n >= 3)
			{
				float fa = LocA(m_DraftPts.Get(0));
				float fb = LocB(m_DraftPts.Get(1));
				if (Hypot(a - fa, b - fb) < 14)
				{
					Finish();
					return;
				}
			}
			m_DraftPts.Insert(wx);
			m_DraftPts.Insert(wz);
			m_Need = m_Need | NEED_DRAFT;
			m_ShapesDirty = true;
			PlaceHandles();
			if (n + 1 >= VB_MAP_POINTS_MAX)
				Finish();
		}
	}

	// Ручка выбранного объекта под курсором.
	protected int HandleAt(float a, float b)
	{
		if (!m_Sel || !CanEdit(m_Sel) || m_Sel.Kind == VB_MapKind.MARK)
			return -1;
		if (m_Sel.Kind == VB_MapKind.CIRCLE)
		{
			float cx = LocA(m_Sel.PX(0)) + m_Sel.Radius * m_Kx;
			float cy = LocB(m_Sel.PZ(0));
			if (Hypot(a - cx, b - cy) < 12)
				return 0;
			return -1;
		}
		for (int i = 0; i < m_Sel.PointCount(); i++)
		{
			if (Hypot(a - LocA(m_Sel.PX(i)), b - LocB(m_Sel.PZ(i))) < 12)
				return i;
		}
		return -1;
	}

	protected VB_MapObject HitObject(float a, float b)
	{
		int i;
		for (i = m_Items.Count() - 1; i >= 0; i--)
		{
			VBT_MapPin pin = m_Items.Get(i);
			if (pin.Obj && pin.Size > 0 && Hypot(a - pin.X, b - pin.Y) <= pin.Size * 0.5 + 2)
				return pin.Obj;
		}
		for (i = m_State.Objects.Count() - 1; i >= 0; i--)
		{
			VB_MapObject o = m_State.Objects.Get(i);
			if (o.Kind == VB_MapKind.MARK || !m_State.IsVisible(o))
				continue;
			if (HitShape(o, a, b))
				return o;
		}
		return null;
	}

	protected bool HitShape(VB_MapObject o, float a, float b)
	{
		if (o.Kind == VB_MapKind.CIRCLE)
			return Hypot(a - LocA(o.PX(0)), b - LocB(o.PZ(0))) <= o.Radius * m_Kx;
		int n = o.PointCount();
		if (o.Kind == VB_MapKind.ZONE)
		{
			bool inside = false;
			int j = n - 1;
			for (int i = 0; i < n; i++)
			{
				float ax = LocA(o.PX(i));
				float ay = LocB(o.PZ(i));
				float bx = LocA(o.PX(j));
				float by = LocB(o.PZ(j));
				bool above1 = ay > b;
				bool above2 = by > b;
				if (above1 != above2 && a < (bx - ax) * (b - ay) / (by - ay) + ax)
					inside = !inside;
				j = i;
			}
			return inside;
		}
		for (int k = 1; k < n; k++)
		{
			float sx = LocA(o.PX(k - 1));
			float sy = LocB(o.PZ(k - 1));
			float ex = LocA(o.PX(k));
			float ey = LocB(o.PZ(k));
			if (SegDist(a, b, sx, sy, ex, ey) < 9)
				return true;
		}
		return false;
	}

	protected float SegDist(float px, float py, float ax, float ay, float bx, float by)
	{
		float dx = bx - ax;
		float dy = by - ay;
		float len = dx * dx + dy * dy;
		if (len <= 0)
			len = 1;
		float t = Math.Clamp(((px - ax) * dx + (py - ay) * dy) / len, 0, 1);
		return Hypot(px - ax - t * dx, py - ay - t * dy);
	}

	// --- данные ------------------------------------------------------------------------------
	// Клановые объекты ведут те, чей ранг это разрешает.
	protected bool Access()
	{
		return m_Client.Can(VB_ClanPerm.MAP);
	}

	protected bool CanEdit(VB_MapObject o)
	{
		if (o.Scope == VB_MapScope.SERVER)
			return m_Client.IsAdmin && !o.IsVpp();
		if (o.Scope == VB_MapScope.CLAN)
			return Access();
		return true;
	}

	// Кому видно новое: личное всегда, клану по рангу, всем только администратору.
	protected array<int> Dests(int kind)
	{
		array<int> list = new array<int>();
		list.Insert(VB_MapScope.ME);
		if (Access())
			list.Insert(VB_MapScope.CLAN);
		if (m_Client.IsAdmin && kind == VB_MapKind.MARK)
			list.Insert(VB_MapScope.SERVER);
		return list;
	}

	protected float DestWidth(int scope)
	{
		if (scope == VB_MapScope.CLAN)
			return 60;
		if (scope == VB_MapScope.SERVER)
			return 52;
		return 76;
	}

	protected string DestText(int scope)
	{
		if (scope == VB_MapScope.CLAN)
			return "#STR_VB_TAB_D_TO_CLAN";
		if (scope == VB_MapScope.SERVER)
			return "#STR_VB_TAB_D_TO_ALL";
		return "#STR_VB_TAB_D_TO_ME";
	}

	protected string Measure(VB_MapObject o)
	{
		string head;
		string tail;
		if (o.Kind == VB_MapKind.ZONE)
		{
			head = VBT.T("#STR_VB_TAB_D_AREA");
			tail = VBT.T("#STR_VB_TAB_D_KM2");
			return head + " " + VBT_MapUi.Fix2(o.AreaOf() / 1000000.0) + " " + tail;
		}
		if (o.Kind == VB_MapKind.CIRCLE)
		{
			int r = Math.Round(o.Radius);
			head = VBT.T("#STR_VB_TAB_D_RADIUS");
			tail = VBT.T("#STR_VB_TAB_D_M");
			return head + " " + r.ToString() + " " + tail;
		}
		if (o.Kind == VB_MapKind.ROUTE)
		{
			head = VBT.T("#STR_VB_TAB_D_LENGTH");
			tail = VBT.T("#STR_VB_TAB_KM");
			return head + " " + VBT_MapUi.Fix2(o.LengthOf() / 1000.0) + " " + tail;
		}
		return VBT_MapUi.Coords(o.PX(0), o.PZ(0));
	}

	protected float DistTo(float x, float z)
	{
		vector me = GetGame().GetPlayer().GetPosition();
		return Hypot(x - me[0], z - me[2]);
	}

	protected string MyName()
	{
		if (m_Client.Me.PlayerName != "")
			return m_Client.Me.PlayerName;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetIdentity())
			return player.GetIdentity().GetName();
		return "";
	}

	protected void SendPref(int what, int key, int mode)
	{
		ScriptRPC rpc = m_Client.Begin(VB_TabletOp.MAP_PREF);
		rpc.Write(what);
		rpc.Write(key);
		rpc.Write(mode);
		m_Client.Post(rpc);
	}

	protected void SendSel()
	{
		if (!m_Sel)
			return;
		ScriptRPC rpc = m_Client.Begin(VB_TabletOp.MAP_SAVE);
		rpc.Write(m_SelOldScope);
		m_Sel.Write(rpc);
		m_Client.Post(rpc);
		m_Sel.Sent = true;
		m_SelOldScope = m_Sel.Scope;
		m_SelDirty = false;
		m_SavePending = true;
	}

	// Несохранённая правка уходит на сервер, что бы ни закрыло форму.
	protected void FlushSelection()
	{
		if (m_Sel && (m_IsNew || m_SelDirty))
			SendSel();
		m_IsNew = false;
	}

	// --- рисование объектов ------------------------------------------------------------------
	protected void SetTool(int tool)
	{
		m_DraftKind = -1;
		m_DraftPts.Clear();
		m_DraftHasCur = false;
		m_Tool = tool;
		CloseSel();
		m_Need = m_Need | NEED_TOOLS | NEED_DRAFT;
		m_ShapesDirty = true;
		PlaceHandles();
	}

	protected void StopDraft()
	{
		SetTool(TOOL_HAND);
	}

	protected void Undo()
	{
		if (m_DraftKind < 0)
			return;
		int n = m_DraftPts.Count() / 2;
		if (n > 1 && m_DraftKind != VB_MapKind.CIRCLE)
		{
			m_DraftPts.Remove(m_DraftPts.Count() - 1);
			m_DraftPts.Remove(m_DraftPts.Count() - 1);
		}
		else
		{
			m_DraftKind = -1;
			m_DraftPts.Clear();
			m_DraftHasCur = false;
		}
		m_Need = m_Need | NEED_DRAFT;
		m_ShapesDirty = true;
		PlaceHandles();
	}

	protected void Finish()
	{
		if (m_DraftKind < 0)
			return;
		int kind = m_DraftKind;
		int count = m_DraftPts.Count() / 2;
		if (kind == VB_MapKind.ZONE && count < 3)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_D_ZONE_MIN"));
			return;
		}
		if (kind == VB_MapKind.ROUTE && count < 2)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_D_ROUTE_MIN"));
			return;
		}
		int scope = s_Dest;
		if (scope == VB_MapScope.CLAN && !Access())
			scope = VB_MapScope.ME;
		if (scope == VB_MapScope.SERVER && !(m_Client.IsAdmin && kind == VB_MapKind.MARK))
			scope = VB_MapScope.ME;
		int lim = m_State.Limits.Of(scope, kind);
		if (m_State.Used(scope, kind) >= lim)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_D_LIMIT") + " " + lim.ToString());
			StopDraft();
			return;
		}
		int same = 1;
		foreach (VB_MapObject other : m_State.Objects)
		{
			if (other.Kind == kind)
				same++;
		}
		VB_MapObject o = new VB_MapObject();
		o.Scope = scope;
		o.Kind = kind;
		o.IconIdx = 0;
		m_DefaultTitle = VBT.T(VBT_MapUi.KindName(kind)) + " " + same.ToString();
		o.Title = m_DefaultTitle;
		o.Color = VB_C_ACCENT;
		o.Author = MyName();
		if (scope == VB_MapScope.SERVER)
			o.Author = "";
		if (scope == VB_MapScope.CLAN)
		{
			VB_MapCat cat = m_State.FindCat(s_CurCat);
			if (cat)
			{
				o.CatId = cat.CatId;
				o.Color = cat.Color;
			}
		}
		for (int i = 0; i < count; i++)
		{
			int px = Math.Round(Math.Clamp(m_DraftPts.Get(i * 2), 0, VB_MAP_WORLD_MAX));
			int pz = Math.Round(Math.Clamp(m_DraftPts.Get(i * 2 + 1), 0, VB_MAP_WORLD_MAX));
			o.AddPoint(px, pz);
		}
		if (kind == VB_MapKind.CIRCLE)
			o.Radius = Math.Round(Math.Clamp(m_DraftR, VB_MAP_RADIUS_MIN, VB_MAP_RADIUS_MAX));
		m_State.Objects.Insert(o);
		m_DraftKind = -1;
		m_DraftPts.Clear();
		m_DraftHasCur = false;
		m_Tool = TOOL_HAND;
		m_IsNew = true;
		m_FocusName = true;
		m_CardEdit = false;
		m_SelDirty = false;
		m_Sel = o;
		m_SelOldScope = scope;
		Reveal(o);
		m_Need = m_Need | NEED_TOOLS | NEED_DRAFT | NEED_PINS | NEED_SIDE | NEED_CARD;
		m_ShapesDirty = true;
		PlaceHandles();
	}

	// Карта сдвигается, если объект оказался бы под карточкой.
	protected void Reveal(VB_MapObject o)
	{
		float x;
		float z;
		o.AnchorOf(x, z);
		float ax = LocA(x);
		float lim = VW - 12 - 244 - 40;
		if (ax > lim)
			ShiftMap((ax - lim) / m_Kx, 0);
	}

	protected void SelectObj(VB_MapObject o)
	{
		FlushSelection();
		m_Sel = o;
		m_SelOldScope = o.Scope;
		m_IsNew = false;
		m_SelDirty = false;
		m_CardEdit = false;
		if (m_MatesOpen)
		{
			m_MatesOpen = false;
			m_Need = m_Need | NEED_MATES;
		}
		Reveal(o);
		m_Need = m_Need | NEED_CARD | NEED_SIDE;
		m_LabelsDue = true;
		m_ShapesDirty = true;
		PlaceHandles();
	}

	protected void CloseSel()
	{
		FlushSelection();
		m_Sel = null;
		m_IsNew = false;
		m_SelDirty = false;
		m_CardEdit = false;
		m_Need = m_Need | NEED_CARD | NEED_SIDE;
		m_LabelsDue = true;
		m_ShapesDirty = true;
		PlaceHandles();
	}

	protected void RemoveSel()
	{
		if (!m_Sel)
			return;
		VB_MapObject o = m_Sel;
		if (o.ObjId > 0)
		{
			int scope = m_SelOldScope;
			int objId = o.ObjId;
			ScriptRPC rpc = m_Client.Begin(VB_TabletOp.MAP_DELETE);
			rpc.Write(scope);
			rpc.Write(objId);
			m_Client.Post(rpc);
		}
		m_State.Objects.RemoveOrdered(m_State.Objects.Find(o));
		m_IsNew = false;
		m_SelDirty = false;
		CloseSel();
		m_Need = m_Need | NEED_PINS;
		INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_D_DELETED"));
	}

	// Режимы показа по кругу: скрыт, 2D, 3D. У зон, кругов и маршрутов 3D нет.
	protected void Cycle(VB_MapObject o)
	{
		if (o.ObjId <= 0)
			return;
		int mode = m_State.ModeOf(o);
		int next = VB_MapMode.HIDDEN;
		if (mode == VB_MapMode.HIDDEN)
		{
			next = VB_MapMode.FLAT;
		}
		else if (mode == VB_MapMode.FLAT && o.Kind == VB_MapKind.MARK && m_State.Limits.Allow3D)
		{
			if (m_State.CountD3() >= m_State.Limits.D3Max)
				INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_D_D3_MAX") + " " + m_State.Limits.D3Max.ToString());
			else
				next = VB_MapMode.WORLD;
		}
		m_State.SetMode(o, next);
		SendPref(VB_MapPref.OBJECT, o.ObjKey(), next);
	}

	// --- панели ------------------------------------------------------------------------------
	protected VBT_MapPlate Plate(float x, float y, float w, float h, int alpha)
	{
		VBT_MapPlate p = new VBT_MapPlate();
		p.X = x;
		p.Y = y;
		p.W = w;
		p.H = h;
		p.Alpha = alpha;
		p.Root = INT_UIF_Draw.Frame(m_Ui, x, y, w, h);
		p.Bg = INT_UIF_Surface.Box(p.Root, 0, 0, w, h, 8, ARGB(alpha, 14, 13, 12), WHITE_09);
		// Клик и колесо по панели не должны доходить до карты.
		INT_UIF_Draw.Hit(p.Root, 0, 0, w, h, m_Handler);
		m_Plates.Insert(p);
		return p;
	}

	protected void DropPlate(VBT_MapPlate p)
	{
		if (!p)
			return;
		m_Plates.RemoveItem(p);
		if (p.Root)
			p.Root.Unlink();
		p.Root = null;
	}

	protected bool Inside(VBT_MapPlate p, float a, float b)
	{
		return a >= p.X && a <= p.X + p.W && b >= p.Y && b <= p.Y + p.H;
	}

	protected void HoverPlates()
	{
		foreach (VBT_MapPlate p : m_Plates)
		{
			bool hot = Inside(p, m_MouseA, m_MouseB);
			if (hot == p.Hot)
				continue;
			p.Hot = hot;
			if (hot)
				p.Bg.SetFill(ARGB(GLASS_HOT, 14, 13, 12));
			else
				p.Bg.SetFill(ARGB(p.Alpha, 14, 13, 12));
		}
	}

	protected void DropBtns(int sect)
	{
		for (int i = m_Btns.Count() - 1; i >= 0; i--)
		{
			if (m_Btns.Get(i).Sect == sect)
				m_Btns.Remove(i);
		}
	}

	protected VBT_MapBtn AddHit(Widget p, float x, float y, float w, float h, int sect, int act, int arg = 0)
	{
		VBT_MapBtn b = new VBT_MapBtn();
		b.Hit = INT_UIF_Draw.Hit(p, x, y, w, h, m_Handler);
		b.Sect = sect;
		b.Act = act;
		b.Arg = arg;
		m_Btns.Insert(b);
		return b;
	}

	// Кнопка-значок без подложки. on: 0 обычная, 1 акцент, 2 мягкая.
	protected VBT_MapBtn IconBtn(Widget p, float x, float y, float s, string ic, int sect, int act, int arg, int on, string tip = "")
	{
		int fill = 0;
		int ink = VB_C_TEXT;
		if (on == 2)
		{
			fill = WHITE_14;
			ink = VB_C_ACCENT;
		}
		else if (on == 1)
		{
			fill = VB_C_ACCENT;
			ink = VB_C_ON;
		}
		INT_UIF_Surface bg = INT_UIF_Surface.Box(p, x, y, s, s, 6, fill);
		VBT.Ico(p, x + (s - 18) * 0.5, y + (s - 18) * 0.5, 18, ic, ink);
		VBT_MapBtn b = AddHit(p, x, y, s, s, sect, act, arg);
		b.Bg = bg;
		b.Fill = fill;
		b.FillHot = fill;
		if (on == 0)
			b.FillHot = WHITE_12;
		b.Tip = tip;
		b.TipParent = p;
		b.TipX = x + s + 10;
		b.TipY = y + (s - 26) * 0.5;
		return b;
	}

	protected VBT_MapBtn WrapBtn(VBT_Button btn, int sect, int act, int arg = 0)
	{
		VBT_MapBtn b = new VBT_MapBtn();
		b.Btn = btn;
		b.Hit = btn.Hit;
		b.Sect = sect;
		b.Act = act;
		b.Arg = arg;
		m_Btns.Insert(b);
		return b;
	}

	protected void ClearTip()
	{
		if (m_Tip)
			m_Tip.Unlink();
		m_Tip = null;
	}

	protected void ShowTip(VBT_MapBtn b)
	{
		ClearTip();
		float w = INT_UIF_Draw.TextWidth(b.Tip, 13) + 20;
		m_Tip = INT_UIF_Draw.Frame(b.TipParent, b.TipX, b.TipY, w, 26);
		INT_UIF_Surface.Box(m_Tip, 0, 0, w, 26, 6, 0xEB0E0D0C);
		INT_UIF_Draw.Text(m_Tip, 10, 0, w - 12, 26, b.Tip, 13, VB_C_TEXT);
	}

	protected void BuildZoom()
	{
		Widget z = Plate(VW - 48, 12, 36, 110, 158).Root;
		IconBtn(z, 2, 2, 32, "plus", SECT_ZOOM, VBT_MapAct.ZOOM_IN, 0, 0);
		IconBtn(z, 2, 38, 32, "minus", SECT_ZOOM, VBT_MapAct.ZOOM_OUT, 0, 0);
		IconBtn(z, 2, 74, 32, "center", SECT_ZOOM, VBT_MapAct.CENTER_ME, 0, 0);
	}

	protected void PaintTools()
	{
		DropPlate(m_RailPlate);
		DropBtns(SECT_TOOLS);
		m_RailPlate = Plate(12, 12, 44, 282, 158);
		Widget b = m_RailPlate.Root;
		array<string> icons = {"cursor", "pin", "polygon", "circleo", "route"};
		array<string> tips = {"#STR_VB_TAB_D_TOOL_0", "#STR_VB_TAB_D_KIND_0", "#STR_VB_TAB_D_KIND_1", "#STR_VB_TAB_D_KIND_2", "#STR_VB_TAB_D_KIND_3"};
		for (int i = 0; i < 5; i++)
		{
			int on = 0;
			if (m_Tool == i)
				on = 1;
			IconBtn(b, 4, 4 + i * 38, 36, icons.Get(i), SECT_TOOLS, VBT_MapAct.TOOL, i, on, tips.Get(i));
		}
		INT_UIF_Draw.Line(b, 8, 197, 28, 1, WHITE_16);
		int soft = 0;
		if (s_Drawer)
			soft = 2;
		IconBtn(b, 4, 204, 36, "layers", SECT_TOOLS, VBT_MapAct.DRAWER, 0, soft, "#STR_VB_TAB_D_OBJECTS");
		soft = 0;
		if (s_Labels)
			soft = 2;
		IconBtn(b, 4, 242, 36, "note", SECT_TOOLS, VBT_MapAct.LABELS, 0, soft, "#STR_VB_TAB_D_LABELS");
	}

	protected int KindOfTool(int tool)
	{
		if (tool == TOOL_ZONE)
			return VB_MapKind.ZONE;
		if (tool == TOOL_CIRCLE)
			return VB_MapKind.CIRCLE;
		if (tool == TOOL_ROUTE)
			return VB_MapKind.ROUTE;
		return VB_MapKind.MARK;
	}

	// Полоска рисования: появляется только с выбранным инструментом.
	protected void PaintDraft()
	{
		DropPlate(m_DraftPlate);
		m_DraftPlate = null;
		DropBtns(SECT_DRAFT);
		if (m_Tool == TOOL_HAND && m_DraftKind < 0)
			return;
		int kind = m_DraftKind;
		if (kind < 0)
			kind = KindOfTool(m_Tool);
		bool pts = m_DraftKind >= 0 && kind != VB_MapKind.CIRCLE && kind != VB_MapKind.MARK;
		array<int> opts = Dests(kind);
		bool many = opts.Count() > 1;
		if (opts.Find(s_Dest) < 0)
			s_Dest = VB_MapScope.ME;
		int lim = m_State.Limits.Of(s_Dest, kind);
		float ow = 0;
		if (many)
		{
			ow = 10;
			foreach (int dw : opts)
				ow = ow + DestWidth(dw) + 2;
		}
		float w = 146 + ow + 72;
		if (pts)
			w = 146 + ow + 204;
		m_DraftPlate = Plate((VW - w) * 0.5, 12, w, 36, 194);
		Widget f = m_DraftPlate.Root;
		string counter;
		if (pts)
		{
			int done = m_DraftPts.Count() / 2;
			int most = VB_MAP_POINTS_MAX;
			counter = done.ToString() + "/" + most.ToString();
		}
		else
		{
			int used = m_State.Used(s_Dest, kind);
			counter = used.ToString() + "/" + lim.ToString();
		}
		INT_UIF_Draw.Text(f, 14, 0, 126, 36, VBT.T(VBT_MapUi.KindName(kind)) + " " + counter, 13, VB_C_TEXT);
		float x = 146;
		if (many)
		{
			float px = x;
			foreach (int dest : opts)
			{
				float bw = DestWidth(dest);
				bool on = s_Dest == dest;
				int fill = WHITE_08;
				int ink = VB_C_TEXT;
				if (on)
				{
					fill = VB_C_ACCENT;
					ink = VB_C_ON;
				}
				INT_UIF_Surface.Box(f, px, 6, bw, 24, 5, fill);
				INT_UIF_Draw.Text(f, px, 6, bw, 24, DestText(dest), 12, ink, INT_UIF_Align.CENTER);
				AddHit(f, px, 6, bw, 24, SECT_DRAFT, VBT_MapAct.DEST, dest);
				px = px + bw + 2;
			}
			x = x + ow;
		}
		ScriptedWidgetEventHandler h = m_Handler;
		if (pts)
		{
			WrapBtn(new VBT_Button(f, x, 5, 62, 26, "#STR_VB_TAB_D_BACK", false, "", h, 12), SECT_DRAFT, VBT_MapAct.UNDO);
			WrapBtn(new VBT_Button(f, x + 66, 5, 66, 26, "#STR_VB_TAB_CANCEL", false, "", h, 12), SECT_DRAFT, VBT_MapAct.CANCEL);
			WrapBtn(new VBT_Button(f, x + 136, 5, 60, 26, "#STR_VB_TAB_DONE", true, "", h, 12), SECT_DRAFT, VBT_MapAct.FINISH);
		}
		else
		{
			WrapBtn(new VBT_Button(f, x, 5, 64, 26, "#STR_VB_TAB_CANCEL", false, "", h, 12), SECT_DRAFT, VBT_MapAct.CANCEL);
		}
	}

	// Карточка объекта: всегда у правого края по центру высоты, короткая, правка по кнопке.
	protected void PaintCard()
	{
		if (m_NameField)
		{
			if (GetFocus())
				SetFocus(null);
			m_NameFieldOld = m_NameField;
			m_NameField = null;
		}
		m_CatDrop = null;
		DropPlate(m_CardPlate);
		m_CardPlate = null;
		DropBtns(SECT_CARD);
		if (!m_Sel)
			return;
		VB_MapObject o = m_Sel;
		bool ed = CanEdit(o);
		array<int> opts = Dests(o.Kind);
		bool many = opts.Count() > 1;
		bool edit = ed && (m_CardEdit || m_IsNew);
		float w = 244;
		float h = 128;
		if (edit)
		{
			h = 46 + 40 + 88 + 24 + 42;
			if (o.Kind == VB_MapKind.MARK)
				h = h + 134;
			if (many)
				h = h + 52;
			if (o.Scope == VB_MapScope.CLAN)
				h = h + 54;
		}
		m_CardPlate = Plate(VW - 12 - w, (VH - h) * 0.5, w, h, 214);
		m_CardPlate.Root.SetSort(50);
		Widget f = m_CardPlate.Root;
		VBT.Cap(f, 12, 10, w - 50, VBT_MapUi.KindName(o.Kind));
		ImageWidget ci = VBT.Ico(f, w - 28, 10, 16, "close", VB_C_DIM);
		VBT_MapBtn cb = AddHit(f, w - 34, 4, 30, 28, SECT_CARD, VBT_MapAct.CARD_CLOSE);
		cb.Img = ci;
		cb.Ink = VB_C_DIM;
		cb.InkHot = VB_C_TEXT;
		if (edit)
			PaintCardEdit(f, o, w, opts, many);
		else
			PaintCardView(f, o, w, ed);
	}

	protected void PaintCardView(Widget f, VB_MapObject o, float w, bool ed)
	{
		float y = 34;
		INT_UIF_Draw.Text(f, 12, y - 2, w - 24, 24, VBT.Fit(VBT.T(o.Title), w - 24, 16), 16, VB_C_TEXT);
		float ax;
		float az;
		o.AnchorOf(ax, az);
		string line = Measure(o) + " · " + VBT_MapUi.Dist(DistTo(ax, az)) + " " + VBT.T("#STR_VB_TAB_D_FROM_YOU");
		INT_UIF_Draw.Text(f, 12, y + 22, w - 24, 18, VBT.Fit(line, w - 24, 12), 12, VB_C_DIM);
		string who = VBT.T("#STR_VB_TAB_D_ONLY_YOU");
		if (o.Scope == VB_MapScope.SERVER)
		{
			who = VBT.T("#STR_VB_TAB_D_ALL_PLAYERS");
		}
		else if (o.Scope == VB_MapScope.CLAN)
		{
			VB_MapCat cat = m_State.FindCat(o.CatId);
			who = VBT.T("#STR_VB_TAB_D_CLAN");
			if (cat)
				who = VBT.T(cat.Title) + " · " + who;
		}
		string author = o.Author;
		if (author == "")
			author = VBT.T("#STR_VB_TAB_D_ADMINS");
		who = who + " · " + author;
		INT_UIF_Draw.Text(f, 12, y + 40, w - 24, 18, VBT.Fit(who, w - 24, 12), 12, VB_C_DIM2);
		float bx = 8;
		if (ed)
		{
			IconBtn(f, bx, 94, 28, "edit", SECT_CARD, VBT_MapAct.CARD_EDIT, 0, 0, "#STR_VB_TAB_D_EDIT");
			bx = bx + 32;
		}
		IconBtn(f, bx, 94, 28, "center", SECT_CARD, VBT_MapAct.CARD_CENTER, 0, 0, "#STR_VB_TAB_D_SHOW");
		bx = bx + 32;
		VisBtn(f, bx, 94, 28, o, -1, SECT_CARD);
		bx = bx + 32;
		if (ed)
			IconBtn(f, bx, 94, 28, "trash", SECT_CARD, VBT_MapAct.CARD_DELETE, 0, 0, "#STR_VB_TAB_D_DELETE");
	}

	protected void PaintCardEdit(Widget f, VB_MapObject o, float w, array<int> opts, bool many)
	{
		float y = 34;
		string value = o.Title;
		string hint = "#STR_VB_TAB_D_NAME";
		// Новый объект: подставленное имя как подсказка, чтобы название писалось с чистого поля.
		if (m_IsNew && o.Title == m_DefaultTitle)
		{
			value = "";
			hint = m_DefaultTitle;
		}
		m_NameField = new VBT_Input(f, 12, y, w - 24, 34, hint, VB_MAP_TITLE_MAX, value);
		if (m_FocusName)
			m_NameField.Focus();
		m_FocusName = false;
		y = y + 44;
		int n;
		if (o.Kind == VB_MapKind.MARK)
		{
			VBT.Cap(f, 12, y, w - 24, "#STR_VB_TAB_D_ICON");
			y = y + 20;
			array<string> icons = VB_MapIcons.Marks();
			for (n = 0; n < icons.Count(); n++)
			{
				int col8 = n % 8;
				int row8 = n / 8;
				float bx = 12 + col8 * 28;
				float cy = y + row8 * 27;
				bool on = o.IconIdx == n;
				int fill = WHITE_08;
				int ink = VB_C_TEXT;
				if (on)
				{
					fill = o.Color;
					ink = INK_DARK;
				}
				INT_UIF_Surface.Box(f, bx, cy, 24, 24, 5, fill);
				VBT.Ico(f, bx + 5, cy + 5, 14, icons.Get(n), ink);
				AddHit(f, bx, cy, 24, 24, SECT_CARD, VBT_MapAct.PICK_ICON, n);
			}
			y = y + 114;
		}
		VBT.Cap(f, 12, y, w - 24, "#STR_VB_TAB_D_COLOR");
		y = y + 20;
		array<int> colors = VBK.Colors();
		for (n = 0; n < colors.Count(); n++)
		{
			int col6 = n % 6;
			int row6 = n / 6;
			float px = 12 + col6 * 37;
			float py = y + row6 * 32;
			int swatch = colors.Get(n);
			if (o.Color == swatch)
				INT_UIF_Surface.Pill(f, px - 2, py - 2, 30, 30, 0, VB_C_TEXT);
			INT_UIF_Surface.Pill(f, px + 1, py + 1, 24, 24, swatch);
			AddHit(f, px - 2, py - 2, 30, 30, SECT_CARD, VBT_MapAct.PICK_COLOR, n);
		}
		y = y + 70;
		if (many)
		{
			VBT.Cap(f, 12, y, w - 24, "#STR_VB_TAB_D_WHO_SEES");
			y = y + 20;
			int cnt = opts.Count();
			float sw = Math.Floor((w - 24 - (cnt - 1) * 4) / cnt);
			for (n = 0; n < cnt; n++)
			{
				int dest = opts.Get(n);
				float tx = 12 + n * (sw + 4);
				bool cur = o.Scope == dest;
				int tf = WHITE_08;
				int ti = VB_C_TEXT;
				if (cur)
				{
					tf = VB_C_ACCENT;
					ti = VB_C_ON;
				}
				string label = DestText(dest);
				if (cnt > 2 && dest == VB_MapScope.ME)
					label = "#STR_VB_TAB_D_TO_ME_SHORT";
				INT_UIF_Surface.Box(f, tx, y, sw, 26, 5, tf);
				INT_UIF_Draw.Text(f, tx, y, sw, 26, label, 12, ti, INT_UIF_Align.CENTER);
				AddHit(f, tx, y, sw, 26, SECT_CARD, VBT_MapAct.PICK_SCOPE, dest);
			}
			y = y + 32;
		}
		if (o.Scope == VB_MapScope.CLAN)
		{
			VBT.Cap(f, 12, y, w - 24, "#STR_VB_TAB_D_CATEGORY");
			y = y + 20;
			array<string> names = new array<string>();
			m_CatDropCats.Clear();
			int selected = 0;
			foreach (VB_MapCat c : m_State.Cats)
			{
				if (c.CatId == o.CatId)
					selected = names.Count();
				names.Insert(c.Title);
				m_CatDropCats.Insert(c);
			}
			m_CatDrop = new VBT_Dropdown(f, 12, y, w - 24, 28, names, selected);
			y = y + 34;
		}
		INT_UIF_Draw.Text(f, 12, y, w - 24, 20, VBT.Fit(Measure(o), w - 24, 12), 12, VB_C_DIM);
		y = y + 24;
		VBT_Button del = new VBT_Button(f, 12, y, 40, 32, "", false, "", m_Handler);
		del.Danger = true;
		del.Bg.SetFill(VB_C_BAD);
		VBT.Ico(f, 23, y + 7, 18, "trash", VB_C_ON);
		WrapBtn(del, SECT_CARD, VBT_MapAct.CARD_DELETE);
		WrapBtn(new VBT_Button(f, 60, y, w - 72, 32, "#STR_VB_TAB_DONE", true, "", m_Handler, 13), SECT_CARD, VBT_MapAct.CARD_CLOSE);
	}

	// Кнопка режима: зачёркнутый глаз, 2D или 3D. arg: индекс строки, -1 выбранный объект.
	protected void VisBtn(Widget p, float x, float y, float s, VB_MapObject o, int arg, int sect)
	{
		INT_UIF_Surface bg = INT_UIF_Surface.Box(p, x, y, s, s, 6, 0);
		int mode = m_State.ModeOf(o);
		if (mode == VB_MapMode.HIDDEN)
		{
			VBT.Ico(p, x + (s - 16) * 0.5, y + (s - 16) * 0.5, 16, "eye_off", VB_C_DIM);
		}
		else if (o.Kind != VB_MapKind.MARK)
		{
			VBT.Ico(p, x + (s - 16) * 0.5, y + (s - 16) * 0.5, 16, "eye", VB_C_TEXT);
		}
		else
		{
			string word = "2D";
			int ink = VB_C_TEXT;
			if (mode == VB_MapMode.WORLD)
			{
				word = "3D";
				ink = VB_C_ACCENT;
			}
			INT_UIF_Draw.Text(p, x, y, s, s, word, 12, ink, INT_UIF_Align.CENTER);
		}
		VBT_MapBtn b = AddHit(p, x, y, s, s, sect, VBT_MapAct.VIS, arg);
		b.Bg = bg;
		b.Fill = 0;
		b.FillHot = WHITE_12;
	}

	// Список объектов: открывается кнопкой, высота по содержимому.
	protected void BuildRows()
	{
		m_Rows.Clear();
		VBT_MapRow row;
		if (s_Side == SIDE_CLAN && m_Client.Clan)
		{
			foreach (VB_MapCat c : m_State.Cats)
			{
				row = new VBT_MapRow();
				row.Cat = c;
				m_Rows.Insert(row);
				bool open = s_OpenCats.Find(c.CatId) >= 0;
				foreach (VB_MapObject o : m_State.Objects)
				{
					if (o.Scope != VB_MapScope.CLAN)
						continue;
					VB_MapCat own = m_State.FindCat(o.CatId);
					if (own != c)
						continue;
					row.Count = row.Count + 1;
					if (open)
					{
						VBT_MapRow sub = new VBT_MapRow();
						sub.Obj = o;
						sub.Ind = 14;
						m_Rows.Insert(sub);
					}
				}
			}
			return;
		}
		int want = VB_MapScope.ME;
		if (s_Side == SIDE_SERVER)
			want = VB_MapScope.SERVER;
		for (int i = m_State.Objects.Count() - 1; i >= 0; i--)
		{
			VB_MapObject item = m_State.Objects.Get(i);
			if (item.Scope != want)
				continue;
			row = new VBT_MapRow();
			row.Obj = item;
			if (want == VB_MapScope.SERVER)
				m_Rows.InsertAt(row, 0);
			else
				m_Rows.Insert(row);
		}
	}

	protected void PaintSide()
	{
		DropPlate(m_SidePlate);
		m_SidePlate = null;
		DropBtns(SECT_SIDE);
		if (!s_Drawer)
			return;
		if (!m_Client.Clan && s_Side == SIDE_CLAN)
			s_Side = SIDE_ME;
		bool clanSide = s_Side == SIDE_CLAN && m_Client.Clan != null;
		bool btn = clanSide && Access();
		BuildRows();
		float tail = 8;
		if (btn)
			tail = 36;
		int total = m_Rows.Count();
		int most = Math.Floor((VH - 24 - 44 - tail) / 36);
		s_Top = Math.Clamp(s_Top, 0, Math.Max(0, total - most));
		int shown = Math.Max(1, Math.Min(total, most));
		float h = 44 + shown * 36 + tail;
		m_SidePlate = Plate(64, 12, 244, h, 194);
		Widget l = m_SidePlate.Root;
		array<int> tabs = new array<int>();
		array<string> names = new array<string>();
		if (m_Client.Clan)
		{
			tabs.Insert(SIDE_CLAN);
			names.Insert("#STR_VB_TAB_D_TAB_CLAN");
		}
		tabs.Insert(SIDE_ME);
		names.Insert("#STR_VB_TAB_D_TAB_ME");
		tabs.Insert(SIDE_SERVER);
		names.Insert("#STR_VB_TAB_D_TAB_SERVER");
		int t;
		for (t = 0; t < tabs.Count(); t++)
		{
			float x = 8 + t * 58;
			bool on = s_Side == tabs.Get(t);
			int fill = WHITE_08;
			int ink = VB_C_TEXT;
			if (on)
			{
				fill = VB_C_ACCENT;
				ink = VB_C_ON;
			}
			INT_UIF_Surface.Box(l, x, 8, 55, 28, 6, fill);
			INT_UIF_Draw.Text(l, x, 8, 55, 28, names.Get(t), 12, ink, INT_UIF_Align.CENTER);
			AddHit(l, x, 8, 55, 28, SECT_SIDE, VBT_MapAct.SIDE_TAB, tabs.Get(t));
		}
		bool vis = SideLayer();
		string eye = "eye_off";
		int ec = VB_C_DIM2;
		if (vis)
		{
			eye = "eye";
			ec = VB_C_TEXT;
		}
		VBT_MapBtn eb = AddHit(l, 182, 8, 28, 28, SECT_SIDE, VBT_MapAct.SIDE_LAYER);
		eb.Img = VBT.Ico(l, 188, 14, 16, eye, ec);
		eb.Ink = ec;
		eb.InkHot = VB_C_ACCENT;
		VBT_MapBtn cb = AddHit(l, 212, 8, 28, 28, SECT_SIDE, VBT_MapAct.SIDE_CLOSE);
		cb.Img = VBT.Ico(l, 218, 14, 16, "close", VB_C_DIM);
		cb.Ink = VB_C_DIM;
		cb.InkHot = VB_C_TEXT;
		Widget b = INT_UIF_Draw.Clip(l, 0, 44, 244, shown * 36);
		if (total == 0)
			INT_UIF_Draw.Text(b, 14, 0, 216, 36, "#STR_VB_TAB_D_EMPTY", 14, VB_C_DIM2);
		int last = Math.Min(total, s_Top + most);
		for (int i = s_Top; i < last; i++)
		{
			VBT_MapRow r = m_Rows.Get(i);
			float y = (i - s_Top) * 36;
			if (r.Cat)
				CatRow(b, r, y);
			else
				ObjRow(b, r, y, i);
		}
		if (btn)
		{
			VBT_MapBtn nb = AddHit(l, 8, h - 34, 228, 28, SECT_SIDE, VBT_MapAct.CAT_NEW);
			nb.Img = VBT.Ico(l, 16, h - 27, 14, "plus", VB_C_DIM);
			nb.Txt = INT_UIF_Draw.Text(l, 36, h - 34, 190, 28, "#STR_VB_TAB_D_CAT_NEW", 13, VB_C_DIM);
			nb.Ink = VB_C_DIM;
			nb.InkHot = VB_C_TEXT;
		}
	}

	protected bool SideLayer()
	{
		if (s_Side == SIDE_CLAN)
			return m_State.LayerClan;
		if (s_Side == SIDE_SERVER)
			return m_State.LayerServer;
		return m_State.LayerMine;
	}

	protected void ObjRow(Widget b, VBT_MapRow r, float y, int index)
	{
		VB_MapObject o = r.Obj;
		bool sel = m_Sel == o;
		bool hidden = m_State.ModeOf(o) == VB_MapMode.HIDDEN;
		float x = 12 + r.Ind;
		float tw = 166 - r.Ind;
		int fill = 0;
		if (sel)
		{
			fill = WHITE_14;
			if (hidden)
				fill = 0x12FFFFFF;
		}
		INT_UIF_Surface bg = INT_UIF_Surface.Box(b, 6, y + 1, 232, 34, 6, fill);
		if (o.Kind == VB_MapKind.MARK)
			VBT_MapUi.Badge(b, x + 9, y + 18, 18, o.Color, VB_MapIcons.MarkName(o.IconIdx), 10);
		else
			VBT.Ico(b, x, y + 9, 18, VBT_MapUi.KindIcon(o.Kind), o.Color);
		int ink = VB_C_TEXT;
		if (hidden)
			ink = VB_C_DIM2;
		INT_UIF_Draw.Text(b, x + 24, y, tw, 36, VBT.Fit(VBT.T(o.Title), tw, 14), 14, ink);
		VBT_MapBtn rb = AddHit(b, 6, y + 1, 200, 34, SECT_SIDE, VBT_MapAct.ROW, index);
		rb.Bg = bg;
		rb.Fill = fill;
		rb.FillHot = fill;
		if (!sel)
			rb.FillHot = WHITE_08;
		VisBtn(b, 208, y + 4, 28, o, index, SECT_SIDE);
	}

	protected void CatRow(Widget b, VBT_MapRow r, float y)
	{
		VB_MapCat c = r.Cat;
		bool open = s_OpenCats.Find(c.CatId) >= 0;
		bool acc = Access();
		bool hidden = m_State.CatHidden(c.CatId);
		float tw = 118;
		if (acc)
			tw = 96;
		INT_UIF_Surface bg = INT_UIF_Surface.Box(b, 6, y + 1, 232, 34, 6, 0);
		string arrow = "chevron_right";
		if (open)
			arrow = "chevron_down";
		VBT.Ico(b, 12, y + 13, 10, arrow, VB_C_DIM);
		VBT.Ico(b, 28, y + 10, 16, VB_MapIcons.CatName(c.IconIdx), c.Color);
		int ink = VB_C_TEXT;
		if (hidden)
			ink = VB_C_DIM2;
		INT_UIF_Draw.Text(b, 52, y, tw, 36, VBT.Fit(VBT.T(c.Title), tw, 14), 14, ink);
		INT_UIF_Draw.Text(b, 150, y, 28, 36, r.Count.ToString(), 12, VB_C_DIM, INT_UIF_Align.RIGHT);
		VBT_MapBtn rb = AddHit(b, 6, y + 1, 172, 34, SECT_SIDE, VBT_MapAct.CAT_ROW, c.CatId);
		rb.Bg = bg;
		rb.FillHot = WHITE_08;
		string eye = "eye";
		int ec = VB_C_DIM;
		if (hidden)
		{
			eye = "eye_off";
			ec = VB_C_DIM2;
		}
		VBT_MapBtn eb = AddHit(b, 208, y + 4, 28, 28, SECT_SIDE, VBT_MapAct.CAT_EYE, c.CatId);
		eb.Img = VBT.Ico(b, 214, y + 11, 14, eye, ec);
		eb.Ink = ec;
		eb.InkHot = VB_C_TEXT;
		if (acc)
		{
			VBT_MapBtn ed = AddHit(b, 180, y + 4, 28, 28, SECT_SIDE, VBT_MapAct.CAT_EDIT, c.CatId);
			ed.Img = VBT.Ico(b, 186, y + 11, 14, "edit", VB_C_DIM);
			ed.Ink = VB_C_DIM;
			ed.InkHot = VB_C_TEXT;
		}
	}

	// Кто в сети: маленький значок со счётчиком, список по клику.
	protected void PaintMates()
	{
		DropPlate(m_ChipPlate);
		DropPlate(m_PopPlate);
		m_ChipPlate = null;
		m_PopPlate = null;
		m_MatesSwitch = null;
		DropBtns(SECT_MATES);
		if (!m_Client.Clan)
			return;
		int n = m_State.Mates.Count();
		m_ChipCount = n;
		bool on = m_State.LayerMates;
		m_ChipPlate = Plate(VW - 154, 12, 98, 36, 158);
		Widget c = m_ChipPlate.Root;
		int dot = VB_C_DIM2;
		if (on)
			dot = VB_C_OK;
		INT_UIF_Surface.Pill(c, 12, 14, 8, 8, dot);
		INT_UIF_Draw.Text(c, 28, 0, 66, 36, VBT.T("#STR_VB_TAB_D_ONLINE") + " " + n.ToString(), 13, VB_C_TEXT);
		AddHit(c, 0, 0, 98, 36, SECT_MATES, VBT_MapAct.CHIP);
		if (!m_MatesOpen)
			return;
		m_PopPlate = Plate(VW - 236, 54, 180, 44 + n * 30, 199);
		Widget p = m_PopPlate.Root;
		INT_UIF_Draw.Text(p, 12, 6, 100, 28, "#STR_VB_TAB_D_ON_MAP", 13, VB_C_DIM);
		m_MatesSwitch = new VBT_Switch(p, 124, 8, on, m_Handler);
		for (int i = 0; i < n; i++)
		{
			VB_MapMate m = m_State.Mates.Get(i);
			float y = 40 + i * 30;
			INT_UIF_Surface bg = INT_UIF_Surface.Box(p, 6, y, 168, 28, 5, 0);
			INT_UIF_Surface.Pill(p, 14, y + 10, 8, 8, VB_C_OK);
			INT_UIF_Draw.Text(p, 30, y, 84, 28, VBT.Fit(m.Who, 84, 13), 13, VB_C_TEXT);
			INT_UIF_Draw.Text(p, 104, y, 62, 28, VBT_MapUi.Dist(DistTo(m.X, m.Z)), 11, VB_C_DIM, INT_UIF_Align.RIGHT);
			VBT_MapBtn b = AddHit(p, 6, y, 168, 28, SECT_MATES, VBT_MapAct.MATE_ROW, i);
			b.Bg = bg;
			b.FillHot = WHITE_10;
		}
	}

	// Соклановцы сдвинулись: метки двигаются, виджеты пересоздаются, только если сменился состав.
	protected void MatesMoved()
	{
		int have = 0;
		// Соклановец: без объекта, размер 12 и не друг из пати (у друга тот же размер, его место ведёт пати).
		foreach (VBT_MapPin pin : m_Items)
		{
			if (!pin.Obj && pin.Size == 12 && pin.FriendId == "")
				have++;
		}
		int n = m_State.Mates.Count();
		if (!m_State.LayerMates)
			n = 0;
		if (have != n)
		{
			m_Need = m_Need | NEED_PINS | NEED_MATES;
			return;
		}
		int k = 0;
		foreach (VBT_MapPin item : m_Items)
		{
			if (item.Obj || item.Size != 12 || item.FriendId != "")
				continue;
			VB_MapMate m = m_State.Mates.Get(k);
			item.WX = m.X;
			item.WZ = m.Z;
			k++;
		}
		PlacePinsOnly();
		m_LabelsDue = true;
		if (m_MatesOpen || m_ChipCount != m_State.Mates.Count())
			m_Need = m_Need | NEED_MATES;
	}

	// --- окно категории ----------------------------------------------------------------------
	protected void OpenCatEdit(int id)
	{
		float w = 560;
		float h = 352;
		VB_MapCat cat = null;
		if (id > 0)
			cat = m_State.FindCat(id);
		m_EditId = id;
		string value = "";
		array<int> colors = VBK.Colors();
		m_EditColor = colors.Get(0);
		m_EditIcon = 0;
		if (cat)
		{
			value = VBT.T(cat.Title);
			m_EditColor = cat.Color;
			m_EditIcon = cat.IconIdx;
		}
		m_EditModal = new VBT_Modal(m_Menu, w, h, false);
		Widget b = m_EditModal.Body;
		string title = "#STR_VB_TAB_D_CAT_NEW";
		if (cat)
			title = "#STR_VB_TAB_D_CAT_TITLE";
		INT_UIF_Draw.Text(b, 28, 22, w - 56, 30, title, 22, VB_C_TEXT);
		m_EditField = new VBT_Input(b, 28, 64, w - 56, 46, "#STR_VB_TAB_D_CAT_HINT", VB_MAP_CAT_TITLE_MAX, value);
		VBT.Cap(b, 28, 122, w - 56, "#STR_VB_TAB_D_COLOR");
		VBT.Cap(b, 28, 196, w - 56, "#STR_VB_TAB_D_ICON");
		m_EditPal = INT_UIF_Draw.Frame(b, 28, 144, w - 56, 120);
		PaintEditPalette();
		ScriptedWidgetEventHandler handler = m_Handler;
		WrapBtn(new VBT_Button(b, w - 28 - 300, h - 64, 140, 44, "#STR_VB_TAB_CANCEL", false, "", handler), SECT_EDIT, VBT_MapAct.EDIT_CANCEL);
		WrapBtn(new VBT_Button(b, w - 28 - 150, h - 64, 150, 44, "#STR_VB_TAB_D_SAVE", true, "", handler), SECT_EDIT, VBT_MapAct.EDIT_SAVE);
		if (cat && cat.CatId != 1)
		{
			VBT_Button del = new VBT_Button(b, 28, h - 64, 140, 44, "#STR_VB_TAB_D_DELETE", false, "trash", handler);
			del.Danger = true;
			del.Bg.SetFill(VB_C_BAD);
			WrapBtn(del, SECT_EDIT, VBT_MapAct.EDIT_DELETE);
		}
	}

	protected void PaintEditPalette()
	{
		if (!m_EditPal)
			return;
		INT_UIF_Widgets.ClearChildren(m_EditPal);
		DropBtns(SECT_EDIT_PAL);
		array<int> colors = VBK.Colors();
		int n;
		for (n = 0; n < colors.Count(); n++)
		{
			float x = n * 42;
			int c = colors.Get(n);
			if (c == m_EditColor)
				INT_UIF_Surface.Pill(m_EditPal, x - 3, 3, 40, 40, 0, VB_C_TEXT);
			INT_UIF_Surface.Pill(m_EditPal, x + 1, 7, 32, 32, c);
			AddHit(m_EditPal, x, 6, 34, 34, SECT_EDIT_PAL, VBT_MapAct.EDIT_COLOR, n);
		}
		array<string> icons = VB_MapIcons.Cats();
		for (n = 0; n < icons.Count(); n++)
		{
			float ix = n * 42;
			bool on = m_EditIcon == n;
			int fill = VB_C_PANEL2;
			int line = VB_C_LINE2;
			int ink = VB_C_DIM;
			if (on)
			{
				fill = VB_C_PANEL3;
				line = VB_C_ACCENT;
				ink = m_EditColor;
			}
			INT_UIF_Surface.Box(m_EditPal, ix, 78, 36, 36, 8, fill, line);
			VBT.Ico(m_EditPal, ix + 8, 86, 20, icons.Get(n), ink);
			AddHit(m_EditPal, ix, 78, 36, 36, SECT_EDIT_PAL, VBT_MapAct.EDIT_ICON, n);
		}
	}

	protected void CloseCatEdit()
	{
		if (GetFocus())
			SetFocus(null);
		m_EditFieldOld = m_EditField;
		m_EditField = null;
		m_EditPal = null;
		DropBtns(SECT_EDIT);
		DropBtns(SECT_EDIT_PAL);
		if (m_EditModal)
			m_EditModal.Close();
		m_EditModal = null;
	}

	protected void SaveCatEdit()
	{
		string title = m_EditField.Get();
		if (title == "")
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_D_CAT_EMPTY"));
			return;
		}
		VB_MapCat c = new VB_MapCat();
		c.CatId = m_EditId;
		c.Title = title;
		c.Color = m_EditColor;
		c.IconIdx = m_EditIcon;
		ScriptRPC rpc = m_Client.Begin(VB_TabletOp.MAP_CAT_SAVE);
		c.Write(rpc);
		m_Client.Post(rpc);
		m_SavePending = true;
		CloseCatEdit();
	}

	protected void DeleteCatEdit()
	{
		int id = m_EditId;
		ScriptRPC rpc = m_Client.Begin(VB_TabletOp.MAP_CAT_DELETE);
		rpc.Write(id);
		m_Client.Post(rpc);
		m_SavePending = true;
		CloseCatEdit();
	}

	// --- клики -------------------------------------------------------------------------------
	override bool OnHit(Widget w)
	{
		if (!w)
			return false;
		if (w == m_Catch)
			return true;
		if (m_MatesSwitch && w == m_MatesSwitch.Hit)
		{
			m_MatesSwitch.Toggle();
			m_State.LayerMates = m_MatesSwitch.On;
			m_Need = m_Need | NEED_PINS | NEED_MATES;
			return true;
		}
		foreach (VBT_MapBtn b : m_Btns)
		{
			if (b.Hit == w)
			{
				DoAct(b.Act, b.Arg);
				return true;
			}
		}
		// Подложка панели: клик съеден, карта его не получит.
		foreach (VBT_MapPlate p : m_Plates)
		{
			if (Inside(p, m_MouseA, m_MouseB))
				return true;
		}
		return false;
	}

	override bool OnHover(Widget w, bool enter)
	{
		foreach (VBT_MapBtn b : m_Btns)
		{
			if (b.Hit != w)
				continue;
			b.Hover(enter);
			if (b.Tip != "")
			{
				if (enter)
					ShowTip(b);
				else
					ClearTip();
			}
			return true;
		}
		return false;
	}

	protected VB_MapObject RowObj(int index)
	{
		if (index < 0)
			return m_Sel;
		if (index >= m_Rows.Count())
			return null;
		return m_Rows.Get(index).Obj;
	}

	protected void DoAct(int act, int arg)
	{
		switch (act)
		{
			case VBT_MapAct.TOOL:
				SetTool(arg);
				break;
			case VBT_MapAct.DRAWER:
				s_Drawer = !s_Drawer;
				s_Top = 0;
				m_Need = m_Need | NEED_TOOLS | NEED_SIDE;
				break;
			case VBT_MapAct.LABELS:
				s_Labels = !s_Labels;
				m_Need = m_Need | NEED_TOOLS;
				m_LabelsDue = true;
				break;
			case VBT_MapAct.ZOOM_IN:
				ZoomBy(0.7, VW * 0.5, VH * 0.5);
				break;
			case VBT_MapAct.ZOOM_OUT:
				ZoomBy(1.4, VW * 0.5, VH * 0.5);
				break;
			case VBT_MapAct.CENTER_ME:
				vector me = GetGame().GetPlayer().GetPosition();
				CenterOn(me[0], me[2]);
				break;
			case VBT_MapAct.DEST:
				s_Dest = arg;
				m_Need = m_Need | NEED_DRAFT;
				break;
			case VBT_MapAct.UNDO:
				Undo();
				break;
			case VBT_MapAct.CANCEL:
				StopDraft();
				break;
			case VBT_MapAct.FINISH:
				Finish();
				break;
			case VBT_MapAct.CARD_CLOSE:
				CloseSel();
				break;
			case VBT_MapAct.CARD_EDIT:
				m_CardEdit = true;
				m_Need = m_Need | NEED_CARD;
				break;
			case VBT_MapAct.CARD_CENTER:
				CenterSel();
				break;
			case VBT_MapAct.CARD_DELETE:
				RemoveSel();
				break;
			case VBT_MapAct.PICK_ICON:
				if (m_Sel)
				{
					m_Sel.IconIdx = arg;
					SelChanged();
				}
				break;
			case VBT_MapAct.PICK_COLOR:
				array<int> colors = VBK.Colors();
				if (m_Sel)
				{
					m_Sel.Color = colors.Get(arg);
					SelChanged();
				}
				break;
			case VBT_MapAct.PICK_SCOPE:
				PickScope(arg);
				break;
			case VBT_MapAct.VIS:
				VB_MapObject vo = RowObj(arg);
				if (vo)
					Cycle(vo);
				break;
			case VBT_MapAct.SIDE_TAB:
				s_Side = arg;
				s_Top = 0;
				m_Need = m_Need | NEED_SIDE;
				break;
			case VBT_MapAct.SIDE_LAYER:
				ToggleLayer();
				break;
			case VBT_MapAct.SIDE_CLOSE:
				s_Drawer = false;
				m_Need = m_Need | NEED_TOOLS | NEED_SIDE;
				break;
			case VBT_MapAct.CAT_NEW:
				OpenCatEdit(0);
				break;
			case VBT_MapAct.ROW:
				VB_MapObject ro = RowObj(arg);
				if (ro)
				{
					float ax;
					float az;
					ro.AnchorOf(ax, az);
					CenterOn(ax, az);
					SelectObj(ro);
				}
				break;
			case VBT_MapAct.CAT_ROW:
				if (s_OpenCats.Find(arg) >= 0)
					s_OpenCats.RemoveItem(arg);
				else
					s_OpenCats.Insert(arg);
				s_CurCat = arg;
				m_Need = m_Need | NEED_SIDE;
				break;
			case VBT_MapAct.CAT_EYE:
				ToggleCat(arg);
				break;
			case VBT_MapAct.CAT_EDIT:
				OpenCatEdit(arg);
				break;
			case VBT_MapAct.CHIP:
				m_MatesOpen = !m_MatesOpen;
				if (m_MatesOpen && m_Sel)
					CloseSel();
				m_Need = m_Need | NEED_MATES;
				break;
			case VBT_MapAct.MATE_ROW:
				if (arg < m_State.Mates.Count())
				{
					VB_MapMate mate = m_State.Mates.Get(arg);
					CenterOn(mate.X, mate.Z);
				}
				break;
			case VBT_MapAct.EDIT_COLOR:
				array<int> palette = VBK.Colors();
				m_EditColor = palette.Get(arg);
				m_Need = m_Need | NEED_EDIT;
				break;
			case VBT_MapAct.EDIT_ICON:
				m_EditIcon = arg;
				m_Need = m_Need | NEED_EDIT;
				break;
			case VBT_MapAct.EDIT_SAVE:
				SaveCatEdit();
				break;
			case VBT_MapAct.EDIT_CANCEL:
				CloseCatEdit();
				break;
			case VBT_MapAct.EDIT_DELETE:
				DeleteCatEdit();
				break;
		}
	}

	protected void CenterSel()
	{
		if (!m_Sel)
			return;
		float x;
		float z;
		m_Sel.AnchorOf(x, z);
		CenterOn(x, z);
		Reveal(m_Sel);
	}

	// Карточка перерисуется в следующем кадре, текст названия сохраняется в объекте.
	protected void SelChanged()
	{
		m_SelDirty = true;
		m_Need = m_Need | NEED_CARD | NEED_PINS | NEED_SIDE;
		m_ShapesDirty = true;
	}

	protected void PickScope(int scope)
	{
		if (!m_Sel || m_Sel.Scope == scope)
			return;
		m_Sel.Scope = scope;
		if (scope == VB_MapScope.CLAN)
		{
			if (m_Sel.CatId == 0)
			{
				VB_MapCat cat = m_State.FindCat(s_CurCat);
				if (cat)
					m_Sel.CatId = cat.CatId;
			}
		}
		else
		{
			m_Sel.CatId = 0;
		}
		SelChanged();
	}

	protected void ToggleLayer()
	{
		if (s_Side == SIDE_CLAN)
			m_State.LayerClan = !m_State.LayerClan;
		else if (s_Side == SIDE_SERVER)
			m_State.LayerServer = !m_State.LayerServer;
		else
			m_State.LayerMine = !m_State.LayerMine;
		m_State.Version++;
	}

	protected void ToggleCat(int id)
	{
		int mode = VB_MapMode.HIDDEN;
		if (m_State.CatHidden(id))
		{
			m_State.HiddenCats.RemoveItem(id);
			mode = VB_MapMode.FLAT;
		}
		else
		{
			m_State.HiddenCats.Insert(id);
		}
		SendPref(VB_MapPref.CATEGORY, id, mode);
		m_State.Version++;
	}

	override void OnInput(VBT_Input field)
	{
		if (field != m_NameField || !m_Sel)
			return;
		string text = field.Get();
		// Пустое поле: новый объект получает имя по умолчанию, у готового остаётся прежнее (а не имя
		// другого объекта и не пустое, которое сервер отклонит вместе со всей правкой).
		if (text == "")
		{
			if (m_Sel.ObjId > 0 || m_DefaultTitle == "")
				return;
			text = m_DefaultTitle;
		}
		if (text == m_Sel.Title)
			return;
		m_Sel.Title = text;
		m_SelDirty = true;
		m_Need = m_Need | NEED_PINS | NEED_SIDE;
	}

	override void OnSubmit(VBT_Input field)
	{
		if (field == m_NameField)
			SetFocus(null);
	}

	override void OnPick(VBT_Dropdown list, int index)
	{
		if (list != m_CatDrop || !m_Sel || index >= m_CatDropCats.Count())
			return;
		VB_MapCat c = m_CatDropCats.Get(index);
		m_Sel.CatId = c.CatId;
		s_CurCat = c.CatId;
		m_SelDirty = true;
		m_Need = m_Need | NEED_SIDE;
	}

	// --- метки на карте ----------------------------------------------------------------------
	protected VBT_MapPin AddPin(VB_MapObject o, float wx, float wz, int fill, string icon, string label, float size)
	{
		VBT_MapPin pin = new VBT_MapPin();
		pin.Obj = o;
		pin.WX = wx;
		pin.WZ = wz;
		pin.Size = size;
		pin.Root = INT_UIF_Draw.Frame(m_Pins, 0, 0, 1, 1);
		if (fill != 0)
		{
			VBT_MapUi.Dot(pin.Root, size + 4, 0xCC0B0E0F);
			VBT_MapUi.Dot(pin.Root, size, fill);
			if (icon != "")
				VBT_MapUi.Glyph(pin.Root, 14, icon, INK_DARK);
		}
		pin.TagW = INT_UIF_Draw.TextWidth(label, 12) + 12;
		pin.Tag = INT_UIF_Draw.Frame(pin.Root, 0, 0, pin.TagW, 20);
		INT_UIF_Surface.Box(pin.Tag, 0, 0, pin.TagW, 20, 4, 0x9E0B0E0F);
		INT_UIF_Draw.Text(pin.Tag, 6, 0, pin.TagW - 6, 20, label, 12, VB_C_TEXT);
		m_Items.Insert(pin);
		return pin;
	}

	protected void PaintPins()
	{
		m_HotPin = null;
		INT_UIF_Widgets.ClearChildren(m_Pins);
		m_Items.Clear();
		foreach (VB_MapObject o : m_State.Objects)
		{
			if (!m_State.IsVisible(o))
				continue;
			float ax;
			float az;
			o.AnchorOf(ax, az);
			string title = VBT.T(o.Title);
			if (o.Kind == VB_MapKind.MARK)
				AddPin(o, ax, az, o.Color, VB_MapIcons.MarkName(o.IconIdx), title, 22);
			else
				AddPin(o, ax, az, 0, "", title, 0);
		}
		VB_Clan clan = m_Client.Clan;
		if (clan && m_State.LayerMates)
		{
			foreach (VB_MapMate m : m_State.Mates)
				AddPin(null, m.X, m.Z, VB_C_OK, "", m.Who, 12);
		}
		m_FriendSig = "";
		if (m_State.LayerMates)
			AddFriends(clan);
		PlacePinsOnly();
		m_ShapesDirty = true;
	}

	// Друзья из INT_PartyMode: те, кого игрок включил в показ, в сети и с известным местом. Соклановцы уже есть.
	protected void AddFriends(VB_Clan clan)
	{
#ifdef INT_PARTYMODE_CL
		if (!INT_PTY_Client.s_Friends)
			return;
		foreach (INT_PTY_Friend fr : INT_PTY_Client.s_Friends)
		{
			if (!ShowFriend(fr, clan))
				continue;
			vector at = fr.WorldPos();
			VBT_MapPin pin = AddPin(null, at[0], at[2], INT_PTY_UI.FriendColor(fr.Id), "", INT_PTY_Client.NameOf(fr.Id), 12);
			pin.FriendId = fr.Id;
			m_FriendSig = m_FriendSig + fr.Id + ";";
		}
#endif
	}

#ifdef INT_PARTYMODE_CL
	protected bool ShowFriend(INT_PTY_Friend fr, VB_Clan clan)
	{
		if (!fr || !fr.Online || fr.Dead || !fr.KnowsPos() || !INT_PTY_Client.IsShown(fr.Id))
			return false;
		if (clan)
		{
			foreach (VB_ClanMember m : clan.Members)
			{
				if (m.Uid == fr.Id)
					return false;
			}
		}
		return true;
	}
#endif

	// Раз в секунду: тот же набор друзей двигается на месте, другой набор перерисовывает метки.
	protected void TickFriends(float dt)
	{
#ifdef INT_PARTYMODE_CL
		m_FriendT = m_FriendT - dt;
		if (m_FriendT > 0)
			return;
		m_FriendT = 1;
		string sig = "";
		if (m_State.LayerMates && INT_PTY_Client.s_Friends)
		{
			foreach (INT_PTY_Friend fr : INT_PTY_Client.s_Friends)
			{
				if (ShowFriend(fr, m_Client.Clan))
					sig = sig + fr.Id + ";";
			}
		}
		if (sig != m_FriendSig)
		{
			m_Need = m_Need | NEED_PINS;
			return;
		}
		if (sig == "")
			return;
		foreach (VBT_MapPin pin : m_Items)
		{
			if (pin.FriendId == "")
				continue;
			foreach (INT_PTY_Friend f : INT_PTY_Client.s_Friends)
			{
				if (f && f.Id == pin.FriendId)
				{
					vector at = f.WorldPos();
					pin.WX = at[0];
					pin.WZ = at[2];
					break;
				}
			}
		}
		PlacePinsOnly();
		m_LabelsDue = true;
#endif
	}

	protected void PlacePinsOnly()
	{
		foreach (VBT_MapPin pin : m_Items)
		{
			float wx = pin.WX;
			float wz = pin.WZ;
			// Подпись зоны или маршрута идёт за точками при перетаскивании.
			if (pin.Obj && pin.Obj == m_Sel)
				pin.Obj.AnchorOf(wx, wz);
			pin.X = LocA(wx);
			pin.Y = LocB(wz);
			pin.Root.SetPos(pin.X * m_F, pin.Y * m_F);
		}
	}

	protected void PlaceAll()
	{
		PlacePinsOnly();
		PlaceHandles();
		m_ShapesDirty = true;
		m_LabelsDue = true;
		PaintScale();
	}

	protected void PaintMe()
	{
		vector at = GetGame().GetPlayer().GetPosition();
		m_MeLayer.SetPos(LocA(at[0]) * m_F, LocB(at[2]) * m_F);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		vector rot = player.GetYawPitchRoll();
		float heading = rot[0];
		if (heading < 0)
			heading = heading + 360;
		int rounded = Math.Round(heading);
		if (rounded == m_TapeHeading)
			return;
		m_TapeHeading = rounded;
		m_ArrowOut.SetRotation(0, 0, rounded);
		m_ArrowIn.SetRotation(0, 0, rounded);
		PaintTape(rounded);
	}

	protected void HoverPin()
	{
		VBT_MapPin hot = null;
		if (GetWidgetUnderCursor() == m_Catch)
		{
			for (int i = m_Items.Count() - 1; i >= 0 && !hot; i--)
			{
				VBT_MapPin pin = m_Items.Get(i);
				if (pin.Size > 0 && Hypot(m_MouseA - pin.X, m_MouseB - pin.Y) <= pin.Size * 0.5)
					hot = pin;
			}
		}
		if (hot == m_HotPin)
			return;
		m_HotPin = hot;
		m_LabelsDue = true;
		m_LabelsIn = 0;
	}

	protected bool BoxFree(array<float> boxes, float x, float y, float w, float h)
	{
		for (int i = 0; i < boxes.Count(); i = i + 4)
		{
			float bx = boxes.Get(i);
			float by = boxes.Get(i + 1);
			float bw = boxes.Get(i + 2);
			float bh = boxes.Get(i + 3);
			if (x < bx + bw + 3 && bx < x + w + 3 && y < by + bh + 2 && by < y + h + 2)
				return false;
		}
		return true;
	}

	// Подписи не наезжают: у каждой несколько мест, берём первое свободное, иначе прячем.
	// Выбранный объект виден всегда. При сдвиге карты считается реже: это самый дорогой шаг.
	protected void Declutter()
	{
		m_LabelsDue = false;
		m_LabelsIn = 0.12;
		array<float> used = new array<float>();
		vector me = GetGame().GetPlayer().GetPosition();
		used.Insert(LocA(me[0]) - 15);
		used.Insert(LocB(me[2]) - 15);
		used.Insert(30.0);
		used.Insert(30.0);
		foreach (VBT_MapPin dot : m_Items)
		{
			if (dot.Size <= 0 || !OnView(dot))
				continue;
			used.Insert(dot.X - dot.Size * 0.5);
			used.Insert(dot.Y - dot.Size * 0.5);
			used.Insert(dot.Size);
			used.Insert(dot.Size);
		}
		for (int rank = 3; rank >= 0; rank--)
		{
			foreach (VBT_MapPin pin : m_Items)
			{
				if (PinRank(pin) == rank)
					PlaceTag(pin, used);
			}
		}
	}

	protected bool OnView(VBT_MapPin pin)
	{
		return pin.X > -120 && pin.X < VW + 120 && pin.Y > -40 && pin.Y < VH + 40;
	}

	protected int PinRank(VBT_MapPin pin)
	{
		int r = 0;
		if (pin.Obj && pin.Obj == m_Sel)
			r = 2;
		if (pin.Size > 0)
			r = r + 1;
		return r;
	}

	protected void AddSpot(array<float> spots, float dx, float dy)
	{
		spots.Insert(dx);
		spots.Insert(dy);
	}

	protected void PlaceTag(VBT_MapPin pin, array<float> used)
	{
		bool must = pin == m_HotPin || (pin.Obj && pin.Obj == m_Sel);
		if (!OnView(pin) || (!s_Labels && !must))
		{
			pin.Tag.Show(false);
			return;
		}
		float w = pin.TagW;
		float h = 20;
		float s = pin.Size * 0.5;
		array<float> spots = new array<float>();
		if (pin.Size > 0)
		{
			AddSpot(spots, s + 4, -10);
			AddSpot(spots, -s - 4 - w, -10);
			AddSpot(spots, -w * 0.5, -s - 4 - h);
			AddSpot(spots, -w * 0.5, s + 4);
		}
		else
		{
			AddSpot(spots, -w * 0.5, -10);
			AddSpot(spots, -w * 0.5, -34);
			AddSpot(spots, -w * 0.5, 14);
		}
		int found = -1;
		for (int i = 0; i < spots.Count() && found < 0; i = i + 2)
		{
			float tx = pin.X + spots.Get(i);
			float ty = pin.Y + spots.Get(i + 1);
			if (BoxFree(used, tx, ty, w, h))
				found = i;
		}
		if (found < 0 && must)
			found = 0;
		if (found < 0)
		{
			pin.Tag.Show(false);
			return;
		}
		float dx = spots.Get(found);
		float dy = spots.Get(found + 1);
		pin.Tag.SetPos(dx * m_F, dy * m_F);
		pin.Tag.Show(true);
		used.Insert(pin.X + dx);
		used.Insert(pin.Y + dy);
		used.Insert(w);
		used.Insert(h);
	}

	// Ручки: готовые круги, двигаются вместе с картой.
	protected void GrowHandles(int count)
	{
		while (m_HandlePool.Count() < count)
		{
			Widget f = INT_UIF_Draw.Frame(m_HandleLayer, 0, 0, 1, 1);
			m_HandleOut.Insert(VBT_MapUi.Dot(f, 16, INK_DARK));
			m_HandleIn.Insert(VBT_MapUi.Dot(f, 12, VB_C_WHITE));
			m_HandlePool.Insert(f);
		}
	}

	// r: радиус круга, как в макете: ручка 6, точка наброска 5, первая точка 8 и цвета наброска.
	protected void PutHandle(int i, float x, float z, float r, int color)
	{
		GrowHandles(i + 1);
		Widget f = m_HandlePool.Get(i);
		VBT_MapUi.Center(m_HandleOut.Get(i), r * 2 + 4);
		ImageWidget inner = m_HandleIn.Get(i);
		VBT_MapUi.Center(inner, r * 2);
		inner.SetColor(color);
		f.SetPos(LocA(x) * m_F, LocB(z) * m_F);
		f.Show(true);
	}

	protected void PlaceHandles()
	{
		int used = 0;
		if (m_DraftKind == VB_MapKind.CIRCLE)
		{
			used = 0;
		}
		else if (m_DraftKind >= 0)
		{
			int n = m_DraftPts.Count() / 2;
			for (int d = 0; d < n; d++)
			{
				float hx = m_DraftPts.Get(d * 2);
				float hz = m_DraftPts.Get(d * 2 + 1);
				if (d == 0)
					PutHandle(used, hx, hz, 8, DraftColor());
				else
					PutHandle(used, hx, hz, 5, VB_C_WHITE);
				used++;
			}
		}
		else if (m_Sel && m_Sel.Kind != VB_MapKind.MARK && CanEdit(m_Sel))
		{
			if (m_Sel.Kind == VB_MapKind.CIRCLE)
			{
				float rx = m_Sel.PX(0) + m_Sel.Radius;
				PutHandle(used, rx, m_Sel.PZ(0), 6, VB_C_WHITE);
				used++;
			}
			else
			{
				for (int k = 0; k < m_Sel.PointCount(); k++)
				{
					PutHandle(used, m_Sel.PX(k), m_Sel.PZ(k), 6, VB_C_WHITE);
					used++;
				}
			}
		}
		for (int i = used; i < m_HandlePool.Count(); i++)
			m_HandlePool.Get(i).Show(false);
	}

	// --- линии -------------------------------------------------------------------------------
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

	// Отрезок обрезается по окну: при сильном приближении координаты уходят далеко за экран.
	protected void Seg(float x1, float y1, float x2, float y2, float w, int color)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		float t0 = 0;
		float t1 = 1;
		if (!ClipT(-dx, x1, t0, t1) || !ClipT(dx, VW - x1, t0, t1) || !ClipT(-dy, y1, t0, t1) || !ClipT(dy, VH - y1, t0, t1))
			return;
		if (t1 <= t0)
			return;
		float f = m_F;
		m_Canvas.DrawLine((x1 + t0 * dx) * f, (y1 + t0 * dy) * f, (x1 + t1 * dx) * f, (y1 + t1 * dy) * f, w * f, color);
	}

	// Пунктир идёт от начала ломаной, поэтому при сдвиге карты штрихи не плывут.
	protected void Dashed(array<float> xs, array<float> ys, float on, float off, float w, int color)
	{
		float period = on + off;
		float passed = 0;
		for (int i = 1; i < xs.Count(); i++)
		{
			float x1 = xs.Get(i - 1);
			float y1 = ys.Get(i - 1);
			float dx = xs.Get(i) - x1;
			float dy = ys.Get(i) - y1;
			float len = Hypot(dx, dy);
			if (len <= 0)
				continue;
			float t0 = 0;
			float t1 = 1;
			bool seen = ClipT(-dx, x1, t0, t1) && ClipT(dx, VW - x1, t0, t1) && ClipT(-dy, y1, t0, t1) && ClipT(dy, VH - y1, t0, t1);
			if (seen && t1 > t0)
			{
				float from = passed + t0 * len;
				float till = passed + t1 * len;
				int k = Math.Floor(from / period);
				float s = k * period;
				while (s < till)
				{
					float a = Math.Max(s, from) - passed;
					float b = Math.Min(s + on, till) - passed;
					if (b > a)
						Seg(x1 + dx * a / len, y1 + dy * a / len, x1 + dx * b / len, y1 + dy * b / len, w, color);
					s = s + period;
				}
			}
			passed = passed + len;
		}
	}

	protected void Solid(array<float> xs, array<float> ys, bool closed, float w, int color)
	{
		int n = xs.Count();
		for (int i = 1; i < n; i++)
			Seg(xs.Get(i - 1), ys.Get(i - 1), xs.Get(i), ys.Get(i), w, color);
		if (closed && n > 2)
			Seg(xs.Get(n - 1), ys.Get(n - 1), xs.Get(0), ys.Get(0), w, color);
	}

	// Штриховка косыми линиями x + y = c, как узор макета: шаг 9 пикселей, толщина 3.
	protected void HatchPoly(array<float> xs, array<float> ys, int color)
	{
		const float STEP = 12.728;
		int n = xs.Count();
		float lo = 100000;
		float hi = -100000;
		int i;
		for (i = 0; i < n; i++)
		{
			float sum = xs.Get(i) + ys.Get(i);
			lo = Math.Min(lo, sum);
			hi = Math.Max(hi, sum);
		}
		lo = Math.Max(lo, 0);
		hi = Math.Min(hi, VW + VH);
		int first = Math.Ceil(lo / STEP);
		float c = first * STEP;
		while (c <= hi)
		{
			m_CutX.Clear();
			m_CutY.Clear();
			for (i = 0; i < n; i++)
			{
				int next = (i + 1) % n;
				float x1 = xs.Get(i);
				float y1 = ys.Get(i);
				float x2 = xs.Get(next);
				float y2 = ys.Get(next);
				float f1 = x1 + y1 - c;
				float f2 = x2 + y2 - c;
				bool below1 = f1 < 0;
				bool below2 = f2 < 0;
				if (below1 != below2)
				{
					float t = f1 / (f1 - f2);
					InsertCut(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
				}
			}
			for (i = 0; i + 1 < m_CutX.Count(); i = i + 2)
				Seg(m_CutX.Get(i), m_CutY.Get(i), m_CutX.Get(i + 1), m_CutY.Get(i + 1), 3, color);
			c = c + STEP;
		}
	}

	protected void InsertCut(float x, float y)
	{
		int at = 0;
		while (at < m_CutX.Count() && m_CutX.Get(at) < x)
			at++;
		m_CutX.InsertAt(x, at);
		m_CutY.InsertAt(y, at);
	}

	protected void HatchCircle(float cx, float cy, float r, int color)
	{
		const float STEP = 12.728;
		const float ROOT2 = 1.41421356;
		float mid = cx + cy;
		float lo = Math.Max(mid - r * ROOT2, 0);
		float hi = Math.Min(mid + r * ROOT2, VW + VH);
		int first = Math.Ceil(lo / STEP);
		float c = first * STEP;
		while (c <= hi)
		{
			float d = (c - mid) / ROOT2;
			float half = r * r - d * d;
			if (half > 0)
			{
				half = Math.Sqrt(half) / ROOT2;
				float px = cx + d / ROOT2;
				float py = cy + d / ROOT2;
				Seg(px - half, py + half, px + half, py - half, 3, color);
			}
			c = c + STEP;
		}
	}

	protected void CirclePoints(float cx, float cy, float r)
	{
		m_LineX.Clear();
		m_LineY.Clear();
		for (int i = 0; i <= 64; i++)
		{
			float ang = i * Math.PI2 / 64;
			m_LineX.Insert(cx + r * Math.Cos(ang));
			m_LineY.Insert(cy + r * Math.Sin(ang));
		}
	}

	protected void ObjPoints(VB_MapObject o)
	{
		m_LineX.Clear();
		m_LineY.Clear();
		for (int i = 0; i < o.PointCount(); i++)
		{
			m_LineX.Insert(LocA(o.PX(i)));
			m_LineY.Insert(LocB(o.PZ(i)));
		}
	}

	protected void PaintObject(VB_MapObject o)
	{
		float w = 2;
		if (m_Sel == o)
			w = 3;
		int hatch = INT_UIF_Color.WithAlpha(o.Color, HATCH_ALPHA);
		if (o.Kind == VB_MapKind.ZONE)
		{
			ObjPoints(o);
			HatchPoly(m_LineX, m_LineY, hatch);
			Solid(m_LineX, m_LineY, true, w, o.Color);
		}
		else if (o.Kind == VB_MapKind.CIRCLE)
		{
			float cx = LocA(o.PX(0));
			float cy = LocB(o.PZ(0));
			float r = o.Radius * m_Kx;
			HatchCircle(cx, cy, r, hatch);
			CirclePoints(cx, cy, r);
			Solid(m_LineX, m_LineY, false, w, o.Color);
		}
		else if (o.Kind == VB_MapKind.ROUTE)
		{
			ObjPoints(o);
			Solid(m_LineX, m_LineY, false, w + 3, 0x8C0E0D0C);
			Dashed(m_LineX, m_LineY, 12, 7, w + 1, o.Color);
		}
	}

	protected int DraftColor()
	{
		if (s_Dest == VB_MapScope.CLAN && m_Client.Clan)
			return m_Client.Clan.Color;
		return VB_C_ACCENT;
	}

	protected void PaintDraftShape()
	{
		int col = DraftColor();
		if (m_DraftKind == VB_MapKind.CIRCLE)
		{
			CirclePoints(LocA(m_DraftPts.Get(0)), LocB(m_DraftPts.Get(1)), m_DraftR * m_Kx);
			Dashed(m_LineX, m_LineY, 8, 6, 2, col);
			return;
		}
		m_LineX.Clear();
		m_LineY.Clear();
		for (int i = 0; i < m_DraftPts.Count(); i = i + 2)
		{
			m_LineX.Insert(LocA(m_DraftPts.Get(i)));
			m_LineY.Insert(LocB(m_DraftPts.Get(i + 1)));
		}
		if (m_DraftHasCur && m_DraftPts.Count() > 0)
		{
			m_LineX.Insert(LocA(m_DraftCurX));
			m_LineY.Insert(LocB(m_DraftCurZ));
		}
		Dashed(m_LineX, m_LineY, 8, 6, 2, col);
	}

	// Контур и штриховка: то, что умеет CanvasWidget.
	protected void PaintShapes()
	{
		m_ShapesDirty = false;
		m_Canvas.Clear();
		foreach (VB_MapObject o : m_State.Objects)
		{
			if (o.Kind != VB_MapKind.MARK && m_State.IsVisible(o))
				PaintObject(o);
		}
		if (m_DraftKind >= 0 && m_DraftPts.Count() > 0)
			PaintDraftShape();
	}

	// --- нижняя полоса -----------------------------------------------------------------------
	protected void BuildCompass()
	{
		float w = 440;
		VBT_MapPlate plate = Plate((VW - w) * 0.5, VH - 40, w, 28, 158);
		Widget f = plate.Root;
		VBT.Ico(f, 10, 7, 14, "gps", VB_C_ACCENT);
		m_CoordsText = INT_UIF_Draw.Text(f, 30, 0, 114, 28, "", 12, VB_C_TEXT);
		INT_UIF_Draw.Line(f, 148, 6, 1, 16, WHITE_16);
		Widget t = INT_UIF_Draw.Clip(f, 154, 0, 180, 28);
		int i;
		for (i = 0; i < 19; i++)
			m_TapeLines.Insert(INT_UIF_Draw.Line(t, 0, 0, 1, 5, VB_C_DIM));
		for (i = 0; i < 3; i++)
			m_TapeTexts.Insert(INT_UIF_Draw.Text(t, 0, 1, 32, 16, "", 11, VB_C_TEXT, INT_UIF_Align.CENTER));
		INT_UIF_Draw.Line(t, 89, 16, 2, 12, VB_C_ACCENT);
		INT_UIF_Draw.Line(f, 340, 6, 1, 16, WHITE_16);
		m_ScaleText = INT_UIF_Draw.Text(f, 348, 1, 86, 16, "", 11, VB_C_TEXT);
		m_ScaleBar = INT_UIF_Draw.Line(f, 348, 20, 40, 2, VB_C_TEXT);
	}

	// Лента компаса: виджеты заранее созданы и только переставляются.
	protected void PaintTape(int heading)
	{
		int low = Math.Ceil((heading - 45) / 5.0);
		int start = low * 5;
		int label = 0;
		for (int i = 0; i < 19; i++)
		{
			ImageWidget tick = m_TapeLines.Get(i);
			int deg = start + i * 5;
			if (deg > heading + 45)
			{
				tick.Show(false);
				continue;
			}
			float x = 90 + (deg - heading) * 2;
			int norm = ((deg % 360) + 360) % 360;
			bool major = norm % 45 == 0;
			tick.Show(true);
			if (major)
			{
				INT_UIF_Draw.Place(tick, x, 18, 1, 8);
				tick.SetColor(VB_C_TEXT);
				if (label < 3)
				{
					TextWidget word = m_TapeTexts.Get(label);
					label++;
					INT_UIF_Draw.Place(word, x - 16, 1, 32, 16);
					int dir = norm / 45;
					word.SetText("#STR_VB_TAB_D_DIR_" + dir.ToString());
					int ink = VB_C_TEXT;
					if (norm == 0)
						ink = VB_C_ACCENT;
					word.SetColor(ink);
					word.Show(true);
				}
			}
			else
			{
				INT_UIF_Draw.Place(tick, x, 21, 1, 5);
				tick.SetColor(VB_C_DIM);
			}
		}
		for (int k = label; k < 3; k++)
			m_TapeTexts.Get(k).Show(false);
	}

	protected void PaintScale()
	{
		array<int> steps = {10, 25, 50, 100, 250, 500, 1000, 2500};
		int v = 10;
		foreach (int s : steps)
		{
			if (s * m_Kx <= 80)
				v = s;
		}
		INT_UIF_Draw.Resize(m_ScaleBar, v * m_Kx, 2);
		string text;
		if (v >= 1000)
		{
			float km = v / 1000.0;
			if (v == 2500)
				text = "2,5 " + VBT.T("#STR_VB_TAB_KM");
			else
			{
				int whole = km;
				text = whole.ToString() + " " + VBT.T("#STR_VB_TAB_KM");
			}
		}
		else
		{
			text = v.ToString() + " " + VBT.T("#STR_VB_TAB_D_M");
		}
		m_ScaleText.SetText(text);
	}

	protected void ShowCoords()
	{
		float x;
		float z;
		if (m_MouseA >= 0 && m_MouseB >= 0 && m_MouseA <= VW && m_MouseB <= VH)
		{
			x = WorldX(m_MouseA);
			z = WorldZ(m_MouseB);
		}
		else
		{
			vector me = GetGame().GetPlayer().GetPosition();
			x = me[0];
			z = me[2];
		}
		string text = VBT_MapUi.Coords(x, z);
		if (text == m_CoordsShown)
			return;
		m_CoordsShown = text;
		m_CoordsText.SetText(text);
	}
}
