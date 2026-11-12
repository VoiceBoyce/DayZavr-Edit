// st_bolt_fidov
// Throw 1 bolt from a stacked SFP_st_bolt01 without removing the stack from hands.

class STThrowBoltFidovReciveData : ActionReciveData
{
	float m_PitchY; // camera direction Y component sent from client
}

class STThrowBoltFidovActionData : ActionData
{
	float m_PitchY; // filled from recive data
}

class ActionSTThrowBoltFidovCB : ActionSingleUseBaseCB
{
	override void EndActionComponent()
	{
		// Ensure the command ends properly (same pattern as ActionDropItemCB)
		SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		m_ActionData.m_State = UA_FINISHED;
	}
};

class ActionSTThrowBoltFidov : ActionSingleUseBase
{
	// Tunables
	protected const float ST_BOLT_THROW_FORCE        = 55.0; // base forward impulse
	protected const float ST_BOLT_DIR_UP_BIAS        = 0.08; // small constant lift (helps not hit ground instantly)
	protected const float ST_BOLT_PITCH_FORCE_SCALE  = 0.85; // how much camera pitch affects vertical impulse
	protected const float ST_BOLT_PITCH_MIN          = -0.95; // allow lower throws (closer to feet) when looking down
	protected const float ST_BOLT_PITCH_MAX          = 0.98;  // allow higher throws when looking up
	protected const float ST_BOLT_SPAWN_FWD          = 0.25; // forward offset from spawn point
	protected const float ST_BOLT_SPAWN_UP           = 0.05; // upward offset from spawn point

	// Dedicated server spawn offsets (stable, because dedicated often has no reliable hand bone pose)
	protected const float ST_BOLT_SPAWN_DEDI_FWD     = 0.40; // forward from player
	protected const float ST_BOLT_SPAWN_DEDI_UP      = 1.55; // height (closer to hand)
	protected const float ST_BOLT_SPAWN_DEDI_SIDE    = 0.15; // +right / -left

	protected const float ST_BOLT_SPAWN_BASE_UP      = 1.15; // fallback if transform/bone is unavailable

	void ActionSTThrowBoltFidov()
	{
		// Reuse the "drop from hands" animation as a quick toss.
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS;
		m_CommandUIDProne   = DayZPlayerConstants.CMD_ACTIONFB_DROPITEM_HANDS;
		m_CallbackClass     = ActionSTThrowBoltFidovCB;
		m_Text              = "Кинуть болт";
	}

	override ActionData CreateActionData()
	{
		return new STThrowBoltFidovActionData;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem   = new CCINone();
		m_ConditionTarget = new CCTNone();
	}

	override typename GetInputType()
	{
		return DefaultActionInput;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool CanBeUsedThrowing()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool UseAcknowledgment()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		HumanCommandMove hcm = player.GetCommand_Move();
		if (hcm && hcm.IsChangingStance())
			return false;

		SFP_st_bolt01 bolt;
		if (!Class.CastTo(bolt, item))
			return false;

		return bolt.GetQuantity() >= 1;
	}

	// Send camera pitch to server (dedicated server doesn't have a camera, so it must come from client)
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);

		float pitchY = 0.0;
		PlayerBase player = action_data.m_Player;
		if (player)
		{
			vector camPos, camDir, camRot;
			player.GetCurrentCameraTransform(camPos, camDir, camRot);
			pitchY = camDir[1];
		}
		ctx.Write(pitchY);
	}

	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
		{
			action_recive_data = new STThrowBoltFidovReciveData;
		}

		super.ReadFromContext(ctx, action_recive_data);

		float pitchY;
		if (!ctx.Read(pitchY))
			return false;

		STThrowBoltFidovReciveData rd = STThrowBoltFidovReciveData.Cast(action_recive_data);
		rd.m_PitchY = pitchY;

		return true;
	}

	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		STThrowBoltFidovReciveData rd = STThrowBoltFidovReciveData.Cast(action_recive_data);
		STThrowBoltFidovActionData ad = STThrowBoltFidovActionData.Cast(action_data);

		if (rd && ad)
			ad.m_PitchY = rd.m_PitchY;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		SFP_st_bolt01 boltStack;

		if (!player)
			return;

		if (!Class.CastTo(boltStack, action_data.m_MainItem))
			return;

		if (boltStack.GetQuantity() < 1)
			return;

		if (player.IsPlacingServer())
			player.PlacingCancelServer();

		// Release any reservations (safe)
		ClearInventoryReservationEx(action_data);

		// Always throw forward relative to player's heading (no backwards throws if camera looks behind).
		vector headingDir = MiscGameplayFunctions.GetHeadingVector(player); // horizontal forward

		// Pitch comes from client (camera direction Y component)
		float pitch = 0.0;
		STThrowBoltFidovActionData ad = STThrowBoltFidovActionData.Cast(action_data);
		if (ad)
			pitch = ad.m_PitchY;
		pitch = Math.Clamp(pitch, ST_BOLT_PITCH_MIN, ST_BOLT_PITCH_MAX);

		// Spawn position:
		// - On dedicated server, hand bones are often NOT in the correct pose (no full animation), so bone-based spawn shifts sideways.
		//   Use a stable offset from player position.
		// - Otherwise (offline / local host), use hand bone for nicer visuals.
		vector spawnPos;
		if (GetGame().IsDedicatedServer())
		{
			// Use transform axes to get a stable spawn point on dedicated server.
			vector mtx[4];
			player.GetTransform(mtx); // [0]=right, [1]=up, [2]=forward, [3]=pos
			spawnPos = mtx[3] + (mtx[2] * ST_BOLT_SPAWN_DEDI_FWD) + (mtx[1] * ST_BOLT_SPAWN_DEDI_UP) + (mtx[0] * ST_BOLT_SPAWN_DEDI_SIDE);
		}
		else
		{
			int handBone = player.GetBoneIndexByName("RightHand_Dummy");
			if (handBone >= 0)
				spawnPos = player.GetBonePositionWS(handBone);
			else
			{
				spawnPos = player.GetPosition();
				spawnPos[1] = spawnPos[1] + ST_BOLT_SPAWN_BASE_UP;
			}
			// Extra small offsets for nicer visuals when the hand bone pose is available.
			spawnPos = spawnPos + (headingDir * ST_BOLT_SPAWN_FWD);
			spawnPos[1] = spawnPos[1] + ST_BOLT_SPAWN_UP;
		}

		SFP_st_bolt01 thrownBolt = SFP_st_bolt01.Cast(GetGame().CreateObjectEx(boltStack.GetType(), spawnPos, ECE_CREATEPHYSICS | ECE_NOLIFETIME));
		if (!thrownBolt)
			return;

		thrownBolt.SetQuantity(1, false, true);
		thrownBolt.SetHealth01("", "", boltStack.GetHealth01());
		thrownBolt.SetOrientation(player.GetOrientation());
		thrownBolt.ST_SetSpawnedAsThrown();

		vector force = (headingDir * ST_BOLT_THROW_FORCE) + GetVelocity(player);
		force[1] = force[1] + (pitch * ST_BOLT_THROW_FORCE * ST_BOLT_PITCH_FORCE_SCALE) + (ST_BOLT_DIR_UP_BIAS * ST_BOLT_THROW_FORCE);
		thrownBolt.ThrowPhysically(player, force, false);

		// Reduce the stack by 1 (item stays in hands)
		boltStack.AddQuantity(-1, false, true);
	}
}
