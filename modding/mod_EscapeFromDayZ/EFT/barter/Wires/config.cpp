class CfgPatches {
	class EFT_Wires {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Wires : Inventory_Base {
		scope = 2;
displayName = "#EFT_Wires_displayName";
descriptionShort = "#EFT_Wires_descriptionShort";
		model = "EFT\barter\Wires\EFT_Wires.p3d";
		isMeleeWeapon = 1;
		weight = 250;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Wires\data\barter.rvmat"}}, {0.7, {"EFT\barter\Wires\data\barter.rvmat"}}, {0.5, {"EFT\barter\Wires\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Wires\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Wires\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
