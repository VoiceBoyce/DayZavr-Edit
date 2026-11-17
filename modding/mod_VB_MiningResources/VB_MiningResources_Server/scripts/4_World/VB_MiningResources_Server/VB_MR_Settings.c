// Настройки VB_MiningResources: $profile:VB_Mods\VB_MiningResources\settings.json.
// Поля и смысл те же, что в GoldRush.json старого мода, классы новые (VB_MR_*).
// Старые классы BRDK_* находятся через наследование (IsKindOf), отдельно их перечислять не нужно.

class VB_MR_VeinData
{
	string VeinName;			// жила
	int MinCount;				// бросков добычи, минимум
	int MaxCount;				// бросков добычи, максимум
	int MinQuant;				// количество в стаке, минимум
	int MaxQuant;				// количество в стаке, максимум
	float OreChance;			// шанс удачного броска, %
	string GoodOre;				// что выпадает при удаче
	string BadOre;				// что выпадает при неудаче (пусто = ничего)
	float ToolDamageOnUse;		// урон инструменту, HP
	float MineTime;				// время добычи, с

	void VB_MR_VeinData(string vein = "", string good = "", float chance = 35, float damage = 25, float time = 12)
	{
		VeinName = vein;
		MinCount = 1;
		MaxCount = 4;
		MinQuant = 1;
		MaxQuant = 1;
		OreChance = chance;
		GoodOre = good;
		BadOre = "Stone";
		ToolDamageOnUse = damage;
		MineTime = time;
	}
}

class VB_MR_SandData
{
	string SandName;			// что выпадает при промывке
	float SandChance;			// шанс, %
	int MinQuant;
	int MaxQuant;
	string OreName;				// во что перерабатывается (чаша или пресс)
	float MeltPressTime;		// время переработки, с

	void VB_MR_SandData(string sand = "", float chance = 35, string ore = "")
	{
		SandName = sand;
		SandChance = chance;
		MinQuant = 1;
		MaxQuant = 5;
		OreName = ore;
		MeltPressTime = 60;
	}
}

class VB_MR_OreData
{
	string OreName;				// самородок или кусок
	string IngotName;			// во что перерабатывается (слиток или брикет)
	float MeltPressTime;		// время переработки, с

	void VB_MR_OreData(string ore = "", string ingot = "")
	{
		OreName = ore;
		IngotName = ingot;
		MeltPressTime = 120;
	}
}

class VB_MR_Settings
{
	protected static const string DIR_MODS = "$profile:VB_Mods";
	protected static const string DIR = "$profile:VB_Mods\\VB_MiningResources";
	protected static const string FILE = "$profile:VB_Mods\\VB_MiningResources\\settings.json";
	protected static ref VB_MR_Settings s_Instance;

	int ConfigVersion;
	float GoldInRiverSearchTime;		// время промывки, с
	int MinCountInRiver;				// бросков при промывке, минимум
	int MaxCountInRiver;				// бросков при промывке, максимум
	ref array<string> BadRiverBounty;	// что выпадает при неудачной промывке
	float MeltUpdatePeriod;				// как часто чаша проверяет плавку, с
	float MeltTempTreshold;				// температура чаши, с которой идёт плавка
	ref array<string> MineTools;		// чем можно добывать
	ref array<ref VB_MR_VeinData> VeineDataDataList;
	ref array<ref VB_MR_SandData> SandDataList;
	ref array<ref VB_MR_OreData> OreDataList;

	static VB_MR_Settings Get()
	{
		if (!s_Instance)
			s_Instance = Load();
		return s_Instance;
	}

	protected static VB_MR_Settings Load()
	{
		string error;
		VB_MR_Settings settings = new VB_MR_Settings();
		if (FileExist(FILE))
		{
			if (JsonFileLoader<VB_MR_Settings>.LoadFile(FILE, settings, error) && settings)
			{
				Print("[VB_MiningResources] Настройки загружены: " + FILE);
				settings.FixEmpty();
				return settings;
			}
			Print("[VB_MiningResources] ОШИБКА в настройках, взяты значения по умолчанию: " + error);
			settings = new VB_MR_Settings();
			settings.SetDefaults();
			return settings;
		}

		settings.SetDefaults();
		if (!FileExist(DIR_MODS))
			MakeDirectory(DIR_MODS);
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		if (JsonFileLoader<VB_MR_Settings>.SaveFile(FILE, settings, error))
			Print("[VB_MiningResources] Создан файл настроек: " + FILE);
		else
			Print("[VB_MiningResources] Не удалось записать настройки: " + error);
		return settings;
	}

	// JsonFileLoader создаёт пустые массивы для полей, которых нет в файле
	protected void FixEmpty()
	{
		if (!BadRiverBounty)
			BadRiverBounty = new array<string>;
		if (!MineTools)
			MineTools = new array<string>;
		if (!VeineDataDataList)
			VeineDataDataList = new array<ref VB_MR_VeinData>;
		if (!SandDataList)
			SandDataList = new array<ref VB_MR_SandData>;
		if (!OreDataList)
			OreDataList = new array<ref VB_MR_OreData>;
	}

	// Значения по умолчанию = рабочие значения сервера 18 из GoldRush.json (2026-11-17)
	protected void SetDefaults()
	{
		ConfigVersion = 1;
		GoldInRiverSearchTime = 20;
		MinCountInRiver = 1;
		MaxCountInRiver = 3;
		BadRiverBounty = {"SmallStone"};
		MeltUpdatePeriod = 30;
		MeltTempTreshold = 150;
		MineTools = {"Pickaxe", "MSFZ_Magic_Kirka"};

		VeineDataDataList = new array<ref VB_MR_VeinData>;
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Gold_Vein", "VB_MR_Gold", 35, 40, 25));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Bronze_Vein", "VB_MR_Bronze", 35, 25, 12));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Silver_Vein", "VB_MR_Silver", 35, 35, 20));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Coal_Vein", "VB_MR_Coal", 35, 25, 12));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Rubin_Vein", "VB_MR_Rubin", 35, 35, 20));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Nephrite_Vein", "VB_MR_Nephrite", 35, 30, 15));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Sulfur_Vein", "VB_MR_Sulfur", 35, 30, 15));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Iron_Vein", "VB_MR_Iron", 35, 25, 12));
		VeineDataDataList.Insert(new VB_MR_VeinData("VB_MR_Titanium_Vein", "VB_MR_Titanium", 35, 40, 25));

		SandDataList = new array<ref VB_MR_SandData>;
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Gold_Sand", 5, "VB_MR_Gold"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Silver_Sand", 10, "VB_MR_Silver"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Bronze_Sand", 35, "VB_MR_Bronze"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Iron_Sand", 35, "VB_MR_Iron"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Titanium_Sand", 5, "VB_MR_Titanium"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Rubin_Sand", 10, "VB_MR_Rubin"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Sulfur_Sand", 20, "VB_MR_Sulfur"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Coal_Sand", 35, "VB_MR_Coal"));
		SandDataList.Insert(new VB_MR_SandData("VB_MR_Nephrite_Sand", 20, "VB_MR_Nephrite"));

		OreDataList = new array<ref VB_MR_OreData>;
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Gold", "VB_MR_Gold_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Silver", "VB_MR_Silver_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Bronze", "VB_MR_Bronze_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Titanium", "VB_MR_Titanium_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Iron", "VB_MR_Iron_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Rubin", "VB_MR_Rubin_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Sulfur", "VB_MR_Sulfur_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Nephrite", "VB_MR_Nephrite_Bar"));
		OreDataList.Insert(new VB_MR_OreData("VB_MR_Coal", "VB_MR_Coal_Bar"));
	}

	bool IsMineTool(string type)
	{
		return MineTools && MineTools.Find(type) != -1;
	}

	// Поиск записи: сначала точное имя класса, потом наследование (старые BRDK_* наследуют VB_MR_*)
	VB_MR_VeinData FindVein(Object obj)
	{
		if (!obj || !VeineDataDataList)
			return null;

		string type = obj.GetType();
		foreach (VB_MR_VeinData exact : VeineDataDataList)
		{
			if (exact.VeinName == type)
				return exact;
		}
		foreach (VB_MR_VeinData parent : VeineDataDataList)
		{
			if (obj.IsKindOf(parent.VeinName))
				return parent;
		}
		return null;
	}

	VB_MR_SandData FindSand(EntityAI item)
	{
		if (!item || !SandDataList)
			return null;

		string type = item.GetType();
		foreach (VB_MR_SandData exact : SandDataList)
		{
			if (exact.SandName == type)
				return exact;
		}
		foreach (VB_MR_SandData parent : SandDataList)
		{
			if (item.IsKindOf(parent.SandName))
				return parent;
		}
		return null;
	}

	VB_MR_OreData FindOre(EntityAI item)
	{
		if (!item || !OreDataList)
			return null;

		string type = item.GetType();
		foreach (VB_MR_OreData exact : OreDataList)
		{
			if (exact.OreName == type)
				return exact;
		}
		foreach (VB_MR_OreData parent : OreDataList)
		{
			if (item.IsKindOf(parent.OreName))
				return parent;
		}
		return null;
	}
}
