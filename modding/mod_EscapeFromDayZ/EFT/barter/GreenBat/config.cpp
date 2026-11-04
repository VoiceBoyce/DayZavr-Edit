class CfgPatches {
	class EFT_GreenBat {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_GreenBat : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GreenBat_displayName";
		descriptionShort = "#EFT_GreenBat_descriptionShort";
		model = "EFT\barter\GreenBat\EFT_GreenBat.p3d";
		isMeleeWeapon = 1;
		weight = 43;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GreenBat\data\barter.rvmat"}}, {0.7, {"EFT\barter\GreenBat\data\barter.rvmat"}}, {0.5, {"EFT\barter\GreenBat\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GreenBat\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GreenBat\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
