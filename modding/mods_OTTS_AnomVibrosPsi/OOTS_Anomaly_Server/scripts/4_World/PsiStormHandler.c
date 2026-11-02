class PsiStormHandler
{
	private BlowOutManager 				m_Parent;
	private PsiStormConfig				m_Cfg;
	
	private float 						m_MainTick;
	private float 						m_UncTick;
	private float 						m_CurPhaseTick;
	
	private int 						m_NextStorm;
	private int							m_WarningTime;
	private int 						m_LifeTime;
	private float 						m_UnconsciousTime;
	
	
	private int 						m_Phase;
	
	private int 						m_CurGameHour;
	
	void PsiStormHandler(BlowOutManager parent, PsiStormConfig cfg)
	{
		m_Parent = parent;
		m_Cfg = cfg;
		
		m_Parent.GetParams(m_NextStorm, m_WarningTime, m_LifeTime, m_UnconsciousTime);
		
		WarningPhase();
	}
	
	void ~PsiStormHandler()
	{
		
	}
	
	void SetPhase(int phase)
	{
		switch(phase)
		{
			case BlowOutPhases.COOLDOWN:
			break;
			case BlowOutPhases.WARNING:
				if(m_Phase == BlowOutPhases.COOLDOWN)
					m_MainTick = 0;
			break;
			case BlowOutPhases.ACTIVE:
				if(m_Phase == BlowOutPhases.WARNING)
					m_MainTick = m_WarningTime + 0.02;
			break;
			case BlowOutPhases.END:
				if(m_Phase == BlowOutPhases.ACTIVE)
					m_MainTick = m_WarningTime + m_LifeTime + 0.02;
			break;
		}
	}
	
	int GetPhase()
	{
		return m_Phase;
	}
	
	int GetCurPhaseDuration()
	{
		int phase_time;
		
		switch(m_Phase)
		{
			case BlowOutPhases.WARNING:
				phase_time = m_WarningTime;
			break;
			case BlowOutPhases.ACTIVE:
				phase_time = m_LifeTime;
			break;
			case BlowOutPhases.END:
				phase_time = m_UnconsciousTime;
			break;
		}
		
		return phase_time - m_CurPhaseTick;
	}
	
	void OnUpdate(float delta_time)
	{
		m_CurPhaseTick += delta_time / 60;
		
		if(m_Phase == BlowOutPhases.END)
		{
			m_UncTick += delta_time;
			if(m_UncTick >= m_UnconsciousTime)
			{
				Final();
				return;
			}
			
			return;
		}
		
		m_MainTick += delta_time / 60;
		
		if(m_MainTick >= m_WarningTime + m_LifeTime)
		{
			EndPhase();
			return;
		}
		else if(m_MainTick >= m_WarningTime)
		{
			ActivePhase();
			return;
		}
	}
	
	bool IsActive()
	{
		return false;
	}
	
	private void WarningPhase()
	{
		if(m_Phase == BlowOutPhases.WARNING)
			return;
		
		m_Phase = BlowOutPhases.WARNING;
		
		m_CurPhaseTick = 0;
		
		PrintFormat("[BlowOut][PsiStorm]: Warning - %1 mins, MissionTime - %2", m_MainTick.ToString(), m_Parent.GetMissionTime());
		
		GetGame().GetWeather().SetWindSpeed( 0.95 );								
		GetGame().GetWeather().SetStorm( 0.8, 0.5, 16);
		GetGame().GetWeather().GetOvercast().Set( 1, m_WarningTime*0.55*60, m_WarningTime*60 );
		
		if(m_Parent.HideZonesWhenActive())
		{
			m_Parent.CreateHideZoneTriggers();
		}
		
		NoticePlayers();
	}
	
	private void ActivePhase()
	{
		if(m_Phase == BlowOutPhases.ACTIVE)
			return;
		
		m_Phase = BlowOutPhases.ACTIVE;
		
		m_CurPhaseTick = 0;
		
		float time = m_Parent.GetMainTick() + m_MainTick;
		PrintFormat("[BlowOut][PsiStorm]: Active - %1 mins, MissionTime - %2", m_MainTick.ToString(), m_Parent.GetMissionTime());
		
		GetGame().GetWeather().SetWindSpeed( 1 );	
		GetGame().GetWeather().SetStorm( 1, 0.5, 8);
		GetGame().GetWeather().GetOvercast().Set( 1, 30, m_LifeTime);
		/*int year;
		int month;
		int day;
		int minute;
		GetGame().GetWorld().GetDate(year, month, day, m_CurGameHour, minute);
		GetGame().GetWorld().SetDate(year, month, day, 23, minute);*/
		
		NoticePlayers();
	}
	
	private void EndPhase()
	{
		if(m_Phase == BlowOutPhases.END)
			return;
		
		m_Phase = BlowOutPhases.END;
		
		m_CurPhaseTick = 0;
		
		float time = m_Parent.GetMainTick() + m_MainTick;
		PrintFormat("[BlowOut][PsiStorm]: End - %1 mins, MissionTime - %2", m_MainTick.ToString(), m_Parent.GetMissionTime());
		
		/*int year;
		int month;
		int day;
		int hour;
		int minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		GetGame().GetWorld().SetDate(year, month, day, m_CurGameHour, minute);*/
		
		GetGame().GetWeather().SetWindSpeed( 0.1 );
		GetGame().GetWeather().SetStorm( 0, 0.9, 5);
		GetGame().GetWeather().GetOvercast().Set( 0.1, Math.RandomIntInclusive(240, 480), Math.RandomIntInclusive(240, 360) );
		
		if(m_Parent.HideZonesWhenActive())
		{
			m_Parent.RemoveHideZoneTriggers();
		}
		
		NoticePlayers();
		
		m_MainTick = 0;
	}
	
	private void Final()
	{
		m_Phase = BlowOutPhases.COOLDOWN;
		
		m_CurPhaseTick = 0;
		
		float time = m_Parent.GetMainTick() + m_MainTick + m_UnconsciousTime;
		PrintFormat("[BlowOut][PsiStorm]: Final - %1 mins, MissionTime - %2", m_MainTick.ToString(), m_Parent.GetMissionTime());
		
		NoticePlayers();
		
		m_Parent.OnPsiStormEnd();
	}
	
	private void NoticePlayers()
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		int phase_duration = m_Parent.GetPhaseDuration(m_Phase);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				/* if(m_Parent.HideZonesWhenActive())
				{
					player.SetBlowOutHidden(false);
				} */
				
				if(m_Phase == BlowOutPhases.WARNING)
				{
					player.GetBlowOutHandler().SetDamageParams(m_Cfg.GetDamageTime(), m_Cfg.GetDamageValue());
				}
				else if(m_Phase == BlowOutPhases.END && !player.IsMonolith())
				{
					if(m_UnconsciousTime > 0)
						player.SetHealth("GlobalHealth", "Shock", 0);
				}
				else if(m_Phase == BlowOutPhases.COOLDOWN && !player.IsMonolith())
				{
					player.SetHealth("GlobalHealth", "Shock", 100);
				}
			
				player.GetBlowOutHandler().SetPhase(m_Phase, phase_duration, true);
			}
		}
		
		if(m_Phase != BlowOutPhases.END)
			m_Parent.SendAlertItemsRPC(m_Phase, true);
	}
}