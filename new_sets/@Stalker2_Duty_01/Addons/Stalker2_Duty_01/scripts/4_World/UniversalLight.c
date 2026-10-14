class Stalker_2_Duty_01_UniversalLight extends UniversalLight
{
    override bool CanPutAsAttachment(EntityAI parent)
    {
        if (parent && parent.IsKindOf("Stalker_2_Duty_01_Helmet_Colorbase"))
        {
            return true;
        }
        
        return false;
    }
}