// INT PartyMode (client) — notification cards of the HUD (concept C .htoast, «ДРУГ В ИГРЕ / ВИТЯ ВОШЁЛ В ИГРУ»):
// 420 × 76 near-black card with a hairline border, a diamond-framed icon, the title in tracked orange capitals, the
// text in white capitals (two lines when long), an orange line at the bottom that shrinks with the time left. Slides in
// from the right (80 px, 0.4 s), up to 3 cards stacked top right, 4.5 s each, the rest wait; the stack closes up when
// a card leaves. Red look (skull) for «Друг погиб» and errors, amber for warnings. Timers stop while the HUD is hidden.

class INT_PTY_HudCard : Managed
{
	static const float W = 420;
	static const float H = 76;
	static const float TEXT_X = 76;
	static const float ROOM = 322;

	Widget m_Root;
	ImageWidget m_Bg;
	ref array<ImageWidget> m_Edges;
	ImageWidget m_Track;
	ImageWidget m_Timer;
	ImageWidget m_Dia;
	ImageWidget m_Icon;
	ref INT_PTY_HudText m_Title;
	ref INT_PTY_HudText m_Text1;
	ref INT_PTY_HudText m_Text2;
	ref INT_PTY_HudText m_TextBig;

	float m_K = 1;
	bool m_Used;
	bool m_Sample;
	float m_Total = 4.5;
	float m_Left;
	float m_Dying = -1;
	float m_Age;
	float m_Y;
	bool m_Placed;
	int m_Accent;
	int m_TitleRgb;
	string m_IconName = "<none>";

	void Build(Widget layer, float k)
	{
		m_K = k;
		m_Edges = new array<ImageWidget>();
		m_Root = INT_PTY_HudKit.Frame(layer, "INT_PTY_Card");
		INT_PTY_HudKit.Rect(m_Root, 0, 0, W, H, k);
		m_Bg = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFF0C0C0C);
		INT_PTY_HudKit.Rect(m_Bg, 0, 0, W, H, k);
		for (int i = 0; i < 4; i++)
		{
			ImageWidget e = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFFFFFF);
			m_Edges.Insert(e);
		}
		INT_PTY_HudKit.Rect(m_Edges.Get(0), 0, 0, W, 1, k);
		INT_PTY_HudKit.Rect(m_Edges.Get(1), 0, 0, 1, H, k);
		INT_PTY_HudKit.Rect(m_Edges.Get(2), W - 1, 0, 1, H, k);
		INT_PTY_HudKit.Rect(m_Edges.Get(3), 0, H - 2, W, 2, k);
		m_Track = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Rect(m_Track, 0, H - 2, W, 2, k);
		m_Timer = INT_PTY_HudKit.Img(m_Root, "pty_px_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Rect(m_Timer, 0, H - 2, W, 2, k);
		m_Dia = INT_PTY_HudKit.Img(m_Root, "pty_diamond_line_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Square(m_Dia, 40, H * 0.5, 34, k);
		m_Icon = INT_PTY_HudKit.Img(m_Root, "pty_icon_user_ca", 0xFFFF7A2F);
		INT_PTY_HudKit.Square(m_Icon, 40, H * 0.5, 15, k);
		m_Title = new INT_PTY_HudText();
		m_Title.Build(m_Root, 13, INT_UIF_Align.LEFT, true, k, 3.6);
		m_Title.Rect(TEXT_X, 13, ROOM + 10, 18, k);
		m_TextBig = new INT_PTY_HudText();
		m_TextBig.Build(m_Root, 18, INT_UIF_Align.LEFT, true, k, 1.1);
		m_TextBig.Rect(TEXT_X, 33, ROOM + 10, 26, k);
		m_Text1 = new INT_PTY_HudText();
		m_Text1.Build(m_Root, 15, INT_UIF_Align.LEFT, true, k, 0.6);
		m_Text1.Rect(TEXT_X, 31, ROOM + 10, 19, k);
		m_Text2 = new INT_PTY_HudText();
		m_Text2.Build(m_Root, 15, INT_UIF_Align.LEFT, true, k, 0.6);
		m_Text2.Rect(TEXT_X, 50, ROOM + 10, 19, k);
		if (m_Root)
			m_Root.Show(false);
	}

	void Destroy()
	{
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}

	// Fill the card (texts measured once here; INT_PTY_Look calibrations of the capitals).
	void Fill(int accent, int titleRgb, string iconName, string title, string text, float seconds)
	{
		m_Used = true;
		m_Accent = accent;
		m_TitleRgb = titleRgb;
		m_Total = seconds;
		m_Left = seconds;
		m_Dying = -1;
		m_Age = 0;
		m_Placed = false;
		if (iconName != m_IconName)
		{
			m_IconName = iconName;
			if (m_Icon)
				m_Icon.LoadImageFile(0, INT_PTY_TEX + "pty_icon_" + iconName + "_ca.paa");
		}
		m_Title.SetText(INT_PTY_Text.Upper(title));
		string up = INT_PTY_Text.Upper(text);
		float big = INT_PTY_Look.TextW(up, 18, true) + up.LengthUtf8() * 1.1;
		if (big <= ROOM)
		{
			m_TextBig.SetText(up);
			m_TextBig.Show(true);
			m_Text1.Show(false);
			m_Text2.Show(false);
		}
		else
		{
			string rest = "";
			string line1 = INT_PTY_Look.FirstLine(up, 15, true, ROOM - 6, rest);
			string line2 = INT_PTY_Look.FitText(rest, 15, true, ROOM - 6);
			m_Text1.SetText(line1);
			m_Text2.SetText(line2);
			m_TextBig.Show(false);
			m_Text1.Show(true);
			m_Text2.Show(rest != "");
		}
		if (m_Root)
			m_Root.Show(true);
	}

	void Release()
	{
		m_Used = false;
		m_Sample = false;
		if (m_Root)
			m_Root.Show(false);
	}

	// x0 / y0 = top-left of the stack (screen px); y = this card's offset in design px; alpha = part opacity.
	void Draw(float x0, float y0, float alpha)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		float appear = INT_PTY_HudKit.OutCubic(INT_PTY_HudKit.Seg(m_Age, 0, 0.4));
		float dx = 80 * (1 - appear);
		float a = alpha * Math.Clamp(m_Age / 0.25, 0, 1);
		if (m_Dying >= 0)
		{
			float d = INT_PTY_HudKit.In(INT_PTY_HudKit.Seg(0.22 - m_Dying, 0, 0.22));
			dx = dx + 24 * d;
			a = a * (1 - d);
		}
		if (m_Root)
			m_Root.SetPos(x0 + dx * f, y0 + m_Y * f);
		INT_PTY_HudKit.Paint(m_Bg, 0xFF0C0C0C, 0.97 * a);
		for (int i = 0; i < 3; i++)
		{
			INT_PTY_HudKit.Paint(m_Edges.Get(i), 0xFFFFFFFF, 0.08 * a);
		}
		INT_PTY_HudKit.Paint(m_Edges.Get(3), 0xFF000000, 0.4 * a);
		INT_PTY_HudKit.Paint(m_Track, m_Accent, 0.25 * a);
		float left = 1;
		if (m_Total > 0 && !m_Sample)
			left = Math.Clamp(m_Left / m_Total, 0, 1);
		INT_PTY_HudKit.Rect(m_Timer, 0, H - 2, W * left, 2, m_K);
		INT_PTY_HudKit.Paint(m_Timer, m_Accent, a);
		INT_PTY_HudKit.Paint(m_Dia, m_Accent, a);
		INT_PTY_HudKit.Paint(m_Icon, m_Accent, a);
		m_Title.Paint(m_TitleRgb, a);
		m_TextBig.Paint(0xFFFFFFFF, a);
		m_Text1.Paint(0xFFFFFFFF, a);
		m_Text2.Paint(0xFFFFFFFF, a);
	}
}

class INT_PTY_HudToastReq : Managed
{
	int m_Kind;
	string m_Title;
	string m_Text;
}

class INT_PTY_HudToasts : Managed
{
	static const int MAX_SHOWN = 3;
	static const int MAX_QUEUE = 20;
	static const float SECONDS = 4.5;
	static const float STEP = 88;			// design px between cards
	static const float DEFAULT_RIGHT = 60;	// design px from the right edge
	static const float DEFAULT_TOP = 110;

	protected Widget m_Layer;
	protected ref array<ref INT_PTY_HudCard> m_Cards;		// the pool (MAX_SHOWN + 1)
	protected ref array<INT_PTY_HudCard> m_Order;			// shown cards, top first
	protected ref array<ref INT_PTY_HudToastReq> m_Queue;
	protected float m_K = 1;
	protected string m_TitleDead;
	protected string m_TitleOnline;
	protected string m_TitleInvite;
	protected string m_TitleAdded;
	protected string m_TitleRemoved;
	protected string m_TitleRemovedBy;
	protected string m_TitleSent;
	static bool s_Hold;			// self-test: cards stay (timers frozen) for screenshots

	void INT_PTY_HudToasts()
	{
		m_Cards = new array<ref INT_PTY_HudCard>();
		m_Order = new array<INT_PTY_HudCard>();
		m_Queue = new array<ref INT_PTY_HudToastReq>();
	}

	void Build(Widget layer, float k)
	{
		foreach (INT_PTY_HudCard c : m_Cards)
		{
			c.Destroy();
		}
		m_Cards.Clear();
		m_Order.Clear();
		m_Layer = layer;
		m_K = k;
		for (int i = 0; i <= MAX_SHOWN; i++)
		{
			INT_PTY_HudCard n = new INT_PTY_HudCard();
			n.Build(layer, k);
			m_Cards.Insert(n);
		}
		string trDead = Widget.TranslateString("#STR_INT_PTY_T_DEAD_H");
		m_TitleDead = trDead;
		string trOnline = Widget.TranslateString("#STR_INT_PTY_T_ONLINE_H");
		m_TitleOnline = trOnline;
		string trInvite = Widget.TranslateString("#STR_INT_PTY_T_INVITE_IN_H");
		m_TitleInvite = trInvite;
		string trAdded = Widget.TranslateString("#STR_INT_PTY_T_FRIEND_ADDED_H");
		m_TitleAdded = trAdded;
		string trRemoved = Widget.TranslateString("#STR_INT_PTY_T_FRIEND_REMOVED_H");
		m_TitleRemoved = trRemoved;
		string trRemovedBy = Widget.TranslateString("#STR_INT_PTY_T_FRIEND_REMOVED_BY_H");
		m_TitleRemovedBy = trRemovedBy;
		string trSent = Widget.TranslateString("#STR_INT_PTY_T_SENT_H");
		m_TitleSent = trSent;
	}

	void Push(int kind, string title, string text)
	{
		INT_PTY_HudToastReq r = new INT_PTY_HudToastReq();
		r.m_Kind = kind;
		r.m_Title = title;
		r.m_Text = text;
		m_Queue.Insert(r);
		while (m_Queue.Count() > MAX_QUEUE)
		{
			m_Queue.RemoveOrdered(0);
		}
	}

	int ActiveCount()
	{
		return m_Order.Count();
	}

	int QueueCount()
	{
		return m_Queue.Count();
	}

	void Clear()
	{
		m_Queue.Clear();
		foreach (INT_PTY_HudCard c : m_Cards)
		{
			c.Release();
		}
		m_Order.Clear();
	}

	protected INT_PTY_HudCard FreeCard()
	{
		foreach (INT_PTY_HudCard c : m_Cards)
		{
			if (!c.m_Used)
				return c;
		}
		return null;
	}

	// Icon and colours of a card: by the title (Notify passes the translated title) and the kind.
	protected void Look(int kind, string title, int accent, out int rgb, out int titleRgb, out string icon)
	{
		rgb = accent;
		titleRgb = accent;
		icon = "bell";
		if (title == m_TitleDead)
		{
			rgb = 0xFFFF4B3E;
			titleRgb = 0xFFFF6D61;
			icon = "skull";
			return;
		}
		if (title == m_TitleOnline)
			icon = "user";
		else if (title == m_TitleInvite)
			icon = "inbox";
		else if (title == m_TitleAdded)
			icon = "user_plus";
		else if (title == m_TitleRemoved || title == m_TitleRemovedBy)
			icon = "user_minus";
		else if (title == m_TitleSent)
			icon = "send";
		if (kind == INT_UIF_ToastKind.WARNING)
		{
			rgb = 0xFFFFC246;
			titleRgb = 0xFFFFC246;
			if (icon == "bell")
				icon = "warning";
		}
		else if (kind == INT_UIF_ToastKind.ERROR)
		{
			rgb = 0xFFFF4B3E;
			titleRgb = 0xFFFF6D61;
			icon = "warning";
		}
	}

	// visible = the HUD layer is drawn this frame (timers run only then); edit = API edit mode (sample card).
	void Update(INT_PTY_HudCtx ctx, bool visible)
	{
		float dt = ctx.m_Dt;
		bool partOn = INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TOASTS);
		if (ctx.m_Edit)
		{
			EnsureSample();
		}
		else
		{
			DropSample();
		}
		if (!visible)
			return;
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_TOASTS);
		for (int i = m_Order.Count() - 1; i >= 0; i--)
		{
			INT_PTY_HudCard c = m_Order.Get(i);
			c.m_Age = c.m_Age + dt;
			if (c.m_Dying >= 0)
			{
				c.m_Dying = c.m_Dying - dt;
				if (c.m_Dying < 0)
				{
					c.Release();
					m_Order.RemoveOrdered(i);
				}
				continue;
			}
			if (c.m_Sample || s_Hold)
				continue;
			c.m_Left = c.m_Left - dt;
			if (c.m_Left <= 0)
				c.m_Dying = 0.22;
		}
		if (!partOn && !ctx.m_Edit)
		{
			// hidden by an editor: the cards are skipped, not stored up
			Clear();
			return;
		}
		while (m_Queue.Count() > 0 && LiveCount() < MAX_SHOWN)
		{
			INT_PTY_HudCard free = FreeCard();
			if (!free)
				break;
			INT_PTY_HudToastReq r = m_Queue.Get(0);
			int rgb;
			int titleRgb;
			string icon;
			Look(r.m_Kind, r.m_Title, accent, rgb, titleRgb, icon);
			free.Fill(rgb, titleRgb, icon, r.m_Title, r.m_Text, SECONDS);
			m_Queue.RemoveOrdered(0);
			m_Order.Insert(free);
		}
		Draw(ctx);
	}

	protected int LiveCount()
	{
		int n = 0;
		foreach (INT_PTY_HudCard c : m_Order)
		{
			if (c.m_Dying < 0)
				n++;
		}
		return n;
	}

	protected void Draw(INT_PTY_HudCtx ctx)
	{
		float x0;
		float y0;
		GetTopLeft(ctx.m_Sw, ctx.m_Sh, x0, y0);
		float alpha = INT_PTY_UI.PartOpacity(INT_PTY_UI.PART_TOASTS);
		if (!INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TOASTS))
			alpha = alpha * 0.35;
		float step = Math.Clamp(ctx.m_Dt * 12.0, 0, 1);
		int slot = 0;
		foreach (INT_PTY_HudCard c : m_Order)
		{
			float target = slot * STEP;
			if (!c.m_Placed)
			{
				c.m_Y = target;
				c.m_Placed = true;
			}
			else
			{
				c.m_Y = c.m_Y + (target - c.m_Y) * step;
			}
			c.Draw(x0, y0, alpha);
			if (c.m_Dying < 0)
				slot++;
		}
	}

	// Top-left of the stack in screen px (editor place or top right).
	void GetTopLeft(float sw, float sh, out float x, out float y)
	{
		float f = INT_PTY_HudKit.s_F * m_K;
		float nx;
		float ny;
		if (INT_PTY_UI.GetPartPosition(INT_PTY_UI.PART_TOASTS, nx, ny))
		{
			x = nx * sw;
			y = ny * sh;
		}
		else
		{
			x = sw - (DEFAULT_RIGHT * INT_PTY_HudKit.s_F) - INT_PTY_HudCard.W * f;
			y = DEFAULT_TOP * INT_PTY_HudKit.s_F;
		}
		x = Math.Clamp(x, 0, Math.Max(0, sw - INT_PTY_HudCard.W * f));
		y = Math.Clamp(y, 0, Math.Max(0, sh - INT_PTY_HudCard.H * f));
	}

	bool GetRect(float sw, float sh, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (m_Order.Count() == 0)
			return false;
		float f = INT_PTY_HudKit.s_F * m_K;
		GetTopLeft(sw, sh, x, y);
		w = INT_PTY_HudCard.W * f;
		h = (INT_PTY_HudCard.H + (m_Order.Count() - 1) * STEP) * f;
		return true;
	}

	protected void EnsureSample()
	{
		foreach (INT_PTY_HudCard c : m_Order)
		{
			if (c.m_Sample)
				return;
		}
		INT_PTY_HudCard free = FreeCard();
		if (!free)
			return;
		string title = Widget.TranslateString("#STR_INT_PTY_T_ONLINE_H");
		string text = INT_PTY_Notify.Tr("STR_INT_PTY_T_ONLINE", "Витя");
		int accent = INT_PTY_UI.PartAccent(INT_PTY_UI.PART_TOASTS);
		free.Fill(accent, accent, "user", title, text, SECONDS);
		free.m_Sample = true;
		m_Order.InsertAt(free, 0);
	}

	protected void DropSample()
	{
		for (int i = m_Order.Count() - 1; i >= 0; i--)
		{
			INT_PTY_HudCard c = m_Order.Get(i);
			if (!c.m_Sample)
				continue;
			c.Release();
			m_Order.RemoveOrdered(i);
		}
	}

	void Preview()
	{
		string title = Widget.TranslateString("#STR_INT_PTY_T_ONLINE_H");
		string text = INT_PTY_Notify.Tr("STR_INT_PTY_T_ONLINE", "Витя");
		Push(INT_UIF_ToastKind.SUCCESS, title, text);
	}
}
