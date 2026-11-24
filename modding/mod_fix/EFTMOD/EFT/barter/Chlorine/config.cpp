class CfgPatches {
	class EFT_Chlorine {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Chlorine : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Chlorine_displayName";
		descriptionShort = "#EFT_Chlorine_descriptionShort";
		model = "EFT\barter\Chlorine\EFT_Chlorine.p3d";
		isMeleeWeapon = 1;
		weight = 350;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Chlorine\data\barter.rvmat"}}, {0.7, {"EFT\barter\Chlorine\data\barter.rvmat"}}, {0.5, {"EFT\barter\Chlorine\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Chlorine\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Chlorine\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
