// ============================================================================
// DRL_CraftMenu - «Раскрой»: центр окна у швейной машинки (DRL_Craft_Sewing),
// макет «макет верстака DRL».
//
// Слева манекен: живой персонаж в вещах сета (PlayerPreviewWidget; сам
// персонаж - локальный PlayerBase только на этом клиенте, спрятан под землёй,
// без симуляции) и метки частей сета (клик - рецепт части). Справа расцветки
// лоскутами, материалы бирками, инструменты и навесы машинки, навесное, защита.
// Данные те же, что у «Чертежа» (DRL_UIBlueprint), меняется только вид.
// ============================================================================

class DRL_UISetPart extends Managed
{
	int m_RecipeId = -1;
	string m_Name;
	bool m_Ok;
	bool m_Selected;
	bool m_Locked;
}

// --- манекен ---------------------------------------------------------------------------
class DRL_UIMannequin extends ScriptedWidgetEventHandler
{
	static const string LAYOUT = "DRL_CraftMenu/UI/layout/DRL_PlayerPreview.layout";
	static const float ZOOM_MIN = 0.85;
	static const float ZOOM_MAX = 2.2;

	protected Widget m_Root;
	protected Widget m_Host;
	protected PlayerPreviewWidget m_Preview;
	protected ButtonWidget m_Hit;
	protected Widget m_Missing;
	protected PlayerBase m_Doll;
	protected string m_OutfitKey;
	protected float m_W;
	protected float m_H;
	protected bool m_Dragging;
	protected int m_DragX;
	protected float m_DragYaw;
	protected float m_Yaw;
	protected float m_Zoom = 1;
	protected bool m_Hidden;

	void ~DRL_UIMannequin()
	{
		Clear();
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Mannequin");
		m_Host = INT_UIF_Draw.Clip(m_Root, 0, 0, w, h, "DRL_MannequinClip");
		Widget pw = INT_UIF_Widgets.CreatePath(LAYOUT, m_Host);
		m_Preview = PlayerPreviewWidget.Cast(pw);
		int faint = DRL_UIC.Dim();
		m_Missing = INT_UIF_Draw.Frame(m_Root, 0, 0, w, h, "DRL_MannequinMissing");
		INT_UIF_Draw.Icon(m_Missing, w * 0.5 - 18, h * 0.5 - 30, 36, "info", faint);
		INT_UIF_Draw.Text(m_Missing, 0, h * 0.5 + 12, w, 20, "#STR_DRL_UI_NO_PREVIEW", 13, faint, INT_UIF_Align.CENTER);
		m_Missing.Show(false);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		m_Hit.SetName("DRL_MannequinHit");
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	// Одеть манекен; тот же набор вещей не пересоздаётся.
	void SetOutfit(array<string> classes)
	{
		string key = "";
		if (classes)
		{
			foreach (string c : classes)
			{
				key = key + c + ";";
			}
		}
		if (key == m_OutfitKey && m_Doll)
			return;
		m_OutfitKey = key;
		if (!m_Preview || !EnsureDoll())
		{
			m_Missing.Show(true);
			return;
		}
		m_Missing.Show(false);
		Dress(classes);
		m_Preview.SetPlayer(m_Doll);
		m_Preview.Refresh();
		ApplyView();
		m_Host.Show(!m_Hidden);
	}

	protected bool EnsureDoll()
	{
		if (m_Doll)
			return true;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		string type = "SurvivorM_Mirek";
		vector pos = "0 0 0";
		if (me)
		{
			type = me.GetType();
			pos = me.GetPosition();
		}
		// Глубоко под землёй и без симуляции: в мире его не видно и не слышно.
		pos[1] = pos[1] - 300;
		Object obj = GetGame().CreateObjectEx(type, pos, ECE_LOCAL | ECE_NOLIFETIME);
		m_Doll = PlayerBase.Cast(obj);
		if (!m_Doll)
		{
			if (obj)
				GetGame().ObjectDelete(obj);
			return false;
		}
		m_Doll.DisableSimulation(true);
		return true;
	}

	protected void Dress(array<string> classes)
	{
		GameInventory inv = m_Doll.GetInventory();
		if (!inv)
			return;
		for (int i = inv.AttachmentCount() - 1; i >= 0; i--)
		{
			EntityAI a = inv.GetAttachmentFromIndex(i);
			if (a)
				GetGame().ObjectDelete(a);
		}
		if (!classes)
			return;
		foreach (string cls : classes)
		{
			if (cls == "" || !GetGame().ConfigIsExisting("CfgVehicles " + cls))
				continue;
			inv.CreateAttachment(cls);
		}
	}

	// Базовый масштаб меньше области (при 1.0 макушку срезало) и небольшой сдвиг вниз:
	// персонаж стоит целиком, с полем сверху. Колесо увеличивает от этой базы.
	static const float BASE_SIZE = 0.86;
	static const float BASE_DROP = 0.03;
	// Превью обрезает модель по своему прямоугольнику: он шире области (лишнее режет
	// клип области), а масштаб модели задаёт только высота. Поза вполоборота стоит
	// левее центра - сдвиг вправо выравнивает поля слева и справа.
	static const float VIEW_W = 1.2;
	static const float SHIFT_X = 0.06;

	protected void ApplyView()
	{
		if (!m_Preview)
			return;
		m_Preview.SetModelPosition(Vector(0, 0, 0.605));
		m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		float size = BASE_SIZE * m_Zoom;
		// При SetSize(size, size) виджет сужался и срезал руку по своему краю.
		m_Preview.SetSize(VIEW_W, size);
		// Размер и позиция относительные (layout без exact): по горизонтали центр (center_ref),
		// по вертикали - по центру области и чуть ниже; увеличение идёт от центра.
		float drop = (1 - size) * 0.5 + BASE_DROP;
		m_Preview.SetPos(SHIFT_X, drop);
	}

	void ResetView()
	{
		m_Yaw = 0;
		m_Zoom = 1;
		ApplyView();
	}

	// Панель поверх: 3D рисуется над 2D, прячем.
	void Show(bool show)
	{
		m_Hidden = !show;
		if (m_Host)
			m_Host.Show(show);
	}

	void Tick(float dt)
	{
		if (!m_Dragging)
			return;
		if (!(GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK))
		{
			m_Dragging = false;
			return;
		}
		int mx;
		int my;
		GetMousePos(mx, my);
		m_Yaw = m_DragYaw + (mx - m_DragX) * 0.5;
		ApplyView();
	}

	void Clear()
	{
		if (m_Preview)
			m_Preview.SetPlayer(null);
		if (m_Doll && GetGame())
			GetGame().ObjectDelete(m_Doll);
		m_Doll = null;
		m_OutfitKey = "";
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		m_Dragging = true;
		int my;
		GetMousePos(m_DragX, my);
		m_DragYaw = m_Yaw;
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_Hit)
			return false;
		m_Zoom = Math.Clamp(m_Zoom + wheel * 0.1, ZOOM_MIN, ZOOM_MAX);
		ApplyView();
		return true;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit)
			return false;
		ResetView();
		return true;
	}
}

// --- лоскут расцветки ------------------------------------------------------------------
class DRL_UICutSwatch extends Managed
{
	static const float W = 118;
	static const float H = 64;

	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref INT_UIF_Surface m_Patch;
	TextWidget m_Name;
	ButtonWidget m_Hit;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, W, H, "DRL_CutSwatch");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, W, H, 10, 0, 0);
		m_Patch = INT_UIF_Surface.Box(m_Root, 6, 6, W - 12, 34, 6, 0, 0);
		int muted = DRL_UIC.Muted();
		m_Name = INT_UIF_Draw.Text(m_Root, 4, 42, W - 8, 18, "", 11, muted, INT_UIF_Align.CENTER);
		m_Name.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, W, H, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_BpVar_" + slot.ToString(), handler);
		m_Root.Show(false);
	}

	void Place(float x, float y, string name, int color, bool selected)
	{
		m_Root.Show(true);
		INT_UIF_Draw.Move(m_Root, x, y);
		m_Name.SetText(name);
		int fill = DRL_UIC.White(10);
		int border = DRL_UIC.White(20);
		int fg = DRL_UIC.Muted();
		if (selected)
		{
			fill = DRL_UIC.A(20);
			border = DRL_UIC.A();
			fg = DRL_UIC.Txt();
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Name.SetColor(fg);
		int patchBorder = DRL_UIC.White(50);
		m_Patch.SetFill(color);
		m_Patch.SetBorder(patchBorder);
	}

	void Hide()
	{
		m_Root.Show(false);
	}
}

// --- бирка материала -------------------------------------------------------------------
class DRL_UICutTag extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Hole;
	Widget m_Clip;
	ref DRL_UIItemView m_View;
	ImageWidget m_Glyph;
	TextWidget m_Name;
	TextWidget m_Value;
	TextWidget m_Need;
	Widget m_Swap;
	ButtonWidget m_Hit;
	ref DRL_UIBpItem m_Data;
	bool m_HasModel;
	bool m_ModelsHidden;
	bool m_Hover;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 200, 48, "DRL_CutTag");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 200, 48, 8, 0, 0);
		int hole = ARGB(255, 15, 26, 21);
		m_Hole = INT_UIF_Draw.Image(m_Root, 7, 20, 8, 8, "iuf_circle16", hole);
		m_Clip = INT_UIF_Draw.Clip(m_Root, 20, 8, 32, 32, "DRL_CutTagClip");
		m_View = new DRL_UIItemView();
		m_View.Create(m_Clip, true);
		m_View.SetOrientation(Vector(0, 0, 0));
		int ink = ARGB(255, 58, 46, 24);
		m_Glyph = INT_UIF_Draw.Icon(m_Root, 27, 15, 18, "gear", ink);
		int dark = ARGB(255, 42, 33, 18);
		m_Name = INT_UIF_Draw.Text(m_Root, 58, 6, 130, 18, "", 11, dark);
		m_Name.SetBold(true);
		m_Value = INT_UIF_Draw.Text(m_Root, 58, 24, 60, 20, "", 13, dark);
		m_Value.SetBold(true);
		int needColor = ARGB(255, 122, 106, 74);
		m_Need = INT_UIF_Draw.Text(m_Root, 90, 25, 80, 18, "", 10, needColor);
		m_Need.SetBold(true);
		m_Swap = INT_UIF_Draw.Frame(m_Root, 180, 5, 15, 15, "DRL_CutTagSwap");
		int swapFill = ARGB(255, 42, 33, 18);
		INT_UIF_Surface.Box(m_Swap, 0, 0, 15, 15, 4, swapFill, 0);
		int swapIcon = ARGB(255, 239, 227, 199);
		ImageWidget swapGlyph = INT_UIF_Draw.Icon(m_Swap, 3, 3, 9, "swap", swapIcon);
		DRL_UIKitUtil.SetSortDeep(m_Swap, DRL_UI_SORT_OVER_MODEL);
		swapGlyph.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_Swap.Show(false);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 200, 48, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Bp_" + slot.ToString(), handler);
		m_Root.Show(false);
	}

	void Place(DRL_UIBpItem d, float x, float y, float w)
	{
		m_Data = d;
		m_Root.Show(true);
		INT_UIF_Draw.Place(m_Root, x, y, w, 48);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, 48);
		INT_UIF_Draw.Resize(m_Hit, w, 48);
		INT_UIF_Draw.Move(m_Swap, w - 20, 5);

		m_HasModel = false;
		if (d.m_Preview != "")
			m_HasModel = m_View.SetClass(d.m_Preview);
		else
			m_View.Clear();
		m_View.Show(m_HasModel && !m_ModelsHidden);
		// SetIcon фреймворка сам показывает значок: сначала значок, потом видимость.
		INT_UIF_Widgets.SetIcon(m_Glyph, d.m_Glyph);
		m_Glyph.Show(!m_HasModel);
		m_Swap.Show(d.m_Variants && !m_ModelsHidden);

		float tw = w - 58 - 10;
		if (d.m_Variants)
			tw = tw - 14;
		INT_UIF_Draw.Place(m_Name, 58, 6, tw, 18);
		m_Name.SetText(d.m_Name);
		m_Value.SetText(d.m_Have);
		float vw = DRL_UIMeasure.TextW(m_Root, d.m_Have, 13, true);
		INT_UIF_Draw.Place(m_Value, 58, 24, vw + 4, 20);
		INT_UIF_Draw.Place(m_Need, 58 + vw + 4, 25, w - 58 - vw - 12, 18);
		m_Need.SetText(d.m_Need);
		Paint();
	}

	void SetHover(bool hover)
	{
		if (m_Hover == hover)
			return;
		m_Hover = hover;
		Paint();
	}

	protected void Paint()
	{
		if (!m_Data)
			return;
		int fill = ARGB(255, 239, 227, 199);
		int value = ARGB(255, 47, 122, 34);
		if (!m_Data.m_Ok)
		{
			fill = ARGB(255, 243, 201, 192);
			value = ARGB(255, 179, 38, 30);
		}
		int border = 0;
		if (m_Data.m_Selected)
			border = DRL_UIC.A();
		else if (m_Hover)
			border = ARGB(255, 185, 165, 123);
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Value.SetColor(value);
	}

	void ShowModel(bool show)
	{
		m_ModelsHidden = !show;
		m_View.Show(show && m_HasModel);
		if (m_Data)
			m_Swap.Show(show && m_Data.m_Variants);
	}

	void Hide()
	{
		m_Data = null;
		m_Root.Show(false);
		m_View.Clear();
		m_HasModel = false;
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_HasModel = false;
	}
}

// --- инструмент или навес машинки ------------------------------------------------------
class DRL_UICutTool extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	Widget m_Clip;
	ref DRL_UIItemView m_View;
	ImageWidget m_Glyph;
	TextWidget m_Name;
	TextWidget m_Sub;
	ButtonWidget m_Hit;
	ref DRL_UIBpItem m_Data;
	bool m_HasModel;
	bool m_ModelsHidden;

	void Build(Widget parent, int slot, ScriptedWidgetEventHandler handler)
	{
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, 160, 40, "DRL_CutTool");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 160, 40, 9, 0, 0);
		m_Clip = INT_UIF_Draw.Clip(m_Root, 6, 6, 28, 28, "DRL_CutToolClip");
		m_View = new DRL_UIItemView();
		m_View.Create(m_Clip, true);
		m_View.SetOrientation(Vector(0, 0, 0));
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		m_Glyph = INT_UIF_Draw.Icon(m_Root, 11, 11, 18, "key", txt);
		m_Name = INT_UIF_Draw.Text(m_Root, 40, 4, 110, 17, "", 11, txt);
		m_Name.SetBold(true);
		m_Sub = INT_UIF_Draw.Text(m_Root, 40, 20, 110, 16, "", 10, muted);
		m_Sub.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 160, 40, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_CutTool_" + slot.ToString(), handler);
		m_Root.Show(false);
	}

	float Measure(DRL_UIBpItem d)
	{
		float nw = DRL_UIMeasure.TextW(m_Root, d.m_Name, 11, true);
		float sw = DRL_UIMeasure.TextW(m_Root, d.m_Sub, 10, true);
		if (sw > nw)
			nw = sw;
		return nw + 52;
	}

	void Place(DRL_UIBpItem d, float x, float y, float w)
	{
		m_Data = d;
		m_Root.Show(true);
		INT_UIF_Draw.Place(m_Root, x, y, w, 40);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, 40);
		INT_UIF_Draw.Resize(m_Hit, w, 40);
		INT_UIF_Draw.Resize(m_Name, w - 46, 17);
		INT_UIF_Draw.Resize(m_Sub, w - 46, 16);
		m_HasModel = false;
		if (d.m_Preview != "")
			m_HasModel = m_View.SetClass(d.m_Preview);
		else
			m_View.Clear();
		m_View.Show(m_HasModel && !m_ModelsHidden);
		// SetIcon фреймворка сам показывает значок: сначала значок, потом видимость.
		INT_UIF_Widgets.SetIcon(m_Glyph, d.m_Glyph);
		m_Glyph.Show(!m_HasModel);
		m_Name.SetText(d.m_Name);
		m_Sub.SetText(d.m_Sub);

		int fill = DRL_UIC.White(10);
		int border = DRL_UIC.White(50);
		int sub = DRL_UIC.Muted();
		int glyph = ARGB(255, 217, 226, 232);
		if (d.m_Kind == DRL_BP_ATT)
		{
			fill = ARGB(180, 4, 22, 26);
			border = DRL_UIC.Tl(140);
			sub = DRL_UIC.Tl();
			glyph = DRL_UIC.Tl();
		}
		if (!d.m_Ok)
		{
			fill = ARGB(205, 40, 10, 12);
			border = DRL_UIC.No(178);
			sub = DRL_UIC.NoText();
			glyph = ARGB(255, 255, 138, 138);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Sub.SetColor(sub);
		m_Glyph.SetColor(glyph);
	}

	void ShowModel(bool show)
	{
		m_ModelsHidden = !show;
		m_View.Show(show && m_HasModel);
	}

	void Hide()
	{
		m_Data = null;
		m_Root.Show(false);
		m_View.Clear();
		m_HasModel = false;
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_HasModel = false;
	}
}

// --- лист «Раскрой» --------------------------------------------------------------------
class DRL_UICutSheet extends DRL_UIBlueprint
{
	static const float PANEL_W = 330;
	static const float DOLL_X = 106;    // левее - столбик меток частей
	static const float FOOT_H = 84;     // подпись сета внизу панели
	static const float PAD_X = 20;
	static const float GAP = 16;
	static const int MAX_TAGS = 15;
	static const int MAX_TOOLS = 8;
	static const int MAX_PARTS = 12;
	static const int SECTIONS = 6;

	protected ref DRL_UIMannequin m_Doll;
	protected Widget m_Panel;
	protected ref INT_UIF_Surface m_PanelBg;
	protected TextWidget m_SetCap;
	protected TextWidget m_SetName;
	protected TextWidget m_SetInfo;
	protected ref array<ref DRL_UISetPart> m_Parts;
	protected ref array<Widget> m_PartRoots;
	protected ref array<ref INT_UIF_Surface> m_PartBgs;
	protected ref array<TextWidget> m_PartTexts;
	protected ref array<ButtonWidget> m_PartHits;
	protected ref array<string> m_Outfit;
	protected string m_SetTitle;

	protected Widget m_Cut;
	protected float m_CutX;
	protected float m_CutW;
	protected ref DRL_UICanvas m_CutLines;
	protected ref array<TextWidget> m_SecLabels;
	protected ref array<TextWidget> m_SecRight;
	protected ref array<ref DRL_UICutSwatch> m_Swatches;
	protected ref array<ref DRL_UICutTag> m_Tags;
	protected ref array<ref DRL_UICutTool> m_Tools;
	protected ref array<ImageWidget> m_ArmorTracks;
	protected ref array<ImageWidget> m_ArmorFills;
	protected int m_HoverTag = -1;

	void DRL_UICutSheet()
	{
		m_Doll = new DRL_UIMannequin();
		m_Parts = new array<ref DRL_UISetPart>();
		m_PartRoots = new array<Widget>();
		m_PartBgs = new array<ref INT_UIF_Surface>();
		m_PartTexts = new array<TextWidget>();
		m_PartHits = new array<ButtonWidget>();
		m_Outfit = new array<string>();
		m_SecLabels = new array<TextWidget>();
		m_SecRight = new array<TextWidget>();
		m_Swatches = new array<ref DRL_UICutSwatch>();
		m_Tags = new array<ref DRL_UICutTag>();
		m_Tools = new array<ref DRL_UICutTool>();
		m_ArmorTracks = new array<ImageWidget>();
		m_ArmorFills = new array<ImageWidget>();
	}

	override void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_CutSheet");
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int bpText = DRL_UIC.BP(220);

		// Невидимая сцена: логика окна (слоты навесного, конфликты) читает её объект.
		m_StageHost = INT_UIF_Draw.Frame(m_Root, 0, 0, 2, 2, "DRL_CutStageHost");
		m_Stage.Build(m_StageHost, 0, 0, 2, 2, true);
		m_StageHost.Show(false);

		// Манекен.
		float ph = h - 26;
		m_Panel = INT_UIF_Draw.Frame(m_Root, PAD_X, 14, PANEL_W, ph, "DRL_CutPanel");
		int panelFill = ARGB(255, 11, 18, 15);
		int panelBorder = DRL_UIC.BP(64);
		m_PanelBg = INT_UIF_Surface.Box(m_Panel, 0, 0, PANEL_W, ph, 14, panelFill, panelBorder);
		int glow = ARGB(70, 70, 110, 85);
		INT_UIF_Draw.Image(m_Panel, PANEL_W * 0.5 - 200, ph * 0.42 - 220, 400, 440, "iuf_radial", glow);
		// 3D персонажа рисуется поверх любого 2D, поэтому у него своя область:
		// справа от столбика меток и выше подписи сета.
		m_Doll.Build(m_Panel, DOLL_X, 6, PANEL_W - DOLL_X - 6, ph - FOOT_H - 8);

		// Подпись сета внизу панели - плашкой над 3D (скругление внутри скругления панели).
		int shade = ARGB(205, 11, 18, 15);
		INT_UIF_Surface footShade = INT_UIF_Surface.Box(m_Panel, 6, ph - FOOT_H - 6, PANEL_W - 12, FOOT_H, 12, shade, 0);
		m_SetCap = INT_UIF_Draw.Text(m_Panel, 14, ph - 76, PANEL_W - 28, 14, "#STR_DRL_UI_CUT_SET", 9, bpText);
		m_SetCap.SetBold(true);
		m_SetName = INT_UIF_Draw.Text(m_Panel, 14, ph - 62, PANEL_W - 28, 26, "", 18, txt);
		m_SetName.SetBold(true);
		m_SetInfo = INT_UIF_Draw.Text(m_Panel, 14, ph - 34, PANEL_W - 28, 18, "", 11, muted);
		m_SetInfo.SetBold(true);
		DRL_UIKitUtil.SetSortDeep(footShade.GetRoot(), DRL_UI_SORT_OVER_MODEL);
		m_SetCap.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_SetName.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_SetInfo.SetSort(DRL_UI_SORT_OVER_MODEL + 1);

		// Метки частей сета - столбиком по левому краю манекена, над 3D.
		for (int p = 0; p < MAX_PARTS; p++)
		{
			Widget pr = INT_UIF_Draw.Frame(m_Panel, 10, 14, 90, 24, "DRL_CutPart");
			INT_UIF_Surface pbg = INT_UIF_Surface.Pill(pr, 0, 0, 90, 24, 0, 0);
			TextWidget pt = INT_UIF_Draw.Text(pr, 10, 0, 76, 24, "", 11, muted);
			pt.SetBold(true);
			ButtonWidget ph2 = INT_UIF_Draw.Hit(pr, 0, 0, 90, 24, this);
			DRL_UIKitUtil.ExposeHit(ph2, "DRL_CutPart_" + p.ToString(), this);
			DRL_UIKitUtil.SetSortDeep(pr, DRL_UI_SORT_OVER_MODEL);
			pt.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
			ph2.SetSort(DRL_UI_SORT_OVER_MODEL + 2);
			pr.Show(false);
			m_PartRoots.Insert(pr);
			m_PartBgs.Insert(pbg);
			m_PartTexts.Insert(pt);
			m_PartHits.Insert(ph2);
		}

		// Правая часть: раскрой.
		m_CutX = PAD_X + PANEL_W + GAP;
		m_CutW = w - m_CutX - PAD_X;
		m_Cut = INT_UIF_Draw.Frame(m_Root, m_CutX, 0, m_CutW, h, "DRL_Cut");
		m_CutLines = new DRL_UICanvas();
		m_CutLines.Build(m_Cut, 0, 0, m_CutW, h);
		for (int s = 0; s < SECTIONS; s++)
		{
			TextWidget sl = INT_UIF_Draw.Text(m_Cut, 0, 0, 300, 16, "", 10, bpText);
			sl.SetBold(true);
			m_SecLabels.Insert(sl);
			TextWidget sr = INT_UIF_Draw.Text(m_Cut, m_CutW - 220, 0, 220, 16, "", 11, muted, INT_UIF_Align.RIGHT);
			sr.SetBold(true);
			m_SecRight.Insert(sr);
		}
		for (int v = 0; v < MAX_VARS; v++)
		{
			DRL_UICutSwatch sw = new DRL_UICutSwatch();
			sw.Build(m_Cut, v, this);
			m_Swatches.Insert(sw);
		}
		for (int t = 0; t < MAX_TAGS; t++)
		{
			DRL_UICutTag tag = new DRL_UICutTag();
			tag.Build(m_Cut, t, this);
			m_Tags.Insert(tag);
		}
		for (int k = 0; k < MAX_TOOLS; k++)
		{
			DRL_UICutTool tool = new DRL_UICutTool();
			tool.Build(m_Cut, k, this);
			m_Tools.Insert(tool);
		}

		// Навесное результата и доп. навесное - те же детали, что у «Чертежа».
		m_VarLabel = INT_UIF_Draw.Text(m_Cut, 0, 0, 10, 10, "", 9, bpText);
		m_VarLabel.Show(false);
		m_SlotsLabel = INT_UIF_Draw.Text(m_Cut, 0, 0, 10, 10, "", 9, bpText);
		m_SlotsLabel.Show(false);
		m_SlotsHost = INT_UIF_Draw.Frame(m_Cut, 0, 0, m_CutW, 32, "DRL_CutSlots");
		m_Slots = new DRL_UISlotStrip();
		m_Slots.Build(m_SlotsHost, 0, 0, m_CutW, 32);
		m_DopLabel = INT_UIF_Draw.Text(m_Cut, 0, 0, 10, 10, "", 9, bpText);
		m_DopLabel.Show(false);
		for (int d = 0; d < MAX_DOP; d++)
		{
			DRL_UIBpDop dop = new DRL_UIBpDop();
			dop.Build(m_Cut, d, this);
			m_DopChips.Insert(dop);
		}

		// Защита: пять ячеек «подпись / значение / полоска».
		m_Stats = INT_UIF_Draw.Frame(m_Cut, 0, 0, m_CutW, 62, "DRL_CutArmor");
		int armorFill = ARGB(64, 0, 0, 0);
		int armorBorder = DRL_UIC.BP(51);
		m_StatsBg = INT_UIF_Surface.Box(m_Stats, 0, 0, m_CutW, 62, 10, armorFill, armorBorder);
		float cell = (m_CutW - 24) / STAT_ROWS;
		int track = DRL_UIC.White(26);
		for (int a = 0; a < STAT_ROWS; a++)
		{
			float ax = 12 + a * cell;
			TextWidget al = INT_UIF_Draw.Text(m_Stats, ax, 9, cell - 10, 14, "", 9, bpText);
			al.SetBold(true);
			m_StatLabels.Insert(al);
			TextWidget av = INT_UIF_Draw.Text(m_Stats, ax, 24, cell - 10, 18, "", 13, txt);
			av.SetBold(true);
			m_StatValues.Insert(av);
			ImageWidget at = INT_UIF_Draw.Line(m_Stats, ax, 47, cell - 16, 4, track);
			ImageWidget af = INT_UIF_Draw.Line(m_Stats, ax, 47, 10, 4, txt);
			m_ArmorTracks.Insert(at);
			m_ArmorFills.Insert(af);
			ImageWidget sep = INT_UIF_Draw.Line(m_Stats, 0, 0, 1, 1, 0);
			sep.Show(false);
			m_StatSeps.Insert(sep);
		}
		m_Stats.Show(false);

		// Печать «не изучен» - на манекене.
		m_Seal = INT_UIF_Draw.Frame(m_Root, PAD_X + PANEL_W * 0.5 - 130, h * 0.5 - 40, 260, 80, "DRL_BpSeal");
		int sealFill = ARGB(225, 30, 22, 4);
		int sealBorder = DRL_UIC.Lk(200);
		INT_UIF_Surface.Box(m_Seal, 0, 0, 260, 80, 12, sealFill, sealBorder);
		int lk = DRL_UIC.Lk();
		ImageWidget sealIcon = INT_UIF_Draw.Icon(m_Seal, 20, 26, 28, "lock", lk);
		TextWidget sealTitle = INT_UIF_Draw.Text(m_Seal, 62, 14, 190, 28, "#STR_DRL_UI_BP_LOCKED", 20, lk);
		sealTitle.SetBold(true);
		TextWidget sealSub = INT_UIF_Draw.Text(m_Seal, 62, 42, 190, 20, "#STR_DRL_UI_BP_LOCKED_SUB", 11, muted);
		DRL_UIKitUtil.SetSortDeep(m_Seal, DRL_UI_SORT_OVER_MODEL);
		sealIcon.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		sealTitle.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		sealSub.SetSort(DRL_UI_SORT_OVER_MODEL + 1);
		m_Seal.Show(false);
	}

	override void SetSet(string name, array<ref DRL_UISetPart> parts, array<string> outfit)
	{
		m_SetTitle = name;
		m_Parts.Clear();
		if (parts)
		{
			foreach (DRL_UISetPart p : parts)
			{
				m_Parts.Insert(p);
			}
		}
		m_Outfit.Clear();
		if (outfit)
		{
			foreach (string c : outfit)
			{
				m_Outfit.Insert(c);
			}
		}
	}

	override void SetCovered(bool covered)
	{
		m_Covered = covered;
		m_Stage.SetHidden(DRL_UIStage.HIDE_POPOVER, covered);
		m_Doll.Show(!covered);
		foreach (DRL_UICutTag tag : m_Tags)
		{
			tag.ShowModel(!covered);
		}
		foreach (DRL_UICutTool tool : m_Tools)
		{
			tool.ShowModel(!covered);
		}
		for (int i = 0; i < m_PartRoots.Count(); i++)
		{
			m_PartRoots[i].Show(!covered && i < m_Parts.Count());
		}
		m_Seal.Show(m_Locked && !covered);
	}

	override void Tick(float dt)
	{
		m_Doll.Tick(dt);
	}

	override void ClearModels()
	{
		if (m_Stage)
			m_Stage.Clear();
		if (m_Doll)
			m_Doll.Clear();
		if (m_Tags)
		{
			foreach (DRL_UICutTag tag : m_Tags)
			{
				if (tag)
					tag.Clear();
			}
		}
		if (m_Tools)
		{
			foreach (DRL_UICutTool tool : m_Tools)
			{
				if (tool)
					tool.Clear();
			}
		}
	}

	override void HideAll()
	{
		foreach (DRL_UICutTag tag : m_Tags)
		{
			tag.Hide();
		}
		foreach (DRL_UICutTool tool : m_Tools)
		{
			tool.Hide();
		}
		foreach (DRL_UICutSwatch sw : m_Swatches)
		{
			sw.Hide();
		}
		foreach (DRL_UIBpDop dop : m_DopChips)
		{
			dop.Hide();
		}
		foreach (Widget pr : m_PartRoots)
		{
			pr.Show(false);
		}
		m_CutLines.Clear();
		foreach (TextWidget sl : m_SecLabels)
		{
			sl.Show(false);
		}
		foreach (TextWidget sr : m_SecRight)
		{
			sr.Show(false);
		}
		m_SlotsHost.Show(false);
		m_Stats.Show(false);
		m_Locked = false;
		m_Seal.Show(false);
		m_Stage.Clear();
		m_Doll.Clear();
	}

	override int VisibleCards()
	{
		int n = 0;
		foreach (DRL_UICutTag tag : m_Tags)
		{
			if (tag.m_Data)
				n++;
		}
		foreach (DRL_UICutTool tool : m_Tools)
		{
			if (tool.m_Data)
				n++;
		}
		return n;
	}

	// --- раскладка --------------------------------------------------------------------------
	override void Relayout()
	{
		m_Doll.SetOutfit(m_Outfit);
		m_Doll.Show(!m_Covered);
		LayoutPanel();

		m_CutLines.Clear();
		float y = 14;
		int sec = 0;

		// Расцветка.
		int nVars = m_VarNames.Count();
		string varsRight = DRL_UIText.Tr("#STR_DRL_UI_CUT_ONE");
		if (nVars > 1)
			varsRight = DRL_UIText.Fmt("#STR_DRL_UI_CUT_VARS_N", nVars.ToString());
		y = Section(sec, y, "#STR_DRL_UI_CUT_COLORS", varsRight);
		sec++;
		y = LayoutSwatches(y, nVars > 1);

		// Материалы (расходники) и инструменты с навесами.
		array<DRL_UIBpItem> mats = new array<DRL_UIBpItem>();
		array<DRL_UIBpItem> tools = new array<DRL_UIBpItem>();
		foreach (DRL_UIBpItem it : m_Side)
		{
			if (it.m_Kind == DRL_BP_INGR)
				mats.Insert(it);
			else
				tools.Insert(it);
		}
		foreach (DRL_UIBpItem top : m_Top)
		{
			tools.Insert(top);
		}
		string matsRight = DRL_UIText.Fmt("#STR_DRL_UI_CUT_MATS_N", mats.Count().ToString());
		y = Section(sec, y, "#STR_DRL_UI_CUT_MATS", matsRight);
		sec++;
		y = LayoutTags(y, mats);

		y = Section(sec, y, "#STR_DRL_UI_CUT_TOOLS", "");
		sec++;
		y = LayoutTools(y, tools);

		// Навесное результата.
		bool hasSlots = m_SlotCount > 0;
		m_SlotsHost.Show(hasSlots);
		if (hasSlots)
		{
			y = Section(sec, y, m_SlotsText, "");
			sec++;
			INT_UIF_Draw.Move(m_SlotsHost, 0, y);
			y = y + 32 + 12;
		}

		// Доп. навесное.
		bool hasDop = m_Dop.Count() > 0;
		if (hasDop)
		{
			y = Section(sec, y, m_DopText, "");
			sec++;
			y = LayoutDopRow(y);
		}
		else
		{
			foreach (DRL_UIBpDop hiddenDop : m_DopChips)
			{
				hiddenDop.Hide();
			}
		}

		// Защита.
		bool hasArmor = StatsFilled();
		if (hasArmor)
		{
			y = Section(sec, y, "#STR_DRL_UI_CUT_ARMOR", "");
			sec++;
			LayoutArmor(y);
		}
		else
		{
			m_Stats.Show(false);
		}

		for (int i = sec; i < SECTIONS; i++)
		{
			m_SecLabels[i].Show(false);
			m_SecRight[i].Show(false);
		}
		// Печать - внизу раскроя: над манекеном её закрыл бы 3D.
		INT_UIF_Draw.Move(m_Seal, m_CutX + m_CutW * 0.5 - 130, m_H - 96);
		m_Seal.Show(m_Locked && !m_Covered);
	}

	// Заголовок раздела: подпись, пунктир до правого текста. Возвращает y под ним.
	protected float Section(int idx, float y, string label, string right)
	{
		TextWidget sl = m_SecLabels[idx];
		TextWidget sr = m_SecRight[idx];
		string lt = DRL_UIText.Tr(label);
		sl.Show(true);
		sl.SetText(lt);
		INT_UIF_Draw.Place(sl, 0, y, m_CutW, 16);
		sr.Show(right != "");
		sr.SetText(right);
		INT_UIF_Draw.Place(sr, m_CutW - 220, y, 220, 16);
		float lw = DRL_UIMeasure.TextW(m_Cut, lt, 10, true);
		float rw = 0;
		if (right != "")
			rw = DRL_UIMeasure.TextW(m_Cut, right, 11, true) + 10;
		float x0 = lw + 10;
		float x1 = m_CutW - rw;
		if (x1 > x0 + 10)
		{
			array<float> xs = new array<float>();
			array<float> ys = new array<float>();
			xs.Insert(x0);
			xs.Insert(x1);
			ys.Insert(y + 8);
			ys.Insert(y + 8);
			int dash = DRL_UIC.BP(90);
			m_CutLines.Dashed(xs, ys, 5, 4, 1.5, dash);
		}
		return y + 24;
	}

	protected float LayoutSwatches(float y, bool show)
	{
		int n = m_VarNames.Count();
		if (!show)
		{
			foreach (DRL_UICutSwatch hidden : m_Swatches)
			{
				hidden.Hide();
			}
			return y + 4;
		}
		int perRow = (int)((m_CutW + 8) / (DRL_UICutSwatch.W + 8));
		if (perRow < 1)
			perRow = 1;
		int rows = 0;
		for (int i = 0; i < MAX_VARS; i++)
		{
			DRL_UICutSwatch sw = m_Swatches[i];
			if (i >= n)
			{
				sw.Hide();
				continue;
			}
			int row = i / perRow;
			int col = i - row * perRow;
			rows = row + 1;
			sw.Place(col * (DRL_UICutSwatch.W + 8), y + row * (DRL_UICutSwatch.H + 8), m_VarNames[i], m_VarColors[i], i == m_VarSel);
		}
		return y + rows * (DRL_UICutSwatch.H + 8) + 6;
	}

	protected float LayoutTags(float y, array<DRL_UIBpItem> mats)
	{
		float tw = (m_CutW - 20) / 3;
		int n = mats.Count();
		int shown = n;
		if (shown > MAX_TAGS)
			shown = MAX_TAGS;
		int rows = 0;
		for (int i = 0; i < MAX_TAGS; i++)
		{
			DRL_UICutTag tag = m_Tags[i];
			if (i >= shown)
			{
				tag.Hide();
				continue;
			}
			int row = i / 3;
			int col = i - row * 3;
			rows = row + 1;
			tag.Place(mats[i], col * (tw + 10), y + row * 56, tw);
			tag.ShowModel(!m_Covered);
		}
		if (rows == 0)
			return y + 4;
		return y + rows * 56 + 6;
	}

	protected float LayoutTools(float y, array<DRL_UIBpItem> tools)
	{
		float x = 0;
		float rowY = y;
		int n = tools.Count();
		for (int i = 0; i < MAX_TOOLS; i++)
		{
			DRL_UICutTool tool = m_Tools[i];
			if (i >= n)
			{
				tool.Hide();
				continue;
			}
			float w = tool.Measure(tools[i]);
			if (w > m_CutW)
				w = m_CutW;
			if (x > 0 && x + w > m_CutW)
			{
				x = 0;
				rowY = rowY + 46;
			}
			tool.Place(tools[i], x, rowY, w);
			tool.ShowModel(!m_Covered);
			x = x + w + 6;
		}
		if (n == 0)
			return y + 4;
		return rowY + 40 + 12;
	}

	protected float LayoutDopRow(float y)
	{
		float x = 0;
		for (int c = 0; c < MAX_DOP; c++)
		{
			DRL_UIBpDop chip = m_DopChips[c];
			if (c >= m_Dop.Count())
			{
				chip.Hide();
				continue;
			}
			float w = chip.Measure(m_Dop[c]);
			if (x + w > m_CutW)
				w = m_CutW - x;
			chip.Place(x, y, w, m_Dop[c]);
			x = x + w + 6;
		}
		return y + 30 + 12;
	}

	protected void LayoutArmor(float y)
	{
		m_Stats.Show(true);
		INT_UIF_Draw.Move(m_Stats, 0, y);
		for (int i = 0; i < STAT_ROWS; i++)
		{
			string lt = m_StatLText[i];
			string vt = m_StatVText[i];
			bool on = lt != "" || vt != "";
			m_StatLabels[i].Show(on);
			m_StatValues[i].Show(on);
			int pct = PercentOf(vt);
			bool bar = on && pct >= 0;
			m_ArmorTracks[i].Show(bar);
			m_ArmorFills[i].Show(bar);
			if (!bar)
				continue;
			float cell = (m_CutW - 24) / STAT_ROWS;
			float fw = (cell - 16) * pct / 100.0;
			if (fw < 2)
				fw = 2;
			INT_UIF_Draw.Resize(m_ArmorFills[i], fw, 4);
			m_ArmorFills[i].SetColor(DRL_UIC.HealthColor(pct));
		}
	}

	// «80%» -> 80; без процента -> -1.
	protected int PercentOf(string v)
	{
		int p = v.IndexOf("%");
		if (p <= 0)
			return -1;
		string num = v.Substring(0, p);
		num = num.Trim();
		int val = num.ToInt();
		return Math.Clamp(val, 0, 100);
	}

	// Подпись сета и метки частей.
	protected void LayoutPanel()
	{
		string title = m_SetTitle;
		if (title == "")
			title = DRL_UIText.Tr("#STR_DRL_UI_CUT_NOSET");
		m_SetName.SetText(title);
		m_SetInfo.SetText(DRL_UIText.Fmt("#STR_DRL_UI_CUT_ITEMS", m_Parts.Count().ToString()));

		float y = 14;
		for (int i = 0; i < m_PartRoots.Count(); i++)
		{
			Widget root = m_PartRoots[i];
			if (i >= m_Parts.Count())
			{
				root.Show(false);
				continue;
			}
			DRL_UISetPart p = m_Parts[i];
			float tw = DRL_UIMeasure.TextW(m_Panel, p.m_Name, 11, true);
			float w = tw + 22;
			root.Show(!m_Covered);
			INT_UIF_Draw.Place(root, 10, y, w, 24);
			INT_UIF_Draw.Resize(m_PartBgs[i].GetRoot(), w, 24);
			INT_UIF_Draw.Place(m_PartTexts[i], 11, 0, tw + 4, 24);
			INT_UIF_Draw.Resize(m_PartHits[i], w, 24);
			m_PartTexts[i].SetText(p.m_Name);
			int fill = ARGB(225, 10, 16, 13);
			int border = DRL_UIC.BP(102);
			int fg = DRL_UIC.Muted();
			if (!p.m_Ok)
				border = DRL_UIC.No(153);
			if (p.m_Locked)
				fg = DRL_UIC.Dim();
			if (p.m_Selected)
			{
				fill = DRL_UIC.A();
				border = 0;
				fg = ARGB(255, 43, 26, 0);
			}
			m_PartBgs[i].SetFill(fill);
			m_PartBgs[i].SetBorder(border);
			m_PartTexts[i].SetColor(fg);
			y = y + 30;
		}
	}

	// --- события ----------------------------------------------------------------------------
	protected int PartOf(Widget w)
	{
		for (int i = 0; i < m_PartHits.Count(); i++)
		{
			if (m_PartHits[i] == w)
				return i;
		}
		return -1;
	}

	protected int SwatchOf(Widget w)
	{
		for (int i = 0; i < m_Swatches.Count(); i++)
		{
			if (m_Swatches[i].m_Hit == w)
				return i;
		}
		return -1;
	}

	protected int TagOf(Widget w)
	{
		for (int i = 0; i < m_Tags.Count(); i++)
		{
			if (m_Tags[i].m_Hit == w)
				return i;
		}
		return -1;
	}

	protected int ToolOf(Widget w)
	{
		for (int i = 0; i < m_Tools.Count(); i++)
		{
			if (m_Tools[i].m_Hit == w)
				return i;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		int pi = PartOf(w);
		if (pi >= 0)
		{
			if (pi < m_Parts.Count())
			{
				DRL_UISound.Select();
				m_OnAction.Invoke("part", m_Parts[pi].m_RecipeId, 0);
			}
			return true;
		}
		int si = SwatchOf(w);
		if (si >= 0)
		{
			DRL_UISound.Select();
			m_OnAction.Invoke("var", si, 0);
			return true;
		}
		int ti = TagOf(w);
		if (ti >= 0)
		{
			DRL_UIBpItem d = m_Tags[ti].m_Data;
			if (d)
			{
				DRL_UISound.Select();
				m_OnAction.Invoke("item", d.m_Tag, d.m_Kind);
			}
			return true;
		}
		if (ToolOf(w) >= 0)
			return true;
		int action;
		int di = DopOf(w, action);
		if (di >= 0)
		{
			DRL_UISound.Select();
			m_OnAction.Invoke("dop", di, action);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (PartOf(w) >= 0 || SwatchOf(w) >= 0)
		{
			DRL_UISound.Hover();
			return true;
		}
		int ti = TagOf(w);
		if (ti >= 0)
		{
			DRL_UICutTag tag = m_Tags[ti];
			tag.SetHover(true);
			DRL_UISound.Hover();
			if (tag.m_Data && tag.m_Data.m_Tip != "")
				INT_UIF_Tooltip.ShowFor(tag.m_Root, tag.m_Data.m_Tip);
			return true;
		}
		int ki = ToolOf(w);
		if (ki >= 0)
		{
			DRL_UICutTool tool = m_Tools[ki];
			DRL_UISound.Hover();
			if (tool.m_Data && tool.m_Data.m_Tip != "")
				INT_UIF_Tooltip.ShowFor(tool.m_Root, tool.m_Data.m_Tip);
			return true;
		}
		int action;
		int di = DopOf(w, action);
		if (di >= 0 && action == 0 && di < m_Dop.Count() && m_Dop[di].m_Tip != "")
		{
			INT_UIF_Tooltip.ShowFor(m_DopChips[di].m_Root, m_Dop[di].m_Tip);
			return true;
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int ti = TagOf(w);
		if (ti >= 0)
		{
			m_Tags[ti].SetHover(false);
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		if (ToolOf(w) >= 0)
		{
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		int action;
		if (DopOf(w, action) >= 0)
		{
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		return false;
	}

	// Тестовый хук: «card» - бирка, «var» - лоскут, «part» - метка части, «dop» - доп. навесное.
	override void TestClick(string what, int index, int action)
	{
		if (what == "card" && index >= 0 && index < m_Tags.Count())
		{
			OnClick(m_Tags[index].m_Hit, 0, 0, MouseState.LEFT);
			return;
		}
		if (what == "var" && index >= 0 && index < m_Swatches.Count())
		{
			OnClick(m_Swatches[index].m_Hit, 0, 0, MouseState.LEFT);
			return;
		}
		if (what == "part" && index >= 0 && index < m_PartHits.Count())
		{
			OnClick(m_PartHits[index], 0, 0, MouseState.LEFT);
			return;
		}
		super.TestClick(what, index, action);
	}
}
