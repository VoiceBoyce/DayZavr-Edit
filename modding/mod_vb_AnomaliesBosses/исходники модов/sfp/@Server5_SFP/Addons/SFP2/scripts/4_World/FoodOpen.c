class SFP_Food_DoshirakRed_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_DoshirakBlue_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_DoshirakGreen_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_DoshirakBrown_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_DoshirakYellou_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_DoshirakPink_pack : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Open");
		ReplaceEdibleWithNew("SFP_Food_Doshirak_Priprava");
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
class SFP_Food_Doshirak_Priprava: Edible_Base
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
		return false;
	}
	
};
class SFP_Food_Doshirak_Open: Edible_Base
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
