// VB_MiningResources: действия игрока. Результаты действий считает серверная часть
// (VB_MiningResources_Server), здесь только условия, время и анимации, 1:1 со старым модом.

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_MR_ActionMine);
		actions.Insert(VB_MR_ActionPan);
		actions.Insert(VB_MR_ActionPress);
		actions.Insert(VB_MR_ActionDismantlePress);
	}
}

// ---------------------------------------------------------------- добыча из жилы
class VB_MR_ActionMineCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = 10;
		VB_MR_Vein_Base vein = VB_MR_Vein_Base.Cast(m_ActionData.m_Target.GetObject());
		if (vein)
			time = vein.m_VB_MR_MineTime;

		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
}

class VB_MR_ActionMine : ActionContinuousBase
{
	void VB_MR_ActionMine()
	{
		m_CallbackClass = VB_MR_ActionMineCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#STR_VB_MR_ACT_MINE";
	}

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;

		if (!item || !item.m_VB_MR_IsMineTool)
			return false;

		return VB_MR_Vein_Base.Cast(target.GetObject()) != null;
	}
}

// ---------------------------------------------------------------- промывка сковородой
class VB_MR_ActionPanCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = 10;
		FryingPan pan = FryingPan.Cast(m_ActionData.m_MainItem);
		if (pan)
			time = pan.m_VB_MR_PanTime;

		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
}

class VB_MR_ActionPan : ActionContinuousBase
{
	void VB_MR_ActionPan()
	{
		m_CallbackClass = VB_MR_ActionPanCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_LockTargetOnUse = false;
		m_Text = "#STR_VB_MR_ACT_PAN";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		// пресная вода, как в старом моде (CCTWaterSurface в 1.30 помечен устаревшим, но работает;
		// замена на CCTLiquid — в этапе исправлений, после проверки у воды)
		m_ConditionTarget = new CCTWaterSurface(UAMaxDistances.DEFAULT, UAWaterType.FRESH);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}
}

// ---------------------------------------------------------------- пресс: «Спрессовать»
class VB_MR_ActionPress : ActionInteractBase
{
	void VB_MR_ActionPress()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.CloseDoors;
		m_Text = "#STR_VB_MR_ACT_PRESS";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_MR_Press press = VB_MR_Press.Cast(target.GetObject());
		if (!press)
			return false;

		return press.IsInventoryVisible() && press.VB_MR_HasFullStack();
	}
}

// ---------------------------------------------------------------- пресс: разборка плоскогубцами
class VB_MR_ActionDismantlePressCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DECONSTRUCT);
	}
}

class VB_MR_ActionDismantlePress : ActionContinuousBase
{
	void VB_MR_ActionDismantlePress()
	{
		m_CallbackClass = VB_MR_ActionDismantlePressCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.BASEBUILDING);
	}

	override string GetText()
	{
		return "#STR_VB_MR_ACT_DISMANTLE";
	}

	// разбирается только пустой пресс
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_MR_Press press = VB_MR_Press.Cast(target.GetObject());
		return press && press.IsEmpty();
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		VB_MR_Press press = VB_MR_Press.Cast(action_data.m_Target.GetObject());
		if (!press)
			return;

		vector pos = action_data.m_Player.GetPosition();
		action_data.m_MainItem.DecreaseHealth(5, false);
		GetGame().CreateObject("VB_MR_Press_Kit", pos);
		press.Delete();
	}
}
