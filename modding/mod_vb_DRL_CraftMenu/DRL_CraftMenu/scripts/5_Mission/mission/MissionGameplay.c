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

		NotificationSystem.AddNotificationExtended(5.0, "DRL Craft", msg, "");
		GetGame().ChatPlayer(msg);
	}

	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = null;

		switch (id)
		{
			case 9501:
			{
				menu = new DRL_CraftMenuUI();
				break;
			}

			case 9502:
			{
				EntityAI bench = DRL_WorkbenchBG.Take();
				menu = new DRL_WorkbenchMenuUI(bench);
				break;
			}

			case 9503:
			{
				menu = new DRL_LearningMenuUI();
				break;
			}
		}

		if (!menu)
		{
			menu = super.CreateScriptedMenu(id);
		}

		return menu;
	}

	override void OnKeyPress(int key)
	{
		UIScriptedMenu m = GetGame().GetUIManager().GetMenu();

		if (m && (m.IsInherited(DRL_CraftMenuUI) || m.IsInherited(DRL_LearningMenuUI)) && key == KeyCode.KC_ESCAPE)
		{
			GetGame().GetUIManager().Back();
			return;
		}

		super.OnKeyPress(key);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

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