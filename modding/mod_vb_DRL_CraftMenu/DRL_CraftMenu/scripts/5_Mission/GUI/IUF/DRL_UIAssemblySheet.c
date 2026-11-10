// ============================================================================
// DRL_CraftMenu - «Сборка»: центр окна у подъёмника (DRL_Craft_Truck_Lift),
// макет «макет верстака DRL».
//
// Слева стенд подъёмника: полосы по краям, схема машины сверху, детали-доп.
// рецепты (AttachedRecipeIds) метками с галочкой, числом колёс и вариантом,
// внизу окраска (варианты результата). Справа спецификация: материалы кузова
// и выбранных деталей по группам, итог «есть / нужно», инструменты.
// Данные те же, что у «Чертежа» (DRL_UIBlueprint), меняется только вид.
// ============================================================================

class DRL_UISpecData extends Managed
{
	bool m_Header;
	string m_Name;
	string m_Value;
	string m_Glyph = "gear";
	string m_Tip;
	bool m_Ok = true;
	bool m_Tool;
}

class DRL_UISpecRow extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Glyph;
	TextWidget m_Name;
	TextWidget m_Value;
	TextWidget m_Head;
	ButtonWidget m_Hit;
	ref DRL_UISpecData m_Data;
}

// Список спецификации с прокруткой колесом (строки из пула).
class DRL_UISpecList extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ButtonWidget m_BgHit;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected ref array<ref DRL_UISpecRow> m_Rows;
	protected ref array<ref DRL_UISpecData> m_Items;
	protected float m_W;
	protected float m_H;
	protected float m_RowH;
	protected int m_First;

	void DRL_UISpecList()
	{
		m_Rows = new array<ref DRL_UISpecRow>();
		m_Items = new array<ref DRL_UISpecData>();
	}

	void Build(Widget parent, float x, float y, float w, float h, float rowH)
	{
		m_W = w;
		m_H = h;
		m_RowH = rowH;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_SpecList");
		m_BgHit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		m_BgHit.SetName("DRL_SpecBg");
		int n = (int)(h / rowH);
		int txt = DRL_UIC.Txt();
		int dim = DRL_UIC.Dim();
		int head = DRL_UIC.BP(200);
		for (int i = 0; i < n; i++)
		{
			DRL_UISpecRow r = new DRL_UISpecRow();
			r.m_Root = INT_UIF_Draw.Frame(m_Root, 0, i * rowH, w - 8, rowH - 2, "DRL_SpecRow");
			r.m_Bg = INT_UIF_Surface.Box(r.m_Root, 0, 0, w - 8, rowH - 2, 6, 0, 0);
			r.m_Glyph = INT_UIF_Draw.Icon(r.m_Root, 8, (rowH - 2 - 14) * 0.5, 14, "gear", dim);
			r.m_Name = INT_UIF_Draw.Text(r.m_Root, 30, 0, w - 8 - 30 - 90, rowH - 2, "", 11, txt);
			r.m_Name.SetBold(true);
			r.m_Value = INT_UIF_Draw.Text(r.m_Root, w - 8 - 96, 0, 88, rowH - 2, "", 12, txt, INT_UIF_Align.RIGHT);
			r.m_Value.SetBold(true);
			r.m_Head = INT_UIF_Draw.Text(r.m_Root, 4, 0, w - 16, rowH - 2, "", 10, head);
			r.m_Head.SetBold(true);
			r.m_Hit = INT_UIF_Draw.Hit(r.m_Root, 0, 0, w - 8, rowH - 2, this);
			r.m_Hit.SetName("DRL_SpecRowHit");
			r.m_Root.Show(false);
			m_Rows.Insert(r);
		}
		int trackColor = DRL_UIC.White(14);
		m_Track = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, h, trackColor);
		int thumbColor = DRL_UIC.BP(170);
		m_Thumb = INT_UIF_Draw.Line(m_Root, w - 4, 0, 4, 40, thumbColor);
	}

	void SetItems(array<ref DRL_UISpecData> items, bool keepScroll)
	{
		m_Items.Clear();
		foreach (DRL_UISpecData d : items)
		{
			m_Items.Insert(d);
		}
		if (!keepScroll)
			m_First = 0;
		m_First = Math.Clamp(m_First, 0, MaxFirst());
		Rebind();
	}

	protected int MaxFirst()
	{
		int m = m_Items.Count() - m_Rows.Count();
		if (m < 0)
			m = 0;
		return m;
	}

	protected void Rebind()
	{
		int ok = DRL_UIC.Ok();
		int no = DRL_UIC.NoText();
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		for (int i = 0; i < m_Rows.Count(); i++)
		{
			DRL_UISpecRow r = m_Rows[i];
			int idx = m_First + i;
			if (idx >= m_Items.Count())
			{
				r.m_Data = null;
				r.m_Root.Show(false);
				continue;
			}
			DRL_UISpecData d = m_Items[idx];
			r.m_Data = d;
			r.m_Root.Show(true);
			r.m_Head.Show(d.m_Header);
			r.m_Glyph.Show(!d.m_Header);
			r.m_Name.Show(!d.m_Header);
			r.m_Value.Show(!d.m_Header);
			if (d.m_Header)
			{
				r.m_Head.SetText(d.m_Name);
				r.m_Bg.SetFill(0);
				r.m_Bg.SetBorder(0);
				continue;
			}
			r.m_Name.SetText(d.m_Name);
			r.m_Value.SetText(d.m_Value);
			INT_UIF_Widgets.SetIcon(r.m_Glyph, d.m_Glyph);
			int fill = DRL_UIC.White(8);
			int valueColor = ok;
			if (d.m_Tool)
				valueColor = muted;
			// Нехватку видно по красному числу; заливка лишь чуть тёплая.
			if (!d.m_Ok)
			{
				fill = DRL_UIC.No(14);
				valueColor = no;
			}
			r.m_Bg.SetFill(fill);
			r.m_Bg.SetBorder(0);
			r.m_Value.SetColor(valueColor);
			r.m_Name.SetColor(txt);
		}
		bool scroll = MaxFirst() > 0;
		m_Track.Show(scroll);
		m_Thumb.Show(scroll);
		if (scroll)
		{
			float th = m_H * m_Rows.Count() / m_Items.Count();
			if (th < 30)
				th = 30;
			float ty = (m_H - th) * m_First / MaxFirst();
			INT_UIF_Draw.Place(m_Thumb, m_W - 4, ty, 4, th);
		}
	}

	protected DRL_UISpecRow RowOf(Widget w)
	{
		foreach (DRL_UISpecRow r : m_Rows)
		{
			if (r.m_Hit == w)
				return r;
		}
		return null;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_BgHit && !RowOf(w))
			return false;
		int target = Math.Clamp(m_First - wheel * 2, 0, MaxFirst());
		if (target != m_First)
		{
			m_First = target;
			Rebind();
		}
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		DRL_UISpecRow r = RowOf(w);
		if (!r || !r.m_Data || r.m_Data.m_Tip == "")
			return false;
		INT_UIF_Tooltip.ShowFor(r.m_Root, r.m_Data.m_Tip);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (!RowOf(w))
			return false;
		INT_UIF_Tooltip.HideTip();
		return true;
	}
}

// --- лист «Сборка» ---------------------------------------------------------------------
class DRL_UIAssemblySheet extends DRL_UIBlueprint
{
	static const float SPEC_W = 330;
	static const float PAD_X = 20;
	static const int ZONES = 7;          // hood, kovh, front, back, left, right, wheel
	static const int PAINTS = 8;

	protected Widget m_Bay;
	protected float m_BayX;
	protected float m_BayY;
	protected float m_BayW;
	protected float m_BayH;
	protected ref DRL_UICanvas m_BayLines;
	protected ref INT_UIF_Surface m_Body;
	protected ref array<ref INT_UIF_Surface> m_Wheels;
	protected ref array<ref INT_UIF_Surface> m_Zones;
	protected TextWidget m_PaintLabel;
	protected ref array<ImageWidget> m_PaintDots;
	protected ref array<ImageWidget> m_PaintRings;
	protected ref array<ButtonWidget> m_PaintHits;

	protected Widget m_Spec;
	protected TextWidget m_SpecCap;
	protected TextWidget m_SpecGot;
	protected TextWidget m_SpecNeed;
	protected ImageWidget m_SpecTrack;
	protected ImageWidget m_SpecFill;
	protected ref DRL_UISpecList m_SpecList;
	protected float m_SpecBarW;

	// Характеристики машины в правом нижнем углу стенда.
	static const float INFO_W = 280;
	protected Widget m_Info;
	protected ref INT_UIF_Surface m_InfoBg;
	protected TextWidget m_InfoCargo;
	protected MultilineTextWidget m_InfoAtts;
	protected ref array<string> m_InfoLines;

	void DRL_UIAssemblySheet()
	{
		m_Wheels = new array<ref INT_UIF_Surface>();
		m_Zones = new array<ref INT_UIF_Surface>();
		m_PaintDots = new array<ImageWidget>();
		m_PaintRings = new array<ImageWidget>();
		m_PaintHits = new array<ButtonWidget>();
		m_SpecList = new DRL_UISpecList();
		m_InfoLines = new array<string>();
	}

	override void SetInfo(array<string> lines)
	{
		m_InfoLines.Clear();
		if (!lines)
			return;
		foreach (string l : lines)
		{
			m_InfoLines.Insert(l);
		}
	}

	override void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_AsmSheet");
		int txt = DRL_UIC.Txt();
		int muted = DRL_UIC.Muted();
		int dim = DRL_UIC.Dim();
		int bpText = DRL_UIC.BP(220);

		// Невидимая сцена: логика окна (слоты навесного, конфликты деталей) читает её объект.
		m_StageHost = INT_UIF_Draw.Frame(m_Root, 0, 0, 2, 2, "DRL_AsmStageHost");
		m_Stage.Build(m_StageHost, 0, 0, 2, 2, true);
		m_StageHost.Show(false);

		// Стенд.
		m_BayX = PAD_X;
		m_BayY = 12;
		m_BayW = w - PAD_X * 2 - SPEC_W - 14;
		m_BayH = h - 24;
		m_Bay = INT_UIF_Draw.Frame(m_Root, m_BayX, m_BayY, m_BayW, m_BayH, "DRL_AsmBay");
		int bayFill = ARGB(235, 10, 14, 22);
		int bayBorder = DRL_UIC.BP(64);
		INT_UIF_Surface.Box(m_Bay, 0, 0, m_BayW, m_BayH, 12, bayFill, bayBorder);
		DRL_UICanvas bayGrid = new DRL_UICanvas();
		Widget gridClip = INT_UIF_Draw.Clip(m_Bay, 2, 16, m_BayW - 4, m_BayH - 32, "DRL_AsmGrid");
		if (bayGrid.Build(gridClip, 0, 0, m_BayW - 4, m_BayH - 32))
		{
			int minor = DRL_UIC.BP(12);
			int major = DRL_UIC.BP(22);
			bayGrid.Grid(m_BayW - 4, m_BayH - 32, 24, minor, major);
		}
		// Жёлто-чёрные полосы по верхнему и нижнему краю стенда.
		BuildStripe(14, 4);
		BuildStripe(m_BayH - 14, m_BayH - 4);

		m_Body = INT_UIF_Surface.Box(m_Bay, 0, 0, 100, 300, 14, 0, 0);
		for (int z = 0; z < ZONES; z++)
		{
			INT_UIF_Surface zone = INT_UIF_Surface.Box(m_Bay, 0, 0, 20, 20, 6, 0, 0);
			zone.Show(false);
			m_Zones.Insert(zone);
		}
		for (int wh = 0; wh < 4; wh++)
		{
			INT_UIF_Surface wheel = INT_UIF_Surface.Box(m_Bay, 0, 0, 24, 70, 6, 0, 0);
			m_Wheels.Insert(wheel);
		}
		m_BayLines = new DRL_UICanvas();
		m_BayLines.Build(m_Bay, 0, 0, m_BayW, m_BayH);

		for (int d = 0; d < MAX_SIDE; d++)
		{
			ImageWidget dot = INT_UIF_Draw.Image(m_Bay, 0, 0, 7, 7, "iuf_circle16", 0);
			dot.Show(false);
			m_Dots.Insert(dot);
		}
		for (int p = 0; p < MAX_DOP; p++)
		{
			DRL_UIBpDop chip = new DRL_UIBpDop();
			chip.Build(m_Bay, p, this);
			m_DopChips.Insert(chip);
		}

		// Окраска (варианты результата).
		m_PaintLabel = INT_UIF_Draw.Text(m_Bay, 18, m_BayH - 52, 90, 16, "#STR_DRL_UI_ASM_PAINT", 9, bpText);
		m_PaintLabel.SetBold(true);
		int ringColor = DRL_UIC.A();
		for (int c = 0; c < PAINTS; c++)
		{
			ImageWidget ring = INT_UIF_Draw.Image(m_Bay, 0, 0, 30, 30, "iuf_ringline32", ringColor);
			ring.Show(false);
			m_PaintRings.Insert(ring);
			ImageWidget pd = INT_UIF_Draw.Image(m_Bay, 0, 0, 22, 22, "iuf_circle32", 0);
			pd.Show(false);
			m_PaintDots.Insert(pd);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_Bay, 0, 0, 26, 26, this);
			DRL_UIKitUtil.ExposeHit(hit, "DRL_BpPaint_" + c.ToString(), this);
			hit.Show(false);
			m_PaintHits.Insert(hit);
		}

		// Характеристики машины: грузовой отсек и навесное (бочки, ящики, рюкзаки, оружие).
		m_Info = INT_UIF_Draw.Frame(m_Bay, m_BayW - 18 - INFO_W, m_BayH - 18 - 96, INFO_W, 96, "DRL_AsmInfo");
		int infoFill = ARGB(215, 8, 12, 20);
		int infoBorder = DRL_UIC.BP(70);
		m_InfoBg = INT_UIF_Surface.Box(m_Info, 0, 0, INFO_W, 96, 10, infoFill, infoBorder);
		TextWidget infoCap = INT_UIF_Draw.Text(m_Info, 12, 8, INFO_W - 24, 14, "#STR_DRL_UI_ASM_INFO", 9, bpText);
		infoCap.SetBold(true);
		m_InfoCargo = INT_UIF_Draw.Text(m_Info, 12, 24, INFO_W - 24, 18, "", 12, txt);
		m_InfoCargo.SetBold(true);
		m_InfoAtts = INT_UIF_Draw.MText(m_Info, 12, 44, INFO_W - 24, 46, "", 11, muted);
		m_Info.Show(false);

		// Спецификация.
		float sx = w - PAD_X - SPEC_W;
		m_Spec = INT_UIF_Draw.Frame(m_Root, sx, 12, SPEC_W, h - 24, "DRL_AsmSpec");
		int specFill = ARGB(150, 4, 10, 14);
		int specBorder = DRL_UIC.BP(56);
		INT_UIF_Surface.Box(m_Spec, 0, 0, SPEC_W, h - 24, 12, specFill, specBorder);
		m_SpecCap = INT_UIF_Draw.Text(m_Spec, 14, 12, SPEC_W - 28, 14, "#STR_DRL_UI_ASM_SPEC", 9, bpText);
		m_SpecCap.SetBold(true);
		m_SpecGot = INT_UIF_Draw.Text(m_Spec, 14, 28, 120, 28, "", 21, txt);
		m_SpecGot.SetBold(true);
		m_SpecNeed = INT_UIF_Draw.Text(m_Spec, 80, 36, 220, 18, "", 12, dim);
		m_SpecNeed.SetBold(true);
		m_SpecBarW = SPEC_W - 28;
		int barTrack = DRL_UIC.White(20);
		m_SpecTrack = INT_UIF_Draw.Line(m_Spec, 14, 62, m_SpecBarW, 4, barTrack);
		int barFill = DRL_UIC.A();
		m_SpecFill = INT_UIF_Draw.Line(m_Spec, 14, 62, 10, 4, barFill);
		m_SpecList.Build(m_Spec, 10, 76, SPEC_W - 16, h - 24 - 76 - 10, 30);

		// Детали, которые нужны общим методам базового класса, но не показываются.
		m_VarLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 10, 10, "", 9, bpText);
		m_VarLabel.Show(false);
		m_SlotsLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 10, 10, "", 9, bpText);
		m_SlotsLabel.Show(false);
		m_SlotsHost = INT_UIF_Draw.Frame(m_Root, 0, 0, 2, 2, "DRL_AsmSlots");
		m_Slots = new DRL_UISlotStrip();
		m_Slots.Build(m_SlotsHost, 0, 0, 2, 2);
		m_SlotsHost.Show(false);
		m_DopLabel = INT_UIF_Draw.Text(m_Root, 0, 0, 10, 10, "", 9, bpText);
		m_DopLabel.Show(false);
		m_Stats = INT_UIF_Draw.Frame(m_Root, 0, 0, 2, 2, "DRL_AsmStats");
		for (int s = 0; s < STAT_ROWS; s++)
		{
			m_StatLabels.Insert(INT_UIF_Draw.Text(m_Stats, 0, 0, 1, 1, "", 9, txt));
			m_StatValues.Insert(INT_UIF_Draw.Text(m_Stats, 0, 0, 1, 1, "", 9, txt));
			m_StatSeps.Insert(INT_UIF_Draw.Line(m_Stats, 0, 0, 1, 1, 0));
		}
		m_Stats.Show(false);

		// Печать «не изучен».
		m_Seal = INT_UIF_Draw.Frame(m_Root, m_BayX + m_BayW * 0.5 - 130, h * 0.5 - 40, 260, 80, "DRL_BpSeal");
		int sealFill = ARGB(225, 30, 22, 4);
		int sealBorder = DRL_UIC.Lk(200);
		INT_UIF_Surface.Box(m_Seal, 0, 0, 260, 80, 12, sealFill, sealBorder);
		int lk = DRL_UIC.Lk();
		INT_UIF_Draw.Icon(m_Seal, 20, 26, 28, "lock", lk);
		TextWidget sealTitle = INT_UIF_Draw.Text(m_Seal, 62, 14, 190, 28, "#STR_DRL_UI_BP_LOCKED", 20, lk);
		sealTitle.SetBold(true);
		INT_UIF_Draw.Text(m_Seal, 62, 42, 190, 20, "#STR_DRL_UI_BP_LOCKED_SUB", 11, muted);
		m_Seal.Show(false);
	}

	// Полоса из косых жёлтых штрихов между y0 и y1 (по всей ширине стенда).
	protected void BuildStripe(float y0, float y1)
	{
		float top = y0;
		float hgt = y1 - y0;
		if (hgt < 0)
		{
			top = y1;
			hgt = -hgt;
		}
		Widget clip = INT_UIF_Draw.Clip(m_Bay, 12, top, m_BayW - 24, hgt, "DRL_AsmStripe");
		int dark = ARGB(220, 20, 18, 10);
		INT_UIF_Draw.Image(clip, 0, 0, m_BayW - 24, hgt, "iuf_white", dark);
		DRL_UICanvas c = new DRL_UICanvas();
		if (!c.Build(clip, 0, 0, m_BayW - 24, hgt))
			return;
		int yellow = DRL_UIC.Lk(200);
		float x = -hgt;
		while (x < m_BayW)
		{
			c.Line(x, hgt, x + hgt, 0, 6, yellow);
			x = x + 16;
		}
	}

	override void SetCovered(bool covered)
	{
		m_Covered = covered;
		m_Stage.SetHidden(DRL_UIStage.HIDE_POPOVER, covered);
		m_Seal.Show(m_Locked && !covered);
	}

	override void Tick(float dt)
	{
	}

	override void ClearModels()
	{
		if (m_Stage)
			m_Stage.Clear();
	}

	override void HideAll()
	{
		if (m_DopChips)
		{
			foreach (DRL_UIBpDop chip : m_DopChips)
			{
				chip.Hide();
			}
		}
		if (m_BayLines)
			m_BayLines.Clear();
		foreach (ImageWidget dot : m_Dots)
		{
			dot.Show(false);
		}
		foreach (INT_UIF_Surface zone : m_Zones)
		{
			zone.Show(false);
		}
		m_Locked = false;
		m_Seal.Show(false);
		m_Stage.Clear();
	}

	override int VisibleCards()
	{
		return m_Side.Count() + m_Top.Count();
	}

	// --- части машины ------------------------------------------------------------------------
	// 0 капот, 1 воздухозаборник, 2 бампер, 3 багажник, 4 водит. дверь, 5 пассаж. дверь, 6 колёса; -1 - не распознано.
	protected int PartPos(string cls)
	{
		string low = cls;
		low.ToLower();
		if (low.IndexOf("wheel") > -1)
			return 6;
		if (low.IndexOf("codriver") > -1)
			return 5;
		if (low.IndexOf("driver") > -1)
			return 4;
		if (low.IndexOf("kovh") > -1)
			return 1;
		if (low.IndexOf("hood") > -1)
			return 0;
		if (low.IndexOf("bamper") > -1 || low.IndexOf("bumper") > -1)
			return 2;
		if (low.IndexOf("trunk") > -1)
			return 3;
		return -1;
	}

	protected string PartLabel(int pos, string fallback)
	{
		if (pos == 0)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_HOOD");
		if (pos == 1)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_KOVH");
		if (pos == 2)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_BUMPER");
		if (pos == 3)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_TRUNK");
		if (pos == 4)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_DRIVER");
		if (pos == 5)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_CODRIVER");
		if (pos == 6)
			return DRL_UIText.Tr("#STR_DRL_UI_CARPART_WHEEL");
		return fallback;
	}

	// --- раскладка --------------------------------------------------------------------------
	override void Relayout()
	{
		m_BayLines.Clear();
		m_DotsUsed = 0;

		// Масштаб схемы под высоту стенда; машина по центру (координаты схемы 150 x 446).
		float sc = (m_BayH - 170) / 470;
		if (sc > 1.05)
			sc = 1.05;
		float cw = 150 * sc;
		float ch = 446 * sc;
		float cx = m_BayW * 0.5;
		float x0 = cx - cw * 0.5;
		float y0 = (m_BayH - ch) * 0.5 - 10;

		// Окраска кузова - выбранный вариант.
		// Без вариантов окраски - приглушённый цвет листа, иначе кузов выглядел белым пятном.
		int paint = DRL_UIC.BP();
		if (m_VarColors.Count() > 1 && m_VarSel >= 0 && m_VarSel < m_VarColors.Count())
			paint = m_VarColors[m_VarSel];
		int bodyFill = INT_UIF_Color.WithAlpha(paint, 46);
		int bodyBorder = DRL_UIC.BP(150);
		INT_UIF_Draw.Place(m_Body.GetRoot(), x0, y0 - 4 * sc, cw, ch + 4 * sc);
		m_Body.SetFill(bodyFill);
		m_Body.SetBorder(bodyBorder);

		// Рамка подъёмника пунктиром и остекление.
		int frame = DRL_UIC.Lk(64);
		DashRect(x0 - 38 * sc, y0 - 14 * sc, 226 * sc, 468 * sc, frame);
		int glass = DRL_UIC.BP(110);
		Poly4(x0, y0, sc, 22, 132, 128, 132, 118, 178, 32, 178, glass);
		Poly4(x0, y0, sc, 32, 180, 118, 180, 118, 300, 32, 300, glass);
		Poly4(x0, y0, sc, 32, 302, 118, 302, 126, 334, 24, 334, glass);

		// Состояние деталей по местам на машине: 0 - нет детали, 1 - выбрана, 2 - выбрана, не хватает, 3 - не выбрана.
		array<int> state = new array<int>();
		for (int s = 0; s < ZONES; s++)
		{
			state.Insert(0);
		}
		for (int i = 0; i < m_Dop.Count(); i++)
		{
			DRL_UIBpDopData d = m_Dop[i];
			int pos = PartPos(d.m_Cls);
			if (pos < 0)
				continue;
			int st = 3;
			if (d.m_Checked && d.m_Ok)
				st = 1;
			else if (d.m_Checked)
				st = 2;
			if (state[pos] == 0 || st < state[pos])
				state.Set(pos, st);
		}
		LayoutZones(x0, y0, sc, state);

		// Колёса.
		int wheelState = state[6];
		int wheelFill = ARGB(255, 22, 26, 32);
		int wheelBorder = DRL_UIC.BP(120);
		if (wheelState == 1)
		{
			wheelFill = DRL_UIC.Ok(60);
			wheelBorder = DRL_UIC.Ok(200);
		}
		else if (wheelState == 2)
		{
			wheelFill = DRL_UIC.No(60);
			wheelBorder = DRL_UIC.No(200);
		}
		array<float> wx = {-12.0, 138.0, -12.0, 138.0};
		array<float> wy = {60.0, 60.0, 330.0, 330.0};
		for (int k = 0; k < 4; k++)
		{
			INT_UIF_Draw.Place(m_Wheels[k].GetRoot(), x0 + wx[k] * sc, y0 + wy[k] * sc, 24 * sc, 70 * sc);
			m_Wheels[k].SetFill(wheelFill);
			m_Wheels[k].SetBorder(wheelBorder);
		}

		LayoutChips(x0, y0, sc, cx);
		LayoutPaint();
		LayoutSpec();
		LayoutInfo();

		for (int u = m_DotsUsed; u < m_Dots.Count(); u++)
		{
			m_Dots[u].Show(false);
		}
		INT_UIF_Draw.Move(m_Seal, m_BayX + m_BayW * 0.5 - 130, m_H * 0.5 - 40);
		m_Seal.Show(m_Locked && !m_Covered);
	}

	protected void DashRect(float x, float y, float w, float h, int color)
	{
		array<float> xs = new array<float>();
		array<float> ys = new array<float>();
		xs.Insert(x);
		ys.Insert(y);
		xs.Insert(x + w);
		ys.Insert(y);
		xs.Insert(x + w);
		ys.Insert(y + h);
		xs.Insert(x);
		ys.Insert(y + h);
		xs.Insert(x);
		ys.Insert(y);
		m_BayLines.Dashed(xs, ys, 8, 6, 1, color);
	}

	// Замкнутый четырёхугольник в координатах схемы.
	protected void Poly4(float x0, float y0, float sc, float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy, int color)
	{
		float px1 = x0 + ax * sc;
		float py1 = y0 + ay * sc;
		float px2 = x0 + bx * sc;
		float py2 = y0 + by * sc;
		float px3 = x0 + cx * sc;
		float py3 = y0 + cy * sc;
		float px4 = x0 + dx * sc;
		float py4 = y0 + dy * sc;
		m_BayLines.Line(px1, py1, px2, py2, 1, color);
		m_BayLines.Line(px2, py2, px3, py3, 1, color);
		m_BayLines.Line(px3, py3, px4, py4, 1, color);
		m_BayLines.Line(px4, py4, px1, py1, 1, color);
	}

	protected void LayoutZones(float x0, float y0, float sc, array<int> state)
	{
		// Прямоугольники зон в координатах схемы: x, y, w, h.
		array<float> zx = {12.0, 55.0, 14.0, 8.0, 2.0, 132.0, 0.0};
		array<float> zy = {36.0, 60.0, -10.0, 340.0, 140.0, 140.0, 0.0};
		array<float> zw = {126.0, 40.0, 122.0, 134.0, 16.0, 16.0, 0.0};
		array<float> zh = {88.0, 40.0, 36.0, 70.0, 110.0, 110.0, 0.0};
		for (int i = 0; i < ZONES; i++)
		{
			INT_UIF_Surface zone = m_Zones[i];
			int st = state[i];
			// Колёса подсвечиваются самими колёсами.
			if (i == 6 || st == 0 || st == 3)
			{
				zone.Show(false);
				continue;
			}
			zone.Show(true);
			INT_UIF_Draw.Place(zone.GetRoot(), x0 + zx[i] * sc, y0 + zy[i] * sc, zw[i] * sc, zh[i] * sc);
			int fill = DRL_UIC.Ok(46);
			int border = DRL_UIC.Ok(170);
			if (st == 2)
			{
				fill = DRL_UIC.No(46);
				border = DRL_UIC.No(170);
			}
			zone.SetFill(fill);
			zone.SetBorder(border);
		}
	}

	// Метки деталей вокруг машины с выносками к их месту.
	protected void LayoutChips(float x0, float y0, float sc, float cx)
	{
		// Место метки (x, y, выравнивание: 0 центр, 1 справа налево, 2 слева направо) и точка на машине.
		array<int> used = {0, 0, 0, 0, 0, 0, 0};
		int extra = 0;
		for (int c = 0; c < MAX_DOP; c++)
		{
			DRL_UIBpDop chip = m_DopChips[c];
			if (c >= m_Dop.Count())
			{
				chip.Hide();
				continue;
			}
			DRL_UIBpDopData d = m_Dop[c];
			int pos = PartPos(d.m_Cls);
			DRL_UIBpDopData shown = new DRL_UIBpDopData();
			shown.m_Checked = d.m_Checked;
			shown.m_Locked = d.m_Locked;
			shown.m_Dim = d.m_Dim;
			shown.m_Qty = d.m_Qty;
			shown.m_VarCount = d.m_VarCount;
			shown.m_Ok = d.m_Ok;
			string label = PartLabel(pos, d.m_Name);
			if (d.m_TimeSec > 0)
				label = label + "  +" + DRL_UIText.Time(d.m_TimeSec);
			shown.m_Name = label;
			float w = chip.Measure(shown);

			float px = 0;
			float py = 0;
			int align = 0;
			float ax = cx;
			float ay = y0 + 200 * sc;
			int n = 0;
			if (pos >= 0)
			{
				n = used[pos];
				used.Set(pos, n + 1);
			}
			if (pos == 0)
			{
				px = cx + 80;
				py = y0 - 14 * sc - 34;
				ax = x0 + 75 * sc;
				ay = y0 + 70 * sc;
			}
			else if (pos == 1)
			{
				px = x0 + 162 * sc + 14;
				py = y0 + 80 * sc;
				align = 2;
				ax = x0 + 75 * sc;
				ay = y0 + 80 * sc;
			}
			else if (pos == 2)
			{
				px = cx - 90;
				py = y0 - 14 * sc - 34;
				ax = x0 + 40 * sc;
				ay = y0 + 6 * sc;
			}
			else if (pos == 3)
			{
				// Левее центра: справа внизу блок характеристик.
				px = cx - 90;
				py = y0 + 446 * sc + 40;
				ax = cx;
				ay = y0 + 390 * sc;
			}
			else if (pos == 4)
			{
				px = x0 - 12 * sc - 14;
				py = y0 + 195 * sc;
				align = 1;
				ax = x0 + 10 * sc;
				ay = y0 + 195 * sc;
			}
			else if (pos == 5)
			{
				px = x0 + 162 * sc + 14;
				py = y0 + 195 * sc;
				align = 2;
				ax = x0 + 140 * sc;
				ay = y0 + 195 * sc;
			}
			else if (pos == 6)
			{
				px = x0 - 12 * sc - 14;
				py = y0 + 365 * sc;
				align = 1;
				ax = x0;
				ay = y0 + 365 * sc;
			}
			else
			{
				// Не распознанная деталь - столбиком справа внизу.
				px = x0 + 162 * sc + 14;
				py = y0 + 300 * sc + extra * 38;
				align = 2;
				ax = x0 + 150 * sc;
				ay = y0 + 300 * sc;
				extra++;
			}
			py = py + n * 38;

			float left = px - w * 0.5;
			float edgeX = px;
			if (align == 1)
			{
				left = px - w;
				edgeX = px;
			}
			else if (align == 2)
			{
				left = px;
				edgeX = px;
			}
			if (left < 8)
				left = 8;
			if (left + w > m_BayW - 8)
				left = m_BayW - 8 - w;
			chip.Place(left, py - 15, w, shown);

			int lead = DRL_UIC.White(90);
			int dotColor = DRL_UIC.White(110);
			if (d.m_Checked && d.m_Ok)
			{
				lead = DRL_UIC.Ok(170);
				dotColor = DRL_UIC.Ok();
			}
			else if (d.m_Checked)
			{
				lead = DRL_UIC.No(190);
				dotColor = DRL_UIC.No();
			}
			float ey = py;
			if (align == 0 && py < ay)
				ey = py + 15;
			else if (align == 0)
				ey = py - 15;
			array<float> xs = new array<float>();
			array<float> ys = new array<float>();
			xs.Insert(edgeX);
			ys.Insert(ey);
			xs.Insert(ax);
			ys.Insert(ay);
			m_BayLines.Dashed(xs, ys, 4, 4, 1.5, lead);
			Dot(ax, ay, 7, dotColor);
		}
	}

	protected void LayoutPaint()
	{
		int n = m_VarNames.Count();
		bool show = n > 1;
		m_PaintLabel.Show(show);
		float x = 18 + 70;
		float y = m_BayH - 56;
		for (int i = 0; i < PAINTS; i++)
		{
			bool on = show && i < n;
			m_PaintDots[i].Show(on);
			m_PaintHits[i].Show(on);
			m_PaintRings[i].Show(on && i == m_VarSel);
			if (!on)
				continue;
			float dx = x + i * 32;
			INT_UIF_Draw.Place(m_PaintDots[i], dx + 4, y + 4, 22, 22);
			m_PaintDots[i].SetColor(m_VarColors[i]);
			INT_UIF_Draw.Place(m_PaintRings[i], dx, y, 30, 30);
			INT_UIF_Draw.Place(m_PaintHits[i], dx + 2, y + 2, 26, 26);
		}
		INT_UIF_Draw.Place(m_PaintLabel, 18, y + 7, 70, 16);
	}

	// Блок характеристик: строка груза и навесное (с переносом), высота по тексту.
	protected void LayoutInfo()
	{
		bool show = m_InfoLines.Count() > 0;
		m_Info.Show(show);
		if (!show)
			return;
		m_InfoCargo.SetText(m_InfoLines[0]);
		string atts = "";
		if (m_InfoLines.Count() > 1)
			atts = m_InfoLines[1];
		m_InfoAtts.SetText(atts);
		// Длина навесного в строках по ширине текста (MultilineTextWidget переносит сам).
		float aw = DRL_UIMeasure.TextW(m_Info, atts, 11, false);
		int lines = (int)(aw / (INFO_W - 28)) + 1;
		if (lines > 4)
			lines = 4;
		float ih = 48 + lines * 15 + 8;
		INT_UIF_Draw.Place(m_Info, m_BayW - 18 - INFO_W, m_BayH - 22 - ih, INFO_W, ih);
		INT_UIF_Draw.Resize(m_InfoBg.GetRoot(), INFO_W, ih);
		INT_UIF_Draw.Resize(m_InfoAtts, INFO_W - 24, lines * 15 + 4);
	}

	// Группа компонента по классу.
	protected int GroupOf(string cls)
	{
		string low = cls;
		low.ToLower();
		array<string> metal = {"metalplate", "metalparts", "bolts", "nuts", "screw", "metal", "nail"};
		array<string> elec = {"battery", "wire", "cable", "rele", "capacit", "headlight", "magnet", "engine", "gauge", "spark", "lamp", "electr"};
		array<string> chem = {"oil", "gasolin", "poxearm", "klean", "foam", "tape", "skoth", "glue", "paint", "fuel"};
		foreach (string a : metal)
		{
			if (low.IndexOf(a) > -1)
				return 0;
		}
		foreach (string b : elec)
		{
			if (low.IndexOf(b) > -1)
				return 1;
		}
		foreach (string c : chem)
		{
			if (low.IndexOf(c) > -1)
				return 2;
		}
		return 3;
	}

	protected void LayoutSpec()
	{
		array<ref DRL_UISpecData> rows = new array<ref DRL_UISpecData>();
		int got = 0;
		int need = 0;
		array<string> groups = {"#STR_DRL_UI_ASM_G_METAL", "#STR_DRL_UI_ASM_G_ELEC", "#STR_DRL_UI_ASM_G_CHEM", "#STR_DRL_UI_ASM_G_BODY"};
		for (int g = 0; g < 4; g++)
		{
			bool headed = false;
			foreach (DRL_UIBpItem it : m_Side)
			{
				if (it.m_Kind != DRL_BP_INGR || GroupOf(it.m_Cls) != g)
					continue;
				if (!headed)
				{
					DRL_UISpecData h = new DRL_UISpecData();
					h.m_Header = true;
					string groupName = DRL_UIText.Tr(groups[g]);
					h.m_Name = groupName;
					rows.Insert(h);
					headed = true;
				}
				DRL_UISpecData r = new DRL_UISpecData();
				r.m_Name = it.m_Name;
				r.m_Value = it.m_Have + " " + it.m_Need;
				r.m_Ok = it.m_Ok;
				r.m_Glyph = it.m_Glyph;
				r.m_Tip = it.m_Tip;
				rows.Insert(r);
				need = need + it.m_NeedN;
				int part = it.m_HaveN;
				if (part > it.m_NeedN)
					part = it.m_NeedN;
				got = got + part;
			}
		}
		bool toolsHeaded = false;
		array<DRL_UIBpItem> tools = new array<DRL_UIBpItem>();
		foreach (DRL_UIBpItem t : m_Side)
		{
			if (t.m_Kind != DRL_BP_INGR)
				tools.Insert(t);
		}
		foreach (DRL_UIBpItem tp : m_Top)
		{
			tools.Insert(tp);
		}
		foreach (DRL_UIBpItem tool : tools)
		{
			if (!toolsHeaded)
			{
				DRL_UISpecData th = new DRL_UISpecData();
				th.m_Header = true;
				th.m_Name = DRL_UIText.Tr("#STR_DRL_UI_ASM_G_TOOLS");
				rows.Insert(th);
				toolsHeaded = true;
			}
			DRL_UISpecData tr = new DRL_UISpecData();
			tr.m_Name = tool.m_Name;
			tr.m_Value = tool.m_Sub;
			tr.m_Ok = tool.m_Ok;
			tr.m_Tool = true;
			tr.m_Glyph = tool.m_Glyph;
			tr.m_Tip = tool.m_Tip;
			rows.Insert(tr);
		}
		m_SpecList.SetItems(rows, false);

		m_SpecGot.SetText(got.ToString());
		float gw = DRL_UIMeasure.TextW(m_Spec, got.ToString(), 21, true);
		INT_UIF_Draw.Place(m_SpecNeed, 14 + gw + 6, 36, SPEC_W - 40 - gw, 18);
		m_SpecNeed.SetText(DRL_UIText.Fmt("#STR_DRL_UI_ASM_ITEMS", need.ToString()));
		float frac = 0;
		if (need > 0)
			frac = got;
		if (need > 0)
			frac = frac / need;
		float fw = m_SpecBarW * frac;
		if (fw < 2)
			fw = 2;
		INT_UIF_Draw.Resize(m_SpecFill, fw, 4);
		int fillColor = DRL_UIC.A();
		if (frac >= 1)
			fillColor = DRL_UIC.Ok();
		m_SpecFill.SetColor(fillColor);
	}

	// --- события ----------------------------------------------------------------------------
	protected int PaintOf(Widget w)
	{
		for (int i = 0; i < m_PaintHits.Count(); i++)
		{
			if (m_PaintHits[i] == w)
				return i;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;
		int pi = PaintOf(w);
		if (pi >= 0)
		{
			DRL_UISound.Select();
			m_OnAction.Invoke("var", pi, 0);
			return true;
		}
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
		int pi = PaintOf(w);
		if (pi >= 0 && pi < m_VarNames.Count())
		{
			DRL_UISound.Hover();
			INT_UIF_Tooltip.ShowFor(m_PaintDots[pi], m_VarNames[pi]);
			return true;
		}
		int action;
		int di = DopOf(w, action);
		if (di >= 0 && action == 0)
			DRL_UISound.Hover();
		if (di >= 0 && action == 0 && di < m_Dop.Count() && m_Dop[di].m_Tip != "")
		{
			INT_UIF_Tooltip.ShowFor(m_DopChips[di].m_Root, m_Dop[di].m_Tip);
			return true;
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int action;
		if (PaintOf(w) >= 0 || DopOf(w, action) >= 0)
		{
			INT_UIF_Tooltip.HideTip();
			return true;
		}
		return false;
	}

	// Тестовый хук: «var» - окраска, «dop» - деталь.
	override void TestClick(string what, int index, int action)
	{
		if (what == "var" && index >= 0 && index < m_PaintHits.Count())
		{
			OnClick(m_PaintHits[index], 0, 0, MouseState.LEFT);
			return;
		}
		super.TestClick(what, index, action);
	}
}
