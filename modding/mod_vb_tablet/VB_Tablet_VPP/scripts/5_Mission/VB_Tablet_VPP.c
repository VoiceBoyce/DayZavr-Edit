// Связка планшета с картой INT_VppMap (форк VanillaPlusPlusMap от INT: аддон VanillaPlusPlus, меню VPPMapMenu и
// MapWidget "Map_Widget" сохранены). Мод карты не меняется, всё через modded class, правки только здесь:
//   - кнопка «В клан» в карточке-редакторе метки: метка уходит в клановые метки планшета с выбранной в карточке
//     видимостью; правленая личная метка удаляется, когда метка появилась у клана (отказ сервера или молчание
//     оставляют её на карте);
//   - группа «Метки клана» в списке «Метки на карте» после «Моих»: фишка режима (3D, 2D, скрыта) пишет настройку
//     планшета, клик по строке ведёт карту к метке.
// Сами клановые объекты рисует на этой карте VB_TabletVppLayer (основной PBO). 3D метки, спрятанные клавишей K,
// карта INT держит скрытыми сама: прежние заплатки для этого не нужны.
class VB_TabletVppUi
{
	static const float RESULT_WAIT = 10;
	// Своя группа в списке карты: у слоёв карты номера 0..4, у строк зон и маршрута 0.
	static const int LAYER_CLAN = 90;
	// Панель «В клан» у нижнего угла редактора (размеры в пикселях макета карты, как у её кнопок).
	static const float CLAN_BTN_W = 166;
	static const float CLAN_BTN_H = 50;
	static const float PLATE_PAD = 8;
	static const float PLATE_GAP = 8;

	// Ожидание ответа на «В клан». Успех сервер не подтверждает отдельным ответом, он рассылает карту клана:
	// метка считается принятой, когда у клана стало больше меток с этим именем в этой точке.
	protected static int s_ResultSeen;
	protected static int s_MapSeen;
	protected static float s_WaitUntil;		// 0: ничего не ждём
	protected static string s_PendTitle;
	protected static vector s_PendPos;
	protected static int s_PendBefore;
	protected static int s_PendMode;
	protected static ref MarkerInfo s_PendMine;	// личная метка карты, которую убрать после ответа; null у новой

	static bool CanClan()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		return client.Clan && client.Can(VB_ClanPerm.MAP);
	}

	// Открытая карта, которая ещё не разобрала себя при закрытии.
	static VPPMapMenu OpenMap()
	{
		UIManager um = GetGame().GetUIManager();
		if (!um)
			return null;
		VPPMapMenu menu = VPPMapMenu.Cast(um.FindMenu(VPP_MENU_MAP));
		if (!menu || !menu.INT_IsAlive())
			return null;
		return menu;
	}

	// --- режимы: у планшета скрыта / 2D / 3D, у карты INT свои номера -------------------------------
	static int ToVpp(int mode)
	{
		if (mode == VB_MapMode.WORLD)
			return INT_VPP_MODE_3D;
		if (mode == VB_MapMode.FLAT)
			return INT_VPP_MODE_2D;
		return INT_VPP_MODE_HIDDEN;
	}

	static int FromVpp(int mode)
	{
		if (mode == INT_VPP_MODE_3D)
			return VB_MapMode.WORLD;
		if (mode == INT_VPP_MODE_2D)
			return VB_MapMode.FLAT;
		return VB_MapMode.HIDDEN;
	}

	// Видимость из карточки-редактора: 0 только на карте, 1 карта и метка в мире, 2 скрыта.
	static int ModeOfVis(int vis)
	{
		if (vis == 1)
			return VB_MapMode.WORLD;
		if (vis == 2)
			return VB_MapMode.HIDDEN;
		return VB_MapMode.FLAT;
	}

	// 3D бывает только у меток, и если сервер планшета его разрешает.
	static bool Can3(VB_MapState st, VB_MapObject o)
	{
		return o.Kind == VB_MapKind.MARK && st.Limits.Allow3D;
	}

	// --- «В клан» ------------------------------------------------------------------------------
	// Новая клановая метка из редактора карты. true: запрос ушёл, редактор можно закрывать.
	// Личная метка mine убирается в Watch, когда сервер её принял.
	static bool SendClanMark(string name, string iconPath, int color, vector pos, int vis, MarkerInfo mine)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (s_WaitUntil > 0)
		{
			INT_UIF_Toast.Info(VBT.T("#STR_VB_TAB_VPP_WAIT"));
			return false;
		}
		if (!client.Clan)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_GC_NOCLAN"));
			return false;
		}
		if (!client.Can(VB_ClanPerm.MAP))
		{
			INT_UIF_Toast.Error(VBT.T(VBK.ResultText(VB_TabletResult.DENIED)));
			return false;
		}
		VB_MapState st = client.Map;
		if (st.Loaded && st.Used(VB_MapScope.CLAN, VB_MapKind.MARK) >= st.Limits.ClanMarks)
		{
			INT_UIF_Toast.Error(VBT.T(VBK.ResultText(VB_TabletResult.MAP_LIMIT)));
			return false;
		}
		string title = name;
		title.TrimInPlace();
		if (title == "")
			title = VBT.T("#STR_VB_TAB_D_KIND_0");
		if (title.LengthUtf8() > VB_MAP_TITLE_MAX)
			title = title.SubstringUtf8(0, VB_MAP_TITLE_MAX);
		if (VB_TabletBridge.Censored(title))
			return false;
		VB_MapObject o = new VB_MapObject();
		o.Scope = VB_MapScope.CLAN;
		o.Kind = VB_MapKind.MARK;
		o.IconIdx = VB_MapIcons.VppMark(iconPath);
		o.Title = title;
		o.Color = ARGB(255, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		o.Author = "";
		o.AddPoint(pos[0], pos[2]);
		ScriptRPC rpc = client.Begin(VB_TabletOp.MAP_SAVE);
		int scope = VB_MapScope.CLAN;
		rpc.Write(scope);
		o.Write(rpc);
		client.Post(rpc);
		s_ResultSeen = client.ResultVersion;
		s_MapSeen = st.Version;
		s_PendTitle = title;
		s_PendPos = pos;
		s_PendBefore = CountSame(st, title, pos);
		s_PendMode = ModeOfVis(vis);
		s_PendMine = mine;
		s_WaitUntil = GetGame().GetTickTime() + RESULT_WAIT;
		return true;
	}

	// Клановая метка с этим именем в пределах 2 м от точки.
	protected static bool Same(VB_MapObject o, string title, vector pos)
	{
		if (o.Scope != VB_MapScope.CLAN || o.Kind != VB_MapKind.MARK || o.Title != title)
			return false;
		float ax;
		float az;
		o.AnchorOf(ax, az);
		float dx = ax - pos[0];
		float dz = az - pos[2];
		return dx * dx + dz * dz < 4;
	}

	protected static int CountSame(VB_MapState st, string title, vector pos)
	{
		int n = 0;
		foreach (VB_MapObject o : st.Objects)
		{
			if (Same(o, title, pos))
				n++;
		}
		return n;
	}

	// Каждый кадр игры (карта может быть уже закрыта): ответ на «В клан».
	static void Watch()
	{
		if (s_WaitUntil <= 0)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.Map.Version != s_MapSeen)
		{
			s_MapSeen = client.Map.Version;
			if (CountSame(client.Map, s_PendTitle, s_PendPos) > s_PendBefore)
			{
				Finish(true);
				return;
			}
		}
		if (client.ResultVersion != s_ResultSeen)
		{
			s_ResultSeen = client.ResultVersion;
			if (!VBK.ResultIsGood(client.LastResult))
			{
				INT_UIF_Toast.Error(VBT.T(VBK.ResultText(client.LastResult)));
				Finish(false);
				return;
			}
		}
		if (GetGame().GetTickTime() > s_WaitUntil)
		{
			INT_UIF_Toast.Error(VBT.T("#STR_VB_TAB_VPP_NOANSWER"));
			Finish(false);
		}
	}

	protected static void Finish(bool ok)
	{
		s_WaitUntil = 0;
		MarkerInfo mine = s_PendMine;
		s_PendMine = null;
		if (!ok)
			return;
		INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_VPP_SENT"));
		ApplyMode();
		if (mine)
			DropMine(mine);
	}

	// Новая клановая метка получает видимость из редактора (у каждого игрока своя, у остальных по умолчанию).
	protected static void ApplyMode()
	{
		VB_MapState st = VB_TabletClient.Get().Map;
		VB_MapObject made = null;
		foreach (VB_MapObject o : st.Objects)
		{
			if (Same(o, s_PendTitle, s_PendPos) && (!made || o.ObjId > made.ObjId))
				made = o;
		}
		if (!made)
			return;
		int mode = s_PendMode;
		if (mode == VB_MapMode.WORLD && !Can3(st, made))
			mode = VB_MapMode.FLAT;
		SetModeEx(made, mode, VB_MapMode.FLAT, true);
	}

	// Личная метка карты уходит: через открытую карту (она сама пересоберёт список и 3D), иначе прямо в кэше карты.
	// Мини-карта сверяет кэш сама. Объект MarkerInfo тот же, что в кэше: карта берёт списки из него при открытии.
	protected static void DropMine(MarkerInfo info)
	{
		VPPMapMenu menu = OpenMap();
		if (menu)
		{
			menu.VbtDropMine(info);
			return;
		}
		ClientMarkersCache cache = g_Game.GetClientMarkersCache();
		if (!cache)
			return;
		array<ref MarkerInfo> mine = cache.GetCachedArray();
		array<ref MarkerInfo> per = null;
		string ap = g_Game.GetConnectAddressPort();
		if (ap != "")
			per = cache.GetCachedArray(ap);
		if (mine)
			mine.RemoveItem(info);
		if (per)
			per.RemoveItem(info);
		cache.SaveCache();
		g_Game.INT_VPP_SyncClient3d(mine, per);
		INT_VPP_3D d3 = INT_VPP_3D.INT_Peek();
		if (d3)
			d3.INT_Refresh();
	}

	// --- группа клана в списке ------------------------------------------------------------------
	static string IconPath(VB_MapObject o)
	{
		string name = VB_MapIcons.MarkName(o.IconIdx);
		if (o.Kind == VB_MapKind.ZONE)
			name = "polygon";
		else if (o.Kind == VB_MapKind.CIRCLE)
			name = "circleo";
		else if (o.Kind == VB_MapKind.ROUTE)
			name = "route";
		return VB_TABLET_ICON + name;
	}

	// Клановые объекты с номером: только что отправленные ждут номера от сервера.
	static void ClanObjects(VB_MapState st, array<VB_MapObject> into)
	{
		into.Clear();
		foreach (VB_MapObject o : st.Objects)
		{
			if (o.Scope == VB_MapScope.CLAN && o.ObjId > 0)
				into.Insert(o);
		}
	}

	// Режим группы на фишке, как у карты: общий у всех, -1 вперемешку.
	static int GroupMode(VB_MapState st, array<VB_MapObject> objs)
	{
		int mode = -2;
		foreach (VB_MapObject o : objs)
		{
			int m = ToVpp(st.ModeOf(o));
			if (mode == -2)
				mode = m;
			else if (mode != m)
				return -1;
		}
		if (mode == -2)
			return -1;
		return mode;
	}

	// Все объекты группы в режиме mode, насколько им можно (у зон и маршрутов нет 3D).
	protected static bool GroupAt(VB_MapState st, array<VB_MapObject> objs, int mode)
	{
		foreach (VB_MapObject o : objs)
		{
			int capped = INT_VPP_Prefs.INT_CapMode(mode, Can3(st, o), true);
			if (capped != ToVpp(st.ModeOf(o)))
				return false;
		}
		return true;
	}

	// Режим, в который группу ставили (2D и «скрыта» раньше 3D), -1 вперемешку. Так же считает сама карта.
	protected static int GroupCapMode(VB_MapState st, array<VB_MapObject> objs)
	{
		if (GroupAt(st, objs, INT_VPP_MODE_2D))
			return INT_VPP_MODE_2D;
		if (GroupAt(st, objs, INT_VPP_MODE_HIDDEN))
			return INT_VPP_MODE_HIDDEN;
		if (GroupAt(st, objs, INT_VPP_MODE_3D))
			return INT_VPP_MODE_3D;
		return -1;
	}

	// Фишка строки: следующий режим по кругу карты 3D → 2D → скрыта. 3D сверх лимита оставляет метку на карте.
	static bool CycleOne(VB_MapState st, VB_MapObject o)
	{
		int cur = ToVpp(st.ModeOf(o));
		int next = INT_VPP_Prefs.INT_NextMode(cur, Can3(st, o), true);
		if (next == cur)
			return false;
		SetModeEx(o, FromVpp(next), VB_MapMode.FLAT, true);
		return true;
	}

	// Фишка заголовка: вся группа в следующий режим; что не влезло в лимит 3D, остаётся на карте, сообщение одно.
	static bool CycleGroup(VB_MapState st, array<VB_MapObject> objs)
	{
		if (objs.Count() == 0)
			return false;
		bool can3 = false;
		foreach (VB_MapObject a : objs)
		{
			if (Can3(st, a))
				can3 = true;
		}
		int next = INT_VPP_Prefs.INT_NextMode(GroupCapMode(st, objs), can3, true);
		if (GroupAt(st, objs, next))
			next = INT_VPP_Prefs.INT_NextMode(next, can3, true);
		bool told = false;
		foreach (VB_MapObject b : objs)
		{
			int mode = FromVpp(INT_VPP_Prefs.INT_CapMode(next, Can3(st, b), true));
			if (!SetModeEx(b, mode, VB_MapMode.FLAT, !told))
				told = true;
		}
		return true;
	}

	// 3D сверх лимита: объект получает fallback, сообщение только если tell. false: лимит 3D не пустил.
	static bool SetModeEx(VB_MapObject o, int mode, int fallback, bool tell)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_MapState st = client.Map;
		bool fit = true;
		if (mode == VB_MapMode.WORLD && st.ModeOf(o) != VB_MapMode.WORLD && st.CountD3() >= st.Limits.D3Max)
		{
			if (tell)
				INT_UIF_Toast.Error(VBT.T(VBK.ResultText(VB_TabletResult.MAP_D3)));
			mode = fallback;
			fit = false;
		}
		if (st.ModeOf(o) == mode)
			return fit;
		st.SetMode(o, mode);
		ScriptRPC rpc = client.Begin(VB_TabletOp.MAP_PREF);
		int what = VB_MapPref.OBJECT;
		int key = o.ObjKey();
		rpc.Write(what);
		rpc.Write(key);
		rpc.Write(mode);
		client.Post(rpc);
		return fit;
	}
}

// Строка списка карты, за которой стоит объект карты планшета.
modded class INT_VPP_ListRow
{
	ref VB_MapObject m_VbtObj;
}

// «В клан» живёт на своей маленькой панели у нижнего угла редактора, снаружи (слева от редактора справа, справа
// от редактора слева): нижний ряд карточки занят подсказкой значка и кнопками карты. Панель — ребёнок карточки,
// поэтому едет вместе с ней при перетаскивании и закрывается вместе с ней.
modded class INT_VPP_EditorCard
{
	protected Widget m_VbtPlate;
	protected ref INT_VPPX_HButton m_VbtClanBtn;
	protected bool m_VbtShown;

	override protected void INT_BuildButtons()
	{
		super.INT_BuildButtons();
		m_VbtPlate = null;
		m_VbtClanBtn = null;
		if (!m_INT_Root || !VB_TabletVppUi.CanClan())
			return;
		float pad = VB_TabletVppUi.PLATE_PAD;
		float pw = VB_TabletVppUi.CLAN_BTN_W + pad * 2;
		float ph = VB_TabletVppUi.CLAN_BTN_H + pad * 2;
		float px = -pw - VB_TabletVppUi.PLATE_GAP;
		if (m_INT_Left)
			px = CARD_W + VB_TabletVppUi.PLATE_GAP;
		m_VbtPlate = INT_VPP_Ui.INT_Panel(m_INT_Root, px, CARD_H - ph, pw, ph, INT_VPP_Ui.CARD_BG, INT_VPP_Ui.CARD_EDGE, true, "VbtClanPlate");
		if (!m_VbtPlate)
			return;
		m_VbtClanBtn = new INT_VPPX_HButton();
		m_VbtClanBtn.Build(m_VbtPlate, pad, pad, VB_TabletVppUi.CLAN_BTN_W, VB_TabletVppUi.CLAN_BTN_H, INT_VPPX_HButton.TONE_GHOST);
		m_VbtClanBtn.SetText(VBT.T("#STR_VB_TAB_VPP_TO_CLAN"));
		m_VbtClanBtn.OnClick().Insert(VbtOnClan);
		m_VbtShown = true;
		VbtSync();
	}

	// Серверная метка администратора карты в клан не уходит: панель только у личной (переключатель «Моя / Серверная»).
	void VbtSync()
	{
		if (!m_VbtPlate)
			return;
		bool want = !m_INT_Srv;
		if (want == m_VbtShown)
			return;
		m_VbtShown = want;
		m_VbtPlate.Show(want);
	}

	void VbtOnClan(INT_VPPX_HButton b)
	{
		if (m_INT_Menu && !m_INT_Srv)
			m_INT_Menu.VbtEditorToClan();
	}
}

modded class VPPMapMenu
{
	protected int m_VbtSeenVer = -1;
	protected bool m_VbtSeenClan;

	// Карта планшета или клан сменились: список меток собирается заново (группа клана).
	override void Update(float timeslice)
	{
		if (m_INT_Built && !m_INT_TornDown)
		{
			VB_TabletClient client = VB_TabletClient.Get();
			bool clan = client.Clan != null;
			if (client.Map.Version != m_VbtSeenVer || clan != m_VbtSeenClan)
			{
				m_VbtSeenVer = client.Map.Version;
				m_VbtSeenClan = clan;
				m_INT_DirtyList = true;
			}
		}
		super.Update(timeslice);
		if (m_INT_Editor)
			m_INT_Editor.VbtSync();
	}

	// «В клан» из редактора: личная метка (или новая) уходит в клановые метки планшета.
	void VbtEditorToClan()
	{
		if (!m_INT_Editor || m_INT_Editor.INT_IsServer())
			return;
		string nm = m_INT_Editor.INT_GetName();
		int col = m_INT_Editor.INT_GetColor();
		string icon = m_INT_Editor.INT_GetIconPath();
		int vis = m_INT_Editor.INT_GetVis();
		vector pos = m_INT_EditorPos;
		if (!VB_TabletVppUi.SendClanMark(nm, icon, col, pos, vis, m_INT_EditInfo))
		{
			INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
			return;
		}
		s_INT_LastColor = col;
		s_INT_LastIcon = icon;
		INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
		INT_CloseEditor();
	}

#ifdef DIAG_DEVELOPER
	// Только DayZDiag: стенд не умеет надёжно кликать мышью по безымянным кнопкам карты.
	// F10 — вкладка «Метки», F11 — фишка группы клана (тот же путь, что и клик: Islands.INT_OnChip),
	// Numpad1 — редактор новой метки в 40 м от игрока, Numpad2 — кнопка «В клан» в открытом редакторе.
	void VbtTestKey(int key)
	{
		if (!m_INT_Built || m_INT_TornDown)
			return;
		if (key == KeyCode.KC_F10)
		{
			if (m_INT_Tab != INT_TAB_MARKS)
				INT_SelectTab(INT_TAB_MARKS);
		}
		else if (key == KeyCode.KC_F11)
		{
			array<ref INT_VPP_ListRow> rows = new array<ref INT_VPP_ListRow>();
			INT_BuildRows(rows);
			for (int i = 0; i < rows.Count(); i++)
			{
				if (rows.Get(i).m_INT_Type == INT_VPP_ListRow.TYPE_HEADER && rows.Get(i).m_INT_Layer == VB_TabletVppUi.LAYER_CLAN)
				{
					bool hit = m_INT_Isl && m_INT_Isl.INT_TestClickChip(i);
					Print("[VB_Tablet_VPP] тест фишки группы, строка " + i.ToString() + ": " + hit.ToString());
					return;
				}
			}
		}
		else if (key == KeyCode.KC_NUMPAD1)
		{
			Man man = GetGame().GetPlayer();
			if (man)
				INT_OpenEditorAt(man.GetPosition() + Vector(40, 0, 40));
		}
		else if (key == KeyCode.KC_NUMPAD2)
		{
			if (m_INT_Editor)
				m_INT_Editor.VbtOnClan(null);
		}
		else if (key == KeyCode.KC_NUMPAD3)
		{
			// Личная метка, как её сохраняет редактор карты, и её редактор (правка существующей).
			Man me = GetGame().GetPlayer();
			if (!me)
				return;
			MarkerInfo made = INT_TestAddMine("VBT личная", me.GetPosition() + Vector(-60, 0, 30));
			INT_VPP_MapItem it = INT_FindItem(made);
			if (it)
				INT_OpenEditorFor(it);
		}
	}
#endif

	// Метка принята кланом: из личных меток её убирает сама карта (списки, кэш, 3D, открытая карточка).
	void VbtDropMine(MarkerInfo info)
	{
		INT_DeleteMarkerInfo(info);
	}

	// Группа клана встаёт после групп меток карты («Сервер», «Мои»), перед «Событиями», зонами и маршрутом.
	override protected void INT_BuildRows(array<ref INT_VPP_ListRow> rows)
	{
		super.INT_BuildRows(rows);
		VB_TabletClient client = VB_TabletClient.Get();
		if (!client.Clan)
			return;
		VB_MapState st = client.Map;
		array<VB_MapObject> objs = new array<VB_MapObject>();
		VB_TabletVppUi.ClanObjects(st, objs);
		if (objs.Count() == 0)
			return;
		int at = 0;
		for (int i = 0; i < rows.Count(); i++)
		{
			INT_VPP_ListRow was = rows.Get(i);
			if (was && was.m_INT_HasMode && (was.m_INT_Layer == INT_VPP_LAYER_SERVER || was.m_INT_Layer == INT_VPP_LAYER_MINE))
				at = i + 1;
		}
		INT_VPP_ListRow head = new INT_VPP_ListRow();
		head.m_INT_Type = INT_VPP_ListRow.TYPE_HEADER;
		head.m_INT_Text = VBT.T("#STR_VB_TAB_VPP_CLAN_GROUP");
		head.m_INT_Sub = INT_VPPX_Text.Pad2(objs.Count());
		head.m_INT_Color = VB_C_ACCENT;
		head.m_INT_Layer = VB_TabletVppUi.LAYER_CLAN;
		head.m_INT_HasMode = true;
		head.m_INT_Mode = VB_TabletVppUi.GroupMode(st, objs);
		rows.InsertAt(head, at);
		at++;
		foreach (VB_MapObject o : objs)
		{
			float ax;
			float az;
			o.AnchorOf(ax, az);
			vector p = Vector(ax, 0, az);
			int mode = st.ModeOf(o);
			INT_VPP_ListRow row = new INT_VPP_ListRow();
			row.m_INT_Type = INT_VPP_ListRow.TYPE_ITEM;
			row.m_INT_Text = VBT.T(o.Title);
			row.m_INT_Sub = INT_DistSub(p);
			row.m_INT_Color = o.Color;
			row.m_INT_Icon = VB_TabletVppUi.IconPath(o);
			row.m_INT_Dim = mode == VB_MapMode.HIDDEN;
			row.m_INT_Pos = p;
			row.m_INT_Layer = VB_TabletVppUi.LAYER_CLAN;
			row.m_INT_HasMode = true;
			row.m_INT_Mode = VB_TabletVppUi.ToVpp(mode);
			row.m_INT_Can3 = VB_TabletVppUi.Can3(st, o);
			row.m_INT_Can2 = true;
			row.m_VbtObj = o;
			rows.InsertAt(row, at);
			at++;
		}
	}

	// Свои строки обрабатываются здесь: у них нет пункта карты, карта сочла бы их пустыми.
	override bool INT_CycleRowMode(INT_VPP_ListRow r)
	{
		if (!r || !r.m_VbtObj)
			return super.INT_CycleRowMode(r);
		VB_MapState st = VB_TabletClient.Get().Map;
		bool changed = VB_TabletVppUi.CycleOne(st, r.m_VbtObj);
		INT_SyncRowMode(r);
		return changed;
	}

	override bool INT_CycleGroupMode(INT_VPP_ListRow h, array<ref INT_VPP_ListRow> rows)
	{
		if (!h || h.m_INT_Layer != VB_TabletVppUi.LAYER_CLAN)
			return super.INT_CycleGroupMode(h, rows);
		VB_MapState st = VB_TabletClient.Get().Map;
		array<VB_MapObject> objs = new array<VB_MapObject>();
		if (rows)
		{
			foreach (INT_VPP_ListRow item : rows)
			{
				if (item && item.m_VbtObj)
					objs.Insert(item.m_VbtObj);
			}
		}
		bool changed = VB_TabletVppUi.CycleGroup(st, objs);
		if (rows)
		{
			foreach (INT_VPP_ListRow other : rows)
			{
				if (other && other.m_INT_Layer == VB_TabletVppUi.LAYER_CLAN)
					INT_SyncRowMode(other);
			}
		}
		return changed;
	}

	override void INT_SyncRowMode(INT_VPP_ListRow r)
	{
		if (!r || r.m_INT_Layer != VB_TabletVppUi.LAYER_CLAN)
		{
			super.INT_SyncRowMode(r);
			return;
		}
		VB_MapState st = VB_TabletClient.Get().Map;
		if (r.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER)
		{
			array<VB_MapObject> objs = new array<VB_MapObject>();
			VB_TabletVppUi.ClanObjects(st, objs);
			r.m_INT_Mode = VB_TabletVppUi.GroupMode(st, objs);
			return;
		}
		if (!r.m_VbtObj)
			return;
		int mode = st.ModeOf(r.m_VbtObj);
		r.m_INT_Mode = VB_TabletVppUi.ToVpp(mode);
		r.m_INT_Dim = mode == VB_MapMode.HIDDEN;
	}
}

// Ответ на «В клан» ждётся и после закрытия карты.
modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		VB_TabletVppUi.Watch();
	}

#ifdef DIAG_DEVELOPER
	// Только DayZDiag: dayz-mcp не нажимает UAInput, поэтому стенд открывает и закрывает карту через F9.
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		if (key == KeyCode.KC_F9)
		{
			if (INT_VPP_IsMapOpen())
				INT_VPP_CloseMap();
			else
				INT_VPP_OpenMap();
			return;
		}
		VPPMapMenu menu = VB_TabletVppUi.OpenMap();
		if (menu)
			menu.VbtTestKey(key);
	}
#endif
}
