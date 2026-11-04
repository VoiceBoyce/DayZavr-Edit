class CfgPatches {
	class EFT_Motorcontroller {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_q_Motorcontroller : Inventory_Base {
		scope = 2;
displayName = "#EFT_q_Motorcontroller_displayName";
descriptionShort = "#EFT_q_Motorcontroller_descriptionShort";
		model = "EFT\barter\Motorcontroller\EFT_Motorcontroller.p3d";
		isMeleeWeapon = 1;
		weight = 2000;
		absorbency = 0.1;
		itemSize[] = {2, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Motorcontroller\data\barter.rvmat"}}, {0.7, {"EFT\barter\Motorcontroller\data\barter.rvmat"}}, {0.5, {"EFT\barter\Motorcontroller\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Motorcontroller\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Motorcontroller\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
