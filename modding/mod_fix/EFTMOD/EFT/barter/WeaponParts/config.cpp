class CfgPatches {
	class EFT_WeaponParts {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_WeaponParts : Inventory_Base {
		scope = 2;
displayName = "#EFT_WeaponParts_displayName";
descriptionShort = "#EFT_WeaponParts_descriptionShort";
		model = "EFT\barter\WeaponParts\EFT_WeaponParts.p3d";
		isMeleeWeapon = 1;
		weight = 560;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\WeaponParts\data\barter.rvmat"}}, {0.7, {"EFT\barter\WeaponParts\data\barter.rvmat"}}, {0.5, {"EFT\barter\WeaponParts\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\WeaponParts\data\barter_damage.rvmat"}}, {0, {"EFT\barter\WeaponParts\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
