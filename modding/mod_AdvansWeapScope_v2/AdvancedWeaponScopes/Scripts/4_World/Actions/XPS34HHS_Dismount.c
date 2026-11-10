class ActionDismountXPS34HHS : ActionSingleUseBase
{
    void ActionDismountXPS34HHS ()
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
            if(item.GetType() == "AD_XPS34_HHS")
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
        Object twoItem = GetGame().CreateObject("AD_XPS34", action_data.m_Player.GetPosition());
        twoItem.SetHealth(healthItem);
		
		//--Check for battery
		array<string> attachmentSlots = new array<string>;
		GetGame().ConfigGetTextArray("CfgVehicles " + action_data.m_MainItem.GetType() + " attachments", attachmentSlots);
		if(attachmentSlots.Find("BatteryD") > -1)
		{
			EntityAI battery = action_data.m_MainItem.FindAttachmentBySlotName("BatteryD");
			if ( battery )
			{
				EntityAI twoItemEAI = EntityAI.Cast( twoItem );
				EntityAI ib_Battery = twoItemEAI.GetInventory().CreateAttachment( "Battery9V" );
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