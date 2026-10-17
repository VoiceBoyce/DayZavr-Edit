// ============================================================================
// INT_UIFramework - modal dialogs (v2).
//
//   INT_UIF_Dialog d = INT_UIF_Dialog.Confirm("Title", "Text");
//   d.OnResult().Insert(MyResultHandler);   // void MyResultHandler(int result, INT_UIF_Dialog dlg)
//
// Result = index of the pressed button in AddButton order (Confirm: 0 = No,
// 1 = Yes), or INT_UIF_DIALOG_CANCEL for ESC. With one of our menus open the
// dialog is an overlay inside it (input stays with that menu); otherwise the
// framework opens INT_UIF_DialogMenu to host it.
//
// Look: charcoal panel with hairline border and shadow, soft-tinted icon tile
// in the kind's colour, title, wrapping text, buttons right-aligned (the last
// one is the main action). The panel height follows the text.
// ============================================================================

class INT_UIF_Dialog extends INT_UIF_Component
{
	static const int RESULT_NO = 0;
	static const int RESULT_YES = 1;
	static const float WIDTH = 500;

	protected string m_TitleText;
	protected string m_BodyText;
	protected string m_IconName;
	protected int m_IconKind;
	protected ref array<string> m_Labels;
	protected ref array<int> m_Variants;
	protected ref array<INT_UIF_Button> m_ButtonList;
	protected INT_UIF_Menu m_Host;
	protected bool m_Finished;
	protected ref ScriptInvoker m_OnResult;

	protected ImageWidget m_Shade;
	protected ButtonWidget m_Blocker;
	protected Widget m_Panel;
	protected float m_PanelH;

	void INT_UIF_Dialog()
	{
		m_Labels = new array<string>();
		m_Variants = new array<int>();
		m_ButtonList = new array<INT_UIF_Button>();
		m_OnResult = new ScriptInvoker();
		m_IconKind = INT_UIF_ToastKind.INFO;
	}

	// --- factories ---------------------------------------------------------------
	static INT_UIF_Dialog Confirm(string title, string text, string yes = "#STR_INT_UIF_YES", string no = "#STR_INT_UIF_NO", int yesVariant = INT_UIF_Variant.PRIMARY)
	{
		INT_UIF_Dialog d = new INT_UIF_Dialog();
		int kind = INT_UIF_ToastKind.WARNING;
		if (yesVariant == INT_UIF_Variant.DANGER)
			kind = INT_UIF_ToastKind.ERROR;
		d.SetContent(title, text, KindIcon(kind), kind);
		d.AddButton(no, INT_UIF_Variant.SECONDARY);
		d.AddButton(yes, yesVariant);
		Present(d);
		return d;
	}

	static INT_UIF_Dialog Alert(string title, string text, int kind = INT_UIF_ToastKind.INFO, string ok = "#STR_INT_UIF_OK")
	{
		INT_UIF_Dialog d = new INT_UIF_Dialog();
		d.SetContent(title, text, KindIcon(kind), kind);
		d.AddButton(ok, INT_UIF_Variant.PRIMARY);
		Present(d);
		return d;
	}

	// Shows a dialog configured by hand (SetContent + AddButton).
	static void Present(INT_UIF_Dialog d)
	{
		if (!d || !GetGame() || GetGame().IsDedicatedServer())
			return;
		INT_UIF_Menu host = INT_UIF_Menu.Current();
		if (host && !host.IsClosingDown())
		{
			host.ShowModal(d);
			return;
		}
		// A menu in its close animation would destroy the dialog in OnHide:
		// finish closing it now and host the dialog in our own menu.
		if (host)
			host.CloseNow();
		INT_UIF_DialogMenu.s_Pending = d;
		if (!INT_UIF_Menu.Open(INT_UIF_MENU_DIALOG))
			INT_UIF_DialogMenu.s_Pending = null;
	}

	static string KindIcon(int kind)
	{
		if (kind == INT_UIF_ToastKind.SUCCESS)
			return "success";
		if (kind == INT_UIF_ToastKind.WARNING)
			return "warning";
		if (kind == INT_UIF_ToastKind.ERROR)
			return "error";
		return "info";
	}

	// --- configuration -------------------------------------------------------------
	void SetContent(string title, string text, string icon = "", int iconKind = INT_UIF_ToastKind.INFO)
	{
		m_TitleText = title;
		m_BodyText = text;
		m_IconName = icon;
		m_IconKind = iconKind;
	}

	void AddButton(string label, int variant = INT_UIF_Variant.SECONDARY)
	{
		m_Labels.Insert(label);
		m_Variants.Insert(variant);
	}

	// (int result, INT_UIF_Dialog dialog)
	ScriptInvoker OnResult()
	{
		return m_OnResult;
	}

	void SetHost(INT_UIF_Menu host)
	{
		m_Host = host;
	}

	bool IsFinished()
	{
		return m_Finished;
	}

	// --- build -------------------------------------------------------------------
	override string GetLayoutName()
	{
		return "dialog.layout";
	}

	protected float TextHeight()
	{
		string body = m_BodyText;
		if (m_BodyText.IndexOf("#") == 0)
			body = Widget.TranslateString(m_BodyText);
		float perLine = (WIDTH - 108) / (13 * 0.56);
		int lines = (int)Math.Ceil(body.LengthUtf8() / perLine);
		if (lines < 1)
			lines = 1;
		return lines * 19 + 6;
	}

	override void OnBuild()
	{
		m_Shade = INT_UIF_Widgets.FindImage(m_Root, "Shade");
		m_Blocker = INT_UIF_Widgets.FindButton(m_Root, "Blocker");
		m_Panel = INT_UIF_Widgets.Find(m_Root, "Panel");
		if (m_Blocker)
			m_Blocker.SetHandler(this);
		if (!m_Panel)
			return;

		INT_UIF_ThemeManager tm = INT_UIF_ThemeManager.Get();
		int hue = tm.ToastColor(m_IconKind);
		float textH = TextHeight();
		m_PanelH = 24 + 26 + 8 + textH + 24 + 38 + 22;
		if (m_PanelH < 168)
			m_PanelH = 168;
		float w = WIDTH;
		INT_UIF_Draw.Resize(m_Panel, w, m_PanelH);
		m_Panel.SetPos(0, 0);

		INT_UIF_Surface.Shadow(m_Panel, -48, -36, w + 96, m_PanelH + 96, tm.Color(INT_UIF_ThemeKeys.SHADOW));
		INT_UIF_Surface.Box(m_Panel, 0, 0, w, m_PanelH, 14, tm.Color(INT_UIF_ThemeKeys.WINDOW), tm.Color(INT_UIF_ThemeKeys.WINDOW_BORDER));
		INT_UIF_Draw.Image(m_Panel, 30, 0, w - 60, 1, "iuf_grad_band", ARGB(40, 255, 255, 255));
		INT_UIF_Draw.Image(m_Panel, -10, -12, 120, 120, "iuf_radial", INT_UIF_Color.WithAlpha(hue, 40));
		if (m_IconName != "")
			INT_UIF_Draw.IconTile(m_Panel, 24, 24, 44, m_IconName, hue, 12);
		TextWidget title = INT_UIF_Draw.Text(m_Panel, 84, 24, w - 108, 26, m_TitleText, 18, tm.Color(INT_UIF_ThemeKeys.TEXT));
		if (title)
			title.SetBold(true);
		INT_UIF_Draw.MText(m_Panel, 84, 58, w - 108, textH, m_BodyText, 13, tm.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		BuildButtons(w);
	}

	// Buttons right-aligned in AddButton order; width follows the label.
	protected void BuildButtons(float w)
	{
		int n = m_Labels.Count();
		float y = m_PanelH - 22 - 38;
		float x = w - 24;
		for (int i = n - 1; i >= 0; i--)
		{
			string label = m_Labels.Get(i);
			float bw = INT_UIF_Draw.TextWidth(label, 13) + 44;
			if (bw < 112)
				bw = 112;
			x = x - bw;
			INT_UIF_Button b = INT_UIF_Button.Create(m_Bag, m_Panel, x, y, bw, 38, label, m_Variants.Get(i));
			if (b)
				b.OnClick().Insert(OnButtonClick);
			// Built right to left: put each in front so that a button's list
			// index (= the result) is its AddButton index. Appending reversed
			// them: "Yes" of Confirm() reported RESULT_NO and "No" RESULT_YES.
			m_ButtonList.InsertAt(b, 0);
			x = x - 10;
		}
	}

	override void ApplyTheme()
	{
		// Light veil: the window behind stays readable (modern modal look).
		if (m_Shade)
			m_Shade.SetColor(ARGB(96, 4, 6, 8));
		if (m_Blocker)
			m_Blocker.SetColor(ARGB(0, 0, 0, 0));
	}

	// --- open / close ---------------------------------------------------------------
	void PlayOpen()
	{
		if (m_Shade)
		{
			m_Shade.SetColor(ARGB(96, 4, 6, 8));
			INT_UIF_Animator.FadeInColor(m_Shade, 0.2);
		}
		if (m_Panel)
			INT_UIF_Animator.Reveal(m_Panel, 0.02, 16, 0.32);
	}

	protected void OnButtonClick(INT_UIF_Button button)
	{
		int idx = m_ButtonList.Find(button);
		if (idx < 0)
			return;
		Finish(idx);
	}

	// Self-test: press the button showing `label` exactly as its click does (the
	// result is then whatever that button reports). False if none shows it.
	bool PressLabel(string label)
	{
		foreach (INT_UIF_Button b : m_ButtonList)
		{
			if (b && b.GetText() == label)
			{
				OnButtonClick(b);
				return true;
			}
		}
		return false;
	}

	// ESC / programmatic cancel.
	void Cancel()
	{
		Finish(INT_UIF_DIALOG_CANCEL);
	}

	void Finish(int result)
	{
		if (m_Finished)
			return;
		m_Finished = true;
		// The result handler may show another dialog: the host then destroys
		// this one and drops its ref. So the close animation starts first, user
		// code runs last, and the local ref keeps us alive until we return.
		INT_UIF_Dialog keep = this;
		INT_UIF_Tween t = null;
		if (m_Panel)
		{
			INT_UIF_Animator.Fade(m_Panel, 0, 0.14, INT_UIF_Ease.IN_QUAD);
			INT_UIF_Animator.MoveY(m_Panel, INT_UIF_Scale.Px(10), 0.16, INT_UIF_Ease.IN_QUAD);
		}
		if (m_Shade)
			t = INT_UIF_Animator.Fade(m_Shade, 0, 0.18, INT_UIF_Ease.IN_QUAD);
		if (t)
			t.OnComplete().Insert(OnCloseAnimDone);

		m_OnResult.Invoke(result, this);

		if (!t)
			Dismiss();
	}

	protected void OnCloseAnimDone(INT_UIF_Tween t)
	{
		Dismiss();
	}

	// Removes the dialog immediately. The host keeps its ref until the next
	// dialog or until it closes, so this object is never freed mid-call.
	void Dismiss()
	{
		m_Finished = true;
		Destroy();
		if (m_Host)
			m_Host.OnModalClosed(this);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		// The blocker swallows clicks behind the panel (modal).
		return w == m_Blocker;
	}
}

// Invisible host menu for dialogs opened while no framework menu is open.
class INT_UIF_DialogMenu extends INT_UIF_Menu
{
	static ref INT_UIF_Dialog s_Pending;

	override bool ShowWindow()
	{
		return false;
	}

	override bool UsesBlur()
	{
		return false;
	}

	override void OnBuild(Widget body)
	{
		if (!s_Pending)
			return;
		INT_UIF_Dialog d = s_Pending;
		s_Pending = null;
		ShowModal(d);
	}

	override void OnModalClosed(INT_UIF_Dialog dialog)
	{
		super.OnModalClosed(dialog);
		if (!HasModal())
			RequestClose();
	}
}
