class ArtModifierBlood: ArtModifierBase
{
	override void Process()
	{
		float blood = m_Player.GetHealth("GlobalHealth", "Blood");

		blood += m_Value * GetRelativeValue();
		
		m_Player.SetHealth("GlobalHealth", "Blood", Math.Clamp(blood, 0, 5000));
	}
}