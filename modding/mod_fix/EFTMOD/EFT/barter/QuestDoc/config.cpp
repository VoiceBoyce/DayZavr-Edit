class CfgPatches {
	class EFT_QuestDoc {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_doc_Goshan : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_doc_Goshan_displayName";
descriptionShort = "#EFT_q_doc_Goshan_descriptionShort";
		model = "EFT\barter\QuestDoc\EFT_QuestDoc1.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\QuestDoc\data\Goshan.rvmat"}}, {0.7, {"EFT\barter\QuestDoc\data\Goshan.rvmat"}}, {0.5, {"EFT\barter\QuestDoc\data\Goshan_damage.rvmat"}}, {0.3, {"EFT\barter\QuestDoc\data\Goshan_damage.rvmat"}}, {0, {"EFT\barter\QuestDoc\data\Goshan_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_doc_IDEA : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_doc_IDEA_displayName";
descriptionShort = "#EFT_q_doc_IDEA_descriptionShort";
		model = "EFT\barter\QuestDoc\EFT_QuestDoc2.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\QuestDoc\data\IDEA.rvmat"}}, {0.7, {"EFT\barter\QuestDoc\data\IDEA.rvmat"}}, {0.5, {"EFT\barter\QuestDoc\data\IDEA_damage.rvmat"}}, {0.3, {"EFT\barter\QuestDoc\data\IDEA_damage.rvmat"}}, {0, {"EFT\barter\QuestDoc\data\IDEA_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_doc_OLI : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_doc_OLI_displayName";
descriptionShort = "#EFT_q_doc_OLI_descriptionShort";
		model = "EFT\barter\QuestDoc\EFT_QuestDoc3.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\QuestDoc\data\OLI.rvmat"}}, {0.7, {"EFT\barter\QuestDoc\data\OLI.rvmat"}}, {0.5, {"EFT\barter\QuestDoc\data\OLI_damage.rvmat"}}, {0.3, {"EFT\barter\QuestDoc\data\OLI_damage.rvmat"}}, {0, {"EFT\barter\QuestDoc\data\OLI_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_q_doc_OLIroute : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_doc_OLIroute_displayName";
descriptionShort = "#EFT_q_doc_OLIroute_descriptionShort";
		model = "EFT\barter\QuestDoc\EFT_QuestDoc4.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\QuestDoc\data\OLIroute.rvmat"}}, {0.7, {"EFT\barter\QuestDoc\data\OLIroute.rvmat"}}, {0.5, {"EFT\barter\QuestDoc\data\OLIroute_damage.rvmat"}}, {0.3, {"EFT\barter\QuestDoc\data\OLIroute_damage.rvmat"}}, {0, {"EFT\barter\QuestDoc\data\OLIroute_destruct.rvmat"}}};
				};
			};
		};
	};
};
