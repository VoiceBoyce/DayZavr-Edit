class CfgPatches
{
	class Vp_SET13
	{
		units[] = {"Vp_SET13_Mask_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET13_Mask_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Samurai_Mask";
		descriptionShort = "$STR_VBC_Samurai_Mask_Desc";
		model = "Set_Oni\set\Mask\model\Mask_g.p3d";
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
		hiddenSelections[] = {"mask","ebalo"};
		headSelectionsToHide[] = {"Clipping_Balaclava"};
		class ClothingTypes
		{
			male = "Set_Oni\set\Mask\model\Mask_mf.p3d";
			female = "Set_Oni\set\Mask\model\Mask_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"Set_Oni\set\Mask\data\Mask.rvmat","Set_Oni\set\Mask\data\Ebalo.rvmat"}},{0.7,{"Set_Oni\set\Mask\data\Mask.rvmat","Set_Oni\set\Mask\data\Ebalo.rvmat"}},{0.5,{"Set_Oni\set\Mask\data\Mask_damage.rvmat","Set_Oni\set\Mask\data\Ebalo_damage.rvmat"}},{0.3,{"Set_Oni\set\Mask\data\Mask_damage.rvmat","Set_Oni\set\Mask\data\Ebalo_damage.rvmat"}},{0.0,{"Set_Oni\set\Mask\data\Mask_destruct.rvmat","Set_Oni\set\Mask\data\Ebalo_destruct.rvmat"}}};
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
	class Vp_SET13_Mask_Black: Vp_SET13_Mask_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"Set_Oni\set\Mask\data\Mask_co.paa","Set_Oni\set\Mask\data\Ebalo_co.paa"};
	};
};
