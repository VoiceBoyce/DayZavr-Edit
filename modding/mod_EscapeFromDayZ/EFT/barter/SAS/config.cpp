class CfgPatches {
	class EFT_SAS {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_SAS : Inventory_Base {
		scope = 2;
displayName = "#EFT_SAS_displayName";
descriptionShort = "#EFT_SAS_descriptionShort";
		model = "EFT\barter\SAS\EFT_SAS.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\SAS\data\barter.rvmat"}}, {0.7, {"EFT\barter\SAS\data\barter.rvmat"}}, {0.5, {"EFT\barter\SAS\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\SAS\data\barter_damage.rvmat"}}, {0, {"EFT\barter\SAS\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
