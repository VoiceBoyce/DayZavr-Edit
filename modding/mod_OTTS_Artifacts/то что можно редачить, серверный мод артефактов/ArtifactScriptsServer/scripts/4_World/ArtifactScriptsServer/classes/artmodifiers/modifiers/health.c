class ArtModifierHealth: ArtModifierBase
{
	override void Process()
	{
		float health = m_Player.GetHealth("GlobalHealth", "Health");

		health += m_Value * GetRelativeValue();
		
		m_Player.SetHealth("GlobalHealth", "Health", Math.Clamp(health, 0, 100));
	}
}