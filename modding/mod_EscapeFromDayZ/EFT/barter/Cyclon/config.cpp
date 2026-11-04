class CfgPatches {
	class EFT_Cyclon {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Cyclon : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Cyclon_displayName";
		descriptionShort = "#EFT_Cyclon_descriptionShort";
		model = "EFT\barter\Cyclon\EFT_Cyclon.p3d";
		isMeleeWeapon = 1;
		weight = 1700;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Cyclon\data\barter.rvmat"}}, {0.7, {"EFT\barter\Cyclon\data\barter.rvmat"}}, {0.5, {"EFT\barter\Cyclon\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Cyclon\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Cyclon\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
