// Состояние сети показывает курсор действий (VB_EB_Info.c, scripts/5_Mission/VB_EchoBase/VB_EB_Cursor.c).

// «Разобрать» устройство обратно в набор (как ActionDeconstructDRLWorkbench): ключ или отвёртка в руках (решение 11-20),
// устройство пустое (без кристалла и вещей в отсеке); набор — у ног игрока, заряд буфера/накопителя теряется
class ActionVB_EBDisassemble : ActionInteractBase
{
	void ActionVB_EBDisassemble()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_VB_EB_disassemble";
	}

	static bool VB_HasTool(PlayerBase player)
	{
		if (!player)
			return false;
		EntityAI inHands = player.GetHumanInventory().GetEntityInHands();
		return inHands && !inHands.IsRuined() && (inHands.IsKindOf("Wrench") || inHands.IsKindOf("Screwdriver"));
	}

	// клиент (меню) и сервер (старт): предмет в руках, вложения и груз устройства — есть на обеих сторонах
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_EB_Device d = VB_EB_Device.Cast(target.GetObject());
		if (!d || d.GetHierarchyParent() || d.IsHologram())
			return false;
		if (d.VB_KitType() == "")
			return false;
		return VB_HasTool(player) && d.VB_IsEmpty();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		VB_EB_Device d = VB_EB_Device.Cast(action_data.m_Target.GetObject());
		if (!d || !d.VB_IsEmpty() || !VB_HasTool(action_data.m_Player))
			return;
		string kit = d.VB_KitType();
		if (kit == "")
			return;
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
