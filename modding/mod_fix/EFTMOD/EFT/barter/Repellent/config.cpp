class CfgPatches {
	class EFT_Repellent {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Repellent : Inventory_Base {
		scope = 2;
displayName = "#EFT_Repellent_displayName";
descriptionShort = "#EFT_Repellent_descriptionShort";
		model = "EFT\barter\Repellent\EFT_Repellent.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Repellent\data\barter.rvmat"}}, {0.7, {"EFT\barter\Repellent\data\barter.rvmat"}}, {0.5, {"EFT\barter\Repellent\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Repellent\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Repellent\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
