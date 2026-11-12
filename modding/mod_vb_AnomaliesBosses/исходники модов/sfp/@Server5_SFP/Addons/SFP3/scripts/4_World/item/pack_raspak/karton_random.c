class ActionKartonPackOpenCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionKartonPackOpen: ActionContinuousBase
{	
	ItemBase resultItem;
	void ActionKartonPackOpen()
	{
		m_CallbackClass = ActionKartonPackOpenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
		
	override string GetText()
	{
		return "#STR_SFP_OpenPackKartonAct";
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			
			// Еще массив со всяким гавном
			array<string> m_ItemsBotleWater = new array<string>;
			
			// Еще массив со всяким гавном
			array<string> m_ItemsForSpawn = new array<string>;
				
			// Заполняем массив именами шмоток
			m_ItemsBotleWater.Insert("PersonalRadio");
			m_ItemsBotleWater.Insert("WoodenCrate");
			m_ItemsBotleWater.Insert("Shovel");
			m_ItemsBotleWater.Insert("WaterBottle");
			m_ItemsBotleWater.Insert("TunaCan");
			m_ItemsBotleWater.Insert("SodaCan_Pipsi");
			
			// Заполняем массив с гавном именами шмоток
			m_ItemsForSpawn.Insert("AthleticShoes_Blue");
			m_ItemsForSpawn.Insert("AthleticShoes_Black");
			m_ItemsForSpawn.Insert("Wellies_Black");
			m_ItemsForSpawn.Insert("Wellies_Brown");
			m_ItemsForSpawn.Insert("WorkingBoots_Grey");
			m_ItemsForSpawn.Insert("JungleBoots_Black");
			m_ItemsForSpawn.Insert("DressShoes_Beige");
			m_ItemsForSpawn.Insert("MilitaryBoots_Bluerock");
			m_ItemsForSpawn.Insert("CombatBoots_Grey");
			m_ItemsForSpawn.Insert("JoggingShoes_Red");
			m_ItemsForSpawn.Insert("LeatherShoes_Black");
			m_ItemsForSpawn.Insert("Sneakers_Gray");
			m_ItemsForSpawn.Insert("SportGlasses_Black");
			m_ItemsForSpawn.Insert("SportGlasses_Blue");
			m_ItemsForSpawn.Insert("SportGlasses_Green");
			m_ItemsForSpawn.Insert("AviatorGlasses");
			m_ItemsForSpawn.Insert("DesignerGlasses");
			m_ItemsForSpawn.Insert("ThickFramesGlasses");
			m_ItemsForSpawn.Insert("ThinFramesGlasses");
			m_ItemsForSpawn.Insert("TacticalGoggles");
			m_ItemsForSpawn.Insert("EyePatch_Improvised");
			m_ItemsForSpawn.Insert("FaceCover_Improvised");
			m_ItemsForSpawn.Insert("CrookedNose");
			m_ItemsForSpawn.Insert("MimeMask_Male");
			m_ItemsForSpawn.Insert("SurgicalMask");
			m_ItemsForSpawn.Insert("MouthRag");
			m_ItemsForSpawn.Insert("Balaclava3Holes_White");
			m_ItemsForSpawn.Insert("Balaclava3Holes_Green");
			m_ItemsForSpawn.Insert("Balaclava3Holes_Black");
			m_ItemsForSpawn.Insert("Balaclava3Holes_Beige");
			m_ItemsForSpawn.Insert("GP5GasMask_Filter");
			m_ItemsForSpawn.Insert("AirborneMask");
			m_ItemsForSpawn.Insert("BalaclavaMask_BDU");
			m_ItemsForSpawn.Insert("BalaclavaMask_White");
			m_ItemsForSpawn.Insert("GasMask");
			m_ItemsForSpawn.Insert("Pen_Black");
			m_ItemsForSpawn.Insert("EpoxyPutty");
			m_ItemsForSpawn.Insert("Whetstone");
			m_ItemsForSpawn.Insert("Pliers");
			m_ItemsForSpawn.Insert("Hammer");
			m_ItemsForSpawn.Insert("MeatTenderizer");
			m_ItemsForSpawn.Insert("Wrench");
			m_ItemsForSpawn.Insert("Sickle");
			m_ItemsForSpawn.Insert("Hacksaw");
			m_ItemsForSpawn.Insert("KitchenKnife");
			m_ItemsForSpawn.Insert("SteakKnife");
			m_ItemsForSpawn.Insert("HayHook");
			m_ItemsForSpawn.Insert("ElectronicRepairKit");
			m_ItemsForSpawn.Insert("Thermometer");
			m_ItemsForSpawn.Insert("AlarmClock_Red");
			m_ItemsForSpawn.Insert("PurificationTablets");
			m_ItemsForSpawn.Insert("CharcoalTablets");
			m_ItemsForSpawn.Insert("PainkillerTablets");
			m_ItemsForSpawn.Insert("VitaminBottle");
			m_ItemsForSpawn.Insert("ChelatingTablets");
			m_ItemsForSpawn.Insert("TetracyclineAntibiotics");
			m_ItemsForSpawn.Insert("Epinephrine");
			m_ItemsForSpawn.Insert("Morphine");
			m_ItemsForSpawn.Insert("ClearSyringe");
			m_ItemsForSpawn.Insert("SalineBag");
			m_ItemsForSpawn.Insert("SalineBagIV");
			m_ItemsForSpawn.Insert("StartKitIV");
			m_ItemsForSpawn.Insert("Canteen");
			m_ItemsForSpawn.Insert("WaterBottle");
			m_ItemsForSpawn.Insert("GlassBottle");
			m_ItemsForSpawn.Insert("WaterBottle");
			m_ItemsForSpawn.Insert("Waterskin_Natural"); // 1.30: WaterPouch_Natural was renamed
			m_ItemsForSpawn.Insert("FilteringBottle");
			m_ItemsForSpawn.Insert("SodaCan_Pipsi");
			m_ItemsForSpawn.Insert("SodaCan_Cola");
			m_ItemsForSpawn.Insert("SodaCan_Spite");
			m_ItemsForSpawn.Insert("SodaCan_Kvass");
			m_ItemsForSpawn.Insert("SodaCan_Fronta");
			m_ItemsForSpawn.Insert("PowderedMilk");
			m_ItemsForSpawn.Insert("SmallGuts");
			m_ItemsForSpawn.Insert("Rice");
			m_ItemsForSpawn.Insert("Marmalade");
			m_ItemsForSpawn.Insert("Honey");
			m_ItemsForSpawn.Insert("Zagorky");
			m_ItemsForSpawn.Insert("ZagorkyChocolate");
			m_ItemsForSpawn.Insert("ZagorkyPeanuts");
			m_ItemsForSpawn.Insert("SaltySticks");
			m_ItemsForSpawn.Insert("Crackers");
			m_ItemsForSpawn.Insert("Chips");
			m_ItemsForSpawn.Insert("Candycane_Green");
			m_ItemsForSpawn.Insert("Candycane_Red");
			m_ItemsForSpawn.Insert("Candycane_RedGreen");
			m_ItemsForSpawn.Insert("Candycane_Yellow");
			m_ItemsForSpawn.Insert("BakedBeansCan");
			m_ItemsForSpawn.Insert("PeachesCan");
			m_ItemsForSpawn.Insert("TacticalBaconCan");
			m_ItemsForSpawn.Insert("SpaghettiCan");
			m_ItemsForSpawn.Insert("SardinesCan");
			m_ItemsForSpawn.Insert("TunaCan");
			m_ItemsForSpawn.Insert("DogFoodCan");
			m_ItemsForSpawn.Insert("CatFoodCan");
			m_ItemsForSpawn.Insert("PorkCan");
			m_ItemsForSpawn.Insert("Lunchmeat");
			m_ItemsForSpawn.Insert("CrabCan");
			m_ItemsForSpawn.Insert("UnknownFoodCan");
			m_ItemsForSpawn.Insert("Pajka");
			m_ItemsForSpawn.Insert("Pate");
			m_ItemsForSpawn.Insert("BrisketSpread");
			m_ItemsForSpawn.Insert("Apple");
			m_ItemsForSpawn.Insert("Plum");
			m_ItemsForSpawn.Insert("Pear");
			m_ItemsForSpawn.Insert("Banana");
			m_ItemsForSpawn.Insert("Orange");
			m_ItemsForSpawn.Insert("Tomato");
			m_ItemsForSpawn.Insert("GreenBellPepper");
			m_ItemsForSpawn.Insert("Zucchini");
			m_ItemsForSpawn.Insert("Pumpkin");
			m_ItemsForSpawn.Insert("SlicedPumpkin");
			m_ItemsForSpawn.Insert("Potato");
			m_ItemsForSpawn.Insert("Kiwi");
			m_ItemsForSpawn.Insert("SambucusBerry");
			m_ItemsForSpawn.Insert("CaninaBerry");
			m_ItemsForSpawn.Insert("Cannabis");
			m_ItemsForSpawn.Insert("AgaricusMushroom");
			m_ItemsForSpawn.Insert("AmanitaMushroom");
			m_ItemsForSpawn.Insert("MacrolepiotaMushroom");
			m_ItemsForSpawn.Insert("LactariusMushroom");
			m_ItemsForSpawn.Insert("PsilocybeMushroom");
			m_ItemsForSpawn.Insert("AuriculariaMushroom");
			m_ItemsForSpawn.Insert("BoletusMushroom");
			m_ItemsForSpawn.Insert("PleurotusMushroom");
			m_ItemsForSpawn.Insert("CraterellusMushroom");
			m_ItemsForSpawn.Insert("PetrolLighter");
			m_ItemsForSpawn.Insert("SFP_Vodka_Rus");
			m_ItemsForSpawn.Insert("SFP_Vodka_Sibir");
			m_ItemsForSpawn.Insert("SFP_Vodka_istrinka");
			m_ItemsForSpawn.Insert("SFP_Vodka_Kazak");
			m_ItemsForSpawn.Insert("FirstAidKit");
			m_ItemsForSpawn.Insert("SmallProtectorCase");
			m_ItemsForSpawn.Insert("AmmoBox");
			m_ItemsForSpawn.Insert("Bear_Pink");
			m_ItemsForSpawn.Insert("Roadflare");
			m_ItemsForSpawn.Insert("NailBox");
			m_ItemsForSpawn.Insert("Matchbox");
			m_ItemsForSpawn.Insert("Battery9V");
			m_ItemsForSpawn.Insert("Paper");
			m_ItemsForSpawn.Insert("SewingKit");
			m_ItemsForSpawn.Insert("WeaponCleaningKit");
			m_ItemsForSpawn.Insert("Heatpack");
			m_ItemsForSpawn.Insert("CanOpener");
			m_ItemsForSpawn.Insert("WorkingGloves_Beige");
			m_ItemsForSpawn.Insert("HandsCover_Improvised");
			m_ItemsForSpawn.Insert("SurgicalGloves_LightBlue");
			m_ItemsForSpawn.Insert("WoolGloves_Tan");
			m_ItemsForSpawn.Insert("WoolGlovesFingerless_Green");
			m_ItemsForSpawn.Insert("SkiGloves_Blue");
			m_ItemsForSpawn.Insert("BaseballCap_Olive");
			m_ItemsForSpawn.Insert("BeanieHat_Beige");
			m_ItemsForSpawn.Insert("Ushanka_Green");
			m_ItemsForSpawn.Insert("Headtorch_Grey");
			m_ItemsForSpawn.Insert("RadarCap_Black");
			m_ItemsForSpawn.Insert("SnowstormUshanka_Brown");
			m_ItemsForSpawn.Insert("SnowstormUshanka_Navy");
			m_ItemsForSpawn.Insert("Headdress_Wolf");
			m_ItemsForSpawn.Insert("SherpaHat_Blue");
			m_ItemsForSpawn.Insert("BoonieHat_Olive");
			m_ItemsForSpawn.Insert("FlatCap_BrownCheck");
			m_ItemsForSpawn.Insert("SFP_Cognac_Bootle");
			m_ItemsForSpawn.Insert("SFP_JackDan_Bootle");
			m_ItemsForSpawn.Insert("SFP_Mobile_NUKEA");
			m_ItemsForSpawn.Insert("SFP_NaseserRuArmy");
			m_ItemsForSpawn.Insert("SFP_Matchbox_Rus");
			m_ItemsForSpawn.Insert("SFP_2cigarettes_pack_camel");
			m_ItemsForSpawn.Insert("SFP_2cigarettes_pack_kent");
			m_ItemsForSpawn.Insert("SFP_2cigarettes_pack_marlboro");
			m_ItemsForSpawn.Insert("SFP_Battery9V_pack_Dur");
			m_ItemsForSpawn.Insert("SFP_Battery9V_pack_Energ");
			
						
			switch(action_data.m_MainItem.GetType()) 
			{
				case "SFP_Karton_box":
					// Тут спавним шмотки из массива m_ItemsForSpawn
					for (int i = 0; i < 7; ++i)
					{
						// Количество вещей для спавна регулируй в i < 15  
						string ItemName = m_ItemsForSpawn.GetRandomElement();
						Object newItem = GetGame().CreateObjectEx(ItemName, action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE); // Заспавнит на земле под ногами
					}
				break;	
				
				/*case "SFP_Karton_box_test":
					// Тут спавним шмотки из массива m_ItemsBotleWater
					for (int i = 0; Fido < 6; ++Fido)
					{
						// Количество вещей для спавна регулируй РІ i < 6  
						string ItemName = m_ItemsBotleWater.GetRandomElement();
						Object newItem = GetGame().CreateObjectEx(ItemName, action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE); // Заспавнит на земле под ногами
					}
				break;	*/
				
				
			}
			GetGame().ObjectDelete(action_data.m_MainItem);
		}
	}
};