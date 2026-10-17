// INT_UIFramework - lifecycle. Init on client mission start, Shutdown on
// mission finish (top-level widgets must be Unlinked there or they stack up as
// ghosts across sessions - dayz-ui-development DELIVERY CHECKLIST).
class INT_UIF_Runtime
{
	protected static bool s_Inited;

	static void Init()
	{
		if (s_Inited)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		s_Inited = true;
		INT_UIF_MenuRegistry.Register(INT_UIF_MENU_DIALOG, INT_UIF_DialogMenu);
		INT_UIF_Assets.EnsureLoaded();
		INT_UIF_ThemeManager.Get();
		INT_UIF_Log.Info("ready v" + INT_UIF_VERSION);
	}

	static void Shutdown()
	{
		if (!s_Inited)
			return;
		s_Inited = false;
		INT_UIF_ToastManager tm = INT_UIF_ToastManager.Peek();
		if (tm)
			tm.Clear();
		INT_UIF_Tooltip.Shutdown();
		INT_UIF_DialogMenu.s_Pending = null;
		INT_UIF_Animator anim = INT_UIF_Animator.Peek();
		if (anim)
			anim.Shutdown();
		INT_UIF_Menu.ResetFreeze();
		INT_UIF_Log.Info("shutdown");
	}

	static bool IsReady()
	{
		return s_Inited;
	}
}
