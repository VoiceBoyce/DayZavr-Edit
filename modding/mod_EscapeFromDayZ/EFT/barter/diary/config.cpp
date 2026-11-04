class CfgPatches {
	class EFT_diary {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_diary : Inventory_Base {
		scope = 2;
		displayName = "#EFT_diary_displayName";
		descriptionShort = "#EFT_diary_descriptionShort";
		model = "EFT\barter\diary\EFT_diary.p3d";
		isMeleeWeapon = 1;
		weight = 250;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\diary\data\barter.rvmat"}}, {0.7, {"EFT\barter\diary\data\barter.rvmat"}}, {0.5, {"EFT\barter\diary\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\diary\data\barter_damage.rvmat"}}, {0, {"EFT\barter\diary\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
