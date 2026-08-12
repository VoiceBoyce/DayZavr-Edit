modded class DayZPlayerImplement
{
    override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);

        PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
        if (!pb)
            return;
		
        UIManager uim = GetGame().GetUIManager();
        if (uim && uim.GetMenu())
            return;
		
        if (!g_Craft_Config)
            return;
        if (!g_Craft_Config.IsFreeMenuEnabled())
            return;
		
        UAInput inOpen = GetUApi().GetInputByName("UADRL_OpenCraftMenu");
        if (!inOpen)
            return;
		
        if (inOpen.LocalClick())
        {
            int FREE_MENU_ID = 9501;

            uim.EnterScriptedMenu(FREE_MENU_ID, null);
        }
    }
}