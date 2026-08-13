class DRL_WorkbenchBG
{
    protected static EntityAI s_LastWorkbench;

    static void Set(EntityAI wb)
	{
		s_LastWorkbench = wb;
	}
    static EntityAI Take()
    {
        EntityAI tmp = s_LastWorkbench;
        s_LastWorkbench = null;
        return tmp;
    }
}