class CfgPatches {
	class EFT_kusajki {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_kusajki : Inventory_Base {
		scope = 2;
displayName = "#EFT_kusajki_displayName";
descriptionShort = "#EFT_kusajki_descriptionShort";
		model = "EFT\barter\kusajki\EFT_kusajki.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\kusajki\data\barter.rvmat"}}, {0.7, {"EFT\barter\kusajki\data\barter.rvmat"}}, {0.5, {"EFT\barter\kusajki\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\kusajki\data\barter_damage.rvmat"}}, {0, {"EFT\barter\kusajki\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
