// INT PartyMode (client) — pixel-smooth list scroller of the main window (owner 2026-10-23: lists can be long, the
// scroll must be smooth, not by whole rows, with an Expedition scrollbar on the LEFT of the list).
// A pool of rows (viewport / row height + 2) is moved by a pixel offset; a row is rebound only when it wraps to
// another item. A clipping frame cuts images, but NOT text (measured: clipchildren hides whole widgets, text is drawn
// whole), so every widget of a row is multiplied by an edge fade: a row half outside the viewport is invisible, and
// its click areas are switched off. The wheel eases toward a target (~180 ms), the thumb can be dragged, a click on
// the track pages. The host page builds and fills rows (INT_PTY_Page.ScrollBuild / ScrollFill).

class INT_PTY_MScroll : Managed
{
	static const float WHEEL_STEP = 80;
	static const float EASE_RATE = 16;
	static const float FADE_OFF = 0.55;		// a row's click areas are off below this fade
	static const float TEXT_EDGE = 6;		// design px over which a text crossing the list edge fades out

	protected INT_PTY_Page m_Host;
	protected INT_PTY_Menu m_Menu;
	int m_Id;
	string m_Key;
	Widget m_Clip;
	float m_X;
	float m_Y;
	float m_W;
	float m_H;
	float m_RH;
	int m_Count;
	float m_Off;
	float m_Target;

	protected int m_Slots;
	protected ref array<Widget> m_Roots;
	protected ref array<int> m_Index;
	protected ref array<ref array<Widget>> m_Widgets;
	protected ref array<ref array<float>> m_Homes;
	protected ref array<ref array<float>> m_Applied;	// alpha last set on each widget
	protected ref array<ref array<float>> m_TextY;		// text widgets: design-px top inside the row (-1 = not text)
	protected ref array<ref array<float>> m_TextH;
	protected ref array<ref array<Widget>> m_Hits;
	protected ref array<float> m_Fade;
	protected ref array<float> m_Top;
	protected ref array<Widget> m_HoverW;
	protected ref array<float> m_HoverA;
	protected int m_HoverSlot = -1;
	protected bool m_Dirty;

	// entrance
	protected float m_EnterT = -1;
	protected float m_EnterDelay;

	// remove animation (an answered request)
	protected int m_RemIndex = -1;
	protected float m_RemT;

	// scrollbar (left of the rows)
	protected Widget m_Bar;
	protected ImageWidget m_Track;
	protected ImageWidget m_Thumb;
	protected ImageWidget m_Grip;
	protected float m_BarX;
	protected bool m_Drag;
	protected float m_DragDy;
	protected int m_LastMx = -1;
	protected int m_LastMy = -1;

	void Init(INT_PTY_Menu menu, INT_PTY_Page host, int id, string key, Widget parent, float x, float y, float w, float h, float rh, float barX)
	{
		m_Menu = menu;
		m_Host = host;
		m_Id = id;
		m_Key = key;
		m_X = x;
		m_Y = y;
		m_W = w;
		m_H = h;
		m_RH = rh;
		m_BarX = barX;
		m_Roots = new array<Widget>();
		m_Index = new array<int>();
		m_Widgets = new array<ref array<Widget>>();
		m_Homes = new array<ref array<float>>();
		m_Applied = new array<ref array<float>>();
		m_TextY = new array<ref array<float>>();
		m_TextH = new array<ref array<float>>();
		m_Hits = new array<ref array<Widget>>();
		m_Fade = new array<float>();
		m_Top = new array<float>();
		m_HoverW = new array<Widget>();
		m_HoverA = new array<float>();
		// the wheel area under the rows: the gaps between rows still scroll
		menu.Hit(parent, x, y, w, h, key + ":area");
		m_Clip = INT_UIF_Draw.Clip(parent, x, y, w, h, "INT_PTY_ScrollClip");
		m_Slots = (int)Math.Ceil(h / rh) + 2;
		for (int i = 0; i < m_Slots; i++)
		{
			Widget root = INT_UIF_Draw.Frame(m_Clip, 0, i * rh, w, rh, "INT_PTY_ScrollRow");
			m_Roots.Insert(root);
			m_Index.Insert(-2);
			m_Widgets.Insert(new array<Widget>());
			m_Homes.Insert(new array<float>());
			m_Applied.Insert(new array<float>());
			m_TextY.Insert(new array<float>());
			m_TextH.Insert(new array<float>());
			m_Hits.Insert(new array<Widget>());
			m_Fade.Insert(-1);
			m_Top.Insert(0);
			m_HoverW.Insert(null);
			m_HoverA.Insert(0);
			host.ScrollBuild(id, i, root);
		}
		// scrollbar: a hairline track, an orange thumb with a diamond grip (Expedition route look)
		m_Bar = INT_UIF_Draw.Frame(parent, barX - 10, y, 20, h, "INT_PTY_ScrollBar");
		m_Track = INT_PTY_MD.Line(m_Bar, 9, 0, 1, h, INT_PTY_MC.L2);
		m_Thumb = INT_PTY_MD.Line(m_Bar, 8, 0, 3, 40, INT_PTY_MC.O);
		m_Grip = INT_PTY_MD.Dm(m_Bar, 9.5, 20, 9, INT_PTY_MC.O);
		ButtonWidget bh = menu.Hit(m_Bar, 0, 0, 20, h, key + ":bar");
		menu.SetSound(bh, false);
		m_Bar.Show(false);
	}

	int SlotCount()
	{
		return m_Slots;
	}

	Widget SlotRoot(int slot)
	{
		return m_Roots.Get(slot);
	}

	// Item bound to a pooled row (-1 = none).
	int SlotIndex(int slot)
	{
		if (slot < 0 || slot >= m_Slots)
			return -1;
		int idx = m_Index.Get(slot);
		if (idx < 0)
			return -1;
		return idx;
	}

	// The row's hover highlight (shown at its built alpha while the cursor is over the row).
	void SetSlotHover(int slot, Widget w)
	{
		if (!w)
			return;
		m_HoverW.Set(slot, w);
		m_HoverA.Set(slot, w.GetAlpha());
		w.SetName(INT_PTY_MenuFx.NO_FADE);
		w.SetAlpha(0);
	}

	float MaxOff()
	{
		float m = m_Count * m_RH - m_H;
		if (m < 0)
			m = 0;
		return m;
	}

	// New item count: rows are rebound; keepOffset = stay where the player scrolled.
	void SetCount(int n, bool keepOffset)
	{
		m_Count = n;
		m_RemIndex = -1;
		if (!keepOffset)
		{
			m_Off = 0;
			m_Target = 0;
		}
		m_Off = Math.Clamp(m_Off, 0, MaxOff());
		m_Target = Math.Clamp(m_Target, 0, MaxOff());
		for (int i = 0; i < m_Slots; i++)
		{
			m_Index.Set(i, -2);
		}
		Layout(true);
	}

	// Data of the bound items changed: rebind every visible row.
	void Refresh()
	{
		for (int i = 0; i < m_Slots; i++)
		{
			m_Index.Set(i, -2);
		}
		Layout(true);
	}

	// Rebind one row that the host repainted (its colours / alphas changed).
	void Recapture(int slot)
	{
		Capture(slot);
		m_Fade.Set(slot, -1);
		Layout(false);
	}

	void Wheel(int wheel)
	{
		if (MaxOff() <= 0)
			return;
		m_Target = Math.Clamp(m_Target - wheel * WHEEL_STEP, 0, MaxOff());
	}

	// Instant offset (tests, follow the selection).
	void Jump(float off)
	{
		m_Off = Math.Clamp(off, 0, MaxOff());
		m_Target = m_Off;
		Layout(false);
	}

	// Smoothly brings item `index` into view.
	void Reveal(int index)
	{
		float top = index * m_RH;
		if (top < m_Target)
			m_Target = top;
		else if (top + m_RH > m_Target + m_H)
			m_Target = top + m_RH - m_H;
		m_Target = Math.Clamp(m_Target, 0, MaxOff());
	}

	void Enter(float delay)
	{
		m_EnterT = 0;
		m_EnterDelay = delay;
		Layout(true);
	}

	// The row of `index` fades and the rows under it close the gap (0.3 s); the host rebuilds when data arrives.
	void AnimateRemove(int index)
	{
		m_RemIndex = index;
		m_RemT = 0;
	}

	bool IsAnimating()
	{
		return m_RemIndex >= 0 || m_EnterT >= 0 || Math.AbsFloat(m_Target - m_Off) > 0.5;
	}

	// Design-px y (in the host page) of the top of item `index`, current offset.
	float ItemY(int index)
	{
		return m_Y + index * m_RH - m_Off;
	}

	// 0..1 how much of item `index` is visible (edge fade).
	float ItemFade(int index)
	{
		float top = index * m_RH - m_Off;
		return EdgeFade(top);
	}

	protected float EdgeFade(float top)
	{
		float outside = 0;
		if (top < 0)
			outside = outside - top;
		float bottom = top + m_RH;
		if (bottom > m_H)
			outside = outside + bottom - m_H;
		return Math.Clamp(1 - outside / (m_RH * 0.5), 0, 1);
	}

	// Every widget of the row and its alpha as filled (fades multiply these).
	// A widget the host did not touch still carries the alpha this scroller last set on it: it keeps its old home. A
	// widget the host recoloured (or a new one) gets its current alpha as home. (Capturing raw alphas made rows that
	// were rebound mid-fade invisible for good — measured on the stand 2026-10-23.)
	// Text widgets also keep their place in the row: the clip frame cuts images but draws text whole, so a text that
	// crosses the list edge is faded out by itself (no letters outside the list area).
	protected void Capture(int slot)
	{
		array<Widget> ws = m_Widgets.Get(slot);
		array<float> hs = m_Homes.Get(slot);
		array<float> ap = m_Applied.Get(slot);
		array<float> tys = m_TextY.Get(slot);
		array<float> ths = m_TextH.Get(slot);
		array<Widget> hits = m_Hits.Get(slot);
		map<Widget, float> oldHome = new map<Widget, float>();
		map<Widget, float> oldSet = new map<Widget, float>();
		for (int i = 0; i < ws.Count(); i++)
		{
			Widget ow = ws.Get(i);
			if (!ow)
				continue;
			oldHome.Set(ow, hs.Get(i));
			if (i < ap.Count())
				oldSet.Set(ow, ap.Get(i));
		}
		ws.Clear();
		hs.Clear();
		ap.Clear();
		tys.Clear();
		ths.Clear();
		hits.Clear();
		Widget root = m_Roots.Get(slot);
		if (root)
			CaptureTree(root, ws, hs, hits);
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		for (int j = 0; j < ws.Count(); j++)
		{
			Widget w = ws.Get(j);
			float cur = hs.Get(j);
			ap.Insert(cur);
			float ty = -1;
			float th = 0;
			if (w && TextWidget.Cast(w))
			{
				ty = RelY(w, root) / f;
				float tw;
				float tsh;
				w.GetSize(tw, tsh);
				th = tsh / f;
			}
			tys.Insert(ty);
			ths.Insert(th);
			float home;
			float wasSet;
			if (!w || !oldHome.Find(w, home) || !oldSet.Find(w, wasSet))
				continue;
			if (Math.AbsFloat(cur - wasSet) < 0.006)
				hs.Set(j, home);
		}
	}

	// Screen-px y of `w` inside `root` (sum of the local positions up the parent chain).
	protected float RelY(Widget w, Widget root)
	{
		float y = 0;
		Widget p = w;
		while (p && p != root)
		{
			float px;
			float py;
			p.GetPos(px, py);
			y = y + py;
			p = p.GetParent();
		}
		return y;
	}

	// 1 inside the list, 0 once a text box is more than TEXT_EDGE px outside it.
	protected float TextFade(float rowTop, float ty, float th)
	{
		if (ty < 0)
			return 1;
		float top = rowTop + ty;
		float outside = 0;
		if (top < 0)
			outside = -top;
		float bottom = top + th;
		if (bottom > m_H)
			outside = outside + bottom - m_H;
		return Math.Clamp(1 - outside / TEXT_EDGE, 0, 1);
	}

	protected void CaptureTree(Widget w, array<Widget> ws, array<float> hs, array<Widget> hits)
	{
		Widget c = w.GetChildren();
		while (c)
		{
			if (m_Menu.ActOf(c))
			{
				hits.Insert(c);
			}
			else if (c.GetName() != INT_PTY_MenuFx.NO_FADE)
			{
				ws.Insert(c);
				hs.Insert(c.GetAlpha());
			}
			CaptureTree(c, ws, hs, hits);
			c = c.GetSibling();
		}
	}

	protected float EnterFade(int k)
	{
		if (m_EnterT < 0)
			return 1;
		float t = (m_EnterT - m_EnterDelay - k * 0.035) / 0.3;
		t = Math.Clamp(t, 0, 1);
		return 1 - (1 - t) * (1 - t) * (1 - t);
	}

	// Places every pooled row, rebinds rows that wrapped, applies edge / entrance / remove fades.
	protected void Layout(bool force)
	{
		if (!m_Clip)
			return;
		int first = (int)Math.Floor(m_Off / m_RH);
		if (first < 0)
			first = 0;
		float rem = 0;
		if (m_RemIndex >= 0)
			rem = Ease(m_RemT);
		for (int k = 0; k < m_Slots; k++)
		{
			int index = first + k;
			int slot = index % m_Slots;
			Widget root = m_Roots.Get(slot);
			if (!root)
				continue;
			if (index >= m_Count)
			{
				if (m_Index.Get(slot) != -1)
				{
					m_Index.Set(slot, -1);
					root.Show(false);
				}
				continue;
			}
			if (m_Index.Get(slot) != index)
			{
				m_Index.Set(slot, index);
				root.Show(true);
				m_Host.ScrollFill(m_Id, slot, index);
				Capture(slot);
				m_Fade.Set(slot, -1);
			}
			float top = index * m_RH - m_Off;
			if (m_RemIndex >= 0 && index > m_RemIndex)
				top = top - m_RH * rem;
			INT_UIF_Draw.Move(root, 0, top);
			float fade = EdgeFade(top);
			float ef = EnterFade(k);
			fade = fade * ef;
			if (m_RemIndex >= 0 && index == m_RemIndex)
				fade = fade * (1 - rem);
			if (m_EnterT >= 0 && ef < 1)
				INT_UIF_Draw.Move(root, -28 * (1 - ef), top);
			ApplyFade(slot, fade, top, force);
		}
		RepaintBar();
		m_Host.ScrollMoved(m_Id);
	}

	protected float Ease(float t)
	{
		float c = Math.Clamp(t, 0, 1);
		return c * c * (3 - 2 * c);
	}

	protected void ApplyFade(int slot, float fade, float top, bool force)
	{
		float old = m_Fade.Get(slot);
		float oldTop = m_Top.Get(slot);
		bool same = old >= 0 && Math.AbsFloat(old - fade) < 0.01;
		if (same && fade > 0 && Math.AbsFloat(oldTop - top) >= 0.5)
			same = false;
		if (!force && same)
			return;
		m_Fade.Set(slot, fade);
		m_Top.Set(slot, top);
		array<Widget> ws = m_Widgets.Get(slot);
		array<float> hs = m_Homes.Get(slot);
		array<float> ap = m_Applied.Get(slot);
		array<float> tys = m_TextY.Get(slot);
		array<float> ths = m_TextH.Get(slot);
		int n = ws.Count();
		for (int i = 0; i < n; i++)
		{
			Widget w = ws.Get(i);
			if (!w)
				continue;
			float a = hs.Get(i) * fade;
			if (a > 0 && i < tys.Count())
			{
				float ty = tys.Get(i);
				float th = ths.Get(i);
				a = a * TextFade(top, ty, th);
			}
			w.SetAlpha(a);
			if (i < ap.Count())
				ap.Set(i, a);
		}
		bool live = fade >= FADE_OFF;
		array<Widget> hits = m_Hits.Get(slot);
		foreach (Widget h : hits)
		{
			if (h)
				h.Show(live);
		}
		Widget hw = m_HoverW.Get(slot);
		if (hw)
		{
			float ha = 0;
			if (slot == m_HoverSlot)
				ha = m_HoverA.Get(slot) * fade;
			hw.SetAlpha(ha);
		}
	}

	protected void RepaintBar()
	{
		if (!m_Bar)
			return;
		float maxOff = MaxOff();
		bool need = maxOff > 0;
		m_Bar.Show(need);
		if (!need)
			return;
		float content = m_Count * m_RH;
		float th = m_H * m_H / content;
		if (th < 36)
			th = 36;
		float ty = (m_H - th) * m_Off / maxOff;
		INT_UIF_Draw.Place(m_Thumb, 8, ty, 3, th);
		INT_PTY_Look.DiamondPlace(m_Grip, 9.5, ty + th * 0.5, 9);
	}

	// Mouse in design px of the host page (canvas space).
	protected void MouseDesign(out float x, out float y)
	{
		int mx;
		int my;
		GetMousePos(mx, my);
		float cx = 0;
		float cy = 0;
		Widget canvas = m_Menu.GetCanvas();
		if (canvas)
			canvas.GetScreenPos(cx, cy);
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		x = (mx - cx) / f;
		y = (my - cy) / f;
	}

	// A press on the bar: on the thumb = drag, elsewhere = one page toward the press.
	void BarDown()
	{
		float mx;
		float my;
		MouseDesign(mx, my);
		float maxOff = MaxOff();
		if (maxOff <= 0)
			return;
		float content = m_Count * m_RH;
		float th = Math.Max(36, m_H * m_H / content);
		float ty = m_Y + (m_H - th) * m_Off / maxOff;
		if (my >= ty - 6 && my <= ty + th + 6)
		{
			m_Drag = true;
			m_DragDy = my - ty;
			return;
		}
		if (my < ty)
			m_Target = Math.Clamp(m_Target - m_H, 0, maxOff);
		else
			m_Target = Math.Clamp(m_Target + m_H, 0, maxOff);
	}

	// The window's drag loop: the thumb follows the mouse.
	void BarDrag(bool done)
	{
		if (!m_Drag)
			return;
		float mx;
		float my;
		MouseDesign(mx, my);
		float maxOff = MaxOff();
		float content = m_Count * m_RH;
		float th = Math.Max(36, m_H * m_H / content);
		float room = m_H - th;
		if (room > 0)
		{
			float frac = Math.Clamp((my - m_DragDy - m_Y) / room, 0, 1);
			m_Target = frac * maxOff;
			m_Off = m_Target;
			Layout(false);
		}
		if (done)
			m_Drag = false;
	}

	// Slot under the cursor (-1 = none / outside the list).
	int SlotAtMouse()
	{
		float mx;
		float my;
		MouseDesign(mx, my);
		if (mx < m_X || mx > m_X + m_W || my < m_Y || my > m_Y + m_H)
			return -1;
		float rel = my - m_Y + m_Off;
		int index = (int)Math.Floor(rel / m_RH);
		if (index < 0 || index >= m_Count)
			return -1;
		int slot = index % m_Slots;
		if (m_Index.Get(slot) != index)
			return -1;
		return slot;
	}

	// Per frame: ease the offset, entrance / remove clocks, hover follows the row under the cursor.
	void Tick(float dt)
	{
		bool moved = false;
		if (Math.AbsFloat(m_Target - m_Off) > 0.3)
		{
			float k = 1 - Math.Pow(2.718281828, -EASE_RATE * dt);
			m_Off = m_Off + (m_Target - m_Off) * k;
			moved = true;
		}
		else if (m_Off != m_Target)
		{
			m_Off = m_Target;
			moved = true;
		}
		if (m_EnterT >= 0)
		{
			m_EnterT = m_EnterT + dt;
			moved = true;
			if (m_EnterT > m_EnterDelay + m_Slots * 0.035 + 0.35)
				m_EnterT = -1;
		}
		if (m_RemIndex >= 0 && m_RemT < 1)
		{
			m_RemT = Math.Min(1, m_RemT + dt / 0.3);
			moved = true;
		}
		if (moved)
			Layout(false);
		int mx;
		int my;
		GetMousePos(mx, my);
		if (!moved && mx == m_LastMx && my == m_LastMy)
			return;
		m_LastMx = mx;
		m_LastMy = my;
		UpdateHover();
	}

	protected void UpdateHover()
	{
		int slot = SlotAtMouse();
		if (m_Menu.HasModal2())
			slot = -1;
		if (slot == m_HoverSlot)
			return;
		int old = m_HoverSlot;
		m_HoverSlot = slot;
		if (old >= 0)
		{
			Widget ow = m_HoverW.Get(old);
			if (ow)
				INT_UIF_Animator.Fade(ow, 0, 0.12, INT_UIF_Ease.OUT_CUBIC);
			m_Host.ScrollHover(m_Id, old, false);
		}
		if (slot >= 0)
		{
			Widget nw = m_HoverW.Get(slot);
			float f = m_Fade.Get(slot);
			if (f < 0)
				f = 1;
			if (nw)
				INT_UIF_Animator.Fade(nw, m_HoverA.Get(slot) * f, 0.12, INT_UIF_Ease.OUT_CUBIC);
			m_Host.ScrollHover(m_Id, slot, true);
		}
	}

	int HoverSlot()
	{
		return m_HoverSlot;
	}

	float Offset()
	{
		return m_Off;
	}
}
