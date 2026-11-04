////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\top\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:04 2026 : 'file' last modified on Wed Nov 04 10:11:04 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class Vp_Set6
	{
		units[] = {"Vp_Set6_top_Black","Vp_Set6_top_Silver","Vp_Set6_top_silver_Away"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_Set6_top_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_King_Top";
		descriptionShort = "$STR_VBC_King_Top_Desc";
		model = "set_cod2\set\top\model\top_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Body"};
		itemInfo[] = {"Clothing","Body"};
		quickBarBonus = 0;
		weight = 1500;
		ragQuantity = 6;
		itemSize[] = {4,3};
		varWetMax = 0.249;
		heatIsolation = 0.7;
		itemsCargoSize[] = {10,6};
		repairableWithKits[] = {3,8};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo"};
		class ClothingTypes
		{
			male = "set_cod2\set\top\model\top_m.p3d";
			female = "set_cod2\set\top\model\top_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\top\data\top.rvmat"}},{0.7,{"set_cod2\set\top\data\top.rvmat"}},{0.5,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.3,{"set_cod2\set\top\data\top_damage.rvmat"}},{0.0,{"set_cod2\set\top\data\top_destruct.rvmat"}}};
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
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
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
					soundSet = "Shirt_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "Shirt_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_Set6_top_Black: Vp_Set6_top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\top\data\top_co.paa"};
	};
	class Vp_Set6_top_Silver: Vp_Set6_top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\top\data\top_silver.paa"};
	};
	class Vp_Set6_top_silver_Away: Vp_Set6_top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\top\data\top_silver_away.paa"};
	};
};
