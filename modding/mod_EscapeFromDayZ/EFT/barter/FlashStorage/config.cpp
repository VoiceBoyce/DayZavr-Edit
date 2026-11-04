class CfgPatches {
	class EFT_FlashStorage {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_FlashStorage : Inventory_Base {
		scope = 2;
		displayName = "#EFT_FlashStorage_displayName";
		descriptionShort = "#EFT_FlashStorage_descriptionShort";
		model = "EFT\barter\FlashStorage\EFT_FlashStorage.p3d";
		isMeleeWeapon = 1;
		weight = 800;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\FlashStorage\data\barter.rvmat"}}, {0.7, {"EFT\barter\FlashStorage\data\barter.rvmat"}}, {0.5, {"EFT\barter\FlashStorage\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\FlashStorage\data\barter_damage.rvmat"}}, {0, {"EFT\barter\FlashStorage\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
