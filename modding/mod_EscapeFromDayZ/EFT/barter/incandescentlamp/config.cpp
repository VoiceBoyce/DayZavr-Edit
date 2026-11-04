class CfgPatches {
	class EFT_incandescentlamp {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_incandescentlamp : Inventory_Base {
		scope = 2;
		displayName = "#EFT_incandescentlamp_displayName";
		descriptionShort = "#EFT_incandescentlamp_descriptionShort";
		model = "EFT\barter\incandescentlamp\EFT_incandescentlamp.p3d";
		isMeleeWeapon = 1;
		weight = 90;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\incandescentlamp\data\barter.rvmat"}}, {0.7, {"EFT\barter\incandescentlamp\data\barter.rvmat"}}, {0.5, {"EFT\barter\incandescentlamp\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\incandescentlamp\data\barter_damage.rvmat"}}, {0, {"EFT\barter\incandescentlamp\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
