class CfgPatches {
	class EFT_gasanalyze {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_gasanalyze : Inventory_Base {
		scope = 2;
		displayName = "#EFT_gasanalyze_displayName";
		descriptionShort = "#EFT_gasanalyze_descriptionShort";
		model = "EFT\barter\gasanalyze\EFT_gasanalyze.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\gasanalyze\data\barter.rvmat"}}, {0.7, {"EFT\barter\gasanalyze\data\barter.rvmat"}}, {0.5, {"EFT\barter\gasanalyze\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\gasanalyze\data\barter_damage.rvmat"}}, {0, {"EFT\barter\gasanalyze\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
