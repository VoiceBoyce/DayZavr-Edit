class CfgPatches {
	class EFT_Salt {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Salt : Inventory_Base {
		scope = 2;
displayName = "#EFT_Salt_displayName";
descriptionShort = "#EFT_Salt_descriptionShort";
		model = "EFT\barter\Salt\EFT_Salt.p3d";
		isMeleeWeapon = 1;
		weight = 320;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Salt\data\barter.rvmat"}}, {0.7, {"EFT\barter\Salt\data\barter.rvmat"}}, {0.5, {"EFT\barter\Salt\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Salt\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Salt\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
