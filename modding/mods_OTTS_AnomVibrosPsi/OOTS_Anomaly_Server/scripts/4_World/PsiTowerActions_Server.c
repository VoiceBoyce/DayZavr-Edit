modded class ActionPlacePsiTower
{
	override void OnEndServer( ActionData action_data  )
	{
		super.OnEndServer(action_data);
		
		if ( !action_data || !action_data.m_MainItem )
			return;
		
		PlaceObjectActionData poActionData;
		poActionData = PlaceObjectActionData.Cast(action_data);
		
		if(poActionData && poActionData.m_AlreadyPlaced)
		{
			GetGame().ObjectDelete(action_data.m_MainItem);
			EntityAI entity = EntityAI.Cast( GetGame().CreateObject("OOTS_item_PsyTower", poActionData.m_Position) );
			if(entity)
				entity.SetOrientation(poActionData.m_Orientation);
		}
	}
}

modded class ActionDecodePsiTower
{
	override void OnFinishProgressServer( ActionData action_data )
	{
		OOTS_item_PsyTower psitower;
		if ( Class.CastTo(psitower, action_data.m_Target.GetObject() ) )
		{
			psitower.EndPsiEmmition();
		}
	}
}