class CfgPatches
{
	class CfgPatches_Terminator_bag
	{
		units[] = {"EFT_bag_Terminator_ColorBase"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters_Vests", "DZ_Characters_Backpacks", "DZ_Scripts", "DZ_Characters", "DZ_Gear_Drinks", "DZ_Gear_Medical", "DZ_Gear_Containers", "DZ_Weapons_Magazines"};
	};
};
class CfgVehicles
{
	class Clothing;
	class MountainBag_ColorBase;
	class EFT_bag_Terminator_ColorBase : MountainBag_ColorBase {
		scope = 0;
		displayName = "F4 Terminator";
		descriptionShort = "Eberlestock F4 Terminator Load Backpack ";
		model = "EFT\Bags\Terminator\Terminator_g.p3d";
		inventorySlot[] = {"Back"};
		attachments[] = {};
		itemInfo[] = {"Clothing", "Back"};
		rotationFlags = 16;
		weight = 3630;
		itemSize[] = {10, 13};
		itemsCargoSize[] = {10, 13};
		varWetMax = 0.79;
		heatIsolation = 0.5;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		soundAttType = "Outdoor";
		randomQuantity = 3;
		hiddenSelections[] = {"zbytek"};
		
		class ClothingTypes {
			male = "EFT\Bags\Terminator\Terminator_mf.p3d";
			female = "EFT\Bags\Terminator\Terminator_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1, {"EFT\Bags\Terminator\data\bag.rvmat"}}, {0.7, {"EFT\Bags\Terminator\data\bag.rvmat"}}, {0.5, {"EFT\Bags\Terminator\data\bag_damage.rvmat"}}, {0.3, {"EFT\Bags\Terminator\data\bag_damage.rvmat"}}, {0, {"EFT\Bags\Terminator\data\bag_destruct.rvmat"}}};
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
	
	class EFT_bag_Terminator : EFT_bag_Terminator_ColorBase {
		scope = 2;
		visibilityModifier = 0.95;
		hiddenSelectionsTextures[] = {"EFT\Bags\Terminator\data\bag.paa"};
	};
};
