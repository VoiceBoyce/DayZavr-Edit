class CfgPatches {
	class EFT_RecipeBook {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_RecipeBook : Inventory_Base {
		scope = 2;
displayName = "#EFT_RecipeBook_displayName";
descriptionShort = "#EFT_RecipeBook_descriptionShort";
		model = "EFT\barter\RecipeBook\EFT_RecipeBook.p3d";
		isMeleeWeapon = 1;
		weight = 250;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\RecipeBook\data\barter.rvmat"}}, {0.7, {"EFT\barter\RecipeBook\data\barter.rvmat"}}, {0.5, {"EFT\barter\RecipeBook\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\RecipeBook\data\barter_damage.rvmat"}}, {0, {"EFT\barter\RecipeBook\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
