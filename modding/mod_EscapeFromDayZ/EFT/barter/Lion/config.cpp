class CfgPatches {
	class EFT_Lion {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Lion : Inventory_Base {
		scope = 2;
displayName = "#EFT_Lion_displayName";
descriptionShort = "#EFT_Lion_descriptionShort";
		model = "EFT\barter\Lion\EFT_Lion.p3d";
		isMeleeWeapon = 1;
		weight = 7200;
		absorbency = 0.1;
		itemSize[] = {3,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Lion\data\barter.rvmat"}}, {0.7, {"EFT\barter\Lion\data\barter.rvmat"}}, {0.5, {"EFT\barter\Lion\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Lion\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Lion\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
