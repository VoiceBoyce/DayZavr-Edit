class CfgPatches {
	class EFT_zibbo_g {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_zibbo_g : Inventory_Base {
		scope = 2;
		displayName = "#EFT_zibbo_g_displayName";
		descriptionShort = "#EFT_zibbo_g_descriptionShort";
		model = "EFT\barter\zibbo_g\EFT_zibbo_g.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\zibbo_g\data\barter.rvmat"}}, {0.7, {"EFT\barter\zibbo_g\data\barter.rvmat"}}, {0.5, {"EFT\barter\zibbo_g\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\zibbo_g\data\barter_damage.rvmat"}}, {0, {"EFT\barter\zibbo_g\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
