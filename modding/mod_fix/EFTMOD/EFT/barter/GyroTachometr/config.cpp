class CfgPatches {
	class EFT_GyroTachometr {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_GyroTachometr : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GyroTachometr_displayName";
		descriptionShort = "#EFT_GyroTachometr_descriptionShort";
		model = "EFT\barter\GyroTachometr\EFT_GyroTachometr.p3d";
		isMeleeWeapon = 1;
		weight = 900;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GyroTachometr\data\barter.rvmat"}}, {0.7, {"EFT\barter\GyroTachometr\data\barter.rvmat"}}, {0.5, {"EFT\barter\GyroTachometr\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GyroTachometr\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GyroTachometr\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
