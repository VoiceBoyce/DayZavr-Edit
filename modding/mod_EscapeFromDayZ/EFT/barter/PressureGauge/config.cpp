class CfgPatches {
	class EFT_PressureGauge {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_PressureGauge : Inventory_Base {
		scope = 2;
displayName = "#EFT_PressureGauge_displayName";
descriptionShort = "#EFT_PressureGauge_descriptionShort";
		model = "EFT\barter\PressureGauge\EFT_PressureGauge.p3d";
		isMeleeWeapon = 1;
		weight = 800;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\PressureGauge\data\barter.rvmat"}}, {0.7, {"EFT\barter\PressureGauge\data\barter.rvmat"}}, {0.5, {"EFT\barter\PressureGauge\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\PressureGauge\data\barter_damage.rvmat"}}, {0, {"EFT\barter\PressureGauge\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
