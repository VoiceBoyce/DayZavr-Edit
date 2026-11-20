class KOD_VSK94_Base : RifleBoltFree_Base
{
	int weaponOpticsAK = InventorySlots.GetSlotIdFromString("weaponOpticsAK");
	int weaponOpticsPic = InventorySlots.GetSlotIdFromString("weaponOptics");
	
	void KOD_VSK94_Base()
	{
		HideScopeMountKOD(this);
	}	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new VSSRecoil(this);
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();
		if( parent && parent.FindAttachmentBySlotName("weaponOptics") != NULL )
		{
			ShowScopeMountKOD(this);
		}	
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);	
		
		EntityAI parent = item.GetHierarchyParent();		
		if( parent && parent.FindAttachmentBySlotName("weaponOptics") == NULL )
		{
			HideScopeMountKOD(this);
		}		
	}	
	
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		switch(slotId)
		{
			case weaponOpticsAK:
			{
				if ( this.FindAttachmentBySlotName("weaponOpticsAK") != NULL || this.FindAttachmentBySlotName("weaponOptics") == NULL)
				{
					return true;
				};
				if ( this.FindAttachmentBySlotName("weaponOpticsAK") == NULL || this.FindAttachmentBySlotName("weaponOptics") != NULL)
				{
					return false;
				};
				break;
			};
			case weaponOpticsPic:
			{
				if ( this.FindAttachmentBySlotName("weaponOptics") != NULL || this.FindAttachmentBySlotName("weaponOpticsAK") == NULL)
				{
					return true;
				};
				if ( this.FindAttachmentBySlotName("weaponOptics") == NULL || this.FindAttachmentBySlotName("weaponOpticsAK") != NULL)
				{
					return false;
				};
				break;
			};
		
			return true;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanDisplayAttachmentSlot( string slot_name)
	{
		if ( slot_name == "weaponOptics" )
		{
			return ( this.FindAttachmentBySlotName("weaponOpticsAK") == NULL );
		}
		if ( slot_name == "weaponOpticsAK" )
		{
			return ( this.FindAttachmentBySlotName("weaponOpticsAK") != NULL );
		}
		return true;
	}
	
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if ( optic && PSO1Optic.Cast(optic) || PSO11Optic.Cast(optic) || KashtanOptic.Cast(optic) || KazuarOptic.Cast(optic) )
			return true;
		return super.CanEnterIronsights();
	}
};

class KOD_VSK94 : KOD_VSK94_Base{};

class KOD_VSK94_Bttstck : ButtstockBase {};
class KOD_VSK94_Hndgrd_Base extends Inventory_Base {};
class KOD_VSK94_Hndgrd_Accs extends KOD_VSK94_Hndgrd_Base {};
class KOD_VSK94_Hndgrd extends KOD_VSK94_Hndgrd_Base {};