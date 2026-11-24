class CfgPatches {
	class EFT_Klean {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Klean : Inventory_Base {
		scope = 2;
displayName = "#EFT_Klean_displayName";
descriptionShort = "#EFT_Klean_descriptionShort";
		model = "EFT\barter\Klean\EFT_Klean.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Klean\data\barter.rvmat"}}, {0.7, {"EFT\barter\Klean\data\barter.rvmat"}}, {0.5, {"EFT\barter\Klean\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Klean\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Klean\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
