class CfgPatches {
	class EFT_SecureUSB {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_SecureUSB1 : Inventory_Base {
		scope = 2;
displayName = "#EFT_SecureUSB1_displayName";
descriptionShort = "#EFT_SecureUSB1_descriptionShort";
		model = "EFT\barter\SecureUSB\EFT_SecureUSB1.p3d";
		isMeleeWeapon = 1;
		weight = 20;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\SecureUSB\data\barter.rvmat"}}, {0.7, {"EFT\barter\SecureUSB\data\barter.rvmat"}}, {0.5, {"EFT\barter\SecureUSB\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\SecureUSB\data\barter_damage.rvmat"}}, {0, {"EFT\barter\SecureUSB\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_SecureUSB2 : Inventory_Base {
		scope = 2;
displayName = "#EFT_SecureUSB2_displayName";
descriptionShort = "#EFT_SecureUSB2_descriptionShort";
		model = "EFT\barter\SecureUSB\EFT_SecureUSB2.p3d";
		isMeleeWeapon = 1;
		weight = 20;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\SecureUSB\data\barter.rvmat"}}, {0.7, {"EFT\barter\SecureUSB\data\barter.rvmat"}}, {0.5, {"EFT\barter\SecureUSB\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\SecureUSB\data\barter_damage.rvmat"}}, {0, {"EFT\barter\SecureUSB\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
