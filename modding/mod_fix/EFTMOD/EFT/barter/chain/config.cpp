class CfgPatches {
	class EFT_chain {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_chain : Inventory_Base {
		scope = 2;
		displayName = "#EFT_chain_displayName";
		descriptionShort = "#EFT_chain_descriptionShort";
		model = "EFT\barter\chain\EFT_chain.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\chain\data\barter.rvmat"}}, {0.7, {"EFT\barter\chain\data\barter.rvmat"}}, {0.5, {"EFT\barter\chain\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\chain\data\barter_damage.rvmat"}}, {0, {"EFT\barter\chain\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
