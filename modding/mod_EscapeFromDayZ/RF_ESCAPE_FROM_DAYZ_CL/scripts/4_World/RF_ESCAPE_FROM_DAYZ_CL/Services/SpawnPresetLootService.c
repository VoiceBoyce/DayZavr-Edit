
class RF_ESCAPE_FROM_DAYZ_CL_SpawnPresetLootService: Managed {

    // MARK: Public Methods

    void DressPlayerWithPresetId(DayZPlayer player, string presetId) {

        RF_ESCAPE_FROM_DAYZ_Log().Info("Раздели игрока");

        PlayerBase.Cast(player).RF_ESCAPE_FROM_DAYZ_SE_RemoveAllPlayerItems();

        RF_ESCAPE_FROM_DAYZ_Log().Info("Начинаем одевать игрока");

        if (!RF_ESCAPE_FROM_DAYZ_CL_Global.storage) {
            RF_ESCAPE_FROM_DAYZ_Log().Warning("Storage не был проинецилизирован! Персонаж не может быть одет.");
            return;
        }

        auto presetConfig = RF_ESCAPE_FROM_DAYZ_CL_Global.storage.GetLootPresetConfigurationBy(presetId);
        Print(presetConfig);

        if (!presetConfig) {
            RF_ESCAPE_FROM_DAYZ_Log().Error(string.Format("Пресета с указанным ID (%1) не существует в списке! Персонаж не может быть одет.", presetId));
            return;
        }

		foreach(auto equipment: presetConfig.equipments) {
			createEntitiesInPlayer(player, equipment);
		}

        // foreach(auto weapon: presetConfig.weapons) {
		// 	spawnWeaponInPlayer(player, weapon);
		// }

		if (!player.FindAttachmentBySlotName("Feet")) {
			player.GetInventory().CreateInInventory("HikingBootsLow_black");
		}
    }

    // MARK: Private Methods

    private EntityAI spawnWeaponInPlayer(
		DayZPlayer player,
		RF_ESCAPE_FROM_DAYZ_WeaponItemConfiguration weaponConfiguration
	) {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Создаем оружие");

		auto entity = player.GetInventory().CreateInInventory(weaponConfiguration.className);

		if (!entity) {
			RF_ESCAPE_FROM_DAYZ_Log().Error(string.Format("Ошибка создания оружия в персонаже. (Classname: %1", weaponConfiguration.className));
			return NULL;
		}

		auto weaponBase = Weapon_Base.Cast(entity);

		if (weaponConfiguration.magName && (weaponConfiguration.magName.Length() > 0)) {
			weaponBase.SpawnAttachedMagazine(weaponConfiguration.magName);
		}

		foreach(auto attachment: weaponConfiguration.attachments) {
			auto attachmentEntity = entity.GetInventory().CreateInInventory(attachment);
		}

		return entity;
	}

    private void createEntitiesInPlayer(
		DayZPlayer player,
		RF_ESCAPE_FROM_DAYZ_VanillaItemConfiguration entityConfiguration
	) {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Создаем одежду");
		for (int i = 0; i < entityConfiguration.amount; i++) {
			spawnEntityInPlayer(player, entityConfiguration);
		}
	}

	private EntityAI spawnEntityInPlayer(
		DayZPlayer player,
		RF_ESCAPE_FROM_DAYZ_VanillaItemConfiguration entityConfiguration
	) {
		auto entity = player.GetInventory().CreateInInventory(entityConfiguration.className);
		if (!entity) {
			RF_ESCAPE_FROM_DAYZ_Log().Error(string.Format("Ошибка создания предмета на или в персонаже. (Classname: %1", entityConfiguration.className));
			return NULL;
		}
		// foreach(auto attachment: entityConfiguration.attachments) {
		// 	entity.GetInventory().CreateInInventory(attachment);
		// }

		return entity;
	}
}