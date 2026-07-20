modded class LoginQueueBase extends LoginScreenBase
{
	override Widget Init()
	{	
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Loadingscreen/gui/ftm_dialog_queue_position.layout");
		
		m_txtPosition = TextWidget.Cast( layoutRoot.FindAnyWidget("txtPosition") );
		m_btnLeave = ButtonWidget.Cast( layoutRoot.FindAnyWidget("btnLeave") );
		return layoutRoot;
	}
};

modded class LoginTimeBase extends LoginScreenBase
{
	override Widget Init()
	{	
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Loadingscreen/gui/ftm_dialog_login_time.layout");
		
		m_txtDescription = TextWidget.Cast( layoutRoot.FindAnyWidget("txtDescription") );
		m_txtLabel = TextWidget.Cast( layoutRoot.FindAnyWidget("txtLabel") );
		m_btnLeave = ButtonWidget.Cast( layoutRoot.FindAnyWidget("btnLeave") );

		return layoutRoot;
	}
};

modded class LoadingScreen
{
	void LoadingScreen(DayZGame game)
	{
		
		m_DayZGame = game;
		
		m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Loadingscreen/gui/ftm_loading.layout");
		Class.CastTo(m_ImageLogoMid, m_WidgetRoot.FindAnyWidget("ImageLogoMid"));
		Class.CastTo(m_ImageLogoCorner, m_WidgetRoot.FindAnyWidget("ImageLogoCorner"));
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		Class.CastTo(m_TextWidgetStatus, m_WidgetRoot.FindAnyWidget("StatusText"));
		Class.CastTo(m_ImageWidgetBackground, m_WidgetRoot.FindAnyWidget("ImageBackground"));
		Class.CastTo(m_ImageLoadingIcon, m_WidgetRoot.FindAnyWidget("ImageLoadingIcon"));
		Class.CastTo(m_ModdedWarning, m_WidgetRoot.FindAnyWidget("ModdedWarning"));
		
		m_ImageBackground = ImageWidget.Cast( m_WidgetRoot.FindAnyWidget("ImageBackground") );
		m_ProgressLoading = ProgressBarWidget.Cast( m_WidgetRoot.FindAnyWidget("LoadingBar") );
		
		string tmp;
		m_ProgressText = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("ProgressText"));
		if (GetGame())
		{
			m_ProgressText.Show( GetGame().CommandlineGetParam("loadingTest", tmp) );
		}
		
		
		//m_ModdedWarning.Show( g_Game.ReportModded() );
		// m_ImageLogoMid.Show(true);
		// m_ImageLogoCorner.Show(false);

		m_ImageLoadingIconRotation += 15;
		// m_ImageLoadingIcon.SetRotation(0, 0, m_ImageLoadingIconRotation);
		
		// m_ImageLoadingIcon.Show(true);
		//m_ProgressText.Show(true);
		
		//m_ProgressBar = m_ProgressBarLarge;
		//m_ProgressBar.SetCurrent(0);
		//m_FrameWidgetSmall.Show( false );
		//m_ImageWidgetBackground.LoadImageFile( 0, GetRandomLoadingBackground() );
		if (m_ImageWidgetBackground)
			m_ImageWidgetBackground.Show( true );		
		m_Counter = 0;
				
		// lighten up your desktop
		game.GetBacklit().LoadingAnim();
		
		ProgressAsync.SetProgressData(m_ProgressLoading);
		ProgressAsync.SetUserData(m_ImageBackground);
	}
	
	override void Show()
	{
		Widget lIcon = m_ImageBackground;
		Widget pText = m_ProgressLoading;
		//m_ProgressText.SetText("");
		m_ProgressLoading.SetCurrent( 0.0 );
		if (m_ImageBackground)
			m_ImageBackground.SetMaskProgress( 0.0 );
		
		if ( !m_WidgetRoot.IsVisible() )
		{
			if ( m_DayZGame.GetUIManager().IsDialogVisible() )
			{
				m_DayZGame.GetUIManager().HideDialog();
			}
			
			if ( m_DayZGame.GetMissionState() == DayZGame.MISSION_STATE_MAINMENU )
			{
				//m_ProgressBar = m_ProgressBarSmall;
				if (m_ImageLogoMid)
					m_ImageLogoMid.Show(false);
				if (m_ImageLogoCorner)
					m_ImageLogoCorner.Show(false);				
				//m_FrameWidgetSmall.Show( true );
				//m_FrameWidgetLarge.Show( false );
				if (m_ImageWidgetBackground)
					m_ImageWidgetBackground.Show( true );
				if (m_TextWidgetStatus)				
					m_TextWidgetStatus.Show(true);
				//lIcon = null;
				//pText = null;
			}
			else
			{
				//m_ProgressBar = m_ProgressBarLarge;
				if (m_ImageLogoMid)
					m_ImageLogoMid.Show(true);
				if (m_ImageLogoCorner)
					m_ImageLogoCorner.Show(false);
				//m_FrameWidgetSmall.Show( false );
				//m_FrameWidgetLarge.Show( true );
				if (m_ImageWidgetBackground)
					m_ImageWidgetBackground.Show( true );
				if (m_TextWidgetStatus)
					m_TextWidgetStatus.Show(false);
			}
			
			m_WidgetRoot.Show( true );
			//m_ProgressBar.SetCurrent( 0 );
			if (m_TextWidgetTitle)
				m_TextWidgetTitle.SetText( "" );
			if (m_TextWidgetStatus)
				m_TextWidgetStatus.SetText( "" );
		}
		
		ProgressAsync.SetProgressData(pText);
		ProgressAsync.SetUserData(lIcon);
	}
	
	override void SetTitle( string title )
	{
		if (m_TextWidgetTitle)
			m_TextWidgetTitle.SetText( title );
	}
};
