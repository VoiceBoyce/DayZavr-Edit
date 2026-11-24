class CfgPatches {
	class EFT_Cloth {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Cloth_aramid : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cloth_aramid_displayName";
		descriptionShort = "#EFT_Cloth_aramid_descriptionShort";
		model = "EFT\barter\Cloth\EFT_Cloth.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\barter\Cloth\data\barter_aramid.paa"};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cloth\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_Cloth_cordura : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cloth_cordura_displayName";
		descriptionShort = "#EFT_Cloth_cordura_descriptionShort";
		model = "EFT\barter\Cloth\EFT_Cloth.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\barter\Cloth\data\barter_cordura.paa"};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cloth\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_Cloth_fleece : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cloth_fleece_displayName";
		descriptionShort = "#EFT_Cloth_fleece_descriptionShort";
		model = "EFT\barter\Cloth\EFT_Cloth.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\barter\Cloth\data\barter_fleece.paa"};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cloth\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_Cloth_ripstop : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cloth_ripstop_displayName";
		descriptionShort = "#EFT_Cloth_ripstop_descriptionShort";
		model = "EFT\barter\Cloth\EFT_Cloth.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\barter\Cloth\data\barter_ripstop.paa"};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cloth\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cloth\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cloth\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
	
	class EFT_Cloth_paracord : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cloth_paracord_displayName";
		descriptionShort = "#EFT_Cloth_paracord_descriptionShort";
		model = "EFT\barter\Cloth\EFT_Cloth_Paracord.p3d";
		isMeleeWeapon = 1;
		weight = 350;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		hiddenSelections[] = {"zbytek"};
		hiddenSelectionsTextures[] = {"EFT\barter\Cloth\data\barter_paracord.paa"};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cloth\data\barter_paracord.rvmat"}}, {0.7, {"EFT\barter\Cloth\data\barter_paracord.rvmat"}}, {0.5, {"EFT\barter\Cloth\data\barter_paracord_damage.rvmat"}}, {0.3, {"EFT\barter\Cloth\data\barter_paracord_damage.rvmat"}}, {0, {"EFT\barter\Cloth\data\barter_paracord_destruct.rvmat"}}};
				};
			};
		};
	};
};
