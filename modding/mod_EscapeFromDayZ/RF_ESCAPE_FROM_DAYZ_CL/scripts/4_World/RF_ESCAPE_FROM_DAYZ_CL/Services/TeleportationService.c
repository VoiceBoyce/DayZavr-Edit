class RF_ESCAPE_FROM_DAYZ_CL_TeleportationService: Managed {

    // MARK: - Private Properties
    
    private autoptr RF_ESCAPE_FROM_DAYZ_CL_RaidLoadingScreen raidLoadingScreenHud;
    autoptr RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu raidEntranceLoadingMenuHud;

    // MARK: - Public Methods

    void StartTeleportationAnimation(RF_ESCAPE_FROM_DAYZ_ExtractionTeleportationDto config) {
        int blackScreenDuration = config.soundLength;
        int openEyesAnimationDuration = RF_ESCAPE_FROM_DAYZ_CL_Global.eyesAnimationService.GetOpenEyesAnimationDuration();
        int closeEyesAnimationDuration = RF_ESCAPE_FROM_DAYZ_CL_Global.eyesAnimationService.GetCloseEyesAnimationDuration();

        RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
        this.playAnimationSound(config.soundSet, config.soundLength, config.loadingPosition);
        RF_ESCAPE_FROM_DAYZ_CL_Global.eyesAnimationService.StartCloseEyesAnimation();

        float openEyeAnimationDelay = (closeEyesAnimationDuration + blackScreenDuration) * 1000;
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(handleOpenLoadingScreenMenu, closeEyesAnimationDuration * 1000, false, config.teleportationType);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RF_ESCAPE_FROM_DAYZ_CL_Global.eyesAnimationService.StartOpenEyesAnimation, openEyeAnimationDelay, false);

        // float unlockControlsDelay = (closeEyeAnimationDuration + openEyesAnimationDuration + blackScreenDuration) * 1000;
        // GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(unlockControls, unlockControlsDelay, false);

        float closeLoadingScreenMenuDelay = (closeEyesAnimationDuration + blackScreenDuration) * 1000;
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Initiate teleportation animation: blackScreenDuration: %1, openEyesAnimationDuration: %2, closeEyesAnimationDuration: %3, openEyeAnimationDelay: %4, closeLoadingScreenMenuDelay: %5", blackScreenDuration, openEyesAnimationDuration, closeEyesAnimationDuration, openEyeAnimationDelay, closeLoadingScreenMenuDelay));
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(hideLoadingScreenMenu, closeLoadingScreenMenuDelay, false);
    }

    void PlayerDidEnterTeleportationZone(PlayerIdentity playerIdentity, string presetId) {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Игрок вошел в триггер телепортации. Начинаем процесс открытия худа для выбора пресета, %1", presetId));
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(handleRaidEntrancePreloadMenuVisibleState, 500, false);
    }

    // MARK: - Private Methods

    private void playAnimationSound(string soundSet, int soundLength, vector position) {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Play sound: %1: %2 seconds (%3)", soundSet, soundLength, position.ToString(false)));
        auto rfSound = SEffectManager.PlaySound(soundSet, position);
        rfSound.SetSoundLoop(false);
        rfSound.SetSoundAutodestroy(true);
    }

    private void createLoadingScreenMenu() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Начали создавать меню телепортации");
        // Закрываем инвентарь, чат и другие меню
        auto player = PlayerBase.Cast(GetGame().GetPlayer());
		player.CloseInventoryMenu();
        GetGame().GetUIManager().CloseAll();
        if (GetGame().GetUIManager().GetMenu() == NULL && raidLoadingScreenHud != null) return;
        RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
        raidLoadingScreenHud = RF_ESCAPE_FROM_DAYZ_CL_RaidLoadingScreen.Cast(GetGame().GetUIManager().EnterScriptedMenu(RF_ESCAPE_FROM_DAYZ_Constant.RAID_LOADING_SCREEN_HUD_UUID, null));
        if (!raidLoadingScreenHud) {
            RF_ESCAPE_FROM_DAYZ_Log().Error("Unable to create raid loading screen menu! (Something is shown instead)");
            return;
        }
        raidLoadingScreenHud.SetMenuOpen(true);
        RF_ESCAPE_FROM_DAYZ_Log().Info("Завершили создавать меню телепортации");
    }

    private void openLoadingScreenMenu() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Начали открывать меню телепортации");
        // Закрываем инвентарь, чат и другие меню
        auto player = PlayerBase.Cast(GetGame().GetPlayer());
		player.CloseInventoryMenu();
        GetGame().GetUIManager().CloseAll();
        if (GetGame().GetUIManager().GetMenu() != NULL) return;
        GetGame().GetUIManager().ShowScriptedMenu(raidLoadingScreenHud, NULL);
        raidLoadingScreenHud.SetMenuOpen(true);
        RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
        RF_ESCAPE_FROM_DAYZ_Log().Info("Завершили открывать меню телепортации");
    }

    private void hideLoadingScreenMenu() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Начали скрывать меню телепортации");
        raidLoadingScreenHud.SetMenuOpen(false);
        RF_ESCAPE_FROM_DAYZ_Utils.UnlockControls();
        if (!raidLoadingScreenHud) {
            RF_ESCAPE_FROM_DAYZ_Log().Error("Попытка закрыть несуществующий худ телепортации");
            return;
        }
        GetGame().GetUIManager().HideScriptedMenu(raidLoadingScreenHud);
        raidLoadingScreenHud = null;
        RF_ESCAPE_FROM_DAYZ_Log().Info("Завершили скрывать меню телепортации");
    }

    private void handleOpenLoadingScreenMenu(int teleportationType) {
        RF_ESCAPE_FROM_DAYZ_Log().Info("handleOpenLoadingScreenMenu");
        if (raidLoadingScreenHud) {
            raidLoadingScreenHud.SetSubtitle(teleportationType);
            openLoadingScreenMenu();
        } else {
            createLoadingScreenMenu();
            raidLoadingScreenHud.SetSubtitle(teleportationType);
        }
    }
    
    private void handleRaidEntrancePreloadMenuVisibleState() {
        if (raidEntranceLoadingMenuHud) {
            if (raidEntranceLoadingMenuHud.IsMenuOpen()) {
                hideRaidEntrancePreloadMenu();
            } else {
               openRaidEntrancePreloadMenu();
            }
        } else {
            createRaidEntrancePreloadMenu();
        }
    }

    private void openRaidEntrancePreloadMenu() {
        if (GetGame().GetUIManager().GetMenu() != NULL) return;
        GetGame().GetUIManager().ShowScriptedMenu(raidEntranceLoadingMenuHud, NULL);
        raidEntranceLoadingMenuHud.SetMenuOpen(true);
        RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
    }

    private void createRaidEntrancePreloadMenu() {
        // Закрываем инвентарь
        auto player = PlayerBase.Cast(GetGame().GetPlayer());
		player.CloseInventoryMenu();
        if (GetGame().GetUIManager().GetMenu() == NULL && raidEntranceLoadingMenuHud != null) return;
        raidEntranceLoadingMenuHud = RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(RF_ESCAPE_FROM_DAYZ_Constant.RAID_PRELOAD_MENU_HUD_UUID, null));
        if (!raidEntranceLoadingMenuHud) return;
        RF_ESCAPE_FROM_DAYZ_Utils.LockControls();
        raidEntranceLoadingMenuHud.SetMenuOpen(true);
    }

    private void hideRaidEntrancePreloadMenu() {
        raidEntranceLoadingMenuHud.SetMenuOpen(false);
        GetGame().GetUIManager().HideScriptedMenu(raidEntranceLoadingMenuHud);
        RF_ESCAPE_FROM_DAYZ_Utils.UnlockControls();
    }
}