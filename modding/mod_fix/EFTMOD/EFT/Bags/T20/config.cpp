class CfgPatches
{
	class CfgPatches_T20_bag
	{
		units[] = {"EFT_bag_T20_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters_Vests", "DZ_Characters_Backpacks", "DZ_Scripts", "DZ_Characters", "DZ_Gear_Drinks", "DZ_Gear_Medical", "DZ_Gear_Containers", "DZ_Weapons_Magazines"};
	};
};
class CfgVehicles
{
	class Clothing;
	class MountainBag_ColorBase;
	class EFT_bag_T20_ColorBase : MountainBag_ColorBase {
		scope = 0;
		displayName = "#EFT_bag_T20_displayName";
		descriptionShort = "#EFT_bag_T20_descriptionShort";
		model = "EFT\Bags\T20\T20_g.p3d";
		inventorySlot[] = {"Back"};
		attachments[] = {};
		itemInfo[] = {"Clothing", "Back"};
		rotationFlags = 16;
		weight = 1500;
		itemSize[] = {5, 9};
		itemsCargoSize[] = {4, 17};
		varWetMax = 0.25;
		heatIsolation = 0.5;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		soundAttType = "Outdoor";
		randomQuantity = 3;
		hiddenSelections[] = {"zbytek"};
		
		class ClothingTypes {
			male = "EFT\Bags\T20\T20_mf.p3d";
			female = "EFT\Bags\T20\T20_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1, {"EFT\Bags\T20\data\bag.rvmat"}}, {0.7, {"EFT\Bags\T20\data\bag.rvmat"}}, {0.5, {"EFT\Bags\T20\data\bag_damage.rvmat"}}, {0.3, {"EFT\Bags\T20\data\bag_damage.rvmat"}}, {0, {"EFT\Bags\T20\data\bag_destruct.rvmat"}}};
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
	
	class EFT_bag_T20 : EFT_bag_T20_ColorBase {
		scope = 2;
		visibilityModifier = 0.95;
		hiddenSelectionsTextures[] = {"EFT\Bags\T20\data\bag.paa"};
	};
};
