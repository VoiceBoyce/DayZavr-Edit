class CfgPatches
{
	class CfgPatches_LBT1476a_bag
	{
		units[] = {"EFT_bag_LBT1476a_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters_Vests", "DZ_Characters_Backpacks", "DZ_Scripts", "DZ_Characters", "DZ_Gear_Drinks", "DZ_Gear_Medical", "DZ_Gear_Containers", "DZ_Weapons_Magazines"};
	};
};
class CfgVehicles
{
	class Clothing;
	class MountainBag_ColorBase;
	class EFT_bag_LBT1476a_ColorBase : MountainBag_ColorBase {
		scope = 0;
		displayName = "#EFT_bag_LBT1476a_displayName";
		descriptionShort = "#EFT_bag_LBT1476a_descriptionShort";
		model = "EFT\Bags\LBT1476a\LBT1476a_g.p3d";
		inventorySlot[] = {"Back"};
		attachments[] = {};
		itemInfo[] = {"Clothing", "Back"};
		rotationFlags = 16;
		weight = 1130;
		itemSize[] = {6, 9};
		itemsCargoSize[] = {7, 14};
		varWetMax = 0.19;
		heatIsolation = 0.5;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		soundAttType = "Outdoor";
		randomQuantity = 3;
		hiddenSelections[] = {"zbytek"};
		
		class ClothingTypes {
			male = "EFT\Bags\LBT1476a\LBT1476a_mf.p3d";
			female = "EFT\Bags\LBT1476a\LBT1476a_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1, {"EFT\Bags\LBT1476a\data\bag.rvmat"}}, {0.7, {"EFT\Bags\LBT1476a\data\bag.rvmat"}}, {0.5, {"EFT\Bags\LBT1476a\data\bag_damage.rvmat"}}, {0.3, {"EFT\Bags\LBT1476a\data\bag_damage.rvmat"}}, {0, {"EFT\Bags\LBT1476a\data\bag_destruct.rvmat"}}};
				};
			};
		};
		
		class AnimEvents {
			class SoundWeapon {
				class pickUpItem_Light {
					soundSet = "pickUpBackPack_Plastic_Light_SoundSet";
					id = 796;
				};
				
				class pickUpItem {
					soundSet = "pickUpBackPack_Plastic_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "taloonbag_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class EFT_bag_LBT1476a : EFT_bag_LBT1476a_ColorBase {
		scope = 2;
		visibilityModifier = 0.95;
		hiddenSelectionsTextures[] = {"EFT\Bags\LBT1476a\data\bag.paa"};
	};
};
