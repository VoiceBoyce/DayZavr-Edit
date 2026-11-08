// INT PartyMode — click actions and entrance animation helpers of the Expedition look (ported from the
// INT_FrameworkSkills menu kit; only ALPHA / POS / SIZE / COLOR / ROTATION / VALUE tweens).

// A clickable area of the window: what it does and what lights up on hover.
class INT_PTY_Act
{
	string Key;
	int Section;
	TextWidget HoverText;
	int TextNormal;
	int TextHover;
	ImageWidget HoverImg;
	int ImgNormal;
	int ImgHover;
	bool Sound = true;
}

class INT_PTY_Fx
{
	// Widget.SetAlpha REPLACES the alpha of the widget's colour: a 18 % wash faded "in" to 1.0 turns opaque.
	// Every entrance therefore fades back to the alpha the widget was built with.
	static float HomeAlpha(Widget w)
	{
		float a = w.GetAlpha();
		if (a < 0.01)
			a = 1;
		return a;
	}

	// Fade in while sliding from +dx (design px) back to the built position.
	static void InX(Widget w, float dx, float delay, float dur, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return;
		Widget twin = INT_PTY_Look.Twin(w);
		if (twin)
			InX(twin, dx, delay, dur, ease);
		float home = HomeAlpha(w);
		float x;
		float y;
		w.GetPos(x, y);
		w.SetPos(x + INT_UIF_Draw.S(dx), y);
		w.SetAlpha(0);
		INT_UIF_Tween m = INT_UIF_Animator.MoveX(w, x, dur, ease);
		if (m)
			m.Delay(delay);
		INT_UIF_Tween f = INT_UIF_Animator.Fade(w, home, dur, ease);
		if (f)
			f.From(0).Delay(delay);
	}

	static void InY(Widget w, float dy, float delay, float dur, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return;
		Widget twin = INT_PTY_Look.Twin(w);
		if (twin)
			InY(twin, dy, delay, dur, ease);
		float home = HomeAlpha(w);
		float x;
		float y;
		w.GetPos(x, y);
		w.SetPos(x, y + INT_UIF_Draw.S(dy));
		w.SetAlpha(0);
		INT_UIF_Tween m = INT_UIF_Animator.MoveY(w, y, dur, ease);
		if (m)
			m.Delay(delay);
		INT_UIF_Tween f = INT_UIF_Animator.Fade(w, home, dur, ease);
		if (f)
			f.From(0).Delay(delay);
	}

	static void InAlpha(Widget w, float delay, float dur, int ease = INT_UIF_Ease.OUT_CUBIC)
	{
		if (!w)
			return;
		Widget twin = INT_PTY_Look.Twin(w);
		if (twin)
			InAlpha(twin, delay, dur, ease);
		float home = HomeAlpha(w);
		w.SetAlpha(0);
		INT_UIF_Tween f = INT_UIF_Animator.Fade(w, home, dur, ease);
		if (f)
			f.From(0).Delay(delay);
	}

	// Width grows from 0 (left edge fixed).
	static void GrowW(Widget w, float delay, float dur, int ease = INT_UIF_Ease.OUT_EXPO)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(0, sh);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeW(w, sw, dur, ease);
		if (t)
			t.Delay(delay);
	}

	// Height grows from 0 (top edge fixed).
	static void GrowH(Widget w, float delay, float dur, int ease = INT_UIF_Ease.OUT_EXPO)
	{
		if (!w)
			return;
		float sw;
		float sh;
		w.GetSize(sw, sh);
		w.SetSize(sw, 0);
		INT_UIF_Tween t = INT_UIF_Animator.ResizeH(w, sh, dur, ease);
		if (t)
			t.Delay(delay);
	}

	// Width grows from the centre outwards (POS_X and SIZE_W together).
	static void GrowFromCenter(Widget w, float delay, float dur, int ease = INT_UIF_Ease.OUT_EXPO)
	{
		if (!w)
			return;
		float x;
		float y;
		float sw;
		float sh;
		w.GetPos(x, y);
		w.GetSize(sw, sh);
		w.SetPos(x + sw * 0.5, y);
		w.SetSize(0, sh);
		INT_UIF_Tween m = INT_UIF_Animator.MoveX(w, x, dur, ease);
		if (m)
			m.Delay(delay);
		INT_UIF_Tween r = INT_UIF_Animator.ResizeW(w, sw, dur, ease);
		if (r)
			r.Delay(delay);
	}

	// A diamond spins from `fromDeg` into its rest angle 0 with an overshoot (ROTATION + ALPHA); -45 starts
	// it as a square standing on its side.
	static void SpinIn(Widget w, float fromDeg, float delay, float dur)
	{
		if (!w)
			return;
		float home = HomeAlpha(w);
		w.SetRotation(0, 0, fromDeg);
		w.SetAlpha(0);
		INT_UIF_Tween r = INT_UIF_Animator.Rotate(w, 0, dur, INT_UIF_Ease.OUT_BACK);
		if (r)
			r.Delay(delay);
		INT_UIF_Tween f = INT_UIF_Animator.Fade(w, home, dur * 0.6, INT_UIF_Ease.OUT_CUBIC);
		if (f)
			f.From(0).Delay(delay);
	}

	// Size from (from) to the built size around the same centre + fade (lock-on frames, bursts).
	static void Scale(Widget w, float fromFactor, float toAlpha, float fromAlpha, float delay, float dur, int ease)
	{
		if (!w)
			return;
		float x;
		float y;
		float sw;
		float sh;
		w.GetPos(x, y);
		w.GetSize(sw, sh);
		float fw = sw * fromFactor;
		float fh = sh * fromFactor;
		w.SetPos(x - (fw - sw) * 0.5, y - (fh - sh) * 0.5);
		w.SetSize(fw, fh);
		w.SetAlpha(fromAlpha);
		INT_UIF_Tween a = INT_UIF_Animator.MoveX(w, x, dur, ease);
		if (a)
			a.Delay(delay);
		INT_UIF_Tween b = INT_UIF_Animator.MoveY(w, y, dur, ease);
		if (b)
			b.Delay(delay);
		INT_UIF_Tween c = INT_UIF_Animator.ResizeW(w, sw, dur, ease);
		if (c)
			c.Delay(delay);
		INT_UIF_Tween d = INT_UIF_Animator.ResizeH(w, sh, dur, ease);
		if (d)
			d.Delay(delay);
		INT_UIF_Tween e = INT_UIF_Animator.Fade(w, toAlpha, dur, ease);
		if (e)
			e.From(fromAlpha).Delay(delay);
	}

	// Grows from the built size to `toFactor` while fading out, then hides (bursts).
	static void Burst(Widget w, float toFactor, float fromAlpha, float delay, float dur)
	{
		if (!w)
			return;
		float x;
		float y;
		float sw;
		float sh;
		w.GetPos(x, y);
		w.GetSize(sw, sh);
		float tw = sw * toFactor;
		float th = sh * toFactor;
		// Hidden until its delay ends (the fade starts at fromAlpha then).
		if (delay > 0)
			w.SetAlpha(0);
		else
			w.SetAlpha(fromAlpha);
		INT_UIF_Tween a = INT_UIF_Animator.MoveX(w, x - (tw - sw) * 0.5, dur, INT_UIF_Ease.OUT_EXPO);
		if (a)
			a.Delay(delay);
		INT_UIF_Tween b = INT_UIF_Animator.MoveY(w, y - (th - sh) * 0.5, dur, INT_UIF_Ease.OUT_EXPO);
		if (b)
			b.Delay(delay);
		INT_UIF_Tween c = INT_UIF_Animator.ResizeW(w, tw, dur, INT_UIF_Ease.OUT_EXPO);
		if (c)
			c.Delay(delay);
		INT_UIF_Tween d = INT_UIF_Animator.ResizeH(w, th, dur, INT_UIF_Ease.OUT_EXPO);
		if (d)
			d.Delay(delay);
		INT_UIF_Tween e = INT_UIF_Animator.Fade(w, 0, dur, INT_UIF_Ease.OUT_EXPO);
		if (e)
			e.From(fromAlpha).Delay(delay).HideOnEnd();
	}

	// Horizontal shake: -6, +6, -4, +4, 0 design px in 260 ms (A5).
	static void Shake(Widget w)
	{
		if (!w)
			return;
		INT_PTY_Shaker.Start(w, 0.26);
	}
}

// One VALUE tween drives the shake; the shaker keeps itself alive until the tween completes.
class INT_PTY_Shaker : Managed
{
	protected static ref array<ref INT_PTY_Shaker> s_Running;

	protected Widget m_W;
	protected float m_HomeX;
	protected float m_HomeY;

	static void Start(Widget w, float dur)
	{
		INT_UIF_Animator.Get().Cancel(w, INT_UIF_TweenProp.POS_X);
		if (!s_Running)
			s_Running = new array<ref INT_PTY_Shaker>();
		INT_PTY_Shaker sh = new INT_PTY_Shaker();
		sh.m_W = w;
		float hx;
		float hy;
		w.GetPos(hx, hy);
		sh.m_HomeX = hx;
		sh.m_HomeY = hy;
		s_Running.Insert(sh);
		INT_UIF_Tween t = INT_UIF_Animator.Value(0, 1, dur, INT_UIF_Ease.LINEAR);
		if (!t)
			return;
		t.OnUpdate().Insert(sh.OnStep);
		t.OnComplete().Insert(sh.OnDone);
	}

	void OnStep(float v, INT_UIF_Tween t)
	{
		if (!m_W)
			return;
		float dx = 0;
		if (v < 0.2)
			dx = -6 * (v / 0.2);
		else if (v < 0.4)
			dx = -6 + 12 * ((v - 0.2) / 0.2);
		else if (v < 0.6)
			dx = 6 - 10 * ((v - 0.4) / 0.2);
		else if (v < 0.8)
			dx = -4 + 8 * ((v - 0.6) / 0.2);
		else
			dx = 4 - 4 * ((v - 0.8) / 0.2);
		m_W.SetPos(m_HomeX + dx * INT_UIF_Draw.S(1), m_HomeY);
	}

	void OnDone(INT_UIF_Tween t)
	{
		if (m_W)
			m_W.SetPos(m_HomeX, m_HomeY);
		int idx = s_Running.Find(this);
		if (idx >= 0)
			s_Running.Remove(idx);
	}
}
