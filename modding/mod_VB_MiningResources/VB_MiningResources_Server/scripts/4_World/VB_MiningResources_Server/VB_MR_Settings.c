// Настройки VB_MiningResources: $profile:VB_Mods\VB_MiningResources\settings.json.
// Поля прежние из GoldRush.json + новые с версии 2 (инструменты, редкие камни, порции, реки, янтарь).
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
	string RareItem;			// редкая находка (пусто = нет), один бросок на всю добычу с жилы
	float RareChance;			// шанс редкой находки, %

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
		RareItem = "";
		RareChance = 0;
	}
}

class VB_MR_SandData
{
	string SandName;			// что выпадает при промывке
	float SandChance;			// шанс, %
	int MinQuant;
	int MaxQuant;
	string OreName;				// во что перерабатывается (чаша или пресс)
	float MeltPressTime;		// время переработки порции, с
	int Portion;				// сколько штук берётся за раз (0 = полный стак)

	void VB_MR_SandData(string sand = "", float chance = 35, string ore = "")
	{
		SandName = sand;
		SandChance = chance;
		MinQuant = 1;
		MaxQuant = 5;
		OreName = ore;
		MeltPressTime = 60;
		Portion = 0;
	}
}

class VB_MR_OreData
{
	string OreName;				// самородок или кусок
	string IngotName;			// во что перерабатывается (слиток или брикет)
	float MeltPressTime;		// время переработки порции, с
	int Portion;				// сколько штук берётся за раз (0 = полный стак)

	void VB_MR_OreData(string ore = "", string ingot = "")
	{
		OreName = ore;
		IngotName = ingot;
		MeltPressTime = 120;
		Portion = 0;
	}
}

// Зона «золотоносной реки»: в радиусе от точки указанные виды песка попадаются чаще
class VB_MR_PanZoneSand
{
	string SandName;
	float ChanceMul = 1.0;
}

class VB_MR_PanZone
{
	string Name;
	vector Position;
	float Radius = 100;
	ref array<ref VB_MR_PanZoneSand> Sands = new array<ref VB_MR_PanZoneSand>;
}

class VB_MR_Settings
{
	protected static const int VERSION = 3;
	protected static const string DIR_MODS = "$profile:VB_Mods";
	protected static const string DIR = "$profile:VB_Mods\\VB_MiningResources";
	protected static const string FILE = "$profile:VB_Mods\\VB_MiningResources\\settings.json";
	protected static ref VB_MR_Settings s_Instance;

	int ConfigVersion;
	float GoldInRiverSearchTime;		// время промывки, с
	int MinCountInRiver;				// бросков при промывке, минимум
	int MaxCountInRiver;				// бросков при промывке, максимум
	ref array<string> BadRiverBounty;	// что выпадает при неудачной промывке
	float MeltUpdatePeriod;				// как часто чаша добавляет время плавки, с
	float MeltTempTreshold;				// температура чаши, с которой идёт плавка
	float PressUpdatePeriod;			// как часто пресс добавляет время прессовки, с
	ref array<string> MineTools;		// инструменты без бонусов (старый список)
	ref array<ref VB_MR_ToolData> MineToolsData;	// инструменты с настройками
	ref array<ref VB_MR_VeinData> VeineDataDataList;
	ref array<ref VB_MR_SandData> SandDataList;
	ref array<ref VB_MR_OreData> OreDataList;
	ref array<ref VB_MR_PanZone> PanZones;
	string AmberItem;					// находка при промывке у рощи (пусто = выключено)
	string AmberSourceClass;			// объект рощи
	float AmberRadius;					// радиус от рощи, м
	float AmberChance;					// шанс за одну промывку, %

	// при первой загрузке сразу собираются общие данные: они нужны и серверным проверкам действий
	static VB_MR_Settings Get()
	{
		if (!s_Instance)
		{
			s_Instance = Load();
			VB_MR_Shared.s_Data = s_Instance.BuildShared();
		}
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
				settings.FixEmpty();
				// пустой или обрезанный файл ({}) — не «старая версия»: иначе миграция перезаписала бы его пустыми списками
				if (settings.VeineDataDataList.Count() == 0 && settings.SandDataList.Count() == 0 && settings.OreDataList.Count() == 0)
				{
					Print("[VB_MiningResources] ОШИБКА: в настройках нет ни жил, ни песка, ни руды — взяты значения по умолчанию, файл не перезаписан: " + FILE);
					settings = new VB_MR_Settings();
					settings.SetDefaults();
					settings.Validate();
					return settings;
				}
				if (settings.ConfigVersion < VERSION)
				{
					settings.Migrate();
					settings.Save("Настройки обновлены до версии " + VERSION.ToString() + ": ");
				}
				settings.Validate();
				Print("[VB_MiningResources] Настройки загружены: " + FILE);
				return settings;
			}
			Print("[VB_MiningResources] ОШИБКА в настройках, взяты значения по умолчанию: " + error);
			settings = new VB_MR_Settings();
			settings.SetDefaults();
			settings.Validate();
			return settings;
		}

		settings.SetDefaults();
		settings.Validate();
		if (!FileExist(DIR_MODS))
			MakeDirectory(DIR_MODS);
		if (!FileExist(DIR))
			MakeDirectory(DIR);
		settings.Save("Создан файл настроек: ");
		return settings;
	}

	protected void Save(string what)
	{
		string error;
		if (JsonFileLoader<VB_MR_Settings>.SaveFile(FILE, this, error))
			Print("[VB_MiningResources] " + what + FILE);
		else
			Print("[VB_MiningResources] Не удалось записать настройки: " + error);
	}

	// JsonFileLoader создаёт пустые массивы для полей, которых нет в файле; null бывает у битых записей
	protected void FixEmpty()
	{
		if (!BadRiverBounty)
			BadRiverBounty = new array<string>;
		if (!MineTools)
			MineTools = new array<string>;
		if (!MineToolsData)
			MineToolsData = new array<ref VB_MR_ToolData>;
		if (!VeineDataDataList)
			VeineDataDataList = new array<ref VB_MR_VeinData>;
		if (!SandDataList)
			SandDataList = new array<ref VB_MR_SandData>;
		if (!OreDataList)
			OreDataList = new array<ref VB_MR_OreData>;
		if (!PanZones)
			PanZones = new array<ref VB_MR_PanZone>;

		// пустые записи ([null], лишняя запятая, {} без имени) выкидываются: JsonFileLoader делает из null
		// запись с пустыми полями, а дальше по ней искали бы инструмент или жилу с именем ""
		int i;
		for (i = MineTools.Count() - 1; i >= 0; i--)
		{
			if (MineTools[i] == "")
				MineTools.Remove(i);
		}
		for (i = MineToolsData.Count() - 1; i >= 0; i--)
		{
			if (!MineToolsData[i] || MineToolsData[i].Tool == "")
				MineToolsData.Remove(i);
			else if (!MineToolsData[i].Veins)
				MineToolsData[i].Veins = new array<string>;
		}
		for (i = VeineDataDataList.Count() - 1; i >= 0; i--)
		{
			if (!VeineDataDataList[i] || VeineDataDataList[i].VeinName == "" || VeineDataDataList[i].GoodOre == "")
				VeineDataDataList.Remove(i);
		}
		for (i = SandDataList.Count() - 1; i >= 0; i--)
		{
			if (!SandDataList[i] || SandDataList[i].SandName == "")
				SandDataList.Remove(i);
		}
		for (i = OreDataList.Count() - 1; i >= 0; i--)
		{
			if (!OreDataList[i] || OreDataList[i].OreName == "")
				OreDataList.Remove(i);
		}
		for (i = PanZones.Count() - 1; i >= 0; i--)
		{
			if (!PanZones[i])
			{
				PanZones.Remove(i);
				continue;
			}
			if (!PanZones[i].Sands)
				PanZones[i].Sands = new array<ref VB_MR_PanZoneSand>;
			for (int s = PanZones[i].Sands.Count() - 1; s >= 0; s--)
			{
				if (!PanZones[i].Sands[s])
					PanZones[i].Sands.Remove(s);
			}
		}
	}

	// Значения вне разумных пределов поправляются в памяти (файл не трогается), в лог — предупреждение
	protected void Validate()
	{
		if (GoldInRiverSearchTime <= 0)
			GoldInRiverSearchTime = Warn("GoldInRiverSearchTime", 20);
		if (MinCountInRiver < 0)
			MinCountInRiver = WarnI("MinCountInRiver", 0);
		if (MaxCountInRiver < MinCountInRiver)
			MaxCountInRiver = WarnI("MaxCountInRiver", MinCountInRiver);
		if (MeltUpdatePeriod <= 0)
			MeltUpdatePeriod = Warn("MeltUpdatePeriod", 30);
		if (PressUpdatePeriod <= 0)
			PressUpdatePeriod = Warn("PressUpdatePeriod", 5);
		if (MeltTempTreshold <= 0)
			MeltTempTreshold = Warn("MeltTempTreshold", 150);
		float bowlMax = GetGame().ConfigGetFloat("CfgVehicles VB_MR_MeltingBowl varTemperatureMax");
		if (bowlMax > 0 && MeltTempTreshold > bowlMax)
			Print(string.Format("[VB_MiningResources] ВНИМАНИЕ: MeltTempTreshold %1 выше предела чаши %2 — чаша не будет плавить", MeltTempTreshold, bowlMax));

		foreach (VB_MR_ToolData tool : MineToolsData)
		{
			if (tool.MineTimeMul <= 0)
				tool.MineTimeMul = Warn(tool.Tool + ".MineTimeMul", 1);
			if (tool.ToolDamageMul < 0)
				tool.ToolDamageMul = Warn(tool.Tool + ".ToolDamageMul", 0);
			if (tool.ExtraRollsMin < 0)
				tool.ExtraRollsMin = WarnI(tool.Tool + ".ExtraRollsMin", 0);
			if (tool.ExtraRollsMax < 0)
				tool.ExtraRollsMax = WarnI(tool.Tool + ".ExtraRollsMax", 0);
			if (tool.BonusChance < 0)
				tool.BonusChance = Warn(tool.Tool + ".BonusChance", 0);
			// находка инструмента из мода, которого нет на этом сервере (янтарь без «Древнего корня») — выключить
			if (tool.BonusItem != "" && !GetGame().ConfigIsExisting("CfgVehicles " + tool.BonusItem))
			{
				Print("[VB_MiningResources] ВНИМАНИЕ: предмета " + tool.BonusItem + " нет в игре, находка инструмента " + tool.Tool + " выключена");
				tool.BonusItem = "";
				tool.BonusChance = 0;
			}
		}
		foreach (VB_MR_VeinData vein : VeineDataDataList)
		{
			if (vein.MinCount < 0)
				vein.MinCount = WarnI(vein.VeinName + ".MinCount", 0);
			if (vein.MaxCount < vein.MinCount)
				vein.MaxCount = WarnI(vein.VeinName + ".MaxCount", vein.MinCount);
			if (vein.MinQuant < 1)
				vein.MinQuant = WarnI(vein.VeinName + ".MinQuant", 1);
			if (vein.MaxQuant < vein.MinQuant)
				vein.MaxQuant = WarnI(vein.VeinName + ".MaxQuant", vein.MinQuant);
			if (vein.MineTime <= 0)
				vein.MineTime = Warn(vein.VeinName + ".MineTime", 12);
			if (vein.ToolDamageOnUse < 0)
				vein.ToolDamageOnUse = Warn(vein.VeinName + ".ToolDamageOnUse", 0);
			// редкая находка из удалённого или чужого мода — выключить, а не создавать несуществующий предмет
			if (vein.RareItem != "" && !GetGame().ConfigIsExisting("CfgVehicles " + vein.RareItem))
			{
				Print("[VB_MiningResources] ВНИМАНИЕ: предмета " + vein.RareItem + " нет в игре, редкая находка у " + vein.VeinName + " выключена");
				vein.RareItem = "";
				vein.RareChance = 0;
			}
		}
		foreach (VB_MR_SandData sand : SandDataList)
		{
			if (sand.MinQuant < 1)
				sand.MinQuant = WarnI(sand.SandName + ".MinQuant", 1);
			if (sand.MaxQuant < sand.MinQuant)
				sand.MaxQuant = WarnI(sand.SandName + ".MaxQuant", sand.MinQuant);
			if (sand.MeltPressTime <= 0)
				sand.MeltPressTime = Warn(sand.SandName + ".MeltPressTime", 60);
		}
		foreach (VB_MR_OreData ore : OreDataList)
		{
			if (ore.MeltPressTime <= 0)
				ore.MeltPressTime = Warn(ore.OreName + ".MeltPressTime", 120);
		}
	}

	protected float Warn(string field, float value)
	{
		Print(string.Format("[VB_MiningResources] ВНИМАНИЕ: неверное значение %1 в настройках, взято %2", field, value));
		return value;
	}

	protected int WarnI(string field, int value)
	{
		Print(string.Format("[VB_MiningResources] ВНИМАНИЕ: неверное значение %1 в настройках, взято %2", field, value));
		return value;
	}

	// Версия 1 (перенос с GoldRush.json) -> 2 -> 3: новые поля со стартовыми значениями (ТЗ v0.3, Велесова кирка);
	// каждый шаг проверяет, не сделан ли он уже, поэтому файл любой старой версии доводится до текущей
	protected void Migrate()
	{
		if (PressUpdatePeriod <= 0)
			PressUpdatePeriod = 5;
		// MSFZ_Magic_Kirka получает бонусы, только если она и раньше добывала; остальные из старого списка — как раньше
		if (MineToolsData.Count() == 0 && MineTools.Find("MSFZ_Magic_Kirka") != -1)
		{
			MineToolsData.Insert(new VB_MR_ToolData("MSFZ_Magic_Kirka", 0.7, 0.5, 10));
			array<string> plain = new array<string>;
			foreach (string tool : MineTools)
			{
				if (tool != "MSFZ_Magic_Kirka")
					plain.Insert(tool);
			}
			MineTools = plain;
		}
		// редкая находка в жиле (RareItem) по умолчанию выключена: огранённые камни убраны 2026-11-18
		if (AmberItem == "" && AmberSourceClass == "")
		{
			AmberItem = "VB_AR_Amber";
			AmberSourceClass = "VB_AR_Heart";
			AmberRadius = 60;
			AmberChance = 3;
		}
		// версия 3: Велесова кирка (ТЗ_Велесова_кирка.md) — быстрее, бережнее, +15 % руды, 0–1 бросок, янтарь 5 %
		bool hasVeles = false;
		foreach (VB_MR_ToolData known : MineToolsData)
		{
			if (known.Tool == "VB_MR_Pickaxe_Veles")
				hasVeles = true;
		}
		if (!hasVeles)
		{
			VB_MR_ToolData veles = new VB_MR_ToolData("VB_MR_Pickaxe_Veles", 0.6, 0.35, 15);
			veles.ExtraRollsMin = 0;
			veles.ExtraRollsMax = 1;
			veles.BonusItem = "VB_AR_Amber";
			veles.BonusChance = 5;
			MineToolsData.Insert(veles);
		}
		ConfigVersion = VERSION;
	}

	// Значения по умолчанию = рабочие значения сервера 18 из GoldRush.json (2026-11-17) + новые из ТЗ v0.3
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
		MineToolsData = new array<ref VB_MR_ToolData>;
		PanZones = new array<ref VB_MR_PanZone>;

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

		Migrate();
	}

	// ---------------------------------------------------------------- данные для игроков
	VB_MR_SharedData BuildShared()
	{
		VB_MR_SharedData data = new VB_MR_SharedData();
		data.PanTime = GoldInRiverSearchTime;
		data.MeltTemp = MeltTempTreshold;

		foreach (VB_MR_ToolData tool : MineToolsData)
			data.Tools.Insert(tool);
		foreach (string plain : MineTools)
		{
			bool known = false;
			foreach (VB_MR_ToolData existing : MineToolsData)
			{
				if (existing.Tool == plain)
					known = true;
			}
			if (!known)
				data.Tools.Insert(new VB_MR_ToolData(plain));
		}

		foreach (VB_MR_VeinData vein : VeineDataDataList)
		{
			VB_MR_VeinInfo info = new VB_MR_VeinInfo();
			info.VeinName = vein.VeinName;
			info.MineTime = vein.MineTime;
			data.Veins.Insert(info);
		}
		foreach (VB_MR_SandData sand : SandDataList)
			data.Recipes.Insert(MakeRecipe(sand.SandName, sand.OreName, sand.MeltPressTime, sand.Portion));
		foreach (VB_MR_OreData ore : OreDataList)
			data.Recipes.Insert(MakeRecipe(ore.OreName, ore.IngotName, ore.MeltPressTime, ore.Portion));
		return data;
	}

	// порция 0 = полный стак предмета, как в старом моде
	protected VB_MR_RecipeInfo MakeRecipe(string item, string result, float time, int portion)
	{
		VB_MR_RecipeInfo recipe = new VB_MR_RecipeInfo();
		recipe.Item = item;
		recipe.Result = result;
		recipe.Time = time;
		recipe.Portion = portion;
		int stackMax = GetGame().ConfigGetInt("CfgVehicles " + item + " varQuantityMax");
		if (recipe.Portion <= 0)
			recipe.Portion = stackMax;
		// порция больше стопки — станок не запустится никогда
		if (stackMax > 0 && recipe.Portion > stackMax)
		{
			Print(string.Format("[VB_MiningResources] ВНИМАНИЕ: порция %1 у %2 больше стопки %3, взята %3", recipe.Portion, item, stackMax));
			recipe.Portion = stackMax;
		}
		if (recipe.Portion <= 0)
			recipe.Portion = 1;
		return recipe;
	}

	// ---------------------------------------------------------------- поиск записей
	// Сначала точное имя класса, потом наследование (старые BRDK_* наследуют VB_MR_*)
	VB_MR_VeinData FindVein(Object obj)
	{
		if (!obj)
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

	// множитель шанса песка по зонам «золотоносных рек» (берётся наибольший)
	float PanZoneMul(vector pos, string sandName)
	{
		float mul = 1.0;
		foreach (VB_MR_PanZone zone : PanZones)
		{
			// расстояние по горизонтали; локальные переменные — два вызова одной функции в аргументах затирают результат
			float dx = pos[0] - zone.Position[0];
			float dz = pos[2] - zone.Position[2];
			if (dx * dx + dz * dz > zone.Radius * zone.Radius)
				continue;
			foreach (VB_MR_PanZoneSand sand : zone.Sands)
			{
				if (sand.SandName == sandName && sand.ChanceMul > mul)
					mul = sand.ChanceMul;
			}
		}
		return mul;
	}
}
