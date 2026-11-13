// Внутриигровой редактор точек боссов (клиент, ТЗ 6). Открыть может только админ из settings.json сервера.
//
// Ctrl+F9 (UAVBABEditor): войти / выйти. Пока редактор открыт, по бокам экрана висят панели (как у Изнанки):
// слева — точки (ближние сверху) и боссы из bosses.json с фильтром по модели и палитре, справа — выбранная точка,
// босс на ней и статус. Середина экрана свободна.
// Два режима, Пробел переключает (как в админке VPP):
//   «интерфейс» (по умолчанию) — персонаж и камера стоят, курсор виден, всё под курсором;
//   «управление» — всё разблокировано, курсора нет, всё по центру экрана.
// Постановка: выбрал босса в списке слева — под курсором маркер-призрак, ЛКМ ставит точку с этим боссом,
//   после установки выбор сбрасывается. ЛКМ без выбранного босса выделяет точку под курсором.
// ПКМ или Delete — удалить подсвеченную точку (без подтверждения); ПКМ мимо точек — снять выбор босса.
// Alt + колесо или PgUp/PgDn — высота точки над землёй.
// Маркеры точек и кольца радиусов — отладочные фигуры только на этом клиенте (VB_AB_EditorMarks).

class VB_AB_Editor
{
	static const float PLACE_DIST = 250.0;		// м: дальше курсор не ставит
	static const float TARGET_DIST = 6.0;		// м по горизонтали: точка под курсором
	static const float HEIGHT_STEP = 0.5;
	static const float HEIGHT_MIN = -5.0;
	static const float HEIGHT_MAX = 50.0;

	// что выключаем на время редактора: удары, действия, подъём оружия и прицеливание на ПКМ
	// (ходьба, бег, колесо и смена вида остаются)
	static const ref TStringArray BLOCK_INPUTS = {
		"UAFire", "UAHeavyMeleeAttack", "UADefaultAction", "UAAction", "UATempRaiseWeapon", "UATempRaiseWeaponGamepad",
		"UAZoomIn", "UAZoomInToggle", "UAZoomInOptics", "UAZoomOutOptics", "UAThrowitem", "UADropitem",
		"UAReloadMagazine", "UAToggleWeapons", "UAGetOver"
	};

	// режим «интерфейс»: персонаж не ходит, не меняет стойку, камера стоит
	static const ref TStringArray LOCK_GROUPS = {"aiming", "movement", "stances"};

	protected static ref VB_AB_Editor s_VB_Instance;

	bool m_VB_Active;
	int m_VB_CurPoint;							// выбранная точка (0 — нет)
	string m_VB_CurBoss;						// босс, выбранный в списке точки («Призвать», «Убрать из точки»)
	string m_VB_Pick;							// какого босса ставить; "" — ничего (после установки сбрасывается)
	float m_VB_Height;
	bool m_VB_ShowRadii;
	bool m_VB_Control;							// Пробел: режим управления (курсора нет, всё разблокировано)
	string m_VB_Msg;
	int m_VB_MsgUntil;

	protected bool m_VB_Waiting;				// ждём ответа сервера на вход
	protected int m_VB_WaitUntil;
	protected ref VB_AB_EditorPanels m_VB_Panels;
	protected ref VB_AB_EditorMarks m_VB_Marks;
	protected bool m_VB_MenuOpen;				// поверх открыто чужое меню (пауза, чат, диалог)
	protected bool m_VB_Look;					// применённый к вводу режим: true — управление
	protected bool m_VB_LookApplied = true;
	protected bool m_VB_PrevSpace;
	protected bool m_VB_AimOk;
	protected vector m_VB_AimPos;
	protected bool m_VB_GhostOk;
	protected vector m_VB_GhostPos;
	protected string m_VB_GhostNote;
	protected int m_VB_TargetId;
	protected bool m_VB_PrevLmb;
	protected bool m_VB_PrevRmb;
	protected bool m_VB_PrevDel;
	protected bool m_VB_PrevUp;
	protected bool m_VB_PrevDown;
	protected int m_VB_NextTestCheck;
	protected ref VB_AB_EditorTest m_VB_Test;

	static VB_AB_Editor VB_Get()
	{
		if (!s_VB_Instance)
			s_VB_Instance = new VB_AB_Editor();
		return s_VB_Instance;
	}

	// --- вход, выход, клавиша ---------------------------------------------------------------------

	static void VB_OnKey()
	{
		VB_AB_Editor ed = VB_Get();
		if (ed.m_VB_Active)
		{
			ed.VB_Deactivate();
			return;
		}
		if (!g_Game.GetUIManager().GetMenu())
			ed.VB_RequestOpen();
	}

	void VB_RequestOpen()
	{
		if (m_VB_Active || (m_VB_Waiting && g_Game.GetTime() < m_VB_WaitUntil))
			return;

		m_VB_Waiting = true;
		m_VB_WaitUntil = g_Game.GetTime() + 5000;
		VB_AB_EditorClient.VB_SendOp(VB_AB_EditorNet.OP_OPEN);
	}

	protected void VB_Activate()
	{
		m_VB_Active = true;
		m_VB_PrevLmb = true;
		m_VB_PrevRmb = true;
		m_VB_PrevDel = true;
		m_VB_PrevSpace = true;
		m_VB_Control = false;
		m_VB_Look = false;
		m_VB_Pick = "";
		m_VB_LookApplied = true;	// чтобы VB_ApplyLook сразу включил курсор
		m_VB_MenuOpen = false;
		g_Game.GetMission().AddActiveInputExcludes(BLOCK_INPUTS);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && !VB_AB_EditorClient.VB_FindPoint(m_VB_CurPoint))
			m_VB_CurPoint = VB_NearestPoint(player.GetPosition(), 1000000.0);

		m_VB_Marks = new VB_AB_EditorMarks();
		m_VB_Panels = new VB_AB_EditorPanels();
		m_VB_Panels.VB_Build();
		VB_ApplyLook();
		Print("[VB_AB] редактор точек: вход");
	}

	void VB_Deactivate()
	{
		if (!m_VB_Active)
			return;

		m_VB_Active = false;
		m_VB_Waiting = false;
		VB_AB_EditorClient.VB_SendOp(VB_AB_EditorNet.OP_CLOSE);
		Mission mission = g_Game.GetMission();
		if (mission)
		{
			mission.RemoveActiveInputExcludes(BLOCK_INPUTS, true);
			mission.RemoveActiveInputExcludes(LOCK_GROUPS, true);
		}
		INT_UIF_Menu.LockCameraMotion(false);
		if (!g_Game.GetUIManager().GetMenu())
			g_Game.GetUIManager().ShowUICursor(false);
		VB_Cleanup();
		Print("[VB_AB] редактор точек: выход");
	}

	protected void VB_Cleanup()
	{
		m_VB_TargetId = 0;
		if (m_VB_Marks)
			m_VB_Marks.VB_Clear();
		m_VB_Marks = null;
		if (m_VB_Panels)
			m_VB_Panels.VB_Destroy();
		m_VB_Panels = null;
	}

	// конец миссии: всё локальное убрать, на сервер ничего не слать
	static void VB_Shutdown()
	{
		if (!s_VB_Instance)
			return;

		s_VB_Instance.m_VB_Active = false;
		s_VB_Instance.VB_Cleanup();
		s_VB_Instance = null;
	}

	// «интерфейс»: курсор, персонаж и камера стоят; «управление»: курсора нет, всё разблокировано
	protected void VB_ApplyLook()
	{
		m_VB_LookApplied = m_VB_Look;
		Mission mission = g_Game.GetMission();
		if (!mission)
			return;

		if (m_VB_Look)
		{
			mission.RemoveActiveInputExcludes(LOCK_GROUPS, false);
			INT_UIF_Menu.LockCameraMotion(false);
			g_Game.GetUIManager().ShowUICursor(false);
		}
		else
		{
			mission.AddActiveInputExcludes(LOCK_GROUPS);
			INT_UIF_Menu.LockCameraMotion(true);	// исключения ловят только новый ввод, это гасит уже начатое движение
			g_Game.GetUIManager().ShowUICursor(true);
		}
	}

	// мышь над панелями редактора (и над раскрытым списком фильтра)
	bool VB_OverPanels()
	{
		if (!m_VB_Panels || m_VB_Look)
			return false;
		int mx;
		int my;
		GetMousePos(mx, my);
		return m_VB_Panels.VB_Contains(mx, my);
	}

	bool VB_IsLook()
	{
		return m_VB_Look;
	}

	// --- кадр -------------------------------------------------------------------------------------

	static void VB_UpdateStatic(float dt)
	{
		VB_Get().VB_Update(dt);
	}

	void VB_Update(float dt)
	{
		VB_PollNet();

		// автотест: файл команд в профиле клиента проверяется раз в 5 с, прочитанный удаляется
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (m_VB_Test && m_VB_Test.VB_Done())
			m_VB_Test = null;
		if (!m_VB_Test && player && player.IsAlive() && g_Game.GetTime() >= m_VB_NextTestCheck)
		{
			m_VB_NextTestCheck = g_Game.GetTime() + 5000;
			m_VB_Test = VB_AB_EditorTest.VB_TryLoad();
		}
		if (m_VB_Test)
			m_VB_Test.VB_Tick(this);

		if (!m_VB_Active)
			return;
		if (!player || !player.IsAlive())
		{
			VB_Deactivate();
			return;
		}

		// чужое меню поверх (пауза, чат): панели и призрак прячем. Диалог подтверждения фреймворка
		// («Удалить точку?») панели не прячет: его окно поднимаем выше панелей
		UIScriptedMenu menu = g_Game.GetUIManager().GetMenu();
		if (menu)
		{
			if (!m_VB_MenuOpen)
			{
				m_VB_MenuOpen = true;
				bool dialog = menu.GetID() == INT_UIF_MENU_DIALOG;
				if (dialog && menu.GetLayoutRoot())
					menu.GetLayoutRoot().SetSort(VB_AB_EditorPanels.SORT + 100);
				if (m_VB_Panels && !dialog)
					m_VB_Panels.VB_Show(false);
				m_VB_GhostOk = false;
				m_VB_AimOk = false;
				m_VB_TargetId = 0;
			}
			m_VB_PrevLmb = true;
			m_VB_PrevRmb = true;
			m_VB_PrevDel = true;	// Delete в чате не должен удалить точку после закрытия чата
			m_VB_PrevUp = true;
			m_VB_PrevDown = true;
			if (m_VB_Marks)
				m_VB_Marks.VB_Tick(this);
			return;
		}
		if (m_VB_MenuOpen)
		{
			// меню закрылось и забрало курсор: вернуть свой режим
			m_VB_MenuOpen = false;
			if (m_VB_Panels)
				m_VB_Panels.VB_Show(true);
			m_VB_Look = m_VB_Control;
			VB_ApplyLook();
		}

		VB_UpdateLook();
		VB_UpdateAim(player);
		VB_UpdateTarget();
		VB_HandleInput();
		if (m_VB_Panels)
			m_VB_Panels.VB_Tick(this);
		if (m_VB_Marks)
			m_VB_Marks.VB_Tick(this);
	}

	protected void VB_UpdateLook()
	{
		// окно игры не активно: клавиши читаются напрямую и ловят нажатия в других окнах (E50) — не реагируем
		if (!g_Game.IsAppActive())
		{
			m_VB_PrevSpace = true;
			return;
		}

		bool space = KeyState(KeyCode.KC_SPACE) != 0;
		if (space && !m_VB_PrevSpace)
			m_VB_Control = !m_VB_Control;
		m_VB_PrevSpace = space;

		m_VB_Look = m_VB_Control;
		if (m_VB_Look != m_VB_LookApplied)
			VB_ApplyLook();
	}

	protected void VB_PollNet()
	{
		VB_AB_EditorClient.VB_Init();
		if (VB_AB_EditorClient.s_Denied)
		{
			VB_AB_EditorClient.s_Denied = false;
			m_VB_Waiting = false;
			INT_UIF_Toast.Error(string.Format(VB_AB_EdText.Tr("STR_VB_AB_Ed_Denied"), VB_AB_EditorClient.s_DeniedId));
			Print("[VB_AB] редактор точек: нет прав, SteamID " + VB_AB_EditorClient.s_DeniedId);
		}

		if (VB_AB_EditorClient.s_Granted)
		{
			VB_AB_EditorClient.s_Granted = false;
			if (m_VB_Waiting && !m_VB_Active)
			{
				m_VB_Waiting = false;
				VB_Activate();
			}
		}

		if (VB_AB_EditorClient.s_SelectPoint != 0)
		{
			m_VB_CurPoint = VB_AB_EditorClient.s_SelectPoint;
			m_VB_CurBoss = "";
			VB_AB_EditorClient.s_SelectPoint = 0;
		}

		while (VB_AB_EditorClient.s_Msgs.Count() > 0)
		{
			string msg = VB_AB_EditorClient.s_Msgs[0];
			VB_AB_EditorClient.s_Msgs.Remove(0);
			VB_Note(msg);
		}
	}

	void VB_Note(string text)
	{
		m_VB_Msg = text;
		m_VB_MsgUntil = g_Game.GetTime() + 8000;
		Print("[VB_AB] редактор точек: " + text);
	}

	// --- прицел и призрак -------------------------------------------------------------------------

	// луч из камеры через курсор (в режиме управления — в центр экрана; со свободной камерой — из неё), только по земле
	protected void VB_UpdateAim(PlayerBase player)
	{
		m_VB_AimOk = false;
		m_VB_GhostOk = false;
		m_VB_GhostNote = "";
		if (!m_VB_Look && VB_OverPanels())
			return;

		vector from = g_Game.GetCurrentCameraPosition();
		vector dir = g_Game.GetCurrentCameraDirection();
		if (!m_VB_Look)
			dir = g_Game.GetPointerDirection();
		vector to = from + dir * PLACE_DIST;
		vector hit;
		vector hitDir;
		int comp;
		m_VB_AimOk = DayZPhysics.RaycastRV(from, to, hit, hitDir, comp, null, null, player, false, true);
		if (!m_VB_AimOk)
		{
			m_VB_GhostNote = string.Format(VB_AB_EdText.Tr("STR_VB_AB_Ed_NoteFar"), PLACE_DIST.ToString());
			return;
		}
		m_VB_AimPos = hit;

		if (m_VB_Pick == "")
			return;
		if (g_Game.SurfaceIsSea(hit[0], hit[2]) || g_Game.SurfaceIsPond(hit[0], hit[2]))
		{
			m_VB_GhostNote = VB_AB_EdText.Tr("STR_VB_AB_Ed_NoteWater");
			return;
		}
		m_VB_GhostPos = Vector(hit[0], g_Game.SurfaceY(hit[0], hit[2]) + m_VB_Height, hit[2]);
		m_VB_GhostOk = true;
	}

	bool VB_GhostOk()
	{
		return m_VB_GhostOk;
	}

	vector VB_GhostPos()
	{
		return m_VB_GhostPos;
	}

	string VB_GhostNote()
	{
		return m_VB_GhostNote;
	}

	// --- точка под курсором -----------------------------------------------------------------------

	protected void VB_UpdateTarget()
	{
		m_VB_TargetId = 0;
		if (!m_VB_AimOk)
			return;

		float bestD = TARGET_DIST;
		float d;
		foreach (VB_AB_EdPoint p : VB_AB_EditorClient.s_Points)
		{
			d = VB_DistXZ(p.Position, m_VB_AimPos);
			if (d < bestD)
			{
				bestD = d;
				m_VB_TargetId = p.Id;
			}
		}
	}

	int VB_TargetId()
	{
		return m_VB_TargetId;
	}

	// --- ввод -------------------------------------------------------------------------------------

	protected void VB_HandleInput()
	{
		// клики и клавиши в других окнах сюда не пускаем (E50: без этого на стенде ставились лишние объекты)
		if (!g_Game.IsAppActive())
		{
			m_VB_PrevLmb = true;
			m_VB_PrevRmb = true;
			m_VB_PrevDel = true;
			m_VB_PrevUp = true;
			m_VB_PrevDown = true;
			return;
		}

		bool over = VB_OverPanels();
		bool lmb = (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) != 0;
		if (lmb && !m_VB_PrevLmb && !over)
			VB_Click();
		m_VB_PrevLmb = lmb;

		// ПКМ (не над панелями): удалить подсвеченную точку, а мимо точек — снять выбор босса
		bool rmb = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) != 0;
		if (rmb && !m_VB_PrevRmb && !over)
		{
			if (m_VB_TargetId != 0)
				VB_DeletePoint(m_VB_TargetId);
			else
				m_VB_Pick = "";
		}
		m_VB_PrevRmb = rmb;

		bool del = KeyState(KeyCode.KC_DELETE) != 0;
		if (del && !m_VB_PrevDel && m_VB_TargetId != 0)
			VB_DeletePoint(m_VB_TargetId);
		m_VB_PrevDel = del;

		bool up = KeyState(KeyCode.KC_PRIOR) != 0;
		bool down = KeyState(KeyCode.KC_NEXT) != 0;
		if (up && !m_VB_PrevUp)
			VB_AddHeight(HEIGHT_STEP);
		if (down && !m_VB_PrevDown)
			VB_AddHeight(-HEIGHT_STEP);
		m_VB_PrevUp = up;
		m_VB_PrevDown = down;

		// Alt + колесо над миром — высота (над панелью колесо листает список)
		int step = 0;
		if (GetUApi().GetInputByID(UAPrevAction).LocalPress())
			step = 1;
		else if (GetUApi().GetInputByID(UANextAction).LocalPress())
			step = -1;
		if (step == 0 || over)
			return;
		bool alt = KeyState(KeyCode.KC_LMENU) != 0 || KeyState(KeyCode.KC_RMENU) != 0;
		if (alt)
			VB_AddHeight(step * HEIGHT_STEP);
	}

	void VB_AddHeight(float dh)
	{
		m_VB_Height = Math.Clamp(m_VB_Height + dh, HEIGHT_MIN, HEIGHT_MAX);
	}

	// ЛКМ по миру: с выбранным боссом — поставить точку, без него — выделить точку под курсором
	protected void VB_Click()
	{
		if (m_VB_Pick == "")
		{
			if (m_VB_TargetId != 0)
				VB_Select(m_VB_TargetId, false);
			else
				VB_Note(VB_AB_EdText.Tr("STR_VB_AB_Ed_PickFirst"));
			return;
		}
		if (!m_VB_GhostOk)
		{
			if (m_VB_GhostNote != "")
				VB_Note(string.Format(VB_AB_EdText.Tr("STR_VB_AB_Ed_CantPlace"), m_VB_GhostNote));
			return;
		}
		VB_PlaceAt(m_VB_GhostPos);
	}

	// поставил — выбор сбрасывается, следующего босса выбирать заново (как в Изнанке)
	void VB_PlaceAt(vector pos)
	{
		if (m_VB_Pick == "")
		{
			VB_Note(VB_AB_EdText.Tr("STR_VB_AB_Ed_PickFirst"));
			return;
		}
		VB_AB_EditorClient.VB_SendAdd(pos, m_VB_Pick);
		m_VB_Pick = "";
	}

	// --- действия (панели, автотест) --------------------------------------------------------------

	void VB_Select(int pointId, bool teleport)
	{
		if (pointId != m_VB_CurPoint)
			m_VB_CurBoss = "";
		m_VB_CurPoint = pointId;
		if (teleport && pointId != 0)
			VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_TELEPORT, pointId);
	}

	void VB_DeletePoint(int pointId)
	{
		if (pointId == 0)
			return;
		VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_DEL, pointId);
		if (pointId == m_VB_CurPoint)
		{
			m_VB_CurPoint = 0;
			m_VB_CurBoss = "";
		}
	}

	void VB_SetDisabled(bool off)
	{
		if (m_VB_CurPoint == 0)
			return;
		int v = 0;
		if (off)
			v = 1;
		VB_AB_EditorClient.VB_SendIntInt(VB_AB_EditorNet.OP_DISABLE, m_VB_CurPoint, v);
	}

	void VB_SetWeight(float w)
	{
		if (m_VB_CurPoint != 0)
			VB_AB_EditorClient.VB_SendIntFloat(VB_AB_EditorNet.OP_WEIGHT, m_VB_CurPoint, w);
	}

	// в выбранную точку — босса, выбранного слева (выбор сбрасывается, как после установки)
	void VB_AddPickedBoss()
	{
		if (m_VB_CurPoint == 0)
			return;
		if (m_VB_Pick == "")
		{
			VB_Note(VB_AB_EdText.Tr("STR_VB_AB_Ed_PickFirst"));
			return;
		}
		VB_AB_EditorClient.VB_SendIntStr(VB_AB_EditorNet.OP_BOSS_ADD, m_VB_CurPoint, m_VB_Pick);
		m_VB_Pick = "";
	}

	void VB_AddBoss(string bossId)
	{
		if (m_VB_CurPoint != 0 && bossId != "")
			VB_AB_EditorClient.VB_SendIntStr(VB_AB_EditorNet.OP_BOSS_ADD, m_VB_CurPoint, bossId);
	}

	void VB_RemoveBoss(string bossId)
	{
		if (m_VB_CurPoint == 0 || bossId == "")
			return;
		VB_AB_EditorClient.VB_SendIntStr(VB_AB_EditorNet.OP_BOSS_DEL, m_VB_CurPoint, bossId);
		if (bossId == m_VB_CurBoss)
			m_VB_CurBoss = "";
	}

	// «Призвать сейчас»: босс, выбранный в списке точки, иначе случайный из её списка
	void VB_Summon()
	{
		if (m_VB_CurPoint == 0)
			return;
		string bossId = "";
		VB_AB_EdPoint p = VB_AB_EditorClient.VB_FindPoint(m_VB_CurPoint);
		if (p && m_VB_CurBoss != "" && p.Bosses.Find(m_VB_CurBoss) != -1)
			bossId = m_VB_CurBoss;
		VB_AB_EditorClient.VB_SendIntStr(VB_AB_EditorNet.OP_SUMMON, m_VB_CurPoint, bossId);
	}

	void VB_SummonBoss(string bossId)
	{
		if (m_VB_CurPoint != 0)
			VB_AB_EditorClient.VB_SendIntStr(VB_AB_EditorNet.OP_SUMMON, m_VB_CurPoint, bossId);
	}

	void VB_FieldOpen()
	{
		if (m_VB_CurPoint != 0)
			VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_FIELD_OPEN, m_VB_CurPoint);
	}

	void VB_FieldReroll()
	{
		if (m_VB_CurPoint != 0)
			VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_FIELD_REROLL, m_VB_CurPoint);
	}

	void VB_Despawn()
	{
		if (m_VB_CurPoint != 0)
			VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_DESPAWN, m_VB_CurPoint);
	}

	void VB_Reload()
	{
		VB_AB_EditorClient.VB_SendOp(VB_AB_EditorNet.OP_RELOAD);
	}

	void VB_SetActivePerRestart(int n)
	{
		VB_AB_EditorClient.VB_SendInt(VB_AB_EditorNet.OP_SET_ACTIVE, n);
	}

	void VB_SetFilter(string model, string palette)
	{
		if (m_VB_Panels)
			m_VB_Panels.VB_SetFilter(model, palette);
	}

	// --- точки ------------------------------------------------------------------------------------

	int VB_NearestPoint(vector pos, float maxDist)
	{
		int best = 0;
		float bestD = maxDist;
		float d;
		foreach (VB_AB_EdPoint p : VB_AB_EditorClient.s_Points)
		{
			d = VB_DistXZ(p.Position, pos);
			if (d < bestD)
			{
				bestD = d;
				best = p.Id;
			}
		}
		return best;
	}

	// босс, чьи радиусы показывать у точки: живой на ней, выбранный в её списке или первый из списка
	VB_AB_EdBoss VB_RadiiBoss(VB_AB_EdPoint p)
	{
		if (!p)
			return null;
		VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(p.Id);
		VB_AB_EdBoss b;
		if (lv && lv.BossId != "")
		{
			b = VB_AB_EditorClient.VB_FindBoss(lv.BossId);
			if (b)
				return b;
		}
		if (m_VB_CurBoss != "" && p.Bosses.Find(m_VB_CurBoss) != -1)
		{
			b = VB_AB_EditorClient.VB_FindBoss(m_VB_CurBoss);
			if (b)
				return b;
		}
		foreach (string id : p.Bosses)
		{
			b = VB_AB_EditorClient.VB_FindBoss(id);
			if (b)
				return b;
		}
		return null;
	}

	static float VB_DistXZ(vector a, vector b)
	{
		float dx = a[0] - b[0];
		float dz = a[2] - b[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}
}

// Автотест редактора без мыши (стенд dayz-mcp). Файл $profile:vb_ab_editor_test.txt на КЛИЕНТЕ,
// по команде в строке, # — комментарий. Клиент проверяет файл раз в 5 с, читает и удаляет.
// Команды: open, exit, wait <мс>, pick <босс|->, height <м>, place <x> <z>, placeh <x> <y> <z>,
// sel <id>, selnear <x> <z>, tp, del [id], delat <x> <z>, enable, disable, weight <w>, addboss <босс>,
// rmboss <босс>, boss <босс|->, summon [босс], openfield, reroll, despawn, radii on|off, reload,
// active <n>, filter <модель|*> <палитра|*>, dump, handprobe (кости кисти в осях предмета в руках).
// Всё пишет в скриптовый лог клиента «[VB_AB] тест редактора: ...».
class VB_AB_EditorTest
{
	static const string TEST_FILE = "$profile:vb_ab_editor_test.txt";

	protected ref TStringArray m_VB_Lines;
	protected int m_VB_Pos;
	protected int m_VB_WaitUntil;

	static VB_AB_EditorTest VB_TryLoad()
	{
		if (!FileExist(TEST_FILE))
			return null;

		FileHandle fh = OpenFile(TEST_FILE, FileMode.READ);
		if (!fh)
			return null;

		VB_AB_EditorTest test = new VB_AB_EditorTest();
		test.m_VB_Lines = new TStringArray;
		string line;
		while (FGets(fh, line) >= 0)
		{
			line = line.Trim();
			if (line != "" && line.IndexOf("#") != 0)
				test.m_VB_Lines.Insert(line);
		}
		CloseFile(fh);
		DeleteFile(TEST_FILE);
		test.m_VB_WaitUntil = g_Game.GetTime() + 1000;
		Print("[VB_AB] тест редактора: " + test.m_VB_Lines.Count().ToString() + " команд");
		return test;
	}

	bool VB_Done()
	{
		return m_VB_Pos >= m_VB_Lines.Count() && g_Game.GetTime() >= m_VB_WaitUntil;
	}

	void VB_Tick(VB_AB_Editor ed)
	{
		if (m_VB_Pos >= m_VB_Lines.Count() || g_Game.GetTime() < m_VB_WaitUntil)
			return;

		string line = m_VB_Lines[m_VB_Pos];
		m_VB_Pos++;
		m_VB_WaitUntil = g_Game.GetTime() + 500;
		Print("[VB_AB] тест редактора: " + line);

		TStringArray p = new TStringArray;
		line.Split(" ", p);
		string cmd = p[0];
		string arg = "";
		if (p.Count() > 1)
			arg = p[1];
		float x;
		float y;
		float z;
		if (cmd == "open")
			ed.VB_RequestOpen();
		else if (cmd == "exit")
			ed.VB_Deactivate();
		else if (cmd == "wait" && arg != "")
			m_VB_WaitUntil = g_Game.GetTime() + arg.ToInt();
		else if (cmd == "pick")
		{
			ed.m_VB_Pick = arg;
			if (arg == "-")
				ed.m_VB_Pick = "";
		}
		else if (cmd == "height" && arg != "")
			ed.m_VB_Height = arg.ToFloat();
		else if (cmd == "place" && p.Count() > 2)
		{
			x = p[1].ToFloat();
			z = p[2].ToFloat();
			ed.VB_PlaceAt(Vector(x, g_Game.SurfaceY(x, z) + ed.m_VB_Height, z));
		}
		else if (cmd == "placeh" && p.Count() > 3)
		{
			x = p[1].ToFloat();
			y = p[2].ToFloat();
			z = p[3].ToFloat();
			ed.VB_PlaceAt(Vector(x, y, z));
		}
		else if (cmd == "sel" && arg != "")
			ed.VB_Select(arg.ToInt(), false);
		else if (cmd == "selnear" && p.Count() > 2)
			ed.VB_Select(ed.VB_NearestPoint(Vector(p[1].ToFloat(), 0, p[2].ToFloat()), 50.0), false);
		else if (cmd == "tp")
			ed.VB_Select(ed.m_VB_CurPoint, true);
		else if (cmd == "del")
		{
			if (arg != "")
				ed.VB_DeletePoint(arg.ToInt());
			else
				ed.VB_DeletePoint(ed.m_VB_CurPoint);
		}
		else if (cmd == "delat" && p.Count() > 2)
		{
			int near = ed.VB_NearestPoint(Vector(p[1].ToFloat(), 0, p[2].ToFloat()), VB_AB_Editor.TARGET_DIST);
			if (near != 0)
				ed.VB_DeletePoint(near);
			else
				Print("[VB_AB] тест редактора: рядом нет точки");
		}
		else if (cmd == "enable")
			ed.VB_SetDisabled(false);
		else if (cmd == "disable")
			ed.VB_SetDisabled(true);
		else if (cmd == "weight" && arg != "")
			ed.VB_SetWeight(arg.ToFloat());
		else if (cmd == "addboss" && arg != "")
			ed.VB_AddBoss(arg);
		else if (cmd == "rmboss" && arg != "")
			ed.VB_RemoveBoss(arg);
		else if (cmd == "boss")
		{
			ed.m_VB_CurBoss = arg;
			if (arg == "-")
				ed.m_VB_CurBoss = "";
		}
		else if (cmd == "summon")
		{
			if (arg != "")
				ed.VB_SummonBoss(arg);
			else
				ed.VB_Summon();
		}
		else if (cmd == "openfield")
			ed.VB_FieldOpen();
		else if (cmd == "reroll")
			ed.VB_FieldReroll();
		else if (cmd == "despawn")
			ed.VB_Despawn();
		else if (cmd == "radii")
			ed.m_VB_ShowRadii = arg != "off";
		else if (cmd == "reload")
			ed.VB_Reload();
		else if (cmd == "active" && arg != "")
			ed.VB_SetActivePerRestart(arg.ToInt());
		else if (cmd == "filter")
		{
			string pal = "*";
			if (p.Count() > 2)
				pal = p[2];
			ed.VB_SetFilter(arg, pal);
		}
		else if (cmd == "dump")
			VB_Dump(ed);
		else if (cmd == "handprobe")
			VB_HandProbe();
		else
			Print("[VB_AB] тест редактора: неизвестная команда " + cmd);
	}

	// Замер хвата (баг №8): кости правой кисти в координатах предмета в руках — где ладонь относительно начала
	// координат модели. По нему подобран HAND_SHIFT_ENGINE в tools/ab_materials.py. «RightHand» и «RightHandRing1»
	// у игрока не находятся; RightHandIndex1/Middle1 — начала пястных костей (у запястья), Middle2 — костяшка
	protected void VB_HandProbe()
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;
		EntityAI item = player.GetHumanInventory().GetEntityInHands();
		if (!item)
		{
			Print("[VB_AB] HANDPROBE: руки пусты");
			return;
		}
		TStringArray bones = {"RightHand", "RightHand_Dummy", "RightHandThumb1", "RightHandIndex1", "RightHandMiddle1", "RightHandRing1", "RightHandPinky1", "RightHandMiddle2", "RightForeArm"};
		int idx;
		vector ws;
		vector loc;
		foreach (string bone : bones)
		{
			idx = player.GetBoneIndexByName(bone);
			if (idx < 0)
			{
				Print("[VB_AB] HANDPROBE " + item.GetType() + " " + bone + " нет");
				continue;
			}
			ws = player.GetBonePositionWS(idx);
			loc = item.WorldToModel(ws);
			Print("[VB_AB] HANDPROBE " + item.GetType() + " " + bone + " " + loc[0].ToString() + " " + loc[1].ToString() + " " + loc[2].ToString());
		}
		vector up = item.VectorToLocal("0 1 0");
		Print("[VB_AB] HANDPROBE " + item.GetType() + " мировой верх в осях предмета " + up[0].ToString() + " " + up[1].ToString() + " " + up[2].ToString());
	}

	protected void VB_Dump(VB_AB_Editor ed)
	{
		VB_AB_EditorClient.VB_Init();
		Print(string.Format("[VB_AB] тест редактора: DUMP активен=%1 точек=%2 боссов=%3 выбрана=%4 босс=%5 ставлю=%6 высота=%7 радиусы=%8",
			ed.m_VB_Active, VB_AB_EditorClient.s_Points.Count(), VB_AB_EditorClient.s_Bosses.Count(), ed.m_VB_CurPoint,
			ed.m_VB_CurBoss, ed.m_VB_Pick, ed.m_VB_Height, ed.m_VB_ShowRadii));
		Print(string.Format("[VB_AB] тест редактора: DUMP за рестарт=%1 будит=%2 м мин. расстояние=%3 м бодрствуют до %4",
			VB_AB_EditorClient.s_ActivePerRestart, VB_AB_EditorClient.s_WakeRadius, VB_AB_EditorClient.s_MinDist, VB_AB_EditorClient.s_MaxAwake));
		foreach (VB_AB_EdPoint pt : VB_AB_EditorClient.s_Points)
		{
			string bosses = "";
			foreach (string b : pt.Bosses)
			{
				bosses += " " + b;
			}
			string live = "нет";
			VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(pt.Id);
			if (lv)
				live = string.Format("%1 босс=%2 здоровье=%3 аномалий=%4", lv.Kind, lv.BossId, lv.Hp, lv.Anomalies);
			Print(string.Format("[VB_AB] тест редактора: DUMP точка %1 %2 вес=%3 выкл=%4 боссы:%5 · рестарт: %6",
				pt.Id, pt.Position.ToString(), pt.Weight, pt.Disabled, bosses, live));
		}
	}
}
