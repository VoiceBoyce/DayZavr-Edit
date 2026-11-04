class RF_ESCAPE_FROM_DAYZ_CL_KeyHandler: Managed {

    // MARK: - Private Properties

    private autoptr TIntArray keys = {
        KeyCode.KC_PRIOR,
        KeyCode.KC_NEXT,
        KeyCode.KC_INSERT,
        KeyCode.KC_DELETE,
        KeyCode.KC_ESCAPE
    };

    // autoptr RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu raidEntranceLoadingMenuHud;

    // MARK: - Init

    void RF_ESCAPE_FROM_DAYZ_CL_KeyHandler() {
        auto game = GetDayZGame();
        if (game) game.Subscribe(this, "OnKeyPress");
    }

    void ~RF_ESCAPE_FROM_DAYZ_CL_KeyHandler() {
        auto game = GetDayZGame();
        if (game) game.Unsubscribe(this, "OnKeyPress");
    }

    bool testCloseAnimation;

    // MARK: - Subscribers

    private void OnKeyPress(int key) {
        if (!isKeyWhitelisted(key)) return;
        switch (key) {
            case KeyCode.KC_PRIOR: { didPressPageUp(); break; }
            case KeyCode.KC_NEXT:  { didPressPageDown(); break; }
            case KeyCode.KC_INSERT:  { didPressInsert(); break; }
            case KeyCode.KC_DELETE:  { didPressDelete(); break; }
            case KeyCode.KC_ESCAPE:  { didPressEscape(); break; }
        }
        RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("didPressKey", key);
    }

    void DelayedMidAirDetonation(float x, float y, float z) {
		EffectSound artilleryFallSound = SEffectManager.PlaySound("Artillery_Fall_SoundSet", Vector(x, y, z));
		artilleryFallSound.SetAutodestroy(true);
	}

    private bool isKeyWhitelisted(int key) {
        auto isExist = keys.Find(key);
        return isExist != -1;
    }
    
    private void onKeyPress_RaidEntrancePreloadMenu(int key) {
        auto raidEntrancePreloadMenu = RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu.Cast(GetGame().GetUIManager().GetMenu());
        if (!raidEntrancePreloadMenu) return;
        raidEntrancePreloadMenu.RF_ESCAPE_FROM_DAYZ_CL_OnKeyPress(key);
    }

    private void didPressPageUp() {}

    private void didPressPageDown() {}

    private void didPressInsert() {}

    private void didPressEscape() {
        auto preloadScreen = RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu.Cast(GetGame().GetUIManager().GetMenu());
        if (!preloadScreen) return;
        preloadScreen.SetMenuOpen(false);
        GetGame().GetUIManager().HideScriptedMenu(preloadScreen);
        RF_ESCAPE_FROM_DAYZ_Utils.UnlockControls();
    }

    private void didPressDelete() {
        // if (raidEntranceLoadingMenuHud) {
        //     if (raidEntranceLoadingMenuHud.IsMenuOpen()) {
        //         RF_ESCAPE_FROM_DAYZ_CL_hideRaidEntrancePreloadMenu();
        //     } else {
        //        RF_ESCAPE_FROM_DAYZ_CL_openRaidEntrancePreloadMenu();
        //     }
        // } else {
        //     RF_ESCAPE_FROM_DAYZ_CL_createRaidEntrancePreloadMenu();
        // }
    }

    // private void RF_ESCAPE_FROM_DAYZ_CL_openRaidEntrancePreloadMenu() {
    //     if (GetGame().GetUIManager().GetMenu() != NULL) return;
    //     GetGame().GetUIManager().ShowScriptedMenu(raidEntranceLoadingMenuHud, NULL);
    //     raidEntranceLoadingMenuHud.SetMenuOpen(true);
    //     RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
    // }

    // private void RF_ESCAPE_FROM_DAYZ_CL_createRaidEntrancePreloadMenu() {
    //     // Закрываем инвентарь
    //     auto player = PlayerBase.Cast(GetGame().GetPlayer());
	// 	player.CloseInventoryMenu();
    //     if (GetGame().GetUIManager().GetMenu() == NULL && raidEntranceLoadingMenuHud != null) return;
    //     RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
    //     raidEntranceLoadingMenuHud = RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(RF_ESCAPE_FROM_DAYZ_Constant.RAID_PRELOAD_MENU_HUD_UUID, null));
    //     raidEntranceLoadingMenuHud.SetMenuOpen(true);
    // }

    // private void RF_ESCAPE_FROM_DAYZ_CL_hideRaidEntrancePreloadMenu() {
    //     raidEntranceLoadingMenuHud.SetMenuOpen(false);
    //     GetGame().GetUIManager().HideScriptedMenu(raidEntranceLoadingMenuHud);
    //     RF_ESCAPE_FROM_DAYZ_Utils.UnlockControls();
    // }
}
