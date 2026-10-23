static const string DIR_CRAFT  		= "$profile:\\DRL_CraftMenu\\";
static const string FILE 			= "$profile:\\DRL_CraftMenu\\CraftConfig.json";
static const string FILE_CATS		= "$profile:\\DRL_CraftMenu\\CraftCategories.json";
static const string FILE_SETTINGS 	= "$profile:\\DRL_CraftMenu\\CraftSettings.json";
static const string FILE_LEARNING 	= "$profile:\\DRL_CraftMenu\\CraftLearningConfig.json";
static const string DIR_PLAYERS 	= "$profile:\\DRL_CraftMenu\\Players\\";

modded class DRL_Craft_Config
{
	void SetDefault()
	{
		DRL_Craft_Recipe r1 = new DRL_Craft_Recipe();
		r1.RecipeId          = 1;
		r1.RecipeName         = "Сшить серую футболку";
		r1.Result             = "TShirt_Grey";
		r1.PreviewClassname   = "";
		r1.CraftIcon          = "DRL_CraftMenu/UI/data/error.edds";
		r1.Description 		  = "Сшиваем серую футболку из лоскутов ткани и швейного набора.";
		r1.CraftTimeSec       = 3;
		r1.ResultCount        = 1;
		r1.ResultQuantityFrac = 0.0;
		r1.ResultDamageFrac   = 0.0;
		
		r1.ResultClasses.Insert("TShirt_Grey");
		r1.ResultClasses.Insert("TShirt_Black");
		r1.ResultClasses.Insert("TShirt_Red");
		r1.ResultClasses.Insert("TShirt_Green");
		
		r1.ShowInFreeMenu = 1;
		r1.WorkbenchClasses.Insert("DRL_Craft_WB");
		
		DRL_Craft_Component r1a0 = new DRL_Craft_Component();
		r1a0.Classname          = "DRL_Craft_lab_bunsen";
		r1a0.Classnames.Insert("DRL_Craft_lab_bunsen");
		r1a0.EnableVariationSelection = 1;
		r1a0.UseInheritance     = 1;
		r1a0.MinHealthFrac      = 3;
		r1a0.Amount             = 1;
		r1a0.Delete             = false;
		r1a0.ApplyDamageFrac    = 0.00;
		r1a0.AttachmentsEx      = 1;
		r1a0.RecraftNeedAttachments = 0;
		r1.CraftComponents.Insert(r1a0);
		
		DRL_Craft_Component r1a1 = new DRL_Craft_Component();
		r1a1.Classname          = "TacticalGloves_Black";
		r1a1.UseInheritance     = 1;
		r1a1.MinHealthFrac      = 3;
		r1a1.Amount             = 1;
		r1a1.Delete             = false;
		r1a1.ApplyDamageFrac    = 0.25;
		r1a1.AttachmentsEx      = 1;
		r1a1.RecraftNeedAttachments = 0;
		r1.CraftComponents.Insert(r1a1);
		
		DRL_Craft_Component r1a2 = new DRL_Craft_Component();
		r1a2.Classname          = "DRL_Craft_Grinder";
		r1a2.UseInheritance     = 1;
		r1a2.MinHealthFrac      = 3;
		r1a2.Amount             = 1;
		r1a2.Delete             = false;
		r1a2.ApplyDamageFrac    = 1.00;
		r1a2.AttachmentsEx      = 1;
		r1a2.RecraftNeedAttachments = 0;
		r1.CraftComponents.Insert(r1a2);
	
		DRL_Craft_Component r1c1 = new DRL_Craft_Component();
		r1c1.Classname       = "Rag";
		r1c1.UseInheritance  = 1;
		r1c1.MinHealthFrac   = 3;
		r1c1.Amount          = 4;
		r1c1.QuantityFrac    = 0.0;
		r1c1.Delete          = true;
		r1c1.ApplyDamageFrac = 0.0;
		r1c1.RecraftDropChance = 1.0;
		r1.CraftComponents.Insert(r1c1);
	
		DRL_Craft_Component r1c2 = new DRL_Craft_Component();
		r1c2.Classname       = "SewingKit";
		r1c2.UseInheritance  = 1;
		r1c2.MinHealthFrac   = 3;
		r1c2.Amount          = 1;
		r1c2.QuantityFrac    = 0.25;
		r1c2.Delete          = false;
		r1c2.ApplyDamageFrac = 0.20;
		r1c2.RecraftDropChance = 0.2;
		r1.CraftComponents.Insert(r1c2);

		DRL_Craft_Component r1c3 = new DRL_Craft_Component();
		r1c3.Classname       = "Fabric";
		r1c3.UseInheritance  = 1;
		r1c3.MinHealthFrac   = 3;
		r1c3.Amount          = 1;
		r1c3.Delete          = true;
		r1c3.OnlyForResults.Insert("TShirt_Black");
		r1.CraftComponents.Insert(r1c3);
	
		CraftItems.Insert(r1);
		
		DRL_Craft_Recipe r2 = new DRL_Craft_Recipe();
		r2.RecipeId          = 2;
		r2.RecipeName         = "Ремонт разгрузки";
		r2.Result             = "UKAssVest_Khaki";
		r2.PreviewClassname   = "";
		r2.CraftIcon          = "";
		r2.Description 		  = "Сшиваем серую футболку из лоскутов ткани и швейного набора.";
		r2.CraftTimeSec       = 5;
		r2.ResultCount        = 1;
		r2.ResultQuantityFrac = 0.0;
		r2.ResultDamageFrac   = 0.25;
	
		DRL_Craft_Component r2c1 = new DRL_Craft_Component();
		r2c1.Classname       = "DuctTape";
		r2c1.UseInheritance  = 1;
		r2c1.MinHealthFrac   = 3;
		r2c1.Amount          = 1;
		r2c1.QuantityFrac    = 0.20;
		r2c1.Delete          = false;
		r2c1.ApplyDamageFrac = 0.50;
		r2c1.RecraftDropChance = 1.0;
		r2.CraftComponents.Insert(r2c1);
	
		CraftItems.Insert(r2);
		
		DRL_Craft_Recipe r3 = new DRL_Craft_Recipe();
		r3.RecipeId            = 3;
		r3.RecipeName           = "Шлем (баллистический)";
		r3.Result               = "BallisticHelmet_Black";
		r3.PreviewClassname     = "";
		r3.CraftIcon            = "";
		r3.Description          = "Усиленный шлем для защиты головы.";
		r3.CraftTimeSec         = 18;
		r3.ResultCount          = 1;
		r3.ResultQuantityFrac   = 0.0;
		r3.ResultDamageFrac     = 0.20;
	
		DRL_Craft_Component r3c1 = new DRL_Craft_Component();
		r3c1.Classname           = "MetalWire";
		r3c1.UseInheritance      = 1;
		r3c1.MinHealthFrac       = 3;
		r3c1.Amount              = 2;
		r3c1.QuantityFrac        = 0.0;
		r3c1.Delete              = true;
		r3c1.ApplyDamageFrac     = 0.0;
		r3c1.RecraftDropChance   = 0.75;
		r3.CraftComponents.Insert(r3c1);
	
		DRL_Craft_Component r3c2 = new DRL_Craft_Component();
		r3c2.Classname           = "Rag";
		r3c2.UseInheritance      = 1;
		r3c2.MinHealthFrac       = 3;
		r3c2.Amount              = 4;
		r3c2.QuantityFrac        = 0.0;
		r3c2.Delete              = true;
		r3c2.ApplyDamageFrac     = 0.0;
		r3c2.RecraftDropChance   = 0.5;
		r3.CraftComponents.Insert(r3c2);
	
		DRL_Craft_Component r3c3 = new DRL_Craft_Component();
		r3c3.Classname           = "DuctTape";
		r3c3.UseInheritance      = 1;
		r3c3.MinHealthFrac       = 3;
		r3c3.Amount              = 1;
		r3c3.QuantityFrac        = 0.35;
		r3c3.Delete              = false;
		r3c3.ApplyDamageFrac     = 0.6;
		r3c3.RecraftDropChance   = 0.25;
		r3.CraftComponents.Insert(r3c3);
	
		DRL_Craft_Component r3c4 = new DRL_Craft_Component();
		r3c4.Classname           = "SewingKit";
		r3c4.UseInheritance      = 1;
		r3c4.MinHealthFrac       = 3;
		r3c4.Amount              = 1;
		r3c4.QuantityFrac        = 0.30;
		r3c4.Delete              = false;
		r3c4.ApplyDamageFrac     = 0.5;
		r3c4.RecraftDropChance   = 0.9;
		r3.CraftComponents.Insert(r3c4);
	
		CraftItems.Insert(r3);
		
		DRL_Craft_Recipe r4 = new DRL_Craft_Recipe();
		r4.RecipeId            = 4;
		r4.RecipeName           = "Перчатки (тактические)";
		r4.Result               = "TacticalGloves_Black";
		r4.PreviewClassname     = "";
		r4.CraftIcon            = "";
		r4.Description          = "Прочные перчатки для боя и работы.";
		r4.CraftTimeSec         = 12;
		r4.ResultCount          = 1;
		r4.ResultQuantityFrac   = 0.0;
		r4.ResultDamageFrac     = 0.15;
	
		DRL_Craft_Component r4c1 = new DRL_Craft_Component();
		r4c1.Classname          = "TannedLeather";
		r4c1.UseInheritance     = 1;
		r4c1.MinHealthFrac      = 3;
		r4c1.Amount             = 1;
		r4c1.QuantityFrac       = 0.0;
		r4c1.Delete             = true;
		r4c1.ApplyDamageFrac    = 0.0;
		r4c1.RecraftDropChance  = 0.55;
		r4.CraftComponents.Insert(r4c1);
	
		DRL_Craft_Component r4c2 = new DRL_Craft_Component();
		r4c2.Classname          = "Rag";
		r4c2.UseInheritance     = 1;
		r4c2.MinHealthFrac      = 3;
		r4c2.Amount             = 2;
		r4c2.QuantityFrac       = 0.0;
		r4c2.Delete             = true;
		r4c2.ApplyDamageFrac    = 0.0;
		r4c2.RecraftDropChance  = 0.4;
		r4.CraftComponents.Insert(r4c2);
	
		DRL_Craft_Component r4c3 = new DRL_Craft_Component();
		r4c3.Classname          = "LeatherSewingKit";
		r4c3.UseInheritance     = 1;
		r4c3.MinHealthFrac      = 3;
		r4c3.Amount             = 1;
		r4c3.QuantityFrac       = 0.25;
		r4c3.Delete             = false;
		r4c3.ApplyDamageFrac    = 0.45;
		r4c3.RecraftDropChance  = 1.0;
		r4.CraftComponents.Insert(r4c3);
	
		CraftItems.Insert(r4);
		
		DRL_Craft_Recipe r5 = new DRL_Craft_Recipe();
		r5.RecipeId            = 5;
		r5.RecipeName           = "Жилет охотничий";
		r5.Result               = "UKAssVest_Khaki";
		r5.PreviewClassname     = "";
		r5.CraftIcon            = "";
		r5.Description          = "Разгрузочный жилет с карманами.";
		r5.CraftTimeSec         = 20;
		r5.ResultCount          = 1;
		r5.ResultQuantityFrac   = 0.0;
		r5.ResultDamageFrac     = 0.20;
	
		DRL_Craft_Component r5c1 = new DRL_Craft_Component();
		r5c1.Classname          = "BurlapSack";
		r5c1.UseInheritance     = 1;
		r5c1.MinHealthFrac      = 3;
		r5c1.Amount             = 2;
		r5c1.QuantityFrac       = 0.0;
		r5c1.Delete             = true;
		r5c1.ApplyDamageFrac    = 0.0;
		r5c1.RecraftDropChance  = 0.65;
		r5.CraftComponents.Insert(r5c1);
	
		DRL_Craft_Component r5c2 = new DRL_Craft_Component();
		r5c2.Classname          = "Rope";
		r5c2.UseInheritance     = 1;
		r5c2.MinHealthFrac      = 3;
		r5c2.Amount             = 1;
		r5c2.QuantityFrac       = 0.0;
		r5c2.Delete             = true;
		r5c2.ApplyDamageFrac    = 0.0;
		r5c2.RecraftDropChance  = 0.5;
		r5.CraftComponents.Insert(r5c2);
	
		DRL_Craft_Component r5c3 = new DRL_Craft_Component();
		r5c3.Classname          = "SewingKit";
		r5c3.UseInheritance     = 1;
		r5c3.MinHealthFrac      = 3;
		r5c3.Amount             = 1;
		r5c3.QuantityFrac       = 0.35;
		r5c3.Delete             = false;
		r5c3.ApplyDamageFrac    = 0.6;
		r5c3.RecraftDropChance  = 1.0;
		r5.CraftComponents.Insert(r5c3);
	
		CraftItems.Insert(r5);
		
		DRL_Craft_Recipe r6 = new DRL_Craft_Recipe();
		r6.RecipeId            = 6;
		r6.RecipeName           = "Броник (Plate Carrier)";
		r6.Result               = "PlateCarrierVest";
		r6.PreviewClassname     = "";
		r6.CraftIcon            = "";
		r6.Description          = "Тяжёлый жилет с бронепластинами.";
		r6.CraftTimeSec         = 35;
		r6.ResultCount          = 1;
		r6.ResultQuantityFrac   = 0.0;
		r6.ResultDamageFrac     = 0.35;
	
		DRL_Craft_Component r6c1 = new DRL_Craft_Component();
		r6c1.Classname          = "MetalWire";
		r6c1.UseInheritance     = 1;
		r6c1.MinHealthFrac      = 3;
		r6c1.Amount             = 2;
		r6c1.QuantityFrac       = 0.0;
		r6c1.Delete             = true;
		r6c1.ApplyDamageFrac    = 0.0;
		r6c1.RecraftDropChance  = 0.8;
		r6.CraftComponents.Insert(r6c1);
	
		DRL_Craft_Component r6c2 = new DRL_Craft_Component();
		r6c2.Classname          = "DuctTape";
		r6c2.UseInheritance     = 1;
		r6c2.MinHealthFrac      = 3;
		r6c2.Amount             = 1;
		r6c2.QuantityFrac       = 0.4;
		r6c2.Delete             = false;
		r6c2.ApplyDamageFrac    = 0.6;
		r6c2.RecraftDropChance  = 0.35;
		r6.CraftComponents.Insert(r6c2);
	
		DRL_Craft_Component r6c3 = new DRL_Craft_Component();
		r6c3.Classname          = "Rag";
		r6c3.UseInheritance     = 1;
		r6c3.MinHealthFrac      = 3;
		r6c3.Amount             = 6;
		r6c3.QuantityFrac       = 0.0;
		r6c3.Delete             = true;
		r6c3.ApplyDamageFrac    = 0.0;
		r6c3.RecraftDropChance  = 0.7;
		r6.CraftComponents.Insert(r6c3);
	
		CraftItems.Insert(r6);
		
		DRL_Craft_Recipe r7 = new DRL_Craft_Recipe();
		r7.RecipeId            = 7;
		r7.RecipeName           = "Куртка (охотничья)";
		r7.Result               = "HuntingJacket_Autumn";
		r7.PreviewClassname     = "";
		r7.CraftIcon            = "";
		r7.Description          = "Тёплая куртка, подходит для охоты.";
		r7.CraftTimeSec         = 16;
		r7.ResultCount          = 1;
		r7.ResultQuantityFrac   = 0.0;
		r7.ResultDamageFrac     = 0.15;
	
		DRL_Craft_Component r7c1 = new DRL_Craft_Component();
		r7c1.Classname          = "Rag";
		r7c1.UseInheritance     = 1;
		r7c1.MinHealthFrac      = 3;
		r7c1.Amount             = 8;
		r7c1.QuantityFrac       = 0.0;
		r7c1.Delete             = true;
		r7c1.ApplyDamageFrac    = 0.0;
		r7c1.RecraftDropChance  = 0.65;
		r7.CraftComponents.Insert(r7c1);
	
		DRL_Craft_Component r7c2 = new DRL_Craft_Component();
		r7c2.Classname          = "SewingKit";
		r7c2.UseInheritance     = 1;
		r7c2.MinHealthFrac      = 3;
		r7c2.Amount             = 1;
		r7c2.QuantityFrac       = 0.45;
		r7c2.Delete             = false;
		r7c2.ApplyDamageFrac    = 0.6;
		r7c2.RecraftDropChance  = 0.95;
		r7.CraftComponents.Insert(r7c2);
	
		CraftItems.Insert(r7);
		
		DRL_Craft_Recipe r8 = new DRL_Craft_Recipe();
		r8.RecipeId            = 8;
		r8.RecipeName           = "Штаны (карго)";
		r8.Result               = "CargoPants_Black";
		r8.PreviewClassname     = "";
		r8.CraftIcon            = "";
		r8.Description          = "Практичные штаны с множеством карманов.";
		r8.CraftTimeSec         = 14;
		r8.ResultCount          = 1;
		r8.ResultQuantityFrac   = 0.0;
		r8.ResultDamageFrac     = 0.15;
	
		DRL_Craft_Component r8c1 = new DRL_Craft_Component();
		r8c1.Classname          = "Rag";
		r8c1.UseInheritance     = 1;
		r8c1.MinHealthFrac      = 3;
		r8c1.Amount             = 8;
		r8c1.QuantityFrac       = 0.0;
		r8c1.Delete             = true;
		r8c1.ApplyDamageFrac    = 0.0;
		r8c1.RecraftDropChance  = 0.6;
		r8.CraftComponents.Insert(r8c1);
	
		DRL_Craft_Component r8c2 = new DRL_Craft_Component();
		r8c2.Classname          = "SewingKit";
		r8c2.UseInheritance     = 1;
		r8c2.MinHealthFrac      = 3;
		r8c2.Amount             = 1;
		r8c2.QuantityFrac       = 0.4;
		r8c2.Delete             = false;
		r8c2.ApplyDamageFrac    = 0.55;
		r8c2.RecraftDropChance  = 0.95;
		r8.CraftComponents.Insert(r8c2);
	
		CraftItems.Insert(r8);
		
		DRL_Craft_Recipe r9 = new DRL_Craft_Recipe();
		r9.RecipeId            = 9;
		r9.RecipeName           = "Автомат (AKM)";
		r9.Result               = "AKM";
		r9.PreviewClassname     = "";
		r9.CraftIcon            = "";
		r9.Description          = "Сборка автомата из деталей и расходников.";
		r9.CraftTimeSec         = 60;
		r9.ResultCount          = 1;
		r9.ResultQuantityFrac   = 0.0;
		r9.ResultDamageFrac     = 0.40;
	
		DRL_Craft_Component r9c1 = new DRL_Craft_Component();
		r9c1.Classname          = "MetalWire";
		r9c1.UseInheritance     = 1;
		r9c1.MinHealthFrac      = 3;
		r9c1.Amount             = 3;
		r9c1.QuantityFrac       = 0.0;
		r9c1.Delete             = true;
		r9c1.ApplyDamageFrac    = 0.0;
		r9c1.RecraftDropChance  = 0.5;
		r9.CraftComponents.Insert(r9c1);
	
		DRL_Craft_Component r9c2 = new DRL_Craft_Component();
		r9c2.Classname          = "Nails";
		r9c2.UseInheritance     = 1;
		r9c2.MinHealthFrac      = 3;
		r9c2.Amount             = 1;
		r9c2.QuantityFrac       = 0.6;
		r9c2.Delete             = true;
		r9c2.ApplyDamageFrac    = 0.0;
		r9c2.RecraftDropChance  = 0.35;
		r9.CraftComponents.Insert(r9c2);
	
		DRL_Craft_Component r9c3 = new DRL_Craft_Component();
		r9c3.Classname          = "Screwdriver";
		r9c3.UseInheritance     = 1;
		r9c3.MinHealthFrac      = 3;
		r9c3.Amount             = 1;
		r9c3.QuantityFrac       = 0.0;
		r9c3.Delete             = false;
		r9c3.ApplyDamageFrac    = 0.6;
		r9c3.RecraftDropChance  = 0.9;
		r9.CraftComponents.Insert(r9c3);
	
		DRL_Craft_Component r9c4 = new DRL_Craft_Component();
		r9c4.Classname          = "DuctTape";
		r9c4.UseInheritance     = 1;
		r9c4.MinHealthFrac      = 3;
		r9c4.Amount             = 1;
		r9c4.QuantityFrac       = 0.5;
		r9c4.Delete             = false;
		r9c4.ApplyDamageFrac    = 0.7;
		r9c4.RecraftDropChance  = 0.2;
		r9.CraftComponents.Insert(r9c4);
	
		CraftItems.Insert(r9);
		
		DRL_Craft_Recipe r10 = new DRL_Craft_Recipe();
		r10.RecipeId            = 10;
		r10.RecipeName          = "Напиток (Pipsi)";
		r10.Result              = "SodaCan_Pipsi";
		r10.PreviewClassname    = "";
		r10.CraftIcon           = "";
		r10.Description         = "Смешиваем газировку из компонентов.";
		r10.CraftTimeSec        = 6;
		r10.ResultCount         = 1;
		r10.ResultQuantityFrac  = 1.0;
		r10.ResultDamageFrac    = 0.0;
	
		DRL_Craft_Component r10c1 = new DRL_Craft_Component();
		r10c1.Classname         = "WaterBottle";
		r10c1.UseInheritance    = 1;
		r10c1.MinHealthFrac     = 3;
		r10c1.Amount            = 1;
		r10c1.QuantityFrac      = 0.35;
		r10c1.Delete            = false;
		r10c1.ApplyDamageFrac   = 0.1;
		r10c1.RecraftDropChance = 0.95;
		r10.CraftComponents.Insert(r10c1);
	
		DRL_Craft_Component r10c2 = new DRL_Craft_Component();
		r10c2.Classname         = "Rice";
		r10c2.UseInheritance    = 1;
		r10c2.MinHealthFrac     = 3;
		r10c2.Amount            = 1;
		r10c2.QuantityFrac      = 0.15;
		r10c2.Delete            = true;
		r10c2.ApplyDamageFrac   = 0.0;
		r10c2.RecraftDropChance = 0.4;
		r10.CraftComponents.Insert(r10c2);
	
		CraftItems.Insert(r10);
		
		DRL_Craft_Recipe r11 = new DRL_Craft_Recipe();
		r11.RecipeId            = 11;
		r11.RecipeName          = "Еда (рагу в банке)";
		r11.Result              = "SpaghettiCan";
		r11.PreviewClassname    = "";
		r11.CraftIcon           = "";
		r11.Description         = "Простое рагу в консерве.";
		r11.CraftTimeSec        = 10;
		r11.ResultCount         = 1;
		r11.ResultQuantityFrac  = 1.0;
		r11.ResultDamageFrac    = 0.0;
	
		DRL_Craft_Component r11c1 = new DRL_Craft_Component();
		r11c1.Classname         = "Zucchini";
		r11c1.UseInheritance    = 1;
		r11c1.MinHealthFrac     = 3;
		r11c1.Amount            = 1;
		r11c1.QuantityFrac      = 0.0;
		r11c1.Delete            = true;
		r11c1.ApplyDamageFrac   = 0.0;
		r11c1.RecraftDropChance = 0.5;
		r11.CraftComponents.Insert(r11c1);
	
		DRL_Craft_Component r11c2 = new DRL_Craft_Component();
		r11c2.Classname         = "GoatSteakMeat";
		r11c2.UseInheritance    = 1;
		r11c2.MinHealthFrac     = 3;
		r11c2.Amount            = 1;
		r11c2.QuantityFrac      = 0.0;
		r11c2.Delete            = true;
		r11c2.ApplyDamageFrac   = 0.0;
		r11c2.RecraftDropChance = 0.45;
		r11.CraftComponents.Insert(r11c2);
	
		DRL_Craft_Component r11c3 = new DRL_Craft_Component();
		r11c3.Classname         = "CanOpener";
		r11c3.UseInheritance    = 1;
		r11c3.MinHealthFrac     = 3;
		r11c3.Amount            = 1;
		r11c3.QuantityFrac      = 0.0;
		r11c3.Delete            = false;
		r11c3.ApplyDamageFrac   = 0.35;
		r11c3.RecraftDropChance = 0.95;
		r11.CraftComponents.Insert(r11c3);
	
		CraftItems.Insert(r11);
		
		DRL_Craft_Recipe r12 = new DRL_Craft_Recipe();
		r12.RecipeId            = 12;
		r12.RecipeName          = "Нож (кухонный)";
		r12.Result              = "KitchenKnife";
		r12.PreviewClassname    = "";
		r12.CraftIcon           = "";
		r12.Description         = "Бытовой нож средней прочности.";
		r12.CraftTimeSec        = 15;
		r12.ResultCount         = 1;
		r12.ResultQuantityFrac  = 0.0;
		r12.ResultDamageFrac    = 0.25;
	
		DRL_Craft_Component r12c1 = new DRL_Craft_Component();
		r12c1.Classname         = "MetalWire";
		r12c1.UseInheritance    = 1;
		r12c1.MinHealthFrac     = 3;
		r12c1.Amount            = 1;
		r12c1.QuantityFrac      = 0.0;
		r12c1.Delete            = true;
		r12c1.ApplyDamageFrac   = 0.0;
		r12c1.RecraftDropChance = 0.7;
		r12.CraftComponents.Insert(r12c1);
	
		DRL_Craft_Component r12c2 = new DRL_Craft_Component();
		r12c2.Classname         = "Whetstone";
		r12c2.UseInheritance    = 1;
		r12c2.MinHealthFrac     = 3;
		r12c2.Amount            = 1;
		r12c2.QuantityFrac      = 0.0;
		r12c2.Delete            = false;
		r12c2.ApplyDamageFrac   = 0.6;
		r12c2.RecraftDropChance = 0.9;
		r12.CraftComponents.Insert(r12c2);
	
		DRL_Craft_Component r12c3 = new DRL_Craft_Component();
		r12c3.Classname         = "Rag";
		r12c3.UseInheritance    = 1;
		r12c3.MinHealthFrac     = 3;
		r12c3.Amount            = 2;
		r12c3.QuantityFrac      = 0.0;
		r12c3.Delete            = true;
		r12c3.ApplyDamageFrac   = 0.0;
		r12c3.RecraftDropChance = 0.5;
		r12.CraftComponents.Insert(r12c3);
	
		CraftItems.Insert(r12);
		
		DRL_Craft_Recipe r13 = new DRL_Craft_Recipe();
		r13.RecipeId            = 13;
		r13.RecipeName          = "Ремкомплект (оружейный)";
		r13.Result              = "WeaponCleaningKit";
		r13.PreviewClassname    = "";
		r13.CraftIcon           = "";
		r13.Description         = "Набор для обслуживания оружия.";
		r13.CraftTimeSec        = 10;
		r13.ResultCount         = 1;
		r13.ResultQuantityFrac  = 0.0;
		r13.ResultDamageFrac    = 0.0;
	
		DRL_Craft_Component r13c1 = new DRL_Craft_Component();
		r13c1.Classname         = "Rag";
		r13c1.UseInheritance    = 1;
		r13c1.MinHealthFrac     = 3;
		r13c1.Amount            = 3;
		r13c1.QuantityFrac      = 0.0;
		r13c1.Delete            = true;
		r13c1.ApplyDamageFrac   = 0.0;
		r13c1.RecraftDropChance = 0.7;
		r13.CraftComponents.Insert(r13c1);
	
		DRL_Craft_Component r13c2 = new DRL_Craft_Component();
		r13c2.Classname         = "AlcoholTincture";
		r13c2.UseInheritance    = 1;
		r13c2.MinHealthFrac     = 3;
		r13c2.Amount            = 1;
		r13c2.QuantityFrac      = 0.40;
		r13c2.Delete            = true;
		r13c2.ApplyDamageFrac   = 0.0;
		r13c2.RecraftDropChance = 0.4;
		r13.CraftComponents.Insert(r13c2);
	
		DRL_Craft_Component r13c3 = new DRL_Craft_Component();
		r13c3.Classname         = "DuctTape";
		r13c3.UseInheritance    = 1;
		r13c3.MinHealthFrac     = 3;
		r13c3.Amount            = 1;
		r13c3.QuantityFrac      = 0.25;
		r13c3.Delete            = false;
		r13c3.ApplyDamageFrac   = 0.55;
		r13c3.RecraftDropChance = 0.3;
		r13.CraftComponents.Insert(r13c3);
	
		CraftItems.Insert(r13);
		
		DRL_Craft_Recipe r14 = new DRL_Craft_Recipe();
		r14.RecipeId            = 14;
		r14.RecipeName          = "Ремкомплект (швейный)";
		r14.Result              = "SewingKit";
		r14.PreviewClassname    = "";
		r14.CraftIcon           = "";
		r14.Description         = "Комплект ниток и игл для ремонта одежды.";
		r14.CraftTimeSec        = 8;
		r14.ResultCount         = 1;
		r14.ResultQuantityFrac  = 0.0;
		r14.ResultDamageFrac    = 0.0;
	
		DRL_Craft_Component r14c1 = new DRL_Craft_Component();
		r14c1.Classname         = "Rag";
		r14c1.UseInheritance    = 1;
		r14c1.MinHealthFrac     = 3;
		r14c1.Amount            = 2;
		r14c1.QuantityFrac      = 0.0;
		r14c1.Delete            = true;
		r14c1.ApplyDamageFrac   = 0.0;
		r14c1.RecraftDropChance = 0.6;
		r14.CraftComponents.Insert(r14c1);
	
		DRL_Craft_Component r14c2 = new DRL_Craft_Component();
		r14c2.Classname         = "BoneHook";
		r14c2.UseInheritance    = 1;
		r14c2.MinHealthFrac     = 3;
		r14c2.Amount            = 1;
		r14c2.QuantityFrac      = 0.0;
		r14c2.Delete            = true;
		r14c2.ApplyDamageFrac   = 0.0;
		r14c2.RecraftDropChance = 0.35;
		r14.CraftComponents.Insert(r14c2);
	
		DRL_Craft_Component r14c3 = new DRL_Craft_Component();
		r14c3.Classname         = "DuctTape";
		r14c3.UseInheritance    = 1;
		r14c3.MinHealthFrac     = 3;
		r14c3.Amount            = 1;
		r14c3.QuantityFrac      = 0.2;
		r14c3.Delete            = false;
		r14c3.ApplyDamageFrac   = 0.5;
		r14c3.RecraftDropChance = 0.25;
		r14.CraftComponents.Insert(r14c3);
	
		CraftItems.Insert(r14);

		DRL_Craft_Recipe r15 = new DRL_Craft_Recipe();
		r15.RecipeId            = 15;
		r15.RecipeName          = "Налобный фонарь";
		r15.Result              = "HeadTorch_Black";
		r15.PreviewClassname    = "";
		r15.CraftIcon           = "";
		r15.Description         = "Простой налобный фонарь для ночной разведки.";
		r15.CraftTimeSec        = 10;
		r15.ResultCount         = 1;
		r15.ResultQuantityFrac  = 0.0;
		r15.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r15c1 = new DRL_Craft_Component();
		r15c1.Classname         = "Rag";
		r15c1.UseInheritance    = 1;
		r15c1.MinHealthFrac     = 3;
		r15c1.Amount            = 2;
		r15c1.QuantityFrac      = 0.0;
		r15c1.Delete            = true;
		r15c1.ApplyDamageFrac   = 0.0;
		r15c1.RecraftDropChance = 0.6;
		r15.CraftComponents.Insert(r15c1);
		
		DRL_Craft_Component r15c2 = new DRL_Craft_Component();
		r15c2.Classname         = "Battery9V";
		r15c2.UseInheritance    = 1;
		r15c2.MinHealthFrac     = 3;
		r15c2.Amount            = 1;
		r15c2.QuantityFrac      = 1.0;
		r15c2.Delete            = true;
		r15c2.ApplyDamageFrac   = 0.0;
		r15c2.RecraftDropChance = 0.9;
		r15.CraftComponents.Insert(r15c2);
		
		DRL_Craft_Component r15c3 = new DRL_Craft_Component();
		r15c3.Classname         = "SewingKit";
		r15c3.UseInheritance    = 1;
		r15c3.MinHealthFrac     = 3;
		r15c3.Amount            = 1;
		r15c3.QuantityFrac      = 0.25;
		r15c3.Delete            = false;
		r15c3.ApplyDamageFrac   = 0.5;
		r15c3.RecraftDropChance = 0.95;
		r15.CraftComponents.Insert(r15c3);
		
		CraftItems.Insert(r15);
		
		
		DRL_Craft_Recipe r16 = new DRL_Craft_Recipe();
		r16.RecipeId            = 16;
		r16.RecipeName          = "Рюкзак (курьерский)";
		r16.Result              = "CourierBag";
		r16.PreviewClassname    = "";
		r16.CraftIcon           = "";
		r16.Description         = "Лёгкий самодельный рюкзак из мешка и верёвки.";
		r16.CraftTimeSec        = 12;
		r16.ResultCount         = 1;
		r16.ResultQuantityFrac  = 0.0;
		r16.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r16c1 = new DRL_Craft_Component();
		r16c1.Classname         = "BurlapSack";
		r16c1.UseInheritance    = 1;
		r16c1.MinHealthFrac     = 3;
		r16c1.Amount            = 1;
		r16c1.QuantityFrac      = 0.0;
		r16c1.Delete            = true;
		r16c1.ApplyDamageFrac   = 0.0;
		r16c1.RecraftDropChance = 0.7;
		r16.CraftComponents.Insert(r16c1);
		
		DRL_Craft_Component r16c2 = new DRL_Craft_Component();
		r16c2.Classname         = "Rope";
		r16c2.UseInheritance    = 1;
		r16c2.MinHealthFrac     = 3;
		r16c2.Amount            = 1;
		r16c2.QuantityFrac      = 0.0;
		r16c2.Delete            = true;
		r16c2.ApplyDamageFrac   = 0.0;
		r16c2.RecraftDropChance = 0.8;
		r16.CraftComponents.Insert(r16c2);
		
		CraftItems.Insert(r16);
		
		
		DRL_Craft_Recipe r17 = new DRL_Craft_Recipe();
		r17.RecipeId            = 17;
		r17.RecipeName          = "Факел";
		r17.Result              = "Torch";
		r17.PreviewClassname    = "";
		r17.CraftIcon           = "";
		r17.Description         = "Простой источник света из палки и ткани.";
		r17.CraftTimeSec        = 5;
		r17.ResultCount         = 1;
		r17.ResultQuantityFrac  = 0.0;
		r17.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r17c1 = new DRL_Craft_Component();
		r17c1.Classname         = "LongWoodenStick";
		r17c1.UseInheritance    = 1;
		r17c1.MinHealthFrac     = 3;
		r17c1.Amount            = 1;
		r17c1.QuantityFrac      = 0.0;
		r17c1.Delete            = true;
		r17c1.ApplyDamageFrac   = 0.0;
		r17c1.RecraftDropChance = 0.8;
		r17.CraftComponents.Insert(r17c1);
		
		DRL_Craft_Component r17c2 = new DRL_Craft_Component();
		r17c2.Classname         = "Rag";
		r17c2.UseInheritance    = 1;
		r17c2.MinHealthFrac     = 3;
		r17c2.Amount            = 2;
		r17c2.QuantityFrac      = 0.0;
		r17c2.Delete            = true;
		r17c2.ApplyDamageFrac   = 0.0;
		r17c2.RecraftDropChance = 0.6;
		r17.CraftComponents.Insert(r17c2);
		
		CraftItems.Insert(r17);
		
		
		DRL_Craft_Recipe r18 = new DRL_Craft_Recipe();
		r18.RecipeId            = 18;
		r18.RecipeName          = "Крюк (костяной)";
		r18.Result              = "BoneHook";
		r18.PreviewClassname    = "";
		r18.CraftIcon           = "";
		r18.Description         = "Костяной рыболовный крючок.";
		r18.CraftTimeSec        = 6;
		r18.ResultCount         = 1;
		r18.ResultQuantityFrac  = 0.0;
		r18.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r18c1 = new DRL_Craft_Component();
		r18c1.Classname         = "Bone";
		r18c1.UseInheritance    = 1;
		r18c1.MinHealthFrac     = 3;
		r18c1.Amount            = 1;
		r18c1.QuantityFrac      = 0.0;
		r18c1.Delete            = true;
		r18c1.ApplyDamageFrac   = 0.0;
		r18c1.RecraftDropChance = 0.7;
		r18.CraftComponents.Insert(r18c1);
		
		DRL_Craft_Component r18c2 = new DRL_Craft_Component();
		r18c2.Classname         = "Knife";
		r18c2.UseInheritance    = 1;
		r18c2.MinHealthFrac     = 3;
		r18c2.Amount            = 1;
		r18c2.QuantityFrac      = 0.0;
		r18c2.Delete            = false;
		r18c2.ApplyDamageFrac   = 0.4;
		r18c2.RecraftDropChance = 1.0;
		r18.CraftComponents.Insert(r18c2);
		
		CraftItems.Insert(r18);
		
		
		DRL_Craft_Recipe r19 = new DRL_Craft_Recipe();
		r19.RecipeId            = 19;
		r19.RecipeName          = "Дрова";
		r19.Result              = "Firewood";
		r19.PreviewClassname    = "";
		r19.CraftIcon           = "";
		r19.Description         = "Подготовка поленьев для костра.";
		r19.CraftTimeSec        = 6;
		r19.ResultCount         = 1;
		r19.ResultQuantityFrac  = 0.0;
		r19.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r19c1 = new DRL_Craft_Component();
		r19c1.Classname         = "WoodenStick";
		r19c1.UseInheritance    = 1;
		r19c1.MinHealthFrac     = 3;
		r19c1.Amount            = 3;
		r19c1.QuantityFrac      = 0.0;
		r19c1.Delete            = true;
		r19c1.ApplyDamageFrac   = 0.0;
		r19c1.RecraftDropChance = 0.8;
		r19.CraftComponents.Insert(r19c1);
		
		DRL_Craft_Component r19c2 = new DRL_Craft_Component();
		r19c2.Classname         = "Hatchet";
		r19c2.UseInheritance    = 1;
		r19c2.MinHealthFrac     = 3;
		r19c2.Amount            = 1;
		r19c2.QuantityFrac      = 0.0;
		r19c2.Delete            = false;
		r19c2.ApplyDamageFrac   = 0.35;
		r19c2.RecraftDropChance = 1.0;
		r19.CraftComponents.Insert(r19c2);
		
		CraftItems.Insert(r19);
		
		
		DRL_Craft_Recipe r20 = new DRL_Craft_Recipe();
		r20.RecipeId            = 20;
		r20.RecipeName          = "Сумка (кожаная)";
		r20.Result              = "LeatherSack_Beige";
		r20.PreviewClassname    = "";
		r20.CraftIcon           = "";
		r20.Description         = "Прочная кожаная сумка.";
		r20.CraftTimeSec        = 22;
		r20.ResultCount         = 1;
		r20.ResultQuantityFrac  = 0.0;
		r20.ResultDamageFrac    = 0.15;
		
		DRL_Craft_Component r20c1 = new DRL_Craft_Component();
		r20c1.Classname         = "TannedLeather";
		r20c1.UseInheritance    = 1;
		r20c1.MinHealthFrac     = 3;
		r20c1.Amount            = 2;
		r20c1.QuantityFrac      = 0.0;
		r20c1.Delete            = true;
		r20c1.ApplyDamageFrac   = 0.0;
		r20c1.RecraftDropChance = 0.7;
		r20.CraftComponents.Insert(r20c1);
		
		DRL_Craft_Component r20c2 = new DRL_Craft_Component();
		r20c2.Classname         = "LeatherSewingKit";
		r20c2.UseInheritance    = 1;
		r20c2.MinHealthFrac     = 3;
		r20c2.Amount            = 1;
		r20c2.QuantityFrac      = 0.35;
		r20c2.Delete            = false;
		r20c2.ApplyDamageFrac   = 0.55;
		r20c2.RecraftDropChance = 0.95;
		r20.CraftComponents.Insert(r20c2);
		
		DRL_Craft_Component r20c3 = new DRL_Craft_Component();
		r20c3.Classname         = "Rope";
		r20c3.UseInheritance    = 1;
		r20c3.MinHealthFrac     = 3;
		r20c3.Amount            = 1;
		r20c3.QuantityFrac      = 0.0;
		r20c3.Delete            = true;
		r20c3.ApplyDamageFrac   = 0.0;
		r20c3.RecraftDropChance = 0.6;
		r20.CraftComponents.Insert(r20c3);
		
		CraftItems.Insert(r20);
		
		
		DRL_Craft_Recipe r21 = new DRL_Craft_Recipe();
		r21.RecipeId            = 21;
		r21.RecipeName          = "Огниво (лучина)";
		r21.Result              = "HandDrillKit";
		r21.PreviewClassname    = "";
		r21.CraftIcon           = "";
		r21.Description         = "Самодельное огниво для розжига.";
		r21.CraftTimeSec        = 6;
		r21.ResultCount         = 1;
		r21.ResultQuantityFrac  = 0.0;
		r21.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r21c1 = new DRL_Craft_Component();
		r21c1.Classname         = "Bark_Oak";
		r21c1.UseInheritance    = 1;
		r21c1.MinHealthFrac     = 3;
		r21c1.Amount            = 1;
		r21c1.QuantityFrac      = 0.0;
		r21c1.Delete            = true;
		r21c1.ApplyDamageFrac   = 0.0;
		r21c1.RecraftDropChance = 0.8;
		r21.CraftComponents.Insert(r21c1);
		
		DRL_Craft_Component r21c2 = new DRL_Craft_Component();
		r21c2.Classname         = "WoodenStick";
		r21c2.UseInheritance    = 1;
		r21c2.MinHealthFrac     = 3;
		r21c2.Amount            = 1;
		r21c2.QuantityFrac      = 0.0;
		r21c2.Delete            = true;
		r21c2.ApplyDamageFrac   = 0.0;
		r21c2.RecraftDropChance = 0.8;
		r21.CraftComponents.Insert(r21c2);
		
		CraftItems.Insert(r21);
		
		
		DRL_Craft_Recipe r22 = new DRL_Craft_Recipe();
		r22.RecipeId            = 22;
		r22.RecipeName          = "Удочка";
		r22.Result              = "FishingRod";
		r22.PreviewClassname    = "";
		r22.CraftIcon           = "";
		r22.Description         = "Самодельная удочка для рыбалки.";
		r22.CraftTimeSec        = 12;
		r22.ResultCount         = 1;
		r22.ResultQuantityFrac  = 0.0;
		r22.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r22c1 = new DRL_Craft_Component();
		r22c1.Classname         = "LongWoodenStick";
		r22c1.UseInheritance    = 1;
		r22c1.MinHealthFrac     = 3;
		r22c1.Amount            = 1;
		r22c1.QuantityFrac      = 0.0;
		r22c1.Delete            = true;
		r22c1.ApplyDamageFrac   = 0.0;
		r22c1.RecraftDropChance = 0.8;
		r22.CraftComponents.Insert(r22c1);
		
		DRL_Craft_Component r22c2 = new DRL_Craft_Component();
		r22c2.Classname         = "Rope";
		r22c2.UseInheritance    = 1;
		r22c2.MinHealthFrac     = 3;
		r22c2.Amount            = 1;
		r22c2.QuantityFrac      = 0.0;
		r22c2.Delete            = true;
		r22c2.ApplyDamageFrac   = 0.0;
		r22c2.RecraftDropChance = 0.7;
		r22.CraftComponents.Insert(r22c2);
		
		CraftItems.Insert(r22);
		
		
		DRL_Craft_Recipe r23 = new DRL_Craft_Recipe();
		r23.RecipeId            = 23;
		r23.RecipeName          = "Капельница (IV)";
		r23.Result              = "SalineBagIV";
		r23.PreviewClassname    = "";
		r23.CraftIcon           = "";
		r23.Description         = "Собираем систему из пакета и набора.";
		r23.CraftTimeSec        = 10;
		r23.ResultCount         = 1;
		r23.ResultQuantityFrac  = 0.0;
		r23.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r23c1 = new DRL_Craft_Component();
		r23c1.Classname         = "SalineBag";
		r23c1.UseInheritance    = 1;
		r23c1.MinHealthFrac     = 3;
		r23c1.Amount            = 1;
		r23c1.QuantityFrac      = 0.0;
		r23c1.Delete            = true;
		r23c1.ApplyDamageFrac   = 0.0;
		r23c1.RecraftDropChance = 0.7;
		r23.CraftComponents.Insert(r23c1);
		
		DRL_Craft_Component r23c2 = new DRL_Craft_Component();
		r23c2.Classname         = "IVStartKit";
		r23c2.UseInheritance    = 1;
		r23c2.MinHealthFrac     = 3;
		r23c2.Amount            = 1;
		r23c2.QuantityFrac      = 0.0;
		r23c2.Delete            = true;
		r23c2.ApplyDamageFrac   = 0.0;
		r23c2.RecraftDropChance = 0.7;
		r23.CraftComponents.Insert(r23c2);
		
		CraftItems.Insert(r23);
		
		
		DRL_Craft_Recipe r24 = new DRL_Craft_Recipe();
		r24.RecipeId            = 24;
		r24.RecipeName          = "Шина (сплинт)";
		r24.Result              = "Splint";
		r24.PreviewClassname    = "";
		r24.CraftIcon           = "";
		r24.Description         = "Иммобилизация конечности из подручных средств.";
		r24.CraftTimeSec        = 6;
		r24.ResultCount         = 1;
		r24.ResultQuantityFrac  = 0.0;
		r24.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r24c1 = new DRL_Craft_Component();
		r24c1.Classname         = "LongWoodenStick";
		r24c1.UseInheritance    = 1;
		r24c1.MinHealthFrac     = 3;
		r24c1.Amount            = 1;
		r24c1.QuantityFrac      = 0.0;
		r24c1.Delete            = true;
		r24c1.ApplyDamageFrac   = 0.0;
		r24c1.RecraftDropChance = 0.8;
		r24.CraftComponents.Insert(r24c1);
		
		DRL_Craft_Component r24c2 = new DRL_Craft_Component();
		r24c2.Classname         = "Rag";
		r24c2.UseInheritance    = 1;
		r24c2.MinHealthFrac     = 3;
		r24c2.Amount            = 2;
		r24c2.QuantityFrac      = 0.0;
		r24c2.Delete            = true;
		r24c2.ApplyDamageFrac   = 0.0;
		r24c2.RecraftDropChance = 0.6;
		r24.CraftComponents.Insert(r24c2);
		
		CraftItems.Insert(r24);
		
		
		DRL_Craft_Recipe r25 = new DRL_Craft_Recipe();
		r25.RecipeId            = 25;
		r25.RecipeName          = "Нож (каменный)";
		r25.Result              = "StoneKnife";
		r25.PreviewClassname    = "";
		r25.CraftIcon           = "";
		r25.Description         = "Простой нож из камней.";
		r25.CraftTimeSec        = 5;
		r25.ResultCount         = 1;
		r25.ResultQuantityFrac  = 0.0;
		r25.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r25c1 = new DRL_Craft_Component();
		r25c1.Classname         = "Stone";
		r25c1.UseInheritance    = 1;
		r25c1.MinHealthFrac     = 3;
		r25c1.Amount            = 2;
		r25c1.QuantityFrac      = 0.0;
		r25c1.Delete            = true;
		r25c1.ApplyDamageFrac   = 0.0;
		r25c1.RecraftDropChance = 0.9;
		r25.CraftComponents.Insert(r25c1);
		
		CraftItems.Insert(r25);
		
		
		DRL_Craft_Recipe r26 = new DRL_Craft_Recipe();
		r26.RecipeId            = 26;
		r26.RecipeName          = "Гилли (на плечи)";
		r26.Result              = "GhillieBushrag_Tan";
		r26.PreviewClassname    = "";
		r26.CraftIcon           = "";
		r26.Description         = "Маскировка для корпуса и плеч.";
		r26.CraftTimeSec        = 20;
		r26.ResultCount         = 1;
		r26.ResultQuantityFrac  = 0.0;
		r26.ResultDamageFrac    = 0.15;
		
		DRL_Craft_Component r26c1 = new DRL_Craft_Component();
		r26c1.Classname         = "BurlapSack";
		r26c1.UseInheritance    = 1;
		r26c1.MinHealthFrac     = 3;
		r26c1.Amount            = 2;
		r26c1.QuantityFrac      = 0.0;
		r26c1.Delete            = true;
		r26c1.ApplyDamageFrac   = 0.0;
		r26c1.RecraftDropChance = 0.7;
		r26.CraftComponents.Insert(r26c1);
		
		DRL_Craft_Component r26c2 = new DRL_Craft_Component();
		r26c2.Classname         = "Netting";
		r26c2.UseInheritance    = 1;
		r26c2.MinHealthFrac     = 3;
		r26c2.Amount            = 1;
		r26c2.QuantityFrac      = 0.0;
		r26c2.Delete            = true;
		r26c2.ApplyDamageFrac   = 0.0;
		r26c2.RecraftDropChance = 0.8;
		r26.CraftComponents.Insert(r26c2);
		
		CraftItems.Insert(r26);
		
		
		DRL_Craft_Recipe r27 = new DRL_Craft_Recipe();
		r27.RecipeId            = 27;
		r27.RecipeName          = "Костёр";
		r27.Result              = "Fireplace";
		r27.PreviewClassname    = "";
		r27.CraftIcon           = "";
		r27.Description         = "Очаг для приготовления и обогрева.";
		r27.CraftTimeSec        = 5;
		r27.ResultCount         = 1;
		r27.ResultQuantityFrac  = 0.0;
		r27.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r27c1 = new DRL_Craft_Component();
		r27c1.Classname         = "Rag";
		r27c1.UseInheritance    = 1;
		r27c1.MinHealthFrac     = 3;
		r27c1.Amount            = 1;
		r27c1.QuantityFrac      = 0.0;
		r27c1.Delete            = true;
		r27c1.ApplyDamageFrac   = 0.0;
		r27c1.RecraftDropChance = 0.6;
		r27.CraftComponents.Insert(r27c1);
		
		DRL_Craft_Component r27c2 = new DRL_Craft_Component();
		r27c2.Classname         = "WoodenStick";
		r27c2.UseInheritance    = 1;
		r27c2.MinHealthFrac     = 3;
		r27c2.Amount            = 3;
		r27c2.QuantityFrac      = 0.0;
		r27c2.Delete            = true;
		r27c2.ApplyDamageFrac   = 0.0;
		r27c2.RecraftDropChance = 0.8;
		r27.CraftComponents.Insert(r27c2);
		
		CraftItems.Insert(r27);
		
		
		DRL_Craft_Recipe r28 = new DRL_Craft_Recipe();
		r28.RecipeId            = 28;
		r28.RecipeName          = "Набор укрытия";
		r28.Result              = "ShelterKit";
		r28.PreviewClassname    = "";
		r28.CraftIcon           = "";
		r28.Description         = "Мини-набор для установки укрытия.";
		r28.CraftTimeSec        = 30;
		r28.ResultCount         = 1;
		r28.ResultQuantityFrac  = 0.0;
		r28.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r28c1 = new DRL_Craft_Component();
		r28c1.Classname         = "WoodenStick";
		r28c1.UseInheritance    = 1;
		r28c1.MinHealthFrac     = 3;
		r28c1.Amount            = 4;
		r28c1.QuantityFrac      = 0.0;
		r28c1.Delete            = true;
		r28c1.ApplyDamageFrac   = 0.0;
		r28c1.RecraftDropChance = 0.8;
		r28.CraftComponents.Insert(r28c1);
		
		DRL_Craft_Component r28c2 = new DRL_Craft_Component();
		r28c2.Classname         = "Rope";
		r28c2.UseInheritance    = 1;
		r28c2.MinHealthFrac     = 3;
		r28c2.Amount            = 1;
		r28c2.QuantityFrac      = 0.0;
		r28c2.Delete            = true;
		r28c2.ApplyDamageFrac   = 0.0;
		r28c2.RecraftDropChance = 0.7;
		r28.CraftComponents.Insert(r28c2);
		
		DRL_Craft_Component r28c3 = new DRL_Craft_Component();
		r28c3.Classname         = "Rag";
		r28c3.UseInheritance    = 1;
		r28c3.MinHealthFrac     = 3;
		r28c3.Amount            = 2;
		r28c3.QuantityFrac      = 0.0;
		r28c3.Delete            = true;
		r28c3.ApplyDamageFrac   = 0.0;
		r28c3.RecraftDropChance = 0.6;
		r28.CraftComponents.Insert(r28c3);
		
		CraftItems.Insert(r28);
		
		
		DRL_Craft_Recipe r29 = new DRL_Craft_Recipe();
		r29.RecipeId            = 29;
		r29.RecipeName          = "Растяжка";
		r29.Result              = "TripwireTrap";
		r29.PreviewClassname    = "";
		r29.CraftIcon           = "";
		r29.Description         = "Простая растяжка на тропе.";
		r29.CraftTimeSec        = 12;
		r29.ResultCount         = 1;
		r29.ResultQuantityFrac  = 0.0;
		r29.ResultDamageFrac    = 0.0;
		
		DRL_Craft_Component r29c1 = new DRL_Craft_Component();
		r29c1.Classname         = "MetalWire";
		r29c1.UseInheritance    = 1;
		r29c1.MinHealthFrac     = 3;
		r29c1.Amount            = 1;
		r29c1.QuantityFrac      = 0.0;
		r29c1.Delete            = true;
		r29c1.ApplyDamageFrac   = 0.0;
		r29c1.RecraftDropChance = 0.8;
		r29.CraftComponents.Insert(r29c1);
		
		DRL_Craft_Component r29c2 = new DRL_Craft_Component();
		r29c2.Classname         = "WoodenStick";
		r29c2.UseInheritance    = 1;
		r29c2.MinHealthFrac     = 3;
		r29c2.Amount            = 2;
		r29c2.QuantityFrac      = 0.0;
		r29c2.Delete            = true;
		r29c2.ApplyDamageFrac   = 0.0;
		r29c2.RecraftDropChance = 0.8;
		r29.CraftComponents.Insert(r29c2);
		
		CraftItems.Insert(r29);
		
		
		DRL_Craft_Recipe r30 = new DRL_Craft_Recipe();
		r30.RecipeId            = 30;
		r30.RecipeName          = "Дальномер";
		r30.Result              = "Rangefinder";
		r30.PreviewClassname    = "";
		r30.CraftIcon           = "";
		r30.Description         = "Оптический прибор для определения дистанции.";
		r30.CraftTimeSec        = 28;
		r30.ResultCount         = 1;
		r30.ResultQuantityFrac  = 0.0;
		r30.ResultDamageFrac    = 0.2;
		
		DRL_Craft_Component r30c1 = new DRL_Craft_Component();
		r30c1.Classname         = "Battery9V";
		r30c1.UseInheritance    = 1;
		r30c1.MinHealthFrac     = 3;
		r30c1.Amount            = 1;
		r30c1.QuantityFrac      = 1.0;
		r30c1.Delete            = true;
		r30c1.ApplyDamageFrac   = 0.0;
		r30c1.RecraftDropChance = 0.9;
		r30.CraftComponents.Insert(r30c1);
		
		DRL_Craft_Component r30c2 = new DRL_Craft_Component();
		r30c2.Classname         = "MetalWire";
		r30c2.UseInheritance    = 1;
		r30c2.MinHealthFrac     = 3;
		r30c2.Amount            = 1;
		r30c2.QuantityFrac      = 0.0;
		r30c2.Delete            = true;
		r30c2.ApplyDamageFrac   = 0.0;
		r30c2.RecraftDropChance = 0.7;
		r30.CraftComponents.Insert(r30c2);
		
		DRL_Craft_Component r30c3 = new DRL_Craft_Component();
		r30c3.Classname         = "PersonalRadio";
		r30c3.UseInheritance    = 1;
		r30c3.MinHealthFrac     = 3;
		r30c3.Amount            = 1;
		r30c3.QuantityFrac      = 0.0;
		r30c3.Delete            = true;
		r30c3.ApplyDamageFrac   = 0.0;
		r30c3.RecraftDropChance = 0.6;
		r30.CraftComponents.Insert(r30c3);
		
		CraftItems.Insert(r30);
		
	}
	
	void BuildDefaultCategories()
	{
		Categories = new array<ref DRL_Craft_Category>();
		
		DRL_Craft_Category catAll = new DRL_Craft_Category();
		catAll.Name    = "Все рецепты";
		catAll.Icon    = "DRL_CraftMenu/UI/data/abstract-050.edds";
		catAll.ShowAll = 1;
		catAll.UseList = 0;
		catAll.ShowOnlyAvailableCraft = 0;
		catAll.ShowOnlyAvailableStudy = 0;
		catAll.CategoryFonImagePath    = "";
		catAll.CategoryCraftFonImagePath    = "";
		catAll.CategoryCraftSoundSet    = "";
		catAll.List    = new array<int>();
		Categories.Insert(catAll);
		
		DRL_Craft_Category catFood = new DRL_Craft_Category();
		catFood.Name    = "Еда и вода";
		catFood.Icon    = "DRL_CraftMenu/UI/data/barbecue.edds";
		catFood.ShowAll = 0;
		catFood.UseList = 1;
		catFood.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catFood.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_food.edds";
		catFood.CategoryCraftSoundSet    = "DRLCraft2_SoundSet";
		catFood.List    = new array<int>();
		catFood.List.Insert(10);
		catFood.List.Insert(11);
		Categories.Insert(catFood);
		
		DRL_Craft_Category catClothes = new DRL_Craft_Category();
		catClothes.Name    = "Обмундирование";
		catClothes.Icon    = "DRL_CraftMenu/UI/data/ironcloth1.edds";
		catClothes.ShowAll = 0;
		catClothes.UseList = 1;
		catClothes.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catClothes.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_cloth.edds";
		catClothes.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catClothes.List    = new array<int>();
		catClothes.List.Insert(7);
		catClothes.List.Insert(8);
		Categories.Insert(catClothes);
		
		DRL_Craft_Category catArmor = new DRL_Craft_Category();
		catArmor.Name    = "Броня";
		catArmor.Icon    = "DRL_CraftMenu/UI/data/kevlar-vest.edds";
		catArmor.ShowAll = 0;
		catArmor.UseList = 1;
		catArmor.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catArmor.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_armor.edds";
		catArmor.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catArmor.List    = new array<int>();
		catArmor.List.Insert(2);
		catArmor.List.Insert(3);
		catArmor.List.Insert(5);
		catArmor.List.Insert(6);
		Categories.Insert(catArmor);
		
		DRL_Craft_Category catWeapons = new DRL_Craft_Category();
		catWeapons.Name    = "Оружие";
		catWeapons.Icon    = "DRL_CraftMenu/UI/data/ak47u.edds";
		catWeapons.ShowAll = 0;
		catWeapons.UseList = 1;
		catWeapons.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catWeapons.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_wepon.edds";
		catWeapons.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catWeapons.List    = new array<int>();
		catWeapons.List.Insert(9);
		catWeapons.List.Insert(29);
		Categories.Insert(catWeapons);
		
		DRL_Craft_Category catElectro = new DRL_Craft_Category();
		catElectro.Name    = "Електроника";
		catElectro.Icon    = "DRL_CraftMenu/UI/data/battery-pack-alt.edds";
		catElectro.ShowAll = 0;
		catElectro.UseList = 1;
		catElectro.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catElectro.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_electro.edds";
		catElectro.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catElectro.List    = new array<int>();
		catElectro.List.Insert(15);
		catElectro.List.Insert(30);
		Categories.Insert(catElectro);
		
		DRL_Craft_Category catMed = new DRL_Craft_Category();
		catMed.Name    = "Медицина";
		catMed.Icon    = "DRL_CraftMenu/UI/data/acid.edds";
		catMed.ShowAll = 0;
		catMed.UseList = 1;
		catMed.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catMed.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_medical.edds";
		catMed.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catMed.List    = new array<int>();
		catMed.List.Insert(23);
		catMed.List.Insert(24);
		Categories.Insert(catMed);
		
		DRL_Craft_Category catTools = new DRL_Craft_Category();
		catTools.Name    = "Инструменты";
		catTools.Icon    = "DRL_CraftMenu/UI/data/build3.edds";
		catTools.ShowAll = 0;
		catTools.UseList = 1;
		catTools.CategoryFonImagePath    = "DRL_CraftMenu/UI/data/work.edds";
		catTools.CategoryCraftFonImagePath    = "DRL_CraftMenu/UI/data/recipemanu_part.edds";
		catTools.CategoryCraftSoundSet    = "DRLCraft_SoundSet";
		catTools.List    = new array<int>();
		catTools.List.Insert(12);
		catTools.List.Insert(13);
		catTools.List.Insert(14);
		catTools.List.Insert(18);
		catTools.List.Insert(21);
		catTools.List.Insert(22);
		catTools.List.Insert(25);
		Categories.Insert(catTools);
		
		DRL_Craft_Category catExtra = new DRL_Craft_Category();
		catExtra.Name    = "Дополнительно";
		catExtra.Icon    = "DRL_CraftMenu/UI/data/cardboard-box-closed.edds";
		catExtra.ShowAll = 0;
		catExtra.UseList = 1;
		catExtra.CategoryFonImagePath    = "";
		catExtra.CategoryCraftFonImagePath    = "";
		catExtra.CategoryCraftSoundSet    = "";
		catExtra.List    = new array<int>();
		catExtra.List.Insert(1);
		catExtra.List.Insert(4);
		catExtra.List.Insert(16);
		catExtra.List.Insert(19);
		catExtra.List.Insert(20);
		catExtra.List.Insert(26);
		catExtra.List.Insert(27);
		catExtra.List.Insert(28);
		Categories.Insert(catExtra);
	}
	
	void Load()
	{
		if (!GetGame().IsServer())
		{
			return;
		}

		DRL_CraftLogger.InitOnce();
		DRL_CraftLogger.Info("Config Load started...");

		if (!FileExist(DIR_CRAFT))
		{
			MakeDirectory(DIR_CRAFT);
		}

		string err;

		if (!Settings)
		{
			Settings = new DRL_Craft_Settings();
		}

		if (FileExist(FILE_SETTINGS))
		{
			if (!JsonFileLoader<DRL_Craft_Settings>.LoadFile(FILE_SETTINGS, Settings, err))
			{
				DRL_CraftLogger.Error("[DRL_Craft_Config::Load] Cannot load data from \"" + FILE_SETTINGS + "\": " + err);
			}
			else
			{
				DRL_CraftLogger.Info("Loaded CraftSettings.json OK");
			}
		}
		else
		{
			SaveSettings();
			DRL_CraftLogger.Warn("CraftSettings.json not found. Default file created: " + FILE_SETTINGS);
		}

		if (!Settings.AdminsID)
		{
			Settings.AdminsID = new array<string>;
		}

		if (Settings.MaxQueueSlots < 1)
		{
			Settings.MaxQueueSlots = 1;
		}

		if (FileExist(FILE_CATS))
		{
			if (!JsonFileLoader<array<ref DRL_Craft_Category>>.LoadFile(FILE_CATS, Categories, err))
			{
				DRL_CraftLogger.Error("[DRL_Craft_Config::Load] Cannot load data from \"" + FILE_CATS + "\": " + err);
			}
			else
			{
				DRL_CraftLogger.Info("Loaded CraftCategories.json OK (Categories: " + Categories.Count().ToString() + ")");
			}
		}
		else
		{
			Categories.Clear();
			BuildDefaultCategories();
			SaveCategories();
			DRL_CraftLogger.Warn("CraftCategories.json not found. Default file created: " + FILE_CATS);
		}

		if (FileExist(FILE))
		{
			if (!JsonFileLoader<array<ref DRL_Craft_Recipe>>.LoadFile(FILE, CraftItems, err))
			{
				DRL_CraftLogger.Error("[DRL_Craft_Config::Load] Cannot load data from \"" + FILE + "\": " + err);
			}
			else
			{
				DRL_CraftLogger.Info("Loaded CraftConfig.json OK (Recipes: " + CraftItems.Count().ToString() + ")");
			}
		}
		else
		{
			CraftItems.Clear();
			SetDefault();
			SaveCraftRecipe();
			DRL_CraftLogger.Warn("CraftConfig.json not found. Default file created: " + FILE);
		}

		ref DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg)
		{
			lcfg = new DRL_Learning_Config();
			g_LearningConfig = lcfg;
		}

		if (FileExist(FILE_LEARNING))
		{
			if (!JsonFileLoader<DRL_Learning_Config>.LoadFile(FILE_LEARNING, lcfg, err))
			{
				DRL_CraftLogger.Error("[DRL_Craft_Config::Load] Cannot load data from \"" + FILE_LEARNING + "\": " + err);
			}
			else
			{
				DRL_CraftLogger.Info("Loaded CraftLearningConfig.json OK (LearningRecipes: " + lcfg.LearningRecipes.Count().ToString() + ")");
			}
		}
		else
		{
			BuildDefaultLearningConfig(lcfg);
			SaveLearningConfig(lcfg);
			DRL_CraftLogger.Warn("CraftLearningConfig.json not found. Default file created: " + FILE_LEARNING);
		}

		DRL_CraftLogger.Info("Config Load finished successfully.");
	}

	void BuildDefaultLearningConfig(DRL_Learning_Config lcfg)
	{
		if (!lcfg)
		{
			return;
		}

		lcfg.EnableLearning = 1;
		lcfg.ResetLearnedOnDeath = 0;
		lcfg.LearningSoundSet = "DRLCraft_SoundSet";
		lcfg.LearningRecipes.Clear();

		DRL_Learning_Recipe lr1 = new DRL_Learning_Recipe();
		lr1.RecipeIds.Insert(1);
		lr1.RecipeIds.Insert(2);
		lr1.StudyMain = 1;

		DRL_Study_Variation var1a = new DRL_Study_Variation();
		var1a.DisplayName = "Изучить через чертежи";
		var1a.RequiredItem = "DRL_Craft_Blueprint";
		var1a.Amount = 3;
		var1a.Delete = 1;
		lr1.StudyVariations.Insert(var1a);

		DRL_Study_Variation var1b = new DRL_Study_Variation();
		var1b.DisplayName = "Изучить через предмет";
		var1b.RequiredItem = "TShirt_Grey";
		var1b.Amount = 1;
		var1b.Delete = 1;
		lr1.StudyVariations.Insert(var1b);

		lcfg.LearningRecipes.Insert(lr1);

		DRL_Learning_Recipe lr2 = new DRL_Learning_Recipe();
		lr2.RecipeIds.Insert(3);
		lr2.RequiredRecipeIds.Insert(1);

		DRL_Study_Variation var2a = new DRL_Study_Variation();
		var2a.DisplayName = "Изучить через чертежи";
		var2a.RequiredItem = "DRL_Craft_Blueprint";
		var2a.Amount = 5;
		var2a.Delete = 1;
		lr2.StudyVariations.Insert(var2a);

		lcfg.LearningRecipes.Insert(lr2);
	}

	void SaveLearningConfig(DRL_Learning_Config lcfg = null)
	{
		if (!lcfg)
		{
			lcfg = GetLearning_Config();
		}
		if (!lcfg)
		{
			return;
		}

		if (!FileExist(DIR_CRAFT))
		{
			MakeDirectory(DIR_CRAFT);
		}

		string err;
		if (!JsonFileLoader<DRL_Learning_Config>.SaveFile(FILE_LEARNING, lcfg, err))
		{
			ErrorEx(err);
		}
	}

	ref DRL_PlayerLearningData GetPlayerLearningData(string playerId)
	{
		return GetPlayerLearningManager().GetPlayerData(playerId);
	}

	void SavePlayerLearningData(DRL_PlayerLearningData data)
	{
		GetPlayerLearningManager().SavePlayerData(data);
	}

	void SaveCraftRecipe()
	{
		if (!FileExist(DIR_CRAFT))
		{
			MakeDirectory(DIR_CRAFT);
		}

		string err;
		if (!JsonFileLoader<array<ref DRL_Craft_Recipe>>.SaveFile(FILE, CraftItems, err))
		{
			ErrorEx(err);
		}
	}

	void SaveCategories()
	{
		if (!FileExist(DIR_CRAFT))
		{
			MakeDirectory(DIR_CRAFT);
		}

		string err;
		if (!JsonFileLoader<array<ref DRL_Craft_Category>>.SaveFile(FILE_CATS, Categories, err))
		{
			ErrorEx(err);
		}
	}

	void SaveSettings()
	{
		if (!FileExist(DIR_CRAFT))
		{
			MakeDirectory(DIR_CRAFT);
		}

		string err;
		if (!JsonFileLoader<DRL_Craft_Settings>.SaveFile(FILE_SETTINGS, Settings, err))
		{
			ErrorEx(err);
		}
	}
}
