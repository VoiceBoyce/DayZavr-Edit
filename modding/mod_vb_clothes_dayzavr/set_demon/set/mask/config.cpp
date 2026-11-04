class CfgPatches {
	class Vp_SET22 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Scripts"};
	};
};

class cfgVehicles {
	class Clothing_Base;
	class Clothing : Clothing_Base {};
	
	class Vp_Set22_Mask_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_VBC_Demon_Mask";
		descriptionShort = "$STR_VBC_Demon_Mask_Desc";
		model = "\set_demon\set\mask\model\mask_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Mask"};
		itemInfo[] = {"Clothing", "Mask"};
		ragQuantity = 6;
		quickBarBonus = 0;
		weight = 150;
		itemSize[] = {2,2};
		varWetMax = 0.249;
		heatIsolation = 0.6;
		repairableWithKits[] = {3, 8};
		repairCosts[] = {75.0, 50.0};
		hiddenSelections[] = {"mask", "mask2"};
		headSelectionsToHide[] = {"Clipping_Balaclava"};
		
		class ClothingTypes {
			male = "\set_demon\set\mask\model\mask_mf.p3d";
			female = "\set_demon\set\mask\model\mask_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 300;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_demon\set\mask\data\mask.rvmat", "\set_demon\set\mask\data\mask2.rvmat"}},
									 {0.7, {"\set_demon\set\mask\data\mask.rvmat", "\set_demon\set\mask\data\mask2.rvmat"}},
									 {0.5, {"\set_demon\set\mask\data\mask_damage.rvmat", "\set_demon\set\mask\data\mask2_damage.rvmat"}},
									 {0.3, {"\set_demon\set\mask\data\mask_damage.rvmat", "\set_demon\set\mask\data\mask2_damage.rvmat"}},
									 {0.0, {"\set_demon\set\mask\data\mask_destruct.rvmat", "\set_demon\set\mask\data\mask2_destruct.rvmat"}}};
				};
			};

		};
		
		class AnimEvents {
			class SoundWeapon {
				class pickUpItem {
					soundSet = "Shirt_pickup_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "Shirt_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class Vp_Set22_Mask_Red: Vp_Set22_Mask_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_demon\set\mask\data\mask_co.paa", "\set_demon\set\mask\data\mask2_ca.paa"};
	};
};
