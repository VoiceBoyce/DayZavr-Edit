enum DRL_CraftRPC
{
	CTS_CRAFT = 57864578,
	CTS_RECRAFT = 57864579,
	CTS_CRAFT_WITH_ATTACHMENTS = 57864580,
	CTS_STUDY_RECIPE = 57864581,
	STC_STUDY_RECIPE_RESPONSE = 57864582,
	STC_SYNC_LEARNED_RECIPES = 57864583,
	CTS_CANCEL_CRAFT = 57864584,
	CTS_REQUEST_ACTIVE_PROCESSES = 57864585,
	STC_SYNC_ACTIVE_PROCESSES = 57864586,
	STC_CRAFT_PROCESS_STARTED = 57864587,
	STC_CRAFT_PROCESS_FINISHED = 57864588,
	STC_CRAFT_NOTIFICATION = 57864589
}

enum DRL_CraftProcessStatus
{
	RUNNING = 0,
	QUEUED = 1
}

enum EDRLCraftNotifType
{
	STARTED = 0,
	QUEUED,
	PROMOTED_FROM_QUEUE,
	SUCCESS,
	CANCELLED_DISTANCE,
	NO_INGREDIENTS,
	SPACE_BLOCKED,
	QUEUE_FULL,
	CANCELLED_MANUAL,
	MISSING_TOOLS,
	NOT_LEARNED,
	RECIPE_LEARNED,
	SYSTEM_INFO
}

enum EDRLWeaponAttachType
{
	NONE = 0,
	MAGAZINE,
	OPTIC,
	BUTTSTOCK,
	HANDGUARD,
	SUPPRESSOR,
	BAYONET,
	WRAP,
	SWITCH,
	OTHER
}

enum EDRLItemQuantityType
{
	DISCRETE = 0,
	STACK,
	RESOURCE_BAR,
	LIQUID,
	ENERGY,
	MAGAZINE,
	FOOD
}

class DRL_AttachmentHelper
{
	static bool GetAttachmentSlots(string className, out TStringArray outSlots)
	{
		outSlots = new TStringArray();
		if (className == "") return false;

		string path = "";
		if (GetGame().ConfigIsExisting("CfgVehicles " + className))
			path = "CfgVehicles " + className;
		else if (GetGame().ConfigIsExisting("CfgWeapons " + className))
			path = "CfgWeapons " + className;
		else if (GetGame().ConfigIsExisting("CfgMagazines " + className))
			path = "CfgMagazines " + className;
		else if (GetGame().ConfigIsExisting("CfgNonAIVehicles " + className))
			path = "CfgNonAIVehicles " + className;
		else
			return false;

		string slotProp = path + " inventorySlot";
		if (!GetGame().ConfigIsExisting(slotProp))
		{
			slotProp = path + " inventorySlots";
		}

		if (GetGame().ConfigIsExisting(slotProp))
		{
			switch (GetGame().ConfigGetType(slotProp))
			{
				case CT_ARRAY:
				{
					TStringArray tmp = new TStringArray();
					GetGame().ConfigGetTextArray(slotProp, tmp);
					for (int i = 0; i < tmp.Count(); i++)
					{
						string sVal = tmp[i];
						sVal.ToLower();
						outSlots.Insert(sVal);
					}
					return (outSlots.Count() > 0);
				}
				case CT_STRING:
				{
					string singleSlot = "";
					GetGame().ConfigGetText(slotProp, singleSlot);
					if (singleSlot != "")
					{
						singleSlot.ToLower();
						outSlots.Insert(singleSlot);
						return true;
					}
					break;
				}
			}
		}

		return false;
	}

	static EDRLWeaponAttachType GetWeaponAttachmentType(string className, string slotName = "")
	{
		if (className == "") return EDRLWeaponAttachType.NONE;

		string classNameToLower = className;
		classNameToLower.ToLower();
		string slotNameToLower = slotName;
		slotNameToLower.ToLower();

		if (GetGame().IsKindOf(className, "Magazine_Base") || classNameToLower.Contains("mag_") || classNameToLower.Contains("magazine") || slotNameToLower.Contains("magazine"))
		{
			return EDRLWeaponAttachType.MAGAZINE;
		}

		if (GetGame().IsKindOf(className, "ItemOptics") || slotNameToLower.Contains("optic") || classNameToLower.Contains("optic"))
		{
			return EDRLWeaponAttachType.OPTIC;
		}

		if (GetGame().IsKindOf(className, "ItemSuppressor") || GetGame().IsKindOf(className, "SuppressorBase") || slotNameToLower.Contains("suppressor") || classNameToLower.Contains("suppressor") || slotNameToLower.Contains("muzzle") || classNameToLower.Contains("compensator") || classNameToLower.Contains("bayonet"))
		{
			if (slotNameToLower.Contains("bayonet") || classNameToLower.Contains("bayonet"))
				return EDRLWeaponAttachType.BAYONET;
			return EDRLWeaponAttachType.SUPPRESSOR;
		}

		if (slotNameToLower.Contains("buttstock") || classNameToLower.Contains("buttstock") || classNameToLower.Contains("bttstck"))
		{
			return EDRLWeaponAttachType.BUTTSTOCK;
		}

		if (slotNameToLower.Contains("handguard") || classNameToLower.Contains("handguard") || classNameToLower.Contains("hndgrd"))
		{
			return EDRLWeaponAttachType.HANDGUARD;
		}

		if (GetGame().IsKindOf(className, "GhillieAtt_ColorBase") || slotNameToLower.Contains("wrap") || classNameToLower.Contains("wrap"))
		{
			return EDRLWeaponAttachType.WRAP;
		}

		if (GetGame().IsKindOf(className, "Switchable_Base") || slotNameToLower.Contains("flashlight") || classNameToLower.Contains("flashlight"))
		{
			return EDRLWeaponAttachType.SWITCH;
		}

		return EDRLWeaponAttachType.OTHER;
	}
}

class DRL_WorkbenchFon
{
    string WB_Name;
    string FonWBImagePath;
}

class DRL_Craft_Settings
{
	int    EnableFreeMenu = 1;
	int    EnableFreeLearningMenu = 1;
	int    MaxQueueSlots = 5;
	string FonImagePath;
	string CraftFonImagePath;
	string LearningFonImagePath;
	string CraftSoundSet;
	string LearningSoundSet;
	
	ref array<ref DRL_WorkbenchFon> WorkbenchFons;
	ref array<string> AdminsID;

	void DRL_Craft_Settings()
	{
		EnableFreeMenu         = 1;
		EnableFreeLearningMenu = 1;
		MaxQueueSlots          = 5;
		FonImagePath           = "set:drl_craft_work image:work";
		CraftFonImagePath      = "set:drl_craft_recipemanust image:recipemanust";
		LearningFonImagePath   = "set:drl_craft_learning_fon image:learning_fon";
		
		if (CraftSoundSet == "")
		{
			CraftSoundSet = "DRLCraft_SoundSet";
		}

		if (LearningSoundSet == "")
		{
			LearningSoundSet = "DRLCraft_SoundSet";
		}

        if (!WorkbenchFons)
        {
            WorkbenchFons = new array<ref DRL_WorkbenchFon>;
        }
		
		if (!AdminsID)
		{
			AdminsID = new array<string>;
		}
	}

    string GetFonWBImagePath(string wbClass)
	{
		if (wbClass == string.Empty)
		{
			return FonImagePath;
		}
	
		if (!WorkbenchFons)
		{
			return FonImagePath;
		}
	
		foreach (DRL_WorkbenchFon f : WorkbenchFons)
		{
			if (!f)
			{
				continue;
			}
	
			if (f.WB_Name == wbClass)
			{
				if (f.FonWBImagePath != string.Empty)
				{
					return f.FonWBImagePath;
				}
			}
		}
	
		return FonImagePath;
	}
}

class DRL_Craft_Recipe
{
    int RecipeId;
	
	int ShowInFreeMenu = 1;
	ref array<string> WorkbenchClasses;
	
	string RecipeName;
	string Result;
	
	ref array<string> ResultClasses = new array<string>();
	
	string PreviewClassname;
	string CraftIcon;
	string Description;

	int    CraftTimeSec;
	int    RecraftTimeSec;
	int    ResultCount;
	float  ResultQuantityFrac;
	float  ResultDamageFrac;
	
	ref array<ref DRL_Craft_Component> CraftComponents;
	ref array<int> AttachedRecipeIds;
	
	int GetId()
    {
        return RecipeId;
    }

	int GetRecraftTimeSec()
	{
		if (RecraftTimeSec > 0)
			return RecraftTimeSec;
		return CraftTimeSec;
	}
	
	int CurrentCraftCount = 1;

	void DRL_Craft_Recipe()
	{
		CraftTimeSec       	= 0;
		RecraftTimeSec     	= 0;
		ResultCount        	= 1;
		ResultQuantityFrac 	= 0.0;
		ResultDamageFrac   	= 0.0;
		Description        	= "";
		CraftComponents    	= new array<ref DRL_Craft_Component>();
		WorkbenchClasses 	= new array<string>();
		AttachedRecipeIds 	= new array<int>();
	}

	bool HasAttachedRecipes()
	{
		return (AttachedRecipeIds && AttachedRecipeIds.Count() > 0);
	}

	bool HasAttachmentRequirements()
	{
		if (!CraftComponents)
		{
			return false;
		}

		for (int i = 0; i < CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = CraftComponents[i];
			if (c && c.AttachmentsEx > 0)
			{
				return true;
			}
		}
		return false;
	}

	int GetAttachReqCount()
	{
		if (!CraftComponents)
		{
			return 0;
		}

		int count = 0;
		for (int i = 0; i < CraftComponents.Count(); i++)
		{
			DRL_Craft_Component c = CraftComponents[i];
			if (c && c.AttachmentsEx > 0)
			{
				count++;
			}
		}
		return count;
	}
}

class DRL_Craft_Component
{
	string Classname;
	ref array<string> Classnames;
	int    EnableVariationSelection = 1;
	
	int    MinHealthFrac = 3;
	int    Amount = 1;
	float  QuantityFrac = 0.0;
	bool   Delete = true;
	float  ApplyDamageFrac = 0.0;
	
	float  RecraftDropChance = 1.0;
	int    RecraftAmount = 1;
	float  RecraftQuantityFrac = 0.0;
	int    UseInheritance = 1;
	int    AttachmentsEx = 0;
	int    RecraftNeedAttachments = 0;
	ref array<string> OnlyForResults = new array<string>();

	void DRL_Craft_Component()
	{
		Classnames             = new array<string>();
		EnableVariationSelection = 1;
		MinHealthFrac          = 3;
		Amount                 = 1;
		QuantityFrac           = 0.0;
		Delete                 = true;
		ApplyDamageFrac        = 0.0;
		RecraftDropChance      = 1.0;
		RecraftAmount          = 1;
		RecraftQuantityFrac    = 0.0;
		UseInheritance         = 1;
		AttachmentsEx          = 0;
		RecraftNeedAttachments = 0;
		OnlyForResults         = new array<string>();
	}

	bool IsApplicableForResult(string resultCls)
	{
		if (!OnlyForResults || OnlyForResults.Count() == 0)
		{
			return true;
		}

		if (resultCls == "")
		{
			return false;
		}

		string lowerTarget = resultCls;
		lowerTarget.ToLower();

		for (int i = 0; i < OnlyForResults.Count(); i++)
		{
			string candidate = OnlyForResults[i];
			candidate.ToLower();
			if (candidate == lowerTarget)
			{
				return true;
			}
		}

		return false;
	}

	bool IsTool()
	{
		return (AttachmentsEx > 0);
	}

	bool IsSlotOnly()
	{
		return (AttachmentsEx == 1);
	}

	bool IsFlexibleAttachment()
	{
		return (AttachmentsEx == 2);
	}

	DRL_Craft_Component CloneWithVariation(string chosenVariation)
	{
		DRL_Craft_Component comp = new DRL_Craft_Component();
		comp.Classname = chosenVariation;
		comp.Classnames = new array<string>();
		comp.Classnames.Insert(chosenVariation);
		comp.EnableVariationSelection = EnableVariationSelection;
		comp.MinHealthFrac = MinHealthFrac;
		comp.Amount = Amount;
		comp.QuantityFrac = QuantityFrac;
		comp.Delete = Delete;
		comp.ApplyDamageFrac = ApplyDamageFrac;
		comp.RecraftDropChance = RecraftDropChance;
		comp.RecraftAmount = RecraftAmount;
		comp.RecraftQuantityFrac = RecraftQuantityFrac;
		comp.UseInheritance = UseInheritance;
		comp.AttachmentsEx = AttachmentsEx;
		comp.RecraftNeedAttachments = RecraftNeedAttachments;
		if (OnlyForResults)
		{
			for (int o = 0; o < OnlyForResults.Count(); o++)
			{
				comp.OnlyForResults.Insert(OnlyForResults[o]);
			}
		}
		return comp;
	}
}

class DRL_CraftMergedComponent
{
	string Classname;
	ref array<string> Classnames;
	int Amount;
	float QuantityFrac;
	float ApplyDamageFrac;
	int MinHealthFrac;
	bool Delete;
	int AttachmentsEx;
	int EnableVariationSelection;
	int UseInheritance;
	ref array<int> ComponentKeys;
	ref array<string> OnlyForResults;

	void DRL_CraftMergedComponent(DRL_Craft_Component src, int srcKey = -1)
	{
		Classname = src.Classname;
		Classnames = new array<string>();
		if (src.Classnames)
		{
			for (int i = 0; i < src.Classnames.Count(); i++)
			{
				Classnames.Insert(src.Classnames[i]);
			}
		}
		OnlyForResults = new array<string>();
		if (src.OnlyForResults)
		{
			for (int o = 0; o < src.OnlyForResults.Count(); o++)
			{
				OnlyForResults.Insert(src.OnlyForResults[o]);
			}
		}
		Amount = src.Amount;
		QuantityFrac = src.QuantityFrac;
		ApplyDamageFrac = src.ApplyDamageFrac;
		MinHealthFrac = src.MinHealthFrac;
		Delete = src.Delete;
		AttachmentsEx = src.AttachmentsEx;
		EnableVariationSelection = src.EnableVariationSelection;
		UseInheritance = src.UseInheritance;
		ComponentKeys = new array<int>();
		if (srcKey >= 0)
		{
			ComponentKeys.Insert(srcKey);
		}
	}

	void Merge(DRL_Craft_Component other, int otherKey = -1)
	{
		Amount += other.Amount;
		QuantityFrac += other.QuantityFrac;
		ApplyDamageFrac += other.ApplyDamageFrac;
		MinHealthFrac = Math.Min(MinHealthFrac, other.MinHealthFrac);
		if (other.Delete)
		{
			Delete = true;
		}
		if (other.EnableVariationSelection == 1)
		{
			EnableVariationSelection = 1;
		}
		if (other.UseInheritance == 1)
		{
			UseInheritance = 1;
		}
		if (other.Classnames && Classnames)
		{
			for (int c = 0; c < other.Classnames.Count(); c++)
			{
				if (Classnames.Find(other.Classnames[c]) == -1)
				{
					Classnames.Insert(other.Classnames[c]);
				}
			}
		}
		if (otherKey >= 0 && ComponentKeys.Find(otherKey) == -1)
		{
			ComponentKeys.Insert(otherKey);
		}
	}

	DRL_Craft_Component ToCraftComponent()
	{
		DRL_Craft_Component comp = new DRL_Craft_Component();
		comp.Classname = Classname;
		comp.Classnames = new array<string>();
		if (Classnames)
		{
			for (int i = 0; i < Classnames.Count(); i++)
			{
				comp.Classnames.Insert(Classnames[i]);
			}
		}
		comp.OnlyForResults = new array<string>();
		if (OnlyForResults)
		{
			for (int o = 0; o < OnlyForResults.Count(); o++)
			{
				comp.OnlyForResults.Insert(OnlyForResults[o]);
			}
		}
		comp.Amount = Amount;
		comp.QuantityFrac = QuantityFrac;
		comp.ApplyDamageFrac = ApplyDamageFrac;
		comp.MinHealthFrac = MinHealthFrac;
		comp.Delete = Delete;
		comp.AttachmentsEx = AttachmentsEx;
		comp.EnableVariationSelection = EnableVariationSelection;
		comp.UseInheritance = UseInheritance;
		return comp;
	}
}

class DRL_VariationClickInfo
{
	DRL_CraftMergedComponent MergedComp;
	int TargetKey;
	string VariantClass;
	int VariantIndex;

	void DRL_VariationClickInfo(DRL_CraftMergedComponent mc, int targetKey, string varCls, int varIdx)
	{
		MergedComp = mc;
		TargetKey = targetKey;
		VariantClass = varCls;
		VariantIndex = varIdx;
	}
}

class DRL_Study_Variation
{
	string DisplayName;
	string RequiredItem;
	int Amount = 1;
	int Delete = 1;

	void DRL_Study_Variation()
	{
		DisplayName = "";
		RequiredItem = "";
		Amount = 1;
		Delete = 1;
	}
}

class DRL_Learning_Recipe
{
	ref array<int> RecipeIds = new array<int>();
	int StudyMain = 0;
	ref array<int> RequiredRecipeIds = new array<int>();
	ref array<ref DRL_Study_Variation> StudyVariations = new array<ref DRL_Study_Variation>();

	void DRL_Learning_Recipe()
	{
		RecipeIds = new array<int>();
		StudyMain = 0;
		RequiredRecipeIds = new array<int>();
		StudyVariations = new array<ref DRL_Study_Variation>();
	}

	bool HasRequiredRecipes()
	{
		return (RequiredRecipeIds && RequiredRecipeIds.Count() > 0);
	}

	int GetPrimaryRecipeId()
	{
		if (RecipeIds && RecipeIds.Count() > 0)
		{
			return RecipeIds[0];
		}
		return -1;
	}

	bool ContainsRecipeId(int id)
	{
		if (!RecipeIds)
		{
			return false;
		}
		return (RecipeIds.Find(id) != -1);
	}

	bool IsStudyMain()
	{
		return (StudyMain == 1);
	}

	bool ShouldShowInLearningList(int recipeId)
	{
		if (StudyMain == 1)
		{
			return (recipeId == GetPrimaryRecipeId());
		}
		return true;
	}
}

class DRL_Learning_Config
{
	int EnableLearning = 1;
	int ResetLearnedOnDeath = 0;
	string LearningSoundSet = "DRLCraft_SoundSet";
	ref array<ref DRL_Learning_Recipe> LearningRecipes = new array<ref DRL_Learning_Recipe>();

	void DRL_Learning_Config()
	{
		EnableLearning = 1;
		ResetLearnedOnDeath = 0;
		LearningSoundSet = "DRLCraft_SoundSet";
		LearningRecipes = new array<ref DRL_Learning_Recipe>();
	}

	DRL_Learning_Recipe FindByPrimaryId(int primaryId)
	{
		if (!LearningRecipes)
		{
			return null;
		}

		foreach (DRL_Learning_Recipe lr : LearningRecipes)
		{
			if (lr && lr.GetPrimaryRecipeId() == primaryId)
			{
				return lr;
			}
		}
		return null;
	}

	DRL_Learning_Recipe FindByAnyRecipeId(int recipeId)
	{
		if (!LearningRecipes)
		{
			return null;
		}

		foreach (DRL_Learning_Recipe lr : LearningRecipes)
		{
			if (lr && lr.ContainsRecipeId(recipeId))
			{
				return lr;
			}
		}
		return null;
	}

	bool RecipeRequiresLearning(int recipeId)
	{
		if (EnableLearning != 1)
		{
			return false;
		}
		return (FindByAnyRecipeId(recipeId) != null);
	}
}

class DRL_PlayerLearningData
{
	string PlayerId;
	ref array<int> LearnedRecipeIds = new array<int>();

	void DRL_PlayerLearningData(string id = "")
	{
		PlayerId = id;
		LearnedRecipeIds = new array<int>();
	}

	bool HasLearned(int recipeId)
	{
		if (!LearnedRecipeIds)
		{
			return false;
		}
		return (LearnedRecipeIds.Find(recipeId) != -1);
	}

	void AddRecipe(int recipeId)
	{
		if (!LearnedRecipeIds)
		{
			LearnedRecipeIds = new array<int>();
		}
		if (LearnedRecipeIds.Find(recipeId) == -1)
		{
			LearnedRecipeIds.Insert(recipeId);
		}
	}
}