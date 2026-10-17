// INT_UIFramework - client mission hooks: lifecycle, menu registry, demo key.
modded class MissionGameplay
{
	protected UAInput m_IUF_DemoInput;
	protected bool m_IUF_InputResolved;

	override void OnInit()
	{
		super.OnInit();
		INT_UIF_Runtime.Init();
		INT_UIF_MenuRegistry.Register(INT_UIF_MENU_DEMO, INT_UIF_DemoMenu);
	}

	// Registered framework/consumer menus are created here; everything else
	// stays with vanilla and with other mods earlier in the chain.
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = super.CreateScriptedMenu(id);
		if (!menu)
			menu = INT_UIF_MenuRegistry.Create(id);
		return menu;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		// GetInputByName returns null when inputs.xml did not register the
		// action; resolve once and guard (dayz-ui-development plan §7).
		if (!m_IUF_InputResolved)
		{
			m_IUF_InputResolved = true;
			m_IUF_DemoInput = GetUApi().GetInputByName("UAIntUifDemo");
		}
		if (m_IUF_DemoInput && m_IUF_DemoInput.LocalPress())
		{
			if (!GetGame().GetUIManager().GetMenu())
				INT_UIF_Menu.Open(INT_UIF_MENU_DEMO);
		}

		// Opt-in in-game self-test (marker file in the client profile).
		INT_UIF_SelfTest.RunIfRequested();
	}

	override void OnMissionFinish()
	{
		INT_UIF_Runtime.Shutdown();
		super.OnMissionFinish();
	}
}
