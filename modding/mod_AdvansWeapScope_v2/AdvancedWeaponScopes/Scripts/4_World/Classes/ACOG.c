class AD_ACOG_Base : ItemOptics 
{
	void AD_ACOG_Base()
	{
		ShowMountAD();
	}	
	
	void HideMountAD ()
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("mount");
			
		SetSimpleHiddenSelectionState(hideAmmoBeltId,false);
	};

	void ShowMountAD()
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("mount");

		SetSimpleHiddenSelectionState(hideAmmoBeltId,true);
	};
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent,slot_id);
		
		if( parent && parent.IsKindOf("AD_M16A2"))
		{
			HideMountAD();
		}	
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent,slot_id);
		
		if( parent && parent.IsKindOf("AD_M16A2"))
		{
			ShowMountAD();
		}	
	}		

	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
class AD_ACOG : AD_ACOG_Base {};
class AD_ACOG_MosinMount : ItemOptics 
{
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
class AD_ACOG_B13 : ItemOptics 
{
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};

class AD_ACOG_RMR_Base : ItemOptics 
{
	void AD_ACOG_RMR_Base()
	{
		ShowMountAD();
	}	
	
	void HideMountAD ()
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("mount");
			
		SetSimpleHiddenSelectionState(hideAmmoBeltId,false);
	};

	void ShowMountAD()
	{
		protected int hideAmmoBeltId = -1;
		TStringArray selectionNames = new TStringArray;

		ConfigGetTextArray("simpleHiddenSelections",selectionNames);

		hideAmmoBeltId = selectionNames.Find("mount");

		SetSimpleHiddenSelectionState(hideAmmoBeltId,true);
	};
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent,slot_id);
		
		if( parent && parent.IsKindOf("AD_M16A2"))
		{
			HideMountAD();
		}	
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent,slot_id);
		
		if( parent && parent.IsKindOf("AD_M16A2"))
		{
			ShowMountAD();
		}	
	}		
	
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
class AD_ACOG_RMR : AD_ACOG_RMR_Base  {};
class AD_ACOG_RMR_MosinMount : ItemOptics 
{
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
class AD_ACOG_RMR_B13 : ItemOptics 
{
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide");
	}
	override void OnOpticExit()
	{
		super.OnOpticExit();

		ShowSelection("hide");
	}
};
