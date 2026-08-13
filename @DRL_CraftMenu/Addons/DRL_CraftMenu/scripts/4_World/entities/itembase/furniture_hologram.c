modded class Hologram
{	
	override string ProjectionBasedOnParent()
	{
		ItemBase item_in_hands = ItemBase.Cast(m_Player.GetHumanInventory().GetEntityInHands());
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit"))
			return "DRL_Craft_WB";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit1"))
			return "DRL_Craft_WB1";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit2"))
			return "DRL_Craft_WB2";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit3"))
			return "DRL_Craft_WB3";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit4"))
			return "DRL_Craft_WB4";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit5"))
			return "DRL_Craft_WB5";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit6"))
			return "DRL_Craft_WB6";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit7"))
			return "DRL_Craft_WB7";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit8"))
			return "DRL_Craft_WB8";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit9"))
			return "DRL_Craft_WB9";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Garden_Kit"))
			return "DRL_Craft_Garden";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Cook_Kit"))
			return "DRL_Craft_Cook";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Build_Kit"))
			return "DRL_Craft_Build";
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Workbench_Kit"))
			return "DRL_Craft_Workbench";
		
		return super.ProjectionBasedOnParent();
	}
	
    override EntityAI PlaceEntity( EntityAI entity_for_placing )
    {    
        ItemBase item_in_hands = ItemBase.Cast( m_Player.GetHumanInventory().GetEntityInHands() );
    
        if ( item_in_hands && item_in_hands.CanMakeGardenplot() )
        {
            entity_for_placing = super.PlaceEntity(entity_for_placing);
        }
            
        if( entity_for_placing && entity_for_placing.CanAffectPathgraph() )
        {        
            entity_for_placing.SetAffectPathgraph( true, false );
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, entity_for_placing);
        }
        
        return entity_for_placing;
    } 

    override void EvaluateCollision(ItemBase action_item = null)
    {
		ItemBase item_in_hands = ItemBase.Cast(m_Player.GetHumanInventory().GetEntityInHands());
		
        if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit1"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit2"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit3"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit4"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit5"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit6"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit7"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit8"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Kit9"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Garden_Kit"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Cook_Kit"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Build_Kit"))
        {
            SetIsColliding(false);
            return;
        }
		
		if (item_in_hands && item_in_hands.IsKindOf("DRL_Craft_Workbench_Kit"))
        {
            SetIsColliding(false);
            return;
        }

        super.EvaluateCollision(action_item);
    }
}