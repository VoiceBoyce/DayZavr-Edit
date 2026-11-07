// INT_PDA (client) — a page of the PDA screen (one per tab, plus the login/registration page). The menu builds every
// page into the screen body (1456 x 674 design px) once, shows one at a time and forwards events to the active page.
// A page is the click handler of its own hit zones (INT_UIF_Draw.Hit); UIF components report through their invokers.
// Components go into the menu's bag (m_Bag); fields keep plain references (the bag owns them).

class INT_PDA_Page : ScriptedWidgetEventHandler
{
	INT_PDA_Menu m_Menu;
	INT_UIF_Bag m_Bag;
	Widget m_Root;
	float m_W;
	float m_H;
	bool m_Active;

	void Build(INT_PDA_Menu menu, Widget parent, float w, float h)
	{
		m_Menu = menu;
		m_Bag = menu.GetBag();
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "INT_PDA_Page");
		OnBuild();
		m_Root.Show(false);
	}

	// --- hooks ----------------------------------------------------------------------------------------------------
	void OnBuild() {}
	void OnShow() {}							// became the visible page (start reveals, ask the server for data)
	void OnHide() {}
	void OnChanged(int what) {}					// INT_PDA_Client.CH_*
	void OnResult(int requestOp, int code) {}
	void Update(float dt) {}					// every frame while visible
	bool HandleEscape() { return false; }		// true = the page used Esc (closed its own popup)
	void OnTestCommand(string cmd) {}
	void HidePreview() {}						// a 3D model draws above every 2D widget: hidden before the power-off

	// --- helpers --------------------------------------------------------------------------------------------------
	void Show(bool show)
	{
		if (!m_Root)
			return;
		m_Root.Show(show);
		m_Active = show;
		if (show)
			OnShow();
		else
			OnHide();
	}

	// A panel of style E: dark plate, hairline border, caption in caps (13 px, dim).
	Widget Panel(Widget parent, float x, float y, float w, float h, string caption = "")
	{
		Widget f = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_Panel");
		INT_UIF_Surface.Box(f, 0, 0, w, h, 6, INT_PDA_Look.PANEL, INT_PDA_Look.LINE);
		if (caption != "")
			INT_UIF_Draw.Text(f, 18, 12, w - 36, 18, INT_PDA_Look.Caps(caption), 13, INT_PDA_Look.DIM);
		return f;
	}

	TextWidget Label(Widget parent, float x, float y, float w, string key)
	{
		return INT_UIF_Draw.Text(parent, x, y, w, 18, INT_PDA_Look.Caps(key), 13, INT_PDA_Look.DIM);
	}

	ButtonWidget Hit(Widget parent, float x, float y, float w, float h)
	{
		return INT_UIF_Draw.Hit(parent, x, y, w, h, this);
	}

	void Reveal(Widget w, float delay)
	{
		if (w)
			INT_UIF_Animator.Reveal(w, delay, 14, 0.34);
	}

	void Toast(int code)
	{
		if (code == INT_PDA_Const.R_OK)
			INT_UIF_Toast.Success(INT_PDA_Look.ResultKey(code));
		else
			INT_UIF_Toast.Error(INT_PDA_Look.ResultKey(code));
	}

	// Module access (spec.md §5). The admin PDA (Alt+L) has every module.
	bool HasMod(string slot)
	{
		if (INT_PDA_Client.IsAdminMode())
			return true;
		if (!m_Menu || !m_Menu.m_Pda)
			return false;
		return m_Menu.m_Pda.HasModule(slot);
	}

	bool InFaction()
	{
		return INT_PDA_Client.s_Profile && INT_PDA_Client.s_Profile.FactionId != "";
	}

	// «No module / no access» plate in the middle of a page: icon, title, one line of text. Returns its frame
	// (hidden by the caller when access returns).
	Widget Locked(Widget parent, float w, float h, string icon, string titleKey, string textKey)
	{
		Widget f = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "INT_PDA_Locked");
		float cy = h * 0.5 - 90;
		INT_PDA_Look.DrawIcon(f, w * 0.5 - 32, cy, 64, icon, INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(f, 0, cy + 84, w, 30, INT_PDA_Look.Caps(titleKey), 22, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		INT_UIF_Draw.Text(f, 0, cy + 124, w, 24, textKey, 16, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		return f;
	}
}

// A small text-input overlay over the screen (rename, password, Steam ID, news…). One or two fields, OK / cancel.
// Built into the menu's overlay layer; the result goes to the callback (bool ok, string a, string b). Enter confirms
// (in the first of two fields it moves on while the second one is empty); TAB moves between the fields (the menu
// polls the key and calls FocusNext). The prompt's components live in its OWN bag, so they die with the prompt
// instead of piling up in the menu's bag. A callback usually closes the prompt (INT_PDA_Menu.ClosePrompt): the menu
// then only hides it and drops it on the next frame, so the object never dies inside its own call (the INT_UIF
// m_Modal way).
class INT_PDA_Prompt : ScriptedWidgetEventHandler
{
	Widget m_Root;
	ref ScriptInvoker m_OnDone = new ScriptInvoker();
	ref INT_UIF_Bag m_OwnBag = new INT_UIF_Bag();
	INT_UIF_TextInput m_A;
	INT_UIF_TextInput m_B;
	ref INT_PDA_PassInput m_PassA;		// own objects (not in a bag): kept by ref
	ref INT_PDA_PassInput m_PassB;
	INT_UIF_Button m_Ok;
	INT_UIF_Button m_Cancel;
	bool m_Done;

	// password: the fields hide the text. second = "" makes a one-field prompt.
	void Build(Widget layer, float layerW, float layerH, string title, string first, string second, bool password, string valueA = "")
	{
		INT_UIF_Bag bag = m_OwnBag;
		float w = 560;
		float h = 230;
		if (second != "")
			h = 310;
		m_Root = INT_UIF_Draw.Frame(layer, 0, 0, layerW, layerH, "INT_PDA_Prompt");
		INT_UIF_Draw.Image(m_Root, 0, 0, layerW, layerH, "iuf_white", ARGB(170, 0, 0, 0));
		float x = (layerW - w) * 0.5;
		float y = (layerH - h) * 0.5;
		Widget box = INT_UIF_Draw.Frame(m_Root, x, y, w, h, "INT_PDA_PromptBox");
		INT_UIF_Surface.Shadow(box, -24, -16, w + 48, h + 44, ARGB(160, 0, 0, 0));
		INT_UIF_Surface.Box(box, 0, 0, w, h, 8, INT_PDA_Look.PANEL, INT_PDA_Look.LINE2);
		INT_UIF_Draw.Line(box, 0, 0, w, 3, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(box, 28, 22, w - 56, 28, INT_PDA_Look.Caps(title), 20, INT_PDA_Look.TEXT);
		float fy = 70;
		INT_UIF_Draw.Text(box, 28, fy, w - 56, 18, INT_PDA_Look.Caps(first), 13, INT_PDA_Look.DIM);
		if (password)
			m_PassA = INT_PDA_PassInput.Create(box, 28, fy + 22, w - 56, 48);
		else
			m_A = INT_UIF_TextInput.Create(bag, box, 28, fy + 22, w - 56, 48, "", 60);
		if (m_A && valueA != "")
			m_A.SetText(valueA);
		if (second != "")
		{
			fy = fy + 80;
			INT_UIF_Draw.Text(box, 28, fy, w - 56, 18, INT_PDA_Look.Caps(second), 13, INT_PDA_Look.DIM);
			if (password)
				m_PassB = INT_PDA_PassInput.Create(box, 28, fy + 22, w - 56, 48);
			else
				m_B = INT_UIF_TextInput.Create(bag, box, 28, fy + 22, w - 56, 48, "", 60);
		}
		m_Cancel = INT_UIF_Button.Create(bag, box, w - 28 - 300, h - 64, 140, 44, "#STR_INT_PDA_CANCEL", INT_UIF_Variant.SECONDARY);
		m_Ok = INT_UIF_Button.Create(bag, box, w - 28 - 150, h - 64, 150, 44, "#STR_INT_PDA_OK", INT_UIF_Variant.PRIMARY, "check");
		if (m_Ok)
			m_Ok.OnClick().Insert(OnOk);
		if (m_Cancel)
			m_Cancel.OnClick().Insert(OnCancel);
		if (m_A)
			m_A.OnSubmit().Insert(OnSubmitA);
		if (m_B)
			m_B.OnSubmit().Insert(OnSubmitB);
		if (m_PassA)
			m_PassA.m_OnSubmit.Insert(OnPassSubmitA);
		if (m_PassB)
			m_PassB.m_OnSubmit.Insert(OnPassSubmitB);
		FocusA();
		INT_UIF_Animator.Reveal(box, 0, 12, 0.22);
	}

	bool HasSecond()
	{
		return m_B != null || m_PassB != null;
	}

	void FocusA()
	{
		if (m_PassA)
			m_PassA.Focus();
		else if (m_A)
			m_A.Focus();
	}

	void FocusB()
	{
		if (m_PassB)
			m_PassB.Focus();
		else if (m_B)
			m_B.Focus();
	}

	// TAB: first field <-> second field.
	void FocusNext()
	{
		if (m_Done || !HasSecond())
			return;
		Widget rootB = null;
		if (m_PassB)
			rootB = m_PassB.m_Root;
		else if (m_B)
			rootB = m_B.GetRoot();
		if (Holds(rootB))
			FocusA();
		else
			FocusB();
	}

	// The keyboard focus is inside `root`.
	protected bool Holds(Widget root)
	{
		if (!root)
			return false;
		Widget f = GetFocus();
		while (f)
		{
			if (f == root)
				return true;
			f = f.GetParent();
		}
		return false;
	}

	// Enter in the first field: on to the empty second field, else confirm.
	protected void SubmitFirst()
	{
		if (HasSecond() && B() == "")
		{
			FocusB();
			return;
		}
		Finish(true);
	}

	protected void OnSubmitA(string text, INT_UIF_TextInput t)
	{
		SubmitFirst();
	}

	protected void OnSubmitB(string text, INT_UIF_TextInput t)
	{
		Finish(true);
	}

	protected void OnPassSubmitA(INT_PDA_PassInput p)
	{
		SubmitFirst();
	}

	protected void OnPassSubmitB(INT_PDA_PassInput p)
	{
		Finish(true);
	}

	string A()
	{
		if (m_PassA)
			return m_PassA.GetText();
		if (m_A)
			return m_A.GetText();
		return "";
	}

	string B()
	{
		if (m_PassB)
			return m_PassB.GetText();
		if (m_B)
			return m_B.GetText();
		return "";
	}

	protected void OnOk(INT_UIF_Button b)
	{
		Finish(true);
	}

	protected void OnCancel(INT_UIF_Button b)
	{
		Finish(false);
	}

	void Finish(bool ok)
	{
		if (m_Done)
			return;
		m_Done = true;
		// the callback closes the prompt (the menu drops its ref): stay alive until this call returns
		INT_PDA_Prompt keep = this;
		INT_PDA_Sfx.Play("Click");
		string a = A();
		string b = B();
		if (m_Root)
			m_Root.Show(false);
		m_OnDone.Invoke(ok, a, b);
	}

	// Called by the menu a frame after the prompt was closed (never from inside the prompt's own events).
	void Destroy()
	{
		m_Done = true;
		m_OnDone.Clear();
		if (Holds(m_Root))
			SetFocus(null);
		m_OwnBag.Clear();
		m_A = null;
		m_B = null;
		m_Ok = null;
		m_Cancel = null;
		if (m_Root)
			m_Root.Unlink();
		m_Root = null;
	}
}

// Password field: a PasswordEditBoxWidget (own literal layout, hidden text) on a style-E plate. The EditBox font
// follows the box height, so the box is as tall as the glyphs and centred in the plate (the UIF Draw.Edit way).
class INT_PDA_PassInput : ScriptedWidgetEventHandler
{
	static const float FONT_H = 16;		// glyph height of the UIF text inputs next to it
	Widget m_Root;
	ref INT_UIF_Surface m_Box;
	ButtonWidget m_Hit;
	PasswordEditBoxWidget m_Edit;
	ImageWidget m_Eye;					// show / hide the typed text (concept E: the eye at the right of the field)
	ButtonWidget m_EyeHit;
	bool m_Revealed;
	ref ScriptInvoker m_OnSubmit = new ScriptInvoker();
	protected int m_LastSubmitMs = -100000;

	static INT_PDA_PassInput Create(Widget parent, float x, float y, float w, float h)
	{
		INT_PDA_PassInput p = new INT_PDA_PassInput();
		p.Build(parent, x, y, w, h);
		return p;
	}

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_Pass");
		// the same plate as the UIF text inputs next to it (tile fill, hairline border)
		m_Box = INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 8, INT_PDA_Look.PANEL2, INT_PDA_Look.LINE2);
		// a click anywhere on the plate focuses the edit box (the UIF TextInput way: a hit zone under the edit)
		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		Widget wdg = GetGame().GetWorkspace().CreateWidgets("INT_PDA_CL/gui/layouts/int_pda_pass.layout", m_Root);
		m_Edit = PasswordEditBoxWidget.Cast(wdg);
		if (!m_Edit)
			return;
		INT_UIF_Draw.Place(m_Edit, 16, (h - FONT_H) * 0.5, w - 72, FONT_H);
		m_Edit.SetHideText(true);
		m_Edit.SetTextColor(INT_PDA_Look.TEXT);
		m_Edit.SetHandler(this);
		m_Eye = INT_PDA_Look.DrawIcon(m_Root, w - 42, (h - 22) * 0.5, 22, "eye", INT_PDA_Look.DIM);
		m_EyeHit = INT_UIF_Draw.Hit(m_Root, w - 52, 0, 52, h, this);
	}

	void SetRevealed(bool on)
	{
		m_Revealed = on;
		if (m_Edit)
			m_Edit.SetHideText(!on);
		if (!m_Eye)
			return;
		string icon = "eye";
		int col = INT_PDA_Look.DIM;
		if (on)
		{
			icon = "eye_off";
			col = INT_PDA_Look.ORANGE;
		}
		m_Eye.LoadImageFile(0, INT_PDA_Look.IconPath(icon));
		m_Eye.SetColor(col);
	}

	string GetText()
	{
		if (!m_Edit)
			return "";
		return m_Edit.GetText();
	}

	void SetText(string text)
	{
		if (m_Edit)
			m_Edit.SetText(text);
	}

	void Focus()
	{
		if (m_Edit)
			SetFocus(m_Edit);
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			Submit();
			return true;
		}
		return false;
	}

	// Enter comes as OnChange(finished = true), its key press never arrives (as in INT_UIF_TextInput 2.0.3); with the
	// key up it is the focus moving away, which must not log in
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (finished)
		{
			if (KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0)
				Submit();
			return true;
		}
		INT_PDA_Sfx.Play("Key");
		return false;
	}

	protected void Submit()
	{
		int now = GetGame().GetTime();
		if (now - m_LastSubmitMs < INT_UIF_TextInput.SUBMIT_GAP_MS)
			return;
		m_LastSubmitMs = now;
		// the handler may close the prompt that owns this field: stay alive until the call returns
		INT_PDA_PassInput keep = this;
		m_OnSubmit.Invoke(this);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_EyeHit)
		{
			INT_PDA_Sfx.Play("Toggle");
			SetRevealed(!m_Revealed);
			Focus();
			return true;
		}
		if (w != m_Hit)
			return false;
		Focus();
		return true;
	}

	// orange border while the field has the keyboard (like the UIF inputs' focus ring)
	override bool OnFocus(Widget w, int x, int y)
	{
		if (m_Box)
			m_Box.FadeBorder(INT_PDA_Look.ORANGE, 0.12);
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (m_Box)
			m_Box.FadeBorder(INT_PDA_Look.LINE2, 0.12);
		return false;
	}
}
