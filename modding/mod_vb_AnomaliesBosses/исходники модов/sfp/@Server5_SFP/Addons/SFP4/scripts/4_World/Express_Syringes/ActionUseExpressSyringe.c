// ================================================================
// Express Syringes - custom single-use action
// ================================================================
//  - Tablet consume animation (ActionOnce.EatTablet*.Once) by using
//    DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET / CMD_ACTIONFB_EAT_TABLET
//  - Can be used with mask on (ignores PlayerBase.CanEatAndDrink())
//  - Can be used while moving (action-mod, not full-body)
//  - Deletes the item after use (single-use)
//  - Plays a 2D (headphones) sound once locally

class ActionUseExpressSyringe : ActionSingleUseBase
{
	protected const string SFP_EXPRESS_HEART_SOUNDSET = "SFP_Ai2_heart_SoundSet";
	protected static int s_LastLocal2DSoundTime;
	protected const int DEFAULT_CONSUMED_QUANTITY = 1;

	void ActionUseExpressSyringe()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_EAT_TABLET;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_EAT_TABLET;
		m_Text = "#use";
	}

	int GetConsumedQuantity()
	{
		return DEFAULT_CONSUMED_QUANTITY;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINotRuinedAndEmpty();
		m_ConditionTarget 	= new CCTSelf();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		// Allow use even with masks (ignore CanEatAndDrink())
		ConsumeConditionData dta = new ConsumeConditionData(player, item);
		return player && item && player.CanConsumeFood(dta) && item.CanBeConsumed(dta);
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);

		PlayerBase player = action_data.m_Player;
		if (!player)
			return;

		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		// Play only for the local player (headphones)
		if (player != PlayerBase.Cast(GetGame().GetPlayer()))
			return;

		// Anti-double-play guard (prediction/network)
		int now = GetGame().GetTime();
		if (now - s_LastLocal2DSoundTime < 200)
			return;
		s_LastLocal2DSoundTime = now;

		PlayLocal2DSound(SFP_EXPRESS_HEART_SOUNDSET);
	}

	protected void PlayLocal2DSound(string soundSet)
	{
		#ifndef SERVER
		SoundParams soundParams = new SoundParams(soundSet);
		if (!soundParams || !soundParams.IsValid())
			return;

		SoundObjectBuilder sob = new SoundObjectBuilder(soundParams);
		SoundObject so = sob.BuildSoundObject();
		if (!so)
			return;

		so.SetKind(WaveKind.WAVEEFFECTEX);

		AbstractWave wave = GetGame().GetSoundScene().Play2D(so, sob);
		if (wave)
		{
			wave.Loop(false);
			wave.Play();
		}
		#endif
	}

	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);

		PlayerBase player = action_data.m_Player;
		if (player && action_data.m_MainItem)
		{
			PlayerConsumeData consumeData = new PlayerConsumeData();
			consumeData.m_Type	= EConsumeType.ITEM_SINGLE_TIME;
			consumeData.m_Amount = GetConsumedQuantity();
			consumeData.m_Source = action_data.m_MainItem;
			consumeData.m_Agents = player.GetBloodyHandsPenaltyAgents();

			player.Consume(consumeData);
		}
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		// IMPORTANT:
		// Do NOT delete the item in OnExecuteServer.
		// If we remove it while the action is still blending out,
		// the client often plays a short "hands change" / "item removed" animation snippet.
		// Setting quantity to 0 at the end lets the engine remove it cleanly.
		ItemBase item = action_data.m_MainItem;
		if (item)
		{
			if (item.HasQuantity())
				item.SetQuantity(0);
			else
				item.Delete();
		}
	}
}
