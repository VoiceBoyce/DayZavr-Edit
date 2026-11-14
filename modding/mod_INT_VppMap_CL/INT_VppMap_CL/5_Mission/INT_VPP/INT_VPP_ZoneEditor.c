// ============================================================================
// VanillaPPMap (our VPP fork) - admin zone editor island (MAP agent, phase 2, 2026-11-12). No concept frame of its
// own: built in the concept B style (opaque island under the top-left one, orange top line, kit controls).
//   list mode   zones (colour, name, shape) -> click = select + centre; «Новый многоугольник» / «Новый круг»
//   edit mode   name, sub-line, 9 colours, geometry drawn on the map (polygon: LMB vertices, click the first one or
//               «Готово» closes, RMB removes the last; circle: LMB centre, LMB radius), Сохранить / Удалить / Назад
// Writes go through INT_VPP_Client.RequestZoneSave / RequestZoneDelete (the server re-checks admin + limits and
// answers only by broadcasting the list: OnZones). Shown only when INT_VPP_Client.IsAdmin(). INT_VPP_Stat kind 5.
// The draft is drawn by INT_VPP_MapView (live canvas); the edited zone is hidden from the zone layer meanwhile.
// Client only.
// ============================================================================

class INT_VPP_ZoneEditor : ScriptedWidgetEventHandler
{
	static const float ISL_W = 440;
	static const float ROW_H = 36;
	static const float CLOSE_PX = 14;
	static const float PENDING_TIMEOUT = 6.0;
	static const int NAME_CHARS = 32;
	static const int SUB_CHARS = 48;
	static const int GREEN = 0xFF56E07E;

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
	protected ref INT_VPPX_HButton m_INT_BtnNewPoly;
	protected ref INT_VPPX_HButton m_INT_BtnNewCircle;

	// edit mode
	protected int m_INT_EditId = -1;
	protected int m_INT_Shape;
	protected ref array<float> m_INT_Xs;
	protected ref array<float> m_INT_Zs;
	protected float m_INT_CX;
	protected float m_INT_CZ;
	protected float m_INT_R;
	protected bool m_INT_Closed;
	protected int m_INT_Stage;
	protected int m_INT_Color;
	protected string m_INT_InitName;
	protected string m_INT_InitSub;
	protected ref INT_VPPX_HInput m_INT_Name;
	protected ref INT_VPPX_HInput m_INT_Sub;
	protected ref array<int> m_INT_Palette;
	protected ref array<ButtonWidget> m_INT_SwHits;
	protected Widget m_INT_SwSel;
	protected TextWidget m_INT_ShapeText;
	protected ref INT_VPPX_HLines m_INT_Hint;
	protected ref INT_VPPX_HButton m_INT_BtnDone;
	protected ref INT_VPPX_HButton m_INT_BtnRedo;
	protected ref INT_VPPX_HButton m_INT_BtnSave;
	protected ref INT_VPPX_HButton m_INT_BtnDel;
	protected ref INT_VPPX_HButton m_INT_BtnBack;

	// status (both modes)
	protected TextWidget m_INT_Status;
	protected string m_INT_StatusText;
	protected int m_INT_StatusColor;
	protected int m_INT_Pending;
	protected float m_INT_PendingAge;
	protected bool m_INT_BodyDirty;
	// review #10: our own request - the zone id asked for (0 = new) and the results revision when it was sent;
	// only an INT_ZoneResult for it ends the pending state (a list update alone never counts as success)
	protected int m_INT_PendId;
	protected int m_INT_PendRev;
	protected bool m_INT_ResultBound;

	void INT_VPP_ZoneEditor()
	{
		m_INT_RowHits = new array<ButtonWidget>();
		m_INT_RowIds = new array<int>();
		m_INT_Xs = new array<float>();
		m_INT_Zs = new array<float>();
		m_INT_Palette = new array<int>();
		m_INT_SwHits = new array<ButtonWidget>();
		INT_VPP_EditorCard.INT_FillPalette(m_INT_Palette);
		m_INT_Color = m_INT_Palette.Get(1);
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
		m_INT_Root = INT_VPP_Ui.INT_Panel(screen, 24, 130, ISL_W, m_INT_IslH, INT_VPP_Ui.ISL_BG, INT_VPP_Ui.ISL_EDGE, true, "IntVppZoneEd");
		if (!m_INT_Root)
			return;
		INT_VPP_Stat.INT_Up(5);
		m_INT_Block = INT_UIF_Draw.Hit(m_INT_Root, 0, 0, ISL_W, m_INT_IslH, this);
		m_INT_CloseBtn = new INT_VPP_IconBtn();
		string closeTex = INT_VPPX_Look.IconPath("close");
		m_INT_CloseBtn.INT_Build(m_INT_Root, ISL_W - 16 - 32, 10, 32, closeTex, 14, INT_VPPX_Look.TEXT);
		m_INT_CloseBtn.INT_OnClick().Insert(INT_OnClose);
		INT_VPP_Ui.INT_Sort(m_INT_Root);
		m_INT_EditId = -1;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl && cl.OnResult)
		{
			cl.OnResult.Insert(INT_OnResultEvt);
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
			if (cl && cl.OnResult)
				cl.OnResult.Remove(INT_OnResultEvt);
		}
		m_INT_Pending = 0;
		if (m_INT_View)
			m_INT_View.INT_SetDraft(false, 0, null, null, 0, 0, 0, 0, false, 0, -1);
		if (m_INT_CloseBtn)
			m_INT_CloseBtn.INT_OnClick().Clear();
		INT_DropBody();
		if (m_INT_Root)
		{
			Widget r = m_INT_Root;
			m_INT_Root = null;
			INT_VPP_Stat.INT_Down(5);
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

	// map clicks are taken while a shape is being drawn
	bool INT_IsDrawing()
	{
		if (m_INT_EditId < 0)
			return false;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_Stage < 2)
				return true;
			return false;
		}
		if (!m_INT_Closed)
			return true;
		return false;
	}

	bool INT_IsTyping()
	{
		if (m_INT_Name && m_INT_Name.IsFocused())
			return true;
		if (m_INT_Sub && m_INT_Sub.IsFocused())
			return true;
		return false;
	}

	void INT_Blur()
	{
		if (m_INT_Name)
			m_INT_Name.Blur();
		if (m_INT_Sub)
			m_INT_Sub.Blur();
	}

	// Esc: drawing -> stop drawing the open part; editing -> back to the list; list -> false (the menu closes us)
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
			if (m_INT_PendingAge > PENDING_TIMEOUT)
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

	// complete zone list arrived (OnZones). Never a success by itself (review #10): our save / delete ends only
	// with its own INT_ZoneResult (INT_OnResultEvt)
	void INT_OnZones()
	{
		if (m_INT_EditId > 0)
		{
			// the zone we edit was removed by another admin (not by our own pending delete): back to the list
			bool ownDelete = false;
			if (m_INT_Pending == 2 && m_INT_PendId == m_INT_EditId)
				ownDelete = true;
			INT_VPP_Client cl = INT_VPP_Client.Peek();
			if (!ownDelete && cl && !cl.FindZone(m_INT_EditId))
			{
				m_INT_Pending = 0;
				m_INT_EditId = -1;
				INT_PushDraft();
				INT_SetStatusLater("Зона удалена другим администратором", INT_VPPX_Look.RED);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
			}
		}
		m_INT_BodyDirty = true;
	}

	// INT_ZoneResult arrived (INT_VPP_Client.OnResult): only the answer to OUR pending request counts - the same kind,
	// sent after we sent ours, and our zone id (a new zone: any id on success, 0 on a refusal; a refusal of an
	// unread request also carries 0). Refused: pending cleared, the server's reason in Russian, still editing.
	void INT_OnResultEvt()
	{
		if (m_INT_Pending == 0 || !m_INT_Root)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		int rev = cl.ResultsRevision();
		if (rev <= m_INT_PendRev)
			return;
		int want = INT_VPP_Net.RESULT_KIND_SAVE;
		if (m_INT_Pending == 2)
			want = INT_VPP_Net.RESULT_KIND_DELETE;
		int kind = cl.LastResultKind();
		if (kind != want)
			return;
		bool ok = cl.LastResultOk();
		int rid = cl.LastResultId();
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
			string err = cl.LastError();
			string ru = INT_VPP_ZoneEditor.INT_ReasonRu(err);
			INT_SetStatus(ru, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		m_INT_EditId = -1;
		INT_PushDraft();
		if (pk == 2)
			INT_SetStatusLater("Удалено", GREEN);
		else
			INT_SetStatusLater("Сохранено", GREEN);
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
			m_INT_PendRev = cl.ResultsRevision();
	}

	// ------------------------------------------------------------------ TEST hooks
	// a request as if it had just been sent (kind 1 save / 2 delete, the zone id asked for, 0 = new)
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

	void INT_TestStartNew(int shape)
	{
		INT_StartNew(shape);
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
		if (reason == "empty name")
			return "Введите название зоны";
		if (reason == "too few vertices")
			return "Нужно минимум 3 точки";
		if (reason == "too many vertices")
			return "Слишком много точек";
		if (reason == "no vertices")
			return "Нарисуйте зону на карте";
		if (reason == "radius out of range")
			return "Радиус от 5 до 5000 м";
		if (reason == "degenerate polygon")
			return "У многоугольника нет площади";
		if (reason == "vertex outside world" || reason == "centre outside world")
			return "Точка за пределами карты";
		if (reason == "not admin")
			return "Нет прав администратора карты";
		if (reason == "too fast")
			return "Слишком часто, подождите";
		if (reason == "no transport")
			return "Нет связи с сервером";
		if (reason == "zone limit")
			return "Достигнут предел зон";
		// server answers (protocol 2, review #10)
		if (reason == "zones read-only")
			return "Зоны только для чтения: файл от новой версии мода";
		if (reason == "unknown id")
			return "Такой зоны на сервере уже нет";
		if (reason == "bad id")
			return "Неверный номер зоны";
		if (reason == "read failed")
			return "Сервер не смог прочитать запрос";
		if (reason == "refused")
			return "Сервер отклонил запрос";
		if (reason == "null zone" || reason == "bad shape" || reason == "vertex arrays differ")
			return "Неверные данные зоны";
		if (reason.IndexOf("protocol") == 0)
			return "Версия мода у сервера другая — обновите мод";
		if (reason.IndexOf("save failed") == 0)
			return "Сервер не смог сохранить зоны, изменение отменено";
		return "Ошибка: " + reason;
	}

	// ------------------------------------------------------------------ body (rebuilt on mode / data change)
	protected void INT_DropBody()
	{
		m_INT_RowHits.Clear();
		m_INT_RowIds.Clear();
		m_INT_SwHits.Clear();
		m_INT_ListWheel = null;
		m_INT_Status = null;
		m_INT_ShapeText = null;
		m_INT_SwSel = null;
		m_INT_Name = null;
		m_INT_Sub = null;
		m_INT_Hint = null;
		m_INT_BtnNewPoly = null;
		m_INT_BtnNewCircle = null;
		m_INT_BtnDone = null;
		m_INT_BtnRedo = null;
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
		// keep typed texts across a rebuild in edit mode
		string keepName = m_INT_InitName;
		string keepSub = m_INT_InitSub;
		if (m_INT_Name)
			keepName = m_INT_Name.GetText();
		if (m_INT_Sub)
			keepSub = m_INT_Sub.GetText();
		INT_DropBody();
		if (!m_INT_Root)
			return;
		m_INT_Body = INT_UIF_Draw.Frame(m_INT_Root, 0, 0, ISL_W, m_INT_IslH, "IntVppZoneBody");
		if (!m_INT_Body)
			return;
		if (m_INT_EditId < 0)
			INT_BuildList();
		else
			INT_BuildEdit(keepName, keepSub);
		INT_VPP_Ui.INT_Sort(m_INT_Body);
		INT_SetStatus(m_INT_StatusText, m_INT_StatusColor);
	}

	protected void INT_BuildList()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		array<ref INT_VPP_Zone> zones = null;
		int maxZones = INT_VPP_Net.HARD_MAX_ZONES;
		if (cl)
		{
			zones = cl.GetZones();
			INT_VPP_ClientSettings st = cl.GetSettings();
			if (st)
				maxZones = st.MaxZones;
		}
		int n = 0;
		if (zones)
			n = zones.Count();
		string cap = INT_VPPX_Text.Caps("Зоны");
		INT_VPPX_Look.Text(m_INT_Body, 18, 14, 160, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		string cnt = n.ToString() + " из " + maxZones.ToString();
		INT_VPPX_Look.Text(m_INT_Body, ISL_W - 16 - 32 - 12 - 120, 14, 120, 20, cnt, 13, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
		INT_VPPX_Look.Fill(m_INT_Body, 18, 50, ISL_W - 36, 1, INT_VPPX_Look.LINE);
		float listTop = 58;
		float listBottom = m_INT_IslH - 120;
		m_INT_VisibleRows = (listBottom - listTop) / ROW_H;
		if (m_INT_VisibleRows < 1)
			m_INT_VisibleRows = 1;
		m_INT_ListWheel = INT_UIF_Draw.Hit(m_INT_Body, 0, listTop, ISL_W, listBottom - listTop, this);
		int maxScroll = n - m_INT_VisibleRows;
		if (maxScroll < 0)
			maxScroll = 0;
		if (m_INT_Scroll > maxScroll)
			m_INT_Scroll = maxScroll;
		if (n == 0)
			INT_VPPX_Look.Text(m_INT_Body, 18, listTop + 8, ISL_W - 36, 24, "Зон пока нет", 15, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		int last = m_INT_Scroll + m_INT_VisibleRows;
		if (last > n)
			last = n;
		float y = listTop;
		for (int i = m_INT_Scroll; i < last; i++)
		{
			INT_VPP_Zone z = zones.Get(i);
			if (!z)
				continue;
			int col = INT_VPP_Validate.ForceAlpha(z.color);
			INT_VPPX_Look.Diamond(m_INT_Body, 28, y + 18, 9, true, col);
			string nm = INT_VPPX_Look.FitText(z.name, 15, false, 230);
			INT_VPPX_Look.Text(m_INT_Body, 44, y + 6, 250, 24, nm, 15, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
			string shape = "";
			if (z.IsCircle())
			{
				string rs = INT_VPP_Fmt.INT_Dist(z.r);
				shape = "круг · " + rs;
			}
			else
			{
				int vc = z.VertexCount();
				shape = "полигон · " + vc.ToString() + " т.";
			}
			INT_VPPX_Look.Text(m_INT_Body, ISL_W - 18 - 130, y + 8, 130, 20, shape, 12, INT_VPP_Ui.M, INT_UIF_Align.RIGHT, false);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Body, 12, y, ISL_W - 24, ROW_H, this);
			m_INT_RowHits.Insert(hit);
			m_INT_RowIds.Insert(z.id);
			y = y + ROW_H;
		}
		m_INT_Status = INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 108, ISL_W - 36, 22, " ", 14, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
		m_INT_BtnNewPoly = new INT_VPPX_HButton();
		m_INT_BtnNewPoly.Build(m_INT_Body, 18, m_INT_IslH - 76, 200, 40, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnNewPoly.SetText("Многоугольник");
		m_INT_BtnNewPoly.OnClick().Insert(INT_OnButton);
		m_INT_BtnNewCircle = new INT_VPPX_HButton();
		m_INT_BtnNewCircle.Build(m_INT_Body, 18 + 200 + 8, m_INT_IslH - 76, ISL_W - 36 - 208, 40, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnNewCircle.SetText("Круг");
		m_INT_BtnNewCircle.OnClick().Insert(INT_OnButton);
		INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 30, ISL_W - 36, 18, "новая зона: выберите форму", 12, INT_VPP_Ui.D, INT_UIF_Align.LEFT, false);
		if (n >= maxZones)
		{
			m_INT_BtnNewPoly.SetEnabled(false);
			m_INT_BtnNewCircle.SetEnabled(false);
		}
	}

	protected void INT_BuildEdit(string keepName, string keepSub)
	{
		string title = "Новая зона";
		if (m_INT_EditId > 0)
			title = "Зона № " + m_INT_EditId.ToString();
		string cap = INT_VPPX_Text.Caps(title);
		INT_VPPX_Look.Text(m_INT_Body, 18, 14, 300, 20, cap, 13, INT_VPPX_Look.ORANGE, INT_UIF_Align.LEFT, true);
		INT_VPPX_Look.Fill(m_INT_Body, 18, 50, ISL_W - 36, 1, INT_VPPX_Look.LINE);
		string ncap = INT_VPPX_Text.Caps("Название");
		INT_VPPX_Look.Text(m_INT_Body, 18, 62, 200, 18, ncap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_Name = new INT_VPPX_HInput();
		m_INT_Name.Build(m_INT_Body, 18, 80, ISL_W - 36, "Например: Военная база", NAME_CHARS, "pencil");
		m_INT_Name.SetText(keepName);
		string scap = INT_VPPX_Text.Caps("Подпись");
		INT_VPPX_Look.Text(m_INT_Body, 18, 132, 200, 18, scap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_Sub = new INT_VPPX_HInput();
		m_INT_Sub.Build(m_INT_Body, 18, 150, ISL_W - 36, "необязательно", SUB_CHARS, "pencil");
		m_INT_Sub.SetText(keepSub);
		string ccap = INT_VPPX_Text.Caps("Цвет");
		INT_VPPX_Look.Text(m_INT_Body, 18, 204, 200, 18, ccap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_SwSel = INT_UIF_Draw.Frame(m_INT_Body, 0, 224, 30, 44, "IntVppZSwSel");
		if (m_INT_SwSel)
		{
			INT_VPPX_Look.Outline(m_INT_SwSel, 0, 0, 30, 30, INT_VPPX_Look.WHITE);
			INT_VPPX_Look.Diamond(m_INT_SwSel, 15, 37, 6, true, INT_VPPX_Look.ORANGE);
		}
		int pn = m_INT_Palette.Count();
		for (int i = 0; i < pn; i++)
		{
			float sx = 18 + i * 38;
			int c = m_INT_Palette.Get(i);
			INT_VPPX_Look.Fill(m_INT_Body, sx + 5, 229, 20, 20, c);
			ButtonWidget hit = INT_UIF_Draw.Hit(m_INT_Body, sx, 224, 30, 30, this);
			m_INT_SwHits.Insert(hit);
		}
		INT_PaintSwatch();
		INT_VPPX_Look.Fill(m_INT_Body, 18, 282, ISL_W - 36, 1, INT_VPPX_Look.LINE);
		string fcap = INT_VPPX_Text.Caps("Форма");
		INT_VPPX_Look.Text(m_INT_Body, 18, 294, 200, 18, fcap, 11, INT_VPP_Ui.M, INT_UIF_Align.LEFT, true);
		m_INT_ShapeText = INT_VPPX_Look.Text(m_INT_Body, 18, 314, ISL_W - 36, 24, " ", 16, INT_VPPX_Look.WHITE, INT_UIF_Align.LEFT, false);
		m_INT_Hint = new INT_VPPX_HLines();
		m_INT_Hint.Build(m_INT_Body, 18, 344, ISL_W - 36, 20, 13, INT_VPP_Ui.T2, 3);
		m_INT_BtnDone = new INT_VPPX_HButton();
		m_INT_BtnDone.Build(m_INT_Body, 18, 414, 196, 40, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnDone.SetText("Замкнуть");
		m_INT_BtnDone.OnClick().Insert(INT_OnButton);
		m_INT_BtnRedo = new INT_VPPX_HButton();
		m_INT_BtnRedo.Build(m_INT_Body, 18 + 196 + 8, 414, ISL_W - 36 - 204, 40, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnRedo.SetText("Нарисовать заново");
		m_INT_BtnRedo.OnClick().Insert(INT_OnButton);
		m_INT_Status = INT_VPPX_Look.Text(m_INT_Body, 18, m_INT_IslH - 120, ISL_W - 36, 22, " ", 14, INT_VPP_Ui.M, INT_UIF_Align.LEFT, false);
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
		m_INT_BtnBack.Build(m_INT_Body, bx, by, ISL_W - 18 - bx, 50, INT_VPPX_HButton.TONE_GHOST);
		m_INT_BtnBack.SetText("Назад");
		m_INT_BtnBack.OnClick().Insert(INT_OnButton);
		INT_PaintShape();
	}

	protected void INT_PaintSwatch()
	{
		if (!m_INT_SwSel)
			return;
		int at = -1;
		int rgb = m_INT_Color & 0x00FFFFFF;
		for (int i = 0; i < m_INT_Palette.Count(); i++)
		{
			int pc = m_INT_Palette.Get(i) & 0x00FFFFFF;
			if (pc == rgb)
				at = i;
		}
		if (at < 0)
		{
			m_INT_SwSel.Show(false);
			return;
		}
		INT_UIF_Draw.Move(m_INT_SwSel, 18 + at * 38, 224);
		m_INT_SwSel.Show(true);
	}

	protected int INT_MaxVertices()
	{
		int maxV = INT_VPP_Net.HARD_MAX_ZONE_VERTICES;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (cl)
		{
			INT_VPP_ClientSettings st = cl.GetSettings();
			if (st && st.MaxZoneVertices >= 3 && st.MaxZoneVertices < maxV)
				maxV = st.MaxZoneVertices;
		}
		return maxV;
	}

	// shape line, hint and the Готово / Заново buttons for the current drawing state
	protected void INT_PaintShape()
	{
		string line = "";
		string hint = "";
		bool showDone = false;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_Stage == 0)
			{
				line = "Круг · центр не задан";
				hint = "ЛКМ по карте — центр круга.";
			}
			else if (m_INT_Stage == 1)
			{
				line = "Круг · задайте радиус";
				hint = "ЛКМ по карте — край круга. ПКМ — другой центр.";
			}
			else
			{
				string rs = INT_VPP_Fmt.INT_Dist(m_INT_R);
				line = "Круг · радиус " + rs;
				hint = "Готово. «Нарисовать заново» — другой круг.";
			}
		}
		else
		{
			int n = m_INT_Xs.Count();
			int maxV = INT_MaxVertices();
			line = "Многоугольник · точек: " + n.ToString() + " из " + maxV.ToString();
			if (m_INT_Closed)
			{
				hint = "Готово. «Нарисовать заново» — другая форма.";
			}
			else
			{
				hint = "ЛКМ по карте — точка. Клик по первой точке или «Замкнуть» — готово. ПКМ — убрать последнюю.";
				if (n >= 3)
					showDone = true;
			}
		}
		if (m_INT_ShapeText)
			m_INT_ShapeText.SetText(line);
		if (m_INT_Hint)
			m_INT_Hint.Set(hint);
		if (m_INT_BtnDone)
			m_INT_BtnDone.Show(showDone);
		INT_PushDraft();
	}

	protected void INT_PushDraft()
	{
		if (!m_INT_View)
			return;
		if (m_INT_EditId < 0)
		{
			m_INT_View.INT_SetDraft(false, 0, null, null, 0, 0, 0, 0, false, 0, -1);
			return;
		}
		bool closed = m_INT_Closed;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE && m_INT_Stage >= 2)
			closed = true;
		m_INT_View.INT_SetDraft(true, m_INT_Shape, m_INT_Xs, m_INT_Zs, m_INT_CX, m_INT_CZ, m_INT_R, m_INT_Color, closed, m_INT_Stage, m_INT_EditId);
	}

	// ------------------------------------------------------------------ mode switches
	protected void INT_StartNew(int shape)
	{
		m_INT_EditId = 0;
		m_INT_Shape = shape;
		m_INT_Xs.Clear();
		m_INT_Zs.Clear();
		m_INT_CX = 0;
		m_INT_CZ = 0;
		m_INT_R = 0;
		m_INT_Closed = false;
		m_INT_Stage = 0;
		m_INT_InitName = "";
		m_INT_InitSub = "";
		m_INT_Name = null;
		m_INT_Sub = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
	}

	void INT_SelectZone(int id)
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		INT_VPP_Zone z = cl.FindZone(id);
		if (!z)
			return;
		m_INT_EditId = z.id;
		m_INT_Shape = z.shape;
		m_INT_Xs.Clear();
		m_INT_Zs.Clear();
		int n = z.VertexCount();
		for (int i = 0; i < n; i++)
		{
			m_INT_Xs.Insert(z.xs.Get(i));
			m_INT_Zs.Insert(z.zs.Get(i));
		}
		m_INT_CX = z.cx;
		m_INT_CZ = z.cz;
		m_INT_R = z.r;
		m_INT_Closed = true;
		m_INT_Stage = 2;
		m_INT_Color = INT_VPP_Validate.ForceAlpha(z.color);
		m_INT_InitName = z.name;
		m_INT_InitSub = z.sub;
		m_INT_Name = null;
		m_INT_Sub = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
		if (m_INT_Menu)
		{
			vector c = Vector(z.cx, 0, z.cz);
			m_INT_Menu.INT_CenterOn(c);
		}
	}

	protected void INT_BackToList()
	{
		m_INT_EditId = -1;
		m_INT_Name = null;
		m_INT_Sub = null;
		m_INT_StatusText = "";
		m_INT_BodyDirty = true;
		INT_PushDraft();
	}

	protected void INT_Redraw()
	{
		m_INT_Xs.Clear();
		m_INT_Zs.Clear();
		m_INT_R = 0;
		m_INT_Closed = false;
		m_INT_Stage = 0;
		INT_PaintShape();
	}

	// ------------------------------------------------------------------ map input (from the menu)
	bool INT_OnMapClick(vector world, float ax, float ay)
	{
		if (!INT_IsDrawing())
			return false;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_Stage == 0)
			{
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
				m_INT_CX = world[0];
				m_INT_CZ = world[2];
				m_INT_Stage = 1;
			}
			else
			{
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
				vector c = Vector(m_INT_CX, 0, m_INT_CZ);
				float r = INT_VPP_Fmt.INT_Dist2D(c, world);
				r = Math.Clamp(r, INT_VPP_Net.CIRCLE_MIN_R, INT_VPP_Net.CIRCLE_MAX_R);
				m_INT_R = r;
				m_INT_Stage = 2;
			}
			INT_PaintShape();
			return true;
		}
		// polygon: a click on the first point closes it
		if (m_INT_View && m_INT_View.INT_NearFirstVertex(ax, ay, CLOSE_PX))
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			m_INT_Closed = true;
			INT_PaintShape();
			return true;
		}
		int maxV = INT_MaxVertices();
		if (m_INT_Xs.Count() >= maxV)
		{
			INT_SetStatus("Предел точек: " + maxV.ToString(), INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
			return true;
		}
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
		m_INT_Xs.Insert(world[0]);
		m_INT_Zs.Insert(world[2]);
		INT_PaintShape();
		return true;
	}

	bool INT_OnMapRmb()
	{
		if (m_INT_EditId < 0)
			return false;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_Stage == 1)
				m_INT_Stage = 0;
			else if (m_INT_Stage == 2)
				m_INT_Stage = 1;
			else
				return true;
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			INT_PaintShape();
			return true;
		}
		if (m_INT_Closed)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			m_INT_Closed = false;
			INT_PaintShape();
			return true;
		}
		int n = m_INT_Xs.Count();
		if (n > 0)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
			m_INT_Xs.Remove(n - 1);
			m_INT_Zs.Remove(n - 1);
			INT_PaintShape();
		}
		return true;
	}

	// ------------------------------------------------------------------ save / delete
	protected void INT_Save()
	{
		if (m_INT_Pending != 0)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
		{
			INT_SetStatus("Нет связи с сервером", INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		string nm = "";
		string sb = "";
		if (m_INT_Name)
			nm = m_INT_Name.GetText();
		if (m_INT_Sub)
			sb = m_INT_Sub.GetText();
		INT_VPP_Zone z = new INT_VPP_Zone();
		z.id = m_INT_EditId;
		if (z.id < 0)
			z.id = 0;
		z.name = INT_VPP_Validate.CleanText(nm, INT_VPP_Net.NAME_MAX_BYTES);
		z.sub = INT_VPP_Validate.CleanText(sb, INT_VPP_Net.SUB_MAX_BYTES);
		z.color = INT_VPP_Validate.ForceAlpha(m_INT_Color);
		z.shape = m_INT_Shape;
		if (m_INT_Shape == INT_VPP_Net.SHAPE_CIRCLE)
		{
			if (m_INT_Stage < 2)
			{
				INT_SetStatus("Нарисуйте круг на карте", INT_VPPX_Look.RED);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
				return;
			}
			z.cx = m_INT_CX;
			z.cz = m_INT_CZ;
			z.r = m_INT_R;
		}
		else
		{
			if (!m_INT_Closed)
			{
				INT_SetStatus("Замкните многоугольник", INT_VPPX_Look.RED);
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
				return;
			}
			int n = m_INT_Xs.Count();
			for (int i = 0; i < n; i++)
			{
				z.xs.Insert(m_INT_Xs.Get(i));
				z.zs.Insert(m_INT_Zs.Get(i));
			}
		}
		// the same check as the server, on a copy (it normalises in place)
		INT_VPP_Zone probe = z.Clone();
		string reason = "";
		int maxV = INT_MaxVertices();
		bool ok = INT_VPP_Validate.ZoneLimits(probe, maxV, reason);
		if (!ok)
		{
			string ru = INT_VPP_ZoneEditor.INT_ReasonRu(reason);
			INT_SetStatus(ru, INT_VPPX_Look.RED);
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
			return;
		}
		// pending before the send: a test transport may answer inside the call
		INT_MarkPending(1, z.id);
		bool sent = cl.RequestZoneSave(z);
		if (!sent)
		{
			m_INT_Pending = 0;
			string err = cl.LastError();
			string ru2 = INT_VPP_ZoneEditor.INT_ReasonRu(err);
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
		bool sent = cl.RequestZoneDelete(id);
		if (!sent)
		{
			m_INT_Pending = 0;
			string err = cl.LastError();
			string ru = INT_VPP_ZoneEditor.INT_ReasonRu(err);
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
			m_INT_Menu.INT_CloseZoneEditor();
	}

	void INT_OnButton(INT_VPPX_HButton b)
	{
		if (!m_INT_Menu)
			return;
		if (b == m_INT_BtnNewPoly)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			INT_StartNew(INT_VPP_Net.SHAPE_POLYGON);
		}
		else if (b == m_INT_BtnNewCircle)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			INT_StartNew(INT_VPP_Net.SHAPE_CIRCLE);
		}
		else if (b == m_INT_BtnDone)
		{
			if (m_INT_Xs.Count() >= 3)
			{
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
				m_INT_Closed = true;
				INT_PaintShape();
			}
			else
			{
				INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
			}
		}
		else if (b == m_INT_BtnRedo)
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
			INT_Redraw();
		}
		else if (b == m_INT_BtnSave)
		{
			INT_Save();
		}
		else if (b == m_INT_BtnDel)
		{
			string nm = m_INT_InitName;
			if (m_INT_Name)
				nm = m_INT_Name.GetText();
			m_INT_Menu.INT_AskZoneDelete(m_INT_EditId, nm);
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
			INT_SelectZone(id);
			return true;
		}
		int sw = m_INT_SwHits.Find(bw);
		if (sw >= 0)
		{
			m_INT_Color = m_INT_Palette.Get(sw);
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
			n = cl.ZonesCount();
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
