class CfgPatches {
	class EFT_Cooler {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Cooler : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cooler_displayName";
		descriptionShort = "#EFT_Cooler_descriptionShort";
		model = "EFT\barter\Cooler\EFT_Cooler.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cooler\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cooler\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cooler\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cooler\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cooler\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
