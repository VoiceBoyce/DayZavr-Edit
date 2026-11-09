modded class ActionTeleportToObelisk
{
    override void OnEndServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
        PlayerBase player = action_data.m_Player;

        if (targetObject.IsInherited(MSFZ_Portal))
        {
            player.SetPosition(ObeliskPositions.Obelisk2Position);
        }
        else if (targetObject.IsInherited(MSFZ_Portal_Exit))
        {
            player.SetPosition(ObeliskPositions.Obelisk1Position);
        }

        Param1<string> msgParam = new Param1<string>("#STR_MSFZ_teleportation");
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msgParam, true, player.GetIdentity());
	}
}
