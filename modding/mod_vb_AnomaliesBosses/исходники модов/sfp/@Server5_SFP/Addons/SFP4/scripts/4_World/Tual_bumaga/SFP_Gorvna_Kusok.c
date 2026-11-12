class SFP_Gorvna_Kusok : House
{
    static const int DELETE_AFTER_MS = 10 * 60 * 1000; // 10 minutes
    static const int STEP_EFFECT_MS  = 10 * 1000;      // 10 seconds

    protected ref EffectParticle m_FliesEff;
    protected int m_FliesIndex = -1;
    protected ref EffectSound m_FliesSound;

    protected ref map<Object, int> m_SFP_LastStepMS;

    void SFP_Gorvna_Kusok()
    {
        m_SFP_LastStepMS = new map<Object, int>;
    }

    override void EEInit()
    {
        super.EEInit();

        // start flies on clients
        if (GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
            SFP_StartFlies();
        }

        // schedule delete on server
        if (GetGame().IsServer())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_DeleteSelf, DELETE_AFTER_MS, false);
        }
    }

    protected void SFP_StartFlies()
    {
        if (!m_FliesEff)
            m_FliesEff = new EffSwarmingFlies();

        m_FliesEff.SetDecalOwner(this);
        m_FliesEff.SetAutodestroy(false);

        if (!SEffectManager.IsEffectExist(m_FliesIndex))
            m_FliesIndex = SEffectManager.PlayOnObject(m_FliesEff, this, "0 0.25 0");

        if (!m_FliesSound)
            PlaySoundSetLoop(m_FliesSound, "Flies_SoundSet", 1.0, 1.0);
    }

    protected void SFP_StopFlies()
    {
        if (m_FliesEff)
            SEffectManager.DestroyEffect(m_FliesEff);

        StopSoundSet(m_FliesSound);

        m_FliesEff = null;
        m_FliesIndex = -1;
    }

    protected void SFP_DeleteSelf()
    {
        if (GetGame() && GetGame().IsServer())
            GetGame().ObjectDelete(this);
    }

    override void EEDelete(EntityAI parent)
    {
        // stop local FX
        if (GetGame().IsClient() || !GetGame().IsMultiplayer())
            SFP_StopFlies();

        super.EEDelete(parent);
    }

    override void EOnContact(IEntity other, Contact extra)
    {
        super.EOnContact(other, extra);

        if (!GetGame().IsServer())
            return;

        Object obj = Object.Cast(other);
        if (!obj)
            return;

        // only players / infected
        if (!obj.IsInherited(PlayerBase) && !obj.IsInherited(ZombieBase))
            return;

        int now = GetGame().GetTime();

        int last = 0;
        if (m_SFP_LastStepMS && m_SFP_LastStepMS.Find(obj, last))
        {
            // don't spam while effect is active
            if ((now - last) < STEP_EFFECT_MS)
                return;
        }

        m_SFP_LastStepMS.Set(obj, now);

        Param1<int> p = new Param1<int>(STEP_EFFECT_MS);
        GetGame().RPCSingleParam(obj, SFP_TualBumagaRPCs.SFP_RPC_TB_PLAY_FLIES_TEMP, p, true);
    }
}
