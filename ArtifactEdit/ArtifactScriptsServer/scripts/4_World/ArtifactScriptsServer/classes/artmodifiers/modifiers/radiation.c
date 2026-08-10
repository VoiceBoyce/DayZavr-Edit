class ArtModifierRadiation: ArtModifierBase
{
	override void Init(PlayerBase player, ArtModifiersManager manager)
	{
		super.Init(player, manager);
		
		m_Time = 5;
	}
	
	override void Process()
	{
		//Скар
		/* int val = m_Value * GetRelativeValue();
		m_Player.SetRadIB(val); */
		
		//Закат, алпин, мельников
		/* int val = m_Value * GetRelativeValue();
		m_Player.SetInrRad(val); */
		
		//Ultima
		/* int val = m_Value * GetRelativeValue();
		m_Player.SetRoentgen(val); */
	}
}