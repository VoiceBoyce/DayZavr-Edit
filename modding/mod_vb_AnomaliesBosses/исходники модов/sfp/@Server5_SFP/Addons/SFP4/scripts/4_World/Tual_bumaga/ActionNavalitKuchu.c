class ActionNavalitKuchuCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(10.0);
    }
};

class ActionNavalitKuchu : ActionContinuousBase
{
    void ActionNavalitKuchu()
    {
        m_CallbackClass = ActionNavalitKuchuCB;

        // Thermometer animation:
        // - Erect/Crouch uses ACTIONMOD
        // - Prone uses fullbody
        // NOTE: CMD_ACTIONFB_TAKETEMPSELF is prone-only in vanilla, using it in erect cancels instantly.
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF;
        m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF;

        // We lock movement manually (client-side) for reliability.
        m_FullBody = false;

        m_SpecialtyWeight = 0;
        m_Text = "Навалить кучу";
    }

    // Standard continuous action input (hold key)
    override typename GetInputType()
    {
        return ContinuousDefaultActionInput;
    }

    // Allow correct prone handling (uses m_CommandUIDProne)
    override bool HasProneException()
    {
        return true;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTSelf;
    }

    override bool HasTarget()
    {
        return false;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!player)
            return false;

        return player.SFP_TB_CanPoop();
    }

    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);

		// Server-side lock (authoritative position)
		if (action_data && action_data.m_Player)
		{
			action_data.m_Player.SFP_TB_LockMovement(20000);
		}

        // start vomit FX for everyone (10 sec)
        Param1<int> p = new Param1<int>(10000);
        GetGame().RPCSingleParam(action_data.m_Player, SFP_TualBumagaRPCs.SFP_RPC_TB_START_VOMIT, p, true);
    }

	override void OnUpdateServer(ActionData action_data)
	{
		super.OnUpdateServer(action_data);
		if (action_data && action_data.m_Player)
		{
			action_data.m_Player.SFP_TB_TickMovementLock();
		}
	}

    override void OnStartClient(ActionData action_data)
    {
        super.OnStartClient(action_data);
        if (action_data && action_data.m_Player)
        {
            // Hard lock movement with a 20s failsafe (unlocks even if something interrupts incorrectly)
            action_data.m_Player.SFP_TB_LockMovement(20000);
        }
    }

    
    override void OnUpdateClient(ActionData action_data)
    {
        super.OnUpdateClient(action_data);

        // Keep movement locked even if other systems try to restore overrides each frame
        if (action_data && action_data.m_Player)
        {
            action_data.m_Player.SFP_TB_TickMovementLock();
        }
    }

override void OnEndClient(ActionData action_data)
    {
        if (action_data && action_data.m_Player)
        {
            action_data.m_Player.SFP_TB_UnlockMovement();
        }
        super.OnEndClient(action_data);
    }

    override void OnEndServer(ActionData action_data)
    {
        super.OnEndServer(action_data);

		// unlock on server as well
		if (action_data && action_data.m_Player)
		{
			action_data.m_Player.SFP_TB_UnlockMovement();
		}

        // stop vomit FX early (for cancel / finish)
        Param1<int> dummy = new Param1<int>(0);
        GetGame().RPCSingleParam(action_data.m_Player, SFP_TualBumagaRPCs.SFP_RPC_TB_STOP_VOMIT, dummy, true);
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);

        PlayerBase player = action_data.m_Player;
        if (!player)
            return;

        // Spawn "piece" at player's current position.
// IMPORTANT: do NOT force terrain SurfaceY here, otherwise if the player is sitting/standing on a vehicle
// the object ends up below the vehicle inside ground textures.
        vector pos = player.GetPosition();
        pos[1] = pos[1] + 0.02; // small lift for stable placement

        GetGame().CreateObjectEx("SFP_Gorvna_Kusok", pos, ECE_PLACE_ON_SURFACE);

        // 1 minute cooldown
        int now = GetGame().GetTime();
        player.SFP_TB_SetLastPoopTimeMS(now);

        Param1<int> pCd = new Param1<int>(now);
        GetGame().RPCSingleParam(player, SFP_TualBumagaRPCs.SFP_RPC_TB_SYNC_COOLDOWN, pCd, true);
    }
};
