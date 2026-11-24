class CfgPatches
{
	class CfgPatches_Redfox_bag
	{
		units[] = {"EFT_bag_Redfox_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters_Vests", "DZ_Characters_Backpacks", "DZ_Scripts", "DZ_Characters", "DZ_Gear_Drinks", "DZ_Gear_Medical", "DZ_Gear_Containers", "DZ_Weapons_Magazines"};
	};
};
class CfgVehicles
{
	class Clothing;
	class MountainBag_ColorBase;
	class EFT_bag_Redfox_ColorBase : MountainBag_ColorBase {
		scope = 0;
		displayName = "#EFT_bag_Redfox_displayName";
		descriptionShort = "#EFT_bag_Redfox_descriptionShort";
		model = "EFT\Bags\Redfox\Redfox_g.p3d";
		inventorySlot[] = {"Back"};
		attachments[] = {};
		itemInfo[] = {"Clothing", "Back"};
		rotationFlags = 16;
		weight = 520;
		itemSize[] = {5, 4};
		itemsCargoSize[] = {9, 8};
		varWetMax = 0.39;
		heatIsolation = 0.5;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		soundAttType = "Outdoor";
		randomQuantity = 3;
		hiddenSelections[] = {"zbytek"};
		
		class ClothingTypes {
			male = "EFT\Bags\Redfox\Redfox_mf.p3d";
			female = "EFT\Bags\Redfox\Redfox_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1, {"EFT\Bags\Redfox\data\bag.rvmat"}}, {0.7, {"EFT\Bags\Redfox\data\bag.rvmat"}}, {0.5, {"EFT\Bags\Redfox\data\bag_damage.rvmat"}}, {0.3, {"EFT\Bags\Redfox\data\bag_damage.rvmat"}}, {0, {"EFT\Bags\Redfox\data\bag_destruct.rvmat"}}};
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
	
	class EFT_bag_Redfox : EFT_bag_Redfox_ColorBase {
		scope = 2;
		visibilityModifier = 0.95;
		hiddenSelectionsTextures[] = {"EFT\Bags\Redfox\data\bag.paa"};
	};
};
