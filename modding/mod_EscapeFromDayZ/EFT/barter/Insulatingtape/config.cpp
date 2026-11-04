class CfgPatches {
	class EFT_Insulatingtape {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Insulatingtape : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Insulatingtape_displayName";
		descriptionShort = "#EFT_Insulatingtape_descriptionShort";
		model = "EFT\barter\Insulatingtape\EFT_Insulatingtape.p3d";
		isMeleeWeapon = 1;
		weight = 50;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Insulatingtape\data\barter.rvmat"}}, {0.7, {"EFT\barter\Insulatingtape\data\barter.rvmat"}}, {0.5, {"EFT\barter\Insulatingtape\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Insulatingtape\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Insulatingtape\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
