class ActionRotateArmHeadsets : ActionSingleUseBase
{
    void ActionRotateArmHeadsets ()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_BATONEXTEND;
    }

    override void CreateConditionComponents ()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return m_Text;
    }

    override bool HasTarget () { return false; }

    override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
    {
        FOG_HelmetArmHeadset_Base armheadsets;
        if (item)
        {
            if (Class.CastTo(armheadsets, item.FindAttachmentBySlotName("FOG_ear_cover")))
            {
                if (armheadsets.GetAnimationPhase("arms") == 0)
                {
                    m_Text = "Flip Headphones Up";
                }
                else if (armheadsets.GetAnimationPhase("arms") == 1.0)
                {
                    m_Text = "Flip Headphones Down";
                }
                else
                {
                    return false;
                }

            return true;
            }
        }
        
        return false;
    }

    override bool ActionConditionContinue ( ActionData action_data ) { return true; }

    override void OnExecuteClient ( ActionData action_data )
    {
        ClearInventoryReservationEx(action_data);
    }

    override void OnExecuteServer ( ActionData action_data )
    {
        if ( !GetGame().IsMultiplayer() )
            ClearInventoryReservationEx(action_data);
        
        FOG_HelmetArmHeadset_Base armheadsets;
        if (Class.CastTo(armheadsets, action_data.m_MainItem.FindAttachmentBySlotName("FOG_ear_cover")))
        {
            if (armheadsets.GetAnimationPhase("arms") == 0.0)
            {
                armheadsets.SetAnimationPhase("arms", 1.0);
            }
            else if (armheadsets.GetAnimationPhase("arms") == 1.0)
            {
                armheadsets.SetAnimationPhase("arms", 0.0);
            }
        }
    }
}