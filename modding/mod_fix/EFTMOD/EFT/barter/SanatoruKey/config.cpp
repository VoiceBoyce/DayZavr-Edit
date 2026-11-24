class CfgPatches {
	class EFT_SanatoruKey {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_SanatoruKey : Inventory_Base {
		scope = 2;
displayName = "#EFT_SanatoruKey_displayName";
descriptionShort = "#EFT_SanatoruKey_descriptionShort";
		model = "EFT\barter\SanatoruKey\EFT_SanatoruKey.p3d";
		isMeleeWeapon = 1;
		weight = 20;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\SanatoruKey\data\barter.rvmat"}}, {0.7, {"EFT\barter\SanatoruKey\data\barter.rvmat"}}, {0.5, {"EFT\barter\SanatoruKey\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\SanatoruKey\data\barter_damage.rvmat"}}, {0, {"EFT\barter\SanatoruKey\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
