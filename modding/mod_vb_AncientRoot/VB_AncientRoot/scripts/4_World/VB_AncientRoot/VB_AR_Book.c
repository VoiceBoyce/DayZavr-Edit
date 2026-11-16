// «Книга древних» (ТЗ_сервер_18.md, ветка 2в; решения пользователя 2026-11-16): предмет и действие «Читать».
// В руках ЛКМ «Читать» — открывается окно-разворот (5_Mission/VB_AR_BookMenu.c, меню MENU_ID через
// MissionGameplay.CreateScriptedMenu — как ванильные заметки). Тексты — VB_AR_BookText.c. Получение — крафт DRL 18 (позже).

class VB_AR_Book extends Inventory_Base
{
	static const int MENU_ID = 7451101;

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionReadBook);
	}

	// клиент: открыть окно, если сейчас не открыто другое меню
	static void VB_OpenMenu()
	{
		if (g_Game.IsDedicatedServer())
			return;
		UIManager ui = g_Game.GetUIManager();
		if (!ui || ui.GetMenu())
			return;
		ui.EnterScriptedMenu(MENU_ID, null);
	}
}

class VB_ActionReadBook : ActionSingleUseBase
{
	void VB_ActionReadBook()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Читать";
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return VB_AR_Book.Cast(item) != null;
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		VB_AR_Book.VB_OpenMenu();
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionReadBook);
	}
}
