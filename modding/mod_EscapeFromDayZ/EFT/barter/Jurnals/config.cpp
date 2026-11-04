class CfgPatches {
	class EFT_Jurnals {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_Jurnals1 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Jurnals1_displayName";
		descriptionShort = "#EFT_q_Jurnals1_descriptionShort";
		model = "EFT\barter\Jurnals\EFT_Jurnals1.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.7, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.5, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Jurnals\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Jurnals2 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Jurnals2_displayName";
		descriptionShort = "#EFT_q_Jurnals2_descriptionShort";
		model = "EFT\barter\Jurnals\EFT_Jurnals2.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.7, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.5, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Jurnals\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Jurnals3 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Jurnals3_displayName";
		descriptionShort = "#EFT_q_Jurnals3_descriptionShort";
		model = "EFT\barter\Jurnals\EFT_Jurnals3.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.7, {"EFT\barter\Jurnals\data\barter.rvmat"}}, {0.5, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Jurnals\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Jurnals\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
