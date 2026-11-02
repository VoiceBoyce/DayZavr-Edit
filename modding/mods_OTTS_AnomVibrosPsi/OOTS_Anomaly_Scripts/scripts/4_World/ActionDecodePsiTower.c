class ActionDecodePsiTowerCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(33);
	}
};

class ActionDecodePsiTower: ActionContinuousBase
{
	void ActionDecodePsiTower()
	{
		m_CallbackClass = ActionDecodePsiTowerCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CRAFTING;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		//m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override string GetText()
	{
		return "Декодировать";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( !target ) 
			return false;
		
		OOTS_item_PsyDecoder decoder = OOTS_item_PsyDecoder.Cast( player.GetItemInHands() );
		if( !decoder || !decoder.GetCompEM().IsWorking())
			return false;

		OOTS_item_PsyTower psitower;
		if ( Class.CastTo(psitower, target.GetObject() ) && psitower.IsActive() )
		{
			return true;
		}
		
		return false;
	}
}