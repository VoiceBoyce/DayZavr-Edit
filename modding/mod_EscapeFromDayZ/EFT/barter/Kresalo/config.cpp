class CfgPatches {
	class EFT_Kresalo {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Kresalo : Inventory_Base {
		scope = 2;
displayName = "#EFT_Kresalo_displayName";
descriptionShort = "#EFT_Kresalo_descriptionShort";
		model = "EFT\barter\Kresalo\EFT_Kresalo.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Kresalo\data\barter.rvmat"}}, {0.7, {"EFT\barter\Kresalo\data\barter.rvmat"}}, {0.5, {"EFT\barter\Kresalo\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Kresalo\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Kresalo\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
