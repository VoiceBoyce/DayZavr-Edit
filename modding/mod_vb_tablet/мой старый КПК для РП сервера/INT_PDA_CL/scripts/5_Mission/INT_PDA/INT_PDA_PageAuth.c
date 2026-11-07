// INT_PDA (client) — registration / login screen (owner brief «Интерфейс 1», concept E register frame). Registration:
// callsign (Latin letters only — checked as you type, owner 2026-10-22), password, Discord ID, avatar (73 portraits,
// pages of 30), «Сохранить пароль». Login: callsign + password (+ the saved ones). The DayZavr logo appears ONLY on
// this screen (owner 2026-10-23). The server decides; its answer comes back through INT_PDA_Client.s_OnResult.

class INT_PDA_PageAuth : INT_PDA_Page
{
	static const int GRID_COLS = 5;
	static const int GRID_ROWS = 6;
	static const float CELL = 70;
	static const float CELL_GAP = 10;

	protected bool m_Register;
	protected Widget m_Left;
	protected Widget m_Form;
	protected Widget m_Grid;
	protected TextWidget m_Kicker;
	protected TextWidget m_Title;
	protected TextWidget m_Title2;
	protected MultilineTextWidget m_Desc;
	protected ImageWidget m_Logo;
	protected ImageWidget m_BigAvatar;
	protected TextWidget m_AvatarNo;
	protected TextWidget m_AvatarCap;
	protected INT_UIF_TextInput m_Callsign;
	protected ref INT_PDA_PassInput m_Pass;
	protected INT_UIF_TextInput m_Discord;
	protected INT_UIF_Checkbox m_Save;
	protected INT_UIF_Button m_Submit;
	protected TextWidget m_Error;
	protected ImageWidget m_ErrorIcon;
	protected TextWidget m_SwitchText;
	protected TextWidget m_SwitchLink;			// the part after "?" in orange (concept: «Уже есть аккаунт? Войти»)
	protected float m_SwitchY;
	protected ButtonWidget m_SwitchHit;
	protected Widget m_DiscordBlock;
	protected Widget m_PassBlock;
	protected TextWidget m_Counter;
	protected ref array<ImageWidget> m_Cells = new array<ImageWidget>();
	protected ref array<ButtonWidget> m_CellHits = new array<ButtonWidget>();
	protected Widget m_Sel;
	protected ButtonWidget m_PrevHit;
	protected ButtonWidget m_NextHit;
	protected TextWidget m_PageText;
	protected int m_GridPage;
	protected int m_Avatar = 6;
	protected bool m_Busy;
	protected int m_ShownKnown = -2;		// INT_PDA_Client.s_KnownAvatar the login form was built with

	override void OnBuild()
	{
		// left column: title, (registration) logo, the chosen avatar
		m_Left = INT_UIF_Draw.Frame(m_Root, 56, 0, 400, m_H, "INT_PDA_AuthLeft");
		m_Logo = INT_UIF_Draw.Texture(m_Left, 0, 20, 300, 84, INT_PDA_Look.LOGO, INT_PDA_Look.WHITE);
		m_Kicker = INT_UIF_Draw.Text(m_Left, 0, 118, 380, 18, "", 13, INT_PDA_Look.ORANGE);
		m_Title = INT_UIF_Draw.Text(m_Left, 0, 140, 400, 62, "", 54, INT_PDA_Look.TEXT);
		m_Title2 = INT_UIF_Draw.Text(m_Left, 0, 198, 400, 62, "", 54, INT_PDA_Look.TEXT);
		m_Desc = INT_PDA_Look.MText(m_Left, 0, 272, 380, 84, "", 17, INT_PDA_Look.DIM);
		Widget av = INT_UIF_Draw.Frame(m_Left, 0, 372, 200, 200, "INT_PDA_AuthAvatar");
		INT_UIF_Surface.Box(av, 0, 0, 200, 200, 6, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		m_BigAvatar = INT_PDA_Look.DrawAvatar(av, 6, 6, 188, m_Avatar);
		INT_UIF_Draw.Line(av, 6, 190, 188, 4, INT_PDA_Look.ORANGE);
		m_AvatarCap = INT_UIF_Draw.Text(m_Left, 220, 410, 180, 18, INT_PDA_Look.Caps("#STR_INT_PDA_AVATAR"), 13, INT_PDA_Look.DIM);
		m_AvatarNo = INT_UIF_Draw.Text(m_Left, 220, 432, 180, 40, "", 32, INT_PDA_Look.TEXT);
		INT_PDA_Look.DrawIcon(m_Left, 0, 604, 18, "warning", INT_PDA_Look.ORANGE);
		INT_PDA_Look.MText(m_Left, 28, 600, 360, 50, "#STR_INT_PDA_AUTH_NOTE", 14, INT_PDA_Look.DIM);
		// middle: the form
		m_Form = INT_UIF_Draw.Frame(m_Root, 500, 0, 450, m_H, "INT_PDA_AuthForm");
		float y = 34;
		INT_UIF_Draw.Text(m_Form, 0, y, 200, 18, INT_PDA_Look.Caps("#STR_INT_PDA_F_CALLSIGN"), 13, INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(m_Form, 200, y, 250, 18, "#STR_INT_PDA_F_CALLSIGN_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_Callsign = INT_UIF_TextInput.Create(m_Bag, m_Form, 0, y + 22, 450, 56, "", INT_PDA_Const.CALLSIGN_MAX);
		if (m_Callsign)
		{
			m_Callsign.OnChanged().Insert(OnCallsignChanged);
			m_Callsign.OnSubmit().Insert(OnSubmitText);
		}
		m_ErrorIcon = INT_PDA_Look.DrawIcon(m_Form, 0, y + 88, 18, "warning", INT_PDA_Look.ENEMY);
		m_Error = INT_UIF_Draw.Text(m_Form, 28, y + 84, 422, 26, "", 15, INT_PDA_Look.ENEMY);
		m_ErrorIcon.Show(false);
		y = y + 132;
		m_PassBlock = INT_UIF_Draw.Frame(m_Form, 0, y, 450, 80, "INT_PDA_AuthPass");
		INT_UIF_Draw.Text(m_PassBlock, 0, 0, 200, 18, INT_PDA_Look.Caps("#STR_INT_PDA_F_PASSWORD"), 13, INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(m_PassBlock, 200, 0, 250, 18, "#STR_INT_PDA_F_PASSWORD_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_Pass = INT_PDA_PassInput.Create(m_PassBlock, 0, 22, 450, 56);
		if (m_Pass)
			m_Pass.m_OnSubmit.Insert(OnPassSubmit);
		y = y + 100;
		m_DiscordBlock = INT_UIF_Draw.Frame(m_Form, 0, y, 450, 80, "INT_PDA_AuthDiscord");
		INT_UIF_Draw.Text(m_DiscordBlock, 0, 0, 200, 18, INT_PDA_Look.Caps("#STR_INT_PDA_F_DISCORD"), 13, INT_PDA_Look.DIM);
		INT_UIF_Draw.Text(m_DiscordBlock, 200, 0, 250, 18, "#STR_INT_PDA_F_DISCORD_HINT", 13, INT_PDA_Look.DIM2, INT_UIF_Align.RIGHT);
		m_Discord = INT_UIF_TextInput.Create(m_Bag, m_DiscordBlock, 0, 22, 450, 56, "", INT_PDA_Const.DISCORD_MAX);
		if (m_Discord)
			m_Discord.OnSubmit().Insert(OnSubmitText);
		y = y + 104;
		m_Save = INT_UIF_Checkbox.Create(m_Bag, m_Form, 0, y, 450, "#STR_INT_PDA_SAVE_PASSWORD", true, 30);
		y = y + 48;
		m_Submit = INT_UIF_Button.Create(m_Bag, m_Form, 0, y, 450, 66, "", INT_UIF_Variant.PRIMARY);
		if (m_Submit)
			m_Submit.OnClick().Insert(OnSubmitClick);
		y = y + 82;
		m_SwitchY = y;
		m_SwitchText = INT_UIF_Draw.Text(m_Form, 0, y, 450, 26, "", 17, INT_PDA_Look.DIM);
		m_SwitchLink = INT_UIF_Draw.Text(m_Form, 0, y, 450, 26, "", 17, INT_PDA_Look.ORANGE);
		m_SwitchHit = Hit(m_Form, 60, y - 4, 330, 34);
		INT_PDA_Look.DrawIcon(m_Form, 110, y + 46, 14, "lock", INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_Form, 132, y + 42, 320, 22, "#STR_INT_PDA_DATA_NOTE", 13, INT_PDA_Look.DIM2);
		// right: the avatar grid (registration only)
		BuildGrid();
	}

	protected void BuildGrid()
	{
		m_Grid = INT_UIF_Draw.Frame(m_Root, 1010, 0, 400, m_H, "INT_PDA_AuthGrid");
		INT_UIF_Draw.Text(m_Grid, 0, 34, 260, 18, INT_PDA_Look.Caps("#STR_INT_PDA_PICK_AVATAR"), 13, INT_PDA_Look.DIM);
		m_Counter = INT_UIF_Draw.Text(m_Grid, 260, 34, 140, 18, "", 13, INT_PDA_Look.ORANGE, INT_UIF_Align.RIGHT);
		m_Sel = INT_UIF_Draw.Frame(m_Grid, 0, 0, CELL + 6, CELL + 6, "INT_PDA_AvatarSel");
		INT_UIF_Surface.Box(m_Sel, 0, 0, CELL + 6, CELL + 6, 6, 0, INT_PDA_Look.ORANGE);
		int gcol;
		int grow;
		float cx;
		float cy;
		for (int i = 0; i < GRID_COLS * GRID_ROWS; i++)
		{
			gcol = i % GRID_COLS;
			grow = i / GRID_COLS;
			cx = gcol * (CELL + CELL_GAP);
			cy = 64 + grow * (CELL + CELL_GAP);
			INT_UIF_Draw.Image(m_Grid, cx, cy, CELL, CELL, "iuf_white", INT_PDA_Look.PANEL2);
			m_Cells.Insert(INT_PDA_Look.DrawAvatar(m_Grid, cx, cy, CELL, 0));
			m_CellHits.Insert(Hit(m_Grid, cx, cy, CELL, CELL));
		}
		float py = 64 + GRID_ROWS * (CELL + CELL_GAP) + 6;
		INT_PDA_Look.DrawIcon(m_Grid, 0, py, 26, "minus", INT_PDA_Look.TEXT);
		m_PrevHit = Hit(m_Grid, 0, py - 4, 40, 34);
		m_PageText = INT_UIF_Draw.Text(m_Grid, 40, py, 310, 26, "", 15, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		INT_PDA_Look.DrawIcon(m_Grid, 364, py, 26, "plus", INT_PDA_Look.TEXT);
		m_NextHit = Hit(m_Grid, 350, py - 4, 50, 34);
	}

	override void OnShow()
	{
		m_Register = !INT_PDA_Client.s_Registered;
		m_Busy = false;
		SetError("");
		if (m_Pass)
			m_Pass.SetRevealed(false);
		INT_PDA_SavedLogin saved = INT_PDA_Client.SavedLogin();
		if (m_Register)
		{
			m_Kicker.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_FIRST_START"));
			m_Title.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_REG_TITLE_1"));
			m_Title2.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_REG_TITLE_2"));
			m_Desc.SetText("#STR_INT_PDA_REG_DESC");
			m_Submit.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_REGISTER"));
			SetSwitchLine("#STR_INT_PDA_HAVE_ACCOUNT");
			m_Callsign.SetText("");
			if (m_Pass)
				m_Pass.SetText("");
		}
		else
		{
			m_Kicker.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_WELCOME_BACK"));
			m_Title.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_LOGIN_TITLE_1"));
			m_Title2.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_LOGIN_TITLE_2"));
			m_Desc.SetText("#STR_INT_PDA_LOGIN_DESC");
			m_Submit.SetText(INT_PDA_Look.Caps("#STR_INT_PDA_LOGIN"));
			SetSwitchLine("");
			m_Callsign.SetText(INT_PDA_Client.s_KnownCallsign);
			if (saved && saved.Password != "")
			{
				m_Callsign.SetText(saved.Callsign);
				m_Pass.SetText(saved.Password);
			}
		}
		// the logo is shown only while registering (owner 2026-10-23)
		m_Logo.Show(m_Register);
		m_Grid.Show(m_Register);
		m_DiscordBlock.Show(m_Register);
		// «Уже есть аккаунт? Войти» exists only on the registration form: no invisible click zone on the login form
		if (m_SwitchHit)
			m_SwitchHit.Show(m_Register);
		ShowAvatarPage(m_Avatar / (GRID_COLS * GRID_ROWS));
		RefreshAvatar();
		// login: the account's own portrait (the server sends it with WELCOME); hidden while unknown
		int known = INT_PDA_Client.s_KnownAvatar;
		m_ShownKnown = known;
		bool showAvatar = m_Register;
		if (!m_Register && known >= 0 && known < INT_PDA_Const.AVATARS)
		{
			showAvatar = true;
			m_BigAvatar.LoadImageFile(0, INT_PDA_Look.AvatarPath(known));
			m_AvatarNo.SetText("№ " + (known + 1).ToString());
		}
		Widget avatarBox = m_BigAvatar.GetParent();
		if (avatarBox)
			avatarBox.Show(showAvatar);
		m_AvatarNo.Show(showAvatar);
		m_AvatarCap.Show(showAvatar);
		Reveal(m_Left, 0.05);
		Reveal(m_Form, 0.12);
		if (m_Register)
			Reveal(m_Grid, 0.2);
		if (m_Callsign)
			m_Callsign.Focus();
	}

	// The PDA opened before the server's WELCOME: the form was built for «registration» (nothing known yet). When the
	// WELCOME says otherwise (an account exists / its portrait is known now) the form is built again.
	override void OnChanged(int what)
	{
		if (!m_Active || m_Busy || what != INT_PDA_Client.CH_WELCOME)
			return;
		bool wantRegister = !INT_PDA_Client.s_Registered;
		bool avatarNew = !m_Register && INT_PDA_Client.s_KnownAvatar != m_ShownKnown;
		if (wantRegister != m_Register || avatarNew)
			OnShow();
	}

	// --- avatar grid ----------------------------------------------------------------------------------------------
	protected void ShowAvatarPage(int page)
	{
		int per = GRID_COLS * GRID_ROWS;
		int pages = (INT_PDA_Const.AVATARS + per - 1) / per;
		m_GridPage = INT_PDA_Util.ClampI(page, 0, pages - 1);
		for (int i = 0; i < m_Cells.Count(); i++)
		{
			int idx = m_GridPage * per + i;
			bool show = idx < INT_PDA_Const.AVATARS;
			m_Cells[i].Show(show);
			m_CellHits[i].Show(show);
			if (show)
				m_Cells[i].LoadImageFile(0, INT_PDA_Look.AvatarPath(idx));
		}
		m_PageText.SetText((m_GridPage + 1).ToString() + " / " + pages.ToString());
		RefreshAvatar();
	}

	protected void RefreshAvatar()
	{
		m_BigAvatar.LoadImageFile(0, INT_PDA_Look.AvatarPath(m_Avatar));
		m_AvatarNo.SetText("№ " + (m_Avatar + 1).ToString());
		m_Counter.SetText((m_Avatar + 1).ToString() + " / " + INT_PDA_Const.AVATARS.ToString());
		int per = GRID_COLS * GRID_ROWS;
		int cell = m_Avatar - m_GridPage * per;
		bool onPage = cell >= 0 && cell < per;
		m_Sel.Show(onPage);
		if (!onPage)
			return;
		int col = cell % GRID_COLS;
		int row = cell / GRID_COLS;
		float cx = col * (CELL + CELL_GAP) - 3;
		float cy = 64 + row * (CELL + CELL_GAP) - 3;
		INT_UIF_Draw.Move(m_Sel, cx, cy);
	}

	// «Question? Link»: split after the last "?" (or the full-width "？" of zh / ja), both parts centred as one line.
	protected void SetSwitchLine(string key)
	{
		if (!m_SwitchText || !m_SwitchLink)
			return;
		string full = "";
		if (key != "")
			full = Widget.TranslateString(key);
		string ask = full;
		string link = "";
		int cut = full.LastIndexOf("?");
		int mark = 1;
		string wideMark = "？";
		int wide = full.LastIndexOf(wideMark);
		if (wide > cut)
		{
			cut = wide;
			mark = wideMark.Length();		// same units as LastIndexOf / Substring
		}
		if (cut >= 0)
		{
			ask = full.Substring(0, cut + mark);
			link = full.Substring(cut + mark, full.Length() - cut - mark);
			link.TrimInPlace();
		}
		float aw = INT_PDA_Look.TextW(ask, 17);
		float lw = 0;
		float gap = 0;
		if (link != "")
		{
			lw = INT_PDA_Look.TextW(link, 17);
			gap = 6;
		}
		float x = (450 - aw - gap - lw) * 0.5;
		INT_UIF_Draw.Place(m_SwitchText, x, m_SwitchY, aw + 4, 26);
		INT_UIF_Draw.Place(m_SwitchLink, x + aw + gap, m_SwitchY, lw + 4, 26);
		m_SwitchText.SetText(ask);
		m_SwitchLink.SetText(link);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_SwitchHit)
		{
			INT_PDA_Sfx.Play("Click");
			INT_PDA_Client.s_Registered = !m_Register;
			OnShow();
			return true;
		}
		if (w == m_PrevHit)
		{
			INT_PDA_Sfx.Play("Click");
			ShowAvatarPage(m_GridPage - 1);
			return true;
		}
		if (w == m_NextHit)
		{
			INT_PDA_Sfx.Play("Click");
			ShowAvatarPage(m_GridPage + 1);
			return true;
		}
		for (int i = 0; i < m_CellHits.Count(); i++)
		{
			if (w == m_CellHits[i])
			{
				m_Avatar = m_GridPage * GRID_COLS * GRID_ROWS + i;
				INT_PDA_Sfx.Play("Toggle");
				RefreshAvatar();
				m_BigAvatar.SetAlpha(0.35);
				INT_UIF_Animator.Fade(m_BigAvatar, 1, 0.3);
				return true;
			}
		}
		return false;
	}

	// --- form -----------------------------------------------------------------------------------------------------
	protected void OnCallsignChanged(string text, INT_UIF_TextInput t)
	{
		INT_PDA_Sfx.Play("Key");
		if (!m_Register || text == "")
		{
			SetError("");
			return;
		}
		if (!INT_PDA_Util.IsCallsign(text))
			SetError("#STR_INT_PDA_R_CALLSIGN_FORMAT");
		else
			SetError("");
	}

	protected void SetError(string key)
	{
		m_Error.SetText(key);
		m_ErrorIcon.Show(key != "");
	}

	protected void OnSubmitText(string text, INT_UIF_TextInput t)
	{
		Submit();
	}

	protected void OnPassSubmit(INT_PDA_PassInput p)
	{
		Submit();
	}

	protected void OnSubmitClick(INT_UIF_Button b)
	{
		Submit();
	}

	protected void Submit()
	{
		if (m_Busy)
			return;
		string callsign = m_Callsign.GetText();
		callsign.TrimInPlace();
		string pass = m_Pass.GetText();
		if (!INT_PDA_Util.IsCallsign(callsign))
		{
			SetError("#STR_INT_PDA_R_CALLSIGN_FORMAT");
			INT_PDA_Sfx.Play("Error");
			return;
		}
		if (!INT_PDA_Util.IsPassword(pass))
		{
			SetError("#STR_INT_PDA_R_PASSWORD_FORMAT");
			INT_PDA_Sfx.Play("Error");
			return;
		}
		m_Busy = true;
		SetError("");
		INT_PDA_Sfx.Play("Click");
		INT_PDA pda = m_Menu.m_Pda;
		string discord = "";
		if (m_Register)
		{
			discord = m_Discord.GetText();
			discord.TrimInPlace();
			if (discord == "")
			{
				m_Busy = false;
				SetError("#STR_INT_PDA_R_DISCORD_FORMAT");
				INT_PDA_Sfx.Play("Error");
				return;
			}
		}
		// saved for the next connect only if the server accepts it (INT_PDA_Client.CommitLogin)
		INT_PDA_Client.StageLogin(callsign, pass, m_Save.IsChecked());
		if (m_Register)
			INT_PDA_Client.SendRegister(callsign, pass, discord, m_Avatar, pda);
		else
			INT_PDA_Client.SendLogin(callsign, pass, pda);
	}

	override void OnResult(int requestOp, int code)
	{
		// the server refused the request itself before the auth (R_TOO_FAST comes as OP_RESULT with the request's op):
		// the form must not stay locked
		if (requestOp == INT_PDA_Const.OP_LOGIN || requestOp == INT_PDA_Const.OP_REGISTER)
		{
			m_Busy = false;
			if (code != INT_PDA_Const.R_OK)
			{
				INT_PDA_Sfx.Play("Error");
				SetError(INT_PDA_Look.ResultKey(code));
			}
			return;
		}
		if (requestOp != INT_PDA_Const.OP_AUTH_RESULT)
			return;
		m_Busy = false;
		if (code == INT_PDA_Const.R_OK)
		{
			INT_PDA_Sfx.Play("LoginOk");
			return;
		}
		INT_PDA_Sfx.Play("Error");
		SetError(INT_PDA_Look.ResultKey(code));
		// a refused password is cleared (the box shows only stars: typing after the old text made a longer password)
		if (!m_Register && code == INT_PDA_Const.R_WRONG_LOGIN && m_Pass)
		{
			m_Pass.SetText("");
			m_Pass.Focus();
		}
		if (code == INT_PDA_Const.R_ALREADY_REGISTERED || code == INT_PDA_Const.R_NOT_REGISTERED)
		{
			INT_PDA_Client.s_Registered = code == INT_PDA_Const.R_ALREADY_REGISTERED;
			OnShow();
			SetError(INT_PDA_Look.ResultKey(code));
		}
	}

	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("auth ") != 0)
			return;
		array<string> parts = new array<string>();
		cmd.Split(" ", parts);
		if (parts.Count() >= 3)
		{
			m_Callsign.SetText(parts[1]);
			m_Pass.SetText(parts[2]);
			if (parts.Count() >= 4 && m_Discord)
				m_Discord.SetText(parts[3]);
			Submit();
		}
	}
}
