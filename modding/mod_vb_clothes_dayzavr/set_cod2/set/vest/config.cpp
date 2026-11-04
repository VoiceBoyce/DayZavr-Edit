////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\vest\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:04 2026 : 'file' last modified on Wed Nov 04 10:11:04 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class Vp_Set6
	{
		units[] = {"Vp_Set6_Vest_black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Characters_Vests","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_Set6_Vest_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_King_Vest";
		descriptionShort = "$STR_VBC_King_Vest_Desc";
		model = "set_cod2\set\vest\model\vest_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		quickBarBonus = 2;
		weight = 6000;
		itemSize[] = {4,4};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo","korona"};
		class ClothingTypes
		{
			male = "set_cod2\set\vest\model\vest_m.p3d";
			female = "set_cod2\set\vest\model\vest_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\vest\data\vest.rvmat","set_cod2\set\vest\data\korona.rvmat"}},{0.7,{"set_cod2\set\vest\data\vest.rvmat","set_cod2\set\vest\data\korona.rvmat"}},{0.5,{"set_cod2\set\vest\data\vest_damage.rvmat","set_cod2\set\vest\data\korona_damage.rvmat"}},{0.3,{"set_cod2\set\vest\data\vest_damage.rvmat","set_cod2\set\vest\data\korona_damage.rvmat"}},{0.0,{"set_cod2\set\vest\data\vest_destruct.rvmat","set_cod2\set\vest\data\korona_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.1;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.1;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.05;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.01;
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
						damage = 0;
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
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_Set6_Vest_black: Vp_Set6_Vest_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\vest\data\vest_co.paa","set_cod2\set\vest\data\korona_co.paa"};
	};
};
