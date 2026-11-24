class CfgPatches {
	class EFT_paperA4 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_paperA4 : Inventory_Base {
		scope = 2;
displayName = "#EFT_paperA4_displayName";
descriptionShort = "#EFT_paperA4_descriptionShort";
		model = "EFT\barter\paperA4\EFT_paperA4.p3d";
		isMeleeWeapon = 1;
		weight = 700;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\paperA4\data\barter.rvmat"}}, {0.7, {"EFT\barter\paperA4\data\barter.rvmat"}}, {0.5, {"EFT\barter\paperA4\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\paperA4\data\barter_damage.rvmat"}}, {0, {"EFT\barter\paperA4\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
