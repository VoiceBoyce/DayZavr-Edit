class RF_ESCAPE_FROM_DAYZ_CL_Storage: Managed {

    // Public Properties ---------------------------------------------------------------------------

    // Статистика игрока
    autoptr RF_ESCAPE_FROM_DAYZ_PlayerStatistics playerStatistics; 

    // Список админов
    autoptr TStringArray admins;

    // Идентификатор пресета дикого
    string presetId;

    // Набор всех конфигов пресетов
    // workaround решение:
    // Ключи отдельно и значения отдельно только потому что при передачи hashMap значения ключей
    // теряются и становятся NULL.
    autoptr TStringArray lootPresetsKeys = {};
    autoptr RF_ESCAPE_FROM_DAYZ_LootPresetConfigurationsArray lootPresetsValues = {};

    // Private Properties ---------------------------------------------------------------------------

    private autoptr RF_ESCAPE_FROM_DAYZ_LootPresetConfigurationsMap lootPresetsMap = new RF_ESCAPE_FROM_DAYZ_LootPresetConfigurationsMap();
     
    // Public Methods ---------------------------------------------------------------------------

    void SetupLootPresets() {
        for (int i = 0; i < this.lootPresetsKeys.Count(); i++) {
            string presetKey = this.lootPresetsKeys[i];
            auto presetValue = this.lootPresetsValues[i];

            auto newPresetConfiguration = new RF_ESCAPE_FROM_DAYZ_LootPresetConfiguration();
            newPresetConfiguration.weapons = presetValue.weapons;
            newPresetConfiguration.equipments = presetValue.equipments;

            this.lootPresetsMap.Insert(presetKey, newPresetConfiguration);
            RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Инициализируем пресет: %1 - (%2) - (%3-%4)", presetKey, presetValue, presetValue.weapons.Count(), presetValue.equipments.Count()));
        }
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Проинициализировано %1 пресетов", this.lootPresetsKeys.Count()));
    }

    RF_ESCAPE_FROM_DAYZ_LootPresetConfiguration GetLootPresetConfigurationBy(string _presetId) {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Запрошен пресет: %1", _presetId));
        Print(this.lootPresetsMap);
        Print(this.lootPresetsMap.GetKeyArray());
        Print(this.lootPresetsMap.GetValueArray());
        Print(this.lootPresetsMap.Get(presetId));

        int index = lootPresetsKeys.Find(presetId);
        if (index == -1) return NULL;
        return this.lootPresetsValues.Get(index);
        // return this.lootPresetsMap.Get(presetId);
    }
}