class TGK_RussianSoldier_v2_GranadePouch_base : Container_Base
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
class TGK_RussianSoldier_v2_MagPouchBig_base : Container_Base
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
class TGK_RussianSoldier_v2_GroinPouch_ColorBase : Container_Base
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
class TGK_RussianSoldier_v2_MagPouchSmall_base : Container_Base
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
class TGK_RussianSoldier_v2_MagPouchSmall2_base : Container_Base
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
class TGK_RussianSoldier_v2_MedPouch2_base : Container_Base
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