class CfgPatches {
	class EFT_OrgGlass {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_OrgGlass : Inventory_Base {
		scope = 2;
displayName = "#EFT_OrgGlass_displayName";
descriptionShort = "#EFT_OrgGlass_descriptionShort";
		model = "EFT\barter\OrgGlass\EFT_OrgGlass.p3d";
		isMeleeWeapon = 1;
		weight = 700;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\OrgGlass\data\barter.rvmat"}}, {0.7, {"EFT\barter\OrgGlass\data\barter.rvmat"}}, {0.5, {"EFT\barter\OrgGlass\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\OrgGlass\data\barter_damage.rvmat"}}, {0, {"EFT\barter\OrgGlass\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};