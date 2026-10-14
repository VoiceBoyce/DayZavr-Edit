class Stalker_2_Duty_01_Helmet_Colorbase : Mich2001Helmet 
{
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if (!super.CanReceiveAttachment(attachment, slotId))
            return false;

        string slot_name = InventorySlots.GetSlotName(slotId);

        if (slot_name == "helmetFlashlight")
        {
            if (attachment && attachment.GetType() == "Stalker_2_Duty_01_UniversalLight")
            {
                return true;
            }
            return false;
        }

        return true;
    }
}

class Stalker_2_Duty_01_Helmet : Stalker_2_Duty_01_Helmet_Colorbase {}
class Stalker_2_Duty_01_Helmet_ClearSky : Stalker_2_Duty_01_Helmet_Colorbase {}
class Stalker_2_Duty_01_Helmet_Korpus : Stalker_2_Duty_01_Helmet_Colorbase {}
class Stalker_2_Duty_01_Helmet_Monolith : Stalker_2_Duty_01_Helmet_Colorbase {}
class Stalker_2_Duty_01_Helmet_MSOP : Stalker_2_Duty_01_Helmet_Colorbase {}
class Stalker_2_Duty_01_Helmet_Warta : Stalker_2_Duty_01_Helmet_Colorbase {}