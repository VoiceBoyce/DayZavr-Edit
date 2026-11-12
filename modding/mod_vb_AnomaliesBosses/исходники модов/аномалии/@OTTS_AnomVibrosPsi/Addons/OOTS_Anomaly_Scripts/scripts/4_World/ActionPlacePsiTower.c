class ActionPlacePsiTower: ActionDeployObject
{
	void ActionPlacePsiTower()
	{
	}
	
	/*override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(!super.ActionCondition(player, target, item))
			return false;
		
	}*/
	
	override void OnEndServer( ActionData action_data  )
	{
		super.OnEndServer(action_data);
	}
}