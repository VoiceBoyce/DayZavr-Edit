class CfgPatches
{
	class Vp_Set15
	{
		units[] = {"Vp_Set15_Mask_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_Set15_Mask_ColorBase: Clothing
	{
		scope = 0;
		RankLevel = 3;
		displayName = "$STR_VBC_Moriarty_Mask";
		descriptionShort = "$STR_VBC_Moriarty_Mask_Desc";
		model = "set_Elegant\set\Mask\model\Mask_g.p3d";
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
		hiddenSelections[] = {"ebalo","mask","head_injury"};
		headSelectionsToHide[] = {"Clipping_Balaclava"};
		class ClothingTypes
		{
			male = "set_Elegant\set\Mask\model\Mask_mf.p3d";
			female = "set_Elegant\set\Mask\model\Mask_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Elegant\set\Mask\data\ebalo.rvmat","set_Elegant\set\Mask\data\Mask.rvmat"}},{0.7,{"set_Elegant\set\Mask\data\ebalo.rvmat","set_Elegant\set\Mask\data\Mask.rvmat"}},{0.5,{"set_Elegant\set\Mask\data\ebalo_damage.rvmat","set_Elegant\set\Mask\data\Mask_damage.rvmat"}},{0.3,{"set_Elegant\set\Mask\data\ebalo_damage.rvmat","set_Elegant\set\Mask\data\Mask_damage.rvmat"}},{0.0,{"set_Elegant\set\Mask\data\ebalo_destruct.rvmat","set_Elegant\set\Mask\data\Mask_destruct.rvmat"}}};
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
	class Vp_Set15_Mask_Black: Vp_Set15_Mask_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_Elegant\set\Mask\data\ebalo_co.paa","set_Elegant\set\Mask\data\Mask_co.paa"};
	};
};
