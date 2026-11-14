class ArtModifierFood: ArtModifierBase
{
	override void Process()
	{
		float food = m_Player.GetStatEnergy().Get();

		float add_value = m_Value * GetRelativeValue();
		
		if((add_value < 0 && food == 0) || (add_value > 0 && food == PlayerConstants.SL_ENERGY_MAX))
			return;
		
		m_Player.GetStatEnergy().Add( m_Value * GetRelativeValue() );
	}
}