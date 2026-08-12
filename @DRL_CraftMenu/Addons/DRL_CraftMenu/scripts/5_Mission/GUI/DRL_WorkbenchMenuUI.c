class DRL_WorkbenchMenuUI : DRL_CraftMenuUI
{
    protected EntityAI m_WB;
    protected string   m_WBCls;

    void DRL_WorkbenchMenuUI(EntityAI workbench)
    {
        m_WB = workbench;
        if (m_WB)
        {
            m_WBCls = m_WB.GetType();
            m_WBCls.Trim();
			m_ContextWorkbench = m_WB;
			
			PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
			if (pb)
			{
				pb.DRL_SetCurrentWorkbench(m_WB);
			}
        }
    }

	override Widget Init()
	{
		Widget root = super.Init();
		UpdateWBName();
		return root;
	}

    override bool RecipeAllowedByContext(DRL_Craft_Recipe r)
    {
        if (!r) return false;
        if (!r.WorkbenchClasses) return false;
        for (int i = 0; i < r.WorkbenchClasses.Count(); i++)
        {
            string w = r.WorkbenchClasses[i];
            if (w == m_WBCls) return true;
        }
        return false;
    }
}