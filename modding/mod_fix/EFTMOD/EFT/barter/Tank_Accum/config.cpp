class CfgPatches {
	class EFT_Tank_Accum {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Tank_Accum : Inventory_Base {
		scope = 2;
displayName = "#EFT_Tank_Accum_displayName";
descriptionShort = "#EFT_Tank_Accum_descriptionShort";
		model = "EFT\barter\Tank_Accum\EFT_Tank_Accum.p3d";
		isMeleeWeapon = 1;
		weight = 43200;
		absorbency = 0.1;
		itemSize[] = {4,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Tank_Accum\data\barter.rvmat"}}, {0.7, {"EFT\barter\Tank_Accum\data\barter.rvmat"}}, {0.5, {"EFT\barter\Tank_Accum\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Tank_Accum\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Tank_Accum\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};