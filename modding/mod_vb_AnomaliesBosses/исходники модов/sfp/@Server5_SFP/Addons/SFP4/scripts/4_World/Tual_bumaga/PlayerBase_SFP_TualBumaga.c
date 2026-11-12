modded class PlayerBase
{
    // cooldown (ms since mission start)
    protected int m_SFP_TB_LastPoopTimeMS;

    // temporary flies
    protected ref EffectParticle m_SFP_TB_FliesEff;
    protected int m_SFP_TB_FliesIndex = -1;
    protected ref EffectSound m_SFP_TB_FliesSound;
    protected ref Timer m_SFP_TB_FliesTimer;

    // vomit (action)
    protected ref EffectParticle m_SFP_TB_VomitEff;
    protected int m_SFP_TB_VomitIndex = -1;
    protected ref Timer m_SFP_TB_VomitTimer;

    // movement lock failsafe
    protected ref Timer m_SFP_TB_MoveLockTimer;
    protected bool m_SFP_TB_IsMoveLocked;
	protected vector m_SFP_TB_MoveLockPos;

    bool SFP_TB_CanPoop()
    {
        // allow immediately until first successful use (m_SFP_TB_LastPoopTimeMS defaults to 0)
        if (m_SFP_TB_LastPoopTimeMS <= 0)
            return true;

        return (GetGame().GetTime() - m_SFP_TB_LastPoopTimeMS) >= 60000;
    }

    void SFP_TB_SetLastPoopTimeMS(int time_ms)
    {
        m_SFP_TB_LastPoopTimeMS = time_ms;
    }

    protected int SFP_TB_GetBestBoneIndex(string preferredBone)
    {
        int idx = GetBoneIndexByName(preferredBone);
        if (idx == -1 && preferredBone == "pelvis")
            idx = GetBoneIndexByName("Pelvis");
        if (idx == -1)
            idx = GetBoneIndexByName("Spine2");
        if (idx == -1)
            idx = GetBoneIndexByName("Spine");
        return idx;
    }

    //============================================================
    //  VOMIT FX
    //============================================================
    void SFP_TB_StartVomit(int duration_ms)
    {
        SFP_TB_StopVomit();

        int boneIdx = SFP_TB_GetBestBoneIndex("pelvis");

        m_SFP_TB_VomitEff = new EffVomit();
        m_SFP_TB_VomitEff.SetDecalOwner(this);
        m_SFP_TB_VomitEff.SetAutodestroy(false);

        // Rotate 180 degrees (mostly fixes direction on some rigs)
        m_SFP_TB_VomitIndex = SEffectManager.PlayOnObject(m_SFP_TB_VomitEff, this, "0 0 0", "180 0 0");
        Particle p = m_SFP_TB_VomitEff.GetParticle();
        if (p && boneIdx != -1)
        {
            p.SetOrientation("180 0 0");
            AddChild(p, boneIdx);
        }

        if (duration_ms > 0)
        {
            if (!m_SFP_TB_VomitTimer)
                m_SFP_TB_VomitTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

            m_SFP_TB_VomitTimer.Run(duration_ms / 1000.0, this, "SFP_TB_StopVomit");
        }
    }

    //============================================================
    //  MOVEMENT LOCK (client-side)
    //============================================================
    void SFP_TB_LockMovement(int max_ms = 20000)
    {
		m_SFP_TB_IsMoveLocked = true;
		m_SFP_TB_MoveLockPos = GetPosition();

		// Client-side input clamp (server has no input controller)
		if (!GetGame().IsDedicatedServer())
		{
			HumanInputController hic = GetInputController();
			if (hic)
			{
				// ENABLED is persistent (ONE_FRAME can allow a "slip" before we reapply)
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 0);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
			}
		}

        // optional failsafe unlock
        if (max_ms > 0)
        {
            if (!m_SFP_TB_MoveLockTimer)
                m_SFP_TB_MoveLockTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

            m_SFP_TB_MoveLockTimer.Stop();
            m_SFP_TB_MoveLockTimer.Run(max_ms / 1000.0, this, "SFP_TB_UnlockMovement");
        }
    }

    void SFP_TB_UnlockMovement()
    {
        if (m_SFP_TB_MoveLockTimer)
            m_SFP_TB_MoveLockTimer.Stop();

        m_SFP_TB_IsMoveLocked = false;

		if (!GetGame().IsDedicatedServer())
		{
			HumanInputController hic = GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.DISABLED, 0);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.DISABLED, 0);
			}
		}
    }

    // Re-apply lock each frame (reliable vs. internal resets)
	void SFP_TB_TickMovementLock()
    {
        if (!m_SFP_TB_IsMoveLocked)
            return;

		// Hard "stay in place" (prevents sliding). We only snap back if we drift.
		vector cur = GetPosition();
		if (vector.DistanceSq(cur, m_SFP_TB_MoveLockPos) > 0.0004) // ~2cm
		{
			SetPosition(m_SFP_TB_MoveLockPos);
		}

		// Client: keep input clamped
		if (!GetGame().IsDedicatedServer())
		{
			HumanInputController hic = GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 0);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
			}
		}
    }


    void SFP_TB_StopVomit()
    {
        if (m_SFP_TB_VomitTimer)
            m_SFP_TB_VomitTimer.Stop();

        if (m_SFP_TB_VomitEff)
            SEffectManager.DestroyEffect(m_SFP_TB_VomitEff);

        m_SFP_TB_VomitEff = null;
        m_SFP_TB_VomitIndex = -1;
    }

    //============================================================
    //  FLIES (temp on alive character)
    //============================================================
    void SFP_TB_PlayTempFlies(int duration_ms)
    {
        SFP_TB_StopTempFlies();

        int boneIdx = SFP_TB_GetBestBoneIndex("Spine2");

        m_SFP_TB_FliesEff = new EffSwarmingFlies();
        m_SFP_TB_FliesEff.SetDecalOwner(this);
        m_SFP_TB_FliesEff.SetAutodestroy(false);

        m_SFP_TB_FliesIndex = SEffectManager.PlayOnObject(m_SFP_TB_FliesEff, this, "0 0.25 0");
        Particle p = m_SFP_TB_FliesEff.GetParticle();
        if (p && boneIdx != -1)
        {
            AddChild(p, boneIdx);
        }

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
        m_SFP_TB_FliesIndex = -1;
    }

    //============================================================
    //  RPC
    //============================================================
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        switch (rpc_type)
        {
            case SFP_TualBumagaRPCs.SFP_RPC_TB_START_VOMIT:
            {
                Param1<int> pStart = new Param1<int>(0);
                if (ctx.Read(pStart))
                    SFP_TB_StartVomit(pStart.param1);
            }
            break;

            case SFP_TualBumagaRPCs.SFP_RPC_TB_STOP_VOMIT:
            {
                SFP_TB_StopVomit();
            }
            break;

            case SFP_TualBumagaRPCs.SFP_RPC_TB_PLAY_FLIES_TEMP:
            {
                Param1<int> pFlies = new Param1<int>(0);
                if (ctx.Read(pFlies))
                    SFP_TB_PlayTempFlies(pFlies.param1);
            }
            break;

            case SFP_TualBumagaRPCs.SFP_RPC_TB_SYNC_COOLDOWN:
            {
                Param1<int> pCd = new Param1<int>(0);
                if (ctx.Read(pCd))
                    SFP_TB_SetLastPoopTimeMS(pCd.param1);
            }
            break;
        }
    }

    override void EEDelete(EntityAI parent)
    {
        SFP_TB_StopVomit();
        SFP_TB_StopTempFlies();
        SFP_TB_UnlockMovement();
        super.EEDelete(parent);
    }
}
