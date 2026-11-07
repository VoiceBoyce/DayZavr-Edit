// Настройки сервера: $profile:VB_Mods/VB_Tablet/config.json, на боевом это Instance_1\\VB_Mods\\VB_Tablet\\.
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

// Запись списка контрактов. Kind: zombie, mutant, animal, km, hours. Tier: 0 обычный, 1 сложный, 2 элитный.
// Title и Hint: ключ таблицы строк с # или обычный текст.
class VB_TabletContractDef
{
	string Code;
	int Tier;
	string Kind;
	int Need;
	int Reward;
	int Cash;
	string Ico;
	string Title;
	string Hint;

	void VB_TabletContractDef(string code = "", int tier = 0, string kind = "", int need = 1, int reward = 0, int cash = 0, string ico = "contract", string title = "", string hint = "")
	{
		Code = code;
		Tier = tier;
		Kind = kind;
		Need = need;
		Reward = reward;
		Cash = cash;
		Ico = ico;
		Title = title;
		Hint = hint;
	}
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
	ref array<string> BannedWords = new array<string>();
	ref array<int> WeekPrizes = new array<int>();	// награда на склад за 1, 2, 3 место недели
	ref array<ref VB_TabletDenom> Money = new array<ref VB_TabletDenom>();
	ref VB_MapLimits MapLimits = new VB_MapLimits();
	int TaxWarnDays = 3;		// предупреждение о налоге при входе: дней до роспуска
	float AtmDist = 4;			// с какого расстояния работает банкомат, метры
	ref array<int> Levels;		// пороги влияния: Levels[n-1] открывает уровень n
	ref array<int> XpMy;		// опыт за личный контракт по сложности
	ref array<ref VB_TabletContractDef> ClanContracts;
	ref array<ref VB_TabletContractDef> MyContracts;

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
		if (!ClanContracts || ClanContracts.Count() == 0)
		{
			ClanContracts = new array<ref VB_TabletContractDef>();
			DefaultClanContracts();
			added = true;
		}
		if (!MyContracts || MyContracts.Count() == 0)
		{
			MyContracts = new array<ref VB_TabletContractDef>();
			DefaultMyContracts();
			added = true;
		}
		return added;
	}

	// Примеры из макета. Настоящий список задаёт владелец сервера.
	protected void DefaultClanContracts()
	{
		ClanContracts.Insert(new VB_TabletContractDef("cl_zombie30", 0, "zombie", 30, 60, 0, "zombie", "#STR_VB_TAB_C_CT_SWEEP", "#STR_VB_TAB_C_CH_ZOMBIE"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_km5", 0, "km", 5, 40, 5000, "route", "#STR_VB_TAB_C_CT_TRIP", "#STR_VB_TAB_C_CH_KM"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_animal5", 0, "animal", 5, 50, 0, "crosshair", "#STR_VB_TAB_C_CT_HUNT", "#STR_VB_TAB_C_CH_ANIMAL"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_zombie15", 0, "zombie", 15, 30, 0, "zombie", "#STR_VB_TAB_C_CT_CLEAN", "#STR_VB_TAB_C_CH_ZOMBIE"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_km3", 0, "km", 3, 30, 2000, "route", "#STR_VB_TAB_C_CT_WALK", "#STR_VB_TAB_C_CH_KM"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_animal3", 0, "animal", 3, 30, 0, "stash", "#STR_VB_TAB_C_CT_PREY", "#STR_VB_TAB_C_CH_ANIMAL"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_hours4", 1, "hours", 4, 120, 0, "heart", "#STR_VB_TAB_C_CT_SURVIVE", "#STR_VB_TAB_C_CH_HOURS"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_mutant5", 1, "mutant", 5, 200, 10000, "crosshair", "#STR_VB_TAB_C_CT_FIGHTER", "#STR_VB_TAB_C_CH_MUTANT"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_km10", 1, "km", 10, 150, 0, "route", "#STR_VB_TAB_C_CT_ROAD", "#STR_VB_TAB_C_CH_KM"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_hours12", 2, "hours", 12, 400, 25000, "heart", "#STR_VB_TAB_C_CT_IRON", "#STR_VB_TAB_C_CH_HOURS"));
		ClanContracts.Insert(new VB_TabletContractDef("cl_mutant25", 2, "mutant", 25, 500, 40000, "crosshair", "#STR_VB_TAB_C_CT_THREAT", "#STR_VB_TAB_C_CH_MUTANT"));
	}

	protected void DefaultMyContracts()
	{
		MyContracts.Insert(new VB_TabletContractDef("my_zombie15", 0, "zombie", 15, 10, 0, "zombie", "#STR_VB_TAB_C_CT_CLEAN", "#STR_VB_TAB_C_CH_ZOMBIE"));
		MyContracts.Insert(new VB_TabletContractDef("my_km3", 0, "km", 3, 10, 0, "route", "#STR_VB_TAB_C_CT_WALK", "#STR_VB_TAB_C_CH_KM"));
		MyContracts.Insert(new VB_TabletContractDef("my_animal3", 0, "animal", 3, 10, 0, "stash", "#STR_VB_TAB_C_CT_PREY", "#STR_VB_TAB_C_CH_ANIMAL"));
		MyContracts.Insert(new VB_TabletContractDef("my_animal5", 0, "animal", 5, 10, 0, "crosshair", "#STR_VB_TAB_C_CT_HUNT", "#STR_VB_TAB_C_CH_ANIMAL"));
		MyContracts.Insert(new VB_TabletContractDef("my_km10", 1, "km", 10, 25, 0, "route", "#STR_VB_TAB_C_CT_ROAD", "#STR_VB_TAB_C_CH_KM"));
		MyContracts.Insert(new VB_TabletContractDef("my_mutant3", 1, "mutant", 3, 25, 0, "crosshair", "#STR_VB_TAB_C_CT_MEET", "#STR_VB_TAB_C_CH_MUTANT"));
		MyContracts.Insert(new VB_TabletContractDef("my_hours2", 1, "hours", 2, 25, 0, "heart", "#STR_VB_TAB_C_CT_HOLD", "#STR_VB_TAB_C_CH_HOURS"));
		MyContracts.Insert(new VB_TabletContractDef("my_mutant10", 2, "mutant", 10, 60, 0, "crosshair", "#STR_VB_TAB_C_CT_PACK", "#STR_VB_TAB_C_CH_MUTANT"));
		MyContracts.Insert(new VB_TabletContractDef("my_km25", 2, "km", 25, 60, 0, "route", "#STR_VB_TAB_C_CT_MARATHON", "#STR_VB_TAB_C_CH_KM"));
	}

	// Запись файла после правки с экрана администратора.
	bool Store()
	{
		if (Broken)
			return false;
		string error;
		if (!JsonFileLoader<VB_TabletConfig>.SaveFile(FILE_CONFIG, this, error))
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
			Print(VB_TABLET_TAG + " ошибка в конфиге, работаем со значениями по умолчанию: " + error);
			s_Instance.Broken = true;
			return;
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
		if (filled)
			data.Store();
		Print(VB_TABLET_TAG + " конфиг загружен, администраторов " + data.Admins.Count());
	}

	bool IsAdmin(string uid)
	{
		return Admins.Find(uid) >= 0;
	}
}
