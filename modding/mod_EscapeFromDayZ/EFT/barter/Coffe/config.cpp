class CfgPatches {
	class EFT_Coffe {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Coffe : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Coffe_displayName";
		descriptionShort = "#EFT_Coffe_descriptionShort";
		model = "EFT\barter\Coffe\EFT_Coffe.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Coffe\data\barter.rvmat"}}, {0.7, {"EFT\barter\Coffe\data\barter.rvmat"}}, {0.5, {"EFT\barter\Coffe\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Coffe\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Coffe\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
