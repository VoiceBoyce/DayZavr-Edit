class CfgPatches {
	class EFT_NaCl {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_NaCl : Inventory_Base {
		scope = 2;
displayName = "#EFT_NaCl_displayName";
descriptionShort = "#EFT_NaCl_descriptionShort";
		model = "EFT\barter\NaCl\EFT_NaCl.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\NaCl\data\barter.rvmat"}}, {0.7, {"EFT\barter\NaCl\data\barter.rvmat"}}, {0.5, {"EFT\barter\NaCl\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\NaCl\data\barter_damage.rvmat"}}, {0, {"EFT\barter\NaCl\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
