// INT PartyMode (client) — tabs «Входящие заявки» and «Исходящие заявки» (3_c_expedition_invites.png): both lists
// side by side as in the concept; the column of the active tab carries the orange label and the white number.
// Incoming: «Принять» (primary) / «Отклонить»; outgoing: «Ожидает ответа» + «Отозвать», or the final state in the
// warning colour (removed by the client core 60 s later). Each column is a pixel-smooth list (INT_PTY_MScroll); an
// answered row fades and the rows under it close the gap.

class INT_PTY_InviteRowW : Managed
{
	Widget Root;
	ImageWidget NodeFill;
	ImageWidget NodeLine;
	TextWidget Nick;
	TextWidget When;
	Widget Act;
	string Id;
	float At;
	bool Out;
	int State;
	ref INT_PTY_MButton BtnA;
	ref INT_PTY_MButton BtnB;
}

class INT_PTY_PageInvites : INT_PTY_Page
{
	static const float LX = 80;
	static const float RX = 1040;
	static const float CW = 800;
	static const float RY = 356;
	static const float RH = 100;
	static const int VISIBLE = 5;
	static const int SID_IN = 2;
	static const int SID_OUT = 3;

	protected bool m_OutMode;
	protected ref array<ref INT_PTY_InviteRowW> m_InRows;
	protected ref array<ref INT_PTY_InviteRowW> m_OutRows;
	protected ref INT_PTY_MScroll m_ScIn;
	protected ref INT_PTY_MScroll m_ScOut;
	protected Widget m_InHead;
	protected Widget m_OutHead;
	protected Widget m_InEmpty;
	protected Widget m_OutEmpty;
	protected Widget m_Note;
	protected float m_TimeAcc;
	protected float m_AnimLeft;
	protected bool m_Dirty;
	protected int m_BlockShown = -1;

	void INT_PTY_PageInvites(bool outMode)
	{
		m_OutMode = outMode;
		m_InRows = new array<ref INT_PTY_InviteRowW>();
		m_OutRows = new array<ref INT_PTY_InviteRowW>();
	}

	protected int InCount()
	{
		if (!INT_PTY_Client.s_Incoming)
			return 0;
		return INT_PTY_Client.s_Incoming.Count();
	}

	protected int OutCount()
	{
		if (!INT_PTY_Client.s_Outgoing)
			return 0;
		return INT_PTY_Client.s_Outgoing.Count();
	}

	protected int OutWaiting()
	{
		int n = 0;
		if (!INT_PTY_Client.s_Outgoing)
			return 0;
		foreach (INT_PTY_InviteRow r : INT_PTY_Client.s_Outgoing)
		{
			if (r && r.State == INT_PTY_Const.OUT_SENT)
				n++;
		}
		return n;
	}

	override void Build(bool animate)
	{
		float d = m_Menu.PageDelay();
		INT_PTY_MD.Line(m_P, 960, 244, 1, 520, INT_PTY_MC.L);
		m_InHead = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_IHead");
		m_OutHead = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_OHead");
		Widget inList = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_IList");
		Widget outList = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_OList");
		m_InEmpty = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_IEmpty");
		m_OutEmpty = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_OEmpty");
		m_Note = INT_UIF_Draw.Frame(m_P, 0, 0, 1920, 1080, "INT_PTY_INote");
		m_InRows.Clear();
		m_OutRows.Clear();
		m_ScIn = new INT_PTY_MScroll();
		m_ScIn.Init(m_Menu, this, SID_IN, "ilist", inList, LX, RY, CW, VISIBLE * RH, RH, LX - 16);
		m_ScOut = new INT_PTY_MScroll();
		m_ScOut.Init(m_Menu, this, SID_OUT, "olist", outList, RX, RY, CW, VISIBLE * RH, RH, RX - 16);
		Refresh(false);
		if (!animate)
			return;
		INT_PTY_MenuFx.BlockIn(m_InHead, 0, 14, d, 0.3);
		INT_PTY_MenuFx.BlockIn(m_OutHead, 0, 14, d + 0.08, 0.3);
		INT_PTY_MenuFx.BlockIn(m_InEmpty, 0, 14, d + 0.1, 0.3);
		INT_PTY_MenuFx.BlockIn(m_OutEmpty, 0, 14, d + 0.14, 0.3);
		INT_PTY_MenuFx.BlockIn(m_Note, 0, 10, d + 0.2, 0.3);
		m_ScIn.Enter(d + 0.06);
		m_ScOut.Enter(d + 0.14);
	}

	// Column head: big number + label; the active tab's column is lit.
	protected void ColHead(Widget p, float x, int count, string label, bool lit)
	{
		INT_UIF_Widgets.ClearChildren(p);
		int numCol = INT_PTY_MC.M;
		int labCol = INT_PTY_MC.M;
		if (lit)
		{
			numCol = INT_PTY_MC.T;
			labCol = INT_PTY_MC.O;
		}
		string num = INT_PTY_Text.Pad2(count);
		INT_PTY_MD.Txt(p, x - 2, 262, 130, 82, num, 64, numCol, INT_UIF_Align.LEFT, false, true);
		float nw = INT_PTY_MD.Measure(p, num, 64, false, true);
		INT_PTY_MD.Caps(p, x + nw + 18, 310, 700, 20, label, 13, labCol);
		INT_PTY_MD.Line(p, x, 356, CW, 1, INT_PTY_MC.L);
	}

	// Heads, empty states, the note and both lists from the current data (offsets kept).
	protected void Refresh(bool keep)
	{
		m_Dirty = false;
		int ni = InCount();
		int nOut = OutCount();
		ColHead(m_InHead, LX, ni, "#STR_INT_PTY_W_I_IN", !m_OutMode);
		string labA = INT_PTY_Text.Tr("#STR_INT_PTY_W_I_OUT");
		string labB = INT_PTY_Text.Tr("#STR_INT_PTY_W_I_OUT_SUB");
		string lab = labA + "  ·  " + labB;
		ColHead(m_OutHead, RX, OutWaiting(), lab, m_OutMode);
		m_Menu.DropHitsUnder(INT_PTY_Menu.SEC_PAGE, m_InEmpty);
		m_Menu.DropHitsUnder(INT_PTY_Menu.SEC_PAGE, m_OutEmpty);
		INT_UIF_Widgets.ClearChildren(m_InEmpty);
		INT_UIF_Widgets.ClearChildren(m_OutEmpty);
		float noteY = RY + 120;
		if (ni == 0)
		{
			INT_PTY_MD.DmLine(m_InEmpty, LX + 10, RY + 46, 12, INT_PTY_MC.D);
			INT_PTY_MD.Txt(m_InEmpty, LX + 38, RY + 26, 760, 40, "#STR_INT_PTY_W_I_EMPTY_IN", 26, INT_PTY_MC.M);
			string sub = "#STR_INT_PTY_W_I_EMPTY_IN_ON";
			if (!INT_PTY_Client.s_Searching)
				sub = "#STR_INT_PTY_W_I_EMPTY_IN_OFF";
			INT_PTY_MD.Para(m_InEmpty, LX + 38, RY + 66, 700, sub, 16, INT_PTY_MC.D, 23, 2);
		}
		else
		{
			int shownIn = ni;
			if (shownIn > VISIBLE)
				shownIn = VISIBLE;
			noteY = RY + shownIn * RH;
		}
		if (nOut == 0)
		{
			INT_PTY_MD.DmLine(m_OutEmpty, RX + 10, RY + 46, 12, INT_PTY_MC.D);
			INT_PTY_MD.Txt(m_OutEmpty, RX + 38, RY + 26, 760, 40, "#STR_INT_PTY_W_I_EMPTY_OUT", 26, INT_PTY_MC.M);
			INT_PTY_MD.Para(m_OutEmpty, RX + 38, RY + 66, 700, "#STR_INT_PTY_W_I_EMPTY_OUT_H", 16, INT_PTY_MC.D, 23, 2);
			ButtonWidget go = m_Menu.Hit(m_OutEmpty, RX + 30, RY + 10, 700, 100, "goto:1");
			m_Menu.SetTip(go, "#STR_INT_PTY_W_TAB_SEARCH", "#STR_INT_PTY_W_TIP_TAB_1", 340);
			m_Menu.SetSound(go, false);
		}
		// note (.note) under the incoming list
		INT_UIF_Widgets.ClearChildren(m_Note);
		INT_PTY_MD.Ico(m_Note, LX, noteY + 30, 20, "info", INT_PTY_MC.O);
		INT_PTY_MD.Para(m_Note, LX + 34, noteY + 27, 640, "#STR_INT_PTY_W_I_NOTE", 16, INT_PTY_MC.M, 23, 3);
		m_BlockShown = INT_PTY_Client.BlockedSeconds();
		if (m_ScIn)
			m_ScIn.SetCount(ni, keep);
		if (m_ScOut)
			m_ScOut.SetCount(nOut, keep);
	}

	// --- pooled rows ---------------------------------------------------------------------------------------------------
	override void ScrollBuild(int sid, int slot, Widget root)
	{
		INT_PTY_InviteRowW w = new INT_PTY_InviteRowW();
		w.Root = root;
		w.Out = sid == SID_OUT;
		INT_PTY_MD.Line(root, 0, RH - 1, CW, 1, ARGB(18, 255, 255, 255));
		INT_PTY_MD.Dm(root, 10, 50, 13, INT_PTY_MC.BG);
		w.NodeFill = INT_PTY_MD.Dm(root, 10, 50, 12, INT_PTY_MC.O);
		w.NodeLine = INT_PTY_MD.DmLine(root, 10, 50, 12, INT_PTY_MC.O);
		w.Nick = INT_PTY_MD.Txt(root, 38, 18, 420, 40, "", 28, INT_PTY_MC.T);
		w.When = INT_PTY_MD.Txt(root, 38, 58, 400, 22, "", 15, INT_PTY_MC.M);
		w.Act = INT_UIF_Draw.Frame(root, 0, 0, CW, RH, "INT_PTY_IAct");
		ButtonWidget hit = m_Menu.Hit(root, 0, 0, 440, RH, "irow:" + sid.ToString());
		m_Menu.SetSound(hit, false);
		if (w.Out)
			m_OutRows.Insert(w);
		else
			m_InRows.Insert(w);
	}

	override void ScrollFill(int sid, int slot, int index)
	{
		if (sid == SID_IN)
			FillIn(slot, index);
		else
			FillOut(slot, index);
	}

	protected void FillIn(int slot, int index)
	{
		if (slot < 0 || slot >= m_InRows.Count() || index < 0 || index >= InCount())
			return;
		INT_PTY_InviteRowW w = m_InRows.Get(slot);
		INT_PTY_InviteRow r = INT_PTY_Client.s_Incoming.Get(index);
		if (!r)
			return;
		w.Id = r.Id;
		w.At = r.At;
		bool fresh = INT_PTY_Client.Clock() - r.At < 60;
		if (w.NodeFill)
			w.NodeFill.Show(fresh);
		if (w.NodeLine)
		{
			w.NodeLine.Show(!fresh);
			w.NodeLine.SetColor(INT_PTY_MC.O);
		}
		string nick = INT_PTY_Look.FitText(r.Name, 28, false, 400);
		if (w.Nick)
		{
			w.Nick.SetText(nick);
			w.Nick.SetColor(INT_PTY_MC.T);
		}
		RepaintWhen(w);
		BuildInActions(w);
	}

	protected void FillOut(int slot, int index)
	{
		if (slot < 0 || slot >= m_OutRows.Count() || index < 0 || index >= OutCount())
			return;
		INT_PTY_InviteRowW w = m_OutRows.Get(slot);
		INT_PTY_InviteRow r = INT_PTY_Client.s_Outgoing.Get(index);
		if (!r)
			return;
		w.Id = r.Id;
		w.At = r.At;
		w.State = r.State;
		bool isFinal = r.State != INT_PTY_Const.OUT_SENT;
		int nodeCol = INT_PTY_MC.M;
		int nickCol = INT_PTY_MC.T;
		if (isFinal)
		{
			nodeCol = INT_PTY_MC.W;
			nickCol = INT_PTY_MC.M;
		}
		if (w.NodeFill)
			w.NodeFill.Show(false);
		if (w.NodeLine)
		{
			w.NodeLine.Show(true);
			w.NodeLine.SetColor(nodeCol);
		}
		string nick = INT_PTY_Look.FitText(r.Name, 28, false, 360);
		if (w.Nick)
		{
			w.Nick.SetText(nick);
			w.Nick.SetColor(nickCol);
		}
		RepaintWhen(w);
		BuildOutActions(w);
	}

	protected void ClearActs(INT_PTY_InviteRowW w)
	{
		if (!w.Act)
			return;
		m_Menu.DropHitsUnder(INT_PTY_Menu.SEC_PAGE, w.Act);
		INT_UIF_Widgets.ClearChildren(w.Act);
		w.BtnA = null;
		w.BtnB = null;
	}

	protected void BuildInActions(INT_PTY_InviteRowW w)
	{
		ClearActs(w);
		Widget a = w.Act;
		if (INT_PTY_Client.IsBlocked())
		{
			int secs = INT_PTY_Client.BlockedSeconds();
			string ss = secs.ToString();
			string wait = INT_PTY_Text.Fmt("#STR_INT_PTY_W_WAIT", ss);
			string tip = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_BLOCK", ss);
			w.BtnA = INT_PTY_MButton.Create(a, CW, 28, 44, "hourglass", wait, INT_PTY_MButton.DIS, true);
			ButtonWidget bh = m_Menu.Hit(w.BtnA.m_Root, 0, 0, w.BtnA.m_W, w.BtnA.m_H, "blocked");
			m_Menu.SetOff(bh, true);
			m_Menu.SetTip(bh, "#STR_INT_PTY_W_TIP_BLOCK_H", tip, 360);
			return;
		}
		w.BtnB = INT_PTY_MButton.Create(a, CW, 28, 44, "close", "#STR_INT_PTY_W_I_DECLINE", INT_PTY_MButton.GH, true);
		ButtonWidget dh = m_Menu.Hit(w.BtnB.m_Root, 0, 0, w.BtnB.m_W, w.BtnB.m_H, "decline:" + w.Id);
		m_Menu.SetHoverBtn(dh, w.BtnB);
		m_Menu.SetTip(dh, "#STR_INT_PTY_W_TIP_DECLINE_H", "#STR_INT_PTY_W_TIP_DECLINE", 340);
		float ax = CW - w.BtnB.m_W - 16;
		w.BtnA = INT_PTY_MButton.Create(a, ax, 28, 44, "check", "#STR_INT_PTY_W_I_ACCEPT", INT_PTY_MButton.PRI, true, 14, "", 152);
		ButtonWidget ah = m_Menu.Hit(w.BtnA.m_Root, 0, 0, w.BtnA.m_W, w.BtnA.m_H, "accept:" + w.Id);
		m_Menu.SetHoverBtn(ah, w.BtnA);
		m_Menu.SetTip(ah, "#STR_INT_PTY_W_TIP_ACCEPT_H", "#STR_INT_PTY_W_TIP_ACCEPT", 340);
	}

	protected string FinalText(int state)
	{
		if (state == INT_PTY_Const.OUT_SEARCH_OFF)
			return "#STR_INT_PTY_W_I_SEARCH_OFF";
		if (state == INT_PTY_Const.OUT_TARGET_LEFT)
			return "#STR_INT_PTY_W_I_LEFT";
		return "#STR_INT_PTY_W_I_DECLINED";
	}

	protected void BuildOutActions(INT_PTY_InviteRowW w)
	{
		ClearActs(w);
		Widget a = w.Act;
		if (w.State != INT_PTY_Const.OUT_SENT)
		{
			string ft = INT_PTY_Text.Tr(FinalText(w.State));
			float fw = INT_PTY_MD.Measure(a, ft, 15, true);
			float roomF = INT_PTY_MD.Room(fw);
			INT_PTY_MD.Ico(a, CW - fw - 28, 41, 18, "warning", INT_PTY_MC.W);
			INT_PTY_Look.Text(a, CW - roomF, 36, roomF, 28, ft, 15, INT_PTY_MC.W, INT_UIF_Align.RIGHT, true);
			return;
		}
		bool blocked = INT_PTY_Client.IsBlocked();
		string bLabel = "#STR_INT_PTY_W_I_CANCEL";
		int bStyle = INT_PTY_MButton.GH;
		string bKey = "cancel:" + w.Id;
		string ss = "";
		if (blocked)
		{
			int secs = INT_PTY_Client.BlockedSeconds();
			ss = secs.ToString();
			bLabel = INT_PTY_Text.Fmt("#STR_INT_PTY_W_WAIT", ss);
			bStyle = INT_PTY_MButton.DIS;
			bKey = "blocked";
		}
		w.BtnA = INT_PTY_MButton.Create(a, CW, 28, 44, "", bLabel, bStyle, true);
		ButtonWidget ch = m_Menu.Hit(w.BtnA.m_Root, 0, 0, w.BtnA.m_W, w.BtnA.m_H, bKey);
		if (blocked)
		{
			string tip = INT_PTY_Text.Fmt("#STR_INT_PTY_W_TIP_BLOCK", ss);
			m_Menu.SetOff(ch, true);
			m_Menu.SetTip(ch, "#STR_INT_PTY_W_TIP_BLOCK_H", tip, 360);
		}
		else
		{
			m_Menu.SetHoverBtn(ch, w.BtnA);
			m_Menu.SetTip(ch, "#STR_INT_PTY_W_TIP_CANCEL_H", "#STR_INT_PTY_W_TIP_CANCEL", 340);
		}
		// ⌛ ОЖИДАЕТ ОТВЕТА left of the button
		string wt = INT_PTY_Text.Caps("#STR_INT_PTY_W_I_WAIT");
		float tw = INT_PTY_MD.Measure(a, wt, 13, true);
		float tx = CW - w.BtnA.m_W - 18 - tw;
		INT_PTY_MD.Ico(a, tx - 24, 42, 16, "hourglass", INT_PTY_MC.M);
		INT_PTY_Look.Text(a, tx, 39, INT_PTY_MD.Room(tw), 22, wt, 13, INT_PTY_MC.T, INT_UIF_Align.LEFT, true);
	}

	protected void RepaintWhen(INT_PTY_InviteRowW w)
	{
		if (!w.When)
			return;
		float secs = INT_PTY_Client.Clock() - w.At;
		string ago = INT_PTY_MenuGeo.Ago(secs);
		if (w.Out && w.State == INT_PTY_Const.OUT_SENT)
		{
			string span = INT_PTY_MenuGeo.Span(secs);
			if (secs < 60)
				ago = INT_PTY_Text.Tr("#STR_INT_PTY_W_I_SENT_NOW");
			else
				ago = INT_PTY_Text.Fmt("#STR_INT_PTY_W_I_SENT_AGO", span);
		}
		w.When.SetText(ago);
	}

	protected void RepaintBlock(bool force)
	{
		int b = INT_PTY_Client.BlockedSeconds();
		if (!force && b == m_BlockShown)
			return;
		m_BlockShown = b;
		for (int i = 0; i < m_InRows.Count(); i++)
		{
			if (!m_ScIn || m_ScIn.SlotIndex(i) < 0)
				continue;
			BuildInActions(m_InRows.Get(i));
			m_ScIn.Recapture(i);
		}
		for (int j = 0; j < m_OutRows.Count(); j++)
		{
			if (!m_ScOut || m_ScOut.SlotIndex(j) < 0)
				continue;
			INT_PTY_InviteRowW wo = m_OutRows.Get(j);
			if (wo.State != INT_PTY_Const.OUT_SENT)
				continue;
			BuildOutActions(wo);
			m_ScOut.Recapture(j);
		}
	}

	// The answered row fades and the rows under it move up (anim 6.95–7.28 s of the concept).
	protected void Collapse(bool outCol, string id)
	{
		int idx = -1;
		if (outCol)
		{
			for (int i = 0; i < OutCount(); i++)
			{
				if (INT_PTY_Client.s_Outgoing.Get(i).Id == id)
					idx = i;
			}
			if (idx >= 0 && m_ScOut)
				m_ScOut.AnimateRemove(idx);
		}
		else
		{
			for (int j = 0; j < InCount(); j++)
			{
				if (INT_PTY_Client.s_Incoming.Get(j).Id == id)
					idx = j;
			}
			if (idx >= 0 && m_ScIn)
				m_ScIn.AnimateRemove(idx);
		}
		m_AnimLeft = 0.4;
		// W-1: always rebuild from the real data after the animation, so a row the server refused comes back.
		m_Dirty = true;
	}

	// --- events --------------------------------------------------------------------------------------------------------
	override bool OnClick(string key, Widget w)
	{
		if (key.IndexOf("accept:") == 0)
		{
			string aid = key.Substring(7, key.Length() - 7);
			if (INT_PTY_Client.IsBlocked())
			{
				INT_PTY_Client.Accept(aid);
				return true;
			}
			INT_PTY_Sfx.Click();
			Collapse(false, aid);
			INT_PTY_Client.Accept(aid);
			return true;
		}
		if (key.IndexOf("decline:") == 0)
		{
			string did = key.Substring(8, key.Length() - 8);
			if (INT_PTY_Client.IsBlocked())
			{
				INT_PTY_Client.Decline(did);
				return true;
			}
			INT_PTY_Sfx.Click();
			Collapse(false, did);
			INT_PTY_Client.Decline(did);
			return true;
		}
		if (key.IndexOf("cancel:") == 0)
		{
			string cid = key.Substring(7, key.Length() - 7);
			if (INT_PTY_Client.IsBlocked())
			{
				INT_PTY_Client.CancelInvite(cid);
				return true;
			}
			INT_PTY_Sfx.Click();
			Collapse(true, cid);
			INT_PTY_Client.CancelInvite(cid);
			return true;
		}
		return false;
	}

	override bool OnWheel(string key, int wheel)
	{
		bool inCol = false;
		bool outCol = false;
		if (key.IndexOf("ilist:") == 0 || key == "irow:2" || key.IndexOf("accept:") == 0 || key.IndexOf("decline:") == 0)
			inCol = true;
		if (key.IndexOf("olist:") == 0 || key == "irow:3" || key.IndexOf("cancel:") == 0)
			outCol = true;
		if (inCol && m_ScIn)
			m_ScIn.Wheel(wheel);
		if (outCol && m_ScOut)
			m_ScOut.Wheel(wheel);
		return inCol || outCol;
	}

	override bool OnDown(string key)
	{
		if (key == "ilist:bar" && m_ScIn)
		{
			m_ScIn.BarDown();
			return true;
		}
		if (key == "olist:bar" && m_ScOut)
		{
			m_ScOut.BarDown();
			return true;
		}
		return false;
	}

	override void OnSlide(string key, int mx, bool done)
	{
		if (key == "ilist:bar" && m_ScIn)
			m_ScIn.BarDrag(done);
		if (key == "olist:bar" && m_ScOut)
			m_ScOut.BarDrag(done);
	}

	override void TestScroll(float px)
	{
		if (m_OutMode && m_ScOut)
			m_ScOut.Jump(px);
		else if (m_ScIn)
			m_ScIn.Jump(px);
	}

	override void OnData(int what)
	{
		if ((what & (INT_PTY_What.INVITES | INT_PTY_What.SEARCH)) != 0)
		{
			m_Dirty = true;
			if (m_AnimLeft <= 0)
				Refresh(true);
		}
		if ((what & INT_PTY_What.BLOCK) != 0)
			RepaintBlock(true);
	}

	override void Tick(float dt)
	{
		if (m_ScIn)
			m_ScIn.Tick(dt);
		if (m_ScOut)
			m_ScOut.Tick(dt);
		if (m_AnimLeft > 0)
		{
			m_AnimLeft = m_AnimLeft - dt;
			if (m_AnimLeft <= 0 && m_Dirty)
				Refresh(true);
		}
		m_TimeAcc = m_TimeAcc + dt;
		if (m_TimeAcc < 1.0)
			return;
		m_TimeAcc = 0;
		for (int i = 0; i < m_InRows.Count(); i++)
		{
			if (m_ScIn && m_ScIn.SlotIndex(i) >= 0)
				RepaintWhen(m_InRows.Get(i));
		}
		for (int j = 0; j < m_OutRows.Count(); j++)
		{
			if (m_ScOut && m_ScOut.SlotIndex(j) >= 0)
				RepaintWhen(m_OutRows.Get(j));
		}
		RepaintBlock(false);
	}

	override string TestInfo()
	{
		string mode = "in";
		if (m_OutMode)
			mode = "out";
		int inRows = 0;
		int outRows = 0;
		for (int i = 0; i < m_InRows.Count(); i++)
		{
			int ii = -1;
			if (m_ScIn)
				ii = m_ScIn.SlotIndex(i);
			if (ii >= 0 && m_ScIn.ItemFade(ii) > 0.5)
				inRows++;
		}
		for (int j = 0; j < m_OutRows.Count(); j++)
		{
			int oi = -1;
			if (m_ScOut)
				oi = m_ScOut.SlotIndex(j);
			if (oi >= 0 && m_ScOut.ItemFade(oi) > 0.5)
				outRows++;
		}
		string r = "invites mode=" + mode + " in=" + InCount().ToString() + " out=" + OutCount().ToString();
		r = r + " inrows=" + inRows.ToString() + " outrows=" + outRows.ToString();
		return r;
	}
}
