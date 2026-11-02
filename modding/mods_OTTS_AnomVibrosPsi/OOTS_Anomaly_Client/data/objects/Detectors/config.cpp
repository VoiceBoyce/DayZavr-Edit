class CfgPatches
{
	class OOTS_Anomaly_Detectors
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Anomaly_Detector_Otklik: Inventory_Base
	{
		scope = 2;
		displayName = "Детектор Отклик";
		descriptionShort = "";
		model = "\OOTS_Anomaly_Client\data\objects\Detectors\Oklik\DetectorOklik.p3d";
		rotationFlags = 0;
		weight = 250;
		itemSize[] = {1,2};
		attachments[] = {"BatteryD"};
		hiddenSelections[] = {"zbytek","Lampa","glass"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Defaultl_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Lampa_Defaultl_Empty_ca.paa","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Glass_Defaultl_ca.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Adef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_lampa.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass.rvmat"};
		class AnimationSources
		{
			class top
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Adef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_lampa.rvmat"}},{0.7,{"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Adef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_lampa.rvmat"}},{0.4,{"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Lampa_damage.rvmat"}},{0.2,{"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Lampa_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Glass_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oklik\date\Oklik_Lampa_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 3;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
				class compass_open
				{
					soundSet = "compass_open_SoundSet";
					id = 204;
				};
				class compass_close
				{
					soundSet = "compass_close_SoundSet";
					id = 205;
				};
			};
		};
	};
	class Anomaly_Detector_Oko: Inventory_Base
	{
		scope = 2;
		displayName = "Детектор Око";
		descriptionShort = "";
		model = "\OOTS_Anomaly_Client\data\objects\Detectors\Oko\DetectorOko.p3d";
		rotationFlags = 0;
		weight = 250;
		itemSize[] = {1,2};
		attachments[] = {"BatteryD"};
		hiddenSelections[] = {"Lampa","Ecran"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\OkoOff_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\OkoOff_co.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_lampa.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat"};
		class AnimationSources
		{
			class top
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_lampa.rvmat"}},{0.7,{"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_lampa.rvmat"}},{0.4,{"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Lampa_damage.rvmat"}},{0.2,{"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Lampa_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Glass_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Oko\date\Oko_Lampa_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 3;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
				class compass_open
				{
					soundSet = "compass_open_SoundSet";
					id = 204;
				};
				class compass_close
				{
					soundSet = "compass_close_SoundSet";
					id = 205;
				};
			};
		};
	};
	class Anomaly_Detector_Medved: Inventory_Base
	{
		scope = 2;
		displayName = "Детектор Медведь";
		descriptionShort = "";
		model = "\OOTS_Anomaly_Client\data\objects\Detectors\Medved\DetectorMedved.p3d";
		rotationFlags = 0;
		isMeleeWeapon = 0;
		weight = 250;
		itemSize[] = {1,2};
		attachments[] = {"BatteryD"};
		hiddenSelections[] = {"zbytek","glass","ekrangloss","ekran","display"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_default_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Glass_DefaultMaterial_ca.paa","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_defaultEckanGloss_ca.paa","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\EkranOff.paa","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\EkranOff.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_EkranOFF.rvmat",""};
		class AnimationSources
		{
			class top
			{
				source = "user";
				animPeriod = 1;
				initPhase = 0;
			};
			class ekran
			{
				source = "user";
				animPeriod = 0.65;
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
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat"}},{0.7,{"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_ADef.rvmat"}},{0.4,{"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_damage.rvmat"}},{0.2,{"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_Glass_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Medved\date\Medved_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 1.2;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
				class compass_open
				{
					soundSet = "compass_open_SoundSet";
					id = 204;
				};
				class compass_close
				{
					soundSet = "compass_close_SoundSet";
					id = 205;
				};
			};
		};
	};
	class Anomaly_Detector_Svarog: Inventory_Base
	{
		scope = 2;
		displayName = "Детектор Сварог";
		descriptionShort = "";
		model = "\OOTS_Anomaly_Client\data\objects\Detectors\Svarog\DetectorSvarog.p3d";
		rotationFlags = 0;
		weight = 250;
		itemSize[] = {1,2};
		attachments[] = {"BatteryD"};
		hiddenSelections[] = {"zbytek","glass","emiss","ekran"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_default_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Glass_DefaultMaterial_ca.paa","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_default_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_default_co.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogEmiss.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogMat.rvmat"};
		dashboardMatOn = "OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_int2.rvmat";
		dashboardMatOff = "OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_int2_nolight.rvmat";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogEmiss.rvmat"}},{0.7,{"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogEmiss.rvmat"}},{0.4,{"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_damage.rvmat"}},{0.2,{"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\SvarogGlass_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Svarog\date\Svarog_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 1;
		};
		class AnimationSources
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
				initPhase = 0;
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
				initPhase = 0;
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
				initPhase = 0;
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
				initPhase = 0;
			};
			class art4_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
			};
		};
	};
	class Anomaly_Detector_Veles: Inventory_Base
	{
		scope = 2;
		displayName = "Детектор Велес";
		descriptionShort = "";
		model = "\OOTS_Anomaly_Client\data\objects\Detectors\Veles\DetectorVeles.p3d";
		rotationFlags = 0;
		weight = 250;
		itemSize[] = {1,2};
		attachments[] = {"BatteryD"};
		hiddenSelections[] = {"zbytek","glass","emiss","ekran"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_default_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Glass_DefaultMaterial_ca.paa","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_default_co.paa","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_default_co.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesEmiss.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesMat.rvmat"};
		dashboardMatOn = "OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_int2.rvmat";
		dashboardMatOff = "OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_int2_nolight.rvmat";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 150;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesEmiss.rvmat"}},{0.7,{"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesMat.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesEmiss.rvmat"}},{0.4,{"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_damage.rvmat"}},{0.2,{"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass_damage.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\VelesGlass_destruct.rvmat","OOTS_Anomaly_Client\data\objects\Detectors\Veles\date\Veles_destruct.rvmat"}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 1;
		};
		class AnimationSources
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
				initPhase = 0;
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
				initPhase = 0;
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
				initPhase = 0;
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
				initPhase = 0;
			};
			class art4_hide
			{
				source = "user";
				animPeriod = 0.1;
				initPhase = 1;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
			};
		};
	};
};
