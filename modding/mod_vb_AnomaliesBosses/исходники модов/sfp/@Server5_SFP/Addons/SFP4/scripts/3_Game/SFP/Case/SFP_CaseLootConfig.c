// ------------------------------------------------------------
// SFP: Настройка лута для кейсов (JSON в профиле сервера)
// ------------------------------------------------------------
//
// [НАСТРАИВАЕТСЯ] Путь к JSON:
//   $profile:\\SFP\\SFP_CaseLoot.json
//
// Формат (v6):
// {
//   "AutoWeaponAttachments": {
//     "Enabled": true,
//     "FillChance": 0.6,
//     "MaxAttachments": 3,
//     "ExcludeSlots": ["magazine", "magazine2", "magazine3"],
//     "Blacklist": []
//   },
//
//   "LootCleanup": {
//     "UntouchedMinutes": 0
//   },
//
//   "OpenedCaseCleanup": {
//     "Enabled": false,
//     "Minutes": 10
//   },
//   "Cases": [
//     {
//       "CaseClass": "SFP_Case_Base01",
//       "LootTableSFP": [
//         {
//           "ClassName": "FNX45",
//           "Chance": 25,
//           "MinCount": 1,
//           "MaxCount": 1,
//           "MinCondition": 10,
//           "MaxCondition": 100,
//           "Attachments": [
//             {
//               "Slot": "pistolOptics",
//               "Options": [
//                 { "ClassName": "PistolOptic", "Chance": 60, "MinCondition": 80, "MaxCondition": 100 },
//                 { "ClassName": "", "Chance": 40 }
//               ]
//             },
//             {
//               "Slot": "pistolMuzzle",
//               "Options": [
//                 { "ClassName": "PistolSuppressor", "Chance": 30 },
//                 { "ClassName": "", "Chance": 70 }
//               ]
//             }
//           ]
//         },
//         { "ClassName": "TacticalBaconCan", "Chance": 25, "MinCount": 1, "MaxCount": 3 }
//       ]
//     }
//   ]
// }
//
// Пояснения:
// - Chance: вес/шанс (может быть любым >0; не обязательно суммировать до 100).
// - MinCount/MaxCount: количество предметов этого типа, которое заспавнится за 1 открытие.
//   Пример: 1..3 => рандомно 1, 2 или 3 штуки.
// - MinCondition/MaxCondition: (опционально) рандом состояния предмета в процентах (0..100).
//   Пример: 10..100 => может выпасть как в очень плохом состоянии, так и почти новый.
//   Пример: 80..100 => предмет будет в относительно хорошем состоянии.
//   ВАЖНО: если оба поля равны 0 (или отсутствуют) — состояние НЕ меняется (движок оставит дефолт).
// - Attachments: (опционально) список групп модулей для оружия.
//   * Slot: имя слота из CfgSlots (например weaponOptics, weaponMuzzle, pistolOptics и т.п.)
//   * Options: список вариантов для этого слота. Можно добавить вариант "" (пустая строка),
//     чтобы с шансом НЕ ставить модуль.
//   * MinCondition/MaxCondition можно указать и у конкретного модуля (Option) — тогда он будет
//     иметь своё состояние. Если у модуля не указано (0/0) — он наследует диапазон от предмета.
//
// [НЕ ТРОГАТЬ] Обратная совместимость:
// Если файл в старом формате (v1): { "LootTableSFP": [...] }
// он будет автоматически мигрирован в v2/v4 с CaseClass="SFP_Case_Base01".

// ------------------------------------------------------------
// Авто-модули оружия (без ручного списка в LootTableSFP)
// ------------------------------------------------------------
//
// [НАСТРАИВАЕТСЯ] В JSON можно включить режим, когда если для оружия
// НЕ задано поле Attachments, то модуль(и) будут подобраны автоматически:
// - слоты читаются из конфига оружия (attachments[] в CfgWeapons/CfgVehicles)
// - для каждого слота подбирается случайный совместимый предмет, у которого
//   inventorySlot содержит этот слот (CfgVehicles)
//
// Параметры настраиваются в секции AutoWeaponAttachments (см. ниже).
//
class SFP_AutoWeaponAttachmentSettings
{
	// [НАСТРАИВАЕТСЯ] Включить/выключить авто-модули глобально
	bool Enabled;

	// [НАСТРАИВАЕТСЯ] Шанс заполнить КАЖДЫЙ слот (0..1).
	// 1.0 = всегда пытаться поставить модуль в слот (если есть совместимые).
	float FillChance;

	// [НАСТРАИВАЕТСЯ] Максимум модулей на одно оружие (0 = без лимита).
	int MaxAttachments;

	// [НАСТРАИВАЕТСЯ] Исключить слоты (например magazine/magazine2), чтобы
	// авто-режим не ставил магазины/патроны, если это нежелательно.
	ref array<string> ExcludeSlots;

	// [НАСТРАИВАЕТСЯ] Чёрный список классов модулей (не спавнить никогда).
	ref array<string> Blacklist;

	void SFP_AutoWeaponAttachmentSettings()
	{
		ExcludeSlots = new array<string>;
		Blacklist = new array<string>;
		// Значения по умолчанию (будут нормализованы в SanitizeConfig)
		Enabled = true;
		FillChance = 0.6;
		MaxAttachments = 4;
		//ExcludeSlots.Insert("magazine");
		//ExcludeSlots.Insert("magazine2");
		//ExcludeSlots.Insert("magazine3");
		Blacklist.Insert("ImprovisedSuppressor");
		Blacklist.Insert("Groza_Barrel_Short");
		Blacklist.Insert("Groza_Barrel_Grip");
		Blacklist.Insert("Groza_Barrel_Suppressor");
		Blacklist.Insert("MakarovPBSuppressor");
		Blacklist.Insert("Groza_LowerReceiver");
		Blacklist.Insert("GrozaGL_LowerReceiver");
		Blacklist.Insert("GhillieAtt_Winter");
		Blacklist.Insert("GhillieAtt_Mossy");
		Blacklist.Insert("GhillieAtt_Woodland");
		Blacklist.Insert("GhillieAtt_Tan");
	}
};

// ------------------------------------------------------------
// Авто-удаление лута без взаимодействия
// ------------------------------------------------------------
//
// [НАСТРАИВАЕТСЯ] Настраивается в JSON: LootCleanup.UntouchedMinutes
// - 0  = не удалять (лежит как настроено на сервере / до рестарта)
// - 1  = удалить через 1 минуту, если никто не брал
// - 5  = удалить через 5 минут и т.д.
//
class SFP_LootCleanupSettings
{
	// [НАСТРАИВАЕТСЯ] Сколько минут держать предметы, если с ними не было
	// взаимодействия (не поднимали/не помещали в инвентарь).
	int UntouchedMinutes;

	void SFP_LootCleanupSettings()
	{
		UntouchedMinutes = 5;
	}
};

// ------------------------------------------------------------
// Авто-удаление ОТКРЫТОГО кейса
// ------------------------------------------------------------
//
// [НАСТРАИВАЕТСЯ] Настраивается в JSON: OpenedCaseCleanup
// - Enabled = true/false: включить/выключить удаление открытого кейса
// - Minutes: через сколько минут удалить кейс после открытия
//
// ВАЖНО:
// - Удаляется сам кейс (объект в мире), чтобы не было засорения открытыми кейсами.
// - Minutes = 0 => НЕ удалять (кейс будет жить как настроено на сервере / до рестарта).
//
class SFP_OpenedCaseCleanupSettings
{
	// [НАСТРАИВАЕТСЯ] Включить/выключить удаление открытого кейса
	bool Enabled;

	// [НАСТРАИВАЕТСЯ] Через сколько минут удалить кейс после открытия (0 = не удалять)
	int Minutes;

	void SFP_OpenedCaseCleanupSettings()
	{
		Enabled = true;
		Minutes = 7;
	}
};

// ------------------------------------------------------------
// Описание модулей/вложенных настроек (для Attachments)
// ------------------------------------------------------------
class SFP_CaseAttachmentOption
{
	string ClassName;
	float Chance;

	// [НАСТРАИВАЕТСЯ] Опционально: состояние конкретного модуля в процентах (0..100).
	// ВАЖНО: если оба поля равны 0 (или отсутствуют) — состояние НЕ переопределяется
	// и будет взято из состояния базового предмета (если у него задан диапазон).
	int MinCondition;
	int MaxCondition;
};

class SFP_CaseAttachmentGroup
{
	// Имя слота (CfgSlots). Пример: weaponOptics / weaponMuzzle / pistolOptics.
	string Slot;
	ref array<ref SFP_CaseAttachmentOption> Options;

	void SFP_CaseAttachmentGroup()
	{
		Options = new array<ref SFP_CaseAttachmentOption>;
	}
};

// ------------------------------------------------------------
// Одна запись лута
// ------------------------------------------------------------
class SFP_CaseLootEntry
{
	string ClassName;
	float Chance; // вес/шанс
	int MinCount; // минимум спавна (>=1)
	int MaxCount; // максимум спавна (>=MinCount)

	// [НАСТРАИВАЕТСЯ] Опционально: состояние предмета в процентах (0..100).
	// 10..100 => может выпасть в плохом или хорошем состоянии.
	// 80..100 => чаще в хорошем.
	// ВАЖНО: если MinCondition/MaxCondition = 0/0 (или поля отсутствуют) — состояние не меняется.
	int MinCondition;
	int MaxCondition;

	// [НАСТРАИВАЕТСЯ] Опционально: модули для оружия.
	// ВАЖНО: будут применены только если заспавненный предмет IsWeapon()==true.
	ref array<ref SFP_CaseAttachmentGroup> Attachments;

	// [НАСТРАИВАЕТСЯ] Авто-модули оружия (если Attachments не заполнен).
	// Позволяет НЕ прописывать список модулей вручную: модули будут подобраны
	// автоматически из конфигов по слотам оружия.
	//
	// -1 = использовать глобальную настройку AutoWeaponAttachments.Enabled
	//  0 = авто-модули выключены для этой записи
	//  1 = авто-модули включены для этой записи
	int AutoAttachments;

	// [НАСТРАИВАЕТСЯ] Переопределение шанса заполнить каждый слот (0..1).
	// -1 = использовать глобальный AutoWeaponAttachments.FillChance
	float AutoAttachmentsFillChance;

	// [НАСТРАИВАЕТСЯ] Переопределение лимита модулей на одно оружие.
	// -1 = использовать глобальный AutoWeaponAttachments.MaxAttachments
	//  0 = без лимита
	int AutoAttachmentsMax;

	void SFP_CaseLootEntry()
	{
		Attachments = new array<ref SFP_CaseAttachmentGroup>;
		// [НЕ ТРОГАТЬ] Значения по умолчанию для tri-state авто-модулей
		AutoAttachments = -1;
		AutoAttachmentsFillChance = -1.0;
		AutoAttachmentsMax = -1;
	}
};

class SFP_CaseLootGroup
{
	string CaseClass; // ключ (обычно classname кейса)
	ref array<ref SFP_CaseLootEntry> LootTableSFP;

	// [НАСТРАИВАЕТСЯ] Индивидуальная настройка авто-удаления лута для этого кейса.
	// Если false — используется глобальное значение LootCleanup.UntouchedMinutes.
	bool UseCustomCleanup;

	// [НАСТРАИВАЕТСЯ] Минуты для удаления лута без взаимодействия (0 = не удалять).
	// Работает только если UseCustomCleanup=true.
	int CleanupMinutes;

	void SFP_CaseLootGroup()
	{
		LootTableSFP = new array<ref SFP_CaseLootEntry>;
		UseCustomCleanup = false;
		CleanupMinutes = 0;
	}
};

class SFP_CaseLootConfig
{
	ref array<ref SFP_CaseLootGroup> Cases;

	// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-модулей оружия
	ref SFP_AutoWeaponAttachmentSettings AutoWeaponAttachments;

	// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-удаления лута
	ref SFP_LootCleanupSettings LootCleanup;

		// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-удаления ОТКРЫТОГО кейса
	ref SFP_OpenedCaseCleanupSettings OpenedCaseCleanup;

	void SFP_CaseLootConfig()
	{
		Cases = new array<ref SFP_CaseLootGroup>;
		AutoWeaponAttachments = new SFP_AutoWeaponAttachmentSettings();
		LootCleanup = new SFP_LootCleanupSettings();
		OpenedCaseCleanup = new SFP_OpenedCaseCleanupSettings();
	}
};

// Старый (v1) конфиг для миграции
class SFP_CaseLootConfig_V1
{
	ref array<ref SFP_CaseLootEntry> LootTableSFP;

	void SFP_CaseLootConfig_V1()
	{
		LootTableSFP = new array<ref SFP_CaseLootEntry>;
	}
};

class SFP_CaseLootConfigHandler
{
	// [НАСТРАИВАЕТСЯ] Папка/имя файла в профиле
	protected static const string DIR_PATH = "$profile:\\SFP";
	protected static const string FILE_NAME = "SFP_CaseLoot.json";

	// [НЕ ТРОГАТЬ] кеш конфига в памяти
	protected static ref SFP_CaseLootConfig m_Config;
	protected static bool m_Loaded;

	// [НЕ ТРОГАТЬ] чтобы не писать файл по 100 раз (регистрация классов)
	protected static ref array<string> m_RegisteredCaseKeys;

	// [НЕ ТРОГАТЬ] кеш пулов модулей по слотам (для авто-модулей оружия)
	protected static ref map<string, ref array<string>> m_AttachmentPoolsBySlot;

	static void EnsureLoaded()
	{
		if (m_Loaded)
			return;

		m_Loaded = true;

		// Только сервер должен читать/писать JSON из $profile
		if (!GetGame() || !(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (!FileExist(DIR_PATH))
			MakeDirectory(DIR_PATH);

		string fullPath = DIR_PATH + "\\" + FILE_NAME;
		m_Config = new SFP_CaseLootConfig();

		string err;
		if (FileExist(fullPath))
		{
			// Пытаемся загрузить v2/v3
			if (JsonFileLoader<SFP_CaseLootConfig>.LoadFile(fullPath, m_Config, err))
			{
				SanitizeConfig(m_Config);
				// [НЕ ТРОГАТЬ] После обновлений структуры конфига
				// дописываем новые поля обратно в JSON, чтобы их было видно и можно править.
				Save();
				return;
			}

			// Fallback: пробуем v1 и мигрируем
			SFP_CaseLootConfig_V1 oldCfg = new SFP_CaseLootConfig_V1();
			string err2;
			if (JsonFileLoader<SFP_CaseLootConfig_V1>.LoadFile(fullPath, oldCfg, err2))
			{
				MigrateV1ToV2(oldCfg, m_Config);
				SanitizeConfig(m_Config);
				JsonFileLoader<SFP_CaseLootConfig>.SaveFile(fullPath, m_Config, err);
				return;
			}
		}

		// Если файла нет или он битый — создаём дефолтный
		CreateDefault(m_Config);
		SanitizeConfig(m_Config);
		JsonFileLoader<SFP_CaseLootConfig>.SaveFile(fullPath, m_Config, err);
	}

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Авто-регистрация кейсов в JSON
	// ------------------------------------------------------------
	// Логика: если в JSON нет блока для указанного caseClass, он будет добавлен
	// с дефолтным лутом (клонируем таблицу из SFP_Case_Base01, если она есть).
	//
	// ВАЖНО: вызывать это нужно ТОЛЬКО из кейсов, которые наследуются от
	// SFP_OpenableCase_Base. Тогда в JSON будут появляться только ваши "открываемые" кейсы.
	static void EnsureCaseRegistered(string caseClass)
	{
		if (caseClass == string.Empty)
			return;

		EnsureLoaded();

		// Сервер-only
		if (!GetGame() || !(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (!m_Config)
			return;

		if (!m_RegisteredCaseKeys)
			m_RegisteredCaseKeys = new array<string>;

		// Уже регистрировали в этой сессии
		if (m_RegisteredCaseKeys.Find(caseClass) != -1)
			return;

		// Уже есть в конфиге
		SFP_CaseLootGroup existing = FindGroup(caseClass);
		if (existing)
		{
			m_RegisteredCaseKeys.Insert(caseClass);
			return;
		}

		// Создаём новый блок
		SFP_CaseLootGroup ng = new SFP_CaseLootGroup();
		ng.CaseClass = caseClass;

		// Пытаемся клонировать таблицу из "SFP_Case_Base01" (как шаблон)
		SFP_CaseLootGroup tpl = FindGroup("SFP_Case_Base01");
		if (!tpl)
			tpl = FindGroup("*");

		if (tpl && tpl.LootTableSFP && tpl.LootTableSFP.Count() > 0)
		{
			int ti;
			for (ti = 0; ti < tpl.LootTableSFP.Count(); ti++)
			{
				SFP_CaseLootEntry te = tpl.LootTableSFP.Get(ti);
				if (!te) continue;
				if (te.ClassName == string.Empty) continue;
				if (te.Chance <= 0) continue;

				ng.LootTableSFP.Insert(CloneEntry(te));
			}
		}
		else
		{
			// Если даже шаблона нет — жёсткий дефолт
			AddEntry(ng, "Pliers", 25, 1, 1);
			AddEntry(ng, "MessTin", 25, 1, 1);
			AddEntry(ng, "TacticalBaconCan", 25, 1, 3);
			AddEntry(ng, "FNX45", 25, 1, 1);
		}

		m_Config.Cases.Insert(ng);
		SanitizeConfig(m_Config);
		Save();

		m_RegisteredCaseKeys.Insert(caseClass);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Миграция/дефолты/валидация
	// ------------------------------------------------------------
	protected static void MigrateV1ToV2(notnull SFP_CaseLootConfig_V1 oldCfg, notnull SFP_CaseLootConfig outNew)
	{
		outNew.Cases.Clear();
		SFP_CaseLootGroup g = new SFP_CaseLootGroup();
		g.CaseClass = "SFP_Case_Base01";
		int i;
		for (i = 0; i < oldCfg.LootTableSFP.Count(); i++)
		{
			SFP_CaseLootEntry e = oldCfg.LootTableSFP.Get(i);
			if (!e)
				continue;
			// гарантируем поля
			if (e.MinCount <= 0) e.MinCount = 1;
			if (e.MaxCount <= 0) e.MaxCount = 1;
			g.LootTableSFP.Insert(e);
		}
		outNew.Cases.Insert(g);
	}

	protected static void CreateDefault(notnull SFP_CaseLootConfig cfg)
	{
		cfg.Cases.Clear();

		// [НАСТРАИВАЕТСЯ] Дефолт: авто-удаление выключено
		if (!cfg.LootCleanup)
			cfg.LootCleanup = new SFP_LootCleanupSettings();
		cfg.LootCleanup.UntouchedMinutes = 0;

		// [НАСТРАИВАЕТСЯ] Дефолт: удаление открытого кейса выключено
		if (!cfg.OpenedCaseCleanup)
			cfg.OpenedCaseCleanup = new SFP_OpenedCaseCleanupSettings();
		cfg.OpenedCaseCleanup.Enabled = false;
		cfg.OpenedCaseCleanup.Minutes = 0;

		// Дефолтная таблица для оригинального кейса
		SFP_CaseLootGroup g1 = new SFP_CaseLootGroup();
		g1.CaseClass = "SFP_Case_Base01";
		AddEntry(g1, "WaterBottle", 25, 1, 1);
		AddEntry(g1, "FilteringBottle", 25, 1, 1);
		AddEntry(g1, "TacticalBaconCan", 25, 1, 3);
		AddEntry(g1, "SodaCan_Pipsi", 25, 1, 3);
		AddEntry(g1, "SodaCan_Cola", 25, 1, 3);
		AddEntry(g1, "BoxCerealCrunchin", 25, 1, 1);
		AddEntry(g1, "Rice", 25, 1, 1);
		AddEntry(g1, "Chips", 25, 1, 1);
		AddEntry(g1, "BakedBeansCan", 25, 1, 1);
		AddEntry(g1, "SpaghettiCan", 25, 1, 1);
		AddEntry(g1, "PainkillerTablets", 25, 1, 3);
		AddEntry(g1, "VitaminBottle", 25, 1, 2);
		AddEntry(g1, "TetracyclineAntibiotics", 25, 1, 2);
		AddEntry(g1, "Epinephrine", 25, 1, 1);
		AddEntry(g1, "Morphine", 25, 1, 1);
		AddEntry(g1, "SalineBagIV", 25, 1, 3);
		AddEntry(g1, "WeaponCleaningKit", 25, 1, 1);
		AddEntry(g1, "Handcuffs", 25, 1, 1);
		AddEntry(g1, "KitchenKnife", 25, 1, 1);
		AddEntry(g1, "EpoxyPutty", 25, 1, 1);
		AddEntry(g1, "WorkingGloves_Black", 25, 1, 1);
		AddEntry(g1, "TacticalGloves_Black", 25, 1, 1);
		AddEntry(g1, "TacticalGloves_Green", 25, 1, 1);
		AddEntry(g1, "SurgicalGloves_Blue", 25, 1, 1);
		AddEntry(g1, "NVGoggles", 25, 1, 1);
		AddEntry(g1, "AviatorGlasses", 25, 1, 1);
		AddEntry(g1, "Glock19", 25, 1, 1);
		AddEntry(g1, "SteakKnife", 25, 1, 1);
		AddEntry(g1, "FNX45", 25, 1, 1);
		AddEntry(g1, "Colt1911", 25, 1, 1);
		AddEntry(g1, "MakarovIJ70", 25, 1, 1);
		AddEntry(g1, "Mag_Glock_15Rnd", 25, 1, 1);
		AddEntry(g1, "Mag_AKM_30Rnd", 25, 1, 2);
		AddEntry(g1, "Mag_AKM_Drum75Rnd", 25, 1, 1);
		AddEntry(g1, "Mag_STANAG_30Rnd", 25, 1, 1);
		AddEntry(g1, "Mag_STANAG_60Rnd", 25, 1, 1);
		AddEntry(g1, "ReflexOptic", 25, 1, 1);
		AddEntry(g1, "ACOGOptic", 25, 1, 1);
		AddEntry(g1, "AK_Suppressor", 25, 1, 1);
		AddEntry(g1, "M4_Suppressor", 25, 1, 1);
		AddEntry(g1, "AK_Bayonet", 25, 1, 1);
		AddEntry(g1, "PistolSuppressor", 25, 1, 1);
		AddEntry(g1, "Battery9V", 25, 1, 4);
		AddEntry(g1, "Matchbox", 25, 1, 2);
		AddEntry(g1, "Roadflare", 25, 1, 3);
		AddEntry(g1, "GasMask_Filter", 25, 1, 3);
		AddEntry(g1, "Canteen", 25, 1, 1);
		AddEntry(g1, "BandageDressing", 25, 1, 4);
		AddEntry(g1, "PersonalRadio", 25, 1, 1);
		AddEntry(g1, "CZ75", 25, 1, 1);
		AddEntry(g1, "Deagle", 25, 1, 1);
		AddEntry(g1, "GP5GasMask", 25, 1, 1);
		AddEntry(g1, "AirborneMask", 25, 1, 1);
		AddEntry(g1, "RGD5Grenade", 25, 1, 2);
		AddEntry(g1, "M67Grenade", 25, 1, 2);
		AddEntry(g1, "RDG2SmokeGrenade_Black", 25, 1, 3);
		AddEntry(g1, "RDG2SmokeGrenade_White", 25, 1, 3);
		AddEntry(g1, "M18SmokeGrenade_Red", 25, 1, 2);
		cfg.Cases.Insert(g1);

		// Пример таблицы для rifle case (можно менять в JSON)
		SFP_CaseLootGroup g2 = new SFP_CaseLootGroup();
		g2.CaseClass = "SFP_Rifle_Case_Base01";
		AddEntry(g2, "R12", 25, 1, 1);
		AddEntry(g2, "Saiga", 25, 1, 1);
		AddEntry(g2, "Mp133Shotgun", 25, 1, 1);
		AddEntry(g2, "AKM", 25, 1, 1);
		AddEntry(g2, "Aug", 25, 1, 1);
		AddEntry(g2, "FAL", 25, 1, 1);
		AddEntry(g2, "FAMAS", 25, 1, 1);
		AddEntry(g2, "M4A1", 25, 1, 1);
		AddEntry(g2, "Scout", 25, 1, 1);
		AddEntry(g2, "SV98", 25, 1, 1);
		AddEntry(g2, "Vikhr", 25, 1, 1);
		AddEntry(g2, "ASVAL", 25, 1, 1);
		AddEntry(g2, "VSS", 25, 1, 1);
		AddEntry(g2, "UMP45", 25, 1, 1);
		AddEntry(g2, "SVD", 25, 1, 1);
		AddEntry(g2, "PP19", 25, 1, 1);
		AddEntry(g2, "SFP_ShotgunMP153", 25, 1, 1);
		AddEntry(g2, "SFP_ShotgunMP153Mod", 25, 1, 1);
		AddEntry(g2, "SFP_VEPR12", 25, 1, 1);
		AddEntry(g2, "AK74", 25, 1, 1);
		AddEntry(g2, "AKS74U", 25, 1, 1);
		AddEntry(g2, "CZ61", 25, 1, 1);
		AddEntry(g2, "M14", 25, 1, 1);
		AddEntry(g2, "M16A2", 25, 1, 1);
		cfg.Cases.Insert(g2);
	}

	protected static void AddEntry(notnull SFP_CaseLootGroup group, string className, float chance, int minCount = 1, int maxCount = 1)
	{
		SFP_CaseLootEntry e = new SFP_CaseLootEntry();
		e.ClassName = className;
		e.Chance = chance;
		e.MinCount = minCount;
		e.MaxCount = maxCount;
		// [НЕ ТРОГАТЬ] 0/0 => состояние не меняем (оставляем дефолт движка)
		e.MinCondition = 0;
		e.MaxCondition = 0;
		group.LootTableSFP.Insert(e);
	}

	protected static SFP_CaseLootEntry CloneEntry(notnull SFP_CaseLootEntry src)
	{
		SFP_CaseLootEntry dst = new SFP_CaseLootEntry();
		dst.ClassName = src.ClassName;
		dst.Chance = src.Chance;
		dst.MinCount = src.MinCount;
		dst.MaxCount = src.MaxCount;
		dst.MinCondition = src.MinCondition;
		dst.MaxCondition = src.MaxCondition;

		// [НЕ ТРОГАТЬ] Настройки авто-модулей (tri-state + overrides)
		dst.AutoAttachments = src.AutoAttachments;
		dst.AutoAttachmentsFillChance = src.AutoAttachmentsFillChance;
		dst.AutoAttachmentsMax = src.AutoAttachmentsMax;

		// Клонируем Attachments (глубокая копия)
		if (src.Attachments)
		{
			int gi;
			for (gi = 0; gi < src.Attachments.Count(); gi++)
			{
				SFP_CaseAttachmentGroup g = src.Attachments.Get(gi);
				if (!g) continue;

				SFP_CaseAttachmentGroup ng = new SFP_CaseAttachmentGroup();
				ng.Slot = g.Slot;

				if (g.Options)
				{
					int oi;
					for (oi = 0; oi < g.Options.Count(); oi++)
					{
						SFP_CaseAttachmentOption o = g.Options.Get(oi);
						if (!o)
							continue;

						SFP_CaseAttachmentOption no = new SFP_CaseAttachmentOption();
						no.ClassName = o.ClassName;
						no.Chance = o.Chance;
						no.MinCondition = o.MinCondition;
						no.MaxCondition = o.MaxCondition;
						ng.Options.Insert(no);
					}
				}

				dst.Attachments.Insert(ng);
			}
		}

		return dst;
	}

	protected static void SanitizeConfig(notnull SFP_CaseLootConfig cfg)
	{
		if (!cfg.Cases)
			cfg.Cases = new array<ref SFP_CaseLootGroup>;

		// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-модулей оружия
		if (!cfg.AutoWeaponAttachments)
			cfg.AutoWeaponAttachments = new SFP_AutoWeaponAttachmentSettings();
		if (!cfg.AutoWeaponAttachments.ExcludeSlots)
			cfg.AutoWeaponAttachments.ExcludeSlots = new array<string>;
		if (!cfg.AutoWeaponAttachments.Blacklist)
			cfg.AutoWeaponAttachments.Blacklist = new array<string>;
		if (cfg.AutoWeaponAttachments.FillChance < 0) cfg.AutoWeaponAttachments.FillChance = 0;
		if (cfg.AutoWeaponAttachments.FillChance > 1) cfg.AutoWeaponAttachments.FillChance = 1;
		if (cfg.AutoWeaponAttachments.MaxAttachments < 0) cfg.AutoWeaponAttachments.MaxAttachments = 0;

		// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-удаления лута
		if (!cfg.LootCleanup)
			cfg.LootCleanup = new SFP_LootCleanupSettings();
		if (cfg.LootCleanup.UntouchedMinutes < 0) cfg.LootCleanup.UntouchedMinutes = 0;

		// [НАСТРАИВАЕТСЯ] Глобальные настройки авто-удаления ОТКРЫТОГО кейса
		if (!cfg.OpenedCaseCleanup)
			cfg.OpenedCaseCleanup = new SFP_OpenedCaseCleanupSettings();
		if (cfg.OpenedCaseCleanup.Minutes < 0) cfg.OpenedCaseCleanup.Minutes = 0;

		int g;
		for (g = 0; g < cfg.Cases.Count(); g++)
		{
			SFP_CaseLootGroup grp = cfg.Cases.Get(g);
			if (!grp)
				continue;
			if (!grp.LootTableSFP)
				grp.LootTableSFP = new array<ref SFP_CaseLootEntry>;

			// [НАСТРАИВАЕТСЯ] Индивидуальный таймер удаления лута
			if (grp.CleanupMinutes < 0) grp.CleanupMinutes = 0;

			int i;
			for (i = 0; i < grp.LootTableSFP.Count(); i++)
			{
				SFP_CaseLootEntry e = grp.LootTableSFP.Get(i);
				if (!e)
					continue;

				if (e.MinCount <= 0)
					e.MinCount = 1;
				if (e.MaxCount <= 0)
					e.MaxCount = e.MinCount;
				if (e.MaxCount < e.MinCount)
					e.MaxCount = e.MinCount;

				// [НАСТРАИВАЕТСЯ] Авто-модули оружия (tri-state + overrides)
				// AutoAttachments: -1/0/1
				if (e.AutoAttachments < -1) e.AutoAttachments = -1;
				if (e.AutoAttachments > 1) e.AutoAttachments = 1;

				// AutoAttachmentsFillChance: -1 или 0..1
				if (e.AutoAttachmentsFillChance < -1.0) e.AutoAttachmentsFillChance = -1.0;
				if (e.AutoAttachmentsFillChance > 1.0) e.AutoAttachmentsFillChance = 1.0;
				if (e.AutoAttachmentsFillChance != -1.0 && e.AutoAttachmentsFillChance < 0.0) e.AutoAttachmentsFillChance = 0.0;

				// AutoAttachmentsMax: -1, 0(без лимита) или >0
				if (e.AutoAttachmentsMax < -1) e.AutoAttachmentsMax = -1;

				// [НАСТРАИВАЕТСЯ] Диапазон состояния (0..100). 0/0 = не менять.
				if (e.MinCondition < 0) e.MinCondition = 0;
				if (e.MaxCondition < 0) e.MaxCondition = 0;
				if (e.MinCondition > 100) e.MinCondition = 100;
				if (e.MaxCondition > 100) e.MaxCondition = 100;
				// Если пользователь задал хотя бы одно из значений, нормализуем max>=min
				if (e.MinCondition != 0 || e.MaxCondition != 0)
				{
					if (e.MaxCondition < e.MinCondition)
						e.MaxCondition = e.MinCondition;
				}

				if (!e.Attachments)
					e.Attachments = new array<ref SFP_CaseAttachmentGroup>;

				// Валидация групп Attachments
				int ag;
				for (ag = 0; ag < e.Attachments.Count(); ag++)
				{
					SFP_CaseAttachmentGroup grpA = e.Attachments.Get(ag);
					if (!grpA) continue;
					if (!grpA.Options)
						grpA.Options = new array<ref SFP_CaseAttachmentOption>;

					// Нормализуем состояния опций модулей
					int oi;
					for (oi = 0; oi < grpA.Options.Count(); oi++)
					{
						SFP_CaseAttachmentOption opt = grpA.Options.Get(oi);
						if (!opt) continue;
						if (opt.MinCondition < 0) opt.MinCondition = 0;
						if (opt.MaxCondition < 0) opt.MaxCondition = 0;
						if (opt.MinCondition > 100) opt.MinCondition = 100;
						if (opt.MaxCondition > 100) opt.MaxCondition = 100;
						if (opt.MinCondition != 0 || opt.MaxCondition != 0)
						{
							if (opt.MaxCondition < opt.MinCondition)
								opt.MaxCondition = opt.MinCondition;
						}
					}
				}
			}
		}
	}

	protected static void Save()
	{
		// Сервер-only
		if (!GetGame() || !(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (!FileExist(DIR_PATH))
			MakeDirectory(DIR_PATH);

		string fullPath = DIR_PATH + "\\" + FILE_NAME;
		string err;
		JsonFileLoader<SFP_CaseLootConfig>.SaveFile(fullPath, m_Config, err);
	}

	protected static SFP_CaseLootGroup FindGroup(string caseClass)
	{
		if (!m_Config || !m_Config.Cases)
			return null;

		int i;
		for (i = 0; i < m_Config.Cases.Count(); i++)
		{
			SFP_CaseLootGroup g = m_Config.Cases.Get(i);
			if (!g)
				continue;
			if (g.CaseClass == caseClass)
				return g;
		}

		// Дополнительно: wildcard группа "*"
		for (i = 0; i < m_Config.Cases.Count(); i++)
		{
			SFP_CaseLootGroup g2 = m_Config.Cases.Get(i);
			if (!g2)
				continue;
			if (g2.CaseClass == "*")
				return g2;
		}

		return null;
	}

	
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления лута для кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён лут,
	// если его никто не трогал (не поднимал/не помещал в инвентарь).
	// 0 = не удалять.
	static int GetLootCleanupMinutesForCase(string caseClass)
	{
		EnsureLoaded();
		if (!m_Config)
			return 0;

		int globalMin = 0;
		if (m_Config.LootCleanup)
			globalMin = m_Config.LootCleanup.UntouchedMinutes;
		if (globalMin < 0)
			globalMin = 0;

		SFP_CaseLootGroup grp = FindGroup(caseClass);
		if (grp && grp.UseCustomCleanup)
		{
			int m = grp.CleanupMinutes;
			if (m < 0) m = 0;
			return m;
		}

		return globalMin;
	}

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления ОТКРЫТОГО кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления ОТКРЫТОГО кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления ОТКРЫТОГО кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления ОТКРЫТОГО кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.

// ------------------------------------------------------------
	// Получить рандомную запись лута (включая Attachments) + количество
	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Авто-регистрацию вызывает базовый класс кейса (SFP_OpenableCase_Base).
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Таймер авто-удаления ОТКРЫТОГО кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// 0 = не удалять.
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает минуты (0 = не удалять).
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает минуты (0 = не удалять).
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает минуты (0 = не удалять).
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает минуты (0 = не удалять).
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает минуты (0 = не удалять).
	static bool GetRandomLootEntryForCase(string caseClass, out SFP_CaseLootEntry outEntry, out int outCount)
	{
		outEntry = null;
		outCount = 0;

		EnsureLoaded();

		SFP_CaseLootGroup grp = FindGroup(caseClass);
		if (!grp || !grp.LootTableSFP || grp.LootTableSFP.Count() == 0)
			return false;

		float total = 0;
		int i;
		for (i = 0; i < grp.LootTableSFP.Count(); i++)
		{
			SFP_CaseLootEntry e = grp.LootTableSFP.Get(i);
			if (!e)
				continue;
			if (e.ClassName == string.Empty)
				continue;
			if (e.Chance <= 0)
				continue;

			total += e.Chance;
		}

		if (total <= 0)
			return false;

		float roll = Math.RandomFloatInclusive(0, total);
		float acc = 0;
		for (i = 0; i < grp.LootTableSFP.Count(); i++)
		{
			SFP_CaseLootEntry e2 = grp.LootTableSFP.Get(i);
			if (!e2)
				continue;
			if (e2.ClassName == string.Empty)
				continue;
			if (e2.Chance <= 0)
				continue;

			acc += e2.Chance;
			if (roll <= acc)
			{
				outEntry = e2;

				int minC = e2.MinCount;
				int maxC = e2.MaxCount;
				if (minC <= 0) minC = 1;
				if (maxC <= 0) maxC = minC;
				if (maxC < minC) maxC = minC;

				outCount = Math.RandomIntInclusive(minC, maxC);
				if (outCount <= 0) outCount = 1;
				return true;
			}
		}

		return false;
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Старый API (оставлен для совместимости)
	// ------------------------------------------------------------
	static bool GetRandomLootForCase(string caseClass, out string outTypeName, out int outCount)
	{
		outTypeName = string.Empty;
		outCount = 0;

		SFP_CaseLootEntry e;
		int c;
		if (!GetRandomLootEntryForCase(caseClass, e, c))
			return false;
		if (!e)
			return false;

		outTypeName = e.ClassName;
		outCount = c;
		return true;
	}

	// ------------------------------------------------------------
	// Авто-модули: доступ к глобальным настройкам
	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Используется в SFP_OpenableCase_Base.
	static SFP_AutoWeaponAttachmentSettings GetAutoWeaponAttachmentSettings()
	{
		EnsureLoaded();
		if (!m_Config) return null;
		if (!m_Config.AutoWeaponAttachments)
			m_Config.AutoWeaponAttachments = new SFP_AutoWeaponAttachmentSettings();
		return m_Config.AutoWeaponAttachments;
	}

	// ------------------------------------------------------------
	// Авто-модули: пул совместимых модулей по слоту (скан конфигов)
	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Кешируется по слотам; сканирование делается один раз на слот.
	static array<string> GetAttachmentPoolForSlot(string slotName)
	{
		EnsureLoaded();
		if (!GetGame()) return null;
		if (slotName == string.Empty) return null;

		if (!m_AttachmentPoolsBySlot)
			m_AttachmentPoolsBySlot = new map<string, ref array<string>>();

		array<string> pool = m_AttachmentPoolsBySlot.Get(slotName);
		if (pool)
			return pool;

		pool = new array<string>();
		BuildPoolForSlot(slotName, pool);
		m_AttachmentPoolsBySlot.Set(slotName, pool);
		return pool;
	}

	protected static void BuildPoolForSlot(string slotName, notnull array<string> outPool)
	{
		outPool.Clear();
		if (!GetGame()) return;
		if (slotName == string.Empty) return;

		string root = "CfgVehicles";
		int cnt = GetGame().ConfigGetChildrenCount(root);
		int i;
		for (i = 0; i < cnt; i++)
		{
			string cname;
			GetGame().ConfigGetChildName(root, i, cname);
			if (cname == string.Empty)
				continue;

			string p = root + " " + cname;
			// [НЕ ТРОГАТЬ] В разных ветках/версиях DayZ сигнатура ConfigGetInt отличается.
			// Здесь используем максимально совместимый вариант: ConfigGetInt(path) -> int.
			int scope = 0;
			if (GetGame().ConfigIsExisting(p + " scope"))
				scope = GetGame().ConfigGetInt(p + " scope");
			if (scope != 2)
				continue;

			// Отбираем только инвентарь (модули обычно наследуются от Inventory_Base)
			if (!GetGame().IsKindOf(cname, "Inventory_Base"))
				continue;

			array<string> slots = new array<string>;
			if (GetGame().ConfigIsExisting(p + " inventorySlot"))
			{
				// [НЕ ТРОГАТЬ] На разных версиях может быть массивом или одиночной строкой.
				GetGame().ConfigGetTextArray(p + " inventorySlot", slots);
				if (slots.Count() == 0)
				{
					string single;
					GetGame().ConfigGetText(p + " inventorySlot", single);
					if (single != string.Empty)
						slots.Insert(single);
				}
			}

			int s;
			bool ok = false;
			for (s = 0; s < slots.Count(); s++)
			{
				if (slots.Get(s) == slotName)
				{
					ok = true;
					break;
				}
			}
			if (!ok)
				continue;

			outPool.Insert(cname);
		}
	}

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Удаление открытого кейса
	// ------------------------------------------------------------
	// Возвращает количество минут, через которое будет удалён КЕЙС после открытия.
	// 0 = не удалять.
	static int GetOpenedCaseCleanupMinutes()
	{
		EnsureLoaded();
		if (!m_Config || !m_Config.OpenedCaseCleanup)
			return 0;
		if (!m_Config.OpenedCaseCleanup.Enabled)
			return 0;
		int m = m_Config.OpenedCaseCleanup.Minutes;
		if (m < 0) m = 0;
		return m;
	}
}
