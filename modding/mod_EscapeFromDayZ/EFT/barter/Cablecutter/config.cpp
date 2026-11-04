class CfgPatches {
	class EFT_Cablecutter {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Cablecutter : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cablecutter_displayName";
		descriptionShort = "#EFT_Cablecutter_descriptionShort";
		model = "EFT\barter\Cablecutter\EFT_Cablecutter.p3d";
		isMeleeWeapon = 1;
		weight = 550;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cablecutter\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cablecutter\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cablecutter\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cablecutter\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cablecutter\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
