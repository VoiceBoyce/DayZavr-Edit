////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\vest2\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:04 2026 : 'file' last modified on Wed Nov 04 10:11:04 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class Vp_Set6
	{
		units[] = {"Vp_Set6_vest2_Silver","Vp_Set6_vest2_Silver_Away"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Characters_vests","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_Set6_vest2_ColorBase: Clothing
	{
		scope = 0;
		displayName = "Бронежилет";
		descriptionShort = "";
		model = "set_cod2\set\vest2\model\vest2_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"vest"};
		itemInfo[] = {"Clothing","vest"};
		quickBarBonus = 2;
		weight = 6000;
		itemSize[] = {4,4};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo","plita"};
		class ClothingTypes
		{
			male = "set_cod2\set\vest2\model\vest2_m.p3d";
			female = "set_cod2\set\vest2\model\vest2_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.7,{"set_cod2\set\vest2\data\vest2.rvmat","set_cod2\set\vest2\data\plita.rvmat"}},{0.5,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.3,{"set_cod2\set\vest2\data\vest2_damage.rvmat","set_cod2\set\vest2\data\plita_damage.rvmat"}},{0.0,{"set_cod2\set\vest2\data\vest2_destruct.rvmat","set_cod2\set\vest2\data\plita_destruct.rvmat"}}};
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
					soundSet = "Smershvest2_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "Smershvest2_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_Set6_vest2_Silver: Vp_Set6_vest2_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\vest2\data\vest2_co.paa","set_cod2\set\vest2\data\plita_co.paa"};
	};
	class Vp_Set6_vest2_Silver_Away: Vp_Set6_vest2_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\vest2\data\vest2_silver_away.paa","set_cod2\set\vest2\data\plita_silver_away.paa"};
	};
};
