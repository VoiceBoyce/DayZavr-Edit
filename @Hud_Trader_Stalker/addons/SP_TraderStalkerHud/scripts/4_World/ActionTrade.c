modded class ActionTrade: ActionInteractBase
{
	void ActionTrade()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_HUDCursorIcon = CursorIcons.CloseHood;
		m_Text = "Торговать";
	}
	
	bool CanOpenTrader(PlayerBase player, Object target)
	{
		m_Player = player;
		vector playerPosition = player.GetPosition();
		m_Trader_AllowedTradeDistance = player.m_Trader_TradingDistance;
		BuildingBase buildingItem = BuildingBase.Cast(target);
		m_traderIndex = getNearbyTraderUID(playerPosition);
		if(buildingItem && buildingItem.m_Trader_TraderIndex > -1)
		{			
			m_traderIndex = buildingItem.m_Trader_TraderIndex;
		}
		PlayerBase playerTrader = PlayerBase.Cast(target);
		if(playerTrader&& playerTrader.m_Trader_TraderIndex > -1)
		{
			m_traderIndex = playerTrader.m_Trader_TraderIndex;
		}
		m_traderID = getTraderID();
		if (m_traderIndex != -1)
		{
			if(player.m_Trader_TraderNames)
			{
				string traderName = player.m_Trader_TraderNames.Get(getTraderID());
				m_Text = "Торговать [" + traderName + "]";
			}
			return true;
		}
		return false;
	}
};