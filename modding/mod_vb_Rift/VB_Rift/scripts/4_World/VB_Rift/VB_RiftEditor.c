// Внутриигровой редактор гнёзд Изнанки (клиент). Открыть может только админ из settings.json сервера.
//
// Ctrl+F8 (UAVBRiftEditor): войти в редактор / выйти. Пока редактор открыт, по бокам экрана висят панели
// (слева гнёзда и что ставить, справа выбранное гнездо, действия и статус), середина экрана свободна.
// Два режима, Пробел переключает (как в админке VPP):
//   «интерфейс» (по умолчанию) — персонаж и камера стоят, курсор виден, клик ЛКМ по земле ставит объект под курсором;
//     после установки выбор сбрасывается: следующий объект снова выбирают в списке слева;
//   «управление» — всё разблокировано: ходьба, камера, свободная камера VPP; курсора нет, объект встаёт по центру экрана.
// В обоих: колесо — поворот на 15° (с Shift — на 3°); Alt + колесо или PgUp/PgDn — высота (для щупалец);
//   ПКМ или Delete — убрать объект под курсором или в центре экрана (он подсвечен жёлтым).
// Сердце начинает новое гнездо (или становится сердцем гнезда без сердца рядом), корни идут в ближайшее гнездо
// не дальше NEST_RADIUS (иначе в новое), щупальца — к ближайшему сердцу, а вдали от сердец — в постоянный декор,
// который стоит на каждом рестарте (решает сервер).
// Гнёзда рядом видны целиком: что сервер сейчас не поставил, показывается локальными копиями только у админа.

class VB_RiftEditor
{
	static const float PLACE_DIST = 80.0;		// м: дальше курсор не ставит
	static const float TARGET_DIST = 4.0;		// м: объект под курсором (до его начала координат)
	static const float PREVIEW_DIST = 400.0;	// м: гнёзда ближе показываются целиком
	static const int PREVIEW_MS = 1500;
	static const int SCATTER_MIN = 8;			// «Обсадить корнями»: столько корней вокруг сердца
	static const int SCATTER_MAX = 12;
	static const float SCATTER_GAP = 2.5;		// м: не ближе к другим объектам гнезда
	static const string GHOST_TEX = "#(argb,8,8,3)color(1,0.25,0.35,0.6,ca)";
	static const string MARK_TEX = "#(argb,8,8,3)color(1,0.8,0.15,0.75,ca)";
	static const string GHOST_MAT = "\\dz\\gear\\camping\\data\\barbed_wire_deployable.rvmat";	// с «\»: E75

	// что выключаем на время редактора: удары, действия, подъём оружия и прицеливание на ПКМ
	// (ходьба, бег, колесо и смена вида остаются)
	static const ref TStringArray BLOCK_INPUTS = {
		"UAFire", "UAHeavyMeleeAttack", "UADefaultAction", "UAAction", "UATempRaiseWeapon", "UATempRaiseWeaponGamepad",
		"UAZoomIn", "UAZoomInToggle", "UAZoomInOptics", "UAZoomOutOptics", "UAThrowitem", "UADropitem",
		"UAReloadMagazine", "UAToggleWeapons", "UAGetOver"
	};

	// режим «интерфейс»: персонаж не ходит, не меняет стойку, камера стоит
	static const ref TStringArray LOCK_GROUPS = {"aiming", "movement", "stances"};

	static const ref TStringArray TYPES = {
		"VB_RiftHeart",
		"VB_RiftRoots_Crawl1", "VB_RiftRoots_Crawl2", "VB_RiftRoots_Fork1",
		"VB_RiftRoots_Cluster1", "VB_RiftRoots_Cluster2", "VB_RiftRoots_Arch1", "VB_RiftRoots_Arch2",
		"VB_RiftRoots_Small1", "VB_RiftRoots_Small2",
		"VB_RiftRoots_Crawl3", "VB_RiftRoots_Fork2", "VB_RiftRoots_Cluster3", "VB_RiftRoots_Small3",
		"VB_RiftHang_Short1", "VB_RiftHang_Short2", "VB_RiftHang_Long1", "VB_RiftHang_Long2", "VB_RiftHang_Drape1"
	};
	static const ref TStringArray NAMES = {
		"Сердце",
		"Ползучий корень 1", "Ползучий корень 2", "Корень-вилка",
		"Пучок корней 1", "Пучок корней 2", "Корни-змеи 1", "Корни-змеи 2",
		"Мелкие корни 1", "Мелкие корни 2",
		"Ползучий корень 3", "Корень-вилка 2", "Пучок корней 3", "Мелкие корни 3",
		"Щупальца короткие 1", "Щупальца короткие 2", "Щупальца длинные 1", "Щупальца длинные 2", "Щупальца-провис"
	};
	// «Обсадить корнями»: из чего выбирать (повторы — вес)
	static const ref TStringArray SCATTER = {
		"VB_RiftRoots_Crawl1", "VB_RiftRoots_Crawl2", "VB_RiftRoots_Crawl3", "VB_RiftRoots_Crawl1", "VB_RiftRoots_Crawl2", "VB_RiftRoots_Crawl3",
		"VB_RiftRoots_Fork1", "VB_RiftRoots_Fork2", "VB_RiftRoots_Fork1", "VB_RiftRoots_Fork2",
		"VB_RiftRoots_Cluster1", "VB_RiftRoots_Cluster2", "VB_RiftRoots_Cluster3",
		"VB_RiftRoots_Arch1", "VB_RiftRoots_Arch2",
		"VB_RiftRoots_Small1", "VB_RiftRoots_Small2", "VB_RiftRoots_Small3", "VB_RiftRoots_Small1", "VB_RiftRoots_Small2", "VB_RiftRoots_Small3"
	};

	protected static ref VB_RiftEditor s_VB_Instance;

	bool m_VB_Active;
	int m_VB_CurNest;							// выбранное гнездо (0 — нет)
	string m_VB_Type;							// что ставить; "" — ничего (после установки выбор сбрасывается)
	float m_VB_Yaw;
	float m_VB_Height;
	protected bool m_VB_Waiting;				// ждём ответа сервера на вход
	protected int m_VB_WaitUntil;
	protected int m_VB_DeniedAt = -100000;		// когда сервер последний раз ответил «нет прав»
	protected ref VB_RiftEditorPanels m_VB_Panels;
	protected bool m_VB_MenuOpen;				// поверх открыто чужое меню (пауза, чат, диалог)
	bool m_VB_Control;							// Пробел: режим управления (курсора нет, всё разблокировано)
	protected bool m_VB_Look;					// применённый к вводу режим: true — управление
	protected bool m_VB_LookApplied = true;
	protected bool m_VB_PrevSpace;
	protected Object m_VB_Ghost;
	protected string m_VB_GhostType;
	protected bool m_VB_GhostOk;
	protected string m_VB_GhostNote;
	protected vector m_VB_GhostPos;
	protected bool m_VB_AimOk;
	protected vector m_VB_AimPos;
	protected ref map<int, Object> m_VB_Preview;	// id объекта -> локальная копия
	protected ref map<string, float> m_VB_Headings;
	protected int m_VB_SeenVersion = -1;
	protected int m_VB_NextPreviewMs;
	protected int m_VB_TargetId;
	protected Object m_VB_TargetObj;
	protected ref VB_RiftNestObj m_VB_TargetData;	// объект под прицелом и его гнездо (ищутся раз в кадр в VB_UpdateTarget)
	protected ref VB_RiftNest m_VB_TargetNest;
	protected bool m_VB_PrevLmb;
	protected bool m_VB_PrevRmb;
	protected bool m_VB_PrevDel;
	protected bool m_VB_PrevUp;
	protected bool m_VB_PrevDown;
	protected int m_VB_ActivePending = -1;		// «Гнёзд за рестарт»: ещё не отправленное число (-1 — нечего)
	protected int m_VB_ActiveSendAt;
#ifdef DIAG_DEVELOPER
	protected int m_VB_NextTestCheck;
	protected ref VB_RiftEditorTest m_VB_Test;
#endif
	string m_VB_Msg;
	int m_VB_MsgUntil;

	static const int WAIT_MS = 5000;			// столько ждём ответа сервера на вход
	static const int ACTIVE_DELAY_MS = 1000;	// счётчик гнёзд уходит на сервер через столько после последнего клика

	static VB_RiftEditor VB_Get()
	{
		if (!s_VB_Instance)
			s_VB_Instance = new VB_RiftEditor();
		return s_VB_Instance;
	}

	void VB_RiftEditor()
	{
		m_VB_Preview = new map<int, Object>;
		m_VB_Headings = new map<string, float>;
	}

	// --- вход, выход, клавиша ---------------------------------------------------------------------

	static void VB_OnKey()
	{
		// поверх открыто меню (пауза, чат, диалог «Удалить гнездо?») — клавишу не трогаем: выход из редактора
		// под открытым диалогом оставлял бы диалог без панелей, которые ждут его ответа
		if (g_Game.GetUIManager().GetMenu())
			return;

		VB_RiftEditor ed = VB_Get();
		if (ed.m_VB_Active)
		{
			ed.VB_Deactivate();
			return;
		}
		ed.VB_RequestOpen();
	}

	void VB_RequestOpen()
	{
		if (m_VB_Active || (m_VB_Waiting && g_Game.GetTime() < m_VB_WaitUntil))
			return;
		// сервер отвечает не-админу не чаще раза в 5 с, повтор раньше он молча отбросит — не спрашиваем зря
		if (g_Game.GetTime() < m_VB_DeniedAt + WAIT_MS)
		{
			INT_UIF_Toast.Error("Редактор Изнанки: нет прав (" + VB_RiftNestClient.s_VB_DeniedId + ")");
			return;
		}

		m_VB_Waiting = true;
		m_VB_WaitUntil = g_Game.GetTime() + WAIT_MS;
		VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_OPEN);
	}

	// «Гнёзд за рестарт»: на сервер уходит последнее число через ACTIVE_DELAY_MS после последнего клика,
	// а не каждый клик (каждый клик сервер пишет settings.json)
	void VB_QueueActive(int n)
	{
		m_VB_ActivePending = n;
		m_VB_ActiveSendAt = g_Game.GetTime() + ACTIVE_DELAY_MS;
	}

	// число, которое сейчас показывать в счётчике: ещё не отправленное или то, что прислал сервер
	int VB_ActiveShown()
	{
		if (m_VB_ActivePending >= 0)
			return m_VB_ActivePending;
		return VB_RiftNestClient.s_VB_ActiveNests;
	}

	protected void VB_FlushActive(bool now)
	{
		if (m_VB_ActivePending < 0)
			return;
		if (!now && g_Game.GetTime() < m_VB_ActiveSendAt)
			return;

		int n = m_VB_ActivePending;
		m_VB_ActivePending = -1;
		VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_SET_ACTIVE, n);
	}

	protected void VB_Activate()
	{
		m_VB_Active = true;
		m_VB_SeenVersion = -1;
		m_VB_PrevLmb = true;
		m_VB_PrevSpace = true;
		m_VB_PrevRmb = true;
		m_VB_Control = false;
		m_VB_Look = false;
		m_VB_Type = "";
		m_VB_LookApplied = true;	// чтобы VB_ApplyLook сразу включил курсор
		m_VB_MenuOpen = false;
		g_Game.GetMission().AddActiveInputExcludes(BLOCK_INPUTS);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && !VB_RiftNestClient.VB_Find(m_VB_CurNest))
			m_VB_CurNest = VB_NearestNest(player.GetPosition(), 1000000.0);

		m_VB_Panels = new VB_RiftEditorPanels();
		m_VB_Panels.VB_Build();
		VB_ApplyLook();
		Print("[VB_Rift] редактор гнёзд: вход");
	}

	void VB_Deactivate()
	{
		if (!m_VB_Active)
			return;

		m_VB_Active = false;
		m_VB_Waiting = false;
		VB_FlushActive(true);
		VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_CLOSE);
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
		Print("[VB_Rift] редактор гнёзд: выход");
	}

	protected void VB_Cleanup()
	{
		VB_SetTarget(0, null);
		VB_DeleteGhost();
		VB_ClearPreview();
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
		s_VB_Instance.m_VB_ActivePending = -1;
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

	// мышь над панелями редактора
	bool VB_OverPanels()
	{
		if (!m_VB_Panels || m_VB_Look)
			return false;
		int mx;
		int my;
		GetMousePos(mx, my);
		return m_VB_Panels.VB_Contains(mx, my);
	}

	// --- кадр -------------------------------------------------------------------------------------

	static void VB_UpdateStatic(float dt)
	{
		VB_Get().VB_Update(dt);
	}

	void VB_Update(float dt)
	{
		VB_PollNet();
		VB_FlushActive(false);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
#ifdef DIAG_DEVELOPER
		// автотест: файл команд в профиле клиента проверяется раз в 5 с, прочитанный удаляется. Только в DayZDiag
		// (стенд): на боевом клиенте файл не читается — забытый файл у админа не выполнит команды на сервере,
		// а обычный игрок не сделает из него макрос запросов (аудит 2026-11-13, №28)
		if (m_VB_Test && m_VB_Test.VB_Done())
			m_VB_Test = null;
		if (!m_VB_Test && player && player.IsAlive() && g_Game.GetTime() >= m_VB_NextTestCheck)
		{
			m_VB_NextTestCheck = g_Game.GetTime() + 5000;
			m_VB_Test = VB_RiftEditorTest.VB_TryLoad();
		}
		if (m_VB_Test)
			m_VB_Test.VB_Tick(this);
#endif

		if (!m_VB_Active)
			return;
		if (!player || !player.IsAlive())
		{
			VB_Deactivate();
			return;
		}

		if (VB_RiftNestClient.s_VB_Version != m_VB_SeenVersion || g_Game.GetTime() >= m_VB_NextPreviewMs)
		{
			m_VB_SeenVersion = VB_RiftNestClient.s_VB_Version;
			VB_RefreshPreview(player.GetPosition());
		}

		// чужое меню поверх (пауза, чат): панели и призрак прячем. Диалог подтверждения фреймворка («Удалить гнездо?»)
		// панели не прячет: его окно поднимаем выше панелей, оно встаёт поверх них
		UIScriptedMenu menu = g_Game.GetUIManager().GetMenu();
		if (menu)
		{
			if (!m_VB_MenuOpen)
			{
				m_VB_MenuOpen = true;
				bool dialog = menu.GetID() == INT_UIF_MENU_DIALOG;
				if (dialog && menu.GetLayoutRoot())
					menu.GetLayoutRoot().SetSort(VB_RiftEditorPanels.SORT + 100);
				if (m_VB_Panels && !dialog)
					m_VB_Panels.VB_Show(false);
				VB_HideGhost();
				VB_SetTarget(0, null);
			}
			m_VB_PrevLmb = true;
			m_VB_PrevRmb = true;
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
		VB_UpdateGhost(player);
		VB_UpdateTarget();
		VB_HandleInput();
		if (m_VB_Panels)
			m_VB_Panels.VB_Tick(this);
	}

	protected void VB_UpdateLook()
	{
		// окно игры не активно: кнопки мыши читаются напрямую и ловят клики по другим окнам — не реагируем
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
		VB_RiftNestClient.VB_Init();
		if (VB_RiftNestClient.s_VB_Denied)
		{
			VB_RiftNestClient.s_VB_Denied = false;
			m_VB_Waiting = false;
			m_VB_DeniedAt = g_Game.GetTime();
			INT_UIF_Toast.Error("Редактор Изнанки: нет прав (" + VB_RiftNestClient.s_VB_DeniedId + ")");
			Print("[VB_Rift] редактор гнёзд: нет прав, SteamID " + VB_RiftNestClient.s_VB_DeniedId);
			// права сняли, пока редактор открыт (админа убрали из settings.json): выйти
			if (m_VB_Active)
				VB_Deactivate();
		}

		if (VB_RiftNestClient.s_VB_Granted)
		{
			VB_RiftNestClient.s_VB_Granted = false;
			if (m_VB_Waiting && !m_VB_Active)
			{
				m_VB_Waiting = false;
				VB_Activate();
			}
		}

		// сервер молчит: на нём нет серверного мода VB_Rift_Server (или он не загрузился)
		if (m_VB_Waiting && !m_VB_Active && g_Game.GetTime() >= m_VB_WaitUntil)
		{
			m_VB_Waiting = false;
			int waitSec = WAIT_MS / 1000;
			INT_UIF_Toast.Error("Редактор Изнанки: сервер не ответил (нет VB_Rift_Server?)");
			Print("[VB_Rift] редактор гнёзд: сервер не ответил за " + waitSec.ToString() + " с (нет VB_Rift_Server?)");
		}

		if (VB_RiftNestClient.s_VB_SelectNest != 0)
		{
			m_VB_CurNest = VB_RiftNestClient.s_VB_SelectNest;
			VB_RiftNestClient.s_VB_SelectNest = 0;
		}

		while (VB_RiftNestClient.s_VB_Msgs.Count() > 0)
		{
			string msg = VB_RiftNestClient.s_VB_Msgs[0];
			VB_RiftNestClient.s_VB_Msgs.Remove(0);
			VB_Note(msg);
		}
	}

	void VB_Note(string text)
	{
		m_VB_Msg = text;
		m_VB_MsgUntil = g_Game.GetTime() + 8000;
		Print("[VB_Rift] редактор гнёзд: " + text);
	}

	// --- призрак ----------------------------------------------------------------------------------

	static bool VB_IsGroundType(string type)
	{
		return g_Game.ConfigGetInt("CfgVehicles " + type + " vbAlignToTerrain") == 1;
	}

	static string VB_TypeName(string type)
	{
		int i = TYPES.Find(type);
		if (i < 0)
			return type;
		return NAMES[i];
	}

	// модель без пульса и прочего (призрак, замер)
	static Object VB_CreateBare(string type, vector pos)
	{
		VB_RiftBase.s_VB_Bare = true;
		Object obj = g_Game.CreateObjectEx(type, pos, ECE_LOCAL);
		VB_RiftBase.s_VB_Bare = false;
		VB_RiftBase rift = VB_RiftBase.Cast(obj);
		if (rift)
			rift.m_VB_Bare = true;
		return obj;
	}

	protected void VB_MakeGhost(vector at)
	{
		VB_DeleteGhost();
		m_VB_GhostType = m_VB_Type;
		m_VB_Ghost = VB_CreateBare(m_VB_Type, at);
		VB_RiftBase rift = VB_RiftBase.Cast(m_VB_Ghost);
		if (rift)
			rift.VB_SetGhostLook(GHOST_TEX, GHOST_MAT);
	}

	protected void VB_DeleteGhost()
	{
		if (m_VB_Ghost && g_Game)
			g_Game.ObjectDelete(m_VB_Ghost);
		m_VB_Ghost = null;
		m_VB_GhostType = "";
	}

	protected void VB_HideGhost()
	{
		if (m_VB_Ghost)
			m_VB_Ghost.SetPosition(Vector(0, -1000, 0));
		m_VB_GhostOk = false;
	}

	// луч из камеры через курсор (в режиме управления — в центр экрана; со свободной камерой — из неё)
	protected void VB_UpdateGhost(PlayerBase player)
	{
		bool picked = m_VB_Type != "";
		bool ground = picked && VB_IsGroundType(m_VB_Type);
		vector from = g_Game.GetCurrentCameraPosition();
		vector dir = g_Game.GetCurrentCameraDirection();
		if (!m_VB_Look)
			dir = g_Game.GetPointerDirection();
		vector to = from + dir * PLACE_DIST;
		vector hit;
		vector hitDir;
		int comp;
		m_VB_AimOk = false;
		m_VB_GhostNote = "";
		if (!m_VB_Look && VB_OverPanels())
		{
			VB_HideGhost();
			return;
		}

		m_VB_AimOk = DayZPhysics.RaycastRV(from, to, hit, hitDir, comp, null, m_VB_Ghost, player, false, ground);
		if (!m_VB_AimOk)
		{
			m_VB_GhostNote = "дальше " + PLACE_DIST.ToString() + " м";
			VB_HideGhost();
			return;
		}
		m_VB_AimPos = hit;

		// ничего не выбрано: призрака нет, прицел нужен только чтобы удалять (Delete)
		if (!picked)
		{
			VB_DeleteGhost();
			m_VB_GhostOk = false;
			return;
		}

		vector pos = hit;
		if (ground)
		{
			if (g_Game.SurfaceIsSea(hit[0], hit[2]) || g_Game.SurfaceIsPond(hit[0], hit[2]))
			{
				m_VB_GhostNote = "вода";
				VB_HideGhost();
				return;
			}
			pos[1] = g_Game.SurfaceY(hit[0], hit[2]);
		}
		else
		{
			pos[1] = pos[1] + m_VB_Height;
		}

		if (!m_VB_Ghost || m_VB_GhostType != m_VB_Type)
			VB_MakeGhost(pos);

		m_VB_GhostPos = pos;
		m_VB_GhostOk = true;
		VB_RiftBase rift = VB_RiftBase.Cast(m_VB_Ghost);
		if (rift)
			rift.VB_PlaceAt(pos, Vector(m_VB_Yaw, 0, 0));
		else if (m_VB_Ghost)
			m_VB_Ghost.SetPosition(pos);
	}

	// --- объект под курсором ----------------------------------------------------------------------

	// объект гнезда ближе TARGET_DIST к точке прицела. Гнёзда, до которых от прицела дальше их разброса,
	// не перебираются (заранее посчитанные центры VB_RiftNestClient.VB_Views, аудит №38)
	protected void VB_UpdateTarget()
	{
		m_VB_TargetData = null;
		m_VB_TargetNest = null;
		if (!m_VB_AimOk)
		{
			VB_SetTarget(0, null);
			return;
		}

		VB_RiftNestObj best;
		VB_RiftNest bestNest;
		float bestD = TARGET_DIST;
		float d;
		array<ref VB_RiftNestView> views = VB_RiftNestClient.VB_Views();
		foreach (VB_RiftNestView v : views)
		{
			d = VB_RiftNestUtil.VB_DistXZ(v.Center, m_VB_AimPos);
			if (d > v.Reach + TARGET_DIST)
				continue;

			foreach (VB_RiftNestObj o : v.Nest.Objects)
			{
				d = vector.Distance(o.Position, m_VB_AimPos);
				if (d < bestD)
				{
					bestD = d;
					best = o;
					bestNest = v.Nest;
				}
			}
		}

		if (best)
		{
			m_VB_TargetData = best;
			m_VB_TargetNest = bestNest;
			VB_SetTarget(best.Id, best);
		}
		else
		{
			VB_SetTarget(0, null);
		}
	}

	protected void VB_SetTarget(int id, VB_RiftNestObj o)
	{
		if (id == m_VB_TargetId && (id == 0 || m_VB_TargetObj))
			return;

		VB_RiftBase old = VB_RiftBase.Cast(m_VB_TargetObj);
		if (old)
			old.VB_SetHighlight(false);
		m_VB_TargetId = id;
		m_VB_TargetObj = null;
		if (id == 0 || !o)
			return;

		m_VB_TargetObj = VB_FindModel(o);
		VB_RiftBase rift = VB_RiftBase.Cast(m_VB_TargetObj);
		if (rift)
			rift.VB_SetHighlight(true, MARK_TEX, GHOST_MAT);
	}

	// модель объекта гнезда: локальная копия или то, что поставил сервер
	protected Object VB_FindModel(VB_RiftNestObj o)
	{
		Object obj;
		if (m_VB_Preview.Find(o.Id, obj) && obj)
			return obj;

		array<Object> found = new array<Object>;
		g_Game.GetObjectsAtPosition(o.Position, 1.0, found, null);
		foreach (Object f : found)
		{
			if (f && f.GetType() == o.Type && VB_RiftNestUtil.VB_DistXZ(f.GetPosition(), o.Position) < 0.3)
				return f;
		}
		return null;
	}

	// объект под прицелом (найден в этом кадре VB_UpdateTarget, без нового перебора)
	VB_RiftNestObj VB_TargetObj(out VB_RiftNest inNest)
	{
		if (m_VB_TargetId == 0 || !m_VB_TargetData || m_VB_TargetData.Id != m_VB_TargetId)
			return null;
		inNest = m_VB_TargetNest;
		return m_VB_TargetData;
	}

	// --- ввод -------------------------------------------------------------------------------------

	protected void VB_HandleInput()
	{
		// клики и клавиши в других окнах сюда не пускаем (без этого на стенде ставились лишние сердца)
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
			VB_PlaceCurrent();
		m_VB_PrevLmb = lmb;

		// удалить подсвеченный объект: ПКМ (не над панелями) или Delete
		bool rmb = (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) != 0;
		if (rmb && !m_VB_PrevRmb && !over && m_VB_TargetId != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_DEL, m_VB_TargetId);
		m_VB_PrevRmb = rmb;

		bool del = KeyState(KeyCode.KC_DELETE) != 0;
		if (del && !m_VB_PrevDel && m_VB_TargetId != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_DEL, m_VB_TargetId);
		m_VB_PrevDel = del;

		bool up = KeyState(KeyCode.KC_PRIOR) != 0;
		bool down = KeyState(KeyCode.KC_NEXT) != 0;
		if (up && !m_VB_PrevUp)
			VB_AddHeight(0.25);
		if (down && !m_VB_PrevDown)
			VB_AddHeight(-0.25);
		m_VB_PrevUp = up;
		m_VB_PrevDown = down;

		// колесо над панелью листает список, над миром — крутит объект
		int step = 0;
		if (GetUApi().GetInputByID(UAPrevAction).LocalPress())
			step = 1;
		else if (GetUApi().GetInputByID(UANextAction).LocalPress())
			step = -1;
		if (step == 0 || over)
			return;

		bool alt = KeyState(KeyCode.KC_LMENU) != 0 || KeyState(KeyCode.KC_RMENU) != 0;
		if (alt)
		{
			VB_AddHeight(step * 0.25);
			return;
		}

		bool shift = KeyState(KeyCode.KC_LSHIFT) != 0 || KeyState(KeyCode.KC_RSHIFT) != 0;
		float deg = 15.0;
		if (shift)
			deg = 3.0;
		m_VB_Yaw = Math.ModFloat(m_VB_Yaw + step * deg + 720.0, 360.0);
	}

	protected void VB_AddHeight(float dh)
	{
		m_VB_Height = Math.Clamp(m_VB_Height + dh, -3.0, 15.0);
	}

	void VB_PlaceCurrent()
	{
		if (m_VB_Type == "")
		{
			VB_Note("Выбери в списке слева, что ставить");
			return;
		}
		if (!m_VB_GhostOk)
		{
			if (m_VB_GhostNote != "")
				VB_Note("Сюда не поставить: " + m_VB_GhostNote);
			return;
		}
		VB_SendOne(m_VB_Type, m_VB_GhostPos, Vector(m_VB_Yaw, 0, 0));

		// поставил — выбор сбрасывается, следующий объект выбирать заново (решение пользователя 11-11)
		m_VB_Type = "";
		VB_DeleteGhost();
	}

	void VB_SendOne(string type, vector pos, vector ori)
	{
		TStringArray types = new TStringArray;
		array<vector> poses = new array<vector>;
		array<vector> oris = new array<vector>;
		types.Insert(type);
		poses.Insert(pos);
		oris.Insert(ori);
		VB_RiftNestClient.VB_SendAdd(m_VB_CurNest, types, poses, oris);
	}

	// --- гнёзда -----------------------------------------------------------------------------------

	// центры и сердца гнёзд берутся из VB_RiftNestClient.VB_Views (посчитаны один раз на версию копии):
	// статус редактора зовёт эти поиски каждый кадр (аудит №38)
	int VB_NearestNest(vector pos, float maxDist, bool heartlessOnly = false)
	{
		int best = 0;
		float bestD = maxDist;
		float d;
		array<ref VB_RiftNestView> views = VB_RiftNestClient.VB_Views();
		foreach (VB_RiftNestView v : views)
		{
			if (v.Nest.Objects.Count() == 0 || v.Nest.Id == VB_RiftNestNet.DECOR_ID)
				continue;
			if (heartlessOnly && v.HasHeart)
				continue;
			d = VB_RiftNestUtil.VB_DistXZ(v.Center, pos);
			if (d < bestD)
			{
				bestD = d;
				best = v.Nest.Id;
			}
		}
		return best;
	}

	// гнездо, чьё сердце ближе всех к pos (не дальше NEST_RADIUS); 0 — нет
	int VB_NearestHeartNest(vector pos)
	{
		int best = 0;
		float bestD = VB_RiftNestNet.NEST_RADIUS;
		float d;
		array<ref VB_RiftNestView> views = VB_RiftNestClient.VB_Views();
		foreach (VB_RiftNestView v : views)
		{
			if (v.Nest.Id == VB_RiftNestNet.DECOR_ID || !v.HasHeart)
				continue;
			d = VB_RiftNestUtil.VB_DistXZ(v.Heart, pos);
			if (d < bestD)
			{
				bestD = d;
				best = v.Nest.Id;
			}
		}
		return best;
	}

	// куда ляжет выбранный объект в точке pos (так же решает сервер): 0 — в новое гнездо, DECOR_ID — в декор.
	// Сердце начинает новое гнездо (или становится сердцем гнезда без сердца рядом), корни — в ближайшее,
	// щупальца — к ближайшему сердцу, а без сердца рядом — в постоянный декор
	int VB_NestFor(vector pos)
	{
		if (m_VB_Type != "" && VB_RiftNestUtil.VB_IsHangType(m_VB_Type))
		{
			int hangNest = VB_NearestHeartNest(pos);
			if (hangNest == 0)
				return VB_RiftNestNet.DECOR_ID;
			return hangNest;
		}
		bool heart = m_VB_Type != "" && VB_RiftNestUtil.VB_IsHeartType(m_VB_Type);
		return VB_NearestNest(pos, VB_RiftNestNet.NEST_RADIUS, heart);
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

	void VB_Undo()
	{
		if (m_VB_CurNest != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_UNDO, m_VB_CurNest);
	}

	void VB_Teleport(int nestId)
	{
		if (nestId != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_TELEPORT, nestId);
	}

	void VB_ToggleNest()
	{
		if (m_VB_CurNest != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_NEST_TOGGLE, m_VB_CurNest);
	}

	void VB_DeleteNest()
	{
		if (m_VB_CurNest != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_NEST_DEL, m_VB_CurNest);
	}

	void VB_Reroll()
	{
		VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_REROLL);
	}

	// направление модели корня в градусах (от +Z к +X) по средней точке срезателей травы cut_*;
	// -1000 — без направления (пучки, россыпь)
	protected float VB_TypeHeading(string type)
	{
		float h;
		if (m_VB_Headings.Find(type, h))
			return h;

		h = -1000;
		vector at = g_Game.GetPlayer().GetPosition() - "0 60 0";
		Object probe = VB_CreateBare(type, at);
		if (probe)
		{
			vector sum = vector.Zero;
			int n = 0;
			string mp;
			for (int i = 1; i < 200; i++)
			{
				mp = "cut_" + i.ToString();
				if (!probe.MemoryPointExists(mp))
					break;
				sum = sum + probe.GetMemoryPointPos(mp);
				n++;
			}
			g_Game.ObjectDelete(probe);

			if (n > 0)
			{
				sum = sum * (1.0 / n);
				if (Math.Sqrt(sum[0] * sum[0] + sum[2] * sum[2]) > 0.8)
					h = Math.Atan2(sum[0], sum[2]) * Math.RAD2DEG;
			}
		}
		m_VB_Headings.Set(type, h);
		return h;
	}

	protected void VB_ScatterRadius(string type, out float rmin, out float rmax)
	{
		rmin = 2.5;
		rmax = 5.0;
		if (type.IndexOf("Cluster") >= 0)
		{
			rmin = 6.0;
			rmax = 11.0;
		}
		else if (type.IndexOf("Small") >= 0)
		{
			rmin = 4.0;
			rmax = 13.0;
		}
	}

	// не вода и не здание
	protected bool VB_GoodGround(float x, float z)
	{
		if (g_Game.SurfaceIsSea(x, z) || g_Game.SurfaceIsPond(x, z))
			return false;

		float y = g_Game.SurfaceY(x, z);
		vector from = Vector(x, y + 25.0, z);
		vector to = Vector(x, y - 0.5, z);
		vector hit;
		vector hitDir;
		int comp;
		set<Object> results = new set<Object>;
		DayZPhysics.RaycastRV(from, to, hit, hitDir, comp, results, null, g_Game.GetPlayer(), false, false);
		foreach (Object obj : results)
		{
			if (obj && obj.IsBuilding())
				return false;
		}
		return true;
	}

	// корни вокруг сердца выбранного гнезда: ползучие, вилки и змеи начинаются у сердца и смотрят наружу,
	// пучки и россыпь — дальше и в случайную сторону
	void VB_Scatter()
	{
		VB_RiftNest nest = VB_RiftNestClient.VB_Find(m_VB_CurNest);
		if (!nest)
		{
			VB_Note("Сначала выбери гнездо");
			return;
		}
		VB_RiftNestObj heart = nest.VB_FindHeart();
		if (!heart)
		{
			VB_Note("В гнезде нет сердца: обсадить не вокруг чего");
			return;
		}

		vector hp = heart.Position;
		array<vector> used = new array<vector>;
		foreach (VB_RiftNestObj o : nest.Objects)
		{
			used.Insert(o.Position);
		}

		TStringArray types = new TStringArray;
		array<vector> poses = new array<vector>;
		array<vector> oris = new array<vector>;
		int want = Math.RandomIntInclusive(SCATTER_MIN, SCATTER_MAX);
		float base = Math.RandomFloat(0, 360);
		int tries = 0;
		string type;
		float rmin;
		float rmax;
		float r;
		float bearing;
		float rad;
		float sx;
		float cz;
		float x;
		float z;
		float heading;
		float yaw;
		float jitter;
		vector p;
		while (types.Count() < want && tries < want * 6)
		{
			tries++;
			type = SCATTER.GetRandomElement();
			VB_ScatterRadius(type, rmin, rmax);
			r = Math.RandomFloat(rmin, rmax);
			bearing = base + tries * 137.508;		// золотой угол: равномерно по кругу
			rad = bearing * Math.DEG2RAD;
			sx = Math.Sin(rad);
			cz = Math.Cos(rad);
			x = hp[0] + sx * r;
			z = hp[2] + cz * r;
			if (!VB_GoodGround(x, z))
				continue;

			p = Vector(x, g_Game.SurfaceY(x, z), z);
			if (VB_TooClose(p, used, SCATTER_GAP))
				continue;

			heading = VB_TypeHeading(type);
			if (heading < -500)
			{
				yaw = Math.RandomFloat(0, 360);
			}
			else
			{
				jitter = Math.RandomFloat(-15, 15);
				yaw = bearing - heading + jitter;
			}
			yaw = Math.ModFloat(yaw + 720.0, 360.0);

			types.Insert(type);
			poses.Insert(p);
			oris.Insert(Vector(yaw, 0, 0));
			used.Insert(p);
		}

		if (types.Count() == 0)
		{
			VB_Note("Вокруг сердца некуда ставить корни");
			return;
		}
		VB_RiftNestClient.VB_SendAdd(nest.Id, types, poses, oris, true);
		VB_Note("Обсажено корнями: " + types.Count().ToString());
	}

	protected bool VB_TooClose(vector p, array<vector> used, float gap)
	{
		foreach (vector u : used)
		{
			if (VB_RiftNestUtil.VB_DistXZ(p, u) < gap)
				return true;
		}
		return false;
	}

	// --- предпросмотр -----------------------------------------------------------------------------

	// гнёзда в PREVIEW_DIST: всё, что сервер сейчас не поставил, — локальными копиями
	protected void VB_RefreshPreview(vector ppos)
	{
		m_VB_NextPreviewMs = g_Game.GetTime() + PREVIEW_MS;
		set<int> want = new set<int>;
		Object obj;
		VB_RiftBase rift;
		array<ref VB_RiftNestView> views = VB_RiftNestClient.VB_Views();
		foreach (VB_RiftNestView v : views)
		{
			if (VB_RiftNestUtil.VB_DistXZ(v.Center, ppos) > PREVIEW_DIST)
				continue;

			foreach (VB_RiftNestObj o : v.Nest.Objects)
			{
				if (VB_RiftNestClient.s_VB_LiveObjs.Find(o.Id) >= 0)
					continue;
				// в nests.json вписали руками чужой класс: копию не создаём (сервер его тоже не ставит)
				if (!VB_RiftNestUtil.VB_IsNestType(o.Type))
					continue;

				want.Insert(o.Id);
				if (m_VB_Preview.Contains(o.Id))
					continue;

				obj = g_Game.CreateObjectEx(o.Type, o.Position, ECE_LOCAL);
				if (!obj)
					continue;

				rift = VB_RiftBase.Cast(obj);
				if (rift)
				{
					rift.VB_PlaceAt(o.Position, o.Orientation);
				}
				else
				{
					obj.SetPosition(o.Position);
					obj.SetOrientation(o.Orientation);
				}
				m_VB_Preview.Set(o.Id, obj);
			}
		}

		TIntArray drop = new TIntArray;
		foreach (int id, Object have : m_VB_Preview)
		{
			if (want.Find(id) < 0)
				drop.Insert(id);
		}
		foreach (int dropId : drop)
		{
			Object gone = m_VB_Preview.Get(dropId);
			if (gone && gone == m_VB_TargetObj)
				VB_SetTarget(0, null);
			if (gone)
				g_Game.ObjectDelete(gone);
			m_VB_Preview.Remove(dropId);
		}
	}

	protected void VB_ClearPreview()
	{
		if (!m_VB_Preview)
			return;

		foreach (int id, Object obj : m_VB_Preview)
		{
			if (obj && g_Game)
				g_Game.ObjectDelete(obj);
		}
		m_VB_Preview.Clear();
	}

	int VB_PreviewCount()
	{
		return m_VB_Preview.Count();
	}

	static string VB_CountWord(int n, string one, string few, string many)
	{
		int n100 = n % 100;
		int n10 = n % 10;
		if (n100 >= 11 && n100 <= 14)
			return n.ToString() + " " + many;
		if (n10 == 1)
			return n.ToString() + " " + one;
		if (n10 >= 2 && n10 <= 4)
			return n.ToString() + " " + few;
		return n.ToString() + " " + many;
	}
}

// Панели редактора по бокам экрана, открыты всё время, пока редактор включён. Слева — гнёзда (ближние сверху)
// и что ставить, справа — выбранное гнездо, действия и статус. Середина экрана свободна для расстановки.
// Managed: на методы панелей подписаны кнопки и диалог подтверждения; если панели снесут раньше, чем придёт
// ответ, подписчик обнулится, а не позовётся на освобождённой памяти. Открытый диалог VB_Destroy ещё и закрывает.
class VB_RiftEditorPanels : Managed
{
	static const float PANEL_W = 400;
	static const float TOP = 70;
	static const float EDGE = 16;
	static const float REFILL_MOVE = 15.0;	// м: игрок отошёл — пересчитать расстояния в списке
	static const int SORT = 900;			// панели выше HUD; диалог подтверждения поднимается выше них

	protected ref INT_UIF_Bag m_VB_Bag;
	protected Widget m_VB_Left;
	protected Widget m_VB_Right;
	protected INT_UIF_List m_VB_NestList;
	protected INT_UIF_List m_VB_TypeList;
	protected TextWidget m_VB_CurTitle;
	protected TextWidget m_VB_CurSub;
	protected INT_UIF_Button m_VB_BtnScatter;
	protected INT_UIF_Button m_VB_BtnUndo;
	protected INT_UIF_Button m_VB_BtnTp;
	protected INT_UIF_Button m_VB_BtnToggle;
	protected INT_UIF_Button m_VB_BtnDelete;
	protected INT_UIF_StatTile m_VB_TilePool;
	protected INT_UIF_Stepper m_VB_ActiveStep;	// сколько гнёзд встаёт за рестарт (ActiveNests в settings.json)
	protected INT_UIF_StatTile m_VB_TileLive;
	protected TextWidget m_VB_Rules;
	protected TextWidget m_VB_StNest;
	protected TextWidget m_VB_StPlace;
	protected TextWidget m_VB_StTarget;
	protected TextWidget m_VB_StMsg;
	protected TextWidget m_VB_StMode;
	protected int m_VB_SeenVersion = -1;
	protected int m_VB_SeenNest = -1;
	protected string m_VB_SeenType;
	protected vector m_VB_FillPos;
	protected bool m_VB_Filling;
	protected INT_UIF_Dialog m_VB_Dialog;		// открытый диалог подтверждения (Managed: обнулится сам, когда закроется)

	void VB_Build()
	{
		m_VB_Bag = new INT_UIF_Bag();
		float canvasW = INT_UIF_Scale.ScreenW() / INT_UIF_Scale.Factor();
		float w = PANEL_W;

		// --- слева: гнёзда и объекты ---
		m_VB_Left = INT_UIF_Draw.Frame(null, EDGE, TOP, w, 950, "VB_RiftEditorLeft");
		if (!m_VB_Left)
			return;
		m_VB_Left.SetSort(SORT);

		INT_UIF_Card nests = INT_UIF_Card.Create(m_VB_Bag, m_VB_Left, 0, 0, w, 420, "ГНЁЗДА · БЛИЖНИЕ СВЕРХУ", "list", "");
		m_VB_NestList = INT_UIF_List.Create(m_VB_Bag, nests.GetContent(), 0, 0, nests.GetContentW(), nests.GetContentH(), "Гнёзд пока нет: поставь сердце");
		m_VB_NestList.SetSelectable(true);	// у списков фреймворка выбор по клику по умолчанию выключен
		m_VB_NestList.OnSelected().Insert(OnVBNestSelected);
		m_VB_NestList.OnActivated().Insert(OnVBNestActivated);

		INT_UIF_Card types = INT_UIF_Card.Create(m_VB_Bag, m_VB_Left, 0, 432, w, 518, "ЧТО СТАВИТЬ", "star", "");
		m_VB_TypeList = INT_UIF_List.Create(m_VB_Bag, types.GetContent(), 0, 0, types.GetContentW(), types.GetContentH(), "");
		m_VB_TypeList.SetSelectable(true);
		m_VB_TypeList.OnSelected().Insert(OnVBTypeSelected);

		// --- справа: гнездо, действия, статус ---
		m_VB_Right = INT_UIF_Draw.Frame(null, canvasW - EDGE - w, TOP, w, 950, "VB_RiftEditorRight");
		if (!m_VB_Right)
			return;
		m_VB_Right.SetSort(SORT);

		INT_UIF_Card acts = INT_UIF_Card.Create(m_VB_Bag, m_VB_Right, 0, 0, w, 620, "ГНЕЗДО", "pin", "");
		Widget c = acts.GetContent();
		float cw = acts.GetContentW();
		float y = 0;
		m_VB_CurTitle = INT_UIF_Draw.Text(c, 0, y, cw, 26, "", 18, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_VB_CurTitle)
			m_VB_CurTitle.SetBold(true);
		y += 28;
		m_VB_CurSub = INT_UIF_Draw.Text(c, 0, y, cw, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		y += 30;

		float bh = 36;
		float step = bh + 6;
		m_VB_BtnScatter = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Обсадить корнями", INT_UIF_Variant.PRIMARY, "star");
		m_VB_BtnScatter.OnClick().Insert(OnVBScatter);
		y += step;
		m_VB_BtnUndo = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Убрать последний объект", INT_UIF_Variant.SECONDARY, "chevron_left");
		m_VB_BtnUndo.OnClick().Insert(OnVBUndo);
		y += step;
		m_VB_BtnTp = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Перенестись к гнезду", INT_UIF_Variant.SECONDARY, "pin");
		m_VB_BtnTp.OnClick().Insert(OnVBTeleport);
		y += step;
		m_VB_BtnToggle = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Выключить из выбора", INT_UIF_Variant.SECONDARY, "eye");
		m_VB_BtnToggle.OnClick().Insert(OnVBToggle);
		y += step;
		m_VB_BtnDelete = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Удалить гнездо", INT_UIF_Variant.DANGER, "trash");
		m_VB_BtnDelete.OnClick().Insert(OnVBDelete);
		y += step + 4;

		INT_UIF_Divider.Create(m_VB_Bag, c, 0, y, cw);
		y += 12;
		INT_UIF_Draw.Text(c, 0, y, cw - 170, 34, "Гнёзд за рестарт", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_VB_ActiveStep = INT_UIF_Stepper.Create(m_VB_Bag, c, cw - 160, y, 160, 0, 500, VB_RiftNestClient.s_VB_ActiveNests, 1, "", 34);
		m_VB_ActiveStep.OnChanged().Insert(OnVBActiveChanged);
		y += 46;
		float tw = (cw - 10) * 0.5;
		m_VB_TilePool = INT_UIF_StatTile.Create(m_VB_Bag, c, 0, y, tw, 60, "ГНЁЗД С СЕРДЦЕМ", "0", INT_UIF_ThemeKeys.TEXT);
		m_VB_TileLive = INT_UIF_StatTile.Create(m_VB_Bag, c, tw + 10, y, tw, 60, "НА КАРТЕ", "0", INT_UIF_ThemeKeys.RED);
		y += 68;
		m_VB_Rules = INT_UIF_Draw.MText(c, 0, y, cw, 44, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		y += 50;

		INT_UIF_Button reroll = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Перебросить гнёзда", INT_UIF_Variant.WARNING, "refresh");
		reroll.OnClick().Insert(OnVBReroll);
		y += step;
		INT_UIF_Button quit = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, cw, bh, "Выйти из редактора", INT_UIF_Variant.GHOST, "logout");
		quit.OnClick().Insert(OnVBQuit);

		INT_UIF_Card status = INT_UIF_Card.Create(m_VB_Bag, m_VB_Right, 0, 632, w, 200, "СТАТУС", "info", "");
		Widget s = status.GetContent();
		float sw = status.GetContentW();
		m_VB_StMode = INT_UIF_Draw.Text(s, 0, 0, sw, 20, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		m_VB_StNest = INT_UIF_Draw.Text(s, 0, 22, sw, 24, "", 15, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_VB_StPlace = INT_UIF_Draw.Text(s, 0, 48, sw, 22, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		m_VB_StTarget = INT_UIF_Draw.Text(s, 0, 72, sw, 22, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.AMBER_TEXT));
		m_VB_StMsg = INT_UIF_Draw.Text(s, 0, 98, sw, 22, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));

		VB_FillTypes();
		VB_Fill();
	}

	void VB_Destroy()
	{
		// диалог «Удалить гнездо?» / «Перебросить?» не должен пережить панели: ответ без панелей некому принять
		if (m_VB_Dialog && !m_VB_Dialog.IsFinished())
		{
			m_VB_Dialog.OnResult().Clear();
			if (!m_VB_Dialog.IsFinished())
				m_VB_Dialog.Cancel();
		}
		m_VB_Dialog = null;

		m_VB_Bag = null;	// сначала компоненты (они сами снимают свои виджеты), потом корни
		if (m_VB_Left)
			m_VB_Left.Unlink();
		if (m_VB_Right)
			m_VB_Right.Unlink();
		m_VB_Left = null;
		m_VB_Right = null;
	}

	void VB_Show(bool show)
	{
		if (m_VB_Left)
			m_VB_Left.Show(show);
		if (m_VB_Right)
			m_VB_Right.Show(show);
	}

	bool VB_Contains(int x, int y)
	{
		return INT_UIF_Widgets.Contains(m_VB_Left, x, y) || INT_UIF_Widgets.Contains(m_VB_Right, x, y);
	}

	void VB_Tick(VB_RiftEditor ed)
	{
		vector ppos = vector.Zero;
		if (g_Game.GetPlayer())
			ppos = g_Game.GetPlayer().GetPosition();

		float moved = VB_RiftNestUtil.VB_DistXZ(ppos, m_VB_FillPos);
		if (m_VB_SeenVersion != VB_RiftNestClient.s_VB_Version || m_VB_SeenNest != ed.m_VB_CurNest || moved > REFILL_MOVE)
			VB_Fill();
		if (m_VB_SeenType != ed.m_VB_Type)
			VB_FillTypes();
		VB_UpdateStatus(ed);
	}

	protected void VB_FillTypes()
	{
		if (!m_VB_TypeList)
			return;

		m_VB_Filling = true;
		m_VB_TypeList.Clear();
		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		m_VB_SeenType = ed.m_VB_Type;
		string group;
		string icon;
		string hue;
		string type;
		int r;
		for (int i = 0; i < VB_RiftEditor.TYPES.Count(); i++)
		{
			type = VB_RiftEditor.TYPES[i];
			group = "Корни";
			icon = "radar";
			hue = INT_UIF_ThemeKeys.AMBER;
			if (type == "VB_RiftHeart")
			{
				group = "Без сердца гнездо не встанет";
				icon = "target";
				hue = INT_UIF_ThemeKeys.RED;
			}
			else if (type.IndexOf("Hang") >= 0)
			{
				group = "Щупальца: висят в воздухе";
				icon = "chevron_down";
				hue = INT_UIF_ThemeKeys.TEAL;
			}
			r = m_VB_TypeList.AddRow(VB_RiftEditor.NAMES[i], group, i);
			m_VB_TypeList.SetRowIcon(r, icon, hue);
			if (type == ed.m_VB_Type)
				m_VB_TypeList.Select(r, false);
		}
		m_VB_Filling = false;
	}

	protected void VB_Fill()
	{
		if (!m_VB_NestList)
			return;

		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		m_VB_SeenVersion = VB_RiftNestClient.s_VB_Version;
		m_VB_SeenNest = ed.m_VB_CurNest;

		vector ppos = vector.Zero;
		if (g_Game.GetPlayer())
			ppos = g_Game.GetPlayer().GetPosition();
		m_VB_FillPos = ppos;

		// ближние сверху: расстояние до каждого гнезда считается один раз (центры — из VB_Views, без поиска сердца)
		array<ref VB_RiftNestView> views = VB_RiftNestClient.VB_Views();
		array<VB_RiftNestView> sorted = new array<VB_RiftNestView>;
		array<float> dists = new array<float>;
		VB_RiftNest decor;
		foreach (VB_RiftNestView vw : views)
		{
			if (vw.Nest.Id == VB_RiftNestNet.DECOR_ID)
			{
				decor = vw.Nest;
				continue;
			}
			sorted.Insert(vw);
			dists.Insert(VB_RiftNestUtil.VB_DistXZ(vw.Center, ppos));
		}
		int count = sorted.Count();
		int bi;
		float bd;
		for (int i = 0; i < count; i++)
		{
			bi = i;
			bd = dists[i];
			for (int j = i + 1; j < count; j++)
			{
				if (dists[j] < bd)
				{
					bd = dists[j];
					bi = j;
				}
			}
			if (bi != i)
			{
				sorted.SwapItems(i, bi);
				dists.SwapItems(i, bi);
			}
		}

		int pool = 0;
		int live = 0;
		m_VB_Filling = true;
		m_VB_NestList.Clear();
		string sec;
		bool heart;
		bool isLive;
		int r;
		int distM;
		VB_RiftNest nest;

		// постоянный декор — первой строкой, он стоит всегда
		if (decor && decor.Objects.Count() > 0)
		{
			sec = VB_RiftEditor.VB_CountWord(decor.Objects.Count(), "щупальце", "щупальца", "щупалец") + " · стоят на каждом рестарте";
			r = m_VB_NestList.AddRow(decor.Name, sec, decor.Id);
			m_VB_NestList.SetRowIcon(r, "star", INT_UIF_ThemeKeys.TEAL);
			m_VB_NestList.SetRowBadge(r, "ВСЕГДА", INT_UIF_Variant.INFO);
			if (decor.Id == ed.m_VB_CurNest)
				m_VB_NestList.Select(r, false);
		}

		for (int k = 0; k < count; k++)
		{
			nest = sorted[k].Nest;
			heart = sorted[k].HasHeart;
			isLive = VB_RiftNestClient.VB_IsLiveNest(nest.Id);
			if (nest.Enabled == 1 && heart)
				pool++;
			if (isLive)
				live++;

			sec = VB_RiftEditor.VB_CountWord(nest.Objects.Count(), "объект", "объекта", "объектов");
			distM = Math.Round(dists[k]);
			sec += " · " + distM.ToString() + " м";
			r = m_VB_NestList.AddRow(nest.Name, sec, nest.Id);
			if (heart)
				m_VB_NestList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.RED);
			else
				m_VB_NestList.SetRowIcon(r, "warning", INT_UIF_ThemeKeys.AMBER);
			if (!heart)
				m_VB_NestList.SetRowBadge(r, "БЕЗ СЕРДЦА", INT_UIF_Variant.WARNING);
			else if (nest.Enabled == 0)
				m_VB_NestList.SetRowBadge(r, "ВЫКЛ", INT_UIF_Variant.DANGER);
			else if (isLive)
				m_VB_NestList.SetRowBadge(r, "НА КАРТЕ", INT_UIF_Variant.SUCCESS);
			if (nest.Id == ed.m_VB_CurNest)
				m_VB_NestList.Select(r, false);
		}
		m_VB_Filling = false;

		m_VB_TilePool.SetValue(pool.ToString());
		m_VB_TileLive.SetValue(live.ToString());
		// пока число ещё не ушло на сервер (задержка после клика), показываем его, а не старое серверное
		if (m_VB_ActiveStep)
			m_VB_ActiveStep.SetValue(ed.VB_ActiveShown(), false);
		int minDistM = Math.Round(VB_RiftNestClient.s_VB_MinDist);
		string rules = "Встают случайные, не ближе " + minDistM.ToString() + " м друг к другу";
		string keepMin = VB_RiftNestClient.s_VB_KeepMin.ToString();
		string keepMax = VB_RiftNestClient.s_VB_KeepMax.ToString();
		rules += " и не те, что на прошлом рестарте; корней " + keepMin + "–" + keepMax + "%.";
		if (VB_RiftNestClient.s_VB_Regrow > 0)
			rules += " Новое прорастает через " + VB_RiftNestClient.s_VB_Regrow.ToString() + " мин после вырезания.";
		if (m_VB_Rules)
			m_VB_Rules.SetText(rules);

		VB_FillCurrent();
	}

	protected void VB_FillCurrent()
	{
		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		VB_RiftNest nest = VB_RiftNestClient.VB_Find(ed.m_VB_CurNest);
		bool has = nest != null;
		m_VB_BtnUndo.SetEnabled(has);
		m_VB_BtnTp.SetEnabled(has);
		m_VB_BtnToggle.SetEnabled(has);
		m_VB_BtnDelete.SetEnabled(has);
		m_VB_BtnScatter.SetEnabled(has && nest.VB_HasHeart());
		if (!has)
		{
			m_VB_CurTitle.SetText("Гнездо не выбрано");
			m_VB_CurSub.SetText("Поставь сердце: гнездо создастся само");
			return;
		}
		if (nest.Id == VB_RiftNestNet.DECOR_ID)
		{
			// декор: только «убрать последний» и «перенестись к ближайшему»
			m_VB_BtnToggle.SetEnabled(false);
			m_VB_BtnDelete.SetEnabled(false);
			m_VB_BtnToggle.SetText("Выключить из выбора");
			m_VB_CurTitle.SetText(nest.Name);
			m_VB_CurSub.SetText(VB_RiftEditor.VB_CountWord(nest.Objects.Count(), "щупальце", "щупальца", "щупалец") + " · стоят на каждом рестарте");
			return;
		}

		m_VB_CurTitle.SetText(nest.Name);
		string sub = VB_RiftEditor.VB_CountWord(nest.Objects.Count(), "объект", "объекта", "объектов");
		if (!nest.VB_HasHeart())
			sub += " · нет сердца, не встанет";
		if (VB_RiftNestClient.VB_IsLiveNest(nest.Id))
			sub += " · на карте";
		if (nest.Enabled == 0)
			sub += " · выключено";
		m_VB_CurSub.SetText(sub);
		if (nest.Enabled == 0)
			m_VB_BtnToggle.SetText("Включить в выбор");
		else
			m_VB_BtnToggle.SetText("Выключить из выбора");
	}

	protected void VB_UpdateStatus(VB_RiftEditor ed)
	{
		string nestLine;
		int into = ed.VB_NestFor(ed.VB_GhostPos());
		VB_RiftNest nest = VB_RiftNestClient.VB_Find(into);
		if (ed.m_VB_Type == "")
		{
			nestLine = "Рядом гнёзд нет";
			if (nest)
				nestLine = "Рядом: " + nest.Name;
		}
		else if (into == VB_RiftNestNet.DECOR_ID)
		{
			nestLine = "Ставлю в: постоянный декор (стоит всегда)";
		}
		else if (nest)
		{
			nestLine = "Ставлю в: " + nest.Name;
			if (VB_RiftNestClient.VB_IsLiveNest(nest.Id))
				nestLine += " · на карте";
		}
		else if (VB_RiftNestUtil.VB_IsHeartType(ed.m_VB_Type))
		{
			nestLine = "Сердце начнёт новое гнездо";
		}
		else
		{
			nestLine = "Новое гнездо, пока без сердца";
		}

		string placeLine = "Ничего не выбрано: выбери в списке слева";
		if (ed.m_VB_Type != "")
		{
			placeLine = VB_RiftEditor.VB_TypeName(ed.m_VB_Type) + " · " + Math.Round(ed.m_VB_Yaw).ToString() + "°";
			if (!VB_RiftEditor.VB_IsGroundType(ed.m_VB_Type))
				placeLine += " · высота " + ed.m_VB_Height.ToString() + " м";
			if (!ed.VB_GhostOk() && ed.VB_GhostNote() != "")
				placeLine += " · " + ed.VB_GhostNote();
		}

		string targetLine = "";
		VB_RiftNest tNest;
		VB_RiftNestObj t = ed.VB_TargetObj(tNest);
		if (t)
			targetLine = "Под прицелом: " + VB_RiftEditor.VB_TypeName(t.Type) + " (" + tNest.Name + ")";

		string msgLine = "";
		if (g_Game.GetTime() < ed.m_VB_MsgUntil)
			msgLine = ed.m_VB_Msg;

		string modeLine = "Режим: интерфейс, персонаж стоит";
		if (ed.m_VB_Control)
			modeLine = "Режим: управление";

		if (m_VB_StNest)
		{
			m_VB_StMode.SetText(modeLine);
			m_VB_StNest.SetText(nestLine);
			m_VB_StPlace.SetText(placeLine);
			m_VB_StTarget.SetText(targetLine);
			m_VB_StMsg.SetText(msgLine);
		}
	}

	protected void OnVBNestSelected(int row, INT_UIF_List list)
	{
		if (m_VB_Filling || row < 0)
			return;
		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		ed.m_VB_CurNest = list.GetData(row);
		m_VB_SeenNest = ed.m_VB_CurNest;
		VB_FillCurrent();
	}

	// двойной клик по гнезду — перенестись к нему
	protected void OnVBNestActivated(int row, INT_UIF_List list)
	{
		if (row < 0)
			return;
		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		ed.m_VB_CurNest = list.GetData(row);
		ed.VB_Teleport(ed.m_VB_CurNest);
	}

	protected void OnVBTypeSelected(int row, INT_UIF_List list)
	{
		if (m_VB_Filling || row < 0)
			return;
		int i = list.GetData(row);
		if (i >= 0 && i < VB_RiftEditor.TYPES.Count())
		{
			VB_RiftEditor.VB_Get().m_VB_Type = VB_RiftEditor.TYPES[i];
			m_VB_SeenType = VB_RiftEditor.TYPES[i];
		}
	}

	// число гнёзд за рестарт: на сервер (settings.json) через секунду после последнего клика;
	// встанет на следующем рестарте или по «Перебросить»
	protected void OnVBActiveChanged(float value, INT_UIF_Stepper stepper)
	{
		int n = Math.Round(value);
		VB_RiftEditor.VB_Get().VB_QueueActive(n);
	}

	protected void OnVBScatter(INT_UIF_Button b)
	{
		VB_RiftEditor.VB_Get().VB_Scatter();
	}

	protected void OnVBUndo(INT_UIF_Button b)
	{
		VB_RiftEditor.VB_Get().VB_Undo();
	}

	protected void OnVBTeleport(INT_UIF_Button b)
	{
		VB_RiftEditor ed = VB_RiftEditor.VB_Get();
		ed.VB_Teleport(ed.m_VB_CurNest);
	}

	protected void OnVBToggle(INT_UIF_Button b)
	{
		VB_RiftEditor.VB_Get().VB_ToggleNest();
	}

	protected void OnVBDelete(INT_UIF_Button b)
	{
		VB_RiftNest nest = VB_RiftNestClient.VB_Find(VB_RiftEditor.VB_Get().m_VB_CurNest);
		if (!nest || (m_VB_Dialog && !m_VB_Dialog.IsFinished()))
			return;
		string title = "Удалить " + nest.Name + "?";
		string text = "Гнездо пропадёт из файла и с карты. Копия файла на старте сервера — nests_backup.json.";
		INT_UIF_Dialog d = INT_UIF_Dialog.Confirm(title, text, "Удалить", "Отмена", INT_UIF_Variant.DANGER);
		if (d)
		{
			m_VB_Dialog = d;
			d.OnResult().Insert(OnVBDeleteConfirm);
		}
	}

	protected void OnVBDeleteConfirm(int result, INT_UIF_Dialog dlg)
	{
		m_VB_Dialog = null;
		if (result == INT_UIF_Dialog.RESULT_YES)
			VB_RiftEditor.VB_Get().VB_DeleteNest();
	}

	protected void OnVBReroll(INT_UIF_Button b)
	{
		if (m_VB_Dialog && !m_VB_Dialog.IsFinished())
			return;
		string text = "Все гнёзда снимутся с карты, и сервер выберет новые, как на рестарте (не у игроков). Гнёзда с вырезанным сердцем до рестарта не вернутся.";
		INT_UIF_Dialog d = INT_UIF_Dialog.Confirm("Перебросить гнёзда?", text, "Перебросить", "Отмена", INT_UIF_Variant.WARNING);
		if (d)
		{
			m_VB_Dialog = d;
			d.OnResult().Insert(OnVBRerollConfirm);
		}
	}

	protected void OnVBRerollConfirm(int result, INT_UIF_Dialog dlg)
	{
		m_VB_Dialog = null;
		if (result == INT_UIF_Dialog.RESULT_YES)
			VB_RiftEditor.VB_Get().VB_Reroll();
	}

	protected void OnVBQuit(INT_UIF_Button b)
	{
		VB_RiftEditor.VB_Get().VB_Deactivate();
	}
}

// Автотест редактора без мыши (стенд dayz-mcp). Файл $profile:vb_rift_editor_test.txt на КЛИЕНТЕ,
// по команде в строке, # — комментарий. Клиент проверяет файл раз в 5 с, читает и удаляет.
// Команды: open, exit, wait <мс>, type <класс>, yaw <град>, height <м>, place <x> <z>, placeh <x> <y> <z>,
// scatter, undo, toggle, delnest, tp, reroll, del <x> <z>, cur <имя гнезда>, dump.
// Всё пишет в скриптовый лог клиента «[VB_Rift] тест редактора: ...».
// Только в DayZDiag (DIAG_DEVELOPER): в боевом клиенте этого класса нет (аудит 2026-11-13, №28).
#ifdef DIAG_DEVELOPER
class VB_RiftEditorTest
{
	static const string TEST_FILE = "$profile:vb_rift_editor_test.txt";

	protected ref TStringArray m_VB_Lines;
	protected int m_VB_Pos;
	protected int m_VB_WaitUntil;

	static VB_RiftEditorTest VB_TryLoad()
	{
		if (!FileExist(TEST_FILE))
			return null;

		FileHandle fh = OpenFile(TEST_FILE, FileMode.READ);
		if (!fh)
			return null;

		VB_RiftEditorTest test = new VB_RiftEditorTest();
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
		Print("[VB_Rift] тест редактора: " + test.m_VB_Lines.Count().ToString() + " команд");
		return test;
	}

	bool VB_Done()
	{
		return m_VB_Pos >= m_VB_Lines.Count() && g_Game.GetTime() >= m_VB_WaitUntil;
	}

	void VB_Tick(VB_RiftEditor ed)
	{
		if (m_VB_Pos >= m_VB_Lines.Count() || g_Game.GetTime() < m_VB_WaitUntil)
			return;

		string line = m_VB_Lines[m_VB_Pos];
		m_VB_Pos++;
		m_VB_WaitUntil = g_Game.GetTime() + 500;
		Print("[VB_Rift] тест редактора: " + line);

		TStringArray p = new TStringArray;
		line.Split(" ", p);
		string cmd = p[0];
		float x;
		float y;
		float z;
		vector at;
		vector ori;
		if (cmd == "open")
			ed.VB_RequestOpen();
		else if (cmd == "exit")
			ed.VB_Deactivate();
		else if (cmd == "wait" && p.Count() > 1)
			m_VB_WaitUntil = g_Game.GetTime() + p[1].ToInt();
		else if (cmd == "type" && p.Count() > 1)
			ed.m_VB_Type = p[1];
		else if (cmd == "yaw" && p.Count() > 1)
			ed.m_VB_Yaw = p[1].ToFloat();
		else if (cmd == "height" && p.Count() > 1)
			ed.m_VB_Height = p[1].ToFloat();
		else if (cmd == "place" && p.Count() > 2)
		{
			// оба вектора — в локальные: два Vector в одном списке аргументов затирают друг друга (E71)
			x = p[1].ToFloat();
			z = p[2].ToFloat();
			y = g_Game.SurfaceY(x, z);
			at = Vector(x, y, z);
			ori = Vector(ed.m_VB_Yaw, 0, 0);
			ed.VB_SendOne(ed.m_VB_Type, at, ori);
		}
		else if (cmd == "placeh" && p.Count() > 3)
		{
			x = p[1].ToFloat();
			y = p[2].ToFloat();
			z = p[3].ToFloat();
			at = Vector(x, y, z);
			ori = Vector(ed.m_VB_Yaw, 0, 0);
			ed.VB_SendOne(ed.m_VB_Type, at, ori);
		}
		else if (cmd == "scatter")
			ed.VB_Scatter();
		else if (cmd == "undo")
			ed.VB_Undo();
		else if (cmd == "toggle")
			ed.VB_ToggleNest();
		else if (cmd == "delnest")
			ed.VB_DeleteNest();
		else if (cmd == "tp")
			ed.VB_Teleport(ed.m_VB_CurNest);
		else if (cmd == "reroll")
			ed.VB_Reroll();
		else if (cmd == "del" && p.Count() > 2)
			VB_DeleteNear(p[1].ToFloat(), p[2].ToFloat());
		else if (cmd == "cur" && p.Count() > 1)
			VB_SelectByName(ed, line.Substring(4, line.Length() - 4));
		else if (cmd == "dump")
			VB_Dump(ed);
		else
			Print("[VB_Rift] тест редактора: неизвестная команда " + cmd);
	}

	protected void VB_DeleteNear(float x, float z)
	{
		vector at = Vector(x, 0, z);
		int best = 0;
		float bestD = 4.0;
		float d;
		foreach (VB_RiftNest nest : VB_RiftNestClient.s_VB_Nests)
		{
			foreach (VB_RiftNestObj o : nest.Objects)
			{
				d = VB_RiftNestUtil.VB_DistXZ(o.Position, at);
				if (d < bestD)
				{
					bestD = d;
					best = o.Id;
				}
			}
		}
		if (best != 0)
			VB_RiftNestClient.VB_SendOp(VB_RiftNestNet.OP_DEL, best);
		else
			Print("[VB_Rift] тест редактора: рядом нет объекта");
	}

	protected void VB_SelectByName(VB_RiftEditor ed, string name)
	{
		foreach (VB_RiftNest nest : VB_RiftNestClient.s_VB_Nests)
		{
			if (nest.Name == name)
			{
				ed.m_VB_CurNest = nest.Id;
				return;
			}
		}
		Print("[VB_Rift] тест редактора: нет гнезда " + name);
	}

	protected void VB_Dump(VB_RiftEditor ed)
	{
		string cur = "нет";
		VB_RiftNest curNest = VB_RiftNestClient.VB_Find(ed.m_VB_CurNest);
		if (curNest)
			cur = curNest.Name;
		// всё, что подставляется, — сначала в локальные: два Count в одном списке аргументов затирают друг друга (E71)
		int nestCount = VB_RiftNestClient.s_VB_Nests.Count();
		int copies = ed.VB_PreviewCount();
		Print(string.Format("[VB_Rift] тест редактора: DUMP активен=%1 гнёзд=%2 выбрано=%3 копий=%4 тип=%5",
			ed.m_VB_Active, nestCount, cur, copies, ed.m_VB_Type));
		int liveObjs;
		int objCount;
		bool nestLive;
		string center;
		foreach (VB_RiftNest nest : VB_RiftNestClient.s_VB_Nests)
		{
			liveObjs = 0;
			foreach (VB_RiftNestObj o : nest.Objects)
			{
				if (VB_RiftNestClient.s_VB_LiveObjs.Find(o.Id) >= 0)
					liveObjs++;
			}
			objCount = nest.Objects.Count();
			nestLive = VB_RiftNestClient.VB_IsLiveNest(nest.Id);
			center = nest.VB_Center().ToString();
			Print(string.Format("[VB_Rift] тест редактора: DUMP %1 id=%2 объектов=%3 на карте=%4/%5 вкл=%6 центр=%7",
				nest.Name, nest.Id, objCount, nestLive, liveObjs, nest.Enabled, center));
		}
	}
}
#endif
