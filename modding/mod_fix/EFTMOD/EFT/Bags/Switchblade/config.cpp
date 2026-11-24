class CfgPatches
{
	class CfgPatches_Switchblade_bag
	{
		units[] = {"EFT_bag_Switchblade_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters_Vests", "DZ_Characters_Backpacks", "DZ_Scripts", "DZ_Characters", "DZ_Gear_Drinks", "DZ_Gear_Medical", "DZ_Gear_Containers", "DZ_Weapons_Magazines"};
	};
};
class CfgVehicles
{
	class Clothing;
	class MountainBag_ColorBase;
	class EFT_bag_Switchblade_ColorBase : MountainBag_ColorBase {
		scope = 0;
		displayName = "#EFT_bag_Switchblade_displayName";
		descriptionShort = "#EFT_bag_Switchblade_descriptionShort";
		model = "EFT\Bags\Switchblade\Switchblade_g.p3d";
		inventorySlot[] = {"Back"};
		attachments[] = {};
		itemInfo[] = {"Clothing", "Back"};
		rotationFlags = 16;
		weight = 1310;
		itemSize[] = {8, 10};
		itemsCargoSize[] = {10, 16};
		varWetMax = 0.19;
		heatIsolation = 0.5;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		soundAttType = "Outdoor";
		randomQuantity = 3;
		hiddenSelections[] = {"zbytek"};
		
		class ClothingTypes {
			male = "EFT\Bags\Switchblade\Switchblade_mf.p3d";
			female = "EFT\Bags\Switchblade\Switchblade_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1, {"EFT\Bags\Switchblade\data\bag.rvmat"}}, {0.7, {"EFT\Bags\Switchblade\data\bag.rvmat"}}, {0.5, {"EFT\Bags\Switchblade\data\bag_damage.rvmat"}}, {0.3, {"EFT\Bags\Switchblade\data\bag_damage.rvmat"}}, {0, {"EFT\Bags\Switchblade\data\bag_destruct.rvmat"}}};
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
	
	class EFT_bag_Switchblade : EFT_bag_Switchblade_ColorBase {
		scope = 2;
		visibilityModifier = 0.95;
		hiddenSelectionsTextures[] = {"EFT\Bags\Switchblade\data\bag.paa"};
	};
};
