class CfgPatches {
	class EFT_Hunter_match {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Hunter_match : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Hunter_match_displayName";
		descriptionShort = "#EFT_Hunter_match_descriptionShort";
		model = "EFT\barter\Hunter_match\EFT_Hunter_match.p3d";
		isMeleeWeapon = 1;
		weight = 5;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Hunter_match\data\barter.rvmat"}}, {0.7, {"EFT\barter\Hunter_match\data\barter.rvmat"}}, {0.5, {"EFT\barter\Hunter_match\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Hunter_match\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Hunter_match\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
