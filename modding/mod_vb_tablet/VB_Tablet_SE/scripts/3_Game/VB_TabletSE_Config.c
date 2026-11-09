// Настройки сервера: папка $profile:VB_Mods/VB_Tablet/config/ (файлы разделов в VB_TabletSE_ConfigFiles.c).
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

// Действие игрока из любого мода. Actions: имена классов действий (ActionGoldMine). Targets, если не пусто:
// цель действия должна быть из этих классов (или начало имени со звёздочкой). Засчитывается каждое
// завершение длительного действия (у действий с повтором каждый круг). PerTarget 1: одна и та же цель
// засчитывается игроку один раз и его клану один раз, как ящик ивента (например, «разные жилы»).
class VB_TabletActionGroup
{
	string Kind;
	ref array<string> Actions = new array<string>();
	ref array<string> Targets = new array<string>();
	int PerTarget;

	void VB_TabletActionGroup(string kind = "", array<string> actions = null, array<string> targets = null, int perTarget = 0)
	{
		Kind = kind;
		PerTarget = perTarget;
		if (actions)
			Actions.Copy(actions);
		if (targets)
			Targets.Copy(targets);
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
// ClanMult: во сколько раз больше у клана. Kind: zombie, mutant, animal, skin, fish, wood, bush, rock,
// craft, repair, build, fire или Kind из KillGroups, LootGroups, ActionGroups, ItemGroups и CraftGroups. Title и Hint: ключ с # или обычный текст.
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

// Все настройки сервера в одном объекте: остальной код читает их отсюда, как раньше.
// Поля заполняются из файлов разделов в папке config/.
class VB_TabletConfig
{
	// general.json
	string ServerName = "DayZ Server";
	ref array<string> Admins = new array<string>();	// Steam64 администраторов планшета
	int NameTags = 1;			// ники над головами: ник, эмблема клана, ранг
	float NameTagRange = 15;	// с какого расстояния виден ник, метры (3..50)
	ref VB_MapLimits MapLimits = new VB_MapLimits();
	// Купюры. При INT_Banking на сервере берутся валюты банка (его INT_BankingConfig.json), этот список только без банка.
	ref array<ref VB_TabletDenom> Money = new array<ref VB_TabletDenom>();
	// clans.json
	int ClanPrice = 1000000;
	int ClanRank = 2;			// индекс ранга, с которого можно создать клан: 2 это «Следопыт»
	int MaxClans = 20;
	int ClanMaxMembers = 20;
	int BankMax = 1000000;
	int ClanTax = 5000;
	int TaxFreeDays = 7;
	int TaxWarnDays = 3;		// предупреждение о налоге при входе: дней до роспуска
	int PriceName = 25000;
	int PriceTag = 15000;
	int PriceEmblem = 20000;
	int PriceColor = 5000;
	ref array<int> Levels;		// пороги влияния: Levels[n-1] открывает уровень n
	ref array<int> WeekPrizes;	// награда на склад за 1, 2, 3 место недели
	// ranks.json
	ref VB_TabletRates Rates = new VB_TabletRates();
	// contracts.json
	int ClanContractHours = 24;
	int MyContractHours = 12;
	ref array<int> XpMy;
	ref array<int> RewardMy;
	ref array<int> RewardClan;
	ref array<int> CashClan;
	ref array<string> Zombies;
	ref array<string> Mutants;
	ref array<string> Animals;
	ref array<string> Bosses;
	ref array<ref VB_TabletKillGroup> KillGroups;
	ref array<ref VB_TabletKillGroup> LootGroups;
	ref array<ref VB_TabletActionGroup> ActionGroups;
	ref array<ref VB_TabletKillGroup> ItemGroups;
	ref array<ref VB_TabletKillGroup> CraftGroups;
	ref array<ref VB_TabletContractType> Contracts;
	// banned_words.json
	ref array<string> BannedWords = new array<string>();
	bool BannedBroken;	// файл слов с ошибкой: админка его не перезаписывает

	protected static ref VB_TabletConfig s_Instance;

	static const string DIR_ROOT = "$profile:VB_Mods";
	static const string DIR_MOD = "$profile:VB_Mods/VB_Tablet";
	static const string DIR_CFG = "$profile:VB_Mods/VB_Tablet/config";
	static const string FILE_OLD = "$profile:VB_Mods/VB_Tablet/config.json";
	static const string FILE_BANNED = "$profile:VB_Mods/VB_Tablet/config/banned_words.json";

	// Запрещённые слова после правки с экрана администратора. Остальные файлы сервер не перезаписывает.
	bool Store()
	{
		if (BannedBroken)
			return false;
		VB_CfgBanned data = new VB_CfgBanned();
		data.BannedWords = BannedWords;
		string error;
		if (!VB_SafeJson<VB_CfgBanned>.Save(FILE_BANNED, data, error))
		{
			Print(VB_TABLET_TAG + " не записан config/banned_words.json: " + error);
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

	// Поля, которых не было в файле (или с неверным числом значений): взяты по умолчанию, файл не менялся.
	protected static void Report(string file, array<string> filled)
	{
		if (filled.Count() == 0)
			return;
		string list = "";
		foreach (string f : filled)
		{
			if (list != "")
				list += ", ";
			list += f;
		}
		Print(VB_TABLET_TAG + " config/" + file + ": по умолчанию взято " + list);
	}

	static void Load()
	{
		MakeDirectory(DIR_ROOT);
		MakeDirectory(DIR_MOD);
		MakeDirectory(DIR_CFG);
		VB_TabletConfig c = new VB_TabletConfig();
		c.LoadGeneral();
		c.LoadClans();
		c.LoadRanks();
		c.LoadContracts();
		c.LoadAchievements();
		c.LoadBanned();
		s_Instance = c;
		VB_TabletRates.SetLevels(c.Levels);
		if (FileExist(FILE_OLD))
			Print(VB_TABLET_TAG + " config.json больше не читается: настройки в папке config/, старый файл можно удалить");
		Print(VB_TABLET_TAG + " конфиг загружен, администраторов " + c.Admins.Count());
	}

	protected void LoadGeneral()
	{
		string name = "general.json";
		VB_CfgGeneral d = new VB_CfgGeneral();
		int st = VB_CfgFile<VB_CfgGeneral>.Read(name, d);
		array<string> filled = new array<string>();
		if (st != VB_CFG_OK)
			d = new VB_CfgGeneral();
		d.Fill(filled);
		if (st == VB_CFG_MISSING)
			VB_CfgFile<VB_CfgGeneral>.Write(name, d);
		else if (st == VB_CFG_OK)
			Report(name, filled);
		ServerName = d.ServerName;
		Admins = d.Admins;
		NameTags = d.NameTags;
		NameTagRange = d.NameTagRange;
		MapLimits = d.MapLimits;
		Money = d.Money;
	}

	protected void LoadClans()
	{
		string name = "clans.json";
		VB_CfgClans d = new VB_CfgClans();
		int st = VB_CfgFile<VB_CfgClans>.Read(name, d);
		array<string> filled = new array<string>();
		if (st != VB_CFG_OK)
			d = new VB_CfgClans();
		d.Fill(filled);
		if (st == VB_CFG_MISSING)
			VB_CfgFile<VB_CfgClans>.Write(name, d);
		else if (st == VB_CFG_OK)
			Report(name, filled);
		ClanPrice = d.ClanPrice;
		ClanRank = d.ClanRank;
		MaxClans = d.MaxClans;
		ClanMaxMembers = d.ClanMaxMembers;
		BankMax = d.BankMax;
		ClanTax = d.ClanTax;
		TaxFreeDays = d.TaxFreeDays;
		TaxWarnDays = d.TaxWarnDays;
		PriceName = d.PriceName;
		PriceTag = d.PriceTag;
		PriceEmblem = d.PriceEmblem;
		PriceColor = d.PriceColor;
		Levels = d.Levels;
		WeekPrizes = d.WeekPrizes;
	}

	protected void LoadRanks()
	{
		string name = "ranks.json";
		VB_TabletRates d = new VB_TabletRates();
		int st = VB_CfgFile<VB_TabletRates>.Read(name, d);
		array<string> filled = new array<string>();
		if (st != VB_CFG_OK || !d.RankNeed || d.RankNeed.Count() != VB_RANK_COUNT)
		{
			if (st == VB_CFG_OK)
				filled.Insert("RankNeed (нужно " + VB_RANK_COUNT.ToString() + " значений)");
			d = new VB_TabletRates();
		}
		if (st == VB_CFG_MISSING)
			VB_CfgFile<VB_TabletRates>.Write(name, d);
		else if (st == VB_CFG_OK)
			Report(name, filled);
		Rates = d;
	}

	protected void LoadContracts()
	{
		string name = "contracts.json";
		VB_CfgContracts d = new VB_CfgContracts();
		int st = VB_CfgFile<VB_CfgContracts>.Read(name, d);
		array<string> filled = new array<string>();
		if (st != VB_CFG_OK)
			d = new VB_CfgContracts();
		d.Fill(filled);
		if (st == VB_CFG_MISSING)
			d.WriteFile(name);
		else if (st == VB_CFG_OK)
			Report(name, filled);
		ClanContractHours = d.ClanContractHours;
		MyContractHours = d.MyContractHours;
		XpMy = d.XpMy;
		RewardMy = d.RewardMy;
		RewardClan = d.RewardClan;
		CashClan = d.CashClan;
		Zombies = d.Zombies;
		Mutants = d.Mutants;
		Animals = d.Animals;
		Bosses = d.Bosses;
		KillGroups = d.KillGroups;
		LootGroups = d.LootGroups;
		ActionGroups = d.ActionGroups;
		ItemGroups = d.ItemGroups;
		CraftGroups = d.CraftGroups;
		Contracts = d.Contracts;
	}

	protected void LoadAchievements()
	{
		string name = "achievements.json";
		VB_CfgAchievements d = new VB_CfgAchievements();
		int st = VB_CfgFile<VB_CfgAchievements>.Read(name, d);
		array<string> filled = new array<string>();
		if (st != VB_CFG_OK)
		{
			d = new VB_CfgAchievements();
			d.Defaults();
		}
		d.Apply(filled);
		if (st == VB_CFG_MISSING)
			d.WriteFile(name);
		else if (st == VB_CFG_OK)
			Report(name, filled);
	}

	protected void LoadBanned()
	{
		string name = "banned_words.json";
		VB_CfgBanned d = new VB_CfgBanned();
		int st = VB_CfgFile<VB_CfgBanned>.Read(name, d);
		if (st != VB_CFG_OK || !d.BannedWords)
			d = new VB_CfgBanned();
		if (st == VB_CFG_MISSING)
			VB_CfgFile<VB_CfgBanned>.Write(name, d);
		BannedBroken = st == VB_CFG_BROKEN;
		BannedWords = d.BannedWords;
	}

	bool IsAdmin(string uid)
	{
		return Admins.Find(uid) >= 0;
	}
}
