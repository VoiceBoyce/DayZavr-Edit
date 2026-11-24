class CfgPatches {
	class EFT_horse {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_horse : Inventory_Base {
		scope = 2;
		displayName = "#EFT_horse_displayName";
		descriptionShort = "#EFT_horse_descriptionShort";
		model = "EFT\barter\horse\EFT_horse.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\horse\data\barter.rvmat"}}, {0.7, {"EFT\barter\horse\data\barter.rvmat"}}, {0.5, {"EFT\barter\horse\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\horse\data\barter_damage.rvmat"}}, {0, {"EFT\barter\horse\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
