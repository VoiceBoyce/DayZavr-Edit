class RF_ESCAPE_FROM_DAYZ_VanillaItemConfiguration: Managed {

    // Шанс спавна
    float spawnChance;

    // Модель
    string className;

    // Кол-во состояния
    int quantity;

    // кол-во стаков
    int amount;

    // Аттачменты
    autoptr TStringArray attachments = {};
}