static const int DRL_CRAFTMENU_ID = 9501;
static const int DRL_WORKBENCHMENU_ID = 9502;
static const int DRL_LEARNINGMENU_ID = 9503;

modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();

		GetRPCManager().AddRPC("DRL_Craft_Config", "SERVER_CraftConfigResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("DRL_Craft_Config", "SERVER_LearningConfigResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("DRL_Craft_Config", "SERVER_CraftConfigReloadNotice", this, SingleplayerExecutionType.Client);

		if (DRL_CraftProcessBridge.s_OnNotification)
		{
			DRL_CraftProcessBridge.s_OnNotification.Insert(this.DRL_OnCraftNotificationReceived);
		}
	}

	void DRL_OnCraftNotificationReceived(string title, string text, string itemClass, string iconPath)
	{
		DRL_CraftNotificationManager.Show(title, text, itemClass, iconPath);
	}

	void SERVER_CraftConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Client)
		{
			Param1<DRL_Craft_Config> data;
			if (!ctx.Read(data))
			{
				Print("[DRL_CraftMenu][CLIENT] ERROR: Failed to read CraftConfig from server!");
				return;
			}

			g_Craft_Config = data.param1;
			int recCount = 0;
			int catCount = 0;
			if (g_Craft_Config)
			{
				if (g_Craft_Config.CraftItems) recCount = g_Craft_Config.CraftItems.Count();
				if (g_Craft_Config.Categories) catCount = g_Craft_Config.Categories.Count();
			}
			Print("[DRL_CraftMenu][CLIENT] Config received from server. Recipes=" + recCount.ToString() + ", Categories=" + catCount.ToString());
		}
	}

	void SERVER_LearningConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Client)
		{
			Param1<DRL_Learning_Config> data;
			if (!ctx.Read(data))
			{
				Print("[DRL_CraftMenu][CLIENT] ERROR: Failed to read LearningConfig from server!");
				return;
			}

			g_LearningConfig = data.param1;
			int rCount = 0;
			if (g_LearningConfig && g_LearningConfig.LearningRecipes)
			{
				rCount = g_LearningConfig.LearningRecipes.Count();
			}
			Print("[DRL_CraftMenu][CLIENT] LearningConfig received from server. LearningRecipes=" + rCount.ToString());
		}
	}

	void SERVER_CraftConfigReloadNotice(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
		{
			return;
		}

		Param1<string> p;
		if (!ctx.Read(p))
		{
			return;
		}

		string msg = p.param1;
		if (msg == "")
		{
			msg = "#STR_DRL_RELOAD_CRAFT_CONFIG";
		}

		if (DRL_NOTIF_USE_TOASTS)
			INT_UIF_Toast.Show(msg, INT_UIF_ToastKind.INFO, 5.0);
		else
			NotificationSystem.AddNotificationExtended(5.0, "DRL Craft", msg, "");
		GetGame().ChatPlayer(msg);
	}

	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = null;

		switch (id)
		{
			// Все окна мода — на INT_UIFramework (старые DRL_CraftMenuUI,
			// DRL_WorkbenchMenuUI и DRL_LearningMenuUI удалены).
			case DRL_UI_MENU_CRAFT:
			{
				DRL_CraftWindow craft = new DRL_CraftWindow();
				craft.SetID(id);
				menu = craft;
				break;
			}

			case DRL_UI_MENU_WORKBENCH:
			{
				// Верстак кладёт ActionOpenDRLWorkbench; контекст задаётся до Init.
				DRL_CraftWindow bench = new DRL_CraftWindow();
				bench.SetWorkbench(DRL_WorkbenchBG.Take());
				bench.SetID(id);
				menu = bench;
				break;
			}

			case DRL_UI_MENU_LEARNING:
			{
				DRL_LearnWindow learn = new DRL_LearnWindow();
				learn.SetID(id);
				menu = learn;
				break;
			}
		}

		if (!menu)
		{
			menu = super.CreateScriptedMenu(id);
		}

		return menu;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		// Тестовый хук нового интерфейса (маркер drl_ui_test.txt в профиле клиента).
		DRL_UITest.Tick(timeslice);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		Input gameInput = Input.Cast(GetGame().GetInput());
		if (!player || !gameInput)
		{
			return;
		}

		UIManager uim = GetGame().GetUIManager();
		if (uim && (uim.GetMenu() != null || uim.IsCursorVisible()))
		{
			return;
		}

		if (g_Craft_Config && g_Craft_Config.IsFreeMenuEnabled())
		{
			if (gameInput.LocalPress("UADRL_OpenCraftMenu"))
			{
				GetGame().GetUIManager().EnterScriptedMenu(DRL_CRAFTMENU_ID, null);
			}
		}

		UAInput reloadInput = GetUApi().GetInputByName("UADRL_ReloadCraftConfig");
		if (reloadInput && reloadInput.LocalPress())
		{
			if (GetGame().GetUIManager().GetMenu() == null)
			{
				GetRPCManager().SendRPC("DRL_Craft_Config", "CLIENT_RequestReloadCraftConfig", null, true, null);
			}
		}
	}

	override void OnMissionStart()
	{
		GetRPCManager().SendRPC("DRL_Craft_Config", "CLIENT_RequestCraftConfig", null, true);
		super.OnMissionStart();
	}
}