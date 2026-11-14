// INT Quests (client) — window/HUD entry point: registers the notebook windows with the framework's menu registry,
// opens them for the 4_World code (the «Поговорить» action, the journal key) and ticks the tracker and the banners.

class INT_QST_UiBridgeImpl : INT_QST_UiBridge
{
	override void OpenGiver(string key)
	{
		INT_QST_GiverMenu.s_Key = key;
		INT_UIF_Menu.Open(INT_QST_Const.MENU_GIVER);
	}

	override void OpenJournal()
	{
		INT_UIF_Menu.Open(INT_QST_Const.MENU_JOURNAL);
	}
}

class INT_QST_Ui
{
	static void Init()
	{
		INT_QST_UiBridge.s_Ui = new INT_QST_UiBridgeImpl();
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_JOURNAL, INT_QST_Journal);
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_GIVER, INT_QST_GiverMenu);
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_MOVER, INT_QST_MoverMenu);
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_BOARD, INT_QST_Board);
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_GIVERS, INT_QST_GiversMenu);
		INT_UIF_MenuRegistry.Register(INT_QST_Const.MENU_EXCHANGE, INT_QST_ExchangeMenu);
		INT_QST_MapLayer.Init();
	}

	static void Tick(float dt)
	{
		INT_QST_MoverMenu.Poll();
		INT_QST_Hud.Get().Tick(dt);
		INT_QST_Banner.Get().Tick(dt);
		// SIGNS-OFF (owner 2026-10-31): signs above the givers, see INT_QST_Markers.c
		// INT_QST_Markers.Get().Tick(dt);
		INT_QST_MapLayer.Tick(dt);
	}

	static void Shutdown()
	{
		INT_QST_Hud.Shutdown();
		INT_QST_Banner.Shutdown();
		// SIGNS-OFF (owner 2026-10-31)
		// INT_QST_Markers.Shutdown();
		INT_QST_MapLayer.Shutdown();
	}
}
