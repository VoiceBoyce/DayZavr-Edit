class CfgPatches {
	class EFT_Bolts {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Bolts : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Bolts_displayName";
		descriptionShort = "#EFT_Bolts_descriptionShort";
		model = "EFT\barter\Bolts\EFT_Bolts.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Bolts\data\barter.rvmat"}}, {0.7, {"EFT\barter\Bolts\data\barter.rvmat"}}, {0.5, {"EFT\barter\Bolts\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Bolts\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Bolts\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
