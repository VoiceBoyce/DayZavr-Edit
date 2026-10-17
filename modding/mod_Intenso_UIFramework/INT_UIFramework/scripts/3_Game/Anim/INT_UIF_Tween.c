// ============================================================================
// INT_UIFramework - tween + animator.
// One animator ticks every tween from GetUpdateQueue(CALL_CATEGORY_GUI), which
// runs every frame on the client, menus open or not (tools.c:9,
// dayzgame.c:3019). The tick allocates nothing (enforce rule 8): finished
// tweens are removed in place, tweens added during the tick wait in a pending
// list that is drained after it.
// ============================================================================

enum INT_UIF_TweenProp
{
	ALPHA,
	POS_X,
	POS_Y,
	SIZE_W,
	SIZE_H,
	COLOR,
	ROTATION,
	VALUE,     // no widget: the eased value goes to OnUpdate(float value, tween)
	MASK       // ImageWidget mask progress 0..1 (ring / radial fills)
}

class INT_UIF_Tween extends Managed
{
	// Widget is Managed: this plain (weak) reference turns null if the widget
	// is destroyed mid-tween, which Step() checks every frame.
	Widget m_Widget;
	int m_Prop;
	float m_From;
	float m_To;
	int m_ColorFrom;
	int m_ColorTo;
	float m_Duration;
	float m_Delay;
	float m_Elapsed;
	int m_Ease;
	bool m_FromCurrent;
	bool m_Started;
	bool m_Cancelled;
	bool m_HideOnEnd;
	int m_Loops;        // 0 = play once, -1 = forever, n = n more runs
	bool m_Yoyo;        // loops run back and forth
	protected ref ScriptInvoker m_OnComplete;
	protected ref ScriptInvoker m_OnUpdate;

	void INT_UIF_Tween(Widget w, int prop, float to, float duration, int ease)
	{
		m_Widget = w;
		m_Prop = prop;
		m_To = to;
		m_Duration = duration;
		m_Ease = ease;
		m_FromCurrent = true;
	}

	// --- fluent configuration ----------------------------------------------
	INT_UIF_Tween From(float from)
	{
		m_From = from;
		m_FromCurrent = false;
		return this;
	}

	INT_UIF_Tween FromColor(int color)
	{
		m_ColorFrom = color;
		m_FromCurrent = false;
		return this;
	}

	INT_UIF_Tween Delay(float seconds)
	{
		m_Delay = seconds;
		return this;
	}

	INT_UIF_Tween HideOnEnd()
	{
		m_HideOnEnd = true;
		return this;
	}

	// Repeat: count -1 = forever. yoyo plays every other run backwards.
	INT_UIF_Tween Loop(int count = -1, bool yoyo = true)
	{
		m_Loops = count;
		m_Yoyo = yoyo;
		return this;
	}

	// Fires once with (INT_UIF_Tween tween) when the tween finishes normally.
	ScriptInvoker OnComplete()
	{
		if (!m_OnComplete)
			m_OnComplete = new ScriptInvoker();
		return m_OnComplete;
	}

	// Fires every frame with (float value, INT_UIF_Tween tween); VALUE tweens.
	ScriptInvoker OnUpdate()
	{
		if (!m_OnUpdate)
			m_OnUpdate = new ScriptInvoker();
		return m_OnUpdate;
	}

	void Cancel()
	{
		m_Cancelled = true;
	}

	bool Targets(Widget w, int prop)
	{
		if (m_Widget != w)
			return false;
		return prop < 0 || prop == m_Prop;
	}

	// --- runtime -------------------------------------------------------------
	protected void CaptureFrom()
	{
		if (!m_FromCurrent || !m_Widget)
			return;
		float x;
		float y;
		if (m_Prop == INT_UIF_TweenProp.ALPHA)
		{
			m_From = m_Widget.GetAlpha();
		}
		else if (m_Prop == INT_UIF_TweenProp.POS_X || m_Prop == INT_UIF_TweenProp.POS_Y)
		{
			m_Widget.GetPos(x, y);
			m_From = y;
			if (m_Prop == INT_UIF_TweenProp.POS_X)
				m_From = x;
		}
		else if (m_Prop == INT_UIF_TweenProp.SIZE_W || m_Prop == INT_UIF_TweenProp.SIZE_H)
		{
			m_Widget.GetSize(x, y);
			m_From = y;
			if (m_Prop == INT_UIF_TweenProp.SIZE_W)
				m_From = x;
		}
		else if (m_Prop == INT_UIF_TweenProp.COLOR)
		{
			m_ColorFrom = m_Widget.GetColor();
		}
		else if (m_Prop == INT_UIF_TweenProp.ROTATION)
		{
			// In-plane widget rotation is the third angle (vanilla
			// mapmenu.c:256, carhud.c:108: SetRotation(0, 0, angle)).
			vector rot = m_Widget.GetRotation();
			m_From = rot[2];
		}
		else if (m_Prop == INT_UIF_TweenProp.MASK)
		{
			ImageWidget mimg = ImageWidget.Cast(m_Widget);
			if (mimg)
				m_From = mimg.GetMaskProgress();
		}
	}

	protected void ApplyValue(float k)
	{
		float v = m_From + (m_To - m_From) * k;
		if (m_Prop == INT_UIF_TweenProp.VALUE)
		{
			if (m_OnUpdate)
				m_OnUpdate.Invoke(v, this);
			return;
		}
		if (!m_Widget)
			return;

		float x;
		float y;
		if (m_Prop == INT_UIF_TweenProp.ALPHA)
		{
			m_Widget.SetAlpha(v);
		}
		else if (m_Prop == INT_UIF_TweenProp.POS_X)
		{
			m_Widget.GetPos(x, y);
			m_Widget.SetPos(v, y);
		}
		else if (m_Prop == INT_UIF_TweenProp.POS_Y)
		{
			m_Widget.GetPos(x, y);
			m_Widget.SetPos(x, v);
		}
		else if (m_Prop == INT_UIF_TweenProp.SIZE_W)
		{
			m_Widget.GetSize(x, y);
			m_Widget.SetSize(v, y);
		}
		else if (m_Prop == INT_UIF_TweenProp.SIZE_H)
		{
			m_Widget.GetSize(x, y);
			m_Widget.SetSize(x, v);
		}
		else if (m_Prop == INT_UIF_TweenProp.COLOR)
		{
			m_Widget.SetColor(INT_UIF_Color.Lerp(m_ColorFrom, m_ColorTo, k));
		}
		else if (m_Prop == INT_UIF_TweenProp.ROTATION)
		{
			m_Widget.SetRotation(0, 0, v);
		}
		else if (m_Prop == INT_UIF_TweenProp.MASK)
		{
			ImageWidget img = ImageWidget.Cast(m_Widget);
			if (img)
				img.SetMaskProgress(v);
		}
		if (m_OnUpdate)
			m_OnUpdate.Invoke(v, this);
	}

	// Advances the tween; returns true when it is finished (or dead).
	bool Step(float dt)
	{
		if (m_Cancelled)
			return true;
		// A widget tween whose widget died has nothing left to do.
		if (m_Prop != INT_UIF_TweenProp.VALUE && !m_Widget)
			return true;

		if (m_Delay > 0)
		{
			m_Delay -= dt;
			if (m_Delay > 0)
				return false;
			dt = -m_Delay;
		}
		if (!m_Started)
		{
			m_Started = true;
			CaptureFrom();
		}

		m_Elapsed += dt;
		float t = 1.0;
		if (m_Duration > 0)
			t = m_Elapsed / m_Duration;
		if (t > 1.0)
			t = 1.0;

		ApplyValue(INT_UIF_Easing.Apply(m_Ease, t));
		if (t < 1.0)
			return false;

		if (m_Loops != 0)
		{
			if (m_Loops > 0)
				m_Loops--;
			m_Elapsed = 0;
			if (m_Yoyo)
			{
				float swapF = m_From;
				m_From = m_To;
				m_To = swapF;
				int swapC = m_ColorFrom;
				m_ColorFrom = m_ColorTo;
				m_ColorTo = swapC;
			}
			return false;
		}

		if (m_HideOnEnd && m_Widget)
			m_Widget.Show(false);
		if (m_OnComplete)
			m_OnComplete.Invoke(this);
		return true;
	}
}

class INT_UIF_Animator
{
	protected static ref INT_UIF_Animator s_Instance;

	protected ref array<ref INT_UIF_Tween> m_Tweens;
	protected ref array<ref INT_UIF_Tween> m_Pending;
	// Components that asked for a per-frame OnTick (weak: a destroyed
	// component turns null and is dropped).
	protected ref array<INT_UIF_Component> m_Tickers;
	protected bool m_Ticking;
	protected bool m_Hooked;

	void INT_UIF_Animator()
	{
		m_Tweens = new array<ref INT_UIF_Tween>();
		m_Pending = new array<ref INT_UIF_Tween>();
		m_Tickers = new array<INT_UIF_Component>();
	}

	static INT_UIF_Animator Get()
	{
		if (!s_Instance)
			s_Instance = new INT_UIF_Animator();
		return s_Instance;
	}

	static INT_UIF_Animator Peek()
	{
		return s_Instance;
	}

	protected void Hook()
	{
		if (m_Hooked || !GetGame())
			return;
		ScriptInvoker queue = GetGame().GetUpdateQueue(CALL_CATEGORY_GUI);
		if (!queue)
			return;
		queue.Insert(Tick);
		m_Hooked = true;
	}

	void Shutdown()
	{
		if (m_Hooked && GetGame())
		{
			ScriptInvoker queue = GetGame().GetUpdateQueue(CALL_CATEGORY_GUI);
			if (queue)
				queue.Remove(Tick);
		}
		m_Hooked = false;
		m_Tweens.Clear();
		m_Pending.Clear();
		m_Tickers.Clear();
	}

	INT_UIF_Tween Add(INT_UIF_Tween tween)
	{
		if (!tween)
			return null;
		// A new tween on the same widget + property replaces the running one.
		if (tween.m_Widget)
			Cancel(tween.m_Widget, tween.m_Prop);
		if (m_Ticking)
			m_Pending.Insert(tween);
		else
			m_Tweens.Insert(tween);
		Hook();
		return tween;
	}

	// prop < 0 cancels every tween of the widget.
	void Cancel(Widget w, int prop = -1)
	{
		if (!w)
			return;
		foreach (INT_UIF_Tween t : m_Tweens)
		{
			if (t && t.Targets(w, prop))
				t.Cancel();
		}
		foreach (INT_UIF_Tween p : m_Pending)
		{
			if (p && p.Targets(w, prop))
				p.Cancel();
		}
	}

	bool IsAnimating(Widget w)
	{
		foreach (INT_UIF_Tween t : m_Tweens)
		{
			if (t && !t.m_Cancelled && t.m_Widget == w)
				return true;
		}
		return false;
	}

	void AddTicker(INT_UIF_Component c)
	{
		if (!c || m_Tickers.Find(c) >= 0)
			return;
		m_Tickers.Insert(c);
		Hook();
	}

	void RemoveTicker(INT_UIF_Component c)
	{
		int idx = m_Tickers.Find(c);
		if (idx < 0)
			return;
		// Never shrink the array while it is iterated: null the slot instead.
		if (m_Ticking)
			m_Tickers.Set(idx, null);
		else
			m_Tickers.Remove(idx);
	}

	void Tick(float dt)
	{
		m_Ticking = true;

		for (int i = m_Tweens.Count() - 1; i >= 0; i--)
		{
			INT_UIF_Tween tw = m_Tweens.Get(i);
			if (!tw || tw.Step(dt))
				m_Tweens.Remove(i);
		}

		for (int k = m_Tickers.Count() - 1; k >= 0; k--)
		{
			INT_UIF_Component c = m_Tickers.Get(k);
			if (!c)
			{
				m_Tickers.Remove(k);
				continue;
			}
			c.OnTick(dt);
		}

		INT_UIF_ToastManager tm = INT_UIF_ToastManager.Peek();
		if (tm)
			tm.OnTick(dt);

		m_Ticking = false;

		if (m_Pending.Count() > 0)
		{
			foreach (INT_UIF_Tween p : m_Pending)
			{
				m_Tweens.Insert(p);
			}
			m_Pending.Clear();
		}
	}

	// --- convenience API -----------------------------------------------------
	static INT_UIF_Tween Fade(Widget w, float to, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.ALPHA, to, duration, ease));
	}

	static INT_UIF_Tween FadeIn(Widget w, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return null;
		w.SetAlpha(0);
		w.Show(true);
		return Fade(w, 1.0, duration, ease).From(0);
	}

	// Fade in to a partial alpha. Widget.SetAlpha REPLACES the alpha of the
	// widget's colour, so a 66 % overlay faded "in" to 1.0 turns opaque: fade
	// tinted overlays to their own colour alpha instead (FadeInColor).
	static INT_UIF_Tween FadeInTo(Widget w, float alpha, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return null;
		w.SetAlpha(0);
		w.Show(true);
		return Fade(w, alpha, duration, ease).From(0);
	}

	// Fade in to the alpha the widget's current colour carries.
	static INT_UIF_Tween FadeInColor(Widget w, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return null;
		float a = INT_UIF_Color.A(w.GetColor()) / 255.0;
		return FadeInTo(w, a, duration, ease);
	}

	static INT_UIF_Tween FadeOut(Widget w, float duration, bool hideOnEnd = true, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		INT_UIF_Tween t = Fade(w, 0.0, duration, ease);
		if (t && hideOnEnd)
			t.HideOnEnd();
		return t;
	}

	static INT_UIF_Tween MoveX(Widget w, float x, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.POS_X, x, duration, ease));
	}

	static INT_UIF_Tween MoveY(Widget w, float y, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.POS_Y, y, duration, ease));
	}

	static INT_UIF_Tween ResizeW(Widget w, float width, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.SIZE_W, width, duration, ease));
	}

	static INT_UIF_Tween ResizeH(Widget w, float height, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.SIZE_H, height, duration, ease));
	}

	static INT_UIF_Tween ColorTo(Widget w, int color, float duration, int ease = INT_UIF_Ease.OUT_QUAD)
	{
		INT_UIF_Tween t = new INT_UIF_Tween(w, INT_UIF_TweenProp.COLOR, 0, duration, ease);
		t.m_ColorTo = color;
		return Get().Add(t);
	}

	static INT_UIF_Tween Rotate(Widget w, float degrees, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.ROTATION, degrees, duration, ease));
	}

	// Widget-less tween: subscribe to OnUpdate(float value, INT_UIF_Tween t).
	static INT_UIF_Tween Value(float from, float to, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		INT_UIF_Tween t = new INT_UIF_Tween(null, INT_UIF_TweenProp.VALUE, to, duration, ease);
		t.From(from);
		return Get().Add(t);
	}

	// Timer built on a VALUE tween: OnComplete fires after `seconds`.
	static INT_UIF_Tween Wait(float seconds)
	{
		return Value(0, 1, seconds, INT_UIF_Ease.LINEAR);
	}

	// Mask progress of an ImageWidget (ring arcs, radial fills).
	static INT_UIF_Tween Mask(ImageWidget w, float to, float duration, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		return Get().Add(new INT_UIF_Tween(w, INT_UIF_TweenProp.MASK, to, duration, ease));
	}

	// Endless breathing between two alphas (live dots, glows).
	static INT_UIF_Tween Pulse(Widget w, float lowAlpha, float highAlpha, float period)
	{
		if (!w)
			return null;
		w.SetAlpha(highAlpha);
		INT_UIF_Tween t = Fade(w, lowAlpha, period * 0.5, INT_UIF_Ease.IN_OUT_SINE);
		if (t)
		{
			t.From(highAlpha);
			t.Loop(-1, true);
		}
		return t;
	}

	// Entrance: fade in while rising dy px (1080p) into place, after `delay`
	// seconds. Staggering the delays makes lists and panels cascade in.
	static void Reveal(Widget w, float delay, float dy = 10, float duration = 0.34)
	{
		if (!w)
			return;
		float x;
		float y;
		w.GetPos(x, y);
		w.SetAlpha(0);
		w.Show(true);
		w.SetPos(x, y + INT_UIF_Scale.Px(dy));
		INT_UIF_Tween f = Fade(w, 1.0, duration, INT_UIF_Ease.OUT_CUBIC);
		if (f)
		{
			f.From(0);
			f.Delay(delay);
		}
		INT_UIF_Tween m = MoveY(w, y, duration + 0.06, INT_UIF_Ease.OUT_CUBIC);
		if (m)
			m.Delay(delay);
	}
}
