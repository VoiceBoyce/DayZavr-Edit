class ArtModifierWater: ArtModifierBase
{
	override void Process()
	{
		float water = m_Player.GetStatWater().Get();

		float add_value = m_Value * GetRelativeValue();
		
		if((add_value < 0 && water == 0) || (add_value > 0 && water == PlayerConstants.SL_WATER_MAX))
			return;
		
		m_Player.GetStatWater().Add( add_value );
	}
}