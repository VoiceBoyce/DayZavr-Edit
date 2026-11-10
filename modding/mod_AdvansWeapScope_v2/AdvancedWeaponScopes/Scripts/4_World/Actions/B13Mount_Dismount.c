class ActionDismountPilad_B13 : ActionSingleUseBase
{
    void ActionDismountPilad_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_Pilad_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_Pilad", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);

        action_data.m_MainItem.Delete();		
    }
};

class ActionDismountDHF5_B13 : ActionSingleUseBase
{
    void ActionDismountDHF5_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_DHF5_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_DHF5", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);

        action_data.m_MainItem.Delete();			
    }
};

class ActionDismountPVS4_B13 : ActionSingleUseBase
{
    void ActionDismountPVS4_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_PVS4_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_PVS4", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountOKP7_B13 : ActionSingleUseBase
{
    void ActionDismountOKP7_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_OKP7_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_OKP7", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountRMRLM_B13 : ActionSingleUseBase
{
    void ActionDismountRMRLM_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_RMR_LM_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_RMR_LM", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountRMRTM_B13 : ActionSingleUseBase
{
    void ActionDismountRMRTM_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_RMR_TM_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_RMR_TM", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountACOG_B13 : ActionSingleUseBase
{
    void ActionDismountACOG_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_ACOG_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_ACOG", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);

        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountACOGRMR_B13 : ActionSingleUseBase
{
    void ActionDismountACOGRMR_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_ACOG_RMR_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_ACOG_RMR", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountMRS_B13 : ActionSingleUseBase
{
    void ActionDismountMRS_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_MRS_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_MRS", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountNFATACR_B13 : ActionSingleUseBase
{
    void ActionDismountNFATACR_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_NFATACR_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_NFATACR", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);

        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountNFATACR_1_8_B13 : ActionSingleUseBase
{
    void ActionDismountNFATACR_1_8_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_NFATACR_1_8_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }
	
	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_NFATACR_1_8", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountSpecterDR_B13 : ActionSingleUseBase
{
    void ActionDismountSpecterDR_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_SpecterDR_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }
	
	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_SpecterDR", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountLeupoldMk4_B13 : ActionSingleUseBase
{
    void ActionDismountLeupoldMk4_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_LeupoldMk4_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_LeupoldMk4", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountXPS34_B13 : ActionSingleUseBase
{
    void ActionDismountXPS34_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_XPS34_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_XPS34", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountXPS34_HHS_B13 : ActionSingleUseBase
{
    void ActionDismountXPS34_HHS_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_XPS34_HHS_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_XPS34_HHS", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI oneItemEAI = EntityAI.Cast( oneItem );
				EntityAI ib_Battery = oneItemEAI.GetInventory().CreateAttachment( "Battery9V" );
//				ItemBase ib_Battery = ItemBase.Cast( GetGame().CreateObject("Battery9v", action_data.m_Player.GetPosition()) );
				if ( ib_Battery )
				{
					ib_Battery.SetHealth( battery.GetHealth() );
					ib_Battery.GetCompEM().SetEnergy( battery.GetCompEM().GetEnergy() );
					ib_Battery.GetCompEM().Synch();
				}
			}
		}	
        action_data.m_MainItem.Delete();	
    }
};

class ActionDismountG33_B13 : ActionSingleUseBase
{
    void ActionDismountG33_B13 ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "#unmount";
    }

    override bool HasTarget () 
    { 
        return false; 
    }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if( item)
        {
            if(item.GetType() == "AD_G33_B13")
                return true;
        }
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) 
    { 
        return true; 
    }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

    override void OnExecuteServer ( ActionData action_data )
    {
        float healthItem = action_data.m_MainItem.GetHealth();
        Object oneItem = GetGame().CreateObject("AD_G33", action_data.m_Player.GetPosition());
        oneItem.SetHealth(healthItem);
        Object twoItem = GetGame().CreateObject("AD_B13Mount", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);

        action_data.m_MainItem.Delete();	
    }
};