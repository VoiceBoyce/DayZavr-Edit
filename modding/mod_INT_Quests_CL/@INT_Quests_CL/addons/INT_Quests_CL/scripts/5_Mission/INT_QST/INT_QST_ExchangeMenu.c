// INT Quests (client) — admin window «Экспорт и импорт» (design B «Полевой дневник», spec.md §7). Left page «Выгрузка»:
// the catalog by branch with checkboxes (the branch box ticks all its quests, a click on the branch opens it for
// single quests), an optional name and «Выгрузить N заданий» -> INT_QST_Admin.Export; the server writes
// Exports/<date>_<name>/<Branch>/ and answers with the folder. Right page «Загрузка»: the folders of Imports/
// (INT_QST_Admin.ImportList, asked again by «Обновить список»), the Id mode and «Загрузить» -> INT_QST_Admin.Import
// after a question; the server copies the files into Quests/ and reloads the catalog for everybody (the left page
// follows through INT_QST_Client.s_OnChanged). Widgets are made once; rows only change words and what is shown.

// --- left page: branches and quests with checkboxes ------------------------------------------------------------------
class INT_QST_XcRow
{
	bool m_Group;
	string m_Branch;
	int m_BranchIdx;
	int m_QuestId;
}

class INT_QST_XcSlot extends INT_QST_VSlot
{
	// branch row
	Widget m_GBox;
	Widget m_GHover;
	ImageWidget m_ChevOpen;
	ImageWidget m_ChevShut;
	ImageWidget m_GOn;
	ImageWidget m_GOff;
	Widget m_GPart;
	Widget m_GTickHover;
	ButtonWidget m_GTick;
	ref INT_QST_GvTwin m_GTitle;
	TextWidget m_GCount;
	// quest row
	Widget m_QBox;
	Widget m_QHover;
	Widget m_QMark;
	ImageWidget m_QOn;
	ImageWidget m_QOff;
	TextWidget m_QName;
	TextWidget m_QId;
}

class INT_QST_XcListSource extends INT_QST_VSource
{
	static const float GROUP_H = 46;
	static const float QUEST_H = 34;

	INT_QST_ExchangeMenu m_Owner;
	ref array<ref INT_QST_XcRow> m_Rows = new array<ref INT_QST_XcRow>();

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		if (m_Rows[i].m_Group)
			return GROUP_H;
		return QUEST_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_XcSlot();
	}

	// Both row kinds live in every slot; BindSlot shows one.
	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_XcSlot s = INT_QST_XcSlot.Cast(slot);
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		int olive = INT_QST_Look.Olive();
		int marker = 0x59F7CC40;
		// branch
		s.m_GBox = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, GROUP_H, "Branch");
		s.m_GHover = INT_UIF_Draw.Frame(s.m_GBox, 0, 2, w, GROUP_H - 4, "Hover");
		INT_UIF_Draw.Line(s.m_GHover, 0, 0, w, GROUP_H - 4, 0x1A2A2218);
		s.m_ChevOpen = INT_QST_Look.Icon(s.m_GBox, 8, 15, 16, "chevron_down", ink2);
		s.m_ChevShut = INT_QST_Look.Icon(s.m_GBox, 8, 15, 16, "chevron_right", ink2);
		Widget tick = INT_UIF_Draw.Frame(s.m_GBox, 30, 9, 28, 28, "Tick");
		s.m_GTickHover = INT_UIF_Draw.Frame(tick, 0, 0, 28, 28, "Hover");
		int th = INT_QST_Look.InkA(40);
		INT_UIF_Surface.Box(s.m_GTickHover, 0, 0, 28, 28, 4, 0x26FFFFFF, th);
		s.m_GOff = INT_QST_Look.Icon(tick, 3, 3, 22, "checkbox_empty", ink2);
		s.m_GOn = INT_QST_Look.Icon(tick, 3, 3, 22, "checkbox_checked", olive);
		s.m_GPart = INT_UIF_Draw.Line(tick, 10, 10, 8, 8, ink2);
		if (m_Owner)
			s.m_GTick = INT_UIF_Draw.Hit(tick, 0, 0, 28, 28, m_Owner.Clicks());
		s.m_GTitle = INT_QST_GvTwin.Make(s.m_GBox, 68, 9, w - 68 - 150, 28, 16, ink);
		s.m_GCount = INT_QST_Look.Text(s.m_GBox, w - 150, 9, 140, 28, "", 13, ink3, INT_UIF_Align.RIGHT);
		INT_QST_Look.Hair(s.m_GBox, 4, GROUP_H - 1, w - 8);
		// quest
		s.m_QBox = INT_UIF_Draw.Frame(s.m_Root, 0, 0, w, QUEST_H, "Quest");
		s.m_QMark = INT_UIF_Draw.Frame(s.m_QBox, 40, 2, w - 40, QUEST_H - 4, "Mark");
		INT_QST_Look.Tex(s.m_QMark, 0, 0, w - 40, QUEST_H - 4, "int_qst_highlight_ca", marker);
		s.m_QHover = INT_UIF_Draw.Frame(s.m_QBox, 40, 2, w - 40, QUEST_H - 4, "Hover");
		INT_UIF_Draw.Line(s.m_QHover, 0, 0, w - 40, QUEST_H - 4, 0x1A2A2218);
		s.m_QOff = INT_QST_Look.Icon(s.m_QBox, 52, 7, 20, "checkbox_empty", ink2);
		s.m_QOn = INT_QST_Look.Icon(s.m_QBox, 52, 7, 20, "checkbox_checked", olive);
		s.m_QName = INT_QST_Look.Text(s.m_QBox, 82, 0, w - 82 - 80, QUEST_H, "", 14, ink);
		s.m_QId = INT_QST_Look.Text(s.m_QBox, w - 80, 0, 72, QUEST_H, "", 12, ink3, INT_UIF_Align.RIGHT);
		INT_QST_Look.Hair(s.m_QBox, 48, QUEST_H - 1, w - 52);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_XcSlot s = INT_QST_XcSlot.Cast(slot);
		INT_QST_XcRow r = m_Rows[i];
		if (!r || !m_Owner)
			return;
		s.m_GBox.Show(r.m_Group);
		s.m_QBox.Show(!r.m_Group);
		if (r.m_Group)
		{
			BindBranch(s, r, w);
			return;
		}
		s.m_Hover = s.m_QHover;
		bool on = m_Owner.IsPicked(r.m_QuestId);
		s.m_QOn.Show(on);
		s.m_QOff.Show(!on);
		s.m_QMark.Show(on);
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(r.m_QuestId);
		string idText = "#" + r.m_QuestId.ToString();
		string nm = idText;
		if (q)
			nm = q.Name;
		float nameW = w - 82 - 86;
		s.m_QName.SetText(INT_QST_Look.Fit(nm, nameW, 14));
		s.m_QId.SetText(idText);
	}

	protected void BindBranch(INT_QST_XcSlot s, INT_QST_XcRow r, float w)
	{
		s.m_Hover = s.m_GHover;
		bool open = m_Owner.IsBranchOpen(r.m_Branch);
		s.m_ChevOpen.Show(open);
		s.m_ChevShut.Show(!open);
		int total = m_Owner.BranchSize(r.m_BranchIdx);
		int picked = m_Owner.PickedIn(r.m_BranchIdx);
		bool all = total > 0 && picked == total;
		s.m_GOn.Show(all);
		s.m_GOff.Show(!all);
		s.m_GPart.Show(picked > 0 && picked < total);
		string title = INT_QST_Client.s_Catalog.BranchTitle(r.m_Branch);
		float titleW = w - 68 - 156;
		string shown = INT_QST_Look.Fit(title, titleW, 16);
		s.m_GTitle.SetLine(shown);
		string pt = picked.ToString();
		string tt = total.ToString();
		s.m_GCount.SetText(pt + " / " + tt);
		if (s.m_GTick)
			m_Owner.Clicks().Bind(s.m_GTick, INT_QST_ExchangeMenu.ID_BRANCH_BOX, r.m_BranchIdx, s.m_GTickHover);
	}

	override void OnRowClick(int i, int button)
	{
		if (!m_Owner || i < 0 || i >= m_Rows.Count())
			return;
		INT_QST_XcRow r = m_Rows[i];
		if (r.m_Group)
			m_Owner.ToggleBranchOpen(r.m_Branch);
		else
			m_Owner.ToggleQuest(r.m_QuestId);
	}
}

// --- right page: folders of Imports/ ---------------------------------------------------------------------------------
class INT_QST_XcFolderSlot extends INT_QST_VSlot
{
	Widget m_Sel;
	ref INT_QST_GvTwin m_Name;
	TextWidget m_Info;
}

class INT_QST_XcFolderSource extends INT_QST_VSource
{
	static const float ROW_H = 50;

	INT_QST_ExchangeMenu m_Owner;

	override int Count()
	{
		if (!m_Owner)
			return 0;
		return m_Owner.FolderCount();
	}

	override float RowHeight(int i)
	{
		return ROW_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_XcFolderSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_XcFolderSlot s = INT_QST_XcFolderSlot.Cast(slot);
		float rh = ROW_H - 4;
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		int marker = INT_QST_Look.Marker();
		Widget hover = INT_UIF_Draw.Frame(s.m_Root, 0, 2, w, rh, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, rh, 0x1F2A2218);
		s.m_Hover = hover;
		s.m_Sel = INT_UIF_Draw.Frame(s.m_Root, 0, 2, w, rh, "Sel");
		INT_QST_Look.Tex(s.m_Sel, 0, 0, w, rh, "int_qst_highlight_ca", marker);
		INT_UIF_Draw.Line(s.m_Sel, 0, 0, 3, rh, ink);
		INT_QST_Look.Icon(s.m_Root, 14, 15, 20, "import", ink2);
		s.m_Name = INT_QST_GvTwin.Make(s.m_Root, 46, 4, w - 56, 24, 15, ink);
		s.m_Info = INT_QST_Look.Text(s.m_Root, 46, 26, w - 56, 20, "", 12, ink3);
		INT_QST_Look.Hair(s.m_Root, 4, ROW_H - 1, w - 8);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_XcFolderSlot s = INT_QST_XcFolderSlot.Cast(slot);
		if (!m_Owner)
			return;
		string name = m_Owner.FolderName(i);
		string info = m_Owner.FolderInfo(i);
		if (s.m_Sel)
			s.m_Sel.Show(m_Owner.SelectedFolder() == i);
		string shown = INT_QST_Look.Fit(name, w - 60, 15);
		s.m_Name.SetLine(shown);
		if (s.m_Info)
			s.m_Info.SetText(info);
	}

	override void OnRowClick(int i, int button)
	{
		if (m_Owner)
			m_Owner.PickFolder(i);
	}
}

// --- the window -------------------------------------------------------------------------------------------------------
class INT_QST_ExchangeMenu extends INT_QST_Book
{
	static const float FOLD = 700;

	static const int ID_BRANCH_BOX = 1;
	static const int ID_ALL = 2;
	static const int ID_NONE = 3;
	static const int ID_EXPORT = 4;
	static const int ID_REFRESH = 5;
	static const int ID_IMPORT = 6;
	static const int ID_TO_JOURNAL = 7;

	static const int ASK_IMPORT = 1;

	static int s_Mode;			// 0 = taken Ids get new ones, 1 = replace (kept between openings)
	static string s_Name;			// last export name
	static string s_LastExport;		// "Exports/<folder> · n" of the last export
	static string s_LastImport;

	protected Widget m_OutHead;
	protected Widget m_InHead;
	protected Widget m_ExportBox;
	protected TextWidget m_OutEmpty;
	protected TextWidget m_LastExportText;
	protected TextWidget m_FolderNote;
	protected TextWidget m_InResult;
	protected TextWidget m_Chosen;

	protected ref INT_QST_VList m_XList;
	protected ref INT_QST_XcListSource m_XSrc;
	protected ref INT_QST_VList m_FList;
	protected ref INT_QST_XcFolderSource m_FSrc;
	protected ref INT_QST_Field m_FName;
	protected ref INT_QST_Choice m_CMode;

	// catalog by branch (rebuilt on every catalog change), picks and open branches (kept)
	protected ref array<string> m_BranchKeys = new array<string>();
	protected ref array<ref array<int>> m_BranchIds = new array<ref array<int>>();
	protected ref map<int, bool> m_Picked = new map<int, bool>();
	protected ref map<string, bool> m_Open = new map<string, bool>();
	protected int m_BtnCount = -1;
	protected int m_HeadPicked = -1;
	protected int m_HeadTotal = -1;

	// Imports/ folders from the last list answer
	protected ref array<string> m_Folders = new array<string>();
	protected ref array<string> m_FolderInfo = new array<string>();
	protected int m_FolderSel = -1;
	protected bool m_ListKnown;
	protected float m_ListLeft;		// a list request is on its way
	protected string m_AskFolder;

	protected int m_WaitOp;			// export / import sent, answer not here yet
	protected float m_WaitLeft;
	protected bool m_Hooked;
	protected int m_SeenShape = -1;		// INT_QST_Client.s_ShapeRevision of the last refresh

	override void GetWindowSize(out float w, out float h)
	{
		w = 1440;
		h = 840;
	}

	override float CoverTop()
	{
		return 92;
	}

	override bool AdminOnly()
	{
		return true;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override string ToggleInput()
	{
		return "";
	}

	INT_QST_Clicks Clicks()
	{
		return m_Clicks;
	}

	// --- cover ----------------------------------------------------------------------------------------------------------
	override void BuildCoverHeader(Widget header, float w)
	{
		INT_QST_GvKit.CoverTitle(header, "export", "#STR_INT_QST_A_EXCHANGE", "#STR_INT_QST_XC_SUB");
		INT_QST_GvKit.JournalTile(header, w, m_Clicks, ID_TO_JOURNAL);
	}

	// --- body (built once) ---------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		Pages(body, FOLD);
		BuildOutPage(body);
		BuildInPage(body);
		AddKey("ESC", "#STR_INT_QST_K_CLOSE");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_LMB"), "#STR_INT_QST_K_SELECT");
	}

	protected void BuildOutPage(Widget body)
	{
		float bh = GetBodyH();
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		m_OutHead = INT_UIF_Draw.Frame(body, 0, 0, FOLD, 56, "OutHead");
		Widget hv;
		Widget ab = INT_QST_Look.Button(body, 32, 60, 180, 32, "#STR_INT_QST_XC_ALL", "", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget ah = INT_UIF_Draw.Hit(ab, 0, 0, 180, 32, m_Clicks);
		m_Clicks.Bind(ah, ID_ALL, 0, hv);
		Widget nb = INT_QST_Look.Button(body, 222, 60, 150, 32, "#STR_INT_QST_XC_NONE", "", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget nh = INT_UIF_Draw.Hit(nb, 0, 0, 150, 32, m_Clicks);
		m_Clicks.Bind(nh, ID_NONE, 0, hv);
		m_XSrc = new INT_QST_XcListSource();
		m_XSrc.m_Owner = this;
		m_XList = new INT_QST_VList();
		m_XList.Build(body, 16, 102, FOLD - 44, bh - 102 - 200, m_XSrc, bar, thumb);
		string empty = INT_QST_Look.Tr("#STR_INT_QST_XC_NO_QUESTS");
		m_OutEmpty = INT_QST_Look.Text(body, 40, 112, FOLD - 80, 26, empty, 15, ink3);
		// name, where the files go, the last export, the button
		m_FName = INT_QST_Field.Create(body, 32, bh - 190, 420, "#STR_INT_QST_XC_NAME", s_Name, 40);
		INT_QST_GvKit.HintLines(body, 466, bh - 176, FOLD - 510, "#STR_INT_QST_XC_NAME_HINT", 12, 2);
		string outHint = INT_QST_Look.Tr("#STR_INT_QST_XC_OUT_HINT");
		outHint = INT_QST_Look.Fit(outHint, FOLD - 70, 12);
		INT_QST_Look.Text(body, 32, bh - 130, FOLD - 64, 20, outHint, 12, ink3);
		m_LastExportText = INT_QST_Look.Text(body, 32, bh - 107, FOLD - 64, 22, "", 13, ink2);
		m_ExportBox = INT_UIF_Draw.Frame(body, 32, bh - 72, 440, 52, "Export");
	}

	protected void BuildInPage(Widget body)
	{
		float bh = GetBodyH();
		float rx = FOLD + 60;
		float rw = GetBodyW() - rx - 34;
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		Widget page = INT_UIF_Draw.Frame(body, rx, 0, rw, bh, "InPage");
		m_InHead = INT_UIF_Draw.Frame(page, 0, 0, rw, 56, "InHead");
		Widget hv;
		Widget rb = INT_QST_Look.Button(page, 0, 60, 280, 40, "#STR_INT_QST_XC_REFRESH", "refresh", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget rh = INT_UIF_Draw.Hit(rb, 0, 0, 280, 40, m_Clicks);
		m_Clicks.Bind(rh, ID_REFRESH, 0, hv);
		m_FSrc = new INT_QST_XcFolderSource();
		m_FSrc.m_Owner = this;
		m_FList = new INT_QST_VList();
		m_FList.Build(page, 0, 110, rw, 200, m_FSrc, bar, thumb);
		m_FolderNote = INT_QST_Look.Text(page, 8, 122, rw - 16, 24, "", 13, ink3);
		// mode
		array<string> modes = new array<string>();
		modes.Insert("#STR_INT_QST_XC_MODE_NEW");
		modes.Insert("#STR_INT_QST_XC_MODE_REPLACE");
		m_CMode = INT_QST_Choice.Create(page, 0, 322, rw, "#STR_INT_QST_XC_MODE", modes, s_Mode);
		m_CMode.Event_Changed.Insert(OnModeChanged);
		// how it works
		float hy = 386;
		hy = HelpParagraph(page, hy, rw, "#STR_INT_QST_XC_HELP_1");
		hy = HelpParagraph(page, hy, rw, "#STR_INT_QST_XC_HELP_2");
		hy = HelpParagraph(page, hy, rw, "#STR_INT_QST_XC_HELP_3");
		hy = HelpParagraph(page, hy, rw, "#STR_INT_QST_XC_HELP_4");
		// result, button, chosen folder
		m_InResult = INT_QST_Look.Text(page, 0, bh - 100, rw, 22, "", 13, ink2);
		int yc = INT_QST_Look.Sticky();
		Widget ib = INT_QST_Look.Button(page, 0, bh - 72, 300, 52, "#STR_INT_QST_XC_LOAD", "import", INT_QST_Look.BTN_INK, yc, hv);
		ButtonWidget ih = INT_UIF_Draw.Hit(ib, 0, 0, 300, 52, m_Clicks);
		m_Clicks.Bind(ih, ID_IMPORT, 0, hv);
		m_Chosen = INT_QST_Look.Text(page, 316, bh - 72, rw - 316, 52, "", 14, ink);
	}

	// One paragraph of the explanation (a small ink square, then wrapped lines); returns the next y.
	protected float HelpParagraph(Widget page, float y, float rw, string key)
	{
		int ink2 = INT_QST_Look.Ink2();
		INT_UIF_Draw.Line(page, 4, y + 8, 5, 5, ink2);
		float used = INT_QST_GvKit.HintLines(page, 18, y, rw - 26, key, 13, 3);
		return y + used + 6;
	}

	override void OnReady()
	{
		m_SeenShape = INT_QST_Client.s_ShapeRevision;
		RebuildBranches();
		FillRows();
		m_XList.Refresh(true);
		DrawOutHead();
		DrawExportButton();
		ShowLastExport();
		DrawInHead();
		ShowFolderNote();
		ShowChosen();
		if (m_InResult && s_LastImport != "")
			m_InResult.SetText(INT_QST_Look.Fmt("#STR_INT_QST_XC_IMPORTED", s_LastImport));
	}

	override void OnShow()
	{
		super.OnShow();
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			INT_QST_Admin.s_OnReply.Insert(OnAdminReply);
			m_Hooked = true;
		}
		if (!INT_QST_Client.IsAdmin())
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_17"));
			return;
		}
		RequestList();
	}

	override void OnHide()
	{
		Unhook();
		if (m_FName)
			s_Name = m_FName.GetText();
		super.OnHide();
	}

	void ~INT_QST_ExchangeMenu()
	{
		Unhook();
	}

	protected void Unhook()
	{
		if (!m_Hooked)
			return;
		m_Hooked = false;
		if (INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
		if (INT_QST_Admin.s_OnReply)
			INT_QST_Admin.s_OnReply.Remove(OnAdminReply);
	}

	// --- export: data ---------------------------------------------------------------------------------------------------
	// Quest ids per branch in catalog order; picks of quests that left the catalog are dropped.
	protected void RebuildBranches()
	{
		m_BranchKeys.Clear();
		m_BranchIds.Clear();
		INT_QST_Catalog cat = INT_QST_Client.s_Catalog;
		map<string, int> index = new map<string, int>();
		foreach (INT_QST_QuestDef q : cat.m_List)
		{
			int bi;
			if (!index.Find(q.m_Branch, bi))
			{
				bi = m_BranchKeys.Count();
				index.Set(q.m_Branch, bi);
				m_BranchKeys.Insert(q.m_Branch);
				array<int> fresh = new array<int>();
				m_BranchIds.Insert(fresh);
			}
			array<int> ids = m_BranchIds[bi];
			ids.Insert(q.Id);
		}
		array<int> gone = new array<int>();
		foreach (int pid, bool pv : m_Picked)
		{
			if (!cat.Get(pid))
				gone.Insert(pid);
		}
		foreach (int gid : gone)
			m_Picked.Remove(gid);
	}

	protected void FillRows()
	{
		m_XSrc.m_Rows.Clear();
		int nb = m_BranchKeys.Count();
		for (int b = 0; b < nb; b++)
		{
			string bk = m_BranchKeys[b];
			INT_QST_XcRow g = new INT_QST_XcRow();
			g.m_Group = true;
			g.m_Branch = bk;
			g.m_BranchIdx = b;
			m_XSrc.m_Rows.Insert(g);
			if (!m_Open.Contains(bk))
				continue;
			array<int> ids = m_BranchIds[b];
			foreach (int id : ids)
			{
				INT_QST_XcRow r = new INT_QST_XcRow();
				r.m_Branch = bk;
				r.m_BranchIdx = b;
				r.m_QuestId = id;
				m_XSrc.m_Rows.Insert(r);
			}
		}
		if (m_OutEmpty)
			m_OutEmpty.Show(nb == 0);
	}

	bool IsBranchOpen(string key)
	{
		return m_Open.Contains(key);
	}

	bool IsPicked(int id)
	{
		return m_Picked.Contains(id);
	}

	int BranchSize(int bi)
	{
		if (bi < 0 || bi >= m_BranchIds.Count())
			return 0;
		array<int> ids = m_BranchIds[bi];
		return ids.Count();
	}

	int PickedIn(int bi)
	{
		if (bi < 0 || bi >= m_BranchIds.Count())
			return 0;
		array<int> ids = m_BranchIds[bi];
		int n = 0;
		foreach (int id : ids)
		{
			if (m_Picked.Contains(id))
				n++;
		}
		return n;
	}

	protected int TotalQuests()
	{
		int n = 0;
		foreach (array<int> ids : m_BranchIds)
		{
			if (ids)
				n = n + ids.Count();
		}
		return n;
	}

	void ToggleBranchOpen(string key)
	{
		if (m_Open.Contains(key))
			m_Open.Remove(key);
		else
			m_Open.Set(key, true);
		FillRows();
		m_XList.Refresh(false);
		INT_QST_Sfx.Play("page");
	}

	void ToggleQuest(int id)
	{
		if (m_Picked.Contains(id))
			m_Picked.Remove(id);
		else
			m_Picked.Set(id, true);
		AfterPick();
	}

	// The branch box: all of its quests on, or all off when they are all on already.
	protected void ToggleBranchPick(int bi)
	{
		if (bi < 0 || bi >= m_BranchIds.Count())
			return;
		array<int> ids = m_BranchIds[bi];
		int picked = PickedIn(bi);
		bool allOn = picked == ids.Count();
		foreach (int id : ids)
		{
			if (allOn)
				m_Picked.Remove(id);
			else
				m_Picked.Set(id, true);
		}
		AfterPick();
	}

	protected void PickAll(bool on)
	{
		m_Picked.Clear();
		if (on)
		{
			foreach (array<int> ids : m_BranchIds)
			{
				foreach (int id : ids)
					m_Picked.Set(id, true);
			}
		}
		AfterPick();
	}

	protected void AfterPick()
	{
		m_XList.Rebind();
		DrawOutHead();
		DrawExportButton();
		INT_QST_Sfx.Play("check");
	}

	// --- export: drawing ------------------------------------------------------------------------------------------------
	// Caption and «отмечено N из M» (redrawn only when the numbers change: s_OnChanged comes often).
	protected void DrawOutHead()
	{
		int pickedN = m_Picked.Count();
		int totalN = TotalQuests();
		if (!m_OutHead || (pickedN == m_HeadPicked && totalN == m_HeadTotal))
			return;
		m_HeadPicked = pickedN;
		m_HeadTotal = totalN;
		INT_UIF_Widgets.ClearChildren(m_OutHead);
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_XC_OUT");
		INT_QST_Look.Heavy(m_OutHead, 32, 20, 300, 22, cap, 13, ink);
		string picked = pickedN.ToString();
		string total = totalN.ToString();
		string line = INT_QST_Look.Fmt("#STR_INT_QST_XC_PICKED", picked, total);
		INT_QST_Look.Text(m_OutHead, FOLD - 330, 19, 300, 22, line, 14, ink3, INT_UIF_Align.RIGHT);
		INT_QST_Look.DoubleRule(m_OutHead, 32, 49, FOLD - 60);
	}

	// «Выгрузить N заданий»: redrawn only when N changes.
	protected void DrawExportButton()
	{
		if (!m_ExportBox)
			return;
		int n = m_Picked.Count();
		if (n == m_BtnCount)
			return;
		m_BtnCount = n;
		// never from the button's own click (DoExport does not redraw it); its old binding goes with it
		m_Clicks.ForgetUnder(m_ExportBox);
		INT_UIF_Widgets.ClearChildren(m_ExportBox);
		int form = INT_QST_GvKit.Plural(n);
		string key = "#STR_INT_QST_XC_BTN_" + form.ToString();
		string label = INT_QST_Look.Fmt(key, n.ToString());
		int style = INT_QST_Look.BTN_INK;
		int ic = INT_QST_Look.Sticky();
		if (n == 0)
		{
			style = INT_QST_Look.BTN_GHOST;
			ic = INT_QST_Look.Ink3();
		}
		Widget hv;
		Widget b = INT_QST_Look.Button(m_ExportBox, 0, 0, 440, 52, label, "export", style, ic, hv);
		ButtonWidget hit = INT_UIF_Draw.Hit(b, 0, 0, 440, 52, m_Clicks);
		m_Clicks.Bind(hit, ID_EXPORT, 0, hv);
	}

	protected void ShowLastExport()
	{
		if (!m_LastExportText)
			return;
		if (s_LastExport == "")
		{
			m_LastExportText.Show(false);
			return;
		}
		string line = INT_QST_Look.Fmt("#STR_INT_QST_XC_LAST", s_LastExport);
		m_LastExportText.SetText(INT_QST_Look.Fit(line, FOLD - 70, 13));
		m_LastExportText.Show(true);
	}

	protected void DoExport()
	{
		if (IsWaiting())
			return;
		array<int> ids = new array<int>();
		foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
		{
			if (m_Picked.Contains(q.Id))
				ids.Insert(q.Id);
		}
		if (ids.Count() == 0)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_XC_ERR_NONE"));
			return;
		}
		string name = "";
		if (m_FName)
			name = m_FName.GetText();
		name.TrimInPlace();
		if (name != "" && !INT_QST_GvKit.IsSafeName(name, 40))
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_XC_ERR_NAME"));
			return;
		}
		s_Name = name;
		INT_QST_Admin.Export(ids, name);
		StartWait(INT_QST_A.EXPORT);
		INT_QST_Sfx.Play("click");
	}

	// --- import ---------------------------------------------------------------------------------------------------------
	int FolderCount()
	{
		return m_Folders.Count();
	}

	string FolderName(int i)
	{
		if (i < 0 || i >= m_Folders.Count())
			return "";
		return m_Folders[i];
	}

	string FolderInfo(int i)
	{
		if (i < 0 || i >= m_FolderInfo.Count())
			return "";
		return m_FolderInfo[i];
	}

	int SelectedFolder()
	{
		return m_FolderSel;
	}

	void PickFolder(int i)
	{
		if (i < 0 || i >= m_Folders.Count() || i == m_FolderSel)
			return;
		m_FolderSel = i;
		m_FList.Rebind();
		ShowChosen();
		INT_QST_Sfx.Play("page");
	}

	protected void RequestList()
	{
		if (m_ListLeft > 0)
			return;
		INT_QST_Admin.ImportList();
		m_ListLeft = 10;
		ShowFolderNote();
	}

	// "branches|quests" of the server -> "веток 2 · заданий 14".
	protected string InfoLine(string raw)
	{
		array<string> parts = new array<string>();
		raw.Split("|", parts);
		if (parts.Count() != 2)
			return raw;
		string nb = parts[0];
		string nq = parts[1];
		return INT_QST_Look.Fmt("#STR_INT_QST_XC_FOLDER_INFO", nb, nq);
	}

	protected void ReadFolders(string text)
	{
		INT_QST_AImportList l = new INT_QST_AImportList();
		JsonSerializer js = new JsonSerializer();
		string err;
		if (!js.ReadFromString(l, text, err) || !l.Folders)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_18"));
			return;
		}
		string keep = FolderName(m_FolderSel);
		m_Folders.Clear();
		m_FolderInfo.Clear();
		m_FolderSel = -1;
		int infoCount = 0;
		if (l.Info)
			infoCount = l.Info.Count();
		int n = l.Folders.Count();
		for (int i = 0; i < n; i++)
		{
			string f = l.Folders[i];
			string raw = "";
			if (i < infoCount)
				raw = l.Info[i];
			m_Folders.Insert(f);
			m_FolderInfo.Insert(InfoLine(raw));
			if (keep != "" && f == keep)
				m_FolderSel = i;
		}
		m_ListKnown = true;
		m_FList.Refresh(false);
		DrawInHead();
		ShowFolderNote();
		ShowChosen();
	}

	protected void DrawInHead()
	{
		if (!m_InHead)
			return;
		INT_UIF_Widgets.ClearChildren(m_InHead);
		float rw = GetBodyW() - (FOLD + 60) - 34;
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_XC_IN");
		INT_QST_Look.Heavy(m_InHead, 0, 20, 300, 22, cap, 13, ink);
		if (m_ListKnown)
		{
			string n = m_Folders.Count().ToString();
			string count = INT_QST_Look.Fmt("#STR_INT_QST_XC_FOLDERS", n);
			INT_QST_Look.Text(m_InHead, rw - 200, 19, 200, 22, count, 14, ink3, INT_UIF_Align.RIGHT);
		}
		INT_QST_Look.DoubleRule(m_InHead, 0, 49, rw);
	}

	protected void ShowFolderNote()
	{
		if (!m_FolderNote)
			return;
		string key = "";
		if (m_ListLeft > 0 && !m_ListKnown)
			key = "#STR_INT_QST_XC_LOADING";
		else if (m_ListKnown && m_Folders.Count() == 0)
			key = "#STR_INT_QST_XC_NO_FOLDERS";
		if (key == "")
		{
			m_FolderNote.Show(false);
			return;
		}
		float rw = GetBodyW() - (FOLD + 60) - 34;
		string text = INT_QST_Look.Tr(key);
		m_FolderNote.SetText(INT_QST_Look.Fit(text, rw - 20, 13));
		m_FolderNote.Show(true);
	}

	protected void ShowChosen()
	{
		if (!m_Chosen)
			return;
		float rw = GetBodyW() - (FOLD + 60) - 34;
		string name = FolderName(m_FolderSel);
		string text = INT_QST_Look.Tr("#STR_INT_QST_XC_PICK_FOLDER");
		int c = INT_QST_Look.Ink3();
		if (name != "")
		{
			text = INT_QST_Look.Fmt("#STR_INT_QST_XC_CHOSEN", name);
			c = INT_QST_Look.Ink();
		}
		m_Chosen.SetText(INT_QST_Look.Fit(text, rw - 326, 14));
		m_Chosen.SetColor(c);
	}

	protected string ModeKey(int mode)
	{
		if (mode == 1)
			return "#STR_INT_QST_XC_MODE_REPLACE";
		return "#STR_INT_QST_XC_MODE_NEW";
	}

	void OnModeChanged(INT_QST_Choice c, int index)
	{
		s_Mode = index;
	}

	protected void AskImport()
	{
		string folder = FolderName(m_FolderSel);
		if (folder == "")
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_XC_PICK_FOLDER"));
			return;
		}
		if (IsWaiting())
			return;
		m_AskFolder = folder;
		string modeText = INT_QST_Look.Tr(ModeKey(s_Mode));
		string text = INT_QST_Look.Fmt("#STR_INT_QST_XC_ASK_TEXT", folder, modeText);
		bool danger = s_Mode == 1;
		Ask(ASK_IMPORT, 0, "#STR_INT_QST_XC_ASK", text, "#STR_INT_QST_XC_LOAD", danger);
	}

	// --- server answers -------------------------------------------------------------------------------------------------
	protected void StartWait(int op)
	{
		m_WaitOp = op;
		m_WaitLeft = 15;
	}

	protected bool IsWaiting()
	{
		if (m_WaitOp == 0)
			return false;
		Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_WAIT"), false);
		return true;
	}

	// Catalog changed (an import, an edit on the board, a reload): the left page follows, picks are kept. The
	// refresh comes from Update, throttled (s_OnChanged also comes with every progress packet).
	void OnDataChanged()
	{
		if (IsClosingDown())
			return;
		MarkDirty();
	}

	override void OnDirtyRefresh()
	{
		if (IsClosingDown())
			return;
		int shape = INT_QST_Client.s_ShapeRevision;
		if (shape == m_SeenShape)
			return;		// only progress counters moved: the catalog is the same
		m_SeenShape = shape;
		RebuildBranches();
		FillRows();
		m_XList.Refresh(false);
		DrawOutHead();
		DrawExportButton();
	}

	void OnAdminReply(int op, int code, string text)
	{
		if (IsClosingDown())
			return;
		if (op != INT_QST_A.EXPORT && op != INT_QST_A.IMPORT_LIST && op != INT_QST_A.IMPORT)
			return;
		if (op == m_WaitOp)
		{
			m_WaitOp = 0;
			m_WaitLeft = 0;
		}
		if (op == INT_QST_A.IMPORT_LIST)
			m_ListLeft = 0;
		if (code != INT_QST_Res.OK)
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + code.ToString()));
			ShowFolderNote();
			return;
		}
		if (op == INT_QST_A.IMPORT_LIST)
		{
			ReadFolders(text);
			return;
		}
		string t = INT_QST_Admin.TextOf(text);
		INT_QST_Sfx.Play("stamp");
		if (op == INT_QST_A.EXPORT)
		{
			s_LastExport = t;
			ShowLastExport();
			Flash(INT_QST_Look.Fmt("#STR_INT_QST_XC_EXPORTED", t), false);
			return;
		}
		s_LastImport = t;
		string sum = INT_QST_Look.Fmt("#STR_INT_QST_XC_IMPORTED", t);
		if (m_InResult)
		{
			float rw = GetBodyW() - (FOLD + 60) - 34;
			m_InResult.SetText(INT_QST_Look.Fit(sum, rw - 10, 13));
		}
		Flash(sum, false);
	}

	// --- input ----------------------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_WaitLeft > 0)
		{
			m_WaitLeft = m_WaitLeft - timeslice;
			if (m_WaitLeft <= 0)
				m_WaitOp = 0;
		}
		if (m_ListLeft > 0)
		{
			m_ListLeft = m_ListLeft - timeslice;
			if (m_ListLeft <= 0)
				ShowFolderNote();
		}
	}

	override void OnClickId(int id, int data, int button)
	{
		if (CommonClick(id))
			return;
		switch (id)
		{
			case ID_BRANCH_BOX:
				ToggleBranchPick(data);
				return;
			case ID_ALL:
				PickAll(true);
				return;
			case ID_NONE:
				PickAll(false);
				return;
			case ID_EXPORT:
				DoExport();
				return;
			case ID_REFRESH:
				INT_QST_Sfx.Play("click");
				RequestList();
				return;
			case ID_IMPORT:
				AskImport();
				return;
			case ID_TO_JOURNAL:
				INT_QST_Sfx.Play("click");
				INT_UIF_Menu.Open(INT_QST_Const.MENU_JOURNAL);
				return;
		}
	}

	// The wheel over a branch box scrolls the list under it.
	override void OnWheelId(int id, int wheel)
	{
		if (id == ID_BRANCH_BOX && m_XList)
			m_XList.ScrollBy(-wheel * 60);
	}

	override void OnAnswer(int tag, int data, bool yes)
	{
		if (tag != ASK_IMPORT || !yes || m_AskFolder == "")
			return;
		if (IsWaiting())
			return;
		string folder = m_AskFolder;
		int mode = s_Mode;
		INT_QST_Admin.Import(folder, mode);
		StartWait(INT_QST_A.IMPORT);
		INT_QST_Sfx.Play("click");
	}
}
