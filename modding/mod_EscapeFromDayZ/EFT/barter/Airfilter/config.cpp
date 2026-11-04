class CfgPatches {
	class EFT_Airfilter {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Airfilter : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Airfilter_displayName";
		descriptionShort = "#EFT_Airfilter_descriptionShort";
		model = "EFT\barter\Airfilter\EFT_Airfilter.p3d";
		isMeleeWeapon = 1;
		weight = 13000;
		absorbency = 0.1;
		itemSize[] = {3, 3};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Airfilter\data\barter.rvmat"}}, {0.7, {"EFT\barter\Airfilter\data\barter.rvmat"}}, {0.5, {"EFT\barter\Airfilter\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Airfilter\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Airfilter\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
