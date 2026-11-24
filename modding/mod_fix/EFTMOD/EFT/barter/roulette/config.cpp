class CfgPatches {
	class EFT_roulette {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_roulette : Inventory_Base {
		scope = 2;
displayName = "#EFT_roulette_displayName";
descriptionShort = "#EFT_roulette_descriptionShort";
		model = "EFT\barter\roulette\EFT_roulette.p3d";
		isMeleeWeapon = 1;
		weight = 150;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\roulette\data\barter.rvmat"}}, {0.7, {"EFT\barter\roulette\data\barter.rvmat"}}, {0.5, {"EFT\barter\roulette\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\roulette\data\barter_damage.rvmat"}}, {0, {"EFT\barter\roulette\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
