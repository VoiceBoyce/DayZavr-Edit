
modded class Clothing
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
            return false;

        array<string> slots = new array<string>;

		ConfigGetTextArray("inventorySlot", slots);

		if (slots.Find("Back") != -1)
		{
			if (ConfigGetString("soundAttType") == "Ghillie")
				return true;
				
			EntityAI vest = parent.FindAttachmentBySlotName("Vest");

														//сюда где в кавычках вписываешь этот подсумок для броника
			if (vest && vest.FindAttachmentBySlotName("BackPanel"))
				return false;
		}
		return true;
	}
}

// Этот класс делаешь основным и привязываешь все броники на этот класс 

class TGK_FSB_ALPHA_Vest_ColorBase : Clothing 
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
            return false;
        
        EntityAI Back = parent.FindAttachmentBySlotName("Back");

												//сюда где в кавычках вписываешь этот подсумок для броника
        if (Back && FindAttachmentBySlotName("BackPanel"))
        {
			if (Back.ConfigGetString("soundAttType") != "Ghillie")
				return false;
        }

														//сюда где в кавычках вписываешь этот подсумок для броника
        else if (Back && !FindAttachmentBySlotName("BackPanel"))
        {
            return true;
        }
		return true;
	}  
}
class TGK_FSB_ALPHA_Vest_Alpine extends TGK_FSB_ALPHA_Vest_ColorBase {};
class TGK_FSB_ALPHA_Vest_Black extends TGK_FSB_ALPHA_Vest_ColorBase {};
class TGK_FSB_ALPHA_Vest_Black_MC extends TGK_FSB_ALPHA_Vest_ColorBase {};
class TGK_FSB_ALPHA_Vest_MC extends TGK_FSB_ALPHA_Vest_ColorBase {};
class TGK_FSB_ALPHA_Vest_ODGreen extends TGK_FSB_ALPHA_Vest_ColorBase {};
class TGK_FSB_ALPHA_Vest_Tropic_MC extends TGK_FSB_ALPHA_Vest_ColorBase {};

// На этот класс привязываешь свой подсумок для броника 

class TGK_FSB_ALPHA_BackPanel_ColorBase : Clothing 
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
            return false;
        
		PlayerBase player = PlayerBase.Cast(parent.GetHierarchyRoot());

        if (player)
        {
			EntityAI Back = player.FindAttachmentBySlotName("Back");

			if (Back && Back.ConfigGetString("soundAttType") != "Ghillie")
				return false;
        }
		return true;
	}
}
class TGK_FSB_ALPHA_BackPanel_Alpine extends TGK_FSB_ALPHA_BackPanel_ColorBase {};
class TGK_FSB_ALPHA_BackPanel_Black extends TGK_FSB_ALPHA_BackPanel_ColorBase {};
class TGK_FSB_ALPHA_BackPanel_Black_MC extends TGK_FSB_ALPHA_BackPanel_ColorBase {};
class TGK_FSB_ALPHA_BackPanel_MC extends TGK_FSB_ALPHA_BackPanel_ColorBase {};
class TGK_FSB_ALPHA_BackPanel_ODGreen extends TGK_FSB_ALPHA_BackPanel_ColorBase {};
class TGK_FSB_ALPHA_BackPanel_Tropic_MC extends TGK_FSB_ALPHA_BackPanel_ColorBase {};