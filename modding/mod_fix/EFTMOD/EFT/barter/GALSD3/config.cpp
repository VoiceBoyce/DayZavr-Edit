class CfgPatches {
	class EFT_GALSD3 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_GALSD3 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_GALSD3_displayName";
		descriptionShort = "#EFT_q_GALSD3_descriptionShort";
		model = "EFT\barter\GALSD3\EFT_GALSD3.p3d";
		isMeleeWeapon = 1;
		weight = 2000;
		absorbency = 0.1;
		itemSize[] = {2, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GALSD3\data\barter.rvmat"}}, {0.7, {"EFT\barter\GALSD3\data\barter.rvmat"}}, {0.5, {"EFT\barter\GALSD3\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GALSD3\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GALSD3\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
