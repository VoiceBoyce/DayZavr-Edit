class FOG_NVG_Togglable_Base extends NVGoggles
{
    bool m_IsReady;

    void FOG_NVG_Togglable_Base()
    {
        RegisterNetSyncVariableBool("m_IsReady");
    }

    void SetReadyGoggles(bool state)
    {
        Print("[SetReadyGoggles] state: " + state + ", m_IsLowered: " + m_IsLowered);

        if (m_IsLowered)
        {
            Print("Bailing out.");
            return;
        }
        
        m_IsReady = state;

        Print("[SetReadyGoggles] m_IsReady: " + m_IsReady);
        
        float animPhase;
        if (state)
            animPhase = 0.0;
        else
            animPhase = 0.7;
        
        SetAnimationPhase("rotate", animPhase);
        SetSynchDirty();
    }

    override void LoweredCheck() //check for animation state, if another player lowered them first (or solve by synced variable)
    {
        if ( m_IsLowered != (GetAnimationPhase("rotate") == 1.0) )
            m_IsLowered = (GetAnimationPhase("rotate") == 1.0);
    }


    override void RotateGoggles(bool state)
    {
        float animPhase = 0.0;
        if (state)
        {
            if (m_IsReady)
            {
                animPhase = 0.7;
            }
            else
            {
                animPhase = 0.0;
            }
        }
        else
        {
            animPhase = 1.0;
        }

        SetAnimationPhase("rotate", animPhase);
        m_IsLowered = !state;

        PlayerBase player;
        int slot_id;
        string slot_name;
        if ( m_Strap && m_Strap.GetInventory().GetCurrentAttachmentSlotInfo(slot_id,slot_name) && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
            player.SetNVGLowered(m_IsLowered);
        
        if ( GetCompEM() )
        {
            if ( !state && GetCompEM().CanWork() )
                GetCompEM().SwitchOn();
            else
                GetCompEM().SwitchOff();
        }
    }
};
class FOG_PVS31A_Base: FOG_NVG_Togglable_Base{}
class FOG_UAPNVG_Black: FOG_NVG_Togglable_Base{}
class FOG_UAPNVG_Tan: FOG_NVG_Togglable_Base{}
class FOG_UAPNVG_RG: FOG_NVG_Togglable_Base{}