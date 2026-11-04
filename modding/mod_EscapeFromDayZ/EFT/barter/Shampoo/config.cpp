class CfgPatches {
	class EFT_Shampoo {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Shampoo : Inventory_Base {
		scope = 2;
displayName = "#EFT_Shampoo_displayName";
descriptionShort = "#EFT_Shampoo_descriptionShort";
		model = "EFT\barter\Shampoo\EFT_Shampoo.p3d";
		isMeleeWeapon = 1;
		weight = 450;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Shampoo\data\barter.rvmat"}}, {0.7, {"EFT\barter\Shampoo\data\barter.rvmat"}}, {0.5, {"EFT\barter\Shampoo\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Shampoo\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Shampoo\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
