class CfgPatches {
	class EFT_Lens {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Lens : Inventory_Base {
		scope = 2;
displayName = "#EFT_Lens_displayName";
descriptionShort = "#EFT_Lens_descriptionShort";
		model = "EFT\barter\Lens\EFT_Lens.p3d";
		isMeleeWeapon = 1;
		weight = 150;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Lens\data\barter.rvmat"}}, {0.7, {"EFT\barter\Lens\data\barter.rvmat"}}, {0.5, {"EFT\barter\Lens\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Lens\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Lens\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
