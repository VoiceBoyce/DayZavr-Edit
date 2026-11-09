// ============================================================================
// DRL_CraftMenu - детали окна крафта по макету «макет верстака DRL»:
// кнопка-пилюля, большая кнопка крафта, очередь карточками и обвесы станции.
// Цвета - из DRL_UIC (акцент станции). Размеры - дизайн-пиксели 1920x1080.
// ============================================================================

// Кнопка-пилюля: заливка, рамка, значок и текст. Ширина по содержимому.
class DRL_UIPill extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref INT_UIF_Surface m_Bg;
	protected ImageWidget m_Icon;
	protected TextWidget m_Text;
	protected ButtonWidget m_Hit;
	protected float m_H;
	protected float m_W;
	protected float m_TextSize;
	protected int m_Fill;
	protected int m_Border;
	protected int m_Fg;
	protected bool m_Enabled = true;
	protected bool m_Hover;
	protected ref ScriptInvoker m_OnClick;

	void DRL_UIPill()
	{
		m_OnClick = new ScriptInvoker();
	}

	// (DRL_UIPill pill)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	void Build(Widget parent, float x, float y, float h, int radius, float textSize, string name)
	{
		m_H = h;
		m_W = 60;
		m_TextSize = textSize;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, 60, h, name + "Root");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, 60, h, radius, 0, 0);
		m_Icon = INT_UIF_Draw.Icon(m_Root, 10, (h - 14) * 0.5, 14, "check", 0);
		m_Icon.Show(false);
		m_Text = INT_UIF_Draw.Text(m_Root, 10, 0, 40, h, "", textSize, 0);
		m_Text.SetBold(true);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, 60, h, this);
		DRL_UIKitUtil.ExposeHit(m_Hit, name, this);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	float GetWidth()
	{
		return m_W;
	}

	// fixedW > 0 - своя ширина, содержимое по центру. Возвращает ширину.
	float Set(string text, string icon, float fixedW = 0)
	{
		string t = DRL_UIText.Tr(text);
		float tw = 0;
		if (t != "")
			tw = DRL_UIMeasure.TextW(m_Root, t, m_TextSize, true);
		float isz = m_TextSize + 2;
		float iw = 0;
		if (icon != "" && t != "")
			iw = isz + 6;
		// Только значок (крестик, минус, плюс): строго по центру, без отступа под текст.
		if (icon != "" && t == "")
		{
			isz = m_TextSize;
			iw = isz;
		}
		m_Text.Show(t != "");
		float content = iw + tw;
		float w = content + 22;
		if (fixedW > 0)
			w = fixedW;
		m_W = w;
		INT_UIF_Draw.Resize(m_Root, w, m_H);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, m_H);
		INT_UIF_Draw.Resize(m_Hit, w, m_H);
		float x = (w - content) * 0.5;
		if (x < 6)
			x = 6;
		m_Icon.Show(icon != "");
		if (icon != "")
		{
			INT_UIF_Widgets.SetIcon(m_Icon, icon);
			INT_UIF_Draw.Place(m_Icon, x, (m_H - isz) * 0.5, isz, isz);
		}
		float textW = w - x - iw - 4;
		INT_UIF_Draw.Place(m_Text, x + iw, 0, textW, m_H);
		m_Text.SetText(t);
		return w;
	}

	void SetColors(int fill, int border, int fg)
	{
		m_Fill = fill;
		m_Border = border;
		m_Fg = fg;
		Paint();
	}

	void SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
		Paint();
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	void Move(float x, float y)
	{
		INT_UIF_Draw.Move(m_Root, x, y);
	}

	void Show(bool show)
	{
		m_Root.Show(show);
	}

	protected void Paint()
	{
		int fill = m_Fill;
		int border = m_Border;
		int fg = m_Fg;
		if (!m_Enabled)
		{
			fill = DRL_UIC.White(14);
			border = 0;
			fg = DRL_UIC.Dim();
		}
		else if (m_Hover)
		{
			if (border != 0)
				border = INT_UIF_Color.WithAlpha(border, 255);
			else
				border = DRL_UIC.White(60);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Text.SetColor(fg);
		m_Icon.SetColor(fg);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit)
			return false;
		if (m_Enabled && button == MouseState.LEFT)
			m_OnClick.Invoke(this);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		Paint();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		Paint();
		return true;
	}
}

// Большая кнопка крафта: стиль по состоянию, вторая строка и полоска прогресса.
const int DRL_BTN_GO = 0;
const int DRL_BTN_QUEUE = 1;
const int DRL_BTN_WAIT = 2;
const int DRL_BTN_CANCEL = 3;
const int DRL_BTN_OFF = 4;
const int DRL_BTN_LEARN = 5;

class DRL_UIBigBtn extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref INT_UIF_Surface m_Bg;
	protected ImageWidget m_Sheen;
	protected ImageWidget m_Icon;
	protected TextWidget m_Text;
	protected TextWidget m_Sub;
	protected ImageWidget m_Track;
	protected ImageWidget m_Fill;
	protected ButtonWidget m_Hit;
	protected float m_W;
	protected float m_H;
	protected int m_Style;
	protected bool m_Enabled = true;
	protected bool m_Hover;
	protected string m_TextValue;
	protected ref ScriptInvoker m_OnClick;

	void DRL_UIBigBtn()
	{
		m_OnClick = new ScriptInvoker();
	}

	// (DRL_UIBigBtn btn)
	ScriptInvoker OnClick()
	{
		return m_OnClick;
	}

	void Build(Widget parent, float x, float y, float w, float h, string name)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, name + "Root");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, 0, 0);
		// Блик с отступом на радиус скругления: углы не вылезают.
		int sheen = DRL_UIC.White(30);
		m_Sheen = INT_UIF_Draw.Image(m_Root, 12, 0, w - 24, h * 0.5, "iuf_grad_v", sheen);
		m_Icon = INT_UIF_Draw.Icon(m_Root, 20, (h - 20) * 0.5, 20, "gear", 0);
		m_Text = INT_UIF_Draw.Text(m_Root, 46, 0, w - 56, h, "", 15, 0);
		m_Text.SetBold(true);
		m_Sub = INT_UIF_Draw.Text(m_Root, 14, h * 0.5, w - 28, 16, "", 11, 0, INT_UIF_Align.CENTER);
		m_Sub.SetBold(true);
		m_Sub.Show(false);
		int trackColor = DRL_UIC.White(20);
		m_Track = INT_UIF_Draw.Line(m_Root, 12, h - 5, w - 24, 4, trackColor);
		int fillColor = DRL_UIC.Tl();
		m_Fill = INT_UIF_Draw.Line(m_Root, 12, h - 5, 10, 4, fillColor);
		m_Track.Show(false);
		m_Fill.Show(false);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		DRL_UIKitUtil.ExposeHit(m_Hit, name, this);
	}

	bool IsEnabled()
	{
		return m_Enabled;
	}

	string GetText()
	{
		return m_TextValue;
	}

	void Show(bool show)
	{
		m_Root.Show(show);
	}

	void SetState(int style, string text, string sub, string icon, bool enabled)
	{
		m_Style = style;
		m_Enabled = enabled;
		string t = DRL_UIText.Tr(text);
		string s = DRL_UIText.Tr(sub);
		m_TextValue = t;
		bool hasSub = s != "";
		float size = 15;
		if (hasSub || style == DRL_BTN_OFF)
			size = 13;
		float tw = DRL_UIMeasure.TextW(m_Root, t, size, true);
		float iw = 0;
		if (icon != "")
			iw = 26;
		float content = iw + tw;
		if (content > m_W - 24)
			content = m_W - 24;
		float x = (m_W - content) * 0.5;
		float textY = 0;
		float textH = m_H;
		if (hasSub)
		{
			textY = 5;
			textH = m_H * 0.5;
		}
		m_Icon.Show(icon != "");
		if (icon != "")
		{
			INT_UIF_Widgets.SetIcon(m_Icon, icon);
			INT_UIF_Draw.Place(m_Icon, x, textY + (textH - 18) * 0.5, 18, 18);
		}
		INT_UIF_Draw.Place(m_Text, x + iw, textY, content - iw + 4, textH);
		m_Text.SetTextExactSize(INT_UIF_Scale.TextPx(size));
		m_Text.SetText(t);
		m_Sub.Show(hasSub);
		if (hasSub)
		{
			INT_UIF_Draw.Place(m_Sub, 12, m_H * 0.5 + 1, m_W - 24, 16);
			m_Sub.SetText(s);
		}
		Paint();
	}

	// p < 0 - без полоски.
	void SetProgress(float p)
	{
		bool on = p >= 0;
		m_Track.Show(on);
		m_Fill.Show(on);
		if (!on)
			return;
		float fw = (m_W - 24) * Math.Clamp(p, 0, 1);
		if (fw < 2)
			fw = 2;
		INT_UIF_Draw.Resize(m_Fill, fw, 4);
	}

	protected void Paint()
	{
		int fill = DRL_UIC.A();
		int border = 0;
		int fg = DRL_UIC.OnAccent();
		bool sheen = true;
		if (m_Style == DRL_BTN_QUEUE)
		{
			fill = DRL_UIC.Blue();
			fg = ARGB(255, 8, 22, 44);
		}
		else if (m_Style == DRL_BTN_WAIT)
		{
			fill = DRL_UIC.Lk(30);
			border = DRL_UIC.Lk(128);
			fg = DRL_UIC.Lk();
			sheen = false;
		}
		else if (m_Style == DRL_BTN_CANCEL)
		{
			fill = DRL_UIC.No(36);
			border = DRL_UIC.No(140);
			fg = ARGB(255, 255, 138, 138);
			sheen = false;
		}
		else if (m_Style == DRL_BTN_OFF)
		{
			fill = DRL_UIC.White(18);
			fg = DRL_UIC.Dim();
			sheen = false;
		}
		else if (m_Style == DRL_BTN_LEARN)
		{
			fill = DRL_UIC.Lk();
			fg = ARGB(255, 43, 26, 0);
		}
		if (m_Hover && m_Enabled)
		{
			if (border != 0)
				border = INT_UIF_Color.WithAlpha(border, 255);
			else
				border = DRL_UIC.White(110);
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		m_Sheen.Show(sheen);
		m_Text.SetColor(fg);
		m_Icon.SetColor(fg);
		int subColor = DRL_UIC.Muted();
		m_Sub.SetColor(subColor);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit)
			return false;
		if (m_Enabled && button == MouseState.LEFT)
			m_OnClick.Invoke(this);
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = true;
		Paint();
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hover = false;
		Paint();
		return true;
	}
}

// --- очередь крафта карточками ----------------------------------------------------------
class DRL_UIQueueRow extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ref INT_UIF_Surface m_Pic;
	Widget m_Clip;
	ref DRL_UIItemView m_View;
	ImageWidget m_Image;
	ImageWidget m_Glyph;
	TextWidget m_Name;
	TextWidget m_Sub;
	TextWidget m_Time;
	ImageWidget m_Clock;
	Widget m_Cancel;
	ButtonWidget m_CancelHit;
	ButtonWidget m_Hit;
	ImageWidget m_Track;
	ImageWidget m_Fill;
	TextWidget m_Free;
	int m_Index = -1;
	float m_W;
	float m_H;
	bool m_HasModel;
	bool m_ModelsHidden;

	void Build(Widget parent, int slot, float x, float y, float w, float h, ScriptedWidgetEventHandler handler)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_QRow");
		m_Bg = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 10, 0, 0);
		int picFill = ARGB(77, 0, 0, 0);
		m_Pic = INT_UIF_Surface.Box(m_Root, 6, 6, h - 12, h - 12, 8, picFill, 0);
		m_Clip = INT_UIF_Draw.Clip(m_Root, 8, 8, h - 16, h - 16, "DRL_QClip");
		m_View = new DRL_UIItemView();
		m_View.Create(m_Clip, true);
		m_View.SetOrientation(Vector(0, 0, 0));
		int white = DRL_UIC.White(235);
		m_Image = INT_UIF_Draw.Image(m_Root, 10, 10, h - 20, h - 20, "iuf_white", white);
		m_Image.Show(false);
		int dim = DRL_UIC.Dim();
		m_Glyph = INT_UIF_Draw.Icon(m_Root, (h - 16) * 0.5, (h - 16) * 0.5, 16, "gear", dim);
		int txt = DRL_UIC.Txt();
		float tx = h + 4;
		m_Name = INT_UIF_Draw.Text(m_Root, tx, 8, w - tx - 90, 18, "", 12, txt);
		m_Name.SetBold(true);
		m_Sub = INT_UIF_Draw.Text(m_Root, tx, 26, w - tx - 90, 16, "", 11, dim);
		m_Sub.SetBold(true);
		int tl = DRL_UIC.Tl();
		m_Time = INT_UIF_Draw.Text(m_Root, w - 96, 0, 56, h, "", 15, tl, INT_UIF_Align.RIGHT);
		m_Time.SetBold(true);
		int lk = DRL_UIC.Lk();
		m_Clock = INT_UIF_Draw.Icon(m_Root, w - 56, (h - 14) * 0.5, 14, "clock", lk);
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, handler);
		DRL_UIKitUtil.ExposeHit(m_Hit, "DRL_Q_" + slot.ToString(), handler);
		m_Cancel = INT_UIF_Draw.Frame(m_Root, w - 34, (h - 26) * 0.5, 26, 26, "DRL_QCancel");
		int cancelFill = DRL_UIC.No(36);
		INT_UIF_Surface.Box(m_Cancel, 0, 0, 26, 26, 6, cancelFill, 0);
		int cancelIcon = ARGB(255, 255, 138, 138);
		INT_UIF_Draw.Icon(m_Cancel, 8, 8, 10, "close", cancelIcon);
		m_CancelHit = INT_UIF_Draw.Hit(m_Cancel, 0, 0, 26, 26, handler);
		DRL_UIKitUtil.ExposeHit(m_CancelHit, "DRL_QCancel_" + slot.ToString(), handler);
		DRL_UIKitUtil.SetSortDeep(m_Cancel, DRL_UI_SORT_OVER_MODEL);
		int trackColor = DRL_UIC.White(16);
		m_Track = INT_UIF_Draw.Line(m_Root, 10, h - 4, w - 20, 3, trackColor);
		m_Fill = INT_UIF_Draw.Line(m_Root, 10, h - 4, 10, 3, tl);
		m_Free = INT_UIF_Draw.Text(m_Root, 0, 0, w, h, "#STR_DRL_UI_Q_FREE", 12, dim, INT_UIF_Align.CENTER);
		m_Free.SetBold(true);
	}

	// proc == null - свободное место.
	void Bind(int index, DRL_UIProc proc)
	{
		m_Index = index;
		bool isFree = proc == null;
		m_Free.Show(isFree);
		m_Pic.Show(!isFree);
		m_Name.Show(!isFree);
		m_Sub.Show(!isFree);
		m_Cancel.Show(!isFree);
		m_Hit.Show(!isFree);
		if (isFree)
		{
			m_View.Clear();
			m_HasModel = false;
			m_Image.Show(false);
			m_Glyph.Show(false);
			m_Time.Show(false);
			m_Clock.Show(false);
			m_Track.Show(false);
			m_Fill.Show(false);
			m_Bg.SetFill(0);
			int freeBorder = DRL_UIC.White(26);
			m_Bg.SetBorder(freeBorder);
			return;
		}
		DRL_Craft_Recipe r = null;
		if (g_Craft_Config)
			r = g_Craft_Config.FindById(proc.GetRecipeId());
		bool useIcon = r && r.CraftIcon != "";
		m_HasModel = false;
		if (useIcon)
		{
			m_View.Clear();
			m_Image.LoadImageFile(0, DRL_CraftUIHelper.ResolveImagePath(r.CraftIcon));
			m_Image.SetImage(0);
		}
		else if (proc.m_Info)
		{
			m_HasModel = m_View.SetClass(proc.m_Info.m_ResultClass);
		}
		m_View.Show(m_HasModel && !m_ModelsHidden);
		m_Image.Show(useIcon);
		m_Glyph.Show(!useIcon && !m_HasModel);
		string name = "";
		if (r)
			name = r.RecipeName;
		else if (proc.m_Info)
			name = DRL_UIText.DisplayName(proc.m_Info.m_ResultClass);
		m_Name.SetText(name);

		bool queued = proc.IsQueued();
		m_Time.Show(!queued);
		m_Clock.Show(queued);
		m_Track.Show(!queued);
		m_Fill.Show(!queued);
		int fill = DRL_UIC.White(13);
		int border = DRL_UIC.White(20);
		if (queued)
		{
			border = DRL_UIC.Lk(90);
			m_Sub.SetText(DRL_UIText.Tr("#STR_DRL_UI_Q_WAIT"));
		}
		else
		{
			int cnt = 1;
			if (proc.m_Info && proc.m_Info.m_CraftCount > 0)
				cnt = proc.m_Info.m_CraftCount;
			if (r && r.ResultCount > 1)
				cnt = cnt * r.ResultCount;
			m_Sub.SetText(DRL_UIText.Fmt("#STR_DRL_UI_Q_RUN", cnt.ToString()));
		}
		m_Bg.SetFill(fill);
		m_Bg.SetBorder(border);
		UpdateProgress(proc);
	}

	void UpdateProgress(DRL_UIProc proc)
	{
		if (!proc || proc.IsQueued())
			return;
		int left = proc.RemainingSec();
		if (left < 0)
			left = 0;
		int mm = left / 60;
		int ss = left - mm * 60;
		string sec = ss.ToString();
		if (ss < 10)
			sec = "0" + sec;
		string mins = mm.ToString();
		if (mm < 10)
			mins = "0" + mins;
		m_Time.SetText(mins + ":" + sec);
		float fw = (m_W - 20) * proc.Percent() / 100.0;
		if (fw < 2)
			fw = 2;
		INT_UIF_Draw.Resize(m_Fill, fw, 3);
	}

	void ShowModel(bool show)
	{
		m_ModelsHidden = !show;
		m_View.Show(show && m_HasModel);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		m_HasModel = false;
	}
}

class DRL_UIQueueList extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected ref array<ref DRL_UIQueueRow> m_Rows;
	protected ref array<ref DRL_UIProc> m_Procs;
	protected TextWidget m_More;
	protected float m_RowH;
	protected float m_Gap;
	protected ref ScriptInvoker m_OnAction;

	void DRL_UIQueueList()
	{
		m_Rows = new array<ref DRL_UIQueueRow>();
		m_Procs = new array<ref DRL_UIProc>();
		m_OnAction = new ScriptInvoker();
	}

	void ~DRL_UIQueueList()
	{
		Clear();
	}

	// (int index, string action, DRL_UIQueueList list): "select" / "cancel"
	ScriptInvoker OnAction()
	{
		return m_OnAction;
	}

	void Build(Widget parent, float x, float y, float w, float rowH, float gap, int maxRows)
	{
		m_RowH = rowH;
		m_Gap = gap;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, maxRows * (rowH + gap) + 20, "DRL_QueueList");
		for (int i = 0; i < maxRows; i++)
		{
			DRL_UIQueueRow row = new DRL_UIQueueRow();
			row.Build(m_Root, i, 0, i * (rowH + gap), w, rowH, this);
			row.m_Root.Show(false);
			m_Rows.Insert(row);
		}
		int dim = DRL_UIC.Dim();
		m_More = INT_UIF_Draw.Text(m_Root, 0, 0, w, 16, "", 11, dim, INT_UIF_Align.CENTER);
		m_More.Show(false);
	}

	// Возвращает занятую высоту.
	float SetProcs(array<ref DRL_UIProc> procs, int maxSlots)
	{
		m_Procs.Clear();
		if (procs)
		{
			foreach (DRL_UIProc p : procs)
			{
				m_Procs.Insert(p);
			}
		}
		int rows = maxSlots;
		if (rows < m_Procs.Count())
			rows = m_Procs.Count();
		if (rows > m_Rows.Count())
			rows = m_Rows.Count();
		for (int i = 0; i < m_Rows.Count(); i++)
		{
			DRL_UIQueueRow row = m_Rows[i];
			if (i >= rows)
			{
				row.m_Root.Show(false);
				row.Clear();
				continue;
			}
			row.m_Root.Show(true);
			DRL_UIProc proc = null;
			if (i < m_Procs.Count())
				proc = m_Procs[i];
			row.Bind(i, proc);
		}
		float h = rows * (m_RowH + m_Gap) - m_Gap;
		int hidden = m_Procs.Count() - rows;
		m_More.Show(hidden > 0);
		if (hidden > 0)
		{
			INT_UIF_Draw.Move(m_More, 0, h + 4);
			m_More.SetText("+" + hidden.ToString());
			h = h + 20;
		}
		return h;
	}

	void Tick()
	{
		for (int i = 0; i < m_Rows.Count() && i < m_Procs.Count(); i++)
		{
			m_Rows[i].UpdateProgress(m_Procs[i]);
		}
	}

	void HideModels(bool hide)
	{
		foreach (DRL_UIQueueRow row : m_Rows)
		{
			row.ShowModel(!hide);
		}
	}

	void Clear()
	{
		foreach (DRL_UIQueueRow row : m_Rows)
		{
			if (row)
				row.Clear();
		}
	}

	void TestAction(int index, string action)
	{
		if (index < 0 || index >= m_Procs.Count())
			return;
		m_OnAction.Invoke(index, action, this);
	}

	protected int RowOf(Widget w, out bool cancel)
	{
		cancel = false;
		for (int i = 0; i < m_Rows.Count(); i++)
		{
			if (m_Rows[i].m_Hit == w)
				return i;
			if (m_Rows[i].m_CancelHit == w)
			{
				cancel = true;
				return i;
			}
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		bool cancel;
		int i = RowOf(w, cancel);
		if (i < 0)
			return false;
		if (i >= m_Procs.Count() || button != MouseState.LEFT)
			return true;
		if (cancel)
			m_OnAction.Invoke(i, "cancel", this);
		else
			m_OnAction.Invoke(i, "select", this);
		return true;
	}
}

// --- обвесы станции ----------------------------------------------------------------------
class DRL_UISlotTile extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	TextWidget m_Name;
	TextWidget m_NameL;
	Widget m_Clip;
	ref DRL_UIItemView m_View;
	ImageWidget m_Glyph;
	TextWidget m_Empty;
	ImageWidget m_DurTrack;
	ImageWidget m_DurFill;
	ButtonWidget m_Hit;
	string m_Tip;
	bool m_HasModel;
	bool m_ModelsHidden;
}

class DRL_UIStationSlots extends ScriptedWidgetEventHandler
{
	static const int COLS = 4;
	static const float TILE_H = 58;
	static const float GAP = 5;

	protected Widget m_Root;
	protected TextWidget m_Title;
	protected TextWidget m_Count;
	protected MultilineTextWidget m_Note;
	protected ref array<ref DRL_UISlotTile> m_Tiles;
	protected float m_W;
	protected float m_TileW;

	void DRL_UIStationSlots()
	{
		m_Tiles = new array<ref DRL_UISlotTile>();
	}

	void ~DRL_UIStationSlots()
	{
		Clear();
	}

	void Build(Widget parent, float x, float y, float w, int maxTiles)
	{
		m_W = w;
		m_TileW = (w - (COLS - 1) * GAP) / COLS;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, 300, "DRL_StationSlots");
		int dim = DRL_UIC.Dim();
		int muted = DRL_UIC.Muted();
		m_Title = INT_UIF_Draw.Text(m_Root, 0, 0, w - 60, 16, "", 10, dim);
		m_Title.SetBold(true);
		m_Count = INT_UIF_Draw.Text(m_Root, w - 80, 0, 80, 16, "", 11, muted, INT_UIF_Align.RIGHT);
		m_Count.SetBold(true);
		int txt = DRL_UIC.Txt();
		int tl = DRL_UIC.Tl();
		int track = DRL_UIC.White(26);
		for (int i = 0; i < maxTiles; i++)
		{
			DRL_UISlotTile t = new DRL_UISlotTile();
			t.m_Root = INT_UIF_Draw.Frame(m_Root, 0, 0, m_TileW, TILE_H, "DRL_StSlot");
			t.m_Bg = INT_UIF_Surface.Box(t.m_Root, 0, 0, m_TileW, TILE_H, 10, 0, 0);
			t.m_Name = INT_UIF_Draw.Text(t.m_Root, 3, 2, m_TileW - 6, 14, "", 9, dim, INT_UIF_Align.CENTER);
			t.m_Name.SetBold(true);
			Widget nameClip = INT_UIF_Draw.Clip(t.m_Root, 4, 2, m_TileW - 8, 14, "DRL_StSlotName");
			t.m_NameL = INT_UIF_Draw.Text(nameClip, 0, 0, 200, 14, "", 9, dim);
			t.m_NameL.SetBold(true);
			t.m_NameL.Show(false);
			t.m_Clip = INT_UIF_Draw.Clip(t.m_Root, (m_TileW - 40) * 0.5, 16, 40, 30, "DRL_StSlotClip");
			t.m_View = new DRL_UIItemView();
			t.m_View.Create(t.m_Clip, true);
			t.m_View.SetOrientation(Vector(0, 30, 0));
			t.m_Glyph = INT_UIF_Draw.Icon(t.m_Root, (m_TileW - 16) * 0.5, 20, 16, "plus", dim);
			t.m_Empty = INT_UIF_Draw.Text(t.m_Root, 2, 38, m_TileW - 4, 14, "", 9, dim, INT_UIF_Align.CENTER);
			t.m_Empty.SetBold(true);
			t.m_DurTrack = INT_UIF_Draw.Line(t.m_Root, 7, TILE_H - 8, m_TileW - 14, 3, track);
			t.m_DurFill = INT_UIF_Draw.Line(t.m_Root, 7, TILE_H - 8, 10, 3, tl);
			t.m_Hit = INT_UIF_Draw.Hit(t.m_Root, 0, 0, m_TileW, TILE_H, this);
			DRL_UIKitUtil.ExposeHit(t.m_Hit, "DRL_StSlot_" + i.ToString(), this);
			t.m_Root.Show(false);
			m_Tiles.Insert(t);
		}
		m_Note = INT_UIF_Draw.MText(m_Root, 0, 0, w, 44, "#STR_DRL_UI_ST_NOTE", 10, dim);
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Show(bool show)
	{
		m_Root.Show(show);
	}

	// need - обвесы (AttachmentsEx = 1) выбранного рецепта. Возвращает высоту блока.
	float SetStation(EntityAI wb, array<DRL_Craft_Component> need, string title, string note)
	{
		m_Title.SetText(title);
		int total = 0;
		if (wb && wb.GetInventory())
			total = wb.GetInventory().GetAttachmentSlotsCount();
		if (total > m_Tiles.Count())
			total = m_Tiles.Count();
		int filled = 0;
		int tl = DRL_UIC.Tl();
		for (int i = 0; i < m_Tiles.Count(); i++)
		{
			DRL_UISlotTile t = m_Tiles[i];
			if (i >= total)
			{
				t.m_Root.Show(false);
				t.m_View.Clear();
				t.m_HasModel = false;
				continue;
			}
			int row = i / COLS;
			int col = i - row * COLS;
			t.m_Root.Show(true);
			INT_UIF_Draw.Move(t.m_Root, col * (m_TileW + GAP), 22 + row * (TILE_H + GAP));
			int slotId = wb.GetInventory().GetAttachmentSlotId(i);
			string slotName = InventorySlots.GetSlotDisplayName(slotId);
			if (slotName == "")
				slotName = InventorySlots.GetSlotName(slotId);
			// Длинное имя слота - по левому краю (обрезка справа), короткое - по центру.
			float nameW = DRL_UIMeasure.TextW(t.m_Root, slotName, 9, true);
			bool fits = nameW <= m_TileW - 6;
			t.m_Name.Show(fits);
			t.m_NameL.Show(!fits);
			t.m_Name.SetText(slotName);
			t.m_NameL.SetText(slotName);
			EntityAI att = wb.GetInventory().FindAttachment(slotId);
			bool isNeeded = SlotNeeded(slotId, att, need);
			if (att)
			{
				filled++;
				t.m_HasModel = t.m_View.SetClass(att.GetType());
				t.m_View.Show(t.m_HasModel && !t.m_ModelsHidden);
				t.m_Glyph.Show(!t.m_HasModel);
				INT_UIF_Widgets.SetIcon(t.m_Glyph, "gear");
				t.m_Glyph.SetColor(tl);
				t.m_Empty.Show(false);
				int level = att.GetHealthLevel();
				int pct = DRL_UIC.HealthPct(level);
				t.m_DurTrack.Show(true);
				t.m_DurFill.Show(true);
				float fw = (m_TileW - 14) * pct / 100.0;
				if (fw < 2)
					fw = 2;
				INT_UIF_Draw.Resize(t.m_DurFill, fw, 3);
				t.m_DurFill.SetColor(DRL_UIC.HealthColor(pct));
				t.m_Tip = slotName + " - " + att.GetDisplayName() + " · " + DRL_UIC.HealthName(level);
				int fill = DRL_UIC.White(13);
				int border = DRL_UIC.Tl(90);
				if (isNeeded)
				{
					fill = DRL_UIC.Tl(26);
					border = DRL_UIC.Tl();
				}
				t.m_Bg.SetFill(fill);
				t.m_Bg.SetBorder(border);
			}
			else
			{
				t.m_View.Clear();
				t.m_HasModel = false;
				t.m_Glyph.Show(true);
				INT_UIF_Widgets.SetIcon(t.m_Glyph, "plus");
				t.m_Empty.Show(true);
				t.m_DurTrack.Show(false);
				t.m_DurFill.Show(false);
				int emptyColor = DRL_UIC.Dim();
				string emptyText = DRL_UIText.Tr("#STR_DRL_UI_ST_EMPTY");
				int emptyBorder = DRL_UIC.White(36);
				if (isNeeded)
				{
					emptyColor = ARGB(255, 255, 138, 138);
					emptyText = DRL_UIText.Tr("#STR_DRL_UI_ST_NEEDED");
					emptyBorder = DRL_UIC.No(190);
				}
				t.m_Glyph.SetColor(emptyColor);
				t.m_Empty.SetColor(emptyColor);
				t.m_Empty.SetText(emptyText);
				t.m_Bg.SetFill(0);
				t.m_Bg.SetBorder(emptyBorder);
				t.m_Tip = slotName + " - " + emptyText;
			}
		}
		m_Count.SetText(filled.ToString() + " / " + total.ToString());
		m_Count.Show(total > 0);
		int rows = (total + COLS - 1) / COLS;
		float y = 22 + rows * (TILE_H + GAP);
		m_Note.SetText(note);
		m_Note.Show(note != "");
		INT_UIF_Draw.Move(m_Note, 0, y + 2);
		if (note != "")
			y = y + 48;
		return y;
	}

	// Слот нужен рецепту: стоящий обвес подходит к компоненту, или пустой слот
	// принимает класс недостающего компонента (inventorySlot в конфиге).
	protected bool SlotNeeded(int slotId, EntityAI att, array<DRL_Craft_Component> need)
	{
		if (!need)
			return false;
		string slotName = InventorySlots.GetSlotName(slotId);
		slotName.ToLower();
		foreach (DRL_Craft_Component c : need)
		{
			if (!c)
				continue;
			if (att)
			{
				ItemBase ib = ItemBase.Cast(att);
				if (ib && DRL_CraftComponentResolver.DoesItemMatchComponent(ib, c))
					return true;
				continue;
			}
			array<string> classes = new array<string>();
			if (c.Classname != "")
				classes.Insert(c.Classname);
			if (c.Classnames)
			{
				foreach (string cn : c.Classnames)
				{
					classes.Insert(cn);
				}
			}
			foreach (string cls : classes)
			{
				if (ClassFitsSlot(cls, slotName))
					return true;
			}
		}
		return false;
	}

	protected bool ClassFitsSlot(string cls, string slotLower)
	{
		string path = "CfgVehicles " + cls + " inventorySlot";
		if (!GetGame().ConfigIsExisting(path))
			return false;
		TStringArray slots = new TStringArray();
		GetGame().ConfigGetTextArray(path, slots);
		if (slots.Count() == 0)
		{
			string single = "";
			GetGame().ConfigGetText(path, single);
			if (single != "")
				slots.Insert(single);
		}
		foreach (string s : slots)
		{
			string low = s;
			low.ToLower();
			if (low == slotLower)
				return true;
		}
		return false;
	}

	void HideModels(bool hide)
	{
		foreach (DRL_UISlotTile t : m_Tiles)
		{
			t.m_ModelsHidden = hide;
			t.m_View.Show(!hide && t.m_HasModel);
		}
	}

	void Clear()
	{
		foreach (DRL_UISlotTile t : m_Tiles)
		{
			if (t && t.m_View)
				t.m_View.Clear();
		}
	}

	protected DRL_UISlotTile TileOf(Widget w)
	{
		foreach (DRL_UISlotTile t : m_Tiles)
		{
			if (t.m_Hit == w)
				return t;
		}
		return null;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		DRL_UISlotTile t = TileOf(w);
		if (!t)
			return false;
		if (t.m_Tip != "")
			INT_UIF_Tooltip.ShowFor(t.m_Root, t.m_Tip);
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (!TileOf(w))
			return false;
		INT_UIF_Tooltip.HideTip();
		return true;
	}
}
