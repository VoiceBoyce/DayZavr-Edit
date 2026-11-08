// Связка планшета с картой VanillaPlusPlusMap, правки только здесь:
//   - кнопка «В клан» в окне метки VPP: метка уходит в клановые метки планшета, личная метка VPP удаляется,
//     когда метка появилась у клана (отказ сервера или молчание оставляют её в VPP);
//   - группа «Метки клана» в списке меток слева: глазок (скрыта, 2D, 3D) по настройкам планшета, клик ведёт карту к метке;
//   - 3D метки VPP остаются скрытыми после окна правки, если игрок спрятал их клавишей K.
class VB_TabletVppUi
{
	static const string LAYOUT_BTN = "VB_Tablet_VPP/gui/vb_vpp_clanbtn.layout";
	static const float RESULT_WAIT = 10;

	// Ожидание ответа на «В клан». Успех сервер не подтверждает отдельным ответом, он рассылает карту клана:
	// метка считается принятой, когда у клана стало больше меток с этим именем в этой точке.
	protected static int s_ResultSeen;
	protected static int s_MapSeen;
	protected static float s_WaitUntil;		// 0: ничего не ждём
	protected static string s_PendTitle;
	protected static vector s_PendPos;
	protected static bool s_PendCustom;
	protected static int s_PendBefore;
	protected static VPPMapMenu s_PendMenu;	// без ref: карта VPP живёт сама по себе, удалённая даёт null

	static bool CanClan()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		return client.Clan && client.Can(VB_ClanPerm.MAP);
	}

	// Новая клановая метка из окна VPP. true: запрос ушёл, окно правки можно закрывать.
	// Сама метка VPP убирается в Watch, когда сервер её принял.
	static bool SendClanMark(VPPMapMenu menu, string name, string iconPath, vector color, vector pos, bool isCustom)
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
		string group = VB_MapIcons.VppGroup(iconPath);
		o.IconIdx = VB_MapIcons.VppIcon(group);
		o.Title = title;
		o.Color = VB_MapIcons.VppColor(color);
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
		s_PendCustom = isCustom;
		s_PendBefore = CountSame(st, title, pos);
		s_PendMenu = menu;
		s_WaitUntil = GetGame().GetTickTime() + RESULT_WAIT;
		return true;
	}

	// Клановые метки с этим именем в пределах 2 м от точки.
	protected static int CountSame(VB_MapState st, string title, vector pos)
	{
		int n = 0;
		foreach (VB_MapObject o : st.Objects)
		{
			if (o.Scope != VB_MapScope.CLAN || o.Kind != VB_MapKind.MARK || o.Title != title)
				continue;
			float ax;
			float az;
			o.AnchorOf(ax, az);
			float dx = ax - pos[0];
			float dz = az - pos[2];
			if (dx * dx + dz * dz < 4)
				n++;
		}
		return n;
	}

	// Каждый кадр игры (карта VPP может быть уже закрыта): ответ на «В клан».
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

	// Принятая сервером метка убирается из личных меток VPP (и из её 3D меток).
	protected static void Finish(bool ok)
	{
		s_WaitUntil = 0;
		VPPMapMenu menu = s_PendMenu;
		s_PendMenu = null;
		if (!ok)
			return;
		INT_UIF_Toast.Success(VBT.T("#STR_VB_TAB_VPP_SENT"));
		if (!menu)
			return;
		menu.RemoveMarkerExact(s_PendPos, s_PendCustom);
		// VPP при перерисовке заново показывает все 3D метки: спрятанные клавишей K остаются спрятанными.
		if (VB_TabletClient.Get().Hide3D)
			g_Game.HideAll3dMarkers();
	}

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

	static vector ColorVec(int argb)
	{
		int r = (argb >> 16) & 0xFF;
		int g = (argb >> 8) & 0xFF;
		int b = argb & 0xFF;
		return Vector(r, g, b);
	}

	static VisibleState StateOf(VB_MapState st, VB_MapObject o)
	{
		int mode = st.ModeOf(o);
		if (mode == VB_MapMode.HIDDEN)
			return VisibleState.NOT_VISIBLE;
		if (mode == VB_MapMode.WORLD)
			return VisibleState.VISIBLE_2D_AND_3D;
		return VisibleState.VISIBLE_ONLY_2D;
	}

	// Глазок по кругу как у VPP: скрыта → 2D → 3D → скрыта. У зон, кругов и маршрутов 3D нет.
	static int NextMode(VB_MapState st, VB_MapObject o)
	{
		int mode = st.ModeOf(o);
		if (mode == VB_MapMode.HIDDEN)
			return VB_MapMode.FLAT;
		if (mode == VB_MapMode.FLAT && o.Kind == VB_MapKind.MARK && st.Limits.Allow3D)
			return VB_MapMode.WORLD;
		return VB_MapMode.HIDDEN;
	}

	// Глазок одного объекта: 3D сверх лимита пропускается, по кругу дальше идёт «скрыта».
	static void SetMode(VB_MapObject o, int mode)
	{
		SetModeEx(o, mode, VB_MapMode.HIDDEN, true);
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

// Пункт списка VPP, за которым стоит объект карты планшета.
class VB_VppClanItem extends ListItem
{
	ref VB_MapObject m_VbtObj;

	void VB_VppClanItem(HeaderItem headerParent, MarkerInfo marker, int markerIndex)
	{
	}
}

modded class MarkersListAdapter
{
	protected ref HeaderItem m_VbtClanHeader;

	HeaderItem VbtClanHeader()
	{
		if (!m_VbtClanHeader)
			m_VbtClanHeader = new HeaderItem(VBT.T("#STR_VB_TAB_VPP_CLAN_GROUP"));
		return m_VbtClanHeader;
	}

	// Клановые объекты из состояния планшета: пересобираются при каждой отрисовке списка.
	protected void VbtFillClan()
	{
		HeaderItem head = VbtClanHeader();
		head.Clear();
		VB_TabletClient client = VB_TabletClient.Get();
		VB_MapState st = client.Map;
		bool anyShown = false;
		bool any3D = false;
		int i = 0;
		foreach (VB_MapObject o : st.Objects)
		{
			if (o.Scope != VB_MapScope.CLAN || o.ObjId <= 0)
				continue;
			float ax;
			float az;
			o.AnchorOf(ax, az);
			int mode = st.ModeOf(o);
			bool active = mode != VB_MapMode.HIDDEN;
			bool d3 = mode == VB_MapMode.WORLD;
			if (active)
				anyShown = true;
			if (d3)
				any3D = true;
			MarkerInfo info = new MarkerInfo(VBT.T(o.Title), VB_TabletVppUi.IconPath(o), VB_TabletVppUi.ColorVec(o.Color), Vector(ax, 0, az), active, d3);
			VB_VppClanItem item = new VB_VppClanItem(head, info, i);
			item.m_VbtObj = o;
			head.AddListItem(item);
			i++;
		}
		if (!anyShown)
			head.SetVisible(VisibleState.NOT_VISIBLE);
		else if (any3D)
			head.SetVisible(VisibleState.VISIBLE_2D_AND_3D);
		else
			head.SetVisible(VisibleState.VISIBLE_ONLY_2D);
	}

	// Группа клана дописывается к копии списка VPP: сам список VPP не меняется и не растёт от перерисовок.
	override void AddAllWidgets(array<ref VPPMapItem> items)
	{
		if (!VB_TabletClient.Get().Clan)
		{
			super.AddAllWidgets(items);
			return;
		}
		VbtFillClan();
		array<ref VPPMapItem> all = new array<ref VPPMapItem>();
		foreach (VPPMapItem it : items)
			all.Insert(it);
		HeaderItem head = VbtClanHeader();
		all.Insert(head);
		if (head.m_IsExpanded)
		{
			foreach (ListItem li : head.m_ListItems)
				all.Insert(li);
		}
		super.AddAllWidgets(all);
	}

	// Свои пункты обрабатываются здесь: VPP приняла бы их за свои серверные метки по номеру.
	override void OnItemClicked(string widgetName, VPPMapItem item)
	{
		HeaderItem head = m_VbtClanHeader;
		if (!head || !item)
		{
			super.OnItemClicked(widgetName, item);
			return;
		}
		VB_VppClanItem clanItem = VB_VppClanItem.Cast(item);
		bool isHead = item == head;
		if (!clanItem && !isHead)
		{
			super.OnItemClicked(widgetName, item);
			return;
		}
		VB_MapState st = VB_TabletClient.Get().Map;
		if (isHead && (widgetName == "collapse_icon" || widgetName == "expand_icon"))
		{
			head.ToggleExpandCollapse();
			return;
		}
		if (widgetName == "visible_button")
		{
			if (clanItem)
			{
				VB_TabletVppUi.SetMode(clanItem.m_VbtObj, VB_TabletVppUi.NextMode(st, clanItem.m_VbtObj));
				return;
			}
			int target = VB_MapMode.FLAT;
			if (head.m_VisibleState == VisibleState.VISIBLE_ONLY_2D)
				target = VB_MapMode.WORLD;
			else if (head.m_VisibleState == VisibleState.VISIBLE_2D_AND_3D)
				target = VB_MapMode.HIDDEN;
			// Вся группа в 3D: что не влезло в лимит 3D, остаётся на карте (2D), сообщение одно.
			bool told = false;
			foreach (ListItem li : head.m_ListItems)
			{
				VB_VppClanItem ci = VB_VppClanItem.Cast(li);
				if (!ci)
					continue;
				int mode = target;
				if (mode == VB_MapMode.WORLD && (ci.m_VbtObj.Kind != VB_MapKind.MARK || !st.Limits.Allow3D))
					mode = VB_MapMode.FLAT;
				if (!VB_TabletVppUi.SetModeEx(ci.m_VbtObj, mode, VB_MapMode.FLAT, !told))
					told = true;
			}
			return;
		}
		if (clanItem && (widgetName == "image" || widgetName == "text") && m_Menu && m_Menu.m_MapWidget)
		{
			vector at = clanItem.m_Marker.GetPosition();
			m_Menu.m_MapWidget.SetMapPos(at);
		}
	}
}

modded class EditDialog
{
	protected Widget m_VbtClanRoot;
	protected ButtonWidget m_VbtClanBtn;

	protected void VbtEnsureButton()
	{
		if (m_VbtClanRoot || !m_Root)
			return;
		Widget panel = m_Root.FindAnyWidget("panel_buttons");
		if (!panel)
			return;
		m_VbtClanRoot = GetGame().GetWorkspace().CreateWidgets(VB_TabletVppUi.LAYOUT_BTN, panel);
		if (m_VbtClanRoot)
			m_VbtClanBtn = ButtonWidget.Cast(m_VbtClanRoot.FindAnyWidget("vbt_button_clan"));
	}

	override void SetMarker(int markerIndex, MarkerInfo marker, bool isCustomServer, bool isNewMarker, string markerSuffix)
	{
		super.SetMarker(markerIndex, marker, isCustomServer, isNewMarker, markerSuffix);
		VbtEnsureButton();
		if (m_VbtClanRoot)
			m_VbtClanRoot.Show(VB_TabletVppUi.CanClan());
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT && m_VbtClanBtn && w == m_VbtClanBtn && m_Marker)
		{
			string name = m_EditBoxName.GetText();
			string iconPath = m_IconListAdapter.GetSelectedIcon();
			vector color = m_ColorListAdapter.GetSelectedColor();
			vector pos = m_Marker.GetPosition();
			if (VB_TabletVppUi.SendClanMark(m_Menu, name, iconPath, color, pos, m_IsCustomServerMarker))
				m_Menu.HideDialog();
			return true;
		}
		return super.OnMouseButtonDown(w, x, y, button);
	}
}

modded class VPPMapMenu
{
	protected int m_VbtSeenVer = -1;
	protected bool m_VbtSeenClan;

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		VB_TabletClient client = VB_TabletClient.Get();
		bool clan = client.Clan != null;
		if (m_Adapter && (client.Map.Version != m_VbtSeenVer || clan != m_VbtSeenClan))
		{
			m_VbtSeenVer = client.Map.Version;
			m_VbtSeenClan = clan;
			m_Adapter.UpdateView();
		}
	}

	// VPP после окна правки показывает все 3D метки: если игрок спрятал их клавишей K, они остаются скрытыми.
	override void HideDialog()
	{
		super.HideDialog();
		if (VB_TabletClient.Get().Hide3D)
			g_Game.HideAll3dMarkers();
	}
}

// Ответ на «В клан» ждётся и после закрытия карты VPP.
modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		VB_TabletVppUi.Watch();
	}
}
