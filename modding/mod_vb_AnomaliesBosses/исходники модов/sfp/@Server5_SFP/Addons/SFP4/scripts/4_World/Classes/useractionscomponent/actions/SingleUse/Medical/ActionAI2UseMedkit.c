// AI2 Medkit - custom consume action
// Goal:
// - Useable while wearing a mask (mouth closed) => we intentionally do NOT check player.CanEatAndDrink()
// - Useable in movement (additive action)
// - Uses tablet consume animation (CMD_ACTIONMOD_EAT_TABLET)
// - Switches lid visuals while action is running
// - Plays local heartbeat sound once when started (only the user hears it)

class ActionAI2UseMedkit : ActionConsumeSingle
{
	protected EffectSound m_AI2_HeartSfx;

	void ActionAI2UseMedkit()
	{
		// Animation (ASI): ActionOnce.EatTabletCro.Once
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_EAT_TABLET;

		// Allow in all stances (incl. raised variants)
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ALL;

		m_Text				= "#use";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		// Intentionally skipping player.CanEatAndDrink() so it works with masks
		ConsumeConditionData dta = new ConsumeConditionData(player, item);
		return player.CanConsumeFood(dta) && item.CanBeConsumed(dta);
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		// Lid: open immediately locally for responsiveness
		SFP_AI2_medkit_standart mk;
		if (action_data && Class.CastTo(mk, action_data.m_MainItem))
		{
			mk.AI2_SetLidOpen_Local(true);
		}

		// Heartbeat sound: only for the local player ("in headphones")
		PlayerBase pb;
		if (action_data && Class.CastTo(pb, action_data.m_Player))
		{
			if (pb == PlayerBase.Cast(GetGame().GetPlayer()))
			{
				pb.PlaySoundSet(m_AI2_HeartSfx, "SFP_Ai2_heart_SoundSet", 0, 0);
			}
		}
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		SFP_AI2_medkit_standart mk;
		if (action_data && Class.CastTo(mk, action_data.m_MainItem))
		{
			mk.AI2_SetLidOpen_Server(true);
		}
	}

	override void OnEndClient(ActionData action_data)
	{
		super.OnEndClient(action_data);

		SFP_AI2_medkit_standart mk;
		if (action_data && Class.CastTo(mk, action_data.m_MainItem))
		{
			mk.AI2_SetLidOpen_Local(false);
		}
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		SFP_AI2_medkit_standart mk;
		if (action_data && Class.CastTo(mk, action_data.m_MainItem))
		{
			mk.AI2_SetLidOpen_Server(false);
		}
	}
}
