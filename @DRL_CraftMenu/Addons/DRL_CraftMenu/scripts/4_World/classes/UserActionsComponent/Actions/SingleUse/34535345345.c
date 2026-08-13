class ActionDeconstructDRLWorkbench: ActionInteractBase
{
	void ActionDeconstructDRLWorkbench()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_HUDCursorIcon = CursorIcons.CloseHood;
	}

	override string GetText()
	{
		return "#STR_DRL_RE_START";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player) return false;
		if (!target) return false;
	
		DRL_Craft_WB wb = DRL_Craft_WB.Cast(target.GetObject());
		if (!wb) return false;
		
		if (wb.NeedsScrewdriverForDeconstruct())
		{
			ItemBase inHands = ItemBase.Cast(player.GetHumanInventory().GetEntityInHands());
			if (!inHands) return false;
			if (!inHands.IsKindOf("DRL_Craft_ScrewDriver"))
				return false;
		}
		
		int attCount = wb.GetInventory().AttachmentCount();
		if (wb.HasAnyCargo() || attCount != 0)
			return false;
	
		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		Object obj = action_data.m_Target.GetObject();
		if (!obj) return;
	
		vector pos = action_data.m_Player.GetPosition();
	
		if (obj.IsKindOf("DRL_Craft_WB9"))
			GetGame().CreateObject("DRL_Craft_Kit9", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB8"))
			GetGame().CreateObject("DRL_Craft_Kit8", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB7"))
			GetGame().CreateObject("DRL_Craft_Kit7", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB6"))
			GetGame().CreateObject("DRL_Craft_Kit6", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB5"))
			GetGame().CreateObject("DRL_Craft_Kit5", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB4"))
			GetGame().CreateObject("DRL_Craft_Kit4", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB3"))
			GetGame().CreateObject("DRL_Craft_Kit3", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB2"))
			GetGame().CreateObject("DRL_Craft_Kit2", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB1"))
			GetGame().CreateObject("DRL_Craft_Kit1", pos, false);
		else if (obj.IsKindOf("DRL_Craft_Garden"))
			GetGame().CreateObject("DRL_Craft_Garden_Kit", pos, false);
		else if (obj.IsKindOf("DRL_Craft_Cook"))
			GetGame().CreateObject("DRL_Craft_Cook_Kit", pos, false);
		else if (obj.IsKindOf("DRL_Craft_Build"))
			GetGame().CreateObject("DRL_Craft_Build_Kit", pos, false);
		else if (obj.IsKindOf("DRL_Craft_Workbench"))
			GetGame().CreateObject("DRL_Craft_Workbench_Kit", pos, false);
		else if (obj.IsKindOf("DRL_Craft_WB"))
			GetGame().CreateObject("DRL_Craft_Kit", pos, false);
	
		GetGame().ObjectDelete(obj);
	}
}