// INT AutoRun - public facade for other mods (re-skin, move, react).
//   bool run = INT_AR_API.IsRunning();
//   INT_AR_API.Get().m_OnRun.Insert(OnArRun);   // void OnArRun(bool running)
//   INT_AR_API.Get().m_OnSpeed.Insert(OnArSpeed); // void OnArSpeed(bool sprint)
//   INT_AR_API.RequestHudPos(750, 905);          // design px, applied next tick
class INT_AR_API extends Managed
{
	ref ScriptInvoker m_OnRun;
	ref ScriptInvoker m_OnSpeed;

	protected static ref INT_AR_API s_Instance;
	protected static bool s_Running;
	protected static bool s_Sprint = true;
	protected static Widget s_HudRoot;
	protected static float s_HudX = INT_AR_Const.HUD_X;
	protected static float s_HudY = INT_AR_Const.HUD_Y;
	protected static bool s_HudDirty;

	void INT_AR_API()
	{
		m_OnRun = new ScriptInvoker();
		m_OnSpeed = new ScriptInvoker();
	}

	static INT_AR_API Get()
	{
		if (!s_Instance)
			s_Instance = new INT_AR_API();
		return s_Instance;
	}

	static bool IsRunning()
	{
		return s_Running;
	}

	static bool IsSprint()
	{
		return s_Sprint;
	}

	static Widget GetHudRoot()
	{
		return s_HudRoot;
	}

	static void RequestHudPos(float x, float y)
	{
		s_HudX = x;
		s_HudY = y;
		s_HudDirty = true;
	}

	static void NotifyRun(bool running)
	{
		s_Running = running;
		INT_AR_API api = Get();
		if (api && api.m_OnRun)
			api.m_OnRun.Invoke(running);
	}

	static void NotifySpeed(bool sprint)
	{
		s_Sprint = sprint;
		INT_AR_API api = Get();
		if (api && api.m_OnSpeed)
			api.m_OnSpeed.Invoke(sprint);
	}

	static void BindHud(Widget root)
	{
		s_HudRoot = root;
	}

	static bool ApplyHudPos()
	{
		if (!s_HudDirty)
			return false;
		if (!s_HudRoot)
			return false;
		s_HudDirty = false;
		s_HudRoot.SetPos(INT_UIF_Scale.Px(s_HudX), INT_UIF_Scale.Px(s_HudY));
		return true;
	}
}
