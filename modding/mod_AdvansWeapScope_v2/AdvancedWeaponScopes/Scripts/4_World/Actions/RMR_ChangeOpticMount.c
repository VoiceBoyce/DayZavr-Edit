class ActionChangeOpticMount_RMR_NONE_to_L : ActionSingleUseBase
{
	void ActionChangeOpticMount_RMR_NONE_to_L ()
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
		return "#dialog_change";
	}

	override bool HasTarget () 
	{ 
		return false; 
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( item)
		{
			if(item.GetType() == "AD_RMR")
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
		if ( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);

		ItemBase old_item = action_data.m_MainItem;
		
		string str = "AD_RMR_LM";
		TurnItemIntoItemLambda lambda = new TurnItemIntoItemLambda(action_data.m_MainItem, str, action_data.m_Player);
		lambda.SetTransferParams(true, true, true, false, 1);
		action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);

	}
};
class ActionChangeOpticMount_RMR_L_to_H : ActionSingleUseBase
{
	void ActionChangeOpticMount_RMR_L_to_H ()
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
		return "#dialog_change";
	}

	override bool HasTarget () 
	{ 
		return false; 
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( item)
		{
			if(item.GetType() == "AD_RMR_LM")
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
		if ( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);

		ItemBase old_item = action_data.m_MainItem;
		
		string str = "AD_RMR_TM";
		TurnItemIntoItemLambda lambda = new TurnItemIntoItemLambda(action_data.m_MainItem, str, action_data.m_Player);
		lambda.SetTransferParams(true, true, true, false, 1);
		action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);

	}
};
class ActionChangeOpticMount_RMR_H_to_NONE : ActionSingleUseBase
{
	void ActionChangeOpticMount_RMR_H_to_NONE ()
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
		return "#dialog_change";
	}

	override bool HasTarget () 
	{ 
		return false; 
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( item)
		{
			if(item.GetType() == "AD_RMR_TM")
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
		if ( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);

		ItemBase old_item = action_data.m_MainItem;
		
		string str = "AD_RMR";
		TurnItemIntoItemLambda lambda = new TurnItemIntoItemLambda(action_data.m_MainItem, str, action_data.m_Player);
		lambda.SetTransferParams(true, true, true, false, 1);
		action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);

	}
};