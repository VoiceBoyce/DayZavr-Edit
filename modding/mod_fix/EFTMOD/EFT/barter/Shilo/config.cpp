class CfgPatches {
	class EFT_Shilo {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Shilo : Inventory_Base {
		scope = 2;

displayName = "#EFT_Shilo_displayName";
descriptionShort = "#EFT_Shilo_descriptionShort";
		model = "EFT\barter\Shilo\EFT_Shilo.p3d";
		isMeleeWeapon = 1;
		weight = 100;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Shilo\data\barter.rvmat"}}, {0.7, {"EFT\barter\Shilo\data\barter.rvmat"}}, {0.5, {"EFT\barter\Shilo\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Shilo\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Shilo\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
