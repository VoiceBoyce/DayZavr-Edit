class CfgPatches {
	class EFT_Termite {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Termite : Inventory_Base {
		scope = 2;
displayName = "#EFT_Termite_displayName";
descriptionShort = "#EFT_Termite_descriptionShort";
		model = "EFT\barter\Termite\EFT_Termite.p3d";
		isMeleeWeapon = 1;
		weight = 650;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Termite\data\barter.rvmat"}}, {0.7, {"EFT\barter\Termite\data\barter.rvmat"}}, {0.5, {"EFT\barter\Termite\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Termite\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Termite\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
