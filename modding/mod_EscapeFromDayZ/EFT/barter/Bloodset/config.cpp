class CfgPatches {
	class EFT_Bloodset {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Bloodset : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Bloodset_displayName";
		descriptionShort = "#EFT_Bloodset_descriptionShort";
		model = "EFT\barter\Bloodset\EFT_Bloodset.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Bloodset\data\barter.rvmat"}}, {0.7, {"EFT\barter\Bloodset\data\barter.rvmat"}}, {0.5, {"EFT\barter\Bloodset\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Bloodset\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Bloodset\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
