// ============================================================================
// INT_UIFramework - in-game self-test (client, opt-in).
//
// Enabled by a marker file in the client profile:
//     <profile>/int_uif_selftest.txt
// Its first line picks what stays on screen afterwards: "keep" leaves the demo
// menu open (for a screenshot), anything else closes it. Words after "keep"
// go to the menu's OnTestCommand, e.g. "keep forms" shows the forms tab and
// "keep forms popup" also opens its dropdown.
//
// Every check prints one line to the script log:
//     [INT_UIF] [TEST] PASS <name>
//     [INT_UIF][ERROR] [TEST] FAIL <name> <detail>
// and the run ends with "[INT_UIF] [TEST] DONE pass=<n> fail=<n>".
// Structure is what is checked (roots, parents, visibility, strays), because
// that is what a script can see; how it LOOKS is the screenshot's job.
// ============================================================================

class INT_UIF_SelfTest
{
	static const string MARKER = "$profile:int_uif_selftest.txt";

	protected static ref INT_UIF_SelfTest s_Instance;
	protected static bool s_Checked;

	protected int m_Pass;
	protected int m_Fail;
	protected bool m_Keep;
	protected string m_Command;
	protected int m_Events;
	protected ref INT_UIF_Bag m_Bag;
	protected Widget m_Host;

	// Called every frame from MissionGameplay.OnUpdate; runs at most once.
	static void RunIfRequested()
	{
		if (s_Checked)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer() || !GetGame().GetWorkspace() || !GetGame().GetPlayer())
			return;
		s_Checked = true;
		if (!FileExist(MARKER))
			return;
		s_Instance = new INT_UIF_SelfTest();
		s_Instance.Start();
	}

	void Start()
	{
		string line = ReadMarker();
		m_Keep = line.IndexOf("keep") == 0;
		m_Command = "";
		if (m_Keep)
		{
			string rest = line.Substring(4, line.Length() - 4);
			m_Command = rest.Trim();
		}
		INT_UIF_Log.Info("[TEST] START keep=" + m_Keep.ToString() + " command='" + m_Command + "'");
		RunLayoutPaths();
		RunComponents();
		RunMenu();
	}

	protected string ReadMarker()
	{
		FileHandle fh = OpenFile(MARKER, FileMode.READ);
		if (fh == 0)
			return "";
		string line = "";
		FGets(fh, line);
		CloseFile(fh);
		line.TrimInPlace();
		return line;
	}

	protected void Check(bool ok, string name, string detail = "")
	{
		if (ok)
		{
			m_Pass++;
			INT_UIF_Log.Info("[TEST] PASS " + name);
		}
		else
		{
			m_Fail++;
			INT_UIF_Log.Error("[TEST] FAIL " + name + " " + detail);
		}
	}

	// --- helpers -------------------------------------------------------------------
	protected int TopLevelCount()
	{
		int n = 0;
		Widget w = GetGame().GetWorkspace().GetChildren();
		while (w)
		{
			n++;
			w = w.GetSibling();
		}
		return n;
	}

	// Top-level widgets carrying a component root name: each one is a stray.
	protected int StrayCount()
	{
		int n = 0;
		Widget w = GetGame().GetWorkspace().GetChildren();
		while (w)
		{
			string nm = w.GetName();
			if (w != m_Host && (nm == "IufCard" || nm == "IufButton" || nm == "IufTabs" || nm == "IufList" || nm == "Slot" || nm == "Label" || nm == "IufBadge" || nm == "Frame" || nm == "Surface" || nm == "IufRing" || nm == "IufDropdownPopup"))
				n++;
			w = w.GetSibling();
		}
		return n;
	}

	protected int CountNamed(Widget root, string name)
	{
		if (!root)
			return 0;
		int n = 0;
		if (root.GetName() == name)
			n++;
		Widget c = root.GetChildren();
		while (c)
		{
			n += CountNamed(c, name);
			c = c.GetSibling();
		}
		return n;
	}

	protected void CheckComp(INT_UIF_Component c, string rootName, string label, Widget parent)
	{
		if (!c || !c.GetRoot())
		{
			Check(false, label + ".root", "component or root is null");
			return;
		}
		Widget root = c.GetRoot();
		Check(root.GetName() == rootName, label + ".root", "expected " + rootName + ", got " + root.GetName());
		Check(root.GetParent() == parent, label + ".parent", "parent=" + INT_UIF_Widgets.NameOf(root.GetParent()));
	}

	// --- layout path experiment ---------------------------------------------------------
	// Loads every framework layout 4x (4 orders) through three path kinds and
	// counts roots that come back wrong: 0 = runtime-built path, 1 = literal
	// returned by a function, 2 = literal inside the CreateWidgets call.
	protected int LoadRound(int kind, int stride, Widget host)
	{
		int bad = 0;
		int n = INT_UIF_LayoutTable.Count();
		WorkspaceWidget ws = GetGame().GetWorkspace();
		for (int k = 0; k < n; k++)
		{
			int idx = (k * stride) % n;
			string nm = INT_UIF_LayoutTable.NameAt(idx);
			Widget w = null;
			if (kind == 0)
			{
				string built = INT_UIF_LAYOUTS + nm;
				w = ws.CreateWidgets(built, host);
			}
			else if (kind == 1)
			{
				w = ws.CreateWidgets(INT_UIF_LayoutTable.LiteralPath(nm), host);
			}
			else
			{
				w = INT_UIF_LayoutTable.Create(nm, host);
			}
			string got = INT_UIF_Widgets.NameOf(w);
			if (got != INT_UIF_LayoutTable.RootName(nm))
			{
				bad++;
				INT_UIF_Log.Info("[TEST] path kind=" + kind.ToString() + " " + nm + " -> " + got);
			}
			if (w)
				w.Unlink();
		}
		return bad;
	}

	protected void RunLayoutPaths()
	{
		Widget host = INT_UIF_Widgets.Create("slot.layout", null);
		if (!host)
		{
			Check(false, "paths.host");
			return;
		}
		host.Show(false);
		array<int> strides = {1, 22, 5, 7};
		array<int> kinds = {0, 1, 2, 0};
		array<int> bad = {0, 0, 0};
		for (int r = 0; r < kinds.Count(); r++)
		{
			int kind = kinds.Get(r);
			for (int s = 0; s < strides.Count(); s++)
			{
				int b = LoadRound(kind, strides.Get(s), host);
				bad.Set(kind, bad.Get(kind) + b);
			}
		}
		host.Unlink();
		int runtimeBad = bad.Get(0);
		int funcBad = bad.Get(1);
		int literalBad = bad.Get(2);
		INT_UIF_Log.Info("[TEST] paths runtime=" + runtimeBad.ToString() + " func=" + funcBad.ToString() + " literal=" + literalBad.ToString());
		Check(literalBad == 0, "paths.literal_exact", "wrong roots=" + literalBad.ToString());
	}

	// --- component suite ------------------------------------------------------------
	protected void RunComponents()
	{
		int topBefore = TopLevelCount();
		m_Bag = new INT_UIF_Bag();
		m_Host = INT_UIF_Widgets.Create("slot.layout", null);
		Check(m_Host != null, "host.create");
		if (!m_Host)
			return;
		int topNow = TopLevelCount();
		Check(topNow == topBefore + 1, "host.toplevel", "count=" + topNow.ToString());

		// v2 components: exact design-px rects, parent = the host itself.
		INT_UIF_Button btn = INT_UIF_Button.Create(m_Bag, m_Host, 0, 0, 140, 38, "Test", INT_UIF_Variant.PRIMARY, "check");
		CheckComp(btn, "IufButton", "button", m_Host);
		CheckComp(INT_UIF_IconButton.Create(m_Bag, m_Host, 0, 0, 32, "gear", INT_UIF_Variant.SECONDARY, ""), "IufIconButton", "iconbutton", m_Host);
		CheckComp(INT_UIF_Badge.Create(m_Bag, m_Host, 0, 0, "OK", INT_UIF_Variant.SUCCESS), "IufBadge", "badge", m_Host);
		INT_UIF_Card card = INT_UIF_Card.Create(m_Bag, m_Host, 0, 0, 320, 220, "CAPTION", "users", "Title");
		CheckComp(card, "IufCard", "card", m_Host);
		if (card)
			Check(card.GetContent() != null && INT_UIF_Widgets.IsDescendant(card.GetContent(), card.GetRoot()), "card.content");
		CheckComp(INT_UIF_Divider.Create(m_Bag, m_Host, 0, 0, 100), "IufDivider", "divider", m_Host);
		CheckComp(INT_UIF_ProgressBar.Create(m_Bag, m_Host, 0, 0, 200, 8, 0.5, "teal"), "IufProgress", "progress", m_Host);
		INT_UIF_Ring ring = INT_UIF_Ring.Create(m_Bag, m_Host, 0, 0, 100, "teal");
		CheckComp(ring, "IufRing", "ring", m_Host);
		if (ring)
		{
			Widget arc = ring.GetRoot().FindAnyWidget("Arc");
			Check(ImageWidget.Cast(arc) != null, "ring.arc");
		}
		CheckComp(INT_UIF_Callout.Create(m_Bag, m_Host, 0, 0, 300, 86, "Title", "Body text"), "IufCallout", "callout", m_Host);
		CheckComp(INT_UIF_StatTile.Create(m_Bag, m_Host, 0, 0, 180, 76, "Label", "42"), "IufStatTile", "stattile", m_Host);

		// Text measurement vs the width estimate (calibration log, not a check).
		TextWidget probe = INT_UIF_Draw.Text(m_Host, 0, 0, 600, 30, "Защита территории Territory", 14, ARGB(255, 255, 255, 255));
		if (probe)
		{
			int mw;
			int mh;
			probe.GetTextSize(mw, mh);
			float est = INT_UIF_Draw.S(INT_UIF_Draw.TextWidth("Защита территории Territory", 14));
			INT_UIF_Log.Info("[TEST] text width measured=" + mw.ToString() + " estimate=" + est.ToString());
		}

		RunForms(btn);

		INT_UIF_List lst = INT_UIF_List.Create(m_Bag, m_Host, 0, 0, 420, 300, "Empty");
		CheckComp(lst, "IufList", "list", m_Host);
		if (lst)
		{
			int r0 = lst.AddRow("Alpha", "first", 0);
			int r1 = lst.AddRow("Bravo", "second", 1);
			lst.AddRow("Charlie", "", 2);
			lst.SetRowAvatar(r0, "A", "teal");
			lst.SetRowIcon(r1, "user", "blue");
			lst.SetRowBadge(r0, "Owner", INT_UIF_Variant.WARNING, "crown");
			lst.AddRowAction(r0, "remove", "user_minus", INT_UIF_Variant.DANGER, "");
			lst.SetRowButton(r1, "add", "Add", INT_UIF_Variant.SUCCESS, "plus", 100);
			int rows = lst.Count();
			Check(rows == 3, "list.rows", "count=" + rows.ToString());
			Check(lst.GetData(1) == 1, "list.data");
			Check(!lst.IsActionsShown(r0), "list.actions_hidden");
			lst.PinHover(r0);
			Check(lst.IsActionsShown(r0), "list.actions_on_hover");
			lst.PinHover(-1);
		}

		Check(INT_UIF_Widgets.Slot(null, 0, 0, 1, 1) == null, "slot.null_parent");
		int strays = StrayCount();
		Check(strays == 0, "components.no_strays", "strays=" + strays.ToString());

		m_Bag.Clear();
		m_Host.Unlink();
		m_Host = null;
		int topAfter = TopLevelCount();
		Check(topAfter == topBefore, "components.cleanup", "count=" + topAfter.ToString() + " expected=" + topBefore.ToString());
	}

	// --- forms suite: v2 inputs, structure + behaviour --------------------------------
	void OnFormEvent()
	{
		m_Events++;
	}

	void OnToggleEvt(bool on, INT_UIF_Toggle t)
	{
		OnFormEvent();
	}

	void OnInputEvt(string text, INT_UIF_TextInput t)
	{
		OnFormEvent();
	}

	void OnDropdownEvt(int index, string text, INT_UIF_Dropdown d)
	{
		OnFormEvent();
	}

	void OnTabsEvt(int index, INT_UIF_Tabs t)
	{
		OnFormEvent();
	}

	protected void RunForms(INT_UIF_Button anchor)
	{
		INT_UIF_Toggle tg = INT_UIF_Toggle.Create(m_Bag, m_Host, 0, 0, 320, "Toggle", true, "Sub line");
		CheckComp(tg, "IufToggle", "toggle", m_Host);
		if (tg)
		{
			m_Events = 0;
			tg.OnChanged().Insert(OnToggleEvt);
			tg.SetOn(false, true, false);
			Check(!tg.IsOn() && m_Events == 1, "toggle.set_notify", "events=" + m_Events.ToString());
			tg.SetOn(true, false, false);
			Check(tg.IsOn() && m_Events == 1, "toggle.set_silent", "events=" + m_Events.ToString());
		}

		INT_UIF_Checkbox cb = INT_UIF_Checkbox.Create(m_Bag, m_Host, 0, 0, 240, "Check", false);
		CheckComp(cb, "IufCheckbox", "checkbox", m_Host);
		if (cb)
		{
			cb.SetChecked(true, false, false);
			Check(cb.IsChecked(), "checkbox.set");
		}

		INT_UIF_TextInput ti = INT_UIF_TextInput.Create(m_Bag, m_Host, 0, 0, 300, 40, "Type", 5, "search", "F");
		CheckComp(ti, "IufTextInput", "textinput", m_Host);
		if (ti)
		{
			Widget editW = ti.GetRoot().FindAnyWidget("Edit");
			Check(EditBoxWidget.Cast(editW) != null, "textinput.edit");
			// UTF-8: five letters, not five bytes.
			ti.SetText("Привет мир");
			string typed = ti.GetText();
			Check(typed == "Приве", "textinput.max_utf8", "text='" + typed + "'");
			m_Events = 0;
			ti.OnSubmit().Insert(OnInputEvt);
			ti.OnKeyPress(editW, 0, 0, KeyCode.KC_RETURN);
			Check(m_Events == 1, "textinput.submit", "events=" + m_Events.ToString());
		}

		INT_UIF_Slider sl = INT_UIF_Slider.Create(m_Bag, m_Host, 0, 0, 300, 0, 100, 50, 5, "Label");
		CheckComp(sl, "IufSlider", "slider", m_Host);
		if (sl)
		{
			sl.SetValue(142, false, false);
			float vMax = sl.GetValue();
			sl.SetValue(-7, false, false);
			float vMin = sl.GetValue();
			sl.SetValue(37, false, false);
			float vStep = sl.GetValue();
			Check(vMax == 100 && vMin == 0, "slider.clamp", "max=" + vMax.ToString() + " min=" + vMin.ToString());
			Check(vStep == 35, "slider.step", "value=" + vStep.ToString());
			sl.SetFormat(0, "%");
			string shown = sl.FormatValue(vStep);
			Check(shown == "35%", "slider.format", "text=" + shown);
		}

		INT_UIF_Stepper st = INT_UIF_Stepper.Create(m_Bag, m_Host, 0, 0, 160, 0, 10, 9, 2);
		CheckComp(st, "IufStepper", "stepper", m_Host);
		if (st)
		{
			st.OnPlus(null);
			float stv = st.GetValue();
			Check(stv == 10, "stepper.clamp", "value=" + stv.ToString());
		}

		array<string> opts = new array<string>();
		opts.Insert("Alpha");
		opts.Insert("Bravo");
		opts.Insert("Charlie");
		opts.Insert("Delta");
		opts.Insert("Echo");
		opts.Insert("Foxtrot");
		opts.Insert("Golf");
		opts.Insert("Hotel");
		opts.Insert("India");
		INT_UIF_Dropdown dd = INT_UIF_Dropdown.Create(m_Bag, m_Host, 0, 0, 260, opts, 1, "Pick");
		CheckComp(dd, "IufDropdown", "dropdown", m_Host);
		if (dd)
		{
			Check(dd.GetSelectedText() == "Bravo", "dropdown.initial", "text=" + dd.GetSelectedText());
			dd.OpenPopup();
			int popRows = dd.GetPopupRowCount();
			Check(dd.IsOpen() && dd.GetPopup() != null && popRows == 9, "dropdown.open", "rows=" + popRows.ToString());
			Check(dd.GetPopupFirstRow() == 0 && dd.IsPopupRowShown(0) && !dd.IsPopupRowShown(8), "dropdown.first_page");
			dd.ScrollPopupTo(1);
			int firstRow = dd.GetPopupFirstRow();
			Check(firstRow == 2 && !dd.IsPopupRowShown(0) && dd.IsPopupRowShown(8), "dropdown.scroll", "first=" + firstRow.ToString());
			m_Events = 0;
			dd.OnSelected().Insert(OnDropdownEvt);
			dd.Pick(3);
			Check(!dd.IsOpen() && dd.GetPopup() == null, "dropdown.closed_on_pick");
			int picked = dd.GetSelected();
			Check(picked == 3 && m_Events == 1, "dropdown.pick", "sel=" + picked.ToString() + " events=" + m_Events.ToString());
			dd.OpenPopup();
			bool closedOne = INT_UIF_Dropdown.CloseActive();
			Check(closedOne && !dd.IsOpen(), "dropdown.close_active");
			Check(!INT_UIF_Dropdown.CloseActive(), "dropdown.close_active_none");
		}

		Widget pageHost = INT_UIF_Draw.Frame(m_Host, 0, 0, 600, 300, "PageHost");
		INT_UIF_Tabs tabs = INT_UIF_Tabs.Create(m_Bag, m_Host, 0, 0, 360, 40);
		CheckComp(tabs, "IufTabs", "tabs", m_Host);
		if (tabs && pageHost)
		{
			tabs.SetPageArea(pageHost, 0, 50, 600, 250);
			tabs.AddTab("One", "home");
			tabs.AddTab("Two", "list", 3);
			tabs.AddTab("Three");
			Widget p0 = tabs.GetPage(0);
			Widget p1 = tabs.GetPage(1);
			Widget p2 = tabs.GetPage(2);
			Check(p0 != null && p1 != null && p2 != null, "tabs.pages", "a page is null");
			if (p0 && p1 && p2)
			{
				Check(p0.GetParent() == pageHost, "tabs.page_parent", "parent=" + INT_UIF_Widgets.NameOf(p0.GetParent()));
				Check(p0.IsVisible() && !p1.IsVisible() && !p2.IsVisible(), "tabs.only_active_visible");
				m_Events = 0;
				tabs.OnChanged().Insert(OnTabsEvt);
				tabs.Select(2, true, false);
				Check(!p0.IsVisible() && p2.IsVisible() && tabs.GetActive() == 2 && m_Events == 1, "tabs.select", "events=" + m_Events.ToString());
				tabs.SetCount(1, 12);
				Check(tabs.Count() == 3 && tabs.GetActive() == 2, "tabs.count_chip");
			}
		}
		INT_UIF_Tabs under = INT_UIF_Tabs.Create(m_Bag, m_Host, 0, 0, 500, 40, INT_UIF_TabStyle.UNDERLINE);
		CheckComp(under, "IufTabs", "tabs_underline", m_Host);
		if (under)
		{
			under.AddTab("A");
			under.AddTab("B", "", 2);
			Check(under.Count() == 2 && under.GetActive() == 0 && under.GetPage(0) == null, "tabs.underline");
		}

		if (anchor)
		{
			INT_UIF_Tooltip.ShowFor(anchor.GetRoot(), "Tip", 0);
			Check(INT_UIF_Tooltip.IsShown(), "tooltip.show");
			INT_UIF_Tooltip.HideTip();
			Check(!INT_UIF_Tooltip.IsShown(), "tooltip.hide");
			// The bubble is a top-level widget kept for reuse: drop it so the
			// cleanup check counts only what the components left behind.
			INT_UIF_Tooltip.Shutdown();
		}
	}

	// --- menu suite: the real engine path (EnterScriptedMenu -> Init) -----------------
	protected int m_MenuWaits;

	protected void RunMenu()
	{
		if (GetGame().GetUIManager().GetMenu())
		{
			// Another menu (pause menu on an unfocused start, a notice...) is
			// up: wait for it instead of failing.
			m_MenuWaits++;
			if (m_MenuWaits <= 20)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RunMenu, 1000, false);
				return;
			}
			Check(false, "menu.precondition", "another menu stayed open");
			Finish();
			return;
		}
		Check(INT_UIF_Menu.Open(INT_UIF_MENU_DEMO), "menu.open");
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(CheckMenu, 1000, false);
	}

	void CheckMenu()
	{
		INT_UIF_Menu m = INT_UIF_Menu.Current();
		Check(m != null, "menu.current");
		if (!m)
		{
			Finish();
			return;
		}
		int menuId = m.GetID();
		Check(menuId == INT_UIF_MENU_DEMO, "menu.id", "id=" + menuId.ToString());
		Widget content = m.GetContent();
		Check(content != null, "menu.content");
		if (content)
		{
			int nested = CountNamed(content, "IufMenu");
			int listsIn = CountNamed(content, "IufList");
			int cardsIn = CountNamed(content, "IufCard");
			int ringsIn = CountNamed(content, "IufRing");
			Check(nested == 0, "menu.no_nested_frame", "nested=" + nested.ToString());
			Check(listsIn >= 1, "menu.list_inside", "lists=" + listsIn.ToString());
			Check(cardsIn >= 3, "menu.cards_inside", "cards=" + cardsIn.ToString());
			Check(ringsIn >= 1, "menu.ring_inside", "rings=" + ringsIn.ToString());
		}
		int menuStrays = StrayCount();
		Check(menuStrays == 0, "menu.no_strays", "strays=" + menuStrays.ToString());
		int frames = 0;
		Widget w = GetGame().GetWorkspace().GetChildren();
		while (w)
		{
			if (w.GetName() == "IufMenu")
				frames++;
			w = w.GetSibling();
		}
		Check(frames == 1, "menu.single_frame", "frames=" + frames.ToString());
		CheckDialogResults();
		if (!m_Keep)
			m.CloseNow();
		else if (m_Command != "")
			m.OnTestCommand(m_Command);
		Finish();
	}

	// Confirm() buttons report their AddButton index: "yes" -> RESULT_YES and
	// "no" -> RESULT_NO. Until 2026-10-12 the button list was reversed and every
	// confirmation did the opposite of the button pressed.
	protected int m_DialogResult;

	protected void CheckDialogResults()
	{
		m_DialogResult = -100;
		INT_UIF_Dialog first = INT_UIF_Dialog.Confirm("Self-test", "Yes?", "#STR_INT_UIF_YES", "#STR_INT_UIF_NO");
		bool pressedYes = false;
		if (first)
		{
			first.OnResult().Insert(OnDialogResult);
			pressedYes = first.PressLabel("#STR_INT_UIF_YES");
		}
		Check(pressedYes && m_DialogResult == INT_UIF_Dialog.RESULT_YES, "dialog.yes_reports_yes", "result=" + m_DialogResult.ToString());

		m_DialogResult = -100;
		INT_UIF_Dialog second = INT_UIF_Dialog.Confirm("Self-test", "No?", "#STR_INT_UIF_YES", "#STR_INT_UIF_NO");
		bool pressedNo = false;
		if (second)
		{
			second.OnResult().Insert(OnDialogResult);
			pressedNo = second.PressLabel("#STR_INT_UIF_NO");
		}
		Check(pressedNo && m_DialogResult == INT_UIF_Dialog.RESULT_NO, "dialog.no_reports_no", "result=" + m_DialogResult.ToString());
	}

	void OnDialogResult(int result, INT_UIF_Dialog dialog)
	{
		m_DialogResult = result;
	}

	protected void Finish()
	{
		int mism = INT_UIF_Widgets.s_RootMismatches;
		Check(mism == 0, "widgets.no_root_mismatch", "mismatches=" + mism.ToString());
		INT_UIF_Log.Info("[TEST] DONE pass=" + m_Pass.ToString() + " fail=" + m_Fail.ToString());
	}
}
