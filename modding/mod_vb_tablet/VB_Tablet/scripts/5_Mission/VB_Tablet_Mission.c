// Клавиши планшета. Все три переназначаются в настройках управления игры.
modded class MissionGameplay
{
	protected UAInput m_VBT_Open;
	protected UAInput m_VBT_Map;
	protected UAInput m_VBT_3D;
	protected bool m_VBT_Resolved;

	override void OnInit()
	{
		super.OnInit();
		INT_UIF_MenuRegistry.Register(VB_TABLET_MENU_ID, VB_TabletMenu);
	}

	override void OnUpdate(float timeslice)
	{
		if (!m_VBT_Resolved)
		{
			m_VBT_Resolved = true;
			m_VBT_Open = GetUApi().GetInputByName("UAVBTabletOpen");
			m_VBT_Map = GetUApi().GetInputByName("UAVBTabletMap");
			m_VBT_3D = GetUApi().GetInputByName("UAVBTablet3D");
		}
		bool openKey = m_VBT_Open && m_VBT_Open.LocalPress();
		bool mapKey = m_VBT_Map && m_VBT_Map.LocalPress();
		// Стрелка вниз в ванили ещё и дальность голоса: нажатие планшета голос не меняет.
		if (openKey)
			GetUApi().GetInputByID(UAVoiceDistanceDown).Supress();

		super.OnUpdate(timeslice);

		if (openKey)
			VB_TabletMenu.Toggle("");
		else if (mapKey)
			VB_TabletMenu.Toggle("map");
		if (m_VBT_3D && m_VBT_3D.LocalPress())
			VBT_Toggle3D();
		VB_TabletMarks3D.Get().Update(timeslice);
	}

	protected void VBT_Toggle3D()
	{
		Widget focus = GetFocus();
		if (focus && (EditBoxWidget.Cast(focus) || MultilineEditBoxWidget.Cast(focus)))
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		client.Hide3D = !client.Hide3D;
		if (client.Hide3D)
			INT_UIF_Toast.Info("#STR_VB_TAB_3D_OFF");
		else
			INT_UIF_Toast.Info("#STR_VB_TAB_3D_ON");
	}

	// M в ванили открывает карту при настройке MapIgnoreMapOwnership: вместо неё откроется планшет.
	override protected void HandleMapToggleByKeyboardShortcut(Man player)
	{
		if (m_VBT_Map && m_VBT_Map.LocalPress())
			return;
		super.HandleMapToggleByKeyboardShortcut(player);
	}

#ifdef DIAG_DEVELOPER
	// Только DayZDiag: dayz-mcp не умеет нажимать UAInput, поэтому стенд открывает планшет через F7 и F8.
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		if (key == KeyCode.KC_F7)
			VB_TabletMenu.Toggle("");
		else if (key == KeyCode.KC_F8)
			VB_TabletMenu.Toggle("map");
		else if (key == KeyCode.KC_F9)
			VB_TabletAtmPlacer.DiagPlace();
	}
#endif

	override void OnMissionFinish()
	{
		VB_TabletMarks3D.Reset();
		VB_TabletClient.Reset();
		super.OnMissionFinish();
	}
}
