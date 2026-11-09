modded class ActionTeleportToObelisk
{
    override void OnEndServer(ActionData action_data)
	{
        MSFZ_Portal_Base portal = MSFZ_Portal_Base.Cast(action_data.m_Target.GetObject());
        PlayerBase player = action_data.m_Player;
        if (!portal || !player)
            return;

        vector destination = portal.MSFZ_GetDestination(player);
        if (destination == vector.Zero)
            return;

        // запомнить вход, чтобы выход вернул игрока туда же
        if (!portal.MSFZ_IsExit() && player.GetIdentity())
            MSFZ_PortalRegistry.s_ReturnPoints.Set(player.GetIdentity().GetPlainId(), portal.GetPosition());

        player.SetPosition(destination);

        Param1<string> msgParam = new Param1<string>("#STR_MSFZ_teleportation");
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msgParam, true, player.GetIdentity());
	}
}
