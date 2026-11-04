class CfgPatches {
	class EFT_matches {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_matches : Inventory_Base {
		scope = 2;
displayName = "#EFT_matches_displayName";
descriptionShort = "#EFT_matches_descriptionShort";
		model = "EFT\barter\matches\EFT_matches.p3d";
		isMeleeWeapon = 1;
		weight = 5;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\matches\data\barter.rvmat"}}, {0.7, {"EFT\barter\matches\data\barter.rvmat"}}, {0.5, {"EFT\barter\matches\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\matches\data\barter_damage.rvmat"}}, {0, {"EFT\barter\matches\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
