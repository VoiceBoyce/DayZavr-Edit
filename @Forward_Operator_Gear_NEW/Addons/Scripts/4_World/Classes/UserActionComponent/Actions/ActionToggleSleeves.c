class ActionToggleSleevesCB extends ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime( 0 );
    }
}

class ActionToggleSleeves: ActionContinuousBase
{
	void ActionToggleSleeves()
	{
		m_CallbackClass = ActionToggleSleevesCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_SEARCHINVENTORY;
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool CanBeUsedInVehicle()
    {
        return true;
    }

	override typename GetInputType()
	{
		return ToggleSleevesActionInput;
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool UseMainItem()
	{
		return false;
	}

	#ifndef SERVER
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		Toggleable_Sleeves_Shirt_Base shirt = Toggleable_Sleeves_Shirt_Base.Cast(player.FindAttachmentBySlotName("Body"));
		if (!shirt) {
			return false;
		}

		return true;
	}
	#endif

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		Toggleable_Sleeves_Shirt_Base shirt = Toggleable_Sleeves_Shirt_Base.Cast(action_data.m_Player.FindAttachmentBySlotName("Body"));
		
		if (!shirt) {
			return;
		}

		int m_SleevesState = shirt.GetSleevesState();

		//Checkered Shirt Has 2 States
		if (shirt.GetSleevesAmount() == 2) {
			if (m_SleevesState != 1) {
				shirt.SetSleevesState(1);
			}
			else {
				shirt.SetSleevesState(0);
			}
			return;
		}

		if (m_SleevesState != 2) {
			shirt.SetSleevesState(m_SleevesState + 1);
		}
		else {
			shirt.SetSleevesState(0);
		}
	}
}

class ToggleSleevesActionInput: DefaultActionInput
{
	ref ActionTarget target_new;

	void ToggleSleevesActionInput(PlayerBase player)
	{
		SetInput("UAToggleSleeves");
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
	}

	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if (player) 
		{
			Toggleable_Sleeves_Shirt_Base shirt = Toggleable_Sleeves_Shirt_Base.Cast(player.FindAttachmentBySlotName("Body"));
			if (shirt)
			{
				target_new = new ActionTarget(shirt, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
}