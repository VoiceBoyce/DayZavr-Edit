class CfgPatches {
	class EFT_Rolex {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Rolex : Inventory_Base {
		scope = 2;

displayName = "#EFT_Rolex_displayName";
descriptionShort = "#EFT_Rolex_descriptionShort";
		model = "EFT\barter\Rolex\EFT_Rolex.p3d";
		isMeleeWeapon = 1;
		weight = 180;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Rolex\data\barter.rvmat"}}, {0.7, {"EFT\barter\Rolex\data\barter.rvmat"}}, {0.5, {"EFT\barter\Rolex\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Rolex\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Rolex\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
