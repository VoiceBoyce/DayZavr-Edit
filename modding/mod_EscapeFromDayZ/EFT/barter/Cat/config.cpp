class CfgPatches {
	class EFT_Cat {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Cat : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cat_displayName";
		descriptionShort = "#EFT_Cat_descriptionShort";
		model = "EFT\barter\Cat\EFT_Cat.p3d";
		isMeleeWeapon = 1;
		weight = 3100;
		absorbency = 0.1;
		itemSize[] = {1,3};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cat\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cat\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cat\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cat\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cat\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
