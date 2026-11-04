class CfgPatches {
	class EFT_ClochingBook {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_ClochingBook1 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_ClochingBook1_displayName";
		descriptionShort = "#EFT_q_ClochingBook1_descriptionShort";
		model = "EFT\barter\ClochingBook\EFT_ClochingBook.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\ClochingBook\data\barter.rvmat"}}, {0.7, {"EFT\barter\ClochingBook\data\barter.rvmat"}}, {0.5, {"EFT\barter\ClochingBook\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\ClochingBook\data\barter_damage.rvmat"}}, {0, {"EFT\barter\ClochingBook\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_ClochingBook2 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_ClochingBook2_displayName";
		descriptionShort = "#EFT_q_ClochingBook2_descriptionShort";
		model = "EFT\barter\ClochingBook\EFT_ClochingBook2.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\ClochingBook\data\barter.rvmat"}}, {0.7, {"EFT\barter\ClochingBook\data\barter.rvmat"}}, {0.5, {"EFT\barter\ClochingBook\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\ClochingBook\data\barter_damage.rvmat"}}, {0, {"EFT\barter\ClochingBook\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
