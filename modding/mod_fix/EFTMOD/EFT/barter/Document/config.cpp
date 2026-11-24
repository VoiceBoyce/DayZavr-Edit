class CfgPatches {
	class EFT_Document {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_Document1 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document1_displayName";
		descriptionShort = "#EFT_q_Document1_descriptionShort";
		model = "EFT\barter\Document\EFT_Document1.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document1.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document1.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document1_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document1_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document1_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Document2 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document2_displayName";
		descriptionShort = "#EFT_q_Document2_descriptionShort";
		model = "EFT\barter\Document\EFT_Document2.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document2.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document2.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document2_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document2_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document2_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Document3 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document3_displayName";
		descriptionShort = "#EFT_q_Document3_descriptionShort";
		model = "EFT\barter\Document\EFT_Document3.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document3.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document3.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document3_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document3_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document3_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Document4 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document4_displayName";
		descriptionShort = "#EFT_q_Document4_descriptionShort";
		model = "EFT\barter\Document\EFT_Document4.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document4.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document4.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document4_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document4_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document4_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Document5 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document5_displayName";
		descriptionShort = "#EFT_q_Document5_descriptionShort";
		model = "EFT\barter\Document\EFT_Document5.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document5.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document5.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document5_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document5_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document5_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_Document6 : Inventory_Base {
		scope = 2;
		displayName = "#EFT_q_Document6_displayName";
		descriptionShort = "#EFT_q_Document6_descriptionShort";
		model = "EFT\barter\Document\EFT_Document6.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1, 2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Document\data\Document6.rvmat"}}, {0.7, {"EFT\barter\Document\data\Document6.rvmat"}}, {0.5, {"EFT\barter\Document\data\Document6_damage.rvmat"}}, {0.3, {"EFT\barter\Document\data\Document6_damage.rvmat"}}, {0, {"EFT\barter\Document\data\Document6_destruct.rvmat"}}};
				};
			};
		};
	};
};
