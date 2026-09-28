modded class ItemBase
{
    bool IsKindOfPouchFOG()
    {
        return false;
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        FOG_Pouch_Base parentPouch;
        FOG_Pouch_Base thisPouch;

        if (Class.CastTo(thisPouch, this) && Class.CastTo(parentPouch, parent))
            return false;

        return !IsHologram();
    }
}

class FOG_Pouch_Base : Container_Base
{
    override bool IsKindOfPouchFOG()
    {
        return true;
    }
}


class FOG_MagPouch_Base : FOG_Pouch_Base
{}
class FOG_PanelPouch_Base : FOG_Pouch_Base
{}
class FOG_VestPouch_Base : FOG_Pouch_Base 
{}
class FOG_MedicalPouch_Base : FOG_Pouch_Base 
{}
class FOG_BeltPouch_Base : FOG_Pouch_Base 
{}

class FOG_Pouch_LBT_Base : FOG_MedicalPouch_Base {}
class FOG_Pouch_IFAK_Base : FOG_MedicalPouch_Base {}
class FOG_TV110T_MedkitBase : FOG_MedicalPouch_Base {}
class FOG_Pouch_TraumaRoll_Base : FOG_MedicalPouch_Base {}

class FOG_Pouch_Dump_Base : FOG_BeltPouch_Base {}
class FOG_Pouch_SingleMag_Base : FOG_BeltPouch_Base {}
class FOG_Pouch_Esstac_Base : FOG_BeltPouch_Base {}
class FOG_Pouch_FannyPack_Base : FOG_BeltPouch_Base {}

class FOG_TV110T_AdminPouchBase : FOG_VestPouch_Base {}
class FOG_TV110T_UtilityPouchBase : FOG_VestPouch_Base {}
class FOG_TV110T_FastMagBase : FOG_VestPouch_Base {}
class FOG_TV110T_GrenadePouchBase : FOG_VestPouch_Base {}
class FOG_Pouch_Admin_Small_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Admin_Spiritus_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Tall_Spiritus_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Mutant_Base : FOG_VestPouch_Base {}
class FOG_Pouch_JSTA_Base : FOG_VestPouch_Base {}
class FOG_PouchAdmin_Chest_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Grenade_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Belly_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Belly_Spiritus_Base : FOG_VestPouch_Base {}
class FOG_Pouch_Belly_Lunchbox_Base : FOG_VestPouch_Base {}
class FOG_Pouch_FerroDangler_Base : FOG_VestPouch_Base {}

class FOG_TV110T_MagPouchesBase : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_AVS_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_AVS_Fold_Base : FOG_Pouch_Mag_AVS_Base {}
class FOG_Pouch_Mag_BlueFor_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_Dope_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_DW_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_Kangaroo_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_KTAR_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_LV119_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_SSCCS_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_SSMK4_Base : FOG_MagPouch_Base {}
class FOG_Pouch_Mag_TEAR_Base : FOG_MagPouch_Base {}
class FOG_Placard_Paraclete_Base : FOG_MagPouch_Base {}

class FOG_JPC_Panel_Flag_Base : FOG_PanelPouch_Base {}
class FOG_FC_Panel_Flag_Base : FOG_PanelPouch_Base {}
class FOG_Panel_MapPackMed_ColorBase : FOG_PanelPouch_Base {}
class FOG_Panel_Spiritus_Base : FOG_PanelPouch_Base {}
class FOG_Panel_CryeZipon_Base : FOG_PanelPouch_Base {}
class FOG_Panel_FerroBanger_Base : FOG_PanelPouch_Base {}
class FOG_Panel_GMR_MiniMap_Base : FOG_PanelPouch_Base {}
