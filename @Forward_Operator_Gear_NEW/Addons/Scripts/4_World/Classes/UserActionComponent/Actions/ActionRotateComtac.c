class ActionRotateComtacs : ActionSingleUseBase
{
    void ActionRotateComtacs ()
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
        FOG_Comtac3_Headphones_Base comtacs;
        if (item)
        {
            if (Class.CastTo(comtacs, item.FindAttachmentBySlotName("SF_Comtacs")))
            {
                if (comtacs.GetAnimationPhase("arc_rotate") == 0)
                {
                    m_Text = "Flip Comtacs Up";
                }
                else if (comtacs.GetAnimationPhase("arc_rotate") == 1.0)
                {
                    m_Text = "Flip Comtacs Down";
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
        
        FOG_Comtac3_Headphones_Base comtacs;
        if (Class.CastTo(comtacs, action_data.m_MainItem.FindAttachmentBySlotName("SF_Comtacs")))
        {
            if (comtacs.GetAnimationPhase("arc_rotate") == 0.0)
            {
                comtacs.SetAnimationPhase("arc_rotate", 1.0);
            }
            else if (comtacs.GetAnimationPhase("arc_rotate") == 1.0)
            {
                comtacs.SetAnimationPhase("arc_rotate", 0.0);
            }
        }
    }
}