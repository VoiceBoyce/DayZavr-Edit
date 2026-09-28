// Togglable NVGs add flip-up/flip-down (raise/lower) behaviour on top of vanilla NVGoggles.
// All other FOG NVGs inherit NVGoggles directly - no extra script needed.
class FOG_NVG_Base extends NVGoggles 
{
    protected int m_IdxGlassback = -1;
    protected int m_IdxSplash = -1;
    protected bool m_SelectionsActive = false;
    protected bool m_SelectionIdsResolved = false;

    protected bool FOG_ShouldShowSelections()
    {
        bool wornByPlayer = m_Strap && m_Strap.GetHierarchyRootPlayer();
        if (!wornByPlayer)
            return false;

        if (!GetCompEM())
            return false;

        if (!GetCompEM().CanWork())
            return false;

        if (!GetCompEM().IsSwitchedOn())
            return false;

        return true;
    }

    protected void FOG_GetSelectionIds()
    {
        if (m_SelectionIdsResolved)
            return;

        TStringArray selections = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections", selections);
        m_IdxGlassback = selections.Find("selection_glassback");
        m_IdxSplash = selections.Find("selection_splash");

        if (m_IdxGlassback == -1 || m_IdxSplash == -1)
        {
            selections.Clear();
            ConfigGetTextArray("hiddenSelections", selections);

            if (m_IdxGlassback == -1)
                m_IdxGlassback = selections.Find("selection_glassback");

            if (m_IdxSplash == -1)
                m_IdxSplash = selections.Find("selection_splash");
        }

        m_SelectionIdsResolved = true;
    }

    protected void FOG_SetSelections(bool show)
    {
        FOG_GetSelectionIds();
        if (m_IdxGlassback != -1) SetSimpleHiddenSelectionState(m_IdxGlassback, show);
        if (m_IdxSplash != -1) SetSimpleHiddenSelectionState(m_IdxSplash, show);
        m_SelectionsActive = show;
    }

    // Called by playerbase when the parent helmet is attached/detached from the player
    void FOG_OnParentWornStateChanged(bool worn)
    {
        if (!worn)
        {
            FOG_SetSelections(false);
            return;
        }

        if (FOG_ShouldShowSelections())
            FOG_SetSelections(true);
        else
            FOG_SetSelections(false);
    }

    override void EEInit()
    {
        super.EEInit();
        FOG_SetSelections(false);
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (FOG_ShouldShowSelections())
            FOG_SetSelections(true);
        else
            FOG_SetSelections(false);
    }

     override void OnWorkStart()
    {
        super.OnWorkStart();
        EffectSound sound;
        PlaySoundSet(sound, "FOG_NVG_ON_SoundSet", 0.1, 0.1);

        if (FOG_ShouldShowSelections())
            FOG_SetSelections(true);
    }

    override void OnWork(float consumed_energy)
    {
        super.OnWork(consumed_energy);

        // Fallback: catch any state drift not handled by the playerbase hook
        bool shouldShow = FOG_ShouldShowSelections();

        if (m_SelectionsActive != shouldShow)
            FOG_SetSelections(shouldShow);
    }

    override void OnWorkStop()
    {
        super.OnWorkStop();
        EffectSound sound;
        PlaySoundSet(sound, "FOG_NVG_OFF_SoundSet", 0.1, 0.1);

        FOG_SetSelections(false);
    }

    override void OnWasDetached(EntityAI parent, int slot_id)
    {
        super.OnWasDetached(parent, slot_id);
        FOG_SetSelections(false);
    }

    override void OnWasAttached(EntityAI parent, int slot_id)
    {
        super.OnWasAttached(parent, slot_id);

        if (FOG_ShouldShowSelections())
            FOG_SetSelections(true);
        else
            FOG_SetSelections(false);
    }
}
class FOG_NVG_Togglable_Base extends FOG_NVG_Base
{
    bool m_IsReady;

    void FOG_NVG_Togglable_Base()
    {
        RegisterNetSyncVariableBool("m_IsReady");
    }

    void SetReadyGoggles(bool state)
    {
        LoweredCheck();

        if (m_IsLowered)
            return;

        m_IsReady = state;

        float animPhase;
        if (state)
            animPhase = 0.7;
        else
            animPhase = 0.0;

        SetAnimationPhase("rotate", animPhase);
        SetSynchDirty();
    }

    protected bool IsReadyPhase(float phase)
    {
        return (phase > 0.35 && phase < 0.95);
    }

    void ToggleReadyGoggles()
    {
        LoweredCheck();

        if (m_IsLowered)
            return;

        float phase = GetAnimationPhase("rotate");
        bool readyByPhase = IsReadyPhase(phase);

        if (m_IsReady != readyByPhase)
            m_IsReady = readyByPhase;

        SetReadyGoggles(!readyByPhase);
    }

    override void LoweredCheck()
    {
        float phase = GetAnimationPhase("rotate");
        bool loweredByPhase = (phase == 1.0);

        if (m_IsLowered != loweredByPhase)
            m_IsLowered = loweredByPhase;
    }

    override void RotateGoggles(bool state)
    {
        float animPhase;
        if (state)
        {
            if (m_IsReady)
                animPhase = 0.7;
            else
                animPhase = 0.0;
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
        if (m_Strap && m_Strap.GetInventory().GetCurrentAttachmentSlotInfo(slot_id, slot_name) && PlayerBase.CastTo(player, m_Strap.GetHierarchyParent()))
            player.SetNVGLowered(m_IsLowered);

        if (GetCompEM())
        {
            if (!state && GetCompEM().CanWork())
                GetCompEM().SwitchOn();
            else
                GetCompEM().SwitchOff();
        }
    }
}

// Non-togglable FOG NVGs
class FOG_PVS31: FOG_NVG_Base {}
class FOG_ANVIS9_Base extends FOG_NVG_Base
{
    override bool CanPutAsAttachment(EntityAI parent)
    {
        if (parent && parent.IsKindOf("FOG_Helmet_HGU56_ColorBase"))
            return super.CanPutAsAttachment(parent);

        return false;
    }
}
class FOG_GPNVG_Base: FOG_NVG_Base {}
class FOG_GPNVG_GSGM_Base: FOG_NVG_Base {}
class FOG_GPNVG_Elite: FOG_NVG_Base {}

// Togglable FOG NVGs
class FOG_PVS31A_Base: FOG_NVG_Togglable_Base {}
class FOG_UAPNVG_Base: FOG_NVG_Togglable_Base {}
