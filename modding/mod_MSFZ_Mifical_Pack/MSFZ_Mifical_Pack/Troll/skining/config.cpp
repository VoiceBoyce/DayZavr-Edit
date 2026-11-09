class CfgPatches
{
	class MSFZ_Troll_Hand
	{
		units[] = {"MSFZ_Troll_Hand","MutantTrollSteakMeat"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class BaseFoodStageTransitions
{
	class Raw
	{
		class ToBaked
		{
			transition_to = 2;
			cooking_method = 1;
		};
		class ToBoiled
		{
			transition_to = 3;
			cooking_method = 2;
		};
		class ToBurned
		{
			transition_to = 5;
			cooking_method = 3;
		};
	};
	class Rotten
	{
		class ToBurned
		{
			transition_to = 5;
			cooking_method = 1;
		};
	};
	class Baked
	{
		class ToBurned
		{
			transition_to = 5;
			cooking_method = 1;
		};
	};
	class Boiled
	{
		class ToBaked
		{
			transition_to = 2;
			cooking_method = 1;
		};
	};
	class Dried
	{
		class ToBaked
		{
			transition_to = 5;
			cooking_method = 1;
		};
		class ToBoiled
		{
			transition_to = 5;
			cooking_method = 2;
		};
	};
	class Burned{};
};
class FoodAnimationSources
{
	class CS_Raw
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
	class CS_Rotten
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
	class CS_Baked
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
	class CS_Boiled
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
	class CS_Dried
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
	class CS_Burned
	{
		source = "user";
		animPeriod = 0.01;
		initPhase = 1;
	};
};
class MeatStageTransitions: BaseFoodStageTransitions{};
class CfgVehicles
{
	class Edible_Base;
	class Inventory_Base;
	class WolfSteakMeat;
	class MSFZ_Troll_Hand: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Troll";
		descriptionShort = "$STR_Troll1";
		model = "MSFZ_Mifical_Pack\Troll\skining\MSFZ_Troll_Hand.p3d";
		weight = 1200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class MutantTrollSteakMeat: WolfSteakMeat
	{
		scope = 2;
		displayName = "$STR_TrollSteak";
		descriptionShort = "$STR_TrollSteak1";
		model = "\dz\gear\food\human_meat.p3d";
		rotationFlags = 34;
		weight = 0;
		interactionWeight = 1;
		quantityBar = 1;
		varQuantityInit = 150;
		varQuantityMin = 0;
		varQuantityMax = 150;
		itemSize[] = {1,2};
		inventorySlot[] = {"Ingredient","DirectCookingA","DirectCookingB","DirectCookingC","SmokingA","SmokingB","SmokingC","SmokingD"};
		hiddenSelections[] = {"cs_raw"};
		hiddenSelectionsTextures[] = {"dz\gear\food\data\meat_steak_raw_CO.paa","dz\gear\food\data\meat_steak_baked_CO.paa","dz\gear\food\data\meat_steak_boiled_co.paa","dz\gear\food\data\meat_steak_dried_CO.paa","dz\gear\food\data\meat_steak_burned_CO.paa"};
		hiddenSelectionsMaterials[] = {"dz\gear\food\data\meat_steak_raw.rvmat","dz\gear\food\data\meat_steak_baked.rvmat","dz\gear\food\data\meat_steak_boiled.rvmat","dz\gear\food\data\meat_steak_dried.rvmat","dz\gear\food\data\meat_steak_burnt.rvmat","dz\gear\food\data\meat_steak_rotten.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
		class AnimationSources: FoodAnimationSources{};
		class Food
		{
			class FoodStages
			{
				class Raw
				{
					visual_properties[] = {0,0,0};
					nutrition_properties[] = {5,50,50,1,0,4};
					cooking_properties[] = {0,0};
				};
				class Rotten
				{
					visual_properties[] = {-1,-1,5};
					nutrition_properties[] = {10,25,25,1,0,"4 + 16"};
					cooking_properties[] = {0,0};
				};
				class Baked
				{
					visual_properties[] = {0,1,1};
					nutrition_properties[] = {2,200,50,1,0};
					cooking_properties[] = {70,45};
				};
				class Boiled
				{
					visual_properties[] = {0,2,2};
					nutrition_properties[] = {2,150,150,1,0};
					cooking_properties[] = {70,55};
				};
				class Dried
				{
					visual_properties[] = {0,3,3};
					nutrition_properties[] = {3,150,0,1,0};
					cooking_properties[] = {70,120,80};
				};
				class Burned
				{
					visual_properties[] = {0,4,4};
					nutrition_properties[] = {5,50,0,1,0,16};
					cooking_properties[] = {100,30};
				};
			};
			class FoodStageTransitions: MeatStageTransitions{};
		};
		class NoiseImpact
		{
			strength = 600;
			type = "sound";
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class openTunaCan
				{
					soundSet = "openTunaCan_SoundSet";
					id = 204;
				};
				class Eating_TakeFood
				{
					soundSet = "Eating_TakeFood_Soundset";
					id = 889;
				};
				class Eating_BoxOpen
				{
					soundSet = "Eating_BoxOpen_Soundset";
					id = 893;
				};
				class Eating_BoxShake
				{
					soundSet = "Eating_BoxShake_Soundset";
					id = 894;
				};
				class Eating_BoxEnd
				{
					soundSet = "Eating_BoxEnd_Soundset";
					id = 895;
				};
			};
		};
	};
};
