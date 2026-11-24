class CfgPatches {
	class EFT_battery {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_battery_AA : Inventory_Base {
		scope = 2;
		displayName = "#EFT_battery_AA_displayName";
		descriptionShort = "#EFT_battery_AA_descriptionShort";
		model = "EFT\barter\battery\EFT_battery_AA.p3d";
		isMeleeWeapon = 1;
		weight = 8;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\battery\data\barter.rvmat"}}, {0.7, {"EFT\barter\battery\data\barter.rvmat"}}, {0.5, {"EFT\barter\battery\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\battery\data\barter_damage.rvmat"}}, {0, {"EFT\barter\battery\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_battery_D : Inventory_Base {
		scope = 2;
		displayName = "#EFT_battery_D_displayName";
		descriptionShort = "#EFT_battery_D_descriptionShort";
		model = "EFT\barter\battery\EFT_battery_D.p3d";
		isMeleeWeapon = 1;
		weight = 8;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\battery\data\barter.rvmat"}}, {0.7, {"EFT\barter\battery\data\barter.rvmat"}}, {0.5, {"EFT\barter\battery\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\battery\data\barter_damage.rvmat"}}, {0, {"EFT\barter\battery\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
