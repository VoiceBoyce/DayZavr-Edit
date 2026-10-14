
class Stalker_2_Duty_01_Helmet_Filter_Colorbase : ItemBase
{
    override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
    {
        if (IsDamageDestroyed() || (HasQuantity() && GetQuantity() <= 0))
            return 0;

        string subclassPath, entryName;

        switch (type)
        {
            case DEF_BIOLOGICAL:
                entryName = "biological";
                break;
            case DEF_CHEMICAL:
                entryName = "chemical";
                break;
            default:
                entryName = "biological";
                break;
        }

        subclassPath = "CfgVehicles " + this.GetType() + " Protection ";

        return GetGame().ConfigGetFloat(subclassPath + entryName);
    }
}


class Stalker_2_Duty_01_Helmet_Filter : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}
class Stalker_2_Duty_01_Helmet_Filter_ClearSky : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}
class Stalker_2_Duty_01_Helmet_Filter_Korpus : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}
class Stalker_2_Duty_01_Helmet_Filter_Monolith : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}
class Stalker_2_Duty_01_Helmet_Filter_MSOP : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}
class Stalker_2_Duty_01_Helmet_Filter_Warta : Stalker_2_Duty_01_Helmet_Filter_Colorbase {}