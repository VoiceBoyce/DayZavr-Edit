class CfgPatches {
	class EFT_CPU {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_CPU : Inventory_Base {
		scope = 2;
		displayName = "#EFT_CPU_displayName";
		descriptionShort = "#EFT_CPU_descriptionShort";
		model = "EFT\barter\CPU\EFT_CPU.p3d";
		isMeleeWeapon = 1;
		weight = 8;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\CPU\data\barter.rvmat"}}, {0.7, {"EFT\barter\CPU\data\barter.rvmat"}}, {0.5, {"EFT\barter\CPU\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\CPU\data\barter_damage.rvmat"}}, {0, {"EFT\barter\CPU\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
