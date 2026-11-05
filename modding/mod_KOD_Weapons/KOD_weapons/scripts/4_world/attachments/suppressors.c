class KOD_M82A2_Suppressor_Base extends SuppressorBase {};
class KOD_M82A2_Suppressor_Grey extends KOD_M82A2_Suppressor_Base {};
class KOD_M82A2_Suppressor_Black extends KOD_M82A2_Suppressor_Base {};
class KOD_M82A2_Suppressor_Tan extends KOD_M82A2_Suppressor_Base {};

class KOD_AWM_Suppressor extends SuppressorBase 
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL )
		{
			return true;
		}
		return false;
	}	
};

class KOD_Suppressor_308 extends SuppressorBase 
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL )
		{
			if( parent.IsKindOf("KOD_R700_Base") || parent.IsKindOf("KOD_HK417_Base") || parent.IsKindOf("KOD_RSASS_Base") || parent.IsKindOf("FAL") || parent.IsKindOf("KOD_M110_Base") || parent.IsKindOf("KOD_SCARH_Base") || parent.IsKindOf("KOD_SA58_Base") || parent.IsKindOf("KOD_HK21_Base") || parent.IsKindOf("KOD_MDR_Base") ) 
			{				
				return true;
			}
		}
		return false;
	}		
};

class KOD_VSK94_Silencer_Base extends ItemSuppressor 
{
	override bool CanDetachAttachment (EntityAI parent)
	{
		if ( Weapon_Base.Cast(parent) && parent.FindAttachmentBySlotName("weaponFlashlight") )
		{
			return false;
		}
		return true;
	}
};
class KOD_VSK94_Silencer extends KOD_VSK94_Silencer_Base {};
class KOD_VSK94_Silencer_Wrap extends KOD_VSK94_Silencer_Base {};

class KOD_M200_Suppressor extends SuppressorBase {};

modded class M4_Suppressor extends SuppressorBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if( parent.IsKindOf("KOD_MDR_Base") ) 
		{				
			return false;
		}
		return super.CanPutAsAttachment(parent);
	}	
};

class KOD_Vector_Suppressor extends SuppressorBase {};

class KOD_Glock17_FlashHider extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( !parent.FindAttachmentBySlotName("pistolMuzzle") && !parent.FindAttachmentBySlotName("weaponMuzzleMP5"))
		{
			return true;
		}
		return false;
	}
};
class KOD_Glock17_Compensator extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( !parent.FindAttachmentBySlotName("pistolMuzzle") && !parent.FindAttachmentBySlotName("weaponMuzzleMP5"))
		{
			return true;
		}
		return false;
	}
};

class KOD_PBS1Suppressor extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponBayonetAK") == NULL )
		{
			return true;
		}
		return false;
	}
};

class KOD_DTKPutnik_Base extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponBayonetAK") == NULL )
		{
			return true;
		}
		return false;
	}
};

class KOD_Weapons_M4Suppressor_Base extends SuppressorBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponBayonet") == NULL )
		{
			return true;
		}
		return false;
	}
}