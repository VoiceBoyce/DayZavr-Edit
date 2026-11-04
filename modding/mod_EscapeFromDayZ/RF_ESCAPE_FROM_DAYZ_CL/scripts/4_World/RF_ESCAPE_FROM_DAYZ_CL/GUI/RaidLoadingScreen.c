class RF_ESCAPE_FROM_DAYZ_CL_RaidLoadingScreen: UIScriptedMenu {

    private bool m_Initialized;
	private bool m_IsMenuOpen;

    private autoptr ImageWidget mainImageWidget;
    private autoptr ImageWidget screenImageWidget;
    private autoptr TextWidget mainSubtitleTextWidget;
    private autoptr TextWidget imageDetailsTitle;
    private autoptr TextWidget imageDetailsText;
    private autoptr TextWidget killsTextWidget;
    private autoptr TextWidget deathsTextWidget;
    private autoptr TextWidget timeInZoneTextWidget;
    private autoptr TextWidget raidsAmountTextWidget;
    private autoptr PlayerPreviewWidget characterPanelWidget;

    private autoptr ImageWidget loaderImageWidget;

    private autoptr Timer eventTimer = new Timer();

    autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenConfig config;

    private const int CHANGE_SCREEN_DELAY = 40;
    private int LOADER_ROTATION_ANIMATION_SPEED = 60;
    private const vector PLAYERS_PREVIEW_POSITION = Vector(6336.373535, 301.045776, 7676.859863);
    
    DayZPlayer previewPlayer;

    // MARK: - Init

    void ~RF_ESCAPE_FROM_DAYZ_CL_RaidLoadingScreen() {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("~ %1", this));
        eventTimer.Stop();

        if (layoutRoot) {
		    layoutRoot.Unlink();
        }
	}

    // MARK: - Override Methods

    override Widget Init() {
		if (!m_Initialized) {
			layoutRoot = GetGame().GetWorkspace().CreateWidgets(RF_ESCAPE_FROM_DAYZ_Constant.RAID_LOADING_SCREEN_HUD, null);
			mainImageWidget = ImageWidget.Cast(layoutRoot.FindAnyWidget("MainImageWidget"));
			screenImageWidget = ImageWidget.Cast(layoutRoot.FindAnyWidget("ScreenImageWidget"));
			loaderImageWidget = ImageWidget.Cast(layoutRoot.FindAnyWidget("LoaderImage"));

			mainSubtitleTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("MainSubtitle"));
			imageDetailsTitle = TextWidget.Cast(layoutRoot.FindAnyWidget("ImageDetailsTitle"));
			imageDetailsText = TextWidget.Cast(layoutRoot.FindAnyWidget("ImageDetailsText"));
			killsTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("KillsValue"));
			deathsTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("DeathsValue"));
			timeInZoneTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("TimeInZoneValue"));
            raidsAmountTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("RaidsAmountValue"));
            
            characterPanelWidget = PlayerPreviewWidget.Cast(layoutRoot.FindAnyWidget("CharacterPanelWidgetTest"));
            updatePlayerPreview();
            characterPanelWidget.Show(true);

			setup();
			m_Initialized = true;
		}
		return layoutRoot;
	}

    void debugProperties() {
        Print(mainImageWidget);
        Print(screenImageWidget);
        Print(loaderImageWidget);
        Print(mainSubtitleTextWidget);
        Print(imageDetailsTitle);
        Print(imageDetailsText);
        Print(killsTextWidget);
        Print(deathsTextWidget);
        Print(timeInZoneTextWidget);
        Print(raidsAmountTextWidget);
        Print(characterPanelWidget);
        Print(eventTimer);
        Print(config);
        Print(CHANGE_SCREEN_DELAY);
        Print(LOADER_ROTATION_ANIMATION_SPEED);
        Print(previewPlayer);
        Print(m_Initialized);
        Print(m_IsMenuOpen);
        Print(layoutRoot);
        Print(this);
    }

    // MARK: - Override Methods

    override void Update(float timeslice) {
        super.Update(timeslice);
        updateRotateAnimation(timeslice);
    }

    override void OnHide() {
        super.OnHide();
        Print("on hide");
        Print(previewPlayer);
        previewPlayer = null;
        eventTimer.Stop();
        eventTimer = null;

        // debugProperties();
    }

    override void OnShow() {
        super.OnShow();
        // Он все еще не фетчит тут
        updateStatistics();
        // if (eventTimer) {
        //     Print("starting timer");
        //     eventTimer.Run(CHANGE_SCREEN_DELAY, this, "tick", NULL, true);
        // }
    }

    void RefreshPlayerPreview() {
		characterPanelWidget.Refresh();
	}

    private void tick() {
        Print("tick");
        updatePlayerPreview();
        updateImageScreen();
    }

    bool IsMenuOpen() {
		return m_IsMenuOpen;
	}

	void SetMenuOpen(bool isMenuOpen) {
		m_IsMenuOpen = isMenuOpen;
		if (m_IsMenuOpen)
			PPEffects.SetBlurMenu(0.75);
		else
			PPEffects.SetBlurMenu(0);
	}

    void SetSubtitle(int teleportationType) {
         switch (teleportationType) {
            case RF_ESCAPE_FROM_DAYZ_TeleportationTypeEnum.ENTERING: {
			    mainSubtitleTextWidget.SetText("#RF_MiddleLoadingScreen_MainSubtitle_EnterPvpZone");
                break;
            }
            case RF_ESCAPE_FROM_DAYZ_TeleportationTypeEnum.EXTRACTING: {
			    mainSubtitleTextWidget.SetText("#RF_MiddleLoadingScreen_MainSubtitle_LeavingPvpZone");
                break;
            }
            case RF_ESCAPE_FROM_DAYZ_TeleportationTypeEnum.EMERGENCY_EXIT: {
			    mainSubtitleTextWidget.SetText("Экстренный выход");
                break;
            }
        }
    }

    private void setup() {
        this.config = new RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenConfig();

        for (int i = 1; i <= 30; i++) {
            string number = string.Format("%1", i);
            if (i <= 9) {
                number = string.Format("0%1", i);
            }
            RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetailsArray details = {};
            details.Insert(getImageDetail(string.Format("#RF_MiddleLoadingScreen_%1_01_Title", number), string.Format("#RF_MiddleLoadingScreen_%1_01_Text", number)));
            details.Insert(getImageDetail(string.Format("#RF_MiddleLoadingScreen_%1_02_Title", number), string.Format("#RF_MiddleLoadingScreen_%1_02_Text", number)));
            this.config.screens.Insert(createLoadingScreenImageConfig(string.Format("RF_MiddleLoadingScreen_%1", number), details));
        }

        updateImageScreen();
        updateStatistics();
        eventTimer.Run(CHANGE_SCREEN_DELAY, this, "tick", NULL, true);
    }

    private void updateStatistics() {
        auto playerStatistics = RF_ESCAPE_FROM_DAYZ_CL_Global.storage.playerStatistics;
        if (!playerStatistics) {
            RF_ESCAPE_FROM_DAYZ_Log().Warning("Trying to show hud without storage data");
            return;
        }
        int timeInMinutes = playerStatistics.timeInsidePVPZone / 1000 / 60;
        int hours = timeInMinutes / 60;
        int minutes = timeInMinutes - hours * 60;
        killsTextWidget.SetText(string.Format("%1", playerStatistics.kills));
        deathsTextWidget.SetText(string.Format("%1", playerStatistics.deaths));
        timeInZoneTextWidget.SetText(string.Format("%1h %2m", hours, minutes));
        raidsAmountTextWidget.SetText(string.Format("%1", playerStatistics.raidsAmount));
    }

    private void updatePlayerPreview() {
        auto player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player) return;

        int choice = player.RF_ESCAPE_FROM_DAYZ_GetPlayerPresetChoice();

        if (choice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM) {
            this.updatePlayerPreviewWithRandomPreset(player);
            return;
        }

		if (previewPlayer != DayZPlayer.Cast(GetGame().GetPlayer())) {
			previewPlayer = DayZPlayer.Cast(GetGame().GetPlayer());
			characterPanelWidget.SetPlayer(previewPlayer);
			characterPanelWidget.SetModelPosition(Vector(-0.2, -0.4, 3.5));
            characterPanelWidget.SetModelOrientation(Vector(0, 33.4, 0));
		}
		
		characterPanelWidget.UpdateItemInHands(GetGame().GetPlayer().GetHumanInventory().GetEntityInHands());
		characterPanelWidget.Refresh();
	}

    private void updatePlayerPreviewWithRandomPreset(PlayerBase player) {
        string playerType = player.GetType();
        auto dummy = RF_ESCAPE_FROM_DAYZ_Utils.GetRandomPlayerPreview(playerType);
        if (previewPlayer != DayZPlayer.Cast(dummy)) {
			previewPlayer = DayZPlayer.Cast(dummy);
			characterPanelWidget.SetPlayer(previewPlayer);
            RF_ESCAPE_FROM_DAYZ_Log().Info("[RAID-ENTRANCE-MENU]: Устанавливаем игрока в превью виджет");
		}
        // previewPlayer = DayZPlayer.Cast(playerEnt);

        string presetId = RF_ESCAPE_FROM_DAYZ_CL_Global.storage.presetId;
        if (presetId) {
            RF_ESCAPE_FROM_DAYZ_CL_Global.spawnPresetLootService.DressPlayerWithPresetId(previewPlayer, presetId);
        } else {
            Print("Не получилось одеть игрока так как пресет не найден");
        }
        characterPanelWidget.SetModelPosition(Vector(-0.2, -0.4, 3.5));
        characterPanelWidget.SetModelOrientation(Vector(0, 33.4, 0));
        characterPanelWidget.Refresh();
    }

    private void updateRotateAnimation(float timeslice) {
        vector rotation = loaderImageWidget.GetRotation();
        rotation[2] = rotation[2] + timeslice * LOADER_ROTATION_ANIMATION_SPEED;
        loaderImageWidget.SetRotation(rotation[0], rotation[1], rotation[2], true);
    }

    private void updateImageScreen() {
        auto screen = this.config.screens.GetRandomElement();
        auto details = screen.details.GetRandomElement();
        imageDetailsTitle.SetText(details.title);
        imageDetailsText.SetText(details.description);
        imageDetailsTitle.Update();
        imageDetailsText.Update();
        this.setImage(screen.imageSrc);
    }

    private void setImage(string imagePath) {
        string path = string.Format("%1%2.edds", RF_ESCAPE_FROM_DAYZ_Constant.IMAGES_FOLDER, imagePath);
		screenImageWidget.LoadImageFile(0, path);
	}

    private RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage createLoadingScreenImageConfig(string src, RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetailsArray details) {
        RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage object = new RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage();
        object.imageSrc = src;
        object.details = details;
        return object;
    }

    private RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail getImageDetail(string title, string description) {
        RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail object = new RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail();
        object.title = title;
        object.description = description;
        return object;
    }
}



class RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenConfig: Managed {

    autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImagesArray screens = {};
}

class RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage: Managed {

    // Путь до картинки
    string imageSrc;

    // Заголовок
    autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetailsArray details = {};
}


typedef array<autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage> RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImagesArray;
typedef map<string, autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImage> RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImagesMap;

class RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail: Managed {

    // Заголовок
   string title;

    // Описание
   string description;
}

typedef array<autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail> RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetailsArray;
typedef map<string, autoptr RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetail> RF_ESCAPE_FROM_DAYZ_CL_LoadingScreenImageDetailsMap;
