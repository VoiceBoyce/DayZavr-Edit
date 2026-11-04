class RF_ESCAPE_FROM_DAYZ_WeaponItemConfiguration: Managed {

    // Шанс спавна
    float spawnChance;

    // Название оружия
    string className;

    // Название магазина
    string magName;

    // Аттачменты
    autoptr TStringArray attachments = {};
}