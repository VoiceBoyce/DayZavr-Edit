class MSFZ_RuneGathering_Base extends BuildingSuper
{
    bool m_MSFZ_IsRuneAlreadyGathed = false;
    ////Сколько времени нужно чтобы экшен стал дооступен
    protected int MSFZ_RuneGatheringCooldown = 30;
		
    void MSFZ_RuneGathering_Base()
    {
        m_MSFZ_IsRuneAlreadyGathed = false;
    }

    bool MSFZ_RuneGatheredCheck(PlayerBase player)
    {
        if (m_MSFZ_IsRuneAlreadyGathed)
        {
            return true;
        }
        return false;
    }
	
	bool MSFZ_Timeoutlimit()
	{
		return MSFZ_RuneGatheringCooldown;
	}
	
    void MSFZ_RuneGatheringTimeout()
    {
        m_MSFZ_IsRuneAlreadyGathed = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_RuneGatheringReGen, MSFZ_RuneGatheringCooldown * 60 * 1000, false);
    }

    void MSFZ_RuneGatheringReGen()
    {
        m_MSFZ_IsRuneAlreadyGathed = false;
    }

    string GetDropItemClassName(string targetClassName)
	{
	    switch(targetClassName)
	    {
	        case "MSFZ_Obelisk":
	            return "MSFZ_RuneStone_Day";
	            break;
	        case "msfz_stone_1":
	            return "MSFZ_RuneStone_FireSmall";
	            break;
	        case "msfz_stone_2":
	            return "MSFZ_RuneStone_FireSmall";
	            break;
	        case "msfz_stone_altar_1":
	            return "MSFZ_RuneStone_Ice";
	            break;
	        case "msfz_stone_altar_2":
	            return "MSFZ_RuneStone_IceSmall";
	            break;
	        case "msfz_stone_altar_3":
	            return "MSFZ_RuneStone_StormSmall";
	            break;
	        case "msfz_stone_altar_4":
	            return "MSFZ_RuneStone_Storm";
	            break;
	        default:
	            return "";
	            break;
	    }
		return "stone";
	}
}

class MSFZ_Obelisk : MSFZ_RuneGathering_Base {};
class msfz_stone_1 : MSFZ_RuneGathering_Base {};
class msfz_stone_2 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_1 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_2 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_3 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_4 : MSFZ_RuneGathering_Base {};

class ObeliskPositions
{
    static vector Obelisk1Position = vector.Zero;
    static vector Obelisk2Position = vector.Zero;
}
class MSFZ_Portal_Base : House {};
class MSFZ_Portal : MSFZ_Portal_Base
{
    vector m_pos_g;
    string m_message_teleport = "#STR_MSFZ_teleportation";
    
    override void EEInit()
    {
        m_pos_g = GetPosition();
        ObeliskPositions.Obelisk1Position = m_pos_g;
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionTeleportToObelisk);
    }
}

class MSFZ_Portal_Exit : MSFZ_Portal_Base
{
    vector m_pos_g;
    string m_message_teleport = "#STR_MSFZ_teleportation";
    
    override void EEInit()
    {
        m_pos_g = GetPosition();
        ObeliskPositions.Obelisk2Position = m_pos_g;
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionTeleportToObelisk);
    }
}

