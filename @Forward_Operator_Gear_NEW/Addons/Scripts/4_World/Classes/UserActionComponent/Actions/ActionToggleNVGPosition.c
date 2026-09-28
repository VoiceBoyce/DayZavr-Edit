class ActionToggleNVGPosition: ActionBase
{
    void ActionToggleNVGPosition()
    {
    }
    
    override bool IsInstant()
    {
        return true;
    }
    
    override void CreateConditionComponents()  
    {    
        m_ConditionItem = new CCINone;
        m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
    }
    
    override typename GetInputType()
    {
        return ToggleNVGPositionActionInput;
    }
    
    override bool HasTarget()
    {
        return true;
    }
    
    override bool UseMainItem()
    {
        return false;
    }
    
    protected override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
        FOG_NVG_Togglable_Base goggles;
        Clothing NVmount;
        NVmount = Clothing.Cast(target.GetObject());
        if ( !NVmount )
            return false;
        goggles = FOG_NVG_Togglable_Base.Cast(NVmount.FindAttachmentBySlotName("NVG"));
        if ( goggles )
            return true;
        
        return false;
    }

    override void Start( ActionData action_data )
    {
        super.Start( action_data );
        
        FOG_NVG_Togglable_Base goggles;
        Clothing NVmount;

        NVmount = Clothing.Cast(action_data.m_Target.GetObject());
        if ( !NVmount )
            return;
        goggles = FOG_NVG_Togglable_Base.Cast(NVmount.FindAttachmentBySlotName("NVG"));

        if ( !goggles )
            return;
        
        goggles.ToggleReadyGoggles();
    }
};