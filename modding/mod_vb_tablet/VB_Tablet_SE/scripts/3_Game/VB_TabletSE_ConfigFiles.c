// Файлы настроек в папке $profile:VB_Mods/VB_Tablet/config/ (на боевом Instance_1\VB_Mods\VB_Tablet\config\).
// Каждый раздел читается сам по себе: файла нет — создаётся со значениями по умолчанию; ошибка в файле —
// раздел работает по умолчанию, файл не перезаписывается. Сервер пишет только banned_words.json (админка).

const int VB_CFG_OK = 0;
const int VB_CFG_MISSING = 1;	// файла нет
const int VB_CFG_BROKEN = 2;	// ошибка в файле

// Чтение одного файла раздела.
class VB_CfgFile<Class T>
{
	static int Read(string name, T data)
	{
		string path = VB_TabletConfig.DIR_CFG + "/" + name;
		if (!FileExist(path))
			return VB_CFG_MISSING;
		string error;
		if (!JsonFileLoader<T>.LoadFile(path, data, error))
		{
			Print(VB_TABLET_TAG + " ОШИБКА в config/" + name + ": " + error + ". Раздел работает по умолчанию, файл не перезаписан");
			return VB_CFG_BROKEN;
		}
		return VB_CFG_OK;
	}

	static void Write(string name, T data)
	{
		string error;
		if (JsonFileLoader<T>.SaveFile(VB_TabletConfig.DIR_CFG + "/" + name, data, error))
			Print(VB_TABLET_TAG + " создан config/" + name);
		else
			Print(VB_TABLET_TAG + " не записан config/" + name + ": " + error);
	}
}

// Запись JSON по одной строке на запись (контракты, группы, достижения). Читается обычным JsonFileLoader.
class VB_CfgWriter
{
	protected FileHandle m_File;
	protected string m_Name;

	bool Open(string name)
	{
		m_Name = name;
		m_File = OpenFile(VB_TabletConfig.DIR_CFG + "/" + name, FileMode.WRITE);
		if (!m_File)
		{
			Print(VB_TABLET_TAG + " не записан config/" + name);
			return false;
		}
		return true;
	}

	void Line(string s)
	{
		FPrintln(m_File, s);
	}

	void Close()
	{
		CloseFile(m_File);
		Print(VB_TABLET_TAG + " создан config/" + m_Name);
	}

	// Запятая после всех элементов, кроме последнего.
	static string Sep(int i, int count)
	{
		if (i < count - 1)
			return ",";
		return "";
	}

	static string Q(string s)
	{
		string r = s + "";
		r.Replace("\\", "\\\\");
		r.Replace("\"", "\\\"");
		return "\"" + r + "\"";
	}

	static string Strs(array<string> list)
	{
		string r = "[";
		if (list)
		{
			for (int i = 0; i < list.Count(); i++)
			{
				if (i > 0)
					r += ", ";
				r += Q(list.Get(i));
			}
		}
		return r + "]";
	}

	static string Ints(array<int> list)
	{
		string r = "[";
		if (list)
		{
			for (int i = 0; i < list.Count(); i++)
			{
				if (i > 0)
					r += ", ";
				r += list.Get(i).ToString();
			}
		}
		return r + "]";
	}
}

// general.json
class VB_CfgGeneral
{
	string Comment = "Admins: Steam64 администраторов планшета. NameTags 1: ники над головами, NameTagRange: дальность в метрах (3..50). MapLimits: лимиты меток, зон и маршрутов. Money: купюры склада (при INT_Banking берутся валюты банка).";
	string ServerName = "DayZ Server";
	ref array<string> Admins = {"76561200715668795"};	// по умолчанию в новом general.json
	int NameTags = 1;
	float NameTagRange = 15;
	ref VB_MapLimits MapLimits = new VB_MapLimits();
	ref array<ref VB_TabletDenom> Money = new array<ref VB_TabletDenom>();

	void Fill(array<string> filled)
	{
		if (!Admins)
			Admins = new array<string>();
		if (!MapLimits)
		{
			MapLimits = new VB_MapLimits();
			filled.Insert("MapLimits");
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
			filled.Insert("Money");
		}
	}
}

// clans.json
class VB_CfgClans
{
	string Comment = "ClanRank: индекс ранга, с которого можно создать клан (2 — «Следопыт»). ClanTax: налог в неделю, TaxFreeDays: льготные дни, TaxWarnDays: за сколько дней до роспуска предупреждать. Price*: цены смены названия, тега, эмблемы и цвета. Levels: влияние для уровней клана 1..5. WeekPrizes: награда на склад за 1, 2, 3 место недели.";
	int ClanPrice = 1000000;
	int ClanRank = 2;
	int MaxClans = 20;
	int ClanMaxMembers = 20;
	int BankMax = 1000000;
	int ClanTax = 5000;
	int TaxFreeDays = 7;
	int TaxWarnDays = 3;
	int PriceName = 25000;
	int PriceTag = 15000;
	int PriceEmblem = 20000;
	int PriceColor = 5000;
	ref array<int> Levels = new array<int>();
	ref array<int> WeekPrizes = new array<int>();

	void Fill(array<string> filled)
	{
		if (!Levels || Levels.Count() != VB_CLAN_LEVELS)
		{
			Levels = {0, 1000, 2000, 5000, 10000};
			filled.Insert("Levels");
		}
		if (!WeekPrizes || WeekPrizes.Count() == 0)
		{
			WeekPrizes = {100000, 60000, 30000};
			filled.Insert("WeekPrizes");
		}
	}
}

// banned_words.json: пишет админка планшета.
class VB_CfgBanned
{
	ref array<string> BannedWords = new array<string>();
}

// contracts.json: вверху настройки и группы «что считается», ниже список контрактов.
class VB_CfgContracts
{
	string Comment = "Вверху настройки и группы «что считается», ниже контракты. Классы со звёздочкой на конце: совпадение по началу имени. Контракт: Need — сколько сделать на обычной, сложной и элитной сложности (0 — такой сложности нет), ClanMult — во сколько раз больше у клана, Title и Hint необязательны (ключи строк по Kind). Контракт на то, чего нет на сервере, не выдаётся.";
	int ClanContractHours = 24;		// как часто меняются клановые контракты, отсчёт от 00:00
	int MyContractHours = 12;		// как часто меняются личные
	ref array<int> XpMy = new array<int>();			// опыт за личный контракт по сложности
	ref array<int> RewardMy = new array<int>();		// влияние за личный контракт
	ref array<int> RewardClan = new array<int>();	// влияние за клановый
	ref array<int> CashClan = new array<int>();		// деньги на склад за клановый
	ref array<string> Zombies = new array<string>();	// классы для счётчиков, с наследниками
	ref array<string> Mutants = new array<string>();
	ref array<string> Animals = new array<string>();
	ref array<string> Bosses = new array<string>();		// кого считает достижение «Убийца гигантов»
	ref array<ref VB_TabletKillGroup> KillGroups = new array<ref VB_TabletKillGroup>();
	ref array<ref VB_TabletKillGroup> LootGroups = new array<ref VB_TabletKillGroup>();
	ref array<ref VB_TabletActionGroup> ActionGroups = new array<ref VB_TabletActionGroup>();
	// «Найти предмет»: засчитывается первому, кто взял с земли или из чужого контейнера предмет этой сессии.
	ref array<ref VB_TabletKillGroup> ItemGroups = new array<ref VB_TabletKillGroup>();
	// «Скрафтить предмет» в DRL_CraftMenu, нужен аддон VB_Tablet_DRL.
	ref array<ref VB_TabletKillGroup> CraftGroups = new array<ref VB_TabletKillGroup>();
	ref array<ref VB_TabletContractType> Contracts = new array<ref VB_TabletContractType>();

	// JsonFileLoader создаёт пустой массив и для поля, которого нет в файле, поэтому пустой список
	// заменяется стандартным. Лишние виды отключаются удалением контрактов.
	void Fill(array<string> filled)
	{
		if (!XpMy || XpMy.Count() != 3)
		{
			XpMy = {10, 25, 50};
			filled.Insert("XpMy");
		}
		if (!RewardMy || RewardMy.Count() != 3)
		{
			RewardMy = {10, 25, 60};
			filled.Insert("RewardMy");
		}
		if (!RewardClan || RewardClan.Count() != 3)
		{
			RewardClan = {30, 120, 400};
			filled.Insert("RewardClan");
		}
		if (!CashClan || CashClan.Count() != 3)
		{
			CashClan = {1000, 5000, 15000};
			filled.Insert("CashClan");
		}
		if (!Zombies || Zombies.Count() == 0)
		{
			Zombies = {"ZombieBase"};
			filled.Insert("Zombies");
		}
		if (!Mutants || Mutants.Count() == 0)
		{
			Mutants = {"SFP_Trupojor", "SFP_BearZMB", "SFP_Tenevnik", "SFP_Shum*", "SFP_MutaWolf*", "SFP_Piyavka*", "SFP_Shipastiy*", "SFP_MozgoTras*", "MSFZ_Ricar*", "MSFZ_Troll*", "MSFZ_Mutant_*"};
			filled.Insert("Mutants");
		}
		if (!Animals || Animals.Count() == 0)
		{
			Animals = {"AnimalBase"};
			filled.Insert("Animals");
		}
		if (!Bosses || Bosses.Count() == 0)
		{
			Bosses = {"MSFZ_Ricar*", "MSFZ_Troll*", "SFP_Trupojor*", "SFP_BearZMB*"};
			filled.Insert("Bosses");
		}
		if (!KillGroups || KillGroups.Count() == 0)
		{
			KillGroups = new array<ref VB_TabletKillGroup>();
			DefaultKillGroups();
			filled.Insert("KillGroups");
		}
		if (!LootGroups || LootGroups.Count() == 0)
		{
			LootGroups = new array<ref VB_TabletKillGroup>();
			DefaultLootGroups();
			filled.Insert("LootGroups");
		}
		if (!ActionGroups || ActionGroups.Count() == 0)
		{
			ActionGroups = new array<ref VB_TabletActionGroup>();
			DefaultActionGroups();
			filled.Insert("ActionGroups");
		}
		if (!ItemGroups || ItemGroups.Count() == 0)
		{
			ItemGroups = new array<ref VB_TabletKillGroup>();
			DefaultItemGroups();
			filled.Insert("ItemGroups");
		}
		if (!CraftGroups || CraftGroups.Count() == 0)
		{
			CraftGroups = new array<ref VB_TabletKillGroup>();
			DefaultCraftGroups();
			filled.Insert("CraftGroups");
		}
		if (!Contracts || Contracts.Count() == 0)
		{
			Contracts = new array<ref VB_TabletContractType>();
			DefaultContracts();
			filled.Insert("Contracts");
		}
	}

	// Новый файл: одна строка на группу и на контракт.
	void WriteFile(string name)
	{
		VB_CfgWriter w = new VB_CfgWriter();
		if (!w.Open(name))
			return;
		w.Line("{");
		w.Line("    \"Comment\": " + VB_CfgWriter.Q(Comment) + ",");
		w.Line("    \"ClanContractHours\": " + ClanContractHours.ToString() + ",");
		w.Line("    \"MyContractHours\": " + MyContractHours.ToString() + ",");
		w.Line("    \"XpMy\": " + VB_CfgWriter.Ints(XpMy) + ",");
		w.Line("    \"RewardMy\": " + VB_CfgWriter.Ints(RewardMy) + ",");
		w.Line("    \"RewardClan\": " + VB_CfgWriter.Ints(RewardClan) + ",");
		w.Line("    \"CashClan\": " + VB_CfgWriter.Ints(CashClan) + ",");
		w.Line("    \"Zombies\": " + VB_CfgWriter.Strs(Zombies) + ",");
		w.Line("    \"Mutants\": " + VB_CfgWriter.Strs(Mutants) + ",");
		w.Line("    \"Animals\": " + VB_CfgWriter.Strs(Animals) + ",");
		w.Line("    \"Bosses\": " + VB_CfgWriter.Strs(Bosses) + ",");
		Groups(w, "KillGroups", KillGroups);
		Groups(w, "LootGroups", LootGroups);
		w.Line("    \"ActionGroups\": [");
		int n = ActionGroups.Count();
		for (int i = 0; i < n; i++)
		{
			VB_TabletActionGroup a = ActionGroups.Get(i);
			w.Line("        {\"Kind\": " + VB_CfgWriter.Q(a.Kind) + ", \"Actions\": " + VB_CfgWriter.Strs(a.Actions) + ", \"Targets\": " + VB_CfgWriter.Strs(a.Targets) + ", \"PerTarget\": " + a.PerTarget.ToString() + "}" + VB_CfgWriter.Sep(i, n));
		}
		w.Line("    ],");
		Groups(w, "ItemGroups", ItemGroups);
		Groups(w, "CraftGroups", CraftGroups);
		w.Line("    \"Contracts\": [");
		n = Contracts.Count();
		for (int k = 0; k < n; k++)
		{
			VB_TabletContractType t = Contracts.Get(k);
			string row = "        {\"Kind\": " + VB_CfgWriter.Q(t.Kind) + ", \"Ico\": " + VB_CfgWriter.Q(t.Ico);
			if (t.Title != "" && t.Title != "#STR_VB_TAB_CK_" + t.Kind)
				row += ", \"Title\": " + VB_CfgWriter.Q(t.Title);
			if (t.Hint != "" && t.Hint != "#STR_VB_TAB_CH_" + t.Kind)
				row += ", \"Hint\": " + VB_CfgWriter.Q(t.Hint);
			row += ", \"Need\": " + VB_CfgWriter.Ints(t.Need) + ", \"ClanMult\": " + t.ClanMult.ToString() + "}" + VB_CfgWriter.Sep(k, n);
			w.Line(row);
		}
		w.Line("    ]");
		w.Line("}");
		w.Close();
	}

	protected void Groups(VB_CfgWriter w, string field, array<ref VB_TabletKillGroup> list)
	{
		w.Line("    \"" + field + "\": [");
		int n = list.Count();
		for (int i = 0; i < n; i++)
		{
			VB_TabletKillGroup g = list.Get(i);
			w.Line("        {\"Kind\": " + VB_CfgWriter.Q(g.Kind) + ", \"Classes\": " + VB_CfgWriter.Strs(g.Classes) + "}" + VB_CfgWriter.Sep(i, n));
		}
		w.Line("    ],");
	}

	// Весь список. Контракт на то, чего нет на сервере, не выдаётся: группы проверяются по классам при запуске.
	protected void DefaultContracts()
	{
		Contracts.Insert(new VB_TabletContractType("zombie", "zombie", 15, 40, 100, 3));
		Contracts.Insert(new VB_TabletContractType("mutant", "crosshair", 3, 10, 25, 3));
		Contracts.Insert(new VB_TabletContractType("animal", "sym_paw", 3, 8, 15, 3));
		Contracts.Insert(new VB_TabletContractType("skin", "sym_axe", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("fish", "fish", 3, 8, 15, 3));
		// Дрова, хворост, камень, крафт, стройка и костры убраны из списка по решению 2026-11-08.
		// Счётчики остались (достижения «Добытчик», «Мастер», «Строитель»), вид можно вернуть строкой в конфиге.
		Contracts.Insert(new VB_TabletContractType("repair", "sym_hammer", 1, 3, 6, 3));
		// Руда BRDK_GoldRush (ActionGroups): без мода не выдаются.
		Contracts.Insert(new VB_TabletContractType("ore", "sym_mountain", 5, 15, 40, 3));
		Contracts.Insert(new VB_TabletContractType("pan", "sym_wave", 5, 15, 30, 3));
		Contracts.Insert(new VB_TabletContractType("ore_precious", "sym_gem", 2, 5, 12, 2));
		Contracts.Insert(new VB_TabletContractType("ore_metal", "sym_cog", 4, 12, 30, 3));
		Contracts.Insert(new VB_TabletContractType("ore_fuel", "sym_flame", 4, 10, 25, 3));
		// Артефакты OOTS (ItemGroups): без мода артефактов не выдаются.
		Contracts.Insert(new VB_TabletContractType("art_any", "sym_star", 2, 5, 12, 3));
		Contracts.Insert(new VB_TabletContractType("art_atom", "radiation", 0, 0, 1, 1));
		Contracts.Insert(new VB_TabletContractType("art_good", "sym_gem", 1, 3, 6, 2));
		Contracts.Insert(new VB_TabletContractType("art_mid", "anomaly", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("art_harm", "danger", 2, 5, 10, 3));
		Contracts.Insert(new VB_TabletContractType("art_deadly", "sym_skull", 2, 5, 10, 3));
		// Крафт артефактов в DRL_CraftMenu (CraftGroups): без аддона VB_Tablet_DRL не выдаются.
		Contracts.Insert(new VB_TabletContractType("craft_art1", "sym_drop", 1, 3, 6, 3));
		Contracts.Insert(new VB_TabletContractType("craft_art2", "sym_bolt", 1, 2, 4, 2));
		Contracts.Insert(new VB_TabletContractType("craft_art3", "sym_moon", 0, 1, 2, 2));
		Contracts.Insert(new VB_TabletContractType("craft_art4", "sym_crown", 0, 0, 1, 1));
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

	// Добыча руды BRDK_GoldRush: жилы киркой (ActionGoldMine) и промывка в реке (ActionSearchGold).
	// Жила после добычи исчезает до рестарта, поэтому каждый заход это новая жила, PerTarget не нужен.
	protected void DefaultActionGroups()
	{
		array<string> mine = {"ActionGoldMine"};
		ActionGroups.Insert(new VB_TabletActionGroup("ore", mine));
		ActionGroups.Insert(new VB_TabletActionGroup("pan", {"ActionSearchGold"}));
		ActionGroups.Insert(new VB_TabletActionGroup("ore_precious", mine, {"BRDK_Gold_Ore", "BRDK_Silver_Ore", "BRDK_Rubin_Ore", "BRDK_Nephrite_Ore"}));
		ActionGroups.Insert(new VB_TabletActionGroup("ore_metal", mine, {"BRDK_Iron_Ore", "BRDK_Bronze_Ore", "BRDK_Titanium_Ore"}));
		ActionGroups.Insert(new VB_TabletActionGroup("ore_fuel", mine, {"BRDK_Coal_Ore", "BRDK_Sulfur_Ore"}));
	}

	// Артефакты OOTS (OTTS_Artifacts) по наборам аномалий сервера 16: детектор выкладывает артефакт на землю у аномалии.
	protected void DefaultItemGroups()
	{
		ItemGroups.Insert(new VB_TabletKillGroup("art_any", {"OOTS_ART_*"}));
		// только Атом
		ItemGroups.Insert(new VB_TabletKillGroup("art_atom", {"OOTS_ART_NanoCell", "OOTS_ART_Light", "OOTS_ART_Compas", "OOTS_ART_Spring", "OOTS_ART_Oasis", "OOTS_ART_Flower", "OOTS_ART_ribka", "OOTS_ART_Monolit"}));
		// Электра, Воронка, Линза
		ItemGroups.Insert(new VB_TabletKillGroup("art_good", {"OOTS_ART_krov_kamnya", "OOTS_ART_vivert", "OOTS_ART_Gravi", "OOTS_ART_NightStar", "OOTS_ART_ShineOfTheForest", "OOTS_ART_Mikrob", "OOTS_ART_Braen"}));
		// Студень, Гроза, Пар, Дедзона
		ItemGroups.Insert(new VB_TabletKillGroup("art_mid", {"OOTS_ART_DarkSoul", "OOTS_ART_kameniy_cvetok3", "OOTS_ART_kameniy_cvetok2", "OOTS_ART_puzir", "OOTS_ART_Burn", "OOTS_ART_Glaz3", "OOTS_ART_Glaz2", "OOTS_ART_Glaz", "OOTS_ART_Crustal2", "OOTS_ART_Crustal", "OOTS_ART_CrustalThorn", "OOTS_ART_Crustal3", "OOTS_ART_busy2", "OOTS_ART_Plamya3", "OOTS_ART_Plamya", "OOTS_ART_Angel", "OOTS_ART_MoonLight", "OOTS_ART_FireBall", "OOTS_ART_Klyaksa", "OOTS_ART_Mohovik", "OOTS_ART_DemonEye", "OOTS_ART_snejinka", "OOTS_ART_vspishka", "OOTS_ART_Sparkler"}));
		// Трамплин, Жарка, Застой, Холера
		ItemGroups.Insert(new VB_TabletKillGroup("art_harm", {"OOTS_ART_svet", "OOTS_ART_Battery3", "OOTS_ART_Battery", "OOTS_ART_AngelFool", "OOTS_ART_BloodStone_New", "OOTS_ART_Plamya2", "OOTS_ART_CrustalThorn", "OOTS_ART_SeaUrchin", "OOTS_ART_myaso"}));
		// Снег, Стекло, ЭМП
		ItemGroups.Insert(new VB_TabletKillGroup("art_deadly", {"OOTS_ART_izolator", "OOTS_ART_Klyaksa", "OOTS_ART_kameniy_cvetok", "OOTS_ART_kolobok", "OOTS_ART_Plasma", "OOTS_ART_snejinka", "OOTS_ART_shturval", "OOTS_ART_DevelHert"}));
	}

	// Ступени дерева крафта артефактов на сервере 18 (глубина от Braen, который делается из руды и в счёт
	// не идёт). Другое дерево крафта: поправить списки в contracts.json.
	protected void DefaultCraftGroups()
	{
		// ступени 1–5
		CraftGroups.Insert(new VB_TabletKillGroup("craft_art1", {"OOTS_ART_svet", "OOTS_ART_Crustal", "OOTS_ART_Drop", "OOTS_ART_Plasma", "OOTS_ART_Plamya", "OOTS_ART_Drop2", "OOTS_ART_Glaz", "OOTS_ART_Plamya2", "OOTS_ART_Klyaksa", "OOTS_ART_vspishka"}));
		// ступени 6–10
		CraftGroups.Insert(new VB_TabletKillGroup("craft_art2", {"OOTS_ART_kameniy_cvetok", "OOTS_ART_NightStar", "OOTS_ART_Plamya3", "OOTS_ART_Drop3", "OOTS_ART_Medusa", "OOTS_ART_Flower", "OOTS_ART_puzir", "OOTS_ART_SeaUrchin", "OOTS_ART_Crustal2", "OOTS_ART_Gravi", "OOTS_ART_Oasis", "OOTS_ART_myaso", "OOTS_ART_kameniy_cvetok2", "OOTS_ART_vivert", "OOTS_ART_Thorn", "OOTS_ART_kolobok", "OOTS_ART_Glaz2", "OOTS_ART_Light", "OOTS_ART_Angel", "OOTS_ART_AngelFool", "OOTS_ART_CrustalThorn"}));
		// ступени 11–14
		CraftGroups.Insert(new VB_TabletKillGroup("craft_art3", {"OOTS_ART_Battery", "OOTS_ART_Mikrob", "OOTS_ART_Glaz3", "OOTS_ART_FireBall", "OOTS_ART_Burn", "OOTS_ART_izolator", "OOTS_ART_BloodStone_New", "OOTS_ART_ribka", "OOTS_ART_Mohovik", "OOTS_ART_Sparkler", "OOTS_ART_kameniy_cvetok3", "OOTS_ART_DemonEye", "OOTS_ART_Battery3", "OOTS_ART_Crustal3", "OOTS_ART_MoonLight", "OOTS_ART_Compas", "OOTS_ART_shturval", "OOTS_ART_snejinka", "OOTS_ART_busy2", "OOTS_ART_krov_kamnya", "OOTS_ART_DevelHert", "OOTS_ART_DarkSoul"}));
		// ступени 15–16, вершина
		CraftGroups.Insert(new VB_TabletKillGroup("craft_art4", {"OOTS_ART_ShineOfTheForest", "OOTS_ART_NanoCell", "OOTS_ART_Monolit", "OOTS_ART_Spring"}));
	}
}

// achievements.json: пороги трёх ступеней каждого достижения и опыт за ступени.
class VB_CfgAch
{
	string Id;
	string Name;	// для удобства чтения, сервер не использует
	ref array<int> Steps = new array<int>();
}

class VB_CfgAchievements
{
	string Comment = "Steps: сколько нужно для 1, 2 и 3 ступени (по возрастанию). Rewards: опыт за 1, 2 и 3 ступень любого достижения. Id менять нельзя, Name только для чтения.";
	ref array<int> Rewards = new array<int>();
	ref array<ref VB_CfgAch> List = new array<ref VB_CfgAch>();

	// Порядок как в планшете (индексы VB_TabletAch).
	static array<string> Ids()
	{
		return {"zombies", "mutants", "animals", "bosses", "travel_km", "life_hours", "days", "drive_km", "gather", "fish", "craft", "build", "contracts", "trophies", "cards", "rank"};
	}

	static array<string> Names()
	{
		return {"Чистильщик", "Гроза мутантов", "Зверолов", "Убийца гигантов", "Дальний путь", "Живучий", "Старожил", "Дальнобойщик", "Добытчик", "Рыбак", "Мастер", "Строитель", "Контрактник", "Искатель трофеев", "Взломщик", "Восхождение"};
	}

	void Defaults()
	{
		Rewards = VB_TabletAch.DefaultRewards();
		List = new array<ref VB_CfgAch>();
		array<int> steps = VB_TabletAch.DefaultSteps();
		array<string> ids = Ids();
		array<string> names = Names();
		for (int a = 0; a < VB_ACH_COUNT; a++)
		{
			VB_CfgAch e = new VB_CfgAch();
			e.Id = ids.Get(a);
			e.Name = names.Get(a);
			for (int s = 0; s < VB_ACH_STEPS; s++)
				e.Steps.Insert(steps.Get(a * VB_ACH_STEPS + s));
			List.Insert(e);
		}
	}

	// Проверенные значения в VB_TabletAch. Неверная строка заменяется значением по умолчанию.
	void Apply(array<string> filled)
	{
		array<int> steps = VB_TabletAch.DefaultSteps();
		array<int> rewards = VB_TabletAch.DefaultRewards();
		if (Rewards && Rewards.Count() == VB_ACH_STEPS && Rewards.Get(0) >= 0 && Rewards.Get(1) >= 0 && Rewards.Get(2) >= 0)
			rewards = Rewards;
		else
			filled.Insert("Rewards");
		array<string> ids = Ids();
		for (int a = 0; a < VB_ACH_COUNT; a++)
		{
			VB_CfgAch e = Find(ids.Get(a));
			if (!e || !Valid(e.Steps))
			{
				filled.Insert(ids.Get(a));
				continue;
			}
			for (int s = 0; s < VB_ACH_STEPS; s++)
				steps.Set(a * VB_ACH_STEPS + s, e.Steps.Get(s));
		}
		VB_TabletAch.Set(steps, rewards);
	}

	protected VB_CfgAch Find(string id)
	{
		if (!List)
			return null;
		foreach (VB_CfgAch e : List)
		{
			if (e && e.Id == id)
				return e;
		}
		return null;
	}

	protected static bool Valid(array<int> steps)
	{
		if (!steps || steps.Count() != VB_ACH_STEPS || steps.Get(0) < 1)
			return false;
		return steps.Get(1) > steps.Get(0) && steps.Get(2) > steps.Get(1);
	}

	void WriteFile(string name)
	{
		VB_CfgWriter w = new VB_CfgWriter();
		if (!w.Open(name))
			return;
		w.Line("{");
		w.Line("    \"Comment\": " + VB_CfgWriter.Q(Comment) + ",");
		w.Line("    \"Rewards\": " + VB_CfgWriter.Ints(Rewards) + ",");
		w.Line("    \"List\": [");
		int n = List.Count();
		for (int i = 0; i < n; i++)
		{
			VB_CfgAch e = List.Get(i);
			w.Line("        {\"Id\": " + VB_CfgWriter.Q(e.Id) + ", \"Name\": " + VB_CfgWriter.Q(e.Name) + ", \"Steps\": " + VB_CfgWriter.Ints(e.Steps) + "}" + VB_CfgWriter.Sep(i, n));
		}
		w.Line("    ]");
		w.Line("}");
		w.Close();
	}
}
