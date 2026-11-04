class CfgPatches {
	class EFT_crikent {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_crikent : Inventory_Base {
		scope = 2;
		displayName = "#EFT_crikent_displayName";
		descriptionShort = "#EFT_crikent_descriptionShort";
		model = "EFT\barter\crikent\EFT_crikent.p3d";
		isMeleeWeapon = 1;
		weight = 5;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\crikent\data\barter.rvmat"}}, {0.7, {"EFT\barter\crikent\data\barter.rvmat"}}, {0.5, {"EFT\barter\crikent\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\crikent\data\barter_damage.rvmat"}}, {0, {"EFT\barter\crikent\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};