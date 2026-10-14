class Stalker_2_Monolith_02_Pouches_ColorBase : Container_Base
{
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;
		
		if (GetInventory().IsAttachment())
		{
			return !GetHierarchyParent().GetInventory().IsInCargo() && (!item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer()));
		}
		
		return !item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer());
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if (!super.CanLoadItemIntoCargo(item))
			return false;
		
		return !item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer());
	}
};