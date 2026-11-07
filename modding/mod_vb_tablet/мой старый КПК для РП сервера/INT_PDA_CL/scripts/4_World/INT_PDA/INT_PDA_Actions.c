// INT_PDA — «Заглянуть в КПК»: a client-local instant action (like vanilla ActionTogglePlaceObject): nothing goes to
// the server, Start() opens the PDA window through INT_PDA_Events. Shown only while the PDA in hands works (energy
// manager state is synced by the engine, so the condition is the same on both sides).

class INT_PDA_ActionOpen : ActionSingleUseBase
{
	void INT_PDA_ActionOpen()
	{
		m_Text = "#STR_INT_PDA_ACTION_OPEN";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool IsLocal()
	{
		return true;
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		INT_PDA pda = INT_PDA.Cast(item);
		return pda && pda.IsWorkingPda();
	}

	override void Start(ActionData action_data)
	{
		super.Start(action_data);
		if (GetGame().IsDedicatedServer())
			return;
		INT_PDA_Events.OpenRequest(action_data.m_MainItem);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(INT_PDA_ActionOpen);
	}
}
