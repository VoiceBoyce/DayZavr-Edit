class CfgPatches {
	class RF_ESCAPE_FROM_DAYZ_CL {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters", "DZ_Characters_Vests", "DZ_Gear_Containers", "DZ_Data"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
    class Roadflare;

	class RF_ESCAPE_FROM_DAYZ_Roadflare: Roadflare
	{
		scope = 2;
		displayName = "#RF_Escapefromdayz_Emergency_Exit_00";
		descriptionShort = "#RF_Escapefromdayz_Emergency_Exit_01";
		model = "RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\rf_escape_flare.p3d";
		overrideDrawArea = "8.0";
		lockType = 4;
		weight = 50;
		weightPerQuantityUnit = 0.0;
		absorbency = 0.5;
		itemSize[] = {1,3};
		stackedUnit = "w";
		quantityBar = 1;
		varQuantityInit = 20;
		varQuantityMin = 0.0;
		varQuantityMax = 20;
		varQuantityDestroyOnMin = 0;
		inventorySlot[] = {"tripWireAttachment"};
		rotationFlags = 17;
		hiddenSelections[] = {"Burning","Burned_out","Pristine","All"};
		hiddenSelectionsTextures[] = {
			"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\rf_escape_flare_co.paa",
			"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\rf_escape_flare_co.paa",
			"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\rf_escape_flare_co.paa",
			"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\rf_escape_flare_co.paa"
			};
		hiddenSelectionsMaterials[] = {"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {
						{1.0,{"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare.rvmat"}},
						{0.7,{"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare.rvmat"}},
						{0.5,{"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare_damage.rvmat"}},
						{0.3,{"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare_damage.rvmat"}},
						{0.0,{"RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\data\road_flare_destruct.rvmat"}}
					};
				};
			};
		};
		class EnergyManager
		{
			energyAtSpawn = 120;
			energyUsagePerSecond = 1;
			updateInterval = 10;
			convertEnergyToQuantity = 1;
		};
		class NoiseRoadFlare
		{
			strength = 30.0;
			type = "shot";
		};
		class AnimationSources
		{
			class Sticks_Flare_Folded
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
			class Sticks_Flare_Unfolded
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
			class Burned_out
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
			class Burning
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
			class Flare_cap
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
			class Pristine
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Flare_strike
				{
					soundSet = "Flare_strike_SoundSet";
					id = 201;
				};
				class Flare_open
				{
					soundSet = "Flare_open_SoundSet";
					id = 202;
				};
			};
		};
	};

	class RF_ESCAPE_FROM_DAYZ_Roadflare_Static: RF_ESCAPE_FROM_DAYZ_Roadflare {};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxyrf_escape_flare: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"tripWireAttachment"};
		model = "RF_ESCAPE_FROM_DAYZ_CL\models\roadflare\rf_escape_flare.p3d";
		class AnimationSources
		{
			class Sticks_Flare_Folded_1
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 0;
			};
			class Sticks_Flare_Unfolded_1
			{
				source = "user";
				animPeriod = 0.01;
				initPhase = 1;
			};
		};
	};
};
