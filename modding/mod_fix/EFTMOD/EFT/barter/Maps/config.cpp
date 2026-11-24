class CfgPatches {
	class EFT_Maps {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Maps : Inventory_Base {
		scope = 2;
displayName = "#EFT_Maps_displayName";
descriptionShort = "#EFT_Maps_descriptionShort";
		model = "EFT\barter\Maps\EFT_Maps.p3d";
		isMeleeWeapon = 1;
		weight = 270;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Maps\data\barter.rvmat"}}, {0.7, {"EFT\barter\Maps\data\barter.rvmat"}}, {0.5, {"EFT\barter\Maps\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Maps\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Maps\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
