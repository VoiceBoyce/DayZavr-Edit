class CfgPatches {
	class EFT_Engine {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Engine : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Engine_displayName";
		descriptionShort = "#EFT_Engine_descriptionShort";
		model = "EFT\barter\Engine\EFT_Engine.p3d";
		isMeleeWeapon = 1;
		weight = 1400;
		absorbency = 0.1;
		itemSize[] = {2,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Engine\data\barter.rvmat"}}, {0.7, {"EFT\barter\Engine\data\barter.rvmat"}}, {0.5, {"EFT\barter\Engine\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Engine\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Engine\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
