class CfgPatches {
	class EFT_OFZ {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_OFZ : Inventory_Base {
		scope = 2;
displayName = "#EFT_OFZ_displayName";
descriptionShort = "#EFT_OFZ_descriptionShort";
		model = "EFT\barter\OFZ\EFT_OFZ.p3d";
		isMeleeWeapon = 1;
		weight = 839;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\OFZ\data\barter.rvmat"}}, {0.7, {"EFT\barter\OFZ\data\barter.rvmat"}}, {0.5, {"EFT\barter\OFZ\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\OFZ\data\barter_damage.rvmat"}}, {0, {"EFT\barter\OFZ\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
