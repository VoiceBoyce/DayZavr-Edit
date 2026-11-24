class CfgPatches {
	class EFT_Ratchetwrench {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Ratchetwrench : Inventory_Base {
		scope = 2;
displayName = "#EFT_Ratchetwrench_displayName";
descriptionShort = "#EFT_Ratchetwrench_descriptionShort";
		model = "EFT\barter\Ratchetwrench\EFT_Ratchetwrench.p3d";
		isMeleeWeapon = 1;
		weight = 1200;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Ratchetwrench\data\barter.rvmat"}}, {0.7, {"EFT\barter\Ratchetwrench\data\barter.rvmat"}}, {0.5, {"EFT\barter\Ratchetwrench\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Ratchetwrench\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Ratchetwrench\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
