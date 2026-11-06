modded class ActionTurnOffWeaponFlashlight
{
	override string GetText()
	{
        ItemBase attachment;
		ItemBase neededItem = ItemBase.Cast(PlayerBase.Cast(GetGame().GetPlayer()).GetHumanInventory().GetEntityInHands());
		if (!neededItem)
			return "#switch_off";

		attachment = ItemBase.Cast(neededItem.FindAttachmentBySlotName("weaponFlashlight"));
		if (!attachment)
			attachment = ItemBase.Cast(neededItem.FindAttachmentBySlotName("pistolFlashlight"));

		Ri_LaserDesignator_Base lcu = Ri_LaserDesignator_Base.Cast(attachment);
		if (lcu && lcu.GetCompEM().IsWorking())
		{
			string tx;
			switch (lcu.MODE)
			{
			case modelaser.RayOnly:
				tx = ": R :";
				break;

			case modelaser.IrRay:
				tx = ": IR :";
				break;

			case modelaser.Flaslight:
				tx = ": F :";
				break;

			case modelaser.RayFlashlight:
				tx = ": RF :";
				break;

			case modelaser.AllOfff:
				tx = ": off :";
				break;

			default:
				break;
			}
                string ret = "#switch_off" + "                           " + tx;
                return ret;
        }

		return "#switch_off";
	}
}
modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
        super.RegisterActions(actions);
		actions.Insert(ActionTuneLaserModule);
    }
}

class ToggleLaserModule : DefaultActionInput
{
	ref ActionTarget target_new;

	void ToggleLaserModule(PlayerBase player)
	{
		SetInput("ChgMode");
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 90;
	}

	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		m_SelectAction = null;
		array<ActionBase_Basic> possibleActions;
		ActionBase action;
		int i;

		m_MainItem = null;
		if (player)
		{
            ItemBase attachment;
			ItemBase neededItem = ItemBase.Cast(PlayerBase.Cast(GetGame().GetPlayer()).GetHumanInventory().GetEntityInHands());
            if(neededItem!=NULL)
            	attachment = ItemBase.Cast(neededItem.FindAttachmentBySlotName("weaponFlashlight"));
				if(neededItem!=NULL && !attachment)
            	attachment = ItemBase.Cast(neededItem.FindAttachmentBySlotName("pistolFlashlight"));
            	Ri_LaserDesignator_Base lcu = Ri_LaserDesignator_Base.Cast(attachment);

			if (lcu)
			{
				target_new = new ActionTarget(lcu, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
			{
				ClearForcedTarget();
			}
		}

		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		if (target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possibleActions);
			if (possibleActions)
			{
				for (i = 0; i < possibleActions.Count(); i++)
				{
					action = ActionBase.Cast(possibleActions.Get(i));
					if (action.Can(player, target, m_MainItem, action_condition_mask))
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
};
class ActionTuneLaserModule : ActionInstantBase
{
	void ActionTuneLaserModule()
	{
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool UseMainItem()
	{
		return false;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override typename GetInputType()
	{
		return ToggleLaserModule;
	}

	// По умолчанию действие запрещено с поднятым оружием, лёжа и в наклоне.
	// А лазер светит как раз с поднятым оружием, поэтому режимы не переключались.
	override int GetStanceMask(PlayerBase player)
	{
		return DayZPlayerConstants.STANCEMASK_ALL;
	}

	override bool CanBeUsedRaised()
	{
		return true;
	}

	override bool CanBeUsedLeaning()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (!target)
			return false;

		Ri_LaserDesignator_Base lcu = Ri_LaserDesignator_Base.Cast(target.GetObject());
		return lcu && lcu.GetCompEM().IsWorking();
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		Ri_LaserDesignator_Base lcu = Ri_LaserDesignator_Base.Cast(action_data.m_Target.GetObject());
		if (lcu)
			lcu.NextMode();
	}
};