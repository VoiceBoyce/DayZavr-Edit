class ActionOpenDRLWorkbench : ActionInteractBase
{
    void ActionOpenDRLWorkbench()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
        m_Text = "#STR_Verstak_StartWork";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!player) return false;
        if (!target) return false;
        Object obj = target.GetObject();
        if (!obj) return false;
		
        if (obj.IsKindOf("DRL_Craft_WB"))
            return true;
		
        if (obj.IsKindOf("OH_workbench"))
            return true;
		
        if (obj.IsKindOf("SRS_Toolsbench"))
            return true;
		
        if (obj.IsKindOf("SRS_Toolsbench_Red"))
            return true;
		
        if (obj.IsKindOf("SRS_Toolsbench_Blue"))
            return true;
		
        return false;
    }

    override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
	
		EntityAI bench = EntityAI.Cast(action_data.m_Target.GetObject());
		if (bench)
		{
			DRL_WorkbenchBG.Set(bench);
			
			GetGame().GetUIManager().EnterScriptedMenu(9502, null);
		}
	}

    override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
	
		PlayerBase pb = PlayerBase.Cast(action_data.m_Player);
		if (!pb) return;
	
		EntityAI bench = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!bench) return;
	
		pb.DRL_SetCurrentWorkbench(bench);
	}
}