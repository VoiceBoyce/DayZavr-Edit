/*modded class Hologram
{	
	override string ProjectionBasedOnParent()
	{
		OOTS_item_PsyBox item_in_hands = OOTS_item_PsyBox.Cast( m_Player.GetItemInHands() );

		if ( item_in_hands )
		{
			return item_in_hands.GetHologram();
		}
		
		return super.ProjectionBasedOnParent();
	}
	
	override EntityAI PlaceEntity( EntityAI entity_for_placing )
	{
		if ( entity_for_placing.IsInherited( OOTS_item_PsyTower ))
		{
			return entity_for_placing;
		}
		
		return super.PlaceEntity(entity_for_placing);
	}
}*/