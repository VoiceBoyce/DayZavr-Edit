// СГЕНЕРИРОВАНО tools/ab_detectors.py — руками не править
// Детекторы Око, Медведь, Сварог, Велес (ТЗ 2.2). Модели OOTS с заменёнными путями, логика в
// scripts/4_World/VB_AB/VB_AB_Detectors2.c. Поля vbAB<часть>Sel — индекс секции в hiddenSelections,
// vbAB<часть><On|Off>Tex/Mat — что кладёт на неё скрипт во включённом и выключенном состоянии.
	// Око: как Отклик (только расстояние, только впереди), лампа мигает, экран горит
	class VB_AB_Detector_Oko: VB_AB_DetectorBase
	{
		scope = 2;
		displayName = "$STR_VB_AB_Detector_Oko";
		descriptionShort = "$STR_VB_AB_Detector_Oko_Desc";
		model = "VB_AB\m\vb_detector_oko_x.p3d";
		hiddenSelections[] = {"Lampa", "Ecran"};
		hiddenSelectionsTextures[] =
		{
			"VB_AB\t\dkx\okoon_co________________________________________.paa",
			"VB_AB\t\dkx\okooff_co________________________________________.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"VB_AB\t\dkx\oko________________________________________.rvmat",
			"VB_AB\t\dkx\oko________________________________________.rvmat"
		};
		vbABLampSel = 0;
		vbABLampOnMat = "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat";
		vbABLampOffMat = "VB_AB\t\dkx\oko________________________________________.rvmat";
		vbABScreenSel = 1;
		vbABScreenOnTex = "VB_AB\t\dkx\okoon_co________________________________________.paa";
		vbABScreenOnMat = "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat";
		vbABScreenOffTex = "VB_AB\t\dkx\okooff_co________________________________________.paa";
		vbABScreenOffMat = "VB_AB\t\dkx\oko________________________________________.rvmat";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] =
					{
						{1.0, {"VB_AB\t\dkx\oko________________________________________.rvmat", "VB_AB\t\dkx\oko_glass________________________________________.rvmat", "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat"}},
						{0.7, {"VB_AB\t\dkx\oko________________________________________.rvmat", "VB_AB\t\dkx\oko_glass________________________________________.rvmat", "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat"}},
						{0.5, {"VB_AB\t\dkx\oko________________________________________.rvmat", "VB_AB\t\dkx\oko_glass________________________________________.rvmat", "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat"}},
						{0.3, {"VB_AB\t\dkx\oko________________________________________.rvmat", "VB_AB\t\dkx\oko_glass________________________________________.rvmat", "VB_AB\t\dkx\oko_lampalight________________________________________.rvmat"}},
						{0.0, {"DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat"}}
					};
				};
			};
		};
	};

	// Медведь: расстояние (картинка экрана) и направление по секторам 15° (поворот экрана)
	class VB_AB_Detector_Medved: VB_AB_DetectorBase
	{
		scope = 2;
		displayName = "$STR_VB_AB_Detector_Medved";
		descriptionShort = "$STR_VB_AB_Detector_Medved_Desc";
		model = "VB_AB\m\vb_detector_medved_x.p3d";
		hiddenSelections[] = {"zbytek", "glass", "ekrangloss", "ekran", "display"};
		hiddenSelectionsTextures[] =
		{
			"VB_AB\t\dmx\medved_default_co___________________________________________.paa",
			"VB_AB\t\dmx\glass_defaultmaterial_ca___________________________________________.paa",
			"VB_AB\t\dmx\medved_defaulteckangloss_ca___________________________________________.paa",
			"VB_AB\t\dmx\ekranoff___________________________________________.paa",
			"VB_AB\t\dmx\ekranoff___________________________________________.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"VB_AB\t\dmx\medved_adef___________________________________________.rvmat",
			"VB_AB\t\dmx\medved_glass___________________________________________.rvmat",
			"VB_AB\t\dmx\medved_adef___________________________________________.rvmat",
			"VB_AB\t\dmx\medved_ekranoff.rvmat"
		};
		vbABScreenSel = 3;
		vbABScreenOnMat = "VB_AB\t\dmx\medved_ekranon___________________________________________.rvmat";
		vbABScreenOffTex = "VB_AB\t\dmx\ekranoff___________________________________________.paa";
		vbABScreenOffMat = "VB_AB\t\dmx\medved_ekranoff.rvmat";
		vbABDisplaySel = 4;
		vbABScreenRangeTex[] =
		{
			"VB_AB\t\dmx\ekran_empty.paa",
			"VB_AB\t\dmx\ekran_low.paa",
			"VB_AB\t\dmx\ekran_mid.paa",
			"VB_AB\t\dmx\ekran_high.paa"
		};
		class AnimationSources: AnimationSources
		{
			class ekran
			{
				source = "user";
				animPeriod = 0.65;
				initPhase = 0.5;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] =
					{
						{1.0, {"VB_AB\t\dmx\medved_adef___________________________________________.rvmat", "VB_AB\t\dmx\medved_glass___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranon___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranoff.rvmat"}},
						{0.7, {"VB_AB\t\dmx\medved_adef___________________________________________.rvmat", "VB_AB\t\dmx\medved_glass___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranon___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranoff.rvmat"}},
						{0.5, {"VB_AB\t\dmx\medved_adef___________________________________________.rvmat", "VB_AB\t\dmx\medved_glass___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranon___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranoff.rvmat"}},
						{0.3, {"VB_AB\t\dmx\medved_adef___________________________________________.rvmat", "VB_AB\t\dmx\medved_glass___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranon___________________________________________.rvmat", "VB_AB\t\dmx\medved_ekranoff.rvmat"}},
						{0.0, {"DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat"}}
					};
				};
			};
		};
	};

	// Сварог: до 4 находок сразу, расстояние и направление к каждой — метками на экране
	class VB_AB_Detector_Svarog: VB_AB_DetectorBase
	{
		scope = 2;
		displayName = "$STR_VB_AB_Detector_Svarog";
		descriptionShort = "$STR_VB_AB_Detector_Svarog_Desc";
		model = "VB_AB\m\vb_detector_svarog_x.p3d";
		hiddenSelections[] = {"zbytek", "glass", "emiss", "ekran"};
		hiddenSelectionsTextures[] =
		{
			"VB_AB\t\dsx\svarog_default_co___________________________________________.paa",
			"VB_AB\t\dsx\glass_defaultmaterial_ca___________________________________________.paa",
			"VB_AB\t\dsx\svarog_default_co___________________________________________.paa",
			"VB_AB\t\dsx\svarog_default_co___________________________________________.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"VB_AB\t\dsx\svarogmat___________________________________________.rvmat",
			"VB_AB\t\dsx\svarogglass___________________________________________.rvmat",
			"VB_AB\t\dsx\svarogmat___________________________________________.rvmat",
			"VB_AB\t\dsx\svarogmat___________________________________________.rvmat"
		};
		vbABEmissSel = 2;
		vbABEmissOnMat = "VB_AB\t\dsx\svarog_emiss_on.rvmat";
		vbABEmissOffMat = "VB_AB\t\dsx\svarogmat___________________________________________.rvmat";
		vbABScreenSel = 3;
		vbABScreenOnTex = "VB_AB\t\dsx\svadog_ekranon___________________________________________.paa";
		vbABScreenOnMat = "VB_AB\t\dsx\svadog_ekranon___________________________________________.rvmat";
		vbABScreenOffTex = "VB_AB\t\dsx\svarog_default_co___________________________________________.paa";
		vbABScreenOffMat = "VB_AB\t\dsx\svarogmat___________________________________________.rvmat";
		class AnimationSources: AnimationSources
		{
			class art1_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art1_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art1_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art2_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art2_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art2_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art3_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art3_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art3_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art4_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art4_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art4_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] =
					{
						{1.0, {"VB_AB\t\dsx\svarogmat___________________________________________.rvmat", "VB_AB\t\dsx\svarogglass___________________________________________.rvmat", "VB_AB\t\dsx\svadog_ekranon___________________________________________.rvmat", "VB_AB\t\dsx\svadog_light___________________________________________.rvmat", "VB_AB\t\dsx\svarog_emiss_on.rvmat"}},
						{0.7, {"VB_AB\t\dsx\svarogmat___________________________________________.rvmat", "VB_AB\t\dsx\svarogglass___________________________________________.rvmat", "VB_AB\t\dsx\svadog_ekranon___________________________________________.rvmat", "VB_AB\t\dsx\svadog_light___________________________________________.rvmat", "VB_AB\t\dsx\svarog_emiss_on.rvmat"}},
						{0.5, {"VB_AB\t\dsx\svarogmat___________________________________________.rvmat", "VB_AB\t\dsx\svarogglass___________________________________________.rvmat", "VB_AB\t\dsx\svadog_ekranon___________________________________________.rvmat", "VB_AB\t\dsx\svadog_light___________________________________________.rvmat", "VB_AB\t\dsx\svarog_emiss_on.rvmat"}},
						{0.3, {"VB_AB\t\dsx\svarogmat___________________________________________.rvmat", "VB_AB\t\dsx\svarogglass___________________________________________.rvmat", "VB_AB\t\dsx\svadog_ekranon___________________________________________.rvmat", "VB_AB\t\dsx\svadog_light___________________________________________.rvmat", "VB_AB\t\dsx\svarog_emiss_on.rvmat"}},
						{0.0, {"DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat"}}
					};
				};
			};
		};
	};

	// Велес: то же, что Сварог, другая модель (метки оранжевые)
	class VB_AB_Detector_Veles: VB_AB_DetectorBase
	{
		scope = 2;
		displayName = "$STR_VB_AB_Detector_Veles";
		descriptionShort = "$STR_VB_AB_Detector_Veles_Desc";
		model = "VB_AB\m\vb_detector_veles_x.p3d";
		hiddenSelections[] = {"zbytek", "glass", "emiss", "ekran"};
		hiddenSelectionsTextures[] =
		{
			"VB_AB\t\dvx\veles_default_co__________________________________________.paa",
			"VB_AB\t\dvx\glass_defaultmaterial_ca___________________________________________.paa",
			"VB_AB\t\dvx\veles_default_co__________________________________________.paa",
			"VB_AB\t\dvx\veles_default_co__________________________________________.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"VB_AB\t\dvx\velesmat__________________________________________.rvmat",
			"VB_AB\t\dvx\svarogglass___________________________________________.rvmat",
			"VB_AB\t\dvx\velesmat__________________________________________.rvmat",
			"VB_AB\t\dvx\velesmat__________________________________________.rvmat"
		};
		vbABEmissSel = 2;
		vbABEmissOnMat = "VB_AB\t\dvx\veles_emiss_on.rvmat";
		vbABEmissOffMat = "VB_AB\t\dvx\velesmat__________________________________________.rvmat";
		vbABScreenSel = 3;
		vbABScreenOnTex = "VB_AB\t\dvx\veles_ekranon__________________________________________.paa";
		vbABScreenOnMat = "VB_AB\t\dvx\veles_light__________________________________________.rvmat";
		vbABScreenOffTex = "VB_AB\t\dvx\veles_default_co__________________________________________.paa";
		vbABScreenOffMat = "VB_AB\t\dvx\velesmat__________________________________________.rvmat";
		class AnimationSources: AnimationSources
		{
			class art1_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art1_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art1_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art2_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art2_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art2_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art3_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art3_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art3_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
			class art4_translate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class art4_rotate
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0.5;
			};
			class art4_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] =
					{
						{1.0, {"VB_AB\t\dvx\velesmat__________________________________________.rvmat", "VB_AB\t\dvx\svarogglass___________________________________________.rvmat", "VB_AB\t\dvx\veles_light__________________________________________.rvmat", "VB_AB\t\dvx\veles_emiss_on.rvmat"}},
						{0.7, {"VB_AB\t\dvx\velesmat__________________________________________.rvmat", "VB_AB\t\dvx\svarogglass___________________________________________.rvmat", "VB_AB\t\dvx\veles_light__________________________________________.rvmat", "VB_AB\t\dvx\veles_emiss_on.rvmat"}},
						{0.5, {"VB_AB\t\dvx\velesmat__________________________________________.rvmat", "VB_AB\t\dvx\svarogglass___________________________________________.rvmat", "VB_AB\t\dvx\veles_light__________________________________________.rvmat", "VB_AB\t\dvx\veles_emiss_on.rvmat"}},
						{0.3, {"VB_AB\t\dvx\velesmat__________________________________________.rvmat", "VB_AB\t\dvx\svarogglass___________________________________________.rvmat", "VB_AB\t\dvx\veles_light__________________________________________.rvmat", "VB_AB\t\dvx\veles_emiss_on.rvmat"}},
						{0.0, {"DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat", "DZ\data\data\default_destruct.rvmat"}}
					};
				};
			};
		};
	};
