class CfgPatches {
	class EFT_Screwdriver_krest {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Screwdriver_krest : Inventory_Base {
		scope = 2;
displayName = "#EFT_Screwdriver_krest_displayName";
descriptionShort = "#EFT_Screwdriver_krest_descriptionShort";
		model = "EFT\barter\Screwdriver_krest\EFT_Screwdriver_krest.p3d";
		isMeleeWeapon = 1;
		weight = 150;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Screwdriver_krest\data\barter.rvmat"}}, {0.7, {"EFT\barter\Screwdriver_krest\data\barter.rvmat"}}, {0.5, {"EFT\barter\Screwdriver_krest\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Screwdriver_krest\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Screwdriver_krest\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
