class RF_ESCAPE_FROM_DAYZ_LootPresetConfiguration: Managed {

	// Список оружия
	autoptr array<autoptr RF_ESCAPE_FROM_DAYZ_WeaponItemConfiguration> weapons = {};

	// Список предметов которые лягут внутрь карго
	autoptr array<autoptr RF_ESCAPE_FROM_DAYZ_VanillaItemConfiguration> equipments = {};
}

typedef array<autoptr RF_ESCAPE_FROM_DAYZ_LootPresetConfiguration> RF_ESCAPE_FROM_DAYZ_LootPresetConfigurationsArray;
typedef map<string, autoptr RF_ESCAPE_FROM_DAYZ_LootPresetConfiguration> RF_ESCAPE_FROM_DAYZ_LootPresetConfigurationsMap;