class SFP_Can_water_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_water_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_water_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Kukuruza_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Kukuruza_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Kukuruza_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Maslin_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Maslin_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Maslin_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_FasolRed_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_FasolRed_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_FasolRed_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_FasolWhite_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_FasolWhite_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_FasolWhite_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_shprota09_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota09_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota09_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_shprota08_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota08_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota08_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_shprota07_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota07_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota07_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Goroh_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Goroh_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Goroh_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Kotlet_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Kotlet_open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Kotlet_open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Meat_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Meat_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Meat_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
class SFP_Can_Milk_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_Milk_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_Milk_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota01_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota01_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota01_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota02_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota02_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota02_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota03_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota03_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota03_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota04_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota04_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota04_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota05_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota05_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota05_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------
//------------------------------------------------------------
class SFP_Can_shprota06_closed : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Can_shprota06_Open");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class SFP_Can_shprota06_Open: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};
//------------------------------------------------------------