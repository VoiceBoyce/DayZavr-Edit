class CfgPatches {
	class EFT_MedTools {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_MedTools : Inventory_Base {
		scope = 2;
displayName = "#EFT_MedTools_displayName";
descriptionShort = "#EFT_MedTools_descriptionShort";
		model = "EFT\barter\MedTools\EFT_MedTools.p3d";
		isMeleeWeapon = 1;
		weight = 250;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\MedTools\data\barter.rvmat"}}, {0.7, {"EFT\barter\MedTools\data\barter.rvmat"}}, {0.5, {"EFT\barter\MedTools\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\MedTools\data\barter_damage.rvmat"}}, {0, {"EFT\barter\MedTools\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
