
modded class PlayerBase {

    // MARK: - Private Properties

    private int RF_ESCAPE_FROM_DAYZ_playerPresetChoice;
    private int RF_ESCAPE_FROM_DAYZ_playerPresetChoiceSynched;
    private bool RF_ESCAPE_FROM_DAYZ_isInTeleportProgress;

    // MARK: - Init

    void PlayerBase() {
        Print(string.Format("[PlayerBase]: Inited: %1", this.GetType()));
        RegisterNetSyncVariableInt("RF_ESCAPE_FROM_DAYZ_playerPresetChoice");
        RegisterNetSyncVariableBool("RF_ESCAPE_FROM_DAYZ_isInTeleportProgress");
    }

    // MARK: - Public Methods

    bool RF_ESCAPE_FROM_DAYZ_GetPlayerPresetChoice() {
        return this.RF_ESCAPE_FROM_DAYZ_playerPresetChoice;
    }

    void RF_ESCAPE_FROM_DAYZ_SetPlayerPresetChoice(int _choice) {
        this.RF_ESCAPE_FROM_DAYZ_playerPresetChoice = _choice;
        this.SetSynchDirty();
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Состояние выбора пресета игрока было изменено: %1", _choice));
    }

    bool RF_ESCAPE_FROM_DAYZ_GetIsInTeleportProgress() {
        return this.RF_ESCAPE_FROM_DAYZ_isInTeleportProgress;
    }

    void RF_ESCAPE_FROM_DAYZ_SetIsInTeleportProgress(int isInProgress) {
        this.RF_ESCAPE_FROM_DAYZ_isInTeleportProgress = isInProgress;
        this.SetSynchDirty();
        string statusString = "НЕ находится в стадии телепортации";
        if (isInProgress) statusString = "находится в стадии телепортации";
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Игрок сменил статус телепортации: %1", statusString));
    }

    void RF_ESCAPE_FROM_DAYZ_SE_RemoveAllPlayerItems() {
		array<EntityAI> itemsArray = new array<EntityAI>;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		
		ItemBase item;
		InventoryLocation itemInventoryLocation = new InventoryLocation();
		
		for (int i = 0; i < itemsArray.Count(); i++) {
			Class.CastTo(item, itemsArray.Get(i));
			if (!(item && !item.IsInherited(SurvivorBase))) continue;
            if (!item.GetInventory().GetCurrentInventoryLocation(itemInventoryLocation)) continue;
            GetInventory().LocalDestroyEntity(item);
		}
	}

    // MARK: - Override Methods

    override void OnVariablesSynchronized() {
		if (RF_ESCAPE_FROM_DAYZ_playerPresetChoiceSynched != RF_ESCAPE_FROM_DAYZ_playerPresetChoice) {
            this.RF_ESCAPE_FROM_DAYZ_OnPresetChoiceBeenChanged(RF_ESCAPE_FROM_DAYZ_playerPresetChoice);
			RF_ESCAPE_FROM_DAYZ_playerPresetChoiceSynched = RF_ESCAPE_FROM_DAYZ_playerPresetChoice;

		}
        super.OnVariablesSynchronized();
	}

    // MARK: - Private Methods

    private void RF_ESCAPE_FROM_DAYZ_OnPresetChoiceBeenChanged(int newValue) {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[PlayerBase]: Переменная выбора пресета была синхронизована: До: %1, После: %2", RF_ESCAPE_FROM_DAYZ_playerPresetChoiceSynched, newValue));
    }
}

// TODO: На реконнекте сбросить в not_chosen