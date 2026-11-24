class CfgPatches {
	class EFT_GP {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_GP : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GP_displayName";
		descriptionShort = "#EFT_GP_descriptionShort ";
		model = "EFT\barter\GP\EFT_GP.p3d";
		isMeleeWeapon = 1;
		weight = 10;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GP\data\barter.rvmat"}}, {0.7, {"EFT\barter\GP\data\barter.rvmat"}}, {0.5, {"EFT\barter\GP\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GP\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GP\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
