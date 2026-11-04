class CfgPatches {
	class EFT_GyroScope {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_GyroScope : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GyroScope_displayName";
		descriptionShort = "#EFT_GyroScope_descriptionShort";
		model = "EFT\barter\GyroScope\EFT_GyroScope.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GyroScope\data\barter.rvmat"}}, {0.7, {"EFT\barter\GyroScope\data\barter.rvmat"}}, {0.5, {"EFT\barter\GyroScope\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GyroScope\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GyroScope\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
