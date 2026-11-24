class CfgPatches {
	class EFT_Filtr {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Filtr : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Filtr_displayName";
		descriptionShort = "#EFT_Filtr_descriptionShort";
		model = "EFT\barter\Filtr\EFT_Filtr.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Filtr\data\barter.rvmat"}}, {0.7, {"EFT\barter\Filtr\data\barter.rvmat"}}, {0.5, {"EFT\barter\Filtr\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Filtr\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Filtr\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
