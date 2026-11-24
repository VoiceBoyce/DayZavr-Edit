class CfgPatches {
	class EFT_Letter {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_Letter1 : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_Letter1_displayName";
descriptionShort = "#EFT_q_Letter1_descriptionShort";
		model = "EFT\barter\Letter\EFT_Letter1.p3d";
		isMeleeWeapon = 1;
		weight = 50;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Letter\data\barter1.rvmat"}}, {0.7, {"EFT\barter\Letter\data\barter1.rvmat"}}, {0.5, {"EFT\barter\Letter\data\barter1_damage.rvmat"}}, {0.3, {"EFT\barter\Letter\data\barter1_damage.rvmat"}}, {0, {"EFT\barter\Letter\data\barter1_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Letter2 : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_Letter2_displayName";
descriptionShort = "#EFT_q_Letter2_descriptionShort";
		model = "EFT\barter\Letter\EFT_Letter2.p3d";
		isMeleeWeapon = 1;
		weight = 50;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Letter\data\barter2.rvmat"}}, {0.7, {"EFT\barter\Letter\data\barter2.rvmat"}}, {0.5, {"EFT\barter\Letter\data\barter2_damage.rvmat"}}, {0.3, {"EFT\barter\Letter\data\barter2_damage.rvmat"}}, {0, {"EFT\barter\Letter\data\barter2_destruct.rvmat"}}};
				};
			};
		};
	};
};
