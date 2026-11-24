class CfgPatches {
	class EFT_GPhone {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_GPhone : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GPhone_displayName";
		descriptionShort = "#EFT_GPhone_descriptionShort";
		model = "EFT\barter\GPhone\EFT_GPhone.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GPhone\data\barter.rvmat"}}, {0.7, {"EFT\barter\GPhone\data\barter.rvmat"}}, {0.5, {"EFT\barter\GPhone\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GPhone\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GPhone\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_GPhone_G : Inventory_Base {
		scope = 2;
		displayName = "#EFT_GPhone_G_displayName";
		descriptionShort = "#EFT_GPhone_G_descriptionShort";
		model = "EFT\barter\GPhone\EFT_GPhone_G.p3d";
		isMeleeWeapon = 1;
		weight = 350;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\GPhone\data\barter.rvmat"}}, {0.7, {"EFT\barter\GPhone\data\barter.rvmat"}}, {0.5, {"EFT\barter\GPhone\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\GPhone\data\barter_damage.rvmat"}}, {0, {"EFT\barter\GPhone\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
