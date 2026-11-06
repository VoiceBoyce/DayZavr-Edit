//	АБВ

class VD_MissionServer
{
	private	ref	VD_RespawnedAnimals		m_VD_RespawnedAnimals	=	new VD_RespawnedAnimals();
	private		int							m_I						=	0;
	private		int							m_TimeStamp				=	GetGame().GetTime();
	private		int							m_Seconds				=	0;
	//координаты|радиус|класснейм|время респавна
	

	void OnUpdate( float timeslice )
	{
		if ( ( GetGame().GetTime() - m_TimeStamp ) < 2000 )
		{
			return;
		}
		
		m_Seconds++;		
		m_TimeStamp	=	GetGame().GetTime();
		if ( m_VD_RespawnedAnimals )
		{
			m_VD_RespawnedAnimals.OnUpdate();
		}
	}
	void PrintWithTime(string message)
	{
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		int second = (GetGame().GetTime() / 1000) % 60;  // Переводим миллисекунды в секунды
	
		Print( string.Format( "[%1:%2:%3] %4", hour, minute, second, message ) );
	}
}