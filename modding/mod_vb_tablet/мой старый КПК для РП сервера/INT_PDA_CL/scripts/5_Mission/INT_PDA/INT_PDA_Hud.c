// INT_PDA (client) — the PDA HUD layer (concept design/concepts_v1/5_e_zone2_hud.png, style E «ЗОНА 2.0»):
//   feed    — the last HudFeedLines lines of the PUBLIC chat while the PDA listens and the player keeps the feed on
//             (profile switch «Лента КПК на экране», else the server's HudFeedDefault); avatar / anonymous plate,
//             callsign in the relation colour, text in one or two rows, fades out after 12 s; top left, grows down
//   toast   — OP_PM_NOTICE «Вам пришло сообщение от X!» (+ all unread private messages), sound Pm, 7 s
//   faction — a mate's line in the faction chat while our PDA is closed: «X: text» (+ unread faction lines), sound Msg
//             (at most every 2 s), 7 s (owner 2026-10-23)
//   news    — OP_NEWS_FLASH banner at the top centre: title + first line of the text, sound News, 8 s
//   sos     — OP_SOS_IN red pulsing alert «SOS · X» + distance, sound Sos, 10 s
//   invite  — OP_INVITE_IN «Приглашение в <группировка> — откройте КПК», sound Invite, 12 s
//   battery — icon + «КПК N%» while the working PDA is below LowBatteryPercent; «КПК разряжен» once when it dies
// toast / faction / sos / invite stack in the top-right column (below INT_UIF toasts when those are on screen).
//
// One top-level frame (sort 0: at 100/200 the 3D previews are not drawn), no focus, ignores the mouse. Every part has
// a fixed root frame (INT_PDA_UI.FindPart) whose content is rebuilt on events — never inside an RPC handler: the
// client invokers only store data, Tick builds. A frame's alpha does not reach its children, so every widget is
// faded on its own (INT_PDA_HudFade keeps each widget's built alpha). Hidden while any menu is open (our PDA too:
// Suppress), the vanilla HUD is hidden or switched off (HUD_HIDE / HUD_DISABLE), the player is dead or unconscious
// (also the UNCONSCIOUS context), or another mod hides it (INT_PDA_UI.SetVisible). A part the player hid makes no
// sound either. Lifetimes count only while the layer is visible — except while our PDA is open (it shows the same data).
// With nothing to show Tick returns after a few checks; the battery is polled once per second (FindPda).
// Public API: INT_PDA_UI (3_Game). Test hook: INT_PDA_Hud.TestCommand(cmd), commands in TestCommand below.

// Widgets of one fade group with the alpha each one was built with.
class INT_PDA_HudFade
{
	ref array<Widget> m_W = new array<Widget>();
	ref array<float> m_A = new array<float>();
	float m_K = -1;

	void Reset()
	{
		m_W.Clear();
		m_A.Clear();
		m_K = -1;
	}

	// Every widget under `parent` (not the parent itself).
	void AddChildren(Widget parent)
	{
		if (!parent)
			return;
		Widget c = parent.GetChildren();
		while (c)
		{
			m_W.Insert(c);
			m_A.Insert(c.GetAlpha());
			AddChildren(c);
			c = c.GetSibling();
		}
	}

	void Apply(float k)
	{
		if (m_K >= 0 && Math.AbsFloat(k - m_K) < 0.004)
			return;
		m_K = k;
		int n = m_W.Count();
		Widget w;
		for (int i = 0; i < n; i++)
		{
			w = m_W.Get(i);
			if (w)
				w.SetAlpha(m_A.Get(i) * k);
		}
	}

	void Force(float k)
	{
		m_K = -1;
		Apply(k);
	}
}

// One line of the feed.
class INT_PDA_HudLine
{
	static const int KIND_PLAYER = 0;
	static const int KIND_ANON = 1;
	static const int KIND_SYSTEM = 2;
	static const int KIND_FACTION = 3;

	int Kind;
	string Name;
	int NameColor;
	int Avatar;
	int Emblem;
	string Body;
	float Age;
	// layout of the last build (design px at the part's scale)
	string NameShown;
	float NameW;
	string Row1;
	string Row2;
	float H;
	ref INT_PDA_HudFade Fade = new INT_PDA_HudFade();
}

// One part: fixed root frame + rebuilt content + state of its alert.
class INT_PDA_HudBox
{
	string m_Name;
	bool m_Right;				// default place: the top-right column
	Widget m_Root;
	Widget m_Body;
	float m_W;					// design px at scale 1 of the current content
	float m_H;
	float m_K = 1;				// scale
	float m_Op = 1;				// opacity
	int m_Accent;
	bool m_Active;				// has real content
	bool m_Dirty;
	bool m_Shown;
	float m_Life;				// seconds (0 = no limit)
	float m_Age;
	float m_CurY;				// screen px (right column)
	float m_TargetY;
	bool m_HasY;
	string m_A;					// alert data
	string m_B;
	int m_N;
	ref INT_PDA_HudFade m_Fade;
	ref INT_PDA_HudFade m_Pulse;

	void INT_PDA_HudBox(string name, bool right)
	{
		m_Name = name;
		m_Right = right;
		m_Fade = new INT_PDA_HudFade();
		m_Pulse = new INT_PDA_HudFade();
	}
}

class INT_PDA_HudLayer : INT_PDA_HudApi
{
	// geometry (design px of the 1920x1080 canvas at scale 1; concept 5_e_zone2_hud)
	static const float MARGIN = 40;
	static const float FEED_W = 460;
	static const float FEED_TOP = 40;			// top-left corner, grows down (concept 5_e_zone2_hud)
	static const float FEED_HEAD = 34;
	static const float FEED_ROW = 22;
	static const float FEED_GAP = 4;
	static const float FEED_PAD = 10;
	static const float FEED_TEXT = 16;
	static const float FEED_LIFE = 12;
	static const float FEED_OUT = 1.2;
	static const float TOAST_W = 440;
	static const float NEWS_W = 800;
	static const float NEWS_TOP = 34;
	static const float SOS_W = 400;
	static const float BAT_X = 98;				// bottom-left, right above the vanilla item-in-hands panel (concept)
	static const float BAT_TOP = 862;			// on the 1080 canvas; placed at 1080 - BAT_TOP from the bottom edge
	static const float COLUMN_GAP = 12;
	static const float IN_TIME = 0.25;
	static const float OUT_TIME = 0.6;
	static const float SLIDE = 30;
	static const float TOAST_LIFE = 7;
	static const float NEWS_LIFE = 8;
	static const float SOS_LIFE = 10;
	static const float INVITE_LIFE = 12;
	static const float DEAD_LIFE = 6;
	static const float FACTION_SOUND_GAP = 2;
	static const float EDIT_HIDDEN_ALPHA = 0.35;
	static const int BAT_LOW = 0;
	static const int BAT_DEAD = 1;

	static int s_Builds;			// self-test: part builds
	static int s_Taken;				// self-test: feed lines taken

	protected Widget m_Root;
	protected TextWidget m_Probe;
	protected int m_ScreenW;
	protected int m_ScreenH;
	protected bool m_Hidden = true;
	protected bool m_Edit;
	protected float m_Time;
	protected float m_RightOff;
	protected ref array<ref INT_PDA_HudBox> m_Boxes;
	protected INT_PDA_HudBox m_Feed;
	protected INT_PDA_HudBox m_Toast;
	protected INT_PDA_HudBox m_News;
	protected INT_PDA_HudBox m_Sos;
	protected INT_PDA_HudBox m_Bat;
	protected INT_PDA_HudBox m_Invite;
	protected INT_PDA_HudBox m_Faction;
	protected float m_FactionSoundT = -100;
	protected ref array<ref INT_PDA_HudLine> m_Lines;
	protected ref array<ref INT_PDA_HudLine> m_Demo;
	protected bool m_FeedForced;
	// battery watch (once per second)
	protected float m_BatAcc;
	protected INT_PDA m_BatPda;
	protected bool m_BatDeadSaid;
	protected bool m_BatLowSaid;
	protected int m_BatLastPct = 100;
	protected int m_BatTest = -1;

	void INT_PDA_HudLayer()
	{
		m_Boxes = new array<ref INT_PDA_HudBox>();
		m_Lines = new array<ref INT_PDA_HudLine>();
		m_Feed = AddBox(INT_PDA_UI.PART_FEED, false);
		m_Toast = AddBox(INT_PDA_UI.PART_TOAST, true);
		m_Faction = AddBox(INT_PDA_UI.PART_FACTION, true);
		m_News = AddBox(INT_PDA_UI.PART_NEWS, false);
		m_Sos = AddBox(INT_PDA_UI.PART_SOS, true);
		m_Bat = AddBox(INT_PDA_UI.PART_BATTERY, false);
		m_Invite = AddBox(INT_PDA_UI.PART_INVITE, true);
	}

	protected INT_PDA_HudBox AddBox(string name, bool right)
	{
		INT_PDA_HudBox b = new INT_PDA_HudBox(name, right);
		m_Boxes.Insert(b);
		return b;
	}

	protected INT_PDA_HudBox BoxOf(string part)
	{
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (b.m_Name == part)
				return b;
		}
		return null;
	}

	void Destroy()
	{
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Probe = null;
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			b.m_Root = null;
			b.m_Body = null;
			b.m_Shown = false;
		}
	}

	// --- intake (may run inside the RPC handler: data only, Tick builds) ---------------------------------------------
	void PushLine(INT_PDA_ChatLine src, bool forced)
	{
		if (!src)
			return;
		INT_PDA_HudLine h = new INT_PDA_HudLine();
		int flags = src.Flags;
		string me = INT_PDA_Client.s_KnownCallsign;
		h.Body = INT_PDA_Util.CleanLine(src.Text, 400, 1000);
		if (src.Channel == INT_PDA_Const.CH_SYSTEM)
		{
			h.Kind = INT_PDA_HudLine.KIND_SYSTEM;
			h.Name = Widget.TranslateString("#STR_INT_PDA_HUD_SYSTEM");
			h.NameColor = INT_PDA_Look.ORANGE;
		}
		else if ((flags & INT_PDA_Const.FL_ANON) != 0)
		{
			h.Kind = INT_PDA_HudLine.KIND_ANON;
			h.Name = Widget.TranslateString("#STR_INT_PDA_HUD_ANON");
			if ((flags & INT_PDA_Const.FL_ADMIN_VIEW) != 0 && src.Author != "")
				h.Name = h.Name + " · " + src.Author;
			h.NameColor = INT_PDA_Look.DIM;
		}
		else if ((flags & INT_PDA_Const.FL_AS_FACTION) != 0 && src.FactionName != "")
		{
			h.Kind = INT_PDA_HudLine.KIND_FACTION;
			h.Name = src.FactionName;
			h.Emblem = src.FactionEmblem;
			h.NameColor = INT_PDA_Look.RelationColor(src.FactionId);
		}
		else
		{
			h.Kind = INT_PDA_HudLine.KIND_PLAYER;
			h.Name = src.Author;
			h.Avatar = src.Avatar;
			if (me != "" && src.Author == me)
				h.NameColor = INT_PDA_Look.OWN;
			else
				h.NameColor = INT_PDA_Look.RelationColor(src.FactionId);
		}
		int most = FeedMax();
		while (m_Lines.Count() >= most)
			m_Lines.RemoveOrdered(0);
		m_Lines.Insert(h);
		if (forced)
			m_FeedForced = true;
		m_Feed.m_Dirty = true;
		s_Taken++;
	}

	// «Непрочитанных» = every unread private message of the client (the lines arrive before the notice, so the
	// counter already has them); the notice's own count only when the counter knows less (tests, a dialog read at once).
	void PushPm(string from, int count)
	{
		INT_PDA_HudBox b = m_Toast;
		int n = INT_PDA_Client.UnreadPrivate();
		if (n < count)
			n = count;
		if (n < 1)
			n = 1;
		StartAlert(b, TOAST_LIFE);
		b.m_A = from;
		b.m_N = n;
		PlayFor(b, "Pm");
	}

	// A mate's line in the faction chat: who (as the chat shows the author) + the text; the count is every unread line
	// of the faction channel (the client bumps it before the line reaches the HUD).
	void PushFaction(INT_PDA_ChatLine src)
	{
		if (!src)
			return;
		string who = src.Author;
		int flags = src.Flags;
		if ((flags & INT_PDA_Const.FL_ANON) != 0)
		{
			who = Widget.TranslateString("#STR_INT_PDA_HUD_ANON");
		}
		else if ((flags & INT_PDA_Const.FL_AS_FACTION) != 0 && src.FactionName != "")
		{
			who = src.FactionName;
		}
		int n = INT_PDA_Client.Unread("faction");
		if (n < 1)
			n = 1;
		INT_PDA_HudBox b = m_Faction;
		StartAlert(b, TOAST_LIFE);
		b.m_A = who;
		b.m_B = INT_PDA_Util.CleanLine(src.Text, 400, 1000);
		b.m_N = n;
		// a busy channel beeps once per gap, not on every line
		if (m_Time - m_FactionSoundT >= FACTION_SOUND_GAP)
		{
			m_FactionSoundT = m_Time;
			PlayFor(b, "Msg");
		}
	}

	// A part the player (or an editor) hid makes no sound either.
	protected void PlayFor(INT_PDA_HudBox b, string sound)
	{
		if (INT_PDA_UI.IsPartShown(b.m_Name))
			INT_PDA_Sfx.Play(sound);
	}

	void PushNews(string title, string text)
	{
		string first = text;
		int nl = first.IndexOf("\n");
		if (nl >= 0)
			first = first.Substring(0, nl);
		string t = INT_PDA_Util.CleanLine(title, 200, 1000);
		string f = INT_PDA_Util.CleanLine(first, 300, 1000);
		if (t == "")
		{
			t = f;
			f = "";
		}
		INT_PDA_HudBox b = m_News;
		StartAlert(b, NEWS_LIFE);
		b.m_A = t;
		b.m_B = f;
		PlayFor(b, "News");
	}

	void PushSos(string who, string dist)
	{
		INT_PDA_HudBox b = m_Sos;
		StartAlert(b, SOS_LIFE);
		b.m_A = who;
		b.m_B = dist;
		PlayFor(b, "Sos");
	}

	void PushInvite(string factionName, string from)
	{
		INT_PDA_HudBox b = m_Invite;
		StartAlert(b, INVITE_LIFE);
		b.m_A = factionName;
		b.m_B = from;
		PlayFor(b, "Invite");
	}

	protected void StartAlert(INT_PDA_HudBox b, float life)
	{
		if (b.m_Active && b.m_Shown)
			b.m_Age = IN_TIME + 0.1;		// already on screen: new text without a second fade-in
		else
			b.m_Age = 0;
		b.m_Life = life;
		b.m_Active = true;
		b.m_Dirty = true;
	}

	void ClearAll()
	{
		ClearFeed();
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			b.m_Active = false;
			b.m_Dirty = true;
		}
		m_BatLowSaid = false;
	}

	protected void ClearFeed()
	{
		m_Lines.Clear();
		m_FeedForced = false;
		m_Feed.m_Dirty = true;
	}

	protected int FeedMax()
	{
		int n = 3;
		if (INT_PDA_Client.s_Settings)
			n = INT_PDA_Client.s_Settings.HudFeedLines;
		return INT_PDA_Util.ClampI(n, 1, 6);
	}

	protected bool FeedAllowed()
	{
		if (m_FeedForced)
			return true;
		if (!INT_PDA_UI.IsPartShown(INT_PDA_UI.PART_FEED))
			return false;
		if (!INT_PDA_Client.HudFeedOn())
			return false;
		return INT_PDA_Client.IsListening();
	}

	// --- battery ----------------------------------------------------------------------------------------------------
	// Once per second: the working PDA's charge; «КПК разряжен» once when the PDA we watched stops working while still
	// switched on and still in our inventory (battery empty or pulled) — switching it off by hand says nothing.
	protected void CheckBattery()
	{
		if (m_BatTest >= 0 || !GetGame())
			return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			m_BatPda = null;
			SetBatLow(false, 0);
			return;
		}
		INT_PDA pda = INT_PDA_Client.FindPda(true);
		if (pda)
		{
			m_BatPda = pda;
			m_BatDeadSaid = false;
			float lowAt = 10;
			if (INT_PDA_Client.s_Settings)
				lowAt = INT_PDA_Client.s_Settings.LowBatteryPercent;
			float level = pda.BatteryLevel();
			int pct = (int)Math.Round(level * 100);
			m_BatLastPct = pct;
			bool isLow = pct < lowAt;
			SetBatLow(isLow, pct);
			return;
		}
		SetBatLow(false, 0);
		if (!m_BatPda)
			return;
		if (m_BatPda.GetHierarchyRootPlayer() != player)
		{
			m_BatPda = null;
			return;
		}
		if (m_BatDeadSaid || m_BatPda.IsRuined() || m_BatPda.IsWorkingPda())
			return;
		// still switched on but not working = no power (the PDA has no autoSwitchOff); an almost empty battery counts too
		ComponentEnergyManager em = m_BatPda.GetCompEM();
		if (!em)
			return;
		if (!em.IsSwitchedOn() && m_BatLastPct > 1)
			return;
		m_BatDeadSaid = true;
		ShowBatDead();
	}

	protected void SetBatLow(bool isLow, int pct)
	{
		INT_PDA_HudBox b = m_Bat;
		if (b.m_Active && b.m_N == BAT_DEAD)
			return;			// the «dead» message runs its time
		if (!isLow)
		{
			if (b.m_Active)
			{
				b.m_Active = false;
				b.m_Dirty = true;
			}
			m_BatLowSaid = false;
			return;
		}
		string txt = pct.ToString();
		if (!b.m_Active || b.m_A != txt)
		{
			if (!b.m_Active)
				b.m_Age = 0;
			b.m_Active = true;
			b.m_N = BAT_LOW;
			b.m_A = txt;
			b.m_Life = 0;
			b.m_Dirty = true;
		}
		if (!m_BatLowSaid)
		{
			m_BatLowSaid = true;
			PlayFor(b, "LowBat");
		}
	}

	protected void ShowBatDead()
	{
		INT_PDA_HudBox b = m_Bat;
		b.m_Active = false;
		StartAlert(b, DEAD_LIFE);
		b.m_N = BAT_DEAD;
		b.m_A = "";
		m_BatLowSaid = false;
		PlayFor(b, "LowBat");
	}

	// Test: pct > 0 = low tag with that charge, 0 = «КПК разряжен», < 0 = back to the real battery.
	void TestBattery(int pct)
	{
		m_BatTest = pct;
		if (pct < 0)
		{
			m_Bat.m_Active = false;
			m_Bat.m_Dirty = true;
			m_BatLowSaid = false;
			m_BatAcc = 1;
			return;
		}
		if (pct == 0)
		{
			ShowBatDead();
			return;
		}
		m_Bat.m_Active = false;
		SetBatLow(true, pct);
	}

	void PokeBattery()
	{
		m_BatAcc = 1;
	}

	// --- frame ------------------------------------------------------------------------------------------------------
	void Tick(float dt)
	{
		m_Time = m_Time + dt;
		m_BatAcc = m_BatAcc + dt;
		if (m_BatAcc >= 1.0)
		{
			m_BatAcc = 0;
			CheckBattery();
		}
		if (m_Lines.Count() > 0 && !FeedAllowed())
			ClearFeed();
		if (!HasWork())
		{
			if (m_Root && !m_Hidden)
			{
				m_Hidden = true;
				m_Root.Show(false);
				HideParts();
			}
			return;
		}
		EnsureRoot();
		if (!m_Root)
			return;
		bool hide = ShouldHide();
		if (hide != m_Hidden)
		{
			m_Hidden = hide;
			m_Root.Show(!hide);
		}
		if (!m_Edit && !INT_PDA_Hud.s_TestHold)
		{
			if (!hide || INT_PDA_Hud.s_Suppressed)
				AgeAll(dt);
		}
		else if (!m_Edit && !hide)
		{
			HoldAge(dt);
		}
		if (hide)
			return;
		BuildDirty();
		Animate(dt);
	}

	protected bool HasWork()
	{
		if (m_Edit)
			return true;
		if (m_Lines.Count() > 0)
			return true;
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (b.m_Active || b.m_Shown)
				return true;
		}
		return false;
	}

	protected void HideParts()
	{
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (!b.m_Shown)
				continue;
			b.m_Shown = false;
			if (b.m_Root)
				b.m_Root.Show(false);
			INT_PDA_UI.OnPartHidden().Invoke(b.m_Name);
		}
	}

	protected bool ShouldHide()
	{
		if (m_Edit)
			return false;
		if (!INT_PDA_UI.IsVisible() || INT_PDA_Hud.s_Suppressed)
			return true;
		if (!GetGame())
			return true;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive() || p.IsUnconscious())
			return true;
		UIManager um = GetGame().GetUIManager();
		if (um && um.GetMenu())
			return true;
		Mission mission = GetGame().GetMission();
		if (!mission)
			return false;
		IngameHud ingame = IngameHud.Cast(mission.GetHud());
		if (!ingame)
			return false;
		IngameHudVisibility vis = ingame.GetHudVisibility();
		if (!vis)
			return false;
		// the vanilla HUD hidden by its key, switched off in the options, or the unconscious context
		if (vis.IsContextFlagActive(EHudContextFlags.HUD_HIDE))
			return true;
		if (vis.IsContextFlagActive(EHudContextFlags.HUD_DISABLE))
			return true;
		if (vis.IsContextFlagActive(EHudContextFlags.UNCONSCIOUS))
			return true;
		return false;
	}

	protected void AgeAll(float dt)
	{
		bool removed = false;
		INT_PDA_HudLine line;
		for (int i = m_Lines.Count() - 1; i >= 0; i--)
		{
			line = m_Lines.Get(i);
			line.Age = line.Age + dt;
			if (line.Age >= FEED_LIFE)
			{
				m_Lines.RemoveOrdered(i);
				removed = true;
			}
		}
		if (removed)
		{
			m_Feed.m_Dirty = true;
			if (m_Lines.Count() == 0)
				m_FeedForced = false;
		}
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (!b.m_Active || b == m_Feed)
				continue;
			b.m_Age = b.m_Age + dt;
			if (b.m_Life > 0 && b.m_Age >= b.m_Life)
				b.m_Active = false;
		}
	}

	// Self-test hold (screenshots): everything finishes its fade-in / slide-in and then stops ageing.
	protected void HoldAge(float dt)
	{
		float cap = IN_TIME + 0.2;
		INT_PDA_HudLine line;
		for (int i = 0; i < m_Lines.Count(); i++)
		{
			line = m_Lines.Get(i);
			if (line.Age < cap)
				line.Age = Math.Min(cap, line.Age + dt);
		}
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (b.m_Active && b != m_Feed && b.m_Age < cap)
				b.m_Age = Math.Min(cap, b.m_Age + dt);
		}
	}

	protected void EnsureRoot()
	{
		int sw = INT_UIF_Scale.ScreenW();
		int sh = INT_UIF_Scale.ScreenH();
		if (m_Root && sw == m_ScreenW && sh == m_ScreenH)
			return;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
		m_Probe = null;
		m_ScreenW = sw;
		m_ScreenH = sh;
		m_Root = INT_UIF_Draw.Frame(null, 0, 0, 10, 10, "INT_PDA_HudRoot");
		if (!m_Root)
			return;
		m_Root.SetPos(0, 0);
		m_Root.SetSize(sw, sh);
		// text probe for the line breaks (transparent; measures with the engine like INT_GlobalChat)
		m_Probe = INT_UIF_Draw.Text(m_Root, 0, 0, 4000, 60, "", FEED_TEXT, 0);
		string rootName;
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			rootName = "INT_PDA_Hud_" + b.m_Name;
			b.m_Root = INT_UIF_Draw.Frame(m_Root, 0, 0, 10, 10, rootName);
			b.m_Body = null;
			b.m_Shown = false;
			b.m_HasY = false;
			b.m_Dirty = true;
			if (b.m_Root)
				b.m_Root.Show(false);
		}
		m_Root.Show(false);
		m_Hidden = true;
		INT_PDA_UI.OnBuilt().Invoke();
	}

	// --- build ------------------------------------------------------------------------------------------------------
	protected void BuildDirty()
	{
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (b.m_Dirty)
				Build(b);
		}
	}

	protected void Build(INT_PDA_HudBox b)
	{
		b.m_Dirty = false;
		if (!b.m_Root)
			return;
		ClearChildren(b.m_Root);
		b.m_Body = null;
		b.m_Fade.Reset();
		b.m_Pulse.Reset();
		bool want = false;
		if (m_Edit)
			want = true;
		else if (b == m_Feed)
			want = m_Lines.Count() > 0;
		else
			want = b.m_Active;
		if (!want)
			return;
		b.m_K = INT_PDA_UI.PartScale(b.m_Name);
		b.m_Op = INT_PDA_UI.PartOpacity(b.m_Name);
		b.m_Accent = INT_PDA_UI.PartAccent(b.m_Name);
		s_Builds++;
		if (b == m_Feed)
			BuildFeed(b);
		else if (b == m_Toast)
			BuildPm(b);
		else if (b == m_News)
			BuildNews(b);
		else if (b == m_Sos)
			BuildSos(b);
		else if (b == m_Bat)
			BuildBattery(b);
		else if (b == m_Invite)
			BuildInvite(b);
		else if (b == m_Faction)
			BuildFaction(b);
		if (m_Edit)
			BuildEditFrame(b);
	}

	protected void ClearChildren(Widget parent)
	{
		Widget c = parent.GetChildren();
		Widget nx;
		while (c)
		{
			nx = c.GetSibling();
			c.Unlink();
			c = nx;
		}
	}

	// Sizes the part's root and makes its "Body" frame (the fade group of the part).
	protected Widget Prepare(INT_PDA_HudBox b, float w, float h)
	{
		b.m_W = w;
		b.m_H = h;
		float k = b.m_K;
		INT_UIF_Draw.Resize(b.m_Root, w * k, h * k);
		b.m_Body = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w * k, h * k, "Body");
		return b.m_Body;
	}

	protected float PartWidth(INT_PDA_HudBox b, float def)
	{
		float w = def;
		INT_PDA_HudPart p = INT_PDA_UI.GetPart(b.m_Name);
		if (p && p.W > 0)
			w = p.W;
		return Math.Min(w, MaxPartWidth(b));
	}

	// Widest a part may be (design px at scale 1): the screen width minus the two margins (a 4:3 screen at a big
	// scale or a hand-edited width must not push the part past the edge).
	protected float MaxPartWidth(INT_PDA_HudBox b)
	{
		float f = INT_UIF_Scale.Factor();
		if (f <= 0)
			f = 1;
		float k = b.m_K;
		if (k <= 0)
			k = 1;
		float screenW = m_ScreenW;
		screenW = screenW / f;
		float w = (screenW - 2 * MARGIN) / k;
		if (w < 160)
			w = 160;
		return w;
	}

	// Caption colour: the light orange of the concept, or the player's accent.
	protected int CaptionColor(INT_PDA_HudBox b)
	{
		INT_PDA_HudPart p = INT_PDA_UI.GetPart(b.m_Name);
		if (p && p.Accent != 0)
			return p.Accent;
		return INT_PDA_Look.OWN;
	}

	protected void BuildFeed(INT_PDA_HudBox b)
	{
		array<ref INT_PDA_HudLine> src = m_Lines;
		if (m_Edit && m_Lines.Count() == 0)
			src = DemoLines();
		float k = b.m_K;
		float w = PartWidth(b, FEED_W);
		int most = FeedMax();
		float boxH = FEED_HEAD + most * (2 * FEED_ROW + 2) + (most - 1) * FEED_GAP + FEED_PAD;
		int count = src.Count();
		int first = count - most;
		if (first < 0)
			first = 0;
		float used = FEED_HEAD + FEED_PAD;
		int i;
		INT_PDA_HudLine line;
		for (i = first; i < count; i++)
		{
			line = src.Get(i);
			WrapLine(line, k, w);
			used = used + line.H;
			if (i > first)
				used = used + FEED_GAP;
		}
		// grows down from the top edge (default place, concept 5_e_zone2_hud); a feed the player moved into the lower
		// half of the screen stays pressed to its bottom edge instead
		float top = 0;
		INT_PDA_HudPart part = INT_PDA_UI.GetPart(b.m_Name);
		if (part && part.Moved == 1 && part.Y > 0.5)
			top = boxH - used;
		Widget body = Prepare(b, w, boxH);
		int accent = b.m_Accent;
		int capColor = CaptionColor(b);
		INT_UIF_Surface.Box(body, 0, top * k, w * k, used * k, 6, INT_PDA_Palette.HUD_PLATE);
		INT_UIF_Draw.Line(body, 0, top * k, 3 * k, used * k, accent);
		INT_PDA_Look.DrawIcon(body, 16 * k, (top + 10) * k, 16 * k, "signal", capColor);
		string cap = INT_PDA_Look.Caps("#STR_INT_PDA_HUD_FEED_TITLE");
		INT_UIF_Draw.Text(body, 40 * k, (top + 8) * k, (w - 56) * k, 20 * k, cap, 13 * k, capColor);
		b.m_Fade.AddChildren(body);
		float y = top + FEED_HEAD;
		for (i = first; i < count; i++)
		{
			line = src.Get(i);
			BuildRow(b.m_Root, line, y, w, k);
			y = y + line.H + FEED_GAP;
		}
	}

	// Name and one or two rows of text for the width of the feed (engine-measured).
	protected void WrapLine(INT_PDA_HudLine line, float k, float w)
	{
		float size = FEED_TEXT * k;
		float full = (w - 58) * k;
		float nameMax = full * 0.6;
		line.NameShown = line.Name;
		line.NameW = 0;
		if (line.Name != "")
		{
			float nw = Measure(line.Name, size);
			if (nw > nameMax)
			{
				line.NameShown = Ellipsize(line.Name, size, nameMax);
				nw = nameMax;
			}
			line.NameW = nw + 8 * k;
		}
		string r1;
		string r2;
		float firstW = full - line.NameW;
		WrapTwo(line.Body, size, firstW, full, r1, r2);
		line.Row1 = r1;
		line.Row2 = r2;
		line.H = FEED_ROW;
		if (r2 != "")
			line.H = 2 * FEED_ROW + 2;
	}

	protected void BuildRow(Widget parent, INT_PDA_HudLine line, float y, float w, float k)
	{
		float size = FEED_TEXT * k;
		float full = (w - 58) * k;
		float rh = FEED_ROW * k;
		float av = 22 * k;
		float ax = 14 * k;
		float ix = 18 * k;
		float isz = 14 * k;
		Widget row = INT_UIF_Draw.Frame(parent, 0, y * k, w * k, line.H * k, "Row");
		if (line.Kind == INT_PDA_HudLine.KIND_ANON)
		{
			INT_UIF_Surface.Box(row, ax, 0, av, av, 4, 0xFF000000, INT_PDA_Look.LINE2);
			INT_PDA_Look.DrawIcon(row, ix, 4 * k, isz, "user", INT_PDA_Look.DIM);
		}
		else if (line.Kind == INT_PDA_HudLine.KIND_SYSTEM)
		{
			INT_UIF_Surface.Box(row, ax, 0, av, av, 4, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
			INT_PDA_Look.DrawIcon(row, ix, 4 * k, isz, "warning", INT_PDA_Look.ORANGE);
		}
		else if (line.Kind == INT_PDA_HudLine.KIND_FACTION)
		{
			INT_PDA_Look.DrawEmblem(row, ax, 0, av, line.Emblem);
		}
		else
		{
			INT_PDA_Look.DrawAvatar(row, ax, 0, av, line.Avatar);
		}
		float tx = 44 * k;
		if (line.NameW > 0)
			INT_UIF_Draw.Text(row, tx, 0, line.NameW, rh, line.NameShown, size, line.NameColor);
		float x1 = tx + line.NameW;
		float w1 = full - line.NameW;
		if (line.Row1 != "")
			INT_UIF_Draw.Text(row, x1, 0, w1, rh, line.Row1, size, INT_PDA_Look.TEXT);
		if (line.Row2 != "")
			INT_UIF_Draw.Text(row, tx, rh + 2 * k, full, rh, line.Row2, size, INT_PDA_Look.TEXT);
		line.Fade.Reset();
		line.Fade.AddChildren(row);
	}

	// Plate of the toast type (PM, invitation): accent square with an icon, caption, 1-2 rows, optional small line.
	protected void BuildCard(INT_PDA_HudBox b, string iconName, string captionKey, string main, string sub)
	{
		float k = b.m_K;
		float w = PartWidth(b, TOAST_W);
		float textX = 92;
		float textW = w - textX - 22;
		float size = 18 * k;
		float rowsW = textW * k;
		string r1;
		string r2;
		WrapTwo(main, size, rowsW, rowsW, r1, r2);
		float y = 40;
		float rowsEnd = y + 24;
		if (r2 != "")
			rowsEnd = y + 48;
		float h = rowsEnd + 16;
		if (sub != "")
			h = rowsEnd + 30;
		if (h < 92)
			h = 92;
		Widget body = Prepare(b, w, h);
		int accent = b.m_Accent;
		int capColor = CaptionColor(b);
		INT_UIF_Surface.Box(body, 0, 0, w * k, h * k, 6, INT_PDA_Palette.TOAST_PLATE);
		INT_UIF_Draw.Line(body, (w - 4) * k, 0, 4 * k, h * k, accent);
		float sq = 56;
		float sqY = (h - sq) * 0.5;
		INT_UIF_Surface.Box(body, 18 * k, sqY * k, sq * k, sq * k, 8, accent);
		INT_PDA_Look.DrawIcon(body, 29 * k, (sqY + 11) * k, 34 * k, iconName, INT_PDA_Look.ON_ORANGE);
		string cap = INT_PDA_Look.Caps(captionKey);
		INT_UIF_Draw.Text(body, textX * k, 14 * k, rowsW, 20 * k, cap, 13 * k, capColor);
		INT_UIF_Draw.Text(body, textX * k, y * k, rowsW, 24 * k, r1, size, INT_PDA_Look.WHITE);
		if (r2 != "")
			INT_UIF_Draw.Text(body, textX * k, (y + 24) * k, rowsW, 24 * k, r2, size, INT_PDA_Look.WHITE);
		if (sub != "")
		{
			string subFit = Ellipsize(sub, 14 * k, rowsW);
			INT_UIF_Draw.Text(body, textX * k, (rowsEnd + 4) * k, rowsW, 20 * k, subFit, 14 * k, INT_PDA_Look.DIM);
		}
		b.m_Fade.AddChildren(body);
	}

	protected void BuildPm(INT_PDA_HudBox b)
	{
		string from = b.m_A;
		int n = b.m_N;
		if (!b.m_Active)
		{
			from = "Sidor";
			n = 3;
		}
		string pre = Widget.TranslateString("#STR_INT_PDA_HUD_PM_FROM");
		string main = pre + " " + from + "!";
		string sub = "";
		if (n > 1)
		{
			string unread = Widget.TranslateString("#STR_INT_PDA_HUD_PM_COUNT");
			sub = unread + " " + n.ToString();
		}
		BuildCard(b, "chat", "#STR_INT_PDA_HUD_PM_TITLE", main, sub);
	}

	protected void BuildFaction(INT_PDA_HudBox b)
	{
		string who = b.m_A;
		string text = b.m_B;
		int n = b.m_N;
		if (!b.m_Active)
		{
			who = "Hunter";
			text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_2");
			n = 2;
		}
		string main = who + ": " + text;
		string sub = "";
		if (n > 1)
		{
			string unread = Widget.TranslateString("#STR_INT_PDA_HUD_PM_COUNT");
			sub = unread + " " + n.ToString();
		}
		BuildCard(b, "users", "#STR_INT_PDA_HUD_FACTION_TITLE", main, sub);
	}

	protected void BuildInvite(INT_PDA_HudBox b)
	{
		string fname = b.m_A;
		string from = b.m_B;
		if (!b.m_Active)
		{
			fname = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_FACTION");
			from = "Hunter";
		}
		string pre = Widget.TranslateString("#STR_INT_PDA_HUD_INVITE_IN");
		string post = Widget.TranslateString("#STR_INT_PDA_HUD_INVITE_OPEN");
		string main = pre + " " + fname + " — " + post;
		string sub = "";
		if (from != "")
		{
			string invBy = Widget.TranslateString("#STR_INT_PDA_HUD_INVITE_BY");
			sub = invBy + " " + from;
		}
		BuildCard(b, "invite", "#STR_INT_PDA_HUD_INVITE_TITLE", main, sub);
	}

	protected void BuildNews(INT_PDA_HudBox b)
	{
		string title = b.m_A;
		string text = b.m_B;
		if (!b.m_Active)
		{
			title = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TITLE");
			text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TEXT");
		}
		float k = b.m_K;
		float w = PartWidth(b, NEWS_W);
		float h = 72;
		Widget body = Prepare(b, w, h);
		int accent = b.m_Accent;
		INT_UIF_Surface.Box(body, 0, 0, w * k, h * k, 6, INT_PDA_Palette.HUD_PLATE);
		INT_UIF_Draw.Line(body, 6 * k, 0, (w - 12) * k, 2 * k, accent);
		string label = INT_PDA_Look.Caps("#STR_INT_PDA_ZONE_NEWS");
		float labelSize = 15 * k;
		float lw = Measure(label, labelSize) + 4 * k;
		INT_UIF_Draw.Text(body, 22 * k, 0, lw, h * k, label, labelSize, accent);
		float dx = 22 * k + lw + 18 * k;
		INT_UIF_Draw.Line(body, dx, (h - 30) * 0.5 * k, 1 * k, 30 * k, INT_PDA_Look.LINE2);
		float tx = dx + 19 * k;
		float tw = w * k - tx - 22 * k;
		float titleSize = 21 * k;
		float textSize = 15 * k;
		string t1 = Ellipsize(title, titleSize, tw);
		string t2 = Ellipsize(text, textSize, tw);
		if (t2 == "")
		{
			INT_UIF_Draw.Text(body, tx, 0, tw, h * k, t1, titleSize, INT_PDA_Look.WHITE);
		}
		else
		{
			INT_UIF_Draw.Text(body, tx, 9 * k, tw, 28 * k, t1, titleSize, INT_PDA_Look.WHITE);
			// light, not grey: the banner plate is see-through and a bright sky behind it ate the grey line
			INT_UIF_Draw.Text(body, tx, 39 * k, tw, 22 * k, t2, textSize, INT_PDA_Look.TEXT);
		}
		b.m_Fade.AddChildren(body);
	}

	protected void BuildSos(INT_PDA_HudBox b)
	{
		string who = b.m_A;
		string dist = b.m_B;
		if (!b.m_Active)
		{
			who = "Kostya";
			dist = DemoDistance();
		}
		float k = b.m_K;
		float w = PartWidth(b, SOS_W);
		float h = 84;
		Widget body = Prepare(b, w, h);
		int accent = b.m_Accent;
		INT_UIF_Surface.Box(body, 0, 0, w * k, h * k, 6, INT_PDA_Palette.SOS_PLATE);
		string sosCap = INT_PDA_Look.Caps("#STR_INT_PDA_HUD_SOS_TITLE");
		string title = sosCap + " · " + who;
		string help = Widget.TranslateString("#STR_INT_PDA_HUD_SOS_HELP");
		string sub = help;
		if (dist != "")
			sub = dist + " — " + help;
		float tx = 80 * k;
		float tw = (w - 96) * k;
		float titleSize = 22 * k;
		float subSize = 16 * k;
		string t1 = Ellipsize(title, titleSize, tw);
		string t2 = Ellipsize(sub, subSize, tw);
		INT_UIF_Draw.Text(body, tx, 12 * k, tw, 30 * k, t1, titleSize, INT_PDA_Look.WHITE);
		INT_UIF_Draw.Text(body, tx, 44 * k, tw, 24 * k, t2, subSize, INT_PDA_Palette.SOS_TEXT);
		b.m_Fade.AddChildren(body);
		// the pulsing part: red bar and the SOS sign
		Widget pulse = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w * k, h * k, "Pulse");
		INT_UIF_Draw.Line(pulse, 0, 0, 4 * k, h * k, accent);
		INT_PDA_Look.DrawIcon(pulse, 20 * k, 20 * k, 44 * k, "sos", accent);
		b.m_Pulse.AddChildren(pulse);
	}

	protected void BuildBattery(INT_PDA_HudBox b)
	{
		float k = b.m_K;
		int mode = b.m_N;
		string pctText = b.m_A;
		if (!b.m_Active)
		{
			mode = BAT_LOW;
			pctText = "9";
		}
		string label;
		if (mode == BAT_DEAD)
		{
			label = Widget.TranslateString("#STR_INT_PDA_TOAST_PDA_DEAD");
		}
		else
		{
			string pdaCap = INT_PDA_Look.Caps("#STR_INT_PDA_HUD_BATTERY");
			label = pdaCap + " " + pctText + "%";
		}
		int accent = b.m_Accent;
		int textColor = INT_PDA_Palette.BAT_TEXT;
		INT_PDA_HudPart p = INT_PDA_UI.GetPart(b.m_Name);
		if (p && p.Accent != 0)
			textColor = accent;
		float size = 17 * k;
		float lw = Measure(label, size);
		float w = 56 + lw / k;
		if (p && p.W > 0)
			w = p.W;
		w = Math.Min(w, MaxPartWidth(b));
		float h = 34;
		Widget body = Prepare(b, w, h);
		INT_UIF_Surface.Box(body, 0, 0, w * k, h * k, 4, INT_PDA_Palette.HUD_PLATE, accent);
		INT_PDA_Look.DrawIcon(body, 12 * k, 6 * k, 22 * k, "battery", textColor);
		float textW = (w - 50) * k;
		string fit = Ellipsize(label, size, textW);
		INT_UIF_Draw.Text(body, 42 * k, 0, textW, h * k, fit, size, textColor);
		b.m_Fade.AddChildren(body);
	}

	// Edit mode: orange outline + the part's name above it (not faded, not part of the content).
	protected void BuildEditFrame(INT_PDA_HudBox b)
	{
		float k = b.m_K;
		float w = b.m_W * k;
		float h = b.m_H * k;
		int c = INT_PDA_Look.ORANGE;
		Widget ef = INT_UIF_Draw.Frame(b.m_Root, 0, 0, w, h, "INT_PDA_EditFrame");
		if (!ef)
			return;
		int fill = INT_UIF_Color.WithAlpha(c, 24);
		INT_UIF_Draw.Line(ef, 0, 0, w, h, fill);
		INT_UIF_Draw.Line(ef, -3, -3, w + 6, 2, c);
		INT_UIF_Draw.Line(ef, -3, h + 1, w + 6, 2, c);
		INT_UIF_Draw.Line(ef, -3, -3, 2, h + 6, c);
		INT_UIF_Draw.Line(ef, w + 1, -3, 2, h + 6, c);
		string key = INT_PDA_UI.PartTitleKey(b.m_Name);
		string title = INT_PDA_Look.Caps(key);
		float lw = w;
		if (lw < 240)
			lw = 240;
		INT_UIF_Draw.Text(ef, 0, -24, lw, 20, title, 13, c);
	}

	protected array<ref INT_PDA_HudLine> DemoLines()
	{
		if (m_Demo)
			return m_Demo;
		m_Demo = new array<ref INT_PDA_HudLine>();
		string anon = Widget.TranslateString("#STR_INT_PDA_HUD_ANON");
		INT_PDA_HudLine d1 = MakeDemo(INT_PDA_HudLine.KIND_PLAYER, "Hunter", 12, INT_PDA_Look.OWN, "#STR_INT_PDA_HUD_DEMO_1");
		INT_PDA_HudLine d2 = MakeDemo(INT_PDA_HudLine.KIND_PLAYER, "Ded Mazay", 31, INT_PDA_Look.ALLY, "#STR_INT_PDA_HUD_DEMO_2");
		INT_PDA_HudLine d3 = MakeDemo(INT_PDA_HudLine.KIND_ANON, anon, 0, INT_PDA_Look.DIM, "#STR_INT_PDA_HUD_DEMO_3");
		m_Demo.Insert(d1);
		m_Demo.Insert(d2);
		m_Demo.Insert(d3);
		return m_Demo;
	}

	protected INT_PDA_HudLine MakeDemo(int kind, string name, int avatar, int color, string key)
	{
		INT_PDA_HudLine d = new INT_PDA_HudLine();
		d.Kind = kind;
		d.Name = name;
		d.Avatar = avatar;
		d.NameColor = color;
		d.Body = Widget.TranslateString(key);
		return d;
	}

	// "340 м · СВ" from the player to a point 340 m to the north-east (demo / test data).
	string DemoDistance()
	{
		if (!GetGame())
			return "";
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return "";
		vector pos = p.GetPosition();
		float px = pos[0] + 196;
		float pz = pos[2] + 278;
		return INT_PDA_Client.DistanceText(px, pz);
	}

	// --- text fitting (only while building) -------------------------------------------------------------------------
	// Width of a one-line string in design px at glyph size `size` (engine probe, estimate as fallback).
	protected float Measure(string s, float size)
	{
		if (s == "")
			return 0;
		float est = INT_UIF_Draw.TextWidth(s, size) * 1.04;
		if (!m_Probe)
			return est;
		int px = INT_UIF_Scale.TextPx(size);
		m_Probe.SetTextExactSize(px);
		m_Probe.SetText(s);
		m_Probe.Update();
		int mw;
		int mh;
		m_Probe.GetTextSize(mw, mh);
		float f = INT_UIF_Scale.Factor();
		if (mw <= 0 || f <= 0)
			return est;
		return mw / f;
	}

	// How many characters of `s` fit into maxW.
	protected int FitChars(string s, float size, float maxW)
	{
		int lo = 0;
		int hi = s.LengthUtf8();
		int mid;
		string part;
		while (lo < hi)
		{
			mid = (lo + hi + 1) / 2;
			part = s.SubstringUtf8(0, mid);
			if (Measure(part, size) <= maxW)
				lo = mid;
			else
				hi = mid - 1;
		}
		return lo;
	}

	protected string Ellipsize(string s, float size, float maxW)
	{
		if (Measure(s, size) <= maxW)
			return s;
		float ew = Measure("…", size);
		int n = FitChars(s, size, maxW - ew);
		string cut = s.SubstringUtf8(0, n);
		cut.TrimInPlace();
		return cut + "…";
	}

	// Two rows: the first breaks at the last space that fits (or inside a long word), the rest ends with «…».
	// (clipchildren does not cut text — measured 2026-10-12 — so rows are whole one-line widgets.)
	protected void WrapTwo(string s, float size, float w1, float w2, out string r1, out string r2)
	{
		r1 = "";
		r2 = "";
		if (s == "")
			return;
		if (Measure(s, size) <= w1)
		{
			r1 = s;
			return;
		}
		int n = s.LengthUtf8();
		int cut = FitChars(s, size, w1);
		string head = s.SubstringUtf8(0, cut);
		string rest;
		int sp = head.LastIndexOf(" ");
		int third = head.Length() / 3;
		if (sp > 0 && sp >= third)
		{
			r1 = head.Substring(0, sp);
			rest = s.Substring(sp + 1, s.Length() - sp - 1);
		}
		else
		{
			r1 = head;
			rest = s.SubstringUtf8(cut, n - cut);
		}
		rest.TrimInPlace();
		if (rest == "")
			return;
		if (Measure(rest, size) <= w2)
			r2 = rest;
		else
			r2 = Ellipsize(rest, size, w2);
	}

	// --- per frame (no allocations) ---------------------------------------------------------------------------------
	protected void Animate(float dt)
	{
		UpdateRightOffset(dt);
		StackRightColumn();
		foreach (INT_PDA_HudBox b : m_Boxes)
			AnimateBox(b, dt);
	}

	protected bool Wants(INT_PDA_HudBox b)
	{
		if (m_Edit)
			return true;
		if (!INT_PDA_UI.IsPartShown(b.m_Name))
			return false;
		if (b == m_Feed)
			return m_Lines.Count() > 0;
		return b.m_Active;
	}

	protected void AnimateBox(INT_PDA_HudBox b, float dt)
	{
		if (!b.m_Root)
			return;
		bool want = Wants(b);
		if (want != b.m_Shown)
		{
			b.m_Shown = want;
			b.m_Root.Show(want);
			b.m_HasY = false;
			if (want)
				INT_PDA_UI.OnPartShown().Invoke(b.m_Name);
			else
				INT_PDA_UI.OnPartHidden().Invoke(b.m_Name);
		}
		if (!want)
			return;
		Place(b, dt);
		float k = b.m_Op;
		if (m_Edit && !INT_PDA_UI.IsPartShown(b.m_Name))
			k = k * EDIT_HIDDEN_ALPHA;
		if (b == m_Feed)
		{
			FadeFeed(b, k);
			return;
		}
		if (!m_Edit)
			k = k * AlertFade(b);
		b.m_Fade.Apply(k);
		if (b.m_Pulse.m_W.Count() > 0)
		{
			float s = Math.Sin(m_Time * 7.0);
			b.m_Pulse.Force(k * (0.7 + 0.3 * s));
		}
	}

	protected float AlertFade(INT_PDA_HudBox b)
	{
		float kin = 1;
		if (b.m_Age < IN_TIME)
			kin = b.m_Age / IN_TIME;
		float kout = 1;
		if (b.m_Life > 0)
		{
			float left = b.m_Life - b.m_Age;
			if (left < OUT_TIME)
				kout = left / OUT_TIME;
		}
		if (kout < 0)
			kout = 0;
		return kin * kout;
	}

	protected float LineFade(float age)
	{
		float kin = 1;
		if (age < IN_TIME)
			kin = age / IN_TIME;
		float left = FEED_LIFE - age;
		float kout = 1;
		if (left < FEED_OUT)
			kout = left / FEED_OUT;
		if (kout < 0)
			kout = 0;
		return kin * kout;
	}

	// Rows fade one by one; the plate and the caption follow the brightest row.
	protected void FadeFeed(INT_PDA_HudBox b, float k)
	{
		array<ref INT_PDA_HudLine> src = m_Lines;
		bool demo = false;
		if (m_Edit && m_Lines.Count() == 0)
		{
			src = DemoLines();
			demo = true;
		}
		float top = 0;
		float lk;
		INT_PDA_HudLine line;
		int n = src.Count();
		for (int i = 0; i < n; i++)
		{
			line = src.Get(i);
			lk = k;
			if (!demo && !m_Edit)
				lk = k * LineFade(line.Age);
			line.Fade.Apply(lk);
			if (lk > top)
				top = lk;
		}
		b.m_Fade.Apply(top);
	}

	// Screen place of a part: the player's place (INT_PDA_UI), else the default anchor; alerts slide in.
	protected void Place(INT_PDA_HudBox b, float dt)
	{
		float sw = m_ScreenW;
		float sh = m_ScreenH;
		float k = b.m_K;
		float w = INT_UIF_Draw.S(b.m_W * k);
		float h = INT_UIF_Draw.S(b.m_H * k);
		float x;
		float y;
		bool moved = false;
		INT_PDA_HudPart p = INT_PDA_UI.GetPart(b.m_Name);
		if (p && p.Moved != 0)
			moved = true;
		if (moved)
		{
			x = p.X * sw;
			y = p.Y * sh;
		}
		else if (b.m_Right)
		{
			x = sw - INT_UIF_Draw.S(MARGIN) - w;
			if (!b.m_HasY)
			{
				b.m_CurY = b.m_TargetY;
				b.m_HasY = true;
			}
			else
			{
				float step = Math.Min(1, dt * 12);
				b.m_CurY = b.m_CurY + (b.m_TargetY - b.m_CurY) * step;
			}
			y = b.m_CurY;
		}
		else if (b == m_News)
		{
			x = (sw - w) * 0.5;
			y = INT_UIF_Draw.S(NEWS_TOP);
		}
		else if (b == m_Bat)
		{
			// kept at its distance from the BOTTOM edge (the item-in-hands panel is there): on 16:10 / 4:3 / 5:4 the
			// UIF factor follows the width, and S(BAT_TOP) alone would move the tag up towards the middle
			x = INT_UIF_Draw.S(BAT_X);
			y = sh - INT_UIF_Draw.S(1080 - BAT_TOP);
		}
		else
		{
			x = INT_UIF_Draw.S(MARGIN);
			y = INT_UIF_Draw.S(FEED_TOP);
		}
		float slideT = IN_TIME + 0.1;
		if (!m_Edit && b != m_Feed && b != m_Bat && b.m_Age < slideT)
		{
			float u = 1 - b.m_Age / slideT;
			float off = INT_UIF_Draw.S(SLIDE) * u * u * u;
			if (b == m_News)
				y = y - off;
			else
				x = x + off;
		}
		float maxX = Math.Max(0, sw - w);
		float maxY = Math.Max(0, sh - h);
		x = Math.Clamp(x, 0, maxX);
		y = Math.Clamp(y, 0, maxY);
		b.m_Root.SetPos(x, y);
	}

	// Default places of the right column: toast, SOS, invitation, one under another (moved parts keep their place).
	protected void StackRightColumn()
	{
		float cursor = INT_UIF_Draw.S(MARGIN) + m_RightOff;
		float gap = INT_UIF_Draw.S(COLUMN_GAP);
		float bh;
		INT_PDA_HudPart p;
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (!b.m_Right || !Wants(b))
				continue;
			p = INT_PDA_UI.GetPart(b.m_Name);
			if (p && p.Moved != 0)
				continue;
			b.m_TargetY = cursor;
			bh = INT_UIF_Draw.S(b.m_H * b.m_K);
			cursor = cursor + bh + gap;
		}
	}

	// The right column moves below the INT_UIF toasts while those are on screen.
	protected void UpdateRightOffset(float dt)
	{
		float want = 0;
		INT_UIF_ToastManager tm = INT_UIF_ToastManager.Peek();
		if (tm && !m_Edit)
		{
			int n = tm.VisibleCount();
			if (n > 0)
			{
				float toastH = INT_UIF_ThemeManager.Get().Metric(INT_UIF_ThemeKeys.TOAST_HEIGHT, 76);
				float slot = INT_UIF_Draw.S(toastH) + INT_UIF_Draw.S(12);
				float bottom = INT_UIF_Draw.S(24) + n * slot;
				want = bottom - INT_UIF_Draw.S(MARGIN) + INT_UIF_Draw.S(6);
				if (want < 0)
					want = 0;
			}
		}
		float step = Math.Min(1, dt * 10);
		m_RightOff = m_RightOff + (want - m_RightOff) * step;
	}

	// --- INT_PDA_HudApi ---------------------------------------------------------------------------------------------
	override void ApplyPart(string part)
	{
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (part == "" || b.m_Name == part)
			{
				b.m_Dirty = true;
				b.m_HasY = false;
			}
		}
	}

	override void PlacePart(string part)
	{
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			if (part == "" || b.m_Name == part)
				b.m_HasY = false;
		}
	}

	override void SetEditMode(bool on)
	{
		if (on == m_Edit)
			return;
		m_Edit = on;
		foreach (INT_PDA_HudBox b : m_Boxes)
		{
			b.m_Dirty = true;
			b.m_HasY = false;
		}
	}

	override void RefreshVisibility()
	{
		if (!m_Root)
			return;
		m_Hidden = ShouldHide();
		m_Root.Show(!m_Hidden);
	}

	override void Preview(string part)
	{
		if (part == INT_PDA_UI.PART_FEED)
		{
			INT_PDA_ChatLine line = new INT_PDA_ChatLine();
			line.Channel = INT_PDA_Const.CH_PUBLIC;
			line.Author = "Hunter";
			line.Avatar = 12;
			line.Text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_1");
			PushLine(line, true);
		}
		else if (part == INT_PDA_UI.PART_TOAST)
		{
			PushPm("Sidor", 1);
		}
		else if (part == INT_PDA_UI.PART_NEWS)
		{
			string title = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TITLE");
			string text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TEXT");
			PushNews(title, text);
		}
		else if (part == INT_PDA_UI.PART_SOS)
		{
			string dist = DemoDistance();
			PushSos("Kostya", dist);
		}
		else if (part == INT_PDA_UI.PART_BATTERY)
		{
			ShowBatDead();
		}
		else if (part == INT_PDA_UI.PART_INVITE)
		{
			string fname = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_FACTION");
			PushInvite(fname, "Hunter");
		}
		else if (part == INT_PDA_UI.PART_FACTION)
		{
			PushFaction(DemoFactionLine("Hunter", ""));
		}
	}

	// A faction chat line for previews and tests (the own faction, if any).
	INT_PDA_ChatLine DemoFactionLine(string author, string text)
	{
		INT_PDA_ChatLine line = new INT_PDA_ChatLine();
		line.Channel = INT_PDA_Const.CH_FACTION;
		line.Author = author;
		line.Text = text;
		if (line.Text == "")
			line.Text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_2");
		INT_PDA_Profile me = INT_PDA_Client.s_Profile;
		if (me)
		{
			line.FactionId = me.FactionId;
			line.FactionName = me.FactionName;
		}
		return line;
	}

	override Widget FindPart(string part)
	{
		INT_PDA_HudBox b = BoxOf(part);
		if (!b)
			return null;
		return b.m_Root;
	}

	override bool GetPartRect(string part, out float x, out float y, out float w, out float h)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		INT_PDA_HudBox b = BoxOf(part);
		if (!b || !b.m_Root || !b.m_Shown || m_Hidden)
			return false;
		b.m_Root.GetScreenPos(x, y);
		b.m_Root.GetScreenSize(w, h);
		return w > 0;
	}

	override bool IsPartActive(string part)
	{
		INT_PDA_HudBox b = BoxOf(part);
		if (!b)
			return false;
		if (b == m_Feed)
			return m_Lines.Count() > 0;
		return b.m_Active;
	}

	// --- self-test ------------------------------------------------------------------------------------------------
	protected string Flag01(bool v)
	{
		if (v)
			return "1";
		return "0";
	}

	string StateLine()
	{
		int lines = m_Lines.Count();
		string s = "feed " + lines.ToString();
		s = s + " toast " + Flag01(m_Toast.m_Active) + "/" + m_Toast.m_N.ToString();
		s = s + " news " + Flag01(m_News.m_Active);
		s = s + " sos " + Flag01(m_Sos.m_Active);
		s = s + " invite " + Flag01(m_Invite.m_Active);
		s = s + " faction " + Flag01(m_Faction.m_Active) + "/" + m_Faction.m_N.ToString();
		s = s + " battery " + Flag01(m_Bat.m_Active) + "/" + m_Bat.m_N.ToString() + "/" + m_Bat.m_A;
		s = s + " edit " + Flag01(m_Edit) + " hidden " + Flag01(m_Hidden) + " suppressed " + Flag01(INT_PDA_Hud.s_Suppressed);
		s = s + " builds " + s_Builds.ToString() + " taken " + s_Taken.ToString();
		return s;
	}
}

// Static entry points used by the mission (INT_PDA_Mission.c) and the menu (INT_PDA_Menu.c: Suppress while open).
class INT_PDA_Hud
{
	protected static ref INT_PDA_HudLayer s_Layer;
	protected static bool s_Hooked;
	static bool s_Suppressed;		// our PDA menu is open
	static bool s_TestHold;			// self-test: nothing ages (screenshots)

	static void Init()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (!s_Layer)
		{
			s_Layer = new INT_PDA_HudLayer();
			INT_PDA_UI.RegisterHud(s_Layer);
			if (INT_PDA_UI.IsEditMode())
				s_Layer.SetEditMode(true);		// an editor switched it on before the mission
		}
		if (s_Hooked)
			return;
		INT_PDA_Client.s_OnLine.Insert(INT_PDA_Hud.OnLine);
		INT_PDA_Client.s_OnNotice.Insert(INT_PDA_Hud.OnNotice);
		INT_PDA_Client.s_OnChanged.Insert(INT_PDA_Hud.OnChanged);
		s_Hooked = true;
	}

	static void Shutdown()
	{
		if (s_Hooked)
		{
			if (INT_PDA_Client.s_OnLine)
				INT_PDA_Client.s_OnLine.Remove(INT_PDA_Hud.OnLine);
			if (INT_PDA_Client.s_OnNotice)
				INT_PDA_Client.s_OnNotice.Remove(INT_PDA_Hud.OnNotice);
			if (INT_PDA_Client.s_OnChanged)
				INT_PDA_Client.s_OnChanged.Remove(INT_PDA_Hud.OnChanged);
		}
		s_Hooked = false;
		if (s_Layer)
			s_Layer.Destroy();
		INT_PDA_UI.RegisterHud(null);
		s_Layer = null;
		s_Suppressed = false;
		s_TestHold = false;
	}

	// Every frame (MissionGameplay.OnUpdate).
	static void Update(float dt)
	{
		if (s_Layer)
			s_Layer.Tick(dt);
	}

	// true while our PDA menu is open: the whole layer hides (the PDA shows the same data).
	static void Suppress(bool on)
	{
		s_Suppressed = on;
		if (s_Layer)
			s_Layer.RefreshVisibility();
	}

	static INT_PDA_HudLayer GetLayer()
	{
		return s_Layer;
	}

	// --- client events --------------------------------------------------------------------------------------------
	// Only the public chat (CH_PUBLIC and system lines without a faction) reaches the feed. A mate's line in the
	// faction chat raises the «faction» toast while our PDA is closed (the open PDA shows the line itself).
	static void OnLine(INT_PDA_ChatLine line)
	{
		if (!s_Layer || !line)
			return;
		if (line.Channel == INT_PDA_Const.CH_FACTION)
		{
			string me = INT_PDA_Client.s_KnownCallsign;
			if (!s_Suppressed && (me == "" || line.Author != me))
				s_Layer.PushFaction(line);
			return;
		}
		if (line.Channel == INT_PDA_Const.CH_PRIVATE)
			return;
		if (line.Channel == INT_PDA_Const.CH_SYSTEM && line.FactionId != "")
			return;
		if (!INT_PDA_UI.IsPartShown(INT_PDA_UI.PART_FEED))
			return;
		if (!INT_PDA_Client.HudFeedOn() || !INT_PDA_Client.IsListening())
			return;
		s_Layer.PushLine(line, false);
	}

	static void OnNotice(int op, string a, string b)
	{
		if (!s_Layer)
			return;
		if (op == INT_PDA_Const.OP_PM_NOTICE)
		{
			int count = b.ToInt();
			s_Layer.PushPm(a, count);
		}
		else if (op == INT_PDA_Const.OP_NEWS_FLASH)
		{
			s_Layer.PushNews(a, b);
		}
		else if (op == INT_PDA_Const.OP_SOS_IN)
		{
			s_Layer.PushSos(a, b);
		}
		else if (op == INT_PDA_Const.OP_INVITE_IN)
		{
			s_Layer.PushInvite(a, b);
		}
	}

	static void OnChanged(int what)
	{
		if (s_Layer && what == INT_PDA_Client.CH_PDA)
			s_Layer.PokeBattery();
	}

	// --- test hook ------------------------------------------------------------------------------------------------
	// "hud feed <text>"     a public line from the own callsign (shown even when the PDA does not listen)
	// "hud pm <callsign>"   PM toast (real OP_PM_NOTICE path)
	// "hud news <title>"    news banner with a sample text
	// "hud sos <callsign>"  SOS alert, 340 m to the north-east of the player
	// "hud invite <name>"   invitation to the faction <name> from "Hunter"
	// "hud faction <text>"  faction chat toast: a line from "Hunter" (the same path as a live faction line)
	// "hud bat <pct>"       battery tag with that charge; "hud bat 0" = «КПК разряжен»; "hud bat off" = real battery
	// "hud edit on|off"     edit mode of INT_PDA_UI
	// "hud hide" / "hud show"  external hide (INT_PDA_UI.SetVisible)
	// "hud hold on|off"     nothing ages (screenshots);  "hud clear"  everything off;  "hud state"  log line only
	// Every command logs "[INT_PDA] HUD-TEST <cmd> -> <state>".
	static void TestCommand(string cmd)
	{
		string c = cmd.Trim();
		if (c.IndexOf("hud") != 0)
			return;
		if (!s_Layer)
			Init();
		INT_PDA_HudLayer layer = s_Layer;
		if (!layer)
			return;
		string rest = c.Substring(3, c.Length() - 3);
		rest.TrimInPlace();
		string verb = rest;
		string arg = "";
		int sp = rest.IndexOf(" ");
		if (sp > 0)
		{
			verb = rest.Substring(0, sp);
			arg = rest.Substring(sp + 1, rest.Length() - sp - 1);
			arg.TrimInPlace();
		}
		if (verb == "feed")
		{
			INT_PDA_ChatLine line = new INT_PDA_ChatLine();
			line.Channel = INT_PDA_Const.CH_PUBLIC;
			line.Author = INT_PDA_Client.s_KnownCallsign;
			if (line.Author == "")
				line.Author = "Tester";
			INT_PDA_Profile me = INT_PDA_Client.s_Profile;
			if (me)
			{
				line.Avatar = me.Avatar;
				line.FactionId = me.FactionId;
				line.FactionName = me.FactionName;
			}
			line.Text = arg;
			if (line.Text == "")
				line.Text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_1");
			layer.PushLine(line, true);
		}
		else if (verb == "pm")
		{
			string pmFrom = arg;
			if (pmFrom == "")
				pmFrom = "Sidor";
			OnNotice(INT_PDA_Const.OP_PM_NOTICE, pmFrom, "1");
		}
		else if (verb == "news")
		{
			string title = arg;
			if (title == "")
				title = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TITLE");
			string text = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_NEWS_TEXT");
			OnNotice(INT_PDA_Const.OP_NEWS_FLASH, title, text);
		}
		else if (verb == "sos")
		{
			string sosFrom = arg;
			if (sosFrom == "")
				sosFrom = "Kostya";
			string dist = layer.DemoDistance();
			OnNotice(INT_PDA_Const.OP_SOS_IN, sosFrom, dist);
		}
		else if (verb == "invite")
		{
			string fname = arg;
			if (fname == "")
				fname = Widget.TranslateString("#STR_INT_PDA_HUD_DEMO_FACTION");
			OnNotice(INT_PDA_Const.OP_INVITE_IN, fname, "Hunter");
		}
		else if (verb == "faction")
		{
			INT_PDA_ChatLine facLine = layer.DemoFactionLine("Hunter", arg);
			OnLine(facLine);
		}
		else if (verb == "bat")
		{
			int pct = -1;
			if (arg != "off" && arg != "")
				pct = arg.ToInt();
			layer.TestBattery(pct);
		}
		else if (verb == "edit")
		{
			bool editOn = false;
			if (arg == "on")
				editOn = true;
			INT_PDA_UI.SetEditMode(editOn);
		}
		else if (verb == "hide")
		{
			INT_PDA_UI.SetVisible(false);
		}
		else if (verb == "show")
		{
			INT_PDA_UI.SetVisible(true);
		}
		else if (verb == "hold")
		{
			s_TestHold = false;
			if (arg == "on")
				s_TestHold = true;
		}
		else if (verb == "clear")
		{
			layer.ClearAll();
		}
		string state = layer.StateLine();
		INT_PDA_Log.Raw("HUD-TEST " + c + " -> " + state);
	}
}
