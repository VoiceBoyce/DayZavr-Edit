class MountPiladOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_Pilad");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_Pilad_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountDHF5OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_DHF5");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_DHF5_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountPVS4OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_PVS4");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_PVS4_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountOKP7OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_OKP7");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_OKP7_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountRMRLMOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_RMR_LM");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_RMR_LM_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountRMRTMOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_RMR_TM");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_RMR_TM_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountACOGOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_ACOG");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_ACOG_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountACOGRMROnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_ACOG_RMR");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_ACOG_RMR_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountMRSOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_MRS");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_MRS_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};

class MountNFATACROnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_NFATACR");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_NFATACR_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountNFATACR_1_8OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_NFATACR_1_8");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_NFATACR_1_8_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountSpecterDROnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_SpecterDR");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_SpecterDR_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};

class MountLeupoldMk4OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_LeupoldMk4");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_LeupoldMk4_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};
class MountG33OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_G33");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_G33_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
};
class MountXPS34OnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_XPS34");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_XPS34_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};
class MountXPS34HHSOnB13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_XPS34_HHS");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_B13Mount");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_XPS34_HHS_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};
class MountXPS34OnG33B13 extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#mount";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"AD_XPS34");//you can insert multiple ingredients this way	

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"AD_G33_B13");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = true;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("AD_XPS34_HHS_B13");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -1;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Debug.Log("Recipe Do method called","recipes");
	}
	
	override void SpawnItems(ItemBase ingredients[], PlayerBase player, array<ItemBase> spawned_objects)
	{
		bool hasBattery = false;
		float bHealth, bEnergy;
		//Copy battery properties
		for (int x = 0; x < MAX_NUMBER_OF_INGREDIENTS; ++x)
		{
			array<string> attachmentSlots = new array<string>;
			GetGame().ConfigGetTextArray("CfgVehicles " + ingredients[x].GetType() + " attachments", attachmentSlots);
			if(attachmentSlots.Find("BatteryD") > -1)
			{
				EntityAI battery = ingredients[x].FindAttachmentBySlotName("BatteryD");
				if ( battery )
				{
					bHealth = battery.GetHealth();
					bEnergy = battery.GetCompEM().GetEnergy();
					hasBattery = true;
					break;
				}
			}
		}

		super.SpawnItems(ingredients, player, spawned_objects); //Super must be placed here.

		if ( hasBattery )
		{
			for (int i = 0; i < spawned_objects.Count(); ++i)
			{
				//Attach battery to the 1st item that can take it
				array<string> attSlots = new array<string>;
				GetGame().ConfigGetTextArray("CfgVehicles " + spawned_objects[i].GetType() + " attachments", attSlots);
				if(attSlots.Find("BatteryD") > -1)
				{
					ItemBase ibBattery = ItemBase.Cast( spawned_objects[i].GetInventory().CreateAttachment("Battery9V") );
					if ( ibBattery )
					{
						ibBattery.SetHealth( bHealth );
						ibBattery.GetCompEM().SetEnergy( bEnergy );
						ibBattery.GetCompEM().Synch();
						break;
					}
				}
			}
		}
	}	
};