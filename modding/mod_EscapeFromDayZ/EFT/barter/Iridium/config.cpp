class CfgPatches {
	class EFT_Iridium {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Iridium : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Iridium_displayName";
		descriptionShort = "#EFT_Iridium_descriptionShort";
		model = "EFT\barter\Iridium\EFT_Iridium.p3d";
		isMeleeWeapon = 1;
		weight = 210;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Iridium\data\barter.rvmat"}}, {0.7, {"EFT\barter\Iridium\data\barter.rvmat"}}, {0.5, {"EFT\barter\Iridium\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Iridium\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Iridium\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
