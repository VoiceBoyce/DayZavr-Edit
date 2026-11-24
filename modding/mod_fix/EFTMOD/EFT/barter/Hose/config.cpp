class CfgPatches {
	class EFT_Hose {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Hose : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Hose_displayName";
		descriptionShort = "#EFT_Hose_descriptionShort";
		model = "EFT\barter\Hose\EFT_Hose.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Hose\data\barter.rvmat"}}, {0.7, {"EFT\barter\Hose\data\barter.rvmat"}}, {0.5, {"EFT\barter\Hose\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Hose\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Hose\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
