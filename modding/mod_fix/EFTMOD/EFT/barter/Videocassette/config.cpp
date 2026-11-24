class CfgPatches {
	class EFT_Videocassette {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Videocassette : Inventory_Base {
		scope = 2;
displayName = "#EFT_Videocassette_displayName";
descriptionShort = "#EFT_Videocassette_descriptionShort";
		model = "EFT\barter\Videocassette\EFT_Videocassette.p3d";
		isMeleeWeapon = 1;
		weight = 150;
		absorbency = 0.1;
		itemSize[] = {2, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Videocassette\data\barter.rvmat"}}, {0.7, {"EFT\barter\Videocassette\data\barter.rvmat"}}, {0.5, {"EFT\barter\Videocassette\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Videocassette\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Videocassette\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
