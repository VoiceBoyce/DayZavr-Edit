class CfgPatches {
	class EFT_PipeCleaner {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_PipeCleaner : Inventory_Base {
		scope = 2;
displayName = "#EFT_PipeCleaner_displayName";
descriptionShort = "#EFT_PipeCleaner_descriptionShort";
		model = "EFT\barter\PipeCleaner\EFT_PipeCleaner.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\PipeCleaner\data\barter.rvmat"}}, {0.7, {"EFT\barter\PipeCleaner\data\barter.rvmat"}}, {0.5, {"EFT\barter\PipeCleaner\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\PipeCleaner\data\barter_damage.rvmat"}}, {0, {"EFT\barter\PipeCleaner\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
