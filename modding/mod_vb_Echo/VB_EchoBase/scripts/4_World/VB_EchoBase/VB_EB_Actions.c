// Состояние сети показывает курсор действий (VB_EB_Info.c, scripts/5_Mission/VB_EchoBase/VB_EB_Cursor.c).

// «Разобрать» устройство обратно в набор: удерживать F 5 с (решение пользователя 11-20 — не мгновенно), ключ или
// отвёртка в руках, устройство пустое (ничего не висит, в отсеке пусто) и не разрушено (иначе разборка = бесплатный
// ремонт: новый набор ставит целое устройство). Набор — у ног игрока, энергия в буферах пропадает.
// Как ванильные ActionPackTent (удержание F на цели) и ActionDismantlePart (анимация разборки).
class ActionVB_EBDisassembleCB : ActionContinuousBaseCB
{
	static const float TIME = 5;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(TIME);
	}
};

class ActionVB_EBDisassemble : ActionContinuousBase
{
	void ActionVB_EBDisassemble()
	{
		m_CallbackClass = ActionVB_EBDisassembleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#STR_VB_EB_disassemble";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override bool UseMainItem()
	{
		return false;
	}

	static bool VB_HasTool(PlayerBase player)
	{
		if (!player)
			return false;
		EntityAI inHands = player.GetHumanInventory().GetEntityInHands();
		return inHands && !inHands.IsRuined() && (inHands.IsKindOf("Wrench") || inHands.IsKindOf("Screwdriver"));
	}

	// клиент (меню) и сервер (старт): предмет в руках, вложения, груз и разрушенность устройства — есть на обеих сторонах
	static bool VB_CanDisassemble(PlayerBase player, Object target)
	{
		VB_EB_Device d = VB_EB_Device.Cast(target);
		if (!d || d.GetHierarchyParent() || d.IsHologram() || d.IsRuined() || d.IsSetForDeletion())
			return false;
		if (d.VB_KitType() == "")
			return false;
		return VB_HasTool(player) && d.VB_IsEmpty();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return VB_CanDisassemble(player, target.GetObject());
	}

	// пока держат: сняли ключ, повесили вещь, устройство разрушили — прерваться
	override bool ActionConditionContinue(ActionData action_data)
	{
		return VB_CanDisassemble(action_data.m_Player, action_data.m_Target.GetObject());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		VB_EB_Device d = VB_EB_Device.Cast(action_data.m_Target.GetObject());
		if (!VB_CanDisassemble(action_data.m_Player, d))
			return;
		string kit = d.VB_KitType();
		GetGame().CreateObjectEx(kit, action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE);
		d.Delete();
	}
};

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionVB_EBDisassemble);
	}
};
