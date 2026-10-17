class DRL_ItemTypeHelper
{
	private static ref map<string, ref array<string>> s_ChildrenCache;
	private static ref map<string, int> s_ScopeCache;

	static int GetClassScope(string clsName)
	{
		if (clsName == "")
		{
			return -1;
		}

		string clsLower = clsName;
		clsLower.ToLower();

		if (!s_ScopeCache)
		{
			s_ScopeCache = new map<string, int>();
		}

		if (s_ScopeCache.Contains(clsLower))
		{
			return s_ScopeCache.Get(clsLower);
		}

		int scope = -1;
		string configPath = "";

		if (g_Game.ConfigIsExisting("CfgVehicles " + clsName))
		{
			configPath = "CfgVehicles " + clsName;
		}
		else if (g_Game.ConfigIsExisting("CfgWeapons " + clsName))
		{
			configPath = "CfgWeapons " + clsName;
		}
		else if (g_Game.ConfigIsExisting("CfgMagazines " + clsName))
		{
			configPath = "CfgMagazines " + clsName;
		}
		else if (g_Game.ConfigIsExisting("CfgNonAIVehicles " + clsName))
		{
			configPath = "CfgNonAIVehicles " + clsName;
		}

		if (configPath != "")
		{
			if (g_Game.ConfigIsExisting(configPath + " scope"))
			{
				scope = g_Game.ConfigGetInt(configPath + " scope");
			}
		}

		s_ScopeCache.Insert(clsLower, scope);
		return scope;
	}

	static bool IsClassPlayable(string clsName)
	{
		return (GetClassScope(clsName) == 2);
	}

	static bool IsClassMatching(string itemType, string reqClass, bool useInheritance = false)
	{
		if (itemType == "" || reqClass == "")
		{
			return false;
		}

		if (useInheritance)
		{
			return g_Game.IsKindOf(itemType, reqClass);
		}

		string t1 = itemType;
		t1.ToLower();
		string t2 = reqClass;
		t2.ToLower();
		return (t1 == t2);
	}

	static array<string> GetPlayableVariants(string baseClsName)
	{
		array<string> res = new array<string>();
		if (baseClsName == "")
		{
			return res;
		}

		string baseLower = baseClsName;
		baseLower.ToLower();

		if (!s_ChildrenCache)
		{
			s_ChildrenCache = new map<string, ref array<string>>();
		}

		if (s_ChildrenCache.Contains(baseLower))
		{
			array<string> cached = s_ChildrenCache.Get(baseLower);
			if (cached)
			{
				for (int c = 0; c < cached.Count(); c++)
				{
					res.Insert(cached[c]);
				}
			}
			return res;
		}

		array<string> categories = new array<string>();
		if (g_Game.ConfigIsExisting("CfgVehicles " + baseClsName))
		{
			categories.Insert("CfgVehicles");
		}
		else if (g_Game.ConfigIsExisting("CfgWeapons " + baseClsName))
		{
			categories.Insert("CfgWeapons");
		}
		else if (g_Game.ConfigIsExisting("CfgMagazines " + baseClsName))
		{
			categories.Insert("CfgMagazines");
		}
		else
		{
			categories.Insert("CfgVehicles");
			categories.Insert("CfgWeapons");
			categories.Insert("CfgMagazines");
		}

		for (int catIdx = 0; catIdx < categories.Count(); catIdx++)
		{
			string cat = categories[catIdx];
			int childCount = g_Game.ConfigGetChildrenCount(cat);
			for (int i = 0; i < childCount; i++)
			{
				string childName;
				if (g_Game.ConfigGetChildName(cat, i, childName))
				{
					int childScope = g_Game.ConfigGetInt(cat + " " + childName + " scope");
					if (childScope == 2)
					{
						if (g_Game.IsKindOf(childName, baseClsName))
						{
							if (res.Find(childName) == -1)
							{
								res.Insert(childName);
							}
						}
					}
				}
			}
		}

		if (res.Count() == 0)
		{
			if (IsClassPlayable(baseClsName))
			{
				res.Insert(baseClsName);
			}
		}

		array<string> toCache = new array<string>();
		for (int k = 0; k < res.Count(); k++)
		{
			toCache.Insert(res[k]);
		}
		s_ChildrenCache.Insert(baseLower, toCache);

		return res;
	}

	static string GetSafePreviewClass(string clsName)
	{
		if (clsName == "")
		{
			return "";
		}

		if (IsClassPlayable(clsName))
		{
			return clsName;
		}

		array<string> vars = GetPlayableVariants(clsName);
		if (vars && vars.Count() > 0)
		{
			return vars[0];
		}

		return clsName;
	}
}
