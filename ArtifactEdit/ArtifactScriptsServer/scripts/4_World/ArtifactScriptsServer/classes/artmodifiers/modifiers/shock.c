class ArtModifierShock: ArtModifierBase
{
	override void Process()
	{
		float shock = m_Player.GetHealth("GlobalHealth", "Shock");

		shock += m_Value * GetRelativeValue();
		
		m_Player.SetHealth("GlobalHealth", "Shock", Math.Clamp(shock, 0, 100));
	}
}