class ActionSFPBandageSelfRunCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float effectivity 		= m_ActionData.m_MainItem.GetBandagingEffectivity();
		float adjustedTimeSpent = 0;

		if (effectivity > 0)
			adjustedTimeSpent = UATimeSpent.BANDAGE / effectivity;

		m_ActionData.m_ActionComponent = new CAContinuousRepeat(adjustedTimeSpent);
	}
}

class ActionSFPBandageSelfRun : ActionContinuousBase
{
	void ActionSFPBandageSelfRun()
	{
		m_CallbackClass 	= ActionSFPBandageSelfRunCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF; // upper-body, allows movement
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF;  // prone fallback animation
		m_FullBody 			= false;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#treat_wound";
	}

	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTSelf();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player && player.IsBleeding();
	}

	protected void ApplyBandageNoAgents(ItemBase item, PlayerBase player)
	{
		if (player.GetBleedingManagerServer())
			player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(item);

		// no agents transmission from this bandage

		if (item.HasQuantity())
			item.AddQuantity(-1, true);
		else
			item.Delete();
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		SFP_BandageDressing_soviet bandage = SFP_BandageDressing_soviet.Cast(action_data.m_MainItem);
		if (bandage)
			bandage.SetPackHidden(true);
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		SFP_BandageDressing_soviet bandage = SFP_BandageDressing_soviet.Cast(action_data.m_MainItem);
		if (bandage)
			bandage.SetPackHidden(true);
	}

	override void OnEndServer(ActionData action_data)
	{
		SFP_BandageDressing_soviet bandage = SFP_BandageDressing_soviet.Cast(action_data.m_MainItem);
		if (bandage)
			bandage.SetPackHidden(false);

		super.OnEndServer(action_data);
	}

	override void OnEndClient(ActionData action_data)
	{
		SFP_BandageDressing_soviet bandage = SFP_BandageDressing_soviet.Cast(action_data.m_MainItem);
		if (bandage)
			bandage.SetPackHidden(false);

		super.OnEndClient(action_data);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{	
		PlayerBase target = PlayerBase.Cast(action_data.m_Player);
		if (action_data.m_MainItem && target)
			ApplyBandageNoAgents(action_data.m_MainItem, target);
	}
}
