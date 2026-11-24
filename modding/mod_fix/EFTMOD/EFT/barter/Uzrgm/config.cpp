class CfgPatches {
	class EFT_Uzrgm {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Uzrgm : Inventory_Base {
		scope = 2;
displayName = "#EFT_Uzrgm_displayName";
descriptionShort = "#EFT_Uzrgm_descriptionShort";
		model = "EFT\barter\Uzrgm\EFT_Uzrgm.p3d";
		isMeleeWeapon = 1;
		weight = 55;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Uzrgm\data\barter.rvmat"}}, {0.7, {"EFT\barter\Uzrgm\data\barter.rvmat"}}, {0.5, {"EFT\barter\Uzrgm\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Uzrgm\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Uzrgm\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
