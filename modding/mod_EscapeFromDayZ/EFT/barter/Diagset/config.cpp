class CfgPatches {
	class EFT_Diagset {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Diagset : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Diagset_displayName";
		descriptionShort = "#EFT_Diagset_descriptionShort";
		model = "EFT\barter\Diagset\EFT_Diagset.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Diagset\data\barter.rvmat"}}, {0.7, {"EFT\barter\Diagset\data\barter.rvmat"}}, {0.5, {"EFT\barter\Diagset\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Diagset\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Diagset\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
