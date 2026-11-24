class CfgPatches {
	class EFT_HandDrill {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_HandDrill : Inventory_Base {
		scope = 2;
		displayName = "#EFT_HandDrill_displayName";
		descriptionShort = "#EFT_HandDrill_descriptionShort";
		model = "EFT\barter\HandDrill\EFT_HandDrill.p3d";
		isMeleeWeapon = 1;
		weight = 2300;
		absorbency = 0.1;
		itemSize[] = {3,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\HandDrill\data\barter.rvmat"}}, {0.7, {"EFT\barter\HandDrill\data\barter.rvmat"}}, {0.5, {"EFT\barter\HandDrill\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\HandDrill\data\barter_damage.rvmat"}}, {0, {"EFT\barter\HandDrill\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
