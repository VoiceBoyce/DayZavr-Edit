class CfgPatches {
	class EFT_Syringer {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Syringer : Inventory_Base {
		scope = 2;
displayName = "#EFT_Syringer_displayName";
descriptionShort = "#EFT_Syringer_descriptionShort";
		model = "EFT\barter\Syringer\EFT_Syringer.p3d";
		isMeleeWeapon = 1;
		weight = 60;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Syringer\data\barter.rvmat"}}, {0.7, {"EFT\barter\Syringer\data\barter.rvmat"}}, {0.5, {"EFT\barter\Syringer\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Syringer\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Syringer\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
