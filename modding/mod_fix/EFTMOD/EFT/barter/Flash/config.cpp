class CfgPatches {
	class EFT_Military_flash {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Military_flash : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Military_flash_displayName";
		descriptionShort = "#EFT_Military_flash_descriptionShort";
		model = "EFT\barter\Flash\EFT_Military_flash.p3d";
		isMeleeWeapon = 1;
		weight = 140;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {}}, {0.7, {}}, {0.5, {}}, {0.3, {}}, {0, {}}};
				};
			};
		};
	};
};
