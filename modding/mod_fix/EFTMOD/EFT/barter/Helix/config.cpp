class CfgPatches {
	class EFT_Helix {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Helix : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Helix_displayName";
		descriptionShort = "#EFT_Helix_descriptionShort";
		model = "EFT\barter\Helix\EFT_Helix.p3d";
		isMeleeWeapon = 1;
		weight = 420;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Helix\data\barter.rvmat"}}, {0.7, {"EFT\barter\Helix\data\barter.rvmat"}}, {0.5, {"EFT\barter\Helix\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Helix\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Helix\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
