////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\belt\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:03 2026 : 'file' last modified on Wed Nov 04 10:11:03 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class VP_SET6
	{
		units[] = {"Belt_SET6_Black","Belt_SET6_Silver","Belt_SET6_Silver_Away"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Belt_SET6_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_King_Belt";
		descriptionShort = "$STR_VBC_King_Belt_Desc";
		model = "set_cod2\set\belt\model\belt_g.p3d";
		attachments[] = {};
		vehicleClass = "Clothing";
		simulation = "clothing";
		inventorySlot[] = {"Hips"};
		itemInfo[] = {"Clothing","Hips"};
		itemSize[] = {5,5};
		itemsCargoSize[] = {4,10};
		quickBarBonus = 2;
		varWetMax = 0.049;
		heatIsolation = 0.5;
		weight = 550;
		repairableWithKits[] = {5,3};
		repairCosts[] = {30.0,25.0};
		lootCategory = "Crafted";
		hiddenSelections[] = {"camo","fast","kabura","pounsh1","pounsh2"};
		hiddenSelectionsTextures[] = {"set_cod2\set\belt\data\belt_co.paa","set_cod2\set\belt\data\fast_co.paa","set_cod2\set\belt\data\kabura_co.paa","set_cod2\set\belt\data\left_poush_co.paa","set_cod2\set\belt\data\right_poush_co.paa"};
		class ClothingTypes
		{
			male = "set_cod2\set\belt\model\belt_m.p3d";
			female = "set_cod2\set\belt\model\belt_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_cod2\set\belt\data\belt.rvmat","set_cod2\set\belt\data\fast.rvmat","set_cod2\set\belt\data\kabura.rvmat","set_cod2\set\belt\data\left_poush.rvmat","set_cod2\set\belt\data\right_poush.rvmat"}},{0.7,{"set_cod2\set\belt\data\belt.rvmat","set_cod2\set\belt\data\fast.rvmat","set_cod2\set\belt\data\kabura.rvmat","set_cod2\set\belt\data\left_poush.rvmat","set_cod2\set\belt\data\right_poush.rvmat"}},{0.5,{"set_cod2\set\belt\data\belt_damage.rvmat","set_cod2\set\belt\data\fast_damage.rvmat","set_cod2\set\belt\data\kabura_damage.rvmat","set_cod2\set\belt\data\left_poush_damage.rvmat","set_cod2\set\belt\data\right_poush_damage.rvmat"}},{0.3,{"set_cod2\set\belt\data\belt_damage.rvmat","set_cod2\set\belt\data\fast_damage.rvmat","set_cod2\set\belt\data\kabura_damage.rvmat","set_cod2\set\belt\data\left_poush_damage.rvmat","set_cod2\set\belt\data\right_poush_damage.rvmat"}},{0,{"set_cod2\set\belt\data\belt_destruct.rvmat","set_cod2\set\belt\data\fast_destruct.rvmat","set_cod2\set\belt\data\kabura_destruct.rvmat","set_cod2\set\belt\data\left_poush_destruct.rvmat","set_cod2\set\belt\data\right_poush_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0.2;
					};
					class Shock
					{
						damage = 0.2;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "WorkingGloves_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "WorkingGloves_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Belt_SET6_Black: Belt_SET6_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\belt\data\belt_co.paa","set_cod2\set\belt\data\fast_co.paa","set_cod2\set\belt\data\kabura_co.paa","set_cod2\set\belt\data\left_poush_co.paa","set_cod2\set\belt\data\right_poush_co.paa"};
	};
	class Belt_SET6_Silver: Belt_SET6_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\belt\data\belt_co.paa","set_cod2\set\belt\data\fast_co.paa","set_cod2\set\belt\data\kabura_silver.paa","set_cod2\set\belt\data\left_poush_silver.paa","set_cod2\set\belt\data\right_poush_silver.paa"};
	};
	class Belt_SET6_Silver_Away: Belt_SET6_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\belt\data\belt_co.paa","set_cod2\set\belt\data\fast_co.paa","set_cod2\set\belt\data\kabura_silver.paa","set_cod2\set\belt\data\left_poush_silver.paa","set_cod2\set\belt\data\right_poush_silver.paa"};
	};
};
