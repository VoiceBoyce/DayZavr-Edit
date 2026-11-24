class CfgPatches {
	class EFT_Household {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Household : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Household_displayName";
		descriptionShort = "#EFT_Household_descriptionShort";
		model = "EFT\barter\Household\EFT_Household.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Household\data\barter.rvmat"}}, {0.7, {"EFT\barter\Household\data\barter.rvmat"}}, {0.5, {"EFT\barter\Household\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Household\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Household\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
