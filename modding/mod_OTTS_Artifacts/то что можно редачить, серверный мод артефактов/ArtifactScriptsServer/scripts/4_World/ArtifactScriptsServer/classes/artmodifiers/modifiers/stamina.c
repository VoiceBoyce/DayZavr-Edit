class ArtModifierStamina: ArtModifierBase
{
	override void Process()
	{
		float stamina = m_Player.GetStaminaHandler().GetStamina();
		
		stamina += stamina * m_Value * GetRelativeValue();
		
		m_Player.GetStaminaHandler().SetStamina(stamina);
		
	}
}