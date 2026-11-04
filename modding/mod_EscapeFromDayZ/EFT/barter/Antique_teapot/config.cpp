class CfgPatches {
	class EFT_Antique_teapot {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Antique_teapot : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Antique_teapot_displayName";
		descriptionShort = "#EFT_Antique_teapot_descriptionShort";
		model = "EFT\barter\Antique_teapot\EFT_Antique_teapot.p3d";
		isMeleeWeapon = 1;
		weight = 1100;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Antique_teapot\data\barter.rvmat"}}, {0.7, {"EFT\barter\Antique_teapot\data\barter.rvmat"}}, {0.5, {"EFT\barter\Antique_teapot\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Antique_teapot\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Antique_teapot\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
