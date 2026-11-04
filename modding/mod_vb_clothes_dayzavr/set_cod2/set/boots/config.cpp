////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\boots\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:03 2026 : 'file' last modified on Wed Nov 04 10:11:03 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class VP_Set6
	{
		units[] = {"VP_Set6_Shoes_Black","VP_Set6_Shoes_Silver","VP_Set6_Shoes_Away"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class CfgVehicles
{
	class Clothing;
	class VP_Set6_Shoes_ColorBase: Clothing
	{
		scope = 0;
		displayName = "Ботинки";
		descriptionShort = "";
		model = "set_cod2\set\boots\model\shoes_g.p3d";
		inventorySlot[] = {"Feet"};
		itemInfo[] = {"Clothing","Feet"};
		itemSize[] = {4,2};
		weight = 500;
		durability = 1;
		varWetMax = 1;
		heatIsolation = 1;
		visibilityModifier = 0.9;
		repairableWithKits[] = {3};
		repairCosts[] = {25};
		soundAttType = "Sneakers";
		soundImpactType = "default";
		hiddenSelections[] = {"camo"};
		class ClothingTypes
		{
			male = "set_cod2\set\boots\model\shoes_m.p3d";
			female = "set_cod2\set\boots\model\shoes_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 90;
					healthLevels[] = {{1,{"set_cod2\set\boots\data\boots.rvmat"}},{0.7,{"set_cod2\set\boots\data\boots.rvmat"}},{0.5,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0.3,{"set_cod2\set\boots\data\boots_damage.rvmat"}},{0,{"set_cod2\set\boots\data\boots_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "AthleticShoes_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "AthleticShoes_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class VP_Set6_Shoes_Black: VP_Set6_Shoes_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\boots\data\boots_co.paa"};
	};
	class VP_Set6_Shoes_Silver: VP_Set6_Shoes_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\boots\data\boots_silver.paa"};
	};
	class VP_Set6_Shoes_Away: VP_Set6_Shoes_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\boots\data\boots_silver.paa"};
	};
};
