class CfgPatches {
	class EFT_Nyball3 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Nyball3 : Inventory_Base {
		scope = 2;
displayName = "#EFT_Nyball3_displayName";
descriptionShort = "#EFT_Nyball3_descriptionShort";
		model = "EFT\barter\Nyball3\EFT_Nyball3.p3d";
		isMeleeWeapon = 1;
		weight = 50;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Nyball3\data\barter.rvmat"}}, {0.7, {"EFT\barter\Nyball3\data\barter.rvmat"}}, {0.5, {"EFT\barter\Nyball3\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Nyball3\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Nyball3\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
