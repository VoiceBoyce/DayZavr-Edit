class CfgPatches {
	class EFT_Chiken {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Chiken : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Chiken_displayName";
		descriptionShort = "#EFT_Chiken_descriptionShort";
		model = "EFT\barter\Chiken\EFT_Chiken.p3d";
		isMeleeWeapon = 1;
		weight = 3800;
		absorbency = 0.1;
		itemSize[] = {2,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Chiken\data\barter.rvmat"}}, {0.7, {"EFT\barter\Chiken\data\barter.rvmat"}}, {0.5, {"EFT\barter\Chiken\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Chiken\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Chiken\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
