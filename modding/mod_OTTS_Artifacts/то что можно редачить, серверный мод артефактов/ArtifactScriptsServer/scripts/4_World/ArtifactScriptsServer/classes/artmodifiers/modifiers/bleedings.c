class ArtModifierBleeds: ArtModifierBase
{
	override void Init(PlayerBase player, ArtModifiersManager manager)
	{
		super.Init(player, manager);
		
		m_Time = 10;
	}
	
	override void Process()
	{
		float chanced = Math.RandomFloatInclusive(0, 1);
		
		float chance = m_Value * GetRelativeValue();

		int source_index;
		int count;
		
		if(chance > 0 && chanced <= chance)
		{
			count = m_Player.GetBleedingManagerServer().m_BleedingSources.Count();
			if(count == 0)
				return;
			
			source_index = Math.RandomIntInclusive(0, count - 1);
			BleedingSource bleeding_source = m_Player.GetBleedingManagerServer().m_BleedingSources.GetElement(source_index);
			m_Player.GetBleedingManagerServer().RequestDeletion(bleeding_source.GetBit());
		}
		else if(chance < 0 && chanced <= Math.AbsFloat(chance))
		{
			count = m_Player.GetBleedingManagerServer().m_BleedingSourceZone.Count() - 1;
			source_index = Math.RandomIntInclusive(0, count);
			BleedingSourceZone zone = m_Player.GetBleedingManagerServer().m_BleedingSourceZone.GetElement(source_index);
			
			m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceDirectly(zone.GetBit());
		}
	}
}