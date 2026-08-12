class DRL_WorkbenchFon
{
    string WB_Name;
    string FonWBImagePath;
}

class DRL_Craft_Settings
{
	int    EnableFreeMenu = 1;
	string FonImagePath;
	string CraftFonImagePath;
	string CraftSoundSet;
	
	ref array<ref DRL_WorkbenchFon> WorkbenchFons;
	ref array<string> AdminsID;

	void DRL_Craft_Settings()
	{
		EnableFreeMenu    = 1;
		FonImagePath      = "DRL_CraftMenu/UI/data/work.edds";
		CraftFonImagePath = "DRL_CraftMenu/UI/data/recipemanust.edds";
		
		if (CraftSoundSet == "")
		{
			CraftSoundSet = "DRLCraft_SoundSet";
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
	int    ResultCount;
	float  ResultQuantityFrac;
	float  ResultDamageFrac;
	int RecraftNeedAttachments = 0;
	
	ref array<ref DRL_AttachRequirement> AttachmentsEx;
	ref array<ref DRL_Craft_Component> CraftComponents;
	
	int GetId()
    {
        return RecipeId;
    }
	
	int CurrentCraftCount = 1;

	void DRL_Craft_Recipe()
	{
		CraftTimeSec       	= 0;
		ResultCount        	= 1;
		ResultQuantityFrac 	= 0.0;
		ResultDamageFrac   	= 0.0;
		Description        	= "";
		CraftComponents    	= new array<ref DRL_Craft_Component>();
		AttachmentsEx 		= new array<ref DRL_AttachRequirement>();
		WorkbenchClasses 	= new array<string>();
	}

	int GetAttachReqCount()
	{
		if (AttachmentsEx && AttachmentsEx.Count() > 0) return AttachmentsEx.Count();
		return 0;
	}
	
	void GetAttachReqAt(int idx, out string token, out float dmg)
	{
		token = "";
		dmg = 0.0;
	
		if (AttachmentsEx && AttachmentsEx.Count() > 0)
		{
			if (idx >= 0 && idx < AttachmentsEx.Count())
			{
				DRL_AttachRequirement ar = AttachmentsEx[idx];
				if (ar)
				{
					token = ar.Token;
					dmg   = ar.DamagePct;
					if (dmg < 0.0) dmg = 0.0;
					if (dmg > 1.0) dmg = 1.0;
					return;
				}
			}
		}
	}

	void Sanitize()
	{
		if (CraftTimeSec < 0) CraftTimeSec = 0;
		
		if (ResultCount < 1) ResultCount = 1;
		
		if (ResultQuantityFrac < 0.0) ResultQuantityFrac = 0.0;
		if (ResultQuantityFrac > 1.0) ResultQuantityFrac = 1.0;
		
		if (ResultDamageFrac < 0.0) ResultDamageFrac = 0.0;
		if (ResultDamageFrac > 1.0) ResultDamageFrac = 1.0;

		if (!CraftComponents) CraftComponents = new array<ref DRL_Craft_Component>();
		int n = CraftComponents.Count();
		int i = 0;
		while (i < n)
		{
			DRL_Craft_Component c = CraftComponents[i];
			if (c) c.Sanitize();
			i = i + 1;
		}
		if (RecipeId < 0) RecipeId = 0;
		
		if (!ResultClasses) ResultClasses = new array<string>();
		for (int rci = ResultClasses.Count() - 1; rci >= 0; rci--)
		{
			string rc = ResultClasses[rci];
			rc.Trim();
			if (rc == "" || rc.Length() == 0) { ResultClasses.Remove(rci); continue; }
			ResultClasses[rci] = rc;
		}
		
		if (!WorkbenchClasses) WorkbenchClasses = new array<string>();
		
		for (i = 0; i < WorkbenchClasses.Count(); i++)
		{
			string s = WorkbenchClasses[i];
			s.Trim();
			
			WorkbenchClasses[i] = s;
		}
	
		if (ShowInFreeMenu != 0 && ShowInFreeMenu != 1)
			ShowInFreeMenu = 1;
	}
}

class DRL_AttachRequirement
{
	string Token;
	float  DamagePct;
}

class DRL_Craft_Component
{
	string Classname;
	
	int  MinHealthFrac;
	int    Amount;
	float  QuantityFrac;
	bool   Delete;
	float  ApplyDamageFrac;
	
	float  RecraftDropChance;
	int    RecraftAmount;
	float  RecraftQuantityFrac;
	int    UseInheritance;

	void DRL_Craft_Component()
	{
		MinHealthFrac   = 3;
		Amount          = 1;
		QuantityFrac    = 0.0;
		Delete          = true;
		ApplyDamageFrac = 0.0;
		RecraftDropChance  = 1.0;
		RecraftAmount       = 1;
		RecraftQuantityFrac = 0.0;
		UseInheritance      = 1;
	}

	void Sanitize()
	{
		if (MinHealthFrac < 0) MinHealthFrac = 0;
		if (MinHealthFrac > 3) MinHealthFrac = 3;
	
		if (Amount < 1) Amount = 1;
		
		if (QuantityFrac < 0.0) QuantityFrac = 0.0;
		if (QuantityFrac > 1.0) QuantityFrac = 1.0;
		
		if (ApplyDamageFrac < 0.0) ApplyDamageFrac = 0.0;
		if (ApplyDamageFrac > 1.0) ApplyDamageFrac = 1.0;
		
		if (RecraftDropChance < 0.0) RecraftDropChance = 0.0;
		if (RecraftDropChance > 1.0) RecraftDropChance = 1.0;
		
		if (RecraftAmount < 1) RecraftAmount = 1;

		if (RecraftQuantityFrac < 0.0) RecraftQuantityFrac = 0.0;
		if (RecraftQuantityFrac > 1.0) RecraftQuantityFrac = 1.0;
		
		if (UseInheritance != 0 && UseInheritance != 1)
			UseInheritance = 1;
	}
}