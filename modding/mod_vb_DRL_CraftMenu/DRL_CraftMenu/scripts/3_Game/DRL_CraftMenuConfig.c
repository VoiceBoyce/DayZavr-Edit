class DRL_Craft_Config
{
	ref array<ref DRL_Craft_Recipe> CraftItems = new array<ref DRL_Craft_Recipe>();
	ref array<ref DRL_Craft_Category> Categories = new array<ref DRL_Craft_Category>();
	ref DRL_Craft_Settings Settings = new DRL_Craft_Settings();

	ref array<ref DRL_Craft_Recipe> GetAll()
	{
		return CraftItems;
	}

	bool IsFreeMenuEnabled()
	{
		if (!Settings)
		{
			return true;
		}

		if (Settings.EnableFreeMenu == 1)
		{
			return true;
		}

		return false;
	}

	bool IsFreeLearningMenuEnabled()
	{
		if (!Settings)
		{
			return true;
		}

		if (Settings.EnableFreeLearningMenu == 1)
		{
			return true;
		}

		return false;
	}

	bool IsLearningEnabled()
	{
		DRL_Learning_Config lc = GetLearning_Config();
		if (!lc)
		{
			return false;
		}

		if (lc.EnableLearning == 1)
		{
			return true;
		}

		return false;
	}

	DRL_Learning_Config GetLearningConfig()
	{
		return GetLearning_Config();
	}

	DRL_Craft_Recipe FindByName(string recipeName)
	{
		string key = recipeName;
		key.ToLower();

		array<ref DRL_Craft_Recipe> items = CraftItems;
		if (!items)
		{
			return null;
		}

		for (int i = 0; i < items.Count(); i++)
		{
			DRL_Craft_Recipe r = items.Get(i);
			if (!r)
			{
				continue;
			}

			string rn = r.RecipeName;
			rn.ToLower();
			if (rn == key)
			{
				return r;
			}
		}

		return null;
	}

	DRL_Craft_Recipe FindById(int id)
	{
		array<ref DRL_Craft_Recipe> items = CraftItems;
		if (!items)
		{
			return null;
		}

		for (int i = 0; i < items.Count(); i++)
		{
			DRL_Craft_Recipe r = items.Get(i);
			if (!r)
			{
				continue;
			}

			if (r.RecipeId == id)
			{
				return r;
			}
		}

		return null;
	}
}

ref DRL_Craft_Config g_Craft_Config;

ref DRL_Craft_Config GetCraft_Config()
{
	#ifdef SERVER
	if (!g_Craft_Config)
	{
		g_Craft_Config = new DRL_Craft_Config();
		g_Craft_Config.Load();
	}
	#endif

	return g_Craft_Config;
}

ref DRL_Learning_Config g_LearningConfig;

ref DRL_Learning_Config GetLearning_Config()
{
	#ifdef SERVER
	if (!g_LearningConfig)
	{
		g_LearningConfig = new DRL_Learning_Config();
	}
	#endif

	return g_LearningConfig;
}