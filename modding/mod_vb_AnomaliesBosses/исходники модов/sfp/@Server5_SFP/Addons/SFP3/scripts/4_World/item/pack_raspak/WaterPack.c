/*class ActionWaterPackOpenCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionWaterPackOpen: ActionContinuousBase
{	
	ItemBase resultItem;
	void ActionWaterPackOpen()
	{
		m_CallbackClass = ActionWaterPackOpenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
		
	override string GetText()
	{
		return "#STR_SFP_OpenPackWaterAct";
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			switch(action_data.m_MainItem.GetType()) 
			{
				case "SFP_VodkaPack12":
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("GlassBottle" , action_data.m_Player.GetPosition(), false) );
				break;
				case "SFP_VodkaSFPPack12":
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Pseno" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Pseno" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Pseno" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Sibir" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Sibir" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Sibir" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_istrinka" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_istrinka" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_istrinka" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Rus" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Rus" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SFP_Vodka_Rus" , action_data.m_Player.GetPosition(), false) );
				break;
				case "SFP_CanTunaPack15":
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("TunaCan" , action_data.m_Player.GetPosition(), false) );
				break;	
				case "SFP_PipsiPack12":
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("SodaCan_Pipsi" , action_data.m_Player.GetPosition(), false) );
				break;	
				case "SFP_CanPack6":
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("BakedBeansCan" , action_data.m_Player.GetPosition(), false) );
				break;	
				case "SFP_BootleWaterPack6":
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
				break;	
				case "SFP_BootleWaterPack12":
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
					resultItem = ItemBase.Cast( GetGame().CreateObject("WaterBottle" , action_data.m_Player.GetPosition(), false) );
				break;
				
			}
			GetGame().ObjectDelete(action_data.m_MainItem);
		}
	}
};