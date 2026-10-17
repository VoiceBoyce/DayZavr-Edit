// ============================================================================
// INT_UIFramework - toasts (v2): non-blocking notifications stacked top-right.
//   INT_UIF_Toast.Show("Saved", INT_UIF_ToastKind.SUCCESS);
// At most MAX_VISIBLE on screen, the rest wait in a queue. Each toast springs
// in from the right, shows a thin time-left bar (paused while hovered) and
// slides out; click dismisses early. Toasts are top-level widgets sorted above
// menus and take no input focus, so they also work with no menu open.
// ============================================================================

class INT_UIF_ToastData extends Managed
{
	string m_Text;
	int m_Kind;
	float m_Seconds;

	void INT_UIF_ToastData(string text, int kind, float seconds)
	{
		m_Text = text;
		m_Kind = kind;
		m_Seconds = seconds;
	}
}

class INT_UIF_ToastItem extends INT_UIF_Component
{
	protected string m_Message;
	protected int m_Kind;
	protected ButtonWidget m_Hit;
	protected ImageWidget m_Timer;
	float m_Total;
	float m_Remaining;
	bool m_Leaving;
	bool m_Gone;
	bool m_Hovered;

	void Configure(string text, int kind, float seconds)
	{
		m_Message = text;
		m_Kind = kind;
		m_Remaining = seconds;
		m_Total = seconds;
		if (m_Total <= 0)
			m_Total = 1;
	}

	override string GetLayoutName()
	{
		return "toast.layout";
	}

	override string GetRootName()
	{
		return "IufToast";
	}

	override void OnBuild()
	{
		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.ToastColor(m_Kind);
		float w = m_W;
		float h = m_H;
		m_Root.SetSort(INT_UIF_SORT_TOAST);

		INT_UIF_Surface.Shadow(m_Root, -30, -22, w + 60, h + 58, tm.Color(INT_UIF_ThemeKeys.SHADOW));
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, tm.Color(INT_UIF_ThemeKeys.TILE), ARGB(26, 255, 255, 255));
		INT_UIF_Draw.Image(m_Root, -26, (h - 100) * 0.5, 100, 100, "iuf_radial", INT_UIF_Color.WithAlpha(hue, 46));
		INT_UIF_Draw.Image(m_Root, 20, 0, w - 40, 1, "iuf_grad_band", ARGB(40, 255, 255, 255));
		INT_UIF_Draw.IconTile(m_Root, 16, (h - 36) * 0.5, 36, INT_UIF_Dialog.KindIcon(m_Kind), hue, 10);

		string msg = m_Message;
		if (m_Message.IndexOf("#") == 0)
			msg = Widget.TranslateString(m_Message);
		float tw = w - 64 - 18;
		if (INT_UIF_Draw.TextWidth(msg, 13) <= tw)
			INT_UIF_Draw.Text(m_Root, 64, 0, tw, h, msg, 13, tm.Color(INT_UIF_ThemeKeys.TEXT));
		else
			INT_UIF_Draw.MText(m_Root, 64, 13, tw, h - 22, msg, 13, tm.Color(INT_UIF_ThemeKeys.TEXT));

		INT_UIF_Draw.Line(m_Root, 14, h - 4, w - 28, 2, ARGB(16, 255, 255, 255));
		m_Timer = INT_UIF_Draw.Line(m_Root, 14, h - 4, w - 28, 2, INT_UIF_Color.WithAlpha(hue, 200));
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
	}

	// Called every frame by the manager while the toast is alive.
	void UpdateTimer()
	{
		if (!m_Timer)
			return;
		float k = Math.Clamp(m_Remaining / m_Total, 0, 1);
		INT_UIF_Draw.Resize(m_Timer, (m_W - 28) * k, 2);
	}

	void SlideIn(float x, float y)
	{
		m_Root.SetPos(INT_UIF_Scale.ScreenW() + INT_UIF_Scale.Px(24), y);
		INT_UIF_Animator.FadeIn(m_Root, 0.22);
		INT_UIF_Animator.MoveX(m_Root, x, 0.42, INT_UIF_Ease.OUT_BACK);
	}

	void MoveTo(float y)
	{
		INT_UIF_Animator.MoveY(m_Root, y, 0.3, INT_UIF_Ease.OUT_CUBIC);
	}

	void Leave()
	{
		if (m_Leaving)
			return;
		m_Leaving = true;
		INT_UIF_Animator.Fade(m_Root, 0, 0.22, INT_UIF_Ease.IN_QUAD);
		INT_UIF_Tween t = INT_UIF_Animator.MoveX(m_Root, INT_UIF_Scale.ScreenW() + INT_UIF_Scale.Px(24), 0.28, INT_UIF_Ease.IN_BACK);
		if (t)
			t.OnComplete().Insert(OnLeaveDone);
		else
			m_Gone = true;
	}

	protected void OnLeaveDone(INT_UIF_Tween t)
	{
		m_Gone = true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit)
			return false;
		m_Remaining = 0;
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hovered = true;
		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w != m_Hit)
			return false;
		m_Hovered = false;
		return true;
	}
}

class INT_UIF_ToastManager
{
	static const int MAX_VISIBLE = 4;

	protected static ref INT_UIF_ToastManager s_Instance;
	protected ref array<ref INT_UIF_ToastItem> m_Active;
	protected ref array<ref INT_UIF_ToastData> m_Queue;

	void INT_UIF_ToastManager()
	{
		m_Active = new array<ref INT_UIF_ToastItem>();
		m_Queue = new array<ref INT_UIF_ToastData>();
	}

	static INT_UIF_ToastManager Get()
	{
		if (!s_Instance)
			s_Instance = new INT_UIF_ToastManager();
		return s_Instance;
	}

	static INT_UIF_ToastManager Peek()
	{
		return s_Instance;
	}

	void Push(string text, int kind, float seconds)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		INT_UIF_ToastData data = new INT_UIF_ToastData(text, kind, seconds);
		if (m_Active.Count() < MAX_VISIBLE)
			Spawn(data);
		else
			m_Queue.Insert(data);
	}

	int VisibleCount()
	{
		return m_Active.Count();
	}

	int QueuedCount()
	{
		return m_Queue.Count();
	}

	protected float ToastW()
	{
		return INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.TOAST_WIDTH, 384);
	}

	protected float ToastH()
	{
		return INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.TOAST_HEIGHT, 76);
	}

	protected float SlotY(int index)
	{
		return INT_UIF_Scale.Px(24) + index * (INT_UIF_Scale.Px(ToastH()) + INT_UIF_Scale.Px(12));
	}

	protected float SlotX()
	{
		return INT_UIF_Scale.ScreenW() - INT_UIF_Scale.Px(ToastW()) - INT_UIF_Scale.Px(24);
	}

	protected void Spawn(INT_UIF_ToastData data)
	{
		INT_UIF_ToastItem item = new INT_UIF_ToastItem();
		item.Configure(data.m_Text, data.m_Kind, data.m_Seconds);
		if (!item.BuildAt(null, 0, 0, ToastW(), ToastH()))
			return;
		m_Active.Insert(item);
		item.SlideIn(SlotX(), SlotY(m_Active.Count() - 1));
	}

	// Ticked by INT_UIF_Animator (no allocations here).
	void OnTick(float dt)
	{
		bool removed = false;
		for (int i = m_Active.Count() - 1; i >= 0; i--)
		{
			INT_UIF_ToastItem item = m_Active.Get(i);
			if (!item || item.m_Gone || !item.GetRoot())
			{
				if (item)
					item.Destroy();
				m_Active.RemoveOrdered(i);
				removed = true;
				continue;
			}
			if (item.m_Leaving)
				continue;
			if (!item.m_Hovered)
				item.m_Remaining -= dt;
			item.UpdateTimer();
			if (item.m_Remaining <= 0)
				item.Leave();
		}
		if (!removed)
			return;

		// Close the gap, then pull waiting toasts in.
		int n = m_Active.Count();
		for (int k = 0; k < n; k++)
		{
			m_Active.Get(k).MoveTo(SlotY(k));
		}
		while (m_Active.Count() < MAX_VISIBLE && m_Queue.Count() > 0)
		{
			INT_UIF_ToastData next = m_Queue.Get(0);
			m_Queue.RemoveOrdered(0);
			Spawn(next);
		}
	}

	void Clear()
	{
		foreach (INT_UIF_ToastItem item : m_Active)
		{
			if (item)
				item.Destroy();
		}
		m_Active.Clear();
		m_Queue.Clear();
	}
}

// Public entry point.
class INT_UIF_Toast
{
	static void Show(string text, int kind = INT_UIF_ToastKind.INFO, float seconds = 4.5)
	{
		INT_UIF_ToastManager.Get().Push(text, kind, seconds);
	}

	static void Info(string text)
	{
		Show(text, INT_UIF_ToastKind.INFO);
	}

	static void Success(string text)
	{
		Show(text, INT_UIF_ToastKind.SUCCESS);
	}

	static void Warning(string text)
	{
		Show(text, INT_UIF_ToastKind.WARNING, 6.0);
	}

	static void Error(string text)
	{
		Show(text, INT_UIF_ToastKind.ERROR, 6.5);
	}
}
