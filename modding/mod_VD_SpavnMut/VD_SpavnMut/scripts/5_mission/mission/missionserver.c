//	АБВ
modded class MissionServer
{
	private	ref	VD_MissionServer	m_VD_MissionServer	=	new VD_MissionServer();

	override void OnUpdate( float timeslice )
	{
		super.OnUpdate( timeslice );
		
		if ( m_VD_MissionServer )
		{
			m_VD_MissionServer.OnUpdate( timeslice );
		}
	}
}