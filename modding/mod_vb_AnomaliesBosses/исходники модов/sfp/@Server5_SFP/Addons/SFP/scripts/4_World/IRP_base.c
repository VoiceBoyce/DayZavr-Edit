
class SFP_RusIRP_Buckwheat : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Buckwheat_open");
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

class SFP_RusIRP_Buckwheat_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Lard : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Lard_open");
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

class SFP_RusIRP_Lard_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Lard2 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Lard2_open");
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

class SFP_RusIRP_Lard2_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_GulashMeatTomato : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_GulashMeatTomato_open");
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

class SFP_RusIRP_GulashMeatTomato_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Meat : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Meat_open");
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

class SFP_RusIRP_Meat_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_KashaRice : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_KashaRice_open");
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

class SFP_RusIRP_KashaRice_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_MeatFasol6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_MeatFasol6_open");
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

class SFP_RusIRP_MeatFasol6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_TeftelMeat6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_TeftelMeat6_open");
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

class SFP_RusIRP_TeftelMeat6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_FrecodelMeat : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_FrecodelMeat_open");
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

class SFP_RusIRP_FrecodelMeat_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_KashaGrecaMeat : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_KashaGrecaMeat_open");
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

class SFP_RusIRP_KashaGrecaMeat_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_MeatFasolFresh : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_MeatFasolFresh_open");
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

class SFP_RusIRP_MeatFasolFresh_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Shpik : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Shpik_Open");
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

class SFP_RusIRP_Shpik_Open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------

//===================================МАЛЕНЬКИЕ ПОРЦИИ

class SFP_RusIRP_ApplePure : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_ApplePure_open");
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

class SFP_RusIRP_ApplePure_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Farsh : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Farsh_open");
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

class SFP_RusIRP_Farsh_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_ikraFresh : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_ikraFresh_open");
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

class SFP_RusIRP_ikraFresh_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_PastetPecen : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_PastetPecen_open");
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

class SFP_RusIRP_PastetPecen_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Plavleniy : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Plavleniy_open");
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

class SFP_RusIRP_Plavleniy_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Farsh6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Farsh6_open");
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

class SFP_RusIRP_Farsh6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_PastetPecenka6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_PastetPecenka6_open");
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

class SFP_RusIRP_PastetPecenka6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_ApplePure6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_ApplePure6_open");
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

class SFP_RusIRP_ApplePure6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_RaguFresh6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_RaguFresh6_open");
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

class SFP_RusIRP_RaguFresh6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Plavleniy6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Plavleniy6_open");
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

class SFP_RusIRP_Plavleniy6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
class SFP_RusIRP_Spik6 : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_RusIRP_Spik6_open");
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

class SFP_RusIRP_Spik6_open: Edible_Base
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
//--------------------------------------------------------------------------------------------------------------------------------------
