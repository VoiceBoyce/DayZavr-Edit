modded class ZombieBase
{
    protected ref EffectParticle m_SFP_TB_FliesEff;
    protected ref EffectSound m_SFP_TB_FliesSound;
    protected ref Timer m_SFP_TB_FliesTimer;

    void SFP_TB_PlayTempFlies(int duration_ms)
    {
        SFP_TB_StopTempFlies();

        m_SFP_TB_FliesEff = new EffSwarmingFlies();
        m_SFP_TB_FliesEff.SetDecalOwner(this);
        m_SFP_TB_FliesEff.SetAutodestroy(false);

        SEffectManager.PlayOnObject(m_SFP_TB_FliesEff, this, "0 0.25 0");
        PlaySoundSetLoop(m_SFP_TB_FliesSound, "Flies_SoundSet", 1.0, 1.0);

        if (duration_ms > 0)
        {
            if (!m_SFP_TB_FliesTimer)
                m_SFP_TB_FliesTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

            m_SFP_TB_FliesTimer.Run(duration_ms / 1000.0, this, "SFP_TB_StopTempFlies");
        }
    }

    void SFP_TB_StopTempFlies()
    {
        if (m_SFP_TB_FliesTimer)
            m_SFP_TB_FliesTimer.Stop();

        if (m_SFP_TB_FliesEff)
            SEffectManager.DestroyEffect(m_SFP_TB_FliesEff);

        StopSoundSet(m_SFP_TB_FliesSound);

        m_SFP_TB_FliesEff = null;
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        switch (rpc_type)
        {
            case SFP_TualBumagaRPCs.SFP_RPC_TB_PLAY_FLIES_TEMP:
            {
                Param1<int> pFlies = new Param1<int>(0);
                if (ctx.Read(pFlies))
                    SFP_TB_PlayTempFlies(pFlies.param1);
            }
            break;
        }
    }

    override void EEDelete(EntityAI parent)
    {
        SFP_TB_StopTempFlies();
        super.EEDelete(parent);
    }
}
