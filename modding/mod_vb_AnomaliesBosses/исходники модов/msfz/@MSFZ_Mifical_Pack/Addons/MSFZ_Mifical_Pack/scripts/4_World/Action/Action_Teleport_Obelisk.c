class ActionTeleportToObelisk: ActionInteractBase
{
    void ActionTeleportToObelisk()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text = "#STR_teleport_to_obelisk";
    }

	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor();
	}

    // Действие доступно только у портала, у которого есть парный портал
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        MSFZ_Portal_Base portal = MSFZ_Portal_Base.Cast(target.GetObject());
        if (!portal)
            return false;

        return portal.MSFZ_GetDestination(player) != vector.Zero;
	}
}
