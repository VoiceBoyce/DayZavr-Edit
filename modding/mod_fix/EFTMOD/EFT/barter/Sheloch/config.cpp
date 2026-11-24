class CfgPatches {
	class EFT_Sheloch {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Sheloch : Inventory_Base {
		scope = 2;
displayName = "#EFT_Sheloch_displayName";
descriptionShort = "#EFT_Sheloch_descriptionShort";
		model = "EFT\barter\Sheloch\EFT_Sheloch.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Sheloch\data\barter.rvmat"}}, {0.7, {"EFT\barter\Sheloch\data\barter.rvmat"}}, {0.5, {"EFT\barter\Sheloch\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Sheloch\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Sheloch\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
