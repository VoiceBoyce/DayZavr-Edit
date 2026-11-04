class CfgPatches {
	class EFT_Mounting_foam {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Mounting_foam : Inventory_Base {
		scope = 2;
displayName = "#EFT_Mounting_foam_displayName";
descriptionShort = "#EFT_Mounting_foam_descriptionShort";
		model = "EFT\barter\Mounting_foam\EFT_polyurethane_foam.p3d";
		isMeleeWeapon = 1;
		weight = 800;
		absorbency = 0.1;
		itemSize[] = {1,3};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Mounting_foam\data\barter.rvmat"}}, {0.7, {"EFT\barter\Mounting_foam\data\barter.rvmat"}}, {0.5, {"EFT\barter\Mounting_foam\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Mounting_foam\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Mounting_foam\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
