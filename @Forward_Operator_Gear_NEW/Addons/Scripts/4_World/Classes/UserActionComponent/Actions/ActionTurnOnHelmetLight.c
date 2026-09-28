class ActionTurnOnHelmetLight: ActionBase
{
	void ActionTurnOnHelmetLight()
	{
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override typename GetInputType()
	{
		return ToggleLightsActionInput;
	}

	protected override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Switchable_Base light;
		FOG_Helmet_Base helmet;
		helmet = FOG_Helmet_Base.Cast(target.GetObject());
		if ( !helmet )
			return false;
		light = Switchable_Base.Cast(helmet.FindAttachmentBySlotName("FOG_HMTL"));
		if ( !light )
			return false;
		
		if ( light.HasEnergyManager() && light.GetCompEM().CanSwitchOn() && light.GetCompEM().CanWork() ) //TODO review conditions for turning off
		{
			return true;
		}
		
		return false;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		Switchable_Base light;// = Switchable_Base.Cast(action_data.m_MainItem.FindAttachmentBySlotName("FOG_HMTL"));
		FOG_Helmet_Base helmet;
		helmet = FOG_Helmet_Base.Cast(action_data.m_Target.GetObject());
		if ( !helmet )
			return;
		light = Switchable_Base.Cast(helmet.FindAttachmentBySlotName("FOG_HMTL"));
		if ( !light )
			return;
		
		if ( light.HasEnergyManager() )
		{
			if ( light.GetCompEM().CanWork() )
			{
				light.GetCompEM().SwitchOn();
			}
		}
	}
};
