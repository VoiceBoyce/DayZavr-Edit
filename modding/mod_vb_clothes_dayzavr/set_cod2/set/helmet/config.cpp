////////////////////////////////////////////////////////////////////
//DeRap: C:\Users\VOICEB~1\AppData\Local\Temp\claude\d1718fbc3272ee86\26d3bac2-d505-4219-af5b-e7c11eb17191\scratchpad\conv\set_cod2\set\helmet\orig.bin
//Produced from mikero's Dos Tools Dll version 10.21
//https://mikero.bytex.digital/Downloads
//'now' is Wed Nov 04 10:11:04 2026 : 'file' last modified on Wed Nov 04 10:11:04 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class Vp_Set6
	{
		units[] = {"Vp_Set6_Mask_Black","Vp_Set6_Mask_Silver","Vp_Set6_Mask_Silver_Away"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_Set6_Mask_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_King_Mask";
		descriptionShort = "$STR_VBC_King_Mask_Desc";
		model = "set_cod2\set\helmet\model\mask_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Mask"};
		itemInfo[] = {"Clothing","Mask"};
		ragQuantity = 6;
		quickBarBonus = 0;
		weight = 150;
		itemSize[] = {2,2};
		varWetMax = 0.249;
		heatIsolation = 0.6;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"mask1","mask2","shapka"};
		headSelectionsToHide[] = {"Clipping_Balaclava"};
		class ClothingTypes
		{
			male = "set_cod2\set\helmet\model\mask_m.p3d";
			female = "set_cod2\set\helmet\model\mask_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_cod2\set\helmet\data\mask1.rvmat","set_cod2\set\helmet\data\mask2.rvmat","set_cod2\set\helmet\data\shapka.rvmat"}},{0.7,{"set_cod2\set\helmet\data\mask1.rvmat","set_cod2\set\helmet\data\mask1.rvmat","set_cod2\set\helmet\data\shapka.rvmat"}},{0.5,{"set_cod2\set\helmet\data\mask1_damage.rvmat","set_cod2\set\helmet\data\mask2_damage.rvmat","set_cod2\set\helmet\data\shapka_damage.rvmat"}},{0.3,{"set_cod2\set\helmet\data\mask1_damage.rvmat","set_cod2\set\helmet\data\mask2_damage.rvmat","set_cod2\set\helmet\data\shapka_damage.rvmat"}},{0.0,{"set_cod2\set\helmet\data\mask1_destruct.rvmat","set_cod2\set\helmet\data\mask2_destruct.rvmat","set_cod2\set\helmet\data\shapka_destruct.rvmat"}}};
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
	class Vp_Set6_Mask_Black: Vp_Set6_Mask_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\helmet\data\mask1_co.paa","set_cod2\set\helmet\data\mask2_co.paa","set_cod2\set\helmet\data\shapka_co.paa"};
	};
	class Vp_Set6_Mask_Silver: Vp_Set6_Mask_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\helmet\data\mask1_co.paa","set_cod2\set\helmet\data\mask2_silver.paa","set_cod2\set\helmet\data\shapka_silver.paa"};
	};
	class Vp_Set6_Mask_Silver_Away: Vp_Set6_Mask_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_cod2\set\helmet\data\mask1_co.paa","set_cod2\set\helmet\data\mask2_silver.paa","set_cod2\set\helmet\data\shapka_silver.paa"};
	};
};
