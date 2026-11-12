class ActionTogglePlacePsiTower: ActionTogglePlaceObject
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		// not placeable if liquid is present; exception for barrels, for now
		OOTS_item_PsyBox psi_box;
		if ( Class.CastTo(psi_box, item) && psi_box.HasComponents() )
		{
			return true;
		}
		
		return false;
	}
}