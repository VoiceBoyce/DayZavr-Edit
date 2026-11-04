class CfgPatches {
	class EFT_Parrot {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Parrot : Inventory_Base {
		scope = 2;
displayName = "#EFT_Parrot_displayName";
descriptionShort = "#EFT_Parrot_descriptionShort";
		model = "EFT\barter\Parrot\EFT_Parrot.p3d";
		isMeleeWeapon = 1;
		weight = 330;
		absorbency = 0.1;
		itemSize[] = {1,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Parrot\data\barter.rvmat"}}, {0.7, {"EFT\barter\Parrot\data\barter.rvmat"}}, {0.5, {"EFT\barter\Parrot\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Parrot\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Parrot\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
