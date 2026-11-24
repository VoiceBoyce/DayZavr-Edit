class CfgPatches {
	class EFT_Magnet {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Magnet : Inventory_Base {
		scope = 2;
displayName = "#EFT_Magnet_displayName";
descriptionShort = "#EFT_Magnet_descriptionShort";
		model = "EFT\barter\Magnet\EFT_Magnet.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Magnet\data\barter.rvmat"}}, {0.7, {"EFT\barter\Magnet\data\barter.rvmat"}}, {0.5, {"EFT\barter\Magnet\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Magnet\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Magnet\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
