modded class MissionBase {

	// MARK: - Override methods

	// Добавить в основной метод задачу отображения худа достижений
	// 
	// - id: Идентификатор худа достижений
	override UIScriptedMenu CreateScriptedMenu(int id) {
		UIScriptedMenu menu = NULL;
		menu = super.CreateScriptedMenu(id);
		if (!menu) {
			switch (id) {
				case RF_ESCAPE_FROM_DAYZ_Constant.RAID_LOADING_SCREEN_HUD_UUID:
					menu = new RF_ESCAPE_FROM_DAYZ_CL_RaidLoadingScreen;
				break;
				case RF_ESCAPE_FROM_DAYZ_Constant.RAID_PRELOAD_MENU_HUD_UUID:
					menu = new RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu;
				break;
			}
			if (menu)
				menu.SetID(id);
		}
		return menu;
	}
}