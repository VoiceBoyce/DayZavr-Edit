static const int DRL_CRAFTMENU_ID = 9501;
static const int DRL_WORKBENCHMENU_ID = 9502;

modded class MissionGameplay
{
	EntityAI m_DRL_LastWorkbenchRef;
	
	override void OnInit()
	{
		super.OnInit();
		
		GetRPCManager().AddRPC("DRL_Craft_Config", "SERVER_CraftConfigResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("DRL_Craft_Config", "SERVER_CraftConfigReloadNotice", this, SingleplayerExecutionType.Client);
	}

	void SERVER_CraftConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Print("[DRL_CraftMenu][INFO] Получен конфиг с сервера");
	
		if (type == CallType.Client)
		{
			Param1<DRL_Craft_Config> data;
			if (!ctx.Read(data)) return;
	
			g_Craft_Config = new DRL_Craft_Config();
			g_Craft_Config = data.param1;
			
			Print("[DRL_CraftMenu][CLIENT] Recipes=" + g_Craft_Config.CraftItems.Count() + " Categories=" + g_Craft_Config.Categories.Count());
		}
	}
	
	void SERVER_CraftConfigReloadNotice(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
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
			msg = "DRL Craft: конфиг обновлён";
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
				menu = new DRL_CraftMenuUI();
				break;
	
			case 9502:
			{
				EntityAI bench = DRL_WorkbenchBG.Take();
				
				menu = new DRL_WorkbenchMenuUI(bench);
				break;
			}
		}
	
		if (!menu) menu = super.CreateScriptedMenu(id);
		return menu;
	}
	
	override void OnKeyPress(int key)
	{
		UIScriptedMenu m = GetGame().GetUIManager().GetMenu();

		if (m && m.IsInherited(DRL_CraftMenuUI) && key == KeyCode.KC_ESCAPE)
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
            return;
		
        UIManager uim = GetGame().GetUIManager();
        if (uim && (uim.GetMenu() != null || uim.IsCursorVisible()))
            return;
		
        if (!g_Craft_Config || !g_Craft_Config.IsFreeMenuEnabled())
            return;
		
        if (gameInput.LocalPress("UADRL_OpenCraftMenu"))
        {
            GetGame().GetUIManager().EnterScriptedMenu(DRL_CRAFTMENU_ID, null);
        }
		
		if (GetUApi().GetInputByName("UADRL_ReloadCraftConfig").LocalPress())
		{
			if (GetGame().GetUIManager().GetMenu() == NULL)
			{
				GetRPCManager().SendRPC("DRL_Craft_Config", "CLIENT_RequestReloadCraftConfig", NULL, true, NULL);
			}
		}
    }

	override void OnMissionStart()
	{
		GetRPCManager().SendRPC("DRL_Craft_Config", "CLIENT_RequestCraftConfig", null, true);
        super.OnMissionStart();
	}
}