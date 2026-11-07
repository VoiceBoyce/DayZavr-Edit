// INT_PDA (client) — «Профиль» (owner brief «Интерфейс 1», concept E profile frame): the player in full height in his
// current clothes (live PlayerPreviewWidget, drag to turn), avatar, callsign, faction card + rank/role, «Сменить
// позывной / Сменить пароль / Выход», status (in the net since, days in the Zone, current life, faction), the PDA's
// battery and modules, the player's own switches «Лента КПК на экране» / «Звуки КПК» (client prefs). Nothing is asked
// from the server here except the renames.

class INT_PDA_PageProfile : INT_PDA_Page
{
	protected Widget m_PreviewHost;
	protected PlayerPreviewWidget m_Preview;
	protected ButtonWidget m_PreviewHit;
	protected bool m_Drag;
	protected int m_DragX;
	protected float m_Yaw;
	protected Widget m_Mid;
	protected ImageWidget m_Avatar;
	protected ButtonWidget m_AvatarHit;
	protected TextWidget m_Callsign;
	protected Widget m_FactionRow;
	protected ImageWidget m_Emblem;
	protected TextWidget m_FactionName;
	protected TextWidget m_FactionRank;
	protected TextWidget m_NoFaction;
	protected ref array<ButtonWidget> m_BtnHits = new array<ButtonWidget>();
	protected ref array<ref INT_UIF_Surface> m_BtnBoxes = new array<ref INT_UIF_Surface>();
	protected ref array<ref INT_UIF_Surface> m_ModTrack = new array<ref INT_UIF_Surface>();
	protected ref array<ref INT_UIF_Surface> m_ModKnob = new array<ref INT_UIF_Surface>();
	protected TextWidget m_StatSince;
	protected TextWidget m_StatDays;
	protected TextWidget m_StatLife;
	protected TextWidget m_StatFaction;
	protected Widget m_Right;
	protected TextWidget m_BatPct;
	protected ImageWidget m_BatBar;
	protected TextWidget m_BatNote;
	protected ref array<TextWidget> m_ModState = new array<TextWidget>();
	protected ref array<Widget> m_ModRow = new array<Widget>();
	protected TextWidget m_ListenLine;
	protected TextWidget m_DiscordLine;
	// player settings under the modules (client-side, saved in INT_PDA_Client.json): 0 = PDA feed on the HUD, 1 = sounds
	protected ref array<ButtonWidget> m_SetHits = new array<ButtonWidget>();
	protected ref array<ref INT_UIF_Surface> m_SetTrack = new array<ref INT_UIF_Surface>();
	protected ref array<ref INT_UIF_Surface> m_SetKnob = new array<ref INT_UIF_Surface>();
	protected float m_Tick;

	override void OnBuild()
	{
		// left: the live model
		m_PreviewHost = INT_UIF_Draw.Frame(m_Root, 20, 0, 420, m_H, "INT_PDA_PreviewHost");
		INT_UIF_Draw.Texture(m_PreviewHost, 0, 0, 420, m_H, INT_PDA_Look.SET_DEV + "vignette", ARGB(120, 255, 255, 255));
		// the model fills its frame: 10 px above the footer line so the boots stay clear of it (resolution pass)
		Widget previewArea = INT_UIF_Draw.Frame(m_PreviewHost, 0, 4, 420, m_H - 14, "INT_PDA_PreviewArea");
		Widget w = GetGame().GetWorkspace().CreateWidgets("INT_PDA_CL/gui/layouts/int_pda_preview.layout", previewArea);
		if (w)
			m_Preview = PlayerPreviewWidget.Cast(w.FindAnyWidget("INT_PDA_Preview"));
		m_PreviewHit = Hit(m_PreviewHost, 0, 0, 420, m_H);
		// middle
		m_Mid = INT_UIF_Draw.Frame(m_Root, 450, 0, 480, m_H, "INT_PDA_ProfMid");
		Widget av = INT_UIF_Draw.Frame(m_Mid, 0, 30, 128, 128, "INT_PDA_ProfAvatar");
		INT_UIF_Surface.Box(av, 0, 0, 128, 128, 6, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_Avatar = INT_PDA_Look.DrawAvatar(av, 4, 4, 120, 0);
		INT_UIF_Draw.Line(av, 4, 124, 120, 4, INT_PDA_Look.ORANGE);
		m_AvatarHit = Hit(m_Mid, 0, 30, 128, 128);
		INT_UIF_Draw.Text(m_Mid, 150, 24, 300, 18, INT_PDA_Look.Caps("#STR_INT_PDA_F_CALLSIGN"), 13, INT_PDA_Look.ORANGE);
		m_Callsign = INT_UIF_Draw.Text(m_Mid, 148, 40, 340, 90, "", 76, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Line(m_Mid, 0, 188, 480, 1, INT_PDA_Look.LINE);
		m_FactionRow = INT_UIF_Draw.Frame(m_Mid, 0, 200, 480, 74, "INT_PDA_ProfFaction");
		m_Emblem = INT_PDA_Look.DrawEmblem(m_FactionRow, 0, 4, 66, 1);
		m_FactionName = INT_UIF_Draw.Text(m_FactionRow, 88, 2, 390, 40, "", 32, INT_PDA_Look.TEXT);
		m_FactionRank = INT_UIF_Draw.Text(m_FactionRow, 88, 42, 390, 26, "", 18, INT_PDA_Look.DIM);
		m_NoFaction = INT_UIF_Draw.Text(m_Mid, 0, 214, 480, 40, "#STR_INT_PDA_NO_FACTION", 20, INT_PDA_Look.DIM2);
		array<string> titles = {"#STR_INT_PDA_BTN_RENAME", "#STR_INT_PDA_BTN_PASSWORD", "#STR_INT_PDA_BTN_LOGOUT"};
		array<string> icons = {"edit", "key", "logout"};
		for (int i = 0; i < 3; i++)
		{
			float by = 290 + i * 62;
			int border = INT_PDA_Look.LINE2;
			int tc = INT_PDA_Look.TEXT;
			if (i == 2)
			{
				border = 0xFF5A2B27;
				tc = 0xFFEC6A5C;
			}
			Widget b = INT_UIF_Draw.Frame(m_Mid, 0, by, 480, 52, "INT_PDA_ProfBtn");
			INT_UIF_Surface box = INT_UIF_Surface.Box(b, 0, 0, 480, 52, 4, INT_PDA_Look.PANEL, border);
			INT_PDA_Look.DrawIcon(b, 20, 15, 22, icons[i], tc);
			INT_UIF_Draw.Text(b, 56, 0, 360, 52, INT_PDA_Look.Caps(titles[i]), 16, tc);
			INT_UIF_Draw.Icon(b, 446, 18, 16, "chevron_right", INT_PDA_Look.DIM2);
			m_BtnBoxes.Insert(box);
			m_BtnHits.Insert(Hit(m_Mid, 0, by, 480, 52));
		}
		// status 2 x 2
		m_StatSince = Stat(m_Mid, 0, 484, "#STR_INT_PDA_STAT_SINCE", false);
		m_StatDays = Stat(m_Mid, 246, 484, "#STR_INT_PDA_STAT_DAYS", false);
		m_StatLife = Stat(m_Mid, 0, 574, "#STR_INT_PDA_STAT_LIFE", false);
		m_StatFaction = Stat(m_Mid, 246, 574, "#STR_INT_PDA_STAT_FACTION", true);
		// right: battery and modules
		m_Right = INT_UIF_Draw.Frame(m_Root, 980, 0, 440, m_H, "INT_PDA_ProfRight");
		INT_UIF_Draw.Text(m_Right, 0, 30, 300, 18, INT_PDA_Look.Caps("#STR_INT_PDA_MODULES"), 13, INT_PDA_Look.DIM);
		m_BatPct = INT_UIF_Draw.Text(m_Right, 0, 50, 220, 70, "", 60, INT_PDA_Look.TEXT);
		INT_UIF_Draw.Image(m_Right, 230, 76, 210, 8, "iuf_white", INT_PDA_Look.LINE2);
		m_BatBar = INT_UIF_Draw.Image(m_Right, 230, 76, 1, 8, "iuf_white", INT_PDA_Look.ORANGE);
		m_BatNote = INT_UIF_Draw.Text(m_Right, 230, 90, 210, 20, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		array<string> names = {"GPS", "#STR_INT_PDA_MOD_MAP", "#STR_INT_PDA_MOD_CHAT", "#STR_INT_PDA_MOD_GP"};
		array<string> subs = {"#STR_INT_PDA_MODSUB_GPS", "#STR_INT_PDA_MODSUB_MAP", "#STR_INT_PDA_MODSUB_CHAT", "#STR_INT_PDA_MODSUB_GP"};
		array<string> micons = {"gps", "map", "chat", "flag"};
		// module rows 76 high (86 before the settings came in), 8 apart: 130 .. 458
		for (int m = 0; m < 4; m++)
		{
			float ry = 130 + m * 84;
			Widget row = INT_UIF_Draw.Frame(m_Right, 0, ry, 440, 76, "INT_PDA_ProfMod");
			INT_UIF_Surface.Box(row, 0, 0, 440, 76, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			INT_UIF_Draw.Line(row, 0, 0, 3, 76, INT_PDA_Look.ORANGE);
			INT_UIF_Surface.Box(row, 20, 12, 52, 52, 6, INT_PDA_Look.PANEL2, 0);
			INT_PDA_Look.DrawIcon(row, 32, 24, 28, micons[m], INT_PDA_Look.ORANGE);
			INT_UIF_Draw.Text(row, 90, 8, 220, 32, INT_PDA_Look.Caps(names[m]), 24, INT_PDA_Look.TEXT);
			INT_UIF_Draw.Text(row, 90, 41, 240, 22, subs[m], 15, INT_PDA_Look.DIM);
			TextWidget st = INT_UIF_Draw.Text(row, 230, 0, 122, 76, "", 15, INT_PDA_Look.ALLY, INT_UIF_Align.RIGHT);
			m_ModState.Insert(st);
			// switch look of concept E (state only: a module is on while it sits in its slot)
			INT_UIF_Surface track = INT_UIF_Surface.Pill(row, 368, 26, 46, 24, 0xFF2B2B30);
			INT_UIF_Surface knob = INT_UIF_Surface.Pill(row, 371, 29, 18, 18, 0xFF77767A);
			m_ModTrack.Insert(track);
			m_ModKnob.Insert(knob);
			m_ModRow.Insert(row);
		}
		INT_UIF_Draw.Line(m_Right, 0, 470, 440, 1, INT_PDA_Look.LINE);
		// player settings: icon + label + the same switch as the modules (but this one is clickable)
		BuildSetting(480, "chat", "#STR_INT_PDA_SET_HUD_FEED");
		BuildSetting(522, "volume", "#STR_INT_PDA_SET_SOUNDS");
		INT_UIF_Draw.Line(m_Right, 0, 572, 440, 1, INT_PDA_Look.LINE);
		INT_PDA_Look.DrawIcon(m_Right, 0, 588, 22, "signal", INT_PDA_Look.ORANGE);
		m_ListenLine = INT_UIF_Draw.Text(m_Right, 36, 584, 404, 30, "", 16, INT_PDA_Look.DIM);
		INT_PDA_Look.DrawIcon(m_Right, 0, 628, 22, "user", INT_PDA_Look.ORANGE);
		m_DiscordLine = INT_UIF_Draw.Text(m_Right, 36, 624, 404, 30, "", 16, INT_PDA_Look.DIM);
	}

	// One settings row (440 x 40) of the right column at y.
	protected void BuildSetting(float y, string icon, string key)
	{
		Widget row = INT_UIF_Draw.Frame(m_Right, 0, y, 440, 40, "INT_PDA_ProfSet");
		INT_PDA_Look.DrawIcon(row, 0, 9, 22, icon, INT_PDA_Look.ORANGE);
		string text = Widget.TranslateString(key);
		string label = INT_PDA_Look.Fit(text, 320, 16);
		INT_UIF_Draw.Text(row, 36, 0, 324, 40, label, 16, INT_PDA_Look.TEXT);
		INT_UIF_Surface track = INT_UIF_Surface.Pill(row, 368, 8, 46, 24, 0xFF2B2B30);
		INT_UIF_Surface knob = INT_UIF_Surface.Pill(row, 371, 11, 18, 18, 0xFF77767A);
		m_SetTrack.Insert(track);
		m_SetKnob.Insert(knob);
		m_SetHits.Insert(Hit(m_Right, 0, y, 440, 40));
	}

	protected void PaintSwitch(INT_UIF_Surface track, INT_UIF_Surface knob, bool on, float knobY)
	{
		if (!track || !knob)
			return;
		if (on)
		{
			track.SetFill(INT_PDA_Look.ORANGE);
			knob.SetFill(INT_PDA_Look.BG);
			INT_UIF_Draw.Move(knob.GetRoot(), 393, knobY);
			return;
		}
		track.SetFill(0xFF2B2B30);
		knob.SetFill(0xFF77767A);
		INT_UIF_Draw.Move(knob.GetRoot(), 371, knobY);
	}

	protected void RefreshSettings()
	{
		bool feedOn = INT_PDA_Client.HudFeedOn();
		bool soundsOn = INT_PDA_Client.SoundsOn();
		INT_UIF_Surface track;
		INT_UIF_Surface knob;
		for (int i = 0; i < m_SetTrack.Count() && i < m_SetKnob.Count(); i++)
		{
			// hoisted: two call results of one type as arguments overwrite each other (INT_PDA_Look header)
			track = m_SetTrack[i];
			knob = m_SetKnob[i];
			if (i == 0)
				PaintSwitch(track, knob, feedOn, 11);
			else
				PaintSwitch(track, knob, soundsOn, 11);
		}
	}

	// The 3D model draws above every 2D widget: the menu hides it before the power-off shutter closes.
	override void HidePreview()
	{
		if (m_Preview)
			m_Preview.Show(false);
	}

	protected TextWidget Stat(Widget parent, float x, float y, string key, bool accent)
	{
		int bar = INT_PDA_Look.LINE2;
		if (accent)
			bar = INT_PDA_Look.ORANGE;
		INT_UIF_Draw.Line(parent, x, y, 2, 76, bar);
		INT_UIF_Draw.Text(parent, x + 16, y, 220, 18, INT_PDA_Look.Caps(key), 13, INT_PDA_Look.DIM);
		return INT_UIF_Draw.Text(parent, x + 16, y + 22, 228, 46, "", 30, INT_PDA_Look.TEXT);
	}

	override void OnShow()
	{
		BindPreview();
		Refresh();
		Reveal(m_Mid, 0.06);
		Reveal(m_Right, 0.14);
	}

	override void OnHide()
	{
		m_Drag = false;
	}

	protected void BindPreview()
	{
		if (!m_Preview)
			return;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		m_Preview.Show(true);
		m_Preview.SetPlayer(p);
		m_Preview.SetModelPosition(Vector(0, 0, 0.605));
		m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		m_Preview.Refresh();
	}

	override void OnChanged(int what)
	{
		if (m_Active && (what == INT_PDA_Client.CH_PROFILE || what == INT_PDA_Client.CH_AUTH || what == INT_PDA_Client.CH_FACTION || what == INT_PDA_Client.CH_PDA))
			Refresh();
	}

	protected void Refresh()
	{
		INT_PDA_Profile p = INT_PDA_Client.s_Profile;
		if (!p)
		{
			m_Callsign.SetText("—");
			m_FactionRow.Show(false);
			m_NoFaction.Show(true);
			m_NoFaction.SetText("#STR_INT_PDA_NO_ACCOUNT");
			m_StatSince.SetText("—");
			m_StatDays.SetText("—");
			m_StatFaction.SetText("—");
			m_DiscordLine.SetText("");
		}
		else
		{
			// cut with «…» to the boxes (callsign 340 px at 76, faction 390 at 32, the status tile 228 at 30)
			string callsign = INT_PDA_Look.Fit(p.Callsign, 330, 76);
			m_Callsign.SetText(callsign);
			m_Avatar.LoadImageFile(0, INT_PDA_Look.AvatarPath(p.Avatar));
			bool inFaction = p.InFaction();
			m_FactionRow.Show(inFaction);
			m_NoFaction.Show(!inFaction);
			m_NoFaction.SetText("#STR_INT_PDA_NO_FACTION");
			if (inFaction)
			{
				// capitals incl. Cyrillic / accented letters (string.ToUpper is ASCII only)
				string fname = INT_PDA_Look.CapsText(p.FactionName);
				fname = INT_PDA_Look.Fit(fname, 386, 32);
				m_FactionName.SetText(fname);
				m_Emblem.LoadImageFile(0, INT_PDA_Look.EmblemPath(p.FactionEmblem));
				string rank = Widget.TranslateString(INT_PDA_Look.RoleKey(p.Role));
				if (p.Rank != "")
					rank = p.Rank + " · " + rank;
				rank = INT_PDA_Look.Fit(rank, 386, 18);
				m_FactionRank.SetText(rank);
				string tile = INT_PDA_Look.Fit(p.FactionName, 224, 30);
				m_StatFaction.SetText(tile);
			}
			else
			{
				m_StatFaction.SetText("—");
			}
			m_StatSince.SetText(DateOf(p.Registered));
			m_StatDays.SetText(p.DaysInZone.ToString());
			string discord = Widget.TranslateString("#STR_INT_PDA_F_DISCORD") + ": " + p.Discord;
			discord = INT_PDA_Look.Fit(discord, 400, 16);
			m_DiscordLine.SetText(discord);
		}
		RefreshLive();
	}

	// "YYYY-MM-DD ..." -> "DD.MM.YYYY"
	protected string DateOf(string stamp)
	{
		if (stamp.Length() < 10)
			return stamp;
		return stamp.Substring(8, 2) + "." + stamp.Substring(5, 2) + "." + stamp.Substring(0, 4);
	}

	// Values that change while the page is open: battery, modules, current life, chat state.
	protected void RefreshLive()
	{
		INT_PDA pda = m_Menu.m_Pda;
		int pct = 0;
		if (pda)
		{
			pct = (int)Math.Round(pda.BatteryLevel() * 100);
			m_BatPct.SetText(pct.ToString() + "%");
			INT_UIF_Draw.Resize(m_BatBar, Math.Max(1, 210 * pct / 100.0), 8);
			int barColor = INT_PDA_Look.ORANGE;
			if (pct <= INT_PDA_Client.s_Settings.LowBatteryPercent)
				barColor = INT_PDA_Look.ENEMY;
			m_BatBar.SetColor(barColor);
			// ~hours left at the config drain (0.0025 per second of a 50-unit 9V battery)
			float hours = pct / 100.0 * 50.0 / 0.0025 / 3600.0;
			m_BatNote.SetText(Widget.TranslateString("#STR_INT_PDA_BATTERY_9V") + " · ~" + ((int)Math.Round(hours)).ToString() + " " + Widget.TranslateString("#STR_INT_PDA_HOURS"));
		}
		else
		{
			// the admin PDA (Alt+L) without a device: no battery to show
			m_BatPct.SetText("—");
			INT_UIF_Draw.Resize(m_BatBar, 1, 8);
			m_BatNote.SetText("");
		}
		array<bool> on = {false, false, false, false};
		if (pda)
		{
			on[0] = pda.HasGps();
			on[1] = pda.HasMap();
			on[2] = pda.HasChat();
			on[3] = pda.HasGp();
		}
		for (int i = 0; i < m_ModState.Count(); i++)
		{
			INT_UIF_Surface track = null;
			INT_UIF_Surface knob = null;
			if (i < m_ModTrack.Count())
			{
				track = m_ModTrack[i];
				knob = m_ModKnob[i];
			}
			if (on[i])
			{
				m_ModState[i].SetText(INT_PDA_Look.Caps("#STR_INT_PDA_MOD_ON"));
				m_ModState[i].SetColor(INT_PDA_Look.ALLY);
			}
			else
			{
				m_ModState[i].SetText(INT_PDA_Look.Caps("#STR_INT_PDA_MOD_OFF"));
				m_ModState[i].SetColor(INT_PDA_Look.DIM2);
			}
			PaintSwitch(track, knob, on[i], 29);
		}
		RefreshSettings();
		INT_PDA_Profile prof = INT_PDA_Client.s_Profile;
		if (prof)
		{
			int secs = prof.LifeSec + (GetGame().GetTime() - INT_PDA_Client.s_ProfileAtMs) / 1000;
			int hh = secs / 3600;
			int mm = (secs % 3600) / 60;
			m_StatLife.SetText(hh.ToString() + " " + Widget.TranslateString("#STR_INT_PDA_H") + " " + mm.ToString() + " " + Widget.TranslateString("#STR_INT_PDA_MIN"));
		}
		else
		{
			m_StatLife.SetText("—");
		}
		if (INT_PDA_Client.IsListening())
			m_ListenLine.SetText("#STR_INT_PDA_NET_ON");
		else
			m_ListenLine.SetText("#STR_INT_PDA_NET_OFF");
	}

	override void Update(float dt)
	{
		m_Tick = m_Tick + dt;
		if (m_Tick >= 1.0)
		{
			m_Tick = 0;
			RefreshLive();
		}
		if (m_Drag)
		{
			int mx;
			int my;
			GetMousePos(mx, my);
			if ((GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) == 0)
			{
				m_Drag = false;
				return;
			}
			m_Yaw = m_Yaw + (mx - m_DragX) * 0.6;
			m_DragX = mx;
			if (m_Preview)
				m_Preview.SetModelOrientation(Vector(0, m_Yaw, 0));
		}
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w == m_PreviewHit && button == MouseState.LEFT)
		{
			m_Drag = true;
			m_DragX = x;
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_AvatarHit)
		{
			OpenAvatarPicker();
			return true;
		}
		// settings (client-side, no account needed)
		ButtonWidget clicked = ButtonWidget.Cast(w);
		int setting = -1;
		if (clicked)
			setting = m_SetHits.Find(clicked);
		if (setting == 0)
		{
			INT_PDA_Sfx.Play("Toggle");
			INT_PDA_Client.SetHudFeed(!INT_PDA_Client.HudFeedOn());
			RefreshSettings();
			return true;
		}
		if (setting == 1)
		{
			// the click sounds while the sounds are still on (switching off) / after they are on again
			bool soundsOn = INT_PDA_Client.SoundsOn();
			if (soundsOn)
				INT_PDA_Sfx.Play("Toggle");
			INT_PDA_Client.SetSounds(!soundsOn);
			if (!soundsOn)
				INT_PDA_Sfx.Play("Toggle");
			RefreshSettings();
			return true;
		}
		for (int i = 0; i < m_BtnHits.Count(); i++)
		{
			if (w != m_BtnHits[i])
				continue;
			INT_PDA_Sfx.Play("Click");
			if (!INT_PDA_Client.s_LoggedIn)
				return true;
			if (i == 0)
			{
				INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_BTN_RENAME", "#STR_INT_PDA_F_CALLSIGN", "", false, INT_PDA_Client.s_KnownCallsign);
				p.m_OnDone.Insert(OnRenameDone);
			}
			else if (i == 1)
			{
				INT_PDA_Prompt q = m_Menu.OpenPrompt("#STR_INT_PDA_BTN_PASSWORD", "#STR_INT_PDA_F_OLD_PASSWORD", "#STR_INT_PDA_F_NEW_PASSWORD", true);
				q.m_OnDone.Insert(OnPasswordDone);
			}
			else
			{
				INT_PDA_Client.SendLogout();
			}
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		for (int i = 0; i < m_BtnHits.Count(); i++)
		{
			if (w == m_BtnHits[i])
				m_BtnBoxes[i].FadeFill(INT_PDA_Look.PANEL2, 0.12);
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		for (int i = 0; i < m_BtnHits.Count(); i++)
		{
			if (w == m_BtnHits[i])
				m_BtnBoxes[i].FadeFill(INT_PDA_Look.PANEL, 0.12);
		}
		return false;
	}

	protected void OnRenameDone(bool ok, string a, string b)
	{
		m_Menu.ClosePrompt();
		if (!ok)
			return;
		a.TrimInPlace();
		if (!INT_PDA_Util.IsCallsign(a))
		{
			INT_UIF_Toast.Error("#STR_INT_PDA_R_CALLSIGN_FORMAT");
			return;
		}
		INT_PDA_Client.SendSetCallsign(a);
	}

	protected void OnPasswordDone(bool ok, string a, string b)
	{
		m_Menu.ClosePrompt();
		if (!ok)
			return;
		if (!INT_PDA_Util.IsPassword(b))
		{
			INT_UIF_Toast.Error("#STR_INT_PDA_R_PASSWORD_FORMAT");
			return;
		}
		// the saved login takes the new password only when the server confirms it (INT_PDA_Client.CommitChange)
		INT_PDA_Client.SendSetPassword(a, b);
	}

	// Next avatar on click (a simple cycle; the full grid is on the registration screen).
	protected void OpenAvatarPicker()
	{
		INT_PDA_Profile p = INT_PDA_Client.s_Profile;
		if (!p)
			return;
		int next = p.Avatar + 1;
		if (next >= INT_PDA_Const.AVATARS)
			next = 0;
		INT_PDA_Sfx.Play("Toggle");
		INT_PDA_Client.SendSetAvatar(next);
	}

	override void OnResult(int requestOp, int code)
	{
		if (requestOp == INT_PDA_Const.OP_SET_CALLSIGN || requestOp == INT_PDA_Const.OP_SET_PASSWORD)
			Toast(code);
	}
}

// «Новости» — Zone news posted by admins (newest first).
class INT_PDA_PageNews : INT_PDA_Page
{
	protected Widget m_List;
	protected TextWidget m_Empty;
	protected Widget m_Feature;
	protected TextWidget m_FTitle;
	protected TextWidget m_FTime;
	protected MultilineTextWidget m_FText;

	override void OnBuild()
	{
		Widget top = Panel(m_Root, 40, 24, 1376, 230, "");
		m_Feature = top;
		INT_UIF_Draw.Line(top, 0, 0, 4, 230, INT_PDA_Look.ORANGE);
		INT_PDA_Look.DrawIcon(top, 36, 34, 30, "radiation", INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(top, 80, 30, 600, 18, INT_PDA_Look.Caps("#STR_INT_PDA_ZONE_NEWS"), 13, INT_PDA_Look.ORANGE);
		m_FTime = INT_UIF_Draw.Text(top, 1000, 30, 340, 18, "", 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
		m_FTitle = INT_UIF_Draw.Text(top, 80, 54, 1260, 56, "", 44, INT_PDA_Look.TEXT);
		m_FText = INT_PDA_Look.MText(top, 80, 120, 1260, 90, "", 19, INT_PDA_Look.DIM);
		m_List = INT_UIF_Draw.Frame(m_Root, 40, 274, 1376, 380, "INT_PDA_NewsList");
		m_Empty = INT_UIF_Draw.Text(m_Root, 40, 300, 1376, 30, "#STR_INT_PDA_NEWS_NONE", 18, INT_PDA_Look.DIM2, INT_UIF_Align.CENTER);
	}

	override void OnShow()
	{
		INT_PDA_Client.MarkRead("news");
		INT_PDA_Client.RequestNews();
		Refresh();
	}

	override void OnChanged(int what)
	{
		if (m_Active && what == INT_PDA_Client.CH_NEWS)
			Refresh();
	}

	// only live news (an admin lifetime ends them; the server sends live ones, this drops one that ran out since)
	protected ref array<INT_PDA_News> m_Live = new array<INT_PDA_News>();
	protected float m_CheckT;

	override void Update(float dt)
	{
		m_CheckT = m_CheckT + dt;
		if (m_CheckT < 5)
			return;
		m_CheckT = 0;
		int before = m_Live.Count();
		array<INT_PDA_News> now = new array<INT_PDA_News>();
		INT_PDA_Client.LiveNews(now);
		if (now.Count() != before)
			Refresh();
	}

	protected void Refresh()
	{
		INT_UIF_Widgets.ClearChildren(m_List);
		INT_PDA_Client.LiveNews(m_Live);
		array<INT_PDA_News> items = m_Live;
		int n = items.Count();
		m_Feature.Show(n > 0);
		m_Empty.Show(n == 0);
		if (n == 0)
			return;
		INT_PDA_News top = items[n - 1];
		// capitals incl. Cyrillic / accented letters (string.ToUpper is ASCII only), no caps cache for server text
		string title = INT_PDA_Look.CapsText(top.Title);
		title = INT_PDA_Look.Fit(title, 1260, 44);
		m_FTitle.SetText(title);
		m_FTime.SetText(top.Time);
		m_FText.SetText(top.Text);
		Reveal(m_Feature, 0.04);
		float y = 0;
		int shown = 0;
		for (int i = n - 2; i >= 0 && shown < 4; i--)
		{
			INT_PDA_News it = items[i];
			Widget row = INT_UIF_Draw.Frame(m_List, 0, y, 1376, 86, "INT_PDA_NewsRow");
			INT_UIF_Surface.Box(row, 0, 0, 1376, 86, 4, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
			string rowTitle = INT_PDA_Look.Fit(it.Title, 900, 20);
			string rowText = INT_PDA_Look.Fit(it.Text, 1320, 16);
			INT_UIF_Draw.Text(row, 24, 12, 900, 26, rowTitle, 20, INT_PDA_Look.TEXT);
			INT_UIF_Draw.Text(row, 1100, 12, 252, 20, it.Time, 13, INT_PDA_Look.DIM, INT_UIF_Align.RIGHT);
			INT_UIF_Draw.Text(row, 24, 44, 1320, 30, rowText, 16, INT_PDA_Look.DIM);
			Reveal(row, 0.08 + shown * 0.06);
			y = y + 96;
			shown++;
		}
	}
}
