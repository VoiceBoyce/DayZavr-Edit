class CfgPatches {
	class EFT_Accum {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Accum : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Accum_displayName";
		descriptionShort = "#EFT_Accum_descriptionShort";
		model = "EFT\barter\accum\EFT_Accum.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\accum\data\barter.rvmat"}}, {0.7, {"EFT\barter\accum\data\barter.rvmat"}}, {0.5, {"EFT\barter\accum\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\accum\data\barter_damage.rvmat"}}, {0, {"EFT\barter\accum\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
