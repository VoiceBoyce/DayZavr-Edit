// RF_CONTAINERS_DayZavr — дополнение к новой версии RF_Containers (1.30).
// Возвращает классы ящиков из старой версии (1.29), которые используются в config.json сервера:
// HLAM / WEAPONS / FOOD / MEDICAL / AMMO и их *_STATIC. Это тот же ящик RF_CONTAINERS_CRATE_03
// с другой текстурой; цветные текстуры взяты из старой версии мода и лежат в data\.
// Ящик 07 (CRATE_07_*) в новой версии убран целиком: модель и текстуры лежат в соседнем
// RF_CONTAINERS_DayZavr_Crate07.pbo с префиксом RF_CONTAINERS_CL\models\MillitaryCrate_07
// (прежние пути, p3d не правится), скриптовый класс RF_CONTAINERS_CRATE_BASE_07 — в scripts\4_World.
// Ставить вместе с @RF_Containers (клиент и сервер).

class CfgPatches
{
	class RF_CONTAINERS_DayZavr
	{
		units[] = {"RF_CONTAINERS_CRATE_HLAM","RF_CONTAINERS_CRATE_WEAPONS","RF_CONTAINERS_CRATE_FOOD","RF_CONTAINERS_CRATE_MEDICAL","RF_CONTAINERS_CRATE_AMMO",
			"RF_CONTAINERS_CRATE_07_SIMPLE","RF_CONTAINERS_CRATE_07_AMMO","RF_CONTAINERS_CRATE_07_FOOD","RF_CONTAINERS_CRATE_07_MEDICALE"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","RF_CONTAINERS_CL","RF_CONTAINERS_CL_S"};
	};
};

class CfgMods
{
	class RF_CONTAINERS_DayZavr
	{
		type = "mod";
		dir = "RF_CONTAINERS_DayZavr";
		name = "RF_CONTAINERS_DayZavr";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"RF_CONTAINERS_DayZavr/scripts/4_World"};
			};
		};
	};
};

class CfgVehicles
{
	class RF_CONTAINERS_CRATE_03;

	class RF_CONTAINERS_CRATE_HLAM: RF_CONTAINERS_CRATE_03
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_03\data\MillitaryCrate_03.paa"};
	};
	class RF_CONTAINERS_CRATE_MEDICAL: RF_CONTAINERS_CRATE_03
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_MEDICAL.paa"};
	};
	class RF_CONTAINERS_CRATE_WEAPONS: RF_CONTAINERS_CRATE_03
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_WEAPONS.paa"};
	};
	class RF_CONTAINERS_CRATE_AMMO: RF_CONTAINERS_CRATE_03
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_AMMO.paa"};
	};
	class RF_CONTAINERS_CRATE_FOOD: RF_CONTAINERS_CRATE_03
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_FOOD.paa"};
	};

	// Статичные варианты (scope = 1): не для спавна по конфигу, а для расстановки в маппинге.
	class RF_CONTAINERS_CRATE_HLAM_STATIC: RF_CONTAINERS_CRATE_03
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_03\data\MillitaryCrate_03.paa"};
	};
	class RF_CONTAINERS_CRATE_MEDICAL_STATIC: RF_CONTAINERS_CRATE_03
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_MEDICAL.paa"};
	};
	class RF_CONTAINERS_CRATE_WEAPONS_STATIC: RF_CONTAINERS_CRATE_03
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_WEAPONS.paa"};
	};
	class RF_CONTAINERS_CRATE_AMMO_STATIC: RF_CONTAINERS_CRATE_03
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_AMMO.paa"};
	};
	class RF_CONTAINERS_CRATE_FOOD_STATIC: RF_CONTAINERS_CRATE_03
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_DayZavr\data\MillitaryCrate_03_FOOD.paa"};
	};

	// Ящик 07 — перенесён из версии 1.29 без изменений (кроме scope у MEDICALE_STATIC: был 2).
	class Container_Base;
	class RF_CONTAINERS_CRATE_BASE_07: Container_Base
	{
		scope = 0;
		displayName = "#RF_CONTAINERS_WoodenBox_01";
		descriptionShort = "#RF_CONTAINERS_WoodenBox_02";
		model = "\RF_CONTAINERS_CL\models\MillitaryCrate_07\MillitaryCrate_07.p3d";
		weight = 10000;
		itemBehaviour = 0;
		itemSize[] = {10,8};
		itemsCargoSize[] = {10,10};
		canBeDigged = 0;
		heavyItem = 1;
		rotationFlags = 2;
		overrideDrawArea = 8.0;
		forceFarBubble = "true";
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		hiddenSelections[] = {"camo"};
		class Cargo
		{
			itemsCargoSize[] = {10,5};
			openable = 1;
			allowOwnedCargoManipulation = 1;
		};
		class AnimationSources
		{
			class lid_1
			{
				source = "user";
				initPhase = 0;
				animPeriod = 0.8;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 10000;
					healthLevels[] = {
						{1,   {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo.rvmat"}},
						{0.7, {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo.rvmat"}},
						{0.5, {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_damage.rvmat"}},
						{0.3, {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_damage.rvmat"}},
						{0.1, {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_destruct.rvmat"}}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class movement
				{
					soundSet = "seachest_movement_SoundSet";
					id = 1;
				};
				class pickUpItem_Light
				{
					soundSet = "pickUpSeaChest_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpSeaChest_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "seachest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class RF_CONTAINERS_CRATE_07_AMMO: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo.rvmat"};
		class Cargo
		{
			itemsCargoSize[] = {5,4};
			openable = 1;
			allowOwnedCargoManipulation = 1;
		};
	};
	class RF_CONTAINERS_CRATE_07_FOOD: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Food_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Food.rvmat"};
		class Cargo
		{
			itemsCargoSize[] = {6,4};
			openable = 1;
			allowOwnedCargoManipulation = 1;
		};
	};
	class RF_CONTAINERS_CRATE_07_SIMPLE: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Simple_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Simple.rvmat"};
	};
	class RF_CONTAINERS_CRATE_07_MEDICALE: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Medicale_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Medicale.rvmat"};
	};
	class RF_CONTAINERS_CRATE_07_AMMO_STATIC: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Ammo.rvmat"};
	};
	class RF_CONTAINERS_CRATE_07_FOOD_STATIC: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Food_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Food.rvmat"};
	};
	class RF_CONTAINERS_CRATE_07_SIMPLE_STATIC: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Simple_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Simple.rvmat"};
	};
	class RF_CONTAINERS_CRATE_07_MEDICALE_STATIC: RF_CONTAINERS_CRATE_BASE_07
	{
		scope = 1;
		hiddenSelectionsTextures[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Medicale_co.paa"};
		hiddenSelectionsMaterials[] = {"RF_CONTAINERS_CL\models\MillitaryCrate_07\data\MillitaryCrate_07_Medicale.rvmat"};
	};
};
