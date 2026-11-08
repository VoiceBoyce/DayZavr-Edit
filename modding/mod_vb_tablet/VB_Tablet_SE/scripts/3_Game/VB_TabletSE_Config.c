// Настройки сервера: $profile:VB_Mods/VB_Tablet/config.json, на боевом это Instance_1\\VB_Mods\\VB_Tablet\\.
// Особая группа существ для отдельных контрактов: Kind пишется в контракт, Classes это классы
// или начало имени со звёздочкой ("ERA_*"). Убийство считается и в группе, и как обычный зомби.
class VB_TabletKillGroup
{
	string Kind;
	ref array<string> Classes = new array<string>();

	void VB_TabletKillGroup(string kind = "", array<string> classes = null)
	{
		Kind = kind;
		if (classes)
			Classes.Copy(classes);
	}
}

class VB_TabletDenom
{
	string Item;
	int Value;

	void VB_TabletDenom(string cls = "", int value = 0)
	{
		Item = cls;
		Value = value;
	}
}

// Тип контракта: Need это сколько сделать на обычной, сложной и элитной сложности, 0 значит такой сложности нет.
// ClanMult: во сколько раз больше у клана. Kind: zombie, mutant, animal, skin, km, car, hours, fish, wood, bush, rock,
// craft, repair, build, fire или Kind из KillGroups и LootGroups. Title и Hint: ключ с # или обычный текст.
class VB_TabletContractType
{
	string Kind;
	string Ico;
	string Title;
	string Hint;
	ref array<int> Need = new array<int>();
	int ClanMult = 3;

	void VB_TabletContractType(string kind = "", string ico = "", int need0 = 0, int need1 = 0, int need2 = 0, int mult = 3)
	{
		Kind = kind;
		Ico = ico;
		ClanMult = mult;
		if (kind == "")
			return;
		Need.Insert(need0);
		Need.Insert(need1);
		Need.Insert(need2);
		Title = "#STR_VB_TAB_CK_" + kind;
		Hint = "#STR_VB_TAB_CH_" + kind;
	}
}

// Один контракт одной сложности, собирается из типа.
class VB_TabletContractDef
{
	string Code;
	int Tier;
	string Kind;
	int Need;
	int ClanMult;
	string Ico;
	string Title;
	string Hint;
}

class VB_TabletConfig
{
	string ServerName = "DayZ Server";
	ref array<string> Admins = new array<string>();	// Steam64 администраторов планшета
	ref array<string> Zombies = {"ZombieBase"};		// классы для счётчиков, с наследниками
	ref array<string> Mutants = new array<string>();
	ref array<string> Animals = {"AnimalBase"};
	ref VB_TabletRates Rates = new VB_TabletRates();
	int ClanPrice = 1000000;
	int ClanRank = 2;			// индекс ранга, с которого можно создать клан: 2 это «Следопыт»
	int MaxClans = 20;
	int ClanMaxMembers = 20;
	int BankMax = 1000000;
	int ClanTax = 5000;
	int TaxFreeDays = 7;
	int PriceName = 25000;
	int PriceTag = 15000;
	int PriceEmblem = 20000;
	int PriceColor = 5000;
	int ClanContractHours = 24;	// как часто меняются клановые контракты, отсчёт от 00:00
	int MyContractHours = 12;	// как часто меняются личные
	ref array<ref VB_TabletKillGroup> KillGroups = new array<ref VB_TabletKillGroup>();
	ref array<string> Bosses = new array<string>();	// кого считает достижение «Убийца гигантов»
	ref array<string> BannedWords = new array<string>();
	ref array<int> WeekPrizes = new array<int>();	// награда на склад за 1, 2, 3 место недели
	// Купюры. При INT_Banking на сервере берутся валюты банка (его INT_BankingConfig.json), этот список только без банка.
	ref array<ref VB_TabletDenom> Money = new array<ref VB_TabletDenom>();
	ref VB_MapLimits MapLimits = new VB_MapLimits();
	int TaxWarnDays = 3;		// предупреждение о налоге при входе: дней до роспуска
	int NameTags = 1;			// ники над головами: ник, эмблема клана, ранг
	float NameTagRange = 15;	// с какого расстояния виден ник, метры (3..50)
	ref array<int> Levels;		// пороги влияния: Levels[n-1] открывает уровень n
	ref array<int> XpMy;		// опыт за личный контракт по сложности
	ref array<ref VB_TabletContractType> Contracts;
	ref array<int> RewardMy;	// влияние за личный контракт по сложности
	ref array<int> RewardClan;	// влияние за клановый
	ref array<int> CashClan;	// деньги на склад за клановый
	ref array<ref VB_TabletKillGroup> LootGroups = new array<ref VB_TabletKillGroup>();

	[NonSerialized()]
	bool Broken;	// файл с ошибкой: значения по умолчанию, файл не перезаписываем
	[NonSerialized()]
	protected static ref VB_TabletConfig s_Instance;

	static const string DIR_ROOT = "$profile:VB_Mods";
	static const string DIR_MOD = "$profile:VB_Mods/VB_Tablet";
	static const string FILE_CONFIG = "$profile:VB_Mods/VB_Tablet/config.json";

	// Подставляет то, чего нет в файле. true: что-то добавлено, файл стоит переписать.
	bool FillDefaults()
	{
		bool added = false;
		if (!Bosses || Bosses.Count() == 0)
		{
			Bosses = {"MSFZ_Ricar*", "MSFZ_Troll*", "SFP_Trupojor*", "SFP_BearZMB*"};
			added = true;
		}
		if (!KillGroups || KillGroups.Count() == 0)
		{
			KillGroups = new array<ref VB_TabletKillGroup>();
			DefaultKillGroups();
			added = true;
		}
		if (!LootGroups || LootGroups.Count() == 0)
		{
			LootGroups = new array<ref VB_TabletKillGroup>();
			DefaultLootGroups();
			added = true;
		}
		if (!Mutants || Mutants.Count() == 0)
		{
			Mutants = {"SFP_Trupojor", "SFP_BearZMB", "SFP_Tenevnik", "SFP_Shum*", "SFP_MutaWolf*", "SFP_Piyavka*", "SFP_Shipastiy*", "SFP_MozgoTras*", "MSFZ_Ricar*", "MSFZ_Troll*", "MSFZ_Mutant_*"};
			added = true;
		}
		if (!RewardMy || RewardMy.Count() != 3)
		{
			RewardMy = {10, 25, 60};
			added = true;
		}
		if (!RewardClan || RewardClan.Count() != 3)
		{
			RewardClan = {30, 120, 400};
			added = true;
		}
		if (!CashClan || CashClan.Count() != 3)
		{
			CashClan = {1000, 5000, 15000};
			added = true;
		}
		if (!WeekPrizes || WeekPrizes.Count() == 0)
		{
			WeekPrizes = new array<int>();
			WeekPrizes.Insert(100000);
			WeekPrizes.Insert(60000);
			WeekPrizes.Insert(30000);
			added = true;
		}
		if (!Money || Money.Count() == 0)
		{
			Money = new array<ref VB_TabletDenom>();
			Money.Insert(new VB_TabletDenom("MoneyRuble1", 1));
			Money.Insert(new VB_TabletDenom("Bablo5Rubley", 5));
			Money.Insert(new VB_TabletDenom("Bablo10Rubley", 10));
			Money.Insert(new VB_TabletDenom("MoneyRuble25", 25));
			Money.Insert(new VB_TabletDenom("Bablo50Rubley", 50));
			Money.Insert(new VB_TabletDenom("Bablo100Rubley", 100));
			Money.Insert(new VB_TabletDenom("Bablo200Rubley", 200));
			Money.Insert(new VB_TabletDenom("Bablo500Rubley", 500));
			Money.Insert(new VB_TabletDenom("Bablo1000Rubley", 1000));
			Money.Insert(new VB_TabletDenom("Bablo2000Rubley", 2000));
			Money.Insert(new VB_TabletDenom("Bablo5000Rubley", 5000));
			added = true;
		}
		if (!Levels || Levels.Count() != VB_CLAN_LEVELS)
		{
			Levels = new array<int>();
			Levels.Insert(0);
			Levels.Insert(1000);
			Levels.Insert(2000);
			Levels.Insert(5000);
			Levels.Insert(10000);
			added = true;
		}
		if (!XpMy || XpMy.Count() != 3)
		{
			XpMy = new array<int>();
			XpMy.Insert(10);
			XpMy.Insert(25);
			XpMy.Insert(50);
			added = true;
		}
		if (!Contracts || Contracts.Count() == 0)
		{
			Contracts = new array<ref VB_TabletContractType>();
			DefaultContracts();
			added = true;
		}
		return added;
	}

	// Весь список. Контракт на то, чего нет на сервере, не выдаётся: группы проверяются по классам при запуске.
	protected void DefaultContracts()
	{
		Contracts.Insert(new VB_TabletContractType("zombie", "zombie", 15, 40, 100, 3));
		Contracts.Insert(new VB_TabletContractType("mutant", "crosshair", 3, 10, 25, 3));
		Contracts.Insert(new VB_TabletContractType("animal", "sym_paw", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("skin", "sym_axe", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("fish", "fish", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("wood", "sym_tree", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("bush", "sym_leaf", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("rock", "sym_mountain", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("craft", "sym_cog", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("repair", "sym_hammer", 1, 3, 6, 3));
		Contracts.Insert(new VB_TabletContractType("build", "base", 2, 6, 12, 3));
		Contracts.Insert(new VB_TabletContractType("fire", "sym_flame", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("z_convoy", "zombie", 3, 8, 15, 2));
		Contracts.Insert(new VB_TabletContractType("l_convoy", "stash", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("l_air", "vault", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("l_koth", "trophy", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("l_card1", "key", 2, 0, 0, 2));
		Contracts.Insert(new VB_TabletContractType("l_card2", "key", 0, 2, 0, 2));
		Contracts.Insert(new VB_TabletContractType("l_card3", "key", 0, 0, 2, 2));
		Contracts.Insert(new VB_TabletContractType("l_room", "sym_key", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("z_era", "zombie", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("z_army", "zombie", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("z_police", "zombie", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("z_medic", "zombie", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("z_worker", "zombie", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("z_hiker", "zombie", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("z_fire", "zombie", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("z_prison", "zombie", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("z_priest", "zombie", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("z_nbc", "zombie", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_wolf", "sym_wolf", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_shum", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_piyavka", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_tenev", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_ship", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_mozg", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_zanoza", "crosshair", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("m_spider", "crosshair", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("m_trup", "crosshair", 1, 3, 5, 2));
		Contracts.Insert(new VB_TabletContractType("m_bear", "crosshair", 1, 3, 5, 2));
		Contracts.Insert(new VB_TabletContractType("m_ricar", "crosshair", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("m_troll", "crosshair", 1, 2, 3, 2));
		Contracts.Insert(new VB_TabletContractType("a_wolf", "sym_wolf", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("a_bear", "sym_paw", 1, 2, 4, 2));
		Contracts.Insert(new VB_TabletContractType("a_boar", "sym_paw", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("a_pig", "sym_paw", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("a_deer", "sym_paw", 2, 4, 8, 3));
		Contracts.Insert(new VB_TabletContractType("a_roe", "sym_paw", 2, 4, 8, 3));
		Contracts.Insert(new VB_TabletContractType("a_fox", "sym_paw", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("a_hare", "sym_paw", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("a_farm", "sym_paw", 2, 4, 8, 3));
	}

	protected void DefaultKillGroups()
	{
		KillGroups.Insert(new VB_TabletKillGroup("z_era", {"ERA_*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_convoy", {"ZmbM_Convoy_*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_army", {"ZmbM_SoldierNormal*", "ZmbF_SoldierNormal*", "ZmbM_usSoldier*", "ZmbF_usSoldier*", "ZmbM_eastSoldier*", "ZmbF_eastSoldier*", "ZmbM_PatrolNormal*", "ZmbF_ArmyOfficer*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_police", {"ZmbM_PolicemanFat*", "ZmbM_PolicemanSpecForce*", "ZmbF_PoliceWomanNormal*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_medic", {"ZmbM_DoctorFat*", "ZmbF_DoctorSkinny*", "ZmbF_NurseFat*", "ZmbM_ParamedicNormal*", "ZmbF_ParamedicNormal*", "ZmbM_PatientSkinny*", "ZmbF_PatientOld*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_worker", {"ZmbM_ConstrWorkerNormal*", "ZmbM_HandymanNormal*", "ZmbM_HeavyIndustryWorker*", "ZmbM_MechanicSkinny*", "ZmbF_MechanicNormal*", "ZmbM_OffshoreWorker*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_hiker", {"ZmbM_HunterOld*", "ZmbM_HikerSkinny*", "ZmbF_HikerSkinny*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_fire", {"ZmbM_FirefighterNormal*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_prison", {"ZmbM_PrisonerSkinny*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_priest", {"ZmbM_priestPopSkinny*"}));
		KillGroups.Insert(new VB_TabletKillGroup("z_nbc", {"ZmbM_NBC*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_wolf", {"SFP_MutaWolf*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_shum", {"SFP_Shum*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_piyavka", {"SFP_Piyavka*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_tenev", {"SFP_Tenevnik*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_ship", {"SFP_Shipastiy*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_mozg", {"SFP_MozgoTras*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_zanoza", {"MSFZ_Mutant_Zanoza*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_spider", {"MSFZ_Mutant_Spider*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_trup", {"SFP_Trupojor*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_bear", {"SFP_BearZMB*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_ricar", {"MSFZ_Ricar*"}));
		KillGroups.Insert(new VB_TabletKillGroup("m_troll", {"MSFZ_Troll*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_wolf", {"Animal_CanisLupus*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_bear", {"Animal_UrsusArctos*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_boar", {"Animal_SusScrofa*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_pig", {"Animal_SusDomesticus*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_deer", {"Animal_CervusElaphus*", "Animal_RangiferTarandus*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_roe", {"Animal_CapreolusCapreolus*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_fox", {"Animal_VulpesVulpes*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_hare", {"Animal_LepusEuropaeus*"}));
		KillGroups.Insert(new VB_TabletKillGroup("a_farm", {"Animal_OvisAries*", "Animal_CapraHircus*", "Animal_BosTaurus*"}));
	}

	protected void DefaultLootGroups()
	{
		LootGroups.Insert(new VB_TabletKillGroup("l_convoy", {"CrateForConvoy"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_air", {"AirdropContainer*"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_koth", {"KothPoint"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_card1", {"DNA_Crate_Yellow", "DNA_Crate_Green", "DNA_WoodCrate_Yellow", "DNA_WoodCrate_Green"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_card2", {"DNA_Crate_Blue", "DNA_WoodCrate_Blue"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_card3", {"DNA_Crate_Purple", "DNA_Crate_Red", "DNA_WoodCrate_Purple", "DNA_WoodCrate_Red"}));
		LootGroups.Insert(new VB_TabletKillGroup("l_room", {"DNA_StorageProxy_*"}));
	}

	// Запись файла после правки с экрана администратора.
	bool Store()
	{
		if (Broken)
			return false;
		string error;
		if (!VB_SafeJson<VB_TabletConfig>.Save(FILE_CONFIG, this, error))
		{
			Print(VB_TABLET_TAG + " не записан конфиг: " + error);
			return false;
		}
		return true;
	}

	static VB_TabletConfig Get()
	{
		if (!s_Instance)
			Load();
		return s_Instance;
	}

	static void Load()
	{
		s_Instance = new VB_TabletConfig();
		s_Instance.FillDefaults();
		VB_TabletRates.SetLevels(s_Instance.Levels);
		MakeDirectory(DIR_ROOT);
		MakeDirectory(DIR_MOD);
		string error;
		if (!FileExist(FILE_CONFIG))
		{
			if (!JsonFileLoader<VB_TabletConfig>.SaveFile(FILE_CONFIG, s_Instance, error))
				Print(VB_TABLET_TAG + " не записан конфиг: " + error);
			return;
		}
		VB_TabletConfig data = new VB_TabletConfig();
		if (!JsonFileLoader<VB_TabletConfig>.LoadFile(FILE_CONFIG, data, error))
		{
			// Сломанный файл не перезаписываем: админ мог ошибиться в одной запятой.
			// Если есть копия (её оставляет запись с экрана администратора), работаем по ней до исправления.
			Print(VB_TABLET_TAG + " ОШИБКА в конфиге: " + error);
			data = new VB_TabletConfig();
			string bakError;
			if (!JsonFileLoader<VB_TabletConfig>.LoadFile(FILE_CONFIG + ".bak", data, bakError))
			{
				Print(VB_TABLET_TAG + " копии конфига нет или она тоже с ошибкой, работаем со значениями по умолчанию");
				s_Instance.Broken = true;
				return;
			}
			Print(VB_TABLET_TAG + " конфиг взят из копии config.json.bak, исправьте config.json");
			data.Broken = true;
		}
		if (!data.Admins)
			data.Admins = new array<string>();
		if (!data.Zombies)
			data.Zombies = new array<string>();
		if (!data.Mutants)
			data.Mutants = new array<string>();
		if (!data.Animals)
			data.Animals = new array<string>();
		if (!data.BannedWords)
			data.BannedWords = new array<string>();
		if (!data.Rates || !data.Rates.RankNeed || data.Rates.RankNeed.Count() != VB_RANK_COUNT)
			data.Rates = new VB_TabletRates();
		if (!data.MapLimits)
			data.MapLimits = new VB_MapLimits();
		bool filled = data.FillDefaults();
		s_Instance = data;
		VB_TabletRates.SetLevels(data.Levels);
		// В старом файле нет новых разделов: дописываем, чтобы их было где править.
		if (filled && !data.Broken)
			data.Store();
		Print(VB_TABLET_TAG + " конфиг загружен, администраторов " + data.Admins.Count());
	}

	bool IsAdmin(string uid)
	{
		return Admins.Find(uid) >= 0;
	}
}
