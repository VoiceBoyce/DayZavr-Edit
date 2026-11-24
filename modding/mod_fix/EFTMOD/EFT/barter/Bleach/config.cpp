class CfgPatches {
	class EFT_Bleach {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Bleach : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Bleach_displayName";
		descriptionShort = "#EFT_Bleach_descriptionShort";
		model = "EFT\barter\Bleach\EFT_Bleach.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Bleach\data\barter.rvmat"}}, {0.7, {"EFT\barter\Bleach\data\barter.rvmat"}}, {0.5, {"EFT\barter\Bleach\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Bleach\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Bleach\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
