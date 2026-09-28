enum ePouchTypesFOG
{
    NONE,
    TIER1,
    TIER2,
	TIER3
}

modded class ItemBase extends InventoryItem
{
    bool IsKindOfPouchFOG()
    {
        return false;
    }
    
    int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.NONE;
    }

    override bool CanPutInCargo( EntityAI parent )
    {
        ItemBase parentItem;
        
        if (IsKindOfPouchFOG() && Class.CastTo(parentItem, parent) && parentItem.IsKindOfPouchFOG() && GetPouchTypeFOG() >= parentItem.GetPouchTypeFOG())
            return false;
            
        return !IsHologram();
    }
}

class FOG_Pouch_Base extends Container_Base
{
    override bool IsKindOfPouchFOG()
    {
        return true;
    }

    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}

class FOG_TV110T_AdminPouchBase : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER3;
    }
}

class FOG_TV110T_UtilityPouchBase : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}

class FOG_TV110T_MagPouchesBase : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}

class FOG_Pouch_Mag_KTAR_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}

class FOG_Pouch_Mag_TEAR_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}


class FOG_Pouch_Mag_BlueFor_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}

class FOG_Pouch_Mag_Kangaroo_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}

class FOG_Pouch_Mag_AVS_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER2;
    }
}

class FOG_Pouch_Belly_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}

class FOG_JPC_Panel_Flag_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}

class FOG_FC_Panel_Flag_Base : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}

class FOG_Panel_MapPackMed_ColorBase : FOG_Pouch_Base
{
    override int GetPouchTypeFOG()
    {
        return ePouchTypesFOG.TIER1;
    }
}