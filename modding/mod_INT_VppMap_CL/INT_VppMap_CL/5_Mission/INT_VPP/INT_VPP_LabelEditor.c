// ============================================================================
// VanillaPPMap (our VPP fork) - admin text label editor island (owner request 2026-11-13). Tab «05 Надписи» (map
// admins only), at the place and in the look of the zone editor island (opaque, orange top line, kit controls):
//   list mode   labels (colour, text, size) -> click = select + centre; LMB on a label ON THE MAP = select it;
//               «Новая надпись»
//   edit mode   text (one line, 48 characters / 96 bytes), 10 colours (the marker / zone palette + white), 6 sizes
//               (14 18 24 32 40 52 design px), place = LMB on the map (also moves an existing label), live preview on
//               the map (INT_VPP_MapView.INT_SetLabelDraft), Сохранить / Удалить (confirm band) / Назад
// Writes go through INT_VPP_Client.INT_RequestLabelSave / INT_RequestLabelDelete (the server re-checks admin, rate,
// text, colour, size, position, limit); a pending request ends only with its own INT_LabelResult (kind 3 / 4).
// Shown only when INT_VPP_Client.IsAdmin(). INT_VPP_Stat kind 9 (the island). Client only.
// ============================================================================

class INT_VPP_LabelEditor : ScriptedWidgetEventHandler
{
	static const float INT_ISL_W = 440;
	static const float INT_ROW_H = 36;
	static const float INT_PENDING_TIMEOUT = 6.0;
	static const int INT_GREEN = 0xFF56E07E;
	static const int INT_DEF_COLOR_IDX = 9;

	protected VPPMapMenu m_INT_Menu;
	protected INT_VPP_MapView m_INT_View;
	protected Widget m_INT_Screen;
	protected float m_INT_ScrW;
	protected float m_INT_ScrH;
	protected float m_INT_IslH;

	protected Widget m_INT_Root;
	protected Widget m_INT_Body;
	protected ButtonWidget m_INT_Block;
	protected ref INT_VPP_IconBtn m_INT_CloseBtn;

	// list mode
	protected ref array<ButtonWidget> m_INT_RowHits;
	protected ref array<int> m_INT_RowIds;
	protected int m_INT_Scroll;
	protected int m_INT_VisibleRows;
	protected ButtonWidget m_INT_ListWheel;
	protected ref INT_VPPX_HButton m_INT_BtnNew;

	// edit mode: -1 = list, 0 = new, > 0 = the label id
	protected int m_INT_EditId = -1;
	protected bool m_INT_HasPos;
	protected vector m_INT_Pos;
	protected int m_INT_Color;
	protected int m_INT_Size;
	protected string m_INT_InitText;
	protected ref INT_VPPX_HInput m_INT_Text;
	protected TextWidget m_INT_Count;
	protected ref array<ButtonWidget> m_INT_SwHits;
	protected Widget m_INT_SwSel;
	protected ref INT_VPPX_HSegment m_INT_SizeSeg;
	protected TextWidget m_INT_PlaceText;
	protected ref INT_VPPX_HLines m_INT_Hint;
	protected ref INT_VPPX_HButton m_INT_BtnSave;
	protected ref INT_VPPX_HButton m_INT_BtnDel;
	protected ref INT_VPPX_HButton m_INT_BtnBack;

	// status (both modes)
	protected TextWidget m_INT_Status;
	protected string m_INT_StatusText;
	protected int m_INT_StatusColor;
	// pending request: 1 save / 2 delete, the label id asked for (0 = new), the result revision when sent
	protected int m_INT_Pending;
	protected int m_INT_PendId;
	protected int m_INT_PendRev;
	protected float m_INT_PendingAge;
	protected bool m_INT_BodyDirty;
	protected bool m_INT_ResultBound;

	void INT_VPP_LabelEditor()
	{
		m_INT_RowHits = new array<ButtonWidget>();
		m_INT_RowIds = new array<int>();
		m_INT_SwHits = new array<ButtonWidget>();
		int c = INT_VPP_Net.INT_LabelColorAt(INT_DEF_COLOR_IDX);
		m_INT_Color = c;
		m_INT_Size = INT_VPP_Net.INT_LABEL_DEF_SIZE;
	}

	// ------------------------------------------------------------------ lifecycle
	void INT_Build(Widget screen, float scrW, float scrH, VPPMapMenu menu, INT_VPP_MapView view)
	{
		m_INT_Screen = screen;
		m_INT_ScrW = scrW;
		m_INT_ScrH = scrH;
		m_INT_Menu = menu;
		m_INT_View = view;
		m_INT_IslH = scrH - INT_VPP_Ui.INT_LAYERS_TOP - 8 - 130;
		m_INT_Root = INT_VPP_Ui.INT_Panel(screen, 24, 130, INT_ISL_W, m_INT_IslH, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, true, "IntVppLabelEd");
		if (!m_INT_Root)
			return;
		INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_LABEL_EDITOR);
		m_INT_Block = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, INT_ISL_W, m_INT_IslH, this);
		m_INT_CloseBtn = new INT_VPP_IconBtn();
		string closeTex = INT_VPPX_Look.IconPath("close");
		m_INT_CloseBtn.INT_Build(m_INT_Root, INT_ISL_W - 16 - 32, 10, 32, closeTex, 14, INT_VPPX_Look.TEXT);
		m_INT_CloseBtn.INT_OnClick().Insert(INT_OnClose);
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		m_INT_EditId = -1;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl && cl.m_INT_OnLabelResult)
		{
			cl.m_INT_OnLabelResult.Insert(INT_OnResultEvt);
			m_INT_ResultBound = true;
		}
		INT_BuildBody();
	}

	// hidden now, unlinked through the menu's graveyard (body first, then the island)
	// the island and its blocker (the drag handle of the map screen, 2026-11-14)
	Widget INT_GetRoot()
	{
		return m_INT_Root;
	}

	ButtonWidget INT_GetBlock()
	{
		return m_INT_Block;
	}

	void INT_Destroy()
	{
		if (m_INT_ResultBound)
		{
			m_INT_ResultBound = false;
			INT_VPP_Client cl = INT_VPP_Client.Peek();
			if (cl && cl.m_INT_OnLabelResult)
				cl.m_INT_OnLabelResult.Remove(INT_OnResultEvt);
		}
		m_INT_Pending = 0;
		if (m_INT_View)
			m_INT_View.INT_SetLabelDraft(false, false, vector.Zero, "", 0, 0, -1);
		if (m_INT_CloseBtn)
			m_INT_CloseBtn.INT_OnClick().Clear();
		INT_DropBody();
		if (m_INT_Root)
		{
			Widget r = m_INT_Root;
			m_INT_Root = null;
			INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_LABEL_EDITOR);
			if (m_INT_Menu)
				m_INT_Menu.INT_Bury(r);
			else
				r.Unlink();
		}
		m_INT_Menu = null;
		m_INT_View = null;
	}

	bool INT_IsOpen()
	{
		if (m_INT_Root)
			return true;
		return false;
	}

	bool INT_IsEditing()
	{
		if (m_INT_EditId >= 0)
			return true;
		return false;
	}

	bool INT_IsTyping()
	{
		if (m_INT_Text && m_INT_Text.IsFocused())
			return true;
		return false;
	}

	void INT_Blur()
	{
		if (m_INT_Text)
			m_INT_Text.Blur();
	}

	// Esc: editing -> back to the list; list -> false (the menu closes us)
	bool INT_Back()
	{
		if (m_INT_EditId < 0)
			return false;
		INT_BackToList();
		return true;
	}

	void INT_Tick(float dt)
	{
		if (m_INT_Pending != 0)
		{
			m_INT_PendingAge = m_INT_PendingAge + dt;
			if (m_INT_PendingAge > INT_PENDING_TIMEOUT)
			{
				m_INT_Pending = 0;
				INT_SetStatus("Сервер не ответил", INT_VPPX_Look.RED);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			}
		}
		if (m_INT_BodyDirty)
		{
			m_INT_BodyDirty = false;
			INT_BuildBody();
		}
	}

	// the complete label list arrived (m_INT_OnLabels): never a success by itself
	void INT_OnLabels()
	{
		if (m_INT_EditId > 0)
		{
			bool ownDelete = false;
			if (m_INT_Pending == 2 && m_INT_PendId == m_INT_EditId)
				ownDelete = true;
			INT_VPP_Client cl = INT_VPP_Client.Peek();
			if (!ownDelete && cl && !cl.INT_FindLabel(m_INT_EditId))
			{
				m_INT_Pending = 0;
				m_INT_EditId = -1;
				INT_PushDraft();
				INT_SetStatusLater("Надпись удалена другим администратором", INT_VPPX_Look.RED);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
			}
		}
		// in edit mode the body stays (typed text); the list follows the data
		if (m_INT_EditId < 0)
			m_INT_BodyDirty = true;
	}

	// INT_LabelResult: only the answer to OUR pending request counts (same kind, after ours, our id; a new label: any
	// id on success, 0 on a refusal; a refusal of an unread request also carries 0)
	void INT_OnResultEvt()
	{
		if (m_INT_Pending == 0 || !m_INT_Root)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		int rev = cl.INT_LabelResultsRevision();
		if (rev <= m_INT_PendRev)
			return;
		int want = INT_VPP_Net.INT_RESULT_KIND_LABEL_SAVE;
		if (m_INT_Pending == 2)
			want = INT_VPP_Net.INT_RESULT_KIND_LABEL_DELETE;
		int kind = cl.INT_LabelResultKind();
		if (kind != want)
			return;
		bool ok = cl.INT_LabelResultOk();
		int rid = cl.INT_LabelResultId();
		bool ours = false;
		if (ok)
		{
			if (rid == m_INT_PendId)
				ours = true;
			if (m_INT_Pending == 1 && m_INT_PendId == 0 && rid > 0)
				ours = true;
		}
		else
		{
			if (rid == m_INT_PendId || rid == 0)
				ours = true;
		}
		if (!ours)
			return;
		int pk = m_INT_Pending;
		m_INT_Pending = 0;
		if (!ok)
		{
			string err = cl.INT_LabelError();
			string ru = INT_VPP_LabelEditor.INT_ReasonRu(err);
			INT_SetStatus(ru, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		m_INT_EditId = -1;
		INT_PushDraft();
		if (pk == 2)
			INT_SetStatusLater("Удалено", INT_GREEN);
		else
			INT_SetStatusLater("Сохранено", INT_GREEN);
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
		m_INT_BodyDirty = true;
	}

	protected void INT_MarkPending(int kind, int id)
	{
		m_INT_Pending = kind;
		m_INT_PendId = id;
		m_INT_PendingAge = 0;
		m_INT_PendRev = 0;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
			m_INT_PendRev = cl.INT_LabelResultsRevision();
	}

	// ------------------------------------------------------------------ TEST hooks
	void INT_TestPending(int kind, int id)
	{
		INT_MarkPending(kind, id);
	}

	int INT_PendingKind()
	{
		return m_INT_Pending;
	}

	string INT_StatusText()
	{
		return m_INT_StatusText;
	}

	int INT_GetEditId()
	{
		return m_INT_EditId;
	}

	bool INT_HasPlace()
	{
		return m_INT_HasPos;
	}

	vector INT_GetPlace()
	{
		return m_INT_Pos;
	}

	void INT_TestStartNew()
	{
		INT_StartNew();
	}

	// the editor's fields as typed / clicked by a player (text through the input, colour / size by index)
	bool INT_TestSetFields(string text, int colorIdx, int sizeIdx)
	{
		if (m_INT_EditId < 0)
			return false;
		if (m_INT_Text)
			m_INT_Text.SetText(text);
		int c = INT_VPP_Net.INT_LabelColorAt(colorIdx);
		if (c != 0)
			m_INT_Color = c;
		int sz = INT_VPP_Net.INT_LabelSizeAt(sizeIdx);
		if (sz > 0)
			m_INT_Size = sz;
		INT_PaintSwatch();
		if (m_INT_SizeSeg)
		{
			int si = INT_VPP_Net.INT_LabelSizeIndex(m_INT_Size);
			m_INT_SizeSeg.Set(si);
		}
		INT_PaintCount();
		INT_PushDraft();
		return true;
	}

	// a map click at a world point, as the menu passes it
	bool INT_TestClick(vector world)
	{
		float ax = 0;
		float ay = 0;
		if (m_INT_View)
			m_INT_View.INT_ToScreen(world, ax, ay);
		bool used = INT_OnMapClick(world, ax, ay);
		return used;
	}

	void INT_TestSave()
	{
		INT_Save();
	}

	// Enter outside the text field (VPPMapMenu.INT_Keys)
	void INT_SaveFromKey()
	{
		if (m_INT_EditId < 0)
			return;
		INT_Save();
	}

	void INT_TestBack()
	{
		INT_BackToList();
	}

	// ------------------------------------------------------------------ status
	protected void INT_SetStatus(string text, int color)
	{
		m_INT_StatusText = text;
		m_INT_StatusColor = color;
		if (!m_INT_Status)
			return;
		string t = text;
		if (t == "")
			t = " ";
		m_INT_Status.SetText(t);
		m_INT_Status.SetColor(color);
	}

	// kept across a body rebuild
	protected void INT_SetStatusLater(string text, int color)
	{
		m_INT_StatusText = text;
		m_INT_StatusColor = color;
	}

	static string INT_ReasonRu(string reason)
	{
		if (reason == "empty text")
			return "Введите текст надписи";
		if (reason == "bad color")
			return "Цвет не из палитры";
		if (reason == "bad size")
			return "Такого размера нет";
		if (reason == "position outside world")
			return "Точка за пределами карты";
		if (reason == "label limit")
			return "Достигнут предел надписей";
		if (reason == "labels read-only")
			return "Надписи только для чтения: файл от новой версии мода";
		if (reason == "unknown id")
			return "Такой надписи на сервере уже нет";
		if (reason == "null label")
			return "Неверные данные надписи";
		if (reason == "no place")
			return "Поставьте надпись на карту (ЛКМ)";
		// shared with the zone editor: not admin, too fast, no transport, bad id, read failed, refused, protocol, save failed
		return INT_VPP_ZoneEditor.INT_ReasonRu(reason);
	}

	// ------------------------------------------------------------------ body (rebuilt on mode / data change)
	protected void INT_DropBody()
	{
		if (m_INT_SizeSeg)
			m_INT_SizeSeg.OnChanged().Clear();
		if (m_INT_Text)
		{
			m_INT_Text.OnChanged().Clear();
			m_INT_Text.OnSubmit().Clear();
		}
		m_INT_RowHits.Clear();
		m_INT_RowIds.Clear();
		m_INT_SwHits.Clear();
		m_INT_ListWheel = null;
		m_INT_Status = null;
		m_INT_Count = null;
		m_INT_PlaceText = null;
		m_INT_SwSel = null;
		m_INT_Text = null;
		m_INT_SizeSeg = null;
		m_INT_Hint = null;
		m_INT_BtnNew = null;
		m_INT_BtnSave = null;
		m_INT_BtnDel = null;
		m_INT_BtnBack = null;
		if (!m_INT_Body)
			return;
		Widget b = m_INT_Body;
		m_INT_Body = null;
		if (m_INT_Menu)
			m_INT_Menu.INT_Bury(b);
		else
			b.Unlink();
	}

	protected void INT_BuildBody()
	{
		string keepText = m_INT_InitText;
		if (m_INT_Text)
			keepText = m_INT_Text.GetText();
		INT_DropBody();
		if (!m_INT_Root)
			return;
		m_INT_Body = INT_UIF_Draw.Frame(m_INT_Root, 0, 0, INT_ISL_W, m_INT_IslH, "IntVppLabelBody");
		if (!m_INT_Body)
			return;
		if (m_INT_EditId < 0)
			INT_BuildList();
		else
			INT_BuildEdit(keepText);
		INT_VPP_Ui.INT_Sort(m_INT_Body);
		INT_SetStatus(m_INT_StatusText, m_INT_StatusColor);
	}

	protected void INT_BuildList()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		array<ref INT_VPP_Label> labels = null;
		int limit = INT_VPP_Net.INT_DEF_MAX_LABELS;
		if (cl)
		{
			labels = cl.INT_GetLabels();
			limit = cl.INT_LabelLimit();
		}
		int n = 0;
		if (labels)
			n = labels.Count();
		string cap = INT_VPPX_Text.Caps("Надписи");
		INT_VPPX_Look.Text(m_INT_Body, 18, 14, 200, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		string cnt = n.ToString() + " из " + limit.ToString();
		INT_VPPX_Look.Text(m_INT_Body, INT_ISL_W - 16 - 32 - 12 - 120, 14, 120, 20, cnt, 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
		INT_VPPX_Look.Fill(m_INT_Body, 18, 50, INT_ISL_W - 36, 1, INT_VPPX_Look.LINE);
		float listTop = 58;
		float listBottom = m_INT_IslH - 120;
		m_INT_VisibleRows = (listBottom - listTop) / INT_ROW_H;
		if (m_INT_VisibleRows < 1)
			m_INT_VisibleRows = 1;
		m_INT_ListWheel = INT_UIF_Draw.Hit(m_INT_Body, 0, listTop, INT_ISL_W, listBottom - listTop, this);
		int maxScroll = n - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		if (m_INT_Scroll > maxScroll)
			m_INT_Scroll = maxScroll;
		if (n == 0)
			INT_VPPX_Look.Text(m_INT_Body, 18, listTop + 8, INT_ISL_W - 36, 24, "Надписей пока нет", 15, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		int last = m_INT_Scroll + m_INT_VisibleRows;
		if (last > n)
			last = n;
		float y = listTop;
		for (int i = m_INT_Scroll; i < last; i++)
		{
			INT_VPP_Label l = labels.Get(i);
			if (!l)
				continue;
			int col = INT_VPP_Validate.ForceAlpha(l.INT_Color);
			INT_VPPX_Look.Diamond(m_INT_Body, 28, y + 18, 9, true, col);
			string nm = INT_VPPX_Look.FitText(l.INT_Text, 15, false, 280);
			INT_VPPX_Look.Text(m_INT_Body, 44, y + 6, 300, 24, nm, 15, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			int lsz = l.INT_Size;
			string sz = lsz.ToString() + " px";
			INT_VPPX_Look.Text(m_INT_Body, INT_ISL_W - 18 - 70, y + 8, 70, 20, sz, 12, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Body, 12, y, INT_ISL_W - 24, INT_ROW_H, this);
			m_INT_RowHits.Insert(hit);
			m_INT_RowIds.Insert(l.INT_Id);
			y = y + INT_ROW_H;
		}
		m_INT_Status = INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 108, INT_ISL_W - 36, 22, " ", 14, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		m_INT_BtnNew = new INT_VPPX_HButton();
		m_INT_BtnNew.Build(m_INT_Body, 18, m_INT_IslH - 76, INT_ISL_W - 36, 40, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnNew.SetText("Новая надпись");
		m_INT_BtnNew.OnClick().Insert(INT_OnButton);
		INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 30, INT_ISL_W - 36, 18, "ЛКМ по надписи на карте — изменить её", 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		if (n >= limit)
			m_INT_BtnNew.SetEnabled(false);
	}

	protected void INT_BuildEdit(string keepText)
	{
		string title = "Новая надпись";
		if (m_INT_EditId > 0)
			title = "Надпись № " + m_INT_EditId.ToString();
		string cap = INT_VPPX_Text.Caps(title);
		INT_VPPX_Look.Text(m_INT_Body, 18, 14, 300, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		INT_VPPX_Look.Fill(m_INT_Body, 18, 50, INT_ISL_W - 36, 1, INT_VPPX_Look.LINE);
		// text
		string tcap = INT_VPPX_Text.Caps("Текст");
		INT_VPPX_Look.Text(m_INT_Body, 18, 62, 200, 18, tcap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_Count = INT_VPPX_Look.Text(m_INT_Body, INT_ISL_W - 18 - 120, 62, 120, 18, " ", 11, INT_VPP_Ui.D, INT_UIF_Align.RIGHT, false);
		m_INT_Text = new INT_VPPX_HInput();
		m_INT_Text.Build(m_INT_Body, 18, 80, INT_ISL_W - 36, "Например: Северный аэродром", INT_VPP_Net.INT_LABEL_TEXT_MAX_CHARS, "pencil");
		m_INT_Text.SetText(keepText);
		m_INT_Text.OnChanged().Insert(INT_OnTextChanged);
		m_INT_Text.OnSubmit().Insert(INT_OnTextSubmit);
		// colour: the marker / zone palette + white
		string ccap = INT_VPPX_Text.Caps("Цвет");
		INT_VPPX_Look.Text(m_INT_Body, 18, 136, 200, 18, ccap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_SwSel = INT_UIF_Draw.Frame(m_INT_Body, 0, 156, 30, 44, "IntVppLSwSel");
		if (m_INT_SwSel)
		{
			INT_VPPX_Look.Outline(m_INT_SwSel, 0, 0, 30, 30, INT_VPPX_Look.WHITE);
			INT_VPPX_Look.Diamond(m_INT_SwSel, 15, 37, 6, true, INT_VPPX_Look.ORANGE);
		}
		for (int i = 0; i < INT_VPP_Net.INT_LABEL_COLOR_COUNT; i++)
		{
			float sx = 18 + i * 38;
			int c = INT_VPP_Net.INT_LabelColorAt(i);
			INT_VPPX_Look.Fill(m_INT_Body, sx + 5, 161, 20, 20, c);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Body, sx, 156, 30, 30, this);
			m_INT_SwHits.Insert(hit);
		}
		INT_PaintSwatch();
		// size
		string scap = INT_VPPX_Text.Caps("Размер");
		INT_VPPX_Look.Text(m_INT_Body, 18, 214, 200, 18, scap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		array<string> sizes = new array<string>();
		for (int s = 0; s < INT_VPP_Net.INT_LABEL_SIZE_COUNT; s++)
		{
			int sv = INT_VPP_Net.INT_LabelSizeAt(s);
			sizes.Insert(sv.ToString());
		}
		int sel = INT_VPP_Net.INT_LabelSizeIndex(m_INT_Size);
		m_INT_SizeSeg = new INT_VPPX_HSegment();
		m_INT_SizeSeg.Build(m_INT_Body, 18, 234, sizes, sel, INT_ISL_W - 36, 40, true, false);
		m_INT_SizeSeg.OnChanged().Insert(INT_OnSize);
		INT_VPPX_Look.Fill(m_INT_Body, 18, 290, INT_ISL_W - 36, 1, INT_VPPX_Look.LINE);
		// place
		string pcap = INT_VPPX_Text.Caps("Место");
		INT_VPPX_Look.Text(m_INT_Body, 18, 302, 200, 18, pcap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_PlaceText = INT_VPPX_Look.Text(m_INT_Body, 18, 322, INT_ISL_W - 36, 24, " ", 16, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_Hint = new INT_VPPX_HLines();
		m_INT_Hint.Build(m_INT_Body, 18, 352, INT_ISL_W - 36, 20, 13, INT_VPP_Ui.T2, 3);
		// status + buttons
		m_INT_Status = INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 120, INT_ISL_W - 36, 22, " ", 14, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		float by = m_INT_IslH - 86;
		m_INT_BtnSave = new INT_VPPX_HButton();
		m_INT_BtnSave.Build(m_INT_Body, 18, by, 170, 50, INT_VPPX_HButton.TONE_PRIMARY);
		m_INT_BtnSave.SetText("Сохранить");
		m_INT_BtnSave.OnClick().Insert(INT_OnButton);
		float bx = 18 + 170 + 8;
		if (m_INT_EditId > 0)
		{
			m_INT_BtnDel = new INT_VPPX_HButton();
			m_INT_BtnDel.Build(m_INT_Body, bx, by, 120, 50, INT_VPPX_HButton.TONE_DANGER);
			m_INT_BtnDel.SetText("Удалить");
			m_INT_BtnDel.OnClick().Insert(INT_OnButton);
			bx = bx + 120 + 8;
		}
		m_INT_BtnBack = new INT_VPPX_HButton();
		m_INT_BtnBack.Build(m_INT_Body, bx, by, INT_ISL_W - 18 - bx, 50, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnBack.SetText("Назад");
		m_INT_BtnBack.OnClick().Insert(INT_OnButton);
		INT_PaintCount();
		INT_PaintPlace();
		INT_PushDraft();
	}

	protected void INT_PaintSwatch()
	{
		if (!m_INT_SwSel)
			return;
		int at = INT_VPP_Net.INT_LabelColorIndex(m_INT_Color);
		if (at < 0)
		{
			m_INT_SwSel.Show(false);
			return;
		}
		INT_UIF_Draw.Move(m_INT_SwSel, 18 + at * 38, 156);
		m_INT_SwSel.Show(true);
	}

	// "12 / 48" characters (the server clips to 48 characters and 96 bytes)
	protected void INT_PaintCount()
	{
		if (!m_INT_Count)
			return;
		string t = "";
		if (m_INT_Text)
			t = m_INT_Text.GetText();
		int n = t.LengthUtf8();
		int mx = INT_VPP_Net.INT_LABEL_TEXT_MAX_CHARS;
		string s = n.ToString() + " / " + mx.ToString();
		m_INT_Count.SetText(s);
	}

	protected void INT_PaintPlace()
	{
		string line = "не задано";
		string hint = "ЛКМ по карте — поставить надпись.";
		if (m_INT_HasPos)
		{
			line = INT_VPP_Fmt.INT_Coords(m_INT_Pos);
			hint = "ЛКМ по карте — перенести. Изменения видны на карте сразу, сохраняются кнопкой «Сохранить».";
		}
		if (m_INT_PlaceText)
			m_INT_PlaceText.SetText(line);
		if (m_INT_Hint)
			m_INT_Hint.Set(hint);
	}

	// the live preview on the map (the edited label is hidden from the layer meanwhile)
	void INT_PushDraft()
	{
		if (!m_INT_View)
			return;
		if (m_INT_EditId < 0)
		{
			m_INT_View.INT_SetLabelDraft(false, false, vector.Zero, "", 0, 0, -1);
			return;
		}
		string t = m_INT_InitText;
		if (m_INT_Text)
			t = m_INT_Text.GetText();
		int skip = m_INT_EditId;
		if (skip <= 0)
			skip = -1;
		m_INT_View.INT_SetLabelDraft(true, m_INT_HasPos, m_INT_Pos, t, m_INT_Color, m_INT_Size, skip);
	}

	// ------------------------------------------------------------------ mode switches
	protected void INT_StartNew()
	{
		m_INT_EditId = 0;
		m_INT_HasPos = false;
		m_INT_Pos = vector.Zero;
		m_INT_InitText = "";
		m_INT_Text = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
	}

	void INT_SelectLabel(int id)
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		INT_VPP_Label l = cl.INT_FindLabel(id);
		if (!l)
			return;
		m_INT_EditId = l.INT_Id;
		vector lp = l.INT_Pos();
		m_INT_Pos = lp;
		m_INT_HasPos = true;
		int col = INT_VPP_Validate.ForceAlpha(l.INT_Color);
		m_INT_Color = col;
		m_INT_Size = l.INT_Size;
		m_INT_InitText = l.INT_Text;
		m_INT_Text = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
		if (m_INT_Menu)
			m_INT_Menu.INT_CenterOn(lp);
	}

	protected void INT_BackToList()
	{
		m_INT_EditId = -1;
		m_INT_Text = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
	}

	// ------------------------------------------------------------------ map input (from the menu)
	// editing: LMB = the place (new or moved); list: LMB on a label of the map = edit it. false = not ours.
	bool INT_OnMapClick(vector world, float ax, float ay)
	{
		if (m_INT_EditId >= 0)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			vector p = world;
			p[1] = 0;
			m_INT_Pos = p;
			m_INT_HasPos = true;
			INT_PaintPlace();
			INT_PushDraft();
			return true;
		}
		if (!m_INT_View)
			return false;
		int hit = m_INT_View.INT_LabelHitTest(ax, ay);
		if (hit <= 0)
			return false;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
		INT_SelectLabel(hit);
		return true;
	}

	// RMB while editing does nothing (no route point under the editor)
	bool INT_OnMapRmb()
	{
		if (m_INT_EditId < 0)
			return false;
		return true;
	}

	// ------------------------------------------------------------------ save / delete
	protected void INT_Save()
	{
		if (m_INT_Pending != 0 || m_INT_EditId < 0)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
		{
			INT_SetStatus("Нет связи с сервером", INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		if (!m_INT_HasPos)
		{
			string ruPos = INT_VPP_LabelEditor.INT_ReasonRu("no place");
			INT_SetStatus(ruPos, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		string t = "";
		if (m_INT_Text)
			t = m_INT_Text.GetText();
		// every value into a local first, then plain assignments into the request's fields
		string clean = INT_VPP_Validate.INT_LabelText(t);
		int col = INT_VPP_Validate.ForceAlpha(m_INT_Color);
		int sz = m_INT_Size;
		vector p = m_INT_Pos;
		float px = p[0];
		float pz = p[2];
		int eid = m_INT_EditId;
		INT_VPP_Label l = new INT_VPP_Label();
		l.INT_Id = eid;
		l.INT_Text = clean;
		l.INT_Color = col;
		l.INT_Size = sz;
		l.INT_X = px;
		l.INT_Z = pz;
		// the same check as the server, on a copy
		INT_VPP_Label probe = l.INT_Clone();
		string reason = "";
		bool ok = INT_VPP_Validate.INT_LabelLimits(probe, reason);
		if (!ok)
		{
			string ru = INT_VPP_LabelEditor.INT_ReasonRu(reason);
			INT_SetStatus(ru, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		// pending before the send: a test transport may answer inside the call
		INT_MarkPending(1, l.INT_Id);
		bool sent = cl.INT_RequestLabelSave(l);
		if (!sent)
		{
			m_INT_Pending = 0;
			string err = cl.INT_LabelError();
			string ru2 = INT_VPP_LabelEditor.INT_ReasonRu(err);
			INT_SetStatus(ru2, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
		if (m_INT_Pending != 0)
			INT_SetStatus("Отправлено…", INT_VPP_Ui.M);
	}

	// after the confirm band (VPPMapMenu)
	void INT_SendDelete(int id)
	{
		if (m_INT_Pending != 0)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		INT_MarkPending(2, id);
		bool sent = cl.INT_RequestLabelDelete(id);
		if (!sent)
		{
			m_INT_Pending = 0;
			string err = cl.INT_LabelError();
			string ru = INT_VPP_LabelEditor.INT_ReasonRu(err);
			INT_SetStatus(ru, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		if (m_INT_Pending != 0)
			INT_SetStatus("Отправлено…", INT_VPP_Ui.M);
	}

	// ------------------------------------------------------------------ events
	void INT_OnClose(INT_VPP_IconBtn b)
	{
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
		if (m_INT_Menu)
			m_INT_Menu.INT_CloseLabelEditor();
	}

	void INT_OnTextChanged(string text, INT_VPPX_HInput i)
	{
		INT_PaintCount();
		INT_PushDraft();
	}

	void INT_OnTextSubmit(string text, INT_VPPX_HInput i)
	{
		INT_Save();
	}

	void INT_OnSize(int index, INT_VPPX_HSegment s)
	{
		int sz = INT_VPP_Net.INT_LabelSizeAt(index);
		if (sz <= 0)
			return;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
		m_INT_Size = sz;
		INT_PushDraft();
	}

	void INT_OnButton(INT_VPPX_HButton b)
	{
		if (!m_INT_Menu)
			return;
		if (b == m_INT_BtnNew)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			INT_StartNew();
		}
		else if (b == m_INT_BtnSave)
		{
			INT_Save();
		}
		else if (b == m_INT_BtnDel)
		{
			string t = m_INT_InitText;
			if (m_INT_Text)
				t = m_INT_Text.GetText();
			m_INT_Menu.INT_AskLabelDelete(m_INT_EditId, t);
		}
		else if (b == m_INT_BtnBack)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
			INT_BackToList();
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
		{
			if (w == m_INT_Block || w == m_INT_ListWheel)
				return true;
			return false;
		}
		ButtonWidget bw = ButtonWidget.Cast(w);
		int row = m_INT_RowHits.Find(bw);
		if (row >= 0)
		{
			int id = m_INT_RowIds.Get(row);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
			INT_SelectLabel(id);
			return true;
		}
		int sw = m_INT_SwHits.Find(bw);
		if (sw >= 0)
		{
			int c = INT_VPP_Net.INT_LabelColorAt(sw);
			m_INT_Color = c;
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
			INT_PaintSwatch();
			INT_PushDraft();
			return true;
		}
		if (w == m_INT_Block || w == m_INT_ListWheel)
			return true;
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		if (w == m_INT_Block || w == m_INT_ListWheel)
			return true;
		if (m_INT_RowHits.Find(bw) >= 0 || m_INT_SwHits.Find(bw) >= 0)
			return true;
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		ButtonWidget bw = ButtonWidget.Cast(w);
		bool inList = false;
		if (w == m_INT_ListWheel || m_INT_RowHits.Find(bw) >= 0)
			inList = true;
		if (!inList)
		{
			if (w == m_INT_Block)
				return true;
			return false;
		}
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		int n = 0;
		if (cl)
			n = cl.INT_LabelsCount();
		int maxScroll = n - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		int next = m_INT_Scroll - wheel;
		if (next < 0)
			next = 0;
		if (next > maxScroll)
			next = maxScroll;
		if (next != m_INT_Scroll)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			m_INT_Scroll = next;
			m_INT_BodyDirty = true;
		}
		return true;
	}
}
