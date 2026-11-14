// INT Quests — «Поговорить» on a quest giver (spec.md §4.3). One interact action registered on the PLAYER: the
// interact input also collects player actions while aiming at a target, so it serves survivor NPCs and any static
// object (HouseNoDestruct classes) without new config classes. Since 1.30.164200 the server's CCTCursor checks only
// the origin distance, so ActionCondition re-checks on BOTH sides that the target is in this side's giver registry.
// The action only opens the giver window on the client; taking/turning in quests goes through RPCs the server
// validates on its own (distance, giver, requirements).

class ActionINTQuestTalk : ActionInteractBase
{
	void ActionINTQuestTalk()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_INT_QST_ACT_TALK";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.REPAIR);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;
		Object o = target.GetObject();
		if (!o)
			return false;
		return INT_QST_GiverReg.IsGiver(o);
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		if (!action_data.m_Target)
			return;
		string key = INT_QST_GiverReg.KeyOf(action_data.m_Target.GetObject());
		if (key != "")
			INT_QST_UiBridge.Get().OpenGiver(key);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionINTQuestTalk);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionINTQuestTalk, InputActionMap);
	}

	// A quest giver NPC cannot be tied up and the infected ignore it (pattern of INT_Trader, live on 1.30).
	// Real players have an identity; only identity-less survivors can be givers (cheap early exit).
	override bool CanBeRestrained()
	{
		if (!GetIdentity() && INT_QST_GiverReg.IsGiver(this))
			return false;
		return super.CanBeRestrained();
	}

	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (!GetIdentity() && INT_QST_GiverReg.IsGiver(this))
			return false;
		return super.CanBeTargetedByAI(ai);
	}
}
