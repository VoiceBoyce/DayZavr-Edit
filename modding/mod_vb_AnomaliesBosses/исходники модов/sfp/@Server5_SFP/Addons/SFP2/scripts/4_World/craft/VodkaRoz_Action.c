/*class ActionOpenVodkaRoz: ActionOpen
{
	void ActionOpenVodkaRoz()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE;
		//m_Animation = "open";
		m_Text = "#STR_Vodka_Roza";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item && !item.IsOpen())
		{ 
			Edible_Base edible;
			if (Class.CastTo(edible,item) && edible.GetIsFrozen())
				return false;
			
			return true;
		}
		return false;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);
		
		action_data.m_MainItem.Open();
	}
};*/
class ActionOpenVodkaRoz: ActionOpen
{
	void ActionOpenVodkaRoz()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE;
		//m_Animation = "open";
		m_Text = "#STR_Vodka_Roza";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( item && !item.IsOpen() )
		{ 
			return true;
		}
		return true;
	}
	
	//override bool ActionConditionContinue( ActionData action_data )
	//{
	//	return true;
	//}
	
	//override void OnExecuteClient( ActionData action_data )
	//{
	//	ClearInventoryReservationEx(action_data);
	//}

	//override void OnExecuteServer( ActionData action_data )
	//{
	//	if( !GetGame().IsMultiplayer() )
	//		ClearInventoryReservationEx(action_data);
	//	
	//	action_data.m_MainItem.Open();
	//}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (!action_data || !action_data.m_MainItem)
			return;

		PlayerBase player = action_data.m_Player;
		ItemBase item = action_data.m_MainItem;

		ReplaceVodkaLambda lambda = new ReplaceVodkaLambda(item, "SFP_Rozocka", player);
		player.ServerReplaceItemInHandsWithNew(lambda);
	}
};

class ReplaceVodkaLambda : ReplaceItemWithNewLambdaBase
{
	void ReplaceVodkaLambda(EntityAI old_item, string new_item_type, PlayerBase player)
	{
	}

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
	}
};
