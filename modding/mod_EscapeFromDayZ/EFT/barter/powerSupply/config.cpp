class CfgPatches {
	class EFT_powerSupply {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_powerSupply : Inventory_Base {
		scope = 2;
displayName = "#EFT_powerSupply_displayName";
descriptionShort = "#EFT_powerSupply_descriptionShort";
		model = "EFT\barter\powerSupply\EFT_powerSupply.p3d";
		isMeleeWeapon = 1;
		weight = 800;
		absorbency = 0.1;
		itemSize[] = {2,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\powerSupply\data\barter.rvmat"}}, {0.7, {"EFT\barter\powerSupply\data\barter.rvmat"}}, {0.5, {"EFT\barter\powerSupply\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\powerSupply\data\barter_damage.rvmat"}}, {0, {"EFT\barter\powerSupply\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
