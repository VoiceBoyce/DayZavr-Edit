class KOD_AWM_Scope_Base extends ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{	
		if( parent.IsKindOf("KOD_AWM_Base"))
		{	
			return true;	
		}			
		return false;	
	}	
}; 
class KOD_AWM_Scope extends KOD_AWM_Scope_Base{}; 

modded class BUISOptic extends ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if ( parent.IsKindOf("KOD_M16A4_Base") )
		{
			return true;
		}

        return super.CanPutAsAttachment(parent);		
	}
};

modded class M4_CarryHandleOptic extends ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if ( parent.IsKindOf("KOD_M16A4_Base") )
		{
			return true;
		}

        return super.CanPutAsAttachment(parent);		
	}
};
