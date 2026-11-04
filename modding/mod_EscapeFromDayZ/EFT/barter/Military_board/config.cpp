class CfgPatches {
	class EFT_Military_board {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Military_board : Inventory_Base {
		scope = 2;
displayName = "#EFT_Military_board_displayName";
descriptionShort = "#EFT_Military_board_descriptionShort";
		model = "EFT\barter\Military_board\EFT_Military_board.p3d";
		isMeleeWeapon = 1;
		weight = 520;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Military_board\data\barter.rvmat"}}, {0.7, {"EFT\barter\Military_board\data\barter.rvmat"}}, {0.5, {"EFT\barter\Military_board\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Military_board\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Military_board\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
