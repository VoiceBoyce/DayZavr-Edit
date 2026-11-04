class RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu: UIScriptedMenu {

    // MARK: - Private Properties

    private bool m_Initialized;
	private bool m_IsMenuOpen;

    private autoptr PlayerPreviewWidget previewPlayerCustomWidget;
    private autoptr PlayerPreviewWidget previewPlayerRandomWidget;
    private autoptr ButtonWidget randomPresetButtonWidget, customPresetButtonWidget, closeMenuButtonWidget, moveForwardButtonWidget;
    private autoptr TextWidget customInfoDescription, randomInfoDescription;

    private DayZPlayer previewPlayerCustom;
    private DayZPlayer previewPlayerRandom;

    private const vector PLAYERS_PREVIEW_POSITION = Vector(6336.373535, 301.045776, 7676.859863);

    private int currentChoice = RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.NOT_CHOOSEN;
    private int buttonColorHovered = ARGB(150, 96, 94, 68);
    private int buttonColorSelected = ARGB(220, 96, 94, 68);
    private int buttonColorDefault = ARGB(0, 96, 94, 68);

    private Widget randomPresetPanelWidget, customPresetPanelWidget;

    // MARK: - Init --------------------------------------------------------------------------------

    void ~RF_ESCAPE_FROM_DAYZ_CL_RaidEntrancePreloadMenu() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("~" + this.ToString());
        // this.resetPlayerPreview();
        if (!layoutRoot) return;
        layoutRoot.Unlink();
    }

    // MARK: - Override Methods --------------------------------------------------------------------

    override Widget Init() {
		if (!m_Initialized) {
			layoutRoot = GetGame().GetWorkspace().CreateWidgets(RF_ESCAPE_FROM_DAYZ_Constant.RAID_PRELOAD_MENU_HUD, null);
            previewPlayerCustomWidget = PlayerPreviewWidget.Cast(layoutRoot.FindAnyWidget("CustomPlayerPreviewWidget"));
            previewPlayerRandomWidget = PlayerPreviewWidget.Cast(layoutRoot.FindAnyWidget("RandomPlayerPreviewWidget"));
            randomPresetButtonWidget = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonRandomPreset"));
            customPresetButtonWidget = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonCustomPreset"));
            moveForwardButtonWidget = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonMoveForward"));
            closeMenuButtonWidget = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonCloseMenu"));
            customInfoDescription = TextWidget.Cast(layoutRoot.FindAnyWidget("CustomInfoDescription"));
            randomInfoDescription = TextWidget.Cast(layoutRoot.FindAnyWidget("RandomInfoDescription"));
            randomPresetPanelWidget = Widget.Cast(layoutRoot.FindAnyWidget("RandomPresetPanelWidget"));
            customPresetPanelWidget = Widget.Cast(layoutRoot.FindAnyWidget("CustomPresetPanelWidget"));
			setup();
            updateDescriptionVisuals();
			m_Initialized = true;
		}
		return layoutRoot;
	}

    override bool OnClick(Widget w, int x, int y, int button) {
        Print(w);
        switch (w) {
            case randomPresetButtonWidget: {
                this.currentChoice = RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM;
                updateDescriptionVisuals();
                // RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("playerDidSelectRaidEntrancePreset", RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM);
                break;
            }
            case customPresetButtonWidget: {
                this.currentChoice = RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.CUSTOM;
                updateDescriptionVisuals();
                // RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("playerDidSelectRaidEntrancePreset", RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.CUSTOM);
                break;
            }
            case moveForwardButtonWidget: {
                if ((this.currentChoice != RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM) && (this.currentChoice != RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.CUSTOM)) break;
                RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("playerDidSelectRaidEntrancePreset", this.currentChoice);
                this.SetMenuOpen(false);
                this.Close();
                break;
            }
            case closeMenuButtonWidget: {
                this.SetMenuOpen(false);
                this.Close();
                break;
            }
        }
		return super.OnClick(w, x, y, button);
	}

    override bool OnMouseEnter(Widget w, int x, int y) {
        bool root = super.OnMouseEnter(w, x, y);
        switch(w) {
            case randomPresetButtonWidget: {
                if (currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM) return root;
                randomPresetPanelWidget.SetColor(buttonColorHovered);
                break;
            }
            case customPresetButtonWidget: {
                if (currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.CUSTOM) return root;
                customPresetPanelWidget.SetColor(buttonColorHovered);
                break;
            }
        }
		return root;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
        bool root = super.OnMouseLeave(w, enterW, x, y);
        switch(w) {
            case randomPresetButtonWidget: {
                if (currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM) return root;
                randomPresetPanelWidget.SetColor(buttonColorDefault);
                break;
            }
            case customPresetButtonWidget: {
                if (currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.CUSTOM) return root;
                customPresetPanelWidget.SetColor(buttonColorDefault);
                break;
            }
        }
		return root;
	}

    override void Update(float timeslice) {
        super.Update(timeslice);
        // updatePlayerPreviews();
        previewPlayerRandomWidget.Refresh();
        previewPlayerRandomWidget.Update();
    }

    override void OnShow() {
        super.OnShow();
        Print("on Show");
        this.updatePlayerPreviews();
        this.moveForwardButtonWidget.Enable(false);
        this.currentChoice = RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.NOT_CHOOSEN;
    }

    override void OnHide() {
        super.OnHide();
        Print("on Hide");
        // this.resetPlayerPreview();
        RF_ESCAPE_FROM_DAYZ_Utils.UnlockControls();
    }

    // MARK: - Public Methods ----------------------------------------------------------------------

    void RF_ESCAPE_FROM_DAYZ_CL_OnKeyPress(int key) {
        this.SetMenuOpen(false);
        this.Close();
        RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("playerDidSelectRaidEntrancePreset", RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.NOT_CHOOSEN);
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

    // MARK: - Private Methods ---------------------------------------------------------------------

    private void setup() {
        // updatePlayerPreviews();
        previewPlayerCustomWidget.Show(true);
        previewPlayerRandomWidget.Show(true);
    }

    private void updateDescriptionVisuals() {
        bool isRandomSelected = currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.RANDOM;
        randomInfoDescription.Show(isRandomSelected);
        customInfoDescription.Show(!isRandomSelected);

        if (isRandomSelected) {
            customPresetPanelWidget.SetColor(buttonColorDefault);
            randomPresetPanelWidget.SetColor(buttonColorSelected);
        } else {
            customPresetPanelWidget.SetColor(buttonColorSelected);
            randomPresetPanelWidget.SetColor(buttonColorDefault);
        }

        if (currentChoice == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.NOT_CHOOSEN) {
            customPresetPanelWidget.SetColor(buttonColorDefault);
            randomPresetPanelWidget.SetColor(buttonColorDefault);
        }
        moveForwardButtonWidget.Enable(true);
    }

    private void resetPlayerPreview() {
        if (previewPlayerRandom) previewPlayerRandom.Delete();
        previewPlayerRandom = NULL;
    }

    private void updatePlayerPreviews() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("[RAID-ENTRANCE-MENU]: Начинаем обновлять превьюхи игроков");
        this.updatePlayerRandomPreview();
        this.updatePlayerCustomPreview();
	}

    private void updatePlayerRandomPreview() {
        auto realPlayer = GetGame().GetPlayer();
        if (!realPlayer) return;

        string realPlayerType = realPlayer.GetType();
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[RAID-ENTRANCE-MENU]: У персонажа выбран (%1) тип игрока", realPlayerType));
		
        // Note: лучше удалять и создавать перса каждый раз нового - иначе могут быть белые лица.
        // this.resetPlayerPreview();
        previewPlayerRandomWidget.Refresh();
        previewPlayerRandomWidget.Update();

        auto dummy = RF_ESCAPE_FROM_DAYZ_Utils.GetRandomPlayerPreview(realPlayerType);
        if (previewPlayerRandom != DayZPlayer.Cast(dummy)) {
			previewPlayerRandom = DayZPlayer.Cast(dummy);
			previewPlayerRandomWidget.SetPlayer(previewPlayerRandom);
            RF_ESCAPE_FROM_DAYZ_Log().Info("[RAID-ENTRANCE-MENU]: Устанавливаем игрока в превью виджет");
		}

        RF_ESCAPE_FROM_DAYZ_Log().Info("[RAID-ENTRANCE-MENU]: инстанс игрока для рандомного пресета был создан или обновлен");
        
        string presetId = RF_ESCAPE_FROM_DAYZ_CL_Global.storage.presetId;
        if (!presetId) {
            RF_ESCAPE_FROM_DAYZ_Log().Error(string.Format("[RAID-ENTRANCE-MENU]: Не получилось одеть игрока так как пресет не найден (%1)", presetId));
            return;
        } 
        
        RF_ESCAPE_FROM_DAYZ_CL_Global.spawnPresetLootService.DressPlayerWithPresetId(previewPlayerRandom, presetId);
        previewPlayerRandomWidget.Refresh();

        RF_ESCAPE_FROM_DAYZ_Log().Info("[RAID-ENTRANCE-MENU]: Превьюха игрока со случайным пресетом одета");
    }

    private void updatePlayerCustomPreview() {
        auto realPlayer = GetGame().GetPlayer();
        if (!realPlayer) return;
        if (previewPlayerCustom != DayZPlayer.Cast(realPlayer)) {
			previewPlayerCustom = DayZPlayer.Cast(realPlayer);
			previewPlayerCustomWidget.SetPlayer(previewPlayerCustom);
		}
        previewPlayerCustomWidget.UpdateItemInHands(realPlayer.GetHumanInventory().GetEntityInHands());
		previewPlayerCustomWidget.Refresh();
        previewPlayerCustomWidget.Update();
    }
}