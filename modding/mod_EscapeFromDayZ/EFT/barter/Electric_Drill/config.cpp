class CfgPatches {
	class EFT_Electric_Drill {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Electric_Drill : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Electric_Drill_displayName";
		descriptionShort = "#EFT_Electric_Drill_descriptionShort";
		model = "EFT\barter\Electric_Drill\EFT_Electric_Drill.p3d";
		isMeleeWeapon = 1;
		weight = 1200;
		absorbency = 0.1;
		itemSize[] = {2,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Electric_Drill\data\barter.rvmat"}}, {0.7, {"EFT\barter\Electric_Drill\data\barter.rvmat"}}, {0.5, {"EFT\barter\Electric_Drill\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Electric_Drill\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Electric_Drill\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
